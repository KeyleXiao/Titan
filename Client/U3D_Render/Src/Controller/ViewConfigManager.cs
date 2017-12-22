/*******************************************************************
** 文件名:	ViewConfigManager.cs
** 修改人: 林建聪
** 日  期: 2015/07/29
** 描  述:  加载只有显示层才会用到的配置文件
** 版  本:	1.0
** 应  用:  
 * 
*******************************************************************
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2016.12.29
** 描  述: 目前只有scp文件夹发布后制作成mpk包
           1、ViewConfigManager.DataScpPath   Data/Scp文件夹路径
           2、ViewConfigManager.DataUserPath  Data/User 文件夹路径
           3、ViewConfigManager只用于编辑器工具(没运行Initialize时使用)   Data/Scp路径
********************************************************************/

using ASpeedGame.Data.CameraConfig;
using ASpeedGame.Data.ChatSettingConfig;
using ASpeedGame.Data.ClanConfig;
using ASpeedGame.Data.ConsumptionGuideConfig;
using ASpeedGame.Data.CreateMatchRoomCustomer;
using ASpeedGame.Data.CrownGenstoneConfig;
using ASpeedGame.Data.GameGoodsLabelDesConfig;
using ASpeedGame.Data.GameMobaSchemes;
using ASpeedGame.Data.GameSettingConfig;
using ASpeedGame.Data.HeroAIConfig;
using ASpeedGame.Data.HeroTalentConfig;
using ASpeedGame.Data.HeroTypeConfig;
using ASpeedGame.Data.HeroXPskillConfig;
using ASpeedGame.Data.KinConfig;
using ASpeedGame.Data.LegendCupConfig;
using ASpeedGame.Data.LifeHero;
using ASpeedGame.Data.LoadingWarTipConfig;
using ASpeedGame.Data.MatchRoomOpenAILevel;
using ASpeedGame.Data.MentorshipConfig;
using ASpeedGame.Data.PersonModelTransFormInfo;
using ASpeedGame.Data.PlayerSettingLocalConfig;
using ASpeedGame.Data.PrizeUnlockConfig;
using ASpeedGame.Data.RunePage;
using ASpeedGame.Data.SelectArrayTipPage;
using ASpeedGame.Data.ShieldWorld;
using ASpeedGame.Data.SlotKeyCodeConfig;
using ASpeedGame.Data.TalentPage;
using ASpeedGame.Data.WarStatisticMap;
using ASpeedGame.Data.WarSupportPlayerConfig;
using Data.ActorPrizeConfig;
using Data.PersonModel;
using Data.PlayerLocalHeroConfig;
using Data.PointShopConfig;
using Data.SceneEffectNodeData;
using Data.SideButtonConfig;
using Data.UISoundConfig;
using Data.WarBgmConfig;
using Data.WarSubFuncConfig;
using UnityEngine;
using USpeedUI;
using ASpeedGame.Data.TaskConfig;
using ASpeedGame.Data.TaskViewTimeConfig;
using ASpeedGame.Data.PreLoadSkinLayout;
using ASpeedGame.Data.PreloadMonsterRes;
using ASpeedGame.Data.PropertyPanelConfig;
using ASpeedGame.Data.ChatDefaultChannel;
using ASpeedGame.Data.MatchRankLimitConfig;
using ASpeedGame.Data.Scheme;
using ASpeedGame.Data.MatchSeasonPrizeConfig;
using ASpeedGame.Data.RecommendPrize;

/// <summary>
/// 加载只有显示层才会用到的配置文件
/// </summary>
public sealed class ViewConfigManager
{
    private static bool bInit = false;
    /// Scp 配置文件夹所在的路径
    private static string m_strMPKViewConfigPath = "Scp\\";
    
    /// User 配置文件夹所在的路径
    private static string m_strDataUserScpPath = "Data";
    private static string m_strUserScpPath = "/Data/User/";

    /// 当前账号 配置文件夹所在的路径
    private static string m_strAccountScpPath = "";
    private static bool accountChanged;

