/*****************************************************************************
** 文件名:	DetourArrayBuffer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭璐
** 日  期:	2015/06/15
** 版  本:	1.0
** 描  述:	定义一个静态缓冲区类(大小固定1024)，内部用数组实现
** 应  用:
**************************** 修改记录 ****************************************
** 修改人:  
** 日  期:  
** 描  述:  
******************************************************************************/

#ifndef DETOURARRAYBUFFER_H
#define DETOURARRAYBUFFER_H

#define INVALID_ARRAY_INDEX		((unsigned short)0xffff)
#define DEFALUT_ARRAY_SIZE      1024
typedef unsigned short			index_type;			

struct TLinkNode
{
	index_type next;
};

/* TODO:缺少迭代器，需要实现iterator,建议使用C++11/Boost中的Array来替代
 */
template<typename T, unsigned short SIZE = DEFALUT_ARRAY_SIZE>
class ArrayBuffer
{
public:
	ArrayBuffer()
	{
		for (index_type i = 0; i <= SIZE-1; ++i)
		{
			_links[i].next = i+1;
		}
        _links[SIZE-1].next = INVALID_ARRAY_INDEX;
		_freelink = 0;
        _size = 0;
        memset(&_data[0], 0, sizeof(T)*SIZE);
	}

	~ArrayBuffer()
	{
	
	}

	inline size_t size() const
	{
		return _size;
	}

	const T& operator[] (index_type i) const
	{
		return _data[i];
	}

	T& operator[] (index_type i)
	{
		return (T&)(const_cast<const ArrayBuffer*>(this)->operator[](i));
	}

	index_type allocNode()
	{
		index_type index = INVALID_ARRAY_INDEX;
		if (_freelink != INVALID_ARRAY_INDEX)
		{
			index = _freelink;
			_freelink = _links[_freelink].next;
		}
		++_size;
		return index;
	}

	void freeNode(index_type index)
	{
		if (index < 0 || index >= SIZE)
			return;

        memset(&_data[index], 0, sizeof(T));
		_links[index].next = _freelink;
		_freelink = index;
		--_size;
	}

	const TLinkNode& getLinkNode(index_type index) const
	{
		return _links[index];
	}

	TLinkNode& getLinkNode(index_type index)
	{
		return (TLinkNode&)(const_cast<const ArrayBuffer*>(this)->getLinkNode(index));
	}

private:
	index_type	_freelink;
	size_t		_size;
	TLinkNode	_links[SIZE];
	T			_data[SIZE];
};

#endif
