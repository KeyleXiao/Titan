/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\MobaGoodsPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场物品部件
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
#include "MobaGoodsDef.h"
#include "WarDef.h"
#include "WarMessageDef.h"
#include "EntityViewDef.h"
#include "EquipSchemeMgr.h"
#include <map>
#include <vector>

using namespace std;
using namespace stdext;
using namespace SPELL;


// 冷却的网络延时补偿
#define FREEZE_NETDELAY_REDEEM			200

// 冷却部件
class CMobaGoodsPart : public IMobaGoodsPart, public IEventExecuteSink
{
	enum
	{
		ETimerEventID_CampInfo = 0,	// buff生命周期
	};
public:
	///////////////////ISerializable////////////////////////////////////
	/** 序列化支持接口
	@return  
	*/
	virtual bool			onSerialize( UID uid, rkt::ibuffer & in,int flag );

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
    
    ////////////////////////////////////////IEventExecuteSink////////////////////////////////////////////////////////////
    virtual void		    OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	virtual bool			onCommand( int cmdid,void * data,size_t len );

	// 消息通讯
	virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	/** 取是主人
	@param   
	@param   
	@return  
	*/
	virtual IEntity*		getHost();

	///////////////////CMobaGoodsPart////////////////////////////////////

	//槽位使用
	virtual void			GoodsUse(SSlotGoodsInfo SlotGoodsInfo);

	//槽位物品交换位置
	virtual void			ExchangeSlot(int nSrcSlotIndex, int nTargetSlotIndex);

	/** 
	@param   
	@param   
	@return  
	*/
	CMobaGoodsPart(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CMobaGoodsPart(void);

private:
	// 所属实体
	IEntity *				m_pMaster;

	// 阵营成员数据下发状态
	bool					m_bHaveCampInfo;

    // 能否购买物品标示
    bool					m_bCanBuyOrSell;

	// 装备管理
	CEquipSchemeMgr			m_EquipSchemeMgr;
};
