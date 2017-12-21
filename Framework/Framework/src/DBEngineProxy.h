/*******************************************************************
** 文件名:	DBEngineProxy.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-29
** 版  本:	1.0
** 描  述:	DBEngine回调			
********************************************************************/

#ifndef __FRAMEWORK_FRAMEWORK_SRC_DBENGINEPROXY_H__
#define __FRAMEWORK_FRAMEWORK_SRC_DBENGINEPROXY_H__


#include "CallbackProxy.h"
#include "Simple_Atom_Lock.h"
#include "IDBRetSink.h"
#include "IDBProxy.h"
//#define DBENGINE_PROXY_DEBUG

// DBEngine回调	
class DBEngineProxy: public IDBProxy, public CallbackProxy<IDBRetSink>
{
public:

#ifdef DBENGINE_PROXY_DEBUG
    static volatile LONG    m_nCount;
    LONG  m_nIndex;

    DBEngineProxy() : m_nIndex (0)
    {
        InterlockedIncrement(&m_nCount);
        m_nIndex = m_nCount;

    }

    ~DBEngineProxy()
    { 
        TraceLn(__FUNCTION__":"<< m_nIndex << ","<< m_strDebugInfo.c_str()<<",addr="<< reinterpret_cast<INT_PTR>(this) );
    }

#endif

    virtual bool init(IDBRetSink * pSink, const char * szDebugInfo )
    {
        void **ppProxyInfo = pSink->GetProxyInfoPtr();
        if ( ppProxyInfo == 0 )
        {
            return false;
        }

        DBEngineProxy* pProxy = *(DBEngineProxy**)ppProxyInfo;
        if ( pProxy != 0 )
        {
            return false;
        }
        
        if ( !CallbackProxy<IDBRetSink>::Init( pSink,szDebugInfo ))
        {
            return false;
        }
        
        *ppProxyInfo = this;

#ifdef DBENGINE_PROXY_DEBUG
        TraceLn(__FUNCTION__": "<< m_nCount << ","<< m_strDebugInfo.c_str()<<",addr="<< reinterpret_cast<INT_PTR>(this) );
#endif

        return true;
    }

    virtual IDBProxy * closeProxy(IDBRetSink * pSink)
    {
        if(pSink == NULL)
        {
            return 0;
        }

        void **ppProxyInfo = pSink->GetProxyInfoPtr();
        if ( ppProxyInfo==0 )
        {
            return 0;
        }

        DBEngineProxy* pProxy = *(DBEngineProxy**)ppProxyInfo;
        if ( pProxy==0 )
        {
            return 0;
        }

        pProxy->m_pRealHandler = 0;
        return pProxy;
    }
    	

	/** 增加引用计数
	@param pSink 数据库请求返回回调接口
	*/
    virtual void addRef()
    {
        CallbackProxy<IDBRetSink>::AddRef();
    }

    /** 释放
	@param pSink 数据库请求返回回调接口
	*/
    virtual void release()
    {
        CallbackProxy<IDBRetSink>::Release();
    }
    
	/** 获取Sink对象
	*/
    virtual IDBRetSink * getHandler(void)
    {
        return m_pRealHandler;
    }

	/** 转成Sink对象
	*/
    virtual IDBRetSink * toSink(void) 
    {
        return this;
    }


	////////IDBRetSink///////////////////////////////////////////////////////
	/** 数据库请求返回回调方法
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
	{
        // 防止当执行到此处时，有可能其他线程又执行了一次Release导致本对象被执行析构
        AddRef();
        // 这里来release，是要保证DB引擎回调时能正常执行，不会由于这个对象为NULL时非法
        Release();


        string strDesc = (pszDBRetDesc == NULL ? "" : pszDBRetDesc);

        // 压入数据
        size_t nBuffSize = sizeof(IDBRetSink *) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(string::size_type) + strDesc.size() + sizeof(int) + nOutLen;

        BUILD_MSG_BUFFER_LEN(DBEngineProxy::OnReturn, nBuffSize);
        // 如果其他线程将这个对象清理了，就不用再执行下去
        if (m_pRealHandler == 0) {
            free(pMsg);
            Release();
            return;
        }
        memcpy(pMsg, m_pMessage, sizeof(SERVICE_MESSAGE));

        t_data << pRealDBRetSink << nCmdID << nDBRetCode << nQueueIndex << strDesc << nOutLen;
        t_data.push_back(pOutData, nOutLen);

        m_pContainer->recv_message(pMsg, false);
	}


    virtual void Do(SERVICE_MESSAGE * pMsg)
    {
        do 
        {
            // 已删除
            if ( m_pRealHandler==0 ) {
                break;
            }

            ibuffer in( pMsg->context,pMsg->context_len );

            IDBRetSink * pRealDBRetSink = NULL;
            int nCmdID = 0;
            int nDBRetCode = 0;
            string strDBRetDesc;
            int nQueueIndex = 0;
            int nOutLen = 0;

            in >> pRealDBRetSink >> nCmdID >> nDBRetCode >> nQueueIndex >> strDBRetDesc >> nOutLen;
            char * pContext = in.current();

            // 弹出事件内容
            if(m_pRealHandler)
            {
                m_pRealHandler->OnReturn(pRealDBRetSink, nCmdID, nDBRetCode, const_cast<char*>(strDBRetDesc.c_str()), nQueueIndex, pContext, nOutLen);
            }
        } while (0);


        free(pMsg);
        Release();
    }
};

#ifdef DBENGINE_PROXY_DEBUG
volatile LONG DBEngineProxy::m_nCount = 0;
#endif

#endif  // __FRAMEWORK_FRAMEWORK_SRC_DBENGINEPROXY_H__


