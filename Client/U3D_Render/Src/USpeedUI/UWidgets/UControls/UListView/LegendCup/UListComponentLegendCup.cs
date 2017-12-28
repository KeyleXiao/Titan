using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using UnityEngine;
using ASpeedGame.Data.LegendCupConfig;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using GameLogic;
using LegendCup_ManagedDef;
using DataCenter;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UExtensions;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentLegendCup : ListViewItem
    {
        public GameObject ItemView;
        public Text ItemIndex;
        public Text CupName;
        public Text CreaterName;
        public Text CupConfigName;
        public Text CompetitionBonus;
        public Text Deadline;
        public Text KinNum;
        public Image LockIcon;
        public Image ClanIcon;
        public Image OnlineIcon;
        public Image BgIcon;
        public Image SelectIcon;
        public UTooltipTrigger LockIconTip;
        public UTooltipTrigger ClanIconTip;
        public UTooltipTrigger OnlineIconTip;

        private long m_lLegendCupID;
        public long LLegendCupID { get { return m_lLegendCupID; } }

        public virtual void SetData(UListItemLegendCup item)
        {
            if (item.nIndex <= 0)
            {
                BgIcon.gameObject.SetActive(true);
                ItemView.SetActive(false);
                BgIcon.raycastTarget = false;
                return;
            }
            BgIcon.gameObject.SetActive(true);
            BgIcon.raycastTarget = true;
            ItemView.SetActive(true);

            m_lLegendCupID = item.data.nLegendCupID;
            string strHour = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "Hour");
            string strMinute = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "Minute");
            LockIconTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupList", "LockIconTip"));
            ClanIconTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupList", "ClanIconTip"));
            OnlineIconTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupList", "OnlineIconTip"));
            ItemIndex.text = item.nIndex.ToString();
            CupName.text = item.data.szCupName;
            CreaterName.text = item.data.szCreateActorName;
            SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(item.data.nCupConfigID);
            CupConfigName.text = cupConfig.sCupConfigName;
            CompetitionBonus.text = item.data.nCompetitionBonus.ToString();
            if (item.data.nIsEnd > 0)
            {
                Deadline.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "EndDesc");
            }
            else if (item.data.nRegistLeftTime > 0)
            {
                int nDeadline = item.data.nRegistLeftTime / 60;
                int nDeadlineHour = nDeadline / 60;
                int nDeadlineMin = nDeadline % 60;
                Deadline.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "LeftTimeDesc") + nDeadlineHour + strHour + nDeadlineMin + strMinute;
            }
            else
            {
                Deadline.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CompetitioningDesc");
            }

            KinNum.text = item.data.nCurrentKinNum + "/" + item.data.nMaxKinNum;
            LockIcon.gameObject.SetActive(item.data.nIsLock > 0);
            ClanIcon.gameObject.SetActive(item.data.nIsClanLimit > 0);
            OnlineIcon.gameObject.SetActive(item.data.nIsOnline > 0);

            Color clr = new Color();
            if (item.data.nIsSelfIn > 0)
            {
                ColorUtility.TryParseHtmlString("#d1ac63", out clr);
            }
            else
            {
                ColorUtility.TryParseHtmlString("#ffffff", out clr);
            }
            ItemIndex.color = clr;
            CupName.color = clr;
            CreaterName.color = clr;
            CupConfigName.color = clr;
            CompetitionBonus.color = clr;
            Deadline.color = clr;
            KinNum.color = clr;
        }

        public virtual void SelectColoring()
        {
            // 选中
            SelectIcon.gameObject.SetActive(true);
        }

        public virtual void DefaultColoring()
        {
            // 未选中
            SelectIcon.gameObject.SetActive(false);
        }
    }

    public class UListComponentLegendCupRegist : ListViewItem
    {
        public GameObject ItemView;
        public Text ItemIndex;
        public Text KinName;
        public Text KinLevel;
        public Text KinCupResult;
        public Image BgIcon;
        public Image SelectIcon;

        public virtual void SetData(UListItemLegendCupRegist item)
        {
            if (item.nIndex <= 0)
            {
                BgIcon.gameObject.SetActive(true);
                ItemView.SetActive(false);
                BgIcon.raycastTarget = false;
                return;
            }
            BgIcon.gameObject.SetActive(true);
            BgIcon.raycastTarget = true;
            ItemView.SetActive(true);

            ItemIndex.text = item.nIndex.ToString();
            KinName.text = item.data.szKinName;
            KinLevel.text = item.data.nKinLevel.ToString();
            KinCupResult.text = item.data.nKinWinNum + "/" + item.data.nKinMaxNum;

            // 颜色设置
            //bool bInThisKin = LogicDataCenter.legendCupDataManager.CheckIsSelfInKinRegistMember(item.data.nKinID);
            int nSlefKinID = LogicDataCenter.kinDataManager.KinBaseData.nKinID;
            bool bInThisKin = nSlefKinID == item.data.nKinID;
            Color clrState = bInThisKin ? UDefines.CommomColor(ECommonColor.ECC_Gold1) : UDefines.CommomColor(ECommonColor.ECC_White);
            ItemIndex.color = clrState;
            KinName.color = clrState;
            KinLevel.color = clrState;
            KinCupResult.color = clrState;

            // 添加右键弹窗
            this.gameObject.RemoveComponent<UPopupMenuHandle>();
            UPopupMenuHandle popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            if (popupMenuHandle != null)
            {
                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataKinView.GetDescString(), new UPopupItemDataKinView(item.data.nKinID));
                popupAction.Add(UPopupItemDataRegistKinMemberView.GetDescString(), new UPopupItemDataRegistKinMemberView(item.data.nLegendCupID, item.data.nKinID));

                popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        //public virtual void SelectColoring()
        //{
        //    // 选中
        //    SelectIcon.gameObject.SetActive(true);
        //}

        //public virtual void DefaultColoring()
        //{
        //    // 未选中
        //    SelectIcon.gameObject.SetActive(false);
        //}
    }

    public class UListComponentLegendCupBlacklist : ListViewItem
    {
        public GameObject ItemView;
        public Text ItemIndex;
        public Text KinName;
        public Image BgIcon;
        public Image SelectIcon;

        public virtual void SetData(UListItemLegendCupBlacklist item)
        {
            if (item.nIndex <= 0)
            {
                BgIcon.gameObject.SetActive(true);
                ItemView.SetActive(false);
                BgIcon.raycastTarget = false;
                return;
            }
            BgIcon.gameObject.SetActive(true);
            BgIcon.raycastTarget = true;
            ItemView.SetActive(true);

            ItemIndex.text = item.nIndex.ToString();
            KinName.text = item.strKinName;
        }

        public virtual void SelectColoring()
        {
            // 选中
            SelectIcon.gameObject.SetActive(true);
        }

        public virtual void DefaultColoring()
        {
            // 未选中
            SelectIcon.gameObject.SetActive(false);
        }
    }

    public class LegendCupGroupKinItem : UCustomUIBehaviour
    {
        public Text KinName;

        public void SetData(legendcup_kininfo kinInfo)
        {
            KinName.text = kinInfo.szKinName;
            KinName.color = UDefines.CommomColor(ECommonColor.ECC_White);
            if (LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(kinInfo.nKinID))
            {
                KinName.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }

            // 添加右键弹窗
            KinName.gameObject.RemoveComponent<UPopupMenuHandle>();
            UPopupMenuHandle popupMenuHandle = KinName.gameObject.AddComponent<UPopupMenuHandle>();
            if (popupMenuHandle != null)
            {
                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataKinView.GetDescString(), new UPopupItemDataKinView(kinInfo.nKinID));
                popupAction.Add(UPopupItemDataCompetitionKinMemberView.GetDescString(), new UPopupItemDataCompetitionKinMemberView(kinInfo.nKinID));

                popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        public void Clear()
        {
            KinName.text = "";
        }
    }

    public class LegendCupBigKnockoutKinItem : UCustomUIBehaviour
    {
        public Text Kin1Name;
        public Text Kin2Name;
        public Text VSDes;

        public void SetData(legendcup_kinvsinfo kinInfo)
        {
            Kin1Name.text = kinInfo.szKin1Name;
            Kin2Name.text = kinInfo.szKin2Name;
            VSDes.text = "vs";
            SetPopupMenu(Kin1Name.gameObject, kinInfo.nKin1ID);
            SetPopupMenu(Kin2Name.gameObject, kinInfo.nKin2ID);
        }

        public void Clear()
        {
            Kin1Name.text = "";
            Kin2Name.text = "";
            VSDes.text = "vs";
        }

        private void SetPopupMenu(GameObject Go, int nKinID)
        {
            // 添加右键弹窗
            Go.RemoveComponent<UPopupMenuHandle>();
            UPopupMenuHandle popupMenuHandle = Go.AddComponent<UPopupMenuHandle>();
            if (popupMenuHandle != null)
            {
                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataKinView.GetDescString(), new UPopupItemDataKinView(nKinID));
                popupAction.Add(UPopupItemDataCompetitionKinMemberView.GetDescString(), new UPopupItemDataCompetitionKinMemberView(nKinID));

                popupMenuHandle.HandlePopupAction = popupAction;
            }
        }
    }

    public enum ECompeitionNodeType
    {
        ECNT_GROUP,
        ECNT_BIGKNOCK,

        ECNT_MAX,
    }

    public class LegendCupCompetitionNodeItem : UCustomUIBehaviour
    {
        public Text Data;
        public Text Time;
        public Text Kin1Name;
        public Text VSDes;
        public Text Kin2Name;
        public Text NodeState;
        //public Text GotoBtnDes;
        public Text ViewCompetitionBtn;
        public Image Kin1WinIcon;
        public Image Kin2WinIcon;
        //public Button GotoBtn;
        public Image BgIcon;
        public GameObject SelectIconAttach;
        public GameObject ViewItem;

        private long m_legendCupID;
        private int m_roundID;
        private int m_searchID;
        public int SearchID { get { return m_searchID; } }
        private cmd_legendcup_recv_competition_node m_nodeInfo;
        public cmd_legendcup_recv_competition_node NodeInfo { get { return m_nodeInfo; } }
        private string strMonth;
        private string strDay;
        //private string strGotoBtn;
        private bool bInThisNode;

        protected override void Awake()
        {
            base.Awake();
            strMonth = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Month");
            strDay = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Day");
            //strGotoBtn = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GotoBtn");
            ViewCompetitionBtn.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "ViewCompetitionBtn");
        }

        public void SetData(long nCupID, cmd_legendcup_recv_competition_node nodeInfo, int nNodeType)
        {
            m_legendCupID = nCupID;
            m_roundID = nodeInfo.nRoundID;
            m_searchID = nodeInfo.nSearchID;
            m_nodeInfo = nodeInfo;

            bInThisNode = LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(nodeInfo.nKin1ID) || LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(nodeInfo.nKin2ID);
            
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_legendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 基本设置
            ViewItem.SetActive(true);
            BgIcon.raycastTarget = true;
            DateTime enterTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nodeInfo.nEnterTime);
            enterTime = enterTime.ToLocalTime();
            DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nodeInfo.nBeginTime);
            beginTime = beginTime.ToLocalTime();
            Data.text = enterTime.Month.ToString("D2") + strMonth + enterTime.Day.ToString("D2") + strDay;
            Time.text = String.Format("{0:D2}:{1:D2} - {2:D2}:{3:D2}", enterTime.Hour, enterTime.Minute, beginTime.Hour, beginTime.Minute);
            Kin1Name.text = nodeInfo.szKin1Name;
            VSDes.text = "VS";
            Kin2Name.text = nodeInfo.szKin2Name;
            //GotoBtnDes.text = strGotoBtn;

            // 考虑到排版，只改变透明度不隐藏，弃权的不显示比分
            bool bKin1Win = nodeInfo.nKin1ID == nodeInfo.nWinnerKinID && nodeInfo.nKin1Score > nodeInfo.nKin2Score;
            bool bKin2Win = nodeInfo.nKin2ID == nodeInfo.nWinnerKinID && nodeInfo.nKin2Score > nodeInfo.nKin1Score;
            Kin1WinIcon.color = bKin1Win ? new Color(1, 1, 1, 1) : new Color(1, 1, 1, 0);
            Kin2WinIcon.color = bKin2Win ? new Color(1, 1, 1, 1) : new Color(1, 1, 1, 0);

            // 设置GotoBtn
            //bool bCanJoin = bInThisNode && (nodeInfo.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter);
            //Color clr = new Color();
            //if (bCanJoin)
            //{
            //    SetEnterBtnState(true);
            //    ColorUtility.TryParseHtmlString("#ffffff", out clr);
            //}
            //else
            //{
            //    SetEnterBtnState(false);
            //    ColorUtility.TryParseHtmlString("#bebebe", out clr);
            //}
            //GotoBtnDes.color = clr;
            
            // 设置NodeState
            NodeState.gameObject.SetActive(true);
            switch (nodeInfo.byCompetitionNodeState)
            {  
                case (byte)ECompetitionNodeState.emNodeState_WaitStart:// 等待开始
                case (byte)ECompetitionNodeState.emNodeState_CanEnter: // 等待进入
                    {
                        NodeState.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "NoCompetition");
                    }
                    break;
                case (byte)ECompetitionNodeState.emNodeState_Competition:// 进行中
                    {
                        NodeState.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Competitioning");
                    }
                    break;
                case (byte)ECompetitionNodeState.emNodeState_WaitJudge:// 等待裁定
                    {
                        NodeState.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "WaitJudge");
                    }
                    break;
                case (byte)ECompetitionNodeState.emNodeState_End:// 比赛已结束
                    {
                        NodeState.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionEnd");
                    }
                    break;
                default:
                    break;
            }

            // 设置颜色
            Color clrState = UDefines.CommomColor(ECommonColor.ECC_White);
            if (bInThisNode && nodeInfo.nWinnerKinID <= 0)
            {
                clrState = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
            Data.color = clrState;
            Time.color = clrState;
            Kin1Name.color = clrState;
            VSDes.color = clrState;
            Kin2Name.color = clrState;
            // 完成态显示红色
            NodeState.color = nodeInfo.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_End ? UDefines.CommomColor(ECommonColor.ECC_Red1) : clrState;
        }

        public void Clear()
        {
            ViewItem.SetActive(false);
            BgIcon.raycastTarget = false;
        }

        //public void SetEnterBtnState(bool bVisible)
        //{
        //    GotoBtn.interactable = bVisible;
        //}

        //public void OnClickGotoBtn()
        //{
        //    if (m_legendCupID == 0 || m_roundID == 0 || m_searchID == 0)
        //    {
        //        return;
        //    }

        //    cmd_legendcup_enter_competition_node nodeData = new cmd_legendcup_enter_competition_node();
        //    nodeData.nLegendCupID = m_legendCupID;
        //    nodeData.nRoundID = m_roundID;
        //    nodeData.nSearchID = m_searchID;
        //    ViewEventHelper.Instance.SendCommand<cmd_legendcup_enter_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE, ref nodeData);

        //    // 删除DIDA提醒
        //    LogicDataCenter.legendCupDataManager.SetLegendCupDIDA(false, m_legendCupID, m_roundID, m_searchID);
        //}

        public void OnClickViewCompetitionBtn()
        {
            if (m_searchID == 0)
            {
                return;
            }

            LegendCupNodeWarData nodeData = new LegendCupNodeWarData();
            nodeData.nLegendCupID = m_legendCupID;
            nodeData.nRoundID = m_roundID;
            nodeData.nSearchID = m_searchID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, nodeData);
        }
    }

    public class UlistComponentLegendCupGroup : ListViewItem
    {
        public Text GroupIndex;
        public RectTransform GroupMemberFrame;
        public GameObject GroupMemberTemplate;
        public RectTransform CompetitionNodeFrame;
        public GameObject CompetitionNodeTemplate;

        const int KIN_ITEM_COUNT = 4;
        private LegendCupGroupKinItem[] m_kinItemList;
        public LegendCupGroupKinItem[] KinItemList { get { return m_kinItemList; } }
        const int NODE_ITEM_COUNT = 6;
        private LegendCupCompetitionNodeItem[] m_nodeItemList;
        public LegendCupCompetitionNodeItem[] NodeItemList { get { return m_nodeItemList; } }

        protected override void Awake()
        {
            base.Awake();
            // GroupMemberFrame
            m_kinItemList = new LegendCupGroupKinItem[KIN_ITEM_COUNT];
            for (int i = 0; i < KIN_ITEM_COUNT; ++i)
            {
                GameObject memberItemObj = ResNode.InstantiateRes(GroupMemberTemplate);
                if (!memberItemObj)
                    return;
                memberItemObj.name = "GroupMemberItem" + (i + 1);
                RectTransform rectTrans = memberItemObj.transform as RectTransform;
                rectTrans.SetParent(GroupMemberFrame, false);

                m_kinItemList[i] = memberItemObj.GetComponent<LegendCupGroupKinItem>();
                if (!m_kinItemList[i])
                    return;
            }
            GroupMemberTemplate.SetActive(false);

            // CompetitionNodeFrame
            m_nodeItemList = new LegendCupCompetitionNodeItem[NODE_ITEM_COUNT];
            for (int j = 0; j < NODE_ITEM_COUNT; ++j)
            {
                GameObject nodeItemObj = ResNode.InstantiateRes(CompetitionNodeTemplate);
                if (!nodeItemObj)
                    return;
                nodeItemObj.name = "CompetitionNodeItem" + (j + 1);
                RectTransform rectTrans = nodeItemObj.transform as RectTransform;
                rectTrans.SetParent(CompetitionNodeFrame, false);

                m_nodeItemList[j] = nodeItemObj.GetComponent<LegendCupCompetitionNodeItem>();
                if (!m_nodeItemList[j])
                    return;
            }
            CompetitionNodeTemplate.SetActive(false);
        }

        public virtual void SetData(UListItemLegendCupGroup item)
        {
            // 组名
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            GroupIndex.text = item.strGroupIndex;
            GroupIndex.color = UDefines.CommomColor(ECommonColor.ECC_White);
            foreach (var itemKin in item.kinInfoList)
            {
                if (LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(itemKin.nKinID))
                {
                    GroupIndex.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                    break;
                }
            }
            
            // GroupMemberFrame
            for (int i = 0; i < KIN_ITEM_COUNT; ++i)
            {
                if (item.kinInfoList[i].nKinID != 0)
                {
                    m_kinItemList[i].SetData(item.kinInfoList[i]);
                }
                else
                {
                    m_kinItemList[i].Clear();
                }
            }

            // CompetitionNodeFrame
            for (int j = 0; j < NODE_ITEM_COUNT; ++j)
            {
                if (j < item.competitionNodeList.Count)
                {
                    m_nodeItemList[j].SetData(item.nLegendCupID, item.competitionNodeList[j], (int)ECompeitionNodeType.ECNT_GROUP);
                }
                else
                {
                    m_nodeItemList[j].Clear();
                }
            }
        }
    }

    public class UlistComponentLegendCupBigKnockout : ListViewItem
    {
        public Text GroupIndex;
        public RectTransform GroupMemberFrame;
        public GameObject GroupMemberTemplate;
        public RectTransform CompetitionNodeFrame;
        public GameObject CompetitionNodeTemplate;

        const int KIN_ITEM_COUNT = 2;
        private LegendCupBigKnockoutKinItem[] m_kinItemList;
        public LegendCupBigKnockoutKinItem[] KinItemList { get { return m_kinItemList; } }
        const int NODE_ITEM_COUNT = 2;
        private LegendCupCompetitionNodeItem[] m_nodeItemList;
        public LegendCupCompetitionNodeItem[] NodeItemList { get { return m_nodeItemList; } }

        protected override void Awake()
        {
            base.Awake();

            // GroupMemberFrame
            m_kinItemList = new LegendCupBigKnockoutKinItem[KIN_ITEM_COUNT];
            for (int i = 0; i < KIN_ITEM_COUNT; ++i)
            {
                GameObject memberItemObj = ResNode.InstantiateRes(GroupMemberTemplate);
                if (!memberItemObj)
                    return;
                memberItemObj.name = "GroupMemberItem" + (i + 1);
                RectTransform rectTrans = memberItemObj.transform as RectTransform;
                rectTrans.SetParent(GroupMemberFrame, false);

                m_kinItemList[i] = memberItemObj.GetComponent<LegendCupBigKnockoutKinItem>();
                if (!m_kinItemList[i])
                    return;

            }
            GroupMemberTemplate.SetActive(false);

            // CompetitionNodeFrame
            m_nodeItemList = new LegendCupCompetitionNodeItem[NODE_ITEM_COUNT];
            for (int j = 0; j < NODE_ITEM_COUNT; ++j)
            {
                GameObject nodeItemObj = ResNode.InstantiateRes(CompetitionNodeTemplate);
                if (!nodeItemObj)
                    return;
                nodeItemObj.name = "CompetitionNodeItem" + (j + 1);
                RectTransform rectTrans = nodeItemObj.transform as RectTransform;
                rectTrans.SetParent(CompetitionNodeFrame, false);

                m_nodeItemList[j] = nodeItemObj.GetComponent<LegendCupCompetitionNodeItem>();
                if (!m_nodeItemList[j])
                    return;
            }
            CompetitionNodeTemplate.SetActive(false);
        }

        public virtual void SetData(UListItemLegendCupBigKnockout item)
        {
            GroupIndex.text = item.strGroupIndex;

            // GroupMemberFrame
            for (int i = 0; i < item.kinVsInfoList.Count; ++i)
            {
                if (item.kinVsInfoList[i].nKin1ID != 0 && item.kinVsInfoList[i].nKin2ID != 0)
                {
                    m_kinItemList[i].SetData(item.kinVsInfoList[i]);
                }
                else
                {
                    m_kinItemList[i].Clear();
                }
            }

            // CompetitionNodeFrame
            for (int j = 0; j < NODE_ITEM_COUNT; ++j)
            {
                if (j < item.competitionNodeList.Count)
                {
                    m_nodeItemList[j].SetData(item.nLegendCupID, item.competitionNodeList[j], (int)ECompeitionNodeType.ECNT_BIGKNOCK);
                }
                else
                {
                    m_nodeItemList[j].Clear();
                }
            }
        }
    }

}
