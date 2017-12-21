/*******************************************************************
** 文件名:	IMovePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/9/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

/**
	移动部件: 
	1.把实体的移动信息定时发送给服务器
	2.接收服务器发过来的其他实体的移动信息,在本客户端上模拟显示
*/

#include "IEntity.h"

struct IMovePart : public IEntityPart
{
    // 被同步到指定位置
    virtual void onSyncPostion( DWORD dwTickcount, const Move3 & move ) = 0;

    // 被强制冲到指定位置
    virtual void onForceMove( const cmd_force_move & move ) = 0;

	// 向目标移动
	virtual void onMovePos( const cmd_creature_move_pos &movePos ) = 0;

    // 获取实体当前移动三元组
    virtual Move3 getMoveContext() = 0;

    // 切换操作模式(假人，影响假人后退攻击时朝向)  
    virtual void setOperateMode(int nMode, Vector3& vTarget) = 0;

    virtual void setRobotMode(bool bSet) = 0;
};

extern "C" IMovePart * createMovePart();