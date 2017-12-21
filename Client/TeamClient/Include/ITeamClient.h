/*******************************************************************
** 文件名:	e:\Rocket\Client\TeamClient\Include\ITeamClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2008/6/3  17:17
** 版  本:	1.0
** 描  述:	组队客户端	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

/*! \addtogroup TeamClient
*  组队客户端
*  @{
*/
#include "DTeamModule.h"
#include "Team_ManagedDef.h"

using namespace Team_ManagedDef;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// 组队客户端


/// 团
struct IGroup
{

	
};


struct ITeam
{

	/**  指向列表最前端
	@return  
	*/
	virtual void				MoveHead(void) = NULL;

	/**  指向列表最后端
	@return  
	*/
	virtual void				MoveEnd(void) = NULL;

	/**  指向列表下一个
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool				MoveNext(void) = NULL;

	/**  取得列表当前指向的数据
	@return  
	*/
	virtual const STeamMemberInfo *	GetListData(void) = NULL;

	/** 队伍ID
	@param   
	@param   
	@return  
	*/
	virtual int				GetTeamID(void) = NULL;

	/** 队长PDBID
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetCaptain(void) = NULL;
	

	/** 队员个数
	@param   
	@param   
	@return  
	*/
	virtual int				GetMemberCount(void) = NULL;

	/** 队友列表
	@param   
	@param   
	@return  
	*/
	virtual TLIST_MEMBERINFO *	GetMemberList(void) = NULL;

	/** 是否为队友
	@param   
	@param   
	@return  
	*/
	virtual bool				IsTeammate(DWORD dwPDBID) = NULL;

	/** 队友信息
	@param   
	@param   
	@return  
	*/
	virtual STeamMemberInfo *	GetMemberInfo(DWORD dwPDBID) = NULL;

};

struct ITeamClient
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

	/** 小退清空数据
	@param   
	@param   
	@return  
	*/
	virtual void			Clean(void) = NULL;

	/** 取得所在队伍
	@param   
	@param   
	@return  
	*/
	virtual ITeam *			GetTeam(void) = NULL;

	/** 取得所在团
	@param   
	@param   
	@return  
	*/
	virtual IGroup *		GetGroup(void) = NULL;

	/** 离开队伍
	@param   
	@param   
	@return  
	*/
	virtual bool			LeaveTeam(void) = NULL;

	/** 解散队伍
	@param   
	@param   
	@return  
	*/
	virtual bool			DisbandTeam(void) = NULL;


	/** 申请加入队伍
	@param
	@param   
	@return  
	*/
	virtual bool			RequestJoin(DWORD dwPDBID) = NULL;

	/** 邀请加入队伍
	@param   
	@param   
	@return  
	*/
	virtual bool			InvitedJoin(DWORD dwPDBID) = NULL;

	/** 开除成员
	@param   
	@param   
	@return  
	*/
	virtual bool			KickoutMember(DWORD dwPDBID) = NULL;

	/** 任命队长
	@param   
	@param   
	@return  
	*/
	virtual bool			AppointCaptain(DWORD dwNewCaptain) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 响应邀请组队
	@param   
	@param   
	@return  
	*/
	virtual bool			InvitedRespond(DWORD dwBuildingSN, int nResult) = NULL;

	/** 响应申请组队
	@param   
	@param   
	@return  
	*/
	virtual bool			RequestRespond(DWORD dwBuildingSN, int nResult) = NULL;


	/** 取得邀请者现场
	@param   
	@param   
	@return  
	*/
	virtual SLuaInviteDialogMember *	GetInviteDialogMember(DWORD dwBuildingSN, int nIndex) = NULL;

	/** 取得申请者现场
	@param   
	@param   
	@return  
	*/
	virtual SLuaInviteDialogMember *	GetRequestPersonContext(DWORD dwBuildingSN) = NULL;


	/**  选中实体
	@return  
	*/
	virtual void								OnSetTarget(UID uid) = NULL;


	/////////////////////// 组队设定 //////////////////////////////////////////////
	/** 设置自动组队
	@param   
	@param   
	@return  
	*/
	virtual void								SetAutoJoinTeam(bool bAuto) = NULL;

	/** 当前自动组队状态
	@param   
	@param   
	@return  
	*/
	virtual bool								GetAutoJoinTeam(void) = NULL;

	/** 设置拒绝组队
	@param   
	@param   
	@return  
	*/
	virtual void								SetRefuseJoinTeam(bool bRefuse) = NULL;

	/** 当前拒绝组队状态
	@param   
	@param   
	@return  
	*/
	virtual bool								GetRefuseJoinTeam(void) = NULL;

	/** 队友血量变更
	@param   
	@param   
	@return  
	*/
	virtual void								MemberHPChange(DWORD dwPDBID, int nCurHP, int nMaxHP) = NULL;

	/** 申请投票踢人
	@param   
	@param   
	@return  
	*/
	virtual void								ApplyVoteKickMember(DWORD dwKickMemberID) = NULL;

	/** 回应投票踢人
	@param   
	@param   
	@return  
	*/
	virtual void								ConsultKickMember(DWORD dwVotingSN, bool bAgree) = NULL;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TEAMCLIENT_STATIC_LIB)		// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(TeamClient))
extern "C" ITeamClient * CreateTeamClient(void);
#	define	CreateTeamClientProc	CreateTeamClient
#	define	CloseTeamClient()
#elif !defined(TEAMCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(TeamClient))
API_IMPORT ITeamClient * CreateTeamClient(void);
#	define	CreateTeamClientProc	CreateTeamClient
#	define	CloseTeamClient()
#else													// 动态加载
typedef ITeamClient * (RKT_CDECL *procCreateTeamClient)(void);
#	define	CreateTeamClientProc	DllApi<procCreateTeamClient>::load(MAKE_DLL_NAME(TeamClient), "CreateTeamClient")
#	define	CloseTeamClient()		DllApi<procCreateTeamClient>::freelib()
#endif
