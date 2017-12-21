//==========================================================================
/**
* @file	  : PkgStream.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-1   11:04
* purpose : PKG流
*/
//==========================================================================


#ifndef __PKGSTREAM_H__
#define __PKGSTREAM_H__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include "Stream.h"
#include "PkgStruct.h"
#include "MemStream.h"

namespace rkt {

/// 包流（各成员函数的使用方法参考Stream的注释），包流的基本操作都是委托MemoryStream进行处理
class RKT_EXPORT PkgStream : public Stream
{
protected:
	Stream*			m_stream;		/// 指包流所属的流对象
	PkgBlock		m_block;		///	块结构
	BlockInfo		m_blkInfo;		/// 块信息
	MemoryStream	m_memStream;	/// 内存流对象

	// 构造（析构）
public:
	PkgStream();
	PkgStream(const char* name);
	virtual ~PkgStream();

	// Stream接口
public:
	/** 打开包流
	 @param mode 包流打开模式，此参数包流忽略
	 @return 成功返回true，否则返回false
	 @note 有关文件流打开模式的信息：<br>
	 */
	virtual bool open(uint flags = modeRead|shareRead|modeExist);

	/// 清除对象内容，包括内部分配的缓冲区的释放
	virtual bool close();

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
		当buffer=0或者toWrite=0时
	 */
	virtual bool write(const void* bufer, uint toWrite);

	/** 
	 @param 读取一行字符串
	 @return 成功返回true，否则当当前位置不在内存块范围内时返回false
	 */
	virtual bool readString(char* strBuffer, uint& bufSize);

	/** 写入一个字符串
	 @param str 要写入的字符串
	 @return 成功返回true，否则当str为NULL或者字符串长度超过内存流剩余空间而又不支持增长时返回false
	 */
	//virtual bool writeString(const char* str);

	/** 内存读写指针定位
	 @param offset 偏移大小
	 @param from 计算偏移位置的参考位置
	 @return 成功返回true，否则返回false
	 @note 失败的可能原因：<br>
		参数无效；通过计算后的位置< 0
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

	/// 设置流的长度（总是返回false，不支持设置长度）
	virtual bool setLength(uint newLen) { return false; }

	/// 包流是否存在（直接返回包流所在的包文件是否存在）
	virtual bool isExist() const;

	/// 包流是否处于打开状态
	virtual bool isOpen() const;

	/// 总是返回false（请通过流系统进行包流的增删）
	virtual bool remove();

	/// 总是返回false（请通过流系统进行包流的更名）
	virtual bool rename(const char* newName);

	/// 将包流数据保存为标准文件
	virtual bool save(const char* newName);

	bool save(const char* newName, ulong encodeFlags);

	/// 获取未压缩的文件大小，这个可以用来判断文件大小是否为0,为0的文件不做后续处理
	virtual uint getFileSize(){return m_block.fSize;}
	// 其他公共方法;
public:
	/** 设置流信息
	 @param stream 包流所属的流对象
	 @param block 包流的块结构信息
	 @return 目前总是返回true
	 */
	bool setStreamInfo(Stream* stream, const PkgBlock& block);
};

} // namespace

#endif // #ifdef SUPPORT_STREAM

#endif // __PKGSTREAM_H__
