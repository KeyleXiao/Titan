/*******************************************************************
** 文件名:	UDefines.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	与UI相关的一些公共定义
		
********************************************************************/

using System;
using UnityEngine;
using USpeedUI;

namespace USpeedUI
{
    #region 静态方法
    /// <summary>
    /// UI路径类
    /// </summary>
    public class UPath
    {
        /// <summary>
        /// UI控件根路径
        /// </summary>
        public const string UWidgetsPath = "UI/Widgets/";

        /// <summary>
        /// UI预制体路径
        /// </summary>
        public const string UPrefabsPath = "UI/Prefabs/";

        /// <summary>
        /// TooltipStyle预制体路径与名字
        /// <para>TooltipStyle预制体名字统一按"TooltipStyle_"来命名</para>
        /// </summary>
        public const string UTooltipStyle = "CommonPrefab/UI/TooltipStyle/TooltipStyle_";

        /// <summary>
        /// UI Shader根路径，Shader文件名统一用UI-***
        /// Shader文件声明的名字USpeedUI/Effects/***,每多一个/都用_代替
        /// </summary>
        public const string UEffectPath = "USpeedUI/Effects/";
        /// <summary>
        /// 美术特效根路径
        /// </summary>
        public const string UArtistEffectPath = "Effect/UI/";

        /// <summary>
        /// 美术字体存储的路径
        /// <para>完整文件名：ArtistFontPage0</para>
        /// <para>注意ArtistFont已经是文件名的一部分，另一部分是Page*,这部分根据EArtistFontPage枚举的_后半部分一致</para>
        /// </summary>
        public const string UArtistFontPath = "UI/ArtistFonts/ArtistFont";

        /// <summary>
        /// 资源的在各个游戏状态主目录
        /// 目前以下路径按游戏状态主目录的：
        /// Prefabs/UI/Effects/ 美术存放UI特效预制体的
        /// Prefabs/UI/  UI布局文件
        /// Artist\UI\View  非共享的UI图素文件（只有StaticState、DynamicState_Moba和LoginState）
        /// </summary>
        public const string UCommomState = "CommomState";
        public const string UStaticState = "StaticState";
        public const string UDynamicMobaState = "DynamicState_Moba";
        public const string UDynamicMcampState = "DynamicState_Mcamp";
        public const string ULoginState = "LoginState";
    }

    public class UDefines
    {
        public static string GetHeroDiffcultWordByType(EHeroDiffcultType eType)
        {
            if (eType < EHeroDiffcultType.EHDT_EASY || eType >= EHeroDiffcultType.EHDT_MAX)
            {
                return null;
            }

            return sDiffcultDesc[(int)eType];
        }

        // 星级跨阶段数字
        public static int UPGRADE_VALUE = 5;

        // 单属性信息个数
        public static int PROPERTY_COUNT = 4;

        public static int BOSS_BEGIN_ICON_ID = 0;

        // 战场最大人数
        public static int WAR_PERSON_MAX_COUNT = 10;

        // 物品格数量
        public static int WAR_ITEM_GRID_COUNT = 7;

        /// <summary>
        /// UI摄像机对象名字
        /// </summary>
        public const string UICamera = "UICamera";

        /// <summary>
        /// 游戏屏幕宽
        /// </summary>
        public const int USpeed_Screen_Width = 1920;
        /// <summary>
        /// 游戏屏幕高
        /// </summary>
        public const int USpeed_Screen_Height = 1080;
        /// <summary>
        /// 操作难度文本
        /// </summary>
        public static string[] sDiffcultDesc = new string[] { "简单", "中等", "困难" };
        /// <summary>
        /// 品級描述
        /// </summary>
        public static string[] sGemstoneGradeDesc = new string[] { "陈旧", "原石", "开光", "收藏", "珍藏", "传世", "稀世" };

        public const int nInlaySlotMax = 5;

        public const int nApparailSlotMax = 2;

		// 装备方案中最大装备数
		public const int nEquipSchemeMaxEquipNum = 12;

