
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
	
template <typename Key, typename T, typename Compare, typename Hash>
class HashIterator;

template <typename T>
class DefaultHash
{
#define HASH_NUMBER		2654435769U
public:
	DefaultHash(int GetNumBuckets = HASH_MAP_BUCKET_NUM) ;
//	DefaultHash(const DefaultHash & Src);
	size_t hash(const T& key);
	size_t hashKey(const T& key) const;
	int GetNumBuckets() { 
		int nTemp;
		m_nNumBuckets.GetValue(nTemp);
		return nTemp; 
	}
	void SetNumBuckets(size_t nNumBuck) {
		if (nNumBuck <= 0) {
			throw (invalid_argument("GetNumBuckets must be > 0"));
		}
		m_nNumBuckets.SetValue(nNumBuck); 
	}
protected:
	atomic_m<int> m_nNumBuckets;
};

template <typename T>
DefaultHash<T>::DefaultHash(int nNumBuckets) 
{
	if (nNumBuckets <= 0) {
		throw (invalid_argument("GetNumBuckets must be > 0"));
	}
	m_nNumBuckets.SetValue(nNumBuckets); 
}

/*
template <typename T>
DefaultHash<T>::DefaultHash(const DefaultHash & Src) const
{	
	m_nNumBuckets.SetValue(Src.GetNumBuckets());
}
//*/

