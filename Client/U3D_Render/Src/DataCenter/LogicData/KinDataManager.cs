using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using Kin_ManagerDef;
using USpeedUI.TooltipAndDialog;
using ASpeedGame.Data.KinConfig;
using Data.SideButtonConfig;

namespace DataCenter
{
    enum ProcessKinMember
    {
        PKM_MEMBER_ADD,
        PKM_MEMBER_UPDATE,
        PKM_MEMBER_REMOVE,
    }

    enum ProcessKinApply
    {
        PKM_APPLY_ADD,
        PKM_APPLY_REMOVE,
    }

    public class KinDataManager
    {

        // KinWnd
        private List<cmd_kin_eventlist> m_KinEventList;
        public List<cmd_kin_eventlist> KinEventList { get { return m_KinEventList; } }

        private cmd_kin_list_head m_KinListHead;
        public cmd_kin_list_head KinListHead { get { return m_KinListHead; } }
        private Dictionary<int, cmd_kin_list_node> m_KinListNode;
        public Dictionary<int, cmd_kin_list_node> KinListData { get { return m_KinListNode; } }

        //KinMemberWnd
        private List<cmd_kin_membernode> m_KinMemberList;
        public List<cmd_kin_membernode> KinMemberList { get { return m_KinMemberList; } }
        
        // 战队基础信息
        private cmd_kin_basedata m_kinBaseData;
        public cmd_kin_basedata KinBaseData { get { return m_kinBaseData; } }

        // 自己的战队成员信息
        private cmd_kin_self_hero_member m_SelfMemberInfo;
        public cmd_kin_self_hero_member SelfMemberInfo { get { return m_SelfMemberInfo; } }

        // 申请战队信息
        private cmd_kin_applylist_head m_ApplyListHead;
        public cmd_kin_applylist_head ApplyListHead { get { return m_ApplyListHead; } }

        // 请求加入战队信息
        private List<cmd_kin_applylist_node> m_ApplyListNode;
        public List<cmd_kin_applylist_node> ApplyListNode { get { return m_ApplyListNode; } }

        // 战队自动收人设置
        private cmd_kin_auto_accept m_AutoAccept;
        public cmd_kin_auto_accept AutoAccept { get { return m_AutoAccept; } }

        // 战队荣誉数据
        private cmd_kin_glory_head m_GloryHead;
        public cmd_kin_glory_head GloryHead { get { return m_GloryHead; } }

        private List<cmd_kin_glory_node> m_GloryNodeList;
        public List<cmd_kin_glory_node> GloryNodeList { get { return m_GloryNodeList; } }

        // 查看杯赛战队信息
        private List<cmd_kin_membernode> m_LegendKinMemberList;
        public List<cmd_kin_membernode> LegendKinMemberList { get { return m_LegendKinMemberList; } }
        private cmd_kin_basedata m_LegendkinBaseData;
        public cmd_kin_basedata LegendKinBaseData { get { return m_LegendkinBaseData; } }
        private cmd_kin_glory_head m_LegendGloryHead;
        public cmd_kin_glory_head LegendGloryHead { get { return m_LegendGloryHead; } }
        private List<cmd_kin_glory_node> m_LegendGloryNodeList;
        public List<cmd_kin_glory_node> LegendGloryNodeList { get { return m_LegendGloryNodeList; } }

