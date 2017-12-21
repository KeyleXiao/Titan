//==========================================================================
/**
* @file	  : safelist.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-13   20:21
* purpose : 能安全遍历的list
*/
//==========================================================================

#ifndef __safelist_h__
#define __safelist_h__

#include <list>

namespace rkt {

	/** 安全list
	对标准list进行了简单封装，使之具有安全遍历的功能（即：在遍历过程中，支持增删节点）

	// 普通遍历
	for (safelist<int>::const_iterator it = list.begin(); it!=list.end(); ++it)
	{
		Info("val = "<<*it<<endl);
	}

	// 安全遍历(允许在遍历的过程中增删任何数目的节点)
	for (safelist<int>::iterator it=list.find_first(); it!=list.end(); it=list.find_next(it))
	{
		Info("val = "<<*it<<endl);
		if (*it == 3)
		{
			list.erase(it);
		}
	}
*/
	template<class _Ty, class _Ax = std::allocator<_Ty> >
	class safelist : public std::list<_Ty, _Ax>
	{
	public:
		typedef typename std::list<_Ty, _Ax>	_Mybase;
		typedef typename safelist<_Ty, _Ax>		_Myt;
		typedef typename _Mybase::_Alloc		_Alloc;

	private:
		mutable _Nodeptr	_Cur;	/// the cursor for for_each

	public:
		safelist() : _Mybase(), _Cur(_Myhead) { }
		explicit safelist(const _Alloc& _Al) : _Mybase(_Al), _Cur(_Myhead) { }
		explicit safelist(size_type _Count) : _Mybase(_Count), _Cur(_Myhead) { }
		safelist(size_type _Count, const _Ty& _Val) : _Mybase(_Count, _Val), _Cur(_Myhead) { }
		safelist(size_type _Count, const _Ty& _Val, const _Alloc& _Al) : _Mybase(_Count, _Val, _Al), _Cur(_Myhead) { }
		safelist(const _Mybase& _Right) : _Mybase(_Right), _Cur(_Myhead) { }
		safelist(const _Myt& _Right) : _Mybase(_Right), _Cur(_Myhead) { }
		template<class _Iter>
		safelist(_Iter _First, _Iter _Last) : _Mybase(_First, _Last), _Cur(_Myhead) { }
		template<class _Iter>
		safelist(_Iter _First, _Iter _Last, const _Alloc& _Al) : _Mybase(_First, _Last, _Al), _Cur(_Myhead) { }

		~safelist()
		{
			_Cur = 0;
		}

		void clear()
		{
			_Mybase::clear();
			_Cur = _Myhead;
		}

		iterator erase(iterator _Where)
		{
			_Nodeptr cur = _Where._Mynode();
			if (_Cur == cur)
				_Cur = _Nextnode(cur);
			return _Mybase::erase(_Where);
		}

		// 用于安全遍历
	public:
#if _SECURE_SCL
		iterator find_first()				{ return iterator(_Cur = _Nextnode(_Myhead), this); }
		const_iterator find_first() const	{ return const_iterator(_Cur = _Nextnode(_Myhead), this); }
#else
		iterator find_first()				{ return iterator(_Cur = _Nextnode(_Myhead)); }
		const_iterator find_first() const	{ return const_iterator(_Cur = _Nextnode(_Myhead)); }
#endif

		iterator find_next(iterator cur)
		{
			if (cur._Mynode() == _Cur)
				_Cur = _Nextnode(_Cur);
#if _SECURE_SCL
			return iterator(_Cur, this);
#else
			return iterator(_Cur);
#endif
		}

		const_iterator find_next(const_iterator cur) const
		{
			if (cur._Mynode() == _Cur)
				_Cur = _Nextnode(_Cur);
#if _SECURE_SCL
			return const_iterator(_Cur, this);
#else
			return const_iterator(_Cur);
#endif
		}

		void remove(const _Ty& _Val_arg)
		{
			const _Ty _Val = _Val_arg;
			iterator _Last = end();
			for (iterator _First = begin(); _First != _Last; )
				if (*_First == _Val)
					_First = erase(_First);
				else
					++_First;
		}

