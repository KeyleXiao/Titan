#pragma once
#include <string>
#include "spinLock.h"
#include "arrayPool.h"
#include <map>
#include <set>

using namespace std;

namespace spin_hashmap
{
#define SHOW_DEBUG_SPIN_HASHMAP
	
template <typename T>
class DefaultHash
{
#define HASH_NUMBER		2654435769U
public:
	DefaultHash();
	size_t hashKey(const T& key) const;
};

template <typename T>
DefaultHash<T>::DefaultHash() 
{
}

template <typename T>
size_t DefaultHash<T>::hashKey(const T& key) const
{
	int bytes = sizeof(T);
	size_t res = 0;
	for (int i = bytes-1; i >=0 ; --i) {
		res = res << 8;		
		res += (unsigned char)*((char*)&key + i);
	}
	return (res*HASH_NUMBER); 
}

// Specialization for the strings
template <>
class DefaultHash<string>
{
#define HASH_NUMBER		2654435769U
public:
	DefaultHash(){}	
	size_t hashKey(const string& key) const
	{
		size_t sum = 0;
		for (int i = key.size()-1; i >=0 ; --i) {
			sum = sum << 8;
			sum += (unsigned char)key[i];
		}
		return (sum*HASH_NUMBER);
	}
};


template<typename Key, typename T>
struct elemBucket;

//////////////////////////////////////////////////////////
//					hash map 数据结点					//
//////////////////////////////////////////////////////////
template<typename Key, typename T, typename Hash = DefaultHash<Key>>
struct node: public pair<Key, T>
{	
public:
	node(): pair<Key, T>(Key(), T()), nHashKey(hash.hashKey(Key())), 
		data(dt), pNext(NULL), pPre(NULL), nUsingCount(0), bDel(false){	};

	node(Key kt, T dt): pair<Key, T>(kt, dt), k(kt), nHashKey(hash.hashKey(kt)), 
		data(dt), pNext(NULL), pPre(NULL), nUsingCount(0), bDel(false){	};

	~node()
	{	
		memset(this, 0, sizeof(*this));	
	}

	bool SetPre(node<Key, T>* pOld, node<Key, T>* pNew)
	{
		return spin_bool_compare_and_swap32(&pPre, pOld, pNew);
	}

	bool SetNext(node<Key, T>* pOld, node<Key, T>* pNew)
	{
		return spin_bool_compare_and_swap32(&pNext, pOld, pNew);
	}

	void	SetNext(node<Key, T>* pNew)	{	pNext = pNew;	}	
	void	SetPre(node<Key, T>* pNew)	{	pPre = pNew;	}
	void	SetData(T& temp)			{	data = temp;	}	
	T&		GetData(void)				{	return data;	}
	Key&	GetKey(void)				{	return k;		}
	node<Key, T>*	GetNext()			{	return pNext;	}
	node<Key, T>*	GetPre()			{	return pPre;	}	
	const size_t	GetHashKey()		{	return nHashKey;}

	bool GetDelFlag()
	{
		while(!spinLock.TryGetLock()){}
		bool bTemp = bDel;
		spinLock.UnLock();
		return bTemp;
	}
	void SetDelFlag(bool bDelFlag)
	{
		while(!spinLock.TryGetLock()){}
		bDel = bDelFlag;
		spinLock.UnLock();
	}
	void AddUsingValue(int nAdd)
	{
		while(!spinLock.TryGetLock()){}
		nUsingCount += nAdd;
		spinLock.UnLock();
	}
	int GetUsingValue()
	{
		while(!spinLock.TryGetLock()){}
		int nTemp = nUsingCount;
		spinLock.UnLock();
		return nTemp;
	}

private:	//	仅用于保障迭代器安全	
	int					nUsingCount;		// 被引用次数
	bool				bDel;				// 是否已从hash列表中移除
	spin_mutex_s		spinLock;			// 结点锁
private:	
	T					data;	
	const Key			k;
	Hash				hash;				// 无数据，只有函数
	const size_t		nHashKey;			// 保存hash值，以减少运算消耗
	node<Key, T>*		pNext;				// 后结点指针
	node<Key, T>*		pPre;				// 前结点指针
};

//////////////////////////////////////////////////////////
//						hash map 桶						//
//////////////////////////////////////////////////////////
template<typename Key, typename T>
struct elemBucket
{		
	elemBucket():head(NULL),m_spinLock(0){};
	node<const Key, T>* GetHead()
	{
		return head;
	}
	bool SetHead(node<const Key, T>* pOldHead, node<const Key, T>* pNewHead)
	{
		while(!m_spinLock.TryGetLock()){};
		bool bSuc = spin_bool_compare_and_swap32(&head, pOldHead, pNewHead);
		m_spinLock.UnLock();
		return bSuc;
	}	
	void SetHeadUnsafe(node<const Key, T>* pHead)
	{		
		head = pHead;
	}
	void LockHead()
	{
		while(!m_spinLock.TryGetLock()){};
	}
	void FreeHead()
	{
		m_spinLock.UnLock();		
	}
private:
	node<const Key, T>* head;
	spin_mutex_s		m_spinLock;
};


//////////////////////////////////////////////////////////
//	动态hash桶池，可快速新增、回收hash_map桶空间		//
//////////////////////////////////////////////////////////
enum
{
	HashKey_Val		=	0,
	Index_Val,
	MaxType_Val,		
};

template<typename Key, typename T>
class bucket_pool: public ArrayPool<elemBucket<Key, T> >
{
#define LIMIT_MAX_HASHMAP_BUCKETS		4194304		//4M
public:
	bucket_pool(size_t count = 1024U):ArrayPool(count, true){}
	~bucket_pool(){}

