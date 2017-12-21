#ifndef __FILESTREAM_H__
#define __FILESTREAM_H__

#include "Stream.h"

#ifdef SUPPORT_STREAM

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/



/// 标准文件流（各方法的使用参考Stream的对应方法）
class RKT_EXPORT FileStream : public Stream
{
protected:
	void* m_stream;		/// 文件流句柄

	// 下面扩充，使之可以针对某个文件的部分虚拟成一个文件流
	__int64	mFileStart;	/// 虚拟文件在原始文件中的开始偏移位置
	__int64	mFileLength;/// 虚拟文件的大小
	__int64	mFilePos;	/// 虚拟文件的指针操作位置，以mFileStart为0为开始处
public:
	FileStream();
	FileStream(const char* path);
	virtual ~FileStream();

	/// 用于将一个文件的部分虚拟成流时进行的设置
	void setFileInfo(ulong fileStart, ulong fileLength);

public:
	/// 获得文件流句柄
	void* getHandle() const		{ return m_stream; }

	/// 文件流转换成文件句柄
	//operator FILE*() const		{ return m_stream; }

	// Stream接口
public:
	/** 打开文件流(默认以只读、二进制形式打开)
	 @param mode 文件打开模式
	 @return 成功返回true，否则返回false
	 @note 有关文件打开模式的信息：<br>

	 - Read		读    ,文件不存在则失败<br>
	 - Write	写    ,文件不存在则创建,存在则清空内容<br>
	 */
	virtual bool open(uint flags = modeRead|shareRead|modeExist);

	/// 关闭文件流
	virtual bool close();

	bool readString(char* str, uint& length);
	/** 从文件流中读取数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toRead 要读取的字节大小
	 @return 成功返回true，否则返回false
	 @note 当读写文件时,发生EOF错误，这里也返回true，因为不影响数据的正确性，<br>
		只有当bad file,access之类的错误才返回false
	 */
	virtual bool read(void* buffer, uint toRead);

	/** 向文件流中写入数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toWrite 要写入的字节大小
	 @return 成功返回true，否则返回false
	 @note 当读写文件时,发生EOF错误，这里也返回true，因为不影响数据的正确性，<br>
		只有当bad file,access之类的错误才返回false
	 */
	virtual bool write(const void* bufer, uint toWrite);

	/// 用法同Stream
	virtual bool seek(__int64 offset, uint from = FILE_BEGIN);

	/// 用法同Stream
	virtual bool seekToBegin();

	/// 用法同Stream
	virtual bool seekToEnd();

	/// 用法同Stream
	virtual ulong getPosition() const;

	/// 用法同Stream
	virtual bool flush() const;

	/// 用法同Stream
	virtual uint getLength() const;

	/// 用法同Stream
	virtual bool setLength(uint newLen);

	/// 用法同Stream
	virtual bool isExist() const;

	/// 用法同Stream
	virtual bool isOpen() const;

	/// 删除文件流
	virtual bool remove();

	/** 重命名文件流
	 @param newName 新的文件流名字
	 @return 成功返回true，否则返回false
	 @note 当前的文件流虽然改了名，但是文件流的状态不变，如文件流未关闭，那么现在也没有关闭，文件指针存取位置也不变。
	 */ 
	virtual bool rename(const char* newName);

	/** 文件流存储为文件
	 @param newName 要保存的文件名
	 @return 成功返回true，否则返回false
	 @note 当newName为NULL时，该函数相当于flush，否则，相当于将文件流现在
		的内容copy一份另存为一个文件，save后当前的文件流状态不变。
	 */
	virtual bool save(const char* newName);
	/// 释放流对象
	virtual void release();
};

	/** @} */
}

#endif // #ifdef SUPPORT_STREAM

#endif // __FILESTREAM_H__
