/*******************************************************************
** 文件名:	IEquipSchemeMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/18
** 版  本:	1.0
** 描  述:	
			装备管理接口
********************************************************************/

#pragma once

class IEquipSchemeMgr 
{
public:
	// 初始化
	virtual bool init(__IEntity* pMaster, LONGLONG WarDBKey, int nMatchTypeID) = 0;
	
	// 释放
	virtual void release() = 0;

	// 序列化
	virtual bool onSerialize(rkt::ibuffer & in, int flag) = 0;

	// 反序列化
	virtual bool deSerialize(rkt::obuf & out, int flag) = 0;

	// 添加装备
	virtual void addEquip(int nEquipID) = 0;

	// 撤销装备
	virtual void revocationEquip() = 0;

	// 战场结束
	virtual void onWarEnd() = 0;

	// 客户端消息
	virtual void onMessage(int msgId, void * data, size_t len) = 0;

	// 是否开启了常用装备保存
	virtual bool isSaveOftenUseEquip() = 0;
};