#ifndef __MemStream_H__
#define __MemStream_H__

#include "Stream.h"

#ifdef SUPPORT_STREAM

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 内存流（成员函数的使用方法参考Stream中的注释）
class RKT_EXPORT MemoryStream : public Stream
{
protected:
	uint	m_growBytes;		/// 当内存不够时，每次递增的字节数
	uint	m_pos;				/// 内存读写位置
	uint	m_bufSize;			/// 缓冲区大小
	uint	m_fileSize;			/// 文件大小（ 小于等于 m_bufSize ）
	uchar*	m_buffer;			/// 缓冲区
	bool	m_autoDelete;		/// 是否在析构时自动删除缓冲区

public:
	/// 针对自动增长的内存，内存还未分配
	MemoryStream(uint growBytes = 1024);

	/// 用于某个已经分配的缓冲区的操作，如果growBytes不为0，那么必须保证buffer是堆内存而不是栈内存
	MemoryStream(uchar* buffer, uint bufSize, uint growBytes = 0);

	/// 分配一个大小为size的缓冲区，析构时候自动删除
	MemoryStream(uint	size,uint growBytes = 1024);

	virtual ~MemoryStream();

public:
	/**捆绑到指定的缓冲区,如果growBytes不为0，那么必须保证buffer是堆内存而不是栈内存
	 @param buffer 缓冲区地址
	 @param bufSize 缓冲区大小（字节数）
	 @param growBytes 当内存不够时，增长的字节数
	 @note growBytes为0一般用于对缓冲区进行读操作，否则进行写操作
	 */
	void attach(uchar* buffer, uint bufSize, uint growBytes = 0);

	/// 从指定的缓冲区分离，与attach匹配使用
	uchar* detach();

	/// 获得缓冲区地址
	uchar* getBuffer() const;

	/// 获取内存不足时的增长字节数
	uint getGrowBytes() const;

	/// 获取缓冲区大小（大于等于文件大小）
	uint getBufferSize() const;

	// Stream接口
public:
	/// 总是返回true
	virtual bool open(uint flags = modeRead|shareRead|modeExist);

	/// 清除对象内容，包括内部分配的缓冲区的释放（如果需要释放的话）
	virtual bool close();

	bool readString(char* str, uint& length);
	/** 读内存操作
	 @param buffer 用于存储数据的缓冲区
	 @param toRead 要读取的字节数
	 @return 成功返回true，否则返回false
	 @note 可能的失败原因：<br>
		当buffer为NULL或者toRead为0,但当读数据指针越过内存流尾部而进行截断操作时，返回true
	 */
	virtual bool read(void* buffer, uint toRead);

	/** 写内存操作
	 @param buffer 要写入的内存地址
	 @param toWrite 要写入的字节数
	 @return 成功返回true，否则返回false
	 @note 可能的失败原因：<br>
		当buffer=0或者toWrite=0或者已经写到了尾部而内存不支持增长时
	 */
	virtual bool write(const void* buffer, uint toWrite);

	/** 内存读写指针定位
	 @param offset 偏移大小
	 @param from 计算偏移位置的参考位置
	 @return 成功返回true，否则返回false
	 @note 失败的可能原因：<br>
		参数无效；通过计算后的位置< 0；通过计算后的位置大于文件尾，但内存不支持增长
	 */
	virtual bool seek(__int64 offset, uint from = SEEK_SET);

	/// 内存读写指针定位到内存开始处
	virtual bool seekToBegin();

	/// 内存读写指针定位到内存结尾处
	virtual bool seekToEnd();

	/// 得到当前的位置
	virtual ulong getPosition() const;

	/// 总是返回true
	virtual bool flush() const;

	/// 获取内存流的大小（小于等于内存缓冲区大小）
	virtual uint getLength() const;

	/** 设置内存流的长度
	 @param newLen 要设定的内存流长度（当小于内存流当前大小时会截取内存流尾部数据）
	 @return 成功返回true，否则当新的长度超过缓冲区大小，而缓冲区不支持增长时返回false
	 */
	virtual bool setLength(uint newLen);

	/// 总是返回true
	virtual bool isExist() const;

	/// 总是返回true
	virtual bool isOpen() const;

	/** 设置流名
	@param name 流名
	@return 总是成功返回true
	*/ 
	virtual bool setPath(const char* path);

	/// 总是返回false
	virtual bool remove();

	/// 总是返回true
	virtual bool rename(const char* newName);

	/** 保存内存文件到标准文件
	 @param newName 要保存的内存文件名（请使用绝对路径，使用相对目录可能会导致意外错误，如果目录不存在会自动创建，目录创建支持递归）
	 @return 成功返回true，否则如果缓冲区不存在，返回false
	 */
	virtual bool save(const char* newName);
	/// 释放流对象
	virtual void release();

protected:
	virtual uchar* _alloc(uint bytes);
	virtual uchar* _realloc(uchar* ptr, uint bytes);
	virtual uchar* __memcpy(uchar* dest, const uchar* src, uint bytes);
	virtual void _free(uchar* ptr);
	virtual bool growFile(uint newLen);
};

	/** @} */
}

#endif // #ifdef SUPPORT_STREAM

#endif // __MEMSTREAM_H__
