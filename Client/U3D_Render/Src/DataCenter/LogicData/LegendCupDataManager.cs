using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using LegendCup_ManagedDef;
using USpeedUI.LegendCup;
using UnityEngine;
using USpeedUI.TooltipAndDialog;

namespace DataCenter
{
    // LegendCupWarRecordWnd
    public class SingleNodeRecordInfo
    {
        public cmd_legendcup_recv_competitionnode_head headInfo;
        public List<cmd_kin_warrecord_total_warnode> baseInfo;
    }

    // LegendCupRegistWnd
    public enum ELegendCupListType
    {
        ELCLT_All,      // 所有
        ELCLT_Clan,     // 联盟限制

        ELCLT_Max,
    }

    public class SingleRegistKinInfo
    {
        public cmd_legendcup_recv_regist_kinlisthead headInfo;
        public List<cmd_legendcup_recv_regist_kinlistnode> nodeInfo;
    }

    public class SingleRegistKinMemberInfo
    {
        public cmd_legendcup_recv_regist_memberlisthead headInfo;
        public List<cmd_legendcup_recv_regist_memberlistnode> nodeInfo;
    }

    public class SingleRegistBlacklistInfo
    {
        public cmd_legendcup_recv_regist_balcklisthead headInfo;
        public List<cmd_legendcup_recv_regist_balcklistnode> nodeInfo;
    }

    // LegendCupCompetition(Group, BigKnockout, SmallKnockout)
    public class SingleCompetitionInfo
    {
        public cmd_legendcup_recv_competition_head headInfo;
        public List<cmd_legendcup_recv_competition_roundinfo> roundInfo;
        public List<cmd_legendcup_recv_competition_node> nodeInfo;
    }

    public class LegendCupDataManager
    {
        // LegendCupWarRecordWnd
        private List<cmd_kin_warrecord_total_warnode> m_totalWarRecordList;
        private SingleNodeRecordInfo m_cupNodeRecord;

        // LegendCupListWnd
        private int m_legendCupListType;
        public int LegendCupListType { get { return m_legendCupListType; } set { m_legendCupListType = value; } }
        private List<cmd_legendcup_recv_cuplist_node> m_legendCupNodeList;
        private Dictionary<long, cmd_legendcup_recv_cuplist_node> m_legendCupSelfInNodeDic;
        public Dictionary<long, cmd_legendcup_recv_cuplist_node> LegendCupSelfInNodeDic { get { return m_legendCupSelfInNodeDic; } }
        private Dictionary<long, cmd_legendcup_recv_cuplist_node> m_legendCupSelfCreateNodeDic;
        public Dictionary<long, cmd_legendcup_recv_cuplist_node> LegendCupSelfCreateNodeDic { get { return m_legendCupSelfCreateNodeDic; } }

        // LegendCupRegistWnd
        private Dictionary<long, SingleRegistKinInfo> m_legendCupRegistKinDic;
        private Dictionary<long, SingleRegistBlacklistInfo> m_legendCupRegistBlacklistDic;
        private Dictionary<int, SingleRegistKinMemberInfo> m_legendCupRegistKinMembrDic;

        // LegendCupCompetition(Group, BigKnockout, SmallKnockout)
        //public const int SMALLKNOCKOUTWND_MAXKINNUM = 16;
        private SingleCompetitionInfo m_legendCupCompetition;
        private Dictionary<int, List<cmd_legendcup_competition_kinmembernode> > m_CompetitionKinMemberDic;

        // Res
        public ResNode m_legendCupPrizeItemRes;

        public void Init()
        {
            m_totalWarRecordList = new List<cmd_kin_warrecord_total_warnode>();
            m_cupNodeRecord = new SingleNodeRecordInfo();

            m_legendCupNodeList = new List<cmd_legendcup_recv_cuplist_node>();
            m_legendCupSelfInNodeDic = new Dictionary<long, cmd_legendcup_recv_cuplist_node>();
            m_legendCupSelfCreateNodeDic = new Dictionary<long, cmd_legendcup_recv_cuplist_node>();

            m_legendCupRegistKinDic = new Dictionary<long, SingleRegistKinInfo>();
            m_legendCupRegistBlacklistDic = new Dictionary<long, SingleRegistBlacklistInfo>();
            m_legendCupRegistKinMembrDic = new Dictionary<int, SingleRegistKinMemberInfo>();

            m_legendCupCompetition = new SingleCompetitionInfo();
            m_CompetitionKinMemberDic = new Dictionary<int, List<cmd_legendcup_competition_kinmembernode>>();

            m_legendCupPrizeItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/LegendCup/PrizeItemTemplate");
        }

