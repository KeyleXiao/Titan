/*******************************************************************
** 文件名: IShareServer.h 
** 版  权: (C) 深圳冰川网络技术有限公司
** 创建人: 李界华
** 日  期: 2015/8/27
** 版  本: 1.0
** 描  述: 服务器间的数据共享
** 应  用:    
    
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

// 调试输出
#ifdef VERSEION_DEBUG
#define SHARESERVER_DEBUG_OUTPUT
#endif
#ifdef SHARESERVER_DEBUG_OUTPUT
#   define SHARESERVER_TRACELN(x)           TraceLn(x)
#   define SHARESERVER_WARNINGLN(x)         WarningLn(x)
#   define SHARESERVER_ERRORLN(x)           ErrorLn(x)
#   define SHARESERVER_EMPHASISLN(x)        EmphasisLn(x)
#else
#   define SHARESERVER_TRACELN(x)
#   define SHARESERVER_WARNINGLN(x)
#   define SHARESERVER_ERRORLN(x)
#   define SHARESERVER_EMPHASISLN(x)
#endif


#include "IEventEngine.h"
#include "SharePropertyDef.h"
#include "Match_Manageddef.h"
#include "ActorDef.h"
struct ICenterConnectorService;
using namespace Match_ManagerDef;


#define IsSameScene(a, b) (a.dwSceneID == b.dwSceneID)

enum SHARELINE_TYPE
{   
    EShareLineType_Active_Login = 1,                // 正常上线登陆
    EShareLineType_Shoot_Login,                     // 跨进程切换登陆
    EShareLineType_Reconnect_Login,                 // 与中心服重连登陆
    EShareLineType_StartTrek_Login,                 // 跨世界切换登陆

    EShareLineType_Active_Logout,                   // 主动下线登出
    EShareLineType_Offline_Logout,                  // 客户端掉线登出
    EShareLineType_Shoot_Logout,                    // 跨进程切换登出
    EShareLineType_Disconnect_Logout,               // 与中心服断线登出
    EShareLineType_StartTrek_Logout,                // 跨世界切换登出
};

enum SHAREUPDATE_REASON
{
    SHAREUPDATE_REASON_UNKNOWN = 0,                 // 未知
    SHAREUPDATE_REASON_CHANGE_GAMESTATE,            // 游戏状态改变
    SHAREUPDATE_REASON_CHANGE_CLANID,               // 帮会ID改变
    SHAREUPDATE_REASON_CHANGE_KINID,                // 战队ID改变
    SHAREUPDATE_REASON_CHANGE_FIGHT,                // 战力改变
    SHAREUPDATE_REASON_CHANGE_BINDGOLD,             // 绑金改变
    SHAREUPDATE_REASON_CHANGE_GOLD,                 // 金币改变
    SHAREUPDATE_REASON_CHANGE_TICKET,               // 点券改变
    SHAREUPDATE_REASON_BUY_HERO,                    // 购买英雄
    SHAREUPDATE_REASON_CHANGE_FACEID,               // 头像改变
    SHAREUPDATE_REASON_CHANGE_SEX,                  // 性别改变
    SHAREUPDATE_REASON_UPDATE_MATCHRANK,            // 更新rank
    SHAREUPDATE_REASON_CHANGE_TEAMID,               // 组队改变
    SHAREUPDATE_REASON_CHANGE_ACTORNAME,            // 角色名称变化
    SHAREUPDATE_REASON_SET_ACTIVITY,                // 角色活跃度变化
    SHAREUPDATE_REASON_CHANGE_MATCHTYPE,            // 排位类型       
    SHAREUPDATE_REASON_CHANGE_RANKSCORE,            // 排位得分       
    SHAREUPDATE_REASON_CHANGE_RANKGRADE,            // 排位等级
    SHAREUPDATE_REASON_CONTINUE_PLAY,               // 断线重连续玩
    SHAREUPDATE_REASON_CHANGE_LEVEL,                // 玩家静态角色等级改变
    SHAREUPDATE_REASON_CHANGE_KINGRANK_ORDER,       // 王者排位排名改变
    
};

enum SHARE_NUM_PARAM
{
    SHARE_PARAM_PDBID,                              // 角色ID
    SHARE_PARAM_CLIENTID,                           // 网关ID
    SHARE_PARAM_MAX,                                // 最大
};

// 场景服信息
struct SShareServer
{
    DWORD       dwComputerID;                       // 计算机ID
    DWORD       dwServerSerialID;                   // 场景服ID（比如：1号场景服）
    DWORD       dwServerID;                         // 场景服ID（比如：中心服分配）
    LONGLONG    nUsedMemory;                        // 本进程占用内存
    DWORD       dwPersonCount;                      // 本进程人数

    SShareServer()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SPersonInfo
{
    DWORD       dwPlayerID;
    DWORD       dwWorldID;

    SPersonInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};


struct ShareMatchRankInfo
{
	BYTE nMatchType;
	WORD wMatchScore;
	WORD wMatchNum;
	WORD wWinNum;
    WORD wHideScore;

	ShareMatchRankInfo()
	{
        memset(this, 0, sizeof(*this));
	}
};

struct ShareHeroData
{
    WORD    wHeroID;
    BYTE    nHeroStar;
    bool    bIsLifeHero;

    ShareHeroData()
    {
        memset(this, 0, sizeof(*this));
    }
};


// 服务器掉线回调函数
typedef  void (* ServerDisconnectCallbackPtr)(WORD wSrvType, char* pData,  size_t nDataSize);

////////////////////////////////////////////////////////////////////////////
// 共享人物
struct ISharePerson
{
    /** 账号ID
    @return  
    */
    virtual DWORD           GetUDBID(void) = NULL;

    /** 名字
    @return  
    */
    virtual LPCSTR          GetName(void) = NULL;

    /** 取得游戏世界ID
    @return  
    */
    virtual DWORD           GetWorldID(void) = NULL;

    /** 取身份证号（由数据库生成的身份证替代号）
    @return  
    */
    virtual DWORD           GetIdCard(void) = NULL;

    /** 取得数值属性，暂支持以下属性ID：
        PROPERTY_ID,            // ID
        PROPERTY_VOCATION,      // 职业ID
        PROPERTY_SEX,           // 性别
        PROPERTY_SKIN,          // 皮肤
        PROPERTY_LEVEL,         // 等级
    @return  
    */
    virtual int             GetNumProp(DWORD dwPropID) = NULL;

    /** 取得等级
    @return  
    */
    virtual int             GetLevel() = NULL;

    /** 取得职业
    @return  
    */
    virtual int             GetVocation() = NULL;

    /** 取得当前控制状态
    @return  
    */
    virtual BYTE            GetCurState(void) = NULL;

    /** 取得游戏状态 ACTOR_GAME_STATE
    @return
    */
    virtual DWORD           GetGameState(void) = NULL;

    /** 取得日活跃度
    @return
    */
    virtual int             GetDayActivity(void) = NULL;

    /** 取得周活跃度
    @return
    */
    virtual int             GetWeekActivity(void) = NULL;

    /** 场景ID
    @return  
    */
    virtual DWORD           GetSceneID(void) = NULL;

    /** 地图ID
    @return  
    */
    virtual DWORD           GetMapID(void) = NULL;

    /** clientID
    @return  
    */
    virtual DWORD           GetClientID(void) = NULL;

    /** 服务器ID（中心服分配的）
    @return  
    */
    virtual DWORD           GetServerID(void) = NULL;

    /** 是否在本进程
    @return  
    */
    virtual bool            IsLocalServer(void) = NULL;

    /** 获取权限
    @return  
    */
    virtual int             GetPrivilege(void) = NULL;

    /** 获取属性
    @return  
    */
    virtual SSharePersonProperty GetProperty(void) = NULL;

    /** 增加英雄列表
    @return  
    */
    virtual void           AddHeroList(ShareHeroData * arrayHero, const DWORD dwArrayCount) = NULL;

    /** 更新英雄
    @return  
    */
    virtual bool            UpdateHero(ShareHeroData& hero, bool isAdd) = NULL;

    /** 获取已有全部英雄
    @return  
    */
    virtual DWORD           GetHeroList(ShareHeroData* arrayHero, const DWORD dwArrayCount) = NULL;

        /** 增加Rank
    @return  
    */
    virtual void            AddHeroRankList(ShareMatchRankInfo* arrayRank, const DWORD dwArrayCount) = NULL;

    /** 更新英雄Rank分
    @return  
    */
    virtual bool            UpdateRankScore(EMMatchType eMatchType, const ShareMatchRankInfo& info) = NULL;

    /** 获取比赛类型Rank分
    @return  
    */
    virtual DWORD           GetMatchTypeRank(EMMatchType eMatchType) = NULL;

    /** 获取比赛类型隐藏Rank分
    @return  
    */
    virtual DWORD           GetMatchTypeHideRank(EMMatchType eMatchType) = NULL;

    /** 获取比赛信息
	@return  
	*/
    virtual const ShareMatchRankInfo* GetMatchRankInfo(EMMatchType eMatchType) = NULL;
};

