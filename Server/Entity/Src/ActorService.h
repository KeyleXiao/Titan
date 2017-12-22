/*******************************************************************
** 文件名:	ActorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

每个玩家对应一个Actor服务(一个协程)
********************************************************************/

#pragma once

#include "IActorService.h"
#include "IPlayerRole.h"
#include "IEntityProperty.h"
#include "IEntityAOI.h"
#include "IMovePart.h"
#include "IEnmityServer.h"
#include "IBuffServer.h"
#include "PlayerRole.h"
#include "PlayerRoleProperty.h"
#include "ISpell.h"
#include "IWarPart.h"
#include "MigrateDef.h"
#include "IFreezeServer.h"
#include "IMobaGoodsPart.h"
#include "IBankPart.h"
#include "ITestServer.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"
#include "MigrateDef.h"
#include "ILoginServerProtocol.h"
#include "ICenterConnectorService.h"
#include "MatchSceneHelper.h"
#include "ILogoutPart.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "TaskStorageDef.h"
#include "IPlayerKillPart.h"
#include "IProgressBarPart.h"
#include "ITitlePart.h"
#include "IAntiAddictServer.h"
#include "IMailPart.h"
#include "ICommonPart.h"
#include "IRuneManager.h"
#include "HeightFieldMgr.h"
#include "ActorServiceDBSink.h"
#include "ChatDef.h"

class ActorService : public IActorService
{
	__IPlayerRole  *  m_pReal;   

	UID_DATA    m_uid;
	ClientID    m_client;
	SERVICE_ID  m_idService;    
	int m_nLoginMode;
    SActorCreateContext m_createContext;
    CActorServiceDBSink m_actorServiceDBSink;

public:
	ActorService() : m_pReal(0),m_idService(INVALID_SERVICE_ID)
	{

	}

