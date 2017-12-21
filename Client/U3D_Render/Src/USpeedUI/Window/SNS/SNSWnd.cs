/*******************************************************************
** 文件名:	SNSWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/7
** 版  本:	1.0
** 描  述:	SNS系统
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using ASpeedGame.Data.ShieldWorld;
using System.Collections;

namespace USpeedUI.SNS
{
    public class SNSWnd : UIPopupWnd<SNSWndView>
    {
		// 是否初始化了
		private bool m_isInit = false;

		public override WndID GetID()
		{
            return WndID.WND_ID_SNS;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "SNS/SNSView";
        }

        public override bool Init()
        {
			m_isInit = false;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_UPDATEGAMESTATE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_ADD_BUDDY, this);

			UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.OpenFriend, KeyStatus.Down, this);

            return base.Init();
        }
		
		public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_UPDATEGAMESTATE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_ADD_BUDDY, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.OpenFriend, KeyStatus.Down, this);

			m_isInit = false;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
						// 进入登录界面,清理上一局数据
						if(m_wndView != null)
						{
							m_wndView.Clear();
						}

						UnloadView();
						m_isInit = false;
					}
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);

						m_isInit = true;

                        if (m_wndView != null)
                        {
                            // 登陆SNS服务器
                            //m_wndView.loginSNSServer_Delay();
                            m_wndView.onEnterStaticScene();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.onLeaveStaticScene();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                    {
                    }
                    break;

                case WndMsgID.WND_MSG_COMMON_UPDATEGAMESTATE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.onGameStateChange();
                        }
                    }
                    break;
				case WndMsgID.WND_MSG_SNS_ADD_BUDDY:
					{
						if (m_wndView != null)
						{
							m_wndView.addBuddy(data as SNSAddBuddyWndMsgData);
						}
					}
					break;
				default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
			if (!m_isInit)
				return true;

            if (index == FuntionShortCutSlotIndex.OpenFriend && status == KeyStatus.Down)
            {
				SetVisible(true);
				//m_wndView.switchMainPanel();
				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SWITCH_MAINPANEL, null);
			}

            return true;
        }
	}

    public class SNSWndView : UIBaseWndView, ITimerHanlder
    {
        enum TimerID
        {
            DeltectIsLeave = 0,
        }

        // 修改名字（群名字或好友名字）
        public int ChangeType { set; get; }
        public int CurrentID { set; get; }
        public string CurrentName { set; get; }

        private bool m_IsLogin = false;

        private Dictionary<int, SNSOpenInputDialogMsgData> m_InputDialogList;
        
        // 最后一次操作鼠标或键盘的时间
        private DateTime m_LastOptTime;
        // 超时时间
        private TimeSpan m_ExpireTime;
        // 鼠标上一帧的位置
        private Vector3 m_MouseLastFramePos;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_InputDialogList = new Dictionary<int, SNSOpenInputDialogMsgData>();

            m_LastOptTime = DateTime.Now;
            // 超时时间为10分钟
            m_ExpireTime = new TimeSpan(0, 10, 0);
            m_MouseLastFramePos = InputManager.MousePosition;

			LogicDataCenter.snsDataManager.Init();

			return true;
		}

        public override void Start()
        {
            base.Start();
        }

        public override void Update()
        {
            base.Update();

        }


        public override void Destroy()
        {
            TimerManager.KillTimer(this, (int)TimerID.DeltectIsLeave);
        }

        public void Clear()
		{
			LogicDataCenter.snsDataManager.Clear();
			m_IsLogin = false;
		}

        public void loginSNSServer_Delay()
        {
            if (m_IsLogin)
                return;

            StartCoroutine(loginSNSServer());
        }

        private IEnumerator loginSNSServer()
        {
            ActorPlayerInfo selfPlayerInfo = new ActorPlayerInfo();
            while(true)
            {
                selfPlayerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
                if (selfPlayerInfo.playerInfo.pDbid <= 0)
                {
                    yield return new WaitForSeconds(0.5f);
                }
                else
                {
                    break;
                }
            }

            LogicDataCenter.snsDataManager.loginSNSServer(selfPlayerInfo);
            m_IsLogin = true;
        }

  

        public void onGameStateChange()
        {
            int nGameState = LogicDataCenter.playerSystemDataManager.SelfGameState;
            string strReverse = "";

            switch(nGameState)
            {
                case (int)ACTOR_GAME_STATE.GAME_STATE_WAR:
                    {
                        strReverse = EntityFactory.MainHeroView.createinfo.szName;
                    }
                    break;
                default:break;
            }

            // 更新自己的状态
            LogicDataCenter.snsDataManager.reqUpdateBuddyStatus((byte)nGameState, strReverse);
        }

        // 检查玩家是否离开了
        private void detectPlayerLeaveState()
        {
            // 是否操作了键盘或者鼠标
            if (InputManager.AnyKeyDown || (InputManager.MousePosition != m_MouseLastFramePos))
            {
                // 离开后再回来
                if (LogicDataCenter.snsDataManager.IsLeave)
                {
                    LogicDataCenter.playerSystemDataManager.reqUpdateGameStateIsLeave(false);
                }

                // 记录最后一个操作时间
                m_LastOptTime = DateTime.Now;
                m_MouseLastFramePos = InputManager.MousePosition;
            }
            else
            {
                // 判断距离最后一次操作是否超过十分钟
                if ((DateTime.Now - m_LastOptTime) > m_ExpireTime && !LogicDataCenter.snsDataManager.IsLeave)
                {
                    LogicDataCenter.playerSystemDataManager.reqUpdateGameStateIsLeave(true);
                }
            }
        }

		// 添加好友
		public void addBuddy(SNSAddBuddyWndMsgData msgData)
		{
			if (msgData == null)
				return;

			LogicDataCenter.snsDataManager.reqAddBuddy(msgData.nUserID, msgData.actorName);
		}

        public void OnTimer(int nTimerID)
        {
            if(nTimerID == (int)TimerID.DeltectIsLeave)
            {
                detectPlayerLeaveState();
            }
        }

        public void onEnterStaticScene()
        {
            TimerManager.SetTimer(this, (int)TimerID.DeltectIsLeave, 1f);
        }
        public void onLeaveStaticScene()
        {
            TimerManager.KillTimer(this, (int)TimerID.DeltectIsLeave);
        }
    }


}
