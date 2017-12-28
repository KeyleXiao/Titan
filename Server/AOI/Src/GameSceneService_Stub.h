/*******************************************************************
** 文件名:	GameSceneService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IGameSceneService接口定义，自动生成的GameSceneService接口
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
#include "IGameSceneService.h"
#include "buffer.h"
#include "Game_ManagedDef.h"

// 这个是服务实现类，你需要自己实现
#include "GameScene_Dota.h"
#include "GameScene.h"
#include "IServerGlobal.h"
#include "IWarManager.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IGameSceneService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class GameSceneService_Proxy : public IGameSceneService
{
public:
	SERVICE_PTR         m_pContainer;
    IGameSceneService * m_pRealService;


	GameSceneService_Proxy() : m_pRealService(0), m_pContainer(0)
    {
	}

	virtual ~GameSceneService_Proxy(){
	}

    void setRealService(IGameSceneService * pRealService)
    {
        m_pRealService = pRealService;
    }
    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	
   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  onCreate( int mapID,int sceneID, bool isCopy, int nWarID )
    {
	    return true;
    };

   virtual bool on_stop()
   {
       return true;
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getSceneID(  )
    {
		return m_pRealService ? m_pRealService->getSceneID() : INVALID_SCENE_ID;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int getMapID(  )
    {
		return m_pRealService ? m_pRealService->getMapID() : INVALID_MAPID;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag = MSG_FLAG_DEFAULT )
    {
        if(m_pContainer == 0)
        {
            return false;
        }

		AOI_PROXY_PTR::T_BAG packer(proxy);
	    BUILD_MSG_CONTEXT_2( IGameSceneService::insertEntity,AOI_PROXY_PTR::T_BAG ,packer,Vector3, loc );

	    rkt::obuf resultBuf;
	    if(!m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK))
        {
            AOI_PROXY_PTR ptrProxy = packer.get();
            return false;
        }

        return true;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag = MSG_FLAG_DEFAULT )
    {
        if(m_pContainer == 0)
        {
            return false;
        }

		AOI_PROXY_PTR::T_BAG packer(proxy);
	    BUILD_MSG_CONTEXT_2( IGameSceneService::removeEntity,AOI_PROXY_PTR::T_BAG ,packer,Vector3, loc );

	    rkt::obuf resultBuf;
	    if(!m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK))
        {
            AOI_PROXY_PTR ptrProxy = packer.get();
            return false;
        }
	    
        return true;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc )
    {
        if(m_pContainer == 0)
        {
            return false;
        }

		AOI_PROXY_PTR::T_BAG packer(proxy);
		BUILD_MSG_CONTEXT_2( IGameSceneService::moveEntity,AOI_PROXY_PTR::T_BAG ,packer,Vector3, newLoc );

		if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
        {
            AOI_PROXY_PTR ptrProxy = packer.get();
            return false;
        }
        return true;
    };

   ////////////////////////////////////////////////////////////////////////////////
   virtual bool setObserver( AOI_PROXY_PTR proxy,bool bObserver )
   {
       if(m_pContainer == 0)
       {
           return false;
       }

       AOI_PROXY_PTR::T_BAG packer(proxy);
       BUILD_MSG_CONTEXT_2( IGameSceneService::setObserver,AOI_PROXY_PTR::T_BAG ,packer,bool,bObserver);

       if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
       {
           AOI_PROXY_PTR ptrProxy = packer.get();
           return false;
       }
       return true;
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual void updateObjVisible( AOI_PROXY_PTR proxy )
   {
       if(m_pContainer == 0)
       {
           return;
       }

       AOI_PROXY_PTR::T_BAG packer(proxy);
       BUILD_MSG_CONTEXT_1( IGameSceneService::updateObjVisible,AOI_PROXY_PTR::T_BAG ,packer );

       if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
       {
           AOI_PROXY_PTR ptrProxy = packer.get();
       }
   }

   virtual void onUpdateObjAntiStealth( AOI_PROXY_PTR proxy )
   {
       if(m_pContainer == 0)
       {
           return;
       }

       AOI_PROXY_PTR::T_BAG packer(proxy);
       BUILD_MSG_CONTEXT_1( IGameSceneService::onUpdateObjAntiStealth,AOI_PROXY_PTR::T_BAG ,packer );

       if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
       {
           AOI_PROXY_PTR ptrProxy = packer.get();
       }
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual void setSight( AOI_PROXY_PTR proxy,int nSight )
   {
       if(m_pContainer == 0)
       {
           return;
       }

       AOI_PROXY_PTR::T_BAG packer(proxy);
       BUILD_MSG_CONTEXT_2( IGameSceneService::setSight,AOI_PROXY_PTR::T_BAG ,packer, int, nSight );

       if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
       {
           AOI_PROXY_PTR ptrProxy = packer.get();
       }
   }

   virtual void updateObjsInSight(AOI_PROXY_PTR proxy)
   {
       if(m_pContainer == 0)
       {
           return;
       }

       AOI_PROXY_PTR::T_BAG packer(proxy);
       BUILD_MSG_CONTEXT_1( IGameSceneService::updateObjsInSight,AOI_PROXY_PTR::T_BAG ,packer );

       if(!m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK))
       {
           AOI_PROXY_PTR ptrProxy = packer.get();
       }
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR * pReturnArray,int nArraySize,int layerMask, bool bCheckDist=false)
    {
        if(m_pRealService == NULL)
        {
            return 0;
        }
		return m_pRealService->k_nearest( loc,fDist,pReturnArray,nArraySize,layerMask, bCheckDist);
	    //BUILD_MSG_CONTEXT_5( IGameSceneService::k_nearest,Vector3 ,loc,float ,fDist,UID* ,pReturnArray,int ,nArraySize,int, layerMask);


	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	    //RETURN_FROM_MSG_BUF(int);
	    //return 0;
    };

   virtual int  k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR * pReturnArray,int nArraySize,int layerMask )
   {
       if(m_pRealService == NULL)
       {
           return 0;
       }

	   return m_pRealService->k_nearestInSpell( loc,fDist,fHeight,pReturnArray,nArraySize,layerMask);
	    //BUILD_MSG_CONTEXT_6( IGameSceneService::k_nearestInSpell,Vector3, loc,float ,fDist,float ,fHeight,UID* ,pReturnArray,int ,nArraySize,int ,layerMask);


	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	    //RETURN_FROM_MSG_BUF(int);
	    //return 0;
   };

   virtual int  k_nearLineest( Vector3 loc,Vector3 dir,float fDist, float fWidth, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int layerMask )
   {
       if(m_pRealService == NULL)
       {
           return 0;
       }

		return m_pRealService->k_nearLineest( loc,dir,fDist,fWidth,fHeight,pReturnArray,nArraySize,layerMask);
	   //obuf128 t_data;
	   //t_data << loc<< dir <<fDist << fWidth << fHeight <<pReturnArray<<nArraySize<<layerMask/*<<strFileInfo<<nFileLine*/;

	   //BUILD_MSG_BUFFER( IGameSceneService::k_nearLineest,t_data);

	   // rkt::obuf resultBuf;
	   // m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	   // RETURN_FROM_MSG_BUF(int);
	   // return 0;
   };

   virtual int  k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int layerMask )
   {
       if(m_pRealService == NULL)
       {
           return 0;
       }

	   	return m_pRealService->k_nearSectorest( loc,dir,fr,squaredR,cosTheta,fHeight,pReturnArray,nArraySize,layerMask);
	   //obuf128 t_data;
	   //t_data << loc<< dir <<fr << squaredR << cosTheta << fHeight <<pReturnArray<<nArraySize<<layerMask;

	   //BUILD_MSG_BUFFER( IGameSceneService::k_nearSectorest,t_data);

	   // rkt::obuf resultBuf;
	   // m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	   // RETURN_FROM_MSG_BUF(int);
	   // return 0;
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  broadcast( Vector3 loc,int dist,BROADCAST_ID id,string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), bool ignoreSelf = false)
    {
        if(m_pContainer == 0)
        {
            return 0;
        }

       int nLen = sizeof(Vector3) + sizeof(int) + sizeof(BROADCAST_ID) + sizeof(string::size_type) + msg.size() + sizeof(AOI_PROXY_PTR) + sizeof(bool) ;
       BUILD_MSG_BUFFER_LEN(IGameSceneService::broadcast, nLen);

       t_data << loc << dist << id << msg << proxy << ignoreSelf;

	    if(!m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK))
        {
            free(pMsg);
        }
        return 0;
    };

   virtual int  broadcastMultiple( char *pLoc,int nCount,BROADCAST_ID id,string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0) )
   {
       if(m_pContainer == 0)
       {
           return 0;
       }

       int nLen = sizeof(int) + sizeof(BROADCAST_ID) + sizeof(string::size_type) + msg.size() + sizeof(AOI_PROXY_PTR) + nCount*sizeof(Vector3);
       BUILD_MSG_BUFFER_LEN(IGameSceneService::broadcastMultiple, nLen);

	   t_data<<nCount<<id<<msg<<proxy;
	   if (nCount > 0)
	   {
		   t_data.push_back(pLoc, nCount*sizeof(Vector3));
	   }

	    if(!m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK))
        {
            free(pMsg);
        }
	    return 0;
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  broadcast_all( BROADCAST_ID id,string & msg,int nCampMask )
    {
        if(m_pContainer == 0)
        {
            return 0;
        }

        int nLen = sizeof(BROADCAST_ID) + sizeof(string::size_type) + msg.size() + sizeof(int);
        BUILD_MSG_BUFFER_LEN(IGameSceneService::broadcast_all, nLen);

		t_data << id << msg << nCampMask;
	    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	    return 0;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  broadcast_nearLoc( Vector3 loc,int dist,BROADCAST_ID id,string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), bool ignoreSelf = false,int campMask=CAMP_MASK_ALL   )
   {
       if(m_pContainer == 0)
       {
           return 0;
       }

       int nLen = sizeof(Vector3) + sizeof(int) + sizeof(BROADCAST_ID) + sizeof(string::size_type) + msg.size() + sizeof(AOI_PROXY_PTR) + sizeof(bool);
       BUILD_MSG_BUFFER_LEN(IGameSceneService::broadcast_nearLoc, nLen);

       t_data << loc<<dist<<id<<msg<<proxy<<ignoreSelf<<campMask;

	    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	    return 0;
   };

   virtual int  broadcastMultiple_nearLoc( char *pLoc,int nCount,BROADCAST_ID id,string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),int campMask=CAMP_MASK_ALL  )
   {
       if(m_pContainer == 0)
       {
           return 0;
       }

       int nLen = sizeof(int) + sizeof(BROADCAST_ID) + sizeof(string::size_type) + msg.size() + sizeof(AOI_PROXY_PTR) + nCount*sizeof(Vector3);
       BUILD_MSG_BUFFER_LEN(IGameSceneService::broadcastMultiple_nearLoc, nLen);

       t_data << nCount<<id<<msg<<proxy<<campMask;
       if (nCount > 0)
       {
           t_data.push_back(pLoc, nCount*sizeof(Vector3));
       }

	    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
	    return 0;
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getPlayerCount(  )
    {
	   if ( m_pRealService )
	   {
	     	return  m_pRealService->getPlayerCount();
	   }

	   return 0;
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getMonsterCount(  )
   {
	   if ( m_pRealService )
	   {
	     	return  m_pRealService->getMonsterCount();
	   }

	   return 0;
   };

   //////////////////////////////////////////////////////////////////////////////////
   // 获取场景更新共享数据
   virtual SceneUpdateShareInfo getUpdateShareInfo()
   {
       if(m_pContainer != 0)
       {
           BUILD_MSG_CONTEXT_VOID( IGameSceneService::getUpdateShareInfo );

           rkt::obuf resultBuf;
           m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
           RETURN_FROM_MSG_BUF(SceneUpdateShareInfo);
       }

       return SceneUpdateShareInfo();     // 请检查默认返回值是否正确
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getAllPlayers( AOI_PROXY_PTR* pReturnArray,int nArraySize )
    {
        if(m_pContainer != 0)
        {
            BUILD_MSG_CONTEXT_2( IGameSceneService::getAllPlayers,AOI_PROXY_PTR* ,pReturnArray,int, nArraySize );

            rkt::obuf resultBuf;
            m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
            RETURN_FROM_MSG_BUF(int);
        }

       return 0;
    };

   virtual UINT_PTR getLogicServiceID()
   {
		return m_pRealService ? m_pRealService->getLogicServiceID() : INVALID_SERVICE_ID;
   }

   // 取得给ob分配的阵营ID
   virtual int getObCampID()
   {
       return m_pRealService ? m_pRealService->getObCampID() : -1; 
   }

   // 关闭场景
   virtual void close() 
   {
       if(m_pContainer != 0)
       {
           BUILD_MSG_CONTEXT_VOID( IGameSceneService::close );

           m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
       }
   }
   //////////////////////////////////////////////////////////////////////////////////
   virtual void  release(  )
    {
        if(m_pContainer != 0)
        {
            BUILD_MSG_CONTEXT_VOID( IGameSceneService::release );

            m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
        }
    };

};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class GameSceneService_Stub : public IServiceStub
{
	GameSceneService_Proxy  m_proxy;
	IGameSceneService *		m_real_service;
    int                     m_nWarID;

public:
	GameSceneService_Stub() : m_real_service(0), m_nWarID(-1){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		ibuffer in(data,len);
        int nLen = 0;
        int sceneID = 0;
        SCreateSceneContext context;
		in >> context >> sceneID >> nLen;
		if ( in.good()==false )
			return false;

        if(nLen >= sizeof(m_nWarID))
        {
            in.pop(&m_nWarID, sizeof(m_nWarID));
        }

        switch(context.gameMode)
        {
        case EGM_Moba:
            {
                m_real_service = new GameScene_Dota(context.nCampCount);
            }
            break;
        case EGM_RPG:
            {
                m_real_service = new GameScene;
            }
            break;
		case EGM_MCAMP:
			{
				m_real_service = new GameScene_Dota(context.nCampCount);
			}
			break;
        default:
            {
                ErrorLn(__FUNCTION__ << ": " << __LINE__ << ", undefined gamemode=" << context.gameMode);
                return false;
            }
        }

        m_proxy.setRealService(m_real_service);

		return m_real_service->onCreate(context.mapID,sceneID, context.isDynamic,m_nWarID);
	}

	// 停止
	virtual bool on_stop()
	{
        int nSceneID = m_real_service->getSceneID();
        bool bResult = m_real_service->on_stop();
        
        m_proxy.m_pContainer = SERVICE_PTR(0);

        // 场景service退出 通知场景管理器
        gServerGlobal->getSceneManager()->onSceneClosed(nSceneID);

        return bResult;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查
		
		if ( TEST_SAME_FUNCTION(IGameSceneService::onCreate) )
		{
			int mapID;int sceneID;BYTE byGameMode;
			GET_MSG_PARAM_3(int ,mapID,int ,sceneID,BYTE ,byGameMode );

			const bool & vReturn= m_real_service->onCreate(mapID,sceneID,byGameMode);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::getSceneID) )
		{
			const int & vReturn= m_real_service->getSceneID();
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::getMapID) )
		{
			const int & vReturn= m_real_service->getMapID();
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::insertEntity) )
		{
			AOI_PROXY_PTR::T_BAG packer;
			AOI_PROXY_PTR proxy;Vector3 loc;
			GET_MSG_PARAM_2(AOI_PROXY_PTR::T_BAG, packer,Vector3, loc );

			const bool & vReturn= m_real_service->insertEntity(packer.get(),loc);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}
		
		if ( TEST_SAME_FUNCTION(IGameSceneService::removeEntity) )
		{
			AOI_PROXY_PTR::T_BAG packer;
			AOI_PROXY_PTR proxy;Vector3 loc;
			GET_MSG_PARAM_2(AOI_PROXY_PTR::T_BAG, packer,Vector3, loc );

			const bool & vReturn= m_real_service->removeEntity(packer.get(),loc);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::moveEntity) )
		{
			AOI_PROXY_PTR::T_BAG packer;
			AOI_PROXY_PTR proxy;Vector3 newLoc;
			GET_MSG_PARAM_2(AOI_PROXY_PTR::T_BAG, packer,Vector3, newLoc );
			const bool & vReturn= m_real_service->moveEntity(packer.get(),newLoc);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IGameSceneService::setObserver) )
        {
            AOI_PROXY_PTR::T_BAG packer;
            AOI_PROXY_PTR proxy;bool bObserver;
            GET_MSG_PARAM_2(AOI_PROXY_PTR::T_BAG, packer, bool, bObserver);
            const bool & vReturn= m_real_service->setObserver(packer.get(), bObserver);
            if ( resultBuf) *resultBuf << vReturn;
            return true;
        }

        // virtual bool setObserver( AOI_PROXY_PTR proxy )

        if ( TEST_SAME_FUNCTION(IGameSceneService::updateObjVisible) )
        {
            AOI_PROXY_PTR::T_BAG packer;
            GET_MSG_PARAM_1(AOI_PROXY_PTR::T_BAG, packer );
            m_real_service->updateObjVisible(packer.get());
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGameSceneService::onUpdateObjAntiStealth) )
        {
            AOI_PROXY_PTR::T_BAG packer;
            GET_MSG_PARAM_1(AOI_PROXY_PTR::T_BAG, packer );
            m_real_service->onUpdateObjAntiStealth(packer.get());
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGameSceneService::setSight) )
        {
            AOI_PROXY_PTR::T_BAG packer;
            int nSight;
            GET_MSG_PARAM_2(AOI_PROXY_PTR::T_BAG, packer, int, nSight );
            m_real_service->setSight(packer.get(), nSight);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGameSceneService::updateObjsInSight) )
        {
            AOI_PROXY_PTR::T_BAG packer;
            GET_MSG_PARAM_1(AOI_PROXY_PTR::T_BAG, packer );
            m_real_service->updateObjsInSight(packer.get());
            return true;
        }

		//if ( TEST_SAME_FUNCTION(IGameSceneService::k_nearest) )
		//{
		//	Vector3 loc;float fDist;UID* pReturnArray;int nArraySize;int layerMask;
		//	GET_MSG_PARAM_5(Vector3, loc,float, fDist,UID* ,pReturnArray,int, nArraySize,int, layerMask );

		//	const int & vReturn= m_real_service->k_nearest(loc,fDist,pReturnArray,nArraySize,layerMask);
		//	if ( resultBuf) *resultBuf << vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IGameSceneService::k_nearestInSpell) )
		//{
		//	Vector3 loc;float fDist;float fHeight;UID* pReturnArray;int nArraySize;int layerMask;
		//	GET_MSG_PARAM_6(Vector3, loc,float, fDist,float, fHeight,UID*, pReturnArray,int ,nArraySize,int, layerMask );

		//	const int & vReturn= m_real_service->k_nearestInSpell(loc,fDist,fHeight,pReturnArray,nArraySize,layerMask);
		//	if ( resultBuf) *resultBuf << vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IGameSceneService::k_nearLineest) )
		//{
		//	Vector3 loc;Vector3 dir;float fDist;float fWidth;float fHeight;UID* pReturnArray;int nArraySize;int layerMask;
		//	rkt::ibuffer in(pMsg->context,pMsg->context_len);
		//	in >> loc>>dir>>fDist>>fWidth>>fHeight>>pReturnArray>>nArraySize>>layerMask;
		//	const int & vReturn= m_real_service->k_nearLineest(loc,dir,fDist,fWidth,fHeight,pReturnArray,nArraySize,layerMask);
		//	if ( resultBuf) *resultBuf << vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IGameSceneService::k_nearSectorest) )
		//{
		//	Vector3 loc;Vector3 dir;float fr;float squaredR;float cosTheta;float fHeight;UID* pReturnArray;int nArraySize;int layerMask;
		//	rkt::ibuffer in(pMsg->context,pMsg->context_len);
		//	in >> loc>>dir>>fr>>squaredR>>cosTheta>>fHeight>>pReturnArray>>nArraySize>>layerMask;
		//	const int & vReturn= m_real_service->k_nearSectorest(loc,dir,fr,squaredR,cosTheta,fHeight,pReturnArray,nArraySize,layerMask);
		//	if ( resultBuf) *resultBuf << vReturn;
		//	return true;
		//}

		if ( TEST_SAME_FUNCTION(IGameSceneService::broadcast) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);

			Vector3 loc;int dist;BROADCAST_ID id;string  msg; AOI_PROXY_PTR proxy;bool ignoreSelf;
			in >> loc >> dist >> id >> msg >> proxy >> ignoreSelf;

            const int & vReturn= m_real_service->broadcast(loc,dist,id,msg,proxy,ignoreSelf);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::broadcastMultiple) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);

			int nCount;BROADCAST_ID id;string msg; AOI_PROXY_PTR proxy;
			in >> nCount>>id>>msg>>proxy;

			const int & vReturn= m_real_service->broadcastMultiple(in.current(), nCount,id,msg,proxy);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::broadcast_all) )
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len);

			BROADCAST_ID id;string msg;int nCampMask;
			in >> id>>msg>>nCampMask;

			const int & vReturn= m_real_service->broadcast_all(id,msg,nCampMask);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IGameSceneService::broadcast_nearLoc) )
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            
            Vector3 loc;int dist;BROADCAST_ID id;string msg; AOI_PROXY_PTR proxy; bool ignoreSelf;int campMask;
            in >> loc>>dist>>id>>msg>>proxy>>ignoreSelf>>campMask;

			const int & vReturn= m_real_service->broadcast_nearLoc(loc,dist,id,msg,proxy,ignoreSelf,campMask);
            if ( resultBuf) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IGameSceneService::broadcastMultiple_nearLoc) )
        {
            rkt::ibuffer in(pMsg->context,pMsg->context_len);

            int nCount;BROADCAST_ID id; AOI_PROXY_PTR proxy;string msg ;int campMask;
            in >> nCount>>id>>msg>>proxy>>campMask;

			const int & vReturn= m_real_service->broadcastMultiple_nearLoc(in.current(), nCount,id,msg,proxy,campMask);
            if ( resultBuf) *resultBuf << vReturn;
            return true;
        }

		if ( TEST_SAME_FUNCTION(IGameSceneService::getPlayerCount) )
		{
			const int & vReturn= m_real_service->getPlayerCount();
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IGameSceneService::getMonsterCount) )
        {
            const int & vReturn= m_real_service->getMonsterCount();
            if ( resultBuf) *resultBuf << vReturn;
            return true;
        }
        
        if ( TEST_SAME_FUNCTION(IGameSceneService::getUpdateShareInfo) )
        {
            if ( resultBuf) *resultBuf <<  m_real_service->getUpdateShareInfo();
            return true;
        }        

		if ( TEST_SAME_FUNCTION(IGameSceneService::getAllPlayers) )
		{
			AOI_PROXY_PTR* pReturnArray;int nArraySize;
			GET_MSG_PARAM_2( AOI_PROXY_PTR* ,pReturnArray,int, nArraySize );

			const int & vReturn= m_real_service->getAllPlayers(pReturnArray,nArraySize);
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::getLogicServiceID) )
		{
			const UINT_PTR & vReturn= m_real_service->getLogicServiceID();
			if ( resultBuf) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::release) )
		{
			m_real_service->release();
			return true;
		}

		if ( TEST_SAME_FUNCTION(IGameSceneService::close) )
		{
			m_real_service->close();
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

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateGameSceneService()
{
	return new GameSceneService_Stub();
}