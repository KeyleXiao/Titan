#pragma once

#include "atomic.h"
#include "cmath"
#include "if.h"
#include "tagged_index.h"

namespace speed{
	typedef tagged_index memeroy_head_info; // 当作为memhead的时候，tag为自己的tag index表示next
	
	// freelist固定大小数组实现
	template<typename T>
	struct fixed_size_freelist_storage
	{
		fixed_size_freelist_storage(std::size_t count)
		{
			if((double)count >= pow((double)2, (int)(sizeof(index_t) * 8)))
			{
				throw std::runtime_error("freelist_storage count flow");
				return;
			}

			nodes_ = (T*)malloc(sizeof(T) * count);

			if(nodes_ == NULL)
			{
				throw std::runtime_error("freelist_storage malloc failed");
				return;
			}
			node_count_ = count;
		}
		
		~fixed_size_freelist_storage()
		{
			if(nodes_ != NULL)
			{
				free(nodes_);
				nodes_ = NULL;
				node_count_ = 0;
			}
		}

		T * nodes(void) const
		{
			return reinterpret_cast<T*>(nodes_);
		}

		std::size_t node_count(void) const
		{
			return node_count_;
		}

		std::size_t node_count_;
		T* nodes_;
	};
	
	// 固定大小内存池
	template<typename T>
	class fixed_size_free_list : fixed_size_freelist_storage<T> 
	{
	public:
		void initialize(void)
		{
			T* array_nodes = nodes();
			for(int i = 0; i < node_count_; ++i)
			{
				atomic<memeroy_head_info>* h = reinterpret_cast<atomic<memeroy_head_info>*>(&(array_nodes[i]));
				h->store(memeroy_head_info(null_handle(), 0));

				destruct(array_nodes + i);
			}
		}

		fixed_size_free_list(index_t count) : fixed_size_freelist_storage(count), pool(tagged_index(count, 0))
		{
			initialize();
		}

		index_t null_handle(void) const
		{
			return static_cast<index_t>(node_count());
		}

		index_t get_handle(T* t) const
		{
			return t - nodes();
		}

		index_t get_handle(const tagged_index& t) const
		{
			return t.get_index(); 
		}

		T* get_pointer(const tagged_index& t) const
		{
			if(t.get_index() == null_handle())
			{
				return NULL;
			}

			return nodes() + t.get_index();
		}

		T* get_pointer(const index_t& i) const
		{
			if(i >= null_handle())
			{
				return NULL;
			}

			return nodes() + i;
		}
		
		// 从自由列表构造一个节点。每次从freelist拿出节点 tag + 1 
		// cas + 忙等 实现线程安全
		template<typename ArgType>
		T* construct(const ArgType& arg)
		{
			index_t find_index;

			for(;;)
			{
				tagged_index old_pool = pool.load();
				find_index = old_pool.get_index();

				if(find_index == null_handle())
				{
					break;
				}

				atomic<memeroy_head_info>* head_info = reinterpret_cast<atomic<memeroy_head_info>*>(nodes_ + find_index);
				index_t next = head_info->load().get_index();
				
				int tag = 0;
				if(next != null_handle()) 
				{
					atomic<memeroy_head_info>* next_head_info = reinterpret_cast<atomic<memeroy_head_info>*>(nodes_ + next);
					tag = next_head_info->load().get_tag(); 
				}

				tagged_index new_pool(next, tag);

				if(pool.compare_exchange(old_pool, new_pool))
				{
					break;
				}
			}

			if(find_index == null_handle())
			{
				return NULL;
			}

			T* node = nodes() + find_index;
			atomic<memeroy_head_info>* p = reinterpret_cast<atomic<memeroy_head_info>*>(node);
			new(node) T(arg, p->load().get_tag() + 1, null_handle());

			return node;
		}
		
		// 返还节点到freelist 
		// 线程安全。cas + 忙等
		void destruct(T* node)
		{
			if(node == NULL)
			{
				return;
			}

			int index = node - nodes_;

			atomic<memeroy_head_info>* head_info = reinterpret_cast<atomic<memeroy_head_info>*>(node);
			memeroy_head_info h = head_info->load();
			index_t new_tag = h.get_tag() + 1;

			node->~T();

			for(;;)
			{
				tagged_index old_pool = pool.load();
				tagged_index new_pool(index, new_tag);
				head_info->store(memeroy_head_info(old_pool.get_index(), new_tag));
				
				if(pool.compare_exchange(old_pool, new_pool))
				{
					return;
				}
			}
		}

