/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
            聊天系统服务提供的接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

/*******************************************************************
** 文件名:	e:\Rocket\SocialServer\TeamService\Include\ITeamService.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2008/6/11  21:15
** 版  本:	1.0
** 描  述:	组队服务	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "TeamDef.h"
#include "ActorDef.h"


/*! \addtogroup TeamService
*  组队服务
*  @{
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// 团
struct IGroup
{

	//////////////////////// IGroup ////////////////////////////////////

	/**  指向列表最前端
	@return  
	*/
	virtual void						MoveHead(void) = NULL;

	/**  指向列表最后端
	@return  
	*/
	virtual void						MoveEnd(void) = NULL;

	/**  指向列表下一个
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool						MoveNext(void) = NULL;

	/**  取得列表当前指向的数据
	@return  
	*/
	virtual const SGroupMemberInfo *	GetListData(void) = NULL;

	/**  指定组 指向列表最前端
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual void						GroupMoveHead(DWORD dwIndex) = NULL;

	/**  指定组 指向列表最后端
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual void						GroupMoveEnd(DWORD dwIndex) = NULL;

	/**  指定组 指向列表下一个
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool						GroupMoveNext(DWORD dwIndex) = NULL;

	/**  指定组 取得列表当前指向的数据
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@return  
	*/
	virtual const SGroupMemberInfo *	GroupGetListData(DWORD dwIndex) = NULL;

	/** 取得团ID
	@return  团ID
	*/	
	virtual DWORD						GetGroupID(void) = NULL;

	/** 取得团长PDBID
	@return  团长PDBID
	*/
	virtual DWORD						GetCaptain(void) = NULL;

	/** 取得团名称
	@return  团名称
	*/
	virtual const char *				GetName(void) = NULL;

	/** 是否已满
	@return  true:已满,false:没有满
	*/
	virtual bool						IsFull(void) = NULL;

	/** 取得队伍个数
	@return  队伍个数
	*/
	virtual int							GetCount(void) = NULL;

	/**  指定队伍ID是否在团中
	@param   int nTeamID	: 队伍ID
	@return  @return true: 存在 false:不存在
	*/
	virtual bool						ExistTeam(int nTeamID) = NULL;

	/** 是否统计
	@return 
	*/
	virtual bool						IsTotal(void) = NULL;

	/** 设定是否统计
	@return 
	*/
	virtual bool						SetTotal(bool bFlag) = NULL;

	/** 清除统计数据
	@return 
	*/
	virtual void						ClearTotal(void) = NULL;

	/** 加入统计数据
	@param   DWORD dwPlayerID	: 玩家ID
	@param  BYTE nType	: 类型	EMGroupTotalType
	@param  DWORD dwNumber : 数量
	@return 
	*/
	virtual bool						AddTotalInfo(DWORD dwPlayerID,BYTE nType,DWORD dwNumber) = NULL;

	/** 更新统计数据玩家在线状态
	@param  DWORD dwPlayerID		: 玩家ID
	@param  BYTE nFlagType			: 指定位标识
	@param  bool bFlag				: 要设置标识状态,true：是，false:否
	@return 成功否
	*/
	virtual bool						UpdateTotalOnlineState(DWORD dwPlayerID,BYTE nFlagType,bool bFlag) = NULL;

	////// 成员事件 ////////////////////////////////////////////////////////////////////

	/**  玩家加入事件
	@param   int nTeamID		: 队伍ID
	@param   DWORD dwPlayerID	: 玩家ID
	@return
	*/
	virtual void						OnPlayerJoin(int nTeamID,DWORD dwPlayerID) = NULL;

	/**  玩家离开事件
	@param   int nTeamID		: 队伍ID
	@param   DWORD dwPlayerID	: 玩家ID
	@return
	*/
	virtual void						OnPlayerLeave(int nTeamID,DWORD dwPlayerID) = NULL;

	/**  更新队长事件事件
	@param   int nTeamID		: 队伍ID
	@param   DWORD dwPlayerID	: 玩家ID(原队长)
	@return
	*/
	virtual void						OnTeamCaptain(int nTeamID,DWORD dwPlayerID) = NULL;

	/**   队伍解散事件
	@param   int nTeamID		: 队伍ID
	@return
	*/
	virtual void						OnTeamDisband(int nTeamID) = NULL;

	/////////////// 团管理 ///////////////////////////////

	/** 加入队伍
	@param   int nTeamID	:	队伍ID
	@param   bool bSync		:	是否同步数据
	@return  成功否
	*/
	virtual bool						Add(int nTeamID, bool bSync = true) = NULL;

	/** 移除队伍
	@param   int nTeamID	:	队伍ID
	@param   bool bSync		:	是否同步数据
	@return  成功否
	*/
	virtual bool						Remove(int nTeamID, bool bSync = true) = NULL;

	/** 加入组
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@param   int nTeamID	:	队伍ID
	@param   bool bSync		:	是否同步数据
	@return  成功否
	*/
	virtual bool						GroupAdd(DWORD dwIndex,int nTeamID) = NULL;

	/** 从组中移除队伍
	@param   DWORD dwIndex	:   组序号 1~GROUP_MAX_GROUP_INDEX
	@param   int nTeamID	:	队伍ID
	@param   bool bSync		:	是否同步数据
	@return  成功否
	*/
	virtual bool						GroupRemove(DWORD dwIndex,int nTeamID) = NULL;

	/** 解散
	@return  
	*/
	virtual void						Clear(void) = NULL;

	/** 设置团长
	@param   DWORD dwPlayerID : 要任命的团长角色ID
	@param    int nReason	  : 原因类型
	@param    bool bSync	  : 是否同步数据
	@return   成功否
	*/
	virtual bool						SetCaptain(DWORD dwPlayerID, int nReason=0, bool bSync=true) = NULL;


	/** 取得团所有成员ClientID列表
	@param   DWORD * pClientArray :ClientID列表数组
	@param   int wClientNum	:	列表数组大小
	@param   int nTeamID	:	队伍ID,0为全部队伍
	@param   bool bIncludeCaptain	:	是否包含团长
	@return  取得的个数
	*/
	virtual int							GetMemberClientIDList(DWORD * pClientArray,int wClientNum,int nTeamID=0,bool bIncludeCaptain=true) = NULL;

	/** 发送团基本信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendBaseInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 发送团的队伍成员信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendTeamInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 发送团的组信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendGroupInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 发送团的统计信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendTotalInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 发送团中队伍和队员信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendTeamMemberListToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 设置拾取模式
	@param   
	@param   
	@return  
	*/
	virtual void						SetPickMode(DWORD dwPickMode) = NULL;

	/** 设置入团模式
	@param   
	@param   
	@return  
	*/
	virtual void						SetActionMode(DWORD dwActionMode) = NULL;


	/** 获得入团模式
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetActionMode() = NULL;

	/** 设置等级限制
	@param   
	@param   
	@return  
	*/
	virtual void						SetLevelLimit(DWORD dwLevelLimit) = NULL;

	/** 获取等级限制
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetLevelLimit() = NULL;

	/** 检测一个队伍所有成员的等级是否满足团队等级限制
	@param   
	@param   
	@return  所有成员都满足返回true，否则返回false
	*/
	//virtual bool						IsAllTeamMemberLevelValid(int nTeamID, ostrbuf & strBuffOut) = NULL;

	///////////////////////////////团召集///////////////////////////////////////////
	/** 开启团召集响应
	@param  dwPlayerID	 ： 玩家ID
	@param  nConveneType ： 团召集的类型 EMGroupConveneType
	@param  nLevel ： 召集等级限制
	@param  nCostType ： 召集喊话类型
	@param  szDescr ： 团描述
	@return  
	*/
	virtual bool						OpenGroupConvene(DWORD dwPlayerID, BYTE nConveneType, int nLevel, int nCostType, const char * szDescr) = NULL;

	/** 关闭团召集
	@return  
	*/
	virtual bool						CloseGroupConvene(void) = NULL;

	/** 玩家响应团召集
	@param  dwPlayerID	： 玩家ID
	@return  
	*/
	virtual void						ResponseConvene(DWORD dwPlayerID) = NULL;

	/** 自动撮合团召集响应玩家到队伍
	@return  
	*/
	virtual bool						AutoConveneMatchTeam(void) = NULL;

	/** 广播召集信息
	@return  
	*/
	virtual bool						BroadcastConveneMessage(void) = NULL;

	/** 响应个人团召集事件
	@return  
	*/
	virtual bool						OnEventAddGroupConvene(void) = NULL;

	/** 开启自由召集召集事件
	@return  
	*/
	virtual bool						OpenFreeGroupConvene(void) = NULL;

	/** 获取是否开启团召集
	@return  
	*/
	virtual bool						IsOpenConvene(void) = NULL;


	/** 获取团总人数
	@return  
	*/
	virtual int							GetMemberTotal(void) = NULL;

	/** 获取团召集描述
	@return  
	*/
	virtual const char *				GetConveneDesc(void) = NULL;
};


