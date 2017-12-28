/*******************************************************************
** 文件名:	ActorService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IActorService接口定义，自动生成的ActorService接口
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
#include "IActorService.h"
#include "buffer.h"
// 这个是服务实现类，你需要自己实现
#include "ActorService.h"

using namespace rkt;
using namespace std;

/** 
代理对象:
该代理实现IActorService接口，把接口函数调用封装为框架系统的消息转发
对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class ActorService_Proxy : public IActorService
{
public:
    IServiceContainer*		m_pContainer;
	ActorService *       m_real_service;

    ActorService_Proxy( ActorService * pReal) : m_real_service(pReal) {
    }

    virtual ~ActorService_Proxy(){
    }

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
    
    virtual UID  getUID(  )
    {
		return m_real_service ? m_real_service->getUID() : 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual string  getName(  )
    {
		return m_real_service ? m_real_service->getName(): "";
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual int  getSceneID(  )
    {
		return m_real_service ? m_real_service->getSceneID(): INVALID_SCENE_ID;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual Vector3  getLocation(  )
    {
		return m_real_service ? m_real_service->getLocation(): Vector3();
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  setLocation( Vector3 vLoc )
    {
        BUILD_MSG_CONTEXT_1( IActorService::setLocation,Vector3,vLoc );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual Vector3  getAngle(  )
    {
		return m_real_service ? m_real_service->getAngle(): Vector3();
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  handleMessage( void *pData,int nLen )
    {
		BUILD_MSG_BUFFER_LEN( IActorService::handleMessage,nLen );
		t_data.push_back(pData,nLen);

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    virtual void handleReplaceableMessage(unsigned long ulKey,void * pData,int nLen)
    {
		BUILD_MSG_BUFFER_LEN( IActorService::handleMessage,nLen );
		t_data.push_back(pData,nLen);

        m_pContainer->post_replaceable_message(ulKey, pMsg,nMsgLen);
    }

    //////////////////////////////////////////////////////////////////////////////////
    virtual void changeState( int nState )
    {
        BUILD_MSG_CONTEXT_1( IActorService::changeState,int,nState );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    // 得到当前状态 
    virtual int getState(void)
    {
		// 4字节只读是多线程安全的
		return m_real_service ? m_real_service->getState() : 0;
    };


    // 设置游戏状态ACTOR_GAME_STATE
    virtual void setGameState(DWORD dwState)
    {
        BUILD_MSG_CONTEXT_1( IActorService::setGameState,DWORD,dwState );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    // 获取游戏状态 ACTOR_GAME_STATE
    virtual DWORD getGameState(void)
    {
		return m_real_service ? m_real_service->getGameState(): 0;
    };


    // 取得玩家所在网关分配ID
    virtual DWORD getClientID() 
    {
		return m_real_service ? m_real_service->getClientID() : INVALID_CLIENT;
    }

    virtual string exportActorData(int nFlag, bool bFromBank)
    {
        BUILD_MSG_CONTEXT_2( IActorService::exportActorData,int,nFlag,bool,bFromBank );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);

        if ( resultBuf.good() && resultBuf.size() > 0  )
        {
            ibuffer t_in(resultBuf.data(),resultBuf.size());
            string vReturn;
            t_in >> vReturn;
            if ( t_in.good() ) return vReturn;
        }

        return string();     // 请检查默认返回值是否正确
    }

    // 续完
    virtual void continuePlay(ClientID client)
    {
        BUILD_MSG_CONTEXT_1( IActorService::continuePlay,ClientID,client );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK );
    }

    virtual void onClientDisconnect()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::onClientDisconnect );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
    }

    // 设置客户端ID
    virtual void setClientID(ClientID client)
    {
        BUILD_MSG_CONTEXT_1( IActorService::setClientID,ClientID,client );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf );
    }

    // 导出角色的共享属性
    virtual SSharePersonProperty exportShareProperties()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::exportShareProperties );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen, &resultBuf, MSG_FLAG_READ_ONLY);
        if (resultBuf.good() && resultBuf.size() > 0)
        {
            return (* ((SSharePersonProperty*)resultBuf.data()) );
        }

        return SSharePersonProperty();     // 请检查默认返回值是否正确
    }


    // 取得创建上下文中的标识数据 ACTOR_CREATE_CONTEXT_FLAG
    virtual BYTE getCreateContextFlag(BYTE nFromActorType, BYTE nToActorType, BYTE nFlag)
    {
        BUILD_MSG_CONTEXT_3( IActorService::getCreateContextFlag,BYTE,nFromActorType,BYTE,nToActorType,BYTE,nFlag );

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf, MSG_FLAG_READ_ONLY);
        RETURN_FROM_MSG_BUF(BYTE);

        return BYTE();     // 请检查默认返回值是否正确
    }

    // 是否为压力测试账号
    virtual bool isTester()
    {
        return m_real_service ? m_real_service->isTester() : false;
    }

	/** 增加/减少经验值
	@param   nExp:            经验值（负值减少，正值增加）
	@param   wApproach:       消耗原因,0.不需要记录日记
	@param   dwTargetID:      如果有目标者，这个就填入目标者ID
	@param   szLogMark:       传入交易日志，供查询  
	@param   nLogReserve:     日志的保留字段
	@return
	*/
	struct addExp_Param
	{
		int nExp;
		WORD wApproach;
		DWORD dwTargetID;
		char pszRemark[128];
		int nLogReserve;
	};

    virtual void addExp(int nExp, WORD wApproach, DWORD dwTargetID, const char * pszRemark, int nLogReserve)
    {
		BUILD_MSG_CONTEXT( IActorService::addExp,addExp_Param );

        pParam->nExp = nExp;
		pParam->wApproach = wApproach;
		pParam->dwTargetID = dwTargetID;
		if ( pszRemark ) 
			lstrcpynA( pParam->pszRemark,pszRemark,sizeof(pParam->pszRemark));
		else
			pParam->pszRemark[0] = 0;

		pParam->nLogReserve= nLogReserve;

        m_pContainer->post_message( pMsg, nMsgLen, 0,MSG_FLAG_NO_BLOCK);
    }

    /** 是否可增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod)
    {
        BUILD_MSG_CONTEXT_2( IActorService::canAddMoney,int,nTicket,int,nGlod );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(bool);

        return bool();     // 请检查默认返回值是否正确
    }


    /** 增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @param  : nReason 添加的原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
    @param  : pszRemark 添加的点券日志的备注（记日志用）
    @param  : nSubclassID, 子类ID，购买物品则填物品ID，没有子类默认0
    @param  : nNum 个数，子类是物品ID则填物品个数
	@param  : bAddictLimit，是否考虑沉迷限制
    @ note  : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
    */
	struct addMoney_Param
	{
		int nTicket;
        int nGlod;
		int nReason;
		char pszRemark[128];
		int nSubclassID;
		short nNum;
		bool bAddictLimit;
	};
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark, int nSubclassID, short nNum, bool bAddictLimit)
    {
        // 同一个线程
        if ( gServerGlobal->getFramework()->get_call_service()==m_pContainer->get_id() )
        {
            return m_real_service ? m_real_service->addMoney(nTicket,nGlod, nReason, pszRemark, nSubclassID, nNum, bAddictLimit) : false;
        }

        BUILD_MSG_CONTEXT( IActorService::addMoney,addMoney_Param );
		pParam->nTicket = nTicket;
        pParam->nGlod = nGlod;
		pParam->nReason = nReason;
		if (pszRemark)
			lstrcpynA(pParam->pszRemark, pszRemark, sizeof(pParam->pszRemark));
		else
			memset(pParam->pszRemark, 0, sizeof(pParam->pszRemark));
		pParam->nSubclassID = nSubclassID;
		pParam->nNum = nNum;
		pParam->bAddictLimit = bAddictLimit;

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
        if ( resultBuf.good() && resultBuf.size() > 0  )
        {
            ibuffer t_in(resultBuf.data(),resultBuf.size());
            bool vReturn;
            t_in >> vReturn;
            if ( t_in.good() ) return vReturn;
        }

        return bool();     // 请检查默认返回值是否正
    }

    //////////////////////////////////////////////////////////////////////////////////
    virtual int  send2Client( PACKAGE_PTR package, int nFlag = 0 )
    {
		return m_real_service ? m_real_service->send2Client(package, nFlag) : 0;
    };

    // 发送数据给客户端 附带发给ob
    virtual void send2ClientAndOB( PACKAGE_PTR package )
    {
        if(m_real_service) m_real_service->send2ClientAndOB(package);
    }

    //////////////////////////////////////////////////////////////////////////////////
    virtual string  getPropertyType( int prop_id )
    {
		return m_real_service ? m_real_service->getPropertyType(prop_id) : "";
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual int  getPropertyFlag( int prop_id )
    {
		return m_real_service ? m_real_service->getPropertyFlag(prop_id) : 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  getProperty_Bool( int prop_id )
    {
		return m_real_service ? m_real_service->getProperty_Bool(prop_id) : false;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  setProperty_Bool( int prop_id,bool value )
    {
        BUILD_MSG_CONTEXT_2( IActorService::setProperty_Bool,int, prop_id,bool, value );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual int  getProperty_Integer( int prop_id )
    {
		return m_real_service ? m_real_service->getProperty_Integer( prop_id ) : 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  setProperty_Integer( int prop_id,int value )
    {
		// 同一个线程
		if ( gServerGlobal->getFramework()->get_call_service()==m_pContainer->get_id() )
		{
			return m_real_service ? m_real_service->setProperty_Integer( prop_id,value ) : false;
		}

        BUILD_MSG_CONTEXT_2( IActorService::setProperty_Integer,int, prop_id,int, value );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  addProperty_Integer( int prop_id,int delta )
    {
		// 同一个线程
		if ( gServerGlobal->getFramework()->get_call_service()==m_pContainer->get_id() )
		{
			return m_real_service ? m_real_service->addProperty_Integer( prop_id,delta ) : false;
		}

        BUILD_MSG_CONTEXT_2( IActorService::addProperty_Integer,int, prop_id,int ,delta );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual string  getProperty_String( int prop_id )
    {
		return m_real_service ? m_real_service->getProperty_String(prop_id) : "";
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  setProperty_String( int prop_id,string value )
    {
        obuf64 t_data;
        t_data << prop_id<<value;

		BUILD_MSG_BUFFER( IActorService::setProperty_String,t_data );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual string  getProperty_custom( int prop_id )
    {
		return m_real_service ? m_real_service->getProperty_custom(prop_id) : "";
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  setProperty_custom( int prop_id,string data )
    {
        obuf64 t_data;
        t_data << prop_id << data;

		BUILD_MSG_BUFFER( IActorService::setProperty_custom,t_data );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    virtual void updateTalentPoint( int nPoint, int nCHangeTYpe, BYTE byAddType = 2)
    {
		BUILD_MSG_CONTEXT_3( IActorService::updateTalentPoint,int ,nPoint, int ,nCHangeTYpe, BYTE ,byAddType);

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }


    //////////////////////////////////////////////////////////////////////////////////
    virtual float  getSight(  )
    {
		return m_real_service ? m_real_service->getSight() : 0.0f;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  setSight( float radii )
    {
        BUILD_MSG_CONTEXT_1( IActorService::setSight,float,radii );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual bool  isHide(  )
    {
		return m_real_service ? m_real_service->isHide() : false;
    };

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  setHide( bool state )
    {
        BUILD_MSG_CONTEXT( IActorService::setHide,bool );
		*pParam = state;

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    virtual void AddEnmityValue(UID uid, int nVal)
    {
        BUILD_MSG_CONTEXT_2( IActorService::AddEnmityValue,UID ,uid, int, nVal );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }
    virtual void AddBroadEnmityValue(UID uid, int nVal)
    {
        BUILD_MSG_CONTEXT_2( IActorService::AddBroadEnmityValue,UID ,uid, int, nVal );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }
    virtual void SetEnmityValue(UID uid, int nVal)
    {
        BUILD_MSG_CONTEXT_2( IActorService::SetEnmityValue,UID ,uid, int, nVal );
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    /////////////////////// IBuffPart ///////////////////////////////////////////////
    /** 给实体添加buff */	
    virtual bool AddBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL ,int nContextLen=0)
    {
        obuf128 t_data;
        t_data << dwBuffID<<dwBuffLevel<<uidAdd<<dwTime<<nContextLen;
        if (pBuffContext && nContextLen > 0)
        {
            t_data.push_back(pBuffContext, nContextLen);
        }

		BUILD_MSG_BUFFER( IActorService::AddBuff,t_data );

        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    /** 给实体移除buff */
    virtual bool RemoveBuff(DWORD dwBuffID, UID uidAdd, UID uidRemove)
    {
        BUILD_MSG_CONTEXT_3( IActorService::RemoveBuff,DWORD, dwBuffID, UID ,uidAdd, UID ,uidRemove );
        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    /** 某个BUFF是否存在 */
    virtual bool IsExistBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
    {
		if (uidAdd == INVALID_UID)
		{
			IBuffServer* pBuffServer = gServerGlobal->getBuffServer();
			int nAccQueryIndex = -1;
			// 如果配了查询加速 查询是线程安全的
			if (pBuffServer != NULL && (nAccQueryIndex = pBuffServer->getAccQueryIndex(dwBuffID, dwBuffLevel)) >= 0)
			{
				return m_real_service ? m_real_service->IsExistBuff(dwBuffID, dwBuffLevel, uidAdd) : false;
			}
		}

		SERVICE_PTR pRunning = gServerGlobal->getFramework()->get_running_service();
		if(pRunning != 0 && pRunning->get_id() != m_pContainer->get_id() && uidAdd == INVALID_UID)
		{
			//EmphasisLn("check buff exist,buffid=" << dwBuffID);
		}

        BUILD_MSG_CONTEXT_3( IActorService::IsExistBuff,DWORD, dwBuffID, DWORD ,dwBuffLevel, UID ,uidAdd );

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(bool);
        return bool();
    }

    /** 某个BUFF类型是否存在 */
    virtual bool IsExistBuffType(DWORD dwBuffType)
    {
        //BUILD_MSG_CONTEXT_1( IActorService::IsExistBuffType,DWORD,dwBuffType );

        //rkt::obuf resultBuf;
        //m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        //RETURN_FROM_MSG_BUF(bool);
        //return bool();
        return m_real_service ? m_real_service->IsExistBuffType(dwBuffType) : false;
    }

    /** 某个BUFF等级 */
    virtual DWORD GetBuffLevel(DWORD dwBuffID, UID uidAdd)
    {
        BUILD_MSG_CONTEXT_2( IActorService::GetBuffLevel,DWORD ,dwBuffID, UID, uidAdd );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(DWORD);
        return 0;
    }

    /** 某个BUFFBUFF剩余时间 */
    virtual int GetLeftTime(DWORD dwBuffID, UID uidAdd)
    {
        BUILD_MSG_CONTEXT_2( IActorService::GetLeftTime,DWORD ,dwBuffID, UID ,uidAdd );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return -1;
    }

    /** 设置累积BUFF */
    virtual void SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL ,int nContextLen=0)
    {
        obuf128 t_data;
        t_data <<dwBuffID<<uidAdd<<bOverlay<<dwTime<<nContextLen;
        if (pBuffContext && nContextLen > 0)
        {
            t_data.push_back(pBuffContext, nContextLen);
        }

        BUILD_MSG_BUFFER( IActorService::SetAccumulateBuff,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    /** 通过索引给实体移除buff */
    virtual bool RemoveBuffByIndex(DWORD dwIndex)
    {
         BUILD_MSG_CONTEXT_1( IActorService::RemoveBuffByIndex,DWORD ,dwIndex );
        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    /** 得到剩余冷却时间 */
    virtual DWORD GetFreezeLeftTime(DWORD dwClassID, DWORD dwFreezeID)
    {
        BUILD_MSG_CONTEXT_2( IActorService::GetFreezeLeftTime,DWORD ,dwClassID, DWORD, dwFreezeID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(DWORD);
        return 0;
    }

	/** 启动冷却 */
	virtual bool FreezeWork(DWORD dwClassID, DWORD dwFreezeID)
    {
        BUILD_MSG_CONTEXT_2( IActorService::FreezeWork,DWORD ,dwClassID, DWORD, dwFreezeID );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(bool);
        return false;
    }

	/** 设置冷却时间 */
	virtual bool SetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime, bool nIsCleanInterferedFreeze,bool bBreak)
	{
		BUILD_MSG_CONTEXT_5( IActorService::SetFreezeTime,DWORD ,dwClassID, DWORD, dwFreezeID, int, nTime, bool, nIsCleanInterferedFreeze, bool, bBreak);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);
		return false;
	}

    /** 得到来源世界ID */
    virtual DWORD getFromGameWorldID()
    {
		return m_real_service ? m_real_service->getFromGameWorldID() : 0;
    }

    virtual DWORD getUserID()
    {
		return m_real_service ? m_real_service->getUserID() : 0;
    }

    // 获取IdCard数据
    virtual DWORD getIdCard()
    {
        return m_real_service ? m_real_service->getIdCard() : 0;
    }

    // 是否为成年人
    virtual bool isAdult()
    {
        return m_real_service ? m_real_service->isAdult() : false;
    }

    // 设置为成年人
    virtual void setAdult() 
    {
        if(m_real_service) m_real_service->setAdult();
    }

    // 获得玩家角色等级
    virtual int getPlayerLevel()
    {
        return (m_real_service ? m_real_service->getPlayerLevel() : 0);
    }

    // 获得玩家角色经验
    virtual int getPlayerExp()
    {
        return (m_real_service ? m_real_service->getPlayerExp() : 0);
    }

	// 设置IdCard数据
	virtual void setIdCard(DWORD dwIdCard)
	{
		BUILD_MSG_CONTEXT_1(IActorService::setIdCard, DWORD, dwIdCard);
		m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
	}

    virtual bool Transport( Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason )
    {
        BUILD_MSG_CONTEXT_5( IActorService::Transport,Vector3 ,newLoc, Vector3, rotation, bool, bTransport, int, nSceneID, int, nReason);
        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
    }

	virtual bool MoveTo(Vector3 newLoc)
	{
        BUILD_MSG_CONTEXT_1( IActorService::MoveTo,Vector3 ,newLoc);
        return m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
	}

    // 获得临时天赋
    virtual void activeTempTalent(int nTalentID)
    {
        BUILD_MSG_CONTEXT_1( IActorService::activeTempTalent,int, nTalentID);
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
    }

    // 取消临时天赋
    virtual void deActiveTempTalent(int nTalentID)
    {
        BUILD_MSG_CONTEXT_1( IActorService::deActiveTempTalent,int, nTalentID);
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
    }

    virtual int getWarID()
    {
        return m_real_service ? m_real_service->getWarID() : INVALID_WAR_ID;
    }

    virtual int getMatchTypeID()
    {
        return m_real_service ? m_real_service->getMatchTypeID() : 0;
    }

    virtual BuffTypeList getBuff(DWORD dwBuffType)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getBuff,DWORD, dwBuffType);

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
        if ( resultBuf.good() && resultBuf.size() > 0  )
        {
            ibuffer t_in(resultBuf.data(),resultBuf.size());
            BuffTypeList vReturn;
            t_in >> vReturn;
            if ( t_in.good() ) return vReturn;
        }

        return BuffTypeList();     // 请检查默认返回值是否正确
    }

    virtual int getSmSkillId(int nSlot)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getSmSkillId,int, nSlot);

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
        RETURN_FROM_MSG_BUF(int);
        return int();
    }

    virtual bool isInsight( int camp )
    {
        BUILD_MSG_CONTEXT_1( IActorService::isInsight,int, camp);

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(bool);

        return bool();     // 请检查默认返回值是否正确
    }

    virtual bool chooseTarget(int nTargetFilter, UID uidTarget)
    {
        return m_real_service ? m_real_service->chooseTarget(nTargetFilter, uidTarget): false;
    };

    virtual int getPKType(void)
    {
        return m_real_service ? m_real_service->getPKType() : 0;
    };

    virtual bool checkPK(UID uidTarget)
    {
        return m_real_service ? m_real_service->checkPK(uidTarget) : false;
    };

    virtual int getPKState(void)
    {
        return m_real_service ? m_real_service->getPKState() : 0;
    };

    //强制结束PK状态
    virtual void  forceEndPK()
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }
        if (m_real_service)
        {
            m_real_service->forceEndPK();
            return;
        }

        BUILD_MSG_CONTEXT_VOID( IActorService::forceEndPK);
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

	virtual int getPKCamp()
	{
		return m_real_service ? m_real_service->getPKCamp() : 0;
	}

    // 机器人行为
    virtual void onEventRobotAction(BYTE nAction, void * data, size_t len)
    {
        obuf64 t_data;
        t_data << nAction << len;
        t_data.push_back(data, len);

		BUILD_MSG_BUFFER( IActorService::onEventRobotAction,t_data);

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    //////////////////////////////////////////////////////////////////////////////////
    virtual void  release(  )
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::release);

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    };

    virtual DWORD getMatchTypeHideRank(EMMatchType eMatchType)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getMatchTypeHideRank,EMMatchType,eMatchType);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(DWORD);
        return DWORD();
    }


    virtual DWORD getMatchTypeRank(EMMatchType eMatchType)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getMatchTypeRank,EMMatchType,eMatchType);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(DWORD);
        return DWORD();
    }

    virtual bool isMatchHeroCheckPassed(EMMatchType eMatchType)
    {
        BUILD_MSG_CONTEXT_1( IActorService::isMatchHeroCheckPassed,EMMatchType,eMatchType);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(bool);
        return bool();
    }

    virtual DWORD getMatchTypeRankGrade(EMMatchType eMatchType)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getMatchTypeRankGrade,EMMatchType,eMatchType);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(DWORD);
        return DWORD();
    }

    virtual void addMatchTypeRank(EMMatchType eMatchType,int nRankScore, int nHideRankScore)
    {
        BUILD_MSG_CONTEXT_3( IActorService::addMatchTypeRank,EMMatchType ,eMatchType,int, nRankScore, int, nHideRankScore);

        m_pContainer->post_message(  pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    virtual void setMatchTypeRank(EMMatchType eMatchType,int nRankScore, int nHideScore)
    {
        BUILD_MSG_CONTEXT_3( IActorService::setMatchTypeRank,EMMatchType ,eMatchType, int, nRankScore, int, nHideScore);

        m_pContainer->post_message(  pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }
   
    virtual void setMatchTypeGrade(EMMatchType eMatchType,int nGrade)
    {
        BUILD_MSG_CONTEXT_2( IActorService::setMatchTypeGrade,EMMatchType ,eMatchType,int, nGrade);

        m_pContainer->post_message(  pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    virtual void setMatchHeroScore(int nHeroID, int nHeroScore)
    {
        BUILD_MSG_CONTEXT_2( IActorService::setMatchHeroScore,int ,nHeroID, int, nHeroScore);

        m_pContainer->post_message(  pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    int getSummonerSkillInfo(int nSolt)
    {
        BUILD_MSG_CONTEXT_1( IActorService::getSummonerSkillInfo,int, nSolt);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return int();
    }
    
    // 设置玩家活跃度
    virtual void setDayActivity( int value, bool isSync = true)
    {
        BUILD_MSG_CONTEXT_2( IActorService::setDayActivity, int ,value, bool, isSync);

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    // 获取玩家活跃度
    virtual int getDayActivity(void)
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getDayActivity);

        rkt::obuf resultBuf;
        m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return int();
    }


    // 设置玩家周活跃度
    virtual void setWeekActivity(int value, bool isSync = true)
    {
        BUILD_MSG_CONTEXT_2(IActorService::setWeekActivity, int, value, bool, isSync);

        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    }

    // 获取玩家周活跃度
    virtual int getWeekActivity(void)
    {
        BUILD_MSG_CONTEXT_VOID(IActorService::getWeekActivity);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return int();
    }

    void setActorBaseInfo(LONG lgValue, int nType, int nMode = ENCHANGEDATAMODE_SET,BYTE byNumType=0, BYTE byOpType=0, WORD wApproach = 0, DWORD dwTargetID = 0, LPCSTR szLogMark = 0, int nLogReserve = 0)
    {
        BUILD_MSG_CONTEXT_3( IActorService::setActorBaseInfo,LONG ,lgValue, int, nType, int, nMode);
		rkt::obuf resultBuf;
		m_pContainer->post_message(  pMsg,nMsgLen,&resultBuf);
    }

    void setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey)
    {
        obuf256 t_data;
		t_data << nType;
		t_data << llWarDBKey;
		t_data.push_back(nData,nLen*sizeof(int) );

		BUILD_MSG_BUFFER( IActorService::setActorSummonerOrGlory,t_data );

        m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }


    virtual void setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode = ENCHANGEDATAMODE_SET, BYTE byNumType=0, BYTE byOpType=0, WORD wApproach=0, DWORD dwTargetID=0, LPCSTR szLogMark=0, int nLogReserve=0)
    {
        string strRemark = (szLogMark ? szLogMark : "");

        obuf256 t_data;
        t_data << nHeroID << lgValue << nType << nMode << byNumType << byOpType << wApproach << dwTargetID << nLogReserve<< strRemark;
        
		BUILD_MSG_BUFFER( IActorService::setHeroOneInfo,t_data );
		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    }

    SHeroInfo getHeroInfo( int nHeroID )
    {
        BUILD_MSG_CONTEXT_1( IActorService::getHeroInfo,int, nHeroID);
        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(SHeroInfo);
        return SHeroInfo();
    }

	SHeroInfo checkHeroInfo( int nHeroID )
	{
		BUILD_MSG_CONTEXT_1( IActorService::checkHeroInfo,int, nHeroID);
		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(SHeroInfo);
		return SHeroInfo();
	}

	// 获得所有英雄信息
	virtual string getHeroArray()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getHeroArray);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}

    // 获取全排位Rank数据
    virtual string getAllMatchTypeRank()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getAllMatchTypeRank);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        if (resultBuf.good() && resultBuf.size() > 0)
        {
            ibuffer t_in(resultBuf.data(), resultBuf.size());
            string vReturn;
            t_in >> vReturn;
            if (t_in.good())
                return vReturn;
        }

        return string();
    }

	// 获得当前玩家战场可用的英雄
	virtual string getPlayerCanUseHeroArray()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getPlayerCanUseHeroArray);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}


    int getHeroCount()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getHeroCount);
        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return int();
    }

    SPlayerInfo getPlayerInfo()
    {
		BUILD_MSG_CONTEXT_VOID( IActorService::getPlayerInfo);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        if ( resultBuf.good() && resultBuf.size() > 0 )
        {
            ibuffer t_in(resultBuf.data(), resultBuf.size());
            SPlayerInfo pReturn;
            t_in >> pReturn;
            if ( t_in.good() )
                return pReturn;
        }
        return SPlayerInfo();
    }

    void getPlayerRoomExData(SMatchPlayerRoomExData *pRoomExData)
    {

		if (m_real_service) m_real_service->getPlayerRoomExData(pRoomExData);
	}


    void setHeroExclusiveTalentEffect( int nHeroID )
    {
        BUILD_MSG_CONTEXT_1( IActorService::setHeroExclusiveTalentEffect,int,nHeroID);
        m_pContainer->post_message(pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    void removeHeroExclusiveTalentEffect()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::removeHeroExclusiveTalentEffect);
        m_pContainer->post_message(pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    bool addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1)
    {
        BUILD_MSG_CONTEXT_3( IActorService::addHeroCardInfo,DWORD*, dwCardID, int*, nValue, int, nCount);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(bool);
        return true;
    }

    void AddTitle(int nTitleID, char * szTitleName, int nTitleNameLen, bool bCoverTitle)
    {
        obuf128 t_data;
        t_data << nTitleID<<bCoverTitle<<nTitleNameLen;
        if (szTitleName && nTitleNameLen > 0)
        {
            t_data.push_back(szTitleName, nTitleNameLen);
        }

        BUILD_MSG_BUFFER( IActorService::AddTitle,t_data );

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    void deleteTitle(int nTitleID)
    {
        BUILD_MSG_CONTEXT_1( IActorService::deleteTitle,int, nTitleID);
        m_pContainer->post_message(pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }


    void updateMapInfo( int nMapID, bool bIsWin )
    {
        BUILD_MSG_CONTEXT_2( IActorService::updateMapInfo,int, nMapID, bool, bIsWin);
        m_pContainer->post_message(pMsg,nMsgLen, 0);
    }

    DWORD getFirstWinTime()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getFirstWinTime);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
        if ( resultBuf.good() && resultBuf.size()==sizeof(DWORD)  )
        {
            return *(DWORD*)resultBuf.data();
        }
        return DWORD();
    }

    void setPlayerFirstWinFlag( DWORD dwTime)
    {
        BUILD_MSG_CONTEXT_1( IActorService::setPlayerFirstWinFlag,DWORD,dwTime);
        m_pContainer->post_message(pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
    }

    int getPlayerFightCapacity()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getPlayerFightCapacity);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		RETURN_FROM_MSG_BUF(int);

        return int();
	}

	// 设置任务数据
	virtual bool addTaskDataInt(int nKey, int nData)
	{
		BUILD_MSG_CONTEXT_2( IActorService::addTaskDataInt,int ,nKey, int, nData);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

        RETURN_FROM_MSG_BUF(bool);

		return bool();
	}

	// 删除任务数据
	virtual bool removeTaskDataInt(int nKey)
	{
		BUILD_MSG_CONTEXT_1( IActorService::removeTaskDataInt,int, nKey);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

        RETURN_FROM_MSG_BUF(bool);

        return bool();
	}

	// 获取任务数据
	virtual int getTaskDataInt(int nKey)
	{
        BUILD_MSG_CONTEXT_1( IActorService::getTaskDataInt,int, nKey);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		RETURN_FROM_MSG_BUF(int);
		return 0;
	}

    // 获取英雄的XP技能ID
    virtual int getXPSKillID(int nHeroID)
	{
        BUILD_MSG_CONTEXT_1( IActorService::getXPSKillID,int, nHeroID);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		RETURN_FROM_MSG_BUF(int);
		return 0;
	}

    // 注册任务数据
    virtual bool declareTaskData(int nKey, bool bIsInt, bool bSave)
    {
        BUILD_MSG_CONTEXT_3( IActorService::declareTaskData, int, nKey, bool, bIsInt, bool, bSave);
        
        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

        RETURN_FROM_MSG_BUF(bool);

        return bool();
    }

    // 获取当前天赋页
    virtual int getCurTalentPage()
    {
		return m_real_service ? m_real_service->getCurTalentPage() : -1;
    }

    // 获取当前符文页
    virtual int getCurRunePage()
    {
		return m_real_service ? m_real_service->getCurRunePage() : -1;
    }

	// 获取当前符文数据
	virtual string getOtherRuneData()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getOtherRuneData);

		rkt::obuf resultBuf;
		 m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}

	// 获取当前符文数据
	virtual string getOtherCrownPageData()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getOtherCrownPageData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}

	// 获取其他玩家当前宝石数据
	virtual string getOtherCrownGemstoneData()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getOtherCrownGemstoneData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}

	// 获取最强英雄信息
	virtual string getOtherStrongHeroInfo()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getOtherStrongHeroInfo);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			string vReturn;
			t_in >> vReturn;
			if (t_in.good())
				return vReturn;
		}

		return string();
	}

	// 是否可以给角色奖励
	bool canPrizeActor(int nPrizeID, int nNum)
	{
		BUILD_MSG_CONTEXT_2( IActorService::canPrizeActor,int,nPrizeID, int,nNum);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

	// 给角色奖励
	bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		obuf256 t_data;
		t_data << nPrizeID;
		t_data << nNum;
		t_data << wApproach;
		t_data << dwTargetID;
		t_data << szLogDesc;

		BUILD_MSG_BUFFER( IActorService::prizeActor,t_data);

		rkt::obuf resultBuf;
       m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		RETURN_FROM_MSG_BUF(bool);


		return false;
	}

	// 给角色奖励
	bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		obuf256 t_data;
		t_data << nPrizeGroupID;
		t_data << pOutPrizeID;
		t_data << nOutPrizeCount;
		t_data << wApproach;
		t_data << dwTargetID;
		t_data << szLogDesc;

		BUILD_MSG_BUFFER( IActorService::groupPrizeActor,t_data);

		rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		
		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

	// 添加宝箱
	bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		obuf256 t_data;
		t_data << nChestType;
		t_data << nNum;
		t_data << wApproach;
		t_data << dwTargetID;
		t_data << szLogDesc;

		BUILD_MSG_BUFFER( IActorService::addPrizeChest,t_data);

		rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

    // 实体通知主人
    virtual void entityNotifyMaster(EntityBelongContext context)
    {
        BUILD_MSG_CONTEXT_1( IActorService::entityNotifyMaster,EntityBelongContext,context);
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

	virtual void setActorTeammateHeroID(int* pTeammateHeroID)
	{
		BUILD_MSG_CONTEXT_1( IActorService::setActorTeammateHeroID,int*,pTeammateHeroID );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	}

	// 获得随机卡牌
	virtual int getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam)
	{
		BUILD_MSG_CONTEXT_5( IActorService::getRandHeroCard,DWORD*,pCardIDList,int*,pCardNumList,int,nCardListSize,WORD,wApproach,int,nParam);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);

		RETURN_FROM_MSG_BUF(int);
		return 0;
	}

	// 玩家读条
	virtual void StartProgressBar( ProgressBarContext* pContext )
	{
		BUILD_MSG_CONTEXT_1( IActorService::StartProgressBar,ProgressBarContext*,pContext);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		return;
	}

    virtual int getMapID(void)
    {
        return m_real_service ? m_real_service->getMapID(): INVALID_MAPID;
    }

	virtual void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple)
	{
		BUILD_MSG_CONTEXT_3( IActorService::calcBenefitCardMultiple, int*, nGoldMultiple, int*, nActorExpMultiple, int*, nHeroExpMultiple);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	}

    // 冲锋
    virtual void shift(void *pData,int nDataLen)
    {
        obuf128 t_data;
        t_data << nDataLen;
        if (pData != NULL && nDataLen > 0)
        {
            t_data.push_back(pData, nDataLen);
        }

        BUILD_MSG_BUFFER(IActorService::shift, t_data);

        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    }

	// 检查玩家能否使用英雄皮肤
	virtual bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID)
	{
		BUILD_MSG_CONTEXT_2( IActorService::canUseHeroSkin, DWORD, dwHeroID, DWORD, dwHeroSkinID );

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

		RETURN_FROM_MSG_BUF(bool);

		return bool();
    }

    virtual DWORD getPKTotalNum()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getPKTotalNum);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);

        RETURN_FROM_MSG_BUF(DWORD);
        return 0;
    }

    virtual DWORD getTotalMatchNum()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getTotalMatchNum);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);

        RETURN_FROM_MSG_BUF(DWORD);
        return 0;
    }

	///////////////////////IMailPart///////////////////////////////////////////////////
	//// 处理邮件详细数据
	//virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus)
	//{
	//	BUILD_MSG_CONTEXT_2( IActorService::dealPlayerMailDetailData,LONGLONG, llMailID,bool, isAutoRecvPlus );

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

	//	RETURN_FROM_MSG_BUF(bool);

	//	return false;
	//}

	//// 添加邮件详细数据
	//virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus)
	//{
	//	BUILD_MSG_CONTEXT_2( IActorService::addPlayerMailDetailData,SMailDetailData, sDetailData,bool, isAutoRecvPlus);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	//	return;
	//}

	// 处理邮件发送附件数据
	virtual bool dealSendMailPlusData(SMailSendData sendData)
	{
		BUILD_MSG_CONTEXT_1( IActorService::dealSendMailPlusData,SMailSendData, sendData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

		RETURN_FROM_MSG_BUF(bool);

		return false;
	}

	//// 获取领取附件时间
	//virtual DWORD getLastRecvPlusTime()
	//{
	//	BUILD_MSG_CONTEXT_VOID( IActorService::getLastRecvPlusTime);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);

	//	RETURN_FROM_MSG_BUF(DWORD);
	//	return 0;
	//}

	//// 设置领取附件时间
	//virtual void setLastRecvPlusTime(DWORD dwTime)
	//{
	//	BUILD_MSG_CONTEXT_1( IActorService::setLastRecvPlusTime,DWORD,dwTime );

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	//}

	//// 邮件是否已领取
	//virtual bool isAreadyRecvMail(LONGLONG llMailID)
	//{
	//	BUILD_MSG_CONTEXT_1( IActorService::isAreadyRecvMail,LONGLONG, llMailID);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

	//	RETURN_FROM_MSG_BUF(bool);

	//	return false;
	//}

	//// 是否在快速取件
	//virtual bool isFastRecvMailPlus()
	//{
	//	BUILD_MSG_CONTEXT_VOID( IActorService::isFastRecvMailPlus);

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);

	//	RETURN_FROM_MSG_BUF(bool);
	//	return false;
	//}

	//// 设置快速取件标识
	//virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount)
	//{
	//	BUILD_MSG_CONTEXT_2( IActorService::setFastRecvMailPlus,bool, bFlag,int, nRecvCount );

	//	rkt::obuf resultBuf;
	//	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	//}
	///////////////////////IMailPart end///////////////////////////////////////////////////

	// 建立师徒关系
	virtual void buildMentorship(DWORD dwDesPDBID)
	{
		BUILD_MSG_CONTEXT_1( IActorService::buildMentorship,DWORD, dwDesPDBID );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	}

	// 发送任务事件
	virtual void sendActorDataTaskEvent(WORD nTaskEvent, int nParam1, int nParam2, int nParam3)
	{
		BUILD_MSG_CONTEXT_4( IActorService::sendActorDataTaskEvent,WORD, nTaskEvent,int, nParam1,int, nParam2,int, nParam3 );

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	}

	// 获取永久英雄个数
	virtual int getPermanentHeroCount()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getPermanentHeroCount);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

	// 获取皮肤总个数
	virtual int getTotalSkinCount()
	{
		BUILD_MSG_CONTEXT_VOID( IActorService::getTotalSkinCount);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

    // 获取静态地图皮肤ID
    virtual int getStaticSkin()
    {
        BUILD_MSG_CONTEXT_VOID( IActorService::getStaticSkin);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

        RETURN_FROM_MSG_BUF(int);

        return 0;
    }

	// 获取达到等级英雄个数
	virtual int getHeroCountByLevel(WORD nLevel)
	{
		BUILD_MSG_CONTEXT_1( IActorService::getHeroCountByLevel,WORD, nLevel);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen, &resultBuf);

		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

    // 保存引导步骤
    virtual void saveGuideStep(int nStep) 
    {
        BUILD_MSG_CONTEXT_1( IActorService::saveGuideStep, int, nStep );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    }

	// 设置实名认证状态标志
	virtual void setRealNameAutherFlag() 
	{
		if(m_real_service) m_real_service->setRealNameAutherFlag();
	}

	// 购买商城物品
	virtual bool onMessagePointShopBuyGoods(void *data, size_t len)
	{
		if (data == nullptr || len == 0)
			return false;

		obuf t_data;
		t_data << len;
		t_data.push_back(data, len);
		BUILD_MSG_BUFFER(IActorService::onMessagePointShopBuyGoods, t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(bool);
		return false;
	}

	// 战场结束设置数据
	virtual void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo)
	{
		BUILD_MSG_CONTEXT_1( IActorService::setMatchRankSeasonInfo, SRankSeasonWarEndInfo, pInfo);

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	}

	// 通知LUA添加交互回复
	virtual void sendLuaAddContactReply(LONGLONG llContactID)
	{
		BUILD_MSG_CONTEXT_1( IActorService::sendLuaAddContactReply, LONGLONG, llContactID );

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	}

	// 通知LUA添加好友申请
	virtual void sendLuaContactApplyFriend(DWORD dwTargetID, bool bIsDifSex)
	{
		BUILD_MSG_CONTEXT_2( IActorService::sendLuaContactApplyFriend, DWORD, dwTargetID, bool, bIsDifSex );

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	}

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect( int nEffectID, void * pAddContext, size_t len)
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return 0;
        }

        if (pAddContext == nullptr || len == 0)
            return 0;

        obuf t_data;
        t_data<<nEffectID << len;
        t_data.push_back(pAddContext, len);
        BUILD_MSG_BUFFER(IActorService::mobaGoodsAddEffect, t_data);

        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
        RETURN_FROM_MSG_BUF(int);
        return int();     // 请检查默认返回值是否正确    
    }

    // 移除效果
    virtual	void mobaGoodsRemoveEffect( int nID)
    {
        if(m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

        BUILD_MSG_CONTEXT_1( IActorService::mobaGoodsRemoveEffect, int ,nID);
        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
    }

    // 设置核晶效果
    virtual	void setCrownEffect(int timeToAddEffect)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return;
        }

        BUILD_MSG_CONTEXT_1(IActorService::setCrownEffect, int, timeToAddEffect);
        m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
    }

    // 购买装备
    virtual	bool luaBuyGoods(int nGoodsID, int nCost)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return false;
        }

        BUILD_MSG_CONTEXT_2(IActorService::luaBuyGoods, int, nGoodsID, int, nCost);
        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
        RETURN_FROM_MSG_BUF(bool);
        return bool();
    }

    // 切换职业
    virtual bool changeVoc(int nNewVoc, int nNewSkinID)
    {
        if (m_pContainer == 0)
        {
            WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
            return false;
        }

        BUILD_MSG_CONTEXT_2(IActorService::changeVoc, int, nNewVoc, int, nNewSkinID);
        rkt::obuf resultBuf;
        m_pContainer->post_message(pMsg, nMsgLen, &resultBuf, MSG_FLAG_NO_BLOCK);

        return true;
    }
};

