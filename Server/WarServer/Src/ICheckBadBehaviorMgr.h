/*******************************************************************
** 文件名: ICheckBadBehaviorMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/11
** 版  本: 1.0
** 描  述: 战场检测恶劣行为接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class ICheckBadBehaviorMgr : public __IWarMgr
{
public:
	// 客户端消息
	virtual void onMessage(BYTE byKeyAction, UID uid, void* data, size_t len) = 0;

	// 战场结束发送挂机记录
	virtual void warEndBottingRec() = 0;
};