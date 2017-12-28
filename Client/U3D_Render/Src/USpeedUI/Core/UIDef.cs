/*******************************************************************
** 文件名:	UIDef.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	窗口的Wnd和相关消息定义

		    每新增一个窗口都需要
                1、UWindowWnd.CreateWindowWnd 里实例化对应的Wnd类
                2、WndID 里定义该窗口的WndID值
                3、WndMsgID 里定义与该窗口相关的消息
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using DataCenter;

namespace USpeedUI
{
    public class UWindowWnd
    {
        public static IEnumerable<IUIWnd> CreateWindowWnd()
        {
            yield return new CommonBlurBG.CommonBlurBGWnd();
            yield return new SNS.SNSWnd();
            yield return new SNS.SNSChatWnd();
            yield return new SNS.SNSMainPanelWnd();
            yield return new SNS.SNSFindBuddyWnd();
            yield return new SNS.SNSInputDialogWnd();
            yield return new WarMain.WarMainWnd();
            yield return new PlayerButtonFrame.UPlayerButtonFrame();

            yield return new PlayerCrownWndPart.PlayerCrownWnd();
            yield return new PlayerFrame.UPlayerFrame();

            yield return new HeroDetail.UHeroDetail();
            yield return new SideButtonList.SideButtonListWnd();
            //yield return new UpgradeFrame.UHeroUpStarFrame();
            yield return new HighLadder.UHighLadderWnd();
            yield return new Chart.ChartWnd();

            yield return new FullScreen.UMediatorFullScreen();
            yield return new LoadingBar.UMediatorLoadingBar();
            yield return new LoginAndCreate.UMediatorLogin();
            yield return new LoginAndCreate.UMediatorCreateHero();

            yield return new TooltipAndDialog.UMeidatorDialog();
            yield return new TooltipAndDialog.UMediatorTooltip();

            //yield return new TalentPagePart.TalentPageWnd();
            yield return new Social.SocialWnd();
            yield return new MsgBoxCreateKin.MsgBoxCreateKinWnd();
            yield return new KinAcceptMember.KinAcceptMemberWnd();
            yield return new MsgBoxCreateClan.MsgBoxCreateClanWnd();
            yield return new ClanAcceptMember.ClanAcceptMemberWnd();
            yield return new ClanSelectRequestCard.ClanSelectRequestCardWnd();
            yield return new MsgBoxMasterIssue.MsgBoxMasterIssueWnd();
            yield return new MsgBoxPrenticeIssue.MsgBoxPrenticeIssueWnd();

            yield return new HonorPage.UMediatorHonorPage();
            yield return new TeamRightPart.TeamRightWnd();
            yield return new WarGameHudTopRight.UWarGameHudTopRightWnd();
            yield return new XPSkillWnd.UXPSkill();
            yield return new WarBottonCenterPart.WarBottonCenterWnd();
            yield return new ProgressBarPart.ProgressBarWnd();

            yield return new WarEndOf.UEndOfWarWnd();
            yield return new WarRecordTableWarning.UWarRecordTableFighting();
            yield return new WarHeroDetailedInfoPanel.UWarHeroDetailedInfoPanelWnd();
            yield return new GameWaitingRoomPart.GameWaitingRoomWnd();
            yield return new LoadingWarPart.LoadingWarWnd();
            yield return new WarReliveTime.UWarReliveTimeWnd();
            yield return new PopupMenu.UPopupMenuWnd();
            yield return new GameHallPart.GameHallWnd();
            yield return new WarRecordTableEnd.UWarRecordTableEnd();
            yield return new WarBroadcastMessageMgr.UWarBroadcastMessageMgrWnd();
            yield return new WarComboAttack.UWarComboAttckWnd();
            yield return new DragonBall.UDragonBallWnd();
            yield return new WarHeroBearing.UWarHeroBearingWnd();
            yield return new WarFloatFont.UWarFloatFontWnd();
            yield return new WarGameHudTopRight.BattleWarKAWnd();
            yield return new WarMinimapWnd.WarMinimapWnd();

            yield return new WarGoodsStore.UWarGoodsStoreWnd();
            yield return new WarMapSignal.WarMapSignalWnd();
            yield return new WarAutoMove.WarAutoMoveWnd();
            yield return new WarFrontSight.WarFrontSightWnd();
            yield return new SystemFloatText.SystemFloatTextWnd();

            yield return new TeammateInfo.UTeammateInfoWnd();
            yield return new TeammateInfo.UMatchTimeInfoWnd();
            yield return new WarHeroTalent.WarHeroTalentWnd();
            yield return new SystemSetting.SystemSettingPanelWnd();
            yield return new PrizeChest.PrizeChestWnd();
            yield return new WaitingRoomDelayJoin.UWaitingRoomDelayJoinWnd();
            yield return new TimeThresholdGoodsPart.TimeThresholdGoodsWnd();
            yield return new PKWnd.PKInfoWnd();
            yield return new PointShop.PointShopWnd();
            yield return new PointShop.PointShopBuyGoodsWnd();
            yield return new PlayerInfoCompare.UPlayerInfoCompareWnd();
            yield return new DIDA.DIDAWnd();
            yield return new WarPlayersVsInfoWnd.WarPlayersVsInfoWnd();
            yield return new WarExtraPlayerSelection.UWarExtraPlayerSelectionWnd();
            yield return new Sniperscope.USniperscopeWnd();
            yield return new StaticBottomCenterWnd.StaticBottomCenterWnd();
            yield return new Loudspeaker.LoudspeakerWnd();
            yield return new WarDeathReplay.WarDeathReplayWnd();
            yield return new AntiAddict.UAntiAddictWnd();
            yield return new GenstoneChestFlop.GenstoneChestFlopWnd();
            yield return new Guide.UGuideWnd();
            yield return new VoiceSystem.VoiceSystemWnd();
            yield return new WarSupportPlayer.WarSupportPlayerWnd();
            yield return new FloatMoney.FloatMoneyWnd();
            yield return new LegendCup.LegendCupWarRecordWnd();
            yield return new LegendCup.LegendCupListWnd();
            yield return new LegendCup.LegendCupCreateWnd();
            yield return new LegendCup.LegendCupTipWnd();
            yield return new LegendCup.LegendCupRegistWnd();
            yield return new LegendCup.LegendCupPasswordWnd();
            yield return new LegendCup.LegendCupBlacklistWnd();
            yield return new LegendCup.LegendCupGroupWnd();
            yield return new LegendCup.LegendCupBigKnockoutWnd();
            yield return new LegendCup.LegendCupSmallKnockoutWnd();
            yield return new LegendCup.LegendCupJudgeWnd();
            yield return new LegendCup.LegendCupKinMemberWnd();
            yield return new LegendCup.LegendCupKinInfoWnd();
            yield return new LegendCup.LegendCupPrizeWnd();
            yield return new LegendCup.LegendCupCreateTypeWnd();
            yield return new LegendCup.LegendCupOnlineResetWnd();
            yield return new NPC.NPCWnd();
            yield return new MailBox.MailBoxWnd();
            yield return new MsgBoxMailFastRecv.MsgBoxMailFastRecvWnd();
            yield return new HeroPreviewFrame.PrizePopupWnd();
            yield return new ConsumptionGuide.ConsumptionGuideWnd();
            yield return new Guide.PrizeTipGuideWnd();
            yield return new PointShop.PointShopReflushCardWnd();
            // 功能暂不需要，先关了。
            //yield return new GoodsDisposition.UGoodsDispositionWnd();
            //yield return new GoodsDisposition.UGoodsDispositionShareWnd();
            //yield return new WarGoodsStore.UWarGoodsDispositionWnd();
            yield return new LoginQueueInfo.ULoginQueueInfoWnd();
            yield return new LoginServerMsgInfo.ULoginServerMsgInfoWnd();
			yield return new ChatBubble.ChatBubbleWnd();
			yield return new AcceptWaitInfo.UAcceptWaitInfoWnd();
            yield return new WarTowerMessage.WarTowerMessageWnd();

            yield return new TeammateInfo.UPunishTimeInfoWnd();
            yield return new TeammateInfo.USideBtnPunishInfoWnd();

            yield return new WarGloryMomentVideo.UWarGloryMomentVideoWnd();
            yield return new FPSBullet.FPSBulletWnd();
            yield return new LifeHero.LifeHeroEntranceWnd();

            yield return new ChatBox.ChatBoxWnd();
            yield return new LifeHero.LifeHeroCultureWnd();
            yield return new LifeHero.LifeHeroSelectWnd();
			yield return new RankBtnClickInfo.URankBtnClickInfoWnd();

            yield return new TaskWnd.TaskMainWnd();
            yield return new TaskWnd.TaskPublishWnd();
            yield return new TaskWnd.TaskUnfoldWnd();
            yield return new TaskWnd.TaskReflushWnd();

            yield return new WarEndRankInfo.UWarRecordTableRankInfoWnd();
            yield return new InvitePlayGame.InvitePlayGameWnd();
            yield return new GamePromotion.GamePromotionWnd();
            yield return new TeamRightPart.BattleWarTeammateWnd();
            yield return new BattleWarRecordTableWnd();
            yield return new WarRecordTableWarning.RecordTableFightingWnd();
            yield return new WarEndOf.EndOfBattleWarWnd();
            yield return new WarObserver.WarObserverWnd();

            yield return new WarMainFrame.RankDetailWnd();
            yield return new WarMainFrame.RankInviteWnd();
            yield return new WarMainFrame.RankRecordWnd();

            yield return new PromotionSideBtnList.PromotionSideBtnListWnd();
        }
    }

    // 窗口ID定义
    public enum WndID
    {
        WND_ID_BASE = 100,          // 窗口ID基础值
        WND_ID_CHATBOX,             // 聊天窗口
        WND_ID_PLAYERBUTTONFRAME,   // 玩家信息按钮
        WND_ID_PLAYERFRAME,         // 玩家系统外框
        WND_ID_HERODETAIL,          // 英雄详细界面
        WND_ID_MAINACTORINFOBOX,    // 玩家基本信息框
        WND_ID_SIDEBUTTONLIST,      // 侧边按钮列表框
        WND_ID_TOPSIDEBUTTONLIST,   // 上部侧边按钮提示框
        WND_ID_UPGRADESTARHINT,     // 升星提示界面
        WND_ID_SOCIALCONTACT,       // 社会关系窗口
        WND_ID_KIN_ACCEPTMEMBER,    // 战队成员接收窗口
        WND_ID_KIN_MSGBOXCREATEKIN, // 战队创建窗口
        WND_ID_CLAN_ACCEPTMEMBER,       // 帮会成员接收窗口
        WND_ID_CLAN_MSGBOXCREATECLAN,   // 帮会创建窗口
        WND_ID_CLAN_SELECTREQUESTCARD,  // 帮会自己英雄窗口
        WND_ID_TOOLTIPS,            // 提示框窗口
        WND_ID_Dialog,              //消息框窗口
        WND_ID_LOGIN,               //登录窗口
        WND_ID_CREATEHERO,          //创建英雄窗口
        WND_ID_FULLSCREEN,          //全屏背景窗口
        WND_ID_LOADINGBAR,          //加载条窗口
        WND_ID_PLAYER_TALENTPAGE,   // 玩家天赋界面
        WND_ID_PLAYER_HONORPAGE,     //玩家荣誉界面
        WND_ID_WAR_MAIN_FRAME,       // 对战主界面
        WND_ID_WAR_TEAMRIGHTVIEW,    // 战场右侧队伍界面
        WND_ID_WAR_BOTTOMCENTERVIEW, // 战场底部界面(技能物品属性buff)
        WND_ID_WAR_PROGRESSBAR,      // 进度条(目前只有技能和翅膀施法)
	    WND_ID_WAR_ENDOFWAR,         // 结束战场战场胜利失败标识
	    WND_ID_WAR_GAMEHUDTOPRIGHTVIEW, // 战场右上角信息框
        WND_ID_WAR_RECORDTABLE_FIGHTING,    // 战绩信息
	    WND_ID_WAR_HERODETAILEDINFOPANEL, //战场英雄详细信息面板
        WND_ID_LOADINGWARVIEW,       // 载入战场等候界面 
		WND_ID_WAR_RELIVETIME,      // 战场复活时间   
	    WND_ID_HIGHLADDER,          //排行榜
        WND_ID_CHART,                   //排行榜窗口(新版)
		WND_ID_POPUPMENUVIEW,       //弹出列表窗口           
        WND_ID_HALLROOMINFOVIEW,     // 房间信息界面   
   	    WND_ID_WAR_RECORDTABLE_END,         // 战后战绩信息
	    WND_ID_WAR_BROADCASTMESSAGEMGR, // 战场广播信息
        WND_ID_WAR_COMBOATTACK,         // 战场连击界面
        WND_ID_DRAGONBALL,              // 七龙珠界面   
        WND_ID_WAR_XPSKILL,             // XP技能界面
        WND_ID_WAR_MINIMAP,         // 小地图窗口
        WND_ID_WAR_HEROBEARING,         // 战场指示器
        WND_ID_WAR_FLOATFONTFRAME,          //战场飘字界面
        WND_ID_WAR_GOODSSTORE,          //商店界面
        WND_ID_WAR_MAPSIGNAL,           // 地图标记窗口
	    WND_ID_WAR_AUTOMOVE,           //战场自动寻路标识
        WND_ID_WAR_FRONTSIGHT,         //战场准星
        WND_ID_GAMEWAITINGROOMVIEW,     // 选人大厅界面
        WND_ID_SYSTEMFLOATTEXT,        //系统飘字
        WND_ID_TEAMMATEINFO,            // 队伍信息窗口
        WND_ID_MATCHTIMEINFO,           // 匹配时间信息
        WND_ID_WARHEROTALENTVIEW,        // 英雄天赋界面
        WND_ID_SYSTEMSETTINGPANEL,       //系统设置主面板
        WND_ID_PRIZECHEST,              // 宝箱
		WND_ID_WAITINGROOM_DELAYJOIN,   // 延迟进入房间
        WND_ID_HEROSKINVIEW,            // 英雄皮肤界面
        WND_ID_TIMETHRESHOLDVIEW,       // 限时物品管理界面
        WND_ID_PKINFO,                  // PK信息窗口
        WND_ID_POINTSHOP,               // 商城
        WND_ID_POINTSHOPBUYGOODS,       // 商城购买物品
        WND_ID_PLAYERINFOCOMPARE,       // 玩家信息比较界面
        WND_ID_DIDA,                    // DIDA界面
        WND_ID_PLAYERSVSINFO,           // 玩家对阵信息界面
        WND_ID_WAR_EXTRAPLAYERSELECTION, //战场额外的英雄头像选择
        WND_ID_SNIPERSCOPE,             //狙击镜界面
        WND_ID_STATICBOTTOMCENTER,      // 静态场景底部界面(技能物品属性buff)
        WND_ID_LOUDSPEAKER,    			// 喇叭跑马灯界面
        WND_ID_WAR_DEATHREPLAY,        // 死亡回放界面
        WND_ID_ANTIADDICT,              // 防沉迷界面
        WND_ID_LEGENDCUP_WARRECORD,     // 比赛记录界面
	    WND_ID_VOICESYSTEM,             // 语音系统
        WND_ID_CROWNPAGE,               // 皇冠界面
        WND_ID_GEMSTONECHESTFLOP,       // 宝石宝箱翻牌界面
        WND_ID_GAMEGUIDE,               // 游戏引导界面
        WND_ID_MASTER_MSGBOXISSUE,      // 师傅消息发布
        WND_ID_PRENTICE_MSGBOXISSUE,    // 学徒消息发布
        WND_ID_WARSURPOORTPLAYERVIEW,   // 点赞界面
        WND_ID_LEGENDCUP_CUPLIST,       // 比赛列表界面
        WND_ID_LEGENDCUP_MSGBOXTIP,     // 比赛说明界面
        WND_ID_LEGENDCUP_MSGBOXCREATE,  // 比赛创建界面
        WND_ID_LEGENDCUP_REGIST,        // 比赛报名界面
        WND_ID_LEGENDCUP_BLACKLIST,     // 比赛黑名单界面
        WND_ID_LEGENDCUP_PASSWORD,      // 比赛密码界面
        WND_ID_LEGENDCUP_GROUP,         // 比赛小组赛界面
        WND_ID_LEGENDCUP_BIGKNOCKOUT,   // 比赛16强以上淘汰赛界面
        WND_ID_LEGENDCUP_SMALLKNOCKOUT, // 比赛16强及以内淘汰赛界面
        WND_ID_LEGENDCUP_JUDGE,         // 比赛裁决界面
        WND_ID_LEGENDCUP_KINMEMBER,     // 比赛家族成员界面
        WND_ID_LEGENDCUP_KININFO,       // 比赛家族信息界面
        WND_ID_LEGENDCUP_PRIZE,         // 比赛奖励界面
        WND_ID_LEGENDCUP_CREATETYPE,    // 比赛创建类型界面
        WND_ID_LEGENDCUP_ONLINE_RESET,  // 线上比赛重置界面
        WND_ID_SNS,                     // SNS系统
        WND_ID_SNS_MAINPANEL,           // SNS系统主面板
        WND_ID_SNS_CHAT,				// SNS系统聊天窗口界面
        WND_ID_SNS_FINDBUDDY,			// SNS系统查找好友界面
        WND_ID_SNS_INPUTDIALOG,			// SNS系统输入窗界面
        WND_ID_FLOATMONEY,              // 加钱飘字界面
        WND_ID_NPC,                     // NPC界面
        WND_ID_MAILBOX,                 // 消息邮箱界面
        WND_ID_MAIL_MSGBOXFASTRECV,     // 邮件快速取件确认界面
        WND_ID_PRIZEPOPUP,              // 奖励弹出界面
        WND_ID_CONSUMPTIONGUIDE,        // 消费引导界面
        WND_ID_PRIZETIPGUIDE,           // 奖励提示界面
        WMD_ID_COMMONBLURBG,            //通用模糊黑底
        WND_ID_POINTSHOPREFLUSHCARD,    // 商城刷新卡牌界面
        WND_ID_GOODSDISPOSITION,        // 物品配置界面
        WND_ID_GOODSDISPOSITION_SHARE,  // 物品配置分享界面
        WND_ID_LOGINQUEUEINFO,          // 登陆排队界面
        WND_ID_LOGINSERVERMSGINFO,      // 服务器信息界面
        WND_ID_CHATBUBBLE,				// 聊天泡泡
		WND_ID_ACCEPTWAITINFO,          // 比赛接受界面
        WND_ID_WarGoodsDisposition,     //战场物品配置推荐页
        WND_ID_WARTOWERMESSAGE,         //防御塔内小兵数量
        WND_ID_PUNISH_TIMEINFO,           // 撮合惩罚信息提示相关
        WND_ID_SIDEBTN_PUNISH_TIMEINFO,   // 悬浮窗惩罚信息提示相关
        WND_ID_WARGLORYMOMENTVIDEO,       //战场精彩时刻视频窗
        WND_ID_FPSBULLET,               //FPS英雄子弹显示
        WND_ID_LIFE_HERO_ENTRANCE,      //本命英雄入口界面
        WND_ID_LIFE_HERO_CULTURE,      //本命英雄培养仓界面
        WND_ID_LIFE_HERO_SELECT,      //本命英雄培选择界面
		WND_ID_RANK_CLICK,              //Rank点击界面
        WND_ID_TASK_MAIN,               // 任务主界面
        WND_ID_TASK_PUBLISH,            // 任务发布界面
        WND_ID_TASK_UNFOLD,             // 任务展开界面
        WND_ID_TASK_REFLUSH,            // 任务刷新界面
        WND_ID_TASK_PRIZE,              // 任务奖励界面
		WND_ID_WAR_END_RANKINFO,        // 战场结束排位升级降级界面
		WND_ID_INVITEPLAYGAME,			// 邀请一起游戏界面
        WND_ID_GAME_PROMOTION,          // 游戏推广界面
        WND_ID_BATTLEWAR_BATTLEWARKA,   // 大逃杀战场右上角击杀助攻界面
        WND_ID_BATTLEWAR_TEAMMATEVIEW,  // 大逃杀战场顶部队友头像列表
        WND_ID_BATTLEWAR_RECORDTABLE,   // 大逃杀战场结算界面
        WND_ID_BATTLEWAR_RECORDTABLE_FIGHTING,    // 战绩信息
        WND_ID_BATTLEWAR_END_OF_BATTLEWAR,    //  大逃杀战场结束提示弹窗
        WND_ID_WAR_OBSERVER,                    //  观战界面

        WND_ID_RANK_DETAIL,             // 排位详情界面
        WND_ID_RANK_INVITE,             // 排位邀请界面
        WND_ID_RANK_RECORD,             // 排位历史记录界面
        WND_MSG_PROMOTION_SIDE_BTN_LIST, //活动侧边按钮列表
    }

    // 窗口消息
    public enum WndMsgID
    {
        // 公用窗口消息
        WND_MSG_COMMON_START = 0,
        WND_MSG_COMMON_LOGINSTATE_ENTER,
        WND_MSG_COMMON_LOGINSTATE_LEAVE,

        WND_MSG_COMMON_CHOOSESTATE_ENTER,
        WND_MSG_COMMON_CHOOSESTATE_LEAVE,

        WND_MSG_COMMOM_STATICGAMESTATE_ENTER,
        WND_MSG_COMMOM_STATICGAMESTATE_LEAVE,

        WND_MSG_COMMOM_MATCHROOMSTATE_ENTER,
        WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE,

        WND_MSG_COMMOM_LOADSCENESTATE_ENTER,
        WND_MSG_COMMOM_LOADSCENESTATE_LEAVE,

        WND_MSG_COMMON_LOADSCENE,
        WND_MSG_COMMON_LOGINMOBA,
        WND_MSG_COMMON_OPEN_LOGINQUEUE,
        WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO,
        WND_MSG_COMMON_BATTLESTATE_ENTER,
        WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie,
        WND_MSG_COMMON_BATTLESTATE_LEAVE,
		
        WND_MSG_COMMON_MAINHEROFIGHTSTATE,
        /// <summary>消息框信息</summary>
        WND_MSG_COMMON_DIALOG,
        /// <summary>隐藏通用弹窗</summary>
        WND_MSG_COMMON_DIALOG_HIDE,
        /// <summary>加载进度</summary>            
        WND_MSG_COMMOM_LOADING_VALUE,
		// <summary>弹出列表信息</summary>            
        WND_MSG_COMMON_POPUPMENU,
        // Layer界面按钮
        WND_MSG_COMMOM_WARHILL_PRESSDOWN,
        // 大厅选人确定
        WND_MSG_COMMOM_WTRSURESELECT,
        // 战场人物死亡的复活时间
        WND_MSG_COMMOM_WARHERORELIVETIME,
        // 战场人物大招状态变化
        WND_MSG_COMMOM_WARHEROMAXSKILLSTATE,
        // 战场物品购买变化
        WND_MSG_COMMOM_WARGOODSINFO,
        /// <summary>
        /// 战场结束 显示胜利/失败界面 时发出,
        /// 如果需要在离开战场状态前释放界面的话用这个消息
        /// </summary>
        WND_MSG_COMMON_WAREND,
        // 玩家游戏状态发生变化
        WND_MSG_COMMON_UPDATEGAMESTATE,
		// 玩家位置更新了
        WND_MSG_COMMON_LOCATION_CHANGE,

        // 玩家获取或者更换英雄角色时触发
        WND_MSG_COMMON_UPDATE_MAIN_HERO_VOCATION,

        // 在主城打开或关闭了弹窗
        WND_MSG_COMMON_POPUPWND_VISIBLE,
        // 在主城打开或关闭全屏界面
        WND_MSG_COMMON_FULLWND_VISIBLE,
        // 接收web url
        WND_MSG_COMMON_PROCESS_WEBURL,

        // 聊天窗口用消息
        WND_MSG_CHATBOX_START = WndID.WND_ID_CHATBOX * 1000,
        WND_MSG_CHATBOX_UPDATEMESSAGE,
        WND_MSG_CHATBOX_CHANGECHANNEL,
        WND_MSG_CHATBOX_INPUTFIELDADDITEM,
        WND_MSG_CHATBOX_CONDITIONHIDE,
        WND_MSG_CHATBOX_CHANGEIME,
        WND_MSG_CHATBOX_PRIVATECHAT,
        WND_MSG_CHATBOX_MOVEPOSDOWN,
        WND_MSG_CHATBOX_MOVEPOSUP,
        WND_MSG_CHATBOX_CLOSEEMOTICONPANEL,
        WND_MSG_CHATBOX_UPDATELOCKSTATE,
        WND_MSG_CHATBOX_TOGGLE,             // 切换最小化
        WND_MSG_CHATBOX_RESETSORTORDER,     // 刷新层级
        WND_MSG_CHATBOX_SENDCHATMESSAGE,           // 发送数据

        // 玩家信息按钮
        WND_MSG_PLAYERBUTTONFRAME_START = WndID.WND_ID_PLAYERBUTTONFRAME * 1000,
        WND_MSG_PLAYERBUTTONFRAME_OPENFRAME,
        WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP,
        WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN,
        /// <summary>打开大厅按钮添加引导控件（没用注册方法，直接通知） </summary>
        WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET,
        WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_REMOVEGUIDEWIDGET,

        // 玩家系统外框
        WND_MSG_PLAYERFRAME_START = WndID.WND_ID_PLAYERFRAME * 1000,
        WND_MSG_PLAYERFRAME_CLOSE,
        // 玩家信息
        WND_MSG_PLAYERINFO_INITINFO,
        WND_MSG_PLAYERINFO_REANMERESULT,
        // 战绩窗口
        WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL,
        WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR,
        WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR,			// 显示某场战绩
        WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR,			// 显示某个玩家的战绩
        // 英雄预览界面
        WND_MSG_HEROPREVIEWFRAME_GETHEROINFO,
        // 符文窗口
        WND_MSG_RUNEPAGE_INITRUNEDATA,
        WND_MSG_RUNEPAGE_PUTRUNE,
        WND_MSG_RUNEPAGE_CHANGEPAGE,
        WND_MSG_RUNEPAGE_INITOTHERRUNEDATA,

        // 英雄详细界面
        WND_MSG_HERODETAIL_START = WndID.WND_ID_HERODETAIL * 1000,
        WND_MSG_HERODETAIL_HERODETAILINFO,
        WND_MSG_HERODETAIL_HERODETAILINFOREFRESH,
        WND_MSG_HERODETAIL_HERODETAILXPUPDATE,
        WND_MSG_HERODETAIL_CLOSE,

        WND_MSG_OPEN_SOCIALSHARE_FRAME,
        WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME,
        WND_MSG_OPEN_GLORYMOMENT_WARDATA,
        /// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_MSG_HERODETAIL_GUIDE_ACTIONBEGIN,
        WND_MSG_HERODETAIL_GUIDE_ACTIONEND,
        WND_MSG_HERODETAIL_UPDATEGOLD,

        // 侧边按钮列表框
        WND_MSG_SIDEBUTTONLIST_START = WndID.WND_ID_SIDEBUTTONLIST * 1000,
        WND_MSG_SIDEBUTTONLIST_REFRESH,

        // 升星提示界面
        WND_MSG_UPGRADESTARHINT_START = WndID.WND_ID_UPGRADESTARHINT * 1000,
        WND_MSG_CHICKUPGRADEBUTTON_RETURN,
        WND_MSG_UPGRADESTARSUCESS_RESULT,

        // 社会关系窗口
        WND_MSG_SOCIALCONTACT_START = WndID.WND_ID_SOCIALCONTACT * 1000,
        WND_MSG_SOCIALCONTACT_OPEN,
        WND_MSG_SOCIALTOPSIDEBUTTON_OPEN,
        WND_MSG_SOCIALCONTACT_REFRESH,
        WND_MSG_KIN_SHOWKINLIST,
        WND_MSG_KIN_REQUESTKINLIST,
        WND_MSG_KIN_RECEV_MEMBERLIST,
        WND_MSG_KIN_RECEV_APPLY_LIST,
        WND_MSG_KIN_UPDATE_APPLY_LIST,
        WND_MSG_KIN_RECEV_KINBASEDATA,
        WND_MSG_KIN_UPDATSELF,
        WND_MSG_KIN_CREATESUCCESS,
        WND_MSG_KIN_UPDATEMEMBER,
        WND_MSG_KIN_STRING_PROP_UPDATE,
        WND_MSG_KIN_NUM_PROP_UPDATE,
        WND_MSG_KIN_AUTO_ACCEPT,
        WND_MSG_KIN_AUTO_ACCEPT_REQUEST,
        WND_MSG_KIN_UPDATEAPPLY_TIPS,
        WND_MSG_KIN_RECV_EVENTLIST,
        WND_MSG_KIN_RECV_GLORYDATA,
        WND_MSG_KIN_RETURN_TO_SELFKIN,
        WND_MSG_KIN_JOIN_SUCCESS,

        WND_MSG_CLAN_REQUESTCLANLIST,
        WND_MSG_CLAN_SHOWCLANLIST,
        WND_MSG_CLAN_BASEDATA,
        WND_MSG_CLAN_APPLY_LIST,
        WND_MSG_CLAN_REMOVEAPPLY,
        WND_MSG_CLAN_ADDAPPLY,
        WND_MSG_CLAN_UPDATEMEMBER,
        WND_MSG_CLAN_REQUEST_CARDLIST,
        WND_MSG_CLAN_REQUEST_CARDLIST_INFO,
        WND_MSG_CLAN_UPDATE_CARDLIST,
        WND_MSG_CLAN_UPDATE_DONATELOG,
        WND_MSG_CLAN_UPDATE_REQUESTCARDTIP,
        WND_MSG_CLAN_CREATESUCCEED,
        WND_MSG_CLAN_RECV_EVENTLIST,
        WND_MSG_CLAN_APPLY_MEMBER_TIP,
        WND_MSG_CLAN_REQUESTCARD,
        WND_MSG_CLAN_APPLYRET,
        WND_MSG_CLAN_STATE_NOTICE,

        WND_MSG_MENTORSHIP_UPDATEPERSONBASE,
        WND_MSG_MENTORSHIP_UPDATEMASTERLIST,
        WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST,
        WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST,
        WND_MSG_MENTORSHIP_MASTERISSUELIST,
        WND_MSG_MENTORSHIP_PRENTICEISSUELIST,
        WND_MSG_MENTORSHIP_APPLYANSWER,
        WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST,
        WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST,
        WND_MSG_MENTORSHIP_OPNESUBPAGE,


        WND_MSG_KIN_UPDATEKINLIST,
        // 天赋窗口
        WND_MSG_TALENTPAGE_START = WndID.WND_ID_PLAYER_TALENTPAGE * 1000,
        WND_MSG_TALENTPAGE_INITTALENTDATA,
        WND_MSG_TALENTPAGE_ROOMOPENTALENT,
        WND_MSG_TALENTPAGE_CHANGEPAGE,

        //创建角色窗口
        WND_MSG_CREATEHERO_START = WndID.WND_ID_CREATEHERO * 1000,
        /// <summary>显示创建角色界面</summary>
        WND_MSG_CREATEHERO_OPEN,
        /// <summary>创建角色界面返回按钮</summary>
        WND_MSG_CREATEHERO_FALLBACK,
        /// <summary> 接收请求随机名字返回结果</summary>
        WND_MSG_CREATEHERO_RANDOMNAMERESULT,
        /// <summary> 接收创建角色返回结果</summary>
        WND_MSG_CREATEHERO_CREATE_PLAYER_RESULT,

        WND_MSG_CREATEHERO_UPDATE_SERVERINFO,
        WND_MSG_CREATEHERO_RECV_DISTRICTLIST,
        WND_MSG_CREATEHERO_RECV_LASTENTER_DISTRICTLIST,
        WND_MSG_CREATEHERO_WEBBROWSER_STATE,
 
        //全屏背景
        WND_MSG_FULLSCREEN_START = WndID.WND_ID_FULLSCREEN * 1000,
        /// <summary>切换全屏背景图片</summary>
        WND_MSG_FULLSCREEN_CHANGEIMAGE,

        //荣誉窗口
        WND_MSG_HONORPAGE_START = WndID.WND_ID_PLAYER_HONORPAGE * 1000,
        WND_MSG_HONORPAGE_OPEN,

        // 对战主界面
        WND_MSG_WAR_MAIN_START = WndID.WND_ID_WAR_MAIN_FRAME * 1000,
        WND_MSG_WAR_MAIN_OPEN,
        WND_MSG_WAR_MAIN_CLOSE,
        WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET,
        WND_MSG_WAR_MAIN_GAMEMODEBTN_REMOVEGUIDEWIDGET,
		/// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_MSG_WAR_MAIN_GUIDE_ACTIONBEGIN,
        WND_MSG_WAR_MAIN_GUIDE_ACTIONEND,
        WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE,

        // 右侧队伍信息
        WND_MSG_TEAMRIGHT_START = WndID.WND_ID_WAR_TEAMRIGHTVIEW * 1000,
        WND_MSG_TEAMRIGHT_INITMYTEAMDATA,
        WND_MSG_TEAMRIGHT_INITMYTEAMUID,

        // 战场底部ui
        WND_MSG_WARBUTTOMCENTER_START = WndID.WND_ID_WAR_BOTTOMCENTERVIEW * 1000,
        WND_MSG_WARBUTTOMCENTER_SKILLUPDATE,
        WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT,
        WND_MSG_WARBUTTOMCENTER_SKILLEFFECT,
        WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT,
        WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT,
        WND_MSG_WARBUTTOMCENTER_BUFFINFO,
        WND_MSG_WARBUTTOMCENTER_SETSHORTCUT,
        /// <summary>模拟天赋按钮点击,打开天赋界面</summary>
        WND_MSG_WARBUTTOMCENTER_WARTALENTBUTTONCLICK,

        // 所有的进度条ui
        WND_ID_WAR_PROGRESSBAR_START = WndID.WND_ID_WAR_PROGRESSBAR * 1000,
        WND_MSG_WARBUTTOMCENTER_PROGRESSBAR,
        WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR,
        WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR,
        WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR,

        //战场结束胜利失败UI
        WND_ID_WAR_ENDOFWAR_START = WndID.WND_ID_WAR_ENDOFWAR * 1000,
        WND_MSG_WAR_SHOWENDOFWAR,
        WND_MSG_WAR_UPDATEPLAYERINFO,
        // 战场右上角信息框
        WND_MSG_WARGAMEHUDTOPRIGHT_START = WndID.WND_ID_WAR_GAMEHUDTOPRIGHTVIEW * 1000,
        WND_MSG_WARGAMEHUDTOPRIGHT_SHOWWINORFAIL,	
        // 战绩信息
        WND_MSG_WAR_RECORDTABLE_FIGHTING = WndID.WND_ID_WAR_RECORDTABLE_FIGHTING * 1000,
        WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT,
        WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME,
        WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO,
        WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND,

       //战场英雄详细信息面板
        WND_MSG_WARHERODETAILEDINFOPANEL_START = WndID.WND_ID_WAR_HERODETAILEDINFOPANEL * 1000, 
        // 加载进入战场界面
        WND_ID_LOADINGWARVIEW_START = WndID.WND_ID_LOADINGWARVIEW * 1000,
        WND_ID_LOADINGWARVIEW_LOADPLAYERINFO,
        WND_ID_LOADINGWARVIEW_LOADPROGREESERATE,
        // 战场复活时间
        WND_MSG_WARRELIVETIME_START = WndID.WND_ID_WAR_RELIVETIME * 1000,
        WND_MSG_WARRELIVETIME_SHOWRELIVETIME,
        WND_MSG_WARRELIVETIME_REMOVERELIVETIME,
		
       //排行榜
        WND_MSG_HIGHLADDER_START = WndID.WND_ID_HIGHLADDER * 1000,
        WND_MSG_HIGHLADDER_OPEN,
        WND_MSG_HIGHLADDER_GETINFO,
        WND_MSG_HIGHLADDER_CLOSE,

        //新版排行榜
        WND_MSG_CHART_START = WndID.WND_ID_CHART * 1000,
        WND_MSG_UPDATE_PERSON_ORDER_SETTING,                     //更新个人设置
        WND_MSG_REFRESHCHARTRANKINGDATA,                                  //刷新排名数据
        WND_MSG_OPENENCOURAGEDIALOG,                                 //我的鼓励对话框
        WND_MSG_RECVREQUESTROLERANKDATA,                             //接收角色段位榜数据
        WND_MSG_RECVREQUESTKILLRANKDATA,                             //接收多杀榜数据
        WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA,                  //英雄王者榜数据
        WND_MSG_RECVREQUESTHERORANKDATA,                             //接收英雄养成榜数据
        WND_MSG_RECVREQUESTHEROTYPECHARTDATA,                        //接收英雄类型榜数据
        WND_MSG_RECVREQUESTCLANCHARTDATA,                            //接收联盟榜数据
        WND_MSG_RECVREQUESTKINCHARTDATA,                             //接收战队榜数据
        WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA,                   //接收本周导师榜数据
        WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA,                 //接收本周学徒榜数据
        WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA,                  //接收上周导师榜数据
        WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA,                //接收上周学徒榜数据
        WND_MSG_RECVREQUESTDONATECHARTDATA,                          //接收捐卡榜数据
        WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA,                       //接收英雄数量榜数据
        WND_MSG_RECVREQUESTSKINCHARTDATA,                            //接收皮肤榜数据
        WND_MSG_RECVREQUESTGEMSTONECHARTDATA,                        //接收宝石榜数据
        WND_MSG_RECVREQUSETSEARCHDATA,                                     //接收搜索数据
        WND_MSG_JUMPTOMYRANKING,                                     

        // 房间信息界面
        WND_ID_HALLROOMINFOVIEW_START = WndID.WND_ID_HALLROOMINFOVIEW * 1000,
        WND_ID_HALLROOMINFOVIEW_ADDROOMINFO,
        WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO,
        WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO,
        WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW,
        WND_ID_HALLROOMINFOVIEW_GAMEMODEINFO,
        WND_ID_HALLROOMINFOVIEW_MAPMODEINFO,
        WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO,
        WND_ID_HALLROOMINFOVIEW_SUREENTERROOM,
        WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK,
        WND_MSG_GAMEWAITINGROOMVISILBLE,
 
        // 战后战绩信息
        WND_MSG_WAR_RECORDTABLE_END = WndID.WND_ID_WAR_RECORDTABLE_END * 1000,
        WND_MSG_WAR_RECORDTABLE_END_SHOW,           // 显示战后统计信息
        WND_MSG_WAR_RECORDTABLE_END_BASEFRAME,      // 基本框架数据
        WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO,  // 初始化奖励信息
        ///<summary>离开按钮显示光效（内部事件，不订阅）</summary>
        WND_MSG_RECORDTABLE_END_EXITBTN_ADDGUIDEWIDGET,
        WND_MSG_RECORDTABLE_END_EXITBTN_REMOVEGUIDEWIDGET,


        // 战场广播信息
        WND_MSG_WARBROADCASTMESSAGEMGR_START = WndID.WND_ID_WAR_BROADCASTMESSAGEMGR * 1000,
        WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT,
        WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV,
        WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE,
        WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE,
        WND_MSG_WARBROADCASTMESSAGEMGR_ALIVEASSIST,
        WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION,
        WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION,

        // 战场连击界面
        WND_MSG_WARCOMBOATTACK_START = WndID.WND_ID_WAR_COMBOATTACK * 1000,
        WND_MSG_WARCOMBOATTACK_SHOWATTACK,
        WND_MSG_WARCOMBOATTACK_SHOWKILL,

        // 七龙珠界面
        WND_MSG_DRAGONBALL_START = WndID.WND_ID_DRAGONBALL * 1000,
		
		WND_MSG_XPSKILL_START = WndID.WND_ID_WAR_XPSKILL * 1000,
        WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF,
        WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER,
        WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL,

        // 小地图窗口
        WND_MSG_WAR_MINIMAP_START = WndID.WND_ID_WAR_MINIMAP * 1000,
        WND_MSG_WAR_MINIMAP_ADDOBJECT,
        WND_MSG_WAR_MINIMAP_REMOVEOBJECT,
        WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY,
        WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT,
        WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT,
        WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT,
        WND_MSG_WAR_MINIMAP_CLEARSTATICOBJECT,
        WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN,
        WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS,
        WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL,
        WND_MSG_WAR_MINIMAP_SETOBJECTRANGETIP,
        WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE,
        WND_MSG_WAR_MINIMAP_SETNOXSAFEZONE,

        // 战场指示器
        WND_MSG_WARHEROBEARING_STRAT = WndID.WND_ID_WAR_HEROBEARING * 1000,

        //战场飘字
        WND_MSG_WAR_FLOATFONT_START = WndID.WND_ID_WAR_FLOATFONTFRAME * 1000,
        WND_MSG_WAR_FLOATFONT_CREATE,
        WND_MSG_WAR_FLOATFONT_REMOVEITEM,
        WND_MSG_WAR_FLOATFONT_UNLOAD,

        //战场商店
        WND_MSG_WAR_GOODSSTORE_START = WndID.WND_ID_WAR_GOODSSTORE * 1000,
        ///<summary>判断当前能否执行物品购买，出售，撤销操作</summary>
        WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL,
        ///<summary>判断当前能否执行撤销操作</summary>
        WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED,
        ///<summary>商店界面开关</summary>
        WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE,
        ///<summary>更新战场内所有玩家物品信息</summary>
        WND_MSG_WAR_GOODSSTORE_UPDATEPLAYERSGOODS,
        ///<summary>更新战场内本地玩家物品信息</summary>
        WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS,
        ///<summary>切换当前推荐物品配置</summary>
        WND_MSG_WAR_GOODSSTORE_CHANGEGOODSDISPOSITION,
        ///<summary>搜索字符串改变事件（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGED,
        ///<summary>搜索字符串结束事件（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGEEND,
        ///<summary>设置当前选择的推荐物品配置信息（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_SELECTEDRECOMMENDDISPOSITION,
        ///<summary>获取当前是否展开推荐配置列表（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_GET_TOGGLEDISPOSITIONSTATE,
        ///<summary>设置当前是否展开推荐配置列表按钮状态（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_TOGGLEDISPOSITIONSTATE,
        ///<summary>创建推荐配置标签按钮（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM,
        ///<summary>选择推荐配置标签按钮（内部事件，不订阅）</summary>
        WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM_SELECTED,


        // 地图标记
        WND_MSG_WAR_MAPSIGNAL_START = WndID.WND_ID_WAR_MAPSIGNAL * 1000,
        WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL,
        WND_MSG_WAR_MAPSIGNAL_MINIMAPDRAGEND,

        //自动寻路标识
        WND_MSG_WAR_AUTOMOVE_START = WndID.WND_ID_WAR_AUTOMOVE * 1000,
        WND_MSG_WAR_AUTOMOVE_SHOW,
        WND_MSG_WAR_AUTOMOVE_HIDE,

        //战场准星
        WND_MSG_WAR_FRONTSIGHT_START = WndID.WND_ID_WAR_FRONTSIGHT * 1000,
        WND_MSG_WAR_FRONTSIGHT_POS,
        WND_MSG_WAR_FRONTSIGHT_COLOR,
        WND_MSG_WAR_FRONTSIGHT_VISIBLE,
        WND_MSG_WAR_FPSBULLET_VISIBLE,
        WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET,
        // 选人大厅界面
        WND_ID_GAMEWAITINGROOMVIEW_START = WndID.WND_ID_GAMEWAITINGROOMVIEW * 1000,      
        WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM,
        WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMROMOVETIMECOUNT,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMMODEL,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMTALENTINROOMCLOSE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN,
        WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM,
        WND_ID_GAMEWAITINGROOMVIEW_SHOW,
        WND_ID_GAMEWAITINGROOMVIEW_SHOWAFTERLOADSCENE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMBUYHERO,
        /// <summary>添加引导控件（没用注册方法，直接通知） </summary>
        WND_ID_GAMEWAITINGROOMVIEW_SELECTHERO_ADDGUIDEWIDGET,
        WND_ID_GAMEWAITINGROOMVIEW_SELECTHERO_REMOVEGUIDEWIDGET,
        WND_ID_GAMEWAITINGROOMVIEW_STARTBTN_ADDGUIDEWIDGET,
        WND_ID_GAMEWAITINGROOMVIEW_STARTBTN_REMOVEGUIDEWIDGET,
		/// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONBEGIN,
        WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONEND,
        WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST,
        WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN,
        WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO,
        WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO,
        WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG,
        WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB,
        WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB,
        WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB,

        //系统飘字
        WND_MSG_SYSTEMFLOATTEXT_START = WndID.WND_ID_SYSTEMFLOATTEXT * 1000,
        WND_MSG_SYSTEMFLOATTEXT_CREATE,

        // 队伍信息界面
        WND_MSG_TEAMMATEINFO_START = WndID.WND_ID_TEAMMATEINFO * 1000,
        WND_MSG_TEAMMATEINFO_UPDATE,
        WND_MSG_TEAMMATEINFO_CHANGECAPTAIN,
        WND_WSG_TEAMMATEINFO_KICKOUTMEMBER,
        WND_MSG_TEAMMATEINFO_LEAVETEAM,
        WND_MSG_TEAMMATEINFO_DISBANDTEAM,
        WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM,
        WND_MSG_TEAMMATEINFO_UPDATETEAMINFO,
        WND_MSG_TEAMMATEINFO_UPDATE_NAME,
        // 匹配时间信息界面WND_ID_MATCHTIMEINFO
        WND_MSG_MATCHTIMEINFO_START = WndID.WND_ID_MATCHTIMEINFO * 1000,
        WND_MSG_MSTCHTIMEINFO_SHOW,
        WND_MSG_MSTCHTIMEINFO_HIDE,
        WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT,
        WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_SHOW,
        WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_HIDE,
        /// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONBEGIN,
        WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONEND,

        // 英雄天赋界面
        WND_ID_HEROTALENTVIEW_START = WndID.WND_ID_WARHEROTALENTVIEW * 1000,
        WND_ID_HEROTALENTVIEW_INITHEROTALENTINFO,
        WND_ID_HEROTALENTVIEW_UPDATEHEROTALENTINFO,
        WND_ID_HEROTALENTVIEW_LEARNHEROTALENT,              // 学了新天赋（技能模块发过来的）
        WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE,      // 已激活的主动学习天赋列表更新（战场模块发过来的）
        WND_ID_HEROTALENTVIEW_INITACTIVEHEROTALENTINRECONNECT,
        WND_ID_HEROTALENTVIEW_HEROTALENTVISIBLE,
        WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE,
        WND_ID_HEROTALENTVIEW_UPDATESTATICTALENT,
        WND_ID_HEROTALENTVIEW_INITWARTALENT,
        /// <summary>给天赋技能按钮添加引导控件（没用注册方法，直接通知） </summary>
        WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET,
        /// <summary>给天赋技能按钮移除引导控件（没用注册方法，直接通知） </summary>
        WND_ID_HEROTALENTVIEW_REMOVEGUIDEWIDGET,
        WND_ID_HEROTALENTVIEW_UPDATETALENTBUTTON,

        //系统设置界面
        WND_MSG_SYSTEMSETTING_START = WndID.WND_ID_SYSTEMSETTINGPANEL * 1000,
        WND_MSG_SYSTEMSETTING_SHOW,
        WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING,
        WND_MSG_SYSTEMSETTING_SAVE,
        WND_MSG_SYSTEMSETTING_RESTORE,
        WND_MSG_SYSTEMCONTROLSETTING_RESTORE,
        WND_MSG_SYSTEMCONTROLSETTING_REFRESH,
        WND_MSG_SYSTEMSETTING_SWITCH,
        WND_MSG_SYSTEMSETTING_QUITGAME,
        WND_MSG_SYSTEMSETTING_FXAA,

        // 宝箱界面
        WND_MSG_PRIZECHEST_START = WndID.WND_ID_PRIZECHEST * 1000,
        WND_MSG_PRIZECHEST_UPDATECHESTINFO,
        WND_MSG_PRIZECHEST_OPENCHESTRESULT,
		
		// 延迟进入房间
        WND_MSG_WAITINGROOM_DELAYJOIN_START = WndID.WND_ID_WAITINGROOM_DELAYJOIN * 1000,
        WND_MSG_WAITINGROOM_DELAYJOIN_SHOW,
        WND_WAITINGROOM_DELAYJOIN_HIDE,
		
        // 英雄皮肤界面
        WND_MSG_HEROSKINVIEW_START =  WndID.WND_ID_HEROSKINVIEW * 1000,
        WND_MSG_HEROSKINVIEW_INITSKINDATA,
        WND_MSG_HEROSKINVIEW_UPDATESKINDATA,
        
        // 限时物品管理界面
        WND_MSG_TIMETHRESHOLD_START = WndID.WND_ID_TIMETHRESHOLDVIEW * 1000,

        // PK信息界面
        WND_MSG_PKINFO_START = WndID.WND_ID_PKINFO * 1000,
        WND_MSG_PKINO_ADD,
        WND_MSG_PKINO_REMOVE,
        WND_MSG_PKINFO_STARTPK,
        WND_MSG_PKINFO_ENDPK,

        // 商城界面
        WND_MSG_POINTSHOP_START = WndID.WND_ID_POINTSHOP * 1000,
        WND_MSG_POINTSHOP_OPEN,
        WND_MSG_POINTSHOP_BUYRESULT,
        WND_MSG_POINTSHOP_UPDATECARDLIST,
        //WND_MSG_POINTSHOP_OPEN_PAY,

        // 商城购买物品界面
        WND_MSG_POINTSHOPBUYGOODS_START = WndID.WND_ID_POINTSHOPBUYGOODS * 1000,
        WND_MSG_POINTSHOPBUYGOODS_OPEN,
        WND_MSG_POINTSHOPBUYGOODS_UPDATEBOTTOMDESC,

        // 玩家信息比较界面
        WND_MSG_PLAYERINFOCOMPARE_START = WndID.WND_ID_PLAYERINFOCOMPARE * 1000,
        WND_MSG_PLAYERINFOCOMPARE_OPEN,

        // DIDA界面
        WND_MSG_DIDA_START = WndID.WND_ID_DIDA * 1000,
        WND_MSG_DIDA_UPDATE,
        WND_MSG_DIDA_UPDATE_LAYER,
		// TAB对战信息界面
        WND_MSG_PLAYERSVSINFO_START = WndID.WND_ID_PLAYERSVSINFO * 1000,

        //战场额外的选择英雄头像界面
        WND_MSG_WAR_EXTRAPLAYERSELECTION_START = WndID.WND_ID_WAR_EXTRAPLAYERSELECTION * 1000,
        ///<summary>额外选择头像显示敌方</summary>
        WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY,

        WND_MSG_SNIPERSCOPE_START = WndID.WND_ID_SNIPERSCOPE * 1000,
        ///<summary>狙击镜显示开关</summary>
        WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,

        // 跑马灯系统消息界面
        WND_MSG_LOUDSPEAKER_START = WndID.WND_ID_LOUDSPEAKER * 1000,
        WND_MSG_LOUDSPEAKER_ADD,

        //死亡回放界面
        WND_MSG_WAR_DEATHREPLAY = WndID.WND_ID_WAR_DEATHREPLAY * 1000,
        WND_MSG_WAR_DEATHREPLAY_SHOW,
        WND_MSG_WAR_DEATHREPLAY_SHOWDETAILINFO,
        WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO,
        WND_MSG_WAR_DEATHREPLAY_CLOSE,
        WND_MSG_WAR_DEATHREPLAY_UNLOAD,

        // 防沉迷界面
        WND_MSG_ANTIADDICT_START = WndID.WND_ID_ANTIADDICT * 1000,
        WND_MSG_ANTIADDICT_SHOWSIDEBTN,
        WND_MSG_ANTIADDICT_SHOWRESULT,
        WND_MSG_ANTIADDICT_SHOWTIP,
		/// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_MSG_ANTIADDICT_GUIDE_ACTIONBEGIN,
        WND_MSG_ANTIADDICT_GUIDE_ACTIONEND,

		 //语音系统
        WND_MSG_VOICESYSTEM_START = WndID.WND_ID_VOICESYSTEM * 1000,
        WND_MSG_VOICESYSTEM_ENTERROOM,
        WND_MSG_VOICESYSTEM_QUITVOICE,
        WND_MSG_VOICESYSTEM_UPDATE_ROOMBASEINFO,
        WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO,
        WND_MSG_VOICESYSTEM_UPDATEMICORDER,
        WND_MSG_VOICESYSTEM_UPDATEUSERINFO,
        WND_MSG_VOICESYSTEM_RECEIVEUSERSEARCHDATA,
        WND_MSG_VOICESYSTEM_UPDATECHANNELLIST,
		
        // 皇冠界面
        WND_MSG_CROWNPAGE_START = WndID.WND_ID_CROWNPAGE*1000,
        WND_MSG_CROWNPAGE_UPDATEVIEW,
        WND_MSG_CROWNPAGE_OTHERUPDATEVIEW,
        WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW,
        WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW,
        WND_MSG_CROWNPAGE_APPRAISALRET,
        WND_MSG_CROWNPAGE_REDPOINTTIP,
        WND_MSG_CROWNPAGE_ACTIVEGEMMODEL,

        // 宝石宝箱翻盘界面
        WND_MSG_GEMSTONECHESTFLOP_START = WndID.WND_ID_GEMSTONECHESTFLOP * 1000,
        WND_MSG_GEMSTONECHESTFLOP_UPDATEVIEW,

        // SNS系统界面
        WND_MSG_SNS_START = WndID.WND_ID_SNS * 1000,
        WND_MSG_SNS_SHOW_MAINPANEL,		// 显示好友面板
        WND_MSG_SNS_SWITCH_MAINPANEL,
        WND_MSG_SNS_ADD_CHAT_MESSAGE,
        WND_MSG_SNS_SHOW_CHAT_WND,
        WND_MSG_SNS_SHOW_INPUTDIALOG,
        WND_MSG_SNS_UPDATE_BUDDYGROUP,
        WND_MSG_SNS_BLACKLIST_BY_OTHER,
        WND_MSG_SNS_UPDATE_SELF_STATUS,
        WND_MSG_SNS_ADD_BUDDY,
        WND_MSG_SNS_HIDE_SEARCH_BUDDY_WND,
        WND_MSG_SNS_SHOW_FINDBUDDY_WND,		// 显示查找好友窗口
        WND_MSG_SNS_SHOW_INVITE_PLAYGAME_WND,	// 显示邀请一起游戏窗口
        WND_MSG_SNS_SELECT_SESSION,	// 选择会话
        WND_MSG_SNS_DELETE_SESSION,	// 删除会话
        WND_MSG_SNS_SHOW_INVITE_MATCH_WND,	// 显示邀请匹配/排位窗口
        WND_MSG_SNS_BUDDY_STATUS_UPDATE,    // 好友状态发生变化


		WND_MSG_GAMEGUIDE_START = WndID.WND_ID_GAMEGUIDE * 1000,
        WND_MSG_GAMEGUIDE_ADDNODEMESSAGE,
        WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE,
        WND_MSG_GAMEGUIDE_REMOVEALLMESSAGE,
		///<summary>自动隐藏的引导消息</summary>
        WND_MSG_GAMEGUIDE_ADDAUTOMESSAGE,
		///<summary>一直显示的引导消息</summary>
        WND_MSG_GAMEGUIDE_ALWAYMESSAGE,
        /// <summary> 在引导界面上挂的光效消息 </summary>
        WND_MSG_GAMEGUIDE_ADDGUIDEEFFECT,
        WND_MSG_GAMEGUIDE_REMOVEGUIDEEFFECT,
        WND_MSG_GAMEGUIDE_ADDNODEREWARDMESSAGE,
        WND_MSG_GAMEGUIDE_REMOVENODEREWARDMESSAGE,

        // 点赞界面
        WND_MSG_WARSURPOORTPLAYERVIEW_START = WndID.WND_ID_WARSURPOORTPLAYERVIEW * 1000,
        WND_MSG_WAR_SHOW_SUPPORT_WND,			// 显示点赞界面
        WND_MSG_WAR_FILTER_SUPPORT_PLAYER,      // 选出点赞台上的人数
        WND_MSG_WAR_DISABLE_ALL_VOTE_BUTTON,	// 隐藏所有点赞按钮
        WND_MSG_WAR_HAS_ADD_LABEL,				// 有玩家获得新的标签

        // 加钱飘字界面
        WND_MSG_FLOATMONEY_START = WndID.WND_ID_FLOATMONEY * 1000,           

        // 比赛记录界面
        WND_MSG_LEGENDCUP_WARRECORD_START = WndID.WND_ID_LEGENDCUP_WARRECORD * 1000,
        WND_MSG_LEGENDCUP_WARRECORD_SHOW,
        WND_MSG_LEGENDCUP_WARRECORD_TOTAL,
        WND_MSG_LEGENDCUP_WARRECORD_SINGLE,
        WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW,
        WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_RECV,
        // 比赛列表界面
        WND_MSG_LEGENDCUP_CUPLIST_START = WndID.WND_ID_LEGENDCUP_CUPLIST * 1000,
        WND_MSG_LEGENDCUP_CUPLIST_SHOW,
        WND_MSG_LEGENDCUP_CUPLIST_RECVLIST,
        WND_MSG_LEGENDCUP_CUPLIST_UPDATELIST,
        WND_MSG_LEGENDCUP_CUPLIST_SYSTEM_CANCEL,
        // 比赛说明界面
        WND_MSG_LEGENDCUP_MSGBOXTIP_START = WndID.WND_ID_LEGENDCUP_MSGBOXTIP * 1000,
        WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW,
        WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW_STR,
        // 比赛创建界面
        WND_MSG_LEGENDCUP_MSGBOXCREATE_START = WndID.WND_ID_LEGENDCUP_MSGBOXCREATE * 1000,
        WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW,
        WND_MSG_LEGENDCUP_MSGBOXCREATE_CLOSE,
        // 比赛报名界面
        WND_MSG_LEGENDCUP_REGIST_START = WndID.WND_ID_LEGENDCUP_REGIST * 1000,
        WND_MSG_LEGENDCUP_REGIST_KINLIST,
        WND_MSG_LEGENDCUP_REGIST_MEMBERLIST,
        WND_MSG_LEGENDCUP_REGIST_CLOSE,
        // 比赛黑名单界面
        WND_MSG_LEGENDCUP_BLACKLIST_START = WndID.WND_ID_LEGENDCUP_BLACKLIST * 1000,
        WND_MSG_LEGENDCUP_BLACKLIST_SHOW,
        WND_MSG_LEGENDCUP_BLACKLIST_BLACKLIST,
        // 比赛密码界面
        WND_MSG_LEGENDCUP_PASSWORD_START = WndID.WND_ID_LEGENDCUP_PASSWORD * 1000,
        WND_MSG_LEGENDCUP_PASSWORD_SHOW,
        WND_MSG_LEGENDCUP_PASSWORD_CLOSE,
        // 比赛小组赛界面
        WND_MSG_LEGENDCUP_GROUP_START = WndID.WND_ID_LEGENDCUP_GROUP * 1000,
        WND_MSG_LEGENDCUP_GROUP_SHOW,
        WND_MSG_LEGENDCUP_GROUP_UPDATE,
        // 比赛16强以上淘汰赛界面
        WND_MSG_LEGENDCUP_BIGKNOCKOUT_START = WndID.WND_ID_LEGENDCUP_BIGKNOCKOUT * 1000,
        WND_MSG_LEGENDCUP_BIGKNOCKOUT_SHOW,
        WND_MSG_LEGENDCUP_BIGKNOCKOUT_UPDATE,
        // 比赛16强及以内淘汰赛界面
        WND_MSG_LEGENDCUP_SMALLKNOCKOUT_START = WndID.WND_ID_LEGENDCUP_SMALLKNOCKOUT * 1000,
        WND_MSG_LEGENDCUP_SMALLKNOCKOUT_SHOW,
        WND_MSG_LEGENDCUP_SMALLKNOCKOUT_UPDATE,
        // 比赛裁决界面
        WND_MSG_LEGENDCUP_JUDGE_START = WndID.WND_ID_LEGENDCUP_JUDGE * 1000,
        WND_MSG_LEGENDCUP_JUDGE_SHOW,
        // 比赛家族成员界面
        WND_MSG_LEGENDCUP_KINMEMBER_START = WndID.WND_ID_LEGENDCUP_KINMEMBER * 1000,
        WND_MSG_LEGENDCUP_KINMEMBER_SHOW,
        WND_MSG_LEGENDCUP_KINMEMBER_REGIST_SHOW,
        // 比赛家族信息界面
        WND_MSG_LEGENDCUP_KININFO_START = WndID.WND_ID_LEGENDCUP_KININFO * 1000,
        WND_MSG_LEGENDCUP_KININFO_SHOW,
        WND_MSG_LEGENDCUP_KININFO_RECV,
        WND_MSG_LEGENDCUP_KINGLORY_RECV,
        // 比赛奖励界面
        WND_MSG_LEGENDCUP_PRIZE_START = WndID.WND_ID_LEGENDCUP_PRIZE * 1000,
        WND_MSG_LEGENDCUP_PRIZE_SHOW,
        // 比赛创建类型界面
        WND_MSG_LEGENDCUP_CREATETYPE_STRAT = WndID.WND_ID_LEGENDCUP_CREATETYPE * 1000,
        WND_MSG_LEGENDCUP_CREATETYPE_SHOW,

        // 线上比赛重置界面
        WND_MSG_LEGENDCUP_ONLINE_RESET_START = WndID.WND_ID_LEGENDCUP_ONLINE_RESET * 1000,
        WND_MSG_LEGENDCUP_ONLINE_RESET_SHOW,
        WND_MSG_LEGENDCUP_ONLINE_RESET_CLOSE,

        // NPC界面
        WND_MSG_NPC_START = WndID.WND_ID_NPC * 1000,
        WND_MSG_NPC_RESPONSE,
        WND_MSG_NPC_CLOSE_DIALOGBOX,
        WND_MSG_NPC_BUY_GOODS_SUCCESS,

        // 消息邮箱窗口
        WND_MSG_MAILBOX_START = WndID.WND_ID_MAILBOX * 1000,
        WND_MSG_MAILBOX_OPEN,
        WND_MSG_MAIL_REQUESTMAILBASELIST,
        WND_MSG_MAIL_DETAILINFO,
        WND_MSG_MAIL_REQFASTRECVPLUS,
        WND_MSG_MAIL_NEWMAILCOUNT,
        WND_MSG_MAIL_RECVPLUSSUCCESS,

        // 奖励弹出界面
        WND_MSG_PRIZEPOPUP_START = WndID.WND_ID_PRIZEPOPUP * 1000,
        WND_MSG_PRIZEPOPUP_OPEN,
        WND_MSG_PRIZEPOPUP_CLOSE, 

        // 消费引导界面
        WND_MSG_CONSUMPTIONGUIDE_START = WndID.WND_ID_CONSUMPTIONGUIDE * 1000,
        WND_MSG_CONSUMPTIONGUIDE_SHOW,   
    
        // 奖励提示界面
        WND_MSG_PRIZETIPGUIDE_START = WndID.WND_ID_PRIZETIPGUIDE * 1000,
        WND_MSG_PRIZETIP_UPDATE,

        // 商城刷新卡牌界面
        WND_MSG_POINTSHOPREFLUSHCARD_START = WndID.WND_ID_POINTSHOPREFLUSHCARD * 1000,
        WND_MSG_POINTSHOPREFLUSHCARD_SHOW,
        WND_MSG_POINTSHOPREFLUSHCARD_CLOSE,

        // 物品配置界面
        WND_MSG_GOODSDISPOSITION_START = WndID.WND_ID_GOODSDISPOSITION * 1000,
        #region 非订阅的消息
        /// <summary> 开始拖拽物品 </summary>
        WND_MSG_GOODSDISPOSITION_BEGINDRAG,
        /// <summary> 拖拽中 </summary>
        WND_MSG_GOODSDISPOSITION_DRAGGING,
        /// <summary> 结束释放物品 </summary>
        WND_MSG_GOODSDISPOSITION_ENDDRAG,
        /// <summary> 更改英雄头像列表数据 </summary>
        WND_MSG_GOODSDISPOSITION_SET_HEROPORTRAITITEMLIST,
        /// <summary> 更改当前英雄 </summary>
        WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO,
        /// <summary> 更改当前英雄的物品配置列表 </summary>
        WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONLIST,
        /// <summary> 设置当前选择的配置物品 </summary>
        WND_MSG_GOODSDISPOSITION_SET_CURRENTDISPOSITION,
        /// <summary> 保存当前英雄的物品配置列表 </summary>
        WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION,
        /// <summary> 增加当前英雄的物品配置列表 </summary>
        WND_MSG_GOODSDISPOSITION_ADD_DISPOSITION,
        /// <summary> 关闭当前英雄的物品配置列表 </summary>
        WND_MSG_GOODSDISPOSITION_CLOSE_GOODSDISPOSITIONLIST,
        /// <summary> 当前选择的物品 </summary>
        WND_MSG_GOODSDISPOSITION_SET_CURRENTSELECTEDGOODS,
        /// <summary> 打开物品合成界面 </summary>
        WND_MSG_GOODSDISPOSITION_OPEN_GOODSSYNTHESISVIEW,
        /// <summary> 打开配置列表已满界面 </summary>
        WND_MSG_GOODSDISPOSITION_OPEN_DISPOSITIONFILLED,
        /// <summary> 复制当前物品配置 </summary>
        WND_MSG_GOODSDISPOSITION_COPY_CURRENTDISPOSITION,
        /// <summary> 更新配置列表已满界面的配置列表 </summary>
        WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONFILLEDLIST,
        #endregion
        /// <summary> 外部设置当前英雄，同时会打开配置界面 </summary>
        WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External,

        //物品配置分享界面
        WND_MSG_GOODSDISPOSITION_SHARE_START = WndID.WND_ID_GOODSDISPOSITION_SHARE * 1000,
        /// <summary> 设置分享物品配置数据，并显示 </summary>
        WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO,
        /// <summary> 保存该分享的物品配置信息 </summary>
        WND_MSG_GOODSDISPOSITION_SHARE_SAVE_DISPOSITIONINFO,
        WND_MSG_GOODSDISPOSITION_SHARE_CLOSE,

        // 聊天气泡
        WND_MSG_CHATBUBBLE_START = WndID.WND_ID_CHATBUBBLE * 1000,
		WND_MSG_CHATBUBBLE_NEW_MESSAGE,			// 新的聊天消息
		WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE,	// 新的聊天消息
		
		// 物品配置界面
        WND_MSG_ACCEPTWAIT_START = WndID.WND_ID_ACCEPTWAITINFO * 1000,
        WND_MSG_ACCEPTWAIT_SHOW,
        WND_MSG_ACCEPTWAIT_CLOSE,

        //战场额外的物品配置列表界面
        WND_MSG_WARGOODSDISPOSITION_START  = WndID.WND_ID_WarGoodsDisposition * 1000,

        WND_MSG_WARTOWER_START = WndID.WND_ID_WARTOWERMESSAGE * 1000,
        WND_MSG_WARTOWER_UPDATE,

        // 匹配时间信息界面WND_ID_MATCHTIMEINFO
        WND_MSG_PUNISH_TIMEINFO_START = WndID.WND_ID_PUNISH_TIMEINFO * 1000,
        WND_MSG_PUNISH_TIMEINFO_SHOW,
        WND_MSG_PUNISH_TIMEINFO_HIDE,


        WND_MSG_SIDEBTN_PUNISH_START = WndID.WND_ID_SIDEBTN_PUNISH_TIMEINFO * 1000,
        WND_MSG_SIDEBTN_PUNISH_SHOW,
        WND_MSG_SIDEBTN_PUNISH_HIDE,

        //FPS英雄子弹界面
        WND_MSG_FPSBULLET_START = WndID.WND_ID_FPSBULLET * 1000,
        WND_MSG_FPSBULLTE_UPDATECOUNT,

        // 本命英雄相关界面
        WND_MSG_LIFEHERO_START = WndID.WND_ID_LIFE_HERO_ENTRANCE * 1000,
        WND_MSG_UPDATENOVICEGUIDEDATA,  // 新手引导步骤变化时通知
        WND_MSG_RECV_ALL_LIFEHERO_INFO,   // 更新本命英雄信息
        WND_MSG_LIFEHERO_INJECT, // 向培养仓注入本命英雄
        WND_MSG_OPEN_LIFEHERO_SELECT,   // 打开本命英雄选择界面
        WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT,   // 本命英雄培养仓英雄提示
        WND_MSG_LIFEHERO_ENTRANCE_REFRESH_DATA,    // 刷新本命英雄入口UI
        WND_MSG_LIFEHERO_RECV_HISTORY_RECORD,      // 收到历史记录

        /// <summary>该窗口响应引导时所需的步骤 </summary>
        WND_MSG_LIFEHERO_GUIDE_ACTIONBEGIN,
        WND_MSG_LIFEHERO_GUIDE_ACTIONEND,
        WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONBEGIN,
        WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONEND,
        WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONBEGIN,
        WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONEND,

        // RANK点击界面
        WND_MSG_RANK_CLICK_START = WndID.WND_ID_RANK_CLICK * 1000,
        WND_MSG_RANK_CLICK_SHOW,
        WND_MSG_RANK_CLICK_CLOSE,

        // 任务界面
        WND_MSG_TASK_MAIN_START = WndID.WND_ID_TASK_MAIN * 1000,
        WND_MSG_TASK_MAIN_SHOW,
        WND_MSG_TASK_MAIN_UPDATE_HOLD,
        WND_MSG_TASK_MAIN_UPDATE_SEARCH,
        WND_MSG_TASK_MAIN_UPDATE_REDTIP,
        WND_MSG_TASK_MAIN_CLOSE_OTHER,
        WND_MSG_TASK_MAIN_ONCLICK_GOTO,

        // 任务发布界面
        WND_MSG_TASK_PUBLISH_START = WndID.WND_ID_TASK_PUBLISH * 1000,
        WND_MSG_TASK_PUBLISH_SHOW,
        WND_MSG_TASK_PUBLISH_CLOSE,
        
        // 任务展开界面
        WND_MSG_TASK_UNFOLD_START = WndID.WND_ID_TASK_UNFOLD * 1000,
        WND_MSG_TASK_UNFOLD_TASK_SHOW,
        WND_MSG_TASK_UNFOLD_PUBLISH_SHOW,
        WND_MSG_TASK_UNFOLD_UPDATE,
        WND_MSG_TASK_UNFOLD_DELETE,
        WND_MSG_TASK_UNFOLD_FINISH,

        // 任务刷新界面
        WND_MSG_TASK_REFLUSH_START = WndID.WND_ID_TASK_REFLUSH * 1000,
        WND_MSG_TASK_REFLUSH_SHOW,
        WND_MSG_TASK_REFLUSH_UPDATE,
        WND_MSG_TASK_REFLUSH_CLOSE,

        // 任务奖励界面
        WND_MSG_TASK_PRIZE_START = WndID.WND_ID_TASK_PRIZE * 1000,
        WND_MSG_TASK_PRIZE_SHOW,

        // 排位升级降级界面
        WND_MSG_WAR_END_RANKINFO_START = WndID.WND_ID_WAR_END_RANKINFO * 1000,
        WND_MSG_WAR_END_RANKINFO_SHOW,
        WND_MSG_WAR_END_RANKINFO_HIDE,

        // 游戏推广界面
        WND_MSG_GAME_PROMOTION_START = WndID.WND_ID_GAME_PROMOTION * 1000,
        WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO, // 更新下家信息
        WND_MSG_GAME_PROMOTION_UPDATE_NAV,
        WND_MSG_GAME_PROMOTION_REDIRECT_WEB,
        WND_MSG_GAME_PROMOTION_RET_RECOMMEND_DATA,
        WND_MSG_GAME_PROMOTION_OBTAIN_PRIZE_SUCCESS,
        WND_MSG_GAME_PROMOTION_SET_QRCODE,              // 设置二维码

        // 生存模式战场击杀助攻显示界面
        WND_MSG_BATTLEWAR_KA_START = WndID.WND_ID_BATTLEWAR_BATTLEWARKA * 1000,
        WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN,
        WND_MSG_BATTLEWAR_COLLECTING_GENIC,
        WND_MSG_BATTLEWAR_UPDATE_DEVILKING,                   // 更新魔王
        // 生存模式战场结算显示界面
        WND_MSG_BATTLEWAR_RECORDTABLE = WndID.WND_ID_BATTLEWAR_RECORDTABLE * 1000,
        WND_MSG_BATTLEWAR_RECORDTABLE_ACE,
        WND_MSG_BATTLEWAR_LIFESTATE,

        WND_MSG_WAR_UPDATE_HEROICON,

        // 战场观战界面
        WND_MSG_WAR_OBSERVER_START = WndID.WND_ID_WAR_OBSERVER * 1000,
        WND_MSG_XP_SKILL_UPDATE,
        WND_MSG_WAR_OBSERVER_SWITCH_HERO,
        WND_MSG_WAR_OBSERVER_HIDE_FRAME_PART,  // 隐藏指定Frame

        // 排位详情界面
        WND_MSG_RANK_DETAIL_START = WndID.WND_ID_RANK_DETAIL * 1000,
        WND_MSG_RANK_DETIAL_SHOW,
        WND_MSG_RANK_DETAIL_RECEIVE,
        
        
        // 排位邀请界面
        WND_MSG_RANK_INVITE_START = WndID.WND_ID_RANK_INVITE * 1000,
        WND_MSG_RANK_INVITE_SHOW,
        WND_MSG_RANK_INVITE_RETURN,

        // 排位历史记录界面
        WND_MSG_RANK_RECORD_START = WndID.WND_ID_RANK_RECORD * 1000,
        WND_MSG_RANK_RECORD_SHOW,
        WND_MSG_RANK_RECORD_RECEIVE,
    }
}
