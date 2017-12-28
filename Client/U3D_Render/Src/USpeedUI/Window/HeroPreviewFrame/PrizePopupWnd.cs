using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using UnityEngine;
using USpeedUI.LegendCup;
using ASpeedGame.Data.HeroTalentConfig;
using DataCenter;

namespace USpeedUI.HeroPreviewFrame
{
    public class PrizePopupWnd : UIPopupWnd<PrizePopupWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_PRIZEPOPUP;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "HeroDetailInfo/PrizePopupView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_CLOSE, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_CLOSE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_PRIZEPOPUP_OPEN:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            m_wndView.OnShowPrizePopupWnd(data as UPrizePopupData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW:
                    {
                        if (LogicDataCenter.playerSystemDataManager.ListActorUnlockPrizeID.Count > 0)
                        {
                            SetVisible(true);
                            if (m_wndView != null)
                            {
                                UPrizePopupData uiData = new UPrizePopupData();
                                uiData.strPrizeTitle = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ActorUnlockTitle");
                                uiData.strPrizeDesc = String.Format(ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ActorUnlockContent"), LogicDataCenter.playerSystemDataManager.WarEndActorLv);
                                uiData.nPrizeIdList = LogicDataCenter.playerSystemDataManager.ListActorUnlockPrizeID;
                                uiData.nHeroID = 0;
                                uiData.nTalentIDList = new List<int>();
                                m_wndView.OnShowPrizePopupWnd(uiData);
                            }
                            // 清除数据
                            LogicDataCenter.playerSystemDataManager.ListActorUnlockPrizeID.Clear();
                            LogicDataCenter.playerSystemDataManager.WarEndActorLv = 0;
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PRIZEPOPUP_CLOSE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class PrizePopupWndView : UIBaseWndView
    {
        public Text PrizeTitle;
        public Text PrizeDescription;
        public Text ConfirmBtnDesc;

        public RectTransform PrizeFrame;
        public GameObject PrizeTemplate;

        public void OnShowPrizePopupWnd(UPrizePopupData data)
        {
            if (data.nPrizeIdList == null)
                return;
            int nPrizeCount = data.nPrizeIdList.Count;

            // 动态改变UI大小
            //// 方案1水平排列
            //RectTransform rectParent = this.gameObject.transform as RectTransform;
            //RectTransform rectItem = PrizeTemplate.transform as RectTransform;
            //float fNeedWidth = (nPrizeCount + 1) * rectItem.sizeDelta.x;
            //Vector2 vecParentSize = rectParent.sizeDelta;
            //vecParentSize.x = Mathf.Max(500.0f, fNeedWidth);
            //rectParent.sizeDelta = vecParentSize;
            // 方案2竖直排列
            //RectTransform rectParent = this.gameObject.transform as RectTransform;
            //RectTransform rectItem = PrizeTemplate.transform as RectTransform;
            //Vector2 vecParentSize = rectParent.sizeDelta;
            //vecParentSize.y = ((nPrizeCount - 1) / 4 + 1) * rectItem.sizeDelta.y + 212;// 4个Item一行
            //rectParent.sizeDelta = vecParentSize;

            // 设置基础
            PrizeTitle.text = data.strPrizeTitle;
            PrizeDescription.text = data.strPrizeDesc;
            PrizeDescription.gameObject.SetActive(!String.IsNullOrEmpty(data.strPrizeDesc));
            ConfirmBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn");

            // 设置奖励Item
            int nChildCount = PrizeFrame.childCount;
            for (int i = nChildCount - 1; i >= 0; i--)
            {
                Transform trans = PrizeFrame.GetChild(i);
                if (trans)
                {
                    var temp = trans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            // 普通奖励
            if (data.nPrizeIdList != null)
            {
                for (int i = 0; i < nPrizeCount && data.nPrizeIdList[i] > 0; i++)
                {
                    GameObject prizeObj = ResNode.InstantiateRes(PrizeTemplate);
                    if (!prizeObj)
                        return;
                    RectTransform prizeRect = prizeObj.transform as RectTransform;
                    if (!prizeRect)
                        return;
                    prizeRect.SetParent(PrizeFrame, false);
                    LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                    if (!prizeItem)
                        return;
                    prizeItem.SetData(data.nPrizeIdList[i]);
                }
            }

            // 天赋奖励
            if (data.nTalentIDList != null)
            {
                int nHeroID = data.nHeroID;
                for (int j = 0; j < data.nTalentIDList.Count; ++j)
                {
                    GameObject prizeObj = ResNode.InstantiateRes(PrizeTemplate);
                    if (!prizeObj)
                        return;
                    RectTransform prizeRect = prizeObj.transform as RectTransform;
                    if (!prizeRect)
                        return;
                    prizeRect.SetParent(PrizeFrame, false);
                    LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                    if (!prizeItem)
                        return;

                    int nTalentID = data.nTalentIDList[j];

                    SSchemeHeroTalentConfig talentConfig = HeroTalentConfigPage.Instance.GetHeroTalentConfig(nHeroID, nTalentID);
                    prizeItem.SetData(talentConfig);
                }
            }

            // 金币、经验奖励
            if (data.nMoneyCount > 0)
            {
                GameObject prizeObj = ResNode.InstantiateRes(PrizeTemplate);
                if (!prizeObj)
                    return;
                RectTransform prizeRect = prizeObj.transform as RectTransform;
                if (!prizeRect)
                    return;
                prizeRect.SetParent(PrizeFrame, false);
                LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                if (!prizeItem)
                    return;
                prizeItem.SetData(data.nMoneyCount, ECupPrizeType.ECPT_Gold);
            }
            if (data.nExpCount > 0)
            {
                GameObject prizeObj = ResNode.InstantiateRes(PrizeTemplate);
                if (!prizeObj)
                    return;
                RectTransform prizeRect = prizeObj.transform as RectTransform;
                if (!prizeRect)
                    return;
                prizeRect.SetParent(PrizeFrame, false);
                LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                if (!prizeItem)
                    return;
                prizeItem.SetData(data.nExpCount, ECupPrizeType.ECPT_Exp);
            }


            PrizeTemplate.SetActive(false);
        }

        public void OnClickConfirmBtn()
        {
            OnClickCloseBtn();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
