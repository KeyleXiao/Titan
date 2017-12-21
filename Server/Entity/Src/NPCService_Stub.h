/*******************************************************************
** 文件名:	NPCService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据INPCService接口定义，自动生成的NPCService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "INPCService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "NPCService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现INPCService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class NPCService_Proxy : public INPCService
{
public:
    BYTE			m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
    IServiceContainer*	m_pContainer;
    NPCService *      m_real_service;

    NPCService_Proxy() : m_nServiceType(GAMELOGIC_SERVICE_MONSTER), m_real_service(0),m_pContainer(0)
    {
    }

    ~NPCService_Proxy() {
    }

    // 取得游戏逻辑服务类型
    BYTE getServiceType()
    {
        return m_nServiceType;
    }

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////
    virtual UID  createNPC(int id, Vector3 loc, CreateNpcContext context)
    {
        if (m_pContainer == 0 || m_real_service == NULL)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return INVALID_UID;
        }

        UID uid = m_real_service->genUID();
        if (uid == INVALID_UID)
        {
            return uid;
        }

        BUILD_TYPE_MSG_CONTEXT_4(INPCService::createNPC, int, id, UID, uid, Vector3, loc, CreateNpcContext, context);
        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);

        return uid;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  destroyNPC(UID uid)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return false;
        }

        BUILD_TYPE_MSG_CONTEXT_1(INPCService::destroyNPC, UID, uid);

        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
        return bool();     // 请检查默认返回值是否正确
    };

    //////////////////////////////////////////////////////////////////////////////////
    // 获取NPC
    virtual NPC_PTR findNPC(UID uid)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return NPC_PTR(0);
        }

        return m_real_service ? m_real_service->findNPC(uid) : NPC_PTR(0);
    }

    //////////////////////////////////////////////////////////////////////////////////
    virtual string  getName(UID uid)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return string();
        }

        return m_real_service ? m_real_service->getName(uid) : "";
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual int  getSceneID()
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return -1;
        }

        return m_real_service ? m_real_service->getSceneID() : 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual Vector3  getLocation(UID uid)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return Vector3();
        }

        return m_real_service ? m_real_service->getLocation(uid) : Vector3();
    };

    virtual void  setLocation(UID uid, Vector3 vLoc)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

        BUILD_TYPE_MSG_CONTEXT_2(INPCService::setLocation, UID, uid, Vector3, vLoc);
        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    };

    virtual Vector3  getAngle(UID uid)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return Vector3();
        }

        return m_real_service ? m_real_service->getAngle(uid) : Vector3();
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  handleMessage(UID uid, void *pData, int nLen)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

        int nTotalLen = sizeof(uid) + sizeof(m_nServiceType) + nLen;
        BUILD_MSG_BUFFER_LEN(INPCService::handleMessage, nTotalLen);
        t_data << m_nServiceType << uid;
        t_data.push_back(pData, nLen);

        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    };

    // 群体怪物收到消息
    virtual void batchHandleMessage(UID* uidArray, DWORD dwArrayCount, BYTE byKeyModule, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData = 0, int nDataLen = 0, int nOneDataLen = 0)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }
        if (uidArray == nullptr || dwArrayCount == 0)
            return;
        if (nDataLen != nOneDataLen)
        {
            if (nDataLen < nOneDataLen)
            {
                ErrorLn(__FUNCTION__": message total length less than single message length! KeyModule=" << byKeyModule << ",PartID=" << byPartID << ",Action=" << byKeyAction);
                return;
            }
            if (nOneDataLen != 0 && (dwArrayCount != (nDataLen / nOneDataLen)))
            {
                ErrorLn(__FUNCTION__": message total length is invalid! KeyModule=" << byKeyModule << ",PartID=" << byPartID << ",Action=" << byKeyAction);
                return;
            }
        }

        OBUF(t_data);
        t_data << dwArrayCount << byKeyModule << byPartID << byKeyAction << nDataLen << nOneDataLen;

        t_data.push_back(uidArray, (sizeof(UID) * dwArrayCount));

        if (pszData && nDataLen > 0)
        {
            t_data.push_back(pszData, nDataLen);
        }

        BUILD_MSG_BUFFER(INPCService::batchHandleMessage, t_data);
        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    }

   virtual void handleReplaceableMessage(UID uid, unsigned long ulKey,void * pData,int nLen)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

	   int nTotalLen = sizeof(uid) + sizeof(m_nServiceType) + nLen;
	   BUILD_MSG_BUFFER_LEN( INPCService::handleMessage,nTotalLen );
	   t_data<<m_nServiceType << uid;
	   t_data.push_back( pData,nLen );

       m_pContainer->post_replaceable_message(ulKey, pMsg,nMsgLen);
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getPropertyType( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return string();
        }

		return m_real_service ? m_real_service->getPropertyType( uid,prop_id ) : "";
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getPropertyFlag( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return int();
        }

		return m_real_service ? m_real_service->getPropertyFlag( uid,prop_id ) : 0;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  getProperty_Bool( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

		return m_real_service ? m_real_service->getProperty_Bool( uid,prop_id ) : false;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_Bool( UID uid,int prop_id,bool value )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

	   BUILD_TYPE_MSG_CONTEXT_3( INPCService::setProperty_Bool, UID, uid,int, prop_id,bool ,value);
	   m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getProperty_Integer( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return int();
        }

		return m_real_service->getProperty_Integer( uid,prop_id );
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_Integer( UID uid,int prop_id,int value )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

		// 同一个线程
		if ( gServerGlobal->getFramework()->get_call_service()==m_pContainer->get_id() )
		{
			return m_real_service->setProperty_Integer( uid,prop_id,value );
		}

	   BUILD_TYPE_MSG_CONTEXT_3( INPCService::setProperty_Integer, UID, uid,int, prop_id,int ,value);
	   m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  addProperty_Integer( UID uid,int prop_id,int delta )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

		// 同一个线程
		if ( gServerGlobal->getFramework()->get_call_service()==m_pContainer->get_id() )
		{
			return m_real_service ? m_real_service->addProperty_Integer( uid,prop_id,delta ) : false;
		}

	   BUILD_TYPE_MSG_CONTEXT_3( INPCService::addProperty_Integer, UID, uid,int, prop_id,int ,delta);
	   m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getProperty_String( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return string();
        }

		return m_real_service->getProperty_String( uid,prop_id );
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_String( UID uid,int prop_id,string value )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

	    OBUF(t_data);
	    t_data << uid<<prop_id<<value;

		BUILD_MSG_BUFFER( INPCService::setProperty_String,t_data);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getProperty_custom( UID uid,int prop_id )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return string();
        }

	    return m_real_service->getProperty_custom( uid,prop_id );
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_custom( UID uid,int prop_id,string value )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

	    OBUF(t_data);
	    t_data << uid<<prop_id<<value;

		BUILD_MSG_BUFFER( INPCService::setProperty_custom,t_data);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	    return bool();     // 请检查默认返回值是否正确
    };


   //////////////////////////////////////////////////////////////////////////////////
   virtual float  getSight( UID uid )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return float();
        }

		return m_real_service ? m_real_service->getSight( uid ) : 0.0f;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  setSight( UID uid,float radii )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

		BUILD_TYPE_MSG_CONTEXT_2( INPCService::setSight,UID, uid,float, radii);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  isHide( UID uid )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return bool();
        }

	    BUILD_TYPE_MSG_CONTEXT_1( INPCService::isHide, UID, uid);

	    rkt::obuf resultBuf;
	    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	    RETURN_FROM_MSG_BUF(bool);

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  setHide( UID uid,bool state )
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

		BUILD_TYPE_MSG_CONTEXT_2( INPCService::setHide, UID, uid,bool, state);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
    };

   virtual void AddEnmityValue(UID uidMonster, UID uidDes, int nVal)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

		BUILD_TYPE_MSG_CONTEXT_3( INPCService::AddEnmityValue, UID ,uidMonster, UID, uidDes, int, nVal);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
   }

   virtual void AddBroadEnmityValue(UID uidMonster, UID uidDes, int nVal)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

		BUILD_TYPE_MSG_CONTEXT_3( INPCService::AddBroadEnmityValue,UID ,uidMonster, UID, uidDes, int, nVal);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
   }

   virtual void SetEnmityValue(UID uidMonster, UID uidDes, int nVal)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

		BUILD_TYPE_MSG_CONTEXT_3( INPCService::SetEnmityValue,UID ,uidMonster, UID, uidDes, int, nVal);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
   }

   virtual void changeState(UID uidMonster, int nState)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

		BUILD_TYPE_MSG_CONTEXT_2( INPCService::changeState,UID ,uidMonster,int, nState);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
   }
   virtual int getState(UID uidMonster)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return int();
       }

	   return m_real_service ? m_real_service->getState(uidMonster) : 0;
   }

   /////////////////////// IBuffPart ///////////////////////////////////////////////
   // 给群体怪物带检测指定BUFFID和BUFF类型来增加BUFF
   virtual bool BatchCheckAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwCheckBuff = 0, DWORD dwCheckLevel = 0, DWORD dwCheckBuffType = 0, DWORD dwTime = 0, void *pBuffContext = NULL, int nContextLen = 0, bool bExistAdd =true)
   {
       if (m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }
       if (uidArray == nullptr || dwArrayCount == 0)
           return bool();

       OBUF(t_data);
       t_data << dwArrayCount << dwBuffID << dwBuffLevel << uidAdd << dwCheckBuff  << dwCheckLevel << dwCheckBuffType << dwTime << nContextLen << bExistAdd;
       t_data.push_back(uidArray, (sizeof(UID) * dwArrayCount));

       if (pBuffContext && nContextLen > 0)
       {
           t_data.push_back(pBuffContext, nContextLen);
       }

       BUILD_MSG_BUFFER(INPCService::BatchCheckAddBuff, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
       return bool();
   }

   // 给群体怪物带检测指定BUFFID和BUFF类型来移除BUFF
   virtual bool BatchCheckRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove, DWORD dwCheckBuff = 0, DWORD dwCheckLevel = 0, DWORD dwCheckBuffType=0, bool bExistRemove=true)
   {
       if (m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }
       if (uidArray == nullptr || dwArrayCount == 0)
           return bool();

       OBUF(t_data);
       t_data << dwArrayCount << dwBuffID << dwBuffID << uidAdd << uidRemove << dwCheckBuff << dwCheckLevel << dwCheckBuffType << bExistRemove;
       t_data.push_back(uidArray, (sizeof(UID) * dwArrayCount));


       BUILD_MSG_BUFFER(INPCService::BatchCheckRemoveBuff, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
       return bool();
   }

   // 给群体怪物增加BUFF
   virtual bool BatchAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime = 0, void *pBuffContext = NULL, int nContextLen = 0)
   {
       if (m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }
        if (uidArray == nullptr || dwArrayCount == 0)
            return bool();

       OBUF(t_data);
       t_data << dwArrayCount << dwBuffID << dwBuffLevel << uidAdd << dwTime << nContextLen;
       t_data.push_back(uidArray, (sizeof(UID) * dwArrayCount));

       if (pBuffContext && nContextLen > 0)
       {
           t_data.push_back(pBuffContext, nContextLen);
       }

       BUILD_MSG_BUFFER(INPCService::BatchAddBuff, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
       return bool();
   }

   // 给群体怪物移除BUFF
   virtual bool BatchRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove)
   {
       if (m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }
        if (uidArray == nullptr || dwArrayCount == 0)
            return bool();

       OBUF(t_data);
       t_data << dwArrayCount << dwBuffID << uidAdd << uidRemove;
       t_data.push_back(uidArray, (sizeof(UID) * dwArrayCount));

       BUILD_MSG_BUFFER(INPCService::BatchRemoveBuff, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
       return bool();
   }

   virtual bool AddBuff(UID uidMonster, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

	   OBUF(t_data);
	   t_data <<uidMonster <<dwBuffID<<dwBuffLevel<<uidAdd<<dwTime<<nContextLen;
	   if (pBuffContext && nContextLen > 0)
	   {
		   t_data.push_back(pBuffContext, nContextLen);
	   }

	   BUILD_MSG_BUFFER( INPCService::AddBuff,t_data);
	   m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	   return bool();
   }
   virtual bool RemoveBuff(UID uidMonster,  DWORD dwBuffID, UID uidAdd, UID uidRemove)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

		BUILD_TYPE_MSG_CONTEXT_4( INPCService::RemoveBuff, UID, uidMonster,  DWORD, dwBuffID, UID, uidAdd, UID ,uidRemove);
	    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK );
	   return bool();
   }
   virtual bool IsExistBuff(UID uidMonster, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
   {
	   if (uidAdd == INVALID_UID)
	   {
		   IBuffServer* pBuffServer = gServerGlobal->getBuffServer();
		   int nAccQueryIndex = -1;
		   // 如果配了查询加速 查询是线程安全的
		   if (pBuffServer != NULL && (nAccQueryIndex = pBuffServer->getAccQueryIndex(dwBuffID, dwBuffLevel)) >= 0)
		   {
			   return m_real_service ? m_real_service->IsExistBuff(uidMonster, dwBuffID, dwBuffLevel, uidAdd) : false;
		   }
	   }

       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

	   SERVICE_PTR pRunning = gServerGlobal->getFramework()->get_running_service();
	   if (pRunning != 0 && pRunning->get_id() != m_pContainer->get_id() && uidAdd == INVALID_UID)
	   {
		   //EmphasisLn("check buff exist,buffid=" << dwBuffID);
	   }

	   BUILD_TYPE_MSG_CONTEXT_4( INPCService::IsExistBuff,UID, uidMonster,  DWORD, dwBuffID, DWORD ,dwBuffLevel, UID, uidAdd);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(bool);
	   return bool();
   }
   virtual bool IsExistBuffType(UID uidMonster, DWORD dwBuffType)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

	   //BUILD_TYPE_MSG_CONTEXT_2( INPCService::IsExistBuffType,UID, uidMonster,  DWORD, dwBuffType);

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   //RETURN_FROM_MSG_BUF(bool);
    //   return bool();
       return m_real_service->IsExistBuffType(uidMonster, dwBuffType);
   }
   virtual DWORD GetBuffLevel(UID uidMonster, DWORD dwBuffID, UID uidAdd)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return DWORD();
       }

	   BUILD_TYPE_MSG_CONTEXT_3( INPCService::GetBuffLevel,UID, uidMonster, DWORD, dwBuffID, UID, uidAdd);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(DWORD);
	   return 0;
   }
   virtual int GetLeftTime(UID uidMonster, DWORD dwBuffID, UID uidAdd)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return int();
       }

	   BUILD_TYPE_MSG_CONTEXT_3( INPCService::GetLeftTime,UID, uidMonster, DWORD, dwBuffID, UID, uidAdd);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(int);
	   return -1;
   }
   virtual void SetAccumulateBuff(UID uidMonster, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL ,int nContextLen=0)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

       OBUF(t_data);
       t_data <<uidMonster<<dwBuffID<<uidAdd<<bOverlay<<dwTime<<nContextLen;
       if (pBuffContext && nContextLen > 0)
       {
           t_data.push_back(pBuffContext, nContextLen);
       }

       BUILD_MSG_BUFFER( INPCService::SetAccumulateBuff,t_data);
       m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   virtual bool RemoveBuffByIndex(UID uidMonster,DWORD dwIndex)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

	   BUILD_TYPE_MSG_CONTEXT_2( INPCService::RemoveBuffByIndex,UID ,uidMonster,DWORD, dwIndex);
	   m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);

	   return false;
   }

   // 传送
   virtual bool Transport(UID uidMonster, Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return false;
       }

	   BUILD_TYPE_MSG_CONTEXT_6( INPCService::Transport,UID ,uidMonster, Vector3 ,newLoc, Vector3, rotation, bool ,bTransport, int ,nSceneID, int, nReason);
	   return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   virtual bool  MoveTo(UID uidMonster, Vector3 &loc)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return false;
       }

	   BUILD_TYPE_MSG_CONTEXT_2( INPCService::MoveTo,UID ,uidMonster, Vector3 ,loc);
	   return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   virtual bool chooseTarget( UID uid, int nTargetFilter, UID uidTarget )
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return false;
       }

       return m_real_service->chooseTarget( uid,nTargetFilter,uidTarget );
   };

   virtual int getPKType( UID uid )
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return 0;
       }

       return m_real_service->getPKType( uid );
   };

   virtual int getPKState( UID uid )
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return 0;
       }

       return m_real_service->getPKState( uid );
   };

   virtual void forceEndPK( UID uid )
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }
       if (m_real_service)
       {
           m_real_service->forceEndPK(uid);
           return;
       }
   
       BUILD_TYPE_MSG_CONTEXT_VOID( INPCService::forceEndPK);
       m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   };

   virtual int getPKCamp(UID uid)
   {
	   if (m_pContainer == 0)
	   {
		   WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
		   return 0;
	   }

	   return m_real_service->getPKCamp(uid);
   };
   

   // 通知指定阵营当前野怪点野怪的生死状况,仅用于隐形实体
   virtual void msgToSpecCampWildInfo(UID uidMonster, int nCamp)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

	   BUILD_TYPE_MSG_CONTEXT_2( INPCService::msgToSpecCampWildInfo,UID ,uidMonster, int ,nCamp);
	   m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   // npc是否对camp可见
   virtual bool isInsight(UID uid, int camp)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return bool();
       }

	   BUILD_TYPE_MSG_CONTEXT_2( INPCService::isInsight,UID, uid, int, camp);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(bool);
       return bool();
   }

   virtual int getWarID(UID uid)
   {
	   BUILD_TYPE_MSG_CONTEXT_1( INPCService::getWarID,UID, uid);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(int);

       return int();     // 请检查默认返回值是否正确
   }

   virtual BuffTypeList getBuff(UID uid, DWORD dwBuffType)
   {
	   BUILD_TYPE_MSG_CONTEXT_2( INPCService::getBuff,UID, uid,DWORD ,dwBuffType);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(BuffTypeList);
       return BuffTypeList();     // 请检查默认返回值是否正确
   }

   virtual int getMonsterArray(int nMonsterID, int nMonsterCamp, UID *pArray, int nArraySize)
   {
	   BUILD_TYPE_MSG_CONTEXT_4( INPCService::getMonsterArray,int ,nMonsterID, int ,nMonsterCamp, UID *,pArray, int ,nArraySize);

	   rkt::obuf resultBuf;
	   m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	   RETURN_FROM_MSG_BUF(int);
       return int();     // 请检查默认返回值是否正确
   }

   // 移除所有NPC
   virtual void destroyAll()
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

	   BUILD_TYPE_MSG_CONTEXT_VOID( INPCService::destroyAll);
	   m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   virtual void sendDynamicObstacleToMovePart(UID uid, void * pObstacleInfo, size_t nLen) 
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return ;
       }

        if (pObstacleInfo == nullptr || nLen == 0)
            return;

       OBUF(t_data);
       t_data << uid << nLen;
       t_data.push_back(pObstacleInfo, nLen);

       BUILD_MSG_BUFFER(INPCService::sendDynamicObstacleToMovePart, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
       return ;     
   }


   // 添加效果 返回效果ID记录
   virtual	int mobaGoodsAddEffect(UID uid, int nEffectID,void * pAddContext, size_t len)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return 0;
       }

       if (pAddContext == nullptr || len == 0)
           return 0;

       OBUF(t_data);
       t_data <<uid<<nEffectID << len;
       t_data.push_back(pAddContext, len);
       BUILD_MSG_BUFFER(INPCService::mobaGoodsAddEffect, t_data);

       rkt::obuf resultBuf;
       m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
       RETURN_FROM_MSG_BUF(int);
       return int();     // 请检查默认返回值是否正确    
   }

   // 移除效果
   virtual	void mobaGoodsRemoveEffect(UID uid,int nID)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

       BUILD_TYPE_MSG_CONTEXT_2( INPCService::mobaGoodsRemoveEffect, UID, uid, int ,nID);
       m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   // 实体通知主人
   virtual void entityNotifyMaster(UID uid, EntityBelongContext context)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

       BUILD_TYPE_MSG_CONTEXT_2( INPCService::entityNotifyMaster, UID, uid, EntityBelongContext,context);
       m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
   }

   virtual int  getMapID(UID uid)
   {
       if (m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return -1;
       }

       return m_real_service ? m_real_service->getMapID(uid) : INVALID_MAPID;
   };

   virtual void shift(UID uidMonster, void *pData,int nDataLen)
   {
       if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

       OBUF(t_data);
       t_data << uidMonster << nDataLen;
       if (pData && nDataLen > 0)
       {
           t_data.push_back(pData, nDataLen);
       }

       BUILD_MSG_BUFFER( INPCService::shift, t_data);
       m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  release(  )
    {
        m_real_service = NULL;

        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

	   BUILD_TYPE_MSG_CONTEXT_VOID( INPCService::release);
	   m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class NPCService_Stub
{
public:
	BYTE			  m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	NPCService_Proxy  m_proxy;
	NPCService *      m_real_service;


public:
	NPCService_Stub(NPCService * pReal) : m_real_service(pReal), m_nServiceType(0){
	}

	// 启动
	virtual bool on_start( IServiceContainer* pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;
		m_proxy.m_real_service = m_real_service;

		int* p = (int*)data;
		int mapid = *p; p++;
		int scene = *p; p++;

		return m_real_service->on_start( mapid,scene );
	}

	// 停止
	virtual bool on_stop()
	{
		bool bResult = m_real_service->on_stop();
        return bResult;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf  )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查		
		if ( TEST_SAME_FUNCTION(INPCService::createNPC) )
		{
			int id;UID uid;Vector3 loc;CreateNpcContext context;
			GET_TYPE_MSG_PARAM_4(int, id,UID, uid,Vector3 ,loc,CreateNpcContext, context );
            m_real_service->doCreate(id, uid, loc, context);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::destroyNPC) )
		{
			UID uid;
			GET_TYPE_MSG_PARAM_1(UID, uid );
			m_real_service->destroyNPC(uid);
			return true;
		}

		if ( TEST_SAME_FUNCTION(INPCService::setLocation) )
		{
			UID uid; Vector3 vLoc;
			GET_TYPE_MSG_PARAM_2(UID ,uid,Vector3 ,vLoc );
			m_real_service->setLocation(uid, vLoc);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::handleMessage) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			in.skip<BYTE>();
			UID uid;
			in >> uid;
			m_real_service->handleMessage(uid,in.current(),in.space());
			return true;
		}
        if (TEST_SAME_FUNCTION(INPCService::batchHandleMessage))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            DWORD dwArrayCount; BYTE byKeyModule; BYTE byPartID; BYTE byKeyAction; int nDataLen; int nOneDataLen;
            in >> dwArrayCount >> byKeyModule >> byPartID >> byKeyAction >> nDataLen >> nOneDataLen;

            UID* uidArray = (dwArrayCount > 0 ? (UID*)in.current() : nullptr);
            LPCSTR pszData = in.current() + (dwArrayCount*sizeof(UID));

            m_real_service->batchHandleMessage(uidArray, dwArrayCount, byKeyModule, byPartID, byKeyAction, pszData, nDataLen, nOneDataLen);
            return true;
        }

		if ( TEST_SAME_FUNCTION(INPCService::setProperty_Bool) )
		{
			UID uid;int prop_id;bool value;
			GET_TYPE_MSG_PARAM_3(UID ,uid,int ,prop_id,bool, value );
			m_real_service->setProperty_Bool(uid,prop_id,value);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::setProperty_Integer) )
		{
			UID uid;int prop_id;int value;
			GET_TYPE_MSG_PARAM_3(UID ,uid,int ,prop_id,int, value );
			m_real_service->setProperty_Integer(uid,prop_id,value);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::addProperty_Integer) )
		{
			UID uid;int prop_id;int delta;
			GET_TYPE_MSG_PARAM_3(UID ,uid,int ,prop_id,int, delta );
			m_real_service->addProperty_Integer(uid,prop_id,delta);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::setProperty_String) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			in.skip<BYTE>();

			UID uid;int prop_id;string value;
			in >> uid>>prop_id>>value;
			m_real_service->setProperty_String(uid,prop_id,value);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::setProperty_custom) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			in.skip<BYTE>();

			UID uid;int prop_id;string value;
			in >> uid>>prop_id>>value;
			m_real_service->setProperty_custom(uid,prop_id,value);
			return true;
		}

		if ( TEST_SAME_FUNCTION(INPCService::setSight) )
		{
			UID uid;float radii;
			GET_TYPE_MSG_PARAM_2(UID ,uid,float ,radii );
			m_real_service->setSight(uid,radii);
			return true;
		}

		if ( TEST_SAME_FUNCTION(INPCService::isHide) )
		{
			UID uid;
			GET_TYPE_MSG_PARAM_1(UID ,uid );
			m_real_service->isHide(uid);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::setHide) )
		{
			UID uid;bool state;
			GET_TYPE_MSG_PARAM_2(UID ,uid,bool, state );
			m_real_service->setHide(uid,state);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::AddEnmityValue) )
		{
			UID uidMonster, uidDes;int nVal;
			GET_TYPE_MSG_PARAM_3(UID, uidMonster, UID,uidDes,int ,nVal );
			m_real_service->AddEnmityValue(uidMonster,uidDes,nVal);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::AddBroadEnmityValue) )
		{
			UID uidMonster, uidDes;int nVal;
			GET_TYPE_MSG_PARAM_3(UID, uidMonster, UID,uidDes,int ,nVal );
			m_real_service->AddBroadEnmityValue(uidMonster,uidDes,nVal);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::SetEnmityValue) )
		{
			UID uidMonster, uidDes;int nVal;
			GET_TYPE_MSG_PARAM_3(UID, uidMonster, UID,uidDes,int ,nVal );
			m_real_service->SetEnmityValue(uidMonster,uidDes,nVal);
			return true;
		}

        if (TEST_SAME_FUNCTION(INPCService::BatchCheckAddBuff))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            DWORD dwArrayCount; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd; DWORD dwCheckBuff; DWORD dwCheckLevel; DWORD dwCheckBuffType; DWORD dwTime; int nContextLen; bool bExistAdd;
            in >> dwArrayCount >> dwBuffID >> dwBuffLevel >> uidAdd >> dwCheckBuff >> dwCheckLevel >> dwCheckBuffType >> dwTime >> nContextLen >> bExistAdd;

            UID* uidArray = (dwArrayCount > 0 ? (UID*)in.current() : nullptr);
            void *pBuffContext = in.current() + (dwArrayCount*sizeof(UID));

            m_real_service->BatchCheckAddBuff(uidArray, dwArrayCount, dwBuffID, dwBuffLevel, uidAdd, dwCheckBuff, dwCheckLevel, dwCheckBuffType, dwTime, pBuffContext, nContextLen, bExistAdd);
            return true;
        }
        if (TEST_SAME_FUNCTION(INPCService::BatchCheckRemoveBuff))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            DWORD dwArrayCount; DWORD dwBuffID; UID uidAdd; UID uidRemove; DWORD dwCheckBuff; DWORD dwCheckLevel; DWORD dwCheckBuffType; bool bExistRemove;
            in >> dwArrayCount >> dwBuffID >> dwBuffID >> uidAdd >> uidRemove >> dwCheckBuff >> dwCheckLevel >> dwCheckBuffType >> bExistRemove;

            UID* uidArray = (dwArrayCount > 0 ? (UID*)in.current() : nullptr);

            m_real_service->BatchCheckRemoveBuff(uidArray, dwArrayCount, dwBuffID, uidAdd, uidRemove, dwCheckBuff, dwCheckLevel, dwCheckBuffType, bExistRemove);
            return true;
        }

        if (TEST_SAME_FUNCTION(INPCService::BatchAddBuff))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            DWORD dwArrayCount; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd; DWORD dwTime; int nContextLen;
            in >> dwArrayCount >> dwBuffID >> dwBuffLevel >> uidAdd >> dwTime >> nContextLen;

            UID* uidArray = (dwArrayCount > 0 ? (UID*)in.current() : nullptr);
            void *pBuffContext = in.current() + (dwArrayCount*sizeof(UID));

            m_real_service->BatchAddBuff(uidArray, dwArrayCount, dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
            return true;
        }
        if (TEST_SAME_FUNCTION(INPCService::BatchRemoveBuff))
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            DWORD dwArrayCount; DWORD dwBuffID; UID uidAdd; UID uidRemove;
            in >> dwArrayCount >> dwBuffID >> uidAdd >> uidRemove;

            UID* uidArray = (dwArrayCount > 0 ? (UID*)in.current() : nullptr);

            m_real_service->BatchRemoveBuff(uidArray, dwArrayCount, dwBuffID, uidAdd, uidRemove);
            return true;
        }


		if ( TEST_SAME_FUNCTION(INPCService::AddBuff) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			in.skip<BYTE>();

			UID uidMonster; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd; DWORD dwTime; int nContextLen = 0;
			in >> uidMonster >> dwBuffID>>dwBuffLevel>>uidAdd>>dwTime>>nContextLen;			
			m_real_service->AddBuff(uidMonster,dwBuffID,dwBuffLevel,uidAdd,dwTime, in.current(), nContextLen);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::RemoveBuff) )
		{
			UID uidMonster; DWORD dwBuffID; UID uidAdd; UID uidRemove;
			GET_TYPE_MSG_PARAM_4(UID ,uidMonster, DWORD, dwBuffID,UID ,uidAdd,UID ,uidRemove );
			m_real_service->RemoveBuff(uidMonster,dwBuffID, uidAdd, uidRemove);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::IsExistBuff) )
		{
			UID uidMonster; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd;
			GET_TYPE_MSG_PARAM_4(UID ,uidMonster,DWORD, dwBuffID,DWORD,dwBuffLevel, UID, uidAdd );
			*resultBuf << m_real_service->IsExistBuff(uidMonster,dwBuffID,dwBuffLevel,uidAdd);
			return true;
		}
        //if ( TEST_SAME_FUNCTION(INPCService::IsExistBuffType) )
        //{
        //    UID uidMonster; DWORD dwBuffType;
        //    GET_TYPE_MSG_PARAM_2(UID ,uidMonster,DWORD, dwBuffType );
        //    *resultBuf << m_real_service->IsExistBuffType(uidMonster,dwBuffType);
        //    return true;
        //}
		if ( TEST_SAME_FUNCTION(INPCService::GetBuffLevel) )
		{
			UID uidMonster; DWORD dwBuffID; UID uidAdd;
			GET_TYPE_MSG_PARAM_3(UID ,uidMonster, DWORD, dwBuffID, UID, uidAdd );
			*resultBuf << m_real_service->GetBuffLevel(uidMonster,dwBuffID,uidAdd);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::GetLeftTime) )
		{
			UID uidMonster; DWORD dwBuffID; UID uidAdd;
			GET_TYPE_MSG_PARAM_3(UID ,uidMonster, DWORD, dwBuffID, UID, uidAdd );
			*resultBuf << m_real_service->GetLeftTime(uidMonster,dwBuffID,uidAdd);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::SetAccumulateBuff) )
		{
            rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in.skip<BYTE>();

			UID uidMonster; DWORD dwBuffID; UID uidAdd; bool bOverlay; DWORD dwTime; int nContextLen = 0;
            in >> uidMonster >>dwBuffID>>uidAdd>>bOverlay>>dwTime>>nContextLen;			
            m_real_service->SetAccumulateBuff(uidMonster,dwBuffID,uidAdd,bOverlay,dwTime, in.current(), nContextLen);
			return true;
		}	
		if ( TEST_SAME_FUNCTION(INPCService::RemoveBuffByIndex) )
		{
			UID uidMonster; DWORD dwIndex; 
			GET_TYPE_MSG_PARAM_2(UID ,uidMonster, DWORD, dwIndex );
			m_real_service->RemoveBuffByIndex(uidMonster,dwIndex);
			return true;
		}
		if ( TEST_SAME_FUNCTION(INPCService::changeState) )
		{
			UID uidMonster;int nVal;
			GET_TYPE_MSG_PARAM_2(UID, uidMonster,int ,nVal);
			m_real_service->changeState(uidMonster,nVal);
			return true;
		}	
		if ( TEST_SAME_FUNCTION(INPCService::getState) )
		{			
			UID uidMonster;
			GET_TYPE_MSG_PARAM_1(UID, uidMonster);
			*resultBuf << m_real_service->getState(uidMonster);
			return true;
		}

		if ( TEST_SAME_FUNCTION(INPCService::Transport) )
		{
			UID uidMonster; Vector3 newLoc; Vector3 rotation; bool bTransport; int nSceneID;int nReason;
			GET_TYPE_MSG_PARAM_6(UID ,uidMonster, Vector3, newLoc, Vector3, rotation, bool, bTransport,int, nSceneID,int,nReason);
			m_real_service->Transport(uidMonster,newLoc,rotation,bTransport,nSceneID,nReason);
			return true;
		}
        if ( TEST_SAME_FUNCTION(INPCService::MoveTo) )
        {			
            UID uidMonster;
            Vector3 loc;
            GET_TYPE_MSG_PARAM_2(UID ,uidMonster, Vector3, loc);
            m_real_service->MoveTo(uidMonster, loc);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::chooseTarget) )
        {			
            UID uidMonster;
            int nTargetFilter;
            UID uidTarget;
            GET_TYPE_MSG_PARAM_3(UID ,uidMonster, int, nTargetFilter,UID,uidTarget);
            m_real_service->chooseTarget(uidMonster,nTargetFilter,uidTarget);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::getPKType) )
        {			
            UID uidMonster;
            GET_TYPE_MSG_PARAM_1(UID ,uidMonster);
			*resultBuf << m_real_service->getPKType(uidMonster);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::getPKState) )
        {			
            UID uidMonster;
            GET_TYPE_MSG_PARAM_1(UID ,uidMonster);
			*resultBuf << m_real_service->getPKState(uidMonster);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::forceEndPK) )
        {			
            UID uidMonster;
            GET_TYPE_MSG_PARAM_1(UID ,uidMonster);
            m_real_service->forceEndPK(uidMonster);
            return true;
        }

		if (TEST_SAME_FUNCTION(INPCService::getPKCamp))
		{
            UID uidMonster;
            GET_TYPE_MSG_PARAM_1(UID ,uidMonster);
			*resultBuf << m_real_service->getPKCamp(uidMonster);
			return true;
		}

        
        if ( TEST_SAME_FUNCTION(INPCService::isInsight) )
        {			
            UID uid;
            int camp;
            GET_TYPE_MSG_PARAM_2(UID ,uid,int,camp);
            *resultBuf << m_real_service->isInsight(uid, camp);
            return true;
        }

        if(TEST_SAME_FUNCTION(INPCService::getWarID))
        {
            UID uid;
            GET_TYPE_MSG_PARAM_1(UID ,uid);
            *resultBuf << m_real_service->getWarID(uid);
            return true;
        }

        if(TEST_SAME_FUNCTION(INPCService::getBuff))
        {
            UID uid;
            DWORD dwBuffType;
            GET_TYPE_MSG_PARAM_2(UID ,uid,DWORD,dwBuffType);
            *resultBuf << m_real_service->getBuff(uid,dwBuffType);
            return true;
        }

        if(TEST_SAME_FUNCTION(INPCService::getMonsterArray))
        {
            int nMonsterID;int nMonsterCamp;UID *pArray;int nArraySize;
            GET_TYPE_MSG_PARAM_4(int ,nMonsterID,int ,nMonsterCamp,UID *,pArray,int, nArraySize);
            *resultBuf << m_real_service->getMonsterArray(nMonsterID,nMonsterCamp,pArray,nArraySize);
            return true;
        }

		// 通知指定阵营当前野怪点野怪的生死状况,仅用于隐形实体
		if ( TEST_SAME_FUNCTION(INPCService::msgToSpecCampWildInfo) )
		{			
			UID uidMonster;
			int nCamp;
			GET_TYPE_MSG_PARAM_2(UID ,uidMonster,int ,nCamp);
			m_real_service->msgToSpecCampWildInfo(uidMonster, nCamp);
			return true;
		}

		if ( TEST_SAME_FUNCTION(INPCService::destroyAll) )
		{
			m_real_service->destroyAll();
			return true;
		}

        if ( TEST_SAME_FUNCTION(INPCService::sendDynamicObstacleToMovePart) )
        {
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

			UID uid; size_t nLen;
			in >> uid >> nLen;

			m_real_service->sendDynamicObstacleToMovePart(uid, in.current(), nLen);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::mobaGoodsAddEffect) )
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in.skip<BYTE>();

            size_t nLen;
            UID uid;
            int nEffectID;
            in>>uid >>nEffectID >> nLen;

            if ( resultBuf ) *resultBuf <<m_real_service->mobaGoodsAddEffect(uid, nEffectID, in.current(), nLen);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::mobaGoodsRemoveEffect) )
        {	
            UID uid;
            int nID;

            GET_TYPE_MSG_PARAM_2(UID, uid, int ,nID);
            m_real_service->mobaGoodsRemoveEffect(uid, nID);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::entityNotifyMaster) )
        {
            UID uid;
            EntityBelongContext context;

            GET_TYPE_MSG_PARAM_2(UID, uid, EntityBelongContext, context);
            m_real_service->entityNotifyMaster(uid, context);
            return true;
        }

        if ( TEST_SAME_FUNCTION(INPCService::shift) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in.skip<BYTE>();

            UID uidMonster; int nDataLen = 0;
            in >> uidMonster >> nDataLen;			
            m_real_service->shift(uidMonster, in.current(), nDataLen);
            return true;
        }
		if ( TEST_SAME_FUNCTION(INPCService::release) )
		{
			m_real_service->release();
			return true;
		}

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
//extern "C" __declspec(dllexport) IServiceStub * CreateNPCService()
//{
//	return new NPCService_Stub(new NPCService);
//}