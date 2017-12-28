using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using GameLogic;
using Stage_ManagerDef;
using System.Runtime.InteropServices;
using System.Threading;
using DataCenter;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using War;

public struct GameViewCommand
{
    public int cmdID;
    public int nParam;
    public string strParam;
    public IntPtr ptrParam;
    public int datalen;
    public byte[] databytes;
    public Type datatype;
}

// 处理逻辑层发过来的命令
public class GameViewCommandHandler : MonoBehaviour
{
    //单件
    public static GameViewCommandHandler Instance = null;

    // 命令处理函数
    public delegate bool FUNC_PTR_COMMAND_HANDLER(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen);

    // 命令映射表
    public static Dictionary<int, FUNC_PTR_COMMAND_HANDLER> m_HandlerMap = new Dictionary<int, FUNC_PTR_COMMAND_HANDLER>();

    // 消息类型映射表，用于反查消息的非托管数据大小
    public static Dictionary<Int32, Type> m_TypeMap = new Dictionary<Int32, Type>();

    // 命令列表，把来自逻辑层的多线程命令归集为单线程命令
    private static Queue<GameViewCommand> commands = new Queue<GameViewCommand>();

    public static bool Queued = false; //多线程指令归集为单线程开关，比如scalefrom来的指令可能在另外一个线程

    private static bool onCommandImp(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (!m_HandlerMap.ContainsKey(cmdID))
        {
            return false;
        }

        return m_HandlerMap[cmdID](cmdID, nParam, strParam, ptrParam, nPtrLen);
    }

