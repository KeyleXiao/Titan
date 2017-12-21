/*******************************************************************
** 文件名:	MonsterProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/23/2014
** 版  本:	1.0
** 描  述:	

			怪物、NPC属性集合
********************************************************************/

#pragma once

#include "EntityProperty.h"
#include "Vector3.h"

#pragma pack(1)


// 怪物属性集部件
class EntityPart_MonsterProperty : public EntityProperty<MonsterProperty>/*, public TimerHandler*/
{
    // 定时器定义   
    enum EPROPERTY_TIMER
    {
        SELF_CHECK_TIMER,               // 自检定时器
    };

    // 定时时间间隔定义
    enum EPROPERTY_TIMER_INTVAL
    {
        SELF_CHECK_INTVAL   = 1000,      // SELF_CHECK_TIMER  自检定时器间间隔
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
        MonsterProperty::serialize_init( s_PropertyDeclare,_countof(s_PropertyDeclare) );
	}

    virtual bool onLoad(__IEntity* pEntity, int nLoginMode);
    virtual void release();

	EntityPart_MonsterProperty();
	~EntityPart_MonsterProperty();

	// 加载成功时回调
	virtual void onEntityLoadFinish();
	// 属性改变时的一些联动计算
	virtual void OnPropertyChange(int prop_id, void * data,int32 len);

	// 属性改变，设置数据，通用公式【(基础值 + 附加值之和) * (1 + 附加百分比值之和)】
	void OnPropertyChange_SetBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID);
	// 属性改变，设置附加数据，通用公式【附加值之和 * (1 + 附加百分比值之和)】
	void OnPropertyChange_SetAppendData(int nAppendID, int nAppendPCTID, int nChangeID);
	// 计算基础属性
	void OnCalculate_Base(int nBaseID);

	//////////////////////////////TimerHandler/////////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	//virtual void OnTimer( unsigned long dwTimerID );
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	bool setVigourRatio(int* pVigourRatio);
private:
    // 取得属性优先级
    BYTE getPropPriority(int prop_id);

    // 广播属性
    void onPropertyBroadcast(int prop_id);

    // 执行广播属性
    void doBroadcast(EPROPERTY_PRIORITY priority);


private:
    // 普通属性ID列表
    std::set<int>   m_arrayNormalPropID;
    // 高精度属性ID列表
    std::set<int>   m_arrayHighPropID;

	int             m_VigourRatio[VPT_ADD_MAX];
};

#pragma pack()