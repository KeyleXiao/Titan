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
    public class ClanChartWnd : UIBaseSubWnd<ClanChartWndView>
    {
        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_CLAN;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/ClanChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUESTCLANCHARTDATA:
                    {
                        if (m_wndView != null)
                            m_wndView.RefreshData();
                    }
                    break;
                case WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ShowSearchData();
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_CLAN);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
            {
                if(m_wndView != null)
                    m_wndView.RefreshData();
            }
        }
    }
       
    public class ClanChartWndView : UIBaseSubWndView
    {
        public Transform ContainerFrame;
        public InputField SearchField;
        public Button PrevPageBtn;
        public Button NextPageBtn;
        public Text PageLabel;

        private int m_CurrentPage;
        private int m_MaxPage;
        private int m_RankingPage;
        private bool bIsQuery;

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            RestData();
            PrevPageBtn.onClick.AddListener(OnPrevBtnClick);
            NextPageBtn.onClick.AddListener(OnNextBtnClick);

            return true;
        }

        public void RefreshData()
        {
            RestData();
            FillData(m_CurrentPage, bIsQuery);
            CalculatePageState();
            UChartRankingMsgData data = new UChartRankingMsgData(LogicDataCenter.chartDataManager.ClanChartList.PersonalRank);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_REFRESHCHARTRANKINGDATA, data);
        }

        public void ShowSearchData()
        {
            m_CurrentPage = 1;
            FillData(1, true);
            CalculatePageState();
        }

        public void OnSearchBtnClick()
        {
            if (!SearchField)
                return;

            if (SearchField.text == "")
            {
                m_CurrentPage = 1;
                bIsQuery = false;
                FillData(1, false);
                CalculatePageState();
                return;
            }

            bIsQuery = true;

            LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_CLAN, SearchField.text);
        }

        public void OnJumpToMyRanking()
        {
            if (bIsQuery)
                return;

            if (m_RankingPage < 0 || m_RankingPage > m_MaxPage)
                return;

            m_CurrentPage = m_RankingPage + 1;
            FillData(m_CurrentPage, bIsQuery);
            CalculatePageState();
        }

        private void OnPrevBtnClick()
        {
            if (m_CurrentPage > 1)
            {
                m_CurrentPage--;
                FillData(m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void OnNextBtnClick()
        {
            if (m_CurrentPage < m_MaxPage)
            {
                m_CurrentPage++;
                FillData(m_CurrentPage, bIsQuery);
                CalculatePageState();
            }
        }

        private void FillData(int page, bool bIsQuery)
        {
            List<cmd_entity_clan_order_info> dataList = new List<cmd_entity_clan_order_info>();
            if (bIsQuery)
                dataList = LogicDataCenter.chartDataManager.ClanChartList.tmpDataList;
            else
            {
                dataList = LogicDataCenter.chartDataManager.ClanChartList.AllDataList;
                if (LogicDataCenter.chartDataManager.ClanChartList.PersonalRank % ContainerFrame.childCount == 0)
                {
                    m_RankingPage = LogicDataCenter.chartDataManager.ClanChartList.PersonalRank / ContainerFrame.childCount;
                    m_RankingPage--;

                    if (m_RankingPage < 0)
                        m_RankingPage = 0;
                }
                else
                {
                    m_RankingPage = LogicDataCenter.chartDataManager.ClanChartList.PersonalRank / ContainerFrame.childCount;
                }
            }

            int count = ContainerFrame.childCount;
            int startIndex = (page - 1) * count;

            m_MaxPage = CalculateMaxPage(dataList.Count, count);

            for (int i = 0; i < count; i++)
            {
                Transform trs = ContainerFrame.GetChild(i);
                if (trs != null)
                {
                    ClanChartItem item = trs.GetComponent<ClanChartItem>();
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

        private void RestData()
        {
            m_CurrentPage = 1;
            m_MaxPage = 0;
            bIsQuery = false;
            SearchField.text = "";
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

    public class ClanChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text clanName;
        public Text score;
        public Text number;
        public Text leader;

        public Sprite[] borderList;
        public Sprite[] top3IndexIcons;
        public Color[] top3TextColor;

        private bool bSelfItem;

        public void Clear()
        {
            index.text = "";
            index.gameObject.SetActive(true);
            indexIcon.gameObject.SetActive(false);
            clanName.text = "";
            score.text = "";
            number.text = "";
            leader.text = "";
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_clan_order_info info)
        {
            index.text = info.nOrderID.ToString();

            clanName.text = info.szKinName;
            score.text = info.dwTotalFightCapacity.ToString();
            number.text = info.dwPlayerCount.ToString();
            leader.text = info.szShaikhName.ToString();

            bool bTop3 = false;
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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) == info.dwClanID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrderID - 1] : Color.white;
            index.color = txtColor;
            clanName.color = txtColor;
            score.color = txtColor;
            number.color = txtColor;
            leader.color = txtColor;
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
