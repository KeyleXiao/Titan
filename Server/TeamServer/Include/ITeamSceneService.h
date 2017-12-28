/*******************************************************************
** 文件名:	E:\speed\Server\TeamServer\Include\ITeamSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	聊天系统场景服 服务提供的接口
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "TeamDef.h"
using Gateway::ClientID;



/// 团
struct IGroup
{
	//////////////////////// IGroup ////////////////////////////////////
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

	/////////////// 团管理 ///////////////////////////////

	/** 加入队伍
	@param   int nTeamID	:	队伍ID
	@return  成功否
	*/
	virtual bool						Add(int nTeamID) = NULL;

	/** 移除队伍
	@param   int nTeamID	:	队伍ID
	@return  成功否
	*/
	virtual bool						Remove(int nTeamID) = NULL;

	/** 解散
	@return  
	*/
	virtual void						Clear(void) = NULL;

	/** 设置团长
	@param   DWORD dwPlayerID : 要任命的团长角色ID
	@return   成功否
	*/
	virtual bool						SetCaptain(DWORD dwPlayerID) = NULL;

	/** 设置拾取模式
	@param   
	@param   
	@return  
	*/
	virtual void						SetPickMode(DWORD dwPickMode) = NULL;

	/** 获取拾取模式
	@param   
	@param   
	@return  
	*/
	virtual DWORD						GetPickMode() = NULL;

};


/// 团
struct IGroupManager
{

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

/// 组队服务器
struct ITeam
{
	/** 队伍ID
	@param   
	@param   
	@return  
	*/
	virtual int						GetTeamID(void) = NULL;

	/** 队长PDBID
	@param   
	@param   
	@return  
	*/
	virtual DWORD					GetCaptain(void) = NULL;

	/** 拾取模式
	@param   
	@param   
	@return  
	*/
	virtual int						GetPickupMode(void) = NULL;

	/** dice模式信息
	@param   
	@param   
	@return  
	*/
	virtual SDiceModeInfo *			GetDiceModeInfo(void) = NULL;

	/** 是否已满
	@param   
	@param   
	@return  
	*/
	virtual bool					IsMemberFull(void) = NULL;

	/** 队员个数
	@param   
	@param   
	@return  
	*/
	virtual int						GetMemberCount(void) = NULL;

	/** 队友列表
	@param   
	@param   
	@return  
	*/
	virtual TLIST_MEMBERINFO *		GetMemberList(void) = NULL;	


	/** 队员信息
	@param   
	@param   
	@return  
	*/
	virtual STeamMemberInfo *		GetMemberInfo(DWORD dwPDBID) = NULL;

};

struct ITeamServer
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
	virtual void					Release(void) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ITeam *					GetTeamByTeamID(int nTeamID) = NULL;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ITeam *					GetTeamByPerson(DWORD dwPDBID) = NULL;


	/** 
	@param   
	@param   
	@return  
	*/
	virtual IGroupManager *			GetGroupManagerInterface(void) = NULL;

	/** 发起申请加入队伍（写在这里方面各模块调用）
	@param   
	@param   
	@return  
	*/
	virtual void					SysJoinTeam(int nTeamID, bool bIsCaptain, DWORD dwPlayer) = NULL;

	/** 发送统计数据到社会服
	@param	nTeamID		: 队伍ID
	@param	bIsAdd		: true：添加，false：清除
	@param  wFlagStatus	: Flag值
	*/
	virtual void					UpdateTeamFlagToSocial(int nTeamID, bool bIsAdd, WORD wFlagStatus ) = NULL;
};


struct  ITeamSceneService
{
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

	// 发送社会服消息
	virtual void    sendToSocial(BYTE nMsgID, void * data, size_t len) = 0;

    /** 获取玩家TeamID
	@param   
	@param   
	@return  
	*/
	virtual DWORD   getTeamIDByPlayer( PDBID PlayerID) = NULL;

    /** 根据TeamID获取成员列表接口
	@param          dwTeamID	:战队ID
    @param return Team成员数	:成员数量
	*/
	virtual DWORD   getTeamMemberList(DWORD dwTeamID, STeamMemberInfo* pReturnArray, DWORD dwArrayMaxSize) = NULL;

	/** 发起申请加入队伍（写在这里方面各模块调用）
	@param   
	@param   
	@return  
	*/
	virtual void    SysJoinTeam(int nTeamID, bool bIsCaptain, DWORD dwPlayer) = NULL;

    /** 外部接口（退出队伍）
    @param   
	@param   
	@return  
	*/
	virtual void    leaveTeam(PDBID dwActorID) = NULL;

};