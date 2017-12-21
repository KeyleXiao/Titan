/*****************************************************************************
** �ļ���:	DetourArrayBuffer.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	���
** ��  ��:	2015/06/15
** ��  ��:	1.0
** ��  ��:	����һ����̬��������(��С�̶�1024)���ڲ�������ʵ��
** Ӧ  ��:
**************************** �޸ļ�¼ ****************************************
** �޸���:  
** ��  ��:  
** ��  ��:  
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

/* TODO:ȱ�ٵ���������Ҫʵ��iterator,����ʹ��C++11/Boost�е�Array�����
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
