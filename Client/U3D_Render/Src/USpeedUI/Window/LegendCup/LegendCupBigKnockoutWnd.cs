using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using GameLogic;
using UIWidgets;
using UnityEngine;
using ASpeedGame.Data.LegendCupConfig;

namespace USpeedUI.LegendCup
{
    public class LegendCupBigKnockoutWnd : UIPopupWnd<LegendCupBigKnockoutWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_BIGKNOCKOUT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupBigKnockoutView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowBigKnockoutWnd(cmdData.nLegendCupID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_BIGKNOCKOUT_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowBigKnockoutWnd(cmdData.nLegendCupID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }
            return base.OnCloseKey();
        }
    }

    public class LegendCupBigKnockoutWndView : UIBaseWndView//, ITimerHanlder
    {
        public Text TitleDes;
        public Text GroupDes;
        public Text DataDes;
        public Text TimeDes;
        public Text AgainstDes;

        public UListLegendCupBigKnockout LegendCupBigKnockList;
        public Image SelectImage;
        private int m_CurSelectIndex;
        private UListItemLegendCupBigKnockout m_CurSelectItem;
        private int m_CurSearchID;

        public URichText CupBigKnockTip;
        public Text KinNumDes;
        public Text KinNum;
        public Text CompetitionBonus;
        public URichText CompetitionAward;
        public Text ViewCompetitionBtnDes;

        private long m_LegendCupID;
        private int m_RoundID;
        private int m_SelfKinID;
        private List<string> m_GroupIndex;
        private List<UListItemLegendCupBigKnockout> itemList;
        //private bool m_EnterBtnFlag;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            GroupDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Group");
            DataDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Data");
            TimeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Time");
            AgainstDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Against");
            KinNumDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "KinNum");
            ViewCompetitionBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "ViewCompetitionBtn");

            CupBigKnockTip.text = "<a href=CupBigKnockTip color=bbbbbc>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CupGroupTip") + "</a>";
            CupBigKnockTip.onHrefClick.AddListener(OnClickCupBigKnockoutRT);

            CompetitionBonus.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionBonus") + ":-";
            CompetitionAward.text = "<a href=CompetitionAward color=38db47>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionAward") + "</a>";
            CompetitionAward.onHrefClick.AddListener(OnClickCompetitionAwardRT);

            string strGroupA = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_A");
            string strGroupB = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_B");
            string strGroupC = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_C");
            string strGroupD = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_D");
            string strGroupE = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_E");
            string strGroupF = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_F");
            string strGroupG = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_G");
            string strGroupH = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupIndex_H");
            m_GroupIndex = new List<string> { strGroupA, strGroupB, strGroupC, strGroupD, strGroupE, strGroupF, strGroupG, strGroupH };

            return true;
        }

        public void OnShowBigKnockoutWnd(long nCupID)
        {
            m_LegendCupID = nCupID;
            SingleCompetitionInfo competitionInfo = LogicDataCenter.legendCupDataManager.GetSingleCompetitionInfo();
            if (competitionInfo == null)
                return;
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_LegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 数据赋值部分
            CompetitionBonus.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionBonus") + ":" + cupBaseData.nCompetitionBonus;
            m_RoundID = competitionInfo.headInfo.nRoundID;
            TitleDes.text = cupBaseData.szCupName;
            KinNum.text = cupBaseData.nCurrentKinNum + "/" + cupBaseData.nMaxKinNum;
            itemList = new List<UListItemLegendCupBigKnockout>(competitionInfo.headInfo.nGroupCount);
            for (int i = 0; i < competitionInfo.headInfo.nGroupCount; i++)
            {
                UListItemLegendCupBigKnockout item = new UListItemLegendCupBigKnockout();
                item.nLegendCupID = nCupID;
                if (i >= 0 && i < m_GroupIndex.Count)
                {
                    item.strGroupIndex = m_GroupIndex[i];
                }
                item.kinVsInfoList = new List<legendcup_kinvsinfo>();
                item.competitionNodeList = new List<cmd_legendcup_recv_competition_node>();
                foreach (cmd_legendcup_recv_competition_node node in competitionInfo.nodeInfo)
                {
                    if (node.nRoundID == competitionInfo.headInfo.nRoundID && node.nGroupID == (i + 1))
                    {
                        legendcup_kinvsinfo info = new legendcup_kinvsinfo();
                        info.nKin1ID = node.nKin1ID;
                        info.nKin2ID = node.nKin2ID;
                        info.szKin1Name = node.szKin1Name;
                        info.szKin2Name = node.szKin2Name;
                        item.kinVsInfoList.Add(info);

                        item.competitionNodeList.Add(node);
                    }
                }
                itemList.Add(item);
            }

            LegendCupBigKnockList.OnSelect.RemoveListener(OnSelectCupBigKnockoutItem);
            LegendCupBigKnockList.DataSource.Clear();
            ObservableList<UListItemLegendCupBigKnockout> dataSource = new ObservableList<UListItemLegendCupBigKnockout>();
            foreach (UListItemLegendCupBigKnockout item in itemList)
            {
                dataSource.Add(item);
            }
            LegendCupBigKnockList.DataSource = dataSource;
            LegendCupBigKnockList.OnSelect.AddListener(OnSelectCupBigKnockoutItem);
        }

        public void OnClickCompetitionAwardRT(string eventName)
        {
            if (eventName == "CompetitionAward")
            {
                LegendCupIDData uiData = new LegendCupIDData();
                uiData.nLegendCupID = m_LegendCupID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW, uiData);
            }
        }

        public void OnClickCupBigKnockoutRT(string eventName)
        {
            if (eventName == "CupBigKnockTip")
            {
                cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_LegendCupID);
                if (cupBaseData.nLegendCupID == 0)
                    return;

                UIMsgCmdData uiCupTipData = null;
                if (cupBaseData.nIsOnline > 0)
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OnLine_Cup32In, "", IntPtr.Zero, 0);
                else
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OffLine_Cup32In, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, uiCupTipData);
            }
        }

        public void OnSelectCupBigKnockoutItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupBigKnockList.DataSource[index];

            m_CurSearchID = 0;
            SelectImage.gameObject.SetActive(false);
            Vector2 mousePos = UISystem.Instance.GetMousePos();
            UlistComponentLegendCupBigKnockout compGroup = item as UlistComponentLegendCupBigKnockout;
            foreach (LegendCupCompetitionNodeItem nodeItem in compGroup.NodeItemList)
            {
                RectTransform rectTrans = nodeItem.gameObject.GetComponent<RectTransform>();
                if (RectTransformUtility.RectangleContainsScreenPoint(rectTrans, mousePos))
                {
                    m_CurSearchID = nodeItem.SearchID;
                    SelectImage.rectTransform.SetParent(nodeItem.SelectIconAttach.GetComponent<RectTransform>(), false);
                    SelectImage.gameObject.SetActive(true);
                    Debug.Log("Select Competition Node! LegendCupID = " + m_LegendCupID + ", SearchID = " + m_CurSearchID + ", Kin1ID = " + nodeItem.NodeInfo.nKin1ID + ", Kin2ID = " + nodeItem.NodeInfo.nKin2ID);
                }
            }
        }

        public void OnClickViewCompetitionBtn()
        {
            if (m_CurSearchID == 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_COMPETITION_NODE);
                return;
            }

            LegendCupNodeWarData nodeData = new LegendCupNodeWarData();
            nodeData.nLegendCupID = m_LegendCupID;
            nodeData.nRoundID = m_RoundID;
            nodeData.nSearchID = m_CurSearchID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, nodeData);
        }

        public void OnClickCloseBtn()
        {
            //TimerManager.KillTimer(this, 1);

            cmd_legendcup_quit_observe observeData = new cmd_legendcup_quit_observe();
            observeData.nLegendCupID = m_LegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_quit_observe>(GameLogicDef.GVIEWCMD_LEGENDCUP_QUITOBSERVE, ref observeData);

            m_wnd.UnloadView();
        }
    }
}