    // 编辑器工具(没运行Initialize时使用) Scp路径
    private static string m_strEidtorUseViewConfigPath;
    private static string m_strDataScpPath = "/Data/Scp/";


    /// <summary>
    /// Data/Scp文件夹路径
    /// </summary>
    public static string DataScpPath
    {
        get
        {
            return m_strMPKViewConfigPath;
        }
    }
    /// <summary>
    /// Data/User 文件夹路径
    /// </summary>
    public static string DataUserPath
    {
        get
        {
            return m_strDataUserScpPath;
        }
    }

    /// <summary>
    /// 当前账号配置 文件夹路径
    /// </summary>
    public static string AccountScpPath
    {
        get
        {
            return m_strAccountScpPath;
        }
    }

    /// <summary>
    /// 只用于编辑器工具(没运行Initialize时使用)   Data/Scp路径
    /// </summary>
    public static string Editor_DataScpPath
    {
        get
        {
            if (m_strEidtorUseViewConfigPath == null)
                ViewConfigManager.SetDefaultViewConfigPath();
            return m_strEidtorUseViewConfigPath;
        }
    }

    public static void Init()
    {
        if (bInit)
        {
            return;
        }
        SetDefaultViewConfigPath();

        LoadViewConfigs();

        RunInitConfig();

        EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
        bInit = true;
    }
    /// <summary>
    /// 释放整个游戏有可能加载过的配置文件
    /// </summary>
    public static void Destroy()
    {
        UnLoadViewConfigs();
        //UnLoadMobaConfigs();
        UnLoadActorConfigs();

        EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);

