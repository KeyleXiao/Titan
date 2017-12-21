#ifndef __STREAM_H__
#define __STREAM_H__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include "Path.h"


namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 流（基类）
class RKT_EXPORT Stream
{
protected:
	CPath	m_path;		/// 流路径

	// 构造（析构）
public:
	Stream() { m_path.isFile(true); }
	Stream(const char* path);
	virtual ~Stream();

	const CPath& getPath() const		{ return m_path; }

	// 接口方法
public:
	/** 打开流(文件流默认以只读、二进制形式打开，其他流类型不关心打开模式)
	 @param mode 流打开模式
	 @return 成功返回true，否则返回false
	 @note 有关文件流打开模式的信息：<br>

	- Read	读    ,文件不存在则失败<br>
	- Write	写    ,文件不存在则创建,存在则清空内容<br>
	 */
	enum OpenFlags
	{
		modeRead		= 0x1,		/// 打开用于读
		modeWrite		= 0x2,		/// 打开用于写
		shareRead		= 0x10,		/// 共享读
		shareWrite		= 0x20,		/// 共享写
		modeExist		= 0x100,	/// 必须存在，否则失败
		modeCreate		= 0x200,	/// 不存在则创建，存在则清空
	};
	virtual bool open(uint flags = modeRead|shareRead|modeExist) { return false; }

	/// 关闭流（跟open对应）
	virtual bool close() { return false; }


	/** 从流中读取数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toRead 要读取的字节大小
	 @return 成功返回true，否则返回false
	 */
	virtual bool read(void* buffer, uint toRead) { return false; }

	/** 从流中读取字符串（对于包文件中的文件对象拒绝所有越界的读写）
	@param str 缓冲区地址
	@param length 缓冲区的最大长度，实际返回的长度也用这个参数带回
	@return 成功返回true，否则返回false
	*/
	virtual bool readString(char* str, uint& length) { return false; }
	
	/** 向流中写入数据（对于包文件中的文件对象拒绝所有越界的读写）
	 @param buffer 缓冲区地址
	 @param toWrite 要写入的字节大小
	 @return 成功返回true，否则返回false
	 */
	virtual bool write(const void* bufer, uint toWrite) { return false; }

	/** 文件读写位置定位
	 @param offset 偏移量
	 @param from 从那种指定的位置开始进行偏移，参考: SEEK_SET 文件头; SEEK_CUR 文件当前位置; SEEK_END 文件尾
	 @return 
	 */ 
	virtual bool seek(__int64 offset, uint from = SEEK_SET) { return false; }

	/// 文件读写位置定位到文件头
	virtual bool seekToBegin() { return false; }

	/// 文件读写位置定位到文件尾
	virtual bool seekToEnd() { return false; }

	/// 获得文件指针的位置
	virtual ulong getPosition() const { return 0; }


	/// 将文件的缓冲区内容写入磁盘(包文件不支持)
	virtual bool flush() { return false; }


	/// 获取文件长度(文件没打开时也能获取长度),如果是包文件,则是指压缩后的长度
	virtual uint getLength() const { return 0; }

	/// 设置流的长度
	virtual bool setLength(uint newLen) { return false; }

	/** 设置流路径
	 @param path 流的路径名
	 @return 成功返回true，否则返回false
	 */ 
	virtual bool setPath(const char* path);


	/// 流是否存在(注: 包流不支持,包流时可通过get(Handle, name)的返回值来判断是否存在)
	virtual bool isExist() const { return false; }

	/// 流是否处于开启状态
	virtual bool isOpen() const	{ return false; }


	/// 删除流
	virtual bool remove() { return false; }

	/** 重命名流
	 @param newName 新的流名
	 @return 成功返回true，否则返回false
	 */ 
	virtual bool rename(const char* newName) { return false; }

	/** 流存储为文件
	 @param newName 要保存的文件名
	 @return 成功返回true，否则返回false
	 */
	virtual bool save(const char* newName) { return false; }

	/// 释放流对象
	virtual void release();
};

}

#endif // #ifdef SUPPORT_STREAM

#endif // __STREAM_H__