    virtual void onClientDisconnect()
    {
        if(m_pReal != NULL)
        {
            __IWarPart* pWarPart = (__IWarPart*)m_pReal->getEntityPart(PART_WAR);
            if(pWarPart != NULL && pWarPart->getWarID() != INVALID_WAR_ID)
            {
                // 战场角色不销毁
                DWORD dwDBID = m_pReal->getIntProperty(PROPERTY_ID); 
                gServerGlobal->getActorMgr()->setClientID(dwDBID, INVALID_CLIENT);
                // 通知战场角色下线
                MatchSceneHelper match;
                if(match.m_ptr)
                {
                    match.m_ptr->onUpdateRoomPlayerClient(dwDBID, INVALID_CLIENT);
                }

                // 发退出事件
                __IEntityEvent * pEntityEvent = m_pReal->getEntityEvent();
                if(pEntityEvent)
                {
                    pEntityEvent->FireExecute(EVENT_ENTITY_ABSENT,NULL,0);
                }

                return;
            }
        }

        // 销毁角色
        UID uidSelf = getUID();
        int nSceneID = UID_2_SCENE(uidSelf);
        SceneLogicHelper helper(nSceneID);
        if(!helper.m_ptr)
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << uidSelf << ", destroy failed, scenelogic not found");
        }
        else
        {
            helper.m_ptr->destroyActor(uidSelf, elogout_offline);
        }
    }
    
    // 续玩
    virtual void continuePlay(ClientID client)
    {
        // 发出续玩事件
        if(m_pReal)
        {
			int nActorID = m_pReal->getIntProperty(PROPERTY_ID);
			gServerGlobal->getActorMgr()->setClientID(nActorID, client);

            event_entity_continue_play eventData;
            eventData.dwDBID = nActorID;
            eventData.clientID = client;

            __IEntityEvent * pEntityEvent = m_pReal->getEntityEvent();
            if(pEntityEvent)
            {
				pEntityEvent->FireExecute(EVENT_ENTITY_CONTINUE_PLAY,(LPCSTR)&eventData,sizeof(eventData));
                pEntityEvent->FireExecute(EVENT_GAME_ACTOR_CONTINUE_PLAY,(LPCSTR)&eventData,sizeof(eventData));
            }

			// 全局发续玩事件
			rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
			if (pEventEngine)
			{
				pEventEngine->FireExecute(EVENT_GAME_ACTOR_CONTINUE_PLAY, SOURCE_TYPE_PERSON, nActorID, (LPCSTR)&eventData, sizeof(eventData));
			}
        }
        else
        {
            ErrorLn(__FUNCTION__ << ", m_pReal = 0");
        }
    }

    // 设置客户端ID
    virtual void setClientID(ClientID client)
    {
        m_client = client;
        if(m_pReal != NULL)
        {
            m_pReal->setClientID(client);
        }
    }


	// 启动
	virtual bool on_start(SERVICE_PTR pContainer, void * data, int len)
	{
        m_actorServiceDBSink.on_start(this);

		// 校验数据合法性
		if(!checkData(data, len))
		{
			ErrorLn(__FUNCTION__ << ", data format error");
			return false;
		}

		// 解包
		m_uid = *((UID_DATA*)data);
		m_nLoginMode = *((int*)((char*)data + sizeof(UID)));

		SActorCreateContext* pContext = (SActorCreateContext*)((char*)data + sizeof(UID) + sizeof(int));
		if(pContext == NULL)
		{
			return false;
		}

        memcpy(&m_createContext, pContext, sizeof(SActorCreateContext)); 
        m_client = pContext->client;
		m_idService = pContainer->get_id();
        
        switch(pContext->eDStype)
        {
        case eds_type_db:
            {
                return m_actorServiceDBSink.loadActorDataFromDataBase(pContext->udbid, pContext->pdbid);
            }
            break;

        case eds_type_cache:
            {
                return createPlayerRole((ELoginMode)m_nLoginMode, pContext + 1, pContext->nCreateDataLen, pContext->nCreateDataFlag, pContext->nSceneID, pContext->locBorn,
                    (char*)(pContext + 1) + pContext->nCreateDataLen, pContext->nUpdateDataLen, pContext->nUpdateDataFlag);
            }
            break;
        default:
            {
                ErrorLn(__FUNCTION__ << ", unknown datasrc type=" << pContext->eDStype);

                UID uidSelf = *((UID*)&m_uid);
                int nSceneID = UID_2_SCENE(uidSelf);
                SceneLogicHelper helper(nSceneID);
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << uidSelf << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(uidSelf, elogout_unknown);
                }

                return false;
            }
        }
	}

	// 停止
	virtual bool on_stop()
	{
		if (m_pReal != 0)
		{
			__ILogoutPart* pLogoutPart = (__ILogoutPart*)m_pReal->getEntityPart(PART_LOGOUT);
			if(pLogoutPart == NULL)
			{
				return false;
			}

			int nReason = pLogoutPart->getLogoutReason();
			PDBID dbid = getProperty_Integer(PROPERTY_ID);
			UID uid = m_pReal->getUID();
			int nSceneID = m_pReal->getSceneID();

            // TODO，由于异步操作，其他的系统中用全局的Logout事件来做操作取不到这个实体，先在释放对象前可额外执行一些逻辑
            onPrevDestroy(dbid, uid, nSceneID, nReason);

			m_pReal->destroy();

			fireLogoutEvent(dbid, uid, nSceneID, nReason);

		}

        m_actorServiceDBSink.on_stop();

		return true;
	}

    void onPrevDestroy(PDBID dwDBID, UID uid, int nSceneID, int nReason)
    {
        // 下线
        if(nReason == elogout_offline)
        {
            // 通知防沉迷模块
            IAntiAddictServer* pAntiAddictServer = gServerGlobal->getAntiAddictServer();
            if(pAntiAddictServer) {
                pAntiAddictServer->onActorLogout(dwDBID, uid, getUserID(), isAdult(), getIdCard());
            }
        }
    }


	//////////////////////////////// IEntity ///////////////////////////////////////////////

	// 取得实体ID
	virtual UID getUID() { return  m_pReal->getUID() ; }

	//#	define getWarPart() ((__IWarPart*)m_pReal->getEntityPart(PART_WAR))
	inline __IWarPart* getWarPart()
	{
		return m_pReal ? (__IWarPart*)m_pReal->getEntityPart(PART_WAR) : 0;
	}


	virtual int getWarID(){__IWarPart* pWarPart = getWarPart() ;  return pWarPart ? pWarPart->getWarID() : 0;}

    virtual int getMatchTypeID(){__IWarPart* pWarPart = getWarPart() ;  return pWarPart ? pWarPart->getMatchTypeID() : 0;}

	virtual int getSmSkillId(int nSlot)
	{
		__IWarPart* pWarPart = getWarPart();
		return pWarPart ? pWarPart->getSmSkillId(nSlot) : 0;
	}

	// 取得实体名称
	virtual string getName() { return m_pReal->getName(); }

	// 取得当前所在场景ID
	virtual int getSceneID() { return m_pReal->getSceneID(); }

	// 取得实体位置
	virtual Vector3 getLocation() { return m_pReal->getLocation(); }

	// 设置实体位置
	virtual void setLocation(Vector3 vLoc) { m_pReal->setLocation(vLoc); }

	// 取得实体角度
	virtual Vector3 getAngle() { return m_pReal->getAngle(); }

	// 收到网络消息
	virtual void handleMessage(void *pData,int nLen) { m_pReal->handleMessage(pData,nLen); }

	virtual void handleReplaceableMessage(unsigned long ulKey,void * pMsg,int nLen){}

	// 发送数据给客户端
	virtual int send2Client(PACKAGE_PTR package, int nFlag = 0) { return m_pReal->send2Client(package, nFlag); }

    // 发送数据给客户端包括OB
    virtual void send2ClientAndOB( PACKAGE_PTR package ) { m_pReal->send2ClientAndOB(package); }

	// 改变当前状态 
	virtual void changeState(int nState) { m_pReal->changeState(nState); }

	// 得到当前状态 
	virtual int getState(void) { return m_pReal->getState(); }

	// 设置游戏状态ACTOR_GAME_STATE
	virtual void setGameState(DWORD dwState) {  m_pReal->setGameState(dwState); }
	// 获取游戏状态 ACTOR_GAME_STATE
	virtual DWORD getGameState() { return m_pReal->getGameState(); }

	// 取得玩家所在网关分配ID
	virtual DWORD getClientID() { return m_pReal->getClientID(); }

	// 导出角色数据
	virtual string exportActorData(int nFlag, bool bFromBank)
	{
		obuf data;
		if(bFromBank)
		{
			m_pReal->deSerializeStaticData(data);
		}
		else
		{
			m_pReal->deSerialize(data, nFlag);
		}
		return string(data.data(), data.size());
	}

	// 导出角色的共享属性
	virtual SSharePersonProperty exportShareProperties()
	{
		SSharePersonProperty prop;
		sstrcpyn(prop.szName, getName().c_str(), sizeof(prop.szName));
        prop.dwWorldID  = getFromGameWorldID();
        prop.dwUDBID    = getUserID();
		prop.dwPDBID    = getProperty_Integer(PROPERTY_ID);
		prop.nSkinID    = getProperty_Integer(PROPERTY_SKIN);
		prop.nVocation  = getProperty_Integer(PROPERTY_VOCATION);

        // 要用角色静态等级
        if (m_pReal)
		{
            __IBankPart * pBankPart = CAST_TYPE(__IBankPart *, m_pReal->getEntityPart(PART_BANK));
            if (pBankPart) {
                prop.wLevel = pBankPart->getPlayerInfo()->nLevel;
            }
        }
        prop.bySex      = getProperty_Integer(PROPERTY_SEX);
		prop.byState    = getState();
		prop.dwGameState = getGameState();
		prop.dwSceneID  = getSceneID();
		prop.dwKinID    = getProperty_Integer(PROPERTY_KIN);
		prop.dwClanID   = getProperty_Integer(PROPERTY_CLAN);
		prop.nFight     = getProperty_Integer(PROPERTY_FIGHTCAPACITY);
		prop.nTicket    = getProperty_Integer(PROPERTY_TICKET);
		prop.nGlod      = getProperty_Integer(PROPERTY_GOLD);
		//prop.nBindGlod  = getProperty_Integer(PROPERTY_BINDGOLD);
		prop.nFaceID    = getProperty_Integer(PROPERTY_FACEID);
        prop.byMatchType = getProperty_Integer(PROPERTY_MATCHTYPE);
        prop.nRankScore = getProperty_Integer(PROPERTY_RANKSCORE);
        prop.nRankGrade = getProperty_Integer(PROPERTY_RANKGRADE);
        prop.nKingRankOrder = getProperty_Integer(PROPERTY_KINGRANK_ORDER);
        

		prop.clientID   = getClientID();
		prop.nServerSerialID = gServerGlobal->getServerID();
		ICenterConnectorService * pCenterConnector = gServerGlobal->getCenterConnectorService();
		if (pCenterConnector)
		{
			prop.serverID = pCenterConnector->getLocalServerCSID();
		}
        prop.nPrivilege = getPlayerInfo().nPrivilege;

		return prop;
	}

	// 取得创建上下文中的标识数据 ACTOR_CREATE_CONTEXT_FLAG
	virtual BYTE getCreateContextFlag(BYTE nFromActorType, BYTE nToActorType, BYTE nFlag)
	{
		bool isAttachThread = false;

		// 是否为压力测试账号
		if (isTester())
		{
			isAttachThread = true;
		}

		// 是否附到场景LOGIC线程
		if(isAttachThread)
		{
			nFlag |= CONTEXT_FLAG_ATTACH_THREAD;
		}
		else
		{
			int n = CONTEXT_FLAG_ATTACH_THREAD;
			nFlag &= (~n);
		}

		return nFlag;
	}

	// 是否为压力测试账号
	virtual bool isTester()
	{
		if(m_pReal == NULL)
			return false;

		__ITestPart * pTestPart = (__ITestPart *)m_pReal->getEntityPart(PART_TEST);
		return pTestPart ? pTestPart->isTester() : false;
	}

	/** 增加/减少经验值
	@param   nExp:            经验值（负值减少，正值增加）
	@param   wApproach:       消耗原因,0.不需要记录日记
	@param   dwTargetID:      如果有目标者，这个就填入目标者ID
	@param   szLogMark:       传入交易日志，供查询  
	@param   nLogReserve:     日志的保留字段
	@return
	*/
	virtual void addExp(int nExp, WORD wApproach, DWORD dwTargetID, const char * pszRemark, int nLogReserve)
	{
		m_pReal ? m_pReal->addExp(nExp, wApproach, dwTargetID, pszRemark, nLogReserve) : false;
	}


    /** 是否可增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod)
	{
		return m_pReal ? m_pReal->canAddMoney(nTicket, nGlod) : false;
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
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark, int nSubclassID, short nNum, bool bAddictLimit)
	{
		return m_pReal ? m_pReal->addMoney(nTicket, nGlod, nReason, pszRemark, nSubclassID, nNum, bAddictLimit) : false;
	}


	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	inline __IEntityProperty* getProperty()
	{
		if(m_pReal == NULL)
		{
			return NULL;
		}
		return (__IEntityProperty*)m_pReal->getEntityPart(PART_PROPERTY);
	}
	//#	define getProperty() ((__IEntityProperty*)m_pReal->getEntityPart(PART_PROPERTY))

	// 取得某个属性的类型
	virtual string getPropertyType(int prop_id)
	{
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty ? pProperty->getPropertyType(prop_id) : ""; 
	}

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag(int prop_id) 
	{
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty  ? pProperty->getPropertyFlag(prop_id) : 0; 
	}

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool(int prop_id) 
	{ 
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty ? pProperty->getProperty_Bool(prop_id) : false; 
	}

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool(int prop_id, bool value)
	{ 
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty ? pProperty->setProperty_Bool(prop_id, value):false;
	}

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( int prop_id)
	{ 
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty ? pProperty->getProperty_Integer(prop_id) : 0;
	}

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer(int prop_id, int value) 
	{ 
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty ? pProperty->setProperty_Integer(prop_id, value) : false; 
	}

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer(int prop_id, int delta) 
	{ 
		__IEntityProperty* pProperty = getProperty();
		return pProperty  ? pProperty->addProperty_Integer(prop_id, delta) : false;
	}

	// 获取实体属性(字符串)
	virtual string getProperty_String(int prop_id) 
	{
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty  ? pProperty->getProperty_String(prop_id) : "";
	}

	// 设置实体属性(字符串)
	virtual bool setProperty_String(int prop_id, string value) 
	{ 
		__IEntityProperty* pProperty = getProperty() ; 
		return pProperty  ? pProperty->setProperty_String(prop_id, value) : false;
	}

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual string getProperty_custom(int prop_id)
	{
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty  ? pProperty->getProperty_String(prop_id) : "";
	}

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom(int prop_id, string value)
	{ 
		__IEntityProperty* pProperty = getProperty(); 
		return pProperty  ? pProperty->setProperty_custom(prop_id, (void *)value.c_str(), value.length()) : false;
	}

	// 更改天赋点属性
	virtual void updateTalentPoint( int nPoint, int nCHangeTYpe, BYTE byAddType = 2)
	{
		if (m_pReal != NULL )
		{
			m_pReal->updateTalentPoint(nPoint, nCHangeTYpe, byAddType);
		}
	}

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	//#	define getAOI() ((__IEntityAOI*)m_pReal->getEntityPart(PART_AOI))
	inline __IEntityAOI* getAOI()
	{
		return m_pReal ? (__IEntityAOI*)m_pReal->getEntityPart(PART_AOI) : 0;
	}

	// 取得视野半径
	virtual float getSight() 
	{ 
		__IEntityAOI* pAOI = getAOI();
		return pAOI  ? pAOI->getSight() : 0.0f; 
	}

	// 调整视野半径
	virtual void setSight(float radii)
	{ 
		if(__IEntityAOI* pAOI = getAOI())
			pAOI->setSight(radii);
	}

	// 是否隐身
	virtual bool isHide() 
	{ 
		__IEntityAOI* pAOI = getAOI(); 
		return pAOI  ? getAOI()->isHide() : false; 
	}

	// 设置隐身状态
	virtual void setHide(bool state) 
	{ 
		if(__IEntityAOI* pAOI = getAOI())
			pAOI->setHide(state);
	}

	/////////////////////// IEnmityPart ///////////////////////////////////////////////
	//#	define getEnmity() ((__IEnmityPart*)m_pReal->getEntityPart(PART_ENMITY))
	inline __IEnmityPart* getEnmity()
	{
		return m_pReal ? (__IEnmityPart*)m_pReal->getEntityPart(PART_ENMITY) : 0;
	}
	// 添加仇恨数据
	virtual void AddEnmityValue(UID uid, int nVal) 
	{ 
		if(__IEnmityPart* pEnmity = getEnmity()) 
			pEnmity->AddEnmityValue(uid, nVal);
	};

	virtual void AddBroadEnmityValue(UID uid, int nVal)
	{ 
		if(__IEnmityPart* pEnmity = getEnmity()) 
			pEnmity->AddBroadEnmityValue(uid, nVal);
	};

	virtual void SetEnmityValue(UID uid, int nVal) 
	{
		if(__IEnmityPart* pEnmity = getEnmity())
			pEnmity->SetEnmityValue(uid, nVal);
	};

	/////////////////////// IBuffPart ///////////////////////////////////////////////
	//#	define getBuffPart() ((__IBuffPart*)m_pReal->getEntityPart(PART_BUFF))
	inline __IBuffPart* getBuffPart()
	{
		return m_pReal ? (__IBuffPart*)m_pReal->getEntityPart(PART_BUFF) : 0;
	}
	/** 给实体添加buff */	
	virtual bool AddBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{ 
		__IBuffPart* pBuff = getBuffPart() ; 
		return pBuff ? pBuff->Add(dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen) : false;
	};

	/** 给实体移除buff */
	virtual bool RemoveBuff(DWORD dwBuffID, UID uidAdd, UID uidRemove)
	{ 
		__IBuffPart* pBuff = getBuffPart() ;
		return pBuff ? pBuff->Remove(dwBuffID, uidAdd, uidRemove) : false;
	};

	/** 某个BUFF是否存在 */
	virtual bool IsExistBuff(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
	{
		__IBuffPart* pBuff = getBuffPart() ; 
		return pBuff ? pBuff->IsExist(dwBuffID, dwBuffLevel, uidAdd) : false;
	};

	/** 某个BUFF类型是否存在 */
	virtual bool IsExistBuffType(DWORD dwBuffType)
	{ 
		__IBuffPart* pBuff = getBuffPart() ; 
		return pBuff ? pBuff->IsExist(dwBuffType) : false;
	};

	/** 取玩家某个BUFF的等级(如果有多个相同ID的BUFF取等级最高的) */
	virtual DWORD GetBuffLevel(DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart* pBuff = getBuffPart() ; 
		return pBuff ? pBuff->GetBuffLevel(dwBuffID, uidAdd) : 0;
	};

	/** 取玩家某个BUFF剩余时间(如果有多个相同ID的BUFF取等级最高的) */
	virtual int GetLeftTime(DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart* pBuff = getBuffPart() ; 
		return pBuff ? pBuff->GetLeftTime(dwBuffID, uidAdd) : 0;
	};

	/** 设置累积BUFF */
	virtual void SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{
		if(__IBuffPart* pBuff = getBuffPart()) 
			pBuff->SetAccumulateBuff(dwBuffID, uidAdd, bOverlay, dwTime, pBuffContext, nContextLen);
	};

	virtual bool RemoveBuffByIndex(DWORD dwIndex)
	{  
		__IBuffPart* pBuff = getBuffPart() ;
		return pBuff ? pBuff->RemoveBuffByIndex(dwIndex) : false;
	};

	virtual BuffTypeList getBuff(DWORD dwBuffType)
	{
		DWORD dwBuffID[BUFF_TYPE_MAX_COUNT] = {0};
		DWORD dwBuffLevel[BUFF_TYPE_MAX_COUNT] = {0};
		int nBuffTime[BUFF_TYPE_MAX_COUNT] = {0};
		int nCount = BUFF_TYPE_MAX_COUNT;

		__IBuffPart* pBuff = getBuffPart();
		if(pBuff == NULL)
		{
			return BuffTypeList();
		}

		getBuffPart()->GetBuff(dwBuffType,dwBuffID,dwBuffLevel,nBuffTime,nCount);

		BuffTypeList buffType;
		buffType.nCount = nCount;
		memcpy(buffType.dwBuffID, dwBuffID, sizeof(buffType.dwBuffID));
		memcpy(buffType.dwBuffLevel, dwBuffLevel, sizeof(buffType.dwBuffLevel));
		memcpy(buffType.nBuffTime, nBuffTime, sizeof(buffType.nBuffTime));

		return buffType;
	}
	/////////////////////// IFreezePart ///////////////////////////////////////////////
#	define getFreezePart() ((__IFreezePart*)m_pReal->getEntityPart(PART_FREEZE))
	// 获取冷却时间
	virtual DWORD GetFreezeLeftTime(DWORD dwClassID, DWORD dwFreezeID)
	{ 
		__IFreezePart* pFreeze = getFreezePart() ; 
		return pFreeze? pFreeze->GetFreezeLeftTime(dwClassID, dwFreezeID) : 0;
	};
	// 启动冷却
	virtual bool FreezeWork(DWORD dwClassID, DWORD dwFreezeID)
	{
		__IFreezePart* pFreeze = getFreezePart() ; 
		return pFreeze? pFreeze->Work(dwClassID, dwFreezeID) : false;
	}
	// 设置冷却时间
	virtual bool SetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nTime, bool nIsCleanInterferedFreeze,bool bBreak)
	{
		__IFreezePart* pFreeze = getFreezePart() ; 
		return pFreeze? pFreeze->SetFreezeTime(dwClassID, dwFreezeID, nTime, nIsCleanInterferedFreeze, bBreak) : false;
	}

	/////////////////////// IBankPart ///////////////////////////////////////////////
