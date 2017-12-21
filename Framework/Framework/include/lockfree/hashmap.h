#pragma once

#include <vector>
#include "atomic.h"
#include "hazard.h"
#include "tagged_index.h"

#define SEGMENT_SIZE (1024)
#define SEGMENT_COUNT (2048)

namespace speed
{
	typedef unsigned int uint;
	typedef uint hash_t;

	template<typename T>
	inline hash_t hash_key(T& key)
	{
		if(sizeof(T) < 4)
		{
			uint k = 0;
			memcpy(&k, &key, sizeof(T));
			return k * 2654435761u; 
		}

		return *((uint*)&key) * 2654435761u; // 最怕类型T的对象前4字节都一样(hash算法不太科学，最好类型T自己提供hash算法)
	}

	// 迭代器
	template<typename Container>
	class hash_map_iterator
	{
	public:
		typedef typename Container::node_t node_t;

		struct item
		{
			typename Container::KEY_TYPE first;
			typename Container::VALUE_TYPE second; 
		};

		hash_map_iterator() : container(NULL), cur(NULL), hpcur(NULL)
		{

		}

		hash_map_iterator(Container* c_, node_t* cur_, hprecord* hp_) : container(c_), cur(cur_), hpcur(hp_){}

		hash_map_iterator(const hash_map_iterator<Container>& rhs)
		{
			operator=(rhs);
		}

		hash_map_iterator& operator=(const hash_map_iterator<Container>& rhs)
		{
			hazard::getinstance()->releasehp(hpcur);

			container = rhs.container;
			cur = rhs.cur;
			hpcur = rhs.hpcur;

			return *this;
		}

		// 匹配 i++
		hash_map_iterator operator++(int)
		{
			hash_map_iterator<Container> ret = *this;
			*this = container->next(*this);
			return ret;
		}

		// 匹配 ++i
		hash_map_iterator& operator++()
		{
			if(container == NULL || cur == NULL)
			{
				return *this;
			}

			*this = container->next(*this);

			return *this;
		}

		bool operator==(const hash_map_iterator<Container>& rhs)
		{
			return container == rhs.container && cur == rhs.cur; 
		}

		bool operator!=(const hash_map_iterator<Container>& rhs)
		{
			return container != rhs.container || cur != rhs.cur; 
		}

		item& operator*()
		{
			return *reinterpret_cast<item*>(&(cur->key));
		}

		item* operator->()
		{
			return &operator*();
		}

		Container* container;
		node_t* cur;
		hprecord* hpcur;
	};

	// 加自旋锁(锁bucket)链表的实现
	template<typename key_t, typename value_t>
	class split_ordered_list_lock_impl
	{
	public:
		typedef void (*IterCallBack)(key_t, value_t, void*);

		class spin_lock
		{
		public:
			enum
			{
				lock_state_unlock,
				lock_state_lock,
			};

			spin_lock() : lock_state(lock_state_unlock){};

			void lock()
			{
				int expect = lock_state_unlock;
				int desire = lock_state_lock;

				while(!lock_state.compare_exchange(expect, desire));
			}

			void unlock()
			{
				lock_state.store(lock_state_unlock);
			}

		private:
			atomic<int> lock_state;
		};

		struct node_t
		{
			struct node_t* next;
			hash_t hash_code;
			key_t key;
			value_t value;
			spin_lock slock; // isdummy--true slock才有用

			bool isdummy()
			{
				return (hash_code & 0x1) == 0 ? true : false; 
			}
		};

		node_t* makenode(key_t key, value_t value, hash_t hash_code)
		{
			node_t* n = (node_t*)calloc(1, sizeof(node_t));
			if(n == NULL)
			{
				return NULL;
			}

			n->next = NULL;
			n->hash_code = hash_code;
			n->key = key;
			n->value = value;
			n->slock = spin_lock();

			return n;
		}

