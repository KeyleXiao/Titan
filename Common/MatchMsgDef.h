/*******************************************************************
** 文件名:	E:\speed\Common\MatchMsgDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/19/2015	19:54
** 版  本:	1.0
** 描  述:	
            比赛大厅相关公共定义
********************************************************************/

#pragma once
#include"KinDef.h"

#pragma pack(1)

// 消息ID定义
//////////////////////////////////////////////////////////////////////////
enum MATCH_SCENE_SERVER_NET_MESSAGE_KEYACTION
{
	/////////////////////////////////////客户端到场景服/////////////////////////////////////
    SC_MATCH_MSG_MATCH = SC_MSG_MATCH_START,

    /////////////////////////////////匹配模块相关消息/////////////////////////////////////////
	MSG_MATCH_DATE_MODULEMSG,						// 匹配模块主消息  子消息：EMMatchDataSubMsgCode

    ////////////////////////////////////桥服务器消息//////////////////////////////////////
	MSG_MATCH_BRIDGE_DATA,						// 匹配跨区桥消息

    //////////////////////////////////房间模块相关主消息////////////////////////////////////////
    MSG_MATCH_ROOM_MODULEMSG,                       // 房间模块主消息  子消息：EMRoomMessageCode

    ////////////////////////////////////模块主消息//////////////////////////////////////
    MSG_MATCH_SERVICE_MODULEMSG,                        

	C_2_MATCH_SCENE_SERVER_NET_MESSAGE_KEYACTIONs,
};

enum EMMATCHMessageCode
{
    /////////////////////////////////房间相关/////////////////////////////////////////

    CS_MSG_ENTER_HALL,	// 进入大厅

    CS_MSG_CREATE_ROOM,						// 创建房间

    CS_MSG_ENTER_ROOM,						// 进入房间

    CS_MSG_START_GAME,						// 开始游戏

    CS_MSG_PRE_SELECT_HERO,					// 等待房间预选英雄

    CS_MSG_SELECT_HERO,						// 选英雄

    CS_MSG_READY_TO_FIGHT,					// 结束选择英雄

    CS_MSG_POS_CHANGE,						// 位置改变

    CS_MSG_EXIT_ROOM,						// 离开房间

    CS_MSG_KICK_OUT_PLAYER,					// 踢人

    CS_MSG_EXIT_HALL,						// 离开大厅

    CS_MSG_CONFIRM_SELECT_HERO,				// 确定所选英雄 

    CS_MSG_EXIT_WAR,						// 离开战场

    CS_MSG_SUMMONER_SKILL,                  // 发送召唤师技能

	CS_MSG_SELECT_HERO_SKIN,				// 选英雄皮肤

    CS_MSG_UPDATE_GUIDE_INFO,				// 更新引导信息

    CS_MSG_MASTER_COVER_COMPUTER,			// 添加电脑开始游戏

    SO_MSG_END,								// 结束

    SO_MSG_BROAD_SUMMONER,                   // 广播召唤师技能

    SO_MSG_ALL_MIGRATE_FINISH,				// 所有人员迁移结束

    SO_MSG_MIGRATE_OUT_BEGIN,				// 战场结束迁出开始

    SO_MSG_WAR_PROCESS_END,                 // 战场流程结束 并未释放

    SO_MSG_SET_TALENT_AND_RUNE,             // 设置当前所选天赋符文页

    SC_MSG_LOAD_SCENE,						// 加载场景	

    SC_MSG_ENTER_SCENE,						// 进入场景

    OS_MSG_MATCH_CREATE_MOBA,					// 创建MOBA战场
	
    OS_MSG_MATCH_CREATE_MOBA_ROLE_DATA,             // 创建战场玩家数据

    OS_MSG_MATCH_CREATE_MOBA_OB_ROLE_DATA,          // 创建战场OB玩家数据

    SO_MSG_MATCH_CREATE_MOBA_RESULT,			// 场景服反馈创建结果

	OS_MSG_MATCH_ROOM_EX_DATA,					// 房间扩展数据

    // 进入大厅结果
    OC_MSG_ENTER_HALL,		

    OC_MSG_LOAD_SCENE,						// 加载场景	

    OC_MSG_ROOM,								// 单个房间数据

    OC_MSG_CREATE_ROOM,						// 创建房间结果

    OC_MSG_ENTER_ROOM,						// 进入房间结果

    OC_MSG_OB_ENTER_ROOM,						// 进入房间结果

    OC_MSG_RETURN_TO_ROOM,						// 重新回到房间

    OC_MSG_ROOM_ROLE,							// 房间角色

    OC_MSG_ROOM_OB_ROLE,						// 房间OB角色信息

    OC_MSG_ROOM_ROLE_COUNT,					// 房间人数

    OC_MSG_ROOM_START_SELECT_HERO,			// 开始选英雄

    OC_MSG_OUTSIDE_ROOM_STATE,				// 房间外状态改变

    OC_MSG_INSIDE_ROOM_STATE,					// 房间内状态改变

    OC_MSG_ROOM_READY_TO_FIGHT,				    // 准备战斗了

    OC_MSG_ROOM_START_GAME_FAIL,				// 开始游戏失败	

    OC_MSG_HERO_CHANGE,						// 英雄改变

    OC_MSG_POS_CHANGE,						// 位置改变

    OC_MSG_OB_POS_CHANGE,                   // OB有位置改变了

    OC_MSG_MASTER_CHANGE,						// 房主改变

