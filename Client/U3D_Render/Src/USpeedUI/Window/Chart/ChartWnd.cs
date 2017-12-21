using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;

namespace USpeedUI.Chart
{
    //子页面分类
    public enum EMChartSubWndType : int
    {
        EMWNDTYPE_ROLE = 0,
        EMWNDTYPE_HERO,
        EMWNDTYPE_COLLECTION,
        EMWNDTYPE_DONATIONCARD,
        EMWNDTYPE_CLAN,
        EMWNDTYPE_KIN,
        EMWNDTYPE_MENTEROSHIP,
        Max,
    }

    public enum EMChartSortType : int 
    {
        EMCHARTSORT_DISTRICT = 0,
        EMCHARTSORT_CLAN,
        EMCHARTSORT_FRIEND,
    }

    public class ChartWnd : UIPopupWnd<ChartWndView>
    {
        //子窗口列表
        private List<IUISubWnd> m_SubWndList;
        public EMChartSubWndType CurrentWndType { get { return m_CurrentWndType; } }
        //当前窗口类型
        private EMChartSubWndType m_CurrentWndType;

        public override WndID GetID()
        {
            return WndID.WND_ID_CHART;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/ChartView";
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            m_bForbid3DSound = true;

            m_SubWndList = new List<IUISubWnd>();

            //添加子窗口
            m_SubWndList.Add(new RoleChartWnd());
            m_SubWndList.Add(new HeroChartWnd());
            m_SubWndList.Add(new ClanChartWnd());
            m_SubWndList.Add(new KinChartWnd());
            m_SubWndList.Add(new MentorShipChartWnd());
            m_SubWndList.Add(new DonationChartWnd());
            m_SubWndList.Add(new CollectionChartWnd());

            //子窗口初始化
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.Init();
            }

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPENENCOURAGEDIALOG, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_UPDATE_PERSON_ORDER_SETTING, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, this);

