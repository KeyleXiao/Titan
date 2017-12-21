/*******************************************************************
** 文件名:	UTileViewComponentHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using System;
using UIWidgets;
using System.Runtime.InteropServices;
using GameLogic;
using UnityEngine.EventSystems;
using DG.Tweening;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using U3D_Render.Common;
using War;
using DataCenter;


namespace USpeedUI.UWidgets
{
    public class UListWarMainModeComponent : ListViewItem
    {
        private UListWarMainModeItem ItemData;
        private UEffectParamBase m_OldEffectParam;
        private UEffectParamBase m_OldEffectParamEX;
        private UEffectParamBase m_OpenEffectParam;
        private UEffectParamBase m_RankEffectParam;

        [SerializeField]
        public Image HeadIcon;

        [SerializeField]
        public Text ModeDesText;

        [SerializeField]
        public Image RankInvalidFrame;

        [SerializeField]
        public Image RankFrame;

        [SerializeField]
        public Image RankImg;

        [SerializeField]
        public Text  RankScore;

        [SerializeField]
        public Text RankDetailDes;

        [SerializeField]
        public Text RankGradeDes;

        [SerializeField]
        public Button RankTipBtn;

        [SerializeField]
        public Transform RankStarFrame;
        

        public void SetData(UListWarMainModeItem item)
        {

            HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarMainMode, WndID.WND_ID_WAR_MAIN_FRAME, item.nModeID);
            if (HeadIcon.sprite == null)
            {
                HeadIcon.gameObject.SetActive(false);
            }
            else
            {
                HeadIcon.gameObject.SetActive(true);
            }
            RankInvalidFrame.gameObject.SetActive(false);
            RankFrame.gameObject.SetActive(false);
            RankTipBtn.gameObject.SetActive(false);
            RankStarFrame.gameObject.SetActive(false);
            if (item.nModeID == (int)EWarModeDef.MODE_RankVS)
            {
                RankTipBtn.gameObject.SetActive(true);
                // 自己是否有排位信息 
                int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                int nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
                int nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
                int nRankIconID = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
                int nRankStar = GameLogicAPI.getActorRankStar(nMatchType, nRankScore);
                int nMaxRankStar = GameLogicAPI.getActorCurRankMaxStar(nMatchType, nRankScore);
                IntPtr ptr = GameLogicAPI.getActorRankNameByGrade(nMatchType, nRankGrade);
                string strRankName = "";
                if (ptr != IntPtr.Zero)
                    strRankName = IntPtrHelper.Ptr2Str(ptr);

                RankImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_WAR_MAIN_FRAME, 1, nRankIconID);
                if (RankImg.sprite == null || nRankScore <= 0)
                {
                    RankInvalidFrame.gameObject.SetActive(true);
                    RankFrame.gameObject.SetActive(false);
                }
                else
                {
                    RankStarFrame.gameObject.SetActive(true);
                    for (int index = 0; index < RankStarFrame.childCount; ++index)
                    {
                        if (nRankStar > 0)
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 2);
                            nRankStar--;
                            nMaxRankStar--;
                        }
                        else if (nMaxRankStar > 0)
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 1);
                            nMaxRankStar--;
                        }
                        else
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().gameObject.SetActive(false);
                        }
                    }

                    RankInvalidFrame.gameObject.SetActive(false);
                    RankFrame.gameObject.SetActive(true);

                    // 设置胜场 胜率  段位描述 星级
                    ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
                    int WinRate = info.playerInfo.nRankMatchNum > 0? (info.playerInfo.nRankWinNum * 100 / info.playerInfo.nRankMatchNum): 0;
                    RankDetailDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "WarStatistic", "MapWinCount")) + info.playerInfo.nRankWinNum + "  " + String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "WarStatistic", "MapWinRate")) + WinRate + "%";
                    RankGradeDes.text = strRankName;
                    RankScore.text = "" + nRankScore;

                    if (m_RankEffectParam != null)
                    {
                        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_RankEffectParam);
                        m_RankEffectParam = null;
                    }

                    int nOffset = info.playerInfo.nRankIconID - 1;
                    if (nOffset >= 0)
                    {
                        UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_RankEffect1 + nOffset, _tfAttachParent: this.RankImg.transform);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

                        m_RankEffectParam = param;
                        UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
                        paramPrefab.EffectGameObject.transform.SetAsFirstSibling();
                    }
                }

            }

            // 隐藏模式详细描述 以及 图片
            ModeDesText.text = item.strModeDesc;
            ModeDesText.gameObject.SetActive(false);

            // 设置tooltip
            //m_tooltip = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            //if (m_tooltip)
            //{
            //    string stTitleDesc = item.strModeTooltip;
            //    UBB.toHtml(ref stTitleDesc, UBB_FORMAT_TYPE.UGUI);
            //    m_tooltip.SetText(UTooltipParamName.BodyText, "<color=#FFFFFFFF><size=20>" + stTitleDesc + "</size></color>");
            //    m_tooltip.tipPosition = TipPosition.MouseTopRightCorner;
            //    m_tooltip.backgroundTint.a = 0;
            //}

            if (m_OpenEffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OpenEffectParam);
                m_OpenEffectParam = null;
            }

            if (item.bOpenEffect)
            {
                UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_CanJoinEffect, _tfAttachParent: this.HeadIcon.transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

                m_OpenEffectParam = param;
                UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
                paramPrefab.EffectGameObject.transform.SetAsFirstSibling();
            }
    
            ItemData = item;


            #region 新手引导部分
            if (!item.bUseGuideWidget)
            {
                gameObject.RemoveComponent<Guide.UGuideWidget>();
            }
            else
            {
                Guide.UGuideWidget guideWidget = gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = this.gameObject.AddComponent<Guide.UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)item.GuideWidgetData.nGuideID;
                guideWidget.GuideStepID = item.GuideWidgetData.nGuideStepID;
                guideWidget.GuideEffectID = item.GuideWidgetData.nGuideEffectID;
                guideWidget.IsForceGuide = item.GuideWidgetData.bForeceGuide;
            }
            #endregion
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            OnPointerExit(eventData);
            base.OnPointerClick(eventData); 
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);
            this.gameObject.transform.DOScale(1.0f,0.2f);
            SetSelectOverEffect();
            // 设置模式描述 以及 图片显示
            ModeDesText.gameObject.SetActive(true);
            
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            this.gameObject.transform.DOScale( 0.88f, 0.2f);
            DestorySelectOverEffect();
            // 隐藏设置模式描述 以及 图片显示
            ModeDesText.gameObject.SetActive(false);

        }

        public UListWarMainModeItem GetData()
        {
            return ItemData;
        }

        public void SetSelectOverEffect()
        {
            if (m_OldEffectParam != null)
            {
                return;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_Select, _tfAttachParent: this.HeadIcon.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParam = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.SetAsFirstSibling();

            UEffectParamBase paramex = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_SelectEX, _tfAttachParent: this.HeadIcon.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref paramex);

            m_OldEffectParamEX = paramex;
            UEffectPrefabParam paramPrefabEX = (UEffectPrefabParam)paramex;
            paramPrefabEX.EffectGameObject.transform.SetAsFirstSibling();

            // 移除开放效果
            if (ItemData.bOpenEffect)
            {
                if (m_OpenEffectParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OpenEffectParam);
                    m_OpenEffectParam = null;
                }
            }
             
        }

        public void DestorySelectOverEffect()
        {
            if (m_OldEffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParam);
                m_OldEffectParam = null;
            }

            if (m_OldEffectParamEX != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParamEX);
                m_OldEffectParamEX = null;
            }

            // 添加开放效果
            if (ItemData.bOpenEffect)
            {
                if (m_OpenEffectParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OpenEffectParam);
                    m_OpenEffectParam = null;
                }

                UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_CanJoinEffect, _tfAttachParent: this.HeadIcon.transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

                m_OpenEffectParam = param;
                UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
                paramPrefab.EffectGameObject.transform.SetAsFirstSibling();
            }
        }
    }
}