    OC_MSG_EXIT_ROOM,							// 离开房间

    OC_MSG_RELEASE_ROOM,						// 释放房间

    //OC_MSG_HALL_ROOM,						// 大厅房间同步

    OC_MSG_EXIT_ROOM_HALL_ROOM,				// 离开房间后获得的大厅房间数据

    OC_MSG_KICK_OUT_PLAYER,					// 踢人

    OC_MSG_KICK_OUT_ROOM_HALL_ROOM,			// 被踢到大厅后获得的大厅房间数据

    OC_MSG_EXIT_HALL,							// 离开大厅 

    OC_MSG_ALL_CONFIRM_HERO,					// 玩家都确认英雄了 开始倒计时开始 

    OC_MSG_CONFIRM_HERO,						// 玩家确认英雄 

    OC_MSG_EXIT_WAR_TO_HALL,					// 通知房间 退出战斗返回大厅

    OC_MSG_EXIT_WAR_TO_GAME,					// 通知房间 退出战斗静态地图

    OC_MSG_ROOM_RULE_BAN_HERO,				    // 房间规则：禁用英雄

    OC_MSG_ROOM_RULE_SELECT_HERO,				// 房间规则：选择英雄

    OC_MSG_BAN_HERO_LIST,						// 禁用英雄列表

    OC_MSG_START_ROOM_RULE,					    // 开始房间规则

    OC_MSG_HERO_CAN_SELECT,					    // 能否选择英雄

    OC_MSG_GLOBLE_BAN_HERO_LIST,				// 禁用英雄

    SC_MSG_ROOM_SUMMONER_SKILL_INROOM,        // 房间内广播召唤师技能

    CS_MSG_ROOM_COMPUTERPLAYER_ADD,              // 添加假人消息

    CS_MSG_ROOM_COMPUTERPLAYER_REMOVE,           // 移除假人

    CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE,       // 假人改变英雄

    CS_MSG_ROOM_COMPUTERPLAYER_CONFIRM_HERO,     // 确认假人选择英雄

    OC_MSG_COMPUTER_HERO_CHANGE,				 // 假人英雄改变

    OC_MSG_COMPUTER_HERO_CAN_SELECT,             // 假人能否选择英雄

    OC_MSG_GLOBLE_COMPUTER_BAN_HERO_LIST,       // 禁用英雄

    CS_MSG_ROOM_COMPUTERPLAYER_HEROAICHANGE,    // 假人改变英雄AI

    OC_MSG_COMPUTER_HEROAI_CHANGE,              // 假人英雄AI改变

    OS_MSG_MATCH_REQ_PLAYER_BANKINFO,           // 请求房间序列化数据

    SO_MSG_MATCH_RET_PLAYER_BANKINFO,           // 回复房间请求序列化数据

	OC_MSG_MATCH_LEGENDCUP_WAIT_COUNTDOWN,		// 线上杯赛房间等待倒计时 

	OC_MSG_HERO_SKIN_CHANGE,					// 英雄皮肤改变

	OC_MSG_EXIT_HALL_FROM_LEGENDCUPROOM,		// 从杯赛房间离开大厅

    OC_MSG_EXIT_HALL_FROM_MATCHROOM,			// 从撮合房间离开

    SO_MSG_MATCH_REMOVE_ACTOR_ROOMINFO,         // 删除社会服玩家房间信息

    OC_MSG_CHANGE_AI_ID_SUCCESS,                // 通知客户端改变AI成功

    OC_MSG_SYN_ROOM_ALL_XP_SKILLID,				// 同步房间所有玩家XP技能

    OC_MSG_SEND_ROOM_CAMP_COMPUTER_BANHEROS,		// 同步房间同阵营电脑所有禁止英雄

    CS_MSG_CREATE_GUIDE_ROOM,						// 创建引导1房间

    OC_MSG_ROOM_CLIENT_MSSAGE_INFO,				// 房间提示信息数据

    OC_MSG_ENTER_ROOM_OHTER_PRE_SELECT,			// 玩家进入房间其他玩家预选信息

    CS_MSG_MATCH_CUSTOMER_INVITE_RESPON,        // 自定义比赛邀请响应

    OC_MSG_BAN_HERO_COUNT_DOWN,			        // ban选倒计时

    OC_MSG_POS_BAN_HERO_FLAG,			        // 可以ban选英雄标志

    CS_MSG_POS_BAN_HERO,			            // ban选英雄

    OC_MSG_POS_BROAD_SINGLE_BAN_HERO,			// 广播ban选英雄

    OC_MSG_POS_BROAD_ALL_BAN_HERO,			    // 广播所有ban选英雄

    OC_MSG_POS_ALL_ALREADY_BAN_HERO,			// 已经ban选的英雄下发

    OC_MSG_LEGENDCUP_WIN_MSG_INFO,				// 杯赛房间直接胜利消息

    OS_MSG_MATCH_ROOM_BANHERO_DATA,			    // 房间ban选数据

    OC_MSG_BROADCAST_ROOM_HERO_WIN_DATA,		// 玩家英雄胜率数据下发

    /////////////////////////////////撮合相关/////////////////////////////////////////

    CS_MSG_MATCH_REQUEST_ADDMATCH,		        // 请求确定加入撮合

    OC_MSG_MATCH_BROAD_WAITACCEPT,		        // 发送撮合确定是否参赛

    OC_MSG_MATCH_BROAD_OTHER_ACCEPTMATCH,       // 广播其他玩家确定比赛

