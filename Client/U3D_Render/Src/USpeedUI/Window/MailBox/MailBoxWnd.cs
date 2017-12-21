using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using USpeedUI;
using DataCenter;
using USpeedUI.UWidgets;

namespace USpeedUI.MailBox
{
    // 消息邮箱子窗口类型
    public enum MailBoxSubWndType
    {
        WND_MAIL,
        WND_SOCIALMSG,
        WND_VERSIONINFO,
    }

    // 消息邮箱按钮标签类型
    public enum MailBoxFrameButtonType
    {
        BTN_MAIL,
        BTN_SOCIALMSG,
        BTN_VERSIONINFO,
    }

    public class MailBoxWnd : UIPopupWnd<MailBoxWndView>
    {
        private MailBoxWndView view { get { return m_wndView as MailBoxWndView; } }

        private List<IUISubWnd> m_subWndList = new List<IUISubWnd>();
        private MailBoxSubWndType m_curShowWndType;
        private MailBoxFrameButtonType m_curShowPageType;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_MAILBOX;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "MailBox/MailBoxFrame";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 同级子窗体（注： 非弹出窗体）
            m_subWndList.Add(new MailWnd());

            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                m_subWndList[i].Init();
            }
            
            SetVisible(false);

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, this);
            // 订阅进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            // MailWnd 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAIL_REQUESTMAILBASELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAIL_DETAILINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS, this);     

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.OpenMailBox, KeyStatus.Down, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, this);
            // 退订进入战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            // MailWnd 
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAIL_REQUESTMAILBASELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAIL_DETAILINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.OpenMailBox, KeyStatus.Down, this);
          
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (!visible)
            {
                MailWnd mailWnd = GetSubWnd(MailBoxSubWndType.WND_MAIL) as MailWnd;
                if (mailWnd != null)
                {
                    mailWnd.ResetViewShow();
                }
            }

            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = visible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        SetVisible(false);
                    }
                    break;

                case WndMsgID.WND_MSG_MAILBOX_OPEN:
                    {
                        UpdateMailBox();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                    }
                    break;

                // WND_MAIL
                case WndMsgID.WND_MSG_MAIL_REQUESTMAILBASELIST:
                case WndMsgID.WND_MSG_MAIL_DETAILINFO:
                case WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS:
                case WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS:
                    {
                        IUISubWnd mailWnd = GetSubWnd(MailBoxSubWndType.WND_MAIL);
                        if (mailWnd != null)
                            mailWnd.OnMessage(msgID, msgData);
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (index == FuntionShortCutSlotIndex.OpenMailBox && status == KeyStatus.Down)
            {
                UpdateMailBox();
            }

            return true;
        }

        private void UpdateMailBox()
        {
            bool bVisible = IsVisible();

            SetVisible(!bVisible);

            if (!bVisible)
            {
                // 隐藏其他同级窗体
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, null);
                UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);
                UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);

                if (EntityFactory.MainHeroView != null)
                {
                    SwitchWndPage(MailBoxFrameButtonType.BTN_MAIL);
                }
            }
        }

        private IUISubWnd GetSubWnd(MailBoxSubWndType wndType)
        {
            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                if (m_subWndList[i].GetWndType() == (int)wndType)
                    return m_subWndList[i];
            }

            return null;
        }

        private void ShowSubWnd(MailBoxSubWndType wndType)
        {
            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                if (m_subWndList[i].GetWndType() == (int)wndType)
                    m_subWndList[i].SetVisible(true);
                else
                    m_subWndList[i].SetVisible(false);
            }
            m_curShowWndType = wndType;
        }

        public void InitSubWndContainer(Transform container)
        {
            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                m_subWndList[i].SetContainer(container);
            }
        }

        public void SwitchWndPage(MailBoxFrameButtonType pageType)
        {
            if (m_wndView == null || EntityFactory.MainHeroView == null)
                return;

            switch (pageType)
            {
                case MailBoxFrameButtonType.BTN_MAIL:
                    {
                        ShowSubWnd(MailBoxSubWndType.WND_MAIL);
                    }
                    break;
                case MailBoxFrameButtonType.BTN_SOCIALMSG:
                    {
                        ShowSubWnd(MailBoxSubWndType.WND_SOCIALMSG);
                    }
                    break;
                case MailBoxFrameButtonType.BTN_VERSIONINFO:
                    {
                        ShowSubWnd(MailBoxSubWndType.WND_VERSIONINFO);
                    }
                    break;
            }

            m_curShowPageType = pageType;
        }
    }

    public class MailBoxWndView : UIBaseWndView
    {
        public Transform confineFrame;
        public UButtonGroup buttonGroup;

        private MailBoxWnd mailBoxWnd { get { return m_wnd as MailBoxWnd; } }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_UseImageEffect = true;

            MailBoxWnd mailBoxWnd = wnd as MailBoxWnd;
            if (mailBoxWnd != null)
                mailBoxWnd.InitSubWndContainer(confineFrame);

            return true;
        }

        public void SetButtonSelect(int nIndex)
        {
            buttonGroup.SetItemSelectedItem(nIndex, true);
        }

        public void ResetMailBtnSelect()
        {
            buttonGroup.SetItemSelectedItem(0, false);
        }

        public void OnMailBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mailBoxWnd.SwitchWndPage(MailBoxFrameButtonType.BTN_MAIL);
        }

        public void OnCloseBtnClick()
        {
            // 关闭按钮点击
            SetVisible(false);
        }

        public void OnConfirmBtnClick()
        {
            // 确定按钮点击
            UnityEngine.Debug.Log("OnConfirmBtnClick ");
        }

        public void OnSocialMsgBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mailBoxWnd.SwitchWndPage(MailBoxFrameButtonType.BTN_SOCIALMSG);

            // 确定按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "社会消息系统",
                                   message: "社会消息系统正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } },
                                   modal: true
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnVersionInfoMsgBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mailBoxWnd.SwitchWndPage(MailBoxFrameButtonType.BTN_VERSIONINFO);

            // 确定按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "版本信息系统",
                                   message: "版本信息系统正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } },
                                   modal: true
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

    }
}