		bool list_insert(node_t* l, node_t* node, value_t& orivalue)
		{
			if(l == NULL || !l->isdummy())
			{
				return false;
			}

			node_t* bucket = l;

			while(true)
			{
				bucket->slock.lock();
				node_t* prev = bucket;

				do
				{
					if(prev->next == NULL)
					{
						node->next = NULL;
						prev->next = node;
						bucket->slock.unlock();
						return true;
					}

					if(prev->next->hash_code == node->hash_code)
					{
						if(prev->next->key == node->key)
						{
							orivalue = prev->next->value;
							bucket->slock.unlock();
							return false;
						}
						prev = prev->next;
					}
					else if(prev->next->hash_code > node->hash_code)
					{
						node->next = prev->next;
						prev->next = node;
						bucket->slock.unlock();
						return true;
					}
					else
					{
						prev = prev->next;
					}

					if(prev->isdummy())
					{
						bucket->slock.unlock();
						bucket = prev;
						break;
					}
				}while(true);
			}
		}
		
		bool list_delete(node_t* l, hash_t hash_code, key_t key)
		{
			if(l == NULL || !l->isdummy())
			{
				return false;
			}

			node_t* bucket = l;
			while(true)
			{
				bucket->slock.lock();
				node_t* prev = bucket;

				do 
				{
					if(prev->next == NULL)
					{
						bucket->slock.unlock();
						return false;
					}

					if(prev->next->hash_code == hash_code)
					{
						if(prev->next->key == key)
						{
							if(!prev->next->isdummy())
							{
								node_t* n = prev->next;
								prev->next = n->next;
								free(n);
								bucket->slock.unlock();
								return true;
							}
							else // dummy节点不允许删除
							{
								bucket->slock.unlock();
								return false;
							}							
						}
						prev = prev->next;
					}
					else if(prev->next->hash_code > hash_code)
					{
						bucket->slock.unlock();
						return false;
					}
					else
					{
						prev = prev->next;
					}
					
					if(prev->isdummy())
					{
						bucket->slock.unlock();
						bucket = prev;
						break;
					}
				}while(true);
			}
		}

		bool list_find(node_t* l, hash_t hash_code, key_t key, value_t& retvalue)
		{
			if(l == NULL || !l->isdummy())
			{
				return false;
			}

			node_t* bucket = l;
			while(true)
			{
				bucket->slock.lock();
				node_t* prev = bucket;

				do 
				{
					if(prev->next == NULL)
					{
						bucket->slock.unlock();
						return false;
					}

					if(prev->next->hash_code == hash_code)
					{
						if(prev->next->key == key)
						{
							retvalue = prev->next->value;
							bucket->slock.unlock();
							return true;
						}
						prev = prev->next;
					}
					else if(prev->next->hash_code > hash_code)
					{
						bucket->slock.unlock();
						return false;
					}
					else
					{
						prev = prev->next;
					}

					if(prev->isdummy())
					{
						bucket->slock.unlock();
						bucket = prev;
						break;
					}
				} while(true);
			}
		}

		node_t* find_next(node_t* l, hash_t code, key_t key, hprecord*& hp)
		{
			if(l == NULL || !l->isdummy())
			{
				return NULL;
			}

			bool nextdone = false;

			node_t* bucket = l;
			while(true)
			{
				bucket->slock.lock();
				node_t* cur = bucket->next;
				node_t* ret = NULL;

				while(cur != NULL)
				{
					if(cur->isdummy())
					{
						bucket->slock.unlock();
						bucket = cur;
						break;
					}

					if(nextdone)
					{
						ret = cur;
						break;
					}

					if(cur->hash_code > code)
					{		
						ret = cur;
						break;
					}
					else if(cur->hash_code == code)
					{
						if(cur->key == key)
						{
							nextdone = true; // 下一个非dummy的node就是要找的。如果迭代器指向的这个节点已被删除，会导致和这个节点hashcode相同的一票节点被略过
						}

						cur = cur->next;
					}
					else
					{
						cur = cur->next;
					}
				}

				if(ret != NULL)
				{
					hp = hazard::getinstance()->acquirehp();
					if(hp == NULL)
					{
						bucket->slock.unlock();
						return NULL;
					}
					hp->phazard = ret;

					bucket->slock.unlock();
					return ret;
				}
				
				if(cur == NULL)
				{
					bucket->slock.unlock();
					hp = NULL;
					return NULL;
				}	
			}
		}
		
