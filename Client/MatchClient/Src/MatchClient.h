/*******************************************************************
** 文件名:	MatchClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	比赛房间系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once
#include "GatewayServiceDef.h"
#include "IMatchClient.h"
#include "IEntityClient.h"
#include "IClientGlobal.h"
#include "CurrentMatchRoomInfo.h"


class MatchClient:public IMatchClient, public IGameViewEventHandler, public IMessageHandler
{
public:

	////////////////////////////IMatchClient//////////////////////////////////////////////
	virtual void		        release(void);

    virtual void                enterHall(void * context,int len) ;

    virtual SMsgCreateRoom_OC*     getRoomData(DWORD RoomID);

    virtual uint32              getCampPlayerCountMax( DWORD dwMatchType);

	virtual	uint32				getObCountMax(DWORD dwMatchType);

    // 获取 房间最大允许人数
    virtual uint32	            getPlayerCountMax(DWORD dwMatchType);

    virtual void	            make(SMsgCreateRoom_OC* pRoom, cmd_match_hall_add_room& cmd);

    virtual SMapSchemeInfo*		getSchemeMap(DWORD MapID);

    virtual SSchemeMatchRoom*	getSchemeMatchRoomByMatchType(DWORD dwMatchType);

    virtual char const*			getTextInfo( int nID );

    virtual BYTE	            getCampCount(DWORD dwMatchType);

    virtual void	            loadSenece(DWORD dwMapID, Vector3 vecBornPos);

    virtual DWORD	            getCurRoomMatchType();

    virtual int	                getCurRoomType();

    virtual DWORD	            getCurRoomID();
    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool		        onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

    /////////////////////////////IMessageHandler/////////////////////////////////////////////////////////
    virtual void		        onMessage( SNetMsgHead* head, void* data, size_t len);

public:
	// MatchClient
	bool    Create(void);

    void    showHall();

    void	initFromRoom( void* data,size_t len );

    void	addRoom(SMsgCreateRoom_OC* pRoomData);
    void	removeRoom(SMsgCreateRoom_OC* pRoom);

    void	syncAddRoom( SMsgCreateRoom_OC* pRoomData );
    void	syncFreshRoom( SMsgCreateRoom_OC* pRoomData );
    void	syncRemoveRoom( DWORD RoomID );

    void	addHallRoom( void* pData, size_t stLen );
    ISchemeMatchRoom*		getSchemeMatchRoom();
    ISchemeTextInfo*		getSchemeTextInfo();
    char const*				getModeName( uchar ModeID );
    char const*				getStateName( int nState );
    char const*				getMapDesc(DWORD dwMatchType);
    char const*				getMapPicPath(DWORD dwMatchType);
    
    // 获取 地图最大阵营数
    BYTE	getCampCount( SMsgCreateRoom_OC* pRoom);

    // 获取 阵营最大人数
    uint32	getCampPlayerCountMax(SMsgCreateRoom_OC* pRoom);

    bool    isShowInHall( SMsgCreateRoom_OC* pRoomData );

    // 将玩家的召唤师技能发送过去
    void    sendAllSummonerSkillInf(int nWarMode);

    // 大厅服消息
    void onMatchMessage( SNetMsgHead* head,void* data,size_t len );
    void onMSEnterHall( SNetMsgHead* head, void* data, size_t len );
    void onMsgCreateRoom(SNetMsgHead* head, void* data, size_t len);
    void onMsgRoom(SNetMsgHead* head, void* data, size_t len);
    void onMsgEnterRoom(SNetMsgHead* head, void* data, size_t len);
    void onMsgOBEnterRoom(SNetMsgHead* head, void* data, size_t len);
    void onMsgRoomRoleCount(SNetMsgHead* head, void* data, size_t len);
    void onMsgRoomState(SNetMsgHead* head, void* data, size_t len);
    void onMsgReleaseRoom( SNetMsgHead* head, void* data, size_t len );
    void onMsgExitRoomToHall( SNetMsgHead* head, void* data, size_t len );
    void onMsgCancelMatch( SNetMsgHead* head, void* data, size_t len );
    void onMsgIsMatching( SNetMsgHead* head, void* data, size_t len );
    void onMsgMatchOK( SNetMsgHead* head, void* data, size_t len );
    void onMsgMatchFail( SNetMsgHead* head, void* data, size_t len );
    void onMsgExitHall( SNetMsgHead* head, void* data, size_t len );
	void onMsgExitHallFromLegendCupRoom( SNetMsgHead* head, void* data, size_t len );
    void onMsgWaitAccept(SNetMsgHead* head, void* data, size_t len);
    void onMsgMatchPlayerCountInfo(SNetMsgHead* head, void* data, size_t len);
    void onMsgBroadOtherAcceptmatch(SNetMsgHead* head, void* data, size_t len);
    void onMsgGetRefuseMatch(SNetMsgHead* head, void* data, size_t len);
    void onMsgSelfPunishInfo(SNetMsgHead* head, void* data, size_t len);
    void onMsgMatchRequestPunishInfo(SNetMsgHead* head, void* data, size_t len);
    void onMsgDelPunishInfo(SNetMsgHead* head, void* data, size_t len);
    void onMsgMatchCloseOrOpen(SNetMsgHead* head, void* data, size_t len);
	void onMsgMatchRecommendRank(SNetMsgHead* head, void* data, size_t len);

    void onViewSendExitGameMsg();
    void onViewSendExitWarMsg();
    void onViewCmdMatchOpenCreateRoom(int nParam,const char * strParam,void * ptrParam);
    void onViewCmdMatchCreateRoom(int nParam,const char * strParam,void * ptrParam);
    void onViewCmdMatchEnterRoom(int nParam,const char * strParam,void * ptrParam);
    void onViewCmdMatchHallRoomData( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdMatchToChooseVocation( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdEnterHall( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdExitHall( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdBeginMatch( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdCancelMatch( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdMatchGuideInfo(int nParam,const char * strParam,void * ptrParam);
    void onViewCmdCreateGuideRoom(int nParam,const char * strParam,void * ptrParam);
    void onViewCmdAcceptMatch( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdRefuseMatch( int nParam, const char * strParam, void * ptrParam );
    void onViewCmdCustomerInviteRespon( int nParam, const char * strParam, void * ptrParam );
    void onViewMasterViewLoaded( int nParam, const char * strParam, void * ptrParam );
	void onViewCmdMatchRecommendResult( int nParam, const char * strParam, void * ptrParam );
    
private:

    typedef map<DWORD, SMsgCreateRoom_OC> MAP_Rooms;
    // 所有房间数据
    MAP_Rooms	m_mapRooms;

    // 当前的正在请求的撮合类型
    int     m_nMatchType;
   
};

extern MatchClient* g_pMatchClient;