        // 战绩数据表格个数
        public static int[] DataArrayNum = { 7, 14, 7 };

        #region 当前版本通用颜色定义
        /// <summary>
        /// 当前版本的通用颜色
        /// 与 ECommonColor 枚举对应
        /// </summary>
        private static string[] m_CommomColor_Hex =
        {
            "FFFFFF",//ECommonColor.ECC_White
            "adadad",//ECommonColor.ECC_Gray
            "a8232e",//ECommonColor.ECC_Red1
            "55ea91",//ECommonColor.ECC_Green1
            "60e333",//ECommonColor.ECC_Green2
            "a5dbef",//ECommonColor.ECC_Blue1
            "48b7fe",//ECommonColor.ECC_Blue2
            "97149c",//ECommonColor.ECC_Purple1
            "ffca27",//ECommonColor.ECC_Gold1
            "4c4c4c",//ECommonColor.ECC_Gray2
        };

        public static string CommomColor_Hex(ECommonColor _eCommomColor)
        {
            return m_CommomColor_Hex[(int)_eCommomColor];
        }

        public static Color CommomColor(ECommonColor _eCommomColor)
        {
            return UExtensions.ColorExtension.FromHex(m_CommomColor_Hex[(int)_eCommomColor]);
        }
		#endregion

		#region 聊天框频道颜色
		private static Vector3[] m_ChatChannelColor =
		{
			new Vector3(255, 0, 0),		// CHAT_CHANNEL_SYSTEM
			new Vector3(232, 71, 2),	// CHAT_CHANNEL_WORLD
			new Vector3(224, 50, 209),	// CHAT_CHANNEL_MATCHROOM
			new Vector3(120, 120, 231),	// CHAT_CHANNEL_TEAMMATE
			new Vector3(2, 178, 224),	// CHAT_CHANNEL_WARSCENE
			new Vector3(204, 0, 255),	// CHAT_CHANNEL_PRIVATE
			new Vector3(64, 171, 85),	// CHAT_CHANNEL_KIN
			new Vector3(113, 216, 705),	// CHAT_CHANNEL_CLAN
			new Vector3(255, 255, 255),	// CHAT_CHANNEL_NEARBY
			new Vector3(0, 158, 204),	// CHAT_CHANNEL_ZONE
			new Vector3(255, 167, 226),	// CHAT_CHANNEL_CAMP

			new Vector3(255, 255, 255),	// CHAT_CHANNEL_MAX,默认颜色
		};

		public static Vector3 ChatChannelColor(int channelType)
		{
			// 传进来的频道无效,则使用默认颜色
			if (channelType < 0 || channelType >= m_ChatChannelColor.Length)
				channelType = m_ChatChannelColor.Length - 1;

			return m_ChatChannelColor[channelType];
		}

		// 天赋可激活的天赋总数
		public static int WAR_LEARN_TALENT_COUNT = 6;

        #endregion

        #region 游戏状态颜色
        // 游戏状态颜色 ACTOR_GAME_STATE
        public static string GetGameStateColor(int nState)
        {
            if (nState < (int)ACTOR_GAME_STATE.GAME_STATE_IDLE || nState >= (int)ACTOR_GAME_STATE.GAME_STATE_MAX)
                return "ffffff";

            return m_sGameStateColor[nState];
        }
        private static string[] m_sGameStateColor = new string[] 
        {
            "18ff00",        //GAME_STATE_IDLE = 0,
            "ffca27",        //GAME_STATE_MATCH = 1,
            "ffca27",        //GAME_STATE_INROOM = 2,
            "ffca27",        //GAME_STATE_WAR = 3,
            "adadad",        //GAME_STATE_OFFLINE = 4,
            "a82327",        //GAME_STATE_LEAVE = 5,
            "ffca27",        //GAME_STATE_OBSERVE = 6,
        };
        #endregion

