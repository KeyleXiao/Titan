
#ifndef RJC_HASHMAP_SPIN
#define RJC_HASHMAP_SPIN

#include <string>
#include <map>
#include <intrin.h>     // _ReadWriteBarrier(), InterlockedCompareExchange()

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

//#include <stdexcept>
//#include <map>

using namespace std;

//		#define FOR_TEST_TIME
//		#define LOG_NUM

#define HASH_MAP_BUCKET_NUM    256

#ifndef spin_val_compare_and_swap32
#define spin_val_compare_and_swap32(destPtr, oldValue, newValue)        \
	InterlockedCompareExchange((volatile LONG *)(destPtr),              \
	(LONG)(newValue), (LONG)(oldValue))
#endif

#ifndef spin_bool_compare_and_swap32
#define spin_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
	(InterlockedCompareExchange((volatile LONG *)(destPtr),             \
	(LONG)(newValue), (LONG)(oldValue))         \
	== (LONG)(oldValue))
#endif

// 精简自旋锁
struct spin_mutex_s
{
#define SLEEP_SPIN_TIME		500
	enum _SpinState
	{
		_StateUnLock	= 0,
		_StateLock		= 1,
		_StateSleep		= SLEEP_SPIN_TIME,
	};
	//*/
	spin_mutex_s():spinState(_StateUnLock){};	
	spin_mutex_s(size_t n):spinState(_StateUnLock){};
	void UnLock()		// 解锁
	{		
		spinState = _StateUnLock;
		_ReadWriteBarrier();
	}
	void Lock()			// 加锁
	{			
		spinState = _StateLock;
		_ReadWriteBarrier();
	}
	bool isLocked()		// 是否已锁
	{			
		return spinState != _StateUnLock;
	}
	bool TryGetLock()
	{
		bool bWriteSuc = false;
		while(!bWriteSuc)
		{
			size_t tStateTemp = spinState;
			if (tStateTemp == _StateUnLock)
			{
				_ReadWriteBarrier();
				bWriteSuc = spin_bool_compare_and_swap32(&spinState, tStateTemp, _StateLock);
				if (bWriteSuc)
				{
					_ReadWriteBarrier();
					return true;
				}
			}
			else if (tStateTemp >= _StateSleep)
			{
				bWriteSuc = spin_bool_compare_and_swap32(&spinState, tStateTemp, _StateLock);
				if (bWriteSuc)
				{
					Sleep(1);
					return false;
				}
			}
			else
			{
				bWriteSuc = spin_bool_compare_and_swap32(&spinState, tStateTemp, tStateTemp+1);
				if (bWriteSuc)
				{
		//			Sleep(1);
					return false;
				}
			}			
		}
		return false;
	}

protected:
	size_t spinState;
};


template<typename T>
struct atomic_m
{
	atomic_m(){};
	atomic_m(T t):data(t),spin_mu(0){};
	T GetValueUnsafe()
	{
		return data;		
	}
	void GetValue(T &temp)
	{
		while(!spin_mu.TryGetLock()){};
		temp = data;
		spin_mu.UnLock();
	}
	void reset()
	{
		while(!spin_mu.TryGetLock()){};
		data = T();
		spin_mu.UnLock();
	}
	void SetValue(T newVal)
	{
		while(!spin_mu.TryGetLock()){};
		data = newVal;
		spin_mu.UnLock();
	}
	bool casSetValue(T oldVal, T newVal)
	{
		while(!spin_mu.TryGetLock()){};
		bool bSuc = spin_bool_compare_and_swap32(&data, oldVal, newVal);
		spin_mu.UnLock();
		return bSuc;
	}
	void AddValue(T addVa)
	{
		while(!spin_mu.TryGetLock()){};
		data += addVa;
		spin_mu.UnLock();
	}	
private:
	spin_mutex_s spin_mu;
	T	data;
};

