using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using GameLogic;
using DataCenter;

namespace USpeedUI.Chart
{
    public enum EmCollectionChartPageType : int
    {
        EMCOLLECTIONCHART_HERO = 0,
        EMCOLLECTIONCHART_SKIN,
        EMCOLLECTIONCHART_GEM,
    }

    public class CollectionChartWnd : UIBaseSubWnd<CollectionChartWndView>
    {
        public EmCollectionChartPageType CurrentPageType { get { return m_CurrentPageType; } }
        //当前窗口类型
        private EmCollectionChartPageType m_CurrentPageType;

        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_COLLECTION;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/CollectionChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EmCollectionChartPageType.EMCOLLECTIONCHART_HERO);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTSKINCHARTDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EmCollectionChartPageType.EMCOLLECTIONCHART_SKIN);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTGEMSTONECHARTDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EmCollectionChartPageType.EMCOLLECTIONCHART_GEM);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ShowSearchData(m_CurrentPageType);
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HEROCOUNT);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_SKIN);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_GEMSTONE);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
                SwitchContentPage(EmCollectionChartPageType.EMCOLLECTIONCHART_HERO);
        }

        //切换页面Wnd
        public void SwitchContentPage(EmCollectionChartPageType pageType)
        {
            if (m_wndView == null)
                return;

            m_CurrentPageType = pageType;

            m_wndView.ShowContentPage(pageType);
            m_wndView.SetButtonSelect((int)m_CurrentPageType);
        }
    }

    public class CollectionChartWndView : UIBaseSubWndView
    {
        public UButtonGroup CollectionChartBtnGroup;
        public GameObject[] RankingLists;
        public Transform[] ContainerList;
        public Dropdown SortTypeDropdown;
        public InputField SearchField;
        public Button PrevPageBtn;
        public Button NextPageBtn;
        public Text PageLabel;

        private EMChartSortType m_CurrentSortType;
        private int m_CurrentPage;
        private int m_MaxPage;
        private int m_RankingPage;
        private bool bIsQuery;

        private CollectionChartWnd collectionChartWnd
        {
            get
            {
                return m_wnd as CollectionChartWnd;
            }
        }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            RestData();

            PrevPageBtn.onClick.AddListener(OnPrevBtnClick);
            NextPageBtn.onClick.AddListener(OnNextBtnClick);
            SortTypeDropdown.onValueChanged.AddListener(OnSortTypeChanged);

            CollectionChartBtnGroup.onButtonGroupSelectedChange.AddListener(OnPageBtnChanged);

            return true;
        }

        public void ShowContentPage(EmCollectionChartPageType type)
        {
            for (int i = 0; i < RankingLists.Count(); i++)
            {
                if (i == (int)type)
                    RankingLists[i].SetActive(true);
                else
                    RankingLists[i].SetActive(false);
            }
            RestData();
            FillData(type, m_CurrentSortType, m_CurrentPage, false);
            CalculatePageState();

            UChartRankingMsgData data = new UChartRankingMsgData();
            switch (type)
            {
                case EmCollectionChartPageType.EMCOLLECTIONCHART_HERO:
                    {
                        data.nRanking = LogicDataCenter.chartDataManager.HeroCountChartList.PersonalRank;
                    }
                    break;
                case EmCollectionChartPageType.EMCOLLECTIONCHART_SKIN:
                    {
                        data.nRanking = LogicDataCenter.chartDataManager.SkinChartList.PersonalRank;
                    }
                    break;
                case EmCollectionChartPageType.EMCOLLECTIONCHART_GEM:
                    {
                        data.nRanking = LogicDataCenter.chartDataManager.GemStoneChartList.PersonalRank;
                    }
                    break;
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
        }

        public void RefreshChartData(EmCollectionChartPageType type)
        {
            if (collectionChartWnd.CurrentPageType == type)
                collectionChartWnd.SwitchContentPage(type);
            else
                FillData(type, EMChartSortType.EMCHARTSORT_DISTRICT, 1, false, true);
        }

        public void ShowSearchData(EmCollectionChartPageType type)
        {
            m_CurrentPage = 1;
            FillData(type, m_CurrentSortType, 1, true);
            CalculatePageState();
        }

        public void SetButtonSelect(int nIndex)
        {
            CollectionChartBtnGroup.SetItemSelectedItem(nIndex);
        }

        public void OnPageBtnChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            int nIndex = CollectionChartBtnGroup.GetItemIndex(newItem);

            if (nIndex == (int)collectionChartWnd.CurrentPageType)
                return;

            collectionChartWnd.SwitchContentPage((EmCollectionChartPageType)nIndex);
        }

        public void OnSearchBtnClick()
        {
            if (!SearchField)
                return;

            if (SearchField.text == "")
            {
                m_CurrentPage = 1;
                bIsQuery = false;
                FillData(collectionChartWnd.CurrentPageType, m_CurrentSortType, 1, false);
                CalculatePageState();
                return;
            }

            bIsQuery = true;

            if(collectionChartWnd.CurrentPageType == EmCollectionChartPageType.EMCOLLECTIONCHART_HERO)
                LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_HEROCOUNT, SearchField.text, m_CurrentSortType);
            else if(collectionChartWnd.CurrentPageType == EmCollectionChartPageType.EMCOLLECTIONCHART_SKIN)
                LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_SKIN, SearchField.text, m_CurrentSortType);
            else
                LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_GEM, SearchField.text, m_CurrentSortType);
        }

        public void OnJumpToMyRanking()
        {
            if (bIsQuery || m_CurrentSortType != EMChartSortType.EMCHARTSORT_DISTRICT)
                return;

            if (m_RankingPage < 0 || m_RankingPage > m_MaxPage)
                return;

            m_CurrentPage = m_RankingPage + 1;
            FillData(collectionChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
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

            FillData(collectionChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnPrevBtnClick()
        {
            if (m_CurrentPage > 1)
            {
                m_CurrentPage--;
                FillData(collectionChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void OnNextBtnClick()
        {
            if (m_CurrentPage < m_MaxPage)
            {
                m_CurrentPage++;
                FillData(collectionChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void FillData(EmCollectionChartPageType pageType, EMChartSortType sortType, int page, bool bIsQuery, bool bRefresh = false)
        {
            switch (pageType)
            {
                case EmCollectionChartPageType.EMCOLLECTIONCHART_HERO:
                    {
                        List<cmd_entity_herocount_order_info> dataList = new List<cmd_entity_herocount_order_info>();

                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.HeroCountChartList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroCountChartList.AllDataList;
                                if(collectionChartWnd.CurrentPageType == EmCollectionChartPageType.EMCOLLECTIONCHART_HERO)
                                {
                                    if (LogicDataCenter.chartDataManager.HeroCountChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.HeroCountChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.HeroCountChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroCountChartList.ClanMemberDataList;
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.HeroCountChartList.FriendDataList;
                            }
                        }

                        int count = ContainerList[(int)pageType].childCount;
                        int startIndex = (page - 1) * count;

                        if(!bRefresh)
                            m_MaxPage = CalculateMaxPage(dataList.Count, count);

                        for (int i = 0; i < count; i++)
                        {
                            Transform trs = ContainerList[(int)pageType].GetChild(i);
                            if (trs != null)
                            {
                                HeroNumChartItem item = trs.GetComponent<HeroNumChartItem>();
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
                    }
                    break;
                case EmCollectionChartPageType.EMCOLLECTIONCHART_SKIN:
                    {
                        List<cmd_entity_skin_order_info> dataList = new List<cmd_entity_skin_order_info>();

                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.SkinChartList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.SkinChartList.AllDataList;
                                if (collectionChartWnd.CurrentPageType == EmCollectionChartPageType.EMCOLLECTIONCHART_SKIN)
                                {
                                    if (LogicDataCenter.chartDataManager.SkinChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.SkinChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.SkinChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.SkinChartList.ClanMemberDataList;
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.SkinChartList.FriendDataList;
                            }
                        }

                        int count = ContainerList[(int)pageType].childCount;
                        int startIndex = (page - 1) * count;

                        if (!bRefresh)
                            m_MaxPage = CalculateMaxPage(dataList.Count, count);

                        for (int i = 0; i < count; i++)
                        {
                            Transform trs = ContainerList[(int)pageType].GetChild(i);
                            if (trs != null)
                            {
                                SkinChartItem item = trs.GetComponent<SkinChartItem>();
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
                    }
                    break;
                case EmCollectionChartPageType.EMCOLLECTIONCHART_GEM:
                    {
                        List<cmd_entity_gemstone_order_info> dataList = new List<cmd_entity_gemstone_order_info>();
                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.GemStoneChartList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.GemStoneChartList.AllDataList;
                                if (collectionChartWnd.CurrentPageType == EmCollectionChartPageType.EMCOLLECTIONCHART_GEM)
                                {
                                    if (LogicDataCenter.chartDataManager.GemStoneChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.GemStoneChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.GemStoneChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.GemStoneChartList.ClanMemberDataList;
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.GemStoneChartList.FriendDataList;
                            }
                        }

                        int count = ContainerList[(int)pageType].childCount;
                        int startIndex = (page - 1) * count;

                        if (!bRefresh)
                            m_MaxPage = CalculateMaxPage(dataList.Count, count);

                        for (int i = 0; i < count; i++)
                        {
                            Transform trs = ContainerList[(int)pageType].GetChild(i);
                            if (trs != null)
                            {
                                GemChartItem item = trs.GetComponent<GemChartItem>();
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
                    }
                    break;
                default:
                    break;
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


    public class HeroNumChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text number;
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
            number.text = "";

            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_herocount_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            number.text = info.dwHeroCount.ToString();

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(info.dwPDBID));

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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.dwPDBID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrder - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            number.color = txtColor;
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

    public class SkinChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text skinNums;
        public RectTransform assetInfoFrame;
        public Text[] assetNums;
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
            skinNums.text = "";
            foreach (Text txt in assetNums)
            {
                txt.text = "";
            }

            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_skin_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            skinNums.text = info.dwSkinCount.ToString();
            assetNums[0].text = info.dwLegendSkinCount.ToString();
            assetNums[1].text = info.dwEpicSkinCount.ToString();
            assetNums[2].text = info.dwLimitedSkinCount.ToString();

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.dwPDBID));

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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.dwPDBID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrder - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            skinNums.color = txtColor;
            foreach(Text txt in assetNums)
            {
                txt.color = txtColor;
            }
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

    public class GemChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text score;
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
            score.text = "";
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_gemstone_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            score.text = info.dwPropVal.ToString();

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.dwPDBID));

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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.dwPDBID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrder - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            score.color = txtColor;
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
}
