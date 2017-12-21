/*******************************************************************
** 文件名:	IEntityAOI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/29/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntity.h"
#include "AOI_Define.h"
#include "IGameSceneService.h"
#include <map>

/**
	管理游戏中玩家的视野逻辑(Area Of Interest)
	具体规则：
	1.玩家上线，向周围视野内的所有玩家广播自己出现的消息(Appear)
	2.同时向该玩家发送周围所以玩家的信息(Nearest)
	3.玩家下线, 向周围视野内的所有玩家广播自己离开的消息(Disappear)
	4.玩家移动, 计算玩家之前所在位置的九宫格列表，再计算玩家新位置的九宫格列表
	  a).对于其中新增的单元格，广播玩家出现的消息(Appear),并把该单元格中的所有玩家列表发给此玩家(Nearest)
	  b).对于其中失效的单元格，广播玩家离开的消息(Disappear)，同时将该单元格内的所有玩家发给此玩家，告知离开
	5.动作广播: 玩家做出动作或者外观变化，则对周围视野内所有玩家进行广播
	6.属性广播: 角色属性发生变化时，需要如果是属于客户端看得见的属性
	则要下发给客户端，如果属于周围玩家看得见的属性，则要广播给周围角色
	7.视野变化：游戏中某些行为可导致视野变化，比如致盲，视野就会变成缩小或没有
	8.隐身功能：提取角色广播信息时返回false
	9.多视野功能: 比如WOW中的鹰眼,LOL中的插眼.查看队友的视野.(视野对象支持订阅功能)

	优化:
	1.属性广播和动作广播，需要根据距离的远近，调整广播频率
	2.视野变化：周围人数非常多时动态缩小视野以减少广播量
	*/

struct __IEntityAOI : public __IEntityPart
{
	// 取得视野半径
	virtual float getSight() = 0;

	// 调整视野半径
	virtual void setSight( float radii ) = 0;

	// 是否隐身
	virtual bool isHide() = 0;

	// 设置隐身状态
	virtual void setHide( bool state ) = 0;

	// 是否触发事件(因为广播事件非常频繁，每次都打开非常浪费消息,见FLAG_ENTER_VIEWPORT等)
	virtual void setEventFlag( int flag ) = 0;

    // 设置隐形(对其他阵营隐形)
    // bAlpha2SelfCamp 是否对本阵营半透
    virtual void setStealth( bool bSteal, bool bAlpha2SelfCamp, int nEffectID ) = 0;

    // 反隐形(设置该单位为反隐形单位,真眼)
    virtual void antiStealth( bool bEnable ) = 0;

    // 设置（取消）对某阵营一直可见
    virtual void keepInsight( int camp, bool bEnable ) = 0;

    // 设置对有所阵营隐形（包括自己）
    virtual void setGhostState( bool bGhost = true) = 0;

    // 是否在某阵营的视野内
    virtual bool isInsight( int camp ) = 0;

    virtual int broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg, const char * pFileInfo, int nFileLine) = 0;

    // 获取AOI代理
    virtual AOI_PROXY_PTR getAOIProxy() = 0;
};

// 因为广播事件非常频繁，每次都打开非常浪费消息,所以需要时手动打开
enum 
{
	FLAG_ENTER_VIEWPORT = 1,
	FLAG_LEAVE_VIEWPORT = 1<<1,
	FLAG_DO_ACTION = 1<<2,
	FLAG_PROPERTY_CHANGED = 1<<3,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IEntityAOI * CreateEntityAOI();
#	define	CreateEntityAOIProc	CreateEntityAOI
#else													/// 动态库版本
typedef __IEntityAOI * (RKT_CDECL * procCreateEntityAOI)();
#	define	CreateEntityAOIProc	DllApi<procCreateEntityAOI>::load(MAKE_DLL_NAME(ENTITY), "CreateEntityAOI")
#endif