/// 团
struct IGroupManager
{

	/** 创建团
	@param   DWORD dwCaptainDBID: 团长角色ID
	@param   bool bSync		:	是否同步数据
	@param   
	@return   IGroup * 团接口指针
	*/
	virtual IGroup *				CreateNewGroupByDBID(DWORD dwCaptainDBID,bool bSync=true) = NULL;

	/** 从列表中删除团
	@param   DWORD dwGroupID	: 团ID
	@param   
	@return  成功否
	*/
	virtual bool					RemoveGroup(DWORD dwGroupID) = NULL;

	/** 加入一个队伍到团
	@param   IGroup * pGroup :指定团
	@param   int nTeamID	: 队伍ID
	@param   
	@return  成功否
	*/
	virtual bool					AddTeam(IGroup * pGroup,int nTeamID) = NULL;

	/** 从团中删除一个队伍
	@param   IGroup * pGroup :指定团
	@param   int nTeamID	: 队伍ID
	@param   
	@return  成功否
	*/
	virtual bool					RemoveTeam(IGroup * pGroup,int nTeamID) = NULL;

	/** 更换团长角色ID
	@param   DWORD dwGroupID		: 团ID
	@param   DWORD dwNewCaptainDBID: 新团长角色ID,为0是自动从队长中选一个
	@param   
	@return  成功否
	*/
	virtual bool					ChangeGroupCaptain(DWORD dwGroupID,DWORD dwNewCaptainDBID) = NULL;

