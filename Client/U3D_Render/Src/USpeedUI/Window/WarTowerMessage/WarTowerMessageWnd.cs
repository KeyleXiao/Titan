using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarTowerMessage
{
    public class WarTowerMessageWnd : UIBaseWnd<UViewWarTowerMessage>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WARTOWERMESSAGE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarTowerMessage/UViewWarTowerMessage";
        }

        public override bool Init()
        {
            // 订阅基础消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.LoadView();
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WARTOWER_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.UpdateData(msgData as UWarTowerSoldierMsgData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UViewWarTowerMessage : UIBaseWndView
    {
        public Text countTxt;
        public void UpdateData(UWarTowerSoldierMsgData data)
        {
            countTxt.text = data.nSoldierCount.ToString();
            m_wnd.SetVisible(data.bShow);
        }
    }
}
