//==========================================================================
/**
* @file	  : AsynStream.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-10-15   12:05
* purpose : 异步流
*/
//==========================================================================

#ifndef __AsynStream_h__
#define __AsynStream_h__

#include "Common.h"

#if defined(SUPPORT_STREAM) && defined(SUPPORT_ASYNC_STREAM)

#include "PkgStruct.h"
#include "Stream.h"
#include "MemStream.h"
#include "Net/IAsynfile.h"

namespace rkt{

	struct IAsynStreamSink;

	class RKT_EXPORT AsynStream : public Stream
	{
	public:
		AsynStream() : Stream() {}
		AsynStream(const char* path) : Stream(path) {}

		virtual bool request(IAsynStreamSink* sink, ulong userdata = 0) = 0;
	};

	struct IAsynStreamSink
	{
		/** 异步请求的结果回调
		@param stream	请求的结果记录在该流中，如果读取文件失败，则stream为NULL
		@param userdata	发起请求时传入的用户数据
		*/
		virtual void onRequest(Stream* stream, ulong userdata = 0) = 0;

		/** 异步请求的数据解码 本调用可在非主线程调用
		@param stream	请求的结果记录在该流中，如果读取文件失败，则stream为NULL
		@param userdata	发起请求时传入的用户数据
		*/
		virtual void onDecode(Stream* stream, ulong userdata = 0) = 0;

	};

	/// 异步文件流（仅仅限于读，各方法的使用参考Stream的对应方法）
	class RKT_EXPORT AsynFileStream : public AsynStream, public IAsynFileHandler
	{
	protected:
		IAsynFile*			m_asynfile;
		MemoryStream*		m_stream;
		IAsynStreamSink*	m_sink;
	public:
		AsynFileStream();
		AsynFileStream(const char* path);
		virtual ~AsynFileStream();

		virtual bool request(IAsynStreamSink* sink, ulong userdata = 0);


		// IAsynFileHandler
	public:
		virtual void HandleReadSuccess(IAsynFile * file,unsigned __int64 pos,const char * data,unsigned long len,long userdata);
		virtual void HandleWriteSuccess(IAsynFile * file,unsigned __int64 pos,unsigned long len,long userdata) { }
		virtual void HandleReadFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long expect_bytes,long userdata);
		virtual void HandleWriteFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long len,long userdata) { }

		// Stream接口
	public:
		virtual bool open(uint flags = modeRead|shareRead|modeExist);

		/// 关闭文件流
		virtual bool close();

		bool readString(char* str, uint& length);
		virtual bool read(void* buffer, uint toRead);

		virtual bool write(const void* bufer, uint toWrite);

		/// 用法同Stream
		virtual bool seek(__int64 offset, uint from = SEEK_SET);

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

		virtual bool rename(const char* newName);

		virtual bool save(const char* newName);
		/// 释放流对象
		virtual void release();
	};


	/// 异步带编码的文件流（仅仅限于读，各方法的使用参考Stream的对应方法）
	class RKT_EXPORT AsynEncodeFileStream : public AsynStream, public IAsynFileHandler
	{
	protected:
		IAsynFile*			m_asynfile;
		MemoryStream*		m_stream;
		IAsynStreamSink*	m_sink;
	public:
		AsynEncodeFileStream();
		AsynEncodeFileStream(const char* path);
		virtual ~AsynEncodeFileStream();

		virtual bool request(IAsynStreamSink* sink, ulong userdata = 0);


		// IAsynFileHandler
	public:
		virtual void HandleReadSuccess(IAsynFile * file,unsigned __int64 pos,const char * data,unsigned long len,long userdata);
		virtual void HandleWriteSuccess(IAsynFile * file,unsigned __int64 pos,unsigned long len,long userdata) { }
		virtual void HandleReadFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long expect_bytes,long userdata);
		virtual void HandleWriteFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long len,long userdata) { }

		// Stream接口
	public:
		virtual bool open(uint flags = modeRead|shareRead|modeExist);

		/// 关闭文件流
		virtual bool close();

		bool readString(char* str, uint& length);
		virtual bool read(void* buffer, uint toRead);

		virtual bool write(const void* bufer, uint toWrite);

		/// 用法同Stream
		virtual bool seek(__int64 offset, uint from = SEEK_SET);

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

		virtual bool rename(const char* newName);

		virtual bool save(const char* newName);
		/// 释放流对象
		virtual void release();
	};


	/// 异步包流（仅仅限于读，各方法的使用参考Stream的对应方法）
	class RKT_EXPORT AsynPkgStream : public AsynStream
	{
	protected:
		IAsynFile*			m_asynfile;		/// 异步文件（如果是内存pkg的异步请求，该参数为NULL）
		PkgBlock			m_block;		///	块结构
		MemoryStream*		m_stream;		/// 异步流的结果
		IAsynStreamSink*	m_sink;			/// 异步流加载后的回调接口
		long				m_userdata;		/// 用户数据
	public:
		AsynPkgStream();
		AsynPkgStream(const char* path);
		virtual ~AsynPkgStream();

		virtual bool request(IAsynStreamSink* sink, ulong userdata = 0); // 只有pkg为文件时，才调用，为内存数据时，该函数没有被调用

		void setBlock(const PkgBlock& b)	{ m_block = b;}
		void setAsynFile(IAsynFile* af)		{ m_asynfile = af; }
		void onReadSuccess(const char* data,unsigned long len);
		void onReadFailed();

		// Stream接口
	public:
		virtual bool open(uint flags = modeRead|shareRead|modeExist);

		/// 关闭文件流
		virtual bool close();

		bool readString(char* str, uint& length);
		virtual bool read(void* buffer, uint toRead);

		virtual bool write(const void* bufer, uint toWrite);

		/// 用法同Stream
		virtual bool seek(__int64 offset, uint from = SEEK_SET);

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

		virtual bool rename(const char* newName);

		virtual bool save(const char* newName);
		/// 释放流对象
		virtual void release();
	};
}

#endif // #if defined(SUPPORT_STREAM) && defined(SUPPORT_ASYNC_STREAM)

#endif // __AsynStream_h__
