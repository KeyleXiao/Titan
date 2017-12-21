/*******************************************************************
** 文件名:	MobaGoodsPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IMobaGoodsPart.h"
#include "IEntityAOI.h"
#include "MobaGoodsDef.h"
#include "IFreezeServer.h"
#include "SpellDef_ManagedDef.h"
#include <stack>
#include "IDBEngineService.h"
#include "IGameDatabaseDef.h"
#include "ChatDef.h"
#include "IEquipSchemeMgr.h"


// 合成物品槽位锁定记录
typedef map<int,bool> T_MAP_COMPOSE_SLOT_CUR_RECORD;

// 属性管理器结构
typedef map<emMobaGoodEffectType, IMobaGoodsProp*> T_MAP_PROP_MANAGER;

// 撤销操作管理器
typedef stack<SRevocationInfo>	T_STACK_REVOCATION_MANAGER;

// 战场中保存购买装备的log 1000毫秒
#define WarBuyEquipLog  180000
enum
{
    ETimerEventID_SaveEquipLog,         // 保存日志定时器
};

// 实体状态部件
class MobaGoodsPart : public __IMobaGoodsPart, public IEventVoteSink, IEventExecuteSink,public IDBRetSink,public TimerHandler
{

public:
	////////////////////* IEntityPart */////////////////////////////////////////////
	/** 取得部件ID
	@return  
	*/
	virtual short			getPartID();
	virtual bool			onLoad( __IEntity* pEntity, int nLoginMode);
	virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	virtual __IEntity *		getHost();
	virtual void			release();
	
	virtual bool			onSerialize( rkt::ibuffer & in,int flag );
	virtual bool			deSerialize( rkt::obuf & out,int flag );

    // 切换英雄
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

    virtual void            restore();

	//使用物品
	virtual bool			CanUseSkill(SSlotSkillInfo *SlotSkillInfo);

	//使用物品技能
	virtual bool			UseSkill(SSlotSkillInfo *SlotSkillInfo);

	////////////////////* IEventVoteSink */////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	////////////////////* Part相关处理函数 */////////////////////////////////////////////
	// 添加属性到属性管理器
	bool					AddEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType);

	// 移除属性管理器属性
	bool					RemoveEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType);

    // 添加分身怪物继承属性
    bool					AddMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType);

    // 移除分身怪物继承属性
    bool					RemoveMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType);

    // 添加新创建分身怪物继承属性
    bool                    AddCreateMonsterInheritEffect(UID uid, int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType);

    // 添加继承怪物
    void                    AddInheritMonster(UID uid);

    // 移除继承怪物
    void                    RemoveInheritMonster(UID uid);

	//获取是否有相同ID装备
	virtual bool			HaveOtherSameGoods(int nGoodID, int nSlotIndex);

	//重算未启用的效果
	virtual void			RecomputeAllGoodsEffect();

	// 设置能否购买或者出售物品
	virtual void			SetCanBuyOrSell(bool isCanBuyOrSell);

    // 外部接口 添加物品
    virtual void            effectAddGood(int nGoodID, int dwCount);

    // 外部接口 移除物品
    virtual void            effectRemoveGood(int nGoodID,int dwCount);

    // 获取记录的多重冷却效果数据
    virtual SEffectCDOverlay getCDOverlayInfo(int nFreezeID);

    // 获取记录的多重冷却效果数据
    virtual void             setCDOverlayInfo(int nFreezeID, SEffectCDOverlay data);

    // 移除多重冷却效果数据
    virtual void             removeCDOverlayInfo(int nFreezeID);

    // 外部调用接口 原价出售物品
    virtual void            sellGoodsOriginalPrice();

    // lua外部接口 购买物品
    virtual bool            luaBuyGoods(int nGoodID, int nCost);

    // lua外部接口 出售物品
    virtual void            luaSellGoods(int nSlotIndex, int nGoodID);

    /////////////////////////////TimerHandler/////////////////////////////////////////////
    virtual void            OnTimer( unsigned long dwTimerID );

	//////////////////////////////////////////////////////////////////////////////////////

    virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen){};