/** 
存根对象:
1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class ActorService_Stub : public IServiceStub
{
    ActorService_Proxy  m_proxy;
    ActorService *      m_real_service;
    bool                m_stoped;

public:
    ActorService_Stub(ActorService * pReal) : m_proxy(pReal),m_real_service(pReal), m_stoped(false){
    }

    // 启动
    virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
    {
        m_proxy.m_pContainer = pContainer;

        return m_real_service->on_start(pContainer,data,len);
    }

    // 停止
    virtual bool on_stop()
    {
        m_real_service->on_stop();
        m_stoped = true;

        return true;
    }

    // 处理消息
    virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
    {
        if(m_stoped) return false;

        if ( TEST_SAME_FUNCTION(IActorService::setLocation) )
        {
            Vector3 vLoc;
			GET_MSG_PARAM_1(Vector3,vLoc);

            m_real_service->setLocation(vLoc);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::handleMessage) )
        {
			m_real_service->handleMessage(pMsg->context,pMsg->context_len);			
            return true;
        }
        if ( TEST_SAME_FUNCTION(IActorService::changeState) )
        {
            int nState;
            GET_MSG_PARAM_1(int,nState);
            m_real_service->changeState(nState);
            return true;
        }
        if ( TEST_SAME_FUNCTION(IActorService::getState) )
        {			
            const int & vReturn= m_real_service->getState();
            if ( resultBuf ) *resultBuf << vReturn;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::setGameState))
        {
            DWORD dwState;
            GET_MSG_PARAM_1(DWORD,dwState);
            m_real_service->setGameState(dwState);
            return true;
        }
        if (TEST_SAME_FUNCTION(IActorService::getGameState))
        {
            if ( resultBuf ) *resultBuf <<  m_real_service->getGameState();
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::exportActorData) )
        {
            int nFlag = TO_INVALID; bool bFromBank = false;
            GET_MSG_PARAM_2(int,nFlag,bool,bFromBank);
            if ( resultBuf ) *resultBuf << m_real_service->exportActorData(nFlag, bFromBank);
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::exportShareProperties))
        {
            if ( resultBuf ) *resultBuf << m_real_service->exportShareProperties();
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::getCreateContextFlag))
        {
            BYTE nFromActorType; BYTE nToActorType; BYTE nFlag;
            GET_MSG_PARAM_3(BYTE ,nFromActorType, BYTE, nToActorType, BYTE, nFlag);
            if ( resultBuf ) *resultBuf << m_real_service->getCreateContextFlag(nFromActorType, nToActorType, nFlag);
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::isTester))
        {
            if ( resultBuf ) *resultBuf << m_real_service->isTester();
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::addExp))
        {
            ActorService_Proxy::addExp_Param * pParam = 0;
			GET_MSG_PARAM(ActorService_Proxy::addExp_Param,pParam);

			m_real_service->addExp(pParam->nExp, pParam->wApproach, pParam->dwTargetID, pParam->pszRemark,pParam->nLogReserve);
            return true;
        }
        if (TEST_SAME_FUNCTION(IActorService::canAddMoney))
        {
            int nTicket; int nGlod;
            GET_MSG_PARAM_2( int,nTicket,int,nGlod);
            if ( resultBuf ) *resultBuf <<m_real_service->canAddMoney(nTicket,nGlod);
            return true;
        }
        if (TEST_SAME_FUNCTION(IActorService::addMoney))
        {
            ActorService_Proxy::addMoney_Param * pParam = 0;
			GET_MSG_PARAM(ActorService_Proxy::addMoney_Param,pParam);
            if ( resultBuf ) *resultBuf <<m_real_service->addMoney(pParam->nTicket, pParam->nGlod, pParam->nReason, pParam->pszRemark, pParam->nSubclassID, pParam->nNum, pParam->bAddictLimit);;
            return true;
        }
        

        if ( TEST_SAME_FUNCTION(IActorService::setProperty_Bool) )
        {
            int prop_id;bool value;
            GET_MSG_PARAM_2(int ,prop_id,bool, value);
            bool ret = m_real_service->setProperty_Bool(prop_id,value);
            if ( resultBuf ) *resultBuf <<ret;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setProperty_Integer) )
        {
            int prop_id;int value;
            GET_MSG_PARAM_2(int ,prop_id, int, value);
            const bool & vReturn= m_real_service->setProperty_Integer(prop_id,value);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }
        if ( TEST_SAME_FUNCTION(IActorService::addProperty_Integer) )
        {
            int prop_id;int delta;
            GET_MSG_PARAM_2(int ,prop_id,int, delta);
            const bool & vReturn= m_real_service->addProperty_Integer(prop_id,delta);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setProperty_String) )
        {
            int prop_id;string value;
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in >> prop_id>>value;
            const bool & vReturn= m_real_service->setProperty_String(prop_id,value);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setProperty_custom) )
        {
            int prop_id;string data;
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in >> prop_id>>data;
            const bool & vReturn= m_real_service->setProperty_custom(prop_id,data);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION( IActorService::updateTalentPoint) )
        {
            int nPoint,nCHangeTYpe;
            BYTE byAddType;
            GET_MSG_PARAM_3(int ,nPoint,int, nCHangeTYpe,BYTE,byAddType);
            m_real_service->updateTalentPoint(nPoint, nCHangeTYpe, byAddType);
            return true;
        }


        if ( TEST_SAME_FUNCTION(IActorService::setSight) )
        {
            float radii;
            GET_MSG_PARAM_1(float ,radii);
            m_real_service->setSight(radii);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setHide) )
        {
            bool state;
            GET_MSG_PARAM_1(bool ,state);
            m_real_service->setHide(state);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::AddEnmityValue) )
        {
            UID uid;int nVal;
            GET_MSG_PARAM_2(UID ,uid,int, nVal);
            m_real_service->AddEnmityValue(uid,nVal);
            return true;
        }
        if ( TEST_SAME_FUNCTION(IActorService::AddBroadEnmityValue) )
        {
            UID uid;int nVal;
            GET_MSG_PARAM_2(UID ,uid,int, nVal);
            m_real_service->AddBroadEnmityValue(uid,nVal);
            return true;
        }
        if ( TEST_SAME_FUNCTION(IActorService::SetEnmityValue) )
        {
            UID uid;int nVal;
            GET_MSG_PARAM_2(UID ,uid,int, nVal);
            m_real_service->SetEnmityValue(uid,nVal);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::AddBuff) )
        {
            DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd; DWORD dwTime; int nContextLen = 0;			
            rkt::ibuffer in(pMsg->context,pMsg->context_len);
			in >> dwBuffID>>dwBuffLevel>>uidAdd>>dwTime>>nContextLen;
            const bool & vReturn = m_real_service->AddBuff(dwBuffID, dwBuffLevel, uidAdd, dwTime, in.current(), nContextLen);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::RemoveBuff) )
        {
            DWORD dwBuffID; UID uidAdd; UID uidRemove;	
            GET_MSG_PARAM_3(DWORD ,dwBuffID,UID ,uidAdd,UID ,uidRemove);
            const bool & vReturn = m_real_service->RemoveBuff(dwBuffID, uidAdd, uidRemove);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::IsExistBuff) )
        {
            DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd;			
            GET_MSG_PARAM_3(DWORD ,dwBuffID,DWORD, dwBuffLevel,UID ,uidAdd);
            const bool & vReturn = m_real_service->IsExistBuff(dwBuffID, dwBuffLevel, uidAdd);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        //if ( TEST_SAME_FUNCTION(IActorService::IsExistBuffType) )
        //{
        //    DWORD dwBuffType;			
        //    GET_MSG_PARAM_1(DWORD ,dwBuffType);
        //    const bool & vReturn = m_real_service->IsExistBuffType(dwBuffType);
        //    if ( resultBuf ) *resultBuf <<vReturn;
        //    return true;
        //}

        if ( TEST_SAME_FUNCTION(IActorService::GetBuffLevel) )
        {
            DWORD dwBuffID; UID uidAdd;			
            GET_MSG_PARAM_2(DWORD ,dwBuffID, UID ,uidAdd);
            const DWORD & vReturn = m_real_service->GetBuffLevel(dwBuffID, uidAdd);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::GetLeftTime) )
        {
            DWORD dwBuffID; UID uidAdd;			
            GET_MSG_PARAM_2(DWORD ,dwBuffID, UID ,uidAdd);
            const int & vReturn = m_real_service->GetLeftTime(dwBuffID, uidAdd);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::SetAccumulateBuff) )
        {
            DWORD dwBuffID; UID uidAdd; bool bOverlay; DWORD dwTime; int nContextLen = 0;
            rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in >> dwBuffID>>uidAdd>>bOverlay>>dwTime>>nContextLen;
            m_real_service->SetAccumulateBuff(dwBuffID, uidAdd, bOverlay, dwTime, in.current(), nContextLen);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::RemoveBuffByIndex) )
        {
            DWORD dwIndex;		
            GET_MSG_PARAM_1(DWORD ,dwIndex);
            m_real_service->RemoveBuffByIndex(dwIndex);
            return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::GetFreezeLeftTime) )
		{
			DWORD dwClassID; DWORD dwFreezeID;	
			GET_MSG_PARAM_2(DWORD ,dwClassID, DWORD, dwFreezeID);
			const DWORD & vReturn = m_real_service->GetFreezeLeftTime(dwClassID, dwFreezeID);
			if ( resultBuf ) *resultBuf <<vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::FreezeWork) )
		{
			DWORD dwClassID; DWORD dwFreezeID;	
			GET_MSG_PARAM_2(DWORD ,dwClassID, DWORD, dwFreezeID);
			const bool & vReturn = m_real_service->FreezeWork(dwClassID, dwFreezeID);
			if ( resultBuf ) *resultBuf <<vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::SetFreezeTime) )
		{
			DWORD dwClassID; DWORD dwFreezeID; int nTime; bool nIsCleanInterferedFreeze;bool bBreak;
			GET_MSG_PARAM_5(DWORD ,dwClassID, DWORD, dwFreezeID, int, nTime, bool, nIsCleanInterferedFreeze, bool,bBreak);
			const bool & vReturn = m_real_service->SetFreezeTime(dwClassID, dwFreezeID, nTime, nIsCleanInterferedFreeze,bBreak);
			if ( resultBuf ) *resultBuf <<vReturn;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::Transport) )
        {
            Vector3 newLoc; Vector3 rotation; bool bTransport; int nSceneID; int nReason; 
            GET_MSG_PARAM_5(Vector3 ,newLoc,Vector3, rotation, bool ,bTransport, int, nSceneID, int, nReason);
            m_real_service->Transport(newLoc, rotation, bTransport, nSceneID, nReason);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::MoveTo) )
        {
            Vector3 newLoc; 
            GET_MSG_PARAM_1(Vector3 ,newLoc);
            m_real_service->MoveTo(newLoc);
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::activeTempTalent))
        {
            int nTalentID;
            GET_MSG_PARAM_1(int ,nTalentID);
            m_real_service->activeTempTalent(nTalentID);
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::deActiveTempTalent))
        {
            int nTalentID;
            GET_MSG_PARAM_1(int ,nTalentID);
            m_real_service->deActiveTempTalent(nTalentID);
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::getWarID))
        {
            int vReturn = m_real_service->getWarID();
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::getMatchTypeID))
        {
            int vReturn = m_real_service->getMatchTypeID();
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::getBuff))
        {
            DWORD dwBuffType;
            GET_MSG_PARAM_1(DWORD ,dwBuffType);

            BuffTypeList vReturn = m_real_service->getBuff(dwBuffType);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::getSmSkillId))
        {
            int nSlot;
            GET_MSG_PARAM_1(int ,nSlot);
            int vReturn = m_real_service->getSmSkillId( nSlot );
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if(TEST_SAME_FUNCTION(IActorService::isInsight))
        {
            int nCamp;
            GET_MSG_PARAM_1(int ,nCamp);
            bool vReturn = m_real_service->isInsight(nCamp);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::onEventRobotAction) )
        {
            BYTE nAction; size_t len ;
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in >> nAction >> len;
            m_real_service->onEventRobotAction(nAction, in.current(), len);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::release) )
        {
            m_real_service->release();
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getMatchTypeRank))
        {
            EMMatchType eMatchType;
            GET_MSG_PARAM_1(EMMatchType ,eMatchType);
            DWORD vReturn = m_real_service->getMatchTypeRank(eMatchType);
			if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getMatchTypeHideRank))
        {
            EMMatchType eMatchType;
            GET_MSG_PARAM_1(EMMatchType ,eMatchType);
            DWORD vReturn = m_real_service->getMatchTypeHideRank(eMatchType);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::isMatchHeroCheckPassed))
        {
            EMMatchType eMatchType;
            GET_MSG_PARAM_1(EMMatchType ,eMatchType);
            bool vReturn = m_real_service->isMatchHeroCheckPassed(eMatchType);
			if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getMatchTypeRankGrade))
        {
            EMMatchType eMatchType;
            GET_MSG_PARAM_1(EMMatchType ,eMatchType);
            DWORD vReturn = m_real_service->getMatchTypeRankGrade(eMatchType);
			if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::addMatchTypeRank))
        {
            EMMatchType eMatchType;
            int nRankScore;
            int nHideRankScore;
            GET_MSG_PARAM_3(EMMatchType ,eMatchType,int ,nRankScore,int, nHideRankScore);
            m_real_service->addMatchTypeRank(eMatchType, nRankScore, nHideRankScore);
            return  /*m_real_service->addMatchTypeRank(eMatchType, nRankScore);*/true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setMatchTypeRank))
        {
            EMMatchType eMatchType;
            int nRankScore;
            int nHideRankScore;
            GET_MSG_PARAM_3(EMMatchType ,eMatchType,int ,nRankScore, int, nHideRankScore);
            m_real_service->setMatchTypeRank(eMatchType, nRankScore, nHideRankScore);
            return  /*m_real_service->addMatchTypeRank(eMatchType, nRankScore);*/true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setMatchTypeGrade))
        {
            EMMatchType eMatchType;
            int nGrade;
            GET_MSG_PARAM_2(EMMatchType ,eMatchType,int ,nGrade);
            m_real_service->setMatchTypeGrade(eMatchType, nGrade);
            return  /*m_real_service->addMatchTypeRank(eMatchType, nRankScore);*/true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setMatchHeroScore))
        {
            int nHeroID; 
            int nHeroScore;

            GET_MSG_PARAM_2(int ,nHeroID,int ,nHeroScore);
            m_real_service->setMatchHeroScore(nHeroID, nHeroScore);
            return  /*m_real_service->setMatchHeroScore(nHeroID, nHeroScore);*/true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getSummonerSkillInfo))
        {
            int nIndex = 0;
            GET_MSG_PARAM_1(int ,nIndex);
            int vReturn =m_real_service->getSummonerSkillInfo(nIndex);
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION( IActorService::setDayActivity) )
        {
            int value;  bool isSync;
            GET_MSG_PARAM_2(int ,value, bool, isSync);
            m_real_service->setDayActivity(value, isSync);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getDayActivity) )
        {
            const int & vReturn= m_real_service->getDayActivity();
            if ( resultBuf ) *resultBuf <<vReturn;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::setWeekActivity))
        {
            int value; bool isSync;
            GET_MSG_PARAM_2(int, value, bool, isSync);
            m_real_service->setWeekActivity(value, isSync);
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::getWeekActivity))
        {
            const int & vReturn = m_real_service->getWeekActivity();
            if (resultBuf) *resultBuf << vReturn;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setActorBaseInfo))
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            LONG lgValue; int nType; int nMode; BYTE byNumType; BYTE byOpType; WORD wApproach; DWORD dwTargetID; string strRemark; int nLogReserve;
            in >> lgValue >> nType >> nMode >> byNumType >> byOpType >> wApproach >> dwTargetID >> nLogReserve >> strRemark;
            m_real_service->setActorBaseInfo( lgValue, nType, nMode, byNumType, byOpType, wApproach, dwTargetID, strRemark.c_str(), nLogReserve);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setActorSummonerOrGlory))
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
			int nType;
			long long llWarDBKey;
			in >> nType >> llWarDBKey;

			if ( !in) return false;
			m_real_service->setActorSummonerOrGlory((int*)in.current(),in.space(), nType,llWarDBKey);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setHeroOneInfo))
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len);
            int nHeroID; LONG lgValue; int nType; int nMode; BYTE byNumType; BYTE byOpType; WORD wApproach; DWORD dwTargetID; string strRemark; int nLogReserve;
            in >> nHeroID >> lgValue >> nType >> nMode >> byNumType >> byOpType >> wApproach >> dwTargetID >> nLogReserve >> strRemark;
            m_real_service->setHeroOneInfo( nHeroID, lgValue, nType, nMode, byNumType, byOpType, wApproach, dwTargetID, strRemark.c_str(), nLogReserve);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getHeroInfo))
        {
            int nHeroID = 0;
            GET_MSG_PARAM_1(int, nHeroID);
            SHeroInfo pResult = m_real_service->getHeroInfo(nHeroID);
            if ( resultBuf ) *resultBuf <<pResult;
            return true;
        }

		if ( TEST_SAME_FUNCTION(IActorService::checkHeroInfo))
		{
			int nHeroID = 0;
			GET_MSG_PARAM_1(int, nHeroID);
			SHeroInfo pResult = m_real_service->checkHeroInfo(nHeroID);
			if ( resultBuf ) *resultBuf <<pResult;
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::getHeroArray))
		{
			string nResult = m_real_service->getHeroArray();
			if (resultBuf) *resultBuf << nResult;
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::getPlayerCanUseHeroArray))
		{
			string nResult = m_real_service->getPlayerCanUseHeroArray();
			if (resultBuf) *resultBuf << nResult;
			return true;
		}

        if (TEST_SAME_FUNCTION(IActorService::getAllMatchTypeRank))
        {
            string nResult = m_real_service->getAllMatchTypeRank();
            if (resultBuf) *resultBuf << nResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getHeroCount))
        {
            const int& nResult = m_real_service->getHeroCount();
            if ( resultBuf ) *resultBuf <<nResult;
            return true;
        }
        
        if ( TEST_SAME_FUNCTION(IActorService::getPlayerInfo))
        {
            SPlayerInfo pResult = m_real_service->getPlayerInfo();
            if ( resultBuf ) *resultBuf <<pResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getPlayerRoomExData))
        {
            SMatchPlayerRoomExData *pRoomExData = NULL;
            GET_MSG_PARAM_1(SMatchPlayerRoomExData *, pRoomExData);
            m_real_service->getPlayerRoomExData(pRoomExData);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::AddTitle))
        {
            int  nTitleID;
            char* szTitleName;
            int nTitleNameLen;
            bool bCoverTitle;
	
            rkt::ibuffer in(pMsg->context,pMsg->context_len);
            in >> nTitleID >> bCoverTitle >> nTitleNameLen;
            m_real_service->AddTitle(nTitleID, in.current(), nTitleNameLen, bCoverTitle);

            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::deleteTitle))
        {
            int  nTitleID;

            GET_MSG_PARAM_1(int ,nTitleID);
            m_real_service->deleteTitle(nTitleID);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setHeroExclusiveTalentEffect))
        {
            int nHeroID = 0;
            GET_MSG_PARAM_1(int, nHeroID);

            m_real_service->setHeroExclusiveTalentEffect(nHeroID);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::removeHeroExclusiveTalentEffect))
        {
            m_real_service->removeHeroExclusiveTalentEffect();
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::addHeroCardInfo))
        {
            DWORD* pCardID = NULL;
            int* pValue = NULL;
            int nCount = 0;
            GET_MSG_PARAM_3(DWORD*, pCardID,int*,pValue,int,nCount);
            bool bResult = m_real_service->addHeroCardInfo(pCardID, pValue, nCount);
            if ( resultBuf ) *resultBuf <<bResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::updateMapInfo))
        {
            int nMapID;
            bool bIsWin;
            GET_MSG_PARAM_2(int, nMapID,bool,bIsWin);

            m_real_service->updateMapInfo(nMapID, bIsWin);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getFirstWinTime))
        {
            
            DWORD dwResult = m_real_service->getFirstWinTime();
            if ( resultBuf ) *resultBuf <<dwResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setPlayerFirstWinFlag))
        {
            DWORD dwTime = 0;
            GET_MSG_PARAM_1(DWORD,dwTime);
            m_real_service->setPlayerFirstWinFlag(dwTime);
            return true;
		}

        //强制结束PK状态
        if ( TEST_SAME_FUNCTION(IActorService::forceEndPK))
        {
            m_real_service->forceEndPK();
            return true;
        }

		if ( TEST_SAME_FUNCTION(IActorService::getPlayerFightCapacity))
		{
			int nResult = m_real_service->getPlayerFightCapacity();
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::addTaskDataInt))
		{
			int nKey, nData;
			GET_MSG_PARAM_2(int,nKey,int,nData);
			bool nResult = m_real_service->addTaskDataInt(nKey, nData);
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::removeTaskDataInt))
		{
			int nKey;
			GET_MSG_PARAM_1(int,nKey);
			bool nResult = m_real_service->removeTaskDataInt(nKey);
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::getTaskDataInt))
		{
			int nKey;
			GET_MSG_PARAM_1(int,nKey);
			int nResult = m_real_service->getTaskDataInt(nKey);
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::getXPSKillID))
		{
			int nHeroID;
			GET_MSG_PARAM_1(int,nHeroID);
			int nResult = m_real_service->getXPSKillID(nHeroID);
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::declareTaskData))
        {
            int nKey; bool bIsInt; bool bSave;
            GET_MSG_PARAM_3(int, nKey, bool, bIsInt, bool, bSave);
            bool nResult = m_real_service->declareTaskData(nKey, bIsInt, bSave);
            if ( resultBuf ) *resultBuf <<nResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getCurTalentPage))
        {
            int nResult = m_real_service->getCurTalentPage();
            if ( resultBuf ) *resultBuf <<nResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getCurRunePage))
        {
            int nResult = m_real_service->getCurRunePage();
            if ( resultBuf ) *resultBuf <<nResult;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::chooseTarget))
        {
            int nTargetFilter;
            UID uidTarget;
            GET_MSG_PARAM_2(int ,nTargetFilter,UID ,uidTarget);
            bool ret= m_real_service->chooseTarget(uidTarget, uidTarget);
			if ( resultBuf ) *resultBuf <<ret;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::checkPK))
        {
            UID uidTarget;
            GET_MSG_PARAM_1(UID ,uidTarget);
            bool ret= m_real_service->checkPK(uidTarget);
			if ( resultBuf ) *resultBuf <<ret;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::getPKState))
        {
            int state = m_real_service->getPKState();
			if ( resultBuf ) *resultBuf <<state;
            return true;
        }

		if (TEST_SAME_FUNCTION(IActorService::getPKCamp))
		{
			*resultBuf << m_real_service->getPKCamp();
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::getOtherRuneData))
		{
			string nResult = m_real_service->getOtherRuneData();
			*resultBuf << nResult;
			return true;
		}
        
		if (TEST_SAME_FUNCTION(IActorService::getOtherCrownPageData))
		{
			string nResult = m_real_service->getOtherCrownPageData();
			*resultBuf << nResult;
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::getOtherCrownGemstoneData))
		{
			string nResult = m_real_service->getOtherCrownGemstoneData();
			*resultBuf << nResult;
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::getOtherStrongHeroInfo))
		{
			string nResult = m_real_service->getOtherStrongHeroInfo();
			*resultBuf << nResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::canPrizeActor))
		{
			int nPrizeID = 0;
			int nNum = 0;
			GET_MSG_PARAM_2(int, nPrizeID,int, nNum);
			bool bResult = m_real_service->canPrizeActor(nPrizeID, nNum);
			*resultBuf << bResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::prizeActor))
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			int nPrizeID = 0;
			int nNum = 0;
			WORD wApproach = 0;
			DWORD dwTargetID = 0;
			LPCSTR szLogDesc = NULL;
			in >> nPrizeID >> nNum >> wApproach >> dwTargetID >> szLogDesc;
			bool bResult = m_real_service->prizeActor(nPrizeID, nNum, wApproach, dwTargetID, szLogDesc);
			if (resultBuf)*resultBuf << bResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::groupPrizeActor))
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			int nPrizeGroupID = 0;
			int *pOutPrizeID = NULL;
			int nOutPrizeCount = 0;
			WORD wApproach = 0;
			DWORD dwTargetID = 0;
			LPCSTR szLogDesc = NULL;
			in >> nPrizeGroupID >> pOutPrizeID >> nOutPrizeCount >> wApproach >> dwTargetID >> szLogDesc;
			bool bResult = m_real_service->groupPrizeActor(nPrizeGroupID, pOutPrizeID, nOutPrizeCount, wApproach, dwTargetID, szLogDesc);
			if ( resultBuf)*resultBuf << bResult;
			if ( resultBuf)*resultBuf << nOutPrizeCount;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::addPrizeChest))
		{
			rkt::ibuffer in(pMsg->context,pMsg->context_len );
			int nChestType = 0;
			int nNum = 0;
			WORD wApproach = 0;
			DWORD dwTargetID = 0;
			LPCSTR szLogDesc = NULL;
			in >> nChestType >> nNum >> wApproach >> dwTargetID >> szLogDesc;
			bool bResult = m_real_service->addPrizeChest(nChestType, nNum, wApproach, dwTargetID, szLogDesc);
			if(resultBuf)*resultBuf << bResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::entityNotifyMaster))
        {
            EntityBelongContext context;
            GET_MSG_PARAM_1(EntityBelongContext, context);
            m_real_service->entityNotifyMaster(context);
            return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::setActorTeammateHeroID))
		{
			int* pTeammateHeroID = NULL;
			GET_MSG_PARAM_1( int*, pTeammateHeroID);
			m_real_service->setActorTeammateHeroID(pTeammateHeroID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::getRandHeroCard))
		{
			DWORD *pCardIDList = NULL;
			int *pCardNumList = NULL;
			int nCardListSize = 0;
			WORD wApproach = 0;
			int nParam = 0;
			GET_MSG_PARAM_5(DWORD*, pCardIDList, int*, pCardNumList, int, nCardListSize, WORD, wApproach, int, nParam);
			int nResult = m_real_service->getRandHeroCard(pCardIDList, pCardNumList, nCardListSize, wApproach, nParam);
			if(resultBuf)*resultBuf << nResult;
			return true;
		}


		if ( TEST_SAME_FUNCTION(IActorService::StartProgressBar))
		{
			ProgressBarContext* pContext = NULL;
			GET_MSG_PARAM_1( ProgressBarContext*, pContext);
			m_real_service->StartProgressBar(pContext);
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::onClientDisconnect))
        {
            m_real_service->onClientDisconnect();
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::continuePlay))
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client);
            m_real_service->continuePlay(client);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::setClientID))
        {
            ClientID client;
            GET_MSG_PARAM_1( ClientID, client);
            m_real_service->setClientID(client);
            return true;
        }

		if ( TEST_SAME_FUNCTION(IActorService::calcBenefitCardMultiple))
		{
			int *nGoldMultiple;
			int *nActorExpMultiple;
			int *nHeroExpMultiple;

			GET_MSG_PARAM_3(int*, nGoldMultiple, int*, nActorExpMultiple, int*, nHeroExpMultiple);
			m_real_service->calcBenefitCardMultiple(nGoldMultiple, nActorExpMultiple, nHeroExpMultiple);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::canUseHeroSkin))
		{
			DWORD dwHeroID;
			DWORD dwHeroSkinID;
			GET_MSG_PARAM_2(DWORD, dwHeroID,DWORD, dwHeroSkinID);
			bool bResult = m_real_service->canUseHeroSkin(dwHeroID, dwHeroSkinID);
			if ( resultBuf ) *resultBuf <<bResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::getPKTotalNum))
        {
            DWORD bResult = m_real_service->getPKTotalNum();
            if ( resultBuf ) *resultBuf <<bResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::getTotalMatchNum))
        {
            DWORD bResult = m_real_service->getTotalMatchNum();
            if ( resultBuf ) *resultBuf <<bResult;
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::shift) )
        {
            int nDataLen = 0;
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            in >> nDataLen;
            m_real_service->shift(in.current(), nDataLen);
            return true;
        }

		//if ( TEST_SAME_FUNCTION(IActorService::dealPlayerMailDetailData) )
		//{
		//	LONGLONG llMailID; bool isAutoRecvPlus;			
		//	GET_MSG_PARAM_2(LONGLONG ,llMailID, bool ,isAutoRecvPlus);
		//	const bool & vReturn = m_real_service->dealPlayerMailDetailData(llMailID, isAutoRecvPlus);
		//	if ( resultBuf ) *resultBuf <<vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IActorService::addPlayerMailDetailData) )
		//{
		//	SMailDetailData sDetailData; bool isAutoRecvPlus;
		//	GET_MSG_PARAM_2(SMailDetailData ,sDetailData, bool ,isAutoRecvPlus);
		//	m_real_service->addPlayerMailDetailData(sDetailData, isAutoRecvPlus);
		//	return true;
		//}

		if ( TEST_SAME_FUNCTION(IActorService::dealSendMailPlusData) )
		{
			SMailSendData sendData;
			GET_MSG_PARAM_1(SMailSendData ,sendData);
			const bool & vReturn = m_real_service->dealSendMailPlusData(sendData);
			if ( resultBuf ) *resultBuf <<vReturn;
			return true;
		}

		//if ( TEST_SAME_FUNCTION(IActorService::getLastRecvPlusTime) )
		//{
		//	const DWORD & vReturn = m_real_service->getLastRecvPlusTime();
		//	if ( resultBuf ) *resultBuf <<vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IActorService::setLastRecvPlusTime) )
		//{	
		//	DWORD dwTime;
		//	GET_MSG_PARAM_1(DWORD ,dwTime);
		//	m_real_service->setLastRecvPlusTime(dwTime);
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IActorService::isAreadyRecvMail) )
		//{	
		//	LONGLONG llMailID;
		//	GET_MSG_PARAM_1(LONGLONG ,llMailID);
		//	const bool & vReturn = m_real_service->isAreadyRecvMail(llMailID);
		//	if ( resultBuf ) *resultBuf <<vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IActorService::isFastRecvMailPlus) )
		//{
		//	const bool & vReturn = m_real_service->isFastRecvMailPlus();
		//	if ( resultBuf ) *resultBuf <<vReturn;
		//	return true;
		//}

		//if ( TEST_SAME_FUNCTION(IActorService::setFastRecvMailPlus) )
		//{	
		//	bool bFlag;
		//	int nRecvCount;
		//	GET_MSG_PARAM_2(bool ,bFlag, int , nRecvCount);
		//	m_real_service->setFastRecvMailPlus(bFlag, nRecvCount);
		//	return true;
		//}

		if (TEST_SAME_FUNCTION(IActorService::setIdCard))
		{
			DWORD dwIdCard;
			GET_MSG_PARAM_1(DWORD, dwIdCard);
			m_real_service->setIdCard(dwIdCard);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::buildMentorship) )
		{	
			DWORD dwDesPDBID;
			GET_MSG_PARAM_1(DWORD ,dwDesPDBID);
			m_real_service->buildMentorship(dwDesPDBID);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::sendActorDataTaskEvent) )
		{	
			WORD nTaskEvent;
			int nParam1;
			int nParam2;
			int nParam3;
			GET_MSG_PARAM_4(WORD ,nTaskEvent, int , nParam1, int, nParam2, int, nParam3);
			m_real_service->sendActorDataTaskEvent(nTaskEvent, nParam1, nParam2, nParam3);
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::getPermanentHeroCount) )
		{
			int nResult = m_real_service->getPermanentHeroCount();
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IActorService::getTotalSkinCount) )
		{
			int nResult = m_real_service->getTotalSkinCount();
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::getStaticSkin) )
        {
            int nResult = m_real_service->getStaticSkin();
            if ( resultBuf ) *resultBuf <<nResult;
            return true;
        }

		if ( TEST_SAME_FUNCTION(IActorService::getHeroCountByLevel) )
		{
			WORD nLevel;
			GET_MSG_PARAM_1(WORD, nLevel);
			int nResult = m_real_service->getHeroCountByLevel(nLevel);
			if ( resultBuf ) *resultBuf <<nResult;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::saveGuideStep) )
        {
            int nStep;
            GET_MSG_PARAM_1(int, nStep);
            m_real_service->saveGuideStep(nStep);
            return true;
        }

		if (TEST_SAME_FUNCTION(IActorService::onMessagePointShopBuyGoods))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			size_t nLen;
			in >> nLen;

			if (resultBuf) *resultBuf << m_real_service->onMessagePointShopBuyGoods(in.current(), nLen);
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::setMatchRankSeasonInfo))
		{
			SRankSeasonWarEndInfo pInfo;
			GET_MSG_PARAM_1(SRankSeasonWarEndInfo, pInfo);

			m_real_service->setMatchRankSeasonInfo(pInfo);
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::sendLuaAddContactReply))
		{
			LONGLONG llContactID;
			GET_MSG_PARAM_1(LONGLONG, llContactID);

			m_real_service->sendLuaAddContactReply(llContactID);
			return true;
		}

		if (TEST_SAME_FUNCTION(IActorService::sendLuaContactApplyFriend))
		{
			DWORD dwTargetID;
			bool bIsDifSex;
			GET_MSG_PARAM_2(DWORD, dwTargetID, bool, bIsDifSex);

			m_real_service->sendLuaContactApplyFriend(dwTargetID, bIsDifSex);
			return true;
		}

        if ( TEST_SAME_FUNCTION(IActorService::mobaGoodsAddEffect) )
        {
            rkt::ibuffer in(pMsg->context, pMsg->context_len);
            size_t nLen;
            int nEffectID;
            in>>nEffectID >> nLen;

            if ( resultBuf ) *resultBuf <<m_real_service->mobaGoodsAddEffect( nEffectID, in.current(), nLen);
            return true;
        }

        if ( TEST_SAME_FUNCTION(IActorService::mobaGoodsRemoveEffect) )
        {	
            int nID;

            GET_MSG_PARAM_1(int ,nID);
            m_real_service->mobaGoodsRemoveEffect( nID);
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::setCrownEffect))
        {
            int timeAddEffect;

            GET_MSG_PARAM_1(int, timeAddEffect);
            m_real_service->setCrownEffect(timeAddEffect);
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::luaBuyGoods))
        {
            int goodsID;
            int nCost;
            GET_MSG_PARAM_2(int, goodsID, int, nCost);
            bool vReturn = m_real_service->luaBuyGoods(goodsID, nCost);
            if (resultBuf) *resultBuf << vReturn;
            return true;
        }

        if (TEST_SAME_FUNCTION(IActorService::changeVoc))
        {
            int nNewVoc;
			int nNewSkin;
            GET_MSG_PARAM_2(int, nNewVoc, int, nNewSkin);
            bool vReturn = m_real_service->changeVoc(nNewVoc, nNewSkin);
            if (resultBuf) *resultBuf << vReturn;
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
        m_proxy.m_pContainer = 0;

        if ( m_real_service )
        {
            m_real_service->release();
            m_real_service = 0;
        }

        delete this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateActorService()
{
    return new ActorService_Stub(new ActorService);
}