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

namespace USpeedUI.Social
{
    // 社会子窗口类型
    public enum SocialSubWndType
    {
        WND_KIN,
        WND_KINMEMBER,
        WND_CLANLIST,
        WND_CLANDETAIL,
        WND_MARRY,
        WND_KINGDOMWAR,
        WND_MENTORSHIP,
    }

    // 社会按钮标签类型
    public enum SocialFrameButtonType
    {
        BTN_CLAN,
        BTN_KIN,
        BTN_MENTORSHIP,
        BTN_MARRY,
        BTN_KINGDOMWAR,
    }

    public enum SocialTimerID
    {
        TimerID_RequestClanList,
    }

    public class SocialWnd : UIPopupWnd<SocialWndView>, ITimerHanlder
    {
        private SocialWndView view { get { return m_wndView as SocialWndView; } }

        private List<IUISubWnd> m_subWndList = new List<IUISubWnd>();
        private SocialSubWndType m_curShowWndType;
        private SocialFrameButtonType m_curShowPageType;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_SOCIALCONTACT;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/SocialFrame";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 同级子窗体（注： 非弹出窗体）
            m_subWndList.Add(new KinWnd());
            m_subWndList.Add(new KinMemberWnd());
            m_subWndList.Add(new ClanListWnd());
            m_subWndList.Add(new ClanDetailWnd());
            m_subWndList.Add(new MentorshipWnd());

            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                m_subWndList[i].Init();
            }
            
