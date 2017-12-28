using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using DataCenter;
using GameLogic;

namespace USpeedUI.Chart
{
    //角色榜页面子窗口
    public enum EMRoleChartPageType : int
    {
        EMROLECHART_RANK = 0,
        EMROLECHART_LEGENDARY,
    }

    public class RoleChartWnd : UIBaseSubWnd<RoleChartWndView>
    {
        public EMRoleChartPageType CurrentPageType { get { return m_CurrentPageType; } }
        //当前窗口类型
        private EMRoleChartPageType m_CurrentPageType;


        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_ROLE;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/RoleChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUESTROLERANKDATA:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EMRoleChartPageType.EMROLECHART_RANK);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUESTKILLRANKDATA:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.RefreshChartData(EMRoleChartPageType.EMROLECHART_LEGENDARY);
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
                        if(m_wndView != null)
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HIGH_LADDER);
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_PLAYERKILL);

            //默认切换至角色段位榜
            SwitchContentPage(EMRoleChartPageType.EMROLECHART_RANK);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
                SwitchContentPage(EMRoleChartPageType.EMROLECHART_RANK);
        }

        //切换页面Wnd
        public void SwitchContentPage(EMRoleChartPageType pageType)
        {
            if (m_wndView == null)
                return;


            m_CurrentPageType = pageType;

            m_wndView.ShowContentPage(pageType);
            m_wndView.SetButtonSelect((int)m_CurrentPageType);
        }
    }

    public class RoleChartWndView : UIBaseSubWndView
    {
        public UButtonGroup RoleChartBtnGroup;
        public GameObject[] RankingLists;
        public Transform[] ContainerList;
        public Dropdown SortTypeDropdown;
        public InputField SearchField;
        public Button PrevPageBtn;
        public Button NextPageBtn;
        public Text PageLabel;

        public Text RankBtnTxt; 
        public Text LegendaryBtnTxt;

        private EMChartSortType m_CurrentSortType;
        private int m_CurrentPage;
        private int m_MaxPage;
        private int m_RankingPage;
        private bool bIsQuery;

        private RoleChartWnd roleChartWnd
        {
            get
            {
                return m_wnd as RoleChartWnd;
            }
        }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            RestData();

            PrevPageBtn.onClick.AddListener(OnPrevBtnClick);
            NextPageBtn.onClick.AddListener(OnNextBtnClick);
            SortTypeDropdown.onValueChanged.AddListener(OnSortTypeChanged);

            RoleChartBtnGroup.onButtonGroupSelectedChange.AddListener(OnPageBtnChanged);

            return true;
        }

        public void ShowContentPage(EMRoleChartPageType type)
        {
            for(int i = 0; i < RankingLists.Count(); i++)
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
                case EMRoleChartPageType.EMROLECHART_RANK:
                    {
                        data.nRanking = LogicDataCenter.chartDataManager.RoleRankList.PersonalRank;
                    }
                    break;
                case EMRoleChartPageType.EMROLECHART_LEGENDARY:
                    {
                        data.nRanking = LogicDataCenter.chartDataManager.KillRankList.PersonalRank;
                    }
                    break;
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
        }

        public void RefreshChartData(EMRoleChartPageType type)
        {
            if (roleChartWnd.CurrentPageType == type)
                roleChartWnd.SwitchContentPage(type);
            else
                FillData(type, EMChartSortType.EMCHARTSORT_DISTRICT, 1, false, true);
        }

        public void ShowSearchData(EMRoleChartPageType type)
        {
            m_CurrentPage = 1;
            FillData(type, m_CurrentSortType, 1, true);
            CalculatePageState();
        }

        public void SetButtonSelect(int nIndex)
        {
            RoleChartBtnGroup.SetItemSelectedItem(nIndex);
        }

        public void OnSearchBtnClick()
        {
            if (!SearchField)
                return;

            if(SearchField.text == "")
            {
                m_CurrentPage = 1;
                bIsQuery = false;
                FillData(roleChartWnd.CurrentPageType, m_CurrentSortType, 1, false);
                CalculatePageState();
                return;
            }

            bIsQuery = true;

            if(roleChartWnd.CurrentPageType == EMRoleChartPageType.EMROLECHART_RANK)
                LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_ROLERANK, SearchField.text, m_CurrentSortType);
            else
                LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_KILLRANK, SearchField.text, m_CurrentSortType);
        }

        public void OnJumpToMyRanking()
        {
            if (bIsQuery || m_CurrentSortType != EMChartSortType.EMCHARTSORT_DISTRICT)
                return;

            if (m_RankingPage < 0 || m_RankingPage > m_MaxPage)
                return;

            m_CurrentPage = m_RankingPage + 1;
            FillData(roleChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnPageBtnChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            int nIndex = RoleChartBtnGroup.GetItemIndex(newItem);

            if (nIndex == (int)roleChartWnd.CurrentPageType)
                return;

            roleChartWnd.SwitchContentPage((EMRoleChartPageType)nIndex);
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

            FillData(roleChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnPrevBtnClick()
        {
            if(m_CurrentPage > 1)
            {
                m_CurrentPage--;
                FillData(roleChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void OnNextBtnClick()
        {
            if (m_CurrentPage < m_MaxPage)
            {
                m_CurrentPage++;
                FillData(roleChartWnd.CurrentPageType, m_CurrentSortType, m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void FillData(EMRoleChartPageType pageType, EMChartSortType sortType, int page, bool bIsQuery, bool bRefresh = false)
        {
            switch (pageType)
            {
                case EMRoleChartPageType.EMROLECHART_RANK:
                    {
                        List<cmd_Entity_HighLadder_Rank_Info> dataList = new List<cmd_Entity_HighLadder_Rank_Info>();

                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.RoleRankList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.RoleRankList.AllDataList;
                                if (roleChartWnd.CurrentPageType == EMRoleChartPageType.EMROLECHART_RANK)
                                {
                                    if (LogicDataCenter.chartDataManager.RoleRankList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.RoleRankList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.RoleRankList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.RoleRankList.ClanMemberDataList;
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.RoleRankList.FriendDataList;
                            }
                        }

                        int count = ContainerList[(int)pageType].childCount;
                        int startIndex = (page - 1) * count;

                        if(!bRefresh)
                            m_MaxPage = CalculateMaxPage(dataList.Count, count);

                        for (int i = 0; i < count; i++)
                        {
                            Transform trs = ContainerList[(int)pageType].GetChild(i);
                            if(trs != null)
                            {
                                RoleRankItem item = trs.GetComponent<RoleRankItem>();
                                if(item != null)
                                {
                                    item.Clear();
                                    if(startIndex + i < dataList.Count)
                                    {
                                        item.SetData(dataList[startIndex + i]);
                                    }
                                }
                            }
                        }
                    }
                    break;
                case EMRoleChartPageType.EMROLECHART_LEGENDARY:
                    {
                        List<cmd_entity_kill_order_info> dataList = new List<cmd_entity_kill_order_info>();

                        if (bIsQuery)
                        {
                            dataList = LogicDataCenter.chartDataManager.KillRankList.tmpDataList;
                        }
                        else
                        {
                            if (sortType == EMChartSortType.EMCHARTSORT_DISTRICT)
                            {
                                dataList = LogicDataCenter.chartDataManager.KillRankList.AllDataList;
                                if(roleChartWnd.CurrentPageType == EMRoleChartPageType.EMROLECHART_LEGENDARY)
                                {
                                    if (LogicDataCenter.chartDataManager.KillRankList.PersonalRank % ContainerList[(int)pageType].childCount == 0)
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.KillRankList.PersonalRank / ContainerList[(int)pageType].childCount;
                                        m_RankingPage--;

                                        if (m_RankingPage < 0)
                                            m_RankingPage = 0;
                                    }
                                    else
                                    {
                                        m_RankingPage = LogicDataCenter.chartDataManager.KillRankList.PersonalRank / ContainerList[(int)pageType].childCount;
                                    }
                                }
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_CLAN)
                            {
                                dataList = LogicDataCenter.chartDataManager.KillRankList.ClanMemberDataList;
                            }
                            else if (sortType == EMChartSortType.EMCHARTSORT_FRIEND)
                            {
                                dataList = LogicDataCenter.chartDataManager.KillRankList.FriendDataList;
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
                                RoleKillItem item = trs.GetComponent<RoleKillItem>();
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
            m_RankingPage = 0;
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

    //角色段位榜数据项
    public class RoleRankItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Image rankIcon;
        public Text rankDesc;
        public RectTransform StarImgFrame;
        public Text StarScore;
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
            rankIcon.gameObject.SetActive(false);
            rankDesc.text = "";
            StarScore.text = "";
            clanName.text = "";
            for (int i = 0; i < StarImgFrame.childCount - 1; i++)
            {
                StarImgFrame.GetChild(i).gameObject.SetActive(false);
            }
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_Entity_HighLadder_Rank_Info info)
        {
            index.text = info.nOrderID.ToString();

            playerName.text = info.szPlayerName;
            rankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_CHART, 2, info.nRankIconID);
            rankIcon.gameObject.SetActive(true);
            rankDesc.text = info.szGradeName;

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            int nRankStar = GameLogicAPI.getActorRankStar((int)Match_ManagerDef.EMMatchType.MatchType_Rank, info.nRankScore);
            int nMaxRankStar = GameLogicAPI.getActorCurRankMaxStar((int)Match_ManagerDef.EMMatchType.MatchType_Rank, info.nRankScore);

            for (int i = 0; i < StarImgFrame.childCount - 1; ++i)
            {
                if (nRankStar > 0)
                {
                    StarImgFrame.GetChild(i).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_CHART, 2);
                    StarImgFrame.GetChild(i).gameObject.SetActive(true);
                    nRankStar--;
                    nMaxRankStar--;
                }
                //else if (nMaxRankStar > 0)
                //{
                //    StarImgFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_CHART, 1);
                //    StarImgFrame.GetChild(index).gameObject.SetActive(true);
                //    nMaxRankStar--;
                //}
                else
                {
                    StarImgFrame.GetChild(i).gameObject.SetActive(false);
                }
            }

            //StarScore.text = info.nRankScore.ToString();
            clanName.text = info.szClanName;

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.idActor));
            popupMenuHandle.HandlePopupAction = PopupAction;

            bool bTop3 = false;
            //如果是前3，加牛逼框框
            if (info.nOrderID == 1 || info.nOrderID == 2 || info.nOrderID == 3)
            {
                bTop3 = true;
                indexIcon.gameObject.SetActive(true);
                indexIcon.sprite = top3IndexIcons[info.nOrderID - 1];
                index.gameObject.SetActive(false);
            }

            if (!U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView))
            {
                if (EntityFactory.MainHeroView.Property)
                {
                    if(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.idActor)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrderID - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            rankDesc.color = txtColor;
            StarScore.color = txtColor;
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

    public class RoleKillItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text fiveKillCount;
        public Image rankIcon;
        public Text rankDesc;
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
            fiveKillCount.text = "";
            rankIcon.gameObject.SetActive(false);
            rankDesc.text = "";
            clanName.text = "";
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_kill_order_info info)
        {
            index.text = info.nOrderID.ToString();

            playerName.text = info.szName;
            fiveKillCount.text = info.n5KillNum.ToString();
            rankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_CHART, 2, info.nRankIconID);
            rankIcon.gameObject.SetActive(true);
            rankDesc.text = info.szGradeName;
            clanName.text = info.szClanName;

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, (int)info.bySex + 1);
            sexIcon.gameObject.SetActive(true);

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.nPDBID));

            popupMenuHandle.HandlePopupAction = PopupAction;

            bool bTop3 = false;
            //如果是前3，加牛逼框框
            if (info.nOrderID == 1 || info.nOrderID == 2 || info.nOrderID == 3)
            {
                bTop3 = true;
                indexIcon.gameObject.SetActive(true);
                indexIcon.sprite = top3IndexIcons[info.nOrderID - 1];
                index.gameObject.SetActive(false);
            }

            if (!U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView))
            {
                if (EntityFactory.MainHeroView.Property)
                {
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.nPDBID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrderID - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            fiveKillCount.color = txtColor;
            rankDesc.color = txtColor;
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
}