        bInit = false;
    }
    /// <summary>
    /// 卸载所有配置文件
    /// </summary>
    private static void UnLoadViewConfigs()
    {
        //GameSettingsXmlManager.Instance.UnLoad();
		ChatSettingConfig.Instance.UnLoad();
        ChatDefaultChannel.Instance.UnLoad();
		GameSettingConfig.Instance.UnLoad();
        SlotKeyCodeConfig.Instance.UnLoad();
        CameraConfig.Instance.UnLoad();
        ShieldWord.Instance.UnLoad();
        TalentPage.Instance.UnLoad();
        ULocalizationService.Instance.Unload();
        TalentPageEffect.Instance.UnLoad();
        TalentPageLevel.Instance.UnLoad();
        RunePage.Instance.UnLoad();
        HeroAIConfig.Instance.UnLoad();
        KinConfig.Instance.UnLoad();
        ClanConfig.Instance.UnLoad();
        WarStatisticMapConfig.Instance.UnLoad();
        SelectArrayTipPage.Instance.UnLoad();
        PersonModelTransFormInfo.Instance.UnLoad();
        DataCenter.LogicDataCenter.goodsSchemesDataManager.UnLoad();
        HeroTalentConfigPage.Instance.UnLoad();
        PrizeUnlockConfig.Instance.UnLoad();
        SideButtonConfig.Instance.Unload();
        Data.CreateHeroConfig.CreateHeroConfig.Instance.UnLoad();
        PlayerSettingLocalConfig.Instance.UnLoad();
        PlayerSlotSettingConfig.Instance.UnLoad();
		CrownPageConfigPage.Instance.UnLoad();
        MentorshipConfig.Instance.UnLoad();
        WarSupportPlayerConfigPage.Instance.UnLoad();
        HeroXPskillConfig.Instance.UnLoad();
        GUIDE.GuideManager.Instance.UnLoad();
        LegendCupConfig.Instance.Unload();
        Data.ChatEmoticonConfig.ChatEmoticonConfig.Instance.UnLoad();
        SceneEffectNodeData.Instance.UnLoad();
        WarBgmConfig.Instance.UnLoad();
        UISoundConfig.Instance.UnLoad();
        HeroTypeConfig.Instance.Unload();
        PersonModelConfig.Instance.UnLoad();
		WarSubFuncConfig.Instance.UnLoad();
        MatchRoomCustomer.Instance.UnLoad();
        ConsumptionGuideConfig.Instance.Unload();
        GameGoodsLabelDesConfig.Instance.UnLoad();
		MatchRoomOpenAILevel.Instance.UnLoad();
        LoadingWarTipConfig.Instance.Unload();
        PlayerLocalHeroConfig.Instance.UnLoad();
        TaskConfig.Instance.Unload();
        PreLoadSkinLayout.Instance.UnLoad();
        PropertyPanelConfig.Instance.Unload();
        PreloadMonsterRes.Instance.UnLoad();
        MatchRankLimitConfig.Instance.Unload();
        MatchSeasonPrizeConfig.Instance.Unload();
        LifeHeroConfig.Instance.UnLoad();
        RecommendPrizeConfig.Instance.UnLoad();

        // 卸载预加载光效配置
        SchemePreLoadLight.Instance.UnLoad();
        SchemeSocialSharing.Instance.UnLoad();
        SchemeLightingEffect.Instance.UnLoad();
        SchemeRandomSound.Instance.UnLoad();
        SchemeHeroSound.Instance.UnLoad();
        SchemeOBControl.Instance.UnLoad();
        SchemeWebUrl.Instance.UnLoad();
    }
    /// <summary>
    /// 加载整个游戏所需要的配置文件
    /// 单独的配置文件单独加载（如：Moba战场配置文件）
    /// </summary>
    private static void LoadViewConfigs()
    {
        PlayerSettingLocalConfig.Instance.Load(GetConfigFullPath("PlayerLocalSetting.csv",false));
        PlayerSlotSettingConfig.Instance.Load(GetConfigFullPath("PlayerSlotSetting.csv", false));
        GameSettingConfig.Instance.Load(GetConfigFullPath("GameSettingConfig.csv"));
		ChatSettingConfig.Instance.Load(GetConfigFullPath("ChatSettingConfig.csv"));
        ChatDefaultChannel.Instance.Load(GetConfigFullPath("ChatDefaultChannel.csv"));

        SlotKeyCodeConfig.Instance.Load(GetConfigFullPath("SlotMapedShortcut.csv"));
        CameraConfig.Instance.Load(GetConfigFullPath("CameraParamConfig.csv"));
		ShieldWord.Instance.Load();
        TalentPage.Instance.Load(GetConfigFullPath("TalentPage.csv"));
        ULocalizationService.Instance.Load(GetConfigFullPath(ULocalizationService.LocalizationConfigFileName));
        TalentPageEffect.Instance.Load(GetConfigFullPath("TalentPageEffect.csv"));
        TalentPageLevel.Instance.Load(GetConfigFullPath("TalentPageLevel.csv"));
        RunePage.Instance.Load(GetConfigFullPath("RuneInfo.csv"));
        HeroAIConfig.Instance.Load(GetConfigFullPath("AIHero.csv"));
        KinConfig.Instance.Load(GetConfigFullPath(""));
        ClanConfig.Instance.Load(GetConfigFullPath(""));
        WarStatisticMapConfig.Instance.Load(GetConfigFullPath("WarStatisticMap.csv"));
        MatchRoomCustomer.Instance.Load(GetConfigFullPath("MatchRoomCustomer.csv"));
        MentorshipConfig.Instance.Load(GetConfigFullPath("MentorshipConfig.csv"));
        MatchRoomOpenAILevel.Instance.Load(GetConfigFullPath("MatchRoomOpenAILevel.csv"));




        SelectArrayTipPage.Instance.Load(GetConfigFullPath("SelectArrayTip.csv"));
        PersonModelTransFormInfo.Instance.Load(GetConfigFullPath("PersonModelTransfromInfo.csv"));
        DataCenter.LogicDataCenter.goodsSchemesDataManager.Load(GetConfigFullPath(MobaSchemesConfig.MOBA_GOODS_SCHEME_FILENAME));
        HeroTalentConfigPage.Instance.Load(GetConfigFullPath("HeroTalentConfig.csv"));
        ActorPrizeConfig.Instance.Load(GetConfigFullPath(""));



        PointShopConfig.Instance.Load(GetConfigFullPath(""));
        PrizeUnlockConfig.Instance.Load(GetConfigFullPath("PrizeUnlock.csv"));
        SideButtonConfig.Instance.Load(GetConfigFullPath("SideButtonList.csv"));
        Data.CreateHeroConfig.CreateHeroConfig.Instance.Load(GetConfigFullPath("CreateHeroView.csv"));
        Data.AnimSpriteConfig.AnimSpriteConfig.Instance.Load(GetConfigFullPath("AnimSprite.csv"));





        CrownPageConfigPage.Instance.Load(GetConfigFullPath("CrownGemstoneInfo.csv"), GetConfigFullPath("CrownGemstoneEffect.csv"), GetConfigFullPath("CrownGemstoneExterior.csv"), GetConfigFullPath("CrownCobblestonExterior.csv"), GetConfigFullPath("CrownGemstoneRandom.csv"));



        WarSupportPlayerConfigPage.Instance.Load(GetConfigFullPath("WarEndSupportVote.csv"), GetConfigFullPath("WarEndSupportPosition.csv"), GetConfigFullPath("Label.csv"));



        HeroXPskillConfig.Instance.Load(GetConfigFullPath("HeroXPSkill.csv"));



        GUIDE.GuideManager.Instance.Load(GetConfigFullPath("GuideConfig.csv"));

        LegendCupConfig.Instance.Load(GetConfigFullPath(""));




        Data.ChatEmoticonConfig.ChatEmoticonConfig.Instance.Load(GetConfigFullPath("ChatEmotion.csv"));
        SceneEffectNodeData.Instance.Load(GetConfigFullPath("SceneEffectNodeData.csv"));
        WarBgmConfig.Instance.Load(GetConfigFullPath("BgmRules.csv"));
        UISoundConfig.Instance.Load(GetConfigFullPath(UISoundConfig.ConfigName));



        HeroTypeConfig.Instance.Load(GetConfigFullPath("HeroType.csv"));
        PersonModelConfig.Instance.Load(GetConfigFullPath("PersonModel.csv"));
		WarSubFuncConfig.Instance.Load(GetConfigFullPath("WarSubFunc.csv"));
        ConsumptionGuideConfig.Instance.Load(GetConfigFullPath("ConsumptionGuideConfig.csv"));


        GameGoodsLabelDesConfig.Instance.Load(GetConfigFullPath(MobaSchemesConfig.MOBA_GOODS_LABEL_DESCRIPTION));
        LoadingWarTipConfig.Instance.Load(GetConfigFullPath(""));
        PlayerLocalHeroConfig.Instance.Load(GetConfigFullPath(""));
        LifeHeroConfig.Instance.Load(GetConfigFullPath(""));
        RecommendPrizeConfig.Instance.Load(GetConfigFullPath(""));
        TaskConfig.Instance.Load(GetConfigFullPath(""));

        PreLoadSkinLayout.Instance.Load(GetConfigFullPath("PreLoadSkinLayout.csv"));
        PropertyPanelConfig.Instance.Load(GetConfigFullPath(""));
        PreloadMonsterRes.Instance.Load(GetConfigFullPath("PreloadMonsterRes.csv"));
        MatchRankLimitConfig.Instance.Load(GetConfigFullPath(""));
        MatchSeasonPrizeConfig.Instance.Load(GetConfigFullPath(""));


        // 加载预加载光效配置
        SchemePreLoadLight.Instance.Load( GetConfigFullPath(SchemePreLoadLight.SCHEME_FILENAME) );
        SchemeSocialSharing.Instance.Load(GetConfigFullPath(SchemeSocialSharing.schemeFileName));
        SchemeLightingEffect.Instance.Load(GetConfigFullPath(SchemeLightingEffect.schemeFileName));
        SchemeRandomSound.Instance.Load(GetConfigFullPath(SchemeRandomSound.schemeFileName));
        SchemeHeroSound.Instance.Load(GetConfigFullPath(SchemeHeroSound.schemeFileName));
        SchemeOBControl.Instance.Load(GetConfigFullPath(SchemeOBControl.schemeFileName));
        SchemeWebUrl.Instance.Load(GetConfigFullPath(SchemeWebUrl.schemeFileName));
    }

    /// <summary>
    /// 这里有个坑，每次进房间就会执行一次，而离开房间并不会执行UnLoadMobaConfigs方法，只有离开战场状态才会执行。
    /// 所以每次进房间都会加载一次战场相关的所有配置。
    /// </summary>
    /// <param name="nWarTypeId"></param>
	public static void LoadMobaConfigs(int nWarTypeId, int nMatchTypeId)
    {
        GameMobaSchemesManager.Instance.LoadSchemes(DataScpPath, nWarTypeId, nMatchTypeId);
    }

    public static void UnLoadMobaConfigs()
    {
        GameMobaSchemesManager.Instance.DestroySchemes();
    }

    /// <summary>
    /// 加载当前帐号所对应的配置文件
    /// </summary>
    public static void LoadActorConfigs() 
    {
        //设置根路径目录
        SetAccountLoacalConfigPath();

        //如果帐号没换，就不用重新加载了
        if (!accountChanged)
            return;

        //先卸载
        UnLoadActorConfigs();

        //精彩时刻数据重新加载一下，因为要分账号存
        DataCenter.LogicDataCenter.screenCaptureDataManager.UnLoad();
        DataCenter.LogicDataCenter.screenCaptureDataManager.Load();

        PlayerLocalHeroConfig.Instance.AccountUnload();
        PlayerLocalHeroConfig.Instance.AccountLoad();

        //以下部分为需要分帐号加载的本地配置文件
        //PlayerLocalHeroConfig.Instance.Load(GetAccountConfigFullPath(""));
        TaskViewTimeConfig.Instance.Load("");
    }

    /// <summary>
    /// 卸载当前帐号相关配置文件
    /// </summary>
    public static void UnLoadActorConfigs()
    {
        //以下部分为需要分帐号加载的本地配置文件
        //PlayerLocalHeroConfig.Instance.UnLoad();
        TaskViewTimeConfig.Instance.Unload();
    }

    public static void SetDefaultViewConfigPath()
    {
        m_strMPKViewConfigPath = "Scp\\";
        int i = Application.dataPath.LastIndexOf("/");
        int l = Application.dataPath.Length;

        string data = Application.dataPath.Substring(0, i);

        m_strDataUserScpPath = data + m_strUserScpPath;

        //没有User目录的话创建一个
        if (!System.IO.Directory.Exists(m_strDataUserScpPath))
        {
            System.IO.Directory.CreateDirectory(m_strDataUserScpPath);
        }

        m_strEidtorUseViewConfigPath = data + m_strDataScpPath;        
    }

    public static string GetConfigFullPath(string _configFileName, bool _bIsScp = true)
    {
        string path = null;

        path = _bIsScp ? DataScpPath : DataUserPath;
        path += _configFileName;

        return path;
    }

    /// <summary>
    /// 获取当前帐号某一配置文件完整路径
    /// </summary>
    /// <param name="_configFileName">配置文件名称</param>
    /// <returns></returns>
    public static string GetAccountConfigFullPath(string _configFileName)
    {
        return AccountScpPath + _configFileName;
    }

    /// <summary>
    /// 执行一些初始的配置
    /// </summary>
    private static void RunInitConfig()
    {

    }

    private static void SetAccountLoacalConfigPath()
    {
        if (null == EntityFactory.MainHeroView || null == EntityFactory.MainHeroView.Property)
            return;

        int nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

        string path = m_strDataUserScpPath;
        path += nActorID.ToString();
        path += "/";

        if (m_strAccountScpPath != path)
        {
            accountChanged = true;
            m_strAccountScpPath = path;
        }
        else
            accountChanged = false;

        //没有目录的话创建一个
        if (!System.IO.Directory.Exists(m_strAccountScpPath))
        {
            System.IO.Directory.CreateDirectory(m_strAccountScpPath);
        }
    }

    private static void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
    {
        if (null == EntityFactory.MainHeroView || null == e.objev)
            return;

        //如果属性PROPERTY_ID有更新，则载入一次帐号相关的配置
        if(e.objev == EntityFactory.MainHeroView && e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_ID)
            LoadActorConfigs();
    }
}