/*******************************************************************
** 文件名:	EntityHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/13/2015
** 版  本:	1.0
** 描  述:	

		实体相关的常用函数包装
********************************************************************/


#pragma once

#include "IEntity.h"
#include "IServerGlobal.h"
#include "ServiceHelper.h"
#include "IEntityProperty.h"
#include "IActorService.h"
#include "INPCService.h"
#include "ITankService.h"
#include "ISceneManager.h"
#include "ISceneLogicService.h"
#include "IGatewayAcceptorService.h"
#include "IActorMgr.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得人物指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
// Actor参数类型
enum ACTOR_HELPER_PARAM
{
    ACTOR_UID=0,            // 角色UID
    ACTOR_DBID,             // 角色ID
    ACTOR_CLIENT,           // 网关ID
};

class ActorHelper
{
public:
	SERVICE_PTR m_pContainer;
	IActorService * m_ptr;

    ActorHelper(DWORD actorID, ACTOR_HELPER_PARAM paramType = ACTOR_UID) : m_ptr(0)
    {
        if (paramType == ACTOR_UID)
        {
            ActorHelper_UID(actorID);
        }
        else if (paramType == ACTOR_DBID)
        {
            ActorHelper_PDBID(actorID);
        }
        else if (paramType == ACTOR_CLIENT)
        {
            ActorHelper_Client(actorID);
        }
    }

private:
    // 用角色UID查找Actor
	void ActorHelper_UID(UID uid)
	{
        if(uid == 0 || uid==INVALID_UID)
        {
            return;
        }
        IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
        if (pActorMgr == 0)
        {
            return;
        }
		m_pContainer = pActorMgr->getActorByUID(uid);
        if (m_pContainer == 0)
        {
            return;
        }
		m_ptr = (IActorService*)m_pContainer->query_interface();
	}

    // 用角色ID查找Actor
    void ActorHelper_PDBID(PDBID pdbid)
    {
        if(pdbid == INVALID_PDBID)
        {
            return;
        }
        IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
        if (pActorMgr == 0)
        {
            return;
        }
        m_pContainer = pActorMgr->getActorByPDBID(pdbid);
        if (m_pContainer == 0)
        {
            return;
        }
        m_ptr = (IActorService*)m_pContainer->query_interface();
    }

