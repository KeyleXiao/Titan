/*******************************************************************
** 文件名:	UWaitingRoomDelayJoinWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	显示延迟进入房间倒计时	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;

namespace USpeedUI.WaitingRoomDelayJoin
{
    enum EN_DelayJoin_Timer
    {
        ENMT_DelayTimer = 0,     // 已用匹配时间定时器id
    }

    public class UWaitingRoomDelayJoinWnd : UIBaseWnd<UWaitingRoomDelayJoinView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAITINGROOM_DELAYJOIN;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "GameWaitingRoomView/WaitingRoomDelayJoinView";
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

            //m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAITINGROOM_DELAYJOIN_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_WAITINGROOM_DELAYJOIN_HIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAITINGROOM_DELAYJOIN_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_WAITINGROOM_DELAYJOIN_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {

                case WndMsgID.WND_MSG_WAITINGROOM_DELAYJOIN_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }

                        if (!IsVisible())
                        {
                            UIMsgCmdData Data = (UIMsgCmdData)msgData;
                            m_wndView.setLeftTime(Data.nParam);
                            SetVisible(true);
                        }
                    }
                    break;
                case WndMsgID.WND_WAITINGROOM_DELAYJOIN_HIDE:
                    {
                        if (m_wndView == null)
                            return;
                        UnloadView();
                        //显示房间
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW, null);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, null);
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW:
                    {
                        // 重新显示撮合之后
                        if (m_wndView == null)
                            return;
                        m_wndView.KillTimer();
                        UnloadView();
                        
                    }
                    break;

                default:
                    break;
            }
        }
    
        // 重载ESC
        public override bool OnCloseKey()
        {
            if (m_wndView != null)
                m_wndView.DelayButtonClick();

            return false;
        }
    }

    public class UWaitingRoomDelayJoinView : UIBaseWndView, ITimerHanlder
    {
        public Text Title;
        public Text Des;
        public Text JoinTimeLeft;
        public Button DelayBtn;
        public Button JoinBtn;
        private int curLeftTimeSec;

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "DelayJoinView", "Title");
            Des.text = ULocalizationService.Instance.Get("UIView", "DelayJoinView", "Des");
            Text DelayBtnText = this.GetTransform().FindChild("DelayButton").FindChild("Text").GetComponent<Text>();
            DelayBtnText.text = ULocalizationService.Instance.Get("UIView", "DelayJoinView", "DelayBtnText");
            Text JoinBtnText = this.GetTransform().FindChild("JoinButton").FindChild("Text").GetComponent<Text>();
            JoinBtnText.text = ULocalizationService.Instance.Get("UIView", "DelayJoinView", "JoinBtnText");
            
            return base.Init(wnd);
        }

        public void setLeftTime(int nTime)
        {
            if (nTime <= 0)
            {
                return;
            }
            curLeftTimeSec = nTime;
            JoinTimeLeft.text = curLeftTimeSec.ToString();
            // 开启定时器
            TimerManager.SetTimer(this, (int)EN_DelayJoin_Timer.ENMT_DelayTimer, 1.0f);
            LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom = true;
            
        }

        public void JoinButtonClick()
        {
            JoinRoom();
        }
        public void DelayButtonClick()
        {
            // 暂时挂起
            SetVisible(false);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, null);
        }

        public void KillTimer()
        {
            // 移除定时器
            TimerManager.KillTimer(this, (int)EN_DelayJoin_Timer.ENMT_DelayTimer);
        }

        public void JoinRoom()
        {
            // 倒计时结束
            TimerManager.KillTimer(this, (int)EN_DelayJoin_Timer.ENMT_DelayTimer);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_WAITINGROOM_DELAYJOIN_HIDE, null);
            LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom = false;
        }
        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_DelayJoin_Timer.ENMT_DelayTimer:
                    {
                        curLeftTimeSec--;
                        if (curLeftTimeSec <= 0)
                        {
                            // 倒计时结束
                            JoinRoom();
                            return;
                        }
                        
                        String strSec = curLeftTimeSec.ToString();
                        // 更新匹配时间
                        JoinTimeLeft.text = strSec;                        
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