namespace spin_hashmap
{	
	
template <typename Key, typename T, typename Compare, typename Hash>
class HashIterator;

template <typename T>
class DefaultHash
{
#define MAX_BIT_NUM		32
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

/*
// Specialization for the strings
template <>
class DefaultHash<string>
{
public:
	DefaultHash(int nNumBuckets = HASH_MAP_BUCKET_NUM)
	{
		if (nNumBuckets <= 0) {
			throw (invalid_argument("GetNumBuckets must be > 0"));
		}
		m_nNumBuckets = nNumBuckets;
	}
	size_t hash(const string& key) const
	{
		size_t sum = 0;
		for (int i = key.size()-1; i >=0 ; --i) {
			sum = sum << 8;
			sum += (unsigned char)key[i];
		}
		return ((sum*HASH_NUMBER) % m_nNumBuckets);
	}
	size_t hashKey(const string& key) const
	{
		size_t sum = 0;
		for (int i = key.size()-1; i >=0 ; --i) {
			sum = sum << 8;
			sum += (unsigned char)key[i];
		}
		return (sum*HASH_NUMBER);
	}
	size_t GetNumBuckets() const { return m_nNumBuckets; }
	void SetNumBuckets(size_t nNumBuck) { 
		if (nNumBuck <= 0) {
			throw (invalid_argument("GetNumBuckets must be > 0"));
		}
		m_nNumBuckets = nNumBuck; 
	}

protected:
	size_t m_nNumBuckets;
};
//*/

//
// hashmap interface
//

template<typename Key, typename T>
struct elemHead;

template<typename Key, typename T, typename Hash = DefaultHash<Key>>
struct node
{	
	node(Key kt, T dt):k(kt), nHashKey(hash.hashKey(kt)), 
		data(dt), pNext(NULL), pPre(NULL)
	{
	};
	~node(){
		memset(this, 0, sizeof(*this));	
	}

	node<Key, T>* GetNext()
	{		
		return pNext;
	}

	bool SetNext(node<Key, T>* pOld, node<Key, T>* pNew)
	{
		return spin_bool_compare_and_swap32(&pNext, pOld, pNew);
	}

	void SetNext(node<Key, T>* pNew)
	{
		pNext = pNew;
	}

	node<Key, T>* GetPre()
	{		
		return pPre;
	}

	bool SetPre(node<Key, T>* pOld, node<Key, T>* pNew)
	{
		return spin_bool_compare_and_swap32(&pPre, pOld, pNew);
	}

	void SetPre(node<Key, T>* pNew)
	{
		pPre = pNew;
	}

	void SetData(T temp)
	{
		data = temp;
	}	

	T& GetData(void)
	{
		return data;
	}
	const Key GetKey()
	{
		return k;
	}
	const size_t GetHashKey()
	{
		return nHashKey;
	}
private:	
	T					data;
	Hash				hash;
	const Key			k;
	const size_t		nHashKey;
	node<Key, T>*		pNext;
	node<Key, T>*		pPre;
};


template<typename Key, typename T>
struct elemHead
{		
	elemHead():head(NULL),m_spinLock(0){};
	node<const Key, T>* GetHead()
	{
	//	while(!m_spinLock.TryGetLock()){};
		return head;
	}
	node<const Key, T>* GetHeadUnsafe()
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

enum
{
	HashKey_Val		=	0,
	Index_Val,
	MaxType_Val,		
};

template<typename Key, typename T>
struct head_pool
{	
#define HEAD_POOL_MAX_LEVEL		32
	head_pool(std::size_t count):m_nLevel(0),m_bBinaryInit(0)
	{		
		memset(m_nLevelNode, 0, sizeof(size_t)*(HEAD_POOL_MAX_LEVEL+1));
		memset(nodes_, 0, sizeof(size_t)*HEAD_POOL_MAX_LEVEL);
		BuildNextLevelBuffer(0, count);
	}

	~head_pool()
	{
		m_nLevel = 0;
		memset(m_nLevelNode, 0, sizeof(size_t)*(HEAD_POOL_MAX_LEVEL+1));
		for (size_t i=0; i<HEAD_POOL_MAX_LEVEL; ++i)
		{
			if (nodes_[i] != NULL)
			{
				delete [] nodes_[i];
				nodes_[i] = NULL;
			}
		}
	}