    // 用网关ID查找Actor
    void ActorHelper_Client(ClientID nClient)
    {
        if(nClient == INVALID_CLIENT)
        {
            return;
        }
        IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
        if (pActorMgr == 0)
        {
            return;
        }
        m_pContainer = pActorMgr->getActorByClientID(nClient);
        if (m_pContainer == 0)
        {
            return;
        }
        m_ptr = (IActorService*)m_pContainer->query_interface();
    }

};

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得怪物指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class MonsterHelper
{
public:
	SERVICE_PTR m_pContainer;
	INPCService * m_ptr;
	UID			m_uid;

	MonsterHelper(UID uid) : m_pContainer(0),m_ptr(0), m_uid(uid)
	{
		onInit(UID_2_SCENE(uid));
	}
	MonsterHelper(int nSceneID) : m_pContainer(0),m_ptr(0), m_uid(0)
	{
		onInit(nSceneID);
	}

	void onInit(int nSceneID)
	{
		ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
		if (pSceneManager == 0)
		{
			return;
		}
		SERVICE_PTR pGameSceneService = pSceneManager->getScene(nSceneID);
		if (pGameSceneService == 0)
		{
			return;
		}
		IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
		if (pScene == 0)
		{
			return;
		}
        UINT_PTR serviceID = pScene->getLogicServiceID();
		if (serviceID == INVALID_SERVICE_ID)
		{
			return;
		}
		m_pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
		if (m_pContainer == 0)
		{
			return;
		}
        ISceneLogicService * pService = (ISceneLogicService*)m_pContainer->query_interface();
        if(pService == 0)
        {
            return;
        }
		m_ptr = pService->getNPCService();
	}	
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得载具指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class TankHelper
{
public:
	SERVICE_PTR m_pContainer;
	ITankService * m_ptr;

	TankHelper(UID uid) : m_pContainer(0),m_ptr(0)
	{
		onInit(UID_2_SCENE(uid));
	}
	TankHelper(int nSceneID) : m_pContainer(0),m_ptr(0)
	{
		onInit(nSceneID);
	}

	void onInit(int nSceneID)
	{
		ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
		if (pSceneManager == 0)
		{
			return;
		}
		SERVICE_PTR pGameSceneService = pSceneManager->getScene(nSceneID);
		if (pGameSceneService == 0)
		{
			return;
		}
		IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
		if (pScene == 0)
		{
			return;
		}
        UINT_PTR serviceID = pScene->getLogicServiceID();
		if (serviceID == INVALID_SERVICE_ID)
		{
			return;
		}
		m_pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
		if (m_pContainer == 0)
		{
			return;
		}
        ISceneLogicService * pService = (ISceneLogicService*)m_pContainer->query_interface();
        if(pService == 0)
        {
            return;
        }
		m_ptr = pService->getTankService();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得SceneHelper指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class SceneHelper
{
public:
	SERVICE_PTR m_pContainer;
	IGameSceneService * m_ptr;

	SceneHelper(int nSenceID) : m_pContainer(0), m_ptr(0)
	{
		ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
		if (pSceneManager == 0)
		{
			return;
		}
		m_pContainer = pSceneManager->getScene(nSenceID);
		if (m_pContainer == 0)
		{
			return;
		}
		m_ptr = (IGameSceneService*)m_pContainer->query_interface();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得SceneLogicService指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class SceneLogicHelper
{
public:
	SERVICE_PTR m_pContainer;
	ISceneLogicService * m_ptr;

	SceneLogicHelper(int nSenceID) : m_pContainer(0),m_ptr(0)
	{
		ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
		if (pSceneManager == 0)
		{
			return;
		}
		SERVICE_PTR pGameSceneService = pSceneManager->getScene(nSenceID);
		if (pGameSceneService == 0)
		{
			return;
		}
		IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
		if (pScene == 0)
		{
			return;
		}
        UINT_PTR serviceID = pScene->getLogicServiceID();
		if (serviceID == INVALID_SERVICE_ID)
		{
			return;
		}
		m_pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
		if (m_pContainer == 0)
		{
			return;
		}
		m_ptr = (ISceneLogicService*)m_pContainer->query_interface();
	}
};

class SceneLogicEventHelper
{
public:
	rkt::IEventEngine * m_ptr;
    SERVICE_PTR m_pContainer;
	
	SceneLogicEventHelper(int nSenceID) : m_ptr(0)
	{
        SceneLogicHelper helper(nSenceID);
        m_pContainer = helper.m_pContainer;

        if(helper.m_ptr)
        {
            m_ptr = helper.m_ptr->getEventService();
        }
	}
};

class SceneLuaHelper
{
public:
    ILuaService * m_ptr;
    SERVICE_PTR m_pContainer;

    SceneLuaHelper(int nSenceID) : m_ptr(0)
    {
        SceneLogicHelper helper(nSenceID);
        m_pContainer = helper.m_pContainer;

        if(helper.m_ptr)
        {
            m_ptr = helper.m_ptr->getLuaService();
        }
    }
};

struct IAIService;

class AIServiceHelper
{
public:
    SERVICE_PTR m_pContainer;
    IAIService* m_ptr;

    AIServiceHelper(int nSceneID) : m_pContainer(0), m_ptr(0)
    {
        SceneLogicHelper helper(nSceneID);
        if(helper.m_ptr)
        {
            m_pContainer = helper.m_ptr->getAIService();
        }

        if(m_pContainer != 0)
        {
            m_ptr = (IAIService*)m_pContainer->query_interface();
        }
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
static bool isActor(UID uid)
{
	return UID_2_TYPE(uid)==TYPE_PLAYER_ROLE;
}

static bool isMonster(UID uid)
{
	return UID_2_TYPE(uid)==TYPE_MONSTER;
}

static bool isTank(UID uid)
{
	return UID_2_TYPE(uid)==TYPE_TANK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 分类型辅助调用
//////////////////////////////////////////////////////////////////////////////////////////////////
#define  UID_HELPER_CALL( uid,method,func,...)				\
	if ( isActor(uid) )										\
	{														\
		ActorHelper proxy(uid);								\
		if ( proxy.m_ptr ){									\
			return proxy.m_ptr->func( __VA_ARGS__ );        \
		}                                                   \
	}														\
															\
	else if ( isMonster(uid) )								\
	{														\
		MonsterHelper proxy(uid);							\
		if ( proxy.m_ptr ){									\
			return proxy.m_ptr->func(uid,__VA_ARGS__);      \
		}													\
	}														\
															\
	else if ( isTank(uid) )                                 \
	{                                                       \
		TankHelper proxy(uid);                              \
		if ( proxy.m_ptr ){                                 \
		    return proxy.m_ptr->func(uid,__VA_ARGS__);      \
		}													\
	}                                                       \

//////////////////////////////////////////////////////////////////////////////////////////////////
// 分类型辅助调用返回值
//////////////////////////////////////////////////////////////////////////////////////////////////
#define  UID_HELPER_CALL_VALUE(nValue,uid,method,func,...)	\
	if ( isActor(uid) )										\
	{														\
		ActorHelper proxy(uid);								\
		if ( proxy.m_ptr ){									\
			nValue = proxy.m_ptr->func( __VA_ARGS__ );      \
		}                                                   \
	}														\
															\
	else if ( isMonster(uid) )								\
	{														\
		MonsterHelper proxy(uid);							\
		if ( proxy.m_ptr ){									\
			nValue = proxy.m_ptr->func(uid,__VA_ARGS__);    \
		}													\
	}														\
															\
	else if ( isTank(uid) )									\
	{                                                       \
		TankHelper proxy(uid);                              \
		if ( proxy.m_ptr ){                                 \
		    nValue = proxy.m_ptr->func(uid,__VA_ARGS__);    \
		}													\
	}                                                       \

// 无效UID
static bool isInvalidUID(UID uid)
{
	return (uid == INVALID_UID || uid == 0);
}
static bool isValidUID(UID uid)
{
    return (uid != INVALID_UID && uid != 0);
}

// 无效点
static bool isInvalidPoint(Vector3 vLoc)
{
    if (_isnan(vLoc.x) || _isnan(vLoc.y) || _isnan(vLoc.z))
    {
        return true;
    }

    return false;
}

// 是否死亡状态
static bool isDieState(UID uid,int nMethod=MSG_FLAG_DEFAULT)
{
	int nValue = 0;
	UID_HELPER_CALL_VALUE( nValue, uid, nMethod, getState );
	if (nValue == ENTITY_STATE_DIE)
	{
		return true;
	}
	return false;
}


// 取得实体位置
static Vector3 getLocation(UID uid,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,getLocation );
	return Vector3(0,0,0);
}

// 设置实体位置
static void setLocation(UID uid, Vector3 vLoc, int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,setLocation, vLoc );
}

// 取得实体角度
static Vector3 getAngle(UID uid,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,getAngle );
	return Vector3(0,0,0);
}

// 传送
static bool Transport(UID uid, Vector3 newLoc, Vector3 rotation=Vector3(-1,-1,-1), bool bTransport=true,int nSceneID=0, int nReason=TRANSPORT_REASON_FLASH,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,Transport,newLoc,rotation,bTransport,nSceneID, nReason );
	return false;
}

// 传送
static bool MoveTo(UID uid, Vector3 newLoc,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,MoveTo,newLoc );
    return false;
}

// 取得实体位置
static int getMapID(UID uid,int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid,nMethod,getMapID );
    return INVALID_MAPID;
}

// 冲锋
static void shift(UID uid, void *pData, int nDataLen, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL(uid, nMethod, shift, pData, nDataLen);
}

// 收到消息
// @param uid : 要处理消息的实体UID
// @param msg : 消息内容
// @param nMethod : 同步 or 异步模式
static void handleMessage( UID uid,void *pData,int nLen,int nMethod=MSG_FLAG_DEFAULT )
{
	UID_HELPER_CALL( uid,nMethod,handleMessage,pData,nLen );
}

// 收到消息2
// @param uid : 要处理消息的实体UID
// @param msg : 消息内容
// @param nMethod : 同步 or 异步模式
static void handleMessageEx( UID uid,unsigned long ulKey,void * pMsg,int nLen )
{
    UID_HELPER_CALL( uid,MSG_FLAG_DEFAULT,handleReplaceableMessage,ulKey, pMsg,nLen );
}

// 发送数据给客户端
// nFlag
static void send2Client( UID uid,PACKAGE_PTR package,int nFlag = 0)
{
	ActorHelper proxy(uid);
	if ( proxy.m_ptr ) 
	{
		proxy.m_ptr->send2Client( package,nFlag );
	}
}

// 发送数据给客户端
static void send2ClientAndOB(UID uid, PACKAGE_PTR package)
{
    ActorHelper proxy(uid);
    if (proxy.m_ptr)
    {
        proxy.m_ptr->send2ClientAndOB(package);
    }
}


struct EntityBelongContext;
// 实体通知主人
static void entityNotifyMaster(UID uid, EntityBelongContext context, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid,nMethod,entityNotifyMaster,context);
}


/////////////////////// IEntityProperty ///////////////////////////////////////////////
// 取得某个属性的类型
// @param prop_id : 属性ID
// @param nMethod : 同步 or 异步模式
static string getPropertyType( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT )
{
	UID_HELPER_CALL( uid,nMethod,getPropertyType,prop_id );
	return "";
}

// 取得某个属性的标志数据(通常是序列化标志)
static int getPropertyFlag( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT )
{
	UID_HELPER_CALL( uid,nMethod,getPropertyFlag,prop_id );
	return 0;
}

// 获取实体属性(布尔类型）
static bool getProperty_Bool( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,getProperty_Bool,prop_id );
	return false;
}

