using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using DataCenter;
using GameLogic;
using UnityEngine.UI;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgets;
using UnityEngine;
using System.Collections;
using ASpeedGame.Data.LegendCupConfig;
using USpeedUI.UEffect;

namespace USpeedUI.LegendCup
{
    public class LegendCupListWnd : UIPopupWnd<LegendCupListWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_CUPLIST;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupListView";
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_RECVLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_UPDATELIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SYSTEM_CANCEL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_RECVLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_UPDATELIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SYSTEM_CANCEL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnOpenCupListWnd(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_RECVLIST:
                case WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_UPDATELIST:
                case WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SYSTEM_CANCEL:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvCupList();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClickCloseBtn();
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

    public class LegendCupListWndView : UIBaseWndView
    {
        public Text SysTitleDes;
        public Text ReturnBtnDes;
        public Text TitleDes;
        public Text IndexDes;
        public Text CupNameDes;
        public Text CreaterNameDes;
        public Text CupConfigNameDes;
        public Text CompetitionBonusDes;
        public Text DeadlineDes;
        public Text KinNumDes;

        public UListLegendCup LegendCupList;
        private int m_CurSelectIndex;
        private UListItemLegendCup m_CurSelectItem;
        private UListComponentLegendCup m_CurSelectComponent;
        private ObservableList<UListItemLegendCup> m_AllDataSource;
        private float m_lastUpdateTime;
        private const int SinglePageItemCount = 14;

        public URichText CupTip;
        public InputField SearchInfo;
        public Text HasCreatedCupBtnDes;
        public Text MyCupBtnDes;
        public Text CreateCupBtnDes;
        public Text ViewCupBtnDes;
        public Button HasCreatedCupBtn;
        public Button MyCupBtn;

        // 标题光效
        public GameObject AttachPoint1;
        public GameObject AttachPoint2;
        private UEffectParamBase param1;
        private UEffectParamBase param2;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            SysTitleDes.text = ULocalizationService.Instance.Get("UIView", "Common", "OpenMatchSystem");
            ReturnBtnDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "Title");
            IndexDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "Index");
            CupNameDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CupName");
            CreaterNameDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CreaterName");
            CupConfigNameDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CupConfigName");
            //LevelLimitDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "LevelLimit");
            CompetitionBonusDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CompetitionBonus");
            DeadlineDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "Deadline");
            KinNumDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "KinNum");
            HasCreatedCupBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "HasCreatedCupBtn");
            MyCupBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "MyCupBtn");
            CreateCupBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "CreateCupBtn");
            ViewCupBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "ViewCupBtn");

            CupTip.text = "<a href=CupTip color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupList", "CupTip") + "</a>";
            CupTip.onHrefClick.AddListener(OnClickCupTipRT);
            SearchInfo.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "SearchPlaceHolder");

            LegendCupList.UserItemCount = SinglePageItemCount;

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            param1 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01, _tfAttachParent: AttachPoint1.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param1);
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }
            param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02, _tfAttachParent: AttachPoint2.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);

            return true;
        }

        // 不太精确的杯赛剩余时间倒计时
        public override void Update()
        {
            base.Update();

            if (Time.unscaledTime - m_lastUpdateTime > 60.0f)
            {
                if (m_AllDataSource != null && m_AllDataSource.Count > 0)
                {
                    ObservableList<UListItemLegendCup> dataSource = new ObservableList<UListItemLegendCup>();
                    UListItemLegendCup[] temp = new UListItemLegendCup[m_AllDataSource.Count];
                    m_AllDataSource.CopyTo(temp);
                    foreach (UListItemLegendCup Item in temp)
                    {
                        UListItemLegendCup cupInfo = new UListItemLegendCup();
                        cupInfo = Item;
                        cupInfo.data.nRegistLeftTime -= 60;
                        dataSource.Add(cupInfo);
                    }

                    LegendCupList.DataSource.BeginUpdate();
                    LegendCupList.DataSource.Clear();
                    LegendCupList.DataSource = dataSource;
                    LegendCupList.DataSource.EndUpdate();
                }

                m_lastUpdateTime = Time.unscaledTime;
             }
        }

        public void OnOpenCupListWnd(int nCupListType)
        {
            LogicDataCenter.legendCupDataManager.LegendCupListType = nCupListType;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST);
        }

        public void OnRecvCupList()
        {
            // 有自身创建的杯赛才显示
            HasCreatedCupBtn.gameObject.SetActive(LogicDataCenter.legendCupDataManager.LegendCupSelfCreateNodeDic.Count > 0);
            // 有自身参加的杯赛才显示
            MyCupBtn.gameObject.SetActive(LogicDataCenter.legendCupDataManager.LegendCupSelfInNodeDic.Count > 0);

            int nSelfClanID = 0;
            int nSelfPDBID = 0;
            if (EntityFactory.MainHeroView != null)
            {
                nSelfClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }
            bool bGMer = GameLogicAPI.bIsGM(nSelfPDBID);

            LegendCupList.OnSelect.RemoveListener(OnSelectCupItem);
            LegendCupList.onDoubleClick.RemoveListener(OnDoubleClickCupItem);
            LegendCupList.DataSource.Clear();
            List<cmd_legendcup_recv_cuplist_node> nodeList = LogicDataCenter.legendCupDataManager.GetTotalLegendCup();
            ObservableList<UListItemLegendCup> dataSource = new ObservableList<UListItemLegendCup>();
            int nIndex = 1;
            foreach (cmd_legendcup_recv_cuplist_node node in nodeList)
            {
                if (!bGMer && node.nClanID > 0)         // 非GM玩家排除其他联盟数据
                {
                    if (nSelfClanID <= 0 || (nSelfClanID > 0 && nSelfClanID != node.nClanID))
                    {
                        continue;
                    }
                }

                UListItemLegendCup item = new UListItemLegendCup();
                item.nIndex = nIndex;
                item.data = node;
                dataSource.Add(item);
                nIndex++;
            }

            // 让m_AllDataSource和LegendCupList.DataSource是两份数据
            m_AllDataSource = new ObservableList<UListItemLegendCup>(dataSource);

            LegendCupList.DataSource = dataSource;
            LegendCupList.onDoubleClick.AddListener(OnDoubleClickCupItem);
            LegendCupList.OnSelect.AddListener(OnSelectCupItem);

            // 默认选中第一个，赋值前已保证列表有序
            if (LegendCupList.DataSource.Count > 0 && LegendCupList.DataSource[0].data.nLegendCupID > 0)
                LegendCupList.Select(0);

            // 更新刷新时间
            m_lastUpdateTime = Time.unscaledTime;
        }

        public void OnSelectCupItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupList.DataSource[index];
            m_CurSelectComponent = item as UListComponentLegendCup;
        }

        public void OnDoubleClickCupItem(int _nIndex, ListViewItem _itemComponent)
        {
            UListComponentLegendCup legendCup = _itemComponent as UListComponentLegendCup;
            ViewLegendCupDetail(legendCup.LLegendCupID);
        }

        public void OnClickCupTipRT(string eventName)
        {
            if (eventName == "CupTip")
            {
                // 杯赛列表界面不区分线上线下说明,取线上即可
                UIMsgCmdData uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OnLine_CupList, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, uiCupTipData);
            }
        }

        public void OnClickSearchBtn()
        {
            ClearSelectIcon();

            LegendCupList.DataSource.BeginUpdate();
            LegendCupList.DataSource.Clear();

            ObservableList<UListItemLegendCup> allDataSource = new ObservableList<UListItemLegendCup>(m_AllDataSource); 

            if (String.IsNullOrEmpty(SearchInfo.text))
            {
                // 显示所有数据
                LegendCupList.DataSource = allDataSource;
            }
            else
            {
                // 显示过滤数据
                IEnumerable<UListItemLegendCup> ListQuery = from item in allDataSource
                                                            where item.data.szCupName.Contains(SearchInfo.text)
                                                            select item;
                ObservableList<UListItemLegendCup> dataSource = new ObservableList<UListItemLegendCup>();
                foreach (UListItemLegendCup Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                LegendCupList.DataSource = dataSource;
            }
            LegendCupList.DataSource.EndUpdate(); 
        }

        public void OnClickHasCreateCupBtn()
        {
            ClearSelectIcon();

            if (LogicDataCenter.legendCupDataManager.LegendCupSelfCreateNodeDic.Count == 0)
            {
                return;
            }

            // 只有一场直接跳转，否则比赛列表界面进行筛选
            if (LogicDataCenter.legendCupDataManager.LegendCupSelfCreateNodeDic.Count == 1)
            {
                ViewLegendCupDetail(LogicDataCenter.legendCupDataManager.LegendCupSelfCreateNodeDic.ElementAt(0).Key);
            }
            else
            {
                int nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                ObservableList<UListItemLegendCup> allDataSource = new ObservableList<UListItemLegendCup>(m_AllDataSource);

                LegendCupList.DataSource.BeginUpdate();
                LegendCupList.DataSource.Clear();
                IEnumerable<UListItemLegendCup> ListQuery = from item in allDataSource
                                                            where item.data.nCreaterPDBID == nSelfPDBID
                                                            select item;
                ObservableList<UListItemLegendCup> dataSource = new ObservableList<UListItemLegendCup>();
                foreach (UListItemLegendCup Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                LegendCupList.DataSource = dataSource;
                LegendCupList.DataSource.EndUpdate();
            }
        }

        public void OnClickMyCupBtn()
        {
            ClearSelectIcon();

            if (LogicDataCenter.legendCupDataManager.LegendCupSelfInNodeDic.Count == 0)
            {
                return;
            }

            // 只有一场直接跳转，否则比赛列表界面进行筛选
            if (LogicDataCenter.legendCupDataManager.LegendCupSelfInNodeDic.Count == 1)
            {
                ViewLegendCupDetail(LogicDataCenter.legendCupDataManager.LegendCupSelfInNodeDic.ElementAt(0).Key);
            }
            else
            {
                ObservableList<UListItemLegendCup> allDataSource = new ObservableList<UListItemLegendCup>(m_AllDataSource);

                LegendCupList.DataSource.BeginUpdate();
                LegendCupList.DataSource.Clear();
                IEnumerable<UListItemLegendCup> ListQuery = from item in allDataSource
                                                            where item.data.nIsSelfIn > 0
                                                            select item;
                ObservableList<UListItemLegendCup> dataSource = new ObservableList<UListItemLegendCup>();
                foreach (UListItemLegendCup Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                LegendCupList.DataSource = dataSource;
                LegendCupList.DataSource.EndUpdate();
            }
        }

        public void OnClickCreateCupBtn()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CREATETYPE_SHOW, null);
        }

        public void OnClickViewCupBtn()
        {
            if (m_CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_CUP);
                return;
            }
            ViewLegendCupDetail(m_CurSelectItem.data.nLegendCupID);
        }

        public void OnClickCloseBtn()
        {
            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }

            m_wnd.UnloadView();
        }

        private void ViewLegendCupDetail(long nLegendCupID)
        {
            cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
            cupdata.nLegendCupID = nLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
        }

        private void ClearSelectIcon()
        {
            if (m_CurSelectComponent != null)
            {
                m_CurSelectComponent.DefaultColoring();
            }

            m_CurSelectIndex = -1;
            m_CurSelectItem = null;
            m_CurSelectComponent = null;
        }
    }

}
