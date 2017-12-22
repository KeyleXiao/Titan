/*******************************************************************
** 文件名:	AcceptWaitInfoWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	匹配接受或者拒绝界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using Match_ManagerDef;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine;


namespace USpeedUI.AcceptWaitInfo
{
    enum EN_AcceptWait_Timer
    {
        ENMT_AcceptWaitTimer = 0,     // 已用匹配时间定时器id
    }

    public class UAcceptWaitInfoWnd : UIPopupWnd<UAcceptWaitInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_ACCEPTWAITINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Teammate/AcceptWaitView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_CLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);

            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);

            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_ACCEPTWAIT_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }

                        if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
                            GameLogic.cmd_wait_accept_wnd data = helper.get<GameLogic.cmd_wait_accept_wnd>();

                            m_wndView.OpenModalWnd();
                            m_wndView.KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
                            m_wndView.StartTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer, data.nWaitTime, data.dwAcceptCheckID, (int)data.byMatchType);
                            Initialize.Instance.FlashWindow();
                        }
                    }
                    break;

                case WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.CloseModalWnd();
                        m_wndView.KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
                    }
                    break;

                case WndMsgID.WND_MSG_ACCEPTWAIT_CLOSE:
                    {
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
                        if (m_wndView == null)
                            return;
                        m_wndView.CloseModalWnd();
                        m_wndView.KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
                    }
                    break;

                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER:
                    {
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
                        if (m_wndView == null)
                            return;
                        m_wndView.CloseModalWnd();
                        m_wndView.KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
                    }
                    break;

                default:
                    break;
            }
        }
    }

    public class UAcceptWaitInfoWndView : UIBaseWndView, ITimerHanlder
    {
        public Text Title;
        public Text AcceptDes;
        public Text LeftSecDes;
        public Text WaitOtherSec;

        public Button AcceptBtn;
        public Button GuideAcceptBtn;
        public Button CloseBtn;

        private bool m_bConfirm;
        private long m_lEndTicks;
        private int m_nMatchType;
        private uint m_dwCheckID;
        private int? m_ModalKey;
		private int ACCEPT_WAIT_SHOW_SOUNDID = 437;

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "Title");
            AcceptDes.text = ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "AcceptDes");
            
            if (GuideAcceptBtn != null)
            {
                GuideAcceptBtn.gameObject.SetActive(false);
            }
            
            return base.Init(wnd);
        }

        public void OpenModalWnd()
        {
            CloseModalWnd();
            m_ModalKey = ModalHelper.Open(this, null, null, null, this.transform as RectTransform);
            SetVisible(true);
        }

        public void CloseModalWnd()
        {
            if (m_ModalKey != null)
            {
                ModalHelper.Close((int)m_ModalKey);
                m_ModalKey = null;
            }
            SetVisible(false);
            
        }
        public void StartTimer(int nTimerID, int nLeftTime, uint nCheckID, int nMatchType)
        {

            if (nMatchType == (int)EMMatchType.MatchType_NewPlayerGuide2)
            {
                GuideMatchTypeShow();
            }
            else
            {
                NormalMatchShow();
            }
            m_nMatchType = nMatchType;
            m_dwCheckID = nCheckID;
            m_lEndTicks = DateTime.Now.AddSeconds((double)nLeftTime).Ticks;
            m_bConfirm = false;

            string strDes = String.Format(ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "LeftTimeDesc"), nLeftTime);
            UBB.toHtml(ref strDes);
            LeftSecDes.text = strDes;
            LeftSecDes.gameObject.SetActive(true);

            strDes = String.Format(ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "WaitTimeDesc"), nLeftTime);
            UBB.toHtml(ref strDes);
            WaitOtherSec.text = strDes;
            WaitOtherSec.gameObject.SetActive(false);
            CloseBtn.gameObject.SetActive(nMatchType != (int)EMMatchType.MatchType_NewPlayerGuide2);
            TimerManager.SetTimer(this, nTimerID, 1.0f);
			SoundManager.CreateUISound(ACCEPT_WAIT_SHOW_SOUNDID);
        }

        public void KillTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, nTimerID);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer:
                    {
                        long leftTick = m_lEndTicks - DateTime.Now.Ticks;
                        int nCurTime = (int)leftTick / 10000000;

                        if (nCurTime <= 0 && !m_bConfirm)
                        {
                            OnConfirmButtonClick();
                            return;
                        }

                        // 倒计时完毕了这么久
                        // todo 长时间没有反应 直接关闭这个界面 查找问题
                        if (nCurTime <= -2)
                        {
                            Trace.LogError("Match error m_dwCheckID =" + m_dwCheckID);
                            KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
                            CloseModalWnd();
                            return;
                        }

                        string strDes = String.Format(ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "LeftTimeDesc"), nCurTime);
                        UBB.toHtml(ref strDes);
                        LeftSecDes.text = strDes;


                        strDes = String.Format(ULocalizationService.Instance.Get("UIView", "AcceptWaitInfo", "WaitTimeDesc"), nCurTime);
                        UBB.toHtml(ref strDes);
                        WaitOtherSec.text = strDes;
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnConfirmButtonClick()
        {
            LogicDataCenter.matchDataManager.OnMatchConfirmCheck((int)m_dwCheckID);
            LeftSecDes.gameObject.SetActive(false);
            WaitOtherSec.gameObject.SetActive(true);
            CloseBtn.gameObject.SetActive(false);
            if (AcceptBtn != null)
            {
                AcceptBtn.gameObject.SetActive(false);
            }

            if (GuideAcceptBtn != null)
            {
                GuideAcceptBtn.gameObject.SetActive(false);
            }
            m_bConfirm = true;
        }

        public void OnRefuseButtonClick()
        {
            LogicDataCenter.matchDataManager.OnRefuseMatch((int)m_dwCheckID);
            KillTimer((int)EN_AcceptWait_Timer.ENMT_AcceptWaitTimer);
            CloseModalWnd();
        }

        #region 新手引导部分

        internal void GuideMatchTypeShow()
        {
            if (AcceptBtn != null)
            {
                AcceptBtn.gameObject.SetActive(false);
            }

            if (GuideAcceptBtn != null)
            {
                GuideAcceptBtn.gameObject.SetActive(true);
            }

            if (CloseBtn != null)
            {
                CloseBtn.gameObject.SetActive(false);
            }
            
        }

        internal void NormalMatchShow()
        {
            if (AcceptBtn != null)
            {
                AcceptBtn.gameObject.SetActive(true);
            }

            if (GuideAcceptBtn != null)
            {
                GuideAcceptBtn.gameObject.SetActive(false);
            }

            if (CloseBtn != null)
            {
                CloseBtn.gameObject.SetActive(true);
            }
        }
        #endregion
    }
}
