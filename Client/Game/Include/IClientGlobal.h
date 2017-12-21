/*******************************************************************
** 文件名:	IClientGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/25/2014
** 版  本:	1.0
** 描  述:	
			
********************************************************************/

#pragma once




namespace rkt
{
	struct IConnection;
}

struct IMessageDispatch;
class Stage_Manager;
struct IEntityFactory;
class TimeSyncClient;
struct IClientEntity;
struct rkt::IEventEngine;
struct rkt::ISchemeEngine;
struct IEntityClient;
struct ISchemeCenter;

namespace GameLogic{
	struct IRenderViewProxy;
}
using namespace GameLogic;
namespace rkt{ struct IConditionFactory; }
struct IEffectFactory;
struct ISpellFactory;
struct IFreezeClient;
struct IBuffClient;
struct ITankClient;
struct IWarClient;
struct SCLPlayerInfo;
struct IPathFindManager;
struct IHandleCommandManager;
struct IChatClient;
struct IClanClient;
struct IKinClient;
struct IMatchClient;
struct IAntiAddictClient;
struct ITaskClient;
struct ILegendCupClient;
struct IMentorshipClient;
struct ISNSClient;
struct ITeamClient;
struct IMailClient;
struct IKeyWordClient;
struct IMiscManager;
struct IScreenCaptureClient;
class  HeightFieldMgr;

#include "IClientConnection.h"

/**
	全局类，通过该类访问各种全局资源
*/
struct IClientGlobal
{
	/// 获得事件中心接口
	virtual IEventEngine *      getEventEngine() = 0;

	/// 获取网络连接
	virtual IClientConnection*  getNetConnection() = 0;

	/// 获取队列里未处理的数据包个数
	virtual unsigned long       getPendingCount(bool bIsSend=false) = 0;

    /// 获取网络延迟
    virtual unsigned long       getNetLatency() = 0;

	/// 获取消息派发器
	virtual IMessageDispatch *  getMessageDispatch() const = 0;

	/// 获取游戏阶段状态管理器
	virtual Stage_Manager *     getStageManager() const = 0;

	/// 获取实体工厂
	virtual IEntityFactory *    getEntityFactory() const = 0;

	/// 取得主角对象
	virtual IClientEntity *     getHero() const  =0;

	virtual void                setHero(IClientEntity * pHero) = 0;

    virtual bool                gotoState( BYTE nState,void * context=NULL,int len=0 ) = 0;

    virtual BYTE                getCurState() = 0;

	/// 取得时间同步器
	virtual TimeSyncClient *    getTimeSyncer() =0;

	/// 取得定时器时间轴
	virtual TimerAxis *         getTimerAxis() = 0;

	/// 取得配置服务
	virtual ISchemeEngine *     getSchemeEngine() = 0;
	
	/// 取得客户端实体模块
	virtual IEntityClient *     getEntityClient() = 0;

	/// 取得显示层接口(U3D)
	virtual IRenderViewProxy *  getRenderView() = 0;

	/// 取得游戏脚本配置中心
	virtual ISchemeCenter*		getSchemeCenter() = 0;

	// 取得条件工厂
	virtual IConditionFactory * getConditionFactory() = 0;

	// 取得效果工厂
	virtual IEffectFactory *    getEffectFactory() = 0;

	// 取得技能工厂
	virtual ISpellFactory *     getSpellFactory() = 0;

	/// 取得冷却客户端
	virtual IFreezeClient*      getFreezeClient() = 0;

	/// 取得Buff客户端
	virtual IBuffClient*        getBuffClient() = 0;

	/// 取得tank客户端
	virtual ITankClient*        getTankClient() = 0;

	/// 取得War客户端
	virtual IWarClient*         getWarClient() = 0;

	// 取得寻路管理接口
	virtual IPathFindManager*   getPathFindManager() = 0;

	// 取得操作命令管理接口
	virtual IHandleCommandManager* getHandleCommandManager() = 0;

	// 获得聊天客户端
	virtual IChatClient*		getChatClient() = 0;

    // 获得帮会客户端
    virtual IClanClient*		getClanClient() = 0;

    // 获得战队客户端
    virtual IKinClient*		    getKinClient() = 0;

    // 获得MatchClient
    virtual IMatchClient*		getMatchClient() = 0;

	// 获得防沉迷客户端
	virtual IAntiAddictClient*	getAntiAddictClient() = 0;

    // 获得任务客户端
    virtual ITaskClient*	    getTaskClient() = 0;

    virtual ILegendCupClient*   getLegendCupClient() = 0;

	// 获得师徒客户端
	virtual IMentorshipClient*	getMentorshipClient() = 0;

	// 获得LOG文件路径
	virtual string				getLogFileName() = 0;

	// 获得SNS客户端
	virtual ISNSClient*	        getSNSClient() = 0;

	// 取得组队系统客户端
	virtual ITeamClient*        getTeamClient() = 0;

	// 获取Mail客户端
	virtual IMailClient*		getMailClient() = 0;

	// 获取关键字客户端
	virtual IKeyWordClient*		getKeyWordClient() = 0;

    // 获得杂项管理对象
    virtual IMiscManager*       getMiscManager() = 0;

	// 获取录屏客户端
	virtual IScreenCaptureClient* getScreenCaptureClient() = 0;

    virtual HeightFieldMgr* getHeightFieldMgr() = 0;
};

#define gClientGlobal	              ((IClientGlobal*)::rkt::getGlobal())


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(GAME_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(Game))
extern "C" IClientGlobal * CreateClientGlobal(void);
#	define	CreateClientGlobalProc	CreateClientGlobal
#elif !defined(GAME_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(Game))
API_IMPORT IClientGlobal * CreateClientGlobal(void);
#	define	CreateClientGlobalProc	CreateClientGlobal
#else													// 动态加载
typedef IClientGlobal * (RKT_CDECL *procCreateClientGlobal)(void);
#	define	CreateClientGlobalProc	DllApi<procCreateClientGlobal>::load(MAKE_DLL_NAME(Game), "CreateClientGlobal")
#endif