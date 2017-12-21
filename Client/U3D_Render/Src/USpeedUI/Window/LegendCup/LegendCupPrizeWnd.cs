using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UnityEngine;
using DataCenter;
using GameLogic;
using ASpeedGame.Data.LegendCupConfig;
using UIWidgets;
using Data.ActorPrizeConfig;
using War;
using ASpeedGame.Data.RunePage;
using U3D_Render.Common;
using USpeedUI.UEffect;
using ASpeedGame.Data.HeroTalentConfig;

namespace USpeedUI.LegendCup
{
    public class LegendCupPrizeWnd : UIPopupWnd<LegendCupPrizeWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_PRIZE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupPrizeView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupIDData cupIDData = (LegendCupIDData)data;
                            m_wndView.OnShowLegndCupPrizeWnd(cupIDData.nLegendCupID);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // 奖励类型（团队、个人、天赋奖励）
    public enum ECupPrizeType
    {
        ECPT_None,
        ECPT_Gold,
        ECPT_Exp,
        ECPT_Personal,

        ECPT_Max,
    }

    // 奖励图标类型（英雄、玩家、金币）
    public enum EPrizeIconType
    {
        EPIT_None,
        EPIT_HeroExp,
        EPIT_ActorExp,
        EPIT_Gold,

        EPIT_Max,
    }

    public class LegendCupPrizeItem : UCustomUIBehaviour
    {
        public Image BaseIcon;
        public Image PrizeIcon;
        public Text PrizeDes;
        public GameObject PrizeEffect;
        public UTooltipTrigger toolTip;

        private UEffectParamBase param;

        public void SetContainer(RectTransform rect)
        {
            if (rect == null)
                return;
            transform.SetParent(rect, false);
        }

