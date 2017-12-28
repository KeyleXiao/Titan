/*******************************************************************
** 文件名:	PlayerRole.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家实体对象
********************************************************************/


#pragma once

#include "IPlayerRole.h"
#include "Vector3.h"
#include "net/net.h"
#include "net/producer_consumer_queue.h"
#include "net/AsynIoFrame.h"
//#include "MessageDispatch.h"
#include "IServerGlobal.h"
#include "IGatewayAcceptorService.h"
#include "EntityEvent.h"
#include "IServiceContainer.h"

#include "EntityStandState.h"
#include "EntityDieState.h"
#include "EntityFightState.h"
#include "MobaGoodsEffect.h"
#include "BelongEntityMgr.h"
#include "EntityUseFlagMgr.h"

#include <list>

using std::list;
using namespace rkt;
struct IEffect;

class PlayerRole : public __IPlayerRole
{
public:
	////////////////////__IPlayerRole//////////////////////////////////////////////////////
	virtual int send2Client( PACKAGE_PTR package, int nFlag = 0 );

	virtual int send2Client( void * data,size_t len)
	{
		return send2Client(PACKAGE_PTR(new string((const char *)data,len)));
	}

    virtual void send2ClientAndOB( PACKAGE_PTR package );

	virtual bool sendMessageToClient(BYTE byPartID, BYTE byAction, void * data,size_t len);

	// 取得玩家所在网关分配ID
	virtual DWORD getClientID() { return m_clientID; }
	// 设置玩家所在网关分配ID
	virtual void setClientID(DWORD clientID) { m_clientID = clientID; }

	// 取得玩家SERVICE_ID
	virtual SERVICE_ID getServiceID() { return m_serviceID; }
	// 设置玩家SERVICE_ID
	virtual void setServiceID(SERVICE_ID serviceID) { m_serviceID = serviceID; }

    // 更改天赋点属性
    virtual void updateTalentPoint( int nPoint, int nChangeType, BYTE byAddType = 2);

    // 导出静态数据
    virtual bool deSerializeStaticData(rkt::obuf & out);

    // 检查是否有对应的权限
    virtual bool checkPrivilege(int nPrivilege);

    // 增加权限
    virtual void addPrivilege(int nPrivilege);

    // 移除权限
    virtual void removePrivilege(int nPrivilege);

    // 设置游戏状态ACTOR_GAME_STATE
    virtual void setGameState(DWORD dwState);
    // 获取游戏状态 ACTOR_GAME_STATE
    virtual DWORD getGameState();

    // 获得玩家角色等级
    virtual int getPlayerLevel();

    // 获得玩家角色经验
    virtual int getPlayerExp();

	/** 增加/减少经验值
	@param   nExp:            经验值（负值减少，正值增加）
	@param   wApproach:       消耗原因,0.不需要记录日记
	@param   dwTargetID:      如果有目标者，这个就填入目标者ID
	@param   szLogMark:       传入交易日志，供查询  
	@param   nLogReserve:     日志的保留字段
	@return
	*/
    virtual bool addExp(int nExp, WORD wApproach, DWORD dwTargetID, const char * pszRemark, int nLogReserve);
    
    /** 是否可增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod);

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
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark, int nSubclassID, short nNum, bool bAddictLimit);

    /** 切职业
    @param nNewVocID: 新的职业ID
    @param nNewSkinID: 新的皮肤ID
    @return : 
    */
    virtual bool changeVoc(int nNewVocID, int nNewSkinID);
    ////////////////////PlayerRole//////////////////////////////////////////////////////

	PlayerRole();

	virtual ~PlayerRole();

	bool create(UID uid,void * context,int len,int flag);

    // 导入对象的创建上下文
    bool importContext(void * pContext, size_t len, int nFlag);

    // 对象创建成功通知
    bool onCreated(ELoginMode loginMode);

	void release(); 

	void destroy();

	virtual UID getUID() const;

	virtual ENTITY_TYPE getType();

	virtual char* getName() ;

	virtual int getSceneID() const;

	// 取得当前所在地图ID
	virtual int getMapID() const;

	virtual Vector3 getLocation() const;

	virtual void setLocation(Vector3 vLoc);

    virtual Vector3 getAngle() const;

	virtual __IEntityPart * getEntityPart(ENTITY_PART id);

	virtual __IEntityEvent * getEntityEvent() { return m_pEventEngine;}

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	virtual void handleMessage( void * data,int32 len );

	// 改变当前状态 
	virtual bool changeState(int nState);

	// 取得当前状态 
	virtual int getState(void);

	// 取得数值属性
	virtual int getIntProperty(int nPropID);

    // 设置数值属性
    virtual bool setIntProperty(int nPropID, int nValue);

	// 取得字符串属性
	virtual std::string getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int	getFromGameWorldID();

    // 取得账号ID
    virtual	DWORD getUserID();

	// 设置地图ID
	void setMapID(int nMapID);

	void updateName(char* pNewName);

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len);

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(int nID);


    // 移除效果
    virtual	void releaseMobaGoodsEffect();

    // 获取附属实体管理对象
    virtual IBelongEntityMgr* getBelongEntityMgr(void) { return &m_belongEntityMgr; }

    // 获取用途标识对象
    virtual IUseFlag* getUseFlag() { return &m_UseFlagMgr; }

	// 通知LUA添加交互回复
	virtual void sendLuaAddContactReply(LONGLONG llContactID);

	// 通知LUA添加好友申请
	virtual void sendLuaContactApplyFriend(DWORD dwTargetID, bool bIsDifSex);

protected:
	// 处理客户端来的消息
	void onMessage(BYTE msgID, SNetMsgHead * head, void * data,int32 len);

    // 执行Destory时释放部件
    void onDestroyFreePartList(void);
    // 执行Release时释放部件
    void onReleaseFreePartList(void);

protected:
	__IEntityPart *		m_pPartList[ENTITY_PART_MAX];		// 记录实体Part指针
	EntityEvent   *		m_pEventEngine;
	DWORD				m_clientID;
	SERVICE_ID			m_serviceID;
	int					m_nMapID;
	int					m_nFlag;
    DWORD               m_dwGameState;          // 角色游戏状态 ACTOR_GAME_STATE

#ifdef VERSEION_DEBUG
    bool                m_bReleased;
#endif

	// 当前控制状态
	BYTE				m_byCurControlState;
	// 当前状态
	CEntityState*		m_pCurrentState;
	// 站立状态
	CEntityStandState	m_EntityStandState;
	// 死亡状态
	CEntityDieState		m_EntityDieState;
	// 战斗死亡状态
	CEntityFightState	m_EntityFightState;
    char                m_szName[ACTORNAME_MAXSIZE];
	bool				m_bInit;			// 实体是否已初始化


    CMobaGoodsEffect*   m_pMobaGoodsEffect;
    // 附属实体管理类（分身、宠物...）
    CBelongEntityMgr    m_belongEntityMgr;
    // 实体用途管理类
    CEntityUseFlagMgr   m_UseFlagMgr;
};
