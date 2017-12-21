/*******************************************************************
** 文件名:	fast_id_map.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include <list>
#include <vector>

/**
	一种快速生成ID，快速索引，并占用内存小的容器，内部用数组实现
	用来替代map容器,map容器在存储海量对象时有以下几个问题
	1.查找复杂度log n，在海量的访问需求下，依然略慢，不如直接数组索引
	2.占用内存有风险，因为std::hash_map的实现机制，当节点不够时可能会分配出2倍桶节点长度，
	  并且不会随着节点的删除而减少，这种内部机制曾经引起事件引擎中一个非常难以排查的内存泄露bug
	3.这里我们封装一个分段数组，每次以1024个节点的元素固定增长，索引时直接查找数组，这样速度最快
	  前期条件是，以ID作为键值，并且ID也是由这个容器快速分配的
*/

// @param ID   : 要作为ID键值的类型
// @param DATA : 要作为数组存储的类型
// @param NULL_DATA  : 空对象
// @param STACK_SIZE : 节点不够时每次分配的数目
template<typename ID=int,typename DATA=void *,int BLOCK_SIZE=1024>
class fast_id_map
{
protected:
	struct Block_Data
	{
		ID      start_id;                     // 本块开始的ID
		int     free_num;                     // 本块空闲的节点数
		DATA  * data_block;                   // 块数组指针，元素个数为BLOCK_SIZE
	};

	std::vector<Block_Data>		block_list;		// 块列表
	std::list<ID>				free_list;      // 空闲可分配的ID列表
    size_t						allocated_size; // 已经分配的ID数
	DATA						NULL_DATA;

public:
	fast_id_map(DATA data=0) : NULL_DATA(data), allocated_size(0) {}

	// 释放内存
	virtual ~fast_id_map()
	{
		for ( size_t i=0;i<block_list.size();++i )
		{
			if ( block_list[i].data_block!=0 )
			{
				delete [](block_list[i].data_block);
			}
		}
		block_list.clear();
	}

	// 分配一个新的ID
	ID allotID()
	{
		if ( free_list.empty() )
		{
			alloc_new_block();
		}

		ID id= free_list.front();
		free_list.pop_front();

		unsigned int block_index = id / BLOCK_SIZE;
		if ( block_index>=block_list.size() )
		{
			ErrorLn("fast_id_map allotID() failed!" << id);
			return id;
		}

        ++allocated_size;
		block_list[block_index].free_num -= 1;
		return id;
	}

	// 归还一个ID
	void revertID( ID id )
	{
		free_list.push_back(id);

		unsigned int block_index = id / BLOCK_SIZE;
		if ( block_index>=block_list.size() )
		{
			ErrorLn("fast_id_map revertID() failed!" << id);
			return;
		}

		int offset = id - block_index*BLOCK_SIZE;
		block_list[block_index].data_block[offset] = NULL_DATA;
		block_list[block_index].free_num += 1;

        --allocated_size;

		// 全部空闲了，则把这个块释放掉
		if ( block_list[block_index].free_num>=BLOCK_SIZE ){ clean(block_index);}
	}

	// 获取数据
	DATA getData( ID id )
	{
		if ( id <0 )  // 容错
		{
			ErrorLn("fast_id_map getData invalid id="<<id );
			return NULL_DATA;
		}

		unsigned int block_index = id / BLOCK_SIZE;
		if ( block_index>=block_list.size() )
		{
			return NULL_DATA;
		}

		if ( block_list[block_index].data_block==0 )
		{
			return NULL_DATA;
		}

		int offset = id - block_index*BLOCK_SIZE;
		return block_list[block_index].data_block[offset];
	}

	// 设置数据
	bool setData( ID id,DATA value )
	{
		if ( id <0 )  // 容错
		{
			ErrorLn("fast_id_map setData invalid id="<<id );
			return false;
		}

		unsigned int block_index = id / BLOCK_SIZE;
		if ( block_index>=block_list.size() )
		{
			return false;
		}

		if ( block_list[block_index].data_block==0 )
		{
			return false;
		}

		int offset = id - block_index*BLOCK_SIZE;
		block_list[block_index].data_block[offset] = value;
		return true;
	}

	size_t getAllDatas( DATA* pDatas,size_t nCapacity )
	{
		size_t nCount = 0;
		for ( size_t i=0;i<block_list.size();++i )
		{
			if ( block_list[i].data_block )
			{
				for ( size_t j=0;j<BLOCK_SIZE;++j)
				{
					if ( nCount<nCapacity )
					{
						if ( !(block_list[i].data_block[j]==NULL_DATA) )
						{
							pDatas[nCount++] = block_list[i].data_block[j];
						}
					}
					else
					{
						return nCount;
					}
				}
			}
		}
		return nCount;
	}

    inline size_t size()
    {
        return allocated_size;
    }

	inline bool empty()
	{
        return ( size()==0 );
	}

private:
	// 当前空闲节点不够用了，要新分配
	void alloc_new_block()
	{
		int block_index = -1;
		for ( size_t i=0;i<block_list.size();++i )
		{
			if ( block_list[i].data_block==0 )
			{
				block_list[i].data_block = new DATA[BLOCK_SIZE];		// 初始化分配
                for (size_t j=0;j<BLOCK_SIZE;++j) block_list[i].data_block[j]=NULL_DATA;
				block_list[i].free_num   = BLOCK_SIZE;
				block_index = (int)i;
                break;
			}
		}

		if ( block_index<0 )
		{
			Block_Data block;
			block.start_id = block_list.size()*BLOCK_SIZE;
			block.free_num = BLOCK_SIZE;
			block.data_block = new DATA[BLOCK_SIZE];		// 初始化分配
			for (size_t i=0;i<BLOCK_SIZE;++i) block.data_block[i]=NULL_DATA;
			block_list.push_back(block);
			block_index = block_list.size()-1;
		}

		if ( block_index>=0 && block_index<(int)block_list.size() )
		{
			for ( int n=0;n<BLOCK_SIZE;++n )
			{
				free_list.push_back(block_index*BLOCK_SIZE + n);
			}
		}
	}

	// 清理空闲节点
	void clean(int index)
	{
		if ( block_list[index].free_num!=BLOCK_SIZE )
		{
			ErrorLn("fast_id_map clean() error!free_num!=BLOCK_SIZE" <<block_list[index].free_num );
			return;
		}

		int clean_count = 0;

		std::list<ID>::iterator it = free_list.begin();
		for ( ;it!=free_list.end(); )
		{
			if ( (*it) / BLOCK_SIZE == index  )
			{
				it=free_list.erase(it);
				++clean_count;
			}else
			{
				++it;
			}
		}

		if ( clean_count!=BLOCK_SIZE)
		{
			ErrorLn("fast_id_map clean() error!clean_count!=BLOCK_SIZE" <<clean_count );
			return;
		}

		if ( block_list[index].data_block )
		{
			delete [](block_list[index].data_block);
			block_list[index].data_block = 0;
		}
	}

};