// 共享人物关注者
struct ISharePersonHandler
{
    /** 上线或者跨进程切换地图后，也是在这里得到通知
    @param   SHARELINE_TYPE ：在线回调类型
    @param   
    @return  
    */
    virtual void            OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType) = NULL;

    /** 下线或者跨进程切换地图后，也是在这里得到通知，当调完以后，就无法再取到
    @param   SHARELINE_TYPE ：在线回调类型
    @param   
    @return  
    */
    virtual void            OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType) = NULL;

    /** 更新数据前，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
    @param   pSharePerson ：更新前的对像
    @param   nUpdateFlag  ：改变原因
    @return  
    */
    virtual void            OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason) = NULL;

    /** 更新数据后，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
    @param   pSharePerson ：更新后的对像
    @param   nUpdateFlag  ：改变原因
    @return  
    */
    virtual void            OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason) = NULL;            
};

// 共享场景
struct IShareScene 
{
    /** 场景ID
    @return  
    */
    virtual DWORD           GetSceneID(void) = NULL;

    /** 地图ID
    @return  
    */
    virtual DWORD           GetMapID(void) = NULL;

    /** 场景状态
    @return  
    */
    virtual bool            IsEnable(void) = NULL;

    /** 取场景所在的场景服序号
    @return  
    */
    virtual DWORD           GetSerialID(void) = NULL;