		template<class _Pr1>
		void remove_if(_Pr1 _Pred)
		{
			iterator _Last = end();
			for (iterator _First = begin(); _First != _Last; )
				if (_Pred(*_First))
					_First = erase(_First);
				else
					++_First;
		}
	};







// 暂时关闭下面的，直接用上面的即可
#if 0 
	/** simple and safe list => sslist
	为特殊需要而写(去掉了迭代器，直接使用Node)
	采用双向循环链表原理实现
	暂时没有将stl的分配器作为模板参数来用

	// 普通遍历
	for (sslist<int>::const_iterator it = list.begin(); it!=list.end(); it = it->next)
	{
		Info("val = "<<it->val<<endl);
	}

	// 安全遍历(允许在遍历的过程中增删任何数目的节点)
	for (sslist<int>::iterator it=list.find_first(); it!=list.end(); it=list.find_next(it))
	{
		Info("val = "<<it->val<<endl);
		if (it->val == 3)
		{
			list.erase(it->prev);
			list.erase(it->next);
			list.erase(it);
		}
	}
*/
	template<class T>
	class sslist
	{
		struct Node
		{
			Node*	prev;
			Node*	next;
			T		val;
		};

	protected:
		Node*	_Head;	/// inner flag node
		size_t	_Size;	/// node count
		Node*	_Cur;	/// the cursor for for_each

	public:
		typedef Node* iterator;
		typedef const Node* const_iterator;
		sslist() : _Size(0)
		{
			_Head = new Node;
			_Head->next = _Head->prev = _Head;
			_Head->val = T();
			_Cur = _Head;
		}

		~sslist()
		{
			clear();
			delete _Head;
			_Head = 0;
			_Cur = 0;
		}

		void push_front(const T& val)			{ _Insert(_Head->next, val); }
		void push_back(const T& val)			{ _Insert(_Head, val); }

		void pop_front()						{ erase(_Head->next); }
		void pop_back()							{ erase(_Head->prev); }

		Node* insert(Node* cur, const T& val)
		{
			_Insert(cur, val);
			return cur->prev;
		}

		Node* erase(Node* cur)
		{
			Node* next = cur->next;
			if (cur != _Head)
			{
				cur->prev->next = cur->next;
				cur->next->prev = cur->prev;
				if (_Cur == cur)
					_Cur = next;
				delete cur;
				--_Size;
			}
			return next;
		}

		Node* erase(Node* first, Node* last) // [first, last)
		{
			if (first == _Head->next && last == _Head)
			{
				clear();
				return _Head;
			}
			else
			{
				while (first != last)
					first = erase(first);
				return last;
			}
		}

		void remove(const T& val)
		{
			for (Node* cur = _Head->next; cur != _Head; )
			{
				if (cur->val == val)
					cur = erase(cur);
				else
					cur = cur->next;
			}
		}

		void clear()
		{
			Node* next;
			Node* node = _Head->next;
			_Head->next = _Head;
			_Head->prev = _Head;
			_Size = 0;

			for (; node != _Head; node = next)
			{
				next = node->next;
				delete node;
			}

			_Cur = _Head;
		}

		void resize(size_t newsize)
		{
			resize(newsize, T());
		}

		void resize(size_t newsize, T val)
		{
			if (_Size < newsize)
				_Insert_n(_Head, newsize - _Size, val);
			else
				while (newsize < _Size)
					pop_back();
		}

		Node* begin()					{ return _Head->next; }
		const Node* begin() const		{ return _Head->next; }

		Node* end()						{ return _Head; }
		const Node* end() const			{ return _Head; }

		T& front()						{ return _Head->next->val; }
		const T& front() const			{ return _Head->next->val; }

		T& back()						{ return _Head->prev->val; }
		const T& back() const			{ return _Head->prev->val; }

		size_t size() const				{ return _Size; }
		bool empty() const				{ return _Size == 0; }

		Node* find(const T& val) const
		{
			return find(begin(), end(), val);
		}

		Node* find(Node* first, Node* last, const T& val) const // [first, last)
		{
			for (; first != last; first = first->next)
				if (first->val == val)
					break;

			return first;
		}

		// 用于安全遍历(就是遍历过程中，可以增删节点，包括删除当前节点)
		Node* find_first()				{ return _Cur = _Head->next; }
		Node* find_next(Node* cur)
		{
			if (cur == _Cur)
				_Cur = _Cur->next;
			return _Cur;
		}

	protected:
		Node* _Buynode(Node* next, Node* prev, const T& val)
		{
			Node* node = new Node;
			node->next = next;
			node->prev = prev;
			node->val = val;
			return node;
		}

		void _Insert(Node* cur, const T& val)
		{
			Node* newnode = _Buynode(cur, cur->prev, val);
			_Size++;
			cur->prev = newnode;
			newnode->prev->next = newnode;
		}

		void _Insert_n(Node* cur, size_t count, const T& val)
		{
			for (; 0 < count; --count)
				_Insert(cur, val);
		}
	};