    CS_MSG_MATCH_ACCEPT_MATCH,                  // 确定加入比赛

    CS_MSG_MATCH_REFUSE_MATCH,                  // 拒绝加入比赛

    OS_MSG_MATCH_SCENE_CHECKDATA,               // 请求场景服校验

    OS_MSG_MATCH_RESULT,                        // 撮合结果 EMMatchStrikeResultFlag

    SC_MSG_IS_MATCHING,                         // 进入撮合

    SC_MSG_CANCEL_MATCH,                        // 取消撮合

    SC_MSG_MATCH_OK,                            // 撮合成功

    SC_MSG_MATCH_FAIL,                          // 撮合失败

    CS_MSG_MATCH_CANCEL_MATCH,                  // 取消撮合

    SC_MSG_MATCH_REFUSE,                        // 拒绝参赛

    OC_MSG_MATCH_BRIDGE_MESSAGE,                // 转发消息信息

    OS_MSG_SYN_PLAYER_ROOM_STATE,               // 同步玩家房间状态

	OC_MSG_MATCH_RECOMMEND_RANK,				// 推送撮合信息

	CS_MSG_MATCH_RECOMMEND_RESULT,				// 推送撮合信息结果

    OC_MSG_MATCH_CUR_MATCHPLAYER_COUT,		    // 发送撮合确定是否参赛

    OS_MSG_MATCH_AUTO_ACCEPT_MATCH,		        // 系统自动接收比赛

    //////////////////////////////////模块主消息////////////////////////////////////////

    MSG_MATCHROOM_DATARESTORE,                  // 复原场景服数据相关

    MSG_MATCH_ACTOR_LOGIN_OK,                   // 登陆成功事件

    MSG_MATCH_UPDATE_HERO_CANUSE,               // 更新可以使用的英雄

    SO_MSG_MATCH_PUNISH_ACTOR,                     // 惩罚玩家

    OC_MSG_MATCH_SLEF_PUNISH_TIMEINFO,               // 惩罚信息相关

    SC_MSG_MATCH_REQUEST_PUNISH_TIMEINFO,          // 请求节点所有惩罚信息相关

    CS_MSG_MATCH_ACTOR_VIEW_LOADED,                 // 客户端加载完成_CS

    SS_MSG_MATCH_ACTOR_VIEW_LOADED,                 // 客户端加载完成_SS

    OC_MSG_MATCH_DEL_SLEF_PUNISH_TIMEINFO,          // 删除某一类惩罚信息


    SS_MSG_MATCH_OPEN_OR_CLOSE,                     // 广播撮合开启或者关闭
    OC_MSG_MATCH_OPEN_OR_CLOSE,                     // 广播撮合开启或者关闭

    SO_MSG_MATCH_PUNISH_SUB_OBSERVE,                // 减少观察期

    SS_MSG_WAR_END_TESTER_EXIT_WAR,						        // 战场结束压力测试玩家离开

    OS_MSG_MATCH_ROOM_DESTORY_ACTOR,				// 通知场景服移除玩家实体
    
	OS_MSG_MATCH_SEASON_BEGIN,						// 赛季开始

	OS_MSG_MATCH_SEASON_END,						// 赛季结束

    MATCH_MSG_MAX,
};
// SO_MSG_MATCH_PUNISH_SUB_OBSERVE
struct SMsgPunishSubObserve_SO
{
    DWORD   dwFromWorldID;
    PDBID   dwActorID;

    SMsgPunishSubObserve_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SS_MSG_MATCH_OPEN_OR_CLOSE
//OC_MSG_MATCH_OPEN_OR_CLOSE
struct SMsgMatchOpenOrClose
{
    bool bActorLogin;
    bool bOpen;
    int  nMatchType;
    int  nDesID;

    SMsgMatchOpenOrClose()
    {
        memset(this, 0, sizeof(*this));
    }
};

// OC_MSG_POS_BROAD_SINGLE_BAN_HERO
// OC_MSG_POS_BROAD_ALL_BAN_HERO
struct SMsgBroadBanHead_OC
{
    int nCount;
    int nCampMaxPlayer;     // 根据这个可以判定哪个阵营

