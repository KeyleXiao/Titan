using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using ASpeedGame.Data.LegendCupConfig;
using U3D_Render.Common;

namespace USpeedUI.LegendCup
{
    public class LegendCupTipWnd : UIPopupWnd<LegendCupTipWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_MSGBOXTIP;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupTipView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW_STR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW_STR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnShowTipWnd(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW_STR:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            UPopupTipMsgData cmdData = (UPopupTipMsgData)data;
                            m_wndView.OnShowTipStrWnd(cmdData.strTitle, cmdData.strContent);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class LegendCupTipWndView : UIBaseWndView
    {
        public Text TipTitle;
        public Text TipContent;

        public void OnShowTipWnd(int nTipType)
        {
            TipTitle.text = "";
            TipContent.text = "";

            if (nTipType <= (int)LegendCupTipType.LCTT_None || nTipType >= (int)LegendCupTipType.LCTT_MAX)
                return;

            SSchemeLegendCupTipConfig tipConfig = LegendCupConfig.Instance.GetCupTipConfig(nTipType);
            if (tipConfig == null)
                return;

            TipTitle.text = tipConfig.sCupTitle;
            if (!tipConfig.sCupContent.Equals(string.Empty))
            {
                UBB.toHtml(ref tipConfig.sCupContent, UBB_FORMAT_TYPE.UGUI);
                TipContent.text = tipConfig.sCupContent;
            }
        }

        public void OnShowTipStrWnd(string strTitle, string strContent)
        {
            UBB.toHtml(ref strTitle);
            UBB.toHtml(ref strContent);
            TipTitle.text = strTitle;
            TipContent.text = strContent;
        }

        public void OnClickCloseBtn()
        {
            //SetVisible(false);
            m_wnd.UnloadView();
        }
    }
}