	private:
		atomic<tagged_index> pool;
	};
	
	// 可动态增长的内存池
	template<typename T>
	class dynamic_size_free_list
	{
	public:
		void initialize(int count)
		{
			for(int i = 0; i < count; ++i)
			{
				T* n = static_cast<T*>(malloc(sizeof(T)));
				if(n == NULL)
				{
					throw std::runtime_error("seed:queue initialize malloc failed!!!");
					return;
				}

				atomic<memeroy_head_info>* h = reinterpret_cast<atomic<memeroy_head_info>*>(n);
				h->store(memeroy_head_info(null_handle(), 0));

				destruct(n);
			}
		}

		dynamic_size_free_list(index_t count) : pool(tagged_index(null_handle(), 0))
		{
			initialize(count);
		}

		~dynamic_size_free_list()
		{
			std::cout << "~dynamic_size_free_list" << std::endl;

			index_t h = pool.load().get_index();
			
			while(h != null_handle())
			{
				T* p = get_pointer(h);
				atomic<memeroy_head_info>* head_info = reinterpret_cast<atomic<memeroy_head_info>*>(p);
				h = head_info->load().get_index();

				free(p);
			}
		}

		index_t null_handle(void) const
		{
			return 0;
		}

		index_t get_handle(T* t) const
		{
			if(t == NULL)
			{
				return null_handle();
			}

			return reinterpret_cast<index_t>(t);
		}

		index_t get_handle(const tagged_index& t) const
		{
			return t.get_index(); 
		}

		T* get_pointer(const tagged_index& t) const
		{
			index_t ret = t.get_index();

			if(ret == null_handle())
			{
				return NULL;
			}

			return reinterpret_cast<T*>(ret);
		}

		T* get_pointer(const index_t& i) const
		{
			if(i == null_handle())
			{
				return NULL;
			}

			return reinterpret_cast<T*>(i);
		}

		template<typename ArgType>
		T* construct(const ArgType& arg)
		{
			index_t find_index;

			for(;;)
			{
				tagged_index old_pool = pool.load();
				find_index = old_pool.get_index();

				if(find_index == null_handle())
				{
					return new T(arg, 0, null_handle());
				}

				atomic<memeroy_head_info>* head_info = reinterpret_cast<atomic<memeroy_head_info>*>(get_pointer(find_index));
				index_t next = head_info->load().get_index();
				
				index_t tag = 0;
				if(next != null_handle())
				{
					atomic<memeroy_head_info>* next_head_info = reinterpret_cast<atomic<memeroy_head_info>*>(get_pointer(next));
					tag = next_head_info->load().get_tag();
				}

				tagged_index new_pool(next, tag);
				if(pool.compare_exchange(old_pool, new_pool))
				{
					T* node = get_pointer(find_index);

					atomic<memeroy_head_info>* p = reinterpret_cast<atomic<memeroy_head_info>*>(node);
					new(node) T(arg, p->load().get_tag() + 1, null_handle());

					return node;
				}
			}
		}

		void destruct(T* node)
		{	
			atomic<memeroy_head_info>* head_info = reinterpret_cast<atomic<memeroy_head_info>*>(node);
			memeroy_head_info h = head_info->load();
			index_t new_tag = h.get_tag() + 1;

			node->~T();

			for(;;)
			{
				tagged_index old_pool = pool.load();
				tagged_index new_pool(get_handle(node), new_tag);
				head_info->store(memeroy_head_info(old_pool.get_index(), new_tag));

				if(pool.compare_exchange(old_pool, new_pool))
				{
					return;
				}
			}
		}

	private:
		atomic<tagged_index> pool;
	};

	template<typename T, bool fixedsize>
	struct select_free_list
	{
		typedef typename aux::if_c<fixedsize, fixed_size_free_list<T>, dynamic_size_free_list<T>>::type type;
	};

}

