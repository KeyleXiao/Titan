/*******************************************************************
** 文件名:	DBEngineService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-25
** 版  本:	1.0
** 描  述:	DBEngine服务			
********************************************************************/

#pragma once



#ifndef __SERVER_SERVERGLOBAL_SRC_DBENGINESERVICE_STUB_H__
#define __SERVER_SERVERGLOBAL_SRC_DBENGINESERVICE_STUB_H__


#include "IServiceContainer.h"
#include "buffer.h"
#include "DBEngineService.h "
#include "IDBProxy.h"
#include "IFramework.h"

using namespace rkt;
using namespace std;


/** 
   代理对象:
   该代理实现IDBEngineService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class CDBEngineService_Proxy : public IDBEngineService
{
public:
	SERVICE_PTR    m_pContainer;

    CDBEngineService_Proxy()
		: m_pContainer(0)
	{
	}

	virtual ~CDBEngineService_Proxy()
	{
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	/** 注册处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool registerHandler(IDBRetSink* pSink, const char* pszDesc)
	{
		if(pSink == NULL)
        {
            return false;
        }

        // 创建DB代理
        IDBProxy * pProxy = gServerGlobal->getFramework()->create_db_proxy();
        if(pProxy == NULL)
        {
            return false;
        }

        if ( !pProxy->init( pSink, pszDesc) )
        {
            delete pProxy;
            pProxy = 0;
            return false;
        }        

        return true;
	}
	

	/** 注销注册消息处理者
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool unregisterHandler(IDBRetSink* pSink)
	{
		if(pSink == NULL)
        {
            return false;
        }

        IDBProxy** pProxyInfo = CAST_TYPE(IDBProxy**, pSink->GetProxyInfoPtr());
        if(pProxyInfo == NULL)
        {
            return false;
        }
        IDBProxy* pProxy = *pProxyInfo;
        if(pProxy == NULL)
        {
            return false;
        } 
        pProxy->closeProxy(pSink);
        pProxy->release();

        return true;
	}

	/** 执行一个存储过程
	@param   dwCmdID		：cmdid
	@param   dwQueueIndex	: 异步队列索引号
	@param   pszInData		：输入数据
	@param   nInDataLen		：输入数据长度
	@param   pDBRetSink		：结果返回接口	
	@param   nGameWorldID	: 向何个游戏世界数据库执行，若为公共区,必填游戏世界id
	@return  		
	@retval buffer 
	*/
	virtual bool executeSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink, int nGameWorldID = 0) 
	{
        if(pDBRetSink == NULL)
        {
            return false;
        }

        IDBProxy** pProxyInfo = CAST_TYPE(IDBProxy**, pDBRetSink->GetProxyInfoPtr());
        if(pProxyInfo == NULL)
        {
            return false;
        }
        IDBProxy* pProxy = *pProxyInfo;
        if(pProxy == NULL)
        {
            return false;
        }        
        pProxy->addRef();

		obuf256 t_data;
		t_data <<dwCmdID<<dwQueueIndex<<pDBRetSink<<pProxy<<nGameWorldID<<nInDataLen;
		t_data.push_back(pszInData, nInDataLen);

		BUILD_MSG_BUFFER(IDBEngineService::executeSP,t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

	/** 输出队列
	*/
    virtual void  outputQueue()
    {
        BUILD_MSG_CONTEXT_VOID( IDBEngineService::outputQueue );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }
};



/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class CDBEngineService_Stub : public IServiceStub
{
	CDBEngineService_Proxy			m_proxy;
	CDBEngineService *				m_real_service;
    obuf							m_return_buffer;

public:
	CDBEngineService_Stub(CDBEngineService * pReal)
		: m_real_service(pReal)
	{
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		return m_real_service->Start();
	}

	// 停止
	virtual bool on_stop()
	{
		m_real_service->Stop();
		return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		////////////////////////////////////////////////////////////////////////////
		// 以下自动生成的消息解包，你需要检查
		if ( TEST_SAME_FUNCTION(IDBEngineService::outputQueue) )
        {
            m_real_service->outputQueue();
            return true;
        }

		if ( TEST_SAME_FUNCTION(IDBEngineService::executeSP) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			DWORD dwCmdID=0; DWORD dwQueueIndex =0; int nInDataLen = 0; IDBRetSink * pDBRetSink=NULL; IDBProxy* pProxy =NULL;int nGameWorldID = 0;
			in >> dwCmdID >> dwQueueIndex >> pDBRetSink >> pProxy >> nGameWorldID >> nInDataLen;

            if(pProxy==NULL)
            {
                *resultBuf << false;
                return true;
            }
            // Sink对象已不存在时,主动release
            if(pProxy->getHandler() == NULL)
            {
                *resultBuf << false;
                pProxy->release();
                return true;
            }

            // 等执行返回时再来Release，否则很容易回调到NULL而导致非法
            *resultBuf << m_real_service->executeSP(dwCmdID, dwQueueIndex, in.current(), nInDataLen, pProxy->toSink(), nGameWorldID);
			return true;
		}

		ErrorLn("CDBEngineService_Stub::handle_message() can not found function name:" );
		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
		if ( m_real_service )
		{
			m_real_service->release();
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateDBEngineService()
{
	return new CDBEngineService_Stub(new CDBEngineService);
}


#endif  // __SERVER_SERVERGLOBAL_SRC_DBENGINESERVICE_STUB_H__
