/*******************************************************************
** 文件名:	SlotPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "ISlotPart.h"
#include "IMobaGoodsPart.h"
#include "SlotDef.h"

// 实体状态部件
class SlotPart : public __ISlotPart, public IEventExecuteSink
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
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void            restore();

	// 设置物品槽位
	virtual void			SetGoodsSlotList(LPCSTR pszData, int nDataLen);

	void					SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	// 获得相应槽位关技能ID
	virtual int				GetSlotSkillID(SLOT_TYPE SlotType, int nIndex);


	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void SlotPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	////////////////////* TestPart */////////////////////////////////////////////
public:
	SlotPart();
	~SlotPart();
private:
	bool Parse(string linkContent);

    void updateSpellSlot2Client(UID uid, int nCamp);
private:
	__IEntity *	    m_pMaster;
	bool			m_bCreateEntity;
	// 技能槽对应的技能信息
	map<int, SSlotSkillInfo> m_SkillSlotMap;

	// 槽位对应的物品或者装备
	map<int, SSlotGoodsInfo> m_GoodsSlotMap;

};