        public void Init()
        {
            m_KinListHead = new cmd_kin_list_head();
            m_KinListNode = new Dictionary<int,cmd_kin_list_node>();
            m_KinMemberList = new List<cmd_kin_membernode>();
            m_ApplyListNode = new List<cmd_kin_applylist_node>();
            m_KinEventList = new List<cmd_kin_eventlist>();
            m_GloryNodeList = new List<cmd_kin_glory_node>();
            m_LegendKinMemberList = new List<cmd_kin_membernode>();
            m_LegendGloryNodeList = new List<cmd_kin_glory_node>();

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
                case (int)ENTITY_PROPERTY.PROPERTY_KIN:    //联盟变化
                    {
                        UpdateKinButton();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnProcessKinMember(IntPtr ptr, int nLen, int nType)
        {
            // 更新成员数据
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            switch ((ProcessKinMember)nType)
            {
                // 添加
                case ProcessKinMember.PKM_MEMBER_ADD :
                    {
                        GameLogic.cmd_kin_membernode cmdKinMember = helper.get<GameLogic.cmd_kin_membernode>();

                        m_KinMemberList.Add(cmdKinMember);
                    }
                    break;
                // 更新
                case ProcessKinMember.PKM_MEMBER_UPDATE :
                    {
                        GameLogic.cmd_kin_membernode cmdKinMember = helper.get<GameLogic.cmd_kin_membernode>();
                        for (int i = 0; i < m_KinMemberList.Count; ++i)
                        {
                            if (m_KinMemberList[i].dwPDBID == cmdKinMember.dwPDBID)
                            {
                                m_KinMemberList[i] = cmdKinMember;
                                break;
                            }
                        }
                    }
                    break;
                // 删除
                case ProcessKinMember.PKM_MEMBER_REMOVE :
                    {
                        
                        GameLogic.cmd_kin_remove_member cmdRemoveMember = helper.get<GameLogic.cmd_kin_remove_member>();
                        for (int i = 0; i < m_KinMemberList.Count; ++i)
                        {
                            if (m_KinMemberList[i].dwPDBID == cmdRemoveMember.nPlayerPDBID)
                            {
                                m_KinMemberList.RemoveAt(i);
                                break;
                            }
                        }
                    }
                    break;

            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_UPDATEMEMBER, null);

        }

        public string GetSelfKinName()
        {
            if (m_kinBaseData.nKinID <= 0)
            {
                return "";
            }
            return m_kinBaseData.szKinName;
        }

        public void OnRecvKinListData(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_KinListHead = helper.get<GameLogic.cmd_kin_list_head>();

            for (int i = 0; i < m_KinListHead.nKinCount; ++i)
            {
                GameLogic.cmd_kin_list_node cmdNodeData = helper.get<GameLogic.cmd_kin_list_node>();
                // 添加到数据管理中心
                m_KinListNode[cmdNodeData.dwID] = cmdNodeData;

            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_REQUESTKINLIST, null);
        }

        public void OnUpdateKinListData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_kin_list_node cmdNodeData = helper.get<cmd_kin_list_node>();
            cmd_kin_list_node temp = new cmd_kin_list_node();
            if (m_KinListNode.TryGetValue(cmdNodeData.dwID, out temp))
            {
                m_KinListNode[cmdNodeData.dwID] = cmdNodeData;
            }
            else
            {
                m_KinListNode.Add(cmdNodeData.dwID, cmdNodeData);
                m_KinListHead.nKinCount++;
            }
            UIMsgCmdData UIData = UIMsgCmdDataHelper.Instance.create(WndMsgID.WND_MSG_KIN_UPDATEKINLIST, cmdNodeData.dwID);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_UPDATEKINLIST, UIData);
        }

        public void OnUpdateSelfKinInfo(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_SelfMemberInfo = helper.get<GameLogic.cmd_kin_self_hero_member>();

            LogicDataCenter.clanDataManager.UpdateClanButton();
        }

