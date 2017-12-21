#pragma once

// 为了刷脚本线程安全，维护一个刷过的脚本的备份列表，不删除。
template<typename T>
class CSchemeCache
{
public:
	CSchemeCache() : m_pNext(NULL){}

	static T* m_pHead;

	// 要刷脚本时用新实例
	static T* NewInstance()
	{
		if(m_pHead != NULL)
		{
			return new T(*m_pHead); 
		}

		return new T;
	}
	
	// 读取时用当前的工作实例
	static T* GetWorkInstance()
	{
		return m_pHead;
	}

	// 刷完后替换工作实例
	void AfterSchemeLoaded()
	{
        // scheme里Load好几个文件会导致相同的对象挂到链表，扫描一遍
        for(T* p = m_pHead; p != NULL; p = p->m_pNext)
        {
            if(this == p)
            {
                return;
            }
        }

		((CSchemeCache<T>*)this)->m_pNext = m_pHead;
		m_pHead = (T*)this;
	}

	// 销毁
	static void CloseAll()
	{
		T* pIter = m_pHead;
		while(pIter != NULL)
		{
			pIter->Close();
			T* pNext = ((CSchemeCache<T>*)pIter)->m_pNext;

			delete pIter;
			pIter = pNext;
		}
		m_pHead = NULL;
	}

private:
	T* m_pNext;
};

template<typename T>
T* CSchemeCache<T>::m_pHead = NULL;