#	define getBankPart() ( m_pReal ? (__IBankPart*)m_pReal->getEntityPart(PART_BANK) : 0)
	/** 获取冷却时间 */	
	virtual DWORD getFromGameWorldID()
	{
		__IBankPart *pBank = getBankPart() ; 
		return pBank ?  pBank->getFromGameWorldID() : 0;
	};

	// 取得账号ID
	virtual DWORD getUserID()
	{ 
		__IBankPart *pBank = getBankPart() ;
		return pBank ? pBank->getUserID() : 0;
	};

    // 获取IdCard数据
    virtual DWORD getIdCard()
    {
        __IBankPart *pBank = getBankPart() ;
        return pBank ? pBank->getIdCard() : 0;
    }

	// 设置IdCard数据
	virtual void setIdCard(DWORD dwIdCard)
	{
		__IBankPart *pBank = getBankPart();
		return pBank ? pBank->setIdCard(dwIdCard) : 0;
	}

    // 是否为成年人
    virtual bool isAdult()
    {
        __IBankPart *pBank = getBankPart() ;
        return pBank ? pBank->isAdult() : false;
    }

    // 设置为成年人
    virtual void setAdult()
    {
        __IBankPart *pBank = getBankPart() ;
        if(pBank) pBank->setAdult();
    }

    // 获得玩家角色等级
    virtual int getPlayerLevel()
    {
        return m_pReal ? m_pReal->getPlayerLevel() : 0;
    }

    // 获得玩家角色经验
    virtual int getPlayerExp()
    {
        return m_pReal ? m_pReal->getPlayerExp() : 0;
    }

    virtual DWORD getMatchTypeRank(EMMatchType eMatchType)
    {
        __IBankPart *pBank = getBankPart() ;
        return pBank ? pBank->getMatchTypeRank(eMatchType) : 0;
    }

    virtual DWORD getMatchTypeHideRank(EMMatchType eMatchType)
    {
        __IBankPart *pBank = getBankPart() ;
        return pBank ? pBank->getMatchTypeHideRank(eMatchType) : 0;
    }

    virtual bool isMatchHeroCheckPassed(EMMatchType eMatchType)
    {
		__IBankPart *pBank = getBankPart() ;
		return pBank ? pBank->isMatchHeroCheckPassed(eMatchType) : false;
	}

	virtual DWORD getMatchTypeRankGrade(EMMatchType eMatchType)
	{
		return getBankPart()->getMatchTypeRankGrade(eMatchType);
	}

	// 设置排位Rank数据
	virtual void addMatchTypeRank(EMMatchType eMatchType,int nRankScore, int nHideRankScore)
	{
		__IBankPart *pBank = getBankPart() ; 
		pBank ? pBank->addMatchTypeRank(eMatchType, nRankScore,nHideRankScore) : false;
	};

	// 设置排位Rank数据
	virtual void setMatchTypeRank(EMMatchType eMatchType,int nRankScore, int nHideRankScore)
	{
		__IBankPart *pBank = getBankPart() ; 
		pBank ? pBank->setMatchTypeRank(eMatchType, nRankScore, nHideRankScore) : false;
	};

    // 设置排位等级(王者段位处理 只更新段位等级 策划要求 这种需求有什么问题就找策划)
    virtual void setMatchTypeGrade(EMMatchType eMatchType, int nGrade)
    {
        __IBankPart *pBank = getBankPart() ; 
        pBank ? pBank->setMatchTypeGrade(eMatchType, nGrade) : false;
    };

	// 设置英雄熟练度信息
	virtual void setMatchHeroScore(int nHeroID, int nHeroScore)
	{ 
		__IBankPart *pBank = getBankPart() ;
		pBank ? pBank->setMatchHeroScore(nHeroID, nHeroScore) : false;
	};

	virtual int getSummonerSkillInfo(int nSolt)
	{
		__IBankPart *pBank = getBankPart() ;
		return pBank ? pBank->getSummonerSkillInfo(nSolt) : 0;
	}
    
    // 设置玩家活跃度
    virtual void setDayActivity( int value, bool isSync = true)
    {
        __IBankPart *pBank = getBankPart();
        if( pBank ) {
            pBank->setDayActivity(value, isSync);
        }
    }

    // 设置玩家活跃度
    virtual int getDayActivity(void)
    {
        __IBankPart *pBank = getBankPart();
        return (pBank  ? pBank->getDayActivity() : 0);
    }
    
    // 设置玩家周活跃度
    virtual void setWeekActivity(int value, bool isSync = true)
    {
        __IBankPart *pBank = getBankPart();
        if (pBank) {
            pBank->setWeekActivity(value, isSync);
        }
    }

    // 获取玩家周活跃度
    virtual int getWeekActivity(void)
    {
        __IBankPart *pBank = getBankPart();
        return (pBank ? pBank->getWeekActivity() : 0);
    }

	/*********************更新玩家基本信息相关内容*********************/
	// 设置玩家单独某个属性
	virtual void setActorBaseInfo(LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve)
	{
		__IBankPart *pBank = getBankPart() ;
		pBank ? pBank->setActorBaseInfo(lgValue, nType, nMode, byNumType, byOpType, wApproach, dwTargetID, szLogMark, nLogReserve) : 0;
	}

	// 设置玩家召唤师技能或者荣耀信息
	virtual void setActorSummonerOrGlory(int nData[], int nLen,int nType, long long llWarDBKey)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return;
		}

		pBank->setActorSummonerOrGlory(nData, nLen, nType, llWarDBKey);
	}

	// 储存同队友和自己的上一把的英雄id
	virtual void setActorTeammateHeroID(int* pTeammateHeroID)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return ;
		}

		pBank->setActorTeammateHeroID(pTeammateHeroID);	
	}
	
	/*********************更新玩家基本信息相关内容*********************/

	/*********************更新英雄基本信息相关内容*********************/
	// 更新英雄单个信息到内存              nType:ENEXPTYPE   nMode:ENCHANGEDATAMODE
	virtual void setHeroOneInfo(int nHeroID, LONG lgValue, int nType, int nMode, BYTE byNumType, BYTE byOpType, WORD wApproach, DWORD dwTargetID, LPCSTR szLogMark, int nLogReserve)
	{
		__IBankPart *pBank = getBankPart();
		pBank ? pBank->setHeroOneInfo( nHeroID, lgValue, nType, nMode, byNumType, byOpType, wApproach, dwTargetID, szLogMark, nLogReserve ) : 0;
	}

	// 获得英雄属性
	virtual SHeroInfo getHeroInfo( int nHeroID )
	{
		SHeroInfo result;
		__IBankPart *pBank = getBankPart() ;
		if(pBank != NULL)
		{
			SHeroInfo * pInfo = pBank->getHeroInfo(nHeroID);
			if( pInfo) result = *pInfo;
		}

		return result;
	}

	// 校验指定英雄信息
	virtual SHeroInfo checkHeroInfo( int nHeroID )
	{
		SHeroInfo result;
		__IBankPart *pBank = getBankPart() ;
		if(pBank != NULL)
		{
			SHeroInfo * pInfo = pBank->checkHeroInfo(nHeroID);
			if( pInfo) result = *pInfo;
		}

		return result;
	}

	// 获得所有英雄信息
	virtual string getHeroArray()
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}

		obuf data;
		pBank->getHeroArray(data);
		return string(data.data(), data.size());
	}

	// 获得当前玩家战场可用的英雄
	virtual string getPlayerCanUseHeroArray()
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}

		obuf data;
		pBank->getPlayerCanUseHeroArray(data);
		return string(data.data(), data.size());
	}

    // 获得当前玩家战场可用的英雄
    virtual string getAllMatchTypeRank()
    {
        __IBankPart *pBank = getBankPart();
        if (pBank == NULL)
        {
            return "";
        }

        obuf data;
        pBank->getAllMatchTypeRank(data);
        return string(data.data(), data.size());
    }

	// 获得英雄数量
	virtual int getHeroCount()
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return 0;
		} 

		return pBank->getHeroCount();
	}

	// 设置英雄专属天赋效果
	virtual void setHeroExclusiveTalentEffect( int nHeroID )
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank != NULL)
		{
			pBank->setHeroExclusiveTalentEffect( nHeroID );
		} 
	}

	// 释放英雄专属天赋效果
	virtual void removeHeroExclusiveTalentEffect()
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank != NULL)
		{
			pBank->removeHeroExclusiveTalentEffect();
		} 
	}

	virtual bool addHeroCardInfo( DWORD* dwCardID, int* nValue, int nCount = 1)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return false;
		} 

		return pBank->addHeroCardInfo(dwCardID, nValue, nCount);
	}

	virtual void updateMapInfo( int nMapID, bool bIsWin)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return ;
		} 

		pBank->updateMapInfo(nMapID, bIsWin);
	}

	virtual DWORD getFirstWinTime()
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return 0;
		} 

		return pBank->getFirstWinTime();
	}

	virtual void setPlayerFirstWinFlag( DWORD dwTime)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return ;
		} 

		pBank->setPlayerFirstWinFlag(dwTime);
	}

	// 检查玩家能否使用英雄皮肤
	virtual bool canUseHeroSkin(DWORD dwHeroID, DWORD dwHeroSkinID)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return false;
		} 

		return pBank->canUseHeroSkin(dwHeroID, dwHeroSkinID);
	}

    // 获取总共PK比赛的场次
    virtual DWORD getPKTotalNum()
    {
        __IBankPart *pBank = getBankPart() ;
        if(pBank == NULL)
        {
            return 0;
        } 

        return pBank->getPKTotalNum();
    }

    // 获取总共比赛场次
    virtual DWORD getTotalMatchNum()
    {
        __IBankPart *pBank = getBankPart() ;
        if(pBank == NULL)
        {
            return 0;
        } 

        return pBank->getTotalMatchNum();
    }

    /////////////////////// ITitlePart ///////////////////////////////////////////////