	/**  更换队伍
	@param   DWORD dwPlayerID :玩家角色ID
	@param   DWORD	dwTeamID;	更换成队的TeamID
	@param   DWORD	dwOpType;	操作类型 0:加入队,1:换队,2:离队,3:换队长
	@param   char * pResultMsg;	返回字串
	@param   int nMsgLen;	返回字串长度
	@return  bool 成功否
	*/
	virtual bool					ChangePlayerTeam(DWORD dwPlayerID,DWORD dwTeamID,DWORD dwOpType,char * pResultMsg,int nMsgLen) = NULL;

	/** 清除所有团数据
	@return  
	*/
	virtual void					Clear(void) = NULL;

	/** 用ID取得团接口指针
	@param   DWORD dwGroupID	: 团ID
	@param   
	@return  IGroup * 团接口指针
	*/
	virtual IGroup *				GetGroupByID(DWORD dwGroupID) = NULL;

	/** 用团长角色ID取得团接口指针
	@param   DWORD dwPDBID		: 团长角色ID
	@param   
	@return  IGroup * 团接口指针
	*/
	virtual IGroup *				GetGroupByCaptain(DWORD dwPDBID) = NULL;

	/** 用角色ID取得团接口指针
	@param   DWORD dwPDBID		: 角色ID
	@param   
	@return  IGroup * 团接口指针
	*/
	virtual IGroup *				GetGroupByPerson(DWORD dwPDBID) = NULL;

	/** 用队伍ID取得团接口指针
	@param   DWORD dwTeamID		: 队伍ID
	@param   
	@return  IGroup * 团接口指针
	*/
	virtual IGroup *				GetGroupByTeam(DWORD dwTeamID) = NULL;
};

//////////////////////////////////////////////////////////////////////////

