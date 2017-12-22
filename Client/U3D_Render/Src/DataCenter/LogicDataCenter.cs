using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DataCenter
{
    /// <summary>
    /// 配置表数据处理接口
    /// </summary>
    public interface IModelConfig
    {
        void Load(string strPath);
        void UnLoad();
    }

    /// <summary>
    /// 配置表数据接口
    /// </summary>
    public interface IModelConfigData<T>
    {
        void CopyTo(ref T _tTargetValue);
    }

    public interface IModelData
    {
        void Init();
    }

    // 模块数据管理中心
    static public class LogicDataCenter
    {
        // 聊天消息管理器
        static public ChatMessageManager chatMessageManager = new ChatMessageManager();
        // 天赋数据管理器
        static public TalentPageDataManager talentPageDataManager = new TalentPageDataManager();
        // 符文数据管理器
        static public RunePageDataManager runePageDataManager = new RunePageDataManager();
        // 战绩数据管理器
        static public WarStatisticDataManager warStatisticDataManager = new WarStatisticDataManager();
        // 战队数据管理器
        static public KinDataManager kinDataManager = new KinDataManager();
        // 帮派数据管理器
        static public ClanDataManager clanDataManager = new ClanDataManager();
        // 物品数据管理器
        static public GoodsSchemesDataManager goodsSchemesDataManager { get { return GoodsSchemesDataManager.Instance; } }
        // 战绩信息管理器
        static public WarRecordTableDataManager recordTableDataManager = new WarRecordTableDataManager();
        // 组队数据管理器
        static public TeamDataManager teamDataManager = new TeamDataManager();
        // 小地图数据管理器
        static public WarMinimapDataManager warMinimapDataManager = new WarMinimapDataManager();
        // 比赛主界面数据管理
        static public MatchDataManager matchDataManager = new MatchDataManager();
        // 战场-Moba 物品数据管理器
        static public WarMobaGoodsDataManager warMobaGoodsDataManager { get { return WarMobaGoodsDataManager.Instance; } }
        // 准星数据管理器
        static public WarFrontSightDataManager frontSightDataManager { get { return WarFrontSightDataManager.Instance; } }
        // 奖励宝箱数据管理器
        static public PrizeChestDataManager prizeChestDataManager = new PrizeChestDataManager();
        //查看其他玩家数据管理器
        static public PlayerSystemDataManager playerSystemDataManager = new PlayerSystemDataManager();
        // 皮肤数据管理器
        static public HeroSkinDataManager heroSkinDataManager = new HeroSkinDataManager();
        // 商城数据管理器
        static public PointShopDataManager pointShopDataManager = new PointShopDataManager();
        // 战场右侧数据管理器
        static public TeamRightWndDataManager teamRightWndDataManager = new TeamRightWndDataManager();
        static public ChartDataManager chartDataManager = new ChartDataManager();
        // DIDA提示数据管理器
        static public DIDADataManager didaDataManager = new DIDADataManager();
        //PK数据管理器
        static public PkDataManager pkDataManager = new PkDataManager();
        // 侧边提示栏数据管理器
        static public SideButtonDataManager sideButtonDataManager = new SideButtonDataManager();
        // 跑马灯公告
        static public LoudspeakerManager  loudspeakerManager = new LoudspeakerManager();

        static public ViewPersonStateDataManager viewPersonStateDataManager = new ViewPersonStateDataManager();
        //战场死亡回放数据管理器
        static public WarDeathReplayDataManager deathReplayDataManager = new WarDeathReplayDataManager();
		// 比赛系统数据管理器
        static public LegendCupDataManager legendCupDataManager = new LegendCupDataManager();

		//任务数据管理器
        static public TaskDataManager taskDataManager = new TaskDataManager();

        //语音系统数据管理器
        static public VoiceSystemDataManager voiceSystemDataManager = new VoiceSystemDataManager();   
		    
		 //皇冠系统数据
        static public CrownPageDataManager crownPageDataManager = new CrownPageDataManager();
        // 师徒数据管理器
        static public MentorshipDataManager mentorshipDataManager = new MentorshipDataManager();
        // 英雄天赋数据
        static public HeroTalentDataManager heroTalentDataManager = new HeroTalentDataManager();
        // 英雄XP数据
        static public HeroXPSkillDataManager heroXPSkillDataManager = new HeroXPSkillDataManager();

        // SNS系统数据管理器
        static public SNSDataManager snsDataManager = new SNSDataManager();

        // NPC数据管理器
        static public NPCDataManager npcDataManager = new NPCDataManager();

        // SNS系统数据管理器
        static public MailDataManager mailDataManager = new MailDataManager();

        // 弹出菜单列表
        static public PopupMenuDataManager popupMenuDataManager { get { return PopupMenuDataManager.Instance; } }

        // 右键弹窗选项列表
        static public RightClickPlayerManager rightClickPlayerManager = new RightClickPlayerManager();

		// 标签数据管理
		static public WarLabelDataManager labelDataManager = new WarLabelDataManager();

        static public DistrictListDataManager districtListDataManger = new DistrictListDataManager();

        // 奖励提示数据管理器
        static public PrizeTipDataManager prizeTipDataManager = new PrizeTipDataManager();

		// 装备管理数据管理器
		static public EquipSchemeDataManager equipSchemeDataMagaer { get { return EquipSchemeDataManager.Instance;}}

		// 物品配置数据管理器
		static public GoodsDispositionDataManager goodsDispositionDataManager { get { return GoodsDispositionDataManager.Instance; } }

        static public ScreenCaptureDataManager screenCaptureDataManager = new ScreenCaptureDataManager();

		// 战场信息数据管理器
		static public WarInfoDataManager warInfoDataManager = new WarInfoDataManager();

        /// <summary>
        /// FPS子弹数据管理器
        /// </summary>
        static public FPSBulletDataManager fpsBulletDataManager = new FPSBulletDataManager();

        // 本命英雄数据管理器
        static public LifeHeroDataManager lifeHeroDataManager = new LifeHeroDataManager();

        // 本命英雄数据管理器
        static public RankDataManager rankDataManager = new RankDataManager();
		// 对话框数据回调管理器
        static public DialogDataManager dialogDataManager  { get{ return DialogDataManager.Instance; } }

        // 游戏推广数据管理器
        static public GamePromotionDataManager gamePromotionDataManager = new GamePromotionDataManager();

        static public WarOBDataManager warOBDataManager = new WarOBDataManager();

        // 观战系统UI数据管理
        static public WarOBUIDataManager warOBUIDataManager = new WarOBUIDataManager();

        // 玩家表情包数据管理
        static public EmojiDataManager emojiDataManager = new EmojiDataManager();

        //// 头像数据管理
        //static public ChangeHeadIconManager changeHeadIconManager = new ChangeHeadIconManager();

        // 初始化
        static public void Init()
        {
            Initialize.PrintTickCheck("LogicDataCenter",_bStart:true);

            popupMenuDataManager.Init();
            Initialize.PrintTickCheck("popupMenuDataManager.Init");

            chatMessageManager.Init();
            Initialize.PrintTickCheck("chatMessageManager.Init");

            warStatisticDataManager.Init();
            Initialize.PrintTickCheck("warStatisticDataManager.Init");

            kinDataManager.Init();
            Initialize.PrintTickCheck("kinDataManager.Init");

            clanDataManager.Init();
            Initialize.PrintTickCheck("clanDataManager.Init");

            recordTableDataManager.Init();
            Initialize.PrintTickCheck("recordTableDataManager.Init");

            teamDataManager.Init();
            Initialize.PrintTickCheck("teamDataManager.Init");

            warMinimapDataManager.Init();
            Initialize.PrintTickCheck("warMinimapDataManager.Init");

            matchDataManager.Init();
            Initialize.PrintTickCheck("matchDataManager.Init");

            goodsSchemesDataManager.Init();
            Initialize.PrintTickCheck("goodsSchemesDataManager.Init");

            warMobaGoodsDataManager.Init();
            Initialize.PrintTickCheck("warMobaGoodsDataManager.Init");

            frontSightDataManager.Init();
            Initialize.PrintTickCheck("frontSightDataManager.Init");

            playerSystemDataManager.Init();
            Initialize.PrintTickCheck("playerSystemDataManager.Init");

            prizeChestDataManager.Init();
            Initialize.PrintTickCheck("prizeChestDataManager.Init");

            pointShopDataManager.Init();
            Initialize.PrintTickCheck("pointShopDataManager.Init");

            teamRightWndDataManager.Init();
            Initialize.PrintTickCheck("teamRightWndDataManager.Init");

            chartDataManager.Init();
            Initialize.PrintTickCheck("chartDataManager.Init");

            didaDataManager.Init();
            Initialize.PrintTickCheck("didaDataManager.Init");

            pkDataManager.Init();
            Initialize.PrintTickCheck("pkDataManager.Init");

            viewPersonStateDataManager.Init();
            Initialize.PrintTickCheck("viewPersonStateDataManager.Init");

            loudspeakerManager.Init();
            Initialize.PrintTickCheck("loudspeakerManager.Init");

            deathReplayDataManager.Init();
            Initialize.PrintTickCheck("deathReplayDataManager.Init");

            legendCupDataManager.Init();
            Initialize.PrintTickCheck("legendCupDataManager.Init");

            taskDataManager.Init();
            Initialize.PrintTickCheck("taskDataManager.Init");

            voiceSystemDataManager.Init();
            Initialize.PrintTickCheck("voiceSystemDataManager.Init");

            mentorshipDataManager.Init();
            Initialize.PrintTickCheck("mentorshipDataManager.Init");

            snsDataManager.Init();
            Initialize.PrintTickCheck("snsDataManager.Init");

            npcDataManager.Init();
            Initialize.PrintTickCheck("npcDataManager.Init");

            mailDataManager.Init();
            Initialize.PrintTickCheck("mailDataManager.Init");

            rightClickPlayerManager.Init();
            Initialize.PrintTickCheck("rightClickPlayerManager.Init");

            labelDataManager.Init();
            Initialize.PrintTickCheck("labelDataManager.Init");

            districtListDataManger.Init();
            Initialize.PrintTickCheck("districtListDataManger.Init");

            prizeTipDataManager.Init();
            Initialize.PrintTickCheck("prizeTipDataManager.Init");

            equipSchemeDataMagaer.Init();
            Initialize.PrintTickCheck("equipSchemeDataMagaer.Init");

            goodsDispositionDataManager.Init();
            Initialize.PrintTickCheck("goodsDispositionDataManager.Init");

            screenCaptureDataManager.Init();
            Initialize.PrintTickCheck("screenCaptureDataManager.Init");

            runePageDataManager.Init();
            Initialize.PrintTickCheck("runePageDataManager.Init");

            warInfoDataManager.Init();
            Initialize.PrintTickCheck("warInfoDataManager.Init");

            fpsBulletDataManager.Init();
            Initialize.PrintTickCheck("fpsBulletDataManager.Init");

            lifeHeroDataManager.Init();
            Initialize.PrintTickCheck("lifeHeroDataManager.Init");

            rankDataManager.Init();
            Initialize.PrintTickCheck("rankDataManager.Init");

            dialogDataManager.Init();
            Initialize.PrintTickCheck("dialogDataManager.Init");

            gamePromotionDataManager.Init();
            Initialize.PrintTickCheck("gamePromotionDataManager.Init");

            warOBDataManager.Init();
            Initialize.PrintTickCheck("warOBDataManager.Init");

            warOBUIDataManager.Init();
            Initialize.PrintTickCheck("warOBUIDataManager.Init");

            emojiDataManager.Init();
            Initialize.PrintTickCheck("emojiDataManager.Init");

            //changeHeadIconManager.Init();
            //Initialize.PrintTickCheck("changeHeadIconManager.Init");
        }

        static public void Clear()
        {
            chatMessageManager.Clear();
            pointShopDataManager.Clear();
            teamRightWndDataManager.Clear();
            didaDataManager.Clear();
            sideButtonDataManager.Clear();
            prizeChestDataManager.Clear();
            loudspeakerManager.Clear();
            deathReplayDataManager.Clear();
            legendCupDataManager.Clear();
			voiceSystemDataManager.Clear();
            crownPageDataManager.Clear();
            heroSkinDataManager.Clear();
            heroTalentDataManager.Clear();
            snsDataManager.Clear();
            npcDataManager.Clear();
            popupMenuDataManager.Clear();
			frontSightDataManager.Clear();
			labelDataManager.Clear();
            districtListDataManger.Clear();
            chartDataManager.Clear();
            prizeTipDataManager.Clear();
			equipSchemeDataMagaer.Clear();
            goodsDispositionDataManager.Clear();
            screenCaptureDataManager.Clear();
            runePageDataManager.Clear();
            clanDataManager.Clear();
            kinDataManager.Clear();
			warInfoDataManager.Clear();
            warStatisticDataManager.Clear();
            matchDataManager.Clear();
            fpsBulletDataManager.Clear();
            lifeHeroDataManager.Clear();
            rankDataManager.Clear();
            dialogDataManager.Clear();
            warOBDataManager.Clear();
            warOBUIDataManager.Clear();
            emojiDataManager.Clear();
            //changeHeadIconManager.Clear();
        }

        static public void ClearInEnterStaticGameState()
        {
            // TODO : 进入房间后，点返回会清理掉玩家数据，
            //playerSystemDataManager.Clear();
        }

        static public void onEnterLoginMobaState()
        {
            heroTalentDataManager.onEnterLoginMobaState();
        }

        // 进入战场
        static public void onEnterBattleState()
        {
            heroTalentDataManager.onEnterBattleState();
        }

        // 离开战场
		static public void onLeaveBattleState()
        {
            warInfoDataManager.onLeaveBattleState();
            recordTableDataManager.onLeaveBattleState();
            teamRightWndDataManager.onLeaveBattleState();
            warOBDataManager.onLeaveBattleState();
            heroTalentDataManager.onLeaveBattleState();
        }

        // 进入登录界面
        static public void onEnterLoginState()
        {
            warInfoDataManager.onEnterLoginState();
            lifeHeroDataManager.onEnterLoginState();
            warOBDataManager.onEnterLoginState();
        }
    }
}