    public static bool onCommand(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen, int nTickCount)
    {
        //因为传递log文件名是在逻辑层创建途中进行的，而这时候显示层没有初始化，所以这里特殊处理一下
        if (cmdID == (int)GameLogicDef.GVIEWCMD_OPEN_LOG_FILE)
        {
            return onCommand_OpenLog(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }

        if (Queued)
        {
            int threadid = Thread.CurrentThread.ManagedThreadId;
            if (threadid > 1)
            {
                Trace.Log("GameViewCommandHandler::onCommand from other thread: cmdid=" + cmdID.ToString() + ",threadid=" + threadid.ToString());
            }

            GameViewCommand command = new GameViewCommand();
            command.cmdID = cmdID;
            command.nParam = nParam;
            command.strParam = strParam;
            command.ptrParam = ptrParam;
            command.datalen = nPtrLen;
            if (m_TypeMap.ContainsKey(cmdID))
            {
                command.datatype = m_TypeMap[cmdID];
                if (command.datatype != null)
                {
                    command.datalen = Marshal.SizeOf(command.datatype);
                }
            }
            command.databytes = null;
            if (command.ptrParam != null && command.ptrParam != IntPtr.Zero && command.datalen > 0)
            {
                command.databytes = new byte[command.datalen];
                try
                {
                    Marshal.Copy(command.ptrParam, command.databytes, 0, command.datalen);
                }
                catch (Exception e)
                {
                    command.databytes = null;
                    Trace.LogError(e.ToString());
                }
            }
            lock (commands)
            {
                commands.Enqueue(command);
            }
        }
        else
        {
            onCommandImp(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public void Update()
    {
        while (true)
        {
            bool bEmpty = true;
            lock (commands)
            {
                bEmpty = (commands.Count <= 0);
            }
            if (bEmpty)
            {
                break;
            }

            GameViewCommand command;
            lock (commands)
            {
                command = commands.Dequeue();
            }

            if (command.databytes != null && command.datalen > 0) //把缓存的参数重新解开到非托管内存
            {
                IntPtr c = Marshal.AllocHGlobal(command.datalen);
                Marshal.Copy(command.databytes, 0, c, command.datalen);
                onCommandImp(command.cmdID, command.nParam, command.strParam, c, command.datalen);
                Marshal.FreeHGlobal(c);
            }
            else
            {
                onCommandImp(command.cmdID, command.nParam, command.strParam, command.ptrParam, command.datalen);
            }
        }
    }

    public void setHandler(Int32 cmdID, FUNC_PTR_COMMAND_HANDLER handler, Type t)
    {
        m_HandlerMap[(int)cmdID] = handler;
        m_TypeMap[(int)cmdID] = t;
    }

    public void Awake()
    {
        Instance = this;

        // 来自逻辑层的消息在这里统一定义，然后再调到界面逻辑
        setHandler((int)GameLogicDef.GVIEWCMD_GAME_STAGE_STATE, onCommand_ChangeGameStageState, null);
        setHandler((int)GameLogicDef.GVIEWCMD_LOGIN_CONNECTING, onCommand_Connecting, null);
        setHandler((int)GameLogicDef.GVIEWCMD_USER_LOGIN_RESULT, onCommand_LoginResult, typeof(gamelogic_updateplayers));
        setHandler((int)GameLogicDef.GVIEWCMD_LOGIN_OPENQUEUE_VIEW, onCommand_OpenLoginQueue, null);
        setHandler((int)GameLogicDef.GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW, onCommand_OpenServerMsgInfo, null);


        setHandler((int)GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER_RESULT, onCommand_UserCreatePlayerResult, null);
        setHandler((int)GameLogicDef.GVIEWCMD_LOAD_GAME, onCommand_LoadScene, typeof(game_LoadSceneInfo));//加载指定的主城场景
        setHandler((int)GameLogicDef.GVIEWCMD_PLAY_PROGRESS_BAR, onCommand_PlayProgressBar, null);
        setHandler((int)GameLogicDef.GVIEWCMD_CLEAR_PROGRESS_BAR, onCommand_ClearProgressBar, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PROGRESS_BAR, onCommand_PlayProgressBar2, null);//翅膀状态的施法条？
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CLEAR_PROGRESS_BAR, onCommand_ClearProgressBar2, null);//翅膀状态的施法条？

        setHandler((int)GameLogicDef.GVIEWCMD_USER_SELECT_HERO, onCommand_UserSelectHero, typeof(cmd_choose_hero_sync));
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM, onCommand_OpenCreateRoom, null);//房主，打开创建房间界面
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_HALL_ADD_ROOM, onCommond_HallAddRoom, typeof(cmd_match_hall_add_room));//刷新大厅房间列表
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM, onCommond_EnterRoom, typeof(cmd_match_hall_add_room));//接收房间数据
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM_OTHER, onCommond_EnterRoomOther, typeof(cmd_match_room_role_data));//进入非房主房间，接收房间内其他玩家数据
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM_SELF, onCommond_EnterRoomSelf, typeof(cmd_match_room_role_data));//进入非房主房间，接收玩家自己的数据
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM_OTHER_OB, onCommond_EnterRoomOtherOb, typeof(cmd_match_room_ob_data));//进入非房主房间，接收房间内其他OB数据
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM_SELF_OB, onCommond_EnterRoomSelfOb, typeof(cmd_match_room_ob_data));//进入非房主房间，接收玩家自己的OB数据

        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM_DATA, onCommond_EnterRoomData, typeof(cmd_match_room_role_data));//房间内玩家接收新玩家进入房间
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_START_SELECT_HERO, onCommand_StartSelectHero, null);//房间内开始选英雄
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_READY_TO_FIGHT, onCommond_ReadyToFight, null);//房间内选英雄结束
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE, onCommand_LoadScene, typeof(game_LoadSceneInfo));//加载Moba战场场景
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE, onCommand_RoomRoleSlotChange, typeof(cmd_match_room_hero_pos_change));//玩家点击空位置后，接收逻辑层发来的新位置
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_FROM_GAME_TO_ROOM, onCommond_FromGameToRoom, null);//游戏比赛结束，释放游戏资源，加载房间界面
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE_OB, onCommand_RoomRoleSlotChangeOb, typeof(cmd_match_room_ob_pos_change_oc));//玩家点击空位置后，接收逻辑层发来的新位置
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_FROM_GAME_TO_ROOM, onCommond_FromGameToRoom,null);//游戏比赛结束，释放游戏资源，加载房间界面
        setHandler((int)GameLogicDef.GVIEWCM_MATCH_OVER_ENTER_ROOM, onCommond_MatchOverEnterRoom, typeof(cmd_match_hall_add_room));//比赛结束，从比赛场景返回房间
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM_MODES, onCommand_CreateRoomModes, typeof(cmd_match_create_room_mode));//设置游戏模式信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_HALL_ROOM_FRESH, onCommand_HallRoomFresh, typeof(cmd_match_hall_add_room));//在大厅的人接收房间刷新信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_EXIT_ROOM_ENTER_HALL, onCommand_ExitRoomEnterHall, null);//逻辑层告知显示层服务器已将玩家离开房间
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_PLAYER_EXIT, onCommand_RoomPlayerExit, typeof(cmd_match_room_hero_exit));//房间内玩家接收到有其他玩家离开
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_HALL_REMOVE_ROOM, OnCommand_HallRemoveRoom, null);//大厅内玩家接收有房间移除消息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_MASTER_CHANGE, onCommand_RoonMasterChange, typeof(cmd_match_room_master_change));//房间内房主改变
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_CHANGE, onCommand_RoomHeroChange, typeof(cmd_match_room_hero_change));//房间玩家英雄改变
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_PRE_HERO_CHANGE_LIST, onCommand_RoomPreHeroList, null);//预选英雄列表显示
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_INFO, onCommand_RoomHeroInfo, typeof(cmd_match_room_hero_Info));//接收当前选择英雄的信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_SKILL, onCommand_RoomHeroSkill, typeof(cmd_choose_hero_sync));//接收当前选择英雄的技能信息
        setHandler((int)GameLogicDef.GVIEWCM_MATCH_PLAYER_EXIT_ROOM_STATE_2_WAIT, onCommand_PlayerExitRoomState2Wait, null);//有人离开房间，房间从选英雄状态变回准备状态
        setHandler((int)GameLogicDef.GVIEWCM_MATCH_MASTER_KICK_ROLE, onCommand_MasterKickRole, typeof(cmd_match_room_hero_exit));//房主踢出玩家
        setHandler((int)GameLogicDef.GVIEWCM_MATCH_HALL_ROOM_DATA, onCommand_MatchHallRoomData, typeof(cmd_match_create_room_map_info));//显示大厅房间地图等信息
        setHandler((int)GameLogicDef.GVIEWCM_MATCH_TO_CHOOSE_VOCATION, onCommand_MatchToChooseVocation, null); //允许从比赛大厅返回选择角色界面
        setHandler((int)GameLogicDef.GVIEWCMD_ENTER_TEST_MAP, onCommand_EnterTestMap, null);//接收直接进入场景的场景名字
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_SELECT_HERO_SLOT, onCommand_SelectHeroSlot, typeof(cmd_match_room_hero_slot));//选择英雄界面，接受英雄插槽信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_SLOT, onCommand_RoomHeroSlot, typeof(cmd_match_room_hero_slot));//等待房间，接受英雄插槽信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_CONFIRM_HERO, onCommand_RoomConfirmHero, typeof(cmd_match_room_confirm_hero));//等待房间玩家确认选择的英雄
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_ALL_CONFIRM_HERO, onCommand_AllConfirmHero, null);//房间所有玩家都选英雄完毕
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_FROM_GAME_TO_HALL, onCommand_FromGameToHall, null);//玩家在战场退出游戏，直接返回选择角色界面
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE_PLAYERINFO, onCommand_MatchLoadSenecePlayerInfo, typeof(cmd_match_load_scene_playerinfo));
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE, onCommand_MatchLoadSceneProgress, null);//加载moba场景前，发这消息显示玩家进度条
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_HALL_REMOVE_ALL_ROOM, onCommand_MatchHallRemoveAllRoom, null);//清空大厅房间列表
        setHandler((int)GameLogicDef.GVIEWCMD_JUDGE_CREATE, onCommand_JudgeCreateUI, null);
        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_CHAT_MESSAGE, onCommand_ShowChatMessage, typeof(gamelogic_show_chat_message));    // 显示聊天消息
        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_PRIVATE_CHAT_MESSAGE, onCommand_ShowPrivateChatMessage, typeof(gamelogic_show_private_chat_message));    // 显示私聊消息
        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_SYSTEM_MESSAGE, onCommand_ShowSystemMessage, typeof(gamelogic_show_system_message));    // 显示系统消息
        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_LOUDSPEAKER, onCommand_ShowLoudspeaker, typeof(gamelogic_show_loudspeaker));    // 显示喇叭
        setHandler((int)GameLogicDef.GVIEWCMD_PLAY_SEND_WAR_TYPEID, onCommand_LoadGoods, null);
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_START_ROOM_RULE, onCommand_MatchStartRoomRule, null);//房主点开始游戏后，逻辑层下发的房间规则消息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO, onCommand_MatchRoomRuleBanHero, /*typeof(cmd_match_room_rule_ban_hero)*/null);//某个玩家进入禁止英雄状态
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO_LIST, onCommand_MatchRoomRuleBanHeroList, null);//当前要禁用的一个英雄ID
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_GLOBLE_BAN_HERO_LIST, onCommand_MatchRoomGlobleBanHeroList, null);//全局要禁用的一个英雄ID
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ENTER_SCENE, onCommand_ExitHallEnterScene, null);//退出大厅进入主场景
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_SHOW_ACCEPT_CHECK, onCommand_ShowAcceptCheckWnd, null);//显示接受比赛窗体
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_HIDE_ACCEPT_CHECK, onCommand_HideAcceptCheckWnd, null);//隐藏接受比赛窗体
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_CUR_MATCH_COUNT_INFO, onCommand_CurMatchCountInfo, null);//隐藏接受比赛窗体
        


        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_RULE_SELECT_HERO, onCommand_MatchRoomRuleSelectHero, null);//选择英雄
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_STATE, onCommand_MatchRoomState, typeof(cmd_change_roomstate));
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_CAN_SELECT, onCommand_Can_Select, typeof(cmd_match_room_hero_can_select));    // 禁用或者是解锁可选择英雄
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_SUMMONER_ALL_SKILL, onCommand_AllSummonerSkill, null);//// 返回给显示层所有的召唤师技能
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_SUMMONER_DEFAULT_SKILL, onCommand_DefSkill, null);  // 刚登陆时将召唤师技能id发送到显示层
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE, onCommand_MatchLoadSceneProgreeseRate, typeof(cmd_load_scene_rate));
        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_MESSAGEBOX, onCommand_ShowMessageBox, typeof(cmd_show_message_box));
        setHandler((int)GameLogicDef.GVIEWCMD_MATCHTYPE_ORDER_RANK_INFO, onCommond_HighLadderInfo, null);   //接收角色排名榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_KILL_ORDER_RANK_INFO, onCommond_KillChartInfo, null);        //接收多杀榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_HERO_ORDER_RANK_INFO, onCommond_HeroRankInfo, null);        //接收英雄养成榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_HEROTYPE_ORDER_RANK_INFO, onCommond_HeroTypeChartInfo, null);    //接收英雄类型榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ORDER_RANK_INFO, onCommond_ClanChartInfo, null);        //接收联盟榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_ORDER_RANK_INFO, onCommond_KinChartInfo, null);         //接收战队榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_DONATE_ORDER_RANK_INFO, onCommond_DonateChartInfo, null);      //接收捐卡榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_CURWEEK_MASTER_ORDER_RANK_INFO, onCommond_CurWeekMasterChartInfo, null);        //本周导师榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_CURWEEK_PRENTICE_ORDER_RANK_INFO, onCommond_CurWeekPrenticeChartInfo, null);    //本周学徒榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_LASTWEEK_MASTER_ORDER_RANK_INFO, onCommond_LastWeekMasterChartInfo, null);      //上周导师榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_LASTWEEK_PRENTICE_ORDER_RANK_INFO, onCommond_LastWeekPrenticeChartInfo, null);  //上周学徒榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_GEMSTONE_ORDER_RANK_INFO, onCommond_GemStoneChartInfo, null);                   // 宝石榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_SKIN_ORDER_RANK_INFO, onCommond_SkinChartInfo, null);                   // 皮肤榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_HEROCOUNT_ORDER_RANK_INFO, onCommond_HeroCountChartInfo, null);       // 英雄数量榜数据
        setHandler((int)GameLogicDef.GVIEWCMD_KING_OF_HERO_RANK_INFO, onCommond_KingOfHeroChartInfo, null);       // 英雄王者榜
        setHandler((int)GameLogicDef.GVIEWCMD_PERSON_ORDER_SET_INFO, onCommond_PersonOrderSetInfo, null);      //个人排行榜设置

        setHandler((int)GameLogicDef.GVIEWCMD_SUMMONER_BROAD_SKILL, onSummonerBroadSkill, typeof(cmd_Summoner_Skill_Change));
        setHandler((int)GameLogicDef.GVIEWCMD_SUMMONER_SET_SKILLPIC_LOGIN, onSummonerSetSkillPicLogin, null);// 当玩家登陆时设置召唤师技能的信息
        setHandler((int)GameLogicDef.GVIEWCMD_DRAW_DYNAMICOBSTACLE, onCommand_DrawDynamicObstacle, typeof(cmd_DynamicObstacle_Info));//房间玩家英雄改变
        setHandler((int)GameLogicDef.GVIEWCMD_PRELOAD_SKIN_LIST, onCommand_PreLoadSkin, typeof(cmd_count));//接受战场皮肤

        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CAN_SELECT, onCommand_Computer_Can_Select, typeof(cmd_match_room_computer_hero_can_select));    // 禁用或者是解锁可选择机器人
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CHANGE, onCommand_RoomComputerChange, typeof(cmd_match_room_computer_hero_change));//房间玩家英雄改变
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_COMPUTER_RULE_BAN_HERO_LIST, onCommand_MatchRoomRuleBanComputerList, null);//房间玩家英雄改变
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_ROOM_WAIT_COUNTDOWN, onCommand_LegendCupRoom_WaitCountdown, typeof(cmd_legendcup_room_wait_countdown));
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_CAN_BAN_HERO_FLAG, onCommand_CanBanHero_Flag, null);
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_CAN_BAN_HERO_COUNT_DOWN, onCommand_BanHero_Countdown, null);
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_SINGLE_BAN_HERO, onCommand_SingleBanHero, null);
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_ALL_BAN_HERO, onCommand_AllBanHero, null);
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_ROOM_WIN_LEGENDCUP_MSGINFO, onCommand_LgendCupWinMsgInfo, null);


        setHandler((int)GameLogicDef.GVIEWCMD_KIN_UPDATEKINLIST, onCommand_UpdateKinList, null); // 更新战队列表
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_REQUESTKINLIST, onCommand_RequestKinList, null); // 战队列表
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_REQUESTMEMBERLIST, onCommand_RequestKinMemberList, null); // 战队成员列表
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_REQUESTBASEDATA, onCommand_RequestKinBaseData, null); // 战队基础数据
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_DISMISS, onCommand_RecvKinDismiss, null); // 战队基础数据

        setHandler((int)GameLogicDef.GVIEWCMD_KIN_APPLY_LIST, onCommand_RequestKinApplyList, null); // 战队申请列表
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_REMOVEAPPLY, onCommand_KinRemoveApply, null); //移除战队人员
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_ADDAPPLY, onCommand_KinAddApply, null); //添加申请人员
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_REMOVEMEMBER, onCommand_KinRemoveMember, null); //移除战队成员
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_ADDMEMBER, onCommand_KinAddMember, null); //添加战队成员
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_UPDATEMEMBER, onCommand_KinUpdateMember, null); //更新战队成员
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_UPDATESELF, onCommand_UpdateKinSelf, null); //更新自己战队成员信息
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_CREATE_SUCCESS, onCommand_KinCreateSuccess, null); //创建战队成功
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_JOIN_SUCCESS, onCommand_KinJoinSuccess, null); //加入战队成功
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_ANSWERINVITE_SUCCESS, onCommand_KinAnswerInviteSuccess, null); //加入战队成功


        setHandler((int)GameLogicDef.GVIEWCMD_KIN_QUIT, onCommand_KinQuit, null); //更新自己战队成员信息
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_STRING_PROP_UPDATE, onCommand_KinStringPropUpdate, null); //更新自己战队字符串属性信息
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_NUM_PROP_UPDATE, onCommand_KinNumPropUpdate, null); //更新自己战队数值属性信息
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_AUTOACCEPT_SAVE, onCommand_AutoAcceptSave, null); //设置自动接收
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_AUTOACCEPT_REQUEST, onCommand_AutoAcceptRequest, null); //设置自动接收
        setHandler((int)GameLogicDef.GVIEWCMD_RECV_KIN_EVENTLIST, onCommand_KinEventListRequest, null); // 战队事件列表
        setHandler((int)GameLogicDef.GVIEWCMD_RECV_KIN_GLORYDATA, onCommand_KinGloryDataRequest, null); // 战队荣誉数据
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_RECV_WARRECORD_TOTAL, onCommand_KinWarRecordTotalRequest, null); // 战队总历史战绩
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_RECVKININVITE, onCommand_KinRecvKinInvite, typeof(cmd_recv_kin_invite)); // 邀请加入战队请求
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_LEGENDCUPKININFO_RECV, onCommand_LegendCupKinInfoRecv, null); // 查看杯赛战队信息
        setHandler((int)GameLogicDef.GVIEWCMD_KIN_LEGENDCUPKINGLORY_RECV, onCommand_LegendCupKinGloryRecv, null); // 查看杯赛战队荣誉

        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REQUESTCLANLIST, onCommand_RequestClanList, null); // 帮会列表
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_BASEDATA, onCommand_ClanBaseData, null); // 帮会基础数据
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_SET_BASEDATA, onCommand_ClanSetBaseData, null); // 设置帮会基础数据
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_APPLY_LIST, onCommand_ClanApplyList, null); // 请求帮会申请人员列表
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REMOVEAPPLY, onCommand_ClanRemoveApply, null); // 移除帮会申请
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ADDAPPLY, onCommand_ClanAddApply, null); // 增加帮会申请
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REMOVEMEMBER, onCommand_ClanRemoveMember, null); // 帮会移除成员
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ADDMEMBER, onCommand_ClanAddMember, null); // 帮会添加成员
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_UPDATECLIENT, onCommand_ClanUpdate, null); // 更新帮会信息

        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REQUEST_CARDLIST, onCommand_RequestCardList, null); // 请求帮会列表
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REQUEST_CARD, onCommand_UpdateRequestCard, null); // 更新请求卡牌
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REMOVE_REQUEST_CARD, onCommand_RemoveRequestCard, null); // 删除请求卡牌
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REQUEST_DONATELOG, onCommand_RequestDonateLog, null); // 请求捐赠记录
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_UPDATE_DONATELOG, onCommand_UpdateDonateLog, null); // 更新捐赠记录
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ADD_REQUESTCARD_TIP, onCommand_AddRequestcardTip, null); // 更新捐赠记录
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_REQUESTCARD_INFO, onCommand_RequestcardInfo, null); // 更新捐赠记录
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_CREATE_SUCCEED, onCommand_ClanCreateSucceed, null); // 创建帮会成功
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ONQUIT, onCommand_ClanOnQuit, null); // 离开帮会通知
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_EVENT_LIST, onCommand_ClanEventListRequest, null); // 帮会事件列表
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_APPLY_RET, onCommand_ClanApplyRet, null); // 联盟申请返回
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_SET_AUTOACCEPT_FLAG, onCommand_ClanSetAutoAcceptFlag, null); // 联盟自动接受设置
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_STATE_NOTICE, onCommand_ClanStateNotice, null); // 联盟状态改变通知
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_CREATE_LEGENDCUP_DIDA, onCommand_ClanCreateLegendCupDida, null); // 联盟状态改变通知
        

        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_SUBINVITED, onCommand_TeamSubInvited, typeof(cmd_team_invited2view)); // 邀请组队
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_SUBREQUEST, onCommand_TeamSubRequest, typeof(cmd_team_subrequest2view)); // 请求组队
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_UPDATETEAMMEMBER, onCommand_TeamUpdateTeamMember, null); // 更新队友消息
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_CHANGECAPTAIN, onCommand_TeamChangeCaptain, typeof(cmd_team_changeCaptain2view)); // 改变队长消息
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_LEAVEMEMBER_KICKOUT, onCommand_TeamKickoutMember, typeof(cmd_team_kickoutMember2view)); // 踢出队员消息
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_LEAVEMEMBER_LEAVE, onCommand_TeamLeaveTeam, typeof(cmd_team_leaveTeam2view)); // 离开队伍消息
        setHandler((int)GameLogicDef.GVIEWCMD_TEAM_LEAVEMEMBER_DISBAND, onCommand_TeamDisbandTeam, null); // 踢出队员消息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_WND_SHOW, onCommand_MatchWndShow, typeof(cmd_add_match_info)); // 显示等待时间界面
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_WND_HIDE, onCommand_MatchWndHide, null); // 隐藏等待时间界面
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_OK, onCommand_MatchOK, null); // 撮合成功
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_FAIL, onCommand_MatchFail, null); // 撮合失败
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_REQUEST_PUNISHINFO, onCommand_MatchRequestPunishInfo, null); // 请求撮合失败惩罚信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_SELF_PUNISHINFO, onCommand_SelfPunishInfo, null); // 自己惩罚信息
        setHandler((int)GameLogicDef.GVIEWCMD_DELETE_PUNISH_TYPE_INFO, onCommand_DeletePunishInfo, null); // 自己惩罚信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_OPEN_OR_CLOSE, onCommand_MatchCloseOrOpen, null); // 自己惩罚信息
        setHandler((int)GameLogicDef.GVIEWCMD_MATCH_RECOMMEND_RANK, onCommand_MatchRecommendRank, null); // 推送排邀请


        setHandler((int)GameLogicDef.GVIEWCMD_USER_RANDOM_PLAYER_NAME_RESULT, onCommand_RequestRandomNameResult, typeof(game_player_request_name_result)); // 接收请求名字结果
        setHandler((int)GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER_RESULT, onCommand_CreatePlayerResult, typeof(gamelogic_create_player_result));//创建角色名返回消息

        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_COMMON_MESSAGEBOX, onCommand_ShowCommonMessageBox, typeof(cmd_show_common_message_box));

        setHandler((int)GameLogicDef.GVIEWCMD_ANTIADDICT_REGIST_RESULT, onCommand_AntiAddictRegistResult, typeof(gamelogic_antiaddict_regist_result)); //防沉迷注册结果
        setHandler((int)GameLogicDef.GVIEWCMD_ANTIADDICT_TIP, onCommand_AntiAddictTip, typeof(gamelogic_antiaddict_tip)); //防沉迷信息提醒

        setHandler((int)GameLogicDef.GVIEWCMD_SHOW_TASK_LIST, onCommand_ShowTaskList, null);
        setHandler((int)GameLogicDef.GVIEWCMD_UPDATE_TASK_LIST, onCommand_UpdateTaskList, null);
        setHandler((int)GameLogicDef.GVIEWCMD_DELETE_TASK_LIST, onCommand_DeleteTaskList, null);
        setHandler((int)GameLogicDef.GVIEWCMD_SEND_TASK_PRIZE, onCommand_SendTaskPrize, null);

        setHandler((int)GameLogicDef.GVIEWCMD_VIEW_TASK_BASE, onCommand_ViewTaskBase, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VIEW_TASK_REPLY, onCommand_ViewTaskReply, null);
        setHandler((int)GameLogicDef.GVIEWCMD_PUBLISH_TASK, onCommand_PublishTaskSuccess, null);
        setHandler((int)GameLogicDef.GVIEWCMD_PRE_CATCH_TASK, onCommand_PreCatchTask, null);
        setHandler((int)GameLogicDef.GVIEWCMD_CATCH_TASK, onCommand_CatchTask, null);
        setHandler((int)GameLogicDef.GVIEWCMD_REFLUSH_OPT_TASK, onCommand_ReflushOptTask, null);
        setHandler((int)GameLogicDef.GVIEWCMD_ACTOR_PUBLISH_CONTACT, onCommand_ActorPublishContact, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VIEW_TASK_DELETE, onCommand_SendContactDataDelete, null);

        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_ENTER_ROOM, onCommand_EnterVoiceRoom, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_UPDATE_ROOM_BASE_INFO, onCommand_UpdateVoiceRoomBaseInfo, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_UPDATE_ROOM_INFO, onCommand_UpdateVoiceRoomInfo, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_UPDATE_USER_INFO, onCommand_UpdateVoiceUserInfo, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_UPDATE_USER_ISSPEAKING, onCommand_UpdateVoiceUserSpeakingState, null);
        setHandler((int)GameLogicDef.GVIEWCMD_VOICE_CHANNEL_LIST, onCommand_UpdateVoiceChannelList, null);

        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTISSUELIST, onCommand_RequestIssueList, null); // 发布列表
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER, onCommand_RequestApplyAnswer, null); // 拜师或收徒应答信息
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTDISMISS, onCommand_RequestMentorshipDismiss, null); // 解散师徒信息 
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_UPDATESHIPINFO, onCommand_UpdateMentorshipInfo, null); // 更新师傅数据信息 
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_UPDATEPERSONBASE, onCommand_MentorshipUpdatePersonBase, null); //更新师徒个人基础数据信息
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_UPDATEMENTORSHIPLIST, onCommand_MentorshipUpdateShipList, null); //更新师徒个人师徒列表信息
        setHandler((int)GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST, onCommand_RequestPrenticeList, null); //师傅徒弟列表

        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_CUP_LIST, onCommand_LegendcupRecvCupList, null); //接收杯赛列表
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_REGIST_KINLIST, onCommand_LegendcupRecvRegistKinList, null); //接收某杯赛战队列表
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_REGIST_MEMBERLIST, onCommand_LegendcupRecvRegistKinMemberList, null); //接收某杯赛战队成员列表
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_REGIST_BLACKLIST, onCommand_LegendcupRecvRegistBlacklist, null); //接收某杯赛战队黑名单
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_COMPETITION_KIN, onCommand_LegendcupRecvCompetitionKin, null); //接收某杯赛参赛成员信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_COMPETITION_NODE, onCommand_LegendcupRecvCompetitionNode, null); //接收某杯赛节点信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_RECV_COMPETITION_RESULT, onCommand_LegendcupRecvCompetitionResult, null); //接收某节点战绩信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_CUP, onCommand_LegendcupUpdateSingleCup, null); //更新某杯赛基础信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_COMPETITION_NODE, onCommand_LegendcupUpdateSingleCompetitionNode, null); //更新杯赛某节点信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_REGISTER_SUCCESS, onCommand_LegendcupRegisterSuccess, null); //加入杯赛成功
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_CREATE_CUP_TYPEINFO, onCommand_LegendcupCreateTypeInfo, null); //创建杯赛类型
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_CREATER_PUBLIC_SUCCESS, onCommand_LegendcupCreaterPublicSuccess, null); //通知创建者发布杯赛成功
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_ROUNDTIME_RECV, onCommand_LegendcupRecvRoundTime, null); //接收某杯赛轮次时间信息
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_PUBLIC_SUCCESS, onCommand_LegendcupPublicSuccess, null); //通知所有人发布杯赛成功
        setHandler((int)GameLogicDef.GVIEWCMD_LEGENDCUP_SYSTEM_CANCEL_CUP, onCommand_LegendcupSyytemCancel, null); //通知取消杯赛

        setHandler((int)GameLogicDef.GLOGICCMD_SNS_MSG_DATA, onCommand_SNS_MsgData, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_UPDATE_RELATIONSHIP, onCommand_SNS_RelationshipStatus, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDY_STATUS, onCommand_SNS_BuddyStatus, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDY_LIST, onCommand_SNS_AllBuddyInfo, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDYGROUP_LIST, onCommand_SNS_AllBuddyGroupInfo, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDYRELATION_LIST, onCommand_SNS_BuddyRelationList, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDYRELATION, onCommand_SNS_BuddyRelation, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDY, onCommand_SNS_Buddy, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDY_REMARK, onCommand_SNS_BuddyRemark, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDY_SELF, onCommand_SNS_BuddySelf, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_USERINFO, onCommand_SNS_UserInfo, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_BUDDYINFO_CHNAGE_NOTIFY, onCommand_SNS_BuddyInfoChagne_Notify, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_INVITE_PLAYGAME_NOTIFY, onCommand_SNS_InvitePlayGame_Notify, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_INVITE_MATCH_TEAM_NOTIFY, onCommand_SNS_InviteMatchTeam_Notify, null);
        setHandler((int)GameLogicDef.GLOGICCMD_SNS_UPDATE_SCENE_NOTIFY, onCommand_SNS_UpdateScene_Notify, null);
        
        setHandler((int)GameLogicDef.GVIEWCMD_MAIL_REQUESTBASEDATALIST, onCommand_Mail_ReqBastInfoList, null);  // 邮件基础列表
        setHandler((int)GameLogicDef.GVIEWCMD_MAIL_RESPONSEREADMAIL, onCommand_Mail_RspDetailData, null);       // 邮件详细信息
        setHandler((int)GameLogicDef.GVIEWCMD_MAIL_NEWMAILCOUNT, onCommand_Mail_NewMailCount, null);            // 未读邮件数
        setHandler((int)GameLogicDef.GVIEWCMD_MAIL_REQUESTFASTRECVPLUS, onCommand_Mail_ReqFastRecvPlus, null);  // 快速取件
        setHandler((int)GameLogicDef.GVIEWCMD_MAIL_RECVPLUSSUCCESS, onCommand_Mail_RecvSuccess, null);          // 取件成功

        setHandler((int)GameLogicDef.GVIEWCMD_SET_ROBOT_MODE, onCommand_setRobotMode, typeof(gameview_set_robot_mode));  // 进入机器人模式
        setHandler((int)GameLogicDef.GVIEWCMD_SET_CAMERA_HEIGHT, onCommand_SetCameraHeight, typeof(gameview_set_camera_height));  // 设置摄像机高度

        setHandler((int)GameLogicDef.GVIEWCMD_RECV_DISTRICT_LIST, onCommand_Recv_District_List, null);
        setHandler((int)GameLogicDef.GVIEWCMD_RECV_SELECT_SERVER_RESULT, onCommand_Recv_Select_server_result, null);
        setHandler((int)GameLogicDef.GVIEWCMD_CHECK_GAME_VERSION, onCommand_Check_Game_version, null);
        setHandler((int)GameLogicDef.GVIEWCMD_RECV_LASTENTER_DISTRICT_LIST, onCommand_Recv_LastEnter_District_List, null);
        setHandler((int)GameLogicDef.GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN, onCommand_ReportOnRightButtonDown, null);

        setHandler((int)GameLogicDef.GVIEWCMD_WAR_MOBA_GOODS_INFO, onCommand_Equip_Info_Table, null);

        setHandler((int)GameLogicDef.GVIEWCMD_SWITCH_BULLET_PANEL, onCommand_SwitchBulletPanel, typeof(cmd_switch_bullet_count_panel));

        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_INVITE_RET, onCommand_ClanInviteReturn, typeof(cmd_clan_invite_ret)); // 邀请加入战队请求
        setHandler((int)GameLogicDef.GVIEWCMD_CLAN_ANSWER_INVITE_SUCCESS, onCommand_AnswerInviteSuccess, null); // 邀请加入战队请求

        setHandler((int)GameLogicDef.GVIEWCMD_WAR_SAFETYZONE_INFO, onCommand_WarSafetyZoneInfo, null); // 安全区信息
        setHandler((int)GameLogicDef.GVIEWCMD_WAR_PLAYER_RECONNECTION, onCommand_PlayerReconnection, null); // 断线重连战场
        setHandler((int)GameLogicDef.GVIEWCMD_WAR_NOX_SPREAD_CD, onCommand_WarNoxSpreadCountDown, null); // 毒气扩散倒计时信息
        setHandler((int)GameLogicDef.GVIEWCMD_WAR_SERVER_TO_VIEW_MSG, onCommand_WarServerToViewMsg, null); // 服务器War直接发到显示层的消息
    }

    bool OnLoginMsg(string msg)
    {
        //获取GFxOutputMessage Label

        //////////////
        return true;
    }

    public bool onCommand_ChangeGameStageState(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        EStage_State currentStage = (EStage_State)nParam;
        StageManager.Instance.SetStageState(currentStage);
        return true;
    }

    public bool onCommand_LoadGoods(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LoadGoods=================== : {0}", nParam));
        }

        cmd_send_war_typeid typeID = IntPtrHelper.toData<cmd_send_war_typeid>(ptrParam);
        
        ViewConfigManager.LoadMobaConfigs(typeID.nConfigWarID, typeID.nMatchTypeID);
        return true;
    }

    //所有加载场景相关的都用这个来接受
    public bool onCommand_LoadScene(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LoadScene=================== : {0}", nParam));
        }

        Game_ManagedDef.EGameMode gamemode = GameLogicAPI.getCurGameMode();
        Debug.Log("游戏模式：" + gamemode);
        game_LoadSceneInfo data = IntPtrHelper.toData<game_LoadSceneInfo>(ptrParam);

        Trace.Log("onCommand_LoadScene:" + data.nCmdID);

        //先移除之前的加载场景信息
        StageManager.Instance.LoadSceneEnd();
        //再重新开始加载场景
        StageManager.Instance.LoadSceneBegin(data);

        LightingEffectFactory.Instance.AcitvieLoadingSceneQueue();

        // 预加载公共光效部分（地图ID为0的是公共资源）
        EffectHelper.LoadTypePreLight(0, ASpeedGame.Data.Scheme.LightType.Monster, CachePriority.PRIORITY_LOW);
        EffectHelper.LoadTypePreLight(0, ASpeedGame.Data.Scheme.LightType.Other, CachePriority.PRIORITY_LOW);
        // 预加载怪物光效部分
        EffectHelper.LoadTypePreLight(data.nMapID, ASpeedGame.Data.Scheme.LightType.Monster, CachePriority.PRIORITY_LOW);
        // 预加载必备光效部分
        EffectHelper.LoadTypePreLight(data.nMapID, ASpeedGame.Data.Scheme.LightType.Other, CachePriority.PRIORITY_LOW);

        LightingEffectFactory.Instance.UnAcitvieLoadingSceneQueue();
        // 创建地图查询对象
        MapFinderManager.Create(data);

        // 通知UI场景切换
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, null);
        return true;
    }

    //显示登陆排队界面
    public bool onCommand_OpenLoginQueue(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_OpenLoginQueue=================== : {0}", nParam));
        }

        // 通知UI场景切换
        UIMsgCmdData uiLoginQueue = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMON_OPEN_LOGINQUEUE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGINQUEUE, uiLoginQueue);
        return true;
    }

    //显示登陆消息界面
    public bool onCommand_OpenServerMsgInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_OpenServerMsgInfo=================== : {0}", nParam));
        }

        // 通知UI场景切换
        UIMsgCmdData uiLoginQueue = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO, uiLoginQueue);
        return true;
    }

    public bool onCommand_Connecting(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        return OnLoginMsg("Server Connecting...");
    }

    public bool onCommand_PlayProgressBar(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_PROGRESSBAR, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_PROGRESSBAR, uiPlayerProgress);
        return false;
    }

    public bool onCommand_ClearProgressBar(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, uiPlayerProgress);
        return false;
    }
    public bool onCommand_PlayProgressBar2(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR, uiPlayerProgress);
        return false;
    }

    public bool onCommand_ClearProgressBar2(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR, uiPlayerProgress);

        return false;
    }

    public bool onCommand_UserSelectHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Trace.LogError("Init onCommand_UserSelectHero");
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UserSelectHero=================== : {0}", nParam));
        }
        else
        {
            Trace.TraceSFUIWaring("GFxHeroSelect", GameLogicDef.GVIEWCMD_USER_SELECT_HERO.ToString(), "SetUserSelectHero");
        }

        return true;
    }

    //房主，打开创建房间界面
    public bool onCommand_OpenCreateRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //发送完，可以把创建房间界面显示出来,大厅界面隐藏
        if (nParam == 0)
        {
            UIMsgCmdData uiCreateRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW, uiCreateRoom);
        }
        return true;
        //Trace.LogError("Enter onCommand_OpenCreateRoom");
    }

    //刷新大厅房间列表
    public bool onCommond_HallAddRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Trace.LogError("onCommond_HallAddRoom");
        //发送完
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HallAddRoom=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoomInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_ADDROOMINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_ADDROOMINFO, uiRoomInfo);
        }
        return true;
    }

    //刷新大厅房间列表
    public bool onCommond_HighLadderInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Trace.LogError("onCommond_HighLadderInfo");
        //发送完
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HighLadderInfo=================== : {0}", nParam));
        }
        else
        {
            //UIMsgCmdData uiHighLadderInfo = new UIMsgCmdData((int)WndMsgID.WND_MSG_HIGHLADDER_GETINFO, nParam, strParam, ptrParam, nPtrLen);
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_GETINFO, uiHighLadderInfo);
            LogicDataCenter.chartDataManager.OnRecvRequestRoleRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_KillChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_KillChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestKillRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_HeroRankInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HeroRankInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestHeroRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_HeroTypeChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HeroTypeChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestHeroTypeRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_ClanChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_ClanChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestClanRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_KinChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_KinChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestKinRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_DonateChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_DonateChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestDonateRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_CurWeekMasterChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_CurWeekMasterChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestCurWeekMasterRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_CurWeekPrenticeChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_CurWeekPrenticeChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestCurWeekPrenticeRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_LastWeekMasterChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_LastWeekMasterChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestLastWeekMasterRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_LastWeekPrenticeChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_LastWeekPrenticeChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestLastWeekPrenticeRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_GemStoneChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_GemStoneChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestGemStoneRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_SkinChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_SkinChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestSkinRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_HeroCountChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HeroCountChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestHeroCountRankData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommond_KingOfHeroChartInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_HeroCountChartInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvRequestKingOfHeroRankData(ptrParam, nPtrLen);
        }
        return true;
    }


    public bool onCommond_PersonOrderSetInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_PersonOrderSetInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.chartDataManager.OnRecvPersonOrderSetInfo(ptrParam, nPtrLen);
        }
        return true;
    }



    public bool onSummonerBroadSkill(int cmdId, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onSummonerBroadSkill=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE, uiRoom);
        }
        return true;
    }

    public bool onSummonerSetSkillPicLogin(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onSummonerSetSkillPicLogin=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL, uiRoom);
        }
        return true;
    }

    //进入非房主的房间,设置房间信息
    public bool onCommond_EnterRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoom=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, uiRoom);
        }
        return true;
    }

    //进入非房主房间，接收房间内其他玩家数据
    public bool onCommond_EnterRoomOther(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoomOther=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiOtherRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, uiOtherRoom);
        }
        return true;
    }

    //进入非房主房间，接收玩家自己的数据
    public bool onCommond_EnterRoomSelf(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoomSelf=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiSelfRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM, uiSelfRoom);
        }
        return true;
    }

    //房间内玩家接收新Ob进入房间
    public bool onCommond_EnterRoomOtherOb(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoomOtherOb=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiEnterRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB, uiEnterRoom);
        }
        return true;
    }

    //进入非房主房间，接收玩家自己的Ob数据
    public bool onCommond_EnterRoomSelfOb(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoomSelfOb=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiSelfRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB, uiSelfRoom);
        }
        return true;
    }

    //房间内玩家接收新玩家进入房间
    public bool onCommond_EnterRoomData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_EnterRoomOther=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiEnterRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, uiEnterRoom);
        }
        return true;
    }

    //房间内开始选英雄
    public bool onCommand_StartSelectHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomSelectHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO, uiRoomSelectHero);
        //显示选英雄倒计时

        return true;
    }

    //房间内选英雄结束
    public bool onCommond_ReadyToFight(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT, uiRoom);

        //选英雄倒计时结束

        return true;
    }

    public bool onCommand_RoomRoleSlotChange(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomRoleSlotChange=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT, uiRoom);
        }
        return true;
    }

    public bool onCommand_RoomRoleSlotChangeOb(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomRoleSlotChangeOb=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB, uiRoom);
        }
        return true;
    }

    //战场结束->释放战场资源,加载大厅资源
    public bool onCommond_FromGameToRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //if (null == ptrParam || IntPtr.Zero == ptrParam)
        //{
        //    Trace.Log(string.Format("==================onCommond_FromGameToRoom=================== : {0}", nParam));
        //}
        //else
        //{
        //GFxLoadingWin loading = SFGFxMovieManager.GetWindow<("GFxLoadingWin") as GFxLoadingWin;
        //loading.LoadHallSceneStart(strParam);
        //Initialize.Instance.LoadHallSceneFormWarStart(strParam, (int)GameLogicDef.GVIEWCMD_MATCH_FROM_GAME_TO_ROOM);
        //}
        Trace.Log("战场结束->释放战场资源,加载大厅资源 ," + (GameLogicDef)cmdID + "目前没有处理这个消息");
        return true;
    }

    //战场结束->进入房间
    public bool onCommond_MatchOverEnterRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_MatchOverEnterRoom=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMMODEL, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMMODEL, uiRoom);

            //   Initialize.Instance.SetState(InitializeState.Home);
        }


        return true;
    }

    public bool onCommand_CreateRoomModes(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CreateRoomModes=================== : {0}", nParam));
        }
        else
        {

            UIMsgCmdData uiGameMode = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_GAMEMODEINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_GAMEMODEINFO, uiGameMode);

            //初始化可创建的房间属性
        }
        return true;

    }



    public bool onCommand_HallRoomFresh(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_HallRoomFresh=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiHallRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO, uiHallRoom);
        }
        return true;
    }

    public bool onCommand_ExitHallEnterScene(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE, null);

        return true;
    }

    //服务器已离开房间，显示大厅
    public bool onCommand_ExitRoomEnterHall(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL, null);

        return true;
    }

    public bool onCommand_ShowAcceptCheckWnd(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowAcceptCheckWnd=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, uiData);
        }
        return true;
    }

   

    public bool onCommand_HideAcceptCheckWnd(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_ACCEPTWAIT_CLOSE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_CLOSE, uiData);
        return true;
    }


    public bool onCommand_RoomPlayerExit(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomPlayerExit=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiKickRole = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, uiKickRole);
        }
        return true;
    }

    public bool OnCommand_HallRemoveRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRemoveItem = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO, uiRemoveItem);
        return true;
    }

    public bool onCommand_RoonMasterChange(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoonMasterChange=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER, uiRoom);

            cmd_match_room_master_change data = IntPtrHelper.toData<cmd_match_room_master_change>(ptrParam);

        }
        return true;
    }



    public bool onCommand_RoomHeroChange(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CreateRoomClickMap=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiChangeHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO, uiChangeHero);
        }
        return true;
    }

    public bool onCommand_RoomPreHeroList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomPreHeroList=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiChangeHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST, uiChangeHero);
        }
        return true;
    }


    public bool onCommand_RoomHeroInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomHeroInfo=================== : {0}", nParam));
        }
        else
        {
            //Trace.TraceSFUIWaring("GFxGameWaitingRoomWin", GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_INFO.ToString(), "SetHeroInformation");
        }
        return true;
    }

    public bool onCommand_RoomHeroSkill(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomHeroSkill=================== : {0}", nParam));
        }
        else
        {
            Trace.TraceSFUIWaring("GFxGameWaitingRoomWin", GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_SKILL.ToString(), "SetSkillSlotInfo");
        }
        return true;
    }

    public bool onCommand_PlayerExitRoomState2Wait(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMROMOVETIMECOUNT, null);
        //收到从选英雄状态到等待状态，倒计时移除
        return true;
    }

    public bool onCommand_MasterKickRole(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomHeroSkill=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiKickRole = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, uiKickRole);
        }
        return true;
    }

    public bool onCommand_MatchHallRoomData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchHallRoomData=================== : {0}", nParam));
        }
        else
        {
            Trace.TraceSFUIWaring("GFxGameHallWin", GameLogicDef.GVIEWCM_MATCH_HALL_ROOM_DATA.ToString(), "SetHallMapInfo");
        }
        return true;
    }

    public bool onCommand_MatchToChooseVocation(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        Trace.TraceSFUIWaring("GFxGameHallWin", GameLogicDef.GVIEWCM_MATCH_TO_CHOOSE_VOCATION.ToString(), "LeaveMovie");
        return true;
    }

    //public bool onCommand_WarEndDisplayInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    //{

    //    //GFxWarRecordTable WarTable = SFGFxMovieManager.GetWindow<GFxWarRecordTable>();

    //    //if (SFGFxMovieManager.CheckTheMovieCanUse(WarTable))
    //    //{
    //    //    WarTable.SetMovieVisible(true);
    //    //    WarTable.isEndWar = 1;
    //    //}

    //    return true;
    //}

    //public bool onCommand_WarEndCloseInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    //{

    //    //GFxWarRecordTable WarTable = SFGFxMovieManager.GetWindow<GFxWarRecordTable>();

    //    //if (SFGFxMovieManager.CheckTheMovieCanUse(WarTable))
    //    //{
    //    //    WarTable.SetMovieVisible(false);
    //    //    WarTable.isEndWar = 0;
    //    //}

    //    return true;
    //}

    public bool onCommand_EnterTestMap(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        Trace.TraceSFUIWaring("GFxHeroSelect", GameLogicDef.GVIEWCMD_ENTER_TEST_MAP.ToString(), "DirectlyEnterScene");
        return true;
    }

    public static bool onCommand_OpenLog(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        Initialize.LogFileName = strParam;
        return true;
    }

    public bool onCommand_SelectHeroSlot(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //现在选择英雄这里不用这个来收静态英雄消息
        return true;
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchHallRoomData=================== : {0}", nParam));
        }
        else
        {
            Trace.TraceSFUIWaring("GFxHeroSelect", GameLogicDef.GVIEWCMD_MATCH_SELECT_HERO_SLOT.ToString(), "SetHeroHeadSlotInfo");
        }
        return true;
    }

    public bool onCommand_RoomHeroSlot(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchHallRoomData=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoomHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO, uiRoomHero);
        }
        return true;
    }

    public bool onCommand_RoomConfirmHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomConfirmHero=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoomConfirm = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM, uiRoomConfirm);
        }
        return true;
    }

    public bool onCommand_AllConfirmHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomAllConfirm = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, uiRoomAllConfirm);

        return true;
    }

    public bool onCommand_FromGameToHall(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        Trace.Log("玩家在战场退出游戏，直接返回选择角色界面," + (GameLogicDef)cmdID + "目前没有处理这个消息");
        return true;
    }

    public bool onCommand_MatchLoadSenecePlayerInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchLoadSenecePlayerInfo=================== : {0}", nParam));
            return false;
        }
        else
        {
            // 保存VS数据
            LogicDataCenter.recordTableDataManager.ClearLoadSenecePlayerVsData();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_Camp_Count cmdCount = helper.get<cmd_Camp_Count>();
            StageManager.Instance.CurPermissionState = cmdCount.bObEnter > 0 ? EStage_PermissionState.ESPS_OB : EStage_PermissionState.ESPS_NORMAL;
            foreach (int nCampCount in cmdCount.vCampCount)
            {
                for (int i = 0; i < nCampCount; ++i)
                {
                    cmd_match_load_scene_playerinfo data = helper.get<cmd_match_load_scene_playerinfo>();
                    LogicDataCenter.recordTableDataManager.UpdatePlayersVsInfo(data);
                }
            }

            // 发到战场Loading界面
            UIMsgCmdData uiLoadPlayerInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_LOADINGWARVIEW_LOADPLAYERINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPLAYERINFO, uiLoadPlayerInfo);
        }
        return true;
    }

    public bool onCommand_LoginResult(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam || nParam != 0)
        {
            Trace.Log(string.Format("==================onCommand_LoginResult=================== : {0}", nParam));
            return false;
        }
        else
        {
            Initialize.Instance.SetState(InitializeState.Play);
            Trace.TraceSFUIWaring("GFxHeroSelect", GameLogicDef.GVIEWCMD_USER_LOGIN_RESULT.ToString(), "PlayerLoginInfo");
        }
        return true;
    }

    public bool onCommand_MatchLoadSceneProgress(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //因为逻辑层是先加载场景->等显示层加载场景结束后回调->切换逻辑层的状态,所以加这个消息用于做切换场景前的操作
        StageManager.Instance.onReadyToLoadMobaScene();
        return true;
    }

    public bool onCommand_MatchHallRemoveAllRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRemoveAll = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, uiRemoveAll);
        return true;
    }

    // 角色名创建成功
    public bool onCommand_UserCreatePlayerResult(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        Initialize.Instance.SetState(InitializeState.Play);

        Trace.TraceSFUIWaring("GFxHeroSelect", GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER_RESULT.ToString(), "PlayerLoginInfo、setHeroDefaultInfo、HeroBtnPress、EnterMovie");
        return true;
    }
    // 当没有英雄时直接跳转到创建界面
    public bool onCommand_JudgeCreateUI(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CREATEHERO_OPEN, null);
        return true;
    }

    // 显示聊天消息
    public bool onCommand_ShowChatMessage(int cmdId, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.LogError("onCommand_ShowChatMessage ptrParam == null");
            return false;
        }
        else
        {
            gamelogic_show_chat_message msgData = IntPtrHelper.toData<gamelogic_show_chat_message>(ptrParam);
            if (msgData.message == null)
            {
                Trace.LogError("onCommand_ShowChatMessage msgData.message == null");
                return false;
            }
            LogicDataCenter.chatMessageManager.AddChatMessage(ref msgData);

            return true;
        }
    }

    // 显示私聊消息
    public bool onCommand_ShowPrivateChatMessage(int cmdId, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.LogError("onCommand_ShowPrivateChatMessage ptrParam == null");
            return false;
        }
        else
        {
            gamelogic_show_private_chat_message msgData = IntPtrHelper.toData<gamelogic_show_private_chat_message>(ptrParam);
            if (msgData.message == null)
            {
                Trace.LogError("onCommand_ShowPrivateChatMessage msgData.message == null");
                return false;
            }
            LogicDataCenter.chatMessageManager.AddPrivateChatMessage(ref msgData);

            return true;
        }
    }

    // 显示系统消息
    public bool onCommand_ShowSystemMessage(int cmdId, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.LogError("onCommand_ShowSystemMessage ptrParam == null");
            return false;
        }
        else
        {
            gamelogic_show_system_message msgData = IntPtrHelper.toData<gamelogic_show_system_message>(ptrParam);
            if (msgData.message == null)
            {
                Trace.LogError("onCommand_ShowSystemMessage msgData.message == null, tipid=" + msgData.nIndexId);
                return false;
            }

            // 系统消息位置定义
            if ((msgData.nPos & (int)EMChatInfoPos.CHAT_INFOPOS_CHATWINDOW) > 0)
            {
                gamelogic_show_chat_message systemMsg = new gamelogic_show_chat_message();
                systemMsg.channel = msgData.nPlusData;
                systemMsg.message = msgData.message;
                LogicDataCenter.chatMessageManager.AddChatMessage(ref systemMsg);
            }
            if ((msgData.nPos & (int)EMChatInfoPos.CHAT_INFOPOS_SKILL_POS1) > 0)
            {
                USystemFloatTextData data = new USystemFloatTextData();
                data.message = msgData.message;
                data.nPos = 1;
                data.nShowTime = msgData.nShowTime;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, data);
            }
            if ((msgData.nPos & (int)EMChatInfoPos.CHAT_INFOPOS_SKILL_POS2) > 0)
            {
                USystemFloatTextData data = new USystemFloatTextData();
                data.message = msgData.message;
                data.nPos = 2;
                data.nShowTime = msgData.nShowTime;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, data);
            }
            if ((msgData.nPos & (int)EMChatInfoPos.CHAT_INFOPOS_UNDER_SCREEN) > 0)
            {
                USystemFloatTextData data = new USystemFloatTextData();
                data.message = msgData.message;
                data.nPos = 3;
                data.nShowTime = msgData.nShowTime;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, data);
            }
            if ((msgData.nPos & (int)EMChatInfoPos.CHAT_INFOPOS_MESSAGE_BOX) > 0)
            {
                UIDialogData data = new UIDialogData(
                    title: msgData.title,
                    message: msgData.message,
                    buttons: LogicDataCenter.dialogDataManager.GetNewAction(EMessageBoxType.EMBT_OK)
                    );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

            }

        }
        return true;
    }

    // 显示喇叭
    public bool onCommand_ShowLoudspeaker(int cmdId, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.LogError("onCommand_ShowSystemNotice ptrParam == null");
            return false;
        }
        else
        {
            gamelogic_show_loudspeaker msgData = IntPtrHelper.toData<gamelogic_show_loudspeaker>(ptrParam);
            if (msgData.message == null)
            {
                Trace.LogError("onCommand_ShowSystemMessage msgData.message == null, tipid=" + msgData.nIndexId);
                return false;
            }

            LogicDataCenter.loudspeakerManager.AddLoudspeakerMessage(ref msgData);
        }
        return true;
    }


    public bool onCommand_MatchRoomState(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomState = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE, uiRoomState);
        return true;
    }
    public bool onCommand_MatchStartRoomRule(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE, uiRoomRule);

        return true;
    }

    public bool onCommand_MatchRoomRuleBanHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("===============onCommand_MatchRoomRuleBanHero===================:{0}", nPtrLen));
            return false;
        }
        else
        {
            UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO, uiRoomRule);
        }
        return true;
    }

    public bool onCommand_MatchRoomRuleBanHeroList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST, uiRoomRule);

        return true;
    }

    public bool onCommand_MatchRoomGlobleBanHeroList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST, uiRoomRule);

        return true;
    }

    public bool onCommand_MatchRoomRuleSelectHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("===============onCommand_MatchRoomRuleSelectHero===================:{0}", nParam));
            return false;
        }
        else
        {
            UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO, uiRoomRule);
        }
        return true;
    }

    public bool onCommand_Can_Select(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_Can_Select===================================:{0}" + nParam);
            return false;
        }
        else
        {
            UIMsgCmdData uiRoomRule = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO, uiRoomRule);
        }
        return true;
    }

    public bool onCommand_AllSummonerSkill(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_AllSummonerSkill===================================:{0}" + nParam);
            return false;
        }
        else
        {
            UIMsgCmdData uiRoomAllSummer = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO, uiRoomAllSummer);
        }

        return true;
    }

    public bool onCommand_DefSkill(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_DefSkill===================================:{0}" + nParam);
            return false;
        }
        else
        {
            UIMsgCmdData uiSummerInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO, uiSummerInfo);
        }

        return true;
    }

    public bool onCommand_MatchLoadSceneProgreeseRate(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_MatchLoadSceneProgreeseRate===================================:{0}" + nParam);
            return false;
        }
        else
        {
            UIMsgCmdData uiLoadRateInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_LOADINGWARVIEW_LOADPROGREESERATE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPROGREESERATE, uiLoadRateInfo);
        }
        return true;
    }

    public bool onCommand_ShowMessageBox(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_ShowMessageBox====failed===============================:{0}" + nParam);
            return false;
        }
        else
        {
            //Trace.TraceSFUIWaring("GFxBaseHud", GameLogicDef.GVIEWCMD_SHOW_MESSAGEBOX.ToString(), "CreateMessageBox");
            cmd_show_message_box cmdData = IntPtrHelper.toData<cmd_show_message_box>(ptrParam);
            ////WIll-TODO :目前逻辑层返回的字符串类型的方法名，先临时做个处理
            //目前好像只用到强制登录
            UIDialogData data = new UIDialogData(
                       title: cmdData.caption,
                       message: cmdData.text,
                       buttons: LogicDataCenter.dialogDataManager.GetNewAction(EMessageBoxType.EMBT_OKCancel, cmdData.okCallBack),
                       modal: true
                       );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
        return true;
    }

    // 画动态障碍 
    public bool onCommand_DrawDynamicObstacle(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_DrawDynamicObstacle=================== : {0}", nParam));
        }
        else
        {

            cmd_DynamicObstacle_Info data = IntPtrHelper.toData<cmd_DynamicObstacle_Info>(ptrParam);
            // 在这调用你的接口数据在data里了
            EntityFactory.Instance.DrawDynamicObstacle(data);
        }
        return true;
    }

    /// <summary>
    /// 接收战场里用到的皮肤ID
    /// </summary>
    /// <param name="cmdID"></param>
    /// <param name="nParam"></param>
    /// <param name="strParam"></param>
    /// <param name="ptrParam"></param>
    /// <param name="nPtrLen"></param>
    /// <returns></returns>
    public bool onCommand_PreLoadSkin(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count cmdCount = helper.get<cmd_count>();
        SkinManager.ClearCacheSkinID();
        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            int nSkinID = helper.get<int>();

            if (nSkinID != 0)
            {
                SkinPreLoadInfo info = new SkinPreLoadInfo();
                info.skinID = nSkinID;
                info.nMask |= (int)CachePriority.PRIORITY_VERY_HIGH;
                // 预加载皮肤
                SkinHelper.PreLoadPlayerSkin(info);
            }
        }

        return true;
    }

    public bool onCommand_Computer_Can_Select(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_Computer_Can_Select===================================:{0}" + nParam);
            return false;
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK, uiRoom);
        }
        return true;
    }

    public bool onCommand_RoomComputerChange(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomComputerChange=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE, uiRoom);
        }
        return true;
    }

    public bool onCommand_MatchRoomRuleBanComputerList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RoomComputerChange=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoom = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN, uiRoom);
        }
        return true;
    }

    public bool onCommand_LegendCupRoom_WaitCountdown(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendCupRoom_WaitCountdown=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRoomWaitCountdown = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN, uiRoomWaitCountdown);
        }
        return true;
    }

    public bool onCommand_BanHero_Countdown(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 禁用英雄倒计时
        UIMsgCmdData uiBanHeroCountdown = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN, uiBanHeroCountdown);

        return true;
    }

    public bool onCommand_SingleBanHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SingleBanHero=================== : {0}", nParam));
        }
        else
        {
            // 单独禁用英雄
            UIMsgCmdData uiSingleBanHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO, uiSingleBanHero);
        }
        return true;
    }

    public bool onCommand_AllBanHero(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AllBanHero=================== : {0}", nParam));
        }
        else
        {
            // 全部禁用英雄
            UIMsgCmdData uiAllBanHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO, uiAllBanHero);
        }
        return true;
    }

    public bool onCommand_LgendCupWinMsgInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LgendCupWinMsgInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.warInfoDataManager.recvWarEndLegendCupInfo(ptrParam, nParam);
            LogicDataCenter.warInfoDataManager.showLegendCupMsgInfo();
        }
        return true;
    }


    public bool onCommand_CanBanHero_Flag(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiBanHero = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG, uiBanHero);
        return true;
    }

    public bool onCommand_RequestKinList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestKinList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinListData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_UpdateKinList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateKinList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnUpdateKinListData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestKinMemberList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestKinMemberList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinMemberList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendCupKinInfoRecv(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendCupKinInfoRecv=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvLegendKinInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendCupKinGloryRecv(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendCupKinGloryRecv=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvLegendKinGlory(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestKinBaseData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestKinBaseData=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinBaseData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RecvKinDismiss(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.kinDataManager.onRecvKinDismiss(ptrParam, nPtrLen);
        return true;
    }

    public bool onCommand_RequestKinApplyList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestKinApplyList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecevKinApplyList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinRemoveApply(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinRemoveApply=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnProcessKinApplyMember(ptrParam, nPtrLen, (int)ProcessKinApply.PKM_APPLY_REMOVE);
        }
        return true;
    }

    public bool onCommand_KinAddApply(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinAddApply=================== : {0}", nParam));
        }
        else
        {

            LogicDataCenter.kinDataManager.OnProcessKinApplyMember(ptrParam, nPtrLen, (int)ProcessKinApply.PKM_APPLY_ADD);

            //GameContextManager.UGameService.recvAddApply(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinRemoveMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RemoveMember=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnProcessKinMember(ptrParam, nPtrLen, (int)ProcessKinMember.PKM_MEMBER_REMOVE);
        }
        return true;
    }


    public bool onCommand_KinAddMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AddMember=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnProcessKinMember(ptrParam, nPtrLen, (int)ProcessKinMember.PKM_MEMBER_ADD);
        }
        return true;
    }

    public bool onCommand_KinUpdateMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinUpdateMember=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnProcessKinMember(ptrParam, nPtrLen, (int)ProcessKinMember.PKM_MEMBER_UPDATE);
            LogicDataCenter.clanDataManager.OnUpdateClanMember(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_UpdateKinSelf(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateKinSelf=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnUpdateSelfKinInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinCreateSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinCreateSuccess=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_KIN_CREATESUCCESS;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_CREATESUCCESS, UIData);
        }
        return true;
    }

    public bool onCommand_KinJoinSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinJoinSuccess=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_KIN_JOIN_SUCCESS;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_JOIN_SUCCESS, UIData);
        }
        return true;
    }

    public bool onCommand_KinAnswerInviteSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_KIN, "", IntPtr.Zero, 0);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        return true;
    }

    public bool onCommand_KinQuit(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinQuit=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnQuitKin();
        }
        return true;
    }

    public bool onCommand_KinStringPropUpdate(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinStringPropUpdate=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnKinStringPropUpdate(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinNumPropUpdate(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinNumPropUpdate=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnKinNumPropUpdate(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_AutoAcceptSave(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AutoAcceptSave=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvAutoAccept(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_AutoAcceptRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AutoAcceptRequest=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvAutoAccept(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinEventListRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinEventListRequest=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinEventListData(nParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinGloryDataRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinGloryDataRequest=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinGloryData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_KinWarRecordTotalRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinWarRecordTotalRequest=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvTotalWarRecord(ptrParam, nPtrLen);
        }
        return true;
    }


    public bool onCommand_RequestClanList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestClanList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanListData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanBaseData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanBaseData=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanBaseData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanSetBaseData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanSetBaseData=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanSetBaseData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanApplyList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanApplyList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanApplyList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanRemoveApply(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanRemoveApply=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.ClanRemoveApply(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanAddApply(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanAddApply=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.ClanAddApply(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanRemoveMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanRemoveMember=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanRemoveMember(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanAddMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanAddMember=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanAddMember(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanUpdate(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanUpdate=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanUpdate(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestCardList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestCardList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvCardList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_UpdateRequestCard(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateRequestCard=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvCardListUpdate(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RemoveRequestCard(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RemoveRequestCard=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvCardListRemove(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestDonateLog(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestDonateLog=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvRequestDonateLog(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_UpdateDonateLog(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateDonateLog=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvUpdateDonateLog(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_AddRequestcardTip(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AddRequestcardTip=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvAddRequestcardTip(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestcardInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestcardInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvRequestcardInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanCreateSucceed(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanCreateSucceed=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_CLAN_CREATESUCCEED;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_CREATESUCCEED, UIData);
        }
        return true;
    }

    public bool onCommand_ClanOnQuit(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.clanDataManager.OnRecvClanQuit();
        return true;
    }

    public bool onCommand_ClanEventListRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanEventListRequest=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvClanEventListData(nParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanApplyRet(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanApplyRet=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_CLAN_APPLYRET;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLYRET, UIData);
        }
        return true;
    }

    public bool onCommand_ClanSetAutoAcceptFlag(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanSetAutoAcceptFlag=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvSetAutoAcceptFlag(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ClanStateNotice(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanStateNotice=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.clanDataManager.OnRecvSetClanState(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_ClanCreateLegendCupDida(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.clanDataManager.ClanShaikhCreateLengedCupDida(cmdID, nParam,strParam, ptrParam, nPtrLen);
        return true;
    }

    public bool onCommand_TeamSubInvited(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamSubInvited=================== : {0}", nParam));
        }
        else
        {
            // 显示邀请提示侧边栏
            LogicDataCenter.teamDataManager.onTeamSubInvitedSideButton(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_TeamSubRequest(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamSubRequest=================== : {0}", nParam));
        }
        else
        {
            // 显示请求提示侧边栏
            LogicDataCenter.teamDataManager.onTeamSubRequestSideButton(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_TeamUpdateTeamMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamUpdateTeamMember=================== : {0}", nParam));
        }
        else
        {
            // 提示显示层显示队友信息
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE, UIData);
        }

        return true;
    }

    public bool onCommand_TeamChangeCaptain(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamChangeCaptain=================== : {0}", nParam));
        }
        else
        {
            // 提示显示层改变队长信息
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN, UIData);
        }

        return true;
    }

    public bool onCommand_TeamKickoutMember(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamKickoutMember=================== : {0}", nParam));
        }
        else
        {
            // 提示显示层踢出队员信息
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER, UIData);
        }

        return true;
    }

    public bool onCommand_TeamLeaveTeam(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamLeaveTeam=================== : {0}", nParam));
        }
        else
        {
            // 提示显示层离开队员信息
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM, UIData);
        }

        return true;
    }

    public bool onCommand_TeamDisbandTeam(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 提示显示层解散队伍信息
        UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
        UIData.msgID = (int)WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM, UIData);

        return true;
    }

    public bool onCommand_MatchWndShow(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchWndShow=================== : {0}", nParam));
        }
        else
        {
            // 显示等待时间界面
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, UIData);
        }

        return true;
    }

    public bool onCommand_MatchWndHide(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 隐藏等待时间界面
        UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
        UIData.msgID = (int)WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE, UIData);

        return true;
    }

    public bool onCommand_CurMatchCountInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CurMatchCountInfo=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT, uiData);
        }
        return true;
    }
    

    public bool onCommand_MatchOK(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 撮合成功
        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
        return true;
    }

    public bool onCommand_MatchFail(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 撮合失败
        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
        return true;
    }

    public bool onCommand_MatchRequestPunishInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // 惩罚信息
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchRequestPunishInfo=================== : {0}", nParam));
        }
        else
        {
            // 显示等待时间界面
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_PUNISH_TIMEINFO_SHOW;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PUNISH_TIMEINFO_SHOW, UIData);
        }

        return true;
    }

    // 惩罚信息
    public bool onCommand_SelfPunishInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SelfPunishInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.matchDataManager.OnRecvSelfPunishInfo(ptrParam, nPtrLen);
        }

        return true;
    }

    // 惩罚信息
    public bool onCommand_DeletePunishInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.matchDataManager.OnRecvDeletePunishInfo(nParam);
        return true;
    }

    public bool onCommand_MatchCloseOrOpen(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MatchCloseOrOpen=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.matchDataManager.OnRecvMatchTypeOpenOrClose(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_MatchRecommendRank(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.matchDataManager.OnRecvRecommendRank();
        return true;
    }

    public bool onCommand_RequestRandomNameResult(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_TeamSubRequest=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_CREATEHERO_RANDOMNAMERESULT;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CREATEHERO_RANDOMNAMERESULT, UIData);
        }

        return true;
    }

    public bool onCommand_ShowCommonMessageBox(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_ShowCommonMessageBox====failed===============================:{0}" + nParam);
            return false;
        }
        else
        {
            cmd_show_common_message_box cmdData = IntPtrHelper.toData<cmd_show_common_message_box>(ptrParam);
            UIDialogData data = new UIDialogData(
                       title: cmdData.szCaption,
                       message: cmdData.szText,
                       buttons: new UIWidgets.DialogActions() { { "确认", null } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
        return true;
    }

    public bool onCommand_AntiAddictRegistResult(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AntiAddictRegistResult=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptrParam, nPtrLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_ANTIADDICT_SHOWRESULT;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWRESULT, UIData);
        }

        return true;
    }

    public bool onCommand_AntiAddictTip(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AntiAddictTip=================== : {0}", nParam));
        }
        else
        {
            gamelogic_antiaddict_tip cmdData = IntPtrHelper.toData<gamelogic_antiaddict_tip>(ptrParam);
            if (cmdData.szRetDes == null)
            {
                return false;
            }

            // 聊天栏直接广播
            gamelogic_show_chat_message msgData = new gamelogic_show_chat_message();
            msgData.channel = (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM;
            msgData.message = cmdData.szRetDes;
            LogicDataCenter.chatMessageManager.AddChatMessage(ref msgData);

            // 弹窗要分是否在战场分别处理
            LogicDataCenter.sideButtonDataManager.StrAddictTip = cmdData.szRetDes;
            if (StageManager.Instance.getCurrentState() == EStage_State.ESS_GAME)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWTIP, null);
            }
        }

        return true;
    }

    // 显示任务列表
    public static bool onCommand_ShowTaskList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvTaskListData(strParam);
        return true;
    }

    // 更新任务列表
    public static bool onCommand_UpdateTaskList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnUpdateTaskListData(strParam);
        return true;
    }

    // 删除任务列表
    public static bool onCommand_DeleteTaskList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnDeleteTaskListData(strParam);
        return true;
    }

    // 发送任务奖励
    public static bool onCommand_SendTaskPrize(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvSendTaskPrize(strParam);
        return true;
    }


    // 查看任务基础
    public static bool onCommand_ViewTaskBase(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ViewTaskBase=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.taskDataManager.OnRecvTaskBase(ptrParam, nPtrLen);
        return true;
    }

    // 查看任务回复
    public static bool onCommand_ViewTaskReply(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ViewTaskReply=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.taskDataManager.OnRecvTaskReply(ptrParam, nPtrLen);
        return true;
    }

    // 添加任务成功
    public static bool onCommand_PublishTaskSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvPublishTask(strParam);
        return true;
    }

    // 预捕获任务
    public static bool onCommand_PreCatchTask(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvPreCatchTask(strParam);
        return true;
    }

    // 捕获任务
    public static bool onCommand_CatchTask(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvCatchTask(strParam);
        return true;
    }

    // 刷新任务操作
    public static bool onCommand_ReflushOptTask(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.taskDataManager.OnRecvReflushTaskOpt(strParam);
        return true;
    }

    // 玩家发布任务
    public static bool onCommand_ActorPublishContact(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ActorPublishContact=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.taskDataManager.OnRecvActorPublishContact(ptrParam, nPtrLen);
        return true;
    }

    // 通知交互数据已删除
    public static bool onCommand_SendContactDataDelete(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SendContactDataDelete=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.taskDataManager.OnRecvContactDataDelete(ptrParam, nPtrLen);
        return true;
    }


    public static bool onCommand_EnterVoiceRoom(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_EnterVoiceRoom=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_enter_room data = IntPtrHelper.toData<cmd_voice_enter_room>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.EnterRoom(data);
        }

        return true;
    }

    public static bool onCommand_UpdateVoiceRoomBaseInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateVoiceRoomBaseInfo=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_update_room_base_info data = IntPtrHelper.toData<cmd_voice_update_room_base_info>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.UpdateRoomBaseInfo(data);
        }

        return true;
    }

    public static bool onCommand_UpdateVoiceRoomInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateVoiceRoomInfo=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_update_room_info data = IntPtrHelper.toData<cmd_voice_update_room_info>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.UpdateRoomInfo(data);
        }

        return true;
    }

    public static bool onCommand_UpdateVoiceUserInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateVoiceUserInfo=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_update_user_info data = IntPtrHelper.toData<cmd_voice_update_user_info>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.UpdateUserInfo(data);
        }

        return true;
    }

    public static bool onCommand_UpdateVoiceUserSpeakingState(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateVoiceUserSpeakingState=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_update_user_ispeaking data = IntPtrHelper.toData<cmd_voice_update_user_ispeaking>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.UpdateUserSpeakingState(data);
        }

        return true;
    }

    public static bool onCommand_UpdateVoiceChannelList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateVoiceUserInfo=================== : {0}", nParam));
            return false;
        }
        else
        {
            cmd_voice_update_channel_list data = IntPtrHelper.toData<cmd_voice_update_channel_list>(ptrParam);
            LogicDataCenter.voiceSystemDataManager.UpdateChannelList(data);
        }

        return true;
    }

    public bool onCommand_RequestIssueList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestIssueList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvMentorshipIssueList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestApplyAnswer(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestApplyAnswer=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvMentorshipApplyAnswer(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestMentorshipDismiss(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestMentorshipDismiss=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvMentorshipDismiss(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_UpdateMentorshipInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateMentorshipInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvUpdateMentorshipInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_MentorshipUpdatePersonBase(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MentorshipUpdatePersonBase=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvUpdatePersonBaseInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_MentorshipUpdateShipList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MentorshipUpdateShipList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvUpdateMentorshipList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_RequestPrenticeList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestPrenticeList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mentorshipDataManager.OnRecvMasterPrenticeList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvCupList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvCupList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvTotalLegendCup(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvRegistKinList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvRegistKinList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupRegistKinList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvRegistKinMemberList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvRegistKinMemberList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupRegistKinMemberList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvRegistBlacklist(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvRegistBlacklist=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupRegistBlacklist(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvCompetitionKin(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvCompetitionKin=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupCompetitionKinMember(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvCompetitionNode(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvCompetitionNode=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupCompetitionInfo(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupRecvCompetitionResult(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvCompetitionResult=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupNodeWarRecord(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_LegendcupUpdateSingleCup(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupUpdateSingleCup=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.UpdateSingleCup(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_LegendcupUpdateSingleCompetitionNode(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupUpdateSingleCompetitionNode=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.UpdateCupCompetitionInfo(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_LegendcupRegisterSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.legendCupDataManager.RecvRegistSuccess();

        return true;
    }

    public bool onCommand_LegendcupCreateTypeInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupCreateTypeInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCreateCupTypeInfo(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_LegendcupCreaterPublicSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.legendCupDataManager.RecvCupCreaterPublicSuccess();

        return true;
    }

    public bool onCommand_LegendcupPublicSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.legendCupDataManager.RecvCupPublicSuccess();

        return true;
    }

    public bool onCommand_LegendcupSyytemCancel(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupSyytemCancel=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.SystemCancelCup(ptrParam, nPtrLen);
        }

        return true;
    }
    

    public bool onCommand_LegendcupRecvRoundTime(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_LegendcupRecvRoundTime=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.legendCupDataManager.RecvCupCompetitionAllRoundTime(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_MsgData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_MsgData=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvMsgData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_SNS_RelationshipStatus(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_RelationshipStatus=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvUpdateRelationship(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_SNS_BuddyStatus(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddyStatus=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyStatus(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_SNS_AllBuddyInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_AllBuddyInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_SNS_AllBuddyGroupInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_AllBuddyGroupInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyGroupList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_SNS_BuddyRelationList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddyRelationList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyRelationList(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_BuddyRelation(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddyRelation=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyRelation(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_Buddy(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_Buddy=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddy(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_BuddyRemark(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddyRemark=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyRemark(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_BuddySelf(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddySelf=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddySelf(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_UserInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_UserInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvUserInfo(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_BuddyInfoChagne_Notify(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_BuddyInfoChagne_Notify=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvBuddyInfoChange(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_InvitePlayGame_Notify(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_InvitePlayGame_Notify=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvInvitePlayGame(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_SNS_InviteMatchTeam_Notify(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_InviteMatchTeam_Notify=================== : {0}", nParam));
        }
        else
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            GameLogic.gamelogic_sns_invite_match_team_notify cmdData = helper.get<GameLogic.gamelogic_sns_invite_match_team_notify>();
            LogicDataCenter.teamDataManager.InviteJoin(cmdData.dwTargetActorID);
        }

        return true;
    }

    public bool onCommand_SNS_UpdateScene_Notify(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SNS_InvitePlayGame_Notify=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.snsDataManager.onRecvUpdateScene(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_KinRecvKinInvite(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_KinRecvKinInvite=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.kinDataManager.OnRecvKinInvite(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_Mail_ReqBastInfoList(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Mail_ReqBastInfoList=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mailDataManager.OnRecvMailBaseInfoList(ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_Mail_RspDetailData(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Mail_RspDetailData=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.mailDataManager.OnRecvMailDetailInfo(cmdID, nParam, strParam, ptrParam, nPtrLen);
        }

        return true;
    }

    public bool onCommand_Mail_NewMailCount(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Mail_NewMailCount=================== : {0}", nParam));
        }
        else
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            GameLogic.gameview_mail_newmailcount cmdData = helper.get<GameLogic.gameview_mail_newmailcount>();

            LogicDataCenter.mailDataManager.MailCount = cmdData.nCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_NEWMAILCOUNT, null);

            if (cmdData.nPlusCount > 0)
            {
                PrizeTipData tipData = new PrizeTipData();
                tipData.eTipType = EMPrizeTipType.EMPrizeTipType_Mail;
                tipData.nPrizeID = 0;

                LogicDataCenter.prizeTipDataManager.AddItem(tipData);
            }
        }

        return true;
    }

    public bool onCommand_Mail_ReqFastRecvPlus(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Mail_ReqFastRecvPlus=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiFastRecvPlus = new UIMsgCmdData((int)WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS, uiFastRecvPlus);
        }

        return true;
    }

    public bool onCommand_Mail_RecvSuccess(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Mail_RecvSuccess=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRecvSuccess = new UIMsgCmdData((int)WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS, uiRecvSuccess);
        }

        return true;
    }

    public bool onCommand_setRobotMode(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null)
        {
            gameview_set_robot_mode data = IntPtrHelper.toData<gameview_set_robot_mode>(ptrParam);
            EntityFactory.MainHeroView.StateMachine.bControlledBySever = (data.nEnable == 1);
        }
        return true;
    }

    public bool onCommand_SetCameraHeight(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        gameview_set_camera_height data = IntPtrHelper.toData<gameview_set_camera_height>(ptrParam);

        if (SoldierCamera.MainInstance<SoldierCamera>() != null)
        {
            Vector3 newAngle = SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle;
            newAngle.x = data.fHeight;
            SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle = newAngle;
        }
        return true;
    }

    public bool onCommand_Recv_District_List(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Recv_District_List=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.districtListDataManger.RecvDistrictListData(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_Recv_Select_server_result(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Recv_Select_server_result=================== : {0}", nParam));
        }
        else
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            gamelogic_district_info result = helper.get<gamelogic_district_info>();
            LogicDataCenter.districtListDataManger.UpdateDistrictInfo(result);
        }
        return true;
    }

    public bool onCommand_Check_Game_version(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Check_Game_version=================== : {0}", nParam));
        }
        else
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            gamelogic_game_version result = helper.get<gamelogic_game_version>();
            Initialize.Instance.CheckGameVersionID(result.nVersion);
        }
        return true;
    }

    public bool onCommand_Recv_LastEnter_District_List(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Recv_LastEnter_District_List=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.districtListDataManger.UpdateLastEnterDistrictList(ptrParam, nPtrLen);
        }
        return true;
    }

    public bool onCommand_ReportOnRightButtonDown(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (nParam == 0)
        {
            // 取消右键上报 todo.
            InputManager.OpenReportOnRightBtnDown = false;
        }
        else
        {
            // 开启右键上报 todo.
            InputManager.OpenReportOnRightBtnDown = true;
        }

        return true;
    }

    public static bool onCommand_Equip_Info_Table(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Equip_Info_Table=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warMobaGoodsDataManager.UpdatePlayersGoods(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_SwitchBulletPanel(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SwitchBulletPanel=================== : {0}", nParam));
            return false;
        }

        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_switch_bullet_count_panel result = helper.get<cmd_switch_bullet_count_panel>();
        
        // result.nOpen 1:开  0:关
        UFPSBulletVisibleData data = new UFPSBulletVisibleData();
        data.bShow = result.nOpen == 0 ? false : true;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FPSBULLET_VISIBLE, data);

        LogicDataCenter.fpsBulletDataManager.SetMaxBulletCount(result.nMaxCount);

        return true;
    }

    public static bool onCommand_CreatePlayerResult(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CreatePlayerResult=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_CREATEHERO_CREATE_PLAYER_RESULT, nParam, strParam, ptrParam, nPtrLen);

        UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_CREATEHERO, WndMsgID.WND_MSG_CREATEHERO_CREATE_PLAYER_RESULT, uiData);

        return true;
    }

    public static bool onCommand_ClanInviteReturn(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ClanInviteReturn=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.clanDataManager.OnClanInviteReturn(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_AnswerInviteSuccess(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_CLAN, "", IntPtr.Zero, 0);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);

        return true;
    }

    public static bool onCommand_WarSafetyZoneInfo(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_WarSafetyZoneInfo=================== : {0}", nParam));
            return false;
        }

        gamelogic_war_safetyzone_info data = IntPtrHelper.toData<gamelogic_war_safetyzone_info>(ptrParam);

        Debug.Log(String.Format("毒气圈({0},{1},{2})-半径:{3}, 安全圈圆心({4},{5},{6})-半径:{7},服务器下发ticks={8},扩散时间={9}", data.fCenterOfNoxZoneX, data.fCenterOfNoxZoneY, data.fCenterOfNoxZoneZ,
            data.fRadiusOfNoxZone, data.fCenterOfSafetyZoneX, data.fCenterOfSafetyZoneY, data.fCenterOfSafetyZoneZ, data.fRadiusOfSafetyZone,
            data.nSpreadBeginTick, data.nSpreadTime));

        Vector3 SafeZoneCenter = new Vector3(data.fCenterOfSafetyZoneX, data.fCenterOfSafetyZoneY, data.fCenterOfSafetyZoneZ);
        float safeZoneRadius = data.fRadiusOfSafetyZone;
        Vector3 NoxSafeZoneCenter = new Vector3(data.fCenterOfNoxZoneX, data.fCenterOfNoxZoneY, data.fCenterOfNoxZoneZ);
        float NoxSafeZoneRadius = data.fRadiusOfNoxZone;
        float SpreadBeginTick = data.nSpreadBeginTick;
        float SpreadTime = data.nSpreadTime;


        SpreadTime = SpreadTime * 0.001f;

        SafeZone.dstCenter = SafeZoneCenter;
        SafeZone.dstRadius = safeZoneRadius;
        SafeZone.srcCenter = NoxSafeZoneCenter;
        SafeZone.srcRadius = NoxSafeZoneRadius;
        SafeZone.StartedSpreadTimeInSecond = (GameLogicAPI.getSyncTickCount() - data.nSpreadBeginTick) * 0.001f;
        SafeZone.SpreadTimeInSecond = SpreadTime;
        SafeZone.isSpreadStarted = data.isSpreadBegin > 0;

        // 是否毒气圈开始扩散
        if (data.isSpreadBegin > 0)
        {
            SafeZoneEffectManager.StartSpreadNoxSafeZone();
            UIMsgCmdData msg = new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, msg);
        }
        else
        {
            SafeZoneEffectManager.SetNoxSafeZone();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETNOXSAFEZONE);
        }

        return true;
    }

    public static bool onCommand_PlayerReconnection(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_PlayerReconnection=================== : {0}", nParam));
            return false;
        }

        gamelogic_war_player_reconnection data = IntPtrHelper.toData<gamelogic_war_player_reconnection>(ptrParam);
        UIReconnectData msgData = new UIReconnectData();
        msgData.msgID = (int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION;
        msgData.nActorID = data.actorID;
        msgData.nHeroID = data.heroID;
        msgData.szActorName = data.actorName;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION, msgData);

        return true;
    }

    // 毒气扩散倒计时信息
    public static bool onCommand_WarNoxSpreadCountDown(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_WarNoxSpreadCountDown=================== : {0}", nParam));
            return false;
        }

        gamelogic_war_nox_spread_cd data = IntPtrHelper.toData<gamelogic_war_nox_spread_cd>(ptrParam);

        Debug.Log("毒气扩散倒计时信息,tick=" + data.nSpreadCDTick + ",time=" + data.nSpreadTotalCDTime);

        // TODO...
        // 当前剩余倒计时 = (data.nSpreadTotalCDTime - (GameLogicAPI.getSyncTickCount() - data.nSpreadCDTick))


        UIMsgCmdData msg = new UIMsgCmdData((int)WndMsgID.WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN, msg);

        return true;
    }

    // 服务器War直接下发到显示层的消息
    public static bool onCommand_WarServerToViewMsg(Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ServerWarToViewMsg=================== : {0}", nParam));
            return false;
        }

        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

        gamelogic_war_server_toview_msg data = helper.get<gamelogic_war_server_toview_msg>();


        EServerWar2ViewMsg msgId = (EServerWar2ViewMsg)data.nKeyAction;
        Debug.Log("WarServer 发到显示层的消息：msgId=" + msgId);

        switch(msgId)
        {
            case EServerWar2ViewMsg.WAR2VIEW_MSG_BATTLEWAR_LIFESTATE:
                {
                    SWar2ViewMsgPersonLifeState msg = helper.get<SWar2ViewMsgPersonLifeState>();
                    Debug.Log("玩家生命状态：pdbid=" + msg.pdbid + ",生命状态=" + (ELifeState)msg.nLifeSate + ",阵营=" + msg.nCamp + ",本地Pidbid=" + EntityUtil.getMainHeroPdbid());

                    UBattleWarLifeState msgData = new UBattleWarLifeState();
                    msgData.lifeState = msg;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE, msgData);
                }
                break;
            case EServerWar2ViewMsg.WAR2VIEW_MSG_BATTLEWAR_ACE:
                {
                    SWar2ViewMsgAce msg = helper.get<SWar2ViewMsgAce>();
                    Debug.Log("团灭：阵营=" + msg.nCamp + ",nRank=" + msg.nRank + ",killNum=" + msg.nKillNum);
                    LogicDataCenter.recordTableDataManager.SelfCampACE = msg;
                }
                break;
            case EServerWar2ViewMsg.WAR2VIEW_MSG_BATTLEWAR_GENIC_UPDATE:
                {
                    SWar2ViewMsgGenicUpdate genicData = helper.get<SWar2ViewMsgGenicUpdate>();
                    UBattleWarCollectGenic msg = new UBattleWarCollectGenic();
                    msg.genicData = genicData;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_BATTLEWAR_COLLECTING_GENIC, msg);
                }
                break;
            case EServerWar2ViewMsg.WAR2VIEW_MSG_WAR_PERSON_INFO_UPDATE:
                {
                    SWar2ViewMsgPersonInfoUpdate msg = helper.get<SWar2ViewMsgPersonInfoUpdate>();

                    LogicDataCenter.recordTableDataManager.updatePersonInfo((ulong)msg.pdbid, (EWarPersonInfoUpdateType)msg.nUpdateType, helper);
                }
                break;
            case EServerWar2ViewMsg.WAR2VIEW_MSG_BATTLEWAR_DEVILKING_UPDATE:
                {
                    SWar2ViewMsgDevilKingUpdate msg = helper.get<SWar2ViewMsgDevilKingUpdate>();
                    UIBattleWarUpdateDevilking msgData = new UIBattleWarUpdateDevilking();
                    msgData.data = msg;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_BATTLEWAR_UPDATE_DEVILKING, msgData);
                }
                break;
            case EServerWar2ViewMsg.WAR2VIEW_MSG_WAR_XP_BEGIN_CD:
                {
                    SWar2ViewMsgXPBeginCD msg = helper.get<SWar2ViewMsgXPBeginCD>();

                    if(msg.pdbid <= 0)
                    {
                        break;
                    }

                    if(msg.pdbid == EntityUtil.getMainHeroPdbid())
                    {
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL, null);
                    }
                    else
                    {
                        // OB处理
                        if(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
                        {
                            Trace.LogFormat("XPSkill data come.{0},{1},{2},{3}", msg.pdbid, msg.nSkillID, msg.uID, msg.nCDTime);
                            UIXPSkillCD xpSkilCD = new UIXPSkillCD();
                            xpSkilCD.data = msg;
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_XP_SKILL_UPDATE, xpSkilCD);
                        }
                    }

                }
                break;
            default:
                {
                    Debug.LogError("Unkown msgid," + msgId);
                    break;
                }
        }

        return true;
    }
}