#	define getTitlePart() ((__ITitlePart*)m_pReal->getEntityPart(PART_TITLE))

    virtual void AddTitle(int nTitleID, char * szTitleName, int nTitleNameLen, bool bCoverTitle)
    {
        __ITitlePart *pTitle = getTitlePart();
        if(pTitle == NULL)
        {
            return ;
        }

        if (nTitleNameLen > TITLE_NAME_MAXSIZE)
        {
            ErrorLn("AddTitle error nTitleNameLen="<<nTitleNameLen);
            return;
        }

        char szName[TITLE_NAME_MAXSIZE]={0};
        sstrcpyn(szName, szTitleName, nTitleNameLen);

        pTitle->addTitle(nTitleID, szName, bCoverTitle);
    }

    virtual void deleteTitle(int nTitleID)
    {
        __ITitlePart *pTitle = getTitlePart();
        if(pTitle == NULL)
        {
            return ;
        } 

        pTitle->deleteTitle(nTitleID);
    }

	virtual int getPlayerFightCapacity()
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank == NULL)
		{
			return 0;
		} 

		return pBank->getPlayerFightCapacity();
	}

	// 获得玩家信息
	virtual SPlayerInfo getPlayerInfo()
	{
		SPlayerInfo result;
		__IBankPart *pBank = getBankPart() ;
		if(pBank != NULL)
		{
			result = (*(pBank->getPlayerInfo()));
		} 

		return result;
	}

    virtual void getPlayerRoomExData(SMatchPlayerRoomExData *pRoomExData)
    {
        __IBankPart *pBank = getBankPart() ;
        if(pBank != NULL)
        {
            pBank->getPlayerRoomExData(pRoomExData);
        }

	}

	// 设置任务数据
	virtual bool addTaskDataInt(int nKey, int nData)
	{
		__IBankPart *pBank = getBankPart() ;
		if (pBank == NULL)
			return false;

		return pBank->addTaskDataInt(nKey, nData);
	}

	// 删除任务数据
	virtual bool removeTaskDataInt(int nKey)
	{
		__IBankPart *pBank = getBankPart() ;
		if (pBank == NULL)
			return false;

		return pBank->removeTaskDataInt(nKey);
	}

	// 获取任务数据
	virtual int getTaskDataInt(int nKey)
	{
		__IBankPart *pBank = getBankPart() ;
		if (pBank == NULL)
			return TASKDATA_DEFAULT_VALUE;

		return pBank->getTaskDataInt(nKey);
	}

    // 获取英雄的XP技能ID
	virtual int getXPSKillID(int nHeroID)
    {
		__IBankPart *pBank = getBankPart() ;
		if (pBank == NULL)
			return TASKDATA_DEFAULT_VALUE;

		return pBank->getXPSKillID(nHeroID);
	
    }

    // 注册任务数据
    virtual bool declareTaskData(int nKey, bool bIsInt, bool bSave)
    {
        __IBankPart *pBank = getBankPart() ;
        if (pBank == NULL)
            return false;

        return pBank->declareTaskData(nKey, bIsInt, bSave);
    }

	// 获取当前天赋页
	virtual int getCurTalentPage()
	{
		__IBankPart *pBank = getBankPart() ;
		if (pBank == NULL)
			return 1;

		return pBank->getCurTalentPage();
	}

	// 获取当前符文页
	virtual int getCurRunePage()
	{
#ifdef FUNCTION_RUNE_OPEN
		__IBankPart *pBank = getBankPart() ;
		if (pBank)
			return pBank->getRuneMgr()->getCurRunePage();
#endif

        return 1;
	}


	// 获取其他玩家当前符文数据
	virtual string getOtherRuneData()
	{
#ifdef FUNCTION_RUNE_OPEN
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}
		obuf data;
		pBank->getRuneMgr()->getOtherRuneData(data);
		return string(data.data(), data.size());