	size_t BuildNextLevelBuffer(size_t nOldCap, size_t nBuildNum = 0)
	{
		while(!m_spinLock.TryGetLock()){return 0;};
		if (nOldCap != m_nLevelNode[m_nLevel])	{
			m_spinLock.UnLock();
			return 0;
		}
		// 确定输入的值一定是2的幂，否则取比它大的最小的幂
		if (m_nLevel == 0){			
			nBuildNum = max(nBuildNum, 2);
			nBuildNum = min(nBuildNum, (1<<16));
			size_t nSizeB = 0;
			while ((nBuildNum & (nBuildNum-1)) != 0){
				nBuildNum &= (nBuildNum-1);
				++nSizeB;
			}
			if (nSizeB > 0)	{
				nBuildNum = nBuildNum<<1;
			}
			size_t nTemp = nBuildNum>>1;
			while (nTemp != 0)
			{
				++m_bBinaryInit;
				nTemp = nTemp>>1;
			}
		}
		else {
			// 非初始化，每次增涨1倍空间
			nBuildNum = m_nLevelNode[m_nLevel];
		}
		if ((nBuildNum & (nBuildNum-1)) != 0) {
			m_spinLock.UnLock();
			return 0;
		}
		printf("### Build Buffer Level:%d(%d) ###\n", m_nLevel, nBuildNum);
		nodes_[m_nLevel] = new elemHead<Key, T>[nBuildNum];	
		if (nodes_[m_nLevel] == NULL){
			m_spinLock.UnLock();
			return 0;
		}
		
		for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i)	{
			elemHead<Key, T>* pHeadElem = nodesUnsafe(i, Index_Val);
			pHeadElem->LockHead();
		}

		if (m_nLevel > 0)
		{
			for (size_t i=0; i<nBuildNum; ++i)
			{
				elemHead<Key, T>* pHeadElem = nodesUnsafe(i, Index_Val);				
									
				node<const Key, T>* pHeadNode = pHeadElem->GetHeadUnsafe();
				node<const Key, T>* pNextNode = pHeadNode;
				while (pNextNode != NULL)
				{
					size_t nMoveBit = HEAD_POOL_MAX_LEVEL - (m_nLevel+m_bBinaryInit);
					size_t nIndexNew = pNextNode->GetHashKey() >> nMoveBit;						
					if ((nIndexNew % 2) == 1)
					{
						if (pNextNode == pHeadNode){
							pHeadElem->SetHeadUnsafe(NULL);
						}
						node<const Key, T>* pPreNode = pNextNode->GetPre();
						if (pPreNode != NULL){
							pPreNode->SetNext(NULL);
						}
						pNextNode->SetPre(NULL);
						reinterpret_cast<elemHead<Key, T>*>(nodes_[m_nLevel]+i)->SetHeadUnsafe(pNextNode);
						break;
					}
					pNextNode = pNextNode->GetNext();
				}
			}
		}
		for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i){
			elemHead<Key, T>* pHeadElem = nodesUnsafe(i, Index_Val);			
			pHeadElem->FreeHead();
		}
		
		++m_nLevel;
		m_nLevelNode[m_nLevel] = m_nLevelNode[m_nLevel-1]+nBuildNum;		
		
