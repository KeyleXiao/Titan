#pragma once

#include "nocopyable.h"
#include "freelist.h"

/* 
实现：
	这种无锁queue是用cas + 忙等实现的。
	cas：compare and set(swap): 原子的比较并更新long 或者 longlong类型的内存内容
	结构组成：queue、atomic操作(封装了cas)、freelist(自己维护自由内存列表)
	加freelist原因：如果直接用系统内存会有ABA问题： cas操作假如只比较并更新节点指针、那么
		这个指针很容易被释放后再申请到相同的指针，cas测试通过即指针相同的情况下说不定queue已经被其他线程修改了。
		所以我们自己维护freelist。每个node内存头信息里都有一个标签tag。tag是累加计数的，cas比较的时候不仅比较指针而且同时比较tag。
		由于freelist也是多线程环境下的，所以freelist也是无锁结构(cas + 忙等)实现。
适用：
	各个线程对队列的操作不是非常频繁（例如while(push) while(pop)）。因为cas测试+忙等，如果竞争越频繁发生 会产生越多cas失败，
	单次cas操作比mutex速度快了很多，但是假如cas太多则会影响一点效率。
优点：
	相对加锁版本提高一般并发环境下对队列访问的速度、避免死锁、线程优先级倒置。
使用：
	queue<T, fixesize>: 根据freelist的实现方式不同实现了两种队列：一种是固定大小freelist（队列长度固定不可增长 push满了返回失败）。一种是可动态增长freelist(队列长度可动态增长)。
	注意：T要实现=操作
*/

namespace speed
{
	// 无锁队列 fixedsize: -- true 用固定大小自由列表 -- false 使用可动态增长自由列表
	template<typename T, bool fixedsize = false>
	class queue: noncopyable
	{
	public:
		struct node // 带头信息(tag, next)的节点
		{
			node()
			{

			}

			node(const T& v, index_t t, index_t null_handle) : data(v)
			{
				node_head.store(memeroy_head_info(null_handle, t));
			}

			~node()
			{
				data.~T();
			}
			
			atomic<memeroy_head_info> node_head;
			T data;
		};

		typedef typename select_free_list<node, fixedsize>::type pool_t;
		
		// 初始化，内部接口
		void initialize()
		{
			T o;
			node* n = pool.construct<T>(o);
			index_t handle = pool.get_handle(n);

			tagged_index dummy(handle, n->node_head.load().get_tag());
			head.store(dummy);
			tail.store(dummy);
		}
		
		queue(std::size_t size) : pool(size)
		{
			initialize();
		}

		~queue() // 先返回给pool 让pool自己释放内存
		{
			T ret;
			while(pop(ret));
		}
		
		// 压入一个元素
		bool push(const T& v)
		{
			node* n = pool.construct<T>(v); // 从自由列表取一个节点并在这个节点上构造待压入的节点
			if(n == NULL)
			{
				return false;
			}

			index_t node_handle = pool.get_handle(n);
			index_t node_tag = n->node_head.load().get_tag();
			
			// 假如其他线程跟本线程发生竞争的概率小，所有线程cas成功率比较高的情况下，大家相安无事，不需要忙等。比每次固定加解锁性能会提高很多。
			// 假如竞争频繁，那么越失败就会尝试越多次的cas，性能就会变得比较差。cas操作应该是硬件级的加锁虽然速度比mutex快，但是开销也比一般的指令高一些。
			for(;;)
			{
				tagged_index old_tail = tail.load();
				node* tail_node = reinterpret_cast<node*>(pool.get_pointer(old_tail));
				memeroy_head_info h = tail_node->node_head.load();

				tagged_index old_tail2 = tail.load();  // 保证h此时取得是正确的，即其他线程没有修改队尾。只有h是正确的，后边的操作才有意义（其他线程可以通过push操作修改队尾。也可以通过pop操作修改队尾的头信息(挂到自由列表)。这两种情况队尾都会变)
				if(old_tail2 == old_tail)
				{
					if(h.get_index() == pool.null_handle()) // 保证取h的时候是从尾节点头取得（其他线程还没有修改尾节点，我们就可以暂时保证逻辑正确的）
					{
						memeroy_head_info new_head_info(node_handle, h.get_tag());
						if(tail_node->node_head.compare_exchange(h, new_head_info)) // 如果tail_node的头节点依然没变（没被其他线程修改），那么我们线程原子的比较是否改变并把新节点挂到队尾（修改tail_node）
						{
							tagged_index new_tail(node_handle, node_tag);
							tail.compare_exchange(old_tail, new_tail); // 更新最新的队尾。假如我们线程把队尾的头信息改了，那么其他线程因为本次操作不完整取到错误队尾，push的上边的cas不会成功。而其他线程的pop会根据本次操作是否完成进行容错。
							return true;
						}
					}
					else // 取h的时候，队尾已经被其他线程挂上了一个节点，但是其他线程还没有更新最新的队尾。那么我们尝试帮其他线程更新一下队尾。
					{
						tagged_index new_tail(h.get_index(), pool.get_pointer(h.get_index())->node_head.load().get_tag());
						tail.compare_exchange(old_tail, new_tail);
					}
				}
			}
		}

		// 弹出一个元素，要求类型T必须实现=操作符。
		bool pop(T& ret)
		{
			for(;;)
			{
				tagged_index old_head = head.load();
				node* old_head_node = pool.get_pointer(old_head);
				tagged_index old_tail = tail.load();

				index_t next = old_head_node->node_head.load().get_index();
				node* next_ptr = pool.get_pointer(next);

				tagged_index old_head2 = head.load();

				if(old_head2 == old_head)
				{
					if(pool.get_handle(old_head) == pool.get_handle(old_tail))
					{
						if(next_ptr == NULL) // 队列空了，返回失败
						{
							return false;
						}
					
						tagged_index new_tail(next, next_ptr->node_head.load().get_tag()); 
						tail.compare_exchange(old_tail, new_tail);
					}
					else
					{
						if(next_ptr == NULL)
						{
							continue;
						}

						ret = next_ptr->data;  // = 操作

						tagged_index new_head(next, next_ptr->node_head.load().get_tag());
						if(head.compare_exchange(old_head, new_head))  // cas操作
						{
							pool.destruct(pool.get_pointer(old_head.get_index())); // 返还内存给freelist
							return true;
						}
					}
				}
			}
		}

		bool empty()
		{
			return pool.get_handle(head.load()) == pool.get_handle(tail.load());
		}

	private:
		atomic<tagged_index> head;
		atomic<tagged_index> tail;
		pool_t pool;
	};
};