	/**
	为特殊需要而写(可以获取Node的地址)
	用法跟std::list类似
	采用双向循环链表原理实现
	*/
	template<class T>
	class mylist
	{
	public:
		struct Node
		{
			Node*	prev;
			Node*	next;
			T		val;
		};

		struct const_iterator
		{
			Node* n;

			const_iterator() : n(0)								{ }
			const_iterator(Node* node) : n(node)				{ }

			const Node* node() const							{ return n; }
			const T& operator *() const							{ return n->val; }
			const_iterator& operator ++()						{ n = n->next; return *this; }
			const_iterator& operator --()						{ n = n->prev; return *this; }
			bool operator ==(const const_iterator& rhs) const	{ return n == rhs.n; }
			bool operator !=(const const_iterator& rhs) const	{ return (!(*this == rhs)); }
			const_iterator operator ++(int)
			{
				const_iterator tmp = *this;
				n = n->next;
				return tmp;
			}
			const_iterator operator --(int)
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}
		};

		struct iterator : public const_iterator
		{
			iterator()									{ }
			iterator(Node* node) : const_iterator(node) { }

			Node* node() const							{ return n; }		
			T& operator *() const						{ return n->val; }
			iterator& operator ++()						{ n = n->next; return *this; }
			iterator& operator --()						{ n = n->prev; return *this; }
			bool operator ==(const iterator& rhs) const	{ return n == rhs.n; }
			bool operator !=(const iterator& rhs) const	{ return (!(*this == rhs)); }
			iterator operator ++(int)
			{
				iterator tmp = *this;
				n = n->next;
				return tmp;
			}
			iterator operator --(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
		};

	protected:
		Node*	_Head;
		size_t	_Size;

	public:
		mylist() : _Size(0)
		{
			_Head = new Node;
			_Head->next = _Head->prev = _Head;
		}

		~mylist()
		{
			clear();
			delete _Head;
			_Head = 0;
		}

		void push_front(const T& val)	{ _Insert(begin(), val); }
		void push_back(const T& val)	{ _Insert(end(), val); }

		void pop_front()				{ erase(begin()); }
		void pop_back()					{ erase(--end()); }

		iterator insert(iterator cur, const T& val)
		{
			_Insert(cur, val);
			return (--cur);
		}

		iterator erase(iterator cur)
		{
			Node* node = (cur++).node();
			if (node != _Head)
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
				--_Size;
			}
			return cur;
		}

		iterator erase(iterator first, iterator last) // [first, last)
		{
			if (first == begin() && last == end())
			{
				clear();
				return (end());
			}
			else
			{
				while (first != last)
					first = erase(first);
				return (last);
			}
		}

		void remove(const T& val)
		{
			iterator last = end();
			for (iterator first = begin(); first != last; )
			{
				if (*first == val)
					first = erase(first);
				else
					++first;
			}
		}

		template<class condition>
		void remove_if(condition cond)
		{
			iterator last = end();
			for (iterator first = begin(); first != last; )
			{
				if (cond(*first))
					first = erase(first);
				else
					++first;
			}
		}

		void clear()
		{
			Node* next;
			Node* node = _Head->next;
			_Head->next = _Head;
			_Head->prev = _Head;
			_Size = 0;

			for (; node != _Head; node = next)
			{
				next = node->next;
				delete node;
			}
		}

		void resize(size_t newsize)
		{
			resize(newsize, T());
		}

		void resize(size_t newsize, T val)
		{
			if (_Size < newsize)
				_Insert_n(end(), newsize - _Size, val);
			else
				while (newsize < _Size)
					pop_back();
		}

		iterator begin()				{ return iterator(_Head->next); }
		const_iterator begin() const	{ return const_iterator(_Head->next); }

		iterator end()					{ return iterator(_Head); }
		const_iterator end() const		{ return const_iterator(_Head); }

		T& front()						{ return *begin(); }
		const T& front() const			{ return *begin(); }

		T& back()						{ return *(--end()); }
		const T& back() const			{ return *(--end()); }

		size_t size() const				{ return _Size; }
		bool empty() const				{ return _Size == 0; }

	protected:
		Node* _Buynode(Node* next, Node* prev, const T& val)
		{
			Node* node = new Node;
			node->next = next;
			node->prev = prev;
			node->val = val;
			return node;
		}

		void _Insert(iterator cur, const T& val)
		{
			Node* node = cur.node();
			Node* newnode = _Buynode(node, node->prev, val);
			_Size++;
			node->prev = newnode;
			newnode->prev->next = newnode;
		}

		void _Insert_n(iterator cur, size_t count, const T& val)
		{
			for (; 0 < count; --count)
				_Insert(cur, val);
		}
	};

#endif



} // namespace

#endif // __safelist_h__