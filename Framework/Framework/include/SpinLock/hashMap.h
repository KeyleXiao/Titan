
#ifndef RJC_HASHMAP_SPIN
#define RJC_HASHMAP_SPIN

#include "hashMapElemDef.h"
#include "hashMapInterator.h"
#include "atomic.h"
#include "spinLock.h"

/*	算法说明
	1、本hash map算法用“桶+双向链表”的数据结构
	2、为保证hash map的多线程访问安全，在对数据进行访问、修改操作时，对数据所在的桶加锁（自旋锁也叫忙等锁）
	3、为了hash map速度及其内存合理使用上的考虑，桶的数量是动态的，map上数据过多或过少时，会进行rehash
	4、为了hash map的迭代器多线程安全使用，本算法做了以下调整		
		（1）桶下面虽然都是各自独立的双向链表，但如果按桶的索引值从小到大的顺序，
			 将各个桶下面的双向链表依次连成一个双向链表，那么你将得到一个按Key的hash值从小到大排好序的有序链表
		（2）桶的数量的动态增涨或减少时，
		     借鉴了（并不一样）《the art of multiprocessor programming》里的13.3 A Lock-Free Hash Set的桶索引的思想，
			 因此，在hash map rehash之前或之后，都能保证：
			       设任意一个桶(此桶索引为X)，桶索引 >=X 的所有数据结点中最小的hash值 >= 桶索引值 <X 的所有数据结点中最大的hash值
				   也就是条件4.1在rehash之前或之后都成立
		（3）由于，多线程的环境，迭代器指向的数据结点，有可能会被其它线程释放。
		     因此，迭代器也做了特殊处理：每个数据结点释放时先判断此对象是否正在被迭代器引用，
				   如果是，则由最后一个引用它的迭代器去释放此内存。如果不是，则立即释放。
//*/

namespace spin_hashmap
{	
	#define REHASH_THRESH_ADD		4	
	#define REHASH_THRESH_DEL		(0.2)
	
template <typename Key, typename T, typename Compare, typename Hash>
class HashIterator;
//
// hashmap interface
template <typename Key, typename T, typename Compare = std::equal_to<Key>,
	typename Hash = DefaultHash<Key> >
class hashmap
{	
	friend class HashIterator<Key, T, Compare, Hash>;
public:
	typedef Key key_type;
	typedef HashIterator<Key, T, Compare, Hash> iterator;
	typedef HashIterator<Key, T, Compare, Hash> const_iterator;

	// Iterator methods
	iterator begin();
	iterator end();	

	explicit hashmap(size_t nBucketNum = 256, bool bDynamicBucket = true, const Compare& comp = Compare(),
		const Hash& hash = Hash()) ;

	~hashmap();	

	bool		insert(const key_type x, T data);		// 没有就新建，有则返回false
	void		setData(const key_type x, T data);		// 没有就新建，有则覆盖	
	bool		getData(const key_type x, T& data);		// 获取指定结点的值（拷贝）
	bool		find(const key_type x);					// 查找是否有指定key的结点
	bool		find_erase(const key_type x, T& data);	// 查找并删除结点
	bool		erase(const key_type x);				// 删除指定key的结点
	void		clear(void);							// 清空hash链表

	bool		empty();
	size_t		size();
	void		swap(hashmap<Key, T, Compare, Hash>& hashIn);

	// 1,左值后，对象可能会被销毁
	// 2,例：hash[2] = t; =之前安全，=操作时，对象就有可能已销毁（虽然可能性很低）
//	T& operator[] (const key_type& x);		// 可使用setData	
private:	
	// 查找大于指定key的结点，仅用于迭代器
	iterator find_upper(const key_type& x, elemBucket<const Key, T>* &pElem);

