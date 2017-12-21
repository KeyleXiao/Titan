/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Src\MatchSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	14:43
** 版  本:	1.0
** 描  述:	
             比赛大厅场景服 服务
********************************************************************/

#pragma once


#include "IMatchSceneService.h"
#include "NetMessageDef.h"
#include "MatchDef.h"
#include "MatchSchemeDef.h"
#include "SchemeDef.h"
#include "MatchSceneRoom.h"
#include "WarMatch.h"
#include "ICenterServerConnector.h "
#include "ICenterConnectorService.h"
#include "IEventEngine.h"
#include "IServiceContainer.h"
#include "ActorDef.h"
#include "lockfree/hashmap.h"
#include "Simple_Atom_Lock.h"


using namespace CenterServer;

struct IMessageDispatch;
//////////////////////////////////////////////////////////////////////////
class MatchSceneService : public IMatchSceneService, public TimerHandler, public IEventExecuteSink, public ITransmitHandler, public IMessageHandler
{
public:
	typedef speed::hashmap<DWORD, MatchSceneRoom*>	MAP_Rooms;
	typedef speed::hashmap<PDBID, DWORD>			MAP_PDBID2RoomID;
	typedef speed::hashmap<DWORD, DWORD>			MAP_WarID2RoomID;

	// 等待检测Actor释放完成的房间
	typedef	set<DWORD>					SET_WaitReleaseRooms;
	// 等待检测Actor创建完成的房间
	typedef	set<DWORD>					SET_WaitCreateRooms;

public:
    MatchSceneService();

    /////////MatchSceneService/////////////////////////////////////////////
    // _Stub::release() call
    void	release();

    // _Stub::on_start call
    bool	create();

    ///////////////////////////////////ITransmitHandler///////////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	////////////////////////////////IMessageHandler////////////////////////////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    ////////////////////////////TimerHandler//////////////////////////
	virtual void    OnTimer( unsigned long dwTimerID );

    virtual void	OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	///////////////////////////IMatchSceneService///////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	virtual void    start();

	virtual void    warEnd(DWORD dwWarID, LONGLONG llDBWarID);

	// 发送社会服消息
	virtual void    sendToSocial(BYTE byKeyAction, BYTE nMsgID, void * data, size_t len);

	// 发送创建结果
	virtual void    sendCreateMobaResult(DWORD dwRoomID, int nWarID, int nSceneID, bool bResult);

    virtual void    setWarProcessEnd(int nWarID);

    virtual void    setRuneTalentPageIndex(PDBID idActor,int nRunePageIndex,int nTalentPageIndex);

    virtual int     getTalentPageSelectIndex(PDBID idActor);

    virtual int     getRunePageSelectIndex(PDBID idActor);

    // 断线或者战场重连的时候调用这个更新Client
    virtual void   onUpdateRoomPlayerClient(PDBID idActor, ClientID client);

    // 主动退出战场
    virtual void   accordLeaveWar(PDBID idActor);

    // 角色是否在战场
    virtual bool   isActorInBattleField(PDBID idActor);

    // 获取角色的房间ID
    virtual int getRoomID(PDBID idActor);

	// 设置战场结果
	virtual void setWarResult(SMsgSetWarResultData resultData);

    // 清理下掉线的玩家
    virtual void warEndDestoryInvalidActor(int nWarID);
    ///////////////////////////IMatchSceneService end///////////////////////////
	// 根据SceneID获取房间ID
	DWORD	        getRoomIDByWarID( DWORD SceneID );

    // 根据玩家ID获取房间ID
	DWORD	        getRoomIDByPDBID( PDBID idActor );

    // 根据房间ID 获取房间信息
	MatchSceneRoom*	getRoomByID( DWORD RoomID );
	
    // 移除房间
    void	        removeRoom(DWORD RoomID);

    // 添加对应关系
	void	        addPDBID2RoomID(PDBID idActor, DWORD RoomID);

    // 移除玩家对应的房间对应关系
	void	        removePDBID2RoomID(PDBID idActor, DWORD dwRoomID);

    // 添加WarID对应的房间ID
	void	        addWarID2RoomID(DWORD dwWarID, DWORD RoomID);

    // 移除WarID对应的关系
	void	        removeWarID2RoomID(DWORD SceneID);

    // 设置玩家场景加载进度
    virtual void    setClientSceneLoadingProgress(PDBID idActor, int nProgress, long lPing);

    // 战场玩家执行迁移
    virtual void    createMobaActors(int nWarID);

    // 获取地图阵营u32Camp[0, ...)着陆配置 
    static SMapLandSchemeInfo*		getSchemeMapLand(uchar ModeID, DWORD MapID, uint32 u32Camp);

    // 战场结束玩家离开战场检查
    void    warEndPersonLeaveCheck( DWORD dwDBID, DWORD dwRoomID);

private:


    // 社会服响应创建战场消息
	void	onMsgCreateMoba( void* pData, size_t nLen );

    // 创建战场玩家信息
    void	onMsgCreateMobaRoleData( void* pData, size_t nLen );

    // 创建战场玩家信息
    void	onMsgCreateMobaOBRoleData( void* pData, size_t nLen );

    void	onMsgCollectionBankData( void* pData, size_t nLen );

	void    onMsgMatchRoomExData(void* pData, size_t nLen);

    void    onMsgMatchRoomBanHeroData(void* pData, size_t nLen);

