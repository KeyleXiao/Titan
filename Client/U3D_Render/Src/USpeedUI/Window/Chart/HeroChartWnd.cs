using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using USpeedUI.UWidgets;
using GameLogic;
using DataCenter;
using Data.PersonModel;

namespace USpeedUI.Chart
{
    public enum EMHeroChartPageType : int
    {
        EMHEROCHART_KING = 0,
        EMHEROCHART_DEVELOP,
        EMHEROCHART_TYPE,
    }

    public class HeroChartWnd : UIBaseSubWnd<HeroChartWndView>
    {
        public EMHeroChartPageType CurrentPageType { get { return m_CurrentPageType; } }
        //当前窗口类型
        private EMHeroChartPageType m_CurrentPageType;

        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_HERO;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/HeroChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA:
                    {
                        UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                        if (m_wndView != null)
                            m_wndView.onRecvHeroKingRankData(cmdData.nParam);
                        
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTHERORANKDATA:
                    {
                        if (m_wndView != null)
                            m_wndView.FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTHEROTYPECHARTDATA:
                    {
                        if (m_wndView != null)
                            m_wndView.FillData(EMHeroChartPageType.EMHEROCHART_TYPE);
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_JUMPTOMYRANKING:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnJumpToMyRanking();
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HERO);

            cmd_order_herotype_request_info data = new cmd_order_herotype_request_info();
            data.nMatchType = (byte)StaMatchType.EMT_TOGETHER;
            ViewEventHelper.Instance.SendCommand<cmd_order_herotype_request_info>(GameLogicDef.GVIEWCMD_OPEN_ORDER_HEROTYPE, ref data);

            SwitchContentPage(EMHeroChartPageType.EMHEROCHART_KING);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
                SwitchContentPage(EMHeroChartPageType.EMHEROCHART_KING);
        }

        //切换页面Wnd
        public void SwitchContentPage(EMHeroChartPageType pageType)
        {
            if (m_wndView == null)
                return;

            m_CurrentPageType = pageType;

            m_wndView.ShowContentPage(pageType);
            m_wndView.SetButtonSelect((int)m_CurrentPageType);
        }
    }

     public class HeroChartWndView : UIBaseSubWndView
    {
        public UButtonGroup HeroChartBtnGroup;
        public GameObject[] RankingLists;
        public GameObject PageFrame;
        public Transform[] ContainerList;
        public GameObject KingOfHeroItemGO;
        public GameObject KingOfHeroFrame;
        public Transform KingOfHeroContainer;
        public Image KingOfHeroIcon;
        public Text KingOfHeroName;
        public Dropdown SortTypeDropdown;
        public InputField SearchField;
        public Button PrevPageBtn;
        public Button NextPageBtn;
        public Text PageLabel;

        public Text KingBtnTxt;
        public Text DevelopBtnTxt;
        public Text TypeBtnTxt;

        private EMChartSortType m_CurrentSortType;
        private int m_CurrentPage;
        private int m_MaxPage;
        private int m_RankingPage;
        private bool bIsQuery;

        private HeroChartWnd heroChartWnd
        {
            get
            {
                return m_wnd as HeroChartWnd;
            }
        }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            RestData();

            PrevPageBtn.onClick.AddListener(OnPrevBtnClick);
            NextPageBtn.onClick.AddListener(OnNextBtnClick);
            SortTypeDropdown.onValueChanged.AddListener(OnSortTypeChanged);

            HeroChartBtnGroup.onButtonGroupSelectedChange.AddListener(OnPageBtnChanged);

            FillKingOfHeroChartData();

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if(!visible)
                KingOfHeroFrame.gameObject.SetActive(false);
        }

        public void ShowContentPage(EMHeroChartPageType type)
        {
            for (int i = 0; i < RankingLists.Count(); i++)
            {
                if (i == (int)type)
                    RankingLists[i].SetActive(true);
                else
                    RankingLists[i].SetActive(false);
            }

            RestData();

            if(type == EMHeroChartPageType.EMHEROCHART_DEVELOP)
            {
                PageFrame.gameObject.SetActive(true);
                SearchField.gameObject.SetActive(true);
                SortTypeDropdown.gameObject.SetActive(true);
                UChartRankingMsgData data = new UChartRankingMsgData(LogicDataCenter.chartDataManager.HeroRankList.PersonalRank);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
            }
            else
            {
                UChartRankingMsgData data = new UChartRankingMsgData(-1, false);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
            }

            FillData(type);
        }

        public void SetButtonSelect(int nIndex)
        {
            HeroChartBtnGroup.SetItemSelectedItem(nIndex);
        }