#else
        return "";
#endif
	}


	// 获取其他玩家当前皇冠页数据
	virtual string getOtherCrownPageData()
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}
		obuf data;
		pBank->getOtherCrownPageData(data);
		return string(data.data(), data.size());
	}

	// 获取其他玩家当前宝石数据
	virtual string getOtherCrownGemstoneData()
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}
		obuf data;
		pBank->getOtherCrownGemstoneData(data);
		return string(data.data(), data.size());
	}

	// 获取其他玩家最强英雄信息
	virtual string getOtherStrongHeroInfo()
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
		{
			return "";
		}

		obuf data;
		pBank->getOtherStrongHeroInfo(data);
		return string(data.data(), data.size());
	}
	/////////////////////// IMovePart ///////////////////////////////////////////////
	virtual bool Transport(Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason)
	{
		__IMovePart * pMovePart = (__IMovePart *)m_pReal->getEntityPart(PART_MOVE);
		if(pMovePart == NULL)
		{
			return false;               
		}

		Move3 move = pMovePart->getMoveState();
		move.vPosition = newLoc;
        if(rotation != Vector3(-1,-1,-1))
        {
            move.vRotation = rotation;
        }

		if (bTransport)
		{
			return pMovePart->stopAt(move, nSceneID, nReason);
		}
		else
		{
			pMovePart->changeMoveState(move, true);
		}
		return true;
	}

	virtual bool MoveTo(Vector3 newLoc)
	{
		// 传送时，目前都优先找到地面，本应是使用此函数外面处理，目前先在此函数处理
		if (true)
		{
			UID uid = m_pReal->getUID();
			// 场景ID
			int nSceneID = UID_2_SCENE(uid);
			PathFindHelper helper(nSceneID);
			IPathFindService *pPathService = helper.m_ptr;
			if (pPathService == NULL)
			{
				return false;
			}
			// 找到地面的点
			vector<Vector3> tempLoc;
			pPathService->GetPath(newLoc, newLoc, tempLoc);
			if (tempLoc.empty())
			{
				return false;
			}
			newLoc = tempLoc[0];
		}

		__IMovePart * pMovePart = (__IMovePart *)m_pReal->getEntityPart(PART_MOVE);
		if(pMovePart == NULL)
		{
			return false;               
		}

		pMovePart->moveTo(newLoc);
		return true;
	}

	// 获得临时天赋
	virtual void activeTempTalent(int nTalentID)
	{
		__ISpellPart* pSpellPart = (__ISpellPart*)m_pReal->getEntityPart(PART_SPELL);
		if(pSpellPart == NULL)
		{
			return;
		}

		pSpellPart->activeTalent(nTalentID);
	}

	// 取消临时天赋
	virtual void deActiveTempTalent(int nTalentID)
	{
		__ISpellPart* pSpellPart = (__ISpellPart*)m_pReal->getEntityPart(PART_SPELL);
		if(pSpellPart == NULL)
		{
			return;
		}

		pSpellPart->deActiveTalent(nTalentID);
	}

	// 是否对某阵营可见
	virtual bool isInsight(int nCamp)
	{
		__IEntityAOI* pEntityAOIPart = (__IEntityAOI*)m_pReal->getEntityPart(PART_AOI);
		if(pEntityAOIPart == NULL)
		{
			return false;
		}

		return pEntityAOIPart->isInsight(nCamp);
	}

	///////////////////////////////IGoodsPart///////////////////////////////////////////

	virtual void release()
	{
		if ( m_pReal )
		{
			m_pReal->release();
			m_pReal = 0;
		}

		delete this;
	}

	// 选择目标
	virtual bool chooseTarget(int nTargetFilter, UID uidTarget)
	{
		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
		if(pPlayerKillPart == NULL)
		{
			return false;
		}

		return pPlayerKillPart->chooseTarget(nTargetFilter, uidTarget);
	}

	// 取得PK类型
	virtual int getPKType(void)
	{
		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
		if(pPlayerKillPart == NULL)
		{
			return 0;
		}

		return pPlayerKillPart->getPKType();
	}

	// 检测切磋
	virtual bool checkPK(UID uidTarget)
	{
		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
		if(pPlayerKillPart == NULL)
		{
			return false;
		}

		return pPlayerKillPart->checkPK(uidTarget);
	}

	// 取得PK状态
	virtual int getPKState(void)
	{
		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
		if(pPlayerKillPart == NULL)
		{
			return 0;
		}

		return pPlayerKillPart->getPKState();
	}

    //强制结束PK状态
    virtual void  forceEndPK()
    {
        __IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return;
        }

        pPlayerKillPart->forceEndPK();
    }

	// 获取PK阵营
	virtual int getPKCamp()
	{
		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)m_pReal->getEntityPart(PART_PK);
		if (pPlayerKillPart == NULL)
		{
			return 0;
		}

		return pPlayerKillPart->getPKCamp();
	}

    virtual int getMapID(void)
    {
        return m_pReal->getMapID(); 
    }

	/////////////////////// __IProgressBarPart ///////////////////////////////////////////////