		m_spinLock.UnLock();
		return nBuildNum;
	}
	bool DelMaxLevelBuffer(size_t nOldCap)
	{
		if (m_nLevel > 1)
		{	
			while(!m_spinLock.TryGetLock()){return false;};
			if (nOldCap != m_nLevelNode[m_nLevel])	{
				m_spinLock.UnLock();
				return false;
			}

			for (size_t i=0; i<m_nLevelNode[m_nLevel]; ++i)	{
				elemHead<Key, T>* pHeadElem = nodesUnsafe(i, Index_Val);			
				node<const Key, T>* pCheakNode = pHeadElem->GetHead();
			}

			size_t nLevelTemp = m_nLevel - 1;
			for (size_t i=0; i<m_nLevelNode[m_nLevel]/2; ++i)
			{				
				elemHead<Key, T>* pHeadElem = reinterpret_cast<elemHead<Key, T>*>(nodes_[nLevelTemp]+i);
				if (pHeadElem != NULL)
				{
					node<const Key, T>* pHeadNode = pHeadElem->GetHeadUnsafe();
					if (pHeadNode != NULL)
					{							
						size_t nLevel = 1;
						size_t nIndexNew = pHeadNode->GetHashKey() >> (HEAD_POOL_MAX_LEVEL - (nLevelTemp+m_bBinaryInit-1));					
						while (nIndexNew%2 == 0 && nLevel < nLevelTemp)
						{
							nIndexNew = nIndexNew>>1;
							++nLevel;
						}
						if (nLevel != nLevelTemp)
						{
							nIndexNew = nIndexNew>>1;
						}
						elemHead<Key, T>* pPreElem = reinterpret_cast<elemHead<Key, T>*>(nodes_[nLevelTemp-nLevel]+nIndexNew);
						if (pPreElem != NULL )
						{
							node<const Key, T>* pHeadKeep = pPreElem->GetHeadUnsafe();
							if (pHeadKeep == NULL){
								pPreElem->SetHeadUnsafe(pHeadNode);
							}
							else{						
								while(pHeadKeep->GetNext() != NULL)	{
									pHeadKeep = pHeadKeep->GetNext();
								}
								pHeadNode->SetPre(pHeadKeep);
								pHeadKeep->SetNext(pHeadNode);
							}
						}
						else{
							printf("ERROR: pPreElem Head Should Not Be NULL\n");
						}
					}
				}
				else{
					printf("ERROR: Head Should Not Be NULL");
				}
			}

			printf("### Delet Buffer Level:%d(%d) ###\n", m_nLevel-1, m_nLevelNode[m_nLevel-1]);
			m_nLevelNode[m_nLevel] = 0;
			--m_nLevel;	
			if (nodes_[m_nLevel] != NULL)
			{				
				delete [] nodes_[m_nLevel];
				nodes_[m_nLevel] = NULL;
			}	
			for (size_t i=0; i<m_nLevelNode[m_nLevel]/2; ++i){
				elemHead<Key, T>* pHeadElem = nodesUnsafe(i, Index_Val);			
				pHeadElem->FreeHead();
			}
			//*/			
			m_spinLock.UnLock();
			return true;
		}
		return false;
	}

	size_t getHeadIndex(size_t nResKey)
	{		
		size_t nLevelTemp = getLevel();
		return nResKey >> (HEAD_POOL_MAX_LEVEL - (nLevelTemp+m_bBinaryInit-1));
	}
	
