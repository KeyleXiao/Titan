using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using GameLogic;
using UnityEngine;
using Clan_ManagerDef;
using USpeedUI.TooltipAndDialog;
using ASpeedGame.Data.ClanConfig;
using Data.SideButtonConfig;
using U3D_Render;
using Kin_ManagerDef;
using LegendCup_ManagedDef;

namespace DataCenter
{
    public class ClanDataManager : ITimerHanlder
    {
        public enum ClanMemberSortType
        {
            Sort_Normal,
            Sort_Name,
            Sort_Job,
            Sort_WeekDonate,
            Sort_Power,
            Sort_Grade,
            Sort_Kin,
        }

        private enum TimerID
        {
            TimerID_RequestTimeCountDown,
        }

        // 联盟列表信息
        private cmd_clan_list_head m_clanListHeadData;
        public cmd_clan_list_head ClanListHeadData { get { return m_clanListHeadData; } }
        private List<cmd_clan_clanlist_node> m_clanListData;
        public List<cmd_clan_clanlist_node> ClanListData { get { return m_clanListData; } }

        // 联盟详细信息
        private cmd_clan_basedata m_clanBaseData;
        public cmd_clan_basedata ClanBaseData { get { return m_clanBaseData; } }
        private List<cmd_clan_memberlist> m_clanMemberData;
        public List<cmd_clan_memberlist> ClanMemberData { get { return m_clanMemberData; } }
        private List<cmd_clan_applymember> m_clanApplyMemberData;
        public List<cmd_clan_applymember> ClanApplyMemberData { get { return m_clanApplyMemberData; } }
        private ClanMemberSortType m_memberSortType;

        // 联盟捐卡信息
        private int m_nLeftRequestCooldownTime;
        private int m_nCurLeftRequestCooldownTime;
        public int LeftRequestCooldownTime { get { return m_nCurLeftRequestCooldownTime; } }
        private int m_nLeftRequestNum;
        public int LeftRequestNum { get { return m_nLeftRequestNum; } }
        private float m_requestCooldownTimeStamp;
        private List<cmd_clan_request_cardlist> m_cardListData;
        public List<cmd_clan_request_cardlist> CardListData { get { return m_cardListData; } }
        private List<cmd_clan_request_selfhero> m_selfCardListData;
        public List<cmd_clan_request_selfhero> SelfCardListData { get { return m_selfCardListData; } }
        private List<cmd_clan_update_donatelog> m_donateLog;
        public List<cmd_clan_update_donatelog> DonateLog { get { return m_donateLog; } }
        private cmd_clan_add_requestcard_tip m_requestcardTip;
        public int RequestcardTipCount { get { return m_requestcardTip.nCount; } }

        // ClanEventWnd
        private List<cmd_clan_eventlist> m_ClanEventList;
        public List<cmd_clan_eventlist> ClanEventList { get { return m_ClanEventList; } }