    SMsgBroadBanHead_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgBroadBanSub_OC
{
    int    nPos;      // 位置
    int    nHeroID;   // ban选英雄

    SMsgBroadBanSub_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// CS_MSG_POS_BAN_HERO
struct SMsgPosBanHero_CS
{
    DWORD  dwActorID;      // ban选的英雄ID
    int    nHeroID;       // ban选英雄

    SMsgPosBanHero_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

//OC_MSG_MATCH_DEL_SLEF_PUNISH_TIMEINFO
struct SMsgDelPunishInfo_OC
{
    BYTE    byPunishType;

    SMsgDelPunishInfo_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SO_MSG_MATCH_PUNISH_ACTOR
struct SMsgPunishActor_SO
{
    bool    bAdd;
    DWORD   dwFromWorldID;
    PDBID   dwActorID;
    BYTE    byPunishType;
    int     nMatchTypeID;

    SMsgPunishActor_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SC_MSG_MATCH_REQUEST_PUNISH_TIMEINFO
struct SMsgRequestPunishInfoHead_OC
{
    // 数量
    bool bIsTeamRequest;
    int nCount;

    SMsgRequestPunishInfoHead_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgRequestPunishInfo_OC
{
    // 数量
    EPunishmentType    ePunishType;
    DWORD	           dwLeftTime;
    char               szName[GAME_NAME_MAXSIZE];

    SMsgRequestPunishInfo_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

//OC_MSG_MATCH_SLEF_PUNISH_TIMEINFO
struct SMsgSelfPunishInfoHead_OC
{
    // 数量
    int nCount;

    SMsgSelfPunishInfoHead_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgSelfPunishInfo_OC
{
    // 数量
    EPunishmentType    ePunishType;
    DWORD	           dwLeftTime;
   
    SMsgSelfPunishInfo_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};


// OC_MSG_SEND_ROOM_CAMP_COMPUTER_BANHEROS
struct SMsgCampComputerBanHeros
{
    // 数量
    int nCamp;
    int	nCount;

    // nCount * size(int);
    SMsgCampComputerBanHeros()
    {
        memset(this, 0, sizeof(*this));
    }
};



// OC_MSG_SYN_ROOM_ALL_XP_SKILLID
struct SMsgSynXPSkillHead_OC
{
    // 数量
    int	nCount;

    SMsgSynXPSkillHead_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgSynXPSkillInfo_OC
{
   
    DWORD dwActorID; // 玩家ID
    int   nHeroXPSkillID;// 玩家XP技能ID

    SMsgSynXPSkillInfo_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// OS_MSG_MATCH_REQ_PLAYER_BANKINFO
struct SMsgMatchReqPlayerBankInfo_OS
{
    DWORD   dwActorID;                    // 玩家ID
    int     nHeroID;

    SMsgMatchReqPlayerBankInfo_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SO_MSG_MATCH_RET_PLAYER_BANKINFO
struct SMsgMatchRetPlayerBankInfo_SO
{
    DWORD   dwActorID;                    // 玩家ID
    int     nHeroID;
    int     nHeroXPID;

    SMsgMatchRetPlayerBankInfo_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};

//OC_MSG_CHANGE_AI_ID_SUCCESS,					// 通知客户端改变AI成功
struct SMsgChangeAISuccess_SO
{
    DWORD   dwComputerID;                    // 玩家ID
    int     nAIID;

    SMsgChangeAISuccess_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SO_MSG_MATCH_REMOVE_ACTOR_ROOMINFO,					// 场景服反馈创建结果
struct SMsgRemoveActorRoomInfo_SO
{
    DWORD	dwRoomID;							// 房间ID
    DWORD   dwActorID;                    // 玩家ID
    bool    bIsOBPlayer;                    // 是否是OB

    SMsgRemoveActorRoomInfo_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};

//////////////////////////////////////////////////////////////////////////

// 消息定义
//////////////////////////////////////////////////////////////////////////

// OC_MSG_ROOM_ROLE		 房间角色
//struct SRoomRoleBase

// 消息定义
//////////////////////////////////////////////////////////////////////////
//MS2MSS_MSG_REMOVE_PLAYER;
struct SMsg_MS2MSS_Remove_Player
{
	PDBID			idActor;
	SMsg_MS2MSS_Remove_Player()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgCreateRoom_OC
{
    DWORD   dwMatchTypeID;                      // 撮合类型ID
	DWORD	dwRoomID;							// 房间ID
	DWORD	dwMapID;							// 地图ID
	BYTE	nModeID;							// 模式的类型 EGameMode
	int		nConfigWarID;						// scheme配置WarID
	BYTE	byState;							// 房间状态
	BYTE	byRoleCount;						// 角色数量
    BYTE	byOBRoleCount;						// OB角色数量
	char	szRoomName[GAME_NAME_MAXSIZE];		// 房间名字
	char	szCreateName[GAME_NAME_MAXSIZE];	// 创建者名字
    bool    bIsMatchRoom;                       // 是否是撮合房间
    bool    bHavePassword;                      // 是否有密码
	BYTE	nRoomType;							// 房间类型 EMHallRoomType
	bool	bOnlineLegendCup;					// 是否是线上杯赛
    bool    bGuidRoom;                          // 是否是引导房间
    DWORD   dwAvgRank;
    bool    bPermitOB;                          // 能否观战

	SMsgCreateRoom_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgDestoryActor_OS
{
    DWORD	dwRoomID;							// 房间ID
    DWORD   dwActorID;                          // 玩家ID
    SMsgDestoryActor_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgCreateRoomRoleData
{
    DWORD	dwRoomID;							// 房间ID
    int     nBufSize;
    bool    bIsEnd;
    
    SMsgCreateRoomRoleData()
    {
        memset(this, 0, sizeof(*this));
    }
};

//SO_MSG_MATCH_CREATE_MOBA_RESULT,					// 场景服反馈创建结果
struct SMsgCreateMobaResult_SO
{
	DWORD	dwRoomID;							// 房间ID
    int     nBattleFieldID;                    // 战场ID
    int     nSceneID;                           // 场景ID
    int     nSerialNo;                      // 场景序号
	bool	bResult;							// 结果

	SMsgCreateMobaResult_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};


struct SMsgMatchCheck_OS
{
    DWORD	dwRequestNodeID;					// 收集信息查找对应ID
    int     nMatchTypeID;                       // 撮合类型ID
    int     dwCheckActorID;                     // 玩家ID

    SMsgMatchCheck_OS()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgMatchCheckResult_SO
{
	DWORD	dwRequestNodeID;							// 收集信息查找对应ID
    int     nMatchTypeID;                               // 撮合类型ID
    DWORD   dwActorID;                                  // 玩家ID
	bool	bResult;							        // 结果
    int     nSelfFailedDesID;                            // 自己提示失败信息
    int     nOtherFailedDesID;                           // 其他人提示失败信息
    char	szName[GAME_NAME_MAXSIZE];	                // 名字

	SMsgMatchCheckResult_SO()
	{
		memset(this, 0, sizeof(*this));
	}
};

// OS_MSG_MATCH_ROOM_EX_DATA
struct SMsgMatchRoomExData
{
	DWORD dwRoomID;
	BYTE  nRoomType;			// 根据房间类型 填充不同的数据

	SMsgMatchRoomExData()
	{
		memset(this, 0, sizeof(*this));
	}
};

// OS_MSG_MATCH_ROOM_EX_DATA
struct SMsgMatchRoomBanHeroData
{
    DWORD dwRoomID;
    BYTE  nRoomType;			// 根据房间类型 填充不同的数据
    int nCount;                 // 数量
    SMsgMatchRoomBanHeroData()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgMatchRoomBanInfo
{
    int nCamp;                  // 阵营
    int nBanHeroID;             // 英雄ID

	SMsgMatchRoomBanInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgLegendCupRoomInfo
{
    bool            bIsOnlineCup;                       // 是否是线上杯赛
	DWORD           dwWorldID;                          // 来自于哪个世界的ID
	LONGLONG        dwLedendCupID;                      // 杯赛ID
	int             nSerchID;                           // 比赛搜索ID
	int				nRoundID;							// 比赛第几轮
    int             nBoCount;                           // 第几场
	DWORD           dwKin1;                             // 参赛队伍1
	DWORD           dwKin2;                             // 参赛队伍2
	uint32			nKin1Camp;							// 队伍1阵容
	uint32			nKin2Camp;							// 队伍2阵容
    BYTE            eRoundType;                         // 本轮类型 淘汰 or 小组赛ERoundCompetitionType
    int             nMaxRound;                          // 最大多少轮
    BYTE            byBoType;                           // bo类型
    int             nNodeKin1Score;                     // Kin1得分
    int             nNodeKin2Score;                     // Kin2得分
    DWORD           dwWarInterval;                      // 节点未结束下一场比赛时间间隔
    bool            bKin1RoundLastNode;                 // 是否本轮最后节点
    DWORD           dwKin1NodeEndNextWarTime;           // 节点若结束下一场比赛的时间

    bool            bKin2RoundLastNode;                 // 是否本轮最后节点
    DWORD           dwKin2NodeEndNextWarTime;           // 节点若结束下一场比赛的时间
	SMsgLegendCupRoomInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

//SC_MSG_LOAD_SCENE;
struct SMsgLoadScene_SC
{
	DWORD	dwMapID;		// 游戏场景地图ID
	SMsgLoadScene_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


//SC_MSG_ENTER_SCENE;
struct SMsgEnterScene
{
	// 游戏的场景ID
	DWORD		m_SceneID;	
	SMsgEnterScene()
	{
		memset(this, 0, sizeof(*this));
	}
};

//SO_MSG_END;
struct SMsgEnd
{
	DWORD		m_RoomID;
	SMsgEnd()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgWar_Process_End
{
    DWORD		m_RoomID;
    SMsgWar_Process_End()
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_ENTER_HALL;
struct SMsgEnterHall_CS
{
    ClientID clientID;                              // 角色ID
    DWORD   dwSrcScene;                             // 来源场景ID
    DWORD   dwSrcWorldID;                           // 来源世界ID
    PDBID	idActor;	                            // 进入大厅人物PDBID
    DWORD	dwHeroID;                               // 选的英雄ID
    char	szCreateName[GAME_NAME_MAXSIZE];	    // 玩家名字
    BYTE    bySex;                                  // 性别
    int     nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能
    int     nTalentEffectPageIndex;                 // 当前选择天赋页
    int     nRuneEffectPageIndex;                   // 当前选择符文页
    SMatchPlayerRoomExData sExData;                 // 附加数据

    SMsgEnterHall_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_CREATE_ROOM;
struct SMsgCreateRoom_CS
{
	PDBID	idActor;          // 角色ID
    DWORD   dwMatchType;     
    DWORD   dwGuideID;       // 引导ID
	// 房间名字
	char		szRoomName[GAME_NAME_MAXSIZE];
    char		szPassword[GAME_ROOMPWD_MAXSIZE];
    bool        bHavePassword;
	bool	    bPermitOb;

	SMsgCreateRoom_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};



//CS_MSG_CREATE_ROOM;
struct SMsgCreateGuideRoom
{
    PDBID	idActor;          // 角色ID
    DWORD   dwMatchType;     
    DWORD   dwGuideID;       // 引导ID
    int     nGuideHeroID;   // 引导英雄ID
    // 房间名字
    char	szRoomName[GAME_NAME_MAXSIZE];
    char	szPassword[GAME_ROOMPWD_MAXSIZE];
    bool    bHavePassword;

    DWORD   dwSrcScene;                             // 来源场景ID
    DWORD   dwSrcWorldID;                           // 来源世界ID
    DWORD	dwHeroID;                               // 选的英雄ID
    BYTE    bySex;                                  // 性别
    char	szCreateName[GAME_NAME_MAXSIZE];	    // 创建者名字
    int     nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能
    int     nTalentEffectPageIndex;                 // 当前选择天赋页
    int     nRuneEffectPageIndex;                   // 当前选择符文页
    SMatchPlayerRoomExData sExData;                 // 附加数据

    SMsgCreateGuideRoom()
    {
        memset(this, 0, sizeof(*this));
    }
};

//CS_MSG_CREATE_ROOM;
struct SMsg_UpdateGuideInfo
{
    
    int nGuidID;

    SMsg_UpdateGuideInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};


//CS_MSG_ENTER_ROOM;
struct SMsgEnterRoom
{
    PDBID	idActor;          // 角色ID
	DWORD	dwRoomID;
    char	szPassword[GAME_ROOMPWD_MAXSIZE];
    bool    bOBEnter;
    uchar   uOBPos;

	SMsgEnterRoom()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_START_GAME;
struct SMsgStartGame
{
	PDBID	idActor;
	SMsgStartGame()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_EXIT_ROOM;
struct SMsgExitRoom
{
	PDBID	idActor;
    bool    bIsOBPos;

	SMsgExitRoom()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_EXIT_HALL

struct SMsgExitHall
{
	PDBID	idActor;

	SMsgExitHall()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_CONFIRM_SELECT_HERO
struct SMsgConfirmSelectHero
{
	PDBID	idActor;

	SMsgConfirmSelectHero()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_SELECT_HERO;
struct SMsgSelectHero
{
	PDBID	idActor;
	// 选的英雄ID
	int	    nHeroID;
 
    // 压力测试玩家
    bool    bIsTestPlayer;

	SMsgSelectHero()
	{
		memset(this, 0, sizeof(*this));
	}
};



//CS_MSG_READY_TO_FIGHT
struct SMsgReadyToFight
{
	PDBID	idActor;
	bool	bIsReady;

	SMsgReadyToFight()
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_SELECT_HERO_SKIN;
struct SMsgSelectHeroSkin
{
	PDBID	idActor;
	// 选的英雄ID
	DWORD	dwHeroID;
	// 英雄皮肤ID
	DWORD   dwSkinID;

	SMsgSelectHeroSkin()
	{
		memset(this, 0, sizeof(*this));
	}
};


// CS_MSG_POS_CHANGE
struct SMsgPosChange
{
    bool    bSelfIsOBPos;
	PDBID	idActor;
	// 想要去的位置
    bool    bDesIsOBPos;
	uchar	byPos;

	SMsgPosChange()
	{
		memset(this, 0, sizeof(*this));
	}
};


// CS_MSG_KICK_OUT_PLAYER
struct SMsgKickOutPlayer
{
	PDBID	idActor;
	// 位置
	uchar	byPos;

	int		nIsOb;

	SMsgKickOutPlayer()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_ENTER_HALL;
struct SMsgEnterHall_OC
{
	// 是否成功进入
	bool		m_bOK;
	SMsgEnterHall_OC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// OC_MSG_HALL_ROOM
struct SMsgHallRoom
{
	// 后面带的房间个数
	unsigned int	m_RoomCount;

	SMsgHallRoom()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_CREATE_ROOM;// 之前会发房间数据 收到这个就直接进房间
struct SMsgCreateRoom
{
	// 结果
	bool m_Result;

    // 召唤师技能id
    int m_SkillId[SUMMONER_SKILL_COUNT];

	SMsgCreateRoom()
	{
		memset(this, 0, sizeof(*this));
	}
};


//OC_MSG_ENTER_ROOM;

struct SMsgSelfEnterRoomData
{
    // 比赛类型
    DWORD       m_dwMatchTypeID;

    bool        m_bIsOBEnter;
    // 结果
    bool        m_Result;
    // 房间ID
	DWORD	    m_RoomID;
	// 房主位置
	uchar		m_MasterPos;
	// 自己位置
	uchar		m_SelfPos;
    // 自己的角色ID
    PDBID       m_PDBID;
    // 召唤师技能
    int         m_SummonerSKill[SUMMONER_SKILL_COUNT];
	// 有几个人
	uchar       m_RoleCount;

    // 有几个OB
    uchar       m_OBRoleCount;

    int         m_SelfHeroID;

    BYTE        m_bySex;

    char        m_szName[GAME_NAME_MAXSIZE];

    int         m_nHeroXPSkill;

	SMsgSelfEnterRoomData()
	{
		memset(this, 0, sizeof(*this));
	}
};

// OC_MSG_ROOM_ROLE_COUNT
struct SMsgRoomRoleCount
{
	DWORD	m_RoomID;
	// 有几个人
	uchar m_RoleCount;
};


// OC_MSG_ROOM_STATE   房间状态改变
struct msg_change_roomState
{
	DWORD	    dwRoomID;           // 房间ID
	BYTE		nState;             // 状态 EMatchRoomState
};


// OC_MSG_HERO_CHANGE
struct SMsgHeroChange
{
	// 位置
	uchar	Pos;
	// 英雄ID
	DWORD	dwHeroID;
    // 英雄星级
    int     nHeroStarLv;
    // 英雄等级
    int     nHeroStaticLv;
	// 是否服务器随机
	bool	bIsServerRand;
};


// OC_MSG_POS_CHANGE
struct SMsgPosChange_OC
{
	// 位置
	uchar		m_Pos[2];
};

// OC_MSG_OB_POS_CHANGE
struct SMsgOBPosChange_OC
{
    bool    bFromIsOBPos;
    uchar   uFromPos;

    bool    bDesIsOBpos;
    uchar   uDesPos;
};

// OC_MSG_MASTER_CHANGE
struct SMsgMasterChange
{
	// 位置
	uchar		m_Pos;
};


// OC_MSG_EXIT_ROOM
struct SMsgExitRoom_OC
{
    bool        m_OBPos;
	// 位置
	uchar		m_Pos;
};

// OC_MSG_RELEASE_ROOM
struct SMsgReleaseRoom
{
	DWORD	m_RoomID;
};

// OC_MSG_KICK_OUT_PLAYER
struct SMsgKickOutPlayer_OC
{
	// 位置
	uchar		m_Pos;

	int			m_nIsOb;
	SMsgKickOutPlayer_OC()
    {
        memset(this, 0, sizeof(*this));
    }
};

// OC_MSG_CONFIRM_HERO
struct SOC_MSG_CONFIRM_HERO
{
	// 位置
	uchar		m_Pos;
};

struct SMsgRulePos
{
	// 可以禁用人的位置
	uchar		m_Pos;
	SMsgRulePos()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_ROOM_RULE_BAN_HERO,				// 房间规则：禁用英雄
struct SMsgRuleBanHero
{
	// 流程ID
	int			nStepID;
	//状态
	int			nRuleType;
	
	// 可禁用英雄位置数量
	int			nCanBanPosCount;

	// 流程时间
	int			nTimeInterval;

	// 可以禁用英雄的位置 
	// SMsgRulePos*nCanSelectCount
	SMsgRuleBanHero()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_ROOM_RULE_SELECT_HERO,			// 房间规则：选择英雄
struct SMsgRuleSelectHero
{
	// 流程ID
	int			nStepID;
	//状态
	int			nRuleType;

	// 可选人位置数量
	int			nCanSelectCount;

	// 流程时间
	int			nTimeInterval;

	// 可以选人的位置 
	// SMsgRulePos*nCanSelectCount

	SMsgRuleSelectHero()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_ROOM_RULE_SELECT_HERO,			// 房间规则：选择英雄
struct SMsgRuleBanHeroList
{
	// 数量
	int			nCount;

	// 办人列表 
	// int *nCanSelectCount

	SMsgRuleBanHeroList()
	{
		memset(this, 0, sizeof(*this));
	}
};

// OC_MSG_HERO_CAN_SELECT
struct SMsgCanSelect
{
	// 位置
	bool	bCanSelect;
	// 英雄ID
	DWORD	m_HeroID;

	SMsgCanSelect()
	{
		memset(this, 0, sizeof(*this));
	}
};

//OC_MSG_GLOBLE_BAN_HERO_LIST,			// 房间规则：选择英雄
struct SMsgGlobleBanHeroList
{
	// 数量
	int			nCount;

	// 办人列表 
	// int HeroID;

	SMsgGlobleBanHeroList()
	{
		memset(this, 0, sizeof(*this));
	}
};


struct SMsgSummoner_Skill
{
    PDBID   pDbid;                                  // 玩家pdbid
    int     nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能
    SMsgSummoner_Skill()
    {
        memset( this, 0, sizeof(*this) );
    }
};

//CS_MSG_ROOM_COMPUTERPLAYER_ADD,              // 添加假人消息
struct SMsg_ComputerPlayerAdd
{
    PDBID   idActor;                                  // 添加假人pdbid
    int     nPos;                                   // 假人所在房间位置
    DWORD   dwSrcScene ;                            // 来源场景（同房主）
    DWORD   dwSrcWorldID;                           // 来源世界（同房主）
    SMsg_ComputerPlayerAdd()
    {
        memset( this, 0, sizeof(*this) );
    }
};

//CS_MSG_ROOM_COMPUTERPLAYER_REMOVE,           // 移除假人
struct SMsg_ComputerPlayerRemove
{
    PDBID   idActor;                               // 添加假人pdbid
    int     nPos;                                // 移除所在房间位置
    SMsg_ComputerPlayerRemove()
    {
        memset( this, 0, sizeof(*this) );
    }
};

//CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE,       // 假人改变英雄
struct SMsg_ComputerPlayerHeroChange
{
    PDBID   idActor;                                  // 添加假人pdbid
    int     nPos;                                   // 改变房间位置
    int     nHeroID;                                // 改变的英雄ID
    SMsg_ComputerPlayerHeroChange()
    {
        memset( this, 0, sizeof(*this) );
    }
};

//CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE,       // 假人改变英雄
struct SMsg_ComputerPlayerHeroAIChange
{
    PDBID   idActor;                                  // 添加假人pdbid
    int     nPos;                                   // 改变房间位置
    int     nHeroID;                                // 英雄ID
    int     nAIID;                                  // 改变的AIID
    SMsg_ComputerPlayerHeroAIChange()
    {
        memset( this, 0, sizeof(*this) );
    }
};


struct SMsgComputerHeroChange
{
    // 位置
    uchar		m_Pos;
    // 英雄ID
    DWORD	    m_HeroID;
    // 默认AIID
    int	        m_AIID;

    char		strPlayerName[GAME_NAME_MAXSIZE];

	// Xp技能
	int			m_XpID;

    SMsgComputerHeroChange()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgComputerHeroAIChange
{
    // 位置
    uchar		m_Pos;
    // 英雄ID
    DWORD	    m_HeroID;
    // AIID
    int	        m_AIID;

    SMsgComputerHeroAIChange()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgComputerCanSelect
{
    int     nCamp;
    // 位置
    bool	bCanSelect;
    // 英雄ID
    DWORD	m_HeroID;

    SMsgComputerCanSelect()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgSetTalnetAndRune 
{
    PDBID   PlayerID;
    int     nTalentEffectPageIndex;                 // 当前选择天赋页
    int     nRuneEffectPageIndex;                   // 当前选择符文页

    SMsgSetTalnetAndRune()
    {
        memset(this, 0, sizeof(*this));
    }
};

//OC_MSG_MATCH_LEGENDCUP_WAIT_COUNTDOWN
struct SMsgLegendCup_WaitCountdown
{
	DWORD dwRoomID;
	int nCountdown;

	SMsgLegendCup_WaitCountdown()
	{
		memset(this, 0, sizeof(*this));
	}
};

// OC_MSG_HERO_SKIN_CHANGE
struct SMsgHeroSkinChange
{
	// 位置
	uchar	Pos;
	// 英雄ID
	DWORD	dwHeroID;
	// 英雄皮肤ID
	DWORD	dwSkinID;
};

///////////////////////////////////////////////////////////////////

struct SMsgMatchRoomDataRestoreHead
{
    int	    nCount;			// 房间数量
    bool    bHaveSendAll;   // 是否全部发送完毕
    DWORD   dwTotalPlayerCount; // 当前存储的数量
    //nCount*SMsgMatchRoomDataRestore
    //SMsgMatchRoomDataRestore

    SMsgMatchRoomDataRestoreHead(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgMatchRoomDataRestore
{
    DWORD   dwMatchTypeID;                      // 撮合类型ID
    DWORD	dwRoomID;							// 房间ID
    DWORD	dwMapID;							// 地图ID
    BYTE	nModeID;							// 模式的类型 EGameMode
    int		nConfigWarID;						// scheme配置WarID
    BYTE	byRoleCount;						// 角色数量
    bool    bIsMatchRoom;                       // 是否是撮合房间
    BYTE	nRoomType;							// 房间类型 EMHallRoomType
    bool	bOnlineLegendCup;					// 是否是线上杯赛
    bool    bGuidRoom;                          // 是否是引导房间
    DWORD   dwPlayerCount;                      // 玩家数量
    int     nSceneID;
    int     nSceneSerialNo;

    
    // 玩家列表
    SMsgMatchRoomDataRestore(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgMatchRoomPersonExData
{
    
    DWORD   dwActorID;              // 玩家ID
    SMatchPlayerRoomExData ExData;  // 附加数据

    SMsgMatchRoomPersonExData(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsgRoomPlayerBankData
{

    DWORD   dwActorID;              // 玩家ID
    int     nBufLen;
    // bankData

    SMsgRoomPlayerBankData(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct	SMsgSetWarResultData
{
	int nWarID;
	LONGLONG llDBWarID; 
	int nFailedCamp;
	int nFailedKDA;
	int nWinKDA;
	int nWinGlory[emLegendGlory_Max]; 
	int nFailGlory[emLegendGlory_Max]; 
	int nGloryLen;
	DWORD dwBeginTime;

	SMsgSetWarResultData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//CS_MSG_MATCH_CUSTOMER_INVITE_RESPON
struct SMsg_MasterInviteRespon_CS
{
    DWORD dwRoomID;
    DWORD dwInviteActorID;
    char	szInviteName[GAME_NAME_MAXSIZE];	    // 邀请者名字

    SMsg_MasterInviteRespon_CS()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsg_MasterInviteRespon_SO
{
    DWORD dwRoomID;
    char	szInviteName[GAME_NAME_MAXSIZE];	    // 邀请者名字

    char	szPlayerName[GAME_NAME_MAXSIZE];	    // 玩家名字
    BYTE    bySex;                                  // 性别
    DWORD   dwSrcScene;                             // 来源场景ID
    DWORD   dwSrcWorldID;                           // 来源世界ID
    PDBID	idActor;	                            // 进入大厅人物PDBID
    DWORD	dwHeroID;                               // 选的英雄ID
    int     nSummonerSkill[SUMMONER_SKILL_COUNT];   // 召唤师技能
    int     nTalentEffectPageIndex;                 // 当前选择天赋页
    int     nRuneEffectPageIndex;                   // 当前选择符文页
    SMatchPlayerRoomExData sExData;                 // 附加数据

    SMsg_MasterInviteRespon_SO()
    {
        memset(this, 0, sizeof(*this));
    }
};


struct SMsg_LegendCupWinMsgInfo
{
    BYTE    eRoundType;                   // 本轮类型 淘汰 or 小组赛 ERoundCompetitionType
    bool    bRoundLastNode;               // 是否本轮最后一个节点
    bool    bIsOnlineCup;
    bool    bIsLastRound;
    bool    bNodeEnd;
    bool    bWin;
    DWORD   dwNextStartInterval;
    DWORD   dwNextRoundStartTime;

    SMsg_LegendCupWinMsgInfo(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SMsg_BroadcastHeroWinData
{
    DWORD   dwActorID;                              // 玩家ID
    float   fWinRate;                               // 所选英雄胜率
    int     nMatchNum;                              // 所选英雄选择的场次
    int     nRankGrade;                             // 当前段位等级

    SMsg_BroadcastHeroWinData(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// 赛季开始 OS_MSG_MATCH_SEASON_BEGIN
struct SMsgMatchSeasonBegin_OS 
{
	int nBeginSeasonIndex;

	SMsgMatchSeasonBegin_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 赛季结束 OS_MSG_MATCH_SEASON_END
struct SMsgMatchSeasonEnd_OS
{
	int nEndSeasonIndex;

	SMsgMatchSeasonEnd_OS()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()