// 设置实体属性(布尔类型）
static bool setProperty_Bool( UID uid,int prop_id,bool value,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,setProperty_Bool,prop_id,value );
	return false;
}

// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
static int getProperty_Integer( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,getProperty_Integer,prop_id );
	return 0;
}

// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
static bool setProperty_Integer( UID uid,int prop_id,int value,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,setProperty_Integer,prop_id,value );
	return false;
}

// 增加属性数值(注意内部需要控制上下限)
static bool addProperty_Integer( UID uid,int prop_id,int delta,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,addProperty_Integer,prop_id,delta );
	return false;
}

// 增加经验数值
static void addExp( UID uid,int nExp, WORD wApproach, DWORD dwTargetID=0, const char * pszRemark=0,int nLogReserve=0, int nMethod=MSG_FLAG_DEFAULT)
{
    if ( UID_2_TYPE(uid)!=TYPE_PLAYER_ROLE )
    {
        return;
    }
    ActorHelper proxy(uid);
    if ( proxy.m_ptr ) 
    {
        proxy.m_ptr->addExp(nExp, wApproach, dwTargetID, pszRemark, nLogReserve);
    }
}

// 更新天赋点属性
//addProperty_Integer(uid,PROPERTY_TALENT_POINT,nAddPoint);
static void updateTalentPoint( UID uid, int nAddPoint,int nType, BYTE byAddType = 2 )
{
    if ( UID_2_TYPE(uid)!=TYPE_PLAYER_ROLE )
    {
        return;
    }

    ActorHelper proxy(uid);								
    if (proxy.m_ptr)
    {
        proxy.m_ptr->updateTalentPoint(nAddPoint,nType, byAddType);
    }
    //UID_HELPER_CALL( uid, nMethod,updateTalentPoint,nAddPoint, nType);
    return;
}