        public void Clear()
        {
            m_totalWarRecordList.Clear();

            m_legendCupNodeList.Clear();
            m_legendCupSelfInNodeDic.Clear();
            m_legendCupSelfCreateNodeDic.Clear();

            m_legendCupRegistKinDic.Clear();
            m_legendCupRegistBlacklistDic.Clear();
            m_legendCupRegistKinMembrDic.Clear();

            m_CompetitionKinMemberDic.Clear();

            AssetBundleManager.DeleteAssets(ref m_legendCupPrizeItemRes, true);
        }


        // LegendCupWarRecordWnd
        public void RecvTotalWarRecord(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nCount = helper.get<int>();
            m_totalWarRecordList.Clear();
            for (int i = 0; i < nCount; ++i)
            {
                cmd_kin_warrecord_total_warnode data = helper.get<cmd_kin_warrecord_total_warnode>();
                m_totalWarRecordList.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_TOTAL, null);
        }

        public void RecvCupNodeWarRecord(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            //m_cupNodeRecord = new SingleNodeRecordInfo();
            m_cupNodeRecord.headInfo = helper.get<cmd_legendcup_recv_competitionnode_head>();

            m_cupNodeRecord.baseInfo = new List<cmd_kin_warrecord_total_warnode>();
            for (int i = 0; i < m_cupNodeRecord.headInfo.nCount; ++i)
            {
                cmd_kin_warrecord_total_warnode data = helper.get<cmd_kin_warrecord_total_warnode>();
                m_cupNodeRecord.baseInfo.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_RECV, null);
        }

        public cmd_legendcup_recv_competitionnode_head GetTotalWarRecordHeadInfo()
        {
            return m_cupNodeRecord.headInfo;
        }

        public List<cmd_kin_warrecord_total_warnode> GetTotalWarRecord()
        {
            return m_totalWarRecordList;
        }

        public SingleNodeRecordInfo GetCupNodeWarRecord()
        {
            return m_cupNodeRecord;
        }


        // LegendCupListWnd
        public void RecvTotalLegendCup(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_legendcup_recv_cuplist_head head = helper.get<cmd_legendcup_recv_cuplist_head>();

            List<cmd_legendcup_recv_cuplist_node> tempList = new List<cmd_legendcup_recv_cuplist_node>();
            for (int i = 0; i < head.nCount; ++i)
            {
                cmd_legendcup_recv_cuplist_node data = helper.get<cmd_legendcup_recv_cuplist_node>();
                // 如果是请求联盟杯赛，客户端做一次筛选
                if (m_legendCupListType == (int)ELegendCupListType.ELCLT_Clan && data.nIsClanLimit <= 0)
                    continue;

                tempList.Add(data);
            }
            SetLegendCupListBySequence(tempList);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_RECVLIST, null);
        }

