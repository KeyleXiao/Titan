/*******************************************************************
** 文件名:	KinClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	战队系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once
#include "IKinClient.h"
#include "IEntityClient.h"
#include "MatchDef.h"
#include "Kin.h"

class KinClient:public IKinClient, public IGameViewEventHandler, public IMessageHandler
{
public:

    /// purpose: 构造函数
    KinClient();

    /// purpose: 虚构函数
    virtual ~KinClient();

	////////////////////////////IKinClient//////////////////////////////////////////////
	virtual void		release(void);

     /** 创建战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallCreateKin(const char* szName);

    /** 申请加入战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallJoinKin(DWORD dwKinID, string strLeaveWord);

     /** 退出战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallQuitKin();

    /** 解散战队
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallDissmissKin();

    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		onMessage( SNetMsgHead* head, void* data, size_t len);

    /// purpose: 客户端进入观察
    virtual void        enterObserveRequest();

    /// purpose: 客户端退出观察（关闭窗口）
    virtual void        exitObserveRequest();

public:
	// KinClient
	bool				Create(void);

private:
    /// purpose: 请求战队改名
    void                requestKinRename(char * szNewName);

    // purpose: 接到邀请消息
    void                onClientInvite(LPCSTR pszMsg, int nLen);

    // purpose: 邀请回应
    void                answerInvite(DWORD dwPDBID, bool bIsAgree);

    /// purpose: 请求设置职位
    void                appointRequest(DWORD dwPDBID, int nIdentity);

    // purpose: 请求邀请加入战队
    void                inviteRequest(DWORD dwKinID, string strLeaveWord);

    // purpose: 请求申请加入战队
    void                joinRequest(DWORD dwKinID, string strLeaveWord);

    // purpose: 请求退出战队
    void                quitRequest();

    /// purpose: 请求开除成员
    void                kickRequest(DWORD dwPDBID);

    /// purpose: 请求创建战队
    void                createKinRequest(const char* szName, const char* szLabel);

    // purpose: 更新成员数据
    void                onUpdateMember(LPCSTR pszMsg, int nLen);

    // purpose: 退出战队
    void                onQuit(LPCSTR pszMsg, int nLen);

    // purpose: 更新成员数据
    void                onMsgKinList(LPCSTR pszMsg, int nLen);

    // purpose: 客户端事件
    void                onClientEvent(LPCSTR pszMsg, int nLen);

    /// purpose: 解散战队
    void                dismissRequest();

    /// purpose: 修改公告
    void                MotifyRequest(int nPropID, char* szText);

	/// purpose: 查看杯赛战队信息
	void				onViewLegendCupKinInfo(LPCSTR pszMsg, int nLen);

	/// purpose: 查看杯赛战队荣誉
	void				onViewLegendCupKinGlory(LPCSTR pszMsg, int nLen);

	// purpose: 更新战队列表
	void                onMsgUpdateKinList(LPCSTR pszMsg, int nLen);

private:
    CKin m_Kin;								// 战队对象

    SKinMember m_MemberInfo;				// 玩家战队信息
    bool m_bMemberVaild;					// 玩家战队信息是否有效
    int  m_myApplyKinID;					// 当前正在申请中的帮会
    int	 m_nCoolingTime;					// 操作开始时间
   
};

extern KinClient* g_pKinClient;