    // 响应进入大厅消息
	void	onMsgEnterHall( ClientID clientID, void* pData, size_t stLen );

    // 创建引导房间
    void	onMsgCreateGuideRoom(ClientID clientID, void* pData, size_t stLen);

    // 响应创建房间消息
	void	onMsgCreateRoom( ClientID clientID, void* pData, size_t stLen );

    // 响应进入房间消息
	void	onMsgEnterRoom( ClientID clientID, void* pData, size_t stLen );

    // 响应开始游戏消息
	void	onMsgStartGame( ClientID clientID, void* pData, size_t stLen );

    // 响应开始游戏消息
    void	onMsgCustomerAddComputer( ClientID clientID, void* pData, size_t stLen );

    // 响应选择英雄消息
	void	onMsgSelectHero( ClientID clientID, void* pData, size_t stLen );

    // 响应预先选英雄消息
    void	onMsgPreSelectHero(ClientID clientID, void* pData, size_t stLen);

    // ban选英雄
    void	onMsgPosBanHero(ClientID clientID, void* pData, size_t stLen);

    // 响应准备战斗
	void	onMsgReadyToFight( ClientID clientID, void* pData, size_t stLen );

	// 响应选择英雄皮肤消息
	void	onMsgSelectHeroSkin( ClientID clientID, void* pData, size_t stLen );

    // 响应改变房间位置消息
	void	onMsgPosChange( ClientID clientID, void* pData, size_t stLen );

    // 响应退出房间消息
	void	onMsgExitRoom( ClientID clientID, void* pData, size_t stLen );

    // 响应踢出玩家消息
	void	onMsgKickOutPlayer( ClientID clientID, void* pData, size_t stLen );

    // 响应退出大厅消息
	void	onMsgExitHall( ClientID clientID, void* pData, size_t stLen );

    // 响应确定选择英雄消息
	void	onMsgConfirmSelectHero( ClientID clientID, void* pData, size_t stLen );

	// 处理匹配相关消息
	void	onMsgMatchDateMessage( ClientID clientID, void* pData, size_t stLen );

    // 处理匹配相关消息
    void	onMsgMatchServiceMessage( ClientID clientID, void* pData, size_t stLen );

    // 处理房间相关消息
    void	onMsgRoomDateMessage( ClientID clientID, void* pData, size_t stLen );

    // 接收召唤师技能信息
    void 	onMsgSummonerTalentSkillMessage( ClientID clientID, void* pData, size_t stLen );

    // 添加假人
    void    onMsgComputerPlayerAdd( ClientID clientID, void* pData, size_t stLen );

    // 移除假人
    void    onMsgComputerPlayerRemove( ClientID clientID, void* pData, size_t stLen );

    // 假人选择英雄
    void    onMsgComputerPlayerHeroChange( ClientID clientID, void* pData, size_t stLen );

    // 假人选择英雄AI
    void    onMsgComputerPlayerHeroAIChange( ClientID clientID, void* pData, size_t stLen );

    // 假人确认选择英雄
    void    onMsgComputerPlayerConfirmHero( ClientID clientID, void* pData, size_t stLen );

    // 更新引导信息
    void    onMsgUpdateGuideInfo( ClientID clientID, void* pData, size_t stLen );

    // 响应主动退出战场的消息
    void 	onMsgExitWar( ClientID clientID, void* pData, size_t stLen );
	
	// 响应进入大厅消息
    void	onMsgCustomerInviteRespon( ClientID clientID, void* pData, size_t stLen );

    // 响应主动退出战场的消息
    void 	onMsgTesterExitWar( void* pData, size_t stLen );

    // 房间内广播场景加载进度
    void    broadSceneLoadingProgress(int nWarID, PDBID idActor, int nProgress, long lPing);

    // 响应惩罚信息
    void	onMsgActorViewLoaded( ClientID clientID, void* pData, size_t stLen );

    // 响应玩家登陆
    void    onActorLogout(void* pLogoutEvent);

    // 响应玩家登陆
    void    onActorLogin(void* pLoginEvent);

    // 响应服务器有更新事件
	void	onEventServerInfoUpdated(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    void    onEventContinuePlayClientIncomming(LPCSTR pszContext, int nLen);
    
    // 更新数据到社会服
    void	dataRestoreToSocialSvr();

    void    createWarTimeOutTimer(int nWarID);

    // 通知社会服更新clientID
    void    notifySocialActorLogined(ClientID client, DWORD dwDBID);

	// 开启赛季
	void	onMsgMatchSeasonBegin(void* pData, size_t stLen);

	// 结束赛季
	void	onMsgMatchSeasonEnd(void* pData, size_t stLen);

private:

    unsigned short			                m_u16ServerID;

    MAP_WarID2RoomID		                m_WarID2RoomID;

    MAP_PDBID2RoomID		                m_PDBID2RoomID;

    MAP_Rooms				                m_RoomID2Room;

    MAP_Rooms                               m_mapWarID2Room;

    CWarMatch				                m_WarMatch;

    vector<int>			                    m_vecTimerID;

    map<int, DWORD>                         m_sceneID2RoomID;

    volatile	LONG                        m_mutex;

    map<DWORD, SMsgCreateRoom_OC>           m_mapCreateRoomContext;
    
    map<DWORD, obuf>                        m_mapCreateRoomRoleInfo;

    map<DWORD, obuf>                        m_mapCreateRoomOBRoleInfo;
};
