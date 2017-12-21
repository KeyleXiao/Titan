using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using USpeedUI.UWidgets;
using Mentorship_ManagerDef;
using ASpeedGame.Data.MentorshipConfig;

namespace USpeedUI.Social
{
    //子页面分类
    public enum MentorshipSubWndType
    {
        WND_MENTORSHIP_PERSON,
        WND_MENTORSHIP_MASTERISSUE,
        WND_MENTORSHIP_PRENTICEISSUE,
    }
    //button组分类
    public enum MentorshipFrameBtnType
    {
        BTN_PERSON,
        BTN_MASTER,
        BTN_PRENTICE,
    }
    public class MentorshipWnd : UIBaseSubWnd<MentorshipWndView>
    {
        private MentorshipWndView view { get { return m_wndView as MentorshipWndView; } }
        //子窗口列表
        private List<IUISubWnd> m_SubWndList = new List<IUISubWnd>();
        //当前窗口类型
        private MentorshipSubWndType m_CurrentWndType;
        //当前按钮类型
        private MentorshipFrameBtnType m_CurrentPageType;

        public override int GetWndType()
        {
            return (int)SocialSubWndType.WND_MENTORSHIP;
        }

        public override string GetPrefabPath()
        {
            return "Social/UViewMentorship";
        }

        public override bool Init()
        {
            //添加子窗口
            m_SubWndList.Add(new MentorshipPersonWnd());
            m_SubWndList.Add(new MentorshipMasterListWnd());
            m_SubWndList.Add(new MentorshipPrenticeListWnd());
            //子窗口初始化
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.Init();
            }
            
            return base.Init();
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
                SwitchWndPage(MentorshipFrameBtnType.BTN_PERSON);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE:
                    {
                        IUISubWnd personWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PERSON);
                        if (personWnd != null)
                            personWnd.OnMessage(msgID, data);

                        IUISubWnd masterIssueWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_MASTERISSUE);
                        if (masterIssueWnd != null)
                            masterIssueWnd.OnMessage(msgID, data);

                        IUISubWnd prenticeIssueWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PRENTICEISSUE);
                        if (prenticeIssueWnd != null)
                            prenticeIssueWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST:
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST:
                    {
                        IUISubWnd personWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PERSON);
                        if (personWnd != null)
                            personWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_MASTERISSUELIST:
                    {
                        IUISubWnd masterIssueWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_MASTERISSUE);
                        if (masterIssueWnd != null)
                            masterIssueWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST:
                    {
                        IUISubWnd prenticeIssueWnd = GetSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PRENTICEISSUE);
                        if (prenticeIssueWnd != null)
                            prenticeIssueWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE:
                    {
                        if (data != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.SetButtonSelect(cmdData.nParam);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private IUISubWnd GetSubWnd(MentorshipSubWndType wndType)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    return wnd;
            }
            return null;
        }

        private void ShowSubWnd(MentorshipSubWndType wndType)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    wnd.SetVisible(true);
                else
                    wnd.SetVisible(false);
            }
            m_CurrentWndType = wndType;
        }

        public void SwitchWndPage(MentorshipFrameBtnType pageType)
        {
            if (m_wndView == null)
                return;
            switch (pageType)
            {
                case MentorshipFrameBtnType.BTN_PERSON:
                    ShowSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PERSON);
                    break;
                case MentorshipFrameBtnType.BTN_MASTER:
                    ShowSubWnd(MentorshipSubWndType.WND_MENTORSHIP_MASTERISSUE);
                    break;
                case MentorshipFrameBtnType.BTN_PRENTICE:
                    ShowSubWnd(MentorshipSubWndType.WND_MENTORSHIP_PRENTICEISSUE);
                    break;
                default:
                    break;
            }
            m_CurrentPageType = pageType;
            m_wndView.SetButtonSelect((int)m_CurrentPageType);
        }

        public void InitSubWndContainer(Transform container)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.SetContainer(container);
            }
        }
    }

    public class MentorshipWndView : UIBaseSubWndView
    {
        //子窗口容器
        public Transform contentFrame;
        //按钮组
        public UButtonGroup mentorshipBtnGroup;

        public Text personBtnText;
        public Text masterIssueBtnText;
        public Text prenticeIssueBtnText;

        public Text masterLevelTitleText;

        private MentorshipWnd mentorshipWnd
        {
            get
            {
                return m_wnd as MentorshipWnd;
            }
        }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            if (mentorshipWnd != null)
                mentorshipWnd.InitSubWndContainer(contentFrame);

            personBtnText.text = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "PersonBtnDes");
            masterIssueBtnText.text = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "MasterIssueBtnDes");
            prenticeIssueBtnText.text = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "PrenticeIssueBtnDes");

            masterLevelTitleText.text = String.Format(ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "MasterLevelTitleDes"), MentorshipConfig.Instance.tMentorshipConfig.nMasterNeedLevel);

            return true;
        }

        public void SetButtonSelect(int nIndex)
        {
            mentorshipBtnGroup.SetItemSelectedItem(nIndex, true);
        }

        public void OnPersonBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mentorshipWnd.SwitchWndPage(MentorshipFrameBtnType.BTN_PERSON);
        }

        public void OnMasterIssueBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mentorshipWnd.SwitchWndPage(MentorshipFrameBtnType.BTN_MASTER);
        }

        public void OnPrenticeIssueBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            mentorshipWnd.SwitchWndPage(MentorshipFrameBtnType.BTN_PRENTICE);
        }

    }
}
