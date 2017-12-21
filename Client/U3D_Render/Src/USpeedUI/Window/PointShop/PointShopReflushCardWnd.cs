using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using DataCenter;
using Data.PointShopConfig;
using GameLogic;

namespace USpeedUI.PointShop
{
    public class PointShopReflushCardWnd : UIPopupWnd<PointShopReflushCardWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_POINTSHOPREFLUSHCARD;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PointShop/PointShopReflushCard";
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
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_CLOSE, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_CLOSE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                            m_wndView.OnOpenWnd();
                    }
                    break;
                case WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.UpdateReflushNum();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_CLOSE:
                    {
                        if (m_wndView != null)
                            m_wndView.SetVisible(false); 
                    }
                    break;

                default:
                    break;
            }
        }
    }

    public class PointShopReflushCardWndView : UIBaseWndView
    {
        public Text titleDesc;
        public Text priceDesc;
        public Text price;
        public Text numDesc;
        public Text reflushBtnDesc;
        public Text cancelBtnDesc;

        public void OnOpenWnd()
        {
            SSchemePointShopGoodsConfig config = null;
            List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Special);
            if (configList == null || configList.Count <= 0)
                return;
            foreach (var item in configList)
            {
                if (item.nConsumpApproach == (int)PointShopConsumpApproach.Approach_MysteryShop_Reflush)
                {
                    config = item;
                    break;
                }
            }
            if (config == null)
                return;

            titleDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "reflushNow");
            priceDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "reflushPrice");
            numDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "reflushNum"), LogicDataCenter.pointShopDataManager.LeftReflushNum);
            reflushBtnDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "confirmReflush");
            cancelBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn");
            price.text = config.nPointPrice.ToString();
        }

        public void UpdateReflushNum()
        {
            numDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "reflushNum"), LogicDataCenter.pointShopDataManager.LeftReflushNum);
        }

        public void OnClickReflushBtn()
        {
            // 次数不足
            if (LogicDataCenter.pointShopDataManager.LeftReflushNum <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_POINTSHOP_NOTHAVE_REFLUSHNUM);
                return;
            }

            // 点券不足
            SSchemePointShopGoodsConfig config = null;
            List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Special);
            if (configList == null || configList.Count <= 0)
                return;
            foreach (var item in configList)
            {
                if (item.nConsumpApproach == (int)PointShopConsumpApproach.Approach_MysteryShop_Reflush)
                {
                    config = item;
                    break;
                }
            }
            if (config == null)
                return;

            int nCurPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
            if (config.nPointPrice > 0 && config.nPointPrice > nCurPoint)
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_Point);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                return;
            }

            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REFLUSH_CARD);
        }

        public void OnClickCancelBtn()
        {
            SetVisible(false);
        }

        public void OnClickCloseBtn()
        {
            SetVisible(false);
        }
    }
}