		// lock版本的foreach操作比迭代器操作性能应该好些。每个bucket只加一次锁。
		// 而迭代器遍历到的每个元素会声明为风险指针这个操作会有一次cas
		void foreach(node_t* head, IterCallBack callback, void* arg)
		{
			if(head == NULL || !head->isdummy())
			{
				return;
			}

			node_t* bucket = head;
			while(true)
			{
				bucket->slock.lock();

				node_t* cur = bucket->next;
				while(cur != NULL)
				{
					if(cur->isdummy())
					{
						bucket->slock.unlock();
						bucket = cur;
						break;
					}

					(*callback)(cur->key, cur->value, arg);
					cur = cur->next;
				}

				if(cur == NULL)
				{
					bucket->slock.unlock();
					return;
				}
			}
		}

		int clearlist(node_t* head)
		{
			if(head == NULL || !head->isdummy())
			{
				return 0;
			}
			
			int count = 0;

			node_t* bucket = head;

			while(true)
			{
				bucket->slock.lock();
				while(true)
				{
					node_t* bnext = bucket->next;

					if(bnext == NULL)
					{
						bucket->slock.unlock();
						return count;
					}

					if(bnext->isdummy())
					{
						node_t* bucketbak = bucket;
						bucket = bnext;
						bucketbak->slock.unlock();
					}
					else
					{
						bucket->next = bnext->next;
						free(bnext);
						++count;
					}
				}
			}
		}
	};
	
	// 无锁链表实现(效率比锁bucket低了4倍左右，因为每次操作列表元素，需要风险指针，而风险指针有cas操作，降低了性能)
	template<typename key_t, typename value_t>
	class split_ordered_list_lock_free_impl
	{
	public:

		typedef void (*IterCallBack)(key_t, value_t, void*);

		struct node_t
		{
			atomic<tagged_index> next;
			hash_t hash_code;
			key_t key;
			value_t value;

			bool isdummy()
			{
				return (hash_code & 0x1) == 0 ? true : false; 
			}
		};

		node_t* makenode(key_t key, value_t value, hash_t hash_code)
		{
			node_t* n = (node_t*)calloc(1, sizeof(node_t));
			if(n == NULL)
			{
				return n;
			}

			n->hash_code = hash_code;
			n->key = key;
			n->value = value;
			n->next.store(tagged_index(0, 0));

			return n;
		}