	elemHead<Key, T>* nodes2(size_t nResKey, size_t nKeyType = HashKey_Val)
	{
		while(!m_spinLock.TryGetLock()){};		
		if (nKeyType >= MaxType_Val && m_nLevel == 0 || (m_nLevel+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL ){
			m_spinLock.UnLock();
			return NULL;
		}

		size_t nBucket = (1<<(m_nLevel+m_bBinaryInit-1));
		size_t nHash = 0;
		if (nKeyType == HashKey_Val)
		{			
			nHash = (nResKey>>(HEAD_POOL_MAX_LEVEL - (m_nLevel+m_bBinaryInit-1)));
		}
		else if (nKeyType == Index_Val && nResKey < nBucket)
		{
			nHash = nResKey;
		}
		else
		{
			m_spinLock.UnLock();
			return NULL;
		}

		size_t nLevel = 1;
		size_t nIndex = nHash;
		while (nIndex%2 == 0 && nLevel < m_nLevel)
		{
			nIndex = nIndex>>1;
			++nLevel;
		}
		if (nLevel != m_nLevel)
		{
			nIndex = nIndex>>1;
		}
		elemHead<Key, T>* pHead = reinterpret_cast<elemHead<Key, T>*>(nodes_[m_nLevel-nLevel]+nIndex);
		pHead->LockHead();
		m_spinLock.UnLock();
		return pHead;
	}
	elemHead<Key, T>* nodes(size_t nResKey, size_t nKeyType = HashKey_Val)
	{
		while(true)
		{
			size_t nLevTemp = m_nLevel;
			if (nKeyType >= MaxType_Val && nLevTemp == 0 || (nLevTemp+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL ){			
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
				m_spinLock.UnLock();
				return NULL;
			}

			size_t nLevel = 1;
			size_t nIndex = nHash;
			while (nIndex%2 == 0 && nLevel < nLevTemp)
			{
				nIndex = nIndex>>1;
				++nLevel;
			}
			if (nLevel != nLevTemp)
			{
				nIndex = nIndex>>1;
			}
			while(!m_spinLock.TryGetLock()){};
			if (nLevTemp == m_nLevel)
			{
				elemHead<Key, T>* pHead = reinterpret_cast<elemHead<Key, T>*>(nodes_[nLevTemp-nLevel]+nIndex);
				pHead->LockHead();
				m_spinLock.UnLock();
				return pHead;
			}
			m_spinLock.UnLock();
		}		
	}

	size_t capacity(void)
	{
		while(!m_spinLock.TryGetLock()){};
		size_t nCap = m_nLevelNode[m_nLevel];	
		m_spinLock.UnLock();
		return nCap;
	}

	size_t getLevel()
	{
		while(!m_spinLock.TryGetLock()){};
		size_t nTemp = m_nLevel;
		m_spinLock.UnLock();
		return nTemp;
	}
private:
	elemHead<Key, T>* nodesUnsafe(size_t nResKey, size_t nKeyType = HashKey_Val)
	{
		while(1)
		{			
			size_t nLevelTemp = m_nLevel;
			if (nKeyType >= MaxType_Val && nLevelTemp == 0 || (nLevelTemp+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL ){
				return NULL;
			}

			size_t nBucket = (1<<(nLevelTemp+m_bBinaryInit-1));
			size_t nHash = 0;
			if (nKeyType == HashKey_Val)
			{			
				nHash = (nResKey>>(HEAD_POOL_MAX_LEVEL - (nLevelTemp+m_bBinaryInit-1)));
			}
			else if (nKeyType == Index_Val && nResKey < nBucket)
			{
				nHash = nResKey;
			}
			else
			{
				return NULL;
			}

			size_t nLevel = 1;
			size_t nIndex = nHash;
			while (nIndex%2 == 0 && nLevel < nLevelTemp)
			{
				nIndex = nIndex>>1;
				++nLevel;
			}
			if (nLevel != nLevelTemp)
			{
				nIndex = nIndex>>1;
			}
			if (nLevelTemp == m_nLevel)
			{
				return reinterpret_cast<elemHead<Key, T>*>(nodes_[nLevelTemp-nLevel]+nIndex);
			}
		}		
	}

private:
	byte				m_bBinaryInit;
	spin_mutex_s		m_spinLock;
	size_t				m_nLevel;
	size_t				m_nLevelNode[HEAD_POOL_MAX_LEVEL+1];
	elemHead<Key, T>*	nodes_[HEAD_POOL_MAX_LEVEL];	
};

template <typename Key, typename T, typename Compare = std::equal_to<Key>,
	typename Hash = DefaultHash<Key> >
class hashmap
{	
//	#define INCREASE_TIMES		2			// bucket数量增涨倍率（note:必须是大于1的正整数)
//  #define HASH_MAP_BUCKET_END_FLAG		0xFFFFFFFE	
public:
	typedef Key key_type;
	typedef T mapped_type;
	typedef pair<const Key, T> value_type;
	typedef pair<const Key, T>& reference;
	typedef const pair<const Key, T>& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef HashIterator<Key, T, Compare, Hash> iterator;
	typedef HashIterator<Key, T, Compare, Hash> const_iterator;	

	// Iterator methods
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	explicit hashmap(size_t nBucketNum, bool bDynamicBucket = true, const Compare& comp = Compare(),
		const Hash& hash = Hash()) ;

	~hashmap();
	hashmap(const hashmap<Key, T, Compare, Hash>& src);
	hashmap<Key, T, Compare, Hash>& operator= (
		const hashmap<Key, T, Compare, Hash>& rhs);

	void		clear(void);

	bool		insert(const key_type x, T data);
	bool		find(const key_type x, T& data);	
	bool		erase(const key_type x, T& data);

	bool		empty();
	size_type	size();
	void		swap(hashmap<Key, T, Compare, Hash>& hashIn);

//	T& operator[] (const key_type& x);
	
	head_pool<const Key, T>			m_tElemsT;
	atomic_m<int>					m_nSize;
	const bool						m_bDynamicBucket;
	Compare							m_Comp;
	Hash							m_Hash;
	spin_mutex_s					m_resize_spinmu;
};

// Construct m_pElems with the number of buckets.
template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::hashmap(size_t nBucketNum, bool bDynamicBucket,
	const Compare& comp, const Hash& hash)  :
m_nSize(0), m_Comp(comp), m_Hash(hash), m_bDynamicBucket(bDynamicBucket), m_tElemsT(nBucketNum)
{	
	
}

template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::~hashmap()
{
	
}
/*
template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::hashmap(
	const hashmap<Key, T, Compare, Hash>& src) :
m_nSize(0), m_Comp(src.m_Comp), m_Hash(src.m_Hash)
{
	
}
//*/


template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::find(const key_type x, T& data)
{		
	return false;
}


template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::insert(const key_type x, T data)
{
#define  REHASH_THRESH_ADD		32	
	node<const Key, T>* pTemp = new node<const Key, T>(x, data);
	size_t nHashInsert = pTemp->GetHashKey();
	bool bReturn = false;
	elemHead<const Key, T>* pElem = m_tElemsT.nodes(pTemp->GetHashKey());		
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		pElem->SetHeadUnsafe(pTemp);
		bReturn = true;
		goto END_AND_RETURN_INSERT; 
	}
	node<const Key, T>* pNodePre = pNodeHead;
	node<const Key, T>* pNodeNext = pNodePre->GetNext();
	while(pNodeNext != NULL && pNodeNext->GetHashKey() <= nHashInsert && pNodePre->GetKey() != x)
	{
		pNodePre = pNodeNext;
		pNodeNext = pNodePre->GetNext();
	}
	// 此值已存在
	if (pNodePre->GetKey() == x)
	{
		bReturn = false;
		goto END_AND_RETURN_INSERT;
	}
	if (pNodePre->GetHashKey() > nHashInsert){
		pNodeNext = pNodePre;
		pNodePre = pNodePre->GetPre();
		if (pNodePre == NULL){
			pElem->SetHeadUnsafe(pTemp);
		}						
	}
	if (pNodePre != NULL)
	{
		pNodePre->SetNext(pTemp);			
	}			
	pTemp->SetPre(pNodePre);
	if (pNodeNext != NULL)
	{
		pNodeNext->SetPre(pTemp);			
	}		
	pTemp->SetNext(pNodeNext);
	bReturn = true;
	
	
END_AND_RETURN_INSERT:
	pElem->FreeHead();

	if (bReturn){
		m_nSize.AddValue(1);
		size_t nCapTemp = m_tElemsT.capacity();
		if ((float)m_nSize.GetValueUnsafe()/nCapTemp > REHASH_THRESH_ADD){
			m_tElemsT.BuildNextLevelBuffer(nCapTemp);
		}
	}
	
	return bReturn;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::erase(const key_type x, T& data)
{	
#define REHASH_THRESH_DEL		(0.5)
	size_t nHashFind = m_Hash.hashKey(x);	
	bool bReturn = false;	
	elemHead<const Key, T>* pElem = m_tElemsT.nodes(nHashFind);			
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		bReturn = false;
		goto END_AND_RETURN_ERASE;
	}
	node<const Key, T>* pNodeFind = pNodeHead;
	node<const Key, T>* pNodeNext = pNodeFind->GetNext();
	while(pNodeNext != NULL && pNodeFind->GetHashKey() <= nHashFind && pNodeFind->GetKey() != x)
	{		
		pNodeFind = pNodeNext;
		pNodeNext = pNodeFind->GetNext();
	}		
	if (pNodeFind->GetKey() != x)
	{		
		bReturn = false;
		goto END_AND_RETURN_ERASE;
	}
	else
	{	// 更改头结点
		node<const Key, T>* pNodePre = pNodeFind->GetPre();
		if (pNodePre == NULL){
			pElem->SetHeadUnsafe(pNodeNext);
		}
		else{
			pNodePre->SetNext(pNodeNext);			
		}
		if (pNodeNext != NULL){
			pNodeNext->SetPre(pNodePre);
		}		
		pNodeFind->SetNext(NULL);
		pNodeFind->SetPre(NULL);
		data = pNodeFind->GetData();
		delete pNodeFind;		
		bReturn = true;
		goto END_AND_RETURN_ERASE;
	}
END_AND_RETURN_ERASE:
	pElem->FreeHead();
	if (bReturn){
		m_nSize.AddValue(-1);
		size_t nCapTemp = m_tElemsT.capacity();
		if ((float)m_nSize.GetValueUnsafe()/nCapTemp < REHASH_THRESH_DEL){
			m_tElemsT.DelMaxLevelBuffer(nCapTemp);
		}
	}
	
	return bReturn;
}

//*/


// unsafe !!!
/*
template <typename Key, typename T, typename Compare, typename Hash>
T& hashmap<Key, T, Compare, Hash>::operator[] (const key_type& x)
{	
		
}
//*/

template <typename Key, typename T, typename Compare, typename Hash>
void hashmap<Key, T, Compare, Hash>::clear(void)
{	
	size_t temp = m_tElemsT.capacity();
	if (m_tElemsT.capacity() == 0)
	{
		return;
	}
	for(size_t i = 0; i<m_tElemsT.capacity(); ++i)
	{
		elemHead<const Key, T>* pElem = m_tElemsT.nodes(i, Index_Val);	
		node<const Key, T>* pHead = pElem->GetHead();		
		pElem->SetHeadUnsafe(NULL);
		while (pHead != NULL)
		{
			node<const Key, T>* pTemp = pHead;
			pHead = pHead->GetNext();
			delete pTemp;
			m_nSize.AddValue(-1);
		}
		pElem->FreeHead();
	}
	if (m_nSize.GetValueUnsafe() != 0)
	{
		printf("### Left Data:%d ###\n", m_nSize.GetValueUnsafe());
		m_nSize.reset();
	}	

	while(m_bDynamicBucket && m_tElemsT.getLevel() > 1)
	{
		if (!m_tElemsT.DelMaxLevelBuffer(m_tElemsT.capacity()))
		{
			printf("### DEL Left Level:%d ###\n");
		}			
	}
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::empty()
{
	int nTemp = 0;
	m_nSize.GetValue(nTemp);
	return (nTemp == 0);
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::size_type
	hashmap<Key, T, Compare, Hash>::size()
{
	return (m_nSize.GetValueUnsafe());
}

template <typename Key, typename T, typename Compare, typename Hash>
void hashmap<Key, T, Compare, Hash>::swap(
	hashmap<Key, T, Compare, Hash>& hashIn)
{
	std::swap(*this, hashIn);
}

/*
template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::begin()
{
	
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::end()
{

}


//
// HashIterator
template <typename Key, typename T, typename Compare, typename Hash>
class HashIterator : public std::iterator<std::bidirectional_iterator_tag,
	pair<const Key, T> >
{
public:
	HashIterator(); 
	HashIterator(int bucket,
		typename map<const Key, T>::iterator mapIter,
		const hashmap<Key, T, Compare, Hash>* inHashmap, bool bDynamic);
	pair<const Key, T>& operator*() const;

	pair<const Key, T>* operator->() const;
	HashIterator<Key, T, Compare, Hash>& operator++();
	const HashIterator<Key, T, Compare, Hash> operator++(int);

	HashIterator<Key, T, Compare, Hash>& operator--();
	const HashIterator<Key, T, Compare, Hash> operator--(int);

	bool operator==(const HashIterator& rhs) const;
	bool operator!=(const HashIterator& rhs) const;

protected:
	int mBucket;
	Key m_key;
	T	m_data;
	const bool m_bDynamicBucket;
	typename map<const Key, T>::iterator mIt;
	const hashmap<Key, T, Compare, Hash>* mHashmap;

	// Helper methods for operator++ and operator--
	void increment();
	void decrement();
};

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>::HashIterator()
{
	mBucket = -1;
	mIt = map<pair<const Key, T> >::iterator();
	mHashmap = NULL;
	m_bDynamicBucket = false;
}

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>::HashIterator(
	int bucket, typename map<const Key, T>::iterator mapIter,
	const hashmap<Key, T, Compare, Hash>* inHashmap, bool bDynamic) :
mBucket(bucket), mIt(mapIter), mHashmap(inHashmap), m_key(mapIter->first), m_bDynamicBucket(bDynamic)
{
}

// Return the actual element
template <typename Key, typename T, typename Compare, typename Hash>
pair<const Key, T>& HashIterator<Key, T, Compare, Hash>::operator*() const
{
	// 需要验证指计的有效性 mBucket是否已变？
	if (m_bDynamicBucket)
	{
		size_t nTemp = mHashmap->m_Hash.Hash(m_key);
		size_t nTempCheck = (*mHashmap->m_pBucket_mutex)[nTemp].Hash(m_key);
		if (nTemp == nTempCheck)	
		{	
			if (mBucket != nTemp)
			{
				mBucket = nTemp;
				mIt = (*mHashmap->m_pElems)[mBucket].find(m_key);
			}
		}
		else
		{
			nTempCheck = (*mHashmap->m_pBucket_mutex)[nTempCheck].Hash(m_key);
			mBucket = nTempCheck;
			mIt = (*mHashmap->m_pElems)[mBucket].find(m_key);
		}
	}
	if (mIt != (*mHashmap->m_pElems)[mBucket].end())
	{
		return (*mIt);
	}	
}

template <typename Key, typename T, typename Compare, typename Hash>
pair<const Key, T>*
	HashIterator<Key, T, Compare, Hash>::operator->() const
{
	// 需要验证指计的有效性 mBucket是否已变？
	return (&(*mIt));
}

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>&
	HashIterator<Key, T, Compare, Hash>::operator++()
{
	increment();
	return (*this);
}

template <typename Key, typename T, typename Compare, typename Hash>
const HashIterator<Key, T, Compare, Hash>
	HashIterator<Key, T, Compare, Hash>::operator++(int)
{
	HashIterator<Key, T, Compare, Hash> oldIt = *this;
	increment();
	return (oldIt);
}

template <typename Key, typename T, typename Compare, typename Hash>
HashIterator<Key, T, Compare, Hash>&
	HashIterator<Key, T, Compare, Hash>::operator--()
{
	decrement();
	return (*this);
}

template <typename Key, typename T, typename Compare, typename Hash>
const HashIterator<Key, T, Compare, Hash>
	HashIterator<Key, T, Compare, Hash>::operator--(int)
{
	HashIterator<Key, T, Compare, Hash> newIt = *this;
	decrement();
	return (newIt);
}

template <typename Key, typename T, typename Compare, typename Hash>
void HashIterator<Key, T, Compare, Hash>::increment()
{
}

template <typename Key, typename T, typename Compare, typename Hash>
void HashIterator<Key, T, Compare, Hash>::decrement()
{
}

template <typename Key, typename T, typename Compare, typename Hash>
bool HashIterator<Key, T, Compare, Hash>::operator==(
	const HashIterator& rhs) const
{
	return (mHashmap == rhs.mHashmap && mBucket == rhs.mBucket &&
		mIt == rhs.mIt);
}

template <typename Key, typename T, typename Compare, typename Hash>
bool HashIterator<Key, T, Compare, Hash>::operator!=(
	const HashIterator& rhs) const
{
	return (!operator==(rhs));
}

//*/

}	// namespace spin_hashmap

#endif // RJC_HASHMAP_SPIN