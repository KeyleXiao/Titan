/*******************************************************************
** 文件名: ISupportMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/5
** 版  本: 1.0
** 描  述: 战场子功能接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"
#include "Vector3.h"

class ISupportMgr : public __IWarMgr
{
public:
	// 客户端点赞操作
	virtual void onClientMsgPersonAddLabelRequest(UID uidActor, void* pData, size_t stLen) = 0;

	// 选出可参与点赞玩家
	virtual void filterPlayersForSupportCondition(bool bAwark) = 0;
};