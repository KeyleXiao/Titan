/*******************************************************************
** 文件名:	USideBtnPunishInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	显示惩罚时间相关
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using Match_ManagerDef;
using U3D_Render.Common;

namespace USpeedUI.TeammateInfo
{
    enum EN_SideBtn_Punish_Timer
    {
        ENMT_CountDownTimer = 0,     // 倒计时计数器
    }

    public class USideBtnPunishInfoWnd : UIPopupWnd<USideBtnPunishInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_SIDEBTN_PUNISH_TIMEINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Teammate/SideBtnPunishInfoView";
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

            // 订阅消息

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SIDEBTN_PUNISH_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SIDEBTN_PUNISH_HIDE, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SIDEBTN_PUNISH_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SIDEBTN_PUNISH_HIDE, this);

            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_SIDEBTN_PUNISH_SHOW:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.SetVisible(true);
                        m_wndView.KillTimer((int)EN_SideBtn_Punish_Timer.ENMT_CountDownTimer);
                        m_wndView.StartTimer((int)EN_SideBtn_Punish_Timer.ENMT_CountDownTimer);
                    }
                    break;

                case WndMsgID.WND_MSG_SIDEBTN_PUNISH_HIDE:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_SideBtn_Punish_Timer.ENMT_CountDownTimer);
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    public class USideBtnPunishInfoWndView : UIBaseWndView, ITimerHanlder
    {
        public Text Title;
        public Text LeftTime;
        public Text SelfPunishDes;
        public Button CloseBtn;
        public Text ConfirmText;
        private byte m_PunishType;
        private int  m_LeftTime;

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "PunishSideBtnDes");
            ConfirmText.text = ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn");
            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();
            TimerManager.KillTimer(this, (int)EN_SideBtn_Punish_Timer.ENMT_CountDownTimer);
        }

        public void StartTimer(int nTimerID)
        {
            LogicDataCenter.matchDataManager.GetSelfSidePunishInfo(ref m_PunishType, ref m_LeftTime);

            switch ((EPunishmentType)m_PunishType)
            {
                case EPunishmentType.emPunishmentType_Plug:     // 外挂惩罚：使用外挂惩罚
                    {
                        string strContent = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "MatchReqPunishPlugDes");
                        UBB.toHtml(ref strContent);
                        SelfPunishDes.text = strContent;
                    }
                    break;
                case EPunishmentType.emPunishmentType_Match:    // 撮合惩罚：撮合退出相关
                    {
                        string strContent = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "MatchCancelPunishDes");
                        UBB.toHtml(ref strContent);
                        SelfPunishDes.text = strContent;
                    }
                    break;
                case EPunishmentType.emPunishmentType_War:      // 战场惩罚：挂机或者逃跑
                    {
                        string strContent = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarPunishDes");
                        UBB.toHtml(ref strContent);
                        SelfPunishDes.text = strContent;
                    }
                    break;
                case EPunishmentType.emPunishmentType_Accuse:   // 举报惩罚：玩家举报
                    {
                        string strContent = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "AccusePunishDes");
                        UBB.toHtml(ref strContent);
                        SelfPunishDes.text = strContent;
                    }
                    break;
                default:
                    break;

            }
            ShowWndInfo();

            TimerManager.SetTimer(this, nTimerID, 1.0f);
        }

        public void ShowWndInfo()
        {
            if (m_LeftTime <= 0)
            {
                OnCloseButton();
                return;
            }

            LeftTime.text = LogicDataCenter.matchDataManager.GetLeftTimeStr(m_LeftTime);
        }
        

        public void KillTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, nTimerID);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_SideBtn_Punish_Timer.ENMT_CountDownTimer:
                    {
                        m_LeftTime--;
                        ShowWndInfo();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnCloseButton()
        {
            SetVisible(false);
        }
    }
}
