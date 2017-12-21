using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using GameLogic;
using DataCenter;

namespace USpeedUI.LegendCup
{
    public class LegendCupPasswordWnd : UIPopupWnd<LegendCupPasswordWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_PASSWORD;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupPasswordView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_CLOSE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_CLOSE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_SHOW:
                    {
                        SetVisible(true);

                        LegendCupIDData cmdData = (LegendCupIDData)data;
                        if (m_wndView != null)
                            m_wndView.OnAddLegendCup(cmdData.nLegendCupID);
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_CLOSE:
                    {
                        if (m_wndView != null)
                            UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class LegendCupPasswordWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text PasswordDes;
        public Text ConfirmDes;
        public Text CancelDes;
        public InputField PasswordIF;

        private long nLegendCupID;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPassword", "Title");
            PasswordDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPassword", "Password");
            ConfirmDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPassword", "ConfirmBtn");
            CancelDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPassword", "CancelBtn");
            PasswordIF.placeholder.GetComponent<Text>().text = "";

            return true;
        }

        public void OnAddLegendCup(long nCupID)
        {
            nLegendCupID = nCupID;
        }

        public void OnClickConfirmBtn()
        {
            if (PasswordIF.text == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_ENTER_ANY_PASSWORD);
                return;
            }

            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;
            cmd_legendcup_add_regist_kinlist addcup = new cmd_legendcup_add_regist_kinlist();
            addcup.nLegendCupID = nLegendCupID;
            addcup.nKinID = LogicDataCenter.kinDataManager.KinBaseData.nKinID;
            addcup.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            addcup.byCreateCupType = cupBaseData.byCreateCupType;
            addcup.szPassword = PasswordIF.text;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_add_regist_kinlist>(GameLogicDef.GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST, ref addcup);
        }

        public void OnClickCancelBtn()
        {
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