private:
	// 能否进行撤销操作
	void					SetCanCancelOperation(bool isCanCancelOperation);

	bool					ComposeGoodCheck(int nGoodID, int &nCost, T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord);
	// 购买装备检查
	EWarCheckBuyGoodErrorCode	CanBuyGood(int nGoodID, emMobaGoodType &nGoodType, int &nCost,emGoodAutoUseType &eAutoUseType, bool &bIsSpecialSlotGood, 
                                        T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord);

	bool					BuyGood(int nGoodID, emMobaGoodType nGoodType, int nCost,emGoodAutoUseType eAutoUseType, bool bIsSpecialSlotGood,T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord, bool bSpecialEffectAdd = false);

	// 能否使用物品
	bool					CanUseGood(int nSlotIndex);

	// 使用物品
	bool					OnClientUseGood(int nSoltIndex);

	// 购买物品
	bool					OnClientBuyGood(int nGoodID, int nGoodsCost = -1);

	// 出售物品
	bool					OnClientSellGood(int nSlotIndex, int nGoodID);

	// 交换物品
	bool					OnClientExChangeGood(int nSrcSlot, int nTargetSolt);

	// 客户端撤销操作
	bool					OnClientRevocationOperation(UID uUID);

	// 撤销购买物品
	bool					RevocationBuyOperation(SRevocationInfo &RevocationInfo);

	// 撤销出售物品
	bool					RevocationSellOperation(SRevocationInfo &RevocationInfo);

    // 撤销交换物品
    bool					RevocationExchangeOperation(SRevocationInfo &RevocationInfo);


	void					SendGoodsListToSelf(BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	void					SendEntityMessage(UID uid, BYTE byKeyAction, BYTE byPartID, LPCSTR pszData, int nDataLen);

	// 广播给有视野的玩家信息
	virtual bool			broadCastDataToHaveSightPlayer(Vector3 vec,BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	// 下发自己所有装备列表
	virtual bool			BroadCastGoodsList();

	// 检测物品是否可以在所在战场购买
	bool                    CheckGoodsIsAtThisWar( SSchemeMobaGoods* pSchemeMobaGoods );

	virtual void	        ResetCanUseCount() ;
	virtual IMobaGoods*     getMobaGoods(SLOT_TYPE  nSlotType,int nSlotIndex);


    // 单独物品下发
    virtual void            UpdateGoodSlotInfo(SLOT_TYPE  nSlotType,int nSlotIndex);

    // 购买物品的日志批处理
    bool batchSaveBuyEquipLog();

    // 填充购买物品的日志
    void fillBuyEquipLog( int nBuyType, int nGoodsId);

    // 设置战场相关信息
    void setWarInfo();

    IMobaGoods*             createMobaGoods(BYTE nGoodType);

    // 将卖装备的钱添加的容器
    //void setAddTalentToMap(int nAdd);

    // 发送物品列表到战场
    void SendGoodListToWar();

	// 记录XP使用
	void RecordUseXp(int nSkillEffectType);


public:
	MobaGoodsPart();
	~MobaGoodsPart();

private:
	bool Parse(string linkContent);

	// 显示飘字
	void showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam = NULL);

private:
	__IEntity *	    m_pMaster;

    int             m_nLoginMode;

	// 槽位对应的物品或者装备
	IMobaGoods*		m_MobaSlotGoods[MAX_GOODS_SLOT_COUNT];

	// 属性状态管集合理器
	T_MAP_PROP_MANAGER		m_MobaGoodsPropManager;

	// 撤销操作管理器
	T_STACK_REVOCATION_MANAGER m_MobaGoodsRevocationManager;

	// 是否能出售购买
	bool		m_bCanBuyOrSell;

	// 是否能撤销
	bool		m_bCanCancelOperation;

    LONGLONG    m_WarDBKey;
	int			m_nMatchTypeID;

    vector<DBREQ_PARAM_WAR_BUYEQUIP_DETAIL> m_vecBugEquipLog;   // 购买物品的log

	// 常用装备管理
	IEquipSchemeMgr* m_pEquipSchemeMgr;

    map<UID, T_MAP_PROP_MANAGER *> m_mapReplicationMonster;

    // 充能效果附加数据
    map<int, SEffectCDOverlay>  m_mapCDOverLayData;
};