        public void Init()
        {
            m_clanListData = new List<cmd_clan_clanlist_node>();
            m_clanMemberData = new List<cmd_clan_memberlist>();
            m_clanApplyMemberData = new List<cmd_clan_applymember>();
            m_memberSortType = ClanMemberSortType.Sort_Normal;
            m_cardListData = new List<cmd_clan_request_cardlist>();
            m_selfCardListData = new List<cmd_clan_request_selfhero>();
            m_donateLog = new List<cmd_clan_update_donatelog>();
            m_ClanEventList = new List<cmd_clan_eventlist>();

            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void Clear()
        {
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:   //等级变化
                case (int)ENTITY_PROPERTY.PROPERTY_CLAN:    //联盟变化
                    {
                        UpdateClanButton();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnClanInvite(int nPlayerID)
        {
            cmd_clan_invite data = new cmd_clan_invite();
            data.nInviteePDBID = nPlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_invite>(GameLogicDef.GVIEWCMD_CLAN_INVITE, ref data);
        }

        public void OnClanInviteReturn(IntPtr ptr, int nLen)
        {
            cmd_clan_invite_ret inviteData = IntPtrHelper.toData<cmd_clan_invite_ret>(ptr);

            DIDAAddData msg = new DIDAAddData();
            msg.nDIDATime = 60;
            msg.nItemID = (int)EDIDAType.EDIDA_Clan;
            msg.bAllowRepeatItem = false;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Clan;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteTooltip");

            ClanInviteInfo inviteInfo = new ClanInviteInfo();
            inviteInfo.nInviterID = inviteData.nInviterPDBID;
            msg.sCustomData = inviteInfo;
            msg.callBack = (CustomerInfo) =>
            {
                ClanInviteInfo customerInfo = CustomerInfo as ClanInviteInfo;
                UIDialogData toolTipData = new UIDialogData(
                    title: ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteTitle"),
                    message: String.Format(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteContent"), inviteData.szInviterName, inviteData.szClanName),
                    buttons: new UIWidgets.DialogActions() {
                            {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {ClanInviteBtnCallBack(customerInfo, true);return true;}},
                            {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),() => {ClanInviteBtnCallBack(customerInfo, false);return true;}}
                });

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        private bool ClanInviteBtnCallBack(ClanInviteInfo customerInfo, bool isAgree)
        {
            cmd_kin_answer_Invite data = new cmd_kin_answer_Invite();
            data.dwPDBID = customerInfo.nInviterID;
            data.bIsAgree = isAgree ? 1 : 0;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_ANSWER_INVITE, ref data);

            return true;
        }

        public bool IsClanBaseDataValid()
        {
            return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0;
        }

        public void OnRecvClanListData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_clanListHeadData = helper.get<cmd_clan_list_head>();
            if (m_clanListHeadData.nTotalNum > 0)
                m_clanListHeadData.nTotalNum = (m_clanListHeadData.nTotalNum - 1) / (int)EMCLAN_LIST.CLAN_LIST_PAGE_SIZE + 1;
            if (m_clanListHeadData.nPageNum > m_clanListHeadData.nTotalNum)
                m_clanListHeadData.nPageNum = m_clanListHeadData.nTotalNum;

            m_clanListData.Clear();
            for (int i = 0; i < m_clanListHeadData.nClanCount; ++i)
            {
                cmd_clan_clanlist_node cmdNodeData = helper.get<cmd_clan_clanlist_node>();
                m_clanListData.Add(cmdNodeData);
            }

            SortClanList();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCLANLIST, null);
        }

        public void OnRecvClanBaseData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);

            m_clanBaseData = helper.get<cmd_clan_basedata>();

            m_clanMemberData.Clear();
            for (int i = 0; i < m_clanBaseData.nMemberCount; ++i)
            {
                cmd_clan_memberlist cmdMemberlist = helper.get<cmd_clan_memberlist>();
                m_clanMemberData.Add(cmdMemberlist);
            }
            SortClanMember();

            m_clanApplyMemberData.Clear();
            for (int i = 0; i < m_clanBaseData.nApplyMemberNum; ++i)
            {
                cmd_clan_applymember cmdApplyMemberlist = helper.get<cmd_clan_applymember>();
                m_clanApplyMemberData.Add(cmdApplyMemberlist);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_BASEDATA, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, null);
        }

        public void OnRecvClanSetBaseData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);

