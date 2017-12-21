#pragma once

#include "atomic.h"

#define SHOW_DEBUG_ARRAY_POOL

// 动态数组池
template <typename T>
class ArrayPool
{
#define HEAD_POOL_MAX_LEVEL		32		// 数组池最大级数（数组段数）
#define ARRAY_BEGIN_SIZE_MIN	2		// 初始数组最小长度
#define ARRAY_BEGIN_SIZE_MAX	65536	// 初始数组最大长度
public:
	ArrayPool(size_t initNum = 2, bool bBinary = false);	// bBinary:是否确保初始值为2的幂
	~ArrayPool();
	
	virtual size_t	BuildNextLevelBuffer(size_t nOldCap);
	// 回收最大一级数组内存
	virtual bool	DelMaxLevelBuffer(size_t nOldCap);
	// 由索引得到数组对象
	virtual T*		nodes(size_t index);
	
	virtual size_t	capacity(void);
	virtual size_t  capacityUnsafe(void);
	virtual size_t	getLevel();
	virtual void    setResizeAble(bool bCan);

protected:
	// 初始化第一级数组
	virtual size_t	InitBuffer(size_t nBuildNum, bool bBinary = false);
	// 得到不小于输入值的最小2的幂
	inline size_t GetMinPowOfNum2(int nNum);
protected:
	byte				m_bBinaryInit;
	bool				m_bCanResize;
	spin_mutex_s		m_spinLock;
	spin_mutex_s		m_resizeLock;
	size_t				m_nLevel;
	size_t				m_nLevelNode[HEAD_POOL_MAX_LEVEL+1];
	T*					nodes_[HEAD_POOL_MAX_LEVEL];
};

template <typename T>
ArrayPool<T>::ArrayPool(size_t initNum/* = 2 */, bool bBinary /* = false */)
	:m_bBinaryInit(0),m_spinLock(0),m_nLevel(0),m_bCanResize(true)
{
	memset(m_nLevelNode, 0, sizeof(m_nLevelNode));
	memset(nodes_, 0, sizeof(nodes_));
	InitBuffer(initNum, bBinary);
}

template <typename T>
ArrayPool<T>::~ArrayPool()
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

template <typename T>
size_t ArrayPool<T>::InitBuffer(size_t nBuildNum, bool bBinary/* = false */)
{
	while(!m_spinLock.TryGetLock()){};
	nBuildNum = max(nBuildNum, ARRAY_BEGIN_SIZE_MIN);
	nBuildNum = min(nBuildNum, ARRAY_BEGIN_SIZE_MAX);		
	if (bBinary)
	{	// 确定输入的值一定是2的幂，否则取不比它小的最小的幂
		nBuildNum = GetMinPowOfNum2(nBuildNum);
	}		
	size_t nTemp = nBuildNum>>1;
	while (nTemp != 0)
	{
		++m_bBinaryInit;
		nTemp = nTemp>>1;
	}		
	nodes_[m_nLevel] = new T[nBuildNum];	
	if (nodes_[m_nLevel] == NULL){
		m_spinLock.UnLock();
		return 0;
	}		
	++m_nLevel;		
	m_nLevelNode[m_nLevel] = nBuildNum;
#ifdef SHOW_DEBUG_ARRAY_POOL
	printf("### Init Buffer Level:%d(%d) ###\n", m_nLevel, nBuildNum);
#endif	
	m_spinLock.UnLock();
	return nBuildNum;
}

template <typename T>
size_t ArrayPool<T>::BuildNextLevelBuffer(size_t nOldCap)
{
	while(!m_spinLock.TryGetLock()){return 0;};
	while(!m_resizeLock.TryGetLock()){};
	if (m_nLevel == 0 || nOldCap != m_nLevelNode[m_nLevel] || !m_bCanResize)
	{
		m_resizeLock.UnLock();
		m_spinLock.UnLock();		
		return 0;
	}
	// 非初始化，每次增涨1倍空间
	size_t nBuildNum = m_nLevelNode[m_nLevel];
#ifdef SHOW_DEBUG_ARRAY_POOL
	printf("### Build Buffer Level:%d(%d) ###\n", m_nLevel, nBuildNum);
#endif	
	nodes_[m_nLevel] = new T[nBuildNum];
	if (nodes_[m_nLevel] == NULL){
		m_resizeLock.UnLock();
		m_spinLock.UnLock();
		return 0;
	}	

	++m_nLevel;	
	m_nLevelNode[m_nLevel] = m_nLevelNode[m_nLevel-1]+nBuildNum;	
	m_resizeLock.UnLock();
	m_spinLock.UnLock();
	return nBuildNum;
}

template <typename T>
bool ArrayPool<T>::DelMaxLevelBuffer(size_t nOldCap)
{
	if (m_nLevel <= 1)	// 初始化的那层内存不释放
	{	
		return false;
	}
	while(!m_spinLock.TryGetLock()){return false;};
	while(!m_resizeLock.TryGetLock()){};
	if (nOldCap != m_nLevelNode[m_nLevel] || !m_bCanResize)	
	{
		m_resizeLock.UnLock();
		m_spinLock.UnLock();
		return false;
	}		
#ifdef SHOW_DEBUG_ARRAY_POOL
	printf("### Delet Buffer Level:%d(%d) ###\n", m_nLevel-1, m_nLevelNode[m_nLevel-1]);
#endif	
	m_nLevelNode[m_nLevel] = 0;
	--m_nLevel;	
	if (nodes_[m_nLevel] != NULL)
	{				
		delete [] nodes_[m_nLevel];
		nodes_[m_nLevel] = NULL;
	}	
	m_resizeLock.UnLock();
	m_spinLock.UnLock();
	return true;
}	

template <typename T>
T* ArrayPool<T>::nodes(size_t index)
{		
	if (m_nLevel == 0 || (m_nLevel+m_bBinaryInit) > HEAD_POOL_MAX_LEVEL )
	{			
		return NULL;
	} 
	while(!m_spinLock.TryGetLock()){};
	for (int i = m_nLevel-1; i>=0; --i)
	{
		int uDis = index - m_nLevelNode[i];
		if (uDis >= 0 && nodes_[i] != NULL)
		{
			m_spinLock.UnLock();
			return reinterpret_cast<T*>(nodes_[i]+ uDis);
		}							
	}
	m_spinLock.UnLock();
	return NULL;
}

template <typename T>
size_t ArrayPool<T>::capacity(void)
{
	while(!m_spinLock.TryGetLock()){};
	size_t nCap = m_nLevelNode[m_nLevel];	
	m_spinLock.UnLock();
	return nCap;
}

template <typename T>
size_t ArrayPool<T>::capacityUnsafe(void)
{
	return m_nLevelNode[m_nLevel];
}

template <typename T>
size_t ArrayPool<T>::getLevel()
{
	while(!m_spinLock.TryGetLock()){};
	size_t nTemp = m_nLevel;
	m_spinLock.UnLock();
	return nTemp;
}

template <typename T>
void ArrayPool<T>::setResizeAble(bool bCan)
{
	while(!m_resizeLock.TryGetLock()){};
	m_bCanResize = bCan;
	m_resizeLock.UnLock();
}

template <typename T>
inline size_t ArrayPool<T>::GetMinPowOfNum2(int nNum)
{
	size_t nSizeB = 0;
	while ((nNum & (nNum-1)) != 0)
	{
		nNum &= (nNum-1);
		++nSizeB;
	}
	if (nSizeB > 0)	
	{
		nNum = nNum<<1;
	}
	return nNum;
}