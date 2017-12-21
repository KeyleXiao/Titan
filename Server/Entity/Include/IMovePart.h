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
#include "MovePath.h"

struct __IMovePart : public __IEntityPart
{
	virtual bool  setSpeed(float speed) = 0;

    // 移动
	virtual bool  moveTo(Vector3 &loc, bool bInitiatively = false) = 0;

	// 移动
	virtual bool move(Vector3* pPointList, int nPointCount, bool bInitiatively = false) = 0;

    // 跳跃
    virtual bool jumpTo(Vector3& loc, bool bAutoCheckGround, float fGroundY) = 0;

    // 旋转方向
    virtual void rotateTo(Vector3 dirNew) = 0; 

    virtual bool  stopMoving(bool bInitiatively = false) = 0;

    // nSceneID = 0 表示本地图传送
    virtual bool  stopAt(Move3& move, int nSceneID = 0, int nReason = TRANSPORT_REASON_FLASH) = 0;
    
    virtual bool changeMoveState(Move3& move, bool bNotifySelf = false, bool bVerify = false) = 0;

	virtual bool  isMoving() = 0;

    // 是否正在转向
    virtual bool isRotating() = 0;

    virtual float getSpeed() = 0;

    virtual Vector3 getAngle() = 0;

    virtual Move3   getMoveState() = 0;

    virtual Vector3 getMovingTarget() = 0;

    // 设置僵直  dwEndTick:僵直结束时间
    virtual void setRigidity(DWORD dwEndTick) = 0; 

    // 丢失位置(动作决定位移，之后由客户端会重新上报位置)
    virtual void lostLocation() = 0;

    virtual bool isLostLocation() = 0;

    // 冲锋   
    virtual bool shift(void *pData, int nDataLen) = 0;

	// 检测两点之间是否可以通过
	virtual bool canPass(Vector3 locFrom, Vector3 locTo, float fAgentHeight) = 0;

    // 设置是否可以通过结界(例如龙母大招驾驭巨龙时可以通过结界)
    virtual void setCanPassBarrier(bool bCanPass) = 0;

    // 当前的移动状态
    virtual Behaviour* getCurrentBehaviour() = 0;

    virtual WORD& getSendPkgSeq() = 0;

    virtual WORD& getSendKeyPkgSeq() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IMovePart * CreateMovePart();
#	define	CreateMovePartProc		CreateMovePart
#else													/// 动态库版本
typedef __IMovePart * (RKT_CDECL * procCreateMovePart)();
#	define	CreateMovePartProc	DllApi<procCreateMovePart>::load(MAKE_DLL_NAME(ENTITY), "CreateMovePart")
#endif
