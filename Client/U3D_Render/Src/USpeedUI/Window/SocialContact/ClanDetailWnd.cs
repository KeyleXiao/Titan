using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using USpeedUI.UWidgets;
using USpeedUI.ClanMember;
using USpeedUI.ClanTask;
using USpeedUI.ClanDonate;
using USpeedUI.ClanEvent;
using USpeedUI.ClanTip;
using DG.Tweening;
using UIWidgets;
using LegendCup_ManagedDef;
using ASpeedGame.Data.ShieldWorld;
using U3D_Render.Common;
using Clan_ManagerDef;
using USpeedUI.TooltipAndDialog;

namespace USpeedUI.Social
{
    // 帮会按钮标签类型
    public enum ClanListFrameButtonType
    {
        BTN_CLAN_MEMBER,        // 联盟成员按钮
        BTN_CLAN_DONATE,        // 联盟捐赠按钮
        BTN_CLAN_EVENT,         // 联盟事件按钮
        BTN_CLAN_TIP,           // 联盟说明按钮
        BTN_CLAN_TASK,          // 联盟任务按钮
    }

    // 帮会窗口标签类型
    public enum ClanListFrameWndType
    {
        WND_CLAN_MEMBER,        // 联盟成员窗口
        WND_CLAN_TASK,          // 联盟任务窗口
        WND_CLAN_DONATE,        // 联盟捐赠窗口
        WND_CLAN_EVENT,         // 联盟事件窗口
        WND_CLAN_TIP,           // 联盟说明窗口
    }

    public class ClanDetailWnd : UIBaseSubWnd<ClanDetailWndView>
    {
        private ClanDetailWndView view { get { return m_wndView as ClanDetailWndView; } }

        private List<IUISubWnd> m_subWndList = new List<IUISubWnd>();
        private ClanListFrameWndType m_curShowWndType;

