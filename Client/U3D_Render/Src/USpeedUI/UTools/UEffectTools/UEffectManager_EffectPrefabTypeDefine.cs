using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace USpeedUI.UEffect
{
    /// <summary>
    /// 命名规则：
    /// UET_View预制体名字/游戏状态名字_描述
    /// </summary>
    public enum UEffectPrefabType
    {
        /// <summary>这个没对应的效果</summary>
        UEPT_None,

        //////////////////////////////////  UCommonState
        UEPT_LoadingBarView__Star_L,
        ///////////////////////////////////////////////

        //////////////////////////////////  ULoginState
        UEPT_CreateHeroView_BtnConfirm_Up,
        UEPT_CreateHeroView_BtnConfirm_Over,
        UEPT_CreateHeroView_BtnConfirm_Down,
        UEPT_CreateHeroView_BtnConfirm_CanInteractable,
        ///////////////////////////////////////////////

        /////////////////////////////////  UStaticState
        /// <summary>开战按钮流光</summary>
        UEPT_PlayerSysButton_BtnWarHall,

        UEPT_HeroDetailInfo_UpstarButton,
        UEPT_HeroDetailInfo_UpstarPrizeItem,

        UEPT_HeroPreviewFrame_GradeFrame1,
        UEPT_HeroPreviewFrame_GradeFrame2,
        UEPT_HeroPreviewFrame_GradeFrame3,
        UEPT_HeroPreviewFrame_GradeFrame4,
        UEPT_HeroPreviewFrame_GradeFrame5,
        UEPT_HeroPreviewFrame_GradeFrame6,

        UEPT_GameWaitingRoomView_GradeFrame1,
        UEPT_GameWaitingRoomView_GradeFrame2,
        UEPT_GameWaitingRoomView_GradeFrame3,
        UEPT_GameWaitingRoomView_GradeFrame4,
        UEPT_GameWaitingRoomView_GradeFrame5,
        UEPT_GameWaitingRoomView_GradeFrame6,

        UEPT_PlayerBaseInfo_RankIcon1,
        UEPT_PlayerBaseInfo_RankIcon2,
        UEPT_PlayerBaseInfo_RankIcon3,
        UEPT_PlayerBaseInfo_RankIcon4,
        UEPT_PlayerBaseInfo_RankIcon5,
        UEPT_PlayerBaseInfo_RankIcon6,

        UEPT_LegendCupListView_ConfineFrame_beisai01,
        UEPT_LegendCupListView_ConfineFrame_beisai02,
        UEPT_LegendCupSmallKnockoutView_JoinMatchBtn,

        UEPT_TaskMain_BgImage,
        UEPT_TaskMain_TaskTemplate,
        UEPT_TaskPublish_Send,
        UEPT_TaskUnfold_Delete,
        UEPT_TaskUnfold_Finish,
        UEPT_TaskUnfold_Search,
        UEPT_TaskMain_FinishState,

        UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_B,
        UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_R,
        UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_B,
        UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_R,
        UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_B,
        UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_R,
        UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_B,
        UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_R,
        ///////////////////////////////////////////////

        ///////////////////////////    UDynamicMobaState
        /// <summary>Moba战场失败效果</summary>
        UEPT_UEndOfWarView_Failure,
        /// <summary>Moba战场胜利</summary>
        UEPT_UEndOfWarView_Victory,
        ///////////////////////////////////////////////
        /// <summary>战斗底部界面xp技能头像效果</summary>
        UEPT_UWarBottomCenter_XpSkillFire,
        /// <summary>队伍右侧界面xp技能头像效果</summary>
        UEPT_UTeamRight_XpSkillFire,
        /// XP充能完成效果
        UEPT_UWarBottomCenter_XpSkillChargeOver,
        /// <summary>房间选人效果</summary>
        UEPT_UWaitingRoom_HeroModelLight,
        UEPT_UWaitingRoom_ReadyButtonLight,
        UEPT_UWaitingRoom_SelectOverLight,
        UEPT_UWaitingRoom_SelectDownLight,
        UEPT_UWaitingRoom_HeroSkinLight,

        UEPT_UWarRecordTableEnd_Medal_A_Continue,
        UEPT_UWarRecordTableEnd_Medal_B_Continue,
        UEPT_UWarRecordTableEnd_Medal_S_Continue,
        UEPT_UWarRecordTableEnd_Medal_C_Continue,
        UEPT_UWarRecordTableEnd_Medal_A,
        UEPT_UWarRecordTableEnd_Medal_B,
        UEPT_UWarRecordTableEnd_Medal_S,
        UEPT_UWarRecordTableEnd_Medal_C,
        UEPT_UWarRecordTableEnd_Medal_A_ADD,
        UEPT_UWarRecordTableEnd_Medal_B_ADD,
        UEPT_UWarRecordTableEnd_Medal_C_ADD,
        UEPT_UWarRecordTableEnd_HeroUpgrade,
        UEPT_UWarRecordTableEnd_ScoreEffect,

        UEPT_UHeroTalent_TalentOpenButtonEffect,
        UEPT_UHeroTalent_TalentOpenButtonEffectForNewbie,

        UEPT_MapSignal_AttackSign,
        UEPT_MapSignal_RetreatSign,
        UEPT_MapSignal_DangerSign,
        UEPT_MapSignal_OnWaySign,
        UEPT_MapSignal_HelpMeSign,
        UEPT_MapSignal_LostEyeSign,

        UEPT_HeroUpStar_Success,

        UEPT_FullScreen_Cloud,

        UEPT_PKInfo_PKStart,
        UEPT_PKInfo_PKWin,
        UEPT_PKInfo_PKLost,
        UEPT_PKInfo_PKTie,

        UEPT_Shop_Hexagon_Blue,
        UEPT_Shop_Hexagon_Orange,

        UEPT_DIDAView_DiDa,

        /// <summary>施法条光效</summary>
        UEPT_ProgressBarWndView_DuTiao,

        /// <summary>开战引导选择效果</summary>
        UEPT_WarMain_Select,
        UEPT_WarMain_SelectEX,
        UEPT_WarMain_CanJoinEffect,
        UEPT_WarMain_RankEffect1,
        UEPT_WarMain_RankEffect2,
        UEPT_WarMain_RankEffect3,
        UEPT_WarMain_RankEffect4,
        UEPT_WarMain_RankEffect5,
        UEPT_WarMain_RankEffect6,


        // 解锁效果
        UEPT_Unlock_Actor_CurLevelUnlock,

        UEPT_MiniMap_BigDragonFire,
        UEPT_MiniMap_SmallDragonFire,

        // 获得金币光效
        UEPT_TeammateView_AddGold,

        //新手引导光效
        UEPT_GuideWidget_TalentLearnItem,
        UEPT_GuideWidget_WarHallButton,
        UEPT_GuideWidget_GameModeButton,
        UEPT_GuideWidget_GameModeButton_Small,
        UEPT_GuideWidget_SelectHero,
        UEPT_GuideWidget_GameStart,
        UEPT_GuideWidget_MouseRotate,
        UEPT_GuideWidget_MouseRotate2,
        UEPT_GuideWidget_ExitButton_huicheng,
        UEPT_GuideWidget_LifeHero_Button,
        UEPT_GuideWidget_LifeHero_jiantou01,
        UEPT_GuideWidget_Reward_heroUnlock,

        //商店
        UEPT_PointShop_PayBtn,

        // 残血特效
        UEPT_UWarBottomCenter_LackHP,

        // 激活大招特效
        UEPT_UWarBottomCenter_ActiveSkill_Large,

        // 本命英雄培养往上填充光效
        UEPT_LifeHero_FillUp,
        // 本命英雄注入界面标题爆炸效果
        UEPT_LifeHero_InjectFrame_Title,
        // 本命英雄获取奖励按钮效果
        UEPT_LifeHero_PrizeFrame_ObtainBtn,
        // 本命英雄入口UI光效1
        UEPT_LifeHero_Entrance_01,
        // 本命英雄入口UI光效2
        UEPT_LifeHero_Entrance_02,
        // 本命英雄满足基因强化的和觉醒条件的时候的光效
        UEPT_LifeHero_Can_AddHero_Upstar,
        // 大逃杀战场结束时弹窗界面胜利光效
        UEPT_EndOfBattleWar_Win,
        // 大逃杀战场结束时弹窗界面失败光效
        UEPT_EndOfBattleWar_Failure,

        /// <summary>这个没对应的效果</summary>
        UEPT_Max
    }

    public partial class UEffectManager
    {
        protected static Dictionary<UEffectPrefabType, UEffectDataParam> GetEffectDataTable()
        {
           return new Dictionary<UEffectPrefabType, UEffectDataParam>()
           {
                { UEffectPrefabType.UEPT_LoadingBarView__Star_L,new UEffectDataParam(UPath.UCommomState + "/ef_UI_LoadingBarView_Star_L") },

                { UEffectPrefabType.UEPT_UEndOfWarView_Failure,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_UEndOfWarView_StateImage_failure") },
                { UEffectPrefabType.UEPT_UEndOfWarView_Victory,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_UEndOfWarView_StateImage_victory") },

                { UEffectPrefabType.UEPT_UWarBottomCenter_XpSkillFire,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarBottomCenter_PersenPart_HeroIcon_fireblue") },
                { UEffectPrefabType.UEPT_UTeamRight_XpSkillFire,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_TeamRightView_MyTeamView_HeadIcon_fireblue") },
                { UEffectPrefabType.UEPT_UWarBottomCenter_XpSkillChargeOver,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_XpSkill_Xp01") },

                { UEffectPrefabType.UEPT_UWarBottomCenter_LackHP,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarBottomCenter_PersenPart_HpBar") },
                { UEffectPrefabType.UEPT_UWarBottomCenter_ActiveSkill_Large,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarBottomCenter_SkillPart_SkillItemLarge01") },


                { UEffectPrefabType.UEPT_UWaitingRoom_HeroModelLight,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_BasePic_liudong") },
                { UEffectPrefabType.UEPT_UWaitingRoom_ReadyButtonLight,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_StartGameButton_zhunbei") },
                { UEffectPrefabType.UEPT_UWaitingRoom_SelectOverLight,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_DefaultItem_kuang") },
                { UEffectPrefabType.UEPT_UWaitingRoom_SelectDownLight,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_DefaultItem_bao") },
                { UEffectPrefabType.UEPT_UWaitingRoom_HeroSkinLight,new UEffectDataParam(UPath.UStaticState + "/ef_UI_StaticState") },

                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A_Continue,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_ZiSe_CX") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B_Continue,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_LanSe_CX") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_S_Continue,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_JinSe_CX") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C_Continue,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_lvse_CX") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_A") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_B") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_S,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_S") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_C") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A_ADD,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_A+") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B_ADD,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_B+") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C_ADD,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_RecordTableEnd4_MedalGroup_C+") },

                { UEffectPrefabType.UEPT_UWarRecordTableEnd_HeroUpgrade,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_ShouSheng_dengjitisheng") },
                { UEffectPrefabType.UEPT_UWarRecordTableEnd_ScoreEffect,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_WarRecordTable_End_MedalGroup_shengya") },

                { UEffectPrefabType.UEPT_UHeroTalent_TalentOpenButtonEffect,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBottomCenter_ButtonLeftPart_FunctionButtonFrame_TianFu") },
                { UEffectPrefabType.UEPT_UHeroTalent_TalentOpenButtonEffectForNewbie,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBottomCenter_ButtonLeftPart_FunctionButtonFrame_KeXueTianFu") },

                { UEffectPrefabType.UEPT_MapSignal_AttackSign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_tishi_l") },
                { UEffectPrefabType.UEPT_MapSignal_RetreatSign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_tishi_h") },
                { UEffectPrefabType.UEPT_MapSignal_DangerSign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_weixiang") },
                { UEffectPrefabType.UEPT_MapSignal_OnWaySign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_zhengzailushang") },
                { UEffectPrefabType.UEPT_MapSignal_HelpMeSign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_xiezhu") },
                { UEffectPrefabType.UEPT_MapSignal_LostEyeSign,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_Image_direnbujian") },

                { UEffectPrefabType.UEPT_HeroUpStar_Success,new UEffectDataParam(UPath.UStaticState + "/ef_UI_MiddleLayer_HeroPreviewFrame_shengxingchengogn") },

                { UEffectPrefabType.UEPT_FullScreen_Cloud,new UEffectDataParam(UPath.UCommomState + "/ef_UI_CommomState_yan") },

                { UEffectPrefabType.UEPT_PKInfo_PKStart,new UEffectDataParam(UPath.UStaticState + "/ef_UI_StaticState_daojishi")},
                { UEffectPrefabType.UEPT_PKInfo_PKLost,new UEffectDataParam(UPath.UStaticState + "/ef_UI_StaticState_shibai")},
                { UEffectPrefabType.UEPT_PKInfo_PKWin,new UEffectDataParam(UPath.UStaticState + "/ef_UI_StaticState_shengli")},
                { UEffectPrefabType.UEPT_PKInfo_PKTie,new UEffectDataParam(UPath.UStaticState + "/ef_UI_StaticState_pingju")},

                { UEffectPrefabType.UEPT_Shop_Hexagon_Blue,new UEffectDataParam(UPath.UCommomState + "/ef_UI_PointShop_liubianxing_blue") },
                { UEffectPrefabType.UEPT_Shop_Hexagon_Orange,new UEffectDataParam(UPath.UCommomState + "/ef_UI_PointShop_liubianxing_orangey") },

                { UEffectPrefabType.UEPT_DIDAView_DiDa,new UEffectDataParam(UPath.UStaticState + "/ef_UI_DIDAView_DiDa") },

                { UEffectPrefabType.UEPT_WarMain_Select,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_KaiZhanYinDao") },
                { UEffectPrefabType.UEPT_WarMain_SelectEX,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_KaiZhanYinDaoXuanZhong") },
                { UEffectPrefabType.UEPT_WarMain_CanJoinEffect,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_Mode3") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect1,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon01") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect2,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon02") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect3,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon03") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect4,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon04") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect5,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon05") },
                { UEffectPrefabType.UEPT_WarMain_RankEffect6,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_RankIcon06") },

                { UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Up,new UEffectDataParam(UPath.UStaticState + "/ef_UI_CreateHeroView_SubFrame01_BtnConfine_Up") },
                { UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Over,new UEffectDataParam(UPath.UStaticState + "/ef_UI_CreateHeroView_SubFrame01_BtnConfine_Over") },
                { UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Down,new UEffectDataParam(UPath.UStaticState + "/ef_UI_CreateHeroView_SubFrame01_BtnConfine_Down") },
                { UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_CanInteractable,new UEffectDataParam(UPath.UStaticState + "/ef_UI_CreateHeroView_SubFrame01_BtnConfine_WaiQuan") },

                { UEffectPrefabType.UEPT_ProgressBarWndView_DuTiao,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_ProgressBar_Attach_dutiao") },

                { UEffectPrefabType.UEPT_Unlock_Actor_CurLevelUnlock,new UEffectDataParam(UPath.UCommomState + "/ef_UnlockActorPrizeItem_jiesuobaoxiang") },

                { UEffectPrefabType.UEPT_MiniMap_BigDragonFire,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_dalonghuotuan1") },
                { UEffectPrefabType.UEPT_MiniMap_SmallDragonFire,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_Minimap_MapFrame_xiaolonghuotuan1") },

                { UEffectPrefabType.UEPT_TeammateView_AddGold,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TeammateView_jinbiguangxiao") },
                { UEffectPrefabType.UEPT_GuideWidget_TalentLearnItem,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_HeroTalentView_WarTalentPart_LearnItem") },
                { UEffectPrefabType.UEPT_GuideWidget_WarHallButton,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerSysButton_TopFrame_WarHallButton_kaizhan") },
                { UEffectPrefabType.UEPT_GuideWidget_GameModeButton,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainModeFrame_xinshouyindao_da") },
                { UEffectPrefabType.UEPT_GuideWidget_GameModeButton_Small,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainModeFrame_xinshouyindao_xiao")},
                { UEffectPrefabType.UEPT_GuideWidget_LifeHero_Button,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarMainFrame_ListWarMainMode_GameModeItem") },
                { UEffectPrefabType.UEPT_GuideWidget_SelectHero,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_SelectHeroPart_HeroItem") },
                { UEffectPrefabType.UEPT_GuideWidget_GameStart,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_BottomFunctionPart_GameStartItem") },
                { UEffectPrefabType.UEPT_GuideWidget_Reward_heroUnlock,new UEffectDataParam(UPath.UStaticState + "/ef_ui_GuideView_Reward_Guide_yingxiongjiesuo")},
                { UEffectPrefabType.UEPT_PlayerSysButton_BtnWarHall,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerSysButton_TopFrame_WarHallButton_kaizhananniu") },

                { UEffectPrefabType.UEPT_HeroDetailInfo_UpstarButton,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroDetailInfo_CommonFrame_UpdateStarItem") },
                { UEffectPrefabType.UEPT_HeroDetailInfo_UpstarPrizeItem,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroDetailInfo_PandectFame_UpStarPrizeItem02") },
                { UEffectPrefabType.UEPT_GuideWidget_MouseRotate,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_GuideView_MouseRotate") },
                { UEffectPrefabType.UEPT_GuideWidget_MouseRotate2,new UEffectDataParam(UPath.UDynamicMobaState + "/ef_UI_GuideView_MouseRotate02") },

                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame1,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem01") },
                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame2,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem02") },
                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame3,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem03") },
                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame4,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem04") },
                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame5,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem05") },
                { UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame6,new UEffectDataParam(UPath.UStaticState + "/ef_UI_HeroPreviewFrame_TileViewHero_GradeFrameItem06") },

                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame1,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem01") },
                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame2,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem02") },
                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame3,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem03") },
                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame4,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem04") },
                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame5,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem05") },
                { UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame6,new UEffectDataParam(UPath.UStaticState + "/ef_UI_GameWaitingRoomView_TeamPart_TeamItem06") },


                { UEffectPrefabType.UEPT_PointShop_PayBtn,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PointShop_TopFrame_ShopPayBtn") },

                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon1,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon01") },
                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon2,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon02") },
                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon3,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon03") },
                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon4,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon04") },
                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon5,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon05") },
                { UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon6,new UEffectDataParam(UPath.UStaticState + "/ef_UI_PlayerBaseInfo_Rank_RankIcon06") },

                { UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LegendCupListView_ConfineFrame_beisai01") },
                { UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LegendCupListView_ConfineFrame_beisai02") },
                { UEffectPrefabType.UEPT_LegendCupSmallKnockoutView_JoinMatchBtn,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LegendCupSmallKnockoutView_JoinMatchBtnFrame_canjiabisai") },
                
                { UEffectPrefabType.UEPT_LifeHero_FillUp,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHero_LifeHeroCulture_benmingyingxiong") },

                {UEffectPrefabType.UEPT_GuideWidget_ExitButton_huicheng,new UEffectDataParam(UPath.UStaticState+"/ef_UI_WarRecordTable_End_MenuButton_ExitButton_huicheng") },
                {UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01,new UEffectDataParam(UPath.UStaticState+"/ef_UI_LifeHero_jiantou01") },
                { UEffectPrefabType.UEPT_LifeHero_InjectFrame_Title,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHero_LifeHeroCulture_juexing") },
                { UEffectPrefabType.UEPT_LifeHero_PrizeFrame_ObtainBtn,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHeroCulture_PrizeFrame_ObtainBtn") },
                { UEffectPrefabType.UEPT_LifeHero_Entrance_01,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHeroEntrance_benminganniu01") },
                { UEffectPrefabType.UEPT_LifeHero_Entrance_02,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHeroEntrance_benminganniu02") },
                { UEffectPrefabType.UEPT_LifeHero_Can_AddHero_Upstar,new UEffectDataParam(UPath.UStaticState + "/ef_UI_LifeHero_LifeHeroCulture_benmingliubian01") },
                { UEffectPrefabType.UEPT_EndOfBattleWar_Win,new UEffectDataParam(UPath.UDynamicMcampState + "/ef_UI_EndOfBattleWar_AceFrame_chengse") },
                { UEffectPrefabType.UEPT_EndOfBattleWar_Failure,new UEffectDataParam(UPath.UDynamicMcampState + "/ef_UI_EndOfBattleWar_AceFrame_lanse") },

                { UEffectPrefabType.UEPT_TaskMain_BgImage,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskMainView_BgImage_scene") },
                { UEffectPrefabType.UEPT_TaskMain_TaskTemplate,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskMainView_TaskTemplate_renwu") },
                { UEffectPrefabType.UEPT_TaskPublish_Send,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskPublishView_fasong") },
                { UEffectPrefabType.UEPT_TaskUnfold_Delete,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskUnfoldView_delete") },
                { UEffectPrefabType.UEPT_TaskUnfold_Finish,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskUnfoldView_FinishRT_finish") },
                { UEffectPrefabType.UEPT_TaskUnfold_Search,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskMainView_BgAttachPoint_saomiao") },
                { UEffectPrefabType.UEPT_TaskMain_FinishState,new UEffectDataParam(UPath.UStaticState + "/ef_UI_TaskMainView_TaskIcon_wancheng") },
           
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_B,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_shuangsha_B") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_R,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_shuangsha_R") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_B,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_sansha_B") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_R,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_sansha_R") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_B,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_sisha_B") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_R,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_sisha_R") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_B,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_wusha_B") },
                { UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_R,new UEffectDataParam(UPath.UStaticState + "/ef_UI_WarBroadcastMessageMgrView_TimeKillFrame_wusha_R") },
           };

        }
    }
}
