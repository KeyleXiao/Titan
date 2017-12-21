/*******************************************************************
** 文件名:	MatchSceneRoom.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	4/21/2015	14:22
** 版  本:	1.0
** 描  述:	战斗中的房间
            
********************************************************************/

#pragma once

#include "MatchDef.h"
#include "SchemeDef.h"
#include "ITimerService.h"
#include "MigrateDef.h"
#include "IDBEngine.h"
 

class MatchSceneService;
struct SSchemeMatchRoom;

//////////////////////////////////////////////////////////////////////////
// 房间
class MatchSceneRoom : public IEventExecuteSink, public TimerHandler
{
	enum 
	{
        ETIMER_CLIENT_AUTO_END_TIMER,		    // 自动移除房间定时器
	};

public:

    MatchSceneRoom();

    ~MatchSceneRoom();

	// 释放
	void    realese();

	// 清理房间数据
	void	clear();

	// 加载房间数据
	void	inital(MatchSceneService* pMatchSceneService, SMsgCreateRoom_OC* pCreateRoom, int nWarID );

    // 加载房间数据
    void	initalOBPlayer( SRoomRoleBase* pFirstRole );

    // 加载房间数据
    void	initalPlayer( SRoomRoleBase* pFirstRole );

    // 房间ID
	DWORD			getRoomID(){return m_msg.dwRoomID;};

    // 获取房间所在场景
	DWORD			getSceneID(){return m_nSceneID;};

    // 房间地图信息
	DWORD			getMapID(){return m_msg.dwMapID;};

    // 模式的类型 EGameMode
	DWORD			getModeID(){return m_msg.nModeID;};

    // 获取scheme配置WarID
	DWORD			getSchemeConfigWarID(){return m_msg.nConfigWarID;};

	// 获取房间类型
	BYTE			getRoomType(){return m_msg.nRoomType;};

	// 获取房间类型
	SMsgLegendCupRoomInfo getLegendCupRoomInfo(){return m_sLegendCupRoomInfo;};

	// 设置杯赛房间数据
	void			setLegendCupRoomInfo(SMsgLegendCupRoomInfo *pExData);

    // 状态
	ESceneRoomState	getState(){return m_eRoomState;};

	// 设置房间状态
	void	        setState(ESceneRoomState state);

    // 设置房间状态
    void	        setStartMigrateTime();

    // 根据ClientId 和 UI位置获得槽位数据
    int             getSummonerSlot( ClientID clientId, int nUiSlot);

	// 给房间所有客户端发消息
	void	        broadcast2Client( char* pData, int nSize );

    // 设置召唤师技能对应的槽位
    void            setSummonerSkillSlot();

    // 通知社会服战场释放了
    void	        send2SocialWarEnd();

    // 根据ClientId 和 UI位置获得槽位数据
    int             getTalentPageSelectIndex(PDBID PlayerID);

    // 根据ClientId 和 UI位置获得槽位数据
    int             getRunePageSelectIndex(PDBID PlayerID);

    // 创建战场人物
    bool            createMobaActor(MatchScenePlayer * pSceneRoomPlayer);

    // 创建OB玩家到战场
    bool            createMobaOBActor(PDBID PlayerID);

    // 设置玩家基本信息
    void            setWarActorBaseInfo();

    // 获取有效人物数量（排除电脑玩家）
    DWORD			getPlayerCount();

    // 获取所有人物数量（包括电脑玩家）
    DWORD			getAllPlayerCount();

    // 设置人物状态
    bool            setPlayerReady(PDBID idActor);

    // 设置OB人物状态
    bool            setOBPlayerReady(PDBID idActor);

    //////////////////////////////////////////////////////////////////////////
    // 注意！使用这个函数之前先查询哪里调用这个函数
    //////////////////////////////////////////////////////////////////////////
    void           setPlayerLogout(PDBID idActor);

    // 设置OB玩家退出
    void           setOBPlayerLogout(PDBID idActor);

    // 角色登陆到动态场景处理
    void            setPlayerLogin(PDBID idActor);