        public void SetData(int nParam, ECupPrizeType eType = ECupPrizeType.ECPT_Personal, bool bShowEffect = false, bool bShowPrizeDes = true)
        {
            string sGold = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Gold");
            string sExp = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Exp");

            BaseIcon.gameObject.SetActive(false);

            if (eType == ECupPrizeType.ECPT_Gold)
            {
                PrizeIcon.sprite = GetPrizeIconSprite((int)EPrizeIconType.EPIT_Gold);
                PrizeDes.text = nParam + sGold;
                toolTip.SetText(UTooltipParamName.BodyText, nParam + sGold);
            }
            else if (eType == ECupPrizeType.ECPT_Exp)
            {
                PrizeIcon.sprite = GetPrizeIconSprite((int)EPrizeIconType.EPIT_ActorExp);
                PrizeDes.text = nParam + sExp;
                toolTip.SetText(UTooltipParamName.BodyText, nParam + sExp);
            }
            else if (eType == ECupPrizeType.ECPT_Personal)
            {
                // 与商城奖励图标一致
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(nParam);
                if (prizeConfig == null)
                    return;

                if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
                {
                    PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 3, prizeConfig.nPrizeParam[0]);
                }
                else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
                {
                    PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 7, prizeConfig.nPrizeParam[0]);
                    BaseIcon.gameObject.SetActive(true);
                }
                else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_RUNE)
                {
                    SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[prizeConfig.nPrizeParam[0]] as SSchemeRuneConfig;
                    if (runeConfig != null)
                        PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, WndID.WND_ID_POINTSHOP, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                }
                else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROXPSKILL)
                {
                    PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, prizeConfig.nPrizeType, prizeConfig.nPrizeIcon);
                }
                else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
                {
                    SkinInfoScheme skinConfig = new SkinInfoScheme();
                    if (GameLogicAPI.GetHeroSkinInfo(prizeConfig.nPrizeParam[1], ref skinConfig))
                    {
                        PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, skinConfig.nSkinIconID, 3, prizeConfig.nPrizeParam[0]);
                    }
                }
                else
                {
                    PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, prizeConfig.nPrizeType, prizeConfig.nPrizeIcon);
                }

                PrizeDes.text = prizeConfig.strPrizeName;
                toolTip.SetText(UTooltipParamName.BodyText, prizeConfig.strPrizeDesc);
            }

            PrizeDes.gameObject.SetActive(bShowPrizeDes);

            if (bShowEffect)
            {
                if (param != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
                }
                param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_HeroDetailInfo_UpstarPrizeItem, _tfAttachParent: PrizeEffect.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            }

            this.gameObject.SetActive(true);
        }

        // 设置天赋数据
        public void SetData(SSchemeHeroTalentConfig talentConfig, bool bShowEffect = false, bool bShowPrizeDes = true)
        {
            BaseIcon.gameObject.SetActive(false);

            PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_HERODETAIL, 1, talentConfig.nTalentIconId);

            string sTalentName = talentConfig.sTalentPrizeName;
            U3D_Render.Common.UBB.toHtml(ref sTalentName, UBB_FORMAT_TYPE.UGUI);
            PrizeDes.text = sTalentName;

            toolTip.SetText(UTooltipParamName.BodyText, talentConfig.sTalenDisableDescInStatic);

            PrizeDes.gameObject.SetActive(bShowPrizeDes);

            if (bShowEffect)
            {
                if (param != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
                }
                param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_HeroDetailInfo_UpstarPrizeItem, _tfAttachParent: PrizeEffect.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            }

            this.gameObject.SetActive(true);
        }
        public void Clear()
        {
            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
            this.gameObject.SetActive(false);
        }

        // 直接用已有的EST_ActorPrize图标， Type为10， ID=1英雄， ID=2玩家， ID=3金币
        Sprite GetPrizeIconSprite(int nID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_LEGENDCUP_PRIZE, 10, nID);
            if (sprite == null)
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_LEGENDCUP_PRIZE, 10, 1);
            return sprite;
        }
    }

    public class LegendCupDetailPrizeItem : UCustomUIBehaviour
    {
        public Image RankIcon;
        public Text RankIconDes;
        public Text RankDes;

        public RectTransform TeamPrizeFrame;
        public GameObject TeamPrizeTeamplate;
        private LegendCupPrizeItem m_teamPrizeItem;

        public RectTransform PersonalPrizeFrame;
        public GameObject PersonalPrizeTemplate;
        private LegendCupPrizeItem[] m_personalPrizeItems;

        public void SetData(SSchemeLegendCupPrizeConfig prizeConfig, int nTotalBonus)
        {
            SetRankData(prizeConfig.nStartPos, prizeConfig.nEndPos);
            SetTeamPrizeData(prizeConfig.nBonusRatio, nTotalBonus);
            SetPrizeData(prizeConfig.nPrizeIDList, prizeConfig.nPrizeIDList.Length);
            this.gameObject.SetActive(true);
        }

        private void SetRankData(int nStartPos, int nEndPos)
        {
            if (nStartPos == 1 || nStartPos == 2 || nStartPos == 3)
            {
                RankIcon.gameObject.SetActive(true);
                RankIconDes.gameObject.SetActive(true);
                RankDes.gameObject.SetActive(false);

                RankIcon.sprite = GetStaticStateSprite(nStartPos);
                RankIconDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Pos" + nStartPos);
            }
            else
            {
                RankIcon.gameObject.SetActive(false);
                RankIconDes.gameObject.SetActive(false);
                RankDes.gameObject.SetActive(true);

                if (nStartPos == nEndPos)
                {
                    RankDes.text = nStartPos + ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Pos");
                }
                else
                {
                    RankDes.text = nStartPos + "-" + nEndPos + ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Pos");
                }
            }
        }

        public void SetTeamPrizeData(int nBonusRatio, int nBonus)
        {
            GameObject TeamPrizeObj = ResNode.InstantiateRes(TeamPrizeTeamplate);
            if (!TeamPrizeObj)
                return;

            RectTransform rectTrans = TeamPrizeObj.transform as RectTransform;
            rectTrans.SetParent(TeamPrizeFrame, false);

            m_teamPrizeItem = LogicDataCenter.legendCupDataManager.CreateLegendCupPrizeItem(rectTrans);
            if (!m_teamPrizeItem)
                return;

            m_teamPrizeItem.SetData(nBonus * nBonusRatio / 100, ECupPrizeType.ECPT_Gold);

            TeamPrizeTeamplate.SetActive(false);
        }

        private void SetPrizeData(int[] nPrizeID, int nPrizeIDLen)
        {
            m_personalPrizeItems = new LegendCupPrizeItem[nPrizeIDLen];

            for (int i = 0; i < nPrizeIDLen; i++)
            {
                GameObject PersonalPrizeObj = ResNode.InstantiateRes(PersonalPrizeTemplate);
                if (!PersonalPrizeObj)
                    return;

                RectTransform rectTrans = PersonalPrizeObj.transform as RectTransform;
                rectTrans.SetParent(PersonalPrizeFrame, false);

                m_personalPrizeItems[i] = LogicDataCenter.legendCupDataManager.CreateLegendCupPrizeItem(rectTrans);
                if (!m_personalPrizeItems[i])
                    return;

                m_personalPrizeItems[i].SetData(nPrizeID[i]);
            }
            PersonalPrizeTemplate.SetActive(false);
        }

        public void ClearData()
        {
            RankIcon.gameObject.SetActive(false);
            RankIconDes.gameObject.SetActive(false);
            RankDes.gameObject.SetActive(false);

            TeamPrizeTeamplate.gameObject.SetActive(false);

            PersonalPrizeTemplate.gameObject.SetActive(false);
        }

        Sprite GetStaticStateSprite(int nID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_StaticState, WndID.WND_ID_LEGENDCUP_PRIZE, nID);
            if (sprite == null)
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_StaticState, WndID.WND_ID_LEGENDCUP_PRIZE, 1);
            return sprite;
        }
    }

    public class LegendCupPrizeWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text TotalBonusDes;
        public Text TotalBonusCount;
        public Text RankDes;
        public Text TeamPrizeDes;
        public Text PersonalPrizeDes;
        public Text PrizeTip;

        public RectTransform DetailPrizeFrame;
        public GameObject DetailPrizeTemplate;
        private LegendCupDetailPrizeItem[] m_DetailPrizeItems;
        private const int SinglePageMaxCount = 6;
        

        public override bool Init(IUIWnd wnd)
        {
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Title");
            TotalBonusDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "TotalBonus");
            RankDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "Rank");
            TeamPrizeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "TeamPrize");
            PersonalPrizeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "PersonalPrize");
            
            string strTip = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "PrizeTip");
            UBB.toHtml(ref strTip);
            PrizeTip.text = strTip;

            return base.Init(wnd);
        }

        public void OnShowLegndCupPrizeWnd(long lLegendCupID)
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(lLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            TotalBonusCount.text = cupBaseData.nCompetitionBonus.ToString();

            // 清空DetailPrizeFrame
            int nCount = DetailPrizeFrame.childCount;
            for (int j = nCount - 1; j >= 0; j--)
            {
                Transform childTrans = DetailPrizeFrame.GetChild(j);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(cupBaseData.nCupConfigID);
            if (cupConfig == null)
                return;
            List<SSchemeLegendCupPrizeConfig> prizeConfigList = LegendCupConfig.Instance.GetCupPrizeConfig(cupConfig.nPrizeConfigID);
            if (prizeConfigList == null)
                return;

            m_DetailPrizeItems = new LegendCupDetailPrizeItem[prizeConfigList.Count];
            for (int i = 0; i < prizeConfigList.Count; i++)
            {
                GameObject detailPrizeObj = ResNode.InstantiateRes(DetailPrizeTemplate);
                if (!detailPrizeObj)
                    return;

                RectTransform rectTrans = detailPrizeObj.transform as RectTransform;
                rectTrans.SetParent(DetailPrizeFrame, false);
                
                m_DetailPrizeItems[i] = detailPrizeObj.GetComponent<LegendCupDetailPrizeItem>();
                if (!m_DetailPrizeItems[i])
                    return;

                m_DetailPrizeItems[i].SetData(prizeConfigList[i], cupBaseData.nCompetitionBonus);
            }
            DetailPrizeTemplate.SetActive(false);
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
