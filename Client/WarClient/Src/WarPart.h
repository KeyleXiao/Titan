/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\WarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场部件
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IWarClient.h"
#include "IClientEntity.h"
#include "DSkillCommon.h"
#include "SpellDef.h"
#include "WarDef.h"
#include "MobaGoodsDef.h"
#include "WarMessageDef.h"
#include "WarStaticMessageDef.h"
#include "EntityViewDef.h"
#include <map>
#include <vector>
#include "TimerHandler.h"



using namespace std;
using namespace stdext;
using namespace SPELL;

class CWarCheckBadBehavior;

// 冷却的网络延时补偿
#define FREEZE_NETDELAY_REDEEM			200
// 插旗在多少时间内的一个循环 单位ms
#define POS_TIME_INTERVAL               3000
// 规定时间内超过5次就暂时屏蔽不让发送
#define POS_NUM_MAX                     5
// 被进制发送信号之后的恢复时间
#define POS_RECOVER_TIME                10000
// 技能类型快捷键
#define ShortCutSlotType_Skills         2
// 冷却部件
class CWarPart : public IWarPart,public IMessageHandler,public TimerHandler, public rkt::IEventExecuteSink
{
    enum
    {
        ETIMER_TABISOPEN = 0,	// buff生命周期
    };
public:
    //////////////////////////////////////////////////////////////////////////
    /*ITimerHandler*/
    /**
    @purpose          : 定时器触发后回调,你可以在这里编写处理代码
    @param	 dwTimerID: 定时器ID,用于区分是哪个定时器
    @return		      : empty
    */
    virtual void			OnTimer( unsigned long dwTimerID );

    ///////////////////ISerializable////////////////////////////////////
    /** 序列化支持接口
    @return  
    */
    virtual bool			onSerialize(UID uid, rkt::ibuffer & in,int flag );

    /** 反序列化支持接口
    @return  
    */
    virtual bool			deSerialize( rkt::obuf & out,int flag );

    ///////////////////IEntityPart////////////////////////////////////
    /** 取是部件ID
    @param   
    @param   
    @return  
    */
    virtual short			getPartID(void);

    /** 创建，重新启用，也会调此接口
    @param   
    @param   
    @return  
    */
    virtual bool			onLoad( IEntity* pEntity);

    /** 释放,会释放内存
    @param   
    @param   
    @return  
    */
    virtual void			release(void);

    virtual void            restore();

    ////////////////////////////////////////IMessageHandler////////////////////////////////////////////////////////////
    // 模块消息处理
    virtual void			onMessage( SNetMsgHead* head, void* data, size_t len);

    // 消息通讯 部件消息处理
    virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    /** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
    @param   
    @param   
    @return  
    */
    virtual bool			onWarMessageID( int msgId,void * data,size_t len );

    /** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
    @param   
    @param   
    @return  
    */
    virtual bool			onWarStaticMessageID( int msgId,void * data,size_t len );

    virtual bool			onCommand( int cmdid,void * data,size_t len );

    /** 取是主人
    @param   
    @param   
    @return  
    */
    virtual IEntity*		getHost();

    // 获得战场id，判断场内场外
    virtual int GetWarID();

	// 获取召唤师技能
	virtual int GetSummerID() { return m_nSummerID; }

    ///////////////////CWarPart////////////////////////////////////
    /** 
    @param   
    @param   
    @return  
    */
    CWarPart(void);
    //	virtual void OnTimer(unsigned long dwTimerID);
    /** 
    @param   
    @param   
    @return  
    */
    virtual ~CWarPart(void);
private:
    void OnMessageMiniMapIcon(void *pData, size_t nLen);

    void OnMessageClearAllMiniMapIcon(void *pData, size_t nLen);

    void OnMessageUpdatePersonInfo(void *pData, size_t nLen);

    void OnMessageInitPersonInfo(void *pData, size_t nLen);

    void OnMessageBackGroundMusic(void *pData, size_t nLen);

	void OnMessageUpdateTab(void *pData, size_t nLen);
    
    // 服务器消息移除buff
    void OnMessageUpdateMonsterTime(void *pData, size_t nLen);

    void OnMessageBroadImprotantEntityDie(void *pData, size_t nLen);

