using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;
using Mentorship_ManagerDef;
using ASpeedGame.Data.MentorshipConfig;
using USpeedUI.TooltipAndDialog;
using U3D_Render;
using USpeedUI.UWidgets;

namespace DataCenter
{
    public class MentorshipDataManager
    {
        //MentorshipMasterIssueWnd
        private int m_MasterIssueCount;
        public int MASTERISSUECOUNT { get { return m_MasterIssueCount; } }
        private List<cmd_mentorship_issueinfo> m_MasterIssueList;
        public List<cmd_mentorship_issueinfo> MasterIssueList { get { return m_MasterIssueList; } }

        //MentorshipPrenticeIssueWnd
        private int m_PrenticeIssueCount;
        public int PRENTICEISSUECOUNT { get { return m_PrenticeIssueCount; } }
        private List<cmd_mentorship_issueinfo> m_PrenticeIssueList;
        public List<cmd_mentorship_issueinfo> PrenticeIssueList { get { return m_PrenticeIssueList; } }
        
        // 师徒个人基础信息
        private cmd_mentorship_person_baseinfo m_MentorshipPersonData;
        public cmd_mentorship_person_baseinfo MentorshipPersonData { get { return m_MentorshipPersonData; } }

        // 师傅列表信息
        private List<cmd_mentorship_personshipinfo> m_MasterList;
        public List<cmd_mentorship_personshipinfo> MasterList { get { return m_MasterList; } }

        // 正式学徒列表信息
        private List<cmd_mentorship_personshipinfo> m_FPrenticeList;
        public List<cmd_mentorship_personshipinfo> FPrenticeList { get { return m_FPrenticeList; } }

        // 见习学徒列表信息
        private List<cmd_mentorship_personshipinfo> m_IPrenticeList;
        public List<cmd_mentorship_personshipinfo> IPrenticeList { get { return m_IPrenticeList; } }

        // 导师正式学徒列表信息
        private List<cmd_mentorship_prenticeinfo> m_MasterFPrenticeList;
        public List<cmd_mentorship_prenticeinfo> MasterFPrenticeList { get { return m_MasterFPrenticeList; } }

        // 导师见习学徒列表信息
        private List<cmd_mentorship_prenticeinfo> m_MasterIPrenticeList;
        public List<cmd_mentorship_prenticeinfo> MasterIPrenticeList { get { return m_MasterIPrenticeList; } }

        // 应答数据
        private Dictionary<int, cmd_mentorship_request_applyanswer> m_ApplyAnwserList;

        // 导师申请记录<PDBID, time>
        private Dictionary<int, float> m_MApplyRecordList;
        public Dictionary<int, float> MApplyRecordList { get { return m_MApplyRecordList; } }
        // 学徒申请记录<PDBID, time>
        private Dictionary<int, float> m_PApplyRecordList;
        public Dictionary<int, float> PApplyRecordList { get { return m_PApplyRecordList; } }

        // 右键玩家弹出菜单
        private PopupMenuDataManager.UPopupItemRegisterInfo m_AddMasterInfo;
        private PopupMenuDataManager.UPopupItemRegisterInfo m_AddPrenticeInfo;

        public void Init()
        {
            m_MasterIssueCount = 0;
            m_PrenticeIssueCount = 0;
            m_MentorshipPersonData = new cmd_mentorship_person_baseinfo();
            m_MasterIssueList = new List<cmd_mentorship_issueinfo>();
            m_PrenticeIssueList = new List<cmd_mentorship_issueinfo>();
            m_MasterList = new List<cmd_mentorship_personshipinfo>();
            m_FPrenticeList = new List<cmd_mentorship_personshipinfo>();
            m_IPrenticeList = new List<cmd_mentorship_personshipinfo>();
            m_MasterFPrenticeList = new List<cmd_mentorship_prenticeinfo>();
            m_MasterIPrenticeList = new List<cmd_mentorship_prenticeinfo>();
            m_ApplyAnwserList = new Dictionary<int, cmd_mentorship_request_applyanswer>();
            m_MApplyRecordList = new Dictionary<int, float>();
            m_PApplyRecordList = new Dictionary<int, float>();

            m_AddMasterInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_AddMaster, processRightClickAddMaster);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_AddMasterInfo);