    /** 取服务器ID
    @return  
    */
    virtual DWORD           GetServerID(void) = NULL;
};

// 服务器间的数据共享接收器
struct IShareReceiver
{
    /** 执行Service的on_stop
    */
    virtual void                            onStop() = NULL;

    /** 释放所有资源，并且销毁此对像
    @return  
    */
    virtual void                            Release(void) = NULL;

    /** 通过pdbid，获取SharePerson
    @param   
    @return  
    */
    virtual SSharePersonProperty            GetSharePerson(DWORD dwPDBID) = NULL;

    /** 通过名字，获取SharePerson
    @param   
    @return  
    */
    virtual SSharePersonProperty            GetSharePerson(LPCSTR pszName) = NULL;

    /** 通过pdbid，获取SharePerson
    @param   
    @param   
    @return  
    */
    virtual SSharePersonProperty            GetSharePersonByUDBID(DWORD dwUDBID) = NULL;

    /** 通过ClientID，获取SharePerson
    @param   
    @return  
    */
    virtual SSharePersonProperty            GetSharePersonByClientID(DWORD clientID) = NULL;

    /** 获取人物共享英雄列表
    @return
    */
    virtual DWORD                           GetPersonShareHeroList(DWORD dwID, ShareHeroData * pArray, const DWORD dwArrayCount, SHARE_NUM_PARAM paramType) = NULL;

    /** 获取所有共享人物特定数值属性
    @return
    */
    virtual DWORD                           GetNumPropertyAllSharePerson(DWORD * pArray, const DWORD dwArrayCount, SHARE_NUM_PARAM paramType) = NULL;