        public void FillData(EMHeroChartPageType type)
        {
            switch (type)
            {
                case EMHeroChartPageType.EMHEROCHART_KING:
                    {

                    }
                    break;
                case EMHeroChartPageType.EMHEROCHART_DEVELOP:
                    {
                        List<cmd_entity_hero_order_info> dataList = new List<cmd_entity_hero_order_info>();

                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.HeroRankList.tmpDataList;
                        }
                        else
                        {
                            if (m_CurrentSortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroRankList.AllDataList;
                                if (LogicDataCenter.chartDataManager.HeroRankList.PersonalRank % ContainerList[(int)EMHeroChartPageType.EMHEROCHART_DEVELOP].childCount == 0)
                                {
                                    m_RankingPage = LogicDataCenter.chartDataManager.HeroRankList.PersonalRank / ContainerList[(int)EMHeroChartPageType.EMHEROCHART_DEVELOP].childCount;
                                    m_RankingPage--;

                                    if (m_RankingPage < 0)
                                        m_RankingPage = 0;
                                }
                                else
                                {
                                    m_RankingPage = LogicDataCenter.chartDataManager.HeroRankList.PersonalRank / ContainerList[(int)EMHeroChartPageType.EMHEROCHART_DEVELOP].childCount;
                                }
                            }
                            else if (m_CurrentSortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroRankList.ClanMemberDataList;
                            }
                            else if (m_CurrentSortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroRankList.FriendDataList;
                            }
                        }

                        int count = ContainerList[(int)EMHeroChartPageType.EMHEROCHART_DEVELOP].childCount;
                        int startIndex = (m_CurrentPage - 1) * count;

                        m_MaxPage = CalculateMaxPage(dataList.Count, count);

                        for (int i = 0; i < count; i++)
                        {
                            Transform trs = ContainerList[(int)EMHeroChartPageType.EMHEROCHART_DEVELOP].GetChild(i);
                            if (trs != null)
                            {
                                HeroDevelopItem item = trs.GetComponent<HeroDevelopItem>();
                                if (item != null)
                                {
                                    item.Clear();
                                    if (startIndex + i < dataList.Count)
                                    {
                                        item.SetData(dataList[startIndex + i]);
                                    }
                                }
                            }
                        }
                        CalculatePageState();
                    }
                    break;
                case EMHeroChartPageType.EMHEROCHART_TYPE:
                    {
                        List<cmd_entity_herotype_order_info> dataList = new List<cmd_entity_herotype_order_info>();
                        dataList = LogicDataCenter.chartDataManager.HeroTypeChartList;
                        for (int i = 0; i < dataList.Count; i++)
                        {
                            if (dataList[i].byStaMatchType != 0 || dataList[i].byHeroType > 4)
                                continue;

                            Transform trs = ContainerList[(int)EMHeroChartPageType.EMHEROCHART_TYPE].GetChild(dataList[i].byHeroType);
                            if (trs != null)
                            {
                                HeroTypeItem item = trs.GetComponent<HeroTypeItem>();
                                if (item != null)
                                {
                                    item.SetData(dataList[i]);
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnSearchBtnClick()
        {
            if (!SearchField)
                return;

            if (SearchField.text == "")
            {
                m_CurrentPage = 1;
                bIsQuery = false;
                FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
                CalculatePageState();
                return;
            }

            bIsQuery = true;

            LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_HERODEVELOP, SearchField.text, m_CurrentSortType);
        }

        public void RequestHeroKingRankData(int id, string name)
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_KING_OF_HERO_ORDER, id);

            int count = KingOfHeroContainer.childCount;
            for (int i = 0; i < count; i++)
            {
                Transform trs = KingOfHeroContainer.GetChild(i);
                if (trs != null)
                {
                    HeroKingRankItem rankItem = trs.GetComponent<HeroKingRankItem>();
                    if (rankItem != null)
                        rankItem.Clear();
                }
            }

            KingOfHeroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, id);
            KingOfHeroName.text = name;
            KingOfHeroFrame.gameObject.SetActive(true);
        }

        public void onRecvHeroKingRankData(int nHeroID)
        {
            List<cmd_entity_king_of_hero_order_info> dataList = new List<cmd_entity_king_of_hero_order_info>();
            dataList = LogicDataCenter.chartDataManager.KingOfHeroChartList.AllDataList;

            int count = KingOfHeroContainer.childCount;
            for (int i = 0; i < count; i++)
            {
                Transform trs = KingOfHeroContainer.GetChild(i);
                if (trs != null)
                {
                    HeroKingRankItem rankItem = trs.GetComponent<HeroKingRankItem>();
                    if (rankItem != null)
                    {
                        rankItem.Clear();
                        if (dataList.Count > i)
                        {
                            rankItem.SetData(dataList[i]);
                        }
                    }
                }
            }
        }

        public void OnHeroKingRankCloseBtnClick()
        {
            KingOfHeroFrame.gameObject.SetActive(false);
        }

        public void OnJumpToMyRanking()
        {
            if (bIsQuery || m_CurrentSortType != EMChartSortType.EMCHARTSORT_DISTRICT)
                return;

            if (m_RankingPage < 0 || m_RankingPage > m_MaxPage)
                return;

            m_CurrentPage = m_RankingPage + 1;
            FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
            CalculatePageState();
        }

        private void FillKingOfHeroChartData()
        {
            foreach (SSchemePersonModelData data in PersonModelConfig.Instance.PersonModelConfigs)
            {
                // 开关配置1为关闭
                if (data.nHeroSwitch > 0)
                    continue;

                GameObject go = GameObject.Instantiate<GameObject>(KingOfHeroItemGO);
                if (go != null)
                {
                    go.transform.SetParent(ContainerList[(int)EMHeroChartPageType.EMHEROCHART_KING]);
                    go.name = "KingOfHeroItem";
                    HeroKingItem kingItem = go.GetComponent<HeroKingItem>();
                    if (kingItem != null)
                        kingItem.SetData(this, data.nHeroID, data.szHeroName, data.szHeroTitle);
                    go.SetActive(true);
                }
            }
        }

        private void OnPageBtnChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            int nIndex = HeroChartBtnGroup.GetItemIndex(newItem);

            if (nIndex == (int)heroChartWnd.CurrentPageType)
                return;

            heroChartWnd.SwitchContentPage((EMHeroChartPageType)nIndex);
        }