	bucket_pool<const Key, T>		m_tElemsT;
	node<const Key, T>				m_nodeEnd;
	atomic_m<int>					m_nSize;	
	Compare							m_Comp;
	Hash							m_Hash;
	const bool						m_bDynamicBucket;
};

/*
template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::begin()
	//*/

// Construct m_pElems with the number of buckets.
template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::hashmap(size_t nBucketNum, bool bDynamicBucket,
	const Compare& comp, const Hash& hash)  :
m_nSize(0), m_Comp(comp), m_Hash(hash), m_bDynamicBucket(bDynamicBucket), m_tElemsT(nBucketNum), m_nodeEnd(Key(),T())
{	
	
}

template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::~hashmap()
{
	clear();
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::begin()
{
	node<const Key, T>* pNodeHead = NULL;
	for (size_t i=0; i<m_tElemsT.capacity(); ++i)
	{
		elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(i, Index_Val);	
		pNodeHead = pElem->GetHead();
		if (pNodeHead != NULL)
		{
			pElem->FreeHead();
			break;
		}
		pElem->FreeHead();
	}
	if (pNodeHead == NULL)
	{
		return (HashIterator<Key, T, Compare, Hash>(&m_nodeEnd, this, m_Comp, m_Hash));		
	}
	return (HashIterator<Key, T, Compare, Hash>(pNodeHead, this, m_Comp, m_Hash));
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::end()
{
	return (HashIterator<Key, T, Compare, Hash>(&m_nodeEnd, this, m_Comp, m_Hash));
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::find_upper(const key_type& x, elemBucket<const Key, T>* &pElem)
{	
	size_t nHashFind = m_Hash.hashKey(x);
	size_t nBucketIndex = m_tElemsT.getHeadIndex(nHashFind);
	elemBucket<const Key, T>* pElemNext = NULL;
FIND_NEXT_BUCKET:	
	if (pElem == NULL){
		return (HashIterator<Key, T, Compare, Hash>(&m_nodeEnd, this, m_Comp, m_Hash));
	}
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{	// 本桶为空，从下个桶开始找		
		++nBucketIndex;	
		if (nBucketIndex >= m_tElemsT.capacityUnsafe())
		{
			return (HashIterator<Key, T, Compare, Hash>(&m_nodeEnd, this, m_Comp, m_Hash));
		}	
		pElem->FreeHead();
		pElemNext = m_tElemsT.hashNodes(nBucketIndex, Index_Val);
		pElem = pElemNext;
		goto FIND_NEXT_BUCKET;
	}
	node<const Key, T>* pNodeFind = pNodeHead;
	node<const Key, T>* pNodeNext = pNodeFind->GetNext();
	while(pNodeNext != NULL && pNodeFind->GetHashKey() <= nHashFind)
	{		
		pNodeFind = pNodeNext;
		pNodeNext = pNodeFind->GetNext();
	}		
	if (pNodeFind->GetHashKey() <= nHashFind)
	{		
		// 本桶结点hash值都比目标值小，则从下个桶开始找
		++nBucketIndex;	
		if (nBucketIndex == m_tElemsT.capacityUnsafe())
		{
			return (HashIterator<Key, T, Compare, Hash>(&m_nodeEnd, this, m_Comp, m_Hash));
		}	
		pElem->FreeHead();
		pElemNext = m_tElemsT.hashNodes(nBucketIndex, Index_Val);
		pElem = pElemNext;
		goto FIND_NEXT_BUCKET;
	}
	else
	{	
		return (HashIterator<Key, T, Compare, Hash>(pNodeFind, this, m_Comp, m_Hash));
	}	
}
//*/

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::find(const key_type x)
{
	T temp;	
	return getData(x, temp);
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::getData(const key_type x, T& data)
{
	size_t nHashFind = m_Hash.hashKey(x);	
	elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(nHashFind);			
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		pElem->FreeHead();
		return false;
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
		pElem->FreeHead();
		return false;
	}
	else
	{	
		data = pNodeFind->GetData();
		pElem->FreeHead();
		return true;
	}
}

template <typename Key, typename T, typename Compare, typename Hash>
void hashmap<Key, T, Compare, Hash>::setData(const key_type x, T data)
{
	node<const Key, T>* pTemp = new node<const Key, T>(x, data);
	size_t nHashInsert = pTemp->GetHashKey();
	bool bReturn = false;
	elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(pTemp->GetHashKey());		
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		m_nSize.AddValue(1);
		pElem->SetHeadUnsafe(pTemp);
		pElem->FreeHead();
		return; 
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
		pNodePre->SetData(data);
		pElem->FreeHead();
		delete pTemp;
		return;
	}
	if (pNodePre->GetHashKey() > nHashInsert){
		pNodeNext = pNodePre;
		pNodePre = pNodePre->GetPre();
		if (pNodePre == NULL){
			pElem->SetHeadUnsafe(pTemp);
		}						
	}
	// 指定位置插入新结点
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
	pElem->FreeHead();
	m_nSize.AddValue(1);
	size_t nCapTemp = m_tElemsT.capacity();
	// bucket动态数量，平均链表过长时，自动扩容
	if (m_bDynamicBucket && (float)m_nSize.GetValueUnsafe()/nCapTemp > REHASH_THRESH_ADD){
		m_tElemsT.BuildNextLevelBuffer(nCapTemp);
	}
	return;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::insert(const key_type x, T data)
{	
	node<const Key, T>* pTemp = new node<const Key, T>(x, data);
	size_t nHashInsert = pTemp->GetHashKey();
	bool bReturn = false;
	elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(pTemp->GetHashKey());		
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		m_nSize.AddValue(1);
		pElem->SetHeadUnsafe(pTemp);
		pElem->FreeHead();
		return true; 
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
		pElem->FreeHead();
		delete pTemp;
		return false;		
	}
	if (pNodePre->GetHashKey() > nHashInsert){
		pNodeNext = pNodePre;
		pNodePre = pNodePre->GetPre();
		if (pNodePre == NULL){
			pElem->SetHeadUnsafe(pTemp);
		}						
	}
	// 指定位置插入新结点
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
	pElem->FreeHead();
	m_nSize.AddValue(1);
	size_t nCapTemp = m_tElemsT.capacity();
	// bucket动态数量，平均链表过长时，自动扩容
	if (m_bDynamicBucket && (float)m_nSize.GetValueUnsafe()/nCapTemp > REHASH_THRESH_ADD){
		m_tElemsT.BuildNextLevelBuffer(nCapTemp);
	}
	return true;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::erase(const key_type x)
{
	T temp;
	return find_erase(x, temp);
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::find_erase(const key_type x, T& data)
{	
	size_t nHashFind = m_Hash.hashKey(x);	
	elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(nHashFind);			
	node<const Key, T>* pNodeHead = pElem->GetHead();
	if (pNodeHead == NULL)
	{
		pElem->FreeHead();
		return false;
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
		pElem->FreeHead();
		return false;
	}
	else
	{	// 找到目标结点，删除
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

	//	delete pNodeFind;	
		// 迭代器已引用了此结点，就留给迭代器删此内存
		// 否则立即删除失效结点
		if (pNodeFind->GetUsingValue() == 0)	
			delete pNodeFind;
		else		
			pNodeFind->SetDelFlag(true);
		//*/

		pElem->FreeHead();
		m_nSize.AddValue(-1);
		size_t nCapTemp = m_tElemsT.capacity();
		// bucket动态数量，链表过于稀疏时，减少一半的桶
		if (m_bDynamicBucket && (float)m_nSize.GetValueUnsafe()/nCapTemp < REHASH_THRESH_DEL){
			m_tElemsT.DelMaxLevelBuffer(nCapTemp);
		}
		return true;
	}
}

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
		elemBucket<const Key, T>* pElem = m_tElemsT.hashNodes(i, Index_Val);	
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
	while(m_bDynamicBucket && m_tElemsT.getLevel() > 1)
	{
		// clear时确保清空
		while (!m_tElemsT.DelMaxLevelBuffer(m_tElemsT.capacity())){}		
	}
	// for test
	if (m_nSize.GetValueUnsafe() != 0)
	{
		printf("### Left Data:%d ###\n", m_nSize.GetValueUnsafe());
		m_nSize.SetValue(0);
	}	
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::empty()
{
	return (m_nSize.GetValue() == 0);
}

template <typename Key, typename T, typename Compare, typename Hash>
size_t hashmap<Key, T, Compare, Hash>::size()
{
	return (m_nSize.GetValue());
}

template <typename Key, typename T, typename Compare, typename Hash>
void hashmap<Key, T, Compare, Hash>::swap(
	hashmap<Key, T, Compare, Hash>& hashIn)
{
	std::swap(*this, hashIn);
}

}	// namespace spin_hashmap

#endif // RJC_HASHMAP_SPIN