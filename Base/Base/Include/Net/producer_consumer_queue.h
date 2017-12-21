/*******************************************************************
** 文件名:	producer_consumer_queue.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	05/22/2009
** 版  本:	1.0
** 描  述:	生产者消费者无锁队列
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __PRODUCTER_CONSUMER_QUEUE_H__
#define __PRODUCTER_CONSUMER_QUEUE_H__

#include "Trace.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
所谓的生产者消费者队列，即一个线程读一个线程写的队列
我们这里实现的是一个无锁的生产者消费者队列,算法的关键点:

1.pop只修改头节点,push只修改尾节点
2.保证head和tail非空,因为一旦head和tail为空,push时就必须同时改写head和tail

特别说明:
1.该生成者消费者队列理论上同时只允许一个线程读和一个线程写
2.可以允许多个读线程申请读,但每一时刻只允许一个真正读,同理可以允许多个写线程申请写,但每一时刻只允许一个写,具体用户参考下面的实现
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _DATA>
class producer_consumer_queue
{
	struct NODE
	{
		_DATA   data;
		NODE *  next;
	};

	NODE *  m_pHead;
	NODE *  m_pTail;

	volatile unsigned long    m_nPushCount;
	volatile unsigned long    m_nPopCount;

#ifdef SUPPORT_NET_NAMEINF
	std::string m_strName;	// 名称，用来调试显示拥有者
#endif

public:
	producer_consumer_queue() : m_nPushCount(0),m_nPopCount(0)
	{
		m_pHead = new NODE;
		assert(m_pHead);

		// 保证 Head 和 Tail 非空
		m_pHead->next = 0;
		m_pTail = m_pHead;
	};

	virtual ~producer_consumer_queue()
	{
		_DATA temp_data;
		while(pop(temp_data))
		{

		}

		if ( m_pHead )
		{
			delete m_pHead;
			m_pHead = 0;
			m_pTail = 0;
		}
	}

	// 同时只允许一个线程push
	void push( _DATA & data )
	{
		NODE * pNode = new NODE;
		pNode->data  = data;
		pNode->next  = 0;

		m_pTail->next= pNode;
		m_pTail      = pNode;

		++m_nPushCount;

		static DWORD s_dwTick = 0;
		if ( m_nPushCount-m_nPopCount>2048 && GetTickCount()-s_dwTick>10000 )
		{
			s_dwTick = GetTickCount();
			DWORD dwPackCounts = m_nPushCount-m_nPopCount;
#ifdef SUPPORT_NET_NAMEINF
			if (dwPackCounts>10000)
			{
				ErrorLn("["<<m_strName.c_str()<<"] Queue busying, count=" << dwPackCounts << " type=" << sizeof(_DATA) );
			}
			else
			{
				WarningLn("["<<m_strName.c_str()<<"] Queue busying, count=" << dwPackCounts << " type=" << sizeof(_DATA) );
			}
#else
			if (dwPackCounts>10000)
			{
				ErrorLn("Queue busying, count=" << dwPackCounts << " type=" << sizeof(_DATA) );
			}
			else
			{
				WarningLn("Queue busying, count=" << dwPackCounts << " type=" << sizeof(_DATA) );
			}
#endif
		}
	}

	// 同时只允许一个线程pop
	bool pop( _DATA & data )
	{
		if ( m_pHead->next != 0 )
		{
			++m_nPopCount;

			NODE * pNode = m_pHead;
			m_pHead = m_pHead->next;
			data    = m_pHead->data;
			delete pNode;
			return true;
		}

		return false;
	}

	unsigned long count()
	{
		return m_nPushCount-m_nPopCount;
	}

public:

	bool setName(const char * pstrName) 
	{
#ifdef SUPPORT_NET_NAMEINF
		if (0==pstrName)
			return false;
		m_strName = pstrName;
		return true;
#else
		return false;
#endif
	}
	const char * getName()
	{
#ifdef SUPPORT_NET_NAMEINF
		return m_strName.c_str();
#else
		return "";
#endif
	}

};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
前面producer_consumer_queue的实现每次push都需要执行一个new操作,每次pop都需要执行一次free操作,性能损失非常大
现实现一个不需要分配内存的版本，性能将大幅度提升，前提条件就是：
1.传入的对象必须实现一个pNext的成员变量
2.每个队列中的最后一个元素将无法取出，这是算法上无法规避的一个缺陷，只能用在最后一个元素不取出也影响不大的地方
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename NODE>
class producer_consumer_queue_ptr
{
	NODE *  m_pHead;
	NODE *  m_pTail;

	NODE    m_HeadPole;

	volatile unsigned long    m_nPushCount;
	volatile unsigned long    m_nPopCount;


public:
	producer_consumer_queue_ptr() : m_nPushCount(0),m_nPopCount(0)
	{
		m_pHead = &m_HeadPole;

		// 保证 Head 和 Tail 非空
		m_pHead->pNext = 0;
		m_pTail = m_pHead;
	};

	virtual ~producer_consumer_queue_ptr()
	{
		NODE * temp_data = 0;
		while(pop(temp_data))
		{

		}

		if ( m_pHead )
		{
			m_pHead = 0;
			m_pTail = 0;
		}
	}

	// 注意事项：
	// 1.同时只允许一个线程push
	// 2.传入的对象必须实现一个next的成员变量
	// 3.每个队列中的最后一个元素将无法取出，这是算法上无法规避的一个缺陷，只能用在最后一个元素不取出也影响不大的地方
	void push( NODE * pNode )
	{
		pNode->pNext  = 0;

		m_pTail->pNext= pNode;
		m_pTail       = pNode;

		++m_nPushCount;
	}


	// 注意事项：
	// 1.同时只允许一个线程pop
	// 2.传入的对象必须实现一个next的成员变量
	// 3.每个队列中的最后一个元素将无法取出，这是算法上无法规避的一个缺陷，只能用在最后一个元素不取出也影响不大的地方
	bool pop( NODE *& pNode  )
	{
		if ( m_pHead->pNext != 0 )
		{
			if ( m_pHead==&m_HeadPole )
			{
				m_pHead = m_pHead->pNext;
				return pop( pNode ); // 第一个元素直接调过
			}

			++m_nPopCount;

			pNode = m_pHead;
			m_pHead = m_pHead->pNext;

			return true;
		}

		return false;
	}

	unsigned long count()
	{
		return m_nPushCount-m_nPopCount;
	}

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
带读写权利控制的生产者消费者队列
即:允许多个读线程申请读,但读之前需要取得读权利,也允许多个写线程申请写,但写之前需要取得写权利
用法:
1.直接调用push_ex和pop_ex
2.自己调用acquire_read_permission和pop等函数,但要注意线程互斥逻辑
3.如果要调用push和pop接口，你需要自己确定只有一个线程调用
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _DATA>
class producer_consumer_queue_ex : public producer_consumer_queue<_DATA>
{
	volatile	LONG  m_nReadBlock;		// 读标志
	volatile    LONG  m_nWriteBlock;    // 写标志
	volatile    LONG  m_nBackBlock;     // 后备队列权限

	producer_consumer_queue<_DATA>  m_back_queue; // 后备写队列

public:
	producer_consumer_queue_ex() : producer_consumer_queue(),m_nReadBlock(0),m_nWriteBlock(0),m_nBackBlock(0)
	{
	}

	virtual ~producer_consumer_queue_ex(){}

	//void push( _DATA * pNode )
	//{
	//	ErrorLn("producer_consumer_queue_ex can not call push method. please call push_ex instead.");
	//}

	//bool pop( _DATA *& pNode  )
	//{
	//	ErrorLn("producer_consumer_queue_ex can not call pop method. please call pop_ex instead.");
	//	return false;
	//}

	// 带权利的写
	bool push_ex( _DATA & data )
	{
		int  i = 0;
		while(true)
		{
			if ( acquire_write_permission() )
			{
				// 写入后备队列
				if( m_back_queue.count()>0 && acquire_back_permission() )
				{
					_DATA temp;
					while( m_back_queue.pop( temp ) )
						producer_consumer_queue::push(temp);

					release_back_permission();
				}

				// 写入写队列
				producer_consumer_queue::push(data);
				release_write_permission();
				return true;
			}else
			{
				// 写入后备队列
				if(acquire_back_permission() )
				{
					m_back_queue.push( data );
					release_back_permission();
					return true;
				}

				if ( ++i>1000 ) // 经测试1000的值最好，少了Sleep的次数会过多，严重影响效率,多了会导致忙等，其他线程拿不到总线，最大等待时间会增加
				{
					i = -1000;  // Sleep一次就增加饥渴度，让他下次更容易获得
                    WarningLn(__FUNCTION__":sleep");
					Sleep(0);
				}
			}
		}

		return false;
	}

	// 带权利的读
	// @param is_peek : 只看一下有没有，如果当前锁住了直接返回
	bool pop_ex( _DATA & data,bool is_peek=false )
	{
		if ( count()==0 )  // 加一个判断，防止过多的调用原子操作无畏的锁内存总线
			return false;

		if ( acquire_read_permission(is_peek) )
		{
			bool ret = producer_consumer_queue::pop( data );
			release_read_permission();
			return ret;
		}

		return false;
	}

	// 取得读权限
	bool acquire_read_permission(bool is_peek=false)
	{
		int  i = 0;
		while ( ::InterlockedCompareExchange(&m_nReadBlock,1,0)!=0 )
		{
			// 直接返回不要硬取
			if (is_peek)
				return false;  
			
			if ( ++i>1000 ) // 经测试1000的值最好，少了Sleep的次数会过多，严重影响效率,多了会导致忙等，其他线程拿不到总线，最大等待时间会增加
			{
				i = -1000;  // Sleep一次就增加饥渴度，让他下次更容易获得
                WarningLn(__FUNCTION__":sleep");
				Sleep(0);
			}
		}

		return true;
	}

	// 释放读权限
	void release_read_permission()
	{
		m_nReadBlock = 0;
	}


	// 取得写权限
	bool acquire_write_permission()
	{
		return ::InterlockedCompareExchange(&m_nWriteBlock,1,0)==0;
	}

	// 释放写权限
	void release_write_permission()
	{
		m_nWriteBlock = 0;
	}

	// 取得后备队列权限
	bool acquire_back_permission()
	{
		return ::InterlockedCompareExchange(&m_nBackBlock,1,0)==0;
	}

	// 释放后备队列权限
	void release_back_permission()
	{
		m_nBackBlock = 0;
	}
};

#endif//__PRODUCTER_CONSUMER_QUEUE_H__