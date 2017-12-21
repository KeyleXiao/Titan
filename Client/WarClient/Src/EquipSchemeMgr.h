/*******************************************************************
** 文件名: EquipSchemeMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/18
** 版  本: 1.0
** 描  述: 装备方案管理
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "StdAfx.h"
#include "IEntity.h"

class CEquipSchemeMgr
{
public:
	CEquipSchemeMgr();
	~CEquipSchemeMgr();

public:
	// 初始化
	bool init(IEntity* pMaster);

	// 重置
	void restore();

	// 序列化
	bool onSerialize(rkt::ibuffer & in, int flag);

	// 反序列化
	bool deSerialize(rkt::obuf & out, int flag);

	// 接收服务器消息
	bool onMessage(int msgId, void * data, size_t len);

	// 接收客户端命令
	bool onCommand(int cmdid, void * data, size_t len);

private:
	// 打印装备方案,For Debug
	void printEquipSchemeInfo(const SWarEquipScheme& scheme);

private:
	IEntity *	    m_pMaster;
};