	// rehash:增长桶数量（一倍）
	virtual size_t BuildNextLevelBuffer(size_t nOldCap);
	// rehash:减少桶数量（一半）
	virtual bool DelMaxLevelBuffer(size_t nOldCap);
	// 由桶索引或hashkey得到桶节点
	elemBucket<Key, T>* hashNodes(size_t nResKey, size_t nKeyType = HashKey_Val);
	// 由哈希值得到当前对应的桶索引
	inline size_t getHeadIndex(size_t nResKey);

private:	
	// 由索引，得到数组池中桶节点的实际地址（函数实际为一个映射关系）
	inline void GetLevelAndIndex(size_t nMaxLevel, size_t& nLevel, size_t& nIndex);
	// 由桶索引或hashkey得到桶节点（非线程安全）
	inline elemBucket<Key, T>* nodesUnsafe(size_t nResKey);
	
};

template<typename Key, typename T>
size_t bucket_pool<Key, T>::BuildNextLevelBuffer(size_t nOldCap)
{	
	while(!m_spinLock.TryGetLock()){return 0;};
	while(!m_resizeLock.TryGetLock()){}
	if (!m_bCanResize || m_nLevel == 0 || nOldCap != m_nLevelNode[m_nLevel] 
		|| LIMIT_MAX_HASHMAP_BUCKETS <= m_nLevelNode[m_nLevel])
	{
		m_resizeLock.UnLock();
		m_spinLock.UnLock();
		return 0;
	}
	// 非初始化，每次增涨1倍空间
	size_t nBuildNum = m_nLevelNode[m_nLevel];	
#ifdef SHOW_DEBUG_SPIN_HASHMAP
	printf("### Build Buffer Level:%d(%d) ###\n", m_nLevel, nBuildNum);
#endif		
	nodes_[m_nLevel] = new elemBucket<Key, T>[nBuildNum];	
	if (nodes_[m_nLevel] == NULL){
		m_resizeLock.UnLock();
		m_spinLock.UnLock();
		return 0;
	}		
	// rehash前先获取到所有桶的锁
	for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i)	{
		elemBucket<Key, T>* pHeadElem = nodesUnsafe(i);
		pHeadElem->LockHead();
	}	
	for (size_t i=0; i<nBuildNum; ++i)
	{
		elemBucket<Key, T>* pHeadElem = nodesUnsafe(i);
		node<const Key, T>* pHeadNode = pHeadElem->GetHead();
		node<const Key, T>* pNextNode = pHeadNode;
		while (pNextNode != NULL)
		{
			size_t nMoveBit = HEAD_POOL_MAX_LEVEL - (m_nLevel+m_bBinaryInit);
			size_t nIndexNew = pNextNode->GetHashKey() >> nMoveBit;						
			if ((nIndexNew % 2) == 1)	// 找到了桶的分割点
			{
				if (pNextNode == pHeadNode)
				{
					pHeadElem->SetHeadUnsafe(NULL);
				}
				node<const Key, T>* pPreNode = pNextNode->GetPre();
				if (pPreNode != NULL){
					pPreNode->SetNext(NULL);
				}
				pNextNode->SetPre(NULL);
				reinterpret_cast<elemBucket<Key, T>*>(nodes_[m_nLevel]+i)->SetHeadUnsafe(pNextNode);
				break;
			}
			pNextNode = pNextNode->GetNext();
		}
	}
	// 释放获取到所有桶的锁
	for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i){
		elemBucket<Key, T>* pHeadElem = nodesUnsafe(i);
		pHeadElem->FreeHead();
	}
	++m_nLevel;	
	m_nLevelNode[m_nLevel] = m_nLevelNode[m_nLevel-1]+nBuildNum;		
	m_resizeLock.UnLock();
	m_spinLock.UnLock();
	return nBuildNum;
}