        #region 游戏状态描述
        public static string GetGameStateDesc(int nState)
        {
            if (nState < (int)ACTOR_GAME_STATE.GAME_STATE_IDLE || nState >= (int)ACTOR_GAME_STATE.GAME_STATE_MAX)
                return "";

            return m_sGameStateDesc[nState];
        }
        private static string[] m_sGameStateDesc = new string[]
        {
            "ActorGameState_Idle",       //GAME_STATE_IDLE = 0,
            "ActorGameState_Match",      //GAME_STATE_MATCH = 1,
            "ActorGameState_InRoom",     //GAME_STATE_INROOM = 2,
            "ActorGameState_War",        //GAME_STATE_WAR = 3,
            "ActorGameState_OffLine",    //GAME_STATE_OFFLINE = 4,
            "ActorGameState_Leave",      //GAME_STATE_LEAVE = 5,
            "ActorGameState_Observe",    //GAME_STATE_OBSERVE = 6,
        };
        #endregion
        #region 游戏状态图标
        public static int GetGameStateIconID(int nState)
        {
            if (nState < (int)ACTOR_GAME_STATE.GAME_STATE_IDLE || nState >= (int)ACTOR_GAME_STATE.GAME_STATE_MAX)
                return 0;

            return m_nGameStateIcons[nState];
        }
        private static int[] m_nGameStateIcons = new int[]
        {
            1,     //GAME_STATE_IDLE = 0,
            2,     //GAME_STATE_MATCH = 1,
            2,     //GAME_STATE_INROOM = 2,
            2,     //GAME_STATE_WAR = 3,
            0,     //GAME_STATE_OFFLINE = 4,
            3,     //GAME_STATE_LEAVE = 5,
            2,     //GAME_STATE_OBSERVE = 6,
        };
        #endregion
    }

	public class TimeParser
    {
        /// 把秒转换成分钟
        public static int SecondToMinute(int Second)
        {
            decimal mm = (decimal)((decimal)Second / (decimal)60);
            return Convert.ToInt32(Math.Ceiling(mm));
        }
        #region 返回某年某月最后一天
        /// <param name="year">年份</param>
        /// <param name="month">月份</param>
        /// <returns>日</returns>
        public static int GetMonthLastDate(int year, int month)
        {
            DateTime lastDay = new DateTime(year, month, new System.Globalization.GregorianCalendar().GetDaysInMonth(year, month));
            int Day = lastDay.Day;
            return Day;
        }
        #endregion
        #region 返回时间差
        public static string DateDiff(DateTime DateTime1, DateTime DateTime2)
        {
            string dateDiff = null;
            try
            {
                //TimeSpan ts1 = new TimeSpan(DateTime1.Ticks);
                //TimeSpan ts = ts1.Subtract(ts2).Duration();
                TimeSpan ts = DateTime1 - DateTime2;
                if (ts.Days >= 1)
                {
                    dateDiff += ts.Days + "天";
                }

                if (ts.Hours >= 1)
                {
                    dateDiff += ts.Hours.ToString() + "小时";
                }

                if (ts.Minutes >= 1)
                {
                    dateDiff += ts.Minutes.ToString() + "分";
                }

            }
            catch
            { }
            return dateDiff;
        }
        #endregion
    }

    public class DealWithPlayerInfo
    {
        /// 把星级转化为阶段位 和 该阶段对应的等级
        public static void StarLvToGrade(int nStarLv, ref int nGrade, ref int nGradeLv)
        {
            nGrade = (nStarLv - 1) / UDefines.UPGRADE_VALUE + 1;
            nGradeLv = nStarLv % UDefines.UPGRADE_VALUE;
            if (nGradeLv == 0)
            {
                nGradeLv = UDefines.UPGRADE_VALUE;
            }

            if (nGrade > 3)
            {
                nGrade = 3;
                nGradeLv = UDefines.UPGRADE_VALUE;
            }
        }
    }
    #endregion

    #region 非系统级别的定义

    // 社会按钮标签类型
    public enum SocialFrameButtonType
    {
        BTN_CLAN,
        BTN_KIN,
        BTN_MENTORSHIP,
        BTN_MARRY,
        BTN_KINGDOMWAR,   
    }

