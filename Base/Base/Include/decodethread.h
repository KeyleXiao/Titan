/*******************************************************************
** 文件名:	DecodeThread.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/22/2011
** 版  本:	1.0
** 描  述:	
** 应  用:  异步解码线程	

**************************** 修改记录 ******************************
** 修改人: 刘晓锋
** 日  期: 2012-3-31
** 描  述: 为了兼容大端打包文件的解码和性能优化 这个文件进行由微端工程移入Base模块，完善大端处理相关功能
********************************************************************/
#pragma once
#include "Common.h"
#include "net/IAsynfile.h"
#include "net/producer_consumer_queue.h"
#include "FileSystem/AsynStream.h"
#include "thread.h"
#include "Singleton.h"
#include "Utility.h"
#include "Api.h"

struct IMicroClientSink
{
	// 文件内容已经解码(此时在另外一个线程，如果你不关心，默认不要处理)
	// @name : 文件名
	// @data : 文件内容
	// @len  : 文件长度
	// @userdata : 用户数据 
	virtual void OnDecodeData( const char * name,void * data,int len,unsigned long userdata_1,unsigned long userdata_2 ) = 0;

	// 获取文件内容
	// @name : 文件名
	// @data : 文件内容
	// @len  : 文件长度
	// @userdata : 用户数据 
	virtual void OnGetFileData( const char * name,void * data,int len,unsigned long userdata_1,unsigned long userdata_2 ) = 0;

	// 校验一段内容是否合法，防止文件错乱导致解压崩溃
	virtual bool VerifyFile( const char * name,const char * pData,unsigned long nLen,IMicroClientSink* sink,unsigned long userdata_1,unsigned long userdata_2 ) = 0;
};

namespace rkt 
{
	struct WD_REPORT_NODE
	{
		ulong fid;
		ulong size;
	};
	typedef producer_consumer_queue<WD_REPORT_NODE>	DECODE_REPORT_LIST;

	struct IDecodeMnanger
	{
		// 设置文件操作目录
		virtual void SetDataDir(std::string baseDir) = 0;

		// 添加解码数据块
		virtual	void AddDecode( const char * path,void * pContent,int nLen,IMicroClientSink* sink,unsigned long userdata_1,unsigned long userdata_2,bool save=true,bool decompress=true  ) = 0;
		
		// 添加解码数据块
		virtual void AddDecompress(IAsynStreamSink* sink, void * pContent, int cLen, int fLen, ulong flag, ulong seek,  long userdata) = 0;
		
		// 主线程处理回调
		virtual	void HandleFinished() = 0;

		// 增加上报资源
		virtual void AddReportInfo(const char * path,int nLen ) = 0;

		// 取得上报列表
		virtual DECODE_REPORT_LIST* GetReportInfo() = 0;

		// 删除文件
		virtual bool DelFile(const char* name) = 0;

		// 停掉解码线程
		virtual void StopWork() = 0;
	};	

	RKT_API IDecodeMnanger* getDecodeManager();

	/**
	异步解码线程
	@param 由于解压缩和模型加载可能很花时间，所以我们另起一个线程来做
	*/
	class  DecodeThread : public IDecodeMnanger, public Thread,public IRunnable, public SingletonEx<DecodeThread>
	{
	private:

		class FileWriter : public IAsynFileHandler
		{
		public:
			FileWriter() {}
			virtual ~FileWriter() {}
		public:
			bool SaveFile( const char * name,void * data,int len );
			bool DelFile(const char* name);
		public:
			virtual void HandleReadSuccess(IAsynFile * file,unsigned __int64 pos,const char * data,unsigned long len,long userdata) {file->Release();}
			virtual void HandleWriteSuccess(IAsynFile * file,unsigned __int64 pos,unsigned long len,long userdata) {file->Release();}
			virtual void HandleReadFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long expect_bytes,long userdata) {file->Release();}
			virtual void HandleWriteFailed(IAsynFile * file,unsigned long err,unsigned __int64 pos,unsigned long len,long userdata) {file->Release();}
		};

		struct DECODE_REQ
		{
			std::string			name;
			IMicroClientSink*	sink;
			unsigned long		userdata_1;
			unsigned long		userdata_2;
			void           *	pContent;
			ulong				nLen;
			bool				bSave;
			bool				bDecode;

			DECODE_REQ() { memset( this,0,sizeof(DECODE_REQ) ); }
		};

		struct DECOMPRESS_REQ
		{
			IAsynStreamSink*  sink;			// 回调对象
			long			  userdata;		// 用户数据
			ulong			  flag;			// 加密压缩标志
			void*			  pContent;		// 文件内容
			ulong			  cLen;			// 压缩文件大小
			ulong			  fLen;			// 原始文件大小
			ulong			  seek;			// 加密种子
		};