        // 获取ID
        public override int GetWndType()
        {
            return (int)SocialSubWndType.WND_CLANDETAIL;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanDetail";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 同级子窗体（注： 非弹出窗体）
            m_subWndList.Add(new ClanMemberWnd());
            m_subWndList.Add(new ClanTaskWnd());
            m_subWndList.Add(new ClanDonateWnd());
            m_subWndList.Add(new ClanEventWnd());
            m_subWndList.Add(new ClanTipWnd());

            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                m_subWndList[i].Init();
            }

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            if (visible)
            {
                if (EntityFactory.MainHeroView == null)
                    return;

                int nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                if (nClanID == 0)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_OPENWND_NO_CLAN);
                    return;
                }

                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_REQUESTCLANBASEINFO);
                //// 为后续联盟杯赛操作做准备
                //LogicDataCenter.legendCupDataManager.LegendCupListType = (int)ELegendCupListType.ELCLT_Clan;
                //ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST);
            }

            base.SetVisible(visible);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_UPDATEMEMBER:
                    {
                        IUISubWnd clanMemberWnd = GetSubWnd(ClanListFrameWndType.WND_CLAN_MEMBER);
                        if (clanMemberWnd != null)
                            clanMemberWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateRequestcardTip();
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_BASEDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdateClanBaseData();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateAcceptMemberTip();
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_REQUESTCARD:
                    {
                        if (m_wndView != null)
                            m_wndView.OnGroupClanDonateBtnClick(true);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST:
                case WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO:
                case WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST:
                case WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG:
                    {
                        IUISubWnd clanDonateWnd = GetSubWnd(ClanListFrameWndType.WND_CLAN_DONATE);
                        if (clanDonateWnd != null)
                            clanDonateWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST:
                    {
                        IUISubWnd clanEventWnd = GetSubWnd(ClanListFrameWndType.WND_CLAN_EVENT);
                        if (clanEventWnd != null)
                            clanEventWnd.OnMessage(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_STATE_NOTICE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClanStateChange();
                    }
                    break;
                default:
                    break;
            }
        }

        private IUISubWnd GetSubWnd(ClanListFrameWndType wndType)
        {
            for (int i = 0; i < m_subWndList.Count; ++i)
            {
                if (m_subWndList[i].GetWndType() == (int)wndType)
                    return m_subWndList[i];
            }

            return null;
        }

        private void ShowSubWnd(ClanListFrameWndType wndType)
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

        public void SwitchButtonPage(ClanListFrameButtonType pageType)
        {
            if (EntityFactory.MainHeroView == null)
                return;

            switch (pageType)
            {
                case ClanListFrameButtonType.BTN_CLAN_MEMBER:
                    {
                        ShowSubWnd(ClanListFrameWndType.WND_CLAN_MEMBER);
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_TASK:
                    {
                        ShowSubWnd(ClanListFrameWndType.WND_CLAN_TASK);
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_DONATE:
                    {
                        ShowSubWnd(ClanListFrameWndType.WND_CLAN_DONATE);
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_EVENT:
                    {
                        ShowSubWnd(ClanListFrameWndType.WND_CLAN_EVENT);
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_TIP:
                    {
                        ShowSubWnd(ClanListFrameWndType.WND_CLAN_TIP);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class ClanDetailWndView : UIBaseSubWndView, ITimerHanlder
    {
        private enum TimerID
        {
            DISMISS_COUNTDOWN_HOUR,
            DISMISS_COUNTDOWN_MIN,
            DISMISS_COUNTDOWN_SEC,
        }

        private enum TimeSpan
        {
            DAYS,
            HOURS,
            MINS,
            SECS,
        }

        public UButtonGroup buttonGroup;

        public Text acceptMemberBtnText;
        public Text giveCardBtnText;
        public Text dismissClanBtnText;
        public Text quitClanBtnText;
        public Text clanListBtnText;
        public Text legendCupBtnText;
        public UTooltipTrigger legendCupBtnTip;
        public Image legendCupBtnEffect;
        private Sequence legendCupBtnSequence;
        public Text CreateCupText;

        public Text clanInfoTitleText;
        public Text clanInfoName;
        public Text clanInfoLeader;
        public Text clanInfoTotalFight;
        public Text clanInfoRank;
        public Text clanInfoMemberCount;
        public Text clanInfoKinCount;
        public Text clanInfoWeekActivity;
        public Text dismissCountdown;

        public Text selfInfoTitleText;
        public Text selfInfoJob;
        public Text selfInfoGaveThisWeek;
        public Text selfInfoGaveTotal;

        public Text clanNoticeTitleText;
        public InputField NoticeInputField;
        public Button NoticeEditBtn;
        public Button NoticeSaveBtn;

        public Button dismissClanBtn;
        public Button acceptMemberBtn;
        public Button quitClanBtn;

        public URedPointTip m_giveCardTip;
        public URedPointTip m_acceptMemberTip;

        public Transform confineFrame;
        private ClanDetailWnd clanDetailWnd { get { return m_wnd as ClanDetailWnd; } }

        private ClanListFrameButtonType m_curButtonIndex;
        private int mRemainTime;

        public override bool Init(IUISubWnd wnd)
        {
            ClanDetailWnd clanDetailWnd = wnd as ClanDetailWnd;
            if (clanDetailWnd != null)
                clanDetailWnd.InitSubWndContainer(confineFrame);

            acceptMemberBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "AcceptMemberBtn");
            giveCardBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "GiveCardBtn");
            quitClanBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "QuitBtn");
            clanListBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanListTitle");
            
            clanInfoTitleText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoTitle");
            clanInfoName.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoName");
            clanInfoLeader.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoLeader");
            clanInfoTotalFight.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoTotalFight");
            clanInfoRank.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoRank");
            clanInfoMemberCount.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoMemberCount");
            clanInfoKinCount.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoKinCount");
            clanInfoWeekActivity.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoWeekActivity");
            clanInfoWeekActivity.GetComponentInParent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoWeekActivityTooltip"));
            selfInfoTitleText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoTitle");
            selfInfoJob.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoJob");
            selfInfoGaveThisWeek.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoGaveThisWeek");
            selfInfoGaveTotal.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoGaveTotal");
            clanNoticeTitleText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanNoticeTitle");
            dismissCountdown.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "ClanDetail", "DismissCountdownTip"));
            //legendCupBtnText.text = "联盟杯赛";
            String CreateCupDesc = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CreateLegendCupDes");
            UBB.toHtml(ref CreateCupDesc, UBB_FORMAT_TYPE.UGUI);
            CreateCupText.text = CreateCupDesc;
            CreateCupText.gameObject.SetActive(false);

            NoticeInputField.enabled = false;
            NoticeEditBtn.gameObject.SetActive(false);
            NoticeSaveBtn.gameObject.SetActive(false);

            m_curButtonIndex = ClanListFrameButtonType.BTN_CLAN_MEMBER;

            SetTailFrameButtonVisible(m_curButtonIndex);

            return base.Init(wnd);
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                m_curButtonIndex = ClanListFrameButtonType.BTN_CLAN_MEMBER;
                SetButtonSelect(m_curButtonIndex);
            }
            else
            {
                ClearLegendCupBtnEffect();
            }

            OnUpdateRequestcardTip();
        }

        public void OnUpdateClanBaseData()
        {
            cmd_clan_basedata cmdBasedata = LogicDataCenter.clanDataManager.ClanBaseData;
            clanInfoName.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoName"), cmdBasedata.szClanName);
            clanInfoLeader.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoLeader"), cmdBasedata.szShaikhName);
            clanInfoTotalFight.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoTotalFight"), cmdBasedata.nTotalFight);
            clanInfoRank.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoRank"), cmdBasedata.nPlace);
            clanInfoWeekActivity.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoWeekActivity"), cmdBasedata.nWeekActivity);

            NoticeInputField.placeholder.GetComponent<Text>().text = cmdBasedata.szNotice;

            int nOnlineCount = 0;
            List<cmd_clan_memberlist> clanMemberList = LogicDataCenter.clanDataManager.ClanMemberData;
            for (int i = 0; i < clanMemberList.Count; ++i)
            {
                if (clanMemberList[i].nIsOnline > 0)
                    ++nOnlineCount;
            }
            clanInfoMemberCount.text = String.Format("{0}<color=#ffffff>{1}/{2}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoMemberCount"), nOnlineCount, cmdBasedata.nMemberCount);
            clanInfoKinCount.text = String.Format("{0}<color=#ffffff>{1}/{2}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoKinCount"), cmdBasedata.nKinCount, cmdBasedata.nMaxKinCount);
            clanInfoWeekActivity.text = String.Format("{0}<color=#ffffff>{1}</color>",
                ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanInfoWeekActivity"), cmdBasedata.nWeekActivity);

            if (EntityFactory.MainHeroView != null)
            {
                int selfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                cmd_clan_memberlist selfData = clanMemberList.Find(x => x.nPDBID == selfID);
                if (selfData.nPDBID == selfID)
                {
                    selfInfoJob.text = String.Format("{0}<color=#ffffff>{1}</color>",
                        ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoJob"), LogicDataCenter.clanDataManager.GetClanIdentityName(selfData.nIdentity));
                    selfInfoGaveThisWeek.text = String.Format("{0}<color=#ffffff>{1}</color>",
                        ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoGaveThisWeek"), selfData.nDonateThisWeek);
                    selfInfoGaveTotal.text = String.Format("{0}<color=#ffffff>{1}</color>",
                        ULocalizationService.Instance.Get("UIView", "ClanDetail", "SelfInfoGaveTotal"), selfData.nDonateTotal);
                }

                if (selfID == cmdBasedata.nShaikhID)
                {
                    NoticeEditBtn.gameObject.SetActive(true);
                    NoticeSaveBtn.gameObject.SetActive(false);
                }
                else
                {
                    NoticeEditBtn.gameObject.SetActive(false);
                    NoticeSaveBtn.gameObject.SetActive(false);
                }
            }

            // 杯赛按钮设置
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            string strTip = "";
            DateTime serverTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(cmdBasedata.nServerTime);
            int nNeedDay = System.DateTime.DaysInMonth(serverTime.Year, serverTime.Month) - serverTime.Day + 1;
            bool bCanFreePublic = GameLogicAPI.clanCanPublicLegendCup();
            CreateCupText.gameObject.SetActive(bCanFreePublic);
            if (cmdBasedata.lLegendCupID > 0)               // 比赛中
            {
                legendCupBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupProcessing");
                strTip = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupProcessingTip");
                ClearLegendCupBtnEffect();
            }
            else
            {
                if (nSelfID == cmdBasedata.nShaikhID)
                {
                    if (cmdBasedata.nLegendCupCount > 0)    // 免费发布
                    {
                        legendCupBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupFreePublic");
                        strTip = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupFreePublicTip");
                        CreateLegendCupBtnEffect();
                    }
                    else                                    // 付费发布
                    {
                        legendCupBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupChargePublic");
                        strTip = String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupChargePublicTip"), nNeedDay);
                        ClearLegendCupBtnEffect();
                    }
                }
                else                                        // 等待发布
                {
                    legendCupBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupWaitPublic");
                    strTip = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CupWaitPublicTip");
                    ClearLegendCupBtnEffect();
                }
            }
            legendCupBtnTip.SetText(UTooltipParamName.BodyText, strTip);

            // 设置联盟解散提示
            SetDismissCountdownVisible();
        }

        public void OnUpdateRequestcardTip()
        {
            int nTipCount = LogicDataCenter.clanDataManager.LeftRequestNum;
            int nCooldownTime = LogicDataCenter.clanDataManager.LeftRequestCooldownTime;

            if (m_curButtonIndex != ClanListFrameButtonType.BTN_CLAN_DONATE)
            {
                m_giveCardTip.SetTipCount(LogicDataCenter.clanDataManager.RequestcardTipCount);
            }
            else
            {
                m_giveCardTip.SetTipCount(0);
            }
        }

        public void OnUpdateAcceptMemberTip()
        {
            m_acceptMemberTip.SetTipCount(LogicDataCenter.clanDataManager.ClanApplyMemberData.Count);
        }

        public void OnClanDismissBtnClick()
        {
            if ((EMClanState)LogicDataCenter.clanDataManager.ClanBaseData.nState == EMClanState.emClanState_Dismissing)
            {
                // 取消解散联盟
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_CANCEL_DISMISS);
            }
            else
            {
                string dismissContent = ULocalizationService.Instance.Get("UIView", "ClanDetail", "DissmissContent_1");
                if (LogicDataCenter.clanDataManager.ClanBaseData.nKinCount > 1)
                {
                    int dismissingLife = ASpeedGame.Data.ClanConfig.ClanConfig.Instance.tClanConfig.nDismissingLife;
                    string tips = ObtainDismissTimeTip(dismissingLife);
                    dismissContent += String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "DissmissContent_2"), tips);
                }

                UBB.toHtml(ref dismissContent);
                TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                            title: ULocalizationService.Instance.Get("UIView", "ClanDetail", "DissmissTitle"),
                            message: dismissContent,
                            buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "ClanDetail", "Confirm"), ClanDissmiss }, { ULocalizationService.Instance.Get("UIView", "ClanDetail", "Cancel"), null } }
                            );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }            
        }

        public virtual void OnTimer(int nTimerID)
        {
            if (nTimerID == (int)TimerID.DISMISS_COUNTDOWN_HOUR)
            {
                mRemainTime -= 3600;
                if (mRemainTime <= 3600)
                {
                    if (mRemainTime > 60)
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_MIN, 1.0f * 60.0f, mRemainTime / 60);
                    }
                    else if (mRemainTime > 0)
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_SEC, 1.0f, mRemainTime);
                    }
                }
            }
            else if (nTimerID == (int)TimerID.DISMISS_COUNTDOWN_MIN)
            {
                mRemainTime -= 60;
                if (mRemainTime <= 60 && mRemainTime > 0)
                {
                    TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_SEC, 1.0f, mRemainTime);
                }
            }
            else if (nTimerID == (int)TimerID.DISMISS_COUNTDOWN_SEC)
            {
                mRemainTime--;
            }
            else
            {
                return;
            }

            RefreshDismissCountdownText(mRemainTime);
        }

        private bool ClanDissmiss()
        {
            // 解散帮会
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_DISMISS);
            return true;
        }

        public void OnClanQuitBtnClick()
        {
            UIDialogData data = new UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "Common", "TipText"),
                                    message: ULocalizationService.Instance.Get("UIView", "ClanDetail", "QuitBtnContent"),
                                    buttons: new DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), OnQuitBtnConfirm },
                                                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public bool OnQuitBtnConfirm()
        {
            // 退出帮会
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_QUIT);

            return true;
        }

        public void OnClanAcceptMemberBtnClick()
        {
            // 招收帮会成员界面显示
            UISystem.Instance.ShowWnd(WndID.WND_ID_CLAN_ACCEPTMEMBER, true);
        }

        public void OnClanListBtnClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_SHOWCLANLIST, null);
        }

        public void OnClanLegendCupBtnClick()
        {
            cmd_clan_basedata cmdBasedata = LogicDataCenter.clanDataManager.ClanBaseData;
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (cmdBasedata.lLegendCupID > 0)               // 比赛中
            {
                cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
                cupdata.nLegendCupID = cmdBasedata.lLegendCupID;
                ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
            }
            else
            {
                if (nSelfID == cmdBasedata.nShaikhID)       // 免费发布、付费发布
                {
                    //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CREATETYPE_SHOW, null);
                    // 直接选择联盟杯赛
                    cmd_legendcup_req_create_cup_type cupType = new cmd_legendcup_req_create_cup_type();
                    cupType.byLegendCupType = (byte)ELegendCupType.emCupType_Clan;	// 杯赛创建类型
                    cupType.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                    ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_create_cup_type>(GameLogicDef.GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ, ref cupType);
                }
                else                                        // 等待发布
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CLAN_WAIT_SHAIKH_CREATE);
                }
            }
        }

        public void SetButtonSelect(ClanListFrameButtonType nButtonType)
        {
            SetTailFrameButtonVisible(nButtonType);
            m_curButtonIndex = nButtonType;
            if ((m_curButtonIndex == ClanListFrameButtonType.BTN_CLAN_MEMBER)
                && ((EMClanState)LogicDataCenter.clanDataManager.ClanBaseData.nState == EMClanState.emClanState_Dismissing))
            {
                dismissCountdown.gameObject.SetActive(true);
            }
            else
            {
                dismissCountdown.gameObject.SetActive(false);
            }

            if (buttonGroup != null)
            {
                buttonGroup.SetItemSelectedItem((int)m_curButtonIndex, true);
            }

            switch (nButtonType)
            {
                case ClanListFrameButtonType.BTN_CLAN_MEMBER:
                    {
                        if (clanDetailWnd != null)
                        {
                            clanDetailWnd.SwitchButtonPage(ClanListFrameButtonType.BTN_CLAN_MEMBER);
                        }
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_TASK:
                    {
                        if (clanDetailWnd != null)
                        {
                            clanDetailWnd.SwitchButtonPage(ClanListFrameButtonType.BTN_CLAN_TASK);
                        }
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_DONATE:
                    {
                        if (clanDetailWnd != null)
                        {
                            clanDetailWnd.SwitchButtonPage(ClanListFrameButtonType.BTN_CLAN_DONATE);
                        }
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_EVENT:
                    {
                        if (clanDetailWnd != null)
                        {
                            clanDetailWnd.SwitchButtonPage(ClanListFrameButtonType.BTN_CLAN_EVENT);
                        }
                    }
                    break;
                case ClanListFrameButtonType.BTN_CLAN_TIP:
                    {
                        if (clanDetailWnd != null)
                        {
                            clanDetailWnd.SwitchButtonPage(ClanListFrameButtonType.BTN_CLAN_TIP);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnGroupClanMemberBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SetButtonSelect(ClanListFrameButtonType.BTN_CLAN_MEMBER);
        }

        public void OnGroupClanTaskBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SetButtonSelect(ClanListFrameButtonType.BTN_CLAN_TASK);
        }

        public void OnGroupClanDonateBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SetButtonSelect(ClanListFrameButtonType.BTN_CLAN_DONATE);
        }

        public void OnGroupClanEventBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SetButtonSelect(ClanListFrameButtonType.BTN_CLAN_EVENT);

            // 请求帮会事件
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_EVENT_LIST);
        }

        public void OnGroupClanTipBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SetButtonSelect(ClanListFrameButtonType.BTN_CLAN_TIP);
        }

        public void OnNoticeEditBtnClick()
        {
            // 编辑公告点击
            NoticeInputField.enabled = true;

            // 显示编辑按钮
            NoticeSaveBtn.gameObject.SetActive(true);
            NoticeEditBtn.gameObject.SetActive(false);
        }

        public void OnNoticeSaveBtnClick()
        {
            // 请求保存
            GameLogic.cmd_clan_modify_notice cmdData = new GameLogic.cmd_clan_modify_notice();

            if (NoticeInputField.textComponent.text == string.Empty)
                cmdData.szText = NoticeInputField.placeholder.GetComponent<Text>().text;
            else
                cmdData.szText = NoticeInputField.textComponent.text;

            // 处理屏蔽词
            if (ShieldWord.Instance.IsHaveShieldWord(cmdData.szText))
            {
                NoticeInputField.placeholder.GetComponent<Text>().text = "";
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_NOTICE_SHIELD_WORD);
                return;
            }

            // 编辑公告点击
            NoticeInputField.enabled = false;

            // 显示编辑按钮
            NoticeEditBtn.gameObject.SetActive(true);
            NoticeSaveBtn.gameObject.SetActive(false);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_MODIFY_NOTICE, ref cmdData);
        }

        public void OnClanStateChange()
        {
            if (m_curButtonIndex != ClanListFrameButtonType.BTN_CLAN_MEMBER)
            {
                dismissCountdown.gameObject.SetActive(false);
            }
            else
            {
                EMClanState state = (EMClanState)LogicDataCenter.clanDataManager.ClanBaseData.nState;
                if (state == EMClanState.emClanState_Dismissing)
                {
                    StartDismissCountdown(ASpeedGame.Data.ClanConfig.ClanConfig.Instance.tClanConfig.nDismissingLife);
                    dismissCountdown.gameObject.SetActive(true);
                    dismissClanBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CancelDismissBtn");
                }
                else
                {
                    dismissCountdown.gameObject.SetActive(false);
                    dismissClanBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "DismissBtn");
                }
            }
        }

        private void SetDismissCountdownVisible()
        {
            if (m_curButtonIndex != ClanListFrameButtonType.BTN_CLAN_MEMBER)
            {
                dismissCountdown.gameObject.SetActive(false);
            }
            else
            {
                EMClanState state = (EMClanState)LogicDataCenter.clanDataManager.ClanBaseData.nState;
                if (state == EMClanState.emClanState_Dismissing)
                {
                    int lifeTime = ASpeedGame.Data.ClanConfig.ClanConfig.Instance.tClanConfig.nDismissingLife;
                    int currentTime = LogicDataCenter.clanDataManager.ClanBaseData.nServerTime;
                    int dismissTime = LogicDataCenter.clanDataManager.ClanBaseData.nDismissApplyTime;
                    if (dismissTime == 0)
                    {
                        dismissTime = currentTime;
                    }

                    int reaminingTime = lifeTime - (currentTime - dismissTime);
                    StartDismissCountdown(reaminingTime);
                    dismissCountdown.gameObject.SetActive(true);
                    dismissClanBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "CancelDismissBtn");
                }
                else
                {
                    dismissCountdown.gameObject.SetActive(false);
                    dismissClanBtnText.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "DismissBtn");
                }
            }
        }

        private string ObtainDismissTimeTip(int remainTime)
        {
            string countdownTip = "";
            switch (TransformTimeIntoTimeSpan(remainTime))
            {
                case TimeSpan.DAYS:
                    {
                        int days = remainTime / (3600 * 24);
                        int hours = (remainTime - days * 3600 * 24) / 3600;
                        countdownTip = String.Format("{0}天{1}小时", days, hours);
                    }
                    break;
                case TimeSpan.HOURS:
                    {
                        int hours = remainTime / 3600;
                        int mins = (remainTime - hours * 3600) / 60;
                        countdownTip = String.Format("{0}小时{1}分钟", hours, mins);
                    }
                    break;
                case TimeSpan.MINS:
                    {
                        int mins = remainTime / 60;
                        countdownTip = String.Format("{0}分钟", mins);
                    }
                    break;
                case TimeSpan.SECS:
                    {
                        countdownTip = String.Format("{0}秒", remainTime);
                    }
                    break;
                default:
                    break;
            }

            return countdownTip;
        }


        private void StartDismissCountdown(int remainTime)
        {
            mRemainTime = remainTime;
            string countdownTip = ObtainDismissTimeTip(remainTime);
            countdownTip = String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "DismissCountdown")) + countdownTip;
            dismissCountdown.text = countdownTip;
            switch (TransformTimeIntoTimeSpan(remainTime))
            {
                case TimeSpan.DAYS:
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_HOUR, 1.0f * 3600.0f, remainTime / 3600);
                    }
                    break;
                case TimeSpan.HOURS:
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_MIN, 1.0f * 60.0f, remainTime / 60);
                    }
                    break;
                case TimeSpan.MINS:
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_MIN, 1.0f * 60.0f, remainTime / 60);
                    }
                    break;
                case TimeSpan.SECS:
                    {
                        TimerManager.SetTimer(this, (int)TimerID.DISMISS_COUNTDOWN_SEC, 1.0f, remainTime);
                    }
                    break;
                default:
                    break;
            }
        }

        private void RefreshDismissCountdownText(int remainTime)
        {
            string countdownTip = ObtainDismissTimeTip(remainTime);
            countdownTip = String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "DismissCountdown")) + countdownTip;
            dismissCountdown.text = countdownTip;
        }

        private TimeSpan TransformTimeIntoTimeSpan(int time)
        {
            if (time >= 3600 * 24)
            {
                return TimeSpan.DAYS;
            }

            if (time >= 3600)
            {
                return TimeSpan.HOURS;
            }
            
            if (time >= 60)
            {
                return TimeSpan.MINS;
            }

            return TimeSpan.SECS;
        }

        private void CreateLegendCupBtnEffect()
        {
            if (legendCupBtnSequence == null)
            {
                Color effectBaseColor = Color.white;
                effectBaseColor.a = 0f;
                legendCupBtnEffect.color = effectBaseColor;
                legendCupBtnSequence = DOTween.Sequence();
                legendCupBtnSequence.Append(DOTween.ToAlpha(() => legendCupBtnEffect.color, x => legendCupBtnEffect.color = x, 1f, 0.5f));
                legendCupBtnSequence.Append(DOTween.ToAlpha(() => legendCupBtnEffect.color, x => legendCupBtnEffect.color = x, 0f, 0.5f).SetDelay(0.2f));
                legendCupBtnSequence.SetLoops(-1, LoopType.Restart);
                legendCupBtnSequence.OnKill(() => legendCupBtnSequence = null);
            }
        }

        private void ClearLegendCupBtnEffect()
        {
            legendCupBtnEffect.color = Color.clear;
            if (legendCupBtnSequence != null)
                legendCupBtnSequence.Kill(true);
        }

        private void SetTailFrameButtonVisible(ClanListFrameButtonType frameButton)
        {
            cmd_clan_basedata cmdBasedata = LogicDataCenter.clanDataManager.ClanBaseData;
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            switch (frameButton)
            {
                case ClanListFrameButtonType.BTN_CLAN_MEMBER:
                    if (nSelfID == LogicDataCenter.clanDataManager.ClanBaseData.nShaikhID)
                    {
                        dismissClanBtn.gameObject.SetActive(true);
                        acceptMemberBtn.gameObject.SetActive(true);
                    }
                    else
                    {
                        dismissClanBtn.gameObject.SetActive(false);
                        acceptMemberBtn.gameObject.SetActive(false);
                    }
                    quitClanBtn.gameObject.SetActive(true);
                    break;
                case ClanListFrameButtonType.BTN_CLAN_DONATE:
                case ClanListFrameButtonType.BTN_CLAN_EVENT:
                case ClanListFrameButtonType.BTN_CLAN_TIP:
                    dismissClanBtn.gameObject.SetActive(false);
                    acceptMemberBtn.gameObject.SetActive(false);
                    quitClanBtn.gameObject.SetActive(false);
                    break;
                default:
                    break;
            }
        }
    }
}