/// 组队服务
struct ITeam
{
	/** 队伍ID
	@param   
	@param   
	@return  
	*/
	virtual int					GetTeamID(void) = NULL;

	/** 队长PDBID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetCaptain(void) = NULL;



	/** 是否已满
	@param   
	@param   
	@return  
	*/
	virtual bool				IsMemberFull(void) = NULL;

	/** 队员个数
	@param   
	@param   
	@return  
	*/
	virtual int					GetMemberCount(void) = NULL;

	/** 队友列表
	@param   
	@param   
	@return  
	*/
	virtual TLIST_MEMBERINFO *	GetMemberList(void) = NULL;	

	/** 队员信息
	@param   
	@param   
	@return  
	*/
	virtual STeamMemberInfo *	GetMemberInfo(DWORD dwPDBID) = NULL;

	/** 是否统计
	@return 
	*/
	virtual bool						IsTotal(void) = NULL;

	/** 设定是否统计
	@return 
	*/
	virtual bool						SetTotal(bool bFlag) = NULL;

	/** 清除统计数据
	@return 
	*/
	virtual void						ClearTotal(void) = NULL;

	/** 加入统计数据
	@param   DWORD dwPlayerID	: 玩家ID
	@param  BYTE nType	: 类型	EMGroupTotalType
	@param  DWORD dwNumber : 数量
	@return 
	*/
	virtual bool						AddTotalInfo(DWORD dwPlayerID,BYTE nType,DWORD dwNumber) = NULL;

	/** 更新统计数据玩家在线状态
	@param  DWORD dwPlayerID		: 玩家ID
	@param  BYTE nFlagType			: 指定位标识
	@param  bool bFlag				: 要设置标识状态,true：是，false:否
	@return 成功否
	*/
	virtual bool						UpdateTotalOnlineState(DWORD dwPlayerID,BYTE nFlagType,bool bFlag) = NULL;

	/** 发送队伍的基本信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendBaseInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 发送团的统计信息到客户端
	@param	pClientArray: 要发送的客户端ID数组
	@param	wClientNum  : 客户端数目
	@return  
	*/
	virtual void						SendTotalInfoToClient(ClientID * pClientArray,int wClientNum) = NULL;

	/** 队员加入
	@param   
	@param   
	@return  
	*/
	virtual bool						JoinMember(STeamMemberInfo * pMember, bool bSync) = NULL;

	/** 队员离开
	@param   
	@param   
	@return  
	*/
	virtual bool						LeaveMember(DWORD dwPDBID, int nReason, bool bSync) = NULL;


	/** 还原队伍数据
	@param   
	@param   
	@return  
	*/
	virtual bool						RestoreMember(STeamMemberInfo * pMember, int nCount) = NULL;

	/** 还原数据至场景服
	@param   
	@param   
	@return  
	*/
	virtual bool						RestoreDataToZoneSvr(DWORD ServerID) = NULL;

	/** 场景服当了，需要更新队友状态
	@param   
	@param   
	@return  
	*/
	virtual void						ZoneSvrDisconnected(DWORD ServerID) = NULL;

	/** 离开队伍
	@param   
	@param   
	@return  
	*/
	virtual bool						DoLeave(DWORD dwPDBID) = NULL;

	/** 队伍解散
	@param   
	@param   
	@return  
	*/
	virtual void						DoDisband(void) = NULL;

    /** 请求其他队员到房间（除去队长）
	@param   
	@param   
	@return  
	*/
	virtual void						inviteMemberToRoom(DWORD dwRoomID) = NULL;
};

struct IStateHandler
{
	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnLogin(DWORD dwPDBID, DWORD clientID, ELoginMode eMode){ };

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnLogout(DWORD dwPDBID, DWORD clientID, ELogoutMode eMode, bool bActiveExit){ };
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 数据镜像服务
/// 很多时候，社会服需要访问场景服的数据，比如角色数据，如果要用的时候，异步去获取，
/// 这样会写程序起来会非常麻烦，所以必须要有一套机制能让场景服的数据能同步社会服来，
/// 但如果所有的角色都自动同步过来，数据量会非常大，并且会影响服务器的效率，所以还
/// 是采取观察的模式，只将需要观察的角色同步过来

// 镜像虚拟人物
struct IMirrorPerson
{
	/** 销毁
	@param   
	@param   
	@return  
	*/ 
	virtual void				Release(void) = NULL;