    enum EN_HERO_GRADE
    {
        EHG_HERO_SILVER = 1,        // 银星
        EHG_HERO_GOLD,              // 金星
        EHG_HERO_DIAMOND,           // 钻石

        EHG_MAX,
    }
    public enum AchiIconType
    {
        Icon_MVP = 1,
        Icon_TopDamage,
        Icon_TopKill,
        Icon_TopAssist,
        Icon_TopTank,
        Icon_TopTowerKill,
        Icon_TopRich,
        Icon_TopFarm,
        Icon_Godlike,
        Icon_TripleKill,
        Icon_FourKill,
        Icon_FiveKill,
        Icon_AliveAssist,
        Icon_DeadlyControl,
        Icon_ThrilSave,
        Icon_TeamControl,
        Icon_KillDragon,        // 杀龙最高
        Icon_JoinBattle,    // 参战率最高
        Icon_MvpOfLoser,        // 失败方mvp
        Icon_Cure,        // 失败方mvp
    }

    public enum EHeroDiffcultType
    {
        EHDT_EASY,
        EHDT_MID,
        EHDT_DIF,

        EHDT_MAX,
    }

    // 线上线下类型
    public enum ECupType
    {
        ECT_Offline,        //线下赛
        ECT_Online,         //线上赛

        ECT_MAX,
    };

    // 是否联盟内部
    public enum EClanLimit
    {
        ECL_UnClanClimit,   // 非联盟内部
        ECL_ClanLimit,      // 联盟内部

        ECL_MAX,
    };

    public enum EConsumptionGuideType
    {
        Guide_HeroCard = 1,          // 勋章
        Guide_Gold,                  // 金币
        Guide_Point,                 // 元宝
        Guide_Xp,                    // 终结技
        Guide_Gemstone,              // 水晶
        Guide_PrizeChest,            // 宝箱
        Guide_Max,
    };

    public enum EConsumptionRequestType
    {
        Request_Upstar = 1,         // 晋升
        Request_UnlockXp,           // 解锁Xp
        Request_BuyGoods,           // 购买物品

        Request_Max,
    };
    /// <summary>
    /// 消息框按钮表现类型
    /// </summary>
    public enum EMessageBoxType : int
    {
        EMBT_None = 0,
        EMBT_OK = 1,
        EMBT_Cancel = 2,
        EMBT_OKCancel = 4
    }
    #endregion


    #region 系统级别的定义
    // 窗口层级定义
    public enum WndLayerDef
    {
        WND_LAYER_NONE,

        /// <summary>固定层</summary>
        WND_LAYER_FIXED,
        /// <summary>弹窗层01,高于固定层，低于全屏覆盖</summary>
        WND_LAYER_POPUP01,
        /// <summary>全屏覆盖层</summary>
        WND_LAYER_FILL,
        /// <summary>弹窗层02</summary>
        WND_LAYER_POPUP02,
        /// <summary>顶层</summary>
        WND_LAYER_OVERHEAD,


        WND_LAYER_MAX
    }

    //窗口互斥优先级定义
    public enum WndMutexLevelDef
    {
        WND_Mutex_None,
        //一般型互斥(Fill层默认值)
        WND_Mutex_Normal,
        //高等级互斥(目前给宝箱界面用，因为开宝箱动画中不允许打断)
        WND_Mutex_High,
        //最高级互斥(如选人，优先级最高)
        WND_Mutex_Highest,
    }

    public enum WndLoadMaskDef
    {
        WND_LoadMask_RPG = 1 << 1,
        WND_LoadMask_Moba = 1 << 2,
        WND_LoadMask_MCAMP = 1 << 3,
        WND_LoadMask_All = 0x7fffffff, // 最高符号位位0          
    }

    public enum WndPermissionMaskDef
    {
        WND_PM_Normal = 1 << 1,
        WND_PM_OB = 1 << 2,
        WND_PM_All = 0x7fffffff,       // 最高符号位位0      
    }