        public void OnRecvKinMemberList(IntPtr ptr, int nLen)
        {
           
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_kinBaseData = helper.get<GameLogic.cmd_kin_basedata>();

            m_KinMemberList.Clear();
            for (int i = 0; i < m_kinBaseData.nMemberCount; ++i)
            {
                GameLogic.cmd_kin_membernode cmdNodeData = helper.get<GameLogic.cmd_kin_membernode>();
                // 添加到数据管理中心
                m_KinMemberList.Add(cmdNodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RECEV_MEMBERLIST, null);
        }

        public void OnKinStringPropUpdate(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_kin_stringprop_update cmdDate = helper.get<GameLogic.cmd_kin_stringprop_update>();
            // 设置战队基本数据
            TKinProp nPropID = (TKinProp)cmdDate.nPropID;
            switch (nPropID)
            {
                // 战队名
                case TKinProp.emKinProp_Name:
                    {

                    }
                    break;
                // 战队的宣言
                case TKinProp.emKinManifesto:
                    {

                    }
                    break;
                // 战队的每日公告
                case TKinProp.emKinNotice:
                    {
                        m_kinBaseData.szNotice = cmdDate.szText;
                    }
                    break;
                // 战队族长名字
                case TKinProp.emKinShaikhName:
                    {
                        m_kinBaseData.szShaikhName = cmdDate.szText;
                    }
                    break;
                // 宣传口号
                case TKinProp.emKinSlogan:
                    {

                    }
                    break;
                default:
                    break;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_STRING_PROP_UPDATE, null);
        }
        
        

        public void OnKinNumPropUpdate(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_kin_numprop_update cmdDate = helper.get<GameLogic.cmd_kin_numprop_update>();
            // 设置战队基本数据
            TKinProp nPropID = (TKinProp)cmdDate.nPropID;
            switch (nPropID)
            {
                // 战队长ID
                case TKinProp.emKinProp_ShaikhID:
                    {
                        m_kinBaseData.dwShaikhID = cmdDate.nValue;
                    }
                    break;
                default:
                    break;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_NUM_PROP_UPDATE, null);
        }


        public bool IsKinBaseDataValid()
        {
            return (m_kinBaseData.nKinID > 0);
        }

        public void OnRecvKinBaseData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_kinBaseData = helper.get<cmd_kin_basedata>();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RECEV_KINBASEDATA, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS, null);
        }

