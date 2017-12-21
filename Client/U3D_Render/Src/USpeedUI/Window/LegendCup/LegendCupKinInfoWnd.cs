using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using USpeedUI.Social;
using USpeedUI.UWidgets;
using GameLogic;
using DataCenter;
using UIWidgets;
using ASpeedGame.Data.LegendCupConfig;

namespace USpeedUI.LegendCup
{
    public class LegendCupKinInfoWnd : UIPopupWnd<LegendCupKinInfoWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_KININFO;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupKinInfoView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_RECV, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINGLORY_RECV, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_RECV, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINGLORY_RECV, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnKinInfoWndOpen(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_KININFO_RECV:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnRecvKinInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_KINGLORY_RECV:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnRecvKinGlory();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // 查看战队信息按钮类型
    public enum KinInfoButtonType
    {
        KIBT_KINMEMBER,
        KIBT_KINGLORY,

        KIBT_MAX,
    }

    public class LegendCupKinMember : UCustomUIBehaviour
    {
        // KinMemberWnd
        public Text HeadPersonName;
        public Text IdentityIcon;
        public Text HeadLevel;
        public Text HeadPower;
        public Text HeadPowerRank;
        public Text HeadGradIcon;
        public Text HeadState;

        public UListKinMember ListKinMember;
        private int nCurSelectIndex;
        private UListItemKinMember CurSelectItem;
        private UPopupAction PopupAction;

        public Text KinInfoSubTitle;
        public Text KinNameDes;
        public Text KinShaikhNameDes;
        public Text KinLevelDes;
        public Text TotalMemberInfoDes;
        public Text TotalFightDes;
        public Text TotalOrderDes;
        public Text KinName;
        public Text KinShaikhName;
        public Text KinLevel;
        public Text TotalMemberInfo;
        public Text TotalFight;
        public Text TotalOrder;

        public Text KinLabelSubTitle;
        public Text KinLabelDes;
        public Text MatchResultDes;
        public Text GoldAwardDes;
        public Text SilverAwardDes;
        public Text KinLabel;
        public Text MatchResult;
        public KinLabelCupIconGroup GoldAward;
        public KinLabelCupIconGroup SilverAward;

        public Text KinNoticeSubTitle;
        public Text KinNoticeDes;

        public Transform OffLineTog;
        public Text OffLineTogDes;

        protected override void Awake()
        {
            base.Awake();

            HeadPersonName.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPersonName");
            IdentityIcon.text = ULocalizationService.Instance.Get("UIView", "KinMember", "IdentityIcon");
            HeadLevel.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadLevel"); ;
            HeadPower.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPower");
            HeadPowerRank.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPowerRank");
            HeadGradIcon.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadGradIcon");
            HeadState.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadState");

            KinInfoSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinInfoDes");
            KinShaikhNameDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinShaikhNameDes");
            KinLevelDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLevelDes");
            KinNameDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinNameDes");
            TotalMemberInfoDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalMemberInfoDes");
            TotalFightDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalFightDes");
            TotalOrderDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalOrderDes");

            KinLabelSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLabelSubTitle");
            KinLabelDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLabelDes");
            MatchResultDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "MatchResultDes");
            GoldAwardDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GoldAwardDes");
            SilverAwardDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "SilverAwardDes");
            
            KinNoticeSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinNoticeSubTitle");
            
            OffLineTogDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "OffLineTogDes");
            Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
            tOffLineTog.isOn = true;
        }

        public void SetKinMemberData()
        {
            ListKinMember.DataSource.Clear();
            ListKinMember.OnSelect.RemoveListener(OnSelectKinMember);
            ListKinMember.onRightClick.RemoveListener(OnSelectKinMember);

            cmd_kin_basedata cmdHead = LogicDataCenter.kinDataManager.LegendKinBaseData;
            List<cmd_kin_membernode> listData = LogicDataCenter.kinDataManager.LegendKinMemberList;
            UIWidgets.ObservableList<UListItemKinMember> dataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            ListKinMember.DataSource.Clear();
            for (int i = 0; i < cmdHead.nMemberCount; ++i)
            {
                GameLogic.cmd_kin_membernode cmdNodeData = listData[i];
                UListItemKinMember item = new UListItemKinMember();
                Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
                if (!tOffLineTog.isOn && cmdNodeData.bIsOnline == 0)
                {
                    // 隐藏不在线玩家
                    continue;
                }
                item.IdentityID = cmdNodeData.nIdentity;
                item.PlayerID = cmdNodeData.dwPDBID;
                item.GradIcon += cmdNodeData.szGradeName;
                item.PersonIcon = cmdNodeData.nFaceID;
                item.PersonName += cmdNodeData.szName;
                item.Power += cmdNodeData.dwBattleVal;
                item.PowerRank += cmdNodeData.dwBVOrder;
                item.GameState = cmdNodeData.byGameState;
                item.Level += cmdNodeData.nLevel;
                item.isOnline = cmdNodeData.bIsOnline;
                item.nSex = cmdNodeData.nSex;
                item.nLastOrder = cmdNodeData.dwLastOrder;
                item.nBVOrder = cmdNodeData.dwBVOrder;
                PopupAction = new UPopupAction();
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != item.PlayerID)
                {
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                }
                item.PopupAction = PopupAction;
                dataSource.Add(item);
            }

            // 排序
            IEnumerable<UListItemKinMember> ListQuery = from item in dataSource
                                                        orderby item.isOnline descending, item.IdentityID descending, item.Power descending
                                                        select item;

            UIWidgets.ObservableList<UListItemKinMember> tDataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                tDataSource.Add(Item);
            }

            ListKinMember.DataSource = tDataSource;

            ListKinMember.OnSelect.AddListener(OnSelectKinMember);
            ListKinMember.onRightClick.AddListener(OnSelectKinMember);

            // 设置战队基本数据
            KinName.text = cmdHead.szKinName;
            KinShaikhName.text = cmdHead.szShaikhName;
            KinLevel.text = cmdHead.nKinLevel.ToString();
            TotalMemberInfo.text = cmdHead.nOnlineCount + " / " + cmdHead.nMemberCount + "( " + ULocalizationService.Instance.Get("UIView", "KinMember", "MaxCountDes") + cmdHead.nKinMaxMember + ")";
            TotalFight.text = cmdHead.nTotalFight.ToString();
            TotalOrder.text = cmdHead.nFightOrder.ToString();

            KinLabel.text = cmdHead.szKinLabel;
            MatchResult.text = cmdHead.nMatchWinNum + " / " + cmdHead.nMatchTotalNum;
            GoldAward.SetData(cmdHead.nGoldNum);
            SilverAward.SetData(cmdHead.nSilverNum);

            KinNoticeDes.text = cmdHead.szNotice;
        }

        public void OnSelectKinMember(int index, UIWidgets.ListViewItem item)
        {
            // 选择当前数据
            nCurSelectIndex = index;
            CurSelectItem = ListKinMember.DataSource[index];
        }

        public void OnOfflineTogChanged(bool bSelect)
        {
            CurSelectItem = null;

            ListKinMember.DataSource.BeginUpdate();
            ListKinMember.DataSource.Clear();
            List<cmd_kin_membernode> KinMemberList = LogicDataCenter.kinDataManager.LegendKinMemberList;
            for (int i = 0; i < KinMemberList.Count; ++i)
            {
                cmd_kin_membernode cmdNodeData = KinMemberList[i];
                UListItemKinMember item = new UListItemKinMember();
                Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
                if (!tOffLineTog.isOn && cmdNodeData.bIsOnline == 0)
                {
                    // 隐藏不在线玩家
                    continue;
                }
                item.IdentityID = cmdNodeData.nIdentity;
                item.PlayerID = cmdNodeData.dwPDBID;
                item.GradIcon += cmdNodeData.szGradeName;
                item.PersonIcon = cmdNodeData.nFaceID;
                item.PersonName += cmdNodeData.szName;
                item.Power = cmdNodeData.dwBattleVal;
                item.PowerRank += cmdNodeData.dwBVOrder;
                item.GameState = cmdNodeData.byGameState;
                item.Level += cmdNodeData.nLevel;
                item.isOnline = cmdNodeData.bIsOnline;
                item.nSex = cmdNodeData.nSex;
                item.nLastOrder = cmdNodeData.dwLastOrder;
                item.nBVOrder = cmdNodeData.dwBVOrder;
                PopupAction = new UPopupAction();
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != item.PlayerID)
                {
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                }
                item.PopupAction = PopupAction;

                ListKinMember.DataSource.Add(item);
            }

            // 排序
            IEnumerable<UListItemKinMember> ListQuery = from item in ListKinMember.DataSource
                                                        orderby item.isOnline descending, item.IdentityID descending, item.Power descending
                                                        select item;

            UIWidgets.ObservableList<UListItemKinMember> dataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                dataSource.Add(Item);
            }

            ListKinMember.DataSource = dataSource;
            ListKinMember.DataSource.EndUpdate();
        }
    }

    public class LegendCupKinGlory : UCustomUIBehaviour
    {
        public Text GloryCupSubTitleDes;

        public Text GloryKinName;
        public Text GloryTotalDes;
        public Text GloryTotalNum;
        public Text GloryWinDes;
        public Text GloryWinNum;
        public Text GloryWinPro;
        public Slider GloryWinSlider;

        public Text GloryGodDes;
        public Text Glory5KillDes;
        public Text Glory4KillDes;
        public Text Glory3KillDes;
        public Text GloryDeathDes;
        public Text GloryKillDes;
        public Text GloryAssistDes;
        public Text GloryGodNum;
        public Text Glory5KillNum;
        public Text Glory4KillNum;
        public Text Glory3KillNum;
        public Text GloryDeathNum;
        public Text GloryKillNum;
        public Text GloryAssistNum;

        public UListKinGloryCup ListKinGloryCup;

        protected override void Awake()
        {
            base.Awake();

            GloryCupSubTitleDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryCupSubTitleDes");
            GloryWinDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "KinMember", "GloryWinDes"));
            GloryTotalDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "KinMember", "GloryTotalDes"));
            GloryGodDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryGodDes");
            Glory5KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory5KillDes");
            Glory4KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory4KillDes");
            Glory3KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory3KillDes");
            GloryDeathDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryDeathDes");
            GloryKillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryKillDes");
            GloryAssistDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryAssistDes");
        }

        public void SetKinGloryData()
        {
            cmd_kin_glory_head gloryHead = LogicDataCenter.kinDataManager.LegendGloryHead;
            GloryKinName.text = gloryHead.szKinName;
            int nTotalNum = gloryHead.nWinNum + gloryHead.nLoseNum;
            GloryTotalNum.text = nTotalNum.ToString();
            GloryWinNum.text = gloryHead.nWinNum.ToString();
            int nWinPro = nTotalNum > 0 ? (gloryHead.nWinNum * 100 / nTotalNum) : 0;
            GloryWinPro.text = String.Format("{0}%{1}", nWinPro, ULocalizationService.Instance.Get("UIView", "Common", "WinRate"));
            GloryWinSlider.value = (float)gloryHead.nWinNum / nTotalNum;
            GloryWinSlider.maxValue = 1;
            GloryGodNum.text = gloryHead.nGodNum.ToString();
            Glory5KillNum.text = gloryHead.n5KillNum.ToString();
            Glory4KillNum.text = gloryHead.n4KillNum.ToString();
            Glory3KillNum.text = gloryHead.n3KillNum.ToString();
            GloryDeathNum.text = gloryHead.nDeathNum.ToString();
            GloryKillNum.text = gloryHead.nKillNum.ToString();
            GloryAssistNum.text = gloryHead.nAssistNum.ToString();

            ListKinGloryCup.DataSource.BeginUpdate();
            ListKinGloryCup.DataSource.Clear();
            ObservableList<UListItemKinGloryCup> dataSource = new ObservableList<UListItemKinGloryCup>();
            List<cmd_kin_glory_node> gloryNodeList = LogicDataCenter.kinDataManager.LegendGloryNodeList;
            for (int i = 0; i < gloryNodeList.Count; i++)
            {
                cmd_kin_glory_node nodeData = gloryNodeList[i];

                SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(nodeData.nCupConfigID);
                if (cupConfig == null)
                    continue;
                // 8强赛保留12名，16强赛保留123名，32强赛保留前8名
                if (cupConfig.nMaxKinNum > 0 && cupConfig.nMaxKinNum <= 8 && nodeData.nCupRank > 2)
                    continue;
                else if (cupConfig.nMaxKinNum > 8 && cupConfig.nMaxKinNum <= 16 && nodeData.nCupRank > 3)
                    continue;
                else if (cupConfig.nMaxKinNum > 16 && cupConfig.nMaxKinNum <= 32 && nodeData.nCupRank > 8)
                    continue;

                UListItemKinGloryCup item = new UListItemKinGloryCup();
                item.nCupRank = nodeData.nCupRank;
                item.MatchName = nodeData.szCupName;
                item.llLegendCupID = nodeData.llLegendCupId;
                item.nCupConfigID = nodeData.nCupConfigID;
                item.nEndTime = nodeData.nEndTime;
                item.ConfigName = cupConfig.sCupConfigName;

                dataSource.Add(item);
            }
            ListKinGloryCup.DataSource = dataSource;
            ListKinGloryCup.DataSource.EndUpdate();
        }
    }

    public class LegendCupKinInfoWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text KinMemberDesc;
        public Text KinGloryDesc;
        public UButtonGroup buttonGroup;
        public LegendCupKinMember FrameKinMember;
        public LegendCupKinGlory FrameKinGlory;
        private KinInfoButtonType m_curShowPageType;

        private int m_nKinID;

        public override bool Init(IUIWnd wnd)
        {
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupKinInfo", "Title");
            KinMemberDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupKinInfo", "KinMember");
            KinGloryDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupKinInfo", "KinGlory");
            buttonGroup.SetAllItemDeSelected();
            m_curShowPageType = KinInfoButtonType.KIBT_KINMEMBER;
            buttonGroup.SkipInactive = false;
            return base.Init(wnd);
        }

        public void OnKinInfoWndOpen(int nKinID)
        {
            m_nKinID = nKinID;
            SetButtonSelect((int)m_curShowPageType);
        }

        public void OnRecvKinInfo()
        {
            FrameKinMember.SetKinMemberData();
        }

        public void OnRecvKinGlory()
        {
            FrameKinGlory.SetKinGloryData();
        }

        public void SetButtonSelect(int nIndex)
        {
            buttonGroup.SetAllItemDeSelected();
            buttonGroup.SetItemSelectedItem(nIndex, true);
        }

        public void OnGroupBtnKinMemberClick(bool bActive)
        {
            if (!bActive)
                return;

            FrameKinMember.gameObject.SetActive(true);
            FrameKinGlory.gameObject.SetActive(false);

            m_curShowPageType = KinInfoButtonType.KIBT_KINMEMBER;
            cmd_kin_legendcup_kininfo kininfo = new cmd_kin_legendcup_kininfo();
            kininfo.nKinID = m_nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_legendcup_kininfo>(GameLogicDef.GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN, ref kininfo);
        }

        public void OnGroupBtnKinGloryClick(bool bActive)
        {
            if (!bActive)
                return;

            FrameKinGlory.gameObject.SetActive(true);
            FrameKinMember.gameObject.SetActive(false);

            m_curShowPageType = KinInfoButtonType.KIBT_KINGLORY;
            cmd_kin_legendcup_kinglory kinglory = new cmd_kin_legendcup_kinglory();
            kinglory.nKinID = m_nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_legendcup_kinglory>(GameLogicDef.GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN, ref kinglory);
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
