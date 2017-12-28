using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using ASpeedGame.Data.LegendCupConfig;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgets;
using UnityEngine;
using USpeedUI.UEffect;
using LegendCup_ManagedDef;
using DG.Tweening;

namespace USpeedUI.LegendCup
{
    public class LegendCupGroupWnd : UIPopupWnd<LegendCupGroupWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_GROUP;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupGroupView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_GROUP_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_GROUP_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowGroupWnd(cmdData.nLegendCupID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_GROUP_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowGroupWnd(cmdData.nLegendCupID);
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

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }
    }

    public class LegendCupGroupWndView : UIBaseWndView//, ITimerHanlder
    {
        public Text SysTitleDes;
        public Text CreaterDes;
        public Text TitleDes;
        public Text GroupDes;
        public Text DataDes;
        public Text TimeDes;
        public Text AgainstDes;

        public UListLegendCupGroup LegendCupGroupList;
        public Image SelectImage;
        private int m_CurSelectIndex;
        private UListItemLegendCupGroup m_CurSelectItem;
        private int m_CurSearchID;

        public URichText CupGroupTip;
        public Text KinNumDes;
        public Text KinNum;
        public Text CompetitionBonus;
        public URichText CompetitionAward;
        //public Text ViewCompetitionBtnDes;
        public Text GotoBtnDes;
        public Text ReturnBtnDes;
        public Button GotoBtn;
        public Text RoundTimeDesc;

        private long m_LegendCupID;
        private int m_RoundID;
        private int m_GotoSearchID;
        private List<string> m_GroupIndex;
        private List<UListItemLegendCupGroup> itemList;

        // 标题光效
        public GameObject AttachPoint1;
        public GameObject AttachPoint2;
        private UEffectParamBase param1;
        private UEffectParamBase param2;
        public Image CatchBtnEffect;
        private Sequence CatchBtnEffectSequence;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            //SysTitleDes.text = ULocalizationService.Instance.Get("UIView", "Common", "OpenMatchSystem");
            GroupDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Group");
            DataDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Data");
            TimeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Time");
            AgainstDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "Against");
            KinNumDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "KinNum");
            //ViewCompetitionBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "ViewCompetitionBtn");
            GotoBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GotoBtn");
            ReturnBtnDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");

            CupGroupTip.text = "<a href=CupGroupTip color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CupGroupTip") + "</a>";
            CupGroupTip.onHrefClick.AddListener(OnClickCupGroupRT);

            CompetitionBonus.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionBonus") + ":-";
            CompetitionAward.text = "<a href=CompetitionAward color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionAward") + "</a>";
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

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
                param1 = null;
            }
            param1 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01, _tfAttachParent: AttachPoint1.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param1);
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
                param2 = null;
            }
            param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02, _tfAttachParent: AttachPoint2.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);

            return true;
        }

        public void OnShowGroupWnd(long nCupID)
        {
            m_LegendCupID = nCupID;
            SingleCompetitionInfo competitionInfo = LogicDataCenter.legendCupDataManager.GetSingleCompetitionInfo(m_LegendCupID);
            if (competitionInfo == null)
                return;
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_LegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 数据赋值部分
            SysTitleDes.text = cupBaseData.szCupName;
            CreaterDes.gameObject.SetActive(!String.IsNullOrEmpty(cupBaseData.szCreateActorName));
            CreaterDes.text = string.Format("举办人：{0}", cupBaseData.szCreateActorName);
            CompetitionBonus.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionBonus") + ":" + cupBaseData.nCompetitionBonus;
            m_RoundID = competitionInfo.headInfo.nRoundID;
            TitleDes.text = cupBaseData.szCupName;
            KinNum.text = cupBaseData.nCurrentKinNum + "/" + cupBaseData.nMaxKinNum;
            itemList = new List<UListItemLegendCupGroup>(competitionInfo.headInfo.nGroupCount);
            for (int i = 0; i < competitionInfo.headInfo.nGroupCount; i++)
            {
                UListItemLegendCupGroup item = new UListItemLegendCupGroup();
                item.nLegendCupID = nCupID;
                if (i >= 0 && i < m_GroupIndex.Count)
                { 
                    item.strGroupIndex = m_GroupIndex[i];
                }
                item.kinInfoList = new List<legendcup_kininfo>();
                for (int j = 0; j < competitionInfo.headInfo.nGroupMaxKin; j++)
                {
                    legendcup_kininfo info = new legendcup_kininfo();
                    info.nKinID = competitionInfo.headInfo.kinInfos[i * competitionInfo.headInfo.nGroupMaxKin + j].nKinID;
                    info.szKinName = competitionInfo.headInfo.kinInfos[i * competitionInfo.headInfo.nGroupMaxKin + j].szKinName;
                    item.kinInfoList.Add(info);
                }
                item.competitionNodeList = new List<cmd_legendcup_recv_competition_node>();
                foreach (cmd_legendcup_recv_competition_node node in competitionInfo.nodeInfo)
                {
                    if (node.nRoundID == competitionInfo.headInfo.nRoundID && node.nGroupID == (i + 1))
                    {
                        item.competitionNodeList.Add(node);
                    }
                }
                itemList.Add(item);
            }

            LegendCupGroupList.onDoubleClick.RemoveListener(OnDoubleClickGroupItem);
            LegendCupGroupList.OnSelect.RemoveListener(OnSelectCupGroupItem);
            LegendCupGroupList.DataSource.Clear();
            ObservableList<UListItemLegendCupGroup> dataSource = new ObservableList<UListItemLegendCupGroup>();
            foreach (UListItemLegendCupGroup item in itemList)
            {
                dataSource.Add(item);
            }

            LegendCupGroupList.DataSource = dataSource;
            LegendCupGroupList.OnSelect.AddListener(OnSelectCupGroupItem);
            LegendCupGroupList.onDoubleClick.AddListener(OnDoubleClickGroupItem);

            // 状态
            string strMonth = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Month");
            string strDay = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Day");
            int nRoundEndTime = 0;
            if (competitionInfo.roundInfo.Count > m_RoundID - 1)
            {
                nRoundEndTime = competitionInfo.roundInfo[m_RoundID - 1].nRoundEndTime;
            }
            DateTime roundEndTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nRoundEndTime);
            roundEndTime = roundEndTime.ToLocalTime();
            RoundTimeDesc.text = String.Format("{0}:{1}{2}{3}{4}{5:D2}:{6:D2}", ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "GroupEndTime"), roundEndTime.Month, strMonth, roundEndTime.Day, strDay, roundEndTime.Hour, roundEndTime.Minute);
            
            foreach (var item1 in competitionInfo.nodeInfo)
            {
                if (LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item1.nKin1ID) || LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item1.nKin2ID))
                {
                    if (item1.byCompetitionNodeState <= (int)ECompetitionNodeState.emNodeState_Competition)//进行中
                    {
                        DateTime enterTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item1.nEnterTime);
                        enterTime = enterTime.ToLocalTime();
                        DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item1.nBeginTime);
                        beginTime = beginTime.ToLocalTime();
                        RoundTimeDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourNextCup") +
                            enterTime.Month + strMonth + enterTime.Day + strDay +
                            String.Format("{0:D2}:{1:D2} - {2:D2}:{3:D2}", enterTime.Hour, enterTime.Minute, beginTime.Hour, beginTime.Minute);
                        break;
                    }
                }
            }

            // 按钮
            GotoBtn.interactable = false;
            GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_Gray);

            CatchBtnEffect.color = Color.clear;
            if (CatchBtnEffectSequence != null)
                CatchBtnEffectSequence.Kill(true);

            foreach (var item2 in competitionInfo.nodeInfo)
            {
                if (LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item2.nKin1ID) || LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item2.nKin2ID))
                {
                    if (item2.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter)
                    {
                        GotoBtn.interactable = true;
                        GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_White);
                        m_GotoSearchID = item2.nSearchID;

                        Color baseColor = Color.white;
                        baseColor.a = 0f;
                        CatchBtnEffect.color = baseColor;
                        CatchBtnEffectSequence = DOTween.Sequence();
                        CatchBtnEffectSequence.Append(DOTween.ToAlpha(() => CatchBtnEffect.color, x => CatchBtnEffect.color = x, 1f, 0.5f));
                        CatchBtnEffectSequence.Append(DOTween.ToAlpha(() => CatchBtnEffect.color, x => CatchBtnEffect.color = x, 0f, 0.5f).SetDelay(0.2f));
                        CatchBtnEffectSequence.SetLoops(-1, LoopType.Restart);
                        CatchBtnEffectSequence.OnKill(() => CatchBtnEffectSequence = null);

                        DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item2.nBeginTime);
                        beginTime = beginTime.ToLocalTime();
                        RoundTimeDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeCanEnter"), beginTime.Hour, beginTime.Minute);

                        break;
                    }
                }
            }
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

        public void OnClickCupGroupRT(string eventName)
        {
            if (eventName == "CupGroupTip")
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

        public void OnSelectCupGroupItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupGroupList.DataSource[index];

            m_CurSearchID = 0;
            SelectImage.gameObject.SetActive(false);
            Vector2 mousePos = UISystem.Instance.GetMousePos();
            UlistComponentLegendCupGroup compGroup = item as UlistComponentLegendCupGroup;
            foreach (LegendCupCompetitionNodeItem nodeItem in compGroup.NodeItemList)
            {
                RectTransform rectTrans = nodeItem.gameObject.GetComponent<RectTransform>();
                if (RectTransformUtility.RectangleContainsScreenPoint(rectTrans, mousePos, UISystem.Instance.GetCamera()))
                {
                    m_CurSearchID = nodeItem.SearchID;
                    if (m_CurSearchID <= 0)
                        return;
                    
                    SelectImage.rectTransform.SetParent(nodeItem.SelectIconAttach.GetComponent<RectTransform>(), false);
                    SelectImage.gameObject.SetActive(true);

                    // GM账号，进入比赛入口状态更新
                    int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                    if (GameLogicAPI.bIsGM(nPDBID))
                    {
                        if (nodeItem.NodeInfo.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter)
                        {
                            GotoBtn.interactable = true;
                            GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_White);
                            m_GotoSearchID = m_CurSearchID;
                        }
                        else
                        {
                            GotoBtn.interactable = false;
                            GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_Gray);
                            m_GotoSearchID = 0;
                        }
                    }
                }
            }
        }

        public void OnDoubleClickGroupItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupGroupList.DataSource[index];

            m_CurSearchID = 0;
            SelectImage.gameObject.SetActive(false);
            Vector2 mousePos = UISystem.Instance.GetMousePos();
            UlistComponentLegendCupGroup compGroup = item as UlistComponentLegendCupGroup;
            foreach (LegendCupCompetitionNodeItem nodeItem in compGroup.NodeItemList)
            {
                RectTransform rectTrans = nodeItem.gameObject.GetComponent<RectTransform>();
                if (RectTransformUtility.RectangleContainsScreenPoint(rectTrans, mousePos, UISystem.Instance.GetCamera()))
                {
                    m_CurSearchID = nodeItem.SearchID;
                    if (m_CurSearchID <= 0)
                        return;

                    SelectImage.rectTransform.SetParent(nodeItem.SelectIconAttach.GetComponent<RectTransform>(), false);
                    SelectImage.gameObject.SetActive(true);

                    LegendCupNodeWarData nodeData = new LegendCupNodeWarData();
                    nodeData.nLegendCupID = m_LegendCupID;
                    nodeData.nRoundID = m_RoundID;
                    nodeData.nSearchID = m_CurSearchID;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, nodeData);

                    // GM账号，进入比赛入口状态更新
                    int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                    if (GameLogicAPI.bIsGM(nPDBID))
                    {
                        if (nodeItem.NodeInfo.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter)
                        {
                            GotoBtn.interactable = true;
                            GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_White);
                            m_GotoSearchID = m_CurSearchID;
                        }
                        else
                        {
                            GotoBtn.interactable = false;
                            GotoBtnDes.color = UDefines.CommomColor(ECommonColor.ECC_Gray);
                            m_GotoSearchID = 0;
                        }
                    }
                }
            }
        }

        public void OnClickGotoBtn()
        {
            if (m_LegendCupID == 0 || m_RoundID == 0 || m_GotoSearchID == 0)
            {
                return;
            }

            cmd_legendcup_enter_competition_node nodeData = new cmd_legendcup_enter_competition_node();
            nodeData.nLegendCupID = m_LegendCupID;
            nodeData.nRoundID = m_RoundID;
            nodeData.nSearchID = m_GotoSearchID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_enter_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE, ref nodeData);

            // 删除DIDA提醒
            LogicDataCenter.legendCupDataManager.SetLegendCupDIDA(false, m_LegendCupID, m_RoundID, m_GotoSearchID);
        }

        public void OnClickCloseBtn()
        {
            //TimerManager.KillTimer(this, 1);

            cmd_legendcup_quit_observe observeData = new cmd_legendcup_quit_observe();
            observeData.nLegendCupID = m_LegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_quit_observe>(GameLogicDef.GVIEWCMD_LEGENDCUP_QUITOBSERVE, ref observeData);

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
                param1 = null;
            }
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
                param2 = null;
            }

            CatchBtnEffect.color = Color.clear;
            if (CatchBtnEffectSequence != null)
                CatchBtnEffectSequence.Kill(true);

            m_wnd.UnloadView();
        }
    }
}