    /// <summary>
    /// TooltipTrigger类型
    /// 注意：这里类型前缀用ETS_,
    ///       后缀统一用TooltipStyle预制体名字下划线后半部分
    ///       新增枚举类型不能乱插，只能在该范围内往后添加
    /// </summary>
    public enum ETooltipStyle
    {
        ETS_Default,

        ETS_Clean = 1000,
        ETS_SummerInRoom,
        ETS_SNSBuddyInfo,
        ETS_ChatEmotion,
        ETS_GemstoneItem,
        ETS_CobstoneItem,
        ETS_EquipDesc,
        ETS_PlayerName,
        ETS_Property,
        ETS_Rune,
        ETS_SystemBtn,
        ETS_PointShop,
        ETS_TalentDesc,

        ETS_WarResultShow = 2000,
        ETS_WarBottomSkill,
        ETS_WarGoods,
        ETS_DeathReplayGood,
        ETS_DeathReplaySkill,
		ETS_WarStatActorInfo,		// 战绩表个人信息
    }

    /// <summary>
    /// UI 使用到的Shader类型
    /// 注意：这里类型前缀用 EST_ 
    ///         后缀统一用Shader文件声明的名字 UI/Effects/***,每多一个/都用_代替
    /// </summary>
    public enum EUIShaderType
    {
        EST_Gray,
    }

    /// <summary>
    /// 美术字符类型
    /// </summary>
    public enum EArtistFont
    {
        //0-9顺序的美术字类型起始码
        ///////////////第一套艺术字体数字字符分割
        /// <summary>
        /// 默认的艺术字样式，可以通过调整颜色，缩放来显示不同效果
        /// </summary>
        EAF_Defalut = 48,
        EAF_Style1 = 128,
        ///////////////第二套艺术字体数字字符分割

        ///////////////第一套艺术字体特殊字符分割
        /// <summary>
        /// 要使用特殊的字符往下面增加，这个只做标识用
        /// </summary>
        EAF_Special = 255,
        EAF_Crit = 256,
        EAF_LevelUp = 257,
        ///////////////第二套艺术字体特殊字符分割
    }

    /// <summary>
    /// 目前字体材质不支持多种材质，所以如果超出目前区域的话只能另起一个字体
    /// <para>注意命名，_后面的部分跟资源上命名对应</para>
    /// </summary>
    public enum EArtistFontPage
    {
        EAFP_Page0,
        EAFP_Page1,
    }

    /// <summary>
    /// 战场动态图:EDSM_VIEW名字_具体作用
    /// </summary>
    public enum EDynamicStateMoba
    {
        EDSM_TeamRightView_Our = 1,
        EDSM_TeamRightView_Enemy,

        EDSM_BroadcastMessage_WarEndAwakeWinFrame = 17,
        EDSM_BroadcastMessage_WarEndAwakeFailFrame,
        EDSM_BroadcastMessage_WarEnd1WinFrame,
        EDSM_BroadcastMessage_WarFailed1WinFrame,

        EDSM_ComboAttack_OneKillIcon = 27,
        EDSM_ComboAttack_DoubleKillIcon,
        EDSM_ComboAttack_TripleKillIcon,
        EDSM_ComboAttack_QuadraKillIcon,
        EDSM_ComboAttack_PentaKillIcon,
        EDSM_ComboAttack_ScoreIcon,
        EDSM_ComboAttack_NumZeroIcon,
        EDSM_ComboAttack_NumOneIcon,
        EDSM_ComboAttack_NumTwoIcon,
        EDSM_ComboAttack_NumThreeIcon,
        EDSM_ComboAttack_NumFourIcon,
        EDSM_ComboAttack_NumFiveIcon,
        EDSM_ComboAttack_NumSixIcon,
        EDSM_ComboAttack_NumSevenIcon,
        EDSM_ComboAttack_NumEightIcon,
        EDSM_ComboAttack_NumNineIcon,