            //Role
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTROLERANKDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTKILLRANKDATA, this);
            //Hero
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHERORANKDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROTYPECHARTDATA, this);
            //Clan
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCLANCHARTDATA, this);
            //Kin
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTKINCHARTDATA, this);
            //MentorShip
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA, this);
            //Donation
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTDONATECHARTDATA, this);
            //Collection
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTSKINCHARTDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTGEMSTONECHARTDATA, this);
            //Search
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPENENCOURAGEDIALOG, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_UPDATE_PERSON_ORDER_SETTING, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, this);

            //Role
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTROLERANKDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTKILLRANKDATA, this);
            //Hero
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHERORANKDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROTYPECHARTDATA, this);
            //Clan
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCLANCHARTDATA, this);
            //Kin
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTKINCHARTDATA, this);
            //MentorShip
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA, this);
            //Donation
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTDONATECHARTDATA, this);
            //Collection
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTSKINCHARTDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUESTGEMSTONECHARTDATA, this);
            //Search
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        
                    }
                    break;
                case WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA:
                    {
                        if(m_wndView != null)
                        {
                            UChartRankingMsgData msgData = data as UChartRankingMsgData;
                            m_wndView.RefreshRanking(msgData.nRanking, msgData.bShow);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTROLERANKDATA:
                case WndMsgID.WND_MSG_RECVREQUESTKILLRANKDATA:
                    {
                        IUISubWnd roleChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_ROLE);
                        if (roleChartWnd != null)
                            roleChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTHERORANKDATA:
                case WndMsgID.WND_MSG_RECVREQUESTHEROTYPECHARTDATA:
                    {
                        IUISubWnd heroChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_HERO);
                        if (heroChartWnd != null)
                            heroChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTCLANCHARTDATA:
                    {
                        IUISubWnd clanChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_CLAN);
                        if (clanChartWnd != null)
                            clanChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTKINCHARTDATA:
                    {
                        IUISubWnd kinChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_KIN);
                        if (kinChartWnd != null)
                            kinChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA:
                    {
                        IUISubWnd kinChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_MENTEROSHIP);
                        if (kinChartWnd != null)
                            kinChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTDONATECHARTDATA:
                    {
                        IUISubWnd donateChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_DONATIONCARD);
                        if (donateChartWnd != null)
                            donateChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTSKINCHARTDATA:
                case WndMsgID.WND_MSG_RECVREQUESTGEMSTONECHARTDATA:
                    {
                        IUISubWnd donateChartWnd = GetSubWnd(EMChartSubWndType.EMWNDTYPE_COLLECTION);
                        if (donateChartWnd != null)
                            donateChartWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA:
                    {
                        foreach(IUISubWnd wnd in m_SubWndList)
                        {
                            wnd.OnMessage(msgID, data);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool LoadView()
        {
            base.LoadView();

            if (m_wndView != null)
                m_wndView.RestData();

            //默认切至角色榜页
            SwitchWndPage(EMChartSubWndType.EMWNDTYPE_ROLE);

            // m_wndView.SetChartSetting();

            return true;
        }
        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            if (_bVisible)
            {
                if (m_wndView != null)
                    m_wndView.RestData();

                SwitchWndPage(EMChartSubWndType.EMWNDTYPE_ROLE);
            }

            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }

        private IUISubWnd GetSubWnd(EMChartSubWndType wndType)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    return wnd;
            }
            return null;
        }

        private void ShowSubWnd(EMChartSubWndType wndType)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    wnd.SetVisible(true);
                else
                    wnd.SetVisible(false);
            }
        }

        public void SwitchWndPage(EMChartSubWndType pageType)
        {
            if (m_wndView == null)
                return;

            ShowSubWnd(pageType);
            m_CurrentWndType = pageType;

            m_wndView.SetButtonSelect(m_CurrentWndType);
        }

        public void InitSubWndContainer(Transform container)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.SetContainer(container);
            }
        }

        public void JumpToMyRank()
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)m_CurrentWndType)
                    wnd.OnMessage(WndMsgID.WND_MSG_JUMPTOMYRANKING, null);
            }
        }
    }

    public class ChartWndView : UIBaseWndView
    {
        //子窗口容器
        public Transform ContentFrame;
        //按钮组
        public UButtonGroup ChartBtnGroup;
        public List<UButtonGroupItem> ChartBtnList;
        //翻页
        public Button[] PageTurningGroup;
        //标题
        public Text ChartTitle;
        //排名
        public Transform RankingFrame;
        public Text RankingTxt;
        //按钮图标
        public List<Sprite> SubWndBtnIconList;
        //按钮文字颜色
        public Color SelectedBtnColor;
        public Color UnSelectedBtnColor;


        private List<string> subWndBtnNameList;
        private int m_Ranking;
        private int topNode;
        private string rankingInChart;
        private string rankingOutChart;
        private ChartWnd chartWnd
        {
            get
            {
                return m_wnd as ChartWnd;
            }
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_UseImageEffect = true;

            m_Ranking = -1;
            topNode = 0;

            if (chartWnd != null)
                chartWnd.InitSubWndContainer(ContentFrame);

            subWndBtnNameList = new List<string>();
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Role"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Hero"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Collection"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Donation"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Clan"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_Kin"));
            subWndBtnNameList.Add(ULocalizationService.Instance.Get("UIView", "ChartWnd", "ChartWndBtn_MentorShip"));

            rankingInChart = ULocalizationService.Instance.Get("UIView", "ChartWnd", "rankingInChart");
            rankingOutChart = ULocalizationService.Instance.Get("UIView", "ChartWnd", "rankingOutChart");

            for (int i = 0; i < ChartBtnList.Count; i++)
            {
                ChartBtnList[i].GetComponentInChildren<Text>().text = subWndBtnNameList[topNode + i];
                ChartBtnList[i].transform.FindChild("Icon").GetComponent<Image>().sprite = SubWndBtnIconList[topNode + i];
            }

            ChartBtnGroup.onButtonGroupSelectedChange.AddListener(OnSubWndBtnChanged);

            return true;
        }

        public void RefreshRanking(int nValue, bool show)
        {
            if(m_Ranking != nValue)
            {
                m_Ranking = nValue;

                RankingTxt.text = m_Ranking > 0 ? string.Format(rankingInChart, m_Ranking) : rankingOutChart;
            }
            RankingFrame.gameObject.SetActive(show);
        }

        public void RestData()
        {
            if (topNode == 0)
                return;

            topNode = 0;
            for (int i = 0; i < ChartBtnList.Count; i++)
            {
                ChartBtnList[i].GetComponentInChildren<Text>().text = subWndBtnNameList[topNode + i];
                ChartBtnList[i].transform.FindChild("Icon").GetComponent<Image>().sprite = SubWndBtnIconList[topNode + i];
            }
        }

        public void SetButtonSelect(EMChartSubWndType wndType)
        {
            int nIndex = (int)wndType - topNode;
            ChartBtnGroup.SetItemSelectedItem(nIndex, true);
            for (int i = 0; i < ChartBtnList.Count; i++)
            {
                ChartBtnList[i].GetComponentInChildren<Text>().color = (i == nIndex ? SelectedBtnColor : UnSelectedBtnColor);
            }

            if (wndType == EMChartSubWndType.EMWNDTYPE_ROLE)
                RefreshBtnInteractable();
        }

        public void OnCloseBtnClick()
        {
            m_wnd.SetVisible(false);
        }

        public void OnUpBtnClick()
        {
            int nIndex = ChartBtnGroup.SelectedIndex;
            //if(nIndex != 0)
            //{
            //    ChartBtnGroup.SetItemSelectedItem(nIndex - 1);
            //    return;
            //}

            if (topNode == 0)
                return;

            topNode--;

            int nSelected = nIndex + topNode;

            for(int i = 0; i < ChartBtnList.Count; i++)
            {
                ChartBtnList[i].GetComponentInChildren<Text>().text = subWndBtnNameList[topNode + i];
                ChartBtnList[i].transform.FindChild("Icon").GetComponent<Image>().sprite = SubWndBtnIconList[topNode + i];
            }

            if (chartWnd != null)
                chartWnd.SwitchWndPage((EMChartSubWndType)nSelected);

            RefreshBtnInteractable();
        }

        public void OnDownBtnClick()
        {
            int nIndex = ChartBtnGroup.SelectedIndex;
            //if (nIndex != ChartBtnList.Count - 1)
            //{
            //    ChartBtnGroup.SetItemSelectedItem(nIndex + 1);
            //    return;
            //}

            if (topNode == ((int)EMChartSubWndType.Max - ChartBtnList.Count))
                return;

            topNode++;

            int nSelected = nIndex + topNode;

            for (int i = 0; i < ChartBtnList.Count; i++)
            {
                ChartBtnList[i].GetComponentInChildren<Text>().text = subWndBtnNameList[topNode + i];
                ChartBtnList[i].transform.FindChild("Icon").GetComponent<Image>().sprite = SubWndBtnIconList[topNode + i];
            }

            if (chartWnd != null)
                chartWnd.SwitchWndPage((EMChartSubWndType)nSelected);

            RefreshBtnInteractable();
        }

        public void OnMyRankBtnClick()
        {
            if(chartWnd != null)
            {
                chartWnd.JumpToMyRank();
            }
        }

        private void RefreshBtnInteractable()
        {
            PageTurningGroup[0].interactable = topNode != 0 ? true : false;
            PageTurningGroup[1].interactable = topNode != ((int)EMChartSubWndType.Max - ChartBtnList.Count) ? true : false;
        }

        private void OnSubWndBtnChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            int nIndex = ChartBtnGroup.GetItemIndex(newItem);
            int nSelected = nIndex + topNode;

            if (nIndex < 0)
                return;

            if (chartWnd == null || nSelected == (int)chartWnd.CurrentWndType)
                return;

            chartWnd.SwitchWndPage((EMChartSubWndType)nSelected);
        }
    }
}