		// 失败ori返回原来的value
		bool list_insert(node_t* l, node_t* n, value_t& value)
		{
			node_t* right_node = NULL;
			node_t* left_node = NULL;
			do 
			{
				std::vector<hprecord*> vechps;
				right_node = search(l, n->hash_code, n->key, &left_node, vechps);
				if(right_node != NULL && right_node->key == n->key && right_node->hash_code == n->hash_code)
				{
					value = right_node->value;

					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}

					return false;
				}

				tagged_index old_left_next(reinterpret_cast<index_t>(right_node), 0);
				n->next.store(old_left_next);
				tagged_index new_left_next(reinterpret_cast<index_t>(n), 0);

				if(left_node->next.compare_exchange(old_left_next, new_left_next))
				{
					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}

					return true;
				}
				else
				{
					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}
				}
			}while (true);
		}

		bool list_delete(node_t* l, hash_t code, key_t key)
		{
			node_t* right_node = NULL;
			node_t* left_node = NULL;
			tagged_index right_node_next;
			std::vector<hprecord*> vechps;

			do
			{
				right_node = search(l, code, key, &left_node, vechps);
				if(right_node == NULL || right_node->key != key)
				{
					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}
					return false;
				}

				right_node_next = right_node->next.load();
				if(right_node_next.get_tag() == 0)
				{
					// 逻辑删除
					tagged_index new_next(right_node_next.get_index(), 1);
					if(right_node->next.compare_exchange(right_node_next, new_next))
					{
						break;
					}
				}
			}while(true);

			// 物理删除
			tagged_index right(reinterpret_cast<index_t>(right_node), 0);
			if(!left_node->next.compare_exchange(right, right_node_next))
			{
				// 物理删除失败(通过search物理删除)
				right_node = search(l, right_node->hash_code, right_node->key, &left_node, vechps);
			}
			else
			{
				hazard::getinstance()->retire(right_node);
			}

			for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
			{
				hazard::getinstance()->releasehp(*iter);
			}

			return true;
		}

		bool list_find(node_t* l, hash_t code, key_t key, value_t& value)
		{
			node_t* right_node = NULL;
			node_t* left_node = NULL;

			std::vector<hprecord*> vechps;
			right_node = search(l, code, key, &left_node, vechps);

			if(right_node == NULL || right_node->key != key)
			{
				for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
				{
					hazard::getinstance()->releasehp(*iter);
				}
				return false;
			}

			value = right_node->value;

			for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
			{
				hazard::getinstance()->releasehp(*iter);
			}

			return true;
		}

		node_t* find_next(node_t* l, hash_t code, key_t key, hprecord*& outhp)
		{
			if(l == NULL || !l->isdummy())
			{
				outhp = NULL;
				return NULL;
			}

			hash_t cur_code = code;
			key_t cur_key = key;

			while(true)
			{
				node_t* left_node = NULL;
				std::vector<hprecord*> vechps;
				node_t* right_node = search(l, cur_code, cur_key, &left_node, vechps);
				if(right_node == NULL)
				{
					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}
					outhp = NULL;
					return NULL;
				}

				if(right_node->isdummy() || right_node->key == key)
				{
					node_t* next_node = NULL;
					hprecord* hpnext = NULL;
					int errorcode = guard_successor(right_node, next_node, hpnext);
					if(errorcode != GUARD_SUCCESSOR_OK)
					{
						for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
						{
							hazard::getinstance()->releasehp(*iter);
						}
						continue;
					}

					if(next_node == NULL)
					{
						for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
						{
							hazard::getinstance()->releasehp(*iter);
						}
						outhp = NULL;
						return NULL;
					}
					
					if(right_node->isdummy())
					{
						l = right_node;
					}

					cur_key = next_node->key;
					cur_code = next_node->hash_code;
					hazard::getinstance()->releasehp(hpnext);
					
					for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
					{
						hazard::getinstance()->releasehp(*iter);
					}
					continue;
				}

				for(std::vector<hprecord*>::iterator iter = vechps.begin(); iter != vechps.end(); ++iter)
				{
					hprecord* hp_ = *iter;
					if(hp_ != NULL)
					{
						if(hp_->phazard != right_node)
						{
							hazard::getinstance()->releasehp(hp_);
						}
						else
						{
							outhp = hp_;
						}
					}
				}

				return right_node;
			}
		}

		int clearlist(node_t* head)
		{
			return -1;
		}

		void foreach(node_t* head, IterCallBack callback, void* arg)
		{
			if(head == NULL || !head->isdummy())
			{
				return;
			}

			node_t* bucket = head;
			node_t* cur = head;
			hprecord* hpcur = NULL;

			while(true)
			{
				node_t* next = NULL;
				hprecord* hpnext = NULL;
				if(guard_successor(cur, next, hpnext) != GUARD_SUCCESSOR_OK)
				{
					node_t* left = NULL;
					std::vector<hprecord*> vechps;
					next = search(bucket, cur->hash_code, cur->key, &left, vechps);

					for(std::vector<hprecord*>::reverse_iterator iter = vechps.rbegin(); iter != vechps.rend(); ++iter)
					{
						hprecord* hp_ = *iter;
						if(hp_ != NULL && hp_->phazard == next)
						{
							hpnext = hp_;
						}
						else
						{
							hazard::getinstance()->releasehp(hp_);
						}
					}
				}

				hazard::getinstance()->releasehp(hpcur);

				if(next == NULL)
				{
					return;
				}

				if(next->isdummy())
				{
					bucket = next;
				}
				else
				{
					(*callback)(next->key, next->value, arg);
				}

				cur = next;
				hpcur = hpnext;
			}
		}