#	define getProgressPart() ((__IProgressBarPart*)m_pReal->getEntityPart(PART_PROGRESS_BAR))
	virtual void StartProgressBar( ProgressBarContext* pContext )
	{
		__IProgressBarPart * pProgressBar = getProgressPart();
		if (pProgressBar == NULL)
		{
			return;
		}

		if ( pProgressBar->GetCurrentContext()!=0 )
		{
			pProgressBar->CloseProgressBar(0,true);
		}

		pProgressBar->StartProgressBar(pContext);	
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 机器人行为
	virtual void onEventRobotAction(BYTE nAction, void * data, size_t len)
	{
		__ITestPart * pTestPart = (__ITestPart *)m_pReal->getEntityPart(PART_TEST);
		if (pTestPart)
		{
			pTestPart->onEventAction(nAction, data, len);
		}
	}

	void onDBReturnPlayerRoleData(void* pData, int nLen)
	{
		if(pData == NULL || nLen < sizeof(DBREQ_RESULT_SELECTACTOR))
		{
			ErrorLn( __FUNCTION__  << "," << __LINE__<<":DBREQ_RESULT_SELECTACTOR failed"); 
			return;
		}

        obuf buf;

        // 压入属性部件ID
        buf << (short)PART_PROPERTY;

        int nLenOffset = buf.size();
        // 压入属性数据长度
        buf << (int)0;
        
        int nBegin = buf.size();
        DBREQ_RESULT_SELECTACTOR* select = (DBREQ_RESULT_SELECTACTOR *)pData;
        PlayerRoleProperty property;
        property.nPDBID       = select->dwActorID;
        sstrcpyn(property.szName, select->szActorName, sizeof(property.szName));
        if (select->dwMapId != m_createContext.nSceneID)    // 地图修正
        {
            WarningLn(__FUNCTION__": relocation scene DBMapID ="<<select->dwMapId<<" relocationID ="<<m_createContext.nSceneID );
            select->dwMapId = m_createContext.nSceneID;
        }
        property.nSceneID       = select->dwMapId;
        property.location.x     = ((float)select->nLocX)/ACTOR_LOCATION_SCALE;
        property.location.y     = ((float)select->nLocY)/ACTOR_LOCATION_SCALE;
        property.location.z     = ((float)select->nLocZ)/ACTOR_LOCATION_SCALE;
        //SMapLandSchemeInfo* pInfo = gServerGlobal->getSchemeCenter()->getSchemeMapLand()->GetMapLandSchemeInfo(select->dwMapId, EGM_RPG, 0);
        //if(pInfo && !pInfo->vecBornPos.empty()) {
        //    property.angle = Vector3(0, pInfo->vecBornPos.front().fAngleY, 0);
        //}
        property.angle          = Vector3(0,((float)select->nAngle)/ACTOR_LOCATION_SCALE, 0);
        //WarningLn(__FUNCTION__": x="<<select->nLocX<<",y="<<select->nLocY<<",z="<<select->nLocZ<<",angle="<<select->nAngle);

        property.nVocation      = select->nHeroID;
        property.bySex          = select->bSex;
        property.nSkin          = select->nSkinID;
        property.nLevel         = select->nLevel;
        property.nKinID         = select->dwKinID;
        property.nClanID        = select->dwClanID;
        property.nFightCapacity = select->dwFightCapacity;
        property.nGold          = select->dwBindTicket;     // 金币
        property.nTicket        = select->dwTicket;         // 点券
        property.nFaceID        = select->wHeadID;
        // 玩家属性兼容处理
        onActorPropertyCompatible(property);

        property.getDataBuffer(buf, TO_DATABASE);

        // 保存属性数据长度
        *((int*)(buf.data() + nLenOffset)) = buf.size() - nBegin; 

        // 压入bank部件ID
        buf << (short)PART_BANK;
        nLenOffset = buf.size();
        // 压入bank部件长度
        buf << (int)0;
        nBegin = buf.size();
        // 压入bank数据
        buf << pData << nLen;
        buf << m_createContext.dwIdCard << m_createContext.bIsAdult << m_createContext.dwPassportFlags << m_createContext.dwServiceFlags;
        string strPwd(m_createContext.szPwd, sizeof(m_createContext.szPwd));
        buf << strPwd;

        // 保存bank数据长度
        *((int*)(buf.data() + nLenOffset)) = buf.size() - nBegin;

        property.byObserver = (select->nPrivilege & PrivilegeFlag_VipViewBattle);
		if(createPlayerRole(elogin_online, buf.data(), buf.size(), TO_DATABASE, select->dwMapId, property.location))
		{
			// 通知社会服上线
			notifySocialActorLogined(m_client, select->dwActorID);
		}

        // 输出下角色上线信息
        WarningLn("Login: name=["<< property.nPDBID <<"]"<< property.szName <<", mapid="<< property.nSceneID<<", x="<<select->nLocX<<",y="<<select->nLocY<<",z="<<select->nLocZ);
	}

    // 玩家属性兼容处理
    virtual void onActorPropertyCompatible(PlayerRoleProperty& prop)
    {
        ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (!pSchemeCenter)
            return;
        ISchemePersonModel * pModel = pSchemeCenter->getSchemePersonModel();
        if (!pModel)
            return;

        // 皮肤是否合法性校验
        const PersonSkinModelItem * pSkinModelItem = pModel->getPersonSkinModelItem(prop.nVocation, prop.nSkin);
        // 没有这个皮肤直接用默认皮肤
        if (!pSkinModelItem)
        {
            const PersonSkinModelItem * pModelItem = pModel->getPersonSkinModelItem(prop.nVocation);
            // 没有这个英雄
            if (!pModelItem)
                return;

            prop.nSkin = pModelItem->nDefaultSkinID;
        }
    }


	// 是否可以给角色奖励
	virtual bool canPrizeActor(int nPrizeID, int nNum)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return false;

		return pBank->canPrizeActor(nPrizeID, nNum);
	}

	// 给角色奖励
	virtual bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return false;

		return pBank->prizeActor(nPrizeID, nNum, wApproach, dwTargetID, szLogDesc);
	}

	// 给角色奖励组
	virtual bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return false;

		return pBank->groupPrizeActor(nPrizeGroupID, pOutPrizeID, nOutPrizeCount, wApproach, dwTargetID, szLogDesc);
	}

	// 添加宝箱
	virtual bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return false;

		return pBank->addPrizeChest(nChestType, nNum, wApproach, dwTargetID, szLogDesc);
	}

    // 实体通知主人
    virtual void entityNotifyMaster(EntityBelongContext context)
	{
        if(!m_pReal)
            return;
        
        m_pReal->getBelongEntityMgr()->push_back(context);
	}

	// 获得随机卡牌
	virtual int getRandHeroCard(DWORD *pCardIDList, int *pCardNumList, int nCardListSize, WORD wApproach, int nParam)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return 0;

		return pBank->getRandHeroCard(pCardIDList, pCardNumList, nCardListSize, wApproach, nParam);
	}

	// 计算增益卡倍数
	virtual void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple)
	{
		__IBankPart *pBank = getBankPart();
		if (pBank == NULL)
			return;
		pBank->calcBenefitCardMultiple(nGoldMultiple, nActorExpMultiple,nHeroExpMultiple);
	}

    // 冲锋
    virtual void shift(void *pData, int nDataLen)
    {
        __IMovePart * pMovePart = (__IMovePart *)m_pReal->getEntityPart(PART_MOVE);
        if(pMovePart == NULL)
        {
            return;               
        }

        pMovePart->shift(pData, nDataLen);
    }

	///////////////////////IMailPart///////////////////////////////////////////////////
	#	define getMailPart() ((__IMailPart*)m_pReal->getEntityPart(PART_MAIL))
	//// 处理邮件详细数据
	//virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus)
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return false;
	//	
	//	return pMail->dealPlayerMailDetailData(llMailID, isAutoRecvPlus);
	//}

	//// 添加邮件详细数据
	//virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus)
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return;

	//	pMail->addPlayerMailDetailData(sDetailData, isAutoRecvPlus);
	//}

	// 处理邮件发送附件数据
	virtual bool dealSendMailPlusData(SMailSendData sendData)
	{
		__IMailPart *pMail = getMailPart();
		if (pMail == NULL)
			return false;

		return pMail->dealSendMailPlusData(sendData);
	}

	//// 获取领取附件时间
	//virtual DWORD getLastRecvPlusTime()
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return 0;

	//	return pMail->getLastRecvPlusTime();
	//}

	//// 设置领取附件时间
	//virtual void setLastRecvPlusTime(DWORD dwTime)
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return;

	//	pMail->setLastRecvPlusTime(dwTime);
	//}

	//// 邮件是否已领取
	//virtual bool isAreadyRecvMail(LONGLONG llMailID)
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return false;

	//	return pMail->isAreadyRecvMail(llMailID);
	//}

	//// 是否在快速取件
	//virtual bool isFastRecvMailPlus()
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return false;

	//	return pMail->isFastRecvMailPlus();
	//}

	//// 设置快速取件标识
	//virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount)
	//{
	//	__IMailPart *pMail = getMailPart();
	//	if (pMail == NULL)
	//		return;

	//	pMail->setFastRecvMailPlus(bFlag, nRecvCount);
	//}
	///////////////////////IMailPart end///////////////////////////////////////////////////

	// 建立师徒关系
	virtual void buildMentorship(DWORD dwDesPDBID)
	{
		__IBankPart *pBankPart = getBankPart();
		if (pBankPart == NULL)
			return;

		pBankPart->buildMentorship(dwDesPDBID);
	}

	// 发送任务事件
	virtual void sendActorDataTaskEvent(WORD nTaskEvent, int nParam1, int nParam2, int nParam3)
	{
		__IBankPart *pBankPart = getBankPart();
		if (pBankPart == NULL)
			return;

		pBankPart->sendActorDataTaskEvent(nTaskEvent, nParam1, nParam2, nParam3);
	}

	// 获取永久英雄个数
	virtual int getPermanentHeroCount()
	{
		__IBankPart *pBankPart = getBankPart();
		if (pBankPart == NULL)
			return 0;

		return pBankPart->getPermanentHeroCount();
	}

	// 获取皮肤总个数
	virtual int getTotalSkinCount()
	{
		__IBankPart *pBankPart = getBankPart();
		if (pBankPart == NULL)
			return 0;

		return pBankPart->getTotalSkinCount();
	}

    // 获取静态地图皮肤ID
    virtual int getStaticSkin()
    {
        __IBankPart *pBankPart = getBankPart();
        if (pBankPart == NULL)
            return 0;

        return pBankPart->getStaticSkin();
    }

	// 获取达到等级英雄个数
	virtual int getHeroCountByLevel(WORD nLevel)
	{
		__IBankPart *pBankPart = getBankPart();
		if (pBankPart == NULL)
			return 0;

		return pBankPart->getHeroCountByLevel(nLevel);
	}

	// 保存引导步骤
	virtual void saveGuideStep(int nStep) 
	{
		__ICommonPart * pCommonPart = CAST_TYPE( __ICommonPart*, m_pReal->getEntityPart(PART_COMMON) );
		if(pCommonPart)
		{
			pCommonPart->saveGuideStep(nStep);
		}
	}

	// 设置实名认证状态标志
	virtual void setRealNameAutherFlag()
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank) pBank->setRealNameAutherFlag();
	}

	// 购买商城物品
	virtual bool onMessagePointShopBuyGoods(void *data, size_t len)
	{
		__IBankPart *pBank = getBankPart();
		return (pBank ? pBank->onMessagePointShopBuyGoods(data, len) : false);
	}

	// 战场结束设置数据
	virtual void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo)
	{
		__IBankPart *pBank = getBankPart() ;
		if(pBank) pBank->setMatchRankSeasonInfo(pInfo);
	}

	// 通知LUA添加交互回复
	virtual void sendLuaAddContactReply(LONGLONG llContactID)
	{
		PlayerRole * pPlayerRole = CAST_TYPE(PlayerRole*, m_pReal);
		if(pPlayerRole)
		{
			pPlayerRole->sendLuaAddContactReply(llContactID);
		}
	}

	// 通知LUA添加好友申请
	virtual void sendLuaContactApplyFriend(DWORD dwTargetID, bool bIsDifSex)
	{
		PlayerRole * pPlayerRole = CAST_TYPE(PlayerRole*, m_pReal);
		if(pPlayerRole)
		{
			pPlayerRole->sendLuaContactApplyFriend(dwTargetID, bIsDifSex);
		}
	}

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect( int nEffectID, void * pAddContext, size_t len)
    {
        PlayerRole * pPlayerRole = CAST_TYPE(PlayerRole*, m_pReal);
        if(!pPlayerRole)
        {
            return 0;
        }
        return pPlayerRole->mobaGoodsAddEffect(nEffectID, pAddContext, len);
    }

    // 移除效果
    virtual	void mobaGoodsRemoveEffect( int nID)
    {
        PlayerRole * pPlayerRole = CAST_TYPE(PlayerRole*, m_pReal);
        if(pPlayerRole)
        {
            pPlayerRole->mobaGoodsRemoveEffect(nID);
        }
    }

    // 设置核晶效果
    virtual	void setCrownEffect(int timeAddEffect)
    {
        __IBankPart *pBank = getBankPart();
        if (pBank) pBank->setCrownEffect(timeAddEffect);
    }

    // 购买装备
    virtual	bool luaBuyGoods(int nGoodsID, int nCost)
    {
        __IMobaGoodsPart * pMobaPart = (m_pReal ? (__IMobaGoodsPart*)m_pReal->getEntityPart(PART_MOBAGOODS) : 0);
        if (pMobaPart) return pMobaPart->luaBuyGoods(nGoodsID, nCost);
        else return false;
    }

    // 切换职业
    virtual bool changeVoc(int nNewVoc, int nNewSkinID)
    {
        return m_pReal ? m_pReal->changeVoc(nNewVoc, nNewSkinID) : false;
    }

