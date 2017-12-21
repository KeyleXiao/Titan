using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using GameLogic;
using DataCenter;

namespace USpeedUI.Chart
{
    public enum EMMentorShipChartPageType : int
    {
        EMMENTORSHIPCHART_MASTER = 0,
        EMMENTORSHIPCHART_PRENTICE,
    }

    public enum EMChartSortTime : int
    {
        EMCHARTSORT_CURRENTWEEK = 0,
        EMCHARTSORT_LASTWEEK,
    }

    public class MentorShipChartWnd : UIBaseSubWnd<MentorShipChartWndView>
    {
        public EMMentorShipChartPageType CurrentPageType { get { return m_CurrentPageType; } }
        //当前窗口类型
        private EMMentorShipChartPageType m_CurrentPageType;

        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_MENTEROSHIP;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/MentorShipChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EMMentorShipChartPageType.EMMENTORSHIPCHART_PRENTICE);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA:
                    {
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA:
                    {
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_CURWEEK_MASTER);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_CURWEEK_PRENTICE);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_LASTWEEK_MASTER);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_LASTWEEK_PRENTICE);

            //默认切换至角色段位榜
            SwitchContentPage(EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
                SwitchContentPage(EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER);
        }

        //切换页面Wnd
        public void SwitchContentPage(EMMentorShipChartPageType pageType)
        {
            if (m_wndView == null)
                return;

            m_CurrentPageType = pageType;

            m_wndView.ShowContentPage(pageType);
            m_wndView.SetButtonSelect((int)m_CurrentPageType);
        }
    }

    public class MentorShipChartWndView : UIBaseSubWndView
    {
        public UButtonGroup MentorShipChartBtnGroup;
        public GameObject[] RankingLists;
        public Transform[] ContainerList;
        public Dropdown SortTypeDropdown;
        public InputField SearchField;
        public Button PrevPageBtn;
        public Button NextPageBtn;
        public Text PageLabel;

        private EMChartSortTime m_CurrentSortType;
        private int m_CurrentPage;
        private int m_MaxPage;
        private int m_RankingPage;
        private bool bIsQuery;

        private MentorShipChartWnd mentorshipChartWnd
        {
            get
            {
                return m_wnd as MentorShipChartWnd;
            }
        }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            RestData();

            PrevPageBtn.onClick.AddListener(OnPrevBtnClick);
            NextPageBtn.onClick.AddListener(OnNextBtnClick);
            SortTypeDropdown.onValueChanged.AddListener(OnSortTypeChanged);

            MentorShipChartBtnGroup.onButtonGroupSelectedChange.AddListener(OnPageBtnChanged);

            return true;
        }

        public void ShowContentPage(EMMentorShipChartPageType type)
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
        }

        public void ShowSearchData(EMMentorShipChartPageType type)
        {
            m_CurrentPage = 1;
            FillData(type, m_CurrentSortType, 1, true);
            CalculatePageState();
        }

        public void RefreshChartData(EMMentorShipChartPageType type)
        {
            if (mentorshipChartWnd.CurrentPageType == type)
                mentorshipChartWnd.SwitchContentPage(type);
            else
                FillData(type, EMChartSortTime.EMCHARTSORT_CURRENTWEEK, 1, false, true);
        }

        public void SetButtonSelect(int nIndex)
        {
            MentorShipChartBtnGroup.SetItemSelectedItem(nIndex);
        }

        public void OnPageBtnChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            int nIndex = MentorShipChartBtnGroup.GetItemIndex(newItem);

            if (nIndex == (int)mentorshipChartWnd.CurrentPageType)
                return;

            mentorshipChartWnd.SwitchContentPage((EMMentorShipChartPageType)nIndex);
        }

        public void OnSearchBtnClick()
        {
            if (!SearchField)
                return;

            if (SearchField.text == "")
            {
                m_CurrentPage = 1;
                bIsQuery = false;
                FillData(mentorshipChartWnd.CurrentPageType, m_CurrentSortType, 1, false);
                CalculatePageState();
                return;
            }

            bIsQuery = true;

            if(mentorshipChartWnd.CurrentPageType == EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER)
            {
                if(m_CurrentSortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                    LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_CURRENTWEEKMASTER, SearchField.text);
                else
                    LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_LASTWEEKMASTER, SearchField.text);
            }
            else
            {
                if (m_CurrentSortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                    LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_CURRENTWEEKPRENTICE, SearchField.text);
                else
                    LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_LASTWEEKPRENTICE, SearchField.text);
            }
        }

        public void OnJumpToMyRanking()
        {
            if (bIsQuery)
                return;

            if (m_RankingPage < 0 || m_RankingPage > m_MaxPage)
                return;

            m_CurrentPage = m_RankingPage + 1;
            FillData(mentorshipChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnSortTypeChanged(int nValue)
        {
            if (nValue == (int)m_CurrentSortType)
                return;
            m_CurrentSortType = (EMChartSortTime)nValue;

            m_CurrentPage = 1;
            m_MaxPage = 0;

            bIsQuery = false;
            SearchField.text = "";

            FillData(mentorshipChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnPrevBtnClick()
        {
            if (m_CurrentPage > 1)
            {
                m_CurrentPage--;
                FillData(mentorshipChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void OnNextBtnClick()
        {
            if (m_CurrentPage < m_MaxPage)
            {
                m_CurrentPage++;
                FillData(mentorshipChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void FillData(EMMentorShipChartPageType pageType, EMChartSortTime sortType, int page, bool bIsQuery, bool bRefresh = false)
        {

            UChartRankingMsgData data = new UChartRankingMsgData();
            switch (pageType)
            {
                case EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER:
                    {
                        List<cmd_entity_mentorship_order_info> dataList = new List<cmd_entity_mentorship_order_info>();

                        if (bIsQuery)
                        {
                            if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                            {
                                dataList = LogicDataCenter.chartDataManager.CurWeekMasterChartList.tmpDataList;
                            }
                            else
                            {
                                dataList = LogicDataCenter.chartDataManager.LastWeekMasterChartList.tmpDataList;
                            }
                        }
                        else
                        {
                            if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                            {
                                dataList = LogicDataCenter.chartDataManager.CurWeekMasterChartList.AllDataList;
                                if(mentorshipChartWnd.CurrentPageType == EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER)
                                {
                                    if (LogicDataCenter.chartDataManager.CurWeekMasterChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.CurWeekMasterChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.CurWeekMasterChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else
                            {
                                dataList = LogicDataCenter.chartDataManager.LastWeekMasterChartList.AllDataList;
                                if(mentorshipChartWnd.CurrentPageType == EMMentorShipChartPageType.EMMENTORSHIPCHART_MASTER)
                                {
                                    if (LogicDataCenter.chartDataManager.LastWeekMasterChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.LastWeekMasterChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.LastWeekMasterChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
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
                                MentorChartItem item = trs.GetComponent<MentorChartItem>();
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
                        if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                            data.nRanking = LogicDataCenter.chartDataManager.CurWeekMasterChartList.PersonalRank;
                        else
                            data.nRanking = LogicDataCenter.chartDataManager.LastWeekMasterChartList.PersonalRank;
                    }
                    break;
                case EMMentorShipChartPageType.EMMENTORSHIPCHART_PRENTICE:
                    {
                        List<cmd_entity_mentorship_order_info> dataList = new List<cmd_entity_mentorship_order_info>();

                        if (bIsQuery)
                        {
                            if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                                dataList = LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.tmpDataList;
                            else
                                dataList = LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                            {
                                dataList = LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.AllDataList;
                                if (mentorshipChartWnd.CurrentPageType == EMMentorShipChartPageType.EMMENTORSHIPCHART_PRENTICE)
                                {
                                    if (LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else
                            {
                                dataList = LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.AllDataList;
                                if (mentorshipChartWnd.CurrentPageType == EMMentorShipChartPageType.EMMENTORSHIPCHART_PRENTICE)
                                {
                                    if (LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
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
                                MentorChartItem item = trs.GetComponent<MentorChartItem>();
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

                        if (sortType == EMChartSortTime.EMCHARTSORT_CURRENTWEEK)
                            data.nRanking = LogicDataCenter.chartDataManager.CurWeekPrenticeChartList.PersonalRank;
                        else
                            data.nRanking = LogicDataCenter.chartDataManager.LastWeekPrenticeChartList.PersonalRank;
                    }
                    break;
                default:
                    break;
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
        }

        private void RestData()
        {
            m_CurrentSortType = EMChartSortTime.EMCHARTSORT_CURRENTWEEK;
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

    public class MentorChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Text score;

        public Sprite[] borderList;
        public Sprite[] top3IndexIcons;
        public Color[] top3TextColor;

        private bool bSelfItem;

        public void Clear()
        {
            index.text = "";
            index.gameObject.SetActive(true);
            indexIcon.gameObject.SetActive(false);
            playerName.text = "";
            score.text = "";
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_mentorship_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            score.text = info.dwShipVal.ToString();

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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN) == info.dwPDBID)
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