            m_AddPrenticeInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_AddPrentice, processRightClickAddPrentice);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_AddPrenticeInfo);
        }

        // 处理拜师菜单项
        private void processRightClickAddMaster(int nEntityID)
        {
            if ( IsInternMasterFull() )
                return;

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int nSelfLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

            EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
            if (ev == null || ev.Property == null)
                return;

            int nPDBID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (nPDBID == 0 || IsHaveMentorship(nPDBID))
                return;

            int nlevel = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

            if (nlevel < MentorshipConfig.Instance.tMentorshipConfig.nMasterNeedLevel || nlevel < nSelfLevel)
                return;

            // 构造弹出菜单列表
            UListComponentItemDataPopup item = new UPopupItemDataApplyInternPrentice(nPDBID, _strItemTypeName:
                UPopupItemDataApplyInternPrentice.GetDescString());

            // 设置弹出菜单列表
            PopupMenuDataManager.Instance.SetPopupItemData(m_AddMasterInfo, ref item);
        }

        // 处理拜师菜单项
        private void processRightClickAddPrentice(int nEntityID)
        {
            if (IsInternPrenticeFull())
                return;

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int nSelfLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

            if (nSelfLevel < MentorshipConfig.Instance.tMentorshipConfig.nMasterNeedLevel)
                return;

            EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
            if (ev == null || ev.Property == null)
                return;

            int nPDBID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (nPDBID == 0 || IsHaveMentorship(nPDBID))
                return;

            int nlevel = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

            if (nlevel > nSelfLevel)
                return;

            // 构造弹出菜单列表
            UListComponentItemDataPopup item = new UPopupItemDataApplyInternMaster(nPDBID, _strItemTypeName:
                UPopupItemDataApplyInternMaster.GetDescString());

            // 设置弹出菜单列表
            PopupMenuDataManager.Instance.SetPopupItemData(m_AddPrenticeInfo, ref item);
        }

        private void sortMasterList()
        {
            m_MasterList.Sort(
                (x, y) =>
                {
                    if (x.nRelationType != y.nRelationType)
                        return (x.nRelationType < y.nRelationType) ? -1 : 1;
                    else
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                }
                );
        }

        private void sortFPrenticeList()
        {
            m_FPrenticeList.Sort(
                (x, y) =>
                {
                    if (x.nIsOnline == y.nIsOnline)
                        return 0;
                    else
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                }
                );
        }

        private void sortIPrenticeList()
        {
            m_IPrenticeList.Sort(
                (x, y) =>
                {
                    if (x.nIsOnline == y.nIsOnline)
                        return 0;
                    else
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                }
                );
        }

        private void sortMasterFPrenticeList()
        {
            m_MasterFPrenticeList.Sort(
                (x, y) =>
                {
                    if (x.nIsOnline == y.nIsOnline)
                        return 0;
                    else
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                }
                );
        }

        private void sortMasterIPrenticeList()
        {
            m_MasterIPrenticeList.Sort(
                (x, y) =>
                {
                    if (x.nIsOnline == y.nIsOnline)
                        return 0;
                    else
                        return (x.nIsOnline > y.nIsOnline) ? -1 : 1;
                }
                );
        }

        public void OnRecvUpdatePersonBaseInfo(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_MentorshipPersonData = helper.get<GameLogic.cmd_mentorship_person_baseinfo>();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE, null);
        }

        public void OnRecvMentorshipIssueList(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_issuelist_head issueListHead = helper.get<GameLogic.cmd_mentorship_issuelist_head>();

            if (issueListHead.nIssueType == (int)EMMentorshipIssueType.emMentorshipIssueType_Master)
            {
               m_MasterIssueList.Clear();
               m_MasterIssueCount = issueListHead.nCount;
               for (int i = 0; i < issueListHead.nCount; ++i)
                {
                    GameLogic.cmd_mentorship_issueinfo cmdNodeData = helper.get<GameLogic.cmd_mentorship_issueinfo>();
                    m_MasterIssueList.Add(cmdNodeData);
                }

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_MASTERISSUELIST, null);
            }
            else
            {
               m_PrenticeIssueList.Clear();
               m_PrenticeIssueCount = issueListHead.nCount;
               for (int i = 0; i < issueListHead.nCount; ++i)
                {
                    GameLogic.cmd_mentorship_issueinfo cmdNodeData = helper.get<GameLogic.cmd_mentorship_issueinfo>();
                    m_PrenticeIssueList.Add(cmdNodeData);
                }

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST, null);
            }  
        }

        public void OnRecvMentorshipApplyAnswer(int cmdID, int nParam, string strParam, IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_request_applyanswer cmdRecvData = helper.get<GameLogic.cmd_mentorship_request_applyanswer>();

            ShowAskApplyMentorship(cmdRecvData);
        }

        public void OnRecvMentorshipDismiss(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_request_dismiss cmdRecvData = helper.get<GameLogic.cmd_mentorship_request_dismiss>();

            foreach (var fItem in m_FPrenticeList)
            {
                if (cmdRecvData.nMentorshipID == fItem.nMentorshipID)
                {
                    m_FPrenticeList.Remove(fItem);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, null);
                    return;
                }
            }
                
            foreach (var iItem in m_IPrenticeList)
            {
                if (cmdRecvData.nMentorshipID == iItem.nMentorshipID)
                {
                    m_IPrenticeList.Remove(iItem);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, null);
                    return;
                }
            }

            int nMasterPDBID = 0;
            foreach (var mItem in m_MasterList)
            {
                if (cmdRecvData.nMentorshipID == mItem.nMentorshipID)
                {
                    nMasterPDBID = mItem.nPDBID;
                    m_MasterList.Remove(mItem);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST, null);
                    break;
                }
            }

            if (nMasterPDBID == 0)
            {
                return;
            }

            bool bClearFlag = false;
            foreach (var mFItem in m_MasterFPrenticeList)
            {
                if (nMasterPDBID == mFItem.nMasterPDBID)
                {
                    bClearFlag = true;
                    break;
                }
            }
                
            if (bClearFlag == true)
            {
                m_MasterFPrenticeList.Clear();
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST, null);
            }

            bClearFlag = false;
            foreach (var mIItem in m_MasterIPrenticeList)
            {
                if (nMasterPDBID == mIItem.nMasterPDBID)
                {
                    bClearFlag = true;
                    break;
                }
            }
                
            if (bClearFlag == true)
            {
                m_MasterIPrenticeList.Clear();
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST, null);
            }
        }

        public void OnRecvUpdateMentorshipInfo(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_updateshipinfo cmdData = helper.get<GameLogic.cmd_mentorship_updateshipinfo>();

            for (int i = 0; i < m_MasterList.Count; ++i)
            {
                if (cmdData.nMentorshipID == m_MasterList[i].nMentorshipID)
                {
                    GameLogic.cmd_mentorship_personshipinfo shipNode = m_MasterList[i];
                    shipNode.nRelationType = cmdData.nRelationType;
                    shipNode.nState = cmdData.nState;
                    shipNode.nSeverPDBID = cmdData.nSeverPDBID;
                    shipNode.nSeverTime = cmdData.nSeverTime;
                    shipNode.szName = cmdData.szName;
                    m_MasterList[i] = shipNode;
                    sortMasterList();
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST, null);
                    return;
                }
            }

            for (int i = 0; i < m_FPrenticeList.Count; ++i)
            {
                if (m_FPrenticeList[i].nMentorshipID == cmdData.nMentorshipID)
                {
                    GameLogic.cmd_mentorship_personshipinfo shipNode = m_FPrenticeList[i];
                    shipNode.nRelationType = cmdData.nRelationType;
                    shipNode.nState = cmdData.nState;
                    shipNode.nSeverPDBID = cmdData.nSeverPDBID;
                    shipNode.nSeverTime = cmdData.nSeverTime;
                    shipNode.szName = cmdData.szName;
                    m_FPrenticeList[i] = shipNode;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, null);
                    return;
                }
            }

            for (int j = 0; j < m_IPrenticeList.Count; ++j)
            {
                if (cmdData.nMentorshipID == m_IPrenticeList[j].nMentorshipID)
                {
                    if (cmdData.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
                    {
                        GameLogic.cmd_mentorship_personshipinfo shipNode = m_IPrenticeList[j];
                        shipNode.nRelationType = cmdData.nRelationType;
                        shipNode.nState = cmdData.nState;
                        shipNode.nSeverPDBID = cmdData.nSeverPDBID;
                        shipNode.nSeverTime = cmdData.nSeverTime;
                        m_FPrenticeList.Add(shipNode);
                        m_IPrenticeList.Remove(m_IPrenticeList[j]);
                        sortFPrenticeList();
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, null);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, null);
                    }
                    else
                    {
                        GameLogic.cmd_mentorship_personshipinfo shipNode = m_IPrenticeList[j];
                        shipNode.nRelationType = cmdData.nRelationType;
                        shipNode.nState = cmdData.nState;
                        shipNode.nSeverPDBID = cmdData.nSeverPDBID;
                        shipNode.nSeverTime = cmdData.nSeverTime;
                        shipNode.szName = cmdData.szName;
                        m_IPrenticeList[j] = shipNode;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, null);
                    }
                    return;
                }
            }         
        }

        public void OnRecvUpdateMentorshipList(IntPtr ptr, int nLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_mentorshiplist_head mentorshipListHead = helper.get<GameLogic.cmd_mentorship_mentorshiplist_head>();

            m_MasterList.Clear();
            m_FPrenticeList.Clear();
            m_IPrenticeList.Clear();

            for (int i = 0; i < mentorshipListHead.nCount; ++i)
            {
                GameLogic.cmd_mentorship_personshipinfo cmdNodeData = helper.get<GameLogic.cmd_mentorship_personshipinfo>();
                if (cmdNodeData.nPost == (int)EMMentorshipPost.emMentorshipPost_Prentice)
                {
                    if(cmdNodeData.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
                    {
                        m_FPrenticeList.Add(cmdNodeData);
                    }
                    else
                    {
                        m_IPrenticeList.Add(cmdNodeData);
                    }
                }
                else if (cmdNodeData.nPost == (int)EMMentorshipPost.emMentorshipPost_Master)
                {
                    m_MasterList.Add(cmdNodeData);
                }
            }

            sortMasterList();
            sortFPrenticeList();
            sortIPrenticeList();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST, null);
        }

        public void OnRecvMasterPrenticeList(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_mentorship_prenticelist_head prenticeListHead = helper.get<GameLogic.cmd_mentorship_prenticelist_head>();

            m_MasterFPrenticeList.Clear();
            m_MasterIPrenticeList.Clear();

            for (int i = 0; i < prenticeListHead.nCount; ++i)
            {
                GameLogic.cmd_mentorship_prenticeinfo cmdNodeData = helper.get<GameLogic.cmd_mentorship_prenticeinfo>();
                if(cmdNodeData.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
                {
                    m_MasterFPrenticeList.Add(cmdNodeData);
                }
                else
                {
                    m_MasterIPrenticeList.Add(cmdNodeData);
                }
            }

            sortMasterFPrenticeList();
            sortMasterIPrenticeList();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST, null);
        }

        private bool IsHaveMentorship(int nDesPDBID)
        {
            foreach (var item in m_MasterList)
            {
                if (item.nPDBID == nDesPDBID)
                    return true;
            }

            foreach (var item in m_FPrenticeList)
            {
                if (item.nPDBID == nDesPDBID)
                    return true;
            }

            foreach (var item in m_IPrenticeList)
            {
                if (item.nPDBID == nDesPDBID)
                    return true;
            }

            return false;
        }

        public bool IsMentorshipDataValid()
        {
            return (m_MentorshipPersonData.nPDBID > 0);
        }

        public bool IsFormalMasterFull()
        {
            int nFormalCount = 0;
            foreach (var item in m_MasterList)
            {
                if (item.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
                {
                    ++nFormalCount;
                }
            }

            return nFormalCount >= MentorshipConfig.Instance.tMentorshipConfig.nFormalMasterCount;
        }

        public bool IsInternMasterFull()
        {
            int nInternCount = 0;
            foreach (var item in m_MasterList)
            {
                if (item.nRelationType == (int)EMMentorshipType.emMentorshipType_Intern)
                {
                    ++nInternCount;
                }
            }

            return nInternCount >= MentorshipConfig.Instance.tMentorshipConfig.nInternMasterCount;
        }

        public bool IsFormalPrenticeFull()
        {
            return m_FPrenticeList.Count >= MentorshipConfig.Instance.tMentorshipConfig.nFormalPrenticeCount;
        }

        public bool IsInternPrenticeFull()
        {
            return m_IPrenticeList.Count >= MentorshipConfig.Instance.tMentorshipConfig.nInternPrenticeCount;
        }

        public bool IsMApplyFreezing(int nPDBID)
        {
            if (!m_MApplyRecordList.ContainsKey(nPDBID))
                return false;

            if (m_MApplyRecordList[nPDBID] <= Time.unscaledTime)
            {
                m_MApplyRecordList.Remove(nPDBID);
                return false;
            }

            return true;
        }

        public bool IsPApplyFreezing(int nPDBID)
        {
            if (!m_PApplyRecordList.ContainsKey(nPDBID))
                return false;

            if (m_PApplyRecordList[nPDBID] <= Time.unscaledTime)
            {
                m_PApplyRecordList.Remove(nPDBID);
                return false;
            }

            return true;
        }

        public void AddMApplyRecord(int nPDBID, float fTime)
        {
            if (m_MApplyRecordList.ContainsKey(nPDBID))
            {
                m_MApplyRecordList[nPDBID] = fTime;
            }
            else
            {
                m_MApplyRecordList.Add(nPDBID, fTime);
            }
        }

        public void AddPApplyRecord(int nPDBID, float fTime)
        {
            if (m_PApplyRecordList.ContainsKey(nPDBID))
            {
                m_PApplyRecordList[nPDBID] = fTime;
            }
            else
            {
                m_PApplyRecordList.Add(nPDBID, fTime);
            }
        }

        public void SendDismissMentorship(int nMentorshipID)
        {
            GameLogic.cmd_mentorship_request_dismiss cmdData = new GameLogic.cmd_mentorship_request_dismiss();
            cmdData.nMentorshipID = nMentorshipID;
            ViewEventHelper.Instance.SendCommand<GameLogic.cmd_mentorship_request_dismiss>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTDISMISS, ref cmdData);
        }

        public void SendPromotePrentice(int nMentorshipID, int nDesPDBID)
        {
            GameLogic.cmd_mentorship_request_promote cmdData = new GameLogic.cmd_mentorship_request_promote();
            cmdData.nMentorshipID = nMentorshipID;
            cmdData.nDesPDBID = nDesPDBID;
            ViewEventHelper.Instance.SendCommand<GameLogic.cmd_mentorship_request_promote>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTPROMOTE, ref cmdData);
        }

        public void SendApplyFormalPrentice(int nDesPDBID)
        {
            GameLogic.cmd_mentorship_request_apply cmdData = new GameLogic.cmd_mentorship_request_apply();
            cmdData.nDesPDBID = nDesPDBID;
            cmdData.nRelationType = (int)EMMentorshipType.emMentorshipType_Formal;
            cmdData.nApplyPost = (int)EMMentorshipPost.emMentorshipPost_Prentice;
            ViewEventHelper.Instance.SendCommand<GameLogic.cmd_mentorship_request_apply>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP, ref cmdData);
        }

        public void SendApplyInternPrentice(int nDesPDBID)
        {
            GameLogic.cmd_mentorship_request_apply cmdData = new GameLogic.cmd_mentorship_request_apply();
            cmdData.nDesPDBID = nDesPDBID;
            cmdData.nRelationType = (int)EMMentorshipType.emMentorshipType_Intern;
            cmdData.nApplyPost = (int)EMMentorshipPost.emMentorshipPost_Prentice;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_apply>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP, ref cmdData);
        }

        public void SendApplyInternMaster(int nDesPDBID)
        {
            GameLogic.cmd_mentorship_request_apply cmdData = new GameLogic.cmd_mentorship_request_apply();
            cmdData.nDesPDBID = nDesPDBID;
            cmdData.nRelationType = (int)EMMentorshipType.emMentorshipType_Intern;
            cmdData.nApplyPost = (int)EMMentorshipPost.emMentorshipPost_Master;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_apply>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP, ref cmdData);
        }

        private void ShowAskApplyMentorship(GameLogic.cmd_mentorship_request_applyanswer cmdData)
        {
            if (m_ApplyAnwserList.ContainsKey(cmdData.nSrcPDBID))
                return;

            m_ApplyAnwserList.Add(cmdData.nSrcPDBID, cmdData);

            string szTmpPost = "";
            if (cmdData.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
            {
                if (cmdData.nApplyPost == (int)EMMentorshipPost.emMentorshipPost_Master)
                {
                    szTmpPost = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "FormalMasterDesc");
                }
                else
                {
                    szTmpPost = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "FormalPrenticeDesc");
                }
            }
            else
            {
                if (cmdData.nApplyPost == (int)EMMentorshipPost.emMentorshipPost_Prentice)
                {
                    szTmpPost = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "InternPrenticeDesc");
                }
                else
                {
                    szTmpPost = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "InternMasterDesc");
                }
            }

            string szMsg = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "ApplyNameDesc");
            szMsg = szMsg.Replace("{1}", cmdData.szSrcName);
            szMsg = szMsg.Replace("{2}", szTmpPost);
            string szAgreeDesc = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "AgreeApplyDesc");
            string szRefuseDesc = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "RefuseApplyDesc");

            DIDAAddData msg = new DIDAAddData();
            msg.eLayout = EDIDALayout.EDT_Middle;
            msg.nDIDATime = 60;
            msg.nItemID = (int)EDIDAType.EDIDA_Mentorship;
            msg.bAllowRepeatItem = true;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Mentorship;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "ApplyTitle");

            SideButtonDataMentorshipInfo applyInfo = new SideButtonDataMentorshipInfo();
            applyInfo.nSrcPDBID = cmdData.nSrcPDBID;
            msg.sCustomData = applyInfo;
            msg.callBack = (CustomerInfo) =>
            {
                SideButtonDataMentorshipInfo customerInfo = CustomerInfo as SideButtonDataMentorshipInfo;
                UIDialogData toolTipData = new UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "MentorshipWnd", "ApplyTitle"),
                        message: szMsg,
                        buttons: new UIWidgets.DialogActions() { { szAgreeDesc, () => { ApplyMentorshipCallBack(customerInfo.nSrcPDBID, true); return true; } }, { szRefuseDesc, () => { ApplyMentorshipCallBack(customerInfo.nSrcPDBID, false); return true; } } }
                        );

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        private bool ApplyMentorshipCallBack(int nSrcPDBID, bool bResult)
        {
            if (!m_ApplyAnwserList.ContainsKey(nSrcPDBID))
            {
                return false;
            }

            cmd_mentorship_request_applyanswer applyAnwser = m_ApplyAnwserList[nSrcPDBID];
            cmd_mentorship_request_applyanswer cmdData = new cmd_mentorship_request_applyanswer();
            cmdData.nSrcPDBID = applyAnwser.nSrcPDBID;
            cmdData.nRelationType = applyAnwser.nRelationType;
            cmdData.nApplyPost = applyAnwser.nApplyPost;
            cmdData.szSrcName = applyAnwser.szSrcName;
            cmdData.nAnwser = (byte)(bResult ? 1 : 0);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER, ref cmdData);

            m_ApplyAnwserList.Remove(nSrcPDBID);

            return true;
        }
    }
}