    void OnMessageBroadImprotantRelive(void *pData, size_t nLen);

    void OnMessageBroadImprotantMonsterRelive(void *pData, size_t nLen,bool nIsEarlyRelive=false);

    void OnMessageBroadMonsterDieText(void *pData, size_t nLen);

    void OnMessageEntityFightMinMapInfo(void *pData, size_t nLen);

    void OnMessageBroadKillCn(void *pData, size_t nLen);


    void RequestPersenInfoComplete();

    void UpdateWarMonsterInfo(cmd_war_monsterinfo *PersonData);

    void BroadImprotantEt(cmd_war_important_entiydieinfo *EntityInfo, const vector<cmd_war_assist_info>& assistList);

    void BroadKillCn(cmd_war_entity_killcountinfo *KillInfo, const vector<cmd_war_assist_info>& assistList);

    void OnMessageWarEndhandle(void *pData, size_t nLen);

    void OnMessageMiniMapInfo(void *pData, size_t nLen);

    void OnMessageSkillState( void* pData, size_t nLen );

    void OnMessageShowKillIcon( void* pData, size_t nLen );

    void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    void OnMessageAllPlayerEntityCreated(void *pData, size_t nLen);

	void OnMessageProcessBroadcast(void *pData, size_t nLen);

	void OnMessageSendWarStaticsTotal(void *pData, size_t nLen);

	void OnMessageSendWarStaticsWar(void *pData, size_t nLen);

	void OnMessageSendHistoryRecord(void *pData, size_t nLen);

	void OnMessageOtherCrownPageInfo(void* pData, int nLen);

	void OnMessageXPRandomPlayerInCool(void *pData, size_t nLen);

	void onMessageActiveTalentList(void* data, size_t len);

	void OnMessageNotifyWarInfo(void *pData, size_t nLen);

    void OnMessageWarEndLegendCupInfo(void *pData, size_t nLen);



protected:
    // 发送战场玩家基本数据
    void SendWarPersonInfo();

    // 限定抛旗次数
    bool limitPosFlagNum();


protected:
    // 填充战场玩家基本信息到逻辑层容器中
    bool fillCmdWarpersoninfo(PDBID idActor, msg_war_person_info *pPersonFirst );

    // 判断玩家的基本数据是否有变化
    bool CheckBaseInfoIsChange( msg_war_person_info *pPersonFirst );

    // 向显示层发送槽位快捷键信息
    void sendSlotInf();


    // 加载战后战绩表所有玩家数据
    void loadEndRecordData(msg_war_entity_end_warinfo* pInfo, int nPlayerCount , obuf& obData );

    // 强天赋技能加载到内存中
    void loadTalentSKill(PDBID pDbid, int nTalentSkill1, int nTalentSkill2);
	// 特殊实体搜索状态改变
	void OnMessageEntitySearchStateChange( void * data, size_t len );
	void onMessageSpecialContribution( void * data, size_t len );
	// 点赞部分人物信息
	void onMessageAllFilterSupportPerson( void * data, size_t len );
	// 添加标签
	void onMessageAddLabel(void* data, size_t len);

private:
	// 战场结束处理
	void onWarEnd();
private:
    // 所属实体
    IEntity *				m_pMaster;

    map<PDBID,cmd_war_personinfo>	m_mapPersonWarRecord;


    std::hash_map<msg_player_skill_state, INT>  m_mapAllCampUIDCollTIme;

    // 同阵营玩家技能大招技能cd时间
    std::hash_map<UID, int>	m_mapSkillCoolTime;

    // tab战绩表是否开启
    bool m_TabIsOpen;

    // tab战绩表是否是以第一申请角色数据
    bool m_IsOnceRequest;

    // 战场是否结束
    bool m_WarIsEnd;

    int m_WarID;

    // 战绩抛出cd时间
    DWORD   m_PosTime;
    int     m_PosNum;
    bool    m_CanPosSignal;

    // 玩家基本数据是否有变化
    bool    m_bBaseInfoIsChange;

	// 玩家挂机次数
	int		m_nBottingCount;

	// 召唤师技能
	int		m_nSummerID;

	// 是否开启恶意行为检测
	bool	m_bCheckBadBehavior;

	CWarCheckBadBehavior* m_pCheckBadBehavior;
};