        public void onRecvKinDismiss(IntPtr ptr, int nLen)
        {
            m_KinMemberList.Clear();
            m_KinEventList.Clear();
            m_KinListNode.Remove(m_kinBaseData.nKinID);
            m_kinBaseData.nKinID = 0;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, null);
        }
        

        public void OnQuitKin()
        {
            m_kinBaseData.nKinID = 0;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, null);
        }

        public void OnRecevKinApplyList(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_ApplyListHead = helper.get<GameLogic.cmd_kin_applylist_head>();

            m_ApplyListNode.Clear();
            for (int i = 0; i < m_ApplyListHead.nCount; ++i)
            {
                GameLogic.cmd_kin_applylist_node cmdNodeData = helper.get<GameLogic.cmd_kin_applylist_node>();
                // 添加到数据管理中心
                m_ApplyListNode.Add(cmdNodeData);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RECEV_APPLY_LIST, null);
        }

        public void OnProcessKinApplyMember(IntPtr ptr, int nLen, int nType)
        {
            // 更新成员数据
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            switch ((ProcessKinApply)nType)
            {
                // 添加
                case ProcessKinApply.PKM_APPLY_ADD:
                    {
                        GameLogic.cmd_kin_applylist_node cmdKinMember = helper.get<GameLogic.cmd_kin_applylist_node>();
                        m_ApplyListNode.Add(cmdKinMember);
                        m_kinBaseData.nApplyCount++;

                        // DIDA
                        DIDAAddData msg = new DIDAAddData();
                        msg.nDIDATime = 60;
                        msg.nItemID = (int)EDIDAType.EDIDA_Kin;
                        msg.bAllowRepeatItem = true;
                        msg.nButtonIconID = (int)EDIDAType.EDIDA_Kin;
                        msg.strTooltip = ULocalizationService.Instance.Get("UIView", "KinMember", "KinApply");

                        KinApplyInfo applyInfo = new KinApplyInfo();
                        applyInfo.nApplyID = cmdKinMember.nPlayerPDBID;
                        msg.sCustomData = applyInfo;
                        msg.callBack = (CustomerInfo) =>
                        {
                            KinApplyInfo customerInfo = CustomerInfo as KinApplyInfo;
                            UIDialogData toolTipData = new UIDialogData(
                                title: ULocalizationService.Instance.Get("UIView", "KinMember", "KinApply"),
                                message: String.Format(ULocalizationService.Instance.Get("UIView", "KinMember", "KinApplyDesc"), cmdKinMember.szName),
                                buttons: new UIWidgets.DialogActions() {
                                        {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {recvKinApplyCallBack(customerInfo, true);return true;}},
                                        {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),() => {recvKinApplyCallBack(customerInfo, false);return true;}}
                            });

                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
                        };

                        LogicDataCenter.didaDataManager.AddItem(msg);
                    }
                    break;
                
                // 删除
                case ProcessKinApply.PKM_APPLY_REMOVE:
                    {

                        GameLogic.cmd_kin_remove_apply cmdRemoveMember = helper.get<GameLogic.cmd_kin_remove_apply>();
                        for (int i = 0; i < m_ApplyListNode.Count; ++i)
                        {
                            if (m_ApplyListNode[i].nPlayerPDBID == cmdRemoveMember.nPlayerPDBID)
                            {
                                m_ApplyListNode.RemoveAt(i);
                                m_kinBaseData.nApplyCount--;
                                break;
                            }
                        }
                    }
                    break;

            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_UPDATE_APPLY_LIST, null);

        }

        public ICollection<cmd_kin_list_node> SortKinList(ICollection<cmd_kin_list_node> paramKinList)
        {
            if (paramKinList == null || paramKinList.Count < 2)
            {
                return paramKinList;
            }

            Dictionary<int, SSchemeKinLevel> levelMap = KinConfig.Instance.tKinLevelMap;
            if (levelMap == null)
            {
                return paramKinList;
            }

            List<cmd_kin_list_node> kinList = new List<cmd_kin_list_node>();
            List<cmd_kin_list_node> fullKinList = new List<cmd_kin_list_node>();
            foreach (cmd_kin_list_node kin in paramKinList)
            {
                SSchemeKinLevel schemeLevel;
                if (levelMap.TryGetValue(kin.nLevel, out schemeLevel))
                {
                    if (schemeLevel.nMaxPersonCount == kin.nMemberCount)
                    {
                        fullKinList.Add(kin);
                    }
                    else
                    {
                        kinList.Add(kin);
                    }
                }
            }

            kinList = kinList.OrderByDescending(n => n.nWeekActivity).OrderByDescending(n => n.nMemberCount).ToList();
            kinList.AddRange(fullKinList);

            return kinList;
        }

        // 请求加入战队回调
        private void recvKinApplyCallBack(KinApplyInfo info, bool isAgree)
        {
            cmd_kin_admit cmdData = new cmd_kin_admit();
            cmdData.bAgree = isAgree ? 1 : 0;
            cmdData.nApplyPDBID = info.nApplyID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_admit>(GameLogicDef.GVIEWCMD_KIN_ADMIT, ref cmdData);
        }

        public void OnRecvAutoAccept(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_AutoAccept = helper.get<GameLogic.cmd_kin_auto_accept>();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_AUTO_ACCEPT, null);
        }

        public void OnRecvKinEventListData(int nCount, IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_KinEventList.Clear();
            for (int i = 0; i < nCount; ++i)
            {
                GameLogic.cmd_kin_eventlist cmdNodeData = helper.get<GameLogic.cmd_kin_eventlist>();
                // 添加到数据管理中心
                m_KinEventList.Add(cmdNodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RECV_EVENTLIST, null);
        }

        public void OnRecvKinGloryData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_GloryHead = helper.get<cmd_kin_glory_head>();

            m_GloryNodeList.Clear();
            for (int i = 0; i < m_GloryHead.nCount; i++)
            {
                cmd_kin_glory_node nodeData = helper.get<cmd_kin_glory_node>();
                m_GloryNodeList.Add(nodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RECV_GLORYDATA, null);
        }

        public void OnKinApply(int nNewKinID)
        {
            ActorPlayerInfo playerData = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
            if (playerData.playerInfo.pDbid == 0)
                return;
                
            cmd_kin_apply data = new cmd_kin_apply();
            data.nOldKinID = m_kinBaseData.nKinID;
            data.nKinID = nNewKinID;
            data.szName = playerData.playerInfo.chPlayerName;
            data.nLevel = (byte)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            data.nVocation = (byte)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            ViewEventHelper.Instance.SendCommand<cmd_kin_apply>(GameLogicDef.GVIEWCMD_KIN_APPLY, ref data);
        }

        public void OnKinInvite(int nPlayerID)
        {
            cmd_kin_invite data = new cmd_kin_invite();
            data.nAnswerPDBID = nPlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_invite>(GameLogicDef.GVIEWCMD_KIN_INVITE, ref data);
        }

        // 收到邀请加入战队请求
        public void OnRecvKinInvite(IntPtr ptr, int nLen)
        {
            cmd_recv_kin_invite inviteData = IntPtrHelper.toData<cmd_recv_kin_invite>(ptr);

            DIDAAddData msg = new DIDAAddData();
            msg.nDIDATime = 60;
            msg.nItemID = (int)EDIDAType.EDIDA_Kin;
            msg.bAllowRepeatItem = false;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Kin;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "KinMember", "KinRequest");

            KinInviteInfo inviteInfo = new KinInviteInfo();
            inviteInfo.nInviterID = inviteData.dwInviterPDBID;
            msg.sCustomData = inviteInfo;
            msg.callBack = (CustomerInfo) =>
            {
                KinInviteInfo customerInfo = CustomerInfo as KinInviteInfo;
                UIDialogData toolTipData = new UIDialogData(
                    title: ULocalizationService.Instance.Get("UIView", "KinMember", "KinRequest"),
                    message: String.Format(ULocalizationService.Instance.Get("UIView", "KinMember", "KinRequestDesc"), inviteData.szKinName, inviteData.szInviteName),
                    buttons: new UIWidgets.DialogActions() {
                            {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {recvKinInviteCallBack(customerInfo, true);return true;}},
                            {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),() => {recvKinInviteCallBack(customerInfo, false);return true;}}
                });

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        // 邀请答复回调
        private void recvKinInviteCallBack(KinInviteInfo info, bool isAgree)
        {
            cmd_kin_answer_Invite data = new cmd_kin_answer_Invite();
            data.dwPDBID = info.nInviterID;
            data.bIsAgree = isAgree ? 1 : 0;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_ANSWER_INVITE, ref data);
        }

        // 查看杯赛战队信息
        public void viewCompetitionKinInfo(int nKinID)
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, nKinID, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, uiData);
        }

        // 查看杯赛战队成员
        public void viewCompetitionKinMember(int nKinID)
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_SHOW, nKinID, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_SHOW, uiData);
        }

        // 接收杯赛战队信息
        public void OnRecvLegendKinInfo(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_LegendkinBaseData = helper.get<cmd_kin_basedata>();

            m_LegendKinMemberList.Clear();
            for (int i = 0; i < m_LegendkinBaseData.nMemberCount; ++i)
            {
                cmd_kin_membernode cmdNodeData = helper.get<cmd_kin_membernode>();
                // 添加到数据管理中心
                m_LegendKinMemberList.Add(cmdNodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_RECV, null);
        }

        // 接收杯赛战队荣誉
        public void OnRecvLegendKinGlory(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_LegendGloryHead = helper.get<cmd_kin_glory_head>();

            m_LegendGloryNodeList.Clear();
            for (int i = 0; i < m_LegendGloryHead.nCount; i++)
            {
                cmd_kin_glory_node nodeData = helper.get<cmd_kin_glory_node>();
                m_LegendGloryNodeList.Add(nodeData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINGLORY_RECV, null);
        }

        // 战队提示
        private void UpdateKinButton()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            bool bHasKin = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN) == 0 ? false : true;
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_JoinKin);

            if (!bHasKin && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_JoinKin;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UIButtonType ButtonType = new UIButtonType((int)SocialFrameButtonType.BTN_KIN);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALTOPSIDEBUTTON_OPEN, ButtonType);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(addMsg);
            }
            else
            {
                SideButtonListRemoveData removeMsg = new SideButtonListRemoveData();
                removeMsg.nItemID = (int)ESideButtonItemType.ESBT_JoinKin;
                LogicDataCenter.sideButtonDataManager.RemoveItem(removeMsg);
            }
        }
    }
}