        public void UpdateSingleCup(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_legendcup_recv_cuplist_node data = helper.get<cmd_legendcup_recv_cuplist_node>();
            if (m_legendCupListType == (int)ELegendCupListType.ELCLT_Clan && data.nIsClanLimit <= 0)
                return;

            List<cmd_legendcup_recv_cuplist_node> tempList = new List<cmd_legendcup_recv_cuplist_node>(m_legendCupNodeList);
            bool bIsIn = false;
            for (int i = 0; i < tempList.Count; i++)
            {
                if (tempList[i].nLegendCupID == data.nLegendCupID)
                {
                    tempList[i] = data;
                    bIsIn = true;
                    break;
                }
            }
            if (!bIsIn)
            {
                tempList.Add(data);
            }
            SetLegendCupListBySequence(tempList);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_UPDATELIST, null);
            if (data.nIsNewCreate > 0)
            {
                // 关闭创建界面
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_CLOSE, null);
                // 打开详细界面
                cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
                cupdata.nLegendCupID = data.nLegendCupID;
                ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
            }
        }

        public List<cmd_legendcup_recv_cuplist_node> GetTotalLegendCup()
        {
            return m_legendCupNodeList;
        }

        public cmd_legendcup_recv_cuplist_node GetSingleLegendCupNode(long nCupID)
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = new cmd_legendcup_recv_cuplist_node();
            foreach (cmd_legendcup_recv_cuplist_node item in m_legendCupNodeList)
            {
                if (item.nLegendCupID == nCupID)
                {
                    cupBaseData = item;
                    break;
                }
            }
            return cupBaseData;
        }

        private void SetLegendCupListBySequence(List<cmd_legendcup_recv_cuplist_node> cupList)
        {
            if (cupList == null || cupList.Count <= 0)
                return;

            // 顺序：系统创建>自身比赛>自身创建>联盟限制>战队数量
            IEnumerable<cmd_legendcup_recv_cuplist_node> ListQuery =
                from item in cupList
                orderby item.nIsSystemCreate descending, item.nIsSelfIn descending,item.nIsSelfCreate descending, item.nIsClanLimit descending, item.nCurrentKinNum descending
                select item;

            int nSelfPDBID = 0;
            if (EntityFactory.MainHeroView != null )
            {
                nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }
            m_legendCupNodeList.Clear();
            m_legendCupSelfInNodeDic.Clear();
            foreach (cmd_legendcup_recv_cuplist_node item in ListQuery)
            {
                m_legendCupNodeList.Add(item);
                // 自身参加
                if (item.nIsSelfIn > 0)
                {
                    m_legendCupSelfInNodeDic[item.nLegendCupID] = item;
                }
                // 自身创建
                if (item.nCreaterPDBID == nSelfPDBID)
                {
                    m_legendCupSelfCreateNodeDic[item.nLegendCupID] = item;
                }
            }
        }

        // LegendCupRegistWnd
        public void RecvCreateCupTypeInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, 0, "", ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, uiData);
        }

        public void RecvCupRegistKinList(IntPtr ptrParam, int nPtrLen)
        {
            SingleRegistKinInfo info = new SingleRegistKinInfo();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            info.headInfo = helper.get<cmd_legendcup_recv_regist_kinlisthead>();
            info.nodeInfo = new List<cmd_legendcup_recv_regist_kinlistnode>();
            for (int i = 0; i < info.headInfo.nCurKinCount; ++i)
            {
                cmd_legendcup_recv_regist_kinlistnode data = helper.get<cmd_legendcup_recv_regist_kinlistnode>();
                info.nodeInfo.Add(data);
            }
            m_legendCupRegistKinDic[info.headInfo.nLegendCupID] = info;

            LegendCupIDData uiData = new LegendCupIDData();
            uiData.nLegendCupID = info.headInfo.nLegendCupID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_KINLIST, uiData);
        }

        public void RecvCupRegistKinMemberList(IntPtr ptrParam, int nPtrLen)
        {
            SingleRegistKinMemberInfo info = new SingleRegistKinMemberInfo();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            info.headInfo = helper.get<cmd_legendcup_recv_regist_memberlisthead>();
            info.nodeInfo = new List<cmd_legendcup_recv_regist_memberlistnode>();
            for (int i = 0; i < info.headInfo.nMemberCount; ++i)
            {
                cmd_legendcup_recv_regist_memberlistnode data = helper.get<cmd_legendcup_recv_regist_memberlistnode>();
                info.nodeInfo.Add(data);
            }
            m_legendCupRegistKinMembrDic[info.headInfo.nKinID] = info;

            UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, info.headInfo.nKinID, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, cmdData);
        }

        public void RecvCupRegistBlacklist(IntPtr ptrParam, int nPtrLen)
        {
            SingleRegistBlacklistInfo info = new SingleRegistBlacklistInfo();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            info.headInfo = helper.get<cmd_legendcup_recv_regist_balcklisthead>();
            info.nodeInfo = new List<cmd_legendcup_recv_regist_balcklistnode>();
            for (int i = 0; i < info.headInfo.nCurBlacklistNum; ++i)
            {
                cmd_legendcup_recv_regist_balcklistnode data = helper.get<cmd_legendcup_recv_regist_balcklistnode>();
                info.nodeInfo.Add(data);
            }
            m_legendCupRegistBlacklistDic[info.headInfo.nLegendCupID] = info;

            LegendCupIDData uiData = new LegendCupIDData();
            uiData.nLegendCupID = info.headInfo.nLegendCupID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_BLACKLIST, uiData);
        }

        public void RecvRegistSuccess()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_CLOSE, null);
        }

        public void RecvCupCreaterPublicSuccess()
        {
            if (!UISystem.Instance.IsWndVisible(WndID.WND_ID_LEGENDCUP_REGIST))
            {
                return;   
            }

            // 关闭注册界面  打开相应的杯赛阶段界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_CLOSE, null);

            if (m_legendCupSelfCreateNodeDic.Count == 1)
            {
                cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
                cupdata.nLegendCupID = m_legendCupSelfCreateNodeDic.ElementAt(0).Value.nLegendCupID;
                ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
            }
        }

        public void RecvCupPublicSuccess()
        {
            if (!UISystem.Instance.IsWndVisible(WndID.WND_ID_LEGENDCUP_REGIST))
            {
                return;
            }

            // 关闭注册界面  打开相应的杯赛阶段界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_CLOSE, null);

            if (m_legendCupSelfInNodeDic.Count == 1)
            {
                cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
                cupdata.nLegendCupID = m_legendCupSelfInNodeDic.ElementAt(0).Value.nLegendCupID;
                ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
            }
        }

        public SingleRegistKinInfo GetSingleRegistKinInfo(long nLegendCupID)
        {
            if (m_legendCupRegistKinDic.ContainsKey(nLegendCupID))
            {
                return m_legendCupRegistKinDic[nLegendCupID];
            }
            return null;
        }

        public SingleRegistKinMemberInfo GetSingleRegistKinMemberInfo(int nKinID)
        {
            if (m_legendCupRegistKinMembrDic.ContainsKey(nKinID))
            {
                return m_legendCupRegistKinMembrDic[nKinID];
            }
            return null;
        }

        public SingleRegistBlacklistInfo GetSingleRegistBlacklistInfo(long nLegendCupID)
        {
            if (m_legendCupRegistBlacklistDic.ContainsKey(nLegendCupID))
            {
                return m_legendCupRegistBlacklistDic[nLegendCupID];
            }
            return null;
        }


        // LegendCupCompetition(Group, BigKnockout, SmallKnockout)
        public void RecvCupCompetitionKinMember(IntPtr ptrParam, int nPtrLen)
        {
            m_CompetitionKinMemberDic.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_legendcup_competition_kinmemberhead headData = helper.get<cmd_legendcup_competition_kinmemberhead>();

            for (int i = 0; i < headData.nMemberCount; ++i)
            {
                cmd_legendcup_competition_kinmembernode data = helper.get<cmd_legendcup_competition_kinmembernode>();
                List<cmd_legendcup_competition_kinmembernode> memberList = null;
                if (m_CompetitionKinMemberDic.TryGetValue(data.nKinID, out memberList))
                {
                    memberList.Add(data);
                }
                else
                {
                    memberList = new List<cmd_legendcup_competition_kinmembernode>();
                    memberList.Add(data);
                    m_CompetitionKinMemberDic[data.nKinID] = memberList;
                }
            }
        }

        public void RecvCupCompetitionAllRoundTime(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_legendcup_competition_kinmemberhead headData = helper.get<cmd_legendcup_competition_kinmemberhead>();
            // 时间从这里获取
            m_legendCupCompetition.roundInfo = new List<cmd_legendcup_recv_competition_roundinfo>();
            for (int i = 0; i < headData.nMemberCount; i++)
            {
                cmd_legendcup_recv_competition_roundinfo roundData = helper.get<cmd_legendcup_recv_competition_roundinfo>();
                m_legendCupCompetition.roundInfo.Add(roundData);
            }
        }

        public void RecvCupCompetitionInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            m_legendCupCompetition.headInfo = helper.get<cmd_legendcup_recv_competition_head>();

            // 时间不从这里获取,改为单独下发RecvCupCompetitionAllRoundTime()
            //m_legendCupCompetition.roundInfo = new List<cmd_legendcup_recv_competition_roundinfo>();
            for (int i = 0; i < m_legendCupCompetition.headInfo.nRoundID; i++)
            {
                cmd_legendcup_recv_competition_roundinfo roundData = helper.get<cmd_legendcup_recv_competition_roundinfo>();
                //m_legendCupCompetition.roundInfo.Add(roundData);
            }
            m_legendCupCompetition.nodeInfo = new List<cmd_legendcup_recv_competition_node>();
            for (int i = 0; i < m_legendCupCompetition.headInfo.nCompetitionNodeCount; ++i)
            {
                cmd_legendcup_recv_competition_node nodeData = helper.get<cmd_legendcup_recv_competition_node>();
                m_legendCupCompetition.nodeInfo.Add(nodeData);
            }

            // 通知相应界面
            LegendCupIDData uiData = new LegendCupIDData();
            uiData.nLegendCupID = m_legendCupCompetition.headInfo.nLegendCupID;
            if (m_legendCupCompetition.headInfo.byCompetitionType == (int)ERoundCompetitionType.emType_GroupCompetition)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_SHOW, uiData);
            }
            else if (m_legendCupCompetition.headInfo.byCompetitionType == (int)ERoundCompetitionType.emType_KnockoutCompetition)
            {
                // 大淘汰赛、小淘汰赛界面合并
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_SHOW, uiData);
            }
        }

        public void UpdateCupCompetitionInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            long llegendCupId = helper.get<long>();
            cmd_legendcup_recv_competition_node node = helper.get<cmd_legendcup_recv_competition_node>();

            // 自己战队dida相关
            if (!(CheckIsSelfInCompetitionMember(node.nKin1ID) == false && CheckIsSelfInCompetitionMember(node.nKin2ID) == false))
            {
                bool bDida = (node.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter);
                SetLegendCupDIDA(bDida, llegendCupId, node.nRoundID, node.nSearchID);
            }

            // 更新节点信息
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(llegendCupId);
            if (cupBaseData.nLegendCupID == 0)
                return;
            if (m_legendCupCompetition == null || m_legendCupCompetition.nodeInfo == null)
                return;
            for (int i = 0; i < m_legendCupCompetition.nodeInfo.Count; i++)
            {
                if (m_legendCupCompetition.nodeInfo[i].nSearchID == node.nSearchID)
                {
                    m_legendCupCompetition.nodeInfo[i] = node;
                    break;
                }
            }

            // 通知相应界面
            LegendCupIDData uiData = new LegendCupIDData();
            uiData.nLegendCupID = m_legendCupCompetition.headInfo.nLegendCupID;
            if (m_legendCupCompetition.headInfo.byCompetitionType == (int)ERoundCompetitionType.emType_GroupCompetition)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_UPDATE, uiData);
            }
            else if (m_legendCupCompetition.headInfo.byCompetitionType == (int)ERoundCompetitionType.emType_KnockoutCompetition)
            {
                // 大淘汰赛、小淘汰赛界面合并
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_UPDATE, uiData);
            }

            
        }

        public List<cmd_legendcup_competition_kinmembernode> GetKinMemberInfo(int nKinID)
        {
            if (m_CompetitionKinMemberDic.ContainsKey(nKinID))
            {
                return m_CompetitionKinMemberDic[nKinID];
            }
            return null;
        }

        public SingleCompetitionInfo GetSingleCompetitionInfo()
        {
            return m_legendCupCompetition;
        }

        // DIDA移除有三处:1、时间到2、点DIDA3、点参加    设置DIDA：该位置、线上赛
        public void SetLegendCupDIDA(bool bActive, long nLegendCupID, int nRoundID, int nSearchID)
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0 || cupBaseData.nIsOnline <= 0)
                return;

            SideButtonLegendBuildingSN dataInfo = new SideButtonLegendBuildingSN();
            dataInfo.nLegendCupID = (uint)nLegendCupID;
            dataInfo.nSearchID = nSearchID;
            if (bActive)
            {
                DIDAAddData addMsg = new DIDAAddData();
                addMsg.nItemID = (int)EDIDAType.EDIDA_LegendCup;
                addMsg.bAllowRepeatItem = true;
                addMsg.nButtonIconID = (int)EDIDAType.EDIDA_LegendCup;
                addMsg.strButtonText = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "JoinCupRoom");
                addMsg.strTooltip = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "JoinCupRoom");
                addMsg.sCustomData = dataInfo;
                addMsg.callBack = (CustomerData) =>
                {
                    SideButtonDataBuildingSN customerData = CustomerData as SideButtonDataBuildingSN;
                    UIDialogData tooltipData = new UIDialogData(
                                       title: cupBaseData.szCupName,
                                       message: "[" + cupBaseData.szCupName + "]" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "JoinCupRoomContent"),
                                       buttons: new UIWidgets.DialogActions()   
                                       {
                                       {ULocalizationService.Instance.Get("UIView", "Team", "AgreeButtonDes"), () => { RequestDIDACallBack(true, nLegendCupID, nRoundID, nSearchID);return true; }},                                         
                                       {ULocalizationService.Instance.Get("UIView", "Team", "DisAgreeButtonDes"), () => { RequestDIDACallBack(false, nLegendCupID, nRoundID, nSearchID);return true; }} 
                                       });
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, tooltipData);
                };

                LogicDataCenter.didaDataManager.AddItem(addMsg);
            }
            else
            {
                DIDARemoveData rmMsg = new DIDARemoveData();
                rmMsg.nItemID = (int)EDIDAType.EDIDA_LegendCup;
                rmMsg.sCustomData = dataInfo;
                LogicDataCenter.didaDataManager.RemoveItem(rmMsg);
            }
        }

        private void RequestDIDACallBack(bool bAgree, long nLegendCupID, int nRoundID, int nSearchID)
        {
            if (!bAgree || nLegendCupID == 0 || nRoundID == 0 || nSearchID == 0)
            {
                return;
            }

            cmd_legendcup_enter_competition_node nodeData = new cmd_legendcup_enter_competition_node();
            nodeData.nLegendCupID = nLegendCupID;
            nodeData.nRoundID = nRoundID;
            nodeData.nSearchID = nSearchID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_enter_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE, ref nodeData);
        }


        // Res
        public LegendCupPrizeItem CreateLegendCupPrizeItem(RectTransform container)
        {
            GameObject newItem = m_legendCupPrizeItemRes.InstanceMainRes();
            LegendCupPrizeItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<LegendCupPrizeItem>();
                if (result)
                {
                    result.SetContainer(container);
                }
                else
                {
                    m_legendCupPrizeItemRes.DestroyInstanceRes(ref newItem);
                }
            }
            return result;
        }

        //// 检测自身是否在某战队报名列表内
        //public bool CheckIsSelfInKinRegistMember(int nKinID)
        //{
        //    int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
        //    SingleRegistKinMemberInfo memberInfo = GetSingleRegistKinMemberInfo(nKinID);
        //    if (memberInfo == null)
        //        return false;

        //    int nHit = memberInfo.nodeInfo.FindIndex(item => item.nPDBID == nSelfID);
        //    if (nHit >= 0)
        //        return true;

        //    return false;
        //}

        // 检测自身是否在某战队比赛列表内
        public bool CheckIsSelfInCompetitionMember(int nKinID)
        {
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            List<cmd_legendcup_competition_kinmembernode> memberInfo = GetKinMemberInfo(nKinID);
            if (memberInfo == null)
                return false;

            int nHit = memberInfo.FindIndex(item => item.nPDBID == nSelfID);
            if (nHit >= 0)
                return true;

            return false;
        }

    }
}
