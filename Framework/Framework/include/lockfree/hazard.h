#pragma once
#include "windows.h"
#include <set>
#include "nocopyable.h"
#include "atomic.h"
#include <iostream>

class hprecord : noncopyable
{
public:
	friend class hazard;

	static atomic<hprecord*> head;

	static void destroy()
	{
		hprecord* h = head.load();
		while(h != NULL)
		{
			hprecord* p = h;
			h = h->next;
			delete p;
		}
	}

	hprecord* acquire()
	{
		hprecord* p = head.load();
		int activeexpected = 0;
		int activedesired = 1;
		while(p != NULL)
		{
			if(p->active.load() != 0 || !p->active.compare_exchange(activeexpected, activedesired))
			{
				p = p->next;
				continue;
			}

			return p;
		}

		p = new hprecord;
		if(p == NULL)
		{
			return NULL;
		}

		do
		{
			p->next = head.load();
			p->active.store(1);
		}while(!head.compare_exchange(p->next, p));
		
		return p;
	}

	void release(hprecord* p)
	{
		if(p == NULL)
		{
			return;
		}
		p->active.store(0);
		p->phazard = 0;
	}

protected:
	hprecord() : next(NULL), phazard(NULL){active.store(0);}

public:
	hprecord* next;
	atomic<int> active;
	void* phazard;
};

#define HAZARD_R (8) 

class tlsrlist
{
public:
	typedef std::set<void*> rlist_t;
	static atomic<DWORD>  tlskey;

	static void init()
	{
		DWORD key = tlskey.load();
		if(key == 0)
		{
			DWORD key_ = TlsAlloc();
			if(!tlskey.compare_exchange(key, key_))
			{
				TlsFree(key_);
			}
		}
	}

	static void destroy()
	{
		DWORD key = tlskey.load();

		if(key != 0)
		{
			if(tlskey.compare_exchange(key, 0))
			{
				TlsFree(key);
			}
		}
	}

	static rlist_t* getrlist()
	{
		init();

		LPVOID p = TlsGetValue(tlskey.load());
		if(p == NULL)
		{
			p = new rlist_t;
			TlsSetValue(tlskey.load(), p);
		}

		return (rlist_t*)p;
	}

	static void retire(void* p)
	{
		rlist_t* rlist = getrlist();
		if(rlist == NULL)
		{
			return;
		}

		rlist->insert(p);

		if(rlist->size() > HAZARD_R)
		{
			scan(hprecord::head.load());
		}
	}

	static void scan(hprecord* head)
	{
		rlist_t* rlist = getrlist();
		if(rlist == NULL)
		{
			return;
		}

		rlist_t hlist;
		while(head != NULL)
		{
			void* p = head->phazard;
			if(p != NULL)
			{
				hlist.insert(p);
			}
			head = head->next;
		}
		
		for(rlist_t::iterator iter = rlist->begin(); iter != rlist->end(); )
		{
			if(hlist.find(*iter) == hlist.end())
			{
				free(*iter);

				rlist->erase(iter++);
				continue;
			}

			++iter;
		}
	}
};

// 对外接口 实现smr(安全内存回收)
class hazard : noncopyable
{
public:
	static atomic<hazard*> s_p;

	static hazard* getinstance()
	{
		hazard* p = s_p.load();
		if(p == NULL)
		{
			hazard* q = new hazard;
			if(!s_p.compare_exchange(p, q))
			{
				delete q;
			}
		}

		return s_p.load();
	}

	static void destroy()
	{
		hazard* p = s_p.load();
		if(p != NULL)
		{
			hazard* q = NULL;
			if(s_p.compare_exchange(p, q))
			{
				delete p;
			}
		}

		s_p.store(NULL);
	}

	// 获取风险指针
	hprecord* acquirehp()
	{
		return m_hp.acquire();
	}

	// 释放风险指针
	void releasehp(hprecord*& p)
	{
		m_hp.release(p);
		p = NULL;
	}
	
	// 归还内存
	void retire(void* p)
	{
		tlsrlist::retire(p);
	}

	void dumpmemory()
	{
		tlsrlist::rlist_t* rlist = tlsrlist::getrlist();
		if(rlist == NULL)
		{
			return;
		}

		std::cout << "thread(" << GetCurrentThreadId() << ") leak nodes count: " << rlist->size() << std::endl;
	}

private:
	hazard(){}
	hprecord m_hp;
};