private:
		// 输入 head--in 链表头 code--in hash_code  left_node--out  
		// 返回 right_node
		// 实现要求
		// 1、left_node的hashcode小于code, right_node的hash_node不小于code
		// 2、这两个节点都没被逻辑删除。 
		// 3、right_node必须是left_node的直接后继，即要求这个操作物理删除left_node
		//		和right_node之间的逻辑删除节点，使他们相接
		// 注意：应该把left_node和right_node标记为风险指针。 当调用者使用完后释放风险指针
		node_t* search(node_t* l, hash_t code, key_t key, node_t** left_node, std::vector<hprecord*>& refvhp)
		{
search_again:
			hprecord* hpprev = NULL;
			node_t* prev = l;
			node_t* cur = NULL;
			hprecord* hpcur = NULL;

			int errorcode = guard_successor(prev, cur, hpcur);
			if(errorcode != GUARD_SUCCESSOR_OK)
			{
				// 如果没成功是因为申请风险指针失败，或者当前节点(prev)被逻辑删除。
				std::cout << "search error!!!: head(dummy) node is removed or acquirehp failed.";
				*left_node = NULL;
				return NULL;
			}

			while(true)
			{
				if(cur == NULL)
				{
					goto done;
				}

				node_t* next_node = NULL;
				hprecord* hp = NULL;

				errorcode = guard_successor(cur, next_node, hp); 

				// 如果当前节点已经被逻辑删除，尝试物理删除当前节点
				if(errorcode == GUARD_SUCCESSOR_CUR_NODE_LOGIC_REMOVED) 
				{
					tagged_index old_prev_next(reinterpret_cast<index_t>(cur), 0);
					tagged_index new_prev_next(cur->next.load().get_index(), 0);
					if(prev->next.compare_exchange(old_prev_next, new_prev_next))
					{
						hazard::getinstance()->releasehp(hpcur);
						hazard::getinstance()->retire(cur);

						if(guard_successor(prev, cur, hpcur) != GUARD_SUCCESSOR_OK)
						{
							hazard::getinstance()->releasehp(hpprev);
							goto search_again;
						}
					}
					else
					{
						hazard::getinstance()->releasehp(hpprev);
						hazard::getinstance()->releasehp(hpcur);
						goto search_again;
					}
				}
				else if(errorcode == GUARD_SUCCESSOR_OK)
				{
					hash_t cur_hash = cur->hash_code;
					key_t cur_key = cur->key;
					if(cur_hash > code || (cur_hash == code && cur_key == key))
					{
						hazard::getinstance()->releasehp(hp);
						goto done;
					}

					prev = cur;
					hazard::getinstance()->releasehp(hpprev);
					hpprev = hpcur;

					hpcur = hp;
					cur = next_node;
				}
				else
				{
					hazard::getinstance()->releasehp(hpprev);
					hazard::getinstance()->releasehp(hpcur);
					*left_node = NULL;
					return NULL;
				}
			}
done:
			refvhp.push_back(hpprev);
			refvhp.push_back(hpcur);
			*left_node = prev;
			return cur;
		}

		// 这个函数的前提是当前节点已经被保护
		// 返回: 0--成功 1--当前节点被删除 2--没有后继节点 3--申请风险指针失败
		enum
		{
			GUARD_SUCCESSOR_OK,
			GUARD_SUCCESSOR_CUR_NODE_LOGIC_REMOVED,
			GUARD_SUCCESSOR_ACQUIRE_HAZARD_POINTER,
		};
		int guard_successor(node_t* cur, node_t*& out_next_node, hprecord*& outhp)
		{
			while(1)
			{
				tagged_index old_next = cur->next.load();
				if(old_next.get_tag() != 0) // 当前节点都被逻辑删除了，next是无效的
				{
					out_next_node = NULL;
					outhp = NULL;
					return GUARD_SUCCESSOR_CUR_NODE_LOGIC_REMOVED;
				}

				out_next_node = reinterpret_cast<node_t*>(old_next.get_index());
				if(out_next_node == NULL)
				{
					outhp = NULL;
					return GUARD_SUCCESSOR_OK;
				}

				outhp = hazard::getinstance()->acquirehp();
				if(outhp == NULL)
				{
					return GUARD_SUCCESSOR_ACQUIRE_HAZARD_POINTER;
				}

				outhp->phazard = out_next_node;

				tagged_index new_next = cur->next.load();
				if(old_next == new_next)
				{
					return GUARD_SUCCESSOR_OK;
				}
				else // 当前节点被删除了或者当前节点的next已经变了， 重来一次
				{
					hazard::getinstance()->releasehp(outhp);
				}
			}
		}
	};
	
	// 选择是否加锁版本的list
	template<typename key_t, typename value_t, bool lock_ = false>
	struct select_split_ordered_list_type
	{
		typedef typename speed::aux::if_c<lock_, split_ordered_list_lock_impl<key_t, value_t>, split_ordered_list_lock_free_impl<key_t, value_t>>::type type;
	};
	
	// hashmap实现
	// 说明: 基于两种列表实现。两种列表都是split_ordered_list。区别是有无加锁。默认是加锁实现
	//   lock_模板参数 true: 表示锁bucket链表实现。 
	//   lock_模板参数 false: 无锁链表实现。
	// 锁bucket链表的实现经过简单测试速度要比无锁链表实现快4倍左右。
	template<typename key_t, typename value_t, bool lock_ = true>
	class hashmap : public select_split_ordered_list_type<key_t, value_t, lock_>::type
	{
	public:
		friend class hash_map_iterator<hashmap<key_t, value_t>>;
		typedef key_t KEY_TYPE;
		typedef value_t VALUE_TYPE;
		typedef hash_map_iterator<hashmap<key_t, value_t, lock_>> iterator;		

		typedef typename select_split_ordered_list_type<key_t, value_t, lock_>::type::node_t node_t;  

		static const uint RE_HASH_FACTOR = 30;  // 当count / size > 30时 size增长为原来两倍

		struct segment_t
		{
			atomic<node_t*> buckets[SEGMENT_SIZE];

			segment_t()
			{
				memset(this, 0, SEGMENT_SIZE * sizeof(atomic<node_t*>));
			}
		};

		hashmap() : bucketnum(2), count(0)
		{
			memset(segements, 0, sizeof(atomic<segment_t*>) * SEGMENT_COUNT);
		}
		
		iterator begin()
		{
			node_t* head = getbucket(0);
			if(head == NULL)
			{
				return end();
			}

			hprecord* hp = NULL;
			node_t* b_ = find_next(head, head->hash_code, head->key, hp);
			if(b_ == NULL)
			{
				return end();
			}

			return iterator(this, b_, hp);
		}

		iterator end()
		{
			return iterator(this, NULL, NULL);
		}

		iterator next(iterator& iter)
		{
			node_t* cur = iter.cur;
			if(cur == NULL)
			{
				return end();
			}

			hash_t hash =  hash_key(cur->key);
			uint bucket = hash % bucketnum.load();
			node_t* bl = getbucket(bucket);
			if(bl == NULL)
			{
				return end();
			}

			hprecord* hp = NULL;
			node_t* n_ = find_next(bl, cur->hash_code, cur->key, hp);
			if(n_ == NULL)
			{
				return end();
			}

			return iterator(this, n_, hp);
		}

		bool insert(const key_t& key, const value_t& value)
		{
			hash_t hash = hash_key(key);
			node_t* n_ = makenode(key, value, hash_code(hash));
			if(n_ == NULL)
			{
				throw std::runtime_error("memory alloc failed");
				return false;
			}

			uint bucket = hash % bucketnum.load();
			node_t* bl = getbucket(bucket);
			if(bl == NULL)
			{
				initializebucket(bucket);
				bl = getbucket(bucket);
			}

			if(bl == NULL)
			{
				std::cout << "error : bucket " << bucket << " no init" << std::endl; 
				return false;
			}

			value_t v;
			if(list_insert(bl, n_, v))
			{
				addcount(1);
				rehash();
				return true;
			}
			else
			{
				free(n_);
				return false;
			}
		}

		// 函数是线程安全的 但是返回值不能作为左值(避免线程不安全)
		value_t operator[](const key_t& key)
		{
			hash_t hash = hash_key(key);

			value_t v;
			node_t* n_ = makenode(key, v, hash_code(hash));
			if(n_ == NULL)
			{
				throw std::runtime_error("memory alloc failed");
				return v;
			}

			uint bucket = hash % bucketnum.load();

			node_t* bl = getbucket(bucket);
			if(bl == NULL)
			{
				initializebucket(bucket);
				bl = getbucket(bucket);
			}

			if(list_insert(bl, n_, v))
			{
				addcount(1);
				rehash();
				return n_->value;
			}
			else
			{
				free(n_);
				return v;
			}
		}

		bool erase(const key_t& key)
		{
			hash_t hash =  hash_key(key);
			uint bucket = hash % bucketnum.load();

			node_t* bl = getbucket(bucket);
			if(bl == NULL)
			{
				return false;
			}

			if(list_delete(bl, hash_code(hash), key))
			{
				addcount(-1);
				return true;
			}

			return false;
		}

		bool find(const key_t& key, value_t& value)
		{
			hash_t hash =  hash_key(key);
			uint bucket = hash % bucketnum.load();

			node_t* bl = getbucket(bucket);
			if(bl == NULL)
			{
				return false;
			}

			return list_find(bl, hash_code(hash), key, value);
		}

		static inline void collect_element_callback(key_t key, value_t value, void* arg)
		{
			std::vector<key_t>* pvec = (std::vector<key_t>*)arg;
			if(pvec == NULL)
			{
				return;
			}
			pvec->push_back(key);
		}

		bool clear()
		{
			int c = clearlist(getbucket(0));

			if(c >= 0)
			{
				addcount(-c);
				return true;
			}
			
			// lockfree版本本函数处理clear
			std::vector<key_t> veckeys;

			foreach(getbucket(0), &collect_element_callback, &veckeys);

			for(std::vector<key_t>::iterator iter = veckeys.begin(); iter != veckeys.end(); ++iter)
			{
				erase(*iter);
			}

			return true;
		}

		uint size()
		{
			return count.load();
		}

	private:
		void initializebucket(uint bucket)
		{
			node_t* dummynode = NULL;

			if(bucket == 0)
			{
				segment_t* segment = segements[0].load();
				if(segment == NULL)
				{
					segment_t* newsegment = new segment_t;
					if(!segements[0].compare_exchange(segment, newsegment))
					{
						delete newsegment; 
					}
				}

				segment = segements[0].load();

				node_t* h = segment->buckets[0].load();
				if(h == NULL)
				{
					dummynode = makenode(key_t(), value_t(), hash_code(bucket, true));
					if(dummynode == NULL)
					{
						return;
					}

					if(!segment->buckets[0].compare_exchange(h, dummynode))
					{
						free(dummynode);
					}
				}

				return;
			}

			uint parent_bucket = get_bucket_parent(bucket);
			node_t* parent_bl = getbucket(parent_bucket);
			if(parent_bl == NULL)
			{
				initializebucket(parent_bucket);
				parent_bl = getbucket(parent_bucket);
			}

			dummynode = makenode(key_t(), value_t(), hash_code(bucket, true));
			value_t v;
			if(list_insert(parent_bl, dummynode, v))
			{
				uint s = bucket / SEGMENT_SIZE;
				uint b = bucket % SEGMENT_SIZE;

				segment_t* segment = segements[s].load();
				if(segment == NULL)
				{
					segment_t* newsegment = new segment_t;
					if(!segements[s].compare_exchange(segment, newsegment))
					{
						delete newsegment; 
					}
				}

				segment = segements[s].load();

				node_t* h = segment->buckets[b].load();
				if(h == NULL)
				{
					segment->buckets[b].compare_exchange(h, dummynode);
				}
			}
			else
			{
				free(dummynode);
			}
		}
		
		node_t* getbucket(uint bucket)
		{
			uint s = bucket / SEGMENT_SIZE;
			uint b = bucket % SEGMENT_SIZE;

			segment_t* segment = segements[s].load();
			if(segment == NULL)
			{
				return NULL;
			}

			return segment->buckets[b].load();
		}

		void rehash()
		{
			uint cur_count = count.load();
			uint cur_size = bucketnum.load();

			if(cur_count / cur_size < RE_HASH_FACTOR)
			{
				return;
			}
			
			uint new_size = cur_size * 2;
			
			if(new_size > SEGMENT_COUNT * SEGMENT_SIZE)
			{
				std::cout << "hashmap capacity limit, can't grow" << std::endl;
				return;
			}

			for(int b = cur_size; b < new_size; ++b)
			{
				initializebucket(b);
			}

			bucketnum.compare_exchange(cur_size, new_size);
		}

		hash_t hash_code(hash_t k, bool dummy = false)
		{
			if(dummy)
			{
				k = k & ~0x80000000;
			}
			else
			{
				k = k | 0x80000000;
			}

			hash_t r = 0;
			for (int i = 0; i < 32; ++i) {
				hash_t bit = (k & (1 << i)) >> i;
				r |= bit << (31 - i);
			}
			return r;
		}

		uint get_bucket_parent(uint bucket)
		{
			int i;
			for(i = 31; i >= 0; --i) 
			{
				if(bucket & (1 << i))
					return bucket & ~(1 << i);
			}
			return 0;
		}

		void addcount(int n)
		{
			while(true)
			{
				uint old_count = count.load();
				uint new_count = old_count + n;
				if(count.compare_exchange(old_count, new_count))
				{
					break;
				}
			}
		}

	protected:
		atomic<uint> count;  // 元素个数
		atomic<uint> bucketnum;	 // bucket个数
		atomic<segment_t*> segements[SEGMENT_COUNT];
	};
}