private:
	bool checkData(void* data, int len)
	{
		int nPreLen = sizeof(UID) + sizeof(int); 
		if(data == NULL || len < nPreLen + sizeof(SActorCreateContext))
		{
			return false;
		}

		SActorCreateContext* pContext = (SActorCreateContext*)((char*)data + nPreLen);
		if(pContext == NULL)
		{
			return false;
		}

		if(len < nPreLen + sizeof(SActorCreateContext) + pContext->nCreateDataLen + pContext->nUpdateDataLen)
		{
			return false;
		}

		return true;
	}

    void crowdResolve()
    {
        __IWarPart* pWarPart = getWarPart();
        if(pWarPart == NULL)
        {
            WarningLn(__FUNCTION__ << " pWarPart == NULL");
            return;
        }

        // 战场内的出生位置是按照玩家序号取的
        if(pWarPart->getWarID() != INVALID_WAR_ID)
        {
            return;
        }

        Vector3 locNow = getLocation();

        int nLoopCnt = 0;
        while(nLoopCnt < 6)
        {
            // 5米内随机偏移
            float fXOffset = mt_range_rand(-50, 50); fXOffset /= (float)10;
            float fZOffset = mt_range_rand(-50, 50); fZOffset /= (float)10;

            Vector3 loc = locNow;
            loc.x += fXOffset;
            loc.z += fZOffset;

            if(gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(getMapID(), loc, 100, 100))
            {
                locNow = loc;
                break;
            }

            ++nLoopCnt;
        }

        setLocation(locNow);
    }

	bool createPlayerRole(ELoginMode mode, void* pCreateData, int nCreateDataLen, int nCreateFlag, 
        int nSceneID, Vector3 vecLoc, void* pUpdateData = NULL, int nUpdateDataLen = 0, int nUpdateFlag = 0)
	{
		m_pReal = new PlayerRole();
		if(m_pReal == 0)
		{
			ErrorLn(__FUNCTION__ ":new PlayerRole failed! out of memory");
			return false;
		}

		PlayerRole * pPlayerRole = CAST_TYPE(PlayerRole*, m_pReal);
		// 创建实体		
		if (!m_pReal->create(*(UID*)&m_uid, pCreateData, nCreateDataLen, nCreateFlag))
		{
			ErrorLn(__FUNCTION__ ":entity create error!");
			return false;
		}

        // 导入更新数据
        if(nUpdateDataLen > 0)
        {
            pPlayerRole->importContext(pUpdateData, nUpdateDataLen, nUpdateFlag);
        }

		m_pReal->setClientID(m_client);
		m_pReal->setServiceID(m_idService);
        
		__IEntityProperty* pProperty = (__IEntityProperty*)m_pReal->getEntityPart(PART_PROPERTY);
		if(pProperty == NULL)
		{
			return false;
		}

		pProperty->setProperty_Integer(PROPERTY_SCENEID, nSceneID);
		if(!(abs(vecLoc.x) < 0.001 && abs(vecLoc.y) < 0.001 && abs(vecLoc.z) < 0.001))
		{
			pProperty->setProperty_custom(PROPERTY_LOCATION, &vecLoc, sizeof(vecLoc));
		}

		// 设置地图
		int nMapID = gServerGlobal->getSceneManager()->sceneIDToMapID(nSceneID);
		pPlayerRole->setMapID(nMapID);

		// 随机一下位置
		crowdResolve();

		// 通知创建完成
		if (!pPlayerRole->onCreated(mode))
		{
			return false;
		}

		// 主城创建角色,则更新BankPart中playerinfo中的mapid,当玩家在主城下线时，才会保存当前地图id到数据库
		if (IsMainMap(nMapID))
		{
			setActorBaseInfo(nMapID, ENPLAYERINFOTYPE_MAPID, ENCHANGEDATAMODE_SET, 0, 0, 0, 0, 0, 0);
		}

        // 发出登进事件
		// if(!m_bIsComputerPlayer)
		{
			fireLoginEvent(pProperty->getProperty_Integer(PROPERTY_ID), mode);
		}

		// 如果在静态场景改变玩家状态
		if (!IsDynamicScene(getSceneID()))
		{
			setGameState(GAME_STATE_IDLE);
		}


		return true;
	}

	// 发出登进事件
	void fireLoginEvent(PDBID dwDBID, ELoginMode eMode)
	{
        // 登录上线时加锁
        if (eMode== elogin_online)
        {
            m_actorServiceDBSink.setActorLoginLockFlag(getUserID(), dwDBID);
        }

		event_actor_login eventdata;
		eventdata.client = m_client;
		eventdata.dwDBID = dwDBID;
		eventdata.uid = m_pReal->getUID();
		eventdata.nSceneID = m_pReal->getSceneID();
		eventdata.eMode = eMode;

        // 人物内部发实体登录事件
        __IEntityEvent *pEntityEvent = m_pReal->getEntityEvent();
        if (pEntityEvent)
        {
            pEntityEvent->FireExecute(EVENT_ACTOR_LOGIN, (LPCSTR)&eventdata, sizeof(eventdata));
            pEntityEvent->FireExecute(EVENT_GAME_ACTOR_LOGIN, (LPCSTR)&eventdata, sizeof(eventdata));
        }
        
        // 全局发实体登录事件
        rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
        if(pEntityEngine)
        {
            pEntityEngine->FireExecute(EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, dwDBID, (LPCSTR)&eventdata, sizeof(eventdata));
        }
	}

	// 发出登出事件
	void fireLogoutEvent(PDBID dwDBID, UID uid, int nSceneID, int nReason)
	{  
		event_actor_logout eventdata;
		eventdata.client = m_client;
		eventdata.dwDBID = dwDBID;
		eventdata.uid = uid;
		eventdata.nSceneID = nSceneID;
		eventdata.eMode = (ELogoutMode)nReason;
        
		rkt:: IEventEngine* pEntityEngine = gServerGlobal->getEventEngine();
		if(pEntityEngine)
		{
            pEntityEngine->FireExecute(EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0, (LPCSTR)&eventdata, sizeof(eventdata));
		}
	}

	// 通知社会服玩家上线
	void notifySocialActorLogined(ClientID client, DWORD dwDBID)
	{
        obuf buf;
        buf << dwDBID << client;

		ICenterConnectorService* pCenterConnectorService =  gServerGlobal->getCenterConnectorService();
		if(pCenterConnectorService)
		{
            // 子消息相关
            SMsgMatchDataSubMsg subMsg;
            subMsg.dwMsgCode = MSG_MATCH_ACTOR_LOGIN_OK;

            // 数据打包
            obuf256 obMsg;
            obMsg<<subMsg;
            obMsg.push_back(buf.data(), buf.size());

            pCenterConnectorService->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCH_SERVICE_MODULEMSG, (const char*)obMsg.data(), obMsg.size());
		}
	}

    


	//////////////////////////////////////////////////////////////////////////
	//#	undef getWarPart
	//#	undef getProperty
	//#	undef getAOI
	//#	undef getEnmity
	//#	undef getBuffPart
};