template <typename T>
size_t DefaultHash<T>::hash(const T& key)
{
	int bytes = sizeof(T);
	size_t res = 0;
	for (int i = bytes-1; i >=0 ; --i) {
		res = res << 8;		
		res += (unsigned char)*((char*)&key + i);
	}
	int nBucketNum = 0;
	m_nNumBuckets.GetValue(nBucketNum); 
	return ((res*HASH_NUMBER) % nBucketNum);    
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
struct node
{
	node():k(0), pNext(NULL){};
	node(Key kt, T dt):k(kt), data(dt), pNext(NULL){};	

	void GetNext(node<Key, T>* &pNode)
	{
		pNode = pNext;
	}
	void SetNext(node<Key, T>* pNode)
	{
		pNext = pNode;
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
private:	
	T					data;
	const Key			k;
	node<Key, T>*		pNext;
};

template<typename Key, typename T>
struct elemHead
{		
	elemHead():head(NULL),nBucketNum(0),nTryGet(0){};
	node<const Key, T>* GetHead()
	{
		return head;
	}
	bool SetHead(node<const Key, T>* pOldHead, node<const Key, T>* pNewHead)
	{
		while(!spin_mu.TryGetLock()){};
		bool bSuc = spin_bool_compare_and_swap32(&head, pOldHead, pNewHead);		
		spin_mu.UnLock();
		return bSuc;
	}	
	void SetHeadUnsafe(node<const Key, T>* pHead)
	{
		head = pHead;
	}
	void LockHead()
	{
		nTryGet.AddValue(1);
		while(!spin_mu.TryGetLock()){};
	}
	void UnLockHead()
	{
		spin_mu.UnLock();
		nTryGet.AddValue(-1);
	}
	size_t GetCheckNum()
	{
		return nBucketNum;
	}
	void SetCheckNum(size_t nBucket)
	{
		nBucketNum = nBucket;
	}
	size_t GetTryCount()
	{
		size_t temp = 0;
		nTryGet.GetValue(temp);
		return temp;
	}
	
private:
	node<const Key, T>* head;
	spin_mutex_s		spin_mu;
	size_t				nBucketNum;
	atomic_m<size_t>	nTryGet;
};


template<typename T>
struct head_pool
{
#define HEAD_POOL_MAX_LEVEL		31
	head_pool(std::size_t count):m_nLevel(0),m_pDelayDel(NULL)
	{		
		memset(m_nLevelNode, 0, sizeof(size_t)*HEAD_POOL_MAX_LEVEL);		
		BuildNextLevelBuffer(count);		
	}

	~head_pool()
	{
		m_nLevel.SetValue(0);
		memset(m_nLevelNode, 0, sizeof(size_t)*HEAD_POOL_MAX_LEVEL);
		for (size_t i=0; i<HEAD_POOL_MAX_LEVEL; ++i)
		{
			if (nodes_[i] != NULL)
			{
				delete [] nodes_[i];
				nodes_[i] = NULL;
			}
		}
		if (m_pDelayDel != NULL)
		{
			delete [] m_pDelayDel;
		}
	}

	size_t BuildNextLevelBuffer(size_t nBuildNum)
	{
		if (nBuildNum == 0 || HEAD_POOL_MAX_LEVEL <= getLevel())
		{
			return 0;
		}
		if(!m_nLock.TryGetLock()){return 0;};
		if (m_pDelayDel != NULL)
		{
			delete [] m_pDelayDel;
			m_pDelayDel = NULL;
		}
		while(true)
		{
			size_t oldLevel;
			m_nLevel.GetValue(oldLevel);					
			if (nodes_[oldLevel] != NULL)
			{
				size_t nDelNum = m_nLevelNode[oldLevel];
				if (oldLevel > 0)
				{
					nDelNum -= m_nLevelNode[oldLevel-1];
				}
				printf("### Fail!!! Level:%d(%d) Builded! ###\n", oldLevel, nDelNum);	
				m_nLock.UnLock();
				return 0;
			}
			nodes_[oldLevel] = new T[nBuildNum];
			if(nodes_[oldLevel] == NULL)
			{
				std::cout << nBuildNum << " new fail" << std::endl;
				throw std::runtime_error("BuildNextLevelBuffer malloc fail");
				m_nLock.UnLock();
				return 0;
			}
			m_nLevelNode[oldLevel+1] = m_nLevelNode[oldLevel] + nBuildNum;
			printf("### Build Level:%d(%d) ###\n", oldLevel+1, nBuildNum);
			bool bSuc = m_nLevel.casSetValue(oldLevel, oldLevel+1);
			if (bSuc)
			{		
				m_nLock.UnLock();
				return nBuildNum;
			}
			delete [] nodes_[oldLevel];
			nodes_[oldLevel] = NULL;
		}
		m_nLock.UnLock();
		return 0;
	}
	bool DelMaxLevelBuffer(bool bDelayDel = false)
	{
		while(!m_nLock.TryGetLock()){};
		size_t nLevelDel = 0;
		m_nLevel.GetValue(nLevelDel);
		while(nLevelDel >0 && nLevelDel == m_nLevel.GetValueUnsafe())
		{			
			bool bSuc = m_nLevel.casSetValue(nLevelDel, nLevelDel-1);
			if (bSuc)
			{				
				if (bDelayDel && m_pDelayDel != NULL){
					delete [] m_pDelayDel;
				}
				m_pDelayDel = nodes_[nLevelDel-1];
				
				if (nodes_[nLevelDel-1] != NULL)
				{
					nodes_[nLevelDel-1] = NULL;
					size_t nDelNum = m_nLevelNode[nLevelDel];
					if (nLevelDel > 0)
					{
						nDelNum -= m_nLevelNode[nLevelDel-1];
					}
					printf("### Del The Max level:%d(%d) ###\n", nLevelDel, nDelNum);			
					m_nLevelNode[nLevelDel] = 0;
					if (!bDelayDel){
						delete [] m_pDelayDel;
						m_pDelayDel = NULL;
					}
				}
				m_nLock.UnLock();
				return true;
			}			
		}
		m_nLock.UnLock();
		return false;
	}

	T * nodes(size_t index)
	{
		size_t nLevel = m_nLevel.GetValueUnsafe();
		for (int i = nLevel; i>0; --i)
		{
			int uDis = index - m_nLevelNode[i-1];
			if (uDis >= 0 && nodes_[i-1] != NULL)
			{
				return reinterpret_cast<T*>(nodes_[i-1]+ uDis);
			}							
		}
		return NULL;
	}
	
	size_t capacity(void)
	{
		while(!m_nLock.TryGetLock()){}	
		size_t nCap = m_nLevelNode[m_nLevel.GetValueUnsafe()];	
		m_nLock.UnLock();
		return nCap;
	}

	size_t getLevel()
	{
		while(!m_nLock.TryGetLock()){}
		size_t nTemp = 0;
		m_nLevel.GetValue(nTemp);
		m_nLock.UnLock();
		return nTemp;
	}

private:	
	spin_mutex_s		m_nLock;
	atomic_m<size_t>	m_nLevel;
	T*					m_pDelayDel;
	size_t				m_nLevelNode[HEAD_POOL_MAX_LEVEL+1];
	T*	nodes_[HEAD_POOL_MAX_LEVEL];	
};

template <typename Key, typename T, typename Compare = std::equal_to<Key>,
	typename Hash = DefaultHash<Key> >
class hashmap
{	
	#define INCREASE_TIMES		2		// bucket数量增涨倍率（note:必须是大于1的正整数)
//#define HASH_MAP_BUCKET_END_FLAG		0xFFFFFFFE	
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
	bool		erase(const key_type& x);
	bool		find(const key_type& x, T &tTemp);

	bool		findElement(const key_type& x, T& data, size_t& bucket);
	bool		DeleteElement(const key_type& x, size_t& bucket);

	bool		ReSetBucketNum(size_t nNewSize);
	bool		empty() const;
	size_type	size() const;
	size_type	max_size() const;
	void		swap(hashmap<Key, T, Compare, Hash>& hashIn);

//	T& operator[] (const key_type& x);
	
	head_pool<elemHead<Key, T> >	m_tElemsT;
	atomic_m<int>					m_nSize;
	atomic_m<size_t>				m_nBucketSet;
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
	m_nBucketSet.SetValue(nBucketNum);
	m_Hash.SetNumBuckets(nBucketNum);
	if (m_Hash.GetNumBuckets() <= 0) {
		throw (invalid_argument("Number of buckets must be positive"));
	}
	for(size_t i=0; i<m_tElemsT.capacity(); ++i)
	{
		m_tElemsT.nodes(i)->SetCheckNum(nBucketNum);
	}
}

template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::~hashmap()
{
	if (m_tElemsT.capacity() != 0)
	{
		clear();		
	}
}
/*
template <typename Key, typename T, typename Compare, typename Hash>
hashmap<Key, T, Compare, Hash>::hashmap(
	const hashmap<Key, T, Compare, Hash>& src) :
m_nSize(0), m_Comp(src.m_Comp), m_Hash(src.m_Hash)
{
	size_t nBucket = (*src.m_pElems).size();
	m_pElems = new vector<elemHead>(nBucket);	
	for(size_t i=0; i<(*m_pElems).size(); ++i)
	{
		m_tElemsT.nodes(i)->SetCheckNum(nBucket);
		node<const Key, T>* pSrcTemp, pDesTemp;
		(*src.m_pElems)[i].GetHead(pSrcTemp);
		m_tElemsT.nodes(i)->GetHead(pDesTemp);
		while (pSrcTemp != NULL)
		{
			node* pNode = new node(pSrcTemp->GetKey(), pSrcTemp->GetData());			
			pNode->SetNext(pDesTemp);	
			m_tElemsT.nodes(i)->SetHead(pNode);			
			pSrcTemp->GetNext(pSrcTemp);
			m_nSize.AddValue(1);
		}
	}
}
//*/

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::findElement(const key_type& x, T& data, size_t& bucket)
{
	size_t nHashKey = m_Hash.hashKey(x);
//	bucket = m_Hash.hash(x);
ReFindElement:	
	elemHead<Key, T>* pBucket = m_tElemsT.nodes(bucket);
	if (pBucket == NULL)
	{
		bucket = m_Hash.hash(x);
		goto ReFindElement;
	}
	pBucket->LockHead();	
	// 检查是否在rehash
	size_t nCheckNum = pBucket->GetCheckNum();
	if (nCheckNum == 0 || bucket != nHashKey%nCheckNum)
	{	
		pBucket->UnLockHead();		
		if (nCheckNum == m_tElemsT.capacity())
		{
			bucket = nHashKey%nCheckNum;
		}
		Sleep(5);
		goto ReFindElement;
	}
	node<const Key, T>* list_temp = pBucket->GetHead();
	while (list_temp != NULL)
	{
		if (list_temp->GetKey() == x)
		{
			data = list_temp->GetData();
			pBucket->UnLockHead();
			return true;
		}
		list_temp->GetNext(list_temp);
	}
	pBucket->UnLockHead();
	return false;
}
template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::DeleteElement(const key_type& x, size_t& bucket)
{
	// Look for the key in the bucket.
	node<const Key, T>* list_temp;
	size_t nHashKey = m_Hash.hashKey(x);
	bucket = m_Hash.hash(x);
ReGetHead:
	elemHead<Key, T>* pBucket = m_tElemsT.nodes(bucket);
	if (pBucket == NULL)
	{
		bucket = m_Hash.hash(x);
		goto ReGetHead;
	}
	pBucket->LockHead();

	size_t nCheckNum = pBucket->GetCheckNum();
	if (bucket != nHashKey%nCheckNum)
	{
		pBucket->UnLockHead();
		if (nCheckNum != 0)
		{
			bucket = nHashKey%nCheckNum;
		}		
		goto ReGetHead;
	}
	list_temp = pBucket->GetHead();
	if (list_temp != NULL )
	{
		node<const Key, T>* list_next;
		list_temp->GetNext(list_next);
		if (list_temp->GetKey() == x)
		{
			pBucket->SetHeadUnsafe(list_next);			
			delete list_temp;
			pBucket->UnLockHead();
			m_nSize.AddValue(-1);
			return true;
		}
		while (list_next != NULL)
		{
			if (list_next->GetKey() == x)
			{
				node<const Key, T>* next;
				list_next->GetNext(next);
				list_temp->SetNext(next);
				delete list_next;
				pBucket->UnLockHead();
				m_nSize.AddValue(-1);
				return true;
			}
			list_temp = list_next;
			list_next->GetNext(list_next);
		}
	}	
	pBucket->UnLockHead();
	return false;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::find(const key_type& x, T &tTemp)
{
	size_t bucket = m_Hash.hash(x);
	bool bFind = findElement(x, tTemp, bucket);
	return bFind;
}

// unsafe !!!
/*
template <typename Key, typename T, typename Compare, typename Hash>
T& hashmap<Key, T, Compare, Hash>::operator[] (const key_type& x)
{	
	size_t bucket = m_Hash.hash(x);
	node<const Key, T>* pFind = findElement(x, bucket);
	if (pFind != NULL)
	{		
		return pFind->GetDataUnsafe();
	}
	else
	{
		insert(x);
	}
	
}
//*/

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::ReSetBucketNum(size_t nNewSize)
{
#define INCREASE_MAX_NUM	1048576
	size_t nOldSize = m_Hash.GetNumBuckets();
	if (nNewSize == nOldSize * INCREASE_TIMES)
	{			
		if (nNewSize >= INCREASE_MAX_NUM || nNewSize == 0 || m_tElemsT.capacity() != nOldSize)
		{
			return false;
		}						
		bool bSuc = m_nBucketSet.casSetValue(nOldSize, nNewSize);
		if (!bSuc){
			return false;
		}
		size_t nBuild = m_tElemsT.BuildNextLevelBuffer((INCREASE_TIMES-1)*nOldSize);
		if (nBuild == 0)
		{			
			m_nBucketSet.SetValue(nOldSize);				
			return false;
		}
		while(!m_resize_spinmu.TryGetLock()){}
		for(size_t i=nOldSize; i< nNewSize; ++i)
		{
			m_tElemsT.nodes(i)->SetCheckNum(nNewSize);
		}			
				
		_ReadWriteBarrier();

		Hash hashTemp;		
		hashTemp.SetNumBuckets(nNewSize);
		for (size_t i=0; i<nOldSize; ++i)
		{		
			m_tElemsT.nodes(i)->LockHead();
			m_tElemsT.nodes(i)->SetCheckNum(nNewSize);
			node<const Key, T>* pHead = m_tElemsT.nodes(i)->GetHead();					
			if(pHead == NULL)
			{
				m_tElemsT.nodes(i)->UnLockHead();
				continue;
			}			
			// 因为是单向链表，删除头指针结点的逻辑不一样.
			size_t newHash = hashTemp.hash(pHead->GetKey());
			while (pHead != NULL && newHash != i)
			{
				node<const Key, T>* pTemp;
				pHead->GetNext(pTemp);						
				// for test
				size_t oldHash = m_Hash.hash(pHead->GetKey());
				if (i != oldHash || newHash < nOldSize)
				{
					printf("###########Error1 Data:%d = %d new bucket:%d############\n", oldHash, i, m_Hash.GetNumBuckets());
				}				

				m_tElemsT.nodes(newHash)->LockHead();
				node<const Key, T>* pHeadOld = m_tElemsT.nodes(newHash)->GetHead();	
				pHead->SetNext(pHeadOld);
				m_tElemsT.nodes(newHash)->SetHeadUnsafe(pHead);				
				m_tElemsT.nodes(newHash)->UnLockHead();

				m_tElemsT.nodes(i)->SetHeadUnsafe(pTemp);								
				pHead = pTemp;
				if (pHead != NULL)
				{
					newHash = hashTemp.hash(pHead->GetKey());
				}				
			}
			if (pHead == NULL)
			{
				m_tElemsT.nodes(i)->UnLockHead();
				continue;
			}	
			// 删除头指针后面的结点
			node<const Key, T>* pPre = pHead;
			node<const Key, T>* pNext = NULL;
			pPre->GetNext(pNext);
			while(pNext != NULL)
			{		
				size_t newHash = hashTemp.hash(pNext->GetKey());	
				if (newHash != i)
				{
					// for test
					size_t oldHash = m_Hash.hash(pNext->GetKey());
					if (i != oldHash || newHash < nOldSize)
					{
						printf("########### Error2 Data:%d = %d new bucket:%d ############\n", oldHash, i, m_Hash.GetNumBuckets());
					}
					node<const Key, T>* pNextNext = NULL;
					pNext->GetNext(pNextNext);
					pPre->SetNext(pNextNext);

					m_tElemsT.nodes(newHash)->LockHead();
					node<const Key, T>* pHeadOld = m_tElemsT.nodes(newHash)->GetHead();	
					pNext->SetNext(pHeadOld);
					m_tElemsT.nodes(newHash)->SetHeadUnsafe(pNext);				
					m_tElemsT.nodes(newHash)->UnLockHead();

					pPre->GetNext(pNext);
				}
				else
				{
					pPre->GetNext(pPre);
					pPre->GetNext(pNext);
				}
			}			
			m_tElemsT.nodes(i)->UnLockHead();
			_ReadWriteBarrier();
		}
		m_Hash.SetNumBuckets(nNewSize);
		m_resize_spinmu.UnLock();
		return true;
		//*/
	}
	else if(nNewSize == nOldSize/INCREASE_TIMES)
	{
		if (nNewSize == 0 || m_tElemsT.capacity() != nOldSize)
		{
			return false;
		}		
		bool bSuc = m_nBucketSet.casSetValue(nOldSize, nNewSize);
		if(!bSuc){
			return false;
		}
		while(!m_resize_spinmu.TryGetLock()){}	
		for(size_t i=0; i< nNewSize; ++i)
		{
			m_tElemsT.nodes(i)->SetCheckNum(nNewSize);
		}		
		m_Hash.SetNumBuckets(nNewSize);	
		_ReadWriteBarrier();		
		
		for (size_t i=nNewSize; i<nOldSize; ++i)
		{
			m_tElemsT.nodes(i)->LockHead();				
			while (m_tElemsT.nodes(i)->GetTryCount() > 1)
			{
				m_tElemsT.nodes(i)->UnLockHead();
				Sleep(1);
				m_tElemsT.nodes(i)->LockHead();	
			}
			m_tElemsT.nodes(i)->SetCheckNum(nNewSize);
			node<const Key, T>* pHead = m_tElemsT.nodes(i)->GetHead();
			node<const Key, T>* pHeadTemp = NULL;
			while(pHead != NULL)
			{
				pHeadTemp = pHead;
				pHead->GetNext(pHead);
				size_t newHash = m_Hash.hash(pHeadTemp->GetKey());
				if (newHash == i)
				{
					printf("#### ERROR hash key ####");
				}
				m_tElemsT.nodes(newHash)->LockHead();
				node<const Key, T>* pHeadNew = m_tElemsT.nodes(newHash)->GetHead();
				pHeadTemp->SetNext(pHeadNew);
				m_tElemsT.nodes(newHash)->SetHeadUnsafe(pHeadTemp);				
				m_tElemsT.nodes(newHash)->UnLockHead();
				_ReadWriteBarrier();
			}
			_ReadWriteBarrier();
			m_tElemsT.nodes(i)->SetHeadUnsafe(NULL);
		}
		m_tElemsT.DelMaxLevelBuffer(true);
		m_resize_spinmu.UnLock();
		return true;
	}
	return false;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::insert(const Key x, T data)
{
	size_t nHashKey = m_Hash.hashKey(x);
	size_t bucket = m_Hash.hash(x);
ReInsert:	
	bool bFind = findElement(x, data, bucket);	
	if (!bFind) {
		int nSizeTemp =0;
		m_nSize.GetValue(nSizeTemp);
		size_t nBucket = 0;
		m_nBucketSet.GetValue(nBucket);
		if (m_bDynamicBucket && nSizeTemp/nBucket > 32){
			if (ReSetBucketNum(nBucket * INCREASE_TIMES))
			{
				bucket = m_Hash.hash(x);
				goto ReInsert;
			}
		}
		node<const Key, T>* pNode = new node<const Key, T>(x, data);
		if (pNode == NULL){
			return false;
		}
		
		elemHead<Key, T>* pBucket = m_tElemsT.nodes(bucket);
		if (pBucket == NULL)
		{
			bucket = m_Hash.hash(x);
			goto ReInsert;
		}
		pBucket->LockHead();
		
		size_t nCheckNum = pBucket->GetCheckNum();
		if (m_bDynamicBucket && (nCheckNum == 0 || bucket != nHashKey%nCheckNum))
		{	// 检查是否在rehash
			delete pNode;
			pBucket->UnLockHead();
			if (nCheckNum == m_tElemsT.capacity())
			{
				bucket = nHashKey%nCheckNum;
			}
			Sleep(5);
			goto ReInsert;
		}		
		node<const Key, T>* pHead = NULL;
		pHead = pBucket->GetHead();
		pNode->SetNext(pHead);							
		pBucket->SetHeadUnsafe(pNode);
		pBucket->UnLockHead();		
		m_nSize.AddValue(1);
		return true;
	}	
	//*/	
}

template <typename Key, typename T, typename Compare, typename Hash>
void hashmap<Key, T, Compare, Hash>::clear(void)
{
	size_t temp = m_tElemsT.capacity();
	if (m_tElemsT.capacity() == 0)
	{
		return;
	}
	int i=m_tElemsT.capacity()-1;
	for(; i>=0; --i)
	{		
		m_tElemsT.nodes(i)->LockHead();
		node<const Key, T>* pHead = m_tElemsT.nodes(i)->GetHead();		
		m_tElemsT.nodes(i)->SetHeadUnsafe(NULL);
		while (pHead != NULL)
		{
			node<const Key, T>* pTemp = pHead;
			pHead->GetNext(pHead);
			delete pTemp;
			m_nSize.AddValue(-1);
		}
		m_tElemsT.nodes(i)->UnLockHead();
	}
	if (m_nSize.GetValueUnsafe() != 0)
	{
		m_nSize.reset();
	}	

	while(m_bDynamicBucket && m_tElemsT.getLevel() > 1)
	{
		if (m_tElemsT.DelMaxLevelBuffer())
		{
			size_t nCapNum = m_tElemsT.capacity();
			m_nBucketSet.SetValue(nCapNum);
			m_Hash.SetNumBuckets(nCapNum);
			for(size_t n = 0; n<nCapNum; ++n)
			{
				m_tElemsT.nodes(n)->SetCheckNum(nCapNum);
			}
		}			
	}	
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::erase(const key_type& x)
{
	size_t bucket = m_Hash.hash(x);
	bool bDel = DeleteElement(x, bucket);	
	if (m_bDynamicBucket && bDel)
	{
		int nNodeNum = 0;
		m_nSize.GetValue(nNodeNum);
		size_t nBucket = 0;
		m_nBucketSet.GetValue(nBucket);
		if (m_tElemsT.getLevel() > 1 && (float)nNodeNum/nBucket < 0.5)
		{
//			ReSetBucketNum(nBucket/INCREASE_TIMES);
		}		
	}
	return bDel;
}

template <typename Key, typename T, typename Compare, typename Hash>
bool hashmap<Key, T, Compare, Hash>::empty() const
{
	return (m_nSize.GetValue() == 0);
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::size_type
	hashmap<Key, T, Compare, Hash>::size() const
{
	return (m_nSize.GetValue());
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
	if (m_nSize.GetValue() == 0) {
		return (end());
	}	
	for (size_t i = 0; i < m_pElems->size(); ++i) {
		if (!(m_tElemsT.nodes(i)->empty())) {
			return (HashIterator<Key, T, Compare, Hash>(i,
				m_tElemsT.nodes(i)->begin(), this));
		}
	}
	return (end());
}

template <typename Key, typename T, typename Compare, typename Hash>
typename hashmap<Key, T, Compare, Hash>::iterator
	hashmap<Key, T, Compare, Hash>::end()
{
	size_t nEnd = m_pElems->size()-1;
	hashmap<Key, T, Compare, Hash>::iterator iter = HashIterator<Key, T, Compare, Hash>(nEnd,
		(*m_pElems)[nEnd].end(), this);
	return iter;
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