using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using Data.PointShopConfig;
using Data.ActorPrizeConfig;
using War;
using ASpeedGame.Data.RunePage;
using GameLogic;
using U3D_Render.Common;

namespace USpeedUI.PointShop
{
    public class PointShopBuyGoodsWnd : UIPopupWnd<PointShopBuyGoodsWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_POINTSHOPBUYGOODS;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PointShop/PointShopBuyGoods";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_BUYRESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_UPDATEBOTTOMDESC, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_BUYRESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_UPDATEBOTTOMDESC, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN:
                    {
                        if(GameLogicAPI.isInWarScene() > 0)
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_POINTSHOP_CANT_BUY_INWAR);
                            return;
                        }

                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            if (!m_wndView.OnOpen(msgData as PointShopBuyGoodsInitData))
                                SetVisible(false);
                        }
                    }
                    break;

                case WndMsgID.WND_MSG_POINTSHOP_BUYRESULT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnBuyResult(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_POINTSHOPBUYGOODS_UPDATEBOTTOMDESC:
                    {
                        if (m_wndView != null)
                        {
                            if (m_wndView.gameObject.activeSelf)
                                m_wndView.ReflushBottomDesc();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO:
                    {
                        if (m_wndView != null)
                            m_wndView.ReflushIconFrame();
                    }
                    break;

                default:
                    break;
            }
        }
    }

    public class PointShopBuyGoodsWndView : UIBaseWndView
    {
        public Text titleText;
        public Text topDescText;
        public RectTransform shopGoodsFrame;
        public Text BottomDescText;

        public GameObject numSelectFrame;
        public Button minusNumBtn;
        public Button minNumBtn;
        public InputField numText;
        public Button plusNumBtn;
        public Button maxNumBtn;

        public GameObject pointPriceFrame;
        public Text curPointDesc;
        public Text needPointDesc;
        public Text leftPointDesc;
        public Text curPointText;
        public Text needPointText;
        public Text leftPointText;
        public Button pointBuyBtn;
        public Text pointBuyBtnText;

        public GameObject moneyPriceFrame;
        public Text curMoneyDesc;
        public Text needMoneyDesc;
        public Text leftMoneyDesc;
        public Text curMoneyText;
        public Text needMoneyText;
        public Text leftMoneyText;
        public Button moneyBuyBtn;
        public Text moneyBuyBtnText;

        private PointShopSingleGoodsItem shopGoodsItem;
        private SSchemePointShopGoodsConfig m_curGoodsConfig;
        private SSchemeActorPrizeConfig m_curPrizeConfig;

        private int m_nBuyNum;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            shopGoodsItem = LogicDataCenter.pointShopDataManager.CreateShopSingleGoodsItem(shopGoodsFrame);

            if (shopGoodsItem == null)
                return false;

            titleText.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "TitleText");
            curPointDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "CurPointDesc");
            needPointDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "NeedPointDesc");
            leftPointDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "LeftPointDesc");
            pointBuyBtnText.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "PointBuyBtn");
            curMoneyDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "CurMoneyDesc");
            needMoneyDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "NeedMoneyDesc");
            leftMoneyDesc.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "LeftMoneyDesc");
            moneyBuyBtnText.text = ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "MoneyBuyBtn");

            return true;
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            if (!visible)
            {
                m_curGoodsConfig = null;
                m_curPrizeConfig = null;
            }

            if (visible)
            {
                EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
            }
            else
            {
                EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
            }
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (e.objev == null || !e.objev.IsHero)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_TICKET:
                case (int)ENTITY_PROPERTY.PROPERTY_GOLD:
                    {
                        ReflushMoneyZone();
                    }
                    break;
                default:
                    break;
            }
        }

        public bool OnOpen(PointShopBuyGoodsInitData uiData)
        {
            if (uiData == null)
                return false;

            m_curGoodsConfig = PointShopConfig.Instance.GetPointShopGoodsConfig(uiData.nSellID);
            if (m_curGoodsConfig == null)
                return false;

            m_curPrizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_curGoodsConfig.nPrizeID);
            if (m_curPrizeConfig == null)
                return false;

            numSelectFrame.SetActive(true);
            if (m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                numSelectFrame.SetActive(false);
            }

            shopGoodsItem.SetGoodsInfo(m_curGoodsConfig);
            m_nBuyNum = 1;
            ReflushTopDesc();
            ReflushNumBtn();
            ReflushMoneyZone();
            ReflushBottomDesc();

            return true;
        }

        public void OnMinusBtn()
        {
            if (m_nBuyNum > 1)
                --m_nBuyNum;
            ReflushNumBtn();
            ReflushMoneyZone();
        }

        public void OnMinNumBtn()
        {
            m_nBuyNum = 1;
            ReflushNumBtn();
            ReflushMoneyZone();
        }

        public void OnPlusBtn()
        {
            if (m_nBuyNum < GetCanBuyMax())
                ++m_nBuyNum;
            ReflushNumBtn();
            ReflushMoneyZone();
        }

        public void OnMaxNumBtn()
        {
            m_nBuyNum = GetCanBuyMax();
            ReflushNumBtn();
            ReflushMoneyZone();
        }

        public void OnPointBuyBtn()
        {
            String strLeftPoint = leftPointText.text;
            int nLeftPoint = Convert.ToInt32(strLeftPoint);
            if (nLeftPoint >= 0)
            {
                String strClientPrice = needPointText.text;
                int nClientPrice = (strClientPrice.Length > 0) ? Convert.ToInt32(strClientPrice) : 0;
                if (m_curGoodsConfig != null && m_nBuyNum > 0 && nClientPrice > 0)
                    LogicDataCenter.pointShopDataManager.ReqeustBuyGoods(m_curGoodsConfig.nSellID, 0, m_nBuyNum, nClientPrice);
            }
            else
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_Point);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
            }
        }

        public void OnMoneyBuyBtn()
        {
            String strLeftMoney = leftMoneyText.text;
            int nLeftMoney = Convert.ToInt32(strLeftMoney);
            if (nLeftMoney >= 0)
            {
                String strClientPrice = needMoneyText.text;
                int nClientPrice = (strClientPrice.Length > 0) ? Convert.ToInt32(strClientPrice) : 0;
                if (m_curGoodsConfig != null && m_nBuyNum > 0 && nClientPrice > 0)
                    LogicDataCenter.pointShopDataManager.ReqeustBuyGoods(m_curGoodsConfig.nSellID, 1, m_nBuyNum, nClientPrice);
            }
            else
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_Gold);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
            }
        }

        void ReflushMoneyZone()
        {
            int nCurPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
            int nNeedPoint = 0;
            if (m_curGoodsConfig.nNumLimitID > 0)
                nNeedPoint = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(m_curGoodsConfig.nNumLimitID, m_curGoodsConfig.nSellID, 0, m_nBuyNum);
            else
                nNeedPoint = m_curGoodsConfig.nPointPrice * m_nBuyNum;

            if (nNeedPoint > 0)
            {
                pointPriceFrame.SetActive(true);

                int nLeftPoint = nCurPoint - nNeedPoint;
                curPointText.text = nCurPoint.ToString();
                needPointText.text = nNeedPoint.ToString();
                leftPointText.text = nLeftPoint.ToString();
                if (nLeftPoint < 0)
                    leftPointText.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                else
                    leftPointText.color = UDefines.CommomColor(ECommonColor.ECC_Blue1);
            }
            else
            {
                pointPriceFrame.SetActive(false);
            }

            //int nCurBindGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BINDGOLD);
            int nCurBindGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
            int nNeedMoney = 0;
            if (m_curGoodsConfig.nNumLimitID > 0)
                nNeedMoney = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(m_curGoodsConfig.nNumLimitID, m_curGoodsConfig.nSellID, 1, m_nBuyNum);
            else
                nNeedMoney = m_curGoodsConfig.nMoneyPrice * m_nBuyNum;

            if (nNeedMoney > 0)
            {
                moneyPriceFrame.SetActive(true);
                int nLeftMoney = nCurBindGold - nNeedMoney;
                curMoneyText.text = nCurBindGold.ToString();
                needMoneyText.text = nNeedMoney.ToString();
                leftMoneyText.text = nLeftMoney.ToString();
                if (nLeftMoney < 0)
                    leftMoneyText.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                else
                    leftMoneyText.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
            else
            {
                moneyPriceFrame.SetActive(false);
            }

            pointBuyBtn.interactable = true;
            moneyBuyBtn.interactable = true;
            if (m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                int nHeroID = m_curPrizeConfig.nPrizeParam[0];
                ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                if (actorHeroInfo.dicEntityHeroInfo.ContainsKey(nHeroID) && actorHeroInfo.dicEntityHeroInfo[nHeroID].bIsBuy > 0)
                {
                    pointBuyBtn.interactable = false;
                    moneyBuyBtn.interactable = false;
                }
            }
        }

        void ReflushNumBtn()
        {
            if (m_nBuyNum <= 1)
            {
                minusNumBtn.interactable = false;
                minNumBtn.interactable = false;
            }
            else
            {
                minusNumBtn.interactable = true;
                minNumBtn.interactable = true;
            }

            if (m_nBuyNum >= GetCanBuyMax())
            {
                plusNumBtn.interactable = false;
                maxNumBtn.interactable = false;
            }
            else
            {
                plusNumBtn.interactable = true;
                maxNumBtn.interactable = true;
            }

            numText.text = m_nBuyNum.ToString();
        }

        void ReflushTopDesc()
        {
            topDescText.text = "";
            if (m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_RUNE)
            {
                int nRuneID = m_curPrizeConfig.nPrizeParam[0];
                SSchemeRuneConfig runeConfig = null;
                RunePage.Instance.GetRuneInfo(nRuneID, ref runeConfig);
                if (runeConfig != null)
                {
                    List<RuneItemInfo> runeList = null;
                    int nRuneNum = 0;
                    LogicDataCenter.runePageDataManager.dicRuneInlayTypeStoreInfo.TryGetValue(runeConfig.nRuneInlayType, out runeList);
                    if (runeList != null)
                    {
                        for (int i = 0; i < runeList.Count; ++i)
                        {
                            if (runeList[i].nRuneId == nRuneID)
                                nRuneNum += runeList[i].nRuneNum;
                        }
                    }
                    topDescText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "HaveRuneNumDesc"), nRuneNum);
                }
            }
            else if (m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
            {
                if (m_curGoodsConfig.nNumLimitID > 0)
                {
                    SPointShopNumLimitPrice limitConfig = PointShopConfig.Instance.GetShopNumLimitPriceConfig(m_curGoodsConfig.nNumLimitID);
                    if (limitConfig != null && limitConfig.limitStepList.Count > 0)
                    {
                        int nMaxNum = limitConfig.limitStepList[limitConfig.limitStepList.Count - 1].nLimitNum;
                        int nBoughtNum = 0;
                        cmd_entity_update_pointshop_buynum_info buyNumInfo = new cmd_entity_update_pointshop_buynum_info();
                        if (LogicDataCenter.pointShopDataManager.BuyNumInfo.TryGetValue(m_curGoodsConfig.nSellID, out buyNumInfo))
                        {
                            nBoughtNum = buyNumInfo.nBoughtNum;
                        }

                        int nLeftNum = nMaxNum - nBoughtNum;

                        topDescText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShopBuyGoods", "CanBuyCardNumDesc"), nLeftNum);
                    }
                }
            }
        }

        public void ReflushBottomDesc()
        {
            BottomDescText.text = "";
            if (m_curGoodsConfig.nGoodsType == (int)PointShopGoodsType.Goods_BenefitCard)
            {
                int nSellID = m_curGoodsConfig.nSellID;
                SSchemePointShopBenefitCardConfig cardConfig = PointShopConfig.Instance.GetBenefitCardConfig(nSellID);
                if (cardConfig == null)
                {
                    return;
                }

                BenefitCardInfo info = DataCenter.LogicDataCenter.playerSystemDataManager.GetBenefitCardManager().getRemainBenefitCard(cardConfig.nBenefitCardType, cardConfig.nBenefitCardSubType);
                if (info == null)
                {
                    return;
                }

                BottomDescText.text = info.getRemainTimeDesc();
            }
        }

        public void ReflushIconFrame()
        {
            if (shopGoodsItem !=null && m_curGoodsConfig != null)
                shopGoodsItem.SetGoodsInfo(m_curGoodsConfig);
        }

        public void OnNumChanged(InputField _input)
        {
            if (_input == null)
                return;

            if (_input.text == "-")
                _input.text = "";

            int nNewNum = (_input.text.Length > 0) ? Convert.ToInt32(_input.text) : 0;
            if (nNewNum < 1)
                nNewNum = 1;
            int nMaxNum = GetCanBuyMax();
            if (nNewNum > nMaxNum)
                nNewNum = nMaxNum;

            m_nBuyNum = nNewNum;
            ReflushNumBtn();
            ReflushMoneyZone();
        }

        public void OnBuyResult(UIMsgCmdData uiData)
        {
            cmd_entity_pointshop_buyresult data = IntPtrHelper.toData<cmd_entity_pointshop_buyresult>(uiData.ptrParam);
            if (data.nSucceed > 0)
            {
                // 买完不关闭窗口
                if (m_curPrizeConfig != null)
				{
					if (m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD
						|| m_curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_BENEFITCARD)
					{
						m_nBuyNum = 1;
						ReflushTopDesc();
						ReflushNumBtn();
						ReflushMoneyZone();
						ReflushBottomDesc();
						return;
					}
				}


                // 关闭购买窗口
                m_wnd.SetVisible(false);
            }
        }

        public void OnCloseBtn()
        {
            m_wnd.SetVisible(false);
        }

        int GetCanBuyMax(int nMinValue = 1)
        {
            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_curGoodsConfig.nPrizeID);
            if (prizeConfig == null)
                return nMinValue;

            int nPointCanBuyMax = 0;
            int nMoneyCanBuyMax = 0;
            int nCurPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
            //int nCurBindGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BINDGOLD);
            int nCurBindGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
            if (m_curGoodsConfig.nNumLimitID > 0)
            {
                nPointCanBuyMax = LogicDataCenter.pointShopDataManager.CalcNumLimitBuyMax(m_curGoodsConfig.nNumLimitID, m_curGoodsConfig.nSellID, 0, nCurPoint);
                nMoneyCanBuyMax = LogicDataCenter.pointShopDataManager.CalcNumLimitBuyMax(m_curGoodsConfig.nNumLimitID, m_curGoodsConfig.nSellID, 1, nCurBindGold);
            }
            else
            {
                if (m_curGoodsConfig.nPointPrice > 0)
                {
                    nPointCanBuyMax = nCurPoint / m_curGoodsConfig.nPointPrice;
                }

                if (m_curGoodsConfig.nMoneyPrice > 0)
                {
                    nMoneyCanBuyMax = nCurBindGold / m_curGoodsConfig.nMoneyPrice;
                }
            }

            int nResult = Mathf.Max(nPointCanBuyMax, nMoneyCanBuyMax);

            // 限制购买的最大数量：（1）当前最大输入3位数，（2）不同种类有不同数量限制，未限制的默认为999
            int nMaxValue = LogicDataCenter.pointShopDataManager.CalcMaxNumByPriceType(prizeConfig.nPrizeType);
            nResult = Mathf.Min(nResult, nMaxValue);

            nResult = Mathf.Max(nResult, nMinValue);
            //if (nResult < nMinValue)
            //    nResult = nMinValue;
            return nResult;
        }

    }
}