    // 获取全部登出数量
    bool           isAllOBLogout();

    // 获取全部登出数量
    DWORD           getAllLogoutCount();

    // 获取无效玩家数量
    DWORD           getClientInvalidCount();

    // OB玩家是否都无效了
    bool            allOBPlayerIsInValid();

    // 设置战场结束标示
    void            setWarProcessEnd();

    // 获取战场结束标示
    bool            getWarProcessEnd();

    // 清理下掉线的玩家
    void             warEndDestoryInvalidActor();

    // 开始创建所有角色
    void            onCreatePlayers();

    // 检测所有角色是否都准备好了
    bool            checkPlayersReady();

    // 取得指定角色数据
    MatchScenePlayer* findPlayer(DWORD dwPlayerID);

    // 取得指定角色数据
    MatchScenePlayer* findOBPlayer(DWORD dwPlayerID);

    //获取创建房间信息
    void            getCreateRoomInfo(SMsgMatchRoomDataRestore &sRestoreInfo);

    //获取房间所有成员信息
    DWORD            getRoomAllMatchScenePlayer(obuf &PlayerInfo);

    // 加载场景超时
    bool             isLoadingSceneTimeOut();

	// 发送战斗结果to社会服杯赛模块
	void			 sendWarResult2SocialLegendCup(SMsgSetWarResultData resultData);

    // 设置杯赛信息
    void             setWarLegendCupInfo();

    // 主动退出战场
    void             accordLeaveWar(PDBID idActor);

	///////////////////////////////////////IEventExecuteSink/////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void    OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////TimerHandler//////////////////////////
    virtual void    OnTimer( unsigned long dwTimerID );

	// 获取推荐返回的主城
	const SSceneNode&		getRecommandScene() { return m_RecommandScene; }

    void            setAutoEnd();

    void            clearAutoEnd();

    void            destoryTrueRoomPlayer();

    void            destoryOBRoomPlayer();
    // 释放房间所有电脑玩家
    void            destoryRoomPlayer();

    // 释放房间所有电脑玩家
    void            destoryPlayer(DWORD dwActorID);

    // 释放房间所有电脑玩家
    void            destoryOBPlayer(DWORD dwActorID);

    // 是否是OB玩家
    bool            isOBPlayer(PDBID idActor);

    // 是否是OB玩家
    MatchScenePlayer*   getOBPlayer(PDBID idActor);

private:
	// 玩家返回主城时，推荐返回同一个场景
	void calcRecommandScene();

    // 移除已经掉线的真实玩家
    void removeDisConnectedTruePlayer();

	// 生成玩家的出生点（有的地图需要支持玩家出生点随机）
	void randomBornPos(DWORD dwModeID, DWORD dwMapID);

	// 获取出生点位置
	bool getBornPos(int nCamp, int nRoleIndex, SPosWithAngle& pos);

public:
	volatile	LONG            m_mutex;
	MatchSceneService*			m_pService;

	SMsgCreateRoom_OC		    m_msg;
	ESceneRoomState				m_eRoomState;
	int						    m_nWarID;
    int                         m_nSceneID;
    MatchScenePlayer*           m_arrayPlayer;
    MatchScenePlayer*           m_arrayOBPlayer;
    DWORD                       m_dwComputerCount;
    DWORD                       m_dwPlayerCount;
    int                         m_nSummonerSlot[SUMMONER_SKILL_COUNT];
    bool                        m_bWarProcessEnd;
    DWORD                       m_dwStartMigrateTime;
    bool                        m_bAllActorOut;
	SMsgLegendCupRoomInfo		m_sLegendCupRoomInfo;

	// 玩家返回主城时，推荐返回同一个场景
	SSceneNode					m_RecommandScene;
    
    // 自动移除标示(处理)
    bool                        m_bAutoEnd;

	// 玩家随机出生点<阵营,出生点>
	map<int,Vector3>			m_mapRandBornPos;

	// 玩家最大下标
	int							m_nMaxRoleIndex;
};