// 获取实体属性(字符串)
static string getProperty_String( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT )
{
	UID_HELPER_CALL( uid,nMethod,getProperty_String,prop_id );
	return "";
}

// 设置实体属性(字符串)
static bool setProperty_String( UID uid,int prop_id,string value,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,setProperty_String,prop_id,value );
	return false;
}

// 获取实体属性(自定义结构,如果属性存在返回)
static string getProperty_custom( UID uid,int prop_id,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,getProperty_custom,prop_id );
	return "";
}

// 设置实体属性(自定义结构)
static bool setProperty_custom( UID uid,int prop_id,string value,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod,setProperty_custom,prop_id,value );
	return false;
}

// 加Buff
static bool AddBuff( UID uid, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0, int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid,nMethod, AddBuff, dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
	return false;
}

// 删Buff
static bool RemoveBuff( UID uid, DWORD dwBuffID, UID uidAdd, UID uidRemove,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid, nMethod,RemoveBuff, dwBuffID, uidAdd, uidRemove);
	return false;
}

// 某Buff是否存在
static bool IsExistBuff( UID uid, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid, nMethod, IsExistBuff, dwBuffID, dwBuffLevel, uidAdd);
	return false;
}

// 某Buff类型是否存在
static bool IsExistBuffType( UID uid, DWORD dwBuffType, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, IsExistBuffType, dwBuffType);
    return false;
}

// 取玩家某个BUFF的等级
static DWORD GetBuffLevel( UID uid, DWORD dwBuffID, UID uidAdd,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid, nMethod,GetBuffLevel, dwBuffID, uidAdd);
	return 0;
}

// 取玩家某个BUFF剩余时间
static int GetLeftTime( UID uid, DWORD dwBuffID, UID uidAdd,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid, nMethod,GetLeftTime, dwBuffID, uidAdd);
	return -1;
}