            SetVisible(false);

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SOCIALTOPSIDEBUTTON_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, this);
            // 订阅进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            // KinWnd 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_SHOWKINLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_REQUESTKINLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEKINLIST, this);
            // KinMemberWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_KINBASEDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_MEMBERLIST, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATSELF, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_STRING_PROP_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_NUM_PROP_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEMEMBER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RECV_EVENTLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RECV_GLORYDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_CREATESUCCESS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RETURN_TO_SELFKIN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_JOIN_SUCCESS, this);
            
            
            

            // ClanListWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_SHOWCLANLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCLANLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_CREATESUCCEED, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLYRET, this);
            // ClanDetailWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_BASEDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCARD, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_STATE_NOTICE, this);
            // ClanDetailWnd->ClanDonateWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG, this);
            // ClanDetailWnd->ClanEventWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST, this);
            // MentorshipWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_MASTERISSUELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, this);

            // 设置请求帮会列表定时器
            TimerManager.SetTimer(this, (int)SocialTimerID.TimerID_RequestClanList, 1.0f);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SOCIALTOPSIDEBUTTON_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, this);
            // 退订进入战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            // KinWnd  
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_SHOWKINLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_REQUESTKINLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEKINLIST, this);
            // KinMemberWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_KINBASEDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_MEMBERLIST, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATSELF, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_STRING_PROP_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_NUM_PROP_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEMEMBER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RECV_EVENTLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RECV_GLORYDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_CREATESUCCESS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RETURN_TO_SELFKIN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_JOIN_SUCCESS, this);
            
            

            // ClanListWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_SHOWCLANLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCLANLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_CREATESUCCEED, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLYRET, this);
            
            // ClanDetailWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_BASEDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCARD, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_STATE_NOTICE, this);
            // ClanDetailWnd->ClanDonateWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG, this);
            // ClanDetailWnd->ClanEventWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST, this);
            // MentorshipWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_MASTERISSUELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, this);

            // 杀掉请求帮会列表定时器
            TimerManager.KillTimer(this, (int)SocialTimerID.TimerID_RequestClanList);
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

                case WndMsgID.WND_MSG_SOCIALCONTACT_OPEN:
                    {
                        // 隐藏其他同级窗体
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, null);
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);

                        SetVisible(true);
                        if (m_wndView == null)
                        {
                            Debug.LogWarning("open socialwnd failed.");
                            return;
                        }

                        if (EntityFactory.MainHeroView != null)
                        {
                            if (msgData != null)
                            {
                                UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                                m_wndView.SetButtonSelect(cmdData.nParam);
                            }
                            else
                            {
                                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0)
                                {
                                    // 打开帮会页面
                                    if (m_subWndList[(int)SocialSubWndType.WND_CLANDETAIL] != null && m_subWndList[(int)SocialSubWndType.WND_CLANDETAIL].IsVisible())
                                    {
                                        m_subWndList[(int)SocialSubWndType.WND_CLANDETAIL].SetVisible(true);
                                    }
                                    else
                                    {
                                        m_wndView.SetButtonSelect((int)SocialFrameButtonType.BTN_CLAN);
                                    }
                                }
                                else
                                {
                                    // 打开战队页面
                                    m_wndView.SetButtonSelect((int)SocialFrameButtonType.BTN_KIN);
                                    SwitchWndPage(SocialFrameButtonType.BTN_KIN);
                                }
                            }                            
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_SOCIALTOPSIDEBUTTON_OPEN:
                    {
                        // 隐藏其他同级窗体
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, null);
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);

                        SetVisible(true);

                        UIButtonType ButtonType = (UIButtonType)msgData;
                        m_wndView.SetButtonSelect(ButtonType.nType);
                    }
                    break;
                case WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH:
                    {
                        SwitchWndPage(m_curShowPageType);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                    }
                    break;

                // WND_KIN
                case WndMsgID.WND_MSG_KIN_SHOWKINLIST:
                    {
                        ShowSubWnd(SocialSubWndType.WND_KIN);
                    }
                    break;
                case WndMsgID.WND_MSG_KIN_REQUESTKINLIST:
                case WndMsgID.WND_MSG_KIN_UPDATEKINLIST:
                    {
                        IUISubWnd kinWnd = GetSubWnd(SocialSubWndType.WND_KIN);
                        if (kinWnd != null)
                            kinWnd.OnMessage(msgID, msgData);
                    }
                    break;

                // WND_KINMEMBER
                case WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateKinApllyTip();

                        IUISubWnd kinMemberWnd = GetSubWnd(SocialSubWndType.WND_KINMEMBER);
                        if (kinMemberWnd != null)
                            kinMemberWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_KIN_CREATESUCCESS:
                case WndMsgID.WND_MSG_KIN_JOIN_SUCCESS:
                case WndMsgID.WND_MSG_KIN_RETURN_TO_SELFKIN:
                    {
                        ShowSubWnd(SocialSubWndType.WND_KINMEMBER);
                    }
                    break;
                    
                case WndMsgID.WND_MSG_KIN_RECEV_KINBASEDATA:
                case WndMsgID.WND_MSG_KIN_RECEV_MEMBERLIST:
				case WndMsgID.WND_MSG_KIN_UPDATSELF:
                case WndMsgID.WND_MSG_KIN_STRING_PROP_UPDATE:
                case WndMsgID.WND_MSG_KIN_NUM_PROP_UPDATE:
                case WndMsgID.WND_MSG_KIN_UPDATEMEMBER:
                case WndMsgID.WND_MSG_KIN_RECV_EVENTLIST:
                case WndMsgID.WND_MSG_KIN_RECV_GLORYDATA:
                    {
                        IUISubWnd kinMemberWnd = GetSubWnd(SocialSubWndType.WND_KINMEMBER);
                        if (kinMemberWnd != null)
                        {
                            kinMemberWnd.OnMessage(msgID, msgData);
                        }
                    }
                    break;
                // WND_CLANLIST
                case WndMsgID.WND_MSG_CLAN_SHOWCLANLIST:
                    {
                        ShowSubWnd(SocialSubWndType.WND_CLANLIST);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_REQUESTCLANLIST:
                case WndMsgID.WND_MSG_CLAN_CREATESUCCEED:
                case WndMsgID.WND_MSG_CLAN_APPLYRET:
                    {
                        IUISubWnd clanlistWnd = GetSubWnd(SocialSubWndType.WND_CLANLIST);
                        if (clanlistWnd != null)
                            clanlistWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateClanRedTip();

                        IUISubWnd clanDetailWnd = GetSubWnd(SocialSubWndType.WND_CLANDETAIL);
                        if (clanDetailWnd != null)
                            clanDetailWnd.OnMessage(msgID, msgData);
                    }
                    break;
                // WND_CLANDETAIL
                case WndMsgID.WND_MSG_CLAN_BASEDATA:
                case WndMsgID.WND_MSG_CLAN_UPDATEMEMBER:
                case WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP:
                case WndMsgID.WND_MSG_CLAN_STATE_NOTICE:
                // ClanDetailWnd->ClanDonateWnd
                case WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST:
                case WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO:
                case WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST:
                case WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG:
                // ClanDetailWnd->ClanEventWnd
                case WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST:
                    {
                        IUISubWnd clanDetailWnd = GetSubWnd(SocialSubWndType.WND_CLANDETAIL);
                        if (clanDetailWnd != null)
                            clanDetailWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_REQUESTCARD:
                    {
                        // 隐藏其他同级窗体
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, null);
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);

                        SetVisible(true);

                        if (EntityFactory.MainHeroView != null)
                        {
                            if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0)
                            {
                                // 打开帮会页面
                                m_wndView.SetButtonSelect((int)SocialFrameButtonType.BTN_CLAN);
                            }
                            else
                            {
                                // 打开战队页面
                                m_wndView.SetButtonSelect((int)SocialFrameButtonType.BTN_KIN);
                            }
                        }

                        IUISubWnd clanDetailWnd = GetSubWnd(SocialSubWndType.WND_CLANDETAIL);
                        if (clanDetailWnd != null)
                            clanDetailWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_MASTERISSUELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE:
                    {
                        IUISubWnd mentorshipWnd = GetSubWnd(SocialSubWndType.WND_MENTORSHIP);
                        if (mentorshipWnd != null)
                            mentorshipWnd.OnMessage(msgID, msgData);
                    }
                    break;
                default:
                    break;
            }
        }

        private IUISubWnd GetSubWnd(SocialSubWndType wndType)
        {
            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                if (m_subWndList[i].GetWndType() == (int)wndType)
                    return m_subWndList[i];
            }

            return null;
        }

        private void ShowSubWnd(SocialSubWndType wndType)
        {
            if (m_wndView == null || m_wndView.IsVisible() == false)
                return;

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

        public void SwitchWndPage(SocialFrameButtonType pageType)
        {
            if (m_wndView == null || EntityFactory.MainHeroView == null)
                return;

            switch (pageType)
            {
                case SocialFrameButtonType.BTN_KIN:
                    {
                        if (LogicDataCenter.kinDataManager.IsKinBaseDataValid())
                            ShowSubWnd(SocialSubWndType.WND_KINMEMBER);
                        else
                            ShowSubWnd(SocialSubWndType.WND_KIN);
                    }
                    break;
                case SocialFrameButtonType.BTN_CLAN:
                    {
                        // 通知逻辑层
                        if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) == 0)
                            ShowSubWnd(SocialSubWndType.WND_CLANLIST);
                        else
                            ShowSubWnd(SocialSubWndType.WND_CLANDETAIL);
                    }
                    break;
                case SocialFrameButtonType.BTN_MARRY:
                    {
                        ShowSubWnd(SocialSubWndType.WND_MARRY);
                    }
                    break;
                case SocialFrameButtonType.BTN_KINGDOMWAR:
                    {
                        ShowSubWnd(SocialSubWndType.WND_KINGDOMWAR);
                    }
                    break;
                case SocialFrameButtonType.BTN_MENTORSHIP:
                    {
                        ShowSubWnd(SocialSubWndType.WND_MENTORSHIP);
                    }
                    break;
            }

            m_curShowPageType = pageType;
        }

        public virtual void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)SocialTimerID.TimerID_RequestClanList:
                    {
                        IUISubWnd clanListWnd = GetSubWnd(SocialSubWndType.WND_CLANLIST);
                        if (clanListWnd == null || !clanListWnd.IsVisible())
                        {
                            break;
                        }

                        (clanListWnd as ClanListWnd).OnTimer(nTimerID);
                    }
                    break;
                default:
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
         
            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }

    }

    public class SocialWndView : UIBaseWndView
    {
        public Transform confineFrame;
        public UButtonGroup buttonGroup;
        public URedPointTip ApplyTip;
        public Transform clanTipFrame;

        private URedPointTip clanRedTip;


        private SocialWnd socialWnd { get { return m_wnd as SocialWnd; } }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_UseImageEffect = true;

            SocialWnd socialWnd = wnd as SocialWnd;
            if (socialWnd != null)
                socialWnd.InitSubWndContainer(confineFrame);

            clanRedTip = URedPointTip.CreateRedPointObject(clanTipFrame as RectTransform);

            return true;
        }

        public void OnUpdateClanRedTip()
        {
            if (clanRedTip != null
                && LogicDataCenter.clanDataManager.ClanBaseData.nShaikhID == LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo().playerInfo.pDbid)
            {
                clanRedTip.SetTipCount(LogicDataCenter.clanDataManager.ClanApplyMemberData.Count);
            }
        }

        public void OnUpdateKinApllyTip()
        {
            int nTipCount = LogicDataCenter.kinDataManager.KinBaseData.nApplyCount;
            ApplyTip.SetTipCount(nTipCount);
        }

        public void SetButtonSelect(int nIndex)
        {
            buttonGroup.SetItemSelectedItem(nIndex, true);
        }

        public void OnKinBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            socialWnd.SwitchWndPage(SocialFrameButtonType.BTN_KIN);
        }

        public void OnClanBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            socialWnd.SwitchWndPage(SocialFrameButtonType.BTN_CLAN);
        }

        public void OnMentorshipBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            socialWnd.SwitchWndPage(SocialFrameButtonType.BTN_MENTORSHIP);
        }

        public void OnCloseBtnClick()
        {
            // 关闭按钮点击
            SetVisible(false);

            //通知退出战队列表和战队成员观察者
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SOCIAL_CLOSE);

        }

        public void OnConfirmBtnClick()
        {
            // 确定按钮点击
            UnityEngine.Debug.Log("OnConfirmBtnClick ");
        }

        public void OnMarryBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            socialWnd.SwitchWndPage(SocialFrameButtonType.BTN_MARRY);

            // 确定按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "结婚系统",
                                   message: "结婚系统正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } },
                                   modal: true
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnKindomWarBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            socialWnd.SwitchWndPage(SocialFrameButtonType.BTN_KINGDOMWAR);

            // 确定按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "王城战系统",
                                   message: "王城战系统正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } },
                                   modal: true
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

    }
}