	/** 取得UID
	@param   
	@param   
	@return  
	*/
	virtual LONGLONG &			GetUID(void) = NULL;

	/** 取得场景ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetZoneID(void) = NULL;

	/** 取得所在的地图位置
	@param   
	@param   
	@return  
	*/
	virtual POINT &				GetMapLoc(void) = NULL;

	/** 取得生物（人物／怪物）名字
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetName(void) = NULL;

	/** 取得数值属性
	@param   
	@param   
	@return  
	*/
	virtual int					GetNumProp(DWORD dwPropID) = NULL;

	/** 取得来源的游戏世界ID，如果是普通区，则为当前游戏世界ID
	@param   
	@param   
	@return  
	*/
	virtual int					GetFromGameWorldID(void) = NULL;
};

// 镜像人物回调
struct IMirrorPersonSink
{
	/** 更新数据前回调，分前后，主要是让应用层知道所关心的属性是否变更了
	@param   
	@param   
	@return  
	*/
	virtual	void				OnPre_Update(DWORD dwPDBID) = NULL;

	/** 更新数据后回调
	@param   
	@param   
	@return  
	*/
	virtual void				OnPost_Update(DWORD dwPDBID) = NULL;

	/** 登陆上线，此时通过dwPDBID取得IMirrorPerson
	@param   bOffline ：true=新登陆，false=切换场景登陆
	@param   
	@return  
	*/
	virtual void				OnLogin(DWORD dwPDBID, bool bOffline) = NULL;

	/** 登出下线
	@param   bOffline ：true=离开游戏，false=切换场景登陆
	@param   
	@return  
	如果bOffline为true，调用此方法后，无法通过dwPDBID取得IMirrorPerson
	*/
	virtual void				OnLogout(DWORD dwPDBID, bool bOffline) = NULL;
};


struct IBuildTeamFlow
{
	/** 销毁
	@param   
	@param   
	@return  
	*/ 
	virtual void				Release(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual	int					GetJoinFlowType(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				OnMessage(DWORD dwPDBID, DWORD dwMsgID, LPCSTR pData, int nLen) = NULL;
};


struct ITeamService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) = 0;

	// 断开连接
	virtual void onClientDisconnect(ClientID client,DWORD dwReason) = 0;

    /** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD				getTeamIDByPlayer( PDBID PlayerID) = NULL;

    /** 根据TeamID获取成员列表接口
	@param          dwTeamID	:战队ID
    @param return Team成员数	:成员数量
	*/
	virtual DWORD               getTeamMemberList(DWORD dwTeamID, STeamMemberInfo* pReturnArray, DWORD dwArrayMaxSize) = NULL;


	/** 发起申请加入队伍（写在这里方面各模块调用）
	@param	dwPDBID		  ：玩家ID
	@param	nTeamID		  ：队伍ID
	@param  bIsCaptain	  ：是否为队长
	@param  nState		  ：在线离线
	@param  pPersonContext：玩家信息
	@return  
	*/
	virtual void				SysJoinTeam(DWORD dwUserID, DWORD dwPDBID, DWORD dwClientID, int nTeamID, bool bIsCaptain, BYTE nState, STeamSyncPersonContext* pPersonContext) = NULL;

	/** 请求加入队伍,会通过场景服取得信息后加入队伍
	@param	dwPDBID		  ：玩家ID
	@param	nTeamID		  ：队伍ID
	@param  bIsCaptain	  ：是否为队长
	@return  
	*/
	virtual void				RequestJoinTeam(DWORD dwPDBID,int nTeamID, bool bIsCaptain) = NULL;

    /** 
	@param	dwPDBID		  ：玩家ID
	@return  
	*/
	virtual bool				isTeamCaptain(DWORD dwPDBID) = NULL;

    /** 
    @param	dwTeamID		  ：组队ID
	@param	dwRoomID		  ：房间ID
	@return  
	*/
	virtual void				inviteMemberToRoom(DWORD dwTeamID, DWORD dwRoomID) = NULL;
};