		typedef producer_consumer_queue<DECODE_REQ>	DECODE_LIST;
		DECODE_LIST		m_decodeList;				// 解码队列
		DECODE_LIST		m_finishList;				// 完成队列
		HANDLE          m_event;					// 事件通知

		typedef producer_consumer_queue<DECOMPRESS_REQ> DECOMPRESS_LIST;
		DECOMPRESS_LIST  m_decompressList;
		DECOMPRESS_LIST  m_decompressedList;

		FileWriter		m_FileWriter;
		std::string		m_strDataDir;

		volatile bool			m_bEable;


	private:		
		DECODE_REPORT_LIST	m_reportList;		

	public:
		DecodeThread()
		{
			m_bEable = true;
			m_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			this->spawn(this);
		}
		virtual ~DecodeThread()
		{
			m_bEable = false;
			::CloseHandle(m_event);
			m_event = NULL;
			StopWork();
		}

		virtual void run();

		virtual void release()	{}	

		virtual void StopWork()
		{
			m_bEable = false;
			this->terminate();			
			DECODE_REQ req;
			while (m_decodeList.pop( req ) )
			{
				if (req.pContent) free(req.pContent);
			}

			DECOMPRESS_REQ node;
			while(m_decompressList.pop(node))
			{
				if (node.pContent) free(req.pContent);
			}

			while( m_finishList.pop( req ) )
			{
				if ( req.pContent )	free( req.pContent );				
			}

			while(m_decompressedList.pop(node))
			{
				if (node.pContent) free(node.pContent);
			}
		}

		virtual void SetDataDir(std::string baseDir) { m_strDataDir = baseDir + "\\";}

		virtual void AddReportInfo( const char * path,int nLen )
		{				
			ulong fid = GetFileID( path,nLen );					
			WD_REPORT_NODE node;
			node.fid = fid;
			node.size = nLen;
			m_reportList.push(node);			
		}

		virtual DECODE_REPORT_LIST* GetReportInfo() { return &m_reportList;}

		// 微端微文件的解码处理
		virtual void AddDecode( const char * path,void * pContent,int nLen,IMicroClientSink* sink,unsigned long userdata_1,unsigned long userdata_2,bool save=true,bool decompress=true  )
		{
			if (!m_bEable) return;

			DECODE_REQ decode;
			decode.name = path;
			decode.sink = sink;
			decode.pContent = malloc(nLen);
			decode.nLen     = nLen;
			decode.userdata_1 = userdata_1;
			decode.userdata_2 = userdata_2;
			decode.bSave      = save;
			decode.bDecode    = decompress;

			memcpy( decode.pContent,pContent,nLen );
			m_decodeList.push( decode );

			// 发出事件通知
			if ( m_event!=NULL/* && m_decodeList.count()==0*/ )
			{
				SetEvent( m_event );
			}

			AddReportInfo( path,nLen );
		}

		// 大端mpk包里的数据处理 cLen 为压缩数据大小， fLen为解压缩后大小， 如果无压缩 clen == flen 切记！
		virtual void AddDecompress(IAsynStreamSink* sink, void * pContent, int cLen, int fLen, ulong flag, ulong seek,  long userdata)
		{
			if (!m_bEable) return;
			DECOMPRESS_REQ node;
			node.sink = sink;
			node.pContent = malloc(cLen);
			node.cLen = cLen;
			node.fLen = fLen;
			node.seek = seek;
			node.flag = flag;
			node.userdata = userdata;
			memcpy(node.pContent,pContent,cLen);
			m_decompressList.push(node);

			if ( m_event!=NULL)
			{
				SetEvent( m_event);
			}
		}
		
		virtual void HandleFinished()
		{
			ulong tick = getTickCount();
			DECODE_REQ req;
			while( m_finishList.pop( req ) )
			{
				if ( req.sink )
				{
					req.sink->OnGetFileData( req.name.c_str(),req.pContent,req.nLen,req.userdata_1,req.userdata_2 );
				}

				if ( req.pContent )
				{
					free( req.pContent );
				}

				if (getTickCount() - tick >15) break;
			}

			DECOMPRESS_REQ node;
			while(m_decompressedList.pop(node))
			{
				if (node.sink)
				{
					rkt::MemoryStream stream((uchar*)node.pContent,node.fLen);
					node.sink->onRequest(&stream,node.userdata);
				}

				if (node.pContent)
				{
					free(node.pContent);
				}

				if (getTickCount() - tick >15) break;
			}
		}
		virtual bool DelFile(const char* name);
	};

};