// 设置累积BUFF
static void SetAccumulateBuff( UID uid, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0, int nMethod=MSG_FLAG_NO_BLOCK)
{
	UID_HELPER_CALL( uid, nMethod, SetAccumulateBuff, dwBuffID, uidAdd, bOverlay, dwTime, pBuffContext, nContextLen);
	return;
}

//  根据索引删BuffBUFF
static bool RemoveBuffByIndex( UID uid, DWORD dwIndex,int nMethod=MSG_FLAG_DEFAULT)
{
	UID_HELPER_CALL( uid, nMethod, RemoveBuffByIndex, dwIndex);
	return false;
}

// 取得实体当前状态
static int GetState( UID uid,int nMethod=MSG_FLAG_DEFAULT)
{	
	UID_HELPER_CALL( uid, nMethod, getState);
	return 0;
}

// 添加仇恨数据
static void AddEnmityValue(UID uid, UID uidDes, int nVal, int nMethod=MSG_FLAG_NO_BLOCK)
{
	UID_HELPER_CALL( uid, nMethod, AddEnmityValue, uidDes, nVal);
	return;
}
static void AddBroadEnmityValue(UID uid, UID uidDes, int nVal, int nMethod=MSG_FLAG_NO_BLOCK)
{
	UID_HELPER_CALL( uid, nMethod, AddBroadEnmityValue, uidDes, nVal);
	return;
}

static bool isInsight(UID uid, int camp, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, isInsight, camp );
    return false;
}

static int getWarID(UID uid, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, getWarID);
    return 0;
}

static BuffTypeList getBuff(UID uid, DWORD dwBuffType, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, getBuff, dwBuffType);

    return BuffTypeList();
}

static int getEntityPKType(UID uid, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, getPKType);
    return 0;
}

static bool chooseTarget(UID uid, int nTargetFilter, UID uidTarget, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, chooseTarget, nTargetFilter, uidTarget);
    return false;
}

static int getEntityPKState(UID uid, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, getPKState);
    return 0;
}


static int mobaGoodsAddEffect( UID uid, int nEffectID, void * pAddContext, size_t len, int nMethod=MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, mobaGoodsAddEffect, nEffectID, pAddContext, len);
    return 0;
}

static void mobaGoodsRemoveEffect(UID uid, UID nID, int nMethod=MSG_FLAG_NO_BLOCK)
{
    UID_HELPER_CALL( uid, nMethod, mobaGoodsRemoveEffect, nID);
    return;
}

static void forceEndPK(UID uid, int nMethod=MSG_FLAG_NO_BLOCK)
{
    UID_HELPER_CALL( uid, nMethod, forceEndPK);
    return;
}

static int getPKCamp(UID uid, int nMethod= MSG_FLAG_DEFAULT)
{
    UID_HELPER_CALL( uid, nMethod, getPKCamp);
    return 0;
}

//////////////////////////////////////////////////////////////////////////
#include "ISchemeCenter.h"
#include "SchemeDef.h"

// 获取静态地图重生点
inline Vector3 getMapReliveLocation(int nMapID, BYTE nModeType, int nCampID)
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter)
	{
		ISchemeMapLand* pSchemeMapLand = pSchemeCenter->getSchemeMapLand();
		if (pSchemeMapLand)
		{
			SMapLandSchemeInfo * pMapLandSchemeInfo = pSchemeMapLand->GetMapLandSchemeInfo(nMapID,nModeType,nCampID);
			if (pMapLandSchemeInfo && !pMapLandSchemeInfo->vecBornPos.empty())
			{
                int nIndex = rand() % pMapLandSchemeInfo->vecBornPos.size(); 
                return pMapLandSchemeInfo->vecBornPos[nIndex].ptPos;
			}	
		}			
	}

	return Vector3();
}

inline __IEntityProperty* getPropertyPart(__IEntity* pEntity)
{
	if (pEntity == NULL) return NULL;
	return (__IEntityProperty*)pEntity->getEntityPart(PART_PROPERTY);
}

// 实体是否可选择
static bool EntitySelect(UID uid)
{
	if (isInvalidUID(uid))
	{
		return false;
	}

	int nMask = getProperty_Integer(uid, PROPERTY_MASK);
	if ((nMask&MASKFLAG_NOT_SELECT) == MASKFLAG_NOT_SELECT)
	{
		return false;
	}

	return true;
}
