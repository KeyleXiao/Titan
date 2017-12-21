#ifndef __STREAMSYSTEM_H__
#define __STREAMSYSTEM_H__

#include "Stream.h"

#ifdef SUPPORT_STREAM

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 枚举流Callback
struct IListFileCallback;
#ifdef SUPPORT_ASYNC_STREAM
struct IAsynStreamSink;
#endif

/// 文件校验接口，用来判断文件是否正确，该接口主要用于判断包中的某个文件是否是最新的，不是最新的就去服务器重新下载
struct IFileVerify;

/// 文件系统接口
struct IFileSystem
{

#ifdef SUPPORT_ASYNC_STREAM
	/** 异步请求一个流对象
	@param streamName	文件名
	@param sink			请求结束后的回调接口
	@param userdata		用户数据
	@return 成功返回true，流不存在，或者请求出错，返回false，返回false时，不回调sink
	*/
	virtual bool request(const char* streamName, IAsynStreamSink* sink, ulong userdata = 0, ulong priority = 0) = 0;
#endif

	/**打开文件系统或者包系统
	@param streamSysPath 文件系统路径或者包文件名
	*/
	virtual bool open(const char* streamSysPath) = 0;

	/**某个文件是否位于此文件系统下
	@param streamName 文件名
	@return 是否找到
	*/
	virtual bool find(const char* streamName) = 0;
	
	/**打开某个文件
	@param streamName 文件名
	@return 文件流，可能是磁盘文件也可能是包文件
	@remarks
	*/
	virtual Stream* get(const char* streamName) = 0;

	/**枚举此文件系统下的所有文件
	@param pCallback 回调接口，用来接收查找到的文件名
	@param p 自定义参数
	*/
	virtual void list(IListFileCallback* pCallback,void *p) = 0;

	/**关闭文件系统
	@return 是否成功
	@remarks
	*/
	virtual bool close() = 0;

	/**释放文件系统
	*/
	virtual void release() = 0;

    /// 文件校验接口，用来判断文件是否正确，该接口主要用于判断包中的某个文件是否是最新的，不是最新的就去服务器重新下载
	virtual void add_verify( IFileVerify * pVerify ) {};
};

 /// 文件校验接口，用来判断文件是否正确，该接口主要用于判断包中的某个文件是否是最新的，不是最新的就去服务器重新下载
 /// 因为有些微端是MPK中也有文件的 
struct IFileVerify
{
	virtual bool verify( const char * name,unsigned __int64 size,unsigned long crc ) = 0;
};

	/** @} */
}

#endif // #ifdef SUPPORT_STREAM


#endif // __STREAMSYSTEM_H__