        EDSM_HonorView_KillBigDragon = 43,
        EDSM_HonorView_KillSmallDragon,
        EDSM_HonorView_EconomyFirst,
        EDSM_HonorView_AssistFirst,
        EDSM_HonorView_TowelFirst,
        EDSM_HonorView_DeathFirst,

        EDSM_BroadcastMessage_TowelOur = 50,
        EDSM_BroadcastMessage_TowelEnemy,
        EDSM_BroadcastMessage_LargeDragon,
        EDSM_BroadcastMessage_SmallDragon,
        EDSM_BroadcastMessage_Soldier,
        //EDSM_BroadcastMessage_Monster,

        EDSM_BroadcastMessage_FiveAssist = 72,
        EDSM_BroadcastMessage_TenAssist,
        EDSM_BroadcastMessage_ThrillSave,
        EDSM_BroadcastMessage_DeadlyControl,
        EDSM_BroadcastMessage_TeamControlThree,
        EDSM_BroadcastMessage_TeamControlFour,
        EDSM_BroadcastMessage_TeamControlFive,

        EDSM_BroadcastMessage_FirstBlood_Self = 100,
        EDSM_BroadcastMessage_FirstBlood_Enmy,
        EDSM_BroadcastMessage_Ace_Self,
        EDSM_BroadcastMessage_Ace_Enmy,
        EDSM_BroadcastMessage_KillHead_Self,
        EDSM_BroadcastMessage_KillHead_Enmy,
        EDSM_BroadcastMessage_KillBottom_Self,
        EDSM_BroadcastMessage_KillBottom_Enmy,
        EDSM_BroadcastMessage_TimeKill_Self,
        EDSM_BroadcastMessage_TimeKill_Enmy,
        EDSM_BroadcastMessage_GodKill_Self,
        EDSM_BroadcastMessage_GodKill_Enmy,
        EDSM_BroadcastMessage_GodKillDes_Self,
        EDSM_BroadcastMessage_GodKillDes_Enmy,


        EDSM_BroadcastMessage_TipBigHead_Self = 150,
        EDSM_BroadcastMessage_TipBigHead_Enmy,
        EDSM_BroadcastMessage_TipSmallHead_Self,
        EDSM_BroadcastMessage_TipSmallHead_Enmy,
        EDSM_BroadcastMessage_TipFrame_Self,
        EDSM_BroadcastMessage_TipFrame_Enmy,
        EDSM_BroadcastMessage_TipKillIcon,

        EDSM_BroadcastMessage_Monster = 157, //预留到200


    }

    /// <summary>
    /// 战场外动态图:ESS_VIEW名字_具体作用
    /// </summary>
    public enum EStaticState
    {
        ESS_LegendCup_Pos1 = 1,
        ESS_LegendCup_Pos2,
        ESS_LegendCup_Pos3,

        ESS_HeroStar_RectFrame1 = 11,
        ESS_HeroStar_RectFrame2,
        ESS_HeroStar_RectFrame3,
        ESS_HeroStar_RectFrame4,
        ESS_HeroStar_RectFrame5,

        ESS_HeroStar_HexagonFrame0 = 20,
        ESS_HeroStar_HexagonFrame1,
        ESS_HeroStar_HexagonFrame2,
        ESS_HeroStar_HexagonFrame3,
        ESS_HeroStar_HexagonFrame4,
        ESS_HeroStar_HexagonFrame5,

    }

    /// <summary>
    /// 当前版本的通用颜色
    /// 与 UDefines.m_CommomColor_Hex 对应
    /// <para>颜色后面带的1、2、3...是指颜色由浅至深</para>     
    /// <para>使用时用UDefines.CommomColor_Hex() 和 UDefines.CommomColor()</para>     
    ///  </summary>
    public enum ECommonColor
    {
       ECC_White,
       ECC_Gray,
       ECC_Red1,
       ECC_Green1,
       ECC_Green2,
       ECC_Blue1,
       ECC_Blue2,
       ECC_Purple1,
       ECC_Gold1,
       ECC_Gray2,
    }
    #endregion
}
