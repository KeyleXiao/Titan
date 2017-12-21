/*******************************************************************
** 文件名:	PlayerRoleProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家属性集对象
********************************************************************/

#pragma once

#include "Property.h"
#include "EntityProperty.h"
#include "Vector3.h"
#include "SpellDef.h"
using namespace SPELL;

#pragma pack(1)

// 玩家属性集部件
class EntityPart_RoleProperty : public EntityProperty<PlayerRoleProperty>, public TimerHandler
{
    // 定时器定义   
    enum EPROPERTY_TIMER
    {
        AUTO_GROW_TIMER = 0,            // 自动增长定时器
        AUTO_DECAY_TIMER,               // 自动衰减定时器
        HP_MP_REPLY_TIMER,              // 血量魔法回复定时器 
        SELF_CHECK_TIMER,               // 自检定时器
		ENTITY_PROPERTY_TIMER,			// 龙珠实体属性请求定时器
    };

    // 定时时间间隔定义
    enum EPROPERTY_TIMER_INTVAL
    {
        AUTO_DECAY_TIME     = 500,      // AUTO_DECAY_TIMER  自动衰减定时器时间
        HP_MP_REPLY_TIME    = 5000,     // HP_MP_REPLY_TIMER 血量魔法回复定时器时间
        SELF_CHECK_INTVAL   = 500,      // SELF_CHECK_TIMER  自检定时器间间隔
		ENTITY_PROPERTY_INTVAL = 500,	// ENTITY_PROPERTY_INTV 龙珠实体属性定时器时间间隔
    };

    // 属性优先级
    enum EPROPERTY_PRIORITY
    {
        PROP_PRIO_NORMAL,
        PROP_PRIO_HIGH,
    };
    
public:
	static void init()
	{
        PlayerRoleProperty::serialize_init( s_PropertyDeclare,_countof(s_PropertyDeclare) );
	}

    virtual bool onLoad(__IEntity* pEntity, int nLoginMode);

    virtual void destroy();

    // 切换英雄
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

	virtual void onEntityLoadFinish();
	virtual void OnPropertyChange(int prop_id, void * data,int32 len);

	//////////////////////////////__IEntityPart/////////////////////////////////////////////

	//////////////////////////////EventExecuteSink/////////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	//////////////////////////////TimerHandler/////////////////////////////////////////////
	// 定时器到了
	virtual void OnTimer( unsigned long dwTimerID );
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    //////////////////////////////EntityPart_RoleProperty//////////////////////////////////
	// 构造函数
	EntityPart_RoleProperty();
	// 析构函数
	~EntityPart_RoleProperty();

	void OnPropertyChange_Exp_After(void * data,int32 len);
	// 属性改变，设置数据，通用公式【(基础值 + 附加值之和) * (1 + 附加百分比值之和)】
	void OnPropertyChange_SetBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID);
	// 属性改变，设置附加数据，通用公式【附加值之和 * (1 + 附加百分比值之和)】
	void OnPropertyChange_SetAppendData(int nAppendID, int nAppendPCTID, int nChangeID);
	// 计算基础属性
	void OnCalculate_Base(int nBaseID);
    // 属性改变,设置附加百分比基础数据
    void OnPropertyChange_SetAppendPCTBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID);
    // 属性改变,设置百分比数据
    void OnPropertyChange_SetPCTData(int nAppendPCTID, int nChangeID);
private:
	// 魔法类型初始化
	void onMagicInit(void);
	// 魔法类型释放
	void onMagicRelease(void);
	// 增加魔法
	void addMagic(int nValue);

	// 血量魔法回复初始化
	void onHpMpReplyInit(void);
	// 血量魔法回复释放
	void onHpMpReplyRelease(void);
	// 限制属性
	void onSetPropertyLimit(void);
    // 设置英雄基础属性
    void onSetPersonBaseProp(void);


    // 取得属性优先级
    BYTE getPropPriority(int prop_id);
    // 广播属性
    void onPropertyBroadcast(int prop_id);
    // 执行广播属性
    void doBroadcast(EPROPERTY_PRIORITY priority);
    void onBroadcastChangeProperties(std::set<int> * pPropertyArray, bool isBroadcast);

    // 执行属性同步,只同步数值类型属性
    void onSyncShareProperty(int prop_id, int prop_value=0);
    // 设置速度衰减
    void onSetSpeedDecay(int &nValue);

	// F11请求指定实体属性
	void onClientMsgEntityProperty(void* pData, size_t stLen );

	// F11发送给客户端指定实体属性
	void sendClientMsgEntityProperty();

	// F11指定实体属性关闭
	void onClientMsgEntityPropertyClose();

private:
	// 最后衰减时间
	DWORD		m_dwLastDecayTick;
	// 衰减状态
	bool		m_bDecayState;
	// 魔法类型
	SMagicType  m_MagicType;

    // 普通属性ID列表
    std::set<int>   m_arrayNormalPropID;
    // 普通属性上次更新时间
    DWORD           m_dwLastNormUpdateTick;
    // 高精度属性ID列表
    std::set<int>   m_arrayHighPropID;

	// 龙珠请求实体属性UID
	UID nReqUID;
};

#pragma pack()