            m_clanBaseData = helper.get<cmd_clan_basedata>();
            m_clanBaseData.nMemberCount = m_clanMemberData.Count;
            m_clanBaseData.nApplyMemberNum = (byte)m_clanApplyMemberData.Count;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_BASEDATA, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
        }

        public void OnRecvClanUpdate(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_selfdata cmdData = helper.get<cmd_clan_selfdata>();
            m_clanBaseData.szClanName = cmdData.szClanName;
            m_clanBaseData.szShaikhName = cmdData.szShaikhName;
            m_clanBaseData.nShaikhID = cmdData.nShaikhID;
            
        }

        public void OnUpdateClanMember(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_kin_membernode cmdKinMember = helper.get<GameLogic.cmd_kin_membernode>();
            for (int i = 0; i < m_clanMemberData.Count; ++i)
            {
                if (m_clanMemberData[i].nPDBID == cmdKinMember.dwPDBID)
                {
                    cmd_clan_memberlist node = m_clanMemberData[i];
                    node.nIsOnline = (byte)cmdKinMember.bIsOnline;
                    node.nGameStatus = cmdKinMember.byGameState;
                    m_clanMemberData[i] = node;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
                    return;
                }
            }
        }

        public void OnRecvClanAddMember(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_memberlist cmdMemberlist = helper.get<cmd_clan_memberlist>();
            for (int i = 0; i < m_clanMemberData.Count; ++i)
            {
                if (m_clanMemberData[i].nPDBID == cmdMemberlist.nPDBID)
                {
                    m_clanMemberData[i] = cmdMemberlist;
                    SortClanMember();
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
                    return;
                }
            }

            m_clanMemberData.Add(cmdMemberlist);
            SortClanMember();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
        }

        public void OnRecvClanRemoveMember(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_remove_member cmdHead = helper.get<cmd_clan_remove_member>();
            for (int i = 0; i < m_clanMemberData.Count; ++i)
            {
                if (m_clanMemberData[i].nPDBID == cmdHead.nDBID)
                {
                    m_clanMemberData.RemoveAt(i);
                    SortClanMember();
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATEMEMBER, null);
                }
            }
        }

        public void OnRecvClanQuit()
        {
            m_clanBaseData = new cmd_clan_basedata();
            // 这里特别变态，处理一下，置空一下帮会ID，界面显示值等服务器到已经晚了
            U3D_Render.EntityView ev = EntityFactory.getMainPlayer();
            if (ev != null)
            {
                // 设置数值属性
                entity_update_Numproperty updatePropData;
                updatePropData.nOtherData = 0;		// 状态数据
                updatePropData.uID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_UID);
                updatePropData.nPBUID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                updatePropData.nPropID = (int)ENTITY_PROPERTY.PROPERTY_CLAN;
                updatePropData.nValue = 0;
                updatePropData.isIntproperty= 1;
                updatePropData.isRaiseIprEvent = 1;
                ev.OnCommand_UpdateNumProp(updatePropData);
            }

            // 退出联盟处理,显示联盟列表
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_SHOWCLANLIST, null);
        }

        public void OnRecvCardList(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_request_cardlist_head cmdhead = helper.get<cmd_clan_request_cardlist_head>();
            m_cardListData.Clear();
            for (int i = 0; i < cmdhead.nCount; ++i)
            {
                cmd_clan_request_cardlist cmdRequestCardlist = helper.get<cmd_clan_request_cardlist>();
                m_cardListData.Add(cmdRequestCardlist);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST, null);
        }

        public void OnRecvCardListUpdate(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);

            bool bFind = false;
            cmd_clan_update_request_card cmd = helper.get<cmd_clan_update_request_card>();
            cmd_clan_request_cardlist data;
            data.nPDBID = cmd.nPDBID;
            data.szName = cmd.szName;
            data.nCardID = cmd.nCardID;
            data.nCurCount = cmd.nCurCount;
            data.nMaxCount = cmd.nMaxCount;
            data.nMyCurCount = cmd.nMyCurCount;
            data.nMyMaxCount = cmd.nMyMaxCount;
            data.nDonateCardCount = cmd.nDonateCardCount;
            data.nDayDonateCardCount = cmd.nDayDonateCardCount;
            // 遍历找到更新数据,没有增加
            for (int i = 0; i < m_cardListData.Count; ++i)
            {
                if (data.nPDBID == m_cardListData[i].nPDBID)
                {
                    m_cardListData[i] = data;
                    bFind = true;
                    break;
                }
            }

            if (!bFind)
            {
                m_cardListData.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST, null);
        }

        public void OnRecvCardListRemove(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_remove_request_card cmd = helper.get<cmd_clan_remove_request_card>();
            for (int i = 0; i < m_cardListData.Count; ++i)
            {
                if (cmd.nPDBID == m_cardListData[i].nPDBID)
                {
                    m_cardListData.RemoveAt(i);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_CARDLIST, null);
                    return;
                }
            }
        }

        public void OnRecvRequestDonateLog(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_request_donatelog_head cmdhead = helper.get<cmd_clan_request_donatelog_head>();
            m_donateLog.Clear();
            for (int i = 0; i < cmdhead.nCount; ++i)
            {
                cmd_clan_update_donatelog cmdDonateLog = helper.get<cmd_clan_update_donatelog>();
                m_donateLog.Add(cmdDonateLog);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG, null);
        }

        public void OnRecvUpdateDonateLog(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_update_donatelog cmdDonateLog = helper.get<cmd_clan_update_donatelog>();
            bool bFound = false;
            for (int i = 0; i < m_donateLog.Count && i < 3; ++i)
            {
                cmd_clan_update_donatelog curLog = m_donateLog[i];
                if (curLog.nGaveActorID == cmdDonateLog.nGaveActorID && curLog.nTakeActorID == cmdDonateLog.nTakeActorID && curLog.nCardID == cmdDonateLog.nCardID)
                {
                    curLog.nCardNum = cmdDonateLog.nCardNum;
                    m_donateLog[i] = curLog;
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
                m_donateLog.Insert(0, cmdDonateLog);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_DONATELOG, null);
        }

        public void OnRecvAddRequestcardTip(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_add_requestcard_tip cmdData = helper.get<cmd_clan_add_requestcard_tip>();
            if (cmdData.nIsInit == 0)
            {
                m_requestcardTip.nCount += cmdData.nCount;
            }
            else
            {
                m_requestcardTip = cmdData;
            }

            //ReflushClanRedPoint();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP, null);
        }

        public void OnRecvRequestcardInfo(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_requestcard_info cmdData = helper.get<cmd_clan_requestcard_info>();
            m_nLeftRequestCooldownTime = cmdData.nLeftCooldownTime;
            m_nCurLeftRequestCooldownTime = m_nLeftRequestCooldownTime;
            m_nLeftRequestNum = cmdData.nLeftRequestNum;
            m_requestCooldownTimeStamp = Time.unscaledTime;

            if (m_nCurLeftRequestCooldownTime > 0 && m_nLeftRequestNum > 0)
            {
                TimerManager.SetTimer(this, (int)TimerID.TimerID_RequestTimeCountDown, 1.0f);
            }
            else
            {
                TimerManager.KillTimer(this, (int)TimerID.TimerID_RequestTimeCountDown);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO, null);

            //ReflushClanRedPoint();
        }

        public virtual void OnTimer(int nTimerID)
        {
            if (m_nCurLeftRequestCooldownTime > 0)
            {
                m_nCurLeftRequestCooldownTime = m_nLeftRequestCooldownTime - (int)(Time.unscaledTime - m_requestCooldownTimeStamp);
            }
            else
            {
                m_nLeftRequestCooldownTime = 0;
                m_nCurLeftRequestCooldownTime = 0;
                TimerManager.KillTimer(this, (int)TimerID.TimerID_RequestTimeCountDown);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUEST_CARDLIST_INFO, null);
        }

        private void ReflushClanRedPoint()
        {
            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 1;
            if (LeftRequestNum > 0 && LeftRequestCooldownTime == 0)
                uiData.nCount = 1;
            else
                uiData.nCount = 0;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP, uiData);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP, null);
        }

        public void ClearRequestcardTipCount()
        {
            m_requestcardTip.nCount = 0;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_UPDATE_REQUESTCARDTIP, null);
        }

        public String GetSlefClanName()
        {
            return m_clanBaseData.szClanName;
           
        }

        public String GetClanIdentityName(int nIdentity)
        {
            // 以后可以缓存起来
            if (nIdentity == (int)EMClanIdentity.EMCLANIDENTITY_SHAIKH)
                return ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanIdentity_Shaikh");
            if (nIdentity == (int)EMClanIdentity.EMCLANIDENTITY_ELDER)
                return ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanIdentity_Elder");

            return ULocalizationService.Instance.Get("UIView", "ClanDetail", "ClanIdentity_Member");
        }

        public void SetSortType(ClanMemberSortType type)
        {
            m_memberSortType = type;
            SortClanMember(false);
        }

        private void SortClanMember(bool bUpdate = true)
        {
            if (bUpdate)
            {
                // 先重新计算战力排名
                m_clanMemberData.Sort((x, y) => { if (x.dwBattleVal == y.dwBattleVal) return 0; else return (x.dwBattleVal > y.dwBattleVal) ? -1 : 1; });
                //int nLastBV = -1;
                //int nLastOrder = 0;
                for (int i = 0; i < m_clanMemberData.Count; ++i)
                {
                    cmd_clan_memberlist memberInfo = m_clanMemberData[i];
                    int nOrder = i + 1;
                    /*
                    if (nLastBV == memberInfo.dwBattleVal)
                    {
                        nOrder = nLastOrder;
                    }
                    else
                    {
                        nLastBV = memberInfo.dwBattleVal;
                        nLastOrder = nOrder;
                    }*/
                    memberInfo.dwBVOrder = nOrder;
                    m_clanMemberData[i] = memberInfo;
                }
            }

            // 再排个序
            m_clanMemberData.Sort(
                (x, y) =>
                {
                    if (x.nIsOnline != y.nIsOnline)
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                    else if (x.nKinID != y.nKinID)
                        return (x.nKinID < y.nKinID) ? -1 : 1;
                    else
                        return (x.dwBVOrder < y.dwBVOrder) ? -1 : 1;
                }
                );
        }

        private void SortClanList()
        {
            Dictionary<int, SSchemeClanLevel> levelMap = ClanConfig.Instance.tClanLevelMap;
            if (levelMap == null)
            {
                return;
            }

            List<cmd_clan_clanlist_node> clanList = new List<cmd_clan_clanlist_node>();
            List<cmd_clan_clanlist_node> fullClanList = new List<cmd_clan_clanlist_node>();
            foreach (cmd_clan_clanlist_node clan in m_clanListData)
            {
                SSchemeClanLevel schemeLevel;
                if (levelMap.TryGetValue(clan.nLevel, out schemeLevel))
                {
                    if (schemeLevel.nMaxKinCount == clan.nKinCount)
                    {
                        fullClanList.Add(clan);
                    }
                    else
                    {
                        clanList.Add(clan);
                    }
                }
            }

            clanList = clanList.OrderByDescending(n => n.nWeekActivity).OrderByDescending(m => m.nKinCount).ToList();
            clanList.AddRange(fullClanList);
            m_clanListData = clanList;
        }

        public void OnRecvSetAutoAcceptFlag(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_clan_set_autoacceptflag cmdData = helper.get<GameLogic.cmd_clan_set_autoacceptflag>();
            m_clanBaseData.nAutoAcceptFlag = cmdData.nAutoAcceptFlag;
        }

        public void OnRecvSetClanState(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_clan_set_state cmdData = helper.get<GameLogic.cmd_clan_set_state>();
            m_clanBaseData.nState = cmdData.nState;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_STATE_NOTICE, null);
        }
        
        public void OnRecvClanEventListData(int nCount, IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_ClanEventList.Clear();
            for (int i = 0; i < nCount; ++i)
            {
                GameLogic.cmd_clan_eventlist cmdNodeData = helper.get<GameLogic.cmd_clan_eventlist>();
                // 添加到数据管理中心
                m_ClanEventList.Add(cmdNodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST, null);
        }

        public void OnRecvClanApplyList(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);

            m_clanApplyMemberData.Clear();
            GameLogic.cmd_clan_applymember_head cmdHead = helper.get<GameLogic.cmd_clan_applymember_head>();
            for (int i = 0; i < cmdHead.nCount; ++i)
            {
                cmd_clan_applymember cmdApplyMemberlist = helper.get<cmd_clan_applymember>();
                m_clanApplyMemberData.Add(cmdApplyMemberlist);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_LIST, null);
        }

        public void ClanRemoveApply(int cmdID, int nParam, string strParam, IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_clan_remove_apply cmdHead = helper.get<GameLogic.cmd_clan_remove_apply>();

            // 遍历 找到则删除
            foreach (var item in m_clanApplyMemberData)
            {
                if (cmdHead.nKinID == item.nKinID)
                {
                    m_clanApplyMemberData.Remove(item);
                    break;
                }
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, null);

            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptr, nLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_CLAN_REMOVEAPPLY;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REMOVEAPPLY, UIData);
        }

        public void ClanAddApply(int cmdID, int nParam, string strParam, IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_add_apply cmdHead = helper.get<cmd_clan_add_apply>();

            cmd_clan_applymember applymember;
            applymember.nKinID = cmdHead.nKinID;
            applymember.szName = cmdHead.szName;
            applymember.szShaikhName = cmdHead.szShaikhName;
            applymember.nMemberCount = cmdHead.nMemberCount;
            m_clanApplyMemberData.Add(applymember);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_MEMBER_TIP, null);

            UIMsgCmdData UIData = new UIMsgCmdData(cmdID, nParam, strParam, ptr, nLen);
            UIData.msgID = (int)WndMsgID.WND_MSG_CLAN_ADDAPPLY;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_ADDAPPLY, UIData);

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != m_clanBaseData.nShaikhID)
                return;

            // DIDA
            string szAgreeDesc = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "AcceptBtn");
            string szRefuseDesc = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "RefuseBtn");
            string szMsg = String.Format(ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ApplyDiDaContent"), cmdHead.szName);
            DIDAAddData msg = new DIDAAddData();
            msg.eLayout = EDIDALayout.EDT_Middle;
            msg.nDIDATime = 60;
            msg.nItemID = (int)EDIDAType.EDIDA_Clan;
            msg.bAllowRepeatItem = true;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Clan;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ApplyDiDaTitle");

            SideButtonDataClanApplyInfo applyInfo = new SideButtonDataClanApplyInfo();
            applyInfo.nKinID = cmdHead.nKinID;
            msg.sCustomData = applyInfo;
            msg.callBack = (CustomerInfo) =>
            {
                SideButtonDataClanApplyInfo customerInfo = CustomerInfo as SideButtonDataClanApplyInfo;
                UIDialogData toolTipData = new UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ApplyDiDaTitle"),
                        message: szMsg,
                        buttons: new UIWidgets.DialogActions() { { szAgreeDesc, () => { ApplyClanAdmitCallBack(customerInfo.nKinID, true); return true; } }, { szRefuseDesc, () => { ApplyClanAdmitCallBack(customerInfo.nKinID, false); return true; } } }
                        );

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        private bool ApplyClanAdmitCallBack(int nKinID, bool bResult)
        {
            bool bFind = false;
            foreach (var item in m_clanApplyMemberData)
            {
                if (nKinID == item.nKinID)
                {
                    bFind = true;
                    break;
                }
            }

            if ( bFind )
            {
                GameLogic.cmd_clan_admit cmdData = new GameLogic.cmd_clan_admit();
                cmdData.bAgree = (bResult ? 1 : 0);
                cmdData.nKinID = nKinID;
                ViewEventHelper.Instance.SendCommand<cmd_clan_admit>(GameLogicDef.GVIEWCMD_CLAN_ADMIT, ref cmdData);
            }

            return true;
        }

        // 联盟提示
        public void UpdateClanButton()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            bool bHasClan = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) == 0 ? false : true;
            cmd_kin_self_hero_member info = LogicDataCenter.kinDataManager.SelfMemberInfo;
            bool bIsShaikh = info.nIdentity == (int)TKinIdentity.emKinIdentity_Shaikh ? true : false;
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_JoinClan);

            if (bIsShaikh && !bHasClan && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_JoinClan;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UIButtonType ButtonType = new UIButtonType((int)SocialFrameButtonType.BTN_CLAN);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALTOPSIDEBUTTON_OPEN, ButtonType);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(addMsg);
            }
            else
            {
                SideButtonListRemoveData removeMsg = new SideButtonListRemoveData();
                removeMsg.nItemID = (int)ESideButtonItemType.ESBT_JoinClan;
                LogicDataCenter.sideButtonDataManager.RemoveItem(removeMsg);
            }
        }

        public void ClanShaikhCreateLengedCupDida(int cmdID, int nParam, string strParam, IntPtr ptr, int nLen)
        {
            // DIDA
            string szAgreeDesc = ULocalizationService.Instance.Get("UIView", "ClanCreateLegnedCup", "ConfirmBtn");
            string szRefuseDesc = ULocalizationService.Instance.Get("UIView", "ClanCreateLegnedCup", "CancleBtn");
            string szMsg = String.Format(ULocalizationService.Instance.Get("UIView", "ClanCreateLegnedCup", "DiDaContent"));
            DIDAAddData msg = new DIDAAddData();
            msg.eLayout = EDIDALayout.EDT_Middle;
            msg.nItemID = (int)EDIDAType.EDIDA_Clan;
            msg.bAllowRepeatItem = false;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Clan;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "ClanCreateLegnedCup", "Title");

            SideButtonDataBuildingSN dataInfo = new SideButtonDataBuildingSN();
            dataInfo.nBuildingSN = (uint)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            msg.sCustomData = dataInfo;

            msg.callBack = (CustomerData) =>
            {
                
                UIDialogData toolTipData = new UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "ClanCreateLegnedCup", "Title"),
                        message: szMsg,
                        buttons: new UIWidgets.DialogActions() { { szAgreeDesc, () => { ConfirmCreateCupCallBack(); return true; } }, { szRefuseDesc, () => { return true; } } }
                        );

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        private bool ConfirmCreateCupCallBack()
        {
            // 直接打开创建联盟杯赛界面
            cmd_legendcup_req_create_cup_type cupType = new cmd_legendcup_req_create_cup_type();
            cupType.byLegendCupType = (byte)ELegendCupType.emCupType_Clan;	// 杯赛创建类型
            cupType.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_create_cup_type>(GameLogicDef.GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ, ref cupType);
            return true;
        }
    }
}
