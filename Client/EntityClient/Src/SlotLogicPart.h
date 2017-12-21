/*******************************************************************
** 文件名:	
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2008/2/28  21:30
** 版  本:	1.0
** 描  述:	槽位部件
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ISlotLogicPart.h"
#include "IEntityClient.h"
#include "DSkillCommon.h"
#include "EventDef.h"
#include "ISchemeCenter.h"
#include "FreezeDef.h"
#include "ISpell.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "SlotMessageDef.h"
#include "EffectDef.h"
#include "IMessageDispatch.h"

#include <map>
#include <vector>
using namespace std;
using namespace stdext;
using namespace EFFECT_CLIENT;
// 目前好像没办法包含SchemeSlotMapedShortcut.h ，所以在此再定义
enum  ShortCutSlotType
{
    ShortCutSlotType_Goods=1,
    ShortCutSlotType_Skills,
    ShortCutSlotType_Function,
};
// 冷却部件
class CSlotLogicPart : public ISlotLogicPart, public IEventExecuteSink, public IMessageHandler 
{

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
	@return  
	*/
	virtual short			getPartID(void);

	/** 创建，重新启用，也会调此接口
	@return  
	*/
	virtual bool			onLoad( IEntity* pEntity);

    virtual void            restore();

	/** 释放,会释放内存
	@return  
	*/
	virtual void			release(void);

	/** 取是主人
	@param   
	@param   
	@return  
	*/
	virtual IEntity*		getHost();

	/** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件 
	@return  
	*/
	virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual bool			onCommand( int cmdid,void * data,size_t len );

	///////////////////IEventExecuteSink////////////////////////////////////
	// 订阅冷却改变事件，然后同步到显示层
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////////////////IMessageHandler////////////////////////////////////////////////////////////
    virtual void			onMessage( SNetMsgHead* head, void* data, size_t len);

	///////////////////ISlotLogicPart////////////////////////////////////
	/** 更新
	@return  
	*/
	virtual void			UpdateSlot(cmd_creature_set_slot *SlotData);

	// 获得相应槽位关技能ID
	virtual int				GetSlotSkillID(SLOT_TYPE SlotType, int nIndex);

	// 获得技能ID相应槽位相关信息
	virtual bool			GetSkillIDInfo(int SkillID, SLOT_TYPE &SlotType, int &nIndex);

    // 进入别人视野发送装备列表
    virtual void			EnterViewSendGoodsRecord();

	///////////////////CSlotLogicPart////////////////////////////////////
	/**   
	@return  
	*/
	CSlotLogicPart(void);

	/**  
	@return  
	*/
	virtual ~CSlotLogicPart(void);

private:
	/** 检查
	@param      
	@return  
	*/
	virtual bool			onCmdSlotCheck(cmd_creature_slotview *SlotData);

	void					UpdateFreezeInfo(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	void					UpdateSlotInfo(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	void					UpdateSkillSlot(msg_slot_update_skill_slot * pSkillSlot);

	void					UpdateGoodsSlot(void * data,size_t len);

	void					SetGoodsFreezeInfo(void * data,size_t len);

    void                    UpdateViewSlotInfo();

	/** 更新 
	@return  
	*/
	void					InitialSlots();

private:

	// 实体创建完成
	bool					m_bHaveCreated;
	// 所属实体
	IEntity *				m_pMaster;

	// 技能槽对应的技能信息
	map<int, SSlotSkillInfo> m_SkillSlotMap;
	

	// 物品槽对应的信息
	map<int, SSlotGoodsInfo> m_GoodsSlotMap;

    obuf                m_enterViewGoodsRecord;
	map<PDBID, obuf>	m_mapAllPlayerGoodsRecord;
};