        private void OnSortTypeChanged(int nValue)
        {
            if (nValue == (int)m_CurrentSortType)
                return;
            m_CurrentSortType = (EMChartSortType)nValue;

            m_CurrentPage = 1;
            m_MaxPage = 0;

            bIsQuery = false;
            SearchField.text = "";

            FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
            CalculatePageState();
        }

        private void OnPrevBtnClick()
        {
            if (m_CurrentPage > 1)
            {
                m_CurrentPage--;
                FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
                CalculatePageState();
            }
        }

        private void OnNextBtnClick()
        {
            if (m_CurrentPage < m_MaxPage)
            {
                m_CurrentPage++;
                FillData(EMHeroChartPageType.EMHEROCHART_DEVELOP);
                CalculatePageState();
            }
        }

        private void RestData()
        {
            m_CurrentSortType = EMChartSortType.EMCHARTSORT_DISTRICT;
            m_CurrentPage = 1;
            m_MaxPage = 0;
            bIsQuery = false;
            SearchField.text = "";
            SortTypeDropdown.value = 0;
            SearchField.gameObject.SetActive(false);
            SortTypeDropdown.gameObject.SetActive(false);
            PageFrame.gameObject.SetActive(false);
        }

        private void CalculatePageState()
        {
            PageLabel.text = "0/0";
            PrevPageBtn.interactable = false;
            NextPageBtn.interactable = false;

            if (m_MaxPage == 0)
                return;

            if (m_CurrentPage < m_MaxPage)
                NextPageBtn.interactable = true;

            if (m_CurrentPage > 1)
                PrevPageBtn.interactable = true;

            PageLabel.text = m_CurrentPage + "/" + m_MaxPage;
        }

        private int CalculateMaxPage(int listCount, int nPageCount)
        {
            if (listCount % nPageCount == 0)
            {
                if (listCount > 0 && listCount < nPageCount)
                    return 1;
                else
                    return listCount / nPageCount;
            }
            else
                return listCount / nPageCount + 1;
        }
    }

    public class HeroKingRankItem : MonoBehaviour
    {
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text winRate;
        public Image rankIcon;
        public Text rank;
        public Text detailData;
        public UPopupMenuHandle popupMenuHandle;

        public Sprite[] top3IndexIcons;
        public Color[] top3TextColor;

        public void Clear()
        {
            index.text = "";
            index.gameObject.SetActive(true);
            indexIcon.gameObject.SetActive(false);
            sexIcon.gameObject.SetActive(false);
            playerName.text = "";
            winRate.text = "";
            rankIcon.gameObject.SetActive(false);
            rank.text = "";
            detailData.text = "";
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
        }
        