    /** 注册人物共享处理者
    @param   
    @param   
    @return  
    */
    virtual void                            RegisterHandler(ISharePersonHandler * pHandler, LPCSTR pszDebugDesc) = NULL;

    /** 取消人物共享处理者
    @param   
    @return  
    */
    virtual void                            UnregisterHandler(ISharePersonHandler * pHandler) = NULL;

    /** 中心服转发过来服务器间消息
    @param   
    @param   
    @return  
    */  
    virtual void                            onTransmit(DWORD server, SNetMsgHead* head, void * data, size_t len) = NULL;

    /** 中心服务器通知服务器的状态
    @param   
    @param  nState      : SERVER_STATE
    @param  pServerData : ServerData
    @return  
    */
    virtual void                            HandleServerInfoUpdated(DWORD ServerID, BYTE nState, void * pServerData) = NULL;

     /** 获取人物比赛Rank分
    @return
    */
    virtual DWORD                           GetPersonShareMatchRank(DWORD dwID,EMMatchType eMatchType,SHARE_NUM_PARAM paramType) = NULL;

    /** 获取人物比赛隐藏Rank分
    @return
    */
    virtual DWORD                           GetPersonShareMatchHideRank(DWORD dwID,EMMatchType eMatchType,SHARE_NUM_PARAM paramType) = NULL;

    /** 获取在线人物数量
    @param   
    @return  
    */
    virtual DWORD                           GetPersonCount() = NULL;

    /** 获取在线人物信息
    @param   
    @return  
    */
    virtual DWORD                           GetALLPersonInfo(SPersonInfo * pArray, const DWORD dwArrayCount) = NULL;


    /** 设置服务器掉线回调函数
    @param   
    @return  
    */  
    virtual bool                            SetServerDisconnectCallback( ServerDisconnectCallbackPtr callback)  = NULL;

};

// 场景服的数据共享源（只支持创建在场景服）
struct IShareSceneSvrSource
{
    /** 执行Service的on_stop
    */
    virtual void                            onStop() = NULL;

    /** 释放所有资源，并且销毁此对像
    */
    virtual void                            Release(void) = NULL;

    /** 人物状态变更
    @param   
    @param   
    @return  
    */
    virtual void                            PersonChangeState(DWORD dwPDBID, BYTE byState) = NULL;

    /** 人物身份证信息变更(此接口暂时没有用)
    @param   
    @param   
    @return  
    */
    //virtual void                          PersonChangeIdCard(DWORD dwPDBID, DWORD dwIdCard) = NULL;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SHARESERVER_STATIC_LIB)    /// 静态库版本
    #   pragma comment(lib, MAKE_LIB_NAME(ShareServer))
    extern "C" IShareReceiver * CreateShareReceiver(ICenterConnectorService * pCenterConnector, IEventEngine * pEventEngine);
    #   define  CreateShareReceiverProc CreateShareReceiver
#else                                                   /// 动态库版本
    typedef IShareReceiver * (RKT_CDECL *procCreateShareReceiver)(ICenterConnectorService * pCenterConnector, IEventEngine * pEventEngine);
    #   define  CreateShareReceiverProc DllApi<procCreateShareReceiver>::load(MAKE_DLL_NAME(ShareServer), "CreateShareReceiver")
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SHARESERVER_STATIC_LIB)    /// 静态库版本
    #   pragma comment(lib, MAKE_LIB_NAME(ShareServer))
    extern "C" IShareSceneSvrSource * CreateShareSceneSvrSource(void);
    #   define  CreateShareSceneSvrSourceProc   CreateShareSceneSvrSource
#else                                                   /// 动态库版本
    typedef IShareSceneSvrSource * (RKT_CDECL *procCreateShareSceneSvrSource)(void);
    #   define  CreateShareSceneSvrSourceProc   DllApi<procCreateShareSceneSvrSource>::load(MAKE_DLL_NAME(ShareServer), "CreateShareSceneSvrSource")
#endif