template<typename Key, typename T>
bool bucket_pool<Key, T>::DelMaxLevelBuffer(size_t nOldCap)
{
	if (m_nLevel <= 1)	// 初始化的那层内存不释放
	{	
		return false;
	}
	while(!m_spinLock.TryGetLock()){return false;};
	while(!m_resizeLock.TryGetLock()){}
	if (!m_bCanResize || nOldCap != m_nLevelNode[m_nLevel])	
	{
		m_resizeLock.UnLock();
		m_spinLock.UnLock();
		return false;
	}
	for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i)	
	{
		elemBucket<Key, T>* pHeadElem = nodesUnsafe(i);			
		pHeadElem->LockHead();
	}
	size_t nLevelTemp = m_nLevel - 1;
	for (size_t i=0; i<m_nLevelNode[m_nLevel]/2; ++i)
	{				
		elemBucket<Key, T>* pHeadElem = reinterpret_cast<elemBucket<Key, T>*>(nodes_[nLevelTemp]+i);
		if (pHeadElem != NULL)
		{
			node<const Key, T>* pHeadNode = pHeadElem->GetHead();
			if (pHeadNode != NULL)
			{
				size_t nIndex = pHeadNode->GetHashKey() >> (HEAD_POOL_MAX_LEVEL - (nLevelTemp+m_bBinaryInit-1));					
				size_t nLevel;
				GetLevelAndIndex(nLevelTemp, nLevel, nIndex);
				elemBucket<Key, T>* pPreElem = reinterpret_cast<elemBucket<Key, T>*>(nodes_[nLevelTemp-nLevel]+nIndex);
				if (pPreElem != NULL )
				{
					node<const Key, T>* pHeadKeep = pPreElem->GetHead();
					if (pHeadKeep == NULL)
					{
						pPreElem->SetHeadUnsafe(pHeadNode);
					}
					else
					{						
						while(pHeadKeep->GetNext() != NULL)	
						{
							pHeadKeep = pHeadKeep->GetNext();
						}
						pHeadNode->SetPre(pHeadKeep);
						pHeadKeep->SetNext(pHeadNode);
					}
				}					
			}
		}
	}
#ifdef SHOW_DEBUG_SPIN_HASHMAP
	printf("### Delet Buffer Level:%d(%d) ###\n", m_nLevel-1, m_nLevelNode[m_nLevel-1]);
#endif			
	m_nLevelNode[m_nLevel] = 0;
	--m_nLevel;	
	if (nodes_[m_nLevel] != NULL)
	{				
		delete [] nodes_[m_nLevel];
		nodes_[m_nLevel] = NULL;
	}	
	for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i)
	{
		elemBucket<Key, T>* pHeadElem = nodesUnsafe(i);			
		pHeadElem->FreeHead();
	}
	//*/		
	m_resizeLock.UnLock();
	m_spinLock.UnLock();
	return true;
}		
	
template<typename Key, typename T>
elemBucket<Key, T>* bucket_pool<Key, T>::hashNodes(size_t nResKey, size_t nKeyType/* = HashKey_Val*/)
{
	while(true)
	{
		size_t nLevTemp = m_nLevel;
		if (nKeyType >= MaxType_Val && nLevTemp == 0 || (nLevTemp+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL )
		{			
			return NULL;
		} 
		size_t nBucket = (1<<(nLevTemp+m_bBinaryInit-1));
		size_t nHash = 0;
		if (nKeyType == HashKey_Val)
		{			
			nHash = (nResKey>>(HEAD_POOL_MAX_LEVEL - (nLevTemp+m_bBinaryInit-1)));
		}
		else if (nKeyType == Index_Val && nResKey < nBucket)
		{
			nHash = nResKey;
		}
		else
		{
			return NULL;
		}
		size_t nLevel, nIndex = nHash;
		GetLevelAndIndex(m_nLevel, nLevel, nIndex);
		// 各个桶的锁一定要在桶池锁解锁前锁住，不然rehash时可能异常
		while(!m_spinLock.TryGetLock()){};
		if (nLevTemp == m_nLevel)
		{
			elemBucket<Key, T>* pHead = reinterpret_cast<elemBucket<Key, T>*>(nodes_[nLevTemp-nLevel]+nIndex);
			pHead->LockHead();
			m_spinLock.UnLock();
			return pHead;
		}
		m_spinLock.UnLock();
	}
}	

template<typename Key, typename T>
inline size_t bucket_pool<Key, T>::getHeadIndex(size_t nResKey)
{		
//	size_t nLevelTemp = getLevel();
	return nResKey >> (HEAD_POOL_MAX_LEVEL - (m_nLevel+m_bBinaryInit-1));
}

template<typename Key, typename T>
inline void bucket_pool<Key, T>::GetLevelAndIndex(size_t nMaxLevel, size_t& nLevel, size_t& nIndex)
{
	nLevel = 1;
	while (nIndex%2 == 0 && nLevel < nMaxLevel){
		nIndex = nIndex>>1;
		++nLevel;
	}
	if (nLevel != nMaxLevel)	{
		nIndex = nIndex>>1;
	}
}

template<typename Key, typename T>
inline elemBucket<Key, T>* bucket_pool<Key, T>::nodesUnsafe(size_t nResKey)
{
	size_t nBucket = (1<<(m_nLevel+m_bBinaryInit-1));
	if (nResKey >= nBucket && m_nLevel == 0 || (m_nLevel+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL ){
		return NULL;
	}		
	size_t nLevel, nIndex = nResKey;
	GetLevelAndIndex(m_nLevel, nLevel, nIndex);
	return reinterpret_cast<elemBucket<Key, T>*>(nodes_[m_nLevel-nLevel]+nIndex);		
}

}	// namespace spin_hashmap