         public void SetData(cmd_entity_king_of_hero_order_info info)
        {
            index.text = info.nOrderID.ToString();

            playerName.text = info.szName;

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            winRate.text = Math.Round((float)info.nWinMatch / (float)info.nTotalMatch * 100, 1).ToString() + "%";

            rankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_CHART, 2, info.nRankIconID);
            rankIcon.gameObject.SetActive(true);
            rank.text = info.szGradeName;
            detailData.text = info.nWinMatch + "/" + info.nTotalMatch;

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.nPDBID));

            popupMenuHandle.HandlePopupAction = PopupAction;

            bool bTop3 = false;
            if (info.nOrderID == 1 || info.nOrderID == 2 || info.nOrderID == 3)
            {
                bTop3 = true;
                indexIcon.gameObject.SetActive(true);
                indexIcon.sprite = top3IndexIcons[info.nOrderID - 1];
                index.gameObject.SetActive(false);
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrderID - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            winRate.color = txtColor;
            rank.color = txtColor;
            detailData.color = txtColor;
        }
    }


    public class HeroKingItem : MonoBehaviour, IPointerClickHandler
    {
        public Image borderImg;
        public Image Icon;
        public Text Name;
        public Text Title;

        public Sprite[] borderList;

        private int m_HeroID;
        private HeroChartWndView wndView;

        public void SetData(HeroChartWndView view, int id, string roleTitle, string roleName)
        {
            m_HeroID = id;
            wndView = view;
            Icon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, m_HeroID);
            Title.text = roleTitle;
            Name.text = roleName;
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            wndView.RequestHeroKingRankData(m_HeroID, Name.text);
        }
    }

    public class HeroDevelopItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text heroName;
        public Image starIcon;
        public Text starName;
        public Text starLv;
        public Text score;
        public Text chanceOfWin;
        public Text clanName;
        public UPopupMenuHandle popupMenuHandle;

        public Sprite[] borderList;
        public Sprite[] top3IndexIcons;
        public Color[] top3TextColor;

        private bool bSelfItem;

        public void Clear()
        {
            index.text = "";
            index.gameObject.SetActive(true);
            indexIcon.gameObject.SetActive(false);
            sexIcon.gameObject.SetActive(false);
            playerName.text = "";
            heroName.text = "";
            starIcon.sprite = USpriteManager.Instance.DefaultSprite;
            starName.text = "";
            starLv.text = "";
            score.text = "";
            chanceOfWin.text = "";
            clanName.text = "";
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_hero_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            heroName.text = info.szHeroName;
            starIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeroStarIcon, "HeroStarIcon" + info.nStarIcon, WndID.WND_ID_CHART);
            starName.text = info.szStarDesc;
            starLv.text = info.nHeroStar.ToString();
            score.text = info.nHeroScore.ToString();

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            chanceOfWin.text = Math.Round((float)info.dwWinNum / (float)info.dwMatchNum * 100, 1).ToString() + "%";
            clanName.text = info.szClanName.ToString();

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.dwPlayerID));

            popupMenuHandle.HandlePopupAction = PopupAction;

            bool bTop3 = false;
            if (info.nOrder == 1 || info.nOrder == 2 || info.nOrder == 3)
            {
                bTop3 = true;
                indexIcon.gameObject.SetActive(true);
                indexIcon.sprite = top3IndexIcons[info.nOrder - 1];
                index.gameObject.SetActive(false);
            }

            if (!U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView))
            {
                if (EntityFactory.MainHeroView.Property)
                {
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.dwPlayerID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrder - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            heroName.color = txtColor;
            starName.color = txtColor;
            starLv.color = txtColor;
            score.color = txtColor;
            chanceOfWin.color = txtColor;
            clanName.color = txtColor;
        }

        void IPointerClickHandler.OnPointerClick(PointerEventData eventData)
        {
            if (bSelfItem)
                return;
            borderImg.sprite = borderList[2];
        }

        void IPointerEnterHandler.OnPointerEnter(PointerEventData eventData)
        {
            if (bSelfItem)
                return;
            borderImg.sprite = borderList[2];
        }

        void IPointerExitHandler.OnPointerExit(PointerEventData eventData)
        {
            if (bSelfItem)
                return;
            borderImg.sprite = borderList[0];
        }
    }

    public class HeroTypeItem : MonoBehaviour
    {
        public Image WinHeroIcon;
        public Text WinHeroName;
        public Text WinNumber;

        public Image OnStageHeroIcon;
        public Text OnStageHeroName;
        public Text OnStageNumber;

        public Image UpperFastestHeroIcon;
        public Text UpperFastestHeroName;
        public Text UpperFastestNumber;

        public void SetData(cmd_entity_herotype_order_info info)
        {

            WinHeroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CHART, 1, 4, (int)info.dwWinHeroID);
            WinHeroName.text = info.szWinHeroName;
            WinNumber.text = info.WinNum + "%";

            OnStageHeroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CHART, 1, 4, (int)info.dwOnStageHeroID);
            OnStageHeroName.text = info.szOnStageHeroName;
            OnStageNumber.text = info.OnStageNum + "%";

            UpperFastestHeroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CHART, 1, 4, (int)info.dwUpperFastestHeroID);
            UpperFastestHeroName.text = info.szUpperFastHeroName;
            UpperFastestNumber.text = info.dwUpperFastestNum.ToString();
        }
    }

}
