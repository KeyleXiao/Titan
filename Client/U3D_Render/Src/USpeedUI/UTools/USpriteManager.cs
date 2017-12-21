/*******************************************************************
** 文件名:	USpriteManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/5/6
** 版  本:	1.0
** 描  述:	动态精灵获取工具
** 应  用:  新增动态UI资源方法：
                1、将资源按文件夹放好放到路径为Artist/UI/Dynamic/下，要打图集的放Atlases，不打图集的放NonAtlases
                2、检查资源的AssetBundle信息是否自动填好，可以通过UI查看器检查
                3、打包资源（这一步已经由每次编译的时候服务器也会打包一次资源，个人不需要管）
                4、在本脚本的模板定义，资源类定义，绑定资源模板与类型区域填写新增的资源
                5、上传新的资源，Scp/UIConfig，StreamingAssets/ResourcePackage/ 本脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using USpeedUI;

public class USpriteManager : Singleton<USpriteManager>
{
    #region 基础方法
    /// <summary>
    /// 直接通过assetBundleName和AssetName获取资源节点
    /// </summary>
    /// <param name="strAssetBundleName"></param>
    /// <param name="strAssetName"></param>
    /// <returns></returns>
    public ResNode GetSpriteRes(string strAssetBundleName, string strAssetName)
    {
        return AssetBundleManager.GetAssets(AssetType.Asset_Sprite,strAssetBundleName, strAssetName);
    }

    /// <summary>
    /// 直接通过assetBundleName和AssetName获取资源节点 (不太建议这种方法）
    /// </summary>
    /// <param name="strAssetBundleName"></param>
    /// <param name="strAssetName"></param>
    /// <returns></returns>
    public Sprite GetSprite(string strAssetBundleName, string strAssetName,USpeedUI.WndID _eUserID)
    {
        ResNode resData = GetSpriteRes(strAssetBundleName, strAssetName);
        if (resData == null)
        {
            Trace.LogWarning("无法获取Sprite:" + strAssetName + " ，在包" + strAssetBundleName+",已用默认的透明图替代");
            return DefaultSprite;
        }
        SetSpriteToUseingDic(_eUserID, resData);
        return resData.ConvertRes<Sprite>();
    }

    /// <summary>
    /// 异步直接通过assetBundleName和AssetName获取资源节点
    /// </summary>
    /// <param name="strAssetBundleName"></param>
    /// <param name="strAssetName"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    /// <param name="priority"></param>
    /// <returns></returns>
    public ResNode GetSpriteRes(string strAssetBundleName, string strAssetName, 
                                                System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                System.Object userDataObj, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        return AssetBundleManager.GetAssetsAsync(AssetType.Asset_Sprite,strAssetBundleName, strAssetName, callBack, userDataObj, priority);
    }
    /// <summary>
    /// 异步通过assetBundleName和AssetName获取资源节点 (不太建议这种方法）
    /// </summary>
    /// <param name="strAssetBundleName"></param>
    /// <param name="strAssetName"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    /// <param name="priority"></param>
    /// <returns></returns>
    public Sprite GetSprite(string strAssetBundleName, string strAssetName, out ResNode resNodeData,
                                            System.Action<ResConfigData, ResNode, System.Object> callBack,
                                            System.Object userDataObj, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        ResNode resData = GetSpriteRes(strAssetBundleName, strAssetName, callBack, userDataObj, priority);
        resNodeData = resData;

        if (resData == null)
        {
            Trace.LogWarning("无法获取Sprite:" + strAssetName + " ，在包" + strAssetBundleName);
            return null;
        }

        return resData.ConvertRes<Sprite>();
    }

    /// <summary>
    /// 通过Resnode信息移除Sprite，不能用于移除异步创建的Sprite
    /// </summary>
    /// <param name="_resNode"></param>
    public void DeleteSprite(ResNode _resNode)
    {
        AssetBundleManager.DeleteAssets(ref _resNode,true);
    }
    /// <summary>
    /// 移除异步创建的Sprite
    /// </summary>
    /// <param name="_resNode">节点信息</param>
    /// <param name="_callBack">回调</param>
    public void DeleteSprite(ResNode _resNode, System.Action<ResConfigData, ResNode, System.Object> _callBack)
    {
        AssetBundleManager.DeleteAssetsAsync(ref _resNode, _callBack, true);
    }
    #endregion

    #region 需要增加类型时，这里的东西要更改

    #region 资源模板定义
    /// <summary>
    /// 
    /// </summary>
    private const string UIAssetBundleRoot = "texture/ui/dynamic/";

    //AssetName（AN）非模板的可以设空
    //AssetBundleName (ABN) 资源包路径
    private const string MobaGoods_ABN = UIAssetBundleRoot + "atlases/mobagoods/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string MobaGoods_AN = "MobaGoods_"+ Symbol + "A_"+ Symbol + "B";//"MobaGoods_@A_@B"

    private const string Skill_ABN = UIAssetBundleRoot + "atlases/skill/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string Skills_AN = "Skill_"+ Symbol + "A_"+ Symbol + "B";

    private const string StartGradeLv_ABN = UIAssetBundleRoot + "atlases/stargradelv/type_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string StartGradeLv_AN = "StarGradeLv_" + Symbol + "A_"+ Symbol + "B";

    private const string Talent_ABN = UIAssetBundleRoot + "atlases/talent/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string Talent_AN = "Talent_"+ Symbol + "A_"+ Symbol + "B";


    private const string HeadPortrait_ABN = UIAssetBundleRoot + "nonatlases/headportrait/hero_" + Symbol + "C." + AssetBundleManager.UIVariant;
    private const string HeadPortrait_AN = "HeadPortrait_"+Symbol+"A_"+Symbol+"B_"+Symbol+"C";

    private const string PlayerHead_ABN = UIAssetBundleRoot + "nonatlases/playerhead/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string PlayerHead_AN = "PlayerHead_" + Symbol + "A_" + Symbol + "B";

    private const string Rune_ABN = UIAssetBundleRoot + "atlases/rune." + AssetBundleManager.UIVariant;
    private const string Rune_AN = "Rune_" + Symbol + "A_" + Symbol + "B";


    private const string HeroBackGround_ABN = UIAssetBundleRoot + "atlases/herobackground/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string HeroBackGround_AN = "HeroBackGround_" + Symbol + "A_" + Symbol + "B";

    private const string RankImage_ABN = UIAssetBundleRoot + "nonatlases/rankimap/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string RankImage_AN = "Tank_" + Symbol + "A_" + Symbol + "B";

    private const string RankStarImage_ABN = UIAssetBundleRoot + "nonatlases/rankstar." + AssetBundleManager.UIVariant;
    private const string RankStarImage_AN = "rankstar_" + Symbol + "A";

    private const string KinIdentityImage_ABN = UIAssetBundleRoot + "nonatlases/kinidentity." + AssetBundleManager.UIVariant;
    private const string KinIdentityImage_AN = "identity_" + Symbol + "A";

    private const string KinBattleOrderImage_ABN = UIAssetBundleRoot + "nonatlases/kinbattlevalueorder." + AssetBundleManager.UIVariant;
    private const string KinBattleOrderImage_AN = "order_" + Symbol + "A";

    private const string KinSexImage_ABN = UIAssetBundleRoot + "nonatlases/kinsex." + AssetBundleManager.UIVariant;
    private const string KinSexImage_AN = "sex_" + Symbol + "A";

    private const string TeamRightImage_ABN = UIAssetBundleRoot + "nonatlases/dynamicstate_moba/teamrightview." + AssetBundleManager.UIVariant;
    private const string TeamRightImage_AN = "TeamView_" + Symbol + "A";

    private const string ScoreLvImage_ABN = UIAssetBundleRoot + "atlases/scorelv/size_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string ScoreLvImage_AN = "ScoreLv_" + Symbol + "A_" + Symbol + "B";

    private const string DynamicState_Moba_ABN = UIAssetBundleRoot + "atlases/pic/dynamicstate_moba." + AssetBundleManager.UIVariant;
    private const string DynamicState_Moba_AN = UPath.UDynamicMobaState + "_"+Symbol + "A";

    private const string StaticState_ABN = UIAssetBundleRoot + "atlases/pic/staticstate." + AssetBundleManager.UIVariant;
    private const string StaticState_AN = UPath.UStaticState + "_" + Symbol + "A";

    private const string SideButtonIcon_ABN = UIAssetBundleRoot + "nonatlases/sidebuttonicon." + AssetBundleManager.UIVariant;
    private const string SideButtonIcon_AN = "sidebuttonicon_" + Symbol + "A";

    private const string CardFrameUI_ABN = UIAssetBundleRoot + "nonatlases/cardframeui." + AssetBundleManager.UIVariant;
    private const string CardFrameUI_AN = "Card_Frame_" + Symbol + "A";

    private const string MinimapIcon_ABN = UIAssetBundleRoot + "atlases/minimapicon." + AssetBundleManager.UIVariant;
    private const string MinimapIcon_AN = "";

    private const string MinimapMap_ABN = UIAssetBundleRoot + "nonatlases/minimapmap." + AssetBundleManager.UIVariant;
    private const string MinimapMap_AN = "";

    private const string WarResultShow_ABN = UIAssetBundleRoot + "atlases/warresultshow." + AssetBundleManager.UIVariant;
    private const string WarResultShow_AN = "";

    private const string UnlockPrize_ABN = UIAssetBundleRoot + "atlases/unlockprize." + AssetBundleManager.UIVariant;
    private const string UnlockPrize_AN = "Unlock_" + Symbol + "A_" + Symbol + "B";

    private const string PointShop_ABN = UIAssetBundleRoot + "nonatlases/pointshop." + AssetBundleManager.UIVariant;
    private const string PointShop_AN = "";

    private const string PrizeChest_ABN = UIAssetBundleRoot + "atlases/prizechest." + AssetBundleManager.UIVariant;
    private const string PrizeChest_AN = "";

    private const string ActorPrize_ABN = UIAssetBundleRoot + "atlases/actorprize/type_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string ActorPrize_AN = "Icon_" + Symbol + "B";

    private const string WarMainMode_ABN = UIAssetBundleRoot + "atlases/warmainmode." + AssetBundleManager.UIVariant;
    private const string WarMainMode_AN = "";

    private const string ClanRankChangeImage_ABN = UIAssetBundleRoot + "nonatlases/clanrankchange." + AssetBundleManager.UIVariant;
    private const string ClanRankChangeImage_AN = "clanrankchange_" + Symbol + "A";

    private const string ClanQiZhiImage_ABN = UIAssetBundleRoot + "atlases/clanqizhi." + AssetBundleManager.UIVariant;
    private const string ClanQiZhiImage_AN = "qizhi_" + Symbol + "A";

    private const string HeroStarIcon_ABN = UIAssetBundleRoot + "atlases/herostaricon." + AssetBundleManager.UIVariant;
    private const string HeroStarIcon_AN = "";

    private const string AnimSprite_ABN = UIAssetBundleRoot + "nonatlases/animsprite." + AssetBundleManager.UIVariant;
    private const string AnimSprite_AN = "";

    private const string KinGloryCup_ABN = UIAssetBundleRoot + "nonatlases/kinglorycup." + AssetBundleManager.UIVariant;
    private const string KinGloryCup_AN = "cup_" + Symbol + "A";

    private const string MentorshipMaster_ABN = UIAssetBundleRoot + "nonatlases/mentorship." + AssetBundleManager.UIVariant;
    private const string MentorshipMaster_AN = "";

    private const string GuideSignSprite_ABN = UIAssetBundleRoot + "nonatlases/guidesprite." + AssetBundleManager.UIVariant;
    private const string GuideSignSprite_AN = "GuideSprite_" + Symbol + "A";

    private const string Gemstone_ABN = UIAssetBundleRoot + "atlases/gemstone/gemstone_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string GemstoneSprite_AN = "Gemstone_" + Symbol + "A_" + Symbol + "B";

    private const string MailSource_ABN = UIAssetBundleRoot + "nonatlases/mailsource." + AssetBundleManager.UIVariant;
    private const string MailSource_AN = "MailSourceIcon_" + Symbol + "A";

    private const string SNS_ABN = UIAssetBundleRoot + "nonatlases/sns." + AssetBundleManager.UIVariant;
    private const string SNSSprite_AN = "";

    private const string FullScreen_ABN = UIAssetBundleRoot + "nonatlases/fullscreen." + AssetBundleManager.UIVariant;
    private const string FullScreen_AN = "FSIR_"+Symbol+"A";

    private const string MonsterHeadPortrait_ABN = UIAssetBundleRoot + "nonatlases/monsterheadportrait/monster_" + Symbol + "A." + AssetBundleManager.UIVariant;
    private const string MonsterHeadPortrait_AN = "MonsterHeadPortrait_" + Symbol + "A_" + Symbol + "B";

    private const string Task_ABN = UIAssetBundleRoot + "nonatlases/task." + AssetBundleManager.UIVariant;
    private const string Task_AN = "";

    private const string GameState_ABN = UIAssetBundleRoot + "nonatlases/gamestate." + AssetBundleManager.UIVariant;
    private const string GameState_AN = "GameState_" + Symbol + "A";

    private const string QRCode_ABN = UIAssetBundleRoot + "nonatlases/qrcode." + AssetBundleManager.UIVariant;
    private const string QRCode_AN = "QR_" + Symbol + "A";

    private const string GamePromotion_ABN = UIAssetBundleRoot + "nonatlases/rwui/gamepromotion." + AssetBundleManager.UIVariant;
    private const string GamePromotion_AN = "GamePromotion_" + Symbol + "A";

    #endregion

    #region 资源类型定义
    public enum ESpriteType
    {
        EST_MobaGoods,
        EST_Skill,
        EST_StartGradeLv,
        EST_Talent,
        EST_HeadPortrait,
        EST_PlayerHead,
        EST_Rune,
        EST_HeroBackground,
        EST_RankImage,
        EST_RankStarImage,
        EST_KinIdentity,
        EST_KinSex,
        EST_KinBattleOrder,
        EST_TeamRightView,
        EST_ScoreLvImage,
        EST_DynamicState_Moba,
        EST_StaticState,
        EST_SideButtonIcon,
        EST_CardFrameUI,
        EST_MinimapIcon,
        EST_MinimapMap,
        EST_WarResultShow,
        EST_UnlockPrize,
        EST_PointShop,
        EST_PrizeChest,
        EST_ActorPrize,
        EST_WarMainMode,
        EST_ClanRankChange,
        EST_ClanQiZhi,
        EST_HeroStarIcon,
        EST_AnimSprite,
        EST_KinGloryCup,
        EST_MentorshipMaster,
        EST_GuideSignSprite,
        EST_Gemstone,
        EST_MailSourceIcon,
        EST_SNS,
        EST_FullScreen,
        EST_MonsterHeadPortrait,
        EST_Task,
        EST_GameState,
        EST_QRCode,
        EST_GamePromotion,
    }
    #endregion

    #region 绑定资源模板与类型
    protected void init()
    {
        bInit = true;
        dicSpriteType.Add(ESpriteType.EST_MobaGoods, new USpriteType(MobaGoods_ABN, MobaGoods_AN, ESpriteType.EST_MobaGoods));
        dicSpriteType.Add(ESpriteType.EST_Skill, new USpriteType(Skill_ABN, Skills_AN, ESpriteType.EST_Skill));
        dicSpriteType.Add(ESpriteType.EST_StartGradeLv, new USpriteType(StartGradeLv_ABN, StartGradeLv_AN, ESpriteType.EST_StartGradeLv));
        dicSpriteType.Add(ESpriteType.EST_Talent, new USpriteType(Talent_ABN, Talent_AN, ESpriteType.EST_Talent));
        dicSpriteType.Add(ESpriteType.EST_HeadPortrait, new USpriteType(HeadPortrait_ABN, HeadPortrait_AN, ESpriteType.EST_HeadPortrait));
        dicSpriteType.Add(ESpriteType.EST_PlayerHead, new USpriteType(PlayerHead_ABN, PlayerHead_AN, ESpriteType.EST_PlayerHead));
        dicSpriteType.Add(ESpriteType.EST_Rune, new USpriteType(Rune_ABN, Rune_AN, ESpriteType.EST_Rune));
        dicSpriteType.Add(ESpriteType.EST_HeroBackground, new USpriteType(HeroBackGround_ABN, HeroBackGround_AN, ESpriteType.EST_HeroBackground));
        dicSpriteType.Add(ESpriteType.EST_RankImage, new USpriteType(RankImage_ABN, RankImage_AN, ESpriteType.EST_RankImage));
        dicSpriteType.Add(ESpriteType.EST_RankStarImage, new USpriteType(RankStarImage_ABN, RankStarImage_AN, ESpriteType.EST_RankStarImage));
        dicSpriteType.Add(ESpriteType.EST_KinIdentity, new USpriteType(KinIdentityImage_ABN, KinIdentityImage_AN, ESpriteType.EST_KinIdentity));
        dicSpriteType.Add(ESpriteType.EST_KinBattleOrder, new USpriteType(KinBattleOrderImage_ABN, KinBattleOrderImage_AN, ESpriteType.EST_KinBattleOrder));
        dicSpriteType.Add(ESpriteType.EST_KinSex, new USpriteType(KinSexImage_ABN, KinSexImage_AN, ESpriteType.EST_KinSex));
        dicSpriteType.Add(ESpriteType.EST_TeamRightView, new USpriteType(TeamRightImage_ABN, TeamRightImage_AN, ESpriteType.EST_TeamRightView));
        dicSpriteType.Add(ESpriteType.EST_ScoreLvImage, new USpriteType(ScoreLvImage_ABN, ScoreLvImage_AN, ESpriteType.EST_ScoreLvImage));
        dicSpriteType.Add(ESpriteType.EST_DynamicState_Moba, new USpriteType(DynamicState_Moba_ABN, DynamicState_Moba_AN, ESpriteType.EST_DynamicState_Moba));
        dicSpriteType.Add(ESpriteType.EST_StaticState, new USpriteType(StaticState_ABN, StaticState_AN, ESpriteType.EST_StaticState));
        dicSpriteType.Add(ESpriteType.EST_SideButtonIcon, new USpriteType(SideButtonIcon_ABN, SideButtonIcon_AN, ESpriteType.EST_SideButtonIcon));
        dicSpriteType.Add(ESpriteType.EST_CardFrameUI, new USpriteType(CardFrameUI_ABN, CardFrameUI_AN, ESpriteType.EST_CardFrameUI));
        dicSpriteType.Add(ESpriteType.EST_MinimapIcon, new USpriteType(MinimapIcon_ABN, MinimapIcon_AN, ESpriteType.EST_MinimapIcon));
        dicSpriteType.Add(ESpriteType.EST_MinimapMap, new USpriteType(MinimapMap_ABN, MinimapMap_AN, ESpriteType.EST_MinimapMap));
        dicSpriteType.Add(ESpriteType.EST_WarResultShow, new USpriteType(WarResultShow_ABN, WarResultShow_AN, ESpriteType.EST_WarResultShow));
        dicSpriteType.Add(ESpriteType.EST_UnlockPrize, new USpriteType(UnlockPrize_ABN, UnlockPrize_AN, ESpriteType.EST_UnlockPrize));
        dicSpriteType.Add(ESpriteType.EST_PointShop, new USpriteType(PointShop_ABN, PointShop_AN, ESpriteType.EST_PointShop));
        dicSpriteType.Add(ESpriteType.EST_PrizeChest, new USpriteType(PrizeChest_ABN, PrizeChest_AN, ESpriteType.EST_PrizeChest));
        dicSpriteType.Add(ESpriteType.EST_ActorPrize, new USpriteType(ActorPrize_ABN, ActorPrize_AN, ESpriteType.EST_ActorPrize));
        dicSpriteType.Add(ESpriteType.EST_WarMainMode, new USpriteType(WarMainMode_ABN, WarMainMode_AN, ESpriteType.EST_WarMainMode));
        dicSpriteType.Add(ESpriteType.EST_ClanRankChange, new USpriteType(ClanRankChangeImage_ABN, ClanRankChangeImage_AN, ESpriteType.EST_ClanRankChange));
        dicSpriteType.Add(ESpriteType.EST_ClanQiZhi, new USpriteType(ClanQiZhiImage_ABN, ClanQiZhiImage_AN, ESpriteType.EST_ClanQiZhi));
        dicSpriteType.Add(ESpriteType.EST_HeroStarIcon, new USpriteType(HeroStarIcon_ABN, HeroStarIcon_AN, ESpriteType.EST_HeroStarIcon));
        dicSpriteType.Add(ESpriteType.EST_AnimSprite, new USpriteType(AnimSprite_ABN, AnimSprite_AN, ESpriteType.EST_AnimSprite));
        dicSpriteType.Add(ESpriteType.EST_KinGloryCup, new USpriteType(KinGloryCup_ABN, KinGloryCup_AN, ESpriteType.EST_KinGloryCup));
        dicSpriteType.Add(ESpriteType.EST_MentorshipMaster, new USpriteType(MentorshipMaster_ABN, MentorshipMaster_AN, ESpriteType.EST_MentorshipMaster));
        dicSpriteType.Add(ESpriteType.EST_GuideSignSprite, new USpriteType(GuideSignSprite_ABN, GuideSignSprite_AN,ESpriteType.EST_GuideSignSprite));
        dicSpriteType.Add(ESpriteType.EST_Gemstone, new USpriteType(Gemstone_ABN, GemstoneSprite_AN, ESpriteType.EST_Gemstone));
        dicSpriteType.Add(ESpriteType.EST_MailSourceIcon, new USpriteType(MailSource_ABN, MailSource_AN, ESpriteType.EST_MailSourceIcon));
        dicSpriteType.Add(ESpriteType.EST_SNS, new USpriteType(SNS_ABN, SNSSprite_AN, ESpriteType.EST_SNS));
        dicSpriteType.Add(ESpriteType.EST_FullScreen, new USpriteType(FullScreen_ABN, FullScreen_AN, ESpriteType.EST_FullScreen));
        dicSpriteType.Add(ESpriteType.EST_MonsterHeadPortrait, new USpriteType(MonsterHeadPortrait_ABN, MonsterHeadPortrait_AN, ESpriteType.EST_MonsterHeadPortrait));
        dicSpriteType.Add(ESpriteType.EST_Task, new USpriteType(Task_ABN, Task_AN, ESpriteType.EST_Task));
        dicSpriteType.Add(ESpriteType.EST_GameState, new USpriteType(GameState_ABN, GameState_AN, ESpriteType.EST_GameState));
        dicSpriteType.Add(ESpriteType.EST_QRCode, new USpriteType(QRCode_ABN, QRCode_AN, ESpriteType.EST_QRCode));
        dicSpriteType.Add(ESpriteType.EST_GamePromotion, new USpriteType(GamePromotion_ABN, GamePromotion_AN, ESpriteType.EST_GamePromotion));
    }
    #endregion
    #endregion

    #region 数据与结构
    //是否执行了Init()
    protected bool bInit = false;
    /// <summary>
    /// ID模板 ，5个还不够用？
    /// </summary>
    protected const string Symbol = "@";
    protected static string[] strPatterns = { Symbol+"A", Symbol+"B", Symbol+"C", Symbol+"D", Symbol+"E" };
    private Dictionary<ESpriteType, USpriteType> dicSpriteType = new Dictionary<ESpriteType, USpriteType>();
    private Dictionary<USpeedUI.WndID, List<ResNode>> dicUseingSprite = new Dictionary<USpeedUI.WndID, List<ResNode>>();
    private Sprite m_DefaultSprite = null;
    public Sprite DefaultSprite
    {
        get
        {
            if (m_DefaultSprite == null && AssetBundleManager.isVaild)
            {

                var DefaultSpriteGO = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UWidgetsPath + "Utility/DefaultSprite");
                if(DefaultSpriteGO == null)
                {
                    Trace.LogError("UI/Widgets/Utility/DefaultSprite 路径下没有找到DefaultSprite的预制体。");
                    return null;
                }
                Texture2D tr = new Texture2D(1,1,TextureFormat.ARGB32,false);
                tr.SetPixel(0,0,new Color(0,0,0,0));
                tr.Apply();
                tr.Compress(false);
                
                m_DefaultSprite = Sprite.Create(tr,new Rect(0,0,1,1),Vector2.zero);
                m_DefaultSprite.name = "defalutSprite";
            }
                
            return m_DefaultSprite;
        }
    }
    private void SetSpriteToUseingDic(USpeedUI.WndID _eWndID,ResNode _resNode)
    {
        if(dicUseingSprite.ContainsKey(_eWndID))
        {
            dicUseingSprite[_eWndID].Add(_resNode);
        }
        else
        {
            List<ResNode> resNode = new List<ResNode>();
            resNode.Add(_resNode);
            dicUseingSprite.Add(_eWndID, resNode);
        }
    }
    protected class USpriteType
    {
        public string TempletAssetBundleName { get; private set; }

        public string TempletAssetName { get; private set; }

        public ESpriteType Type { get; private set; }
        
        public USpriteType(string _strAssetBundleName,string _strAssetName,ESpriteType _eType)
        {
            TempletAssetBundleName = _strAssetBundleName;
            TempletAssetName = _strAssetName;
            Type = _eType;

        }


        public string AssetBundleName()
        {
            if (TempletAssetBundleName.Contains(Symbol))
                Trace.LogWarning("你这个AssetBundleName有模板" + Symbol + ",请用模板的方法重新取。");

            return TempletAssetBundleName;
        }
        public string AssetBundleName(params int[] _nIDArray)
        {
            string strAssetBundleName = TempletAssetBundleName;
            for (int i = 0; i < _nIDArray.Length; i++)
            {
                strAssetBundleName = strAssetBundleName.Replace(strPatterns[i], _nIDArray[i].ToString());
            }

            return strAssetBundleName;
        }

        public string AssetName(params int[] _nIDArray)
        {
            string strAssetName = TempletAssetName;
            for (int i = 0; i < _nIDArray.Length; i++)
            {
                strAssetName = strAssetName.Replace(strPatterns[i], _nIDArray[i].ToString());
            }
            return strAssetName;
        }
    }
    #endregion

    #region 常用的方法
    public static new USpriteManager Instance
    {
        get
        {
            if (!instance.bInit)
                instance.init();
            return instance;
        }
    }

    /// <summary>
    /// 移除界面下非异步的Sprite
    /// <para>这方法一般在界面父类执行了</para>
    /// </summary>
    /// <param name="_eUserID">界面ID</param>
    public void DeleteSprite(USpeedUI.WndID _eUserID)
    {
        if(dicUseingSprite.ContainsKey(_eUserID))
        {
            dicUseingSprite[_eUserID].ForEach(DeleteSprite);
            dicUseingSprite[_eUserID].Clear();
            dicUseingSprite.Remove(_eUserID);
        }
    }

    /// <summary>
    /// 获取模板ID Sprite
    /// </summary>
    /// <param name="_eType">图片类型</param>
    /// <param name="_nIDArray">模板ID顺序（A,B,C,...）</param>
    /// <returns></returns>
    public Sprite GetSprite(ESpriteType _eType, USpeedUI.WndID _eUserID,params int[] _nIDArray)
    {
        if (_nIDArray[_nIDArray.Length - 1] <= 0)
            return DefaultSprite;
        return  GetSprite(dicSpriteType[_eType].AssetBundleName(_nIDArray), dicSpriteType[_eType].AssetName(_nIDArray), _eUserID);
    }
                                                    
    /// <summary>
    /// 获取非模板ID Sprite
    /// </summary>
    /// <param name="_eType">图片类型</param>
    /// <returns></returns>
    public Sprite GetSprite(ESpriteType _eType,string _strAssetName, USpeedUI.WndID _eUserID)
    {
        return GetSprite(dicSpriteType[_eType].AssetBundleName(), _strAssetName,_eUserID);
    }

    /// <summary>
    /// 异步获取带模板ID Sprite
    /// <para>请自行调用USpriteManager.DeleteSprite()移除异步Sprite方法</para>
    /// </summary>
    /// <param name="_eType">图片类型</param>
    /// <param name="callBack">回调</param>
    /// <param name="userDataObj">用户数据，这个会出现在资源回调里</param>
    /// <param name="priority">加载优先级</param>
    /// <param name="_nIDArray">模板ID顺序（A,B,C,...）</param>
    /// <returns></returns>
    public Sprite GetSprite(ESpriteType _eType, out ResNode _resNodeData,System.Action<ResConfigData, ResNode, System.Object> callBack = null,
                                            System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal, params int[] _nIDArray)
    {
        return GetSprite(dicSpriteType[_eType].AssetBundleName(_nIDArray),
                                         dicSpriteType[_eType].AssetName(_nIDArray),out _resNodeData, callBack, userDataObj, priority);
    }

    /// <summary>
    /// 异步获取非模板ID Sprite
    /// <para>请自行调用USpriteManager.DeleteSprite()移除异步Sprite方法</para>
    /// </summary>
    /// <param name="_eType">图片类型</param>
    /// <param name="callBack">回调</param>
    /// <param name="userDataObj">用户数据，这个会出现在资源回调里</param>
    /// <param name="priority">加载优先级</param>
    /// <returns></returns>
    public Sprite GetSprite(ESpriteType _eType, string _strAssetName, out ResNode _resNodeData, System.Action<ResConfigData, ResNode, System.Object> callBack = null,
                                            System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        return GetSprite(dicSpriteType[_eType].AssetBundleName(), _strAssetName, out _resNodeData,
                                         callBack, userDataObj, priority);
    }

    public Sprite GetAnimSprite(string _strAnimName)
    {
        Data.AnimSpriteConfig.SSchemeAnimFrameData configData = Data.AnimSpriteConfig.AnimSpriteConfig.Instance.GetData(_strAnimName);
        if (configData == null)
            return DefaultSprite;
        //WndID写个整个游戏进程都会有的UI
        return GetSprite(ESpriteType.EST_AnimSprite, configData.strAssetName, USpeedUI.WndID.WND_ID_TOOLTIPS);
    }


    #endregion


    
}
