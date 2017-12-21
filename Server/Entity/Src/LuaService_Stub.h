/*******************************************************************
** 文件名:	LuaService_Stub.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-5
** 版  本:	1.0
** 描  述:	本文件为根据ILuaService接口定义，自动生成的LuaService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "ILuaService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "LuaService.h"


using namespace rkt;
using namespace std;


/** 
   代理对象:
   该代理实现ILuaService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class LuaService_Proxy : public ILuaService
{
public:
	IServiceContainer*		m_pContainer;
	BYTE			m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE

	////// LuaService_Proxy ////////////////////////////////////////////////////////////////////

	LuaService_Proxy() : m_nServiceType(GAMELOGIC_SERVICE_LUA),m_pContainer(0)
	{
	}

	~LuaService_Proxy()
	{
	}

	// 取得游戏逻辑服务类型
	BYTE getServiceType()
	{
		return m_nServiceType;
	}


	////// ILuaService ////////////////////////////////////////////////////////////////////
	/** 执行一个lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void LoadLuaFile(const char* szLuaFileName) 
    {
	    OBUF(t_data);
	    t_data << szLuaFileName;

		BUILD_MSG_BUFFER( ILuaService::LoadLuaFile,t_data);

	    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

   //////////////////////////////////////////////////////////////////////////////////
	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@return  成功：返回true,失败：false
	*/
	virtual void RunMemoryLua(const char* pLuaData, int nDataLen)
   {
	   OBUF(t_data);
	   t_data << pLuaData<<nDataLen;

	   BUILD_MSG_BUFFER( ILuaService::RunMemoryLua,t_data);

	   m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   };

   //////////////////////////////////////////////////////////////////////////////////
	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  成功：返回true,失败：false
	*/
	virtual void RunLuaFunction(const char* szFunName, CLuaParam * pInParam, int nInNum/*, CLuaParam * pOutParam, int nOutNum*/)
   {
	   OBUF(t_data);
	   t_data << szFunName;	// LUA函数
	   t_data << nInNum;	// 输入参数数量
	   if (nInNum > 0)
	   {
		   for (int i=0; i<nInNum; ++i)
		   {
			   t_data << pInParam[i];
		   }
	   }

	   BUILD_MSG_BUFFER( ILuaService::RunLuaFunction,t_data);

	   m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   };

	/** 是否存在某个函数
	*/
	virtual bool IsFunction(const char * szFunName)
    {
	    OBUF(t_data);
	    t_data << szFunName;

		BUILD_MSG_BUFFER( ILuaService::IsFunction,t_data);

	    rkt::obuf resultBuf;
	    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
		RETURN_FROM_MSG_BUF(bool);
		return false;
    };


	/** 刷新lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void UpdateLuaFile(const char* szLuaFileName)
    {
        OBUF(t_data);
        t_data << szLuaFileName;

        BUILD_MSG_BUFFER( ILuaService::UpdateLuaFile,t_data);

	    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }


	// 释放
	virtual void release()
    {
	   OBUF(t_data);

	   BUILD_MSG_BUFFER( ILuaService::release,t_data);

	   m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class LuaService_Stub
{
public:
	BYTE			  m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	LuaService_Proxy  m_proxy;
	LuaService *      m_real_service;

public:
	LuaService_Stub(LuaService * pReal) : m_real_service(pReal),m_nServiceType(0){
	}

	// 启动
	virtual bool on_start( IServiceContainer* pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		int* p = (int*)data;
		int mapid = *p; p++;
		int scene = *p; p++;

		return m_real_service->on_start(scene);
	}

	// 停止
	virtual bool on_stop()
	{
		bool bResult = m_real_service->on_stop();
        return bResult;
	}

    // 初始化
    bool on_init()
    {
        return m_real_service->on_init();
    }

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		if ( pMsg->context_len==0 || pMsg->context==0 )
			return false;

		ibuffer in(pMsg->context,pMsg->context_len);

		////////////////////////////////////////////////////////////////////////////
		// 以下自动生成的消息解包，你需要检查
		
		if ( TEST_SAME_FUNCTION(ILuaService::release) )
		{
            in.skip<BYTE>();
			m_real_service->release();
			return true;
		}

		if ( TEST_SAME_FUNCTION(ILuaService::LoadLuaFile) )
		{
            in.skip<BYTE>();
			char * szLuaFileName;
			in >> szLuaFileName;
			m_real_service->LoadLuaFile(szLuaFileName);
			return true;
		}

		if ( TEST_SAME_FUNCTION(ILuaService::RunMemoryLua) )
		{
            in.skip<BYTE>();
			char * pLuaData;int nDataLen;
			in >> pLuaData>>nDataLen;
			m_real_service->RunMemoryLua(pLuaData,nDataLen);
			return true;
		}

		if ( TEST_SAME_FUNCTION(ILuaService::RunLuaFunction) )
		{
            in.skip<BYTE>();
			char * szFunName;int nInNum; //CLuaParam pInParam;CLuaParam pOutParam;int nOutNum;
			in >> szFunName>>nInNum;
			CLuaParam pInParam[20];
			if (nInNum > 0)
			{
				for (int i=0; i<nInNum; ++i)
				{
					in >> pInParam[i];
				}
			}
			m_real_service->RunLuaFunction(szFunName,pInParam,nInNum);

			return true;
		}

		if ( TEST_SAME_FUNCTION(ILuaService::IsFunction) )
		{
            in.skip<BYTE>();
			char * szFunName;
			in >> szFunName;
			m_real_service->IsFunction(szFunName);
			return true;
		}

        if ( TEST_SAME_FUNCTION(ILuaService::UpdateLuaFile) )
        {
            in.skip<BYTE>();
            char * szLuaFileName;
            in >> szLuaFileName;
            m_real_service->UpdateLuaFile(szLuaFileName);
            return true;
        }
        
        ErrorLn(__FUNCTION__":can not found function name:" << MsgFunctionName<SERVICE_MESSAGE>()(pMsg) );
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

        m_proxy.m_pContainer = 0;

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//extern "C" __declspec(dllexport) IServiceStub * CreateLuaService()
//{
//	return new LuaService_Stub(new LuaService);
//}