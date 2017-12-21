#ifndef __I_BufferManager_H__
#define __I_BufferManager_H__

#include "RenderEngineCommon.h"

namespace rkt
{
	/*! addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

/** 顶点缓冲区接口
*/
struct IVertexBuffer
{
	/** 释放定点缓冲区对象
	*/
	virtual void 	release() = 0;

	/** 获得顶点大小，每个顶点占用的字节数
	@return 顶点大小
	*/
	virtual uint	getVertexSize() const = 0;

	/** 获得顶点的数量
	@return 顶点数量
	*/
	virtual uint	getNumVertices() const = 0;

	/** Buffer的使用方式
	@return Buffer的使用方式
	@see BufferUsage
	*/
	virtual BufferUsage	getUsage() const = 0;

	/** 加锁缓冲区，获得数据的指针，从而可以操作数据
	@param offset 从数据开始的偏移值
	@param length 加锁的区域长度
	@param options 加锁的方式
	@see LockOptions
	@return 数据区指针
	*/
	virtual void *	lock(uint offset,uint length,LockOptions options) = 0;

	/** 解锁缓冲区，和lock配套使用
	*/
	virtual void 	unlock() = 0;

	/** 获取缓冲区是否处于被加锁状态
	@return true缓冲区被加锁，false未锁定
	*/
	virtual bool	isLocked() = 0;

	/** 从缓冲区读取数据，放到指定的缓存
	@param offset 从数据开始的偏移值
	@param length 要读取的数据长度
	@param pDest 目标缓冲区地址
	*/
	virtual void 	readData(uint offset,uint length,void * pDest) = 0;	

	/** 把数据写入缓冲区
	@param offset 从数据开始的偏移值
	@param length 要写入的的数据长度
	@param pSource 要写入的数据地址
	@param discardWholeBuffer 是否要抛弃整个buffer的数据(会提高速度)
	*/
	virtual void 	writeData(																			
	uint offset,																	
	uint length,																	
	const void * pSource,															
	bool discardWholeBuffer = false) = 0;

	/** 克隆顶点缓冲区的数据，用户负责返回的顶点缓冲区对象的生命周期
	@return 克隆出来的顶点缓冲区
	*/
	virtual IVertexBuffer*	clone() = 0;

	virtual void invalidateDeviceObjects() = 0;
};																	

/** 索引缓冲区接口
*/
struct IIndexBuffer
{
	/** 获得索引缓冲区类型，参考IndexType枚举类型
	@see IndexType
	@return 缓冲区类型
	*/
	virtual IndexType	getType(void ) const = 0;							

	/** 获取索引的数量
	@return 索引的数量
	*/
	virtual uint		getNumIndices(void ) const = 0;						

	/** 获得索引的长度(字节)
	@return 每个索引的长度，一般为2,4
	*/
	virtual uint		getIndexSize(void ) const = 0;						

	/** 获得索引缓冲区的使用方式，参考BufferUsage
	@see BufferUsage
	@return 索引缓冲区的使用方式
	*/
	virtual BufferUsage	getUsage() const = 0;

	/** 加锁缓冲区，从来可以直接存取数据
	@param offset 从数据开始的偏移（字节）
	@param length 要加锁的数据区的长度
	@param options 加锁的方式
	@see LockOptions
	@return 直接数据的地址
	*/
	virtual void *		lock(uint offset,uint length,LockOptions options) = 0;						

	/** 解锁缓冲区，和lock配套使用
	*/
	virtual void 		unlock() = 0;		

	/** 获取索引缓冲区是否处于加锁状态
	@return true表示处于加锁状态，false则未加锁
	*/
	virtual bool		isLocked() = 0;		

	/** 释放缓冲区对象
	*/
	virtual void 		release() = 0;		

	/** 从缓冲区读取数据，放到指定的缓存
	@param offset 从数据开始的偏移值
	@param length 要读取的数据长度
	@param pDest 目标缓冲区地址
	*/
	virtual void 	readData(uint offset,uint length,void * pDest) = 0;	

	/** 把数据写入缓冲区
	@param offset 从数据开始的偏移值
	@param length 要写入的的数据长度
	@param pSource 要写入的数据地址
	@param discardWholeBuffer 是否要抛弃整个buffer的数据(会提高速度)
	*/
	virtual void 	writeData(																			
		uint offset,																	
		uint length,																	
		const void * pSource,															
		bool discardWholeBuffer = false) = 0;
};

class VertexDeclaration;
class VertexBufferBinding;

/** 缓冲区管理接口
*/
struct IBufferManager
{
	/** 创建顶点缓冲区
	@param vertexSize 每个顶点的数据长度(字节)
	@param numVertices 顶点的数量
	@param usage 缓冲区的使用方式，参考BufferUsage
	@see BufferUsage
	@return 顶点缓冲区接口
	*/
	virtual IVertexBuffer*	createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED) = 0;

	/** 创建索引缓冲区
	@param itype 索引类型
	@see IndexType
	@param numIndices 索引的数量
	@param usage 缓冲区的使用方式
	@see BufferUsage
	@return 索引缓冲区接口
	*/
	virtual IIndexBuffer*	createIndexBuffer(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED) = 0;	

	/** 创建内存顶点缓冲区
	@param vertexSize 每个顶点的数据长度(字节)
	@param numVertices 顶点的数量
	@param usage 缓冲区的使用方式，参考BufferUsage
	@see BufferUsage
	@return 顶点缓冲区接口
	*/
	virtual IVertexBuffer*	createVertexBufferUP(uint vertexSize,uint numVertices,BufferUsage usage,ResPool pool = POOL_MANAGED)=0;
	/** 创建内存索引缓冲区
	@param itype 索引类型
	@see IndexType
	@param numIndices 索引的数量
	@param usage 缓冲区的使用方式
	@see BufferUsage
	@return 索引缓冲区接口
	*/
	virtual IIndexBuffer*	createIndexBufferUP(IndexType itype,uint numIndices,BufferUsage usage,ResPool pool = POOL_MANAGED)=0;

	virtual void injectFrame() = 0;

	/// 缓冲统计的输出
	virtual void dump() = 0;

	virtual void intialize() = 0;
	virtual void finalize() = 0;
	/** 检查DEFAULT的顶点缓冲是否都释放掉了，因为微端在下载地图资源时，
	改变分辨率，有部分VertexBuffer是没有释放掉的，没有查出是哪里创
	建了这个VertexBuffer，先用这个方法解决。这个函数一定要在
	RenderEngine::invalidateDeviceObjects()中放在最后调用，因为只是做
	一个检查而已，切记！！
	*/
	virtual void CheckDefaultPoolVertexBufferReleased() = 0;

	/** 增加DEFAULT的顶点缓冲,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能
	@param pVb 顶点缓冲
	*/
	virtual void AddDefaultPoolVertexBuffer(IVertexBuffer *pVb)=0;
	/** 移除DEFAULT的顶点缓冲,此函数目前只和检查DEFAULT的顶点缓冲是否都释放掉有关，不做其他功能
	@param pVb 顶点缓冲
	*/
	virtual void RemoveDefaultPoolVertexBuffer(IVertexBuffer *pVb)=0;
};
	/** @} */

}
#endif