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
    public class DonationChartWnd : UIBaseSubWnd<DonationChartWndView>
    {
        public override int GetWndType()
        {
            return (int)EMChartSubWndType.EMWNDTYPE_DONATIONCARD;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Chart/DonationChartView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECVREQUESTDONATECHARTDATA:
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

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_DONATE);

            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
            {
                if (m_wndView != null)
                    m_wndView.RefreshData();
            }
        }
    }

    public class DonationChartWndView : UIBaseSubWndView
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
            UChartRankingMsgData data = new UChartRankingMsgData(LogicDataCenter.chartDataManager.DonateChartList.PersonalRank);
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

            LogicDataCenter.chartDataManager.SearchChartInfo(EMChartType.EMCHART_DONATECHART, SearchField.text);
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
            List<cmd_entity_donate_order_info> dataList = new List<cmd_entity_donate_order_info>();
            if (bIsQuery)
                dataList = LogicDataCenter.chartDataManager.DonateChartList.tmpDataList;
            else
                dataList = LogicDataCenter.chartDataManager.DonateChartList.AllDataList;

            int count = ContainerFrame.childCount;
            int startIndex = (page - 1) * count;

            m_MaxPage = CalculateMaxPage(dataList.Count, count);

            for (int i = 0; i < count; i++)
            {
                Transform trs = ContainerFrame.GetChild(i);
                if (trs != null)
                {
                    DonationChartItem item = trs.GetComponent<DonationChartItem>();
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

    public class DonationChartItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        public Image borderImg;
        public Text index;
        public Image indexIcon;
        public Text playerName;
        public Image sexIcon;
        public Text DonateNum;
        public Text DonationNum;
        public Text totalNum;
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
            playerName.text = "";
            DonateNum.text = "";
            DonationNum.text = "";
            totalNum.text = "";
            clanName.text = "";
            sexIcon.gameObject.SetActive(false);
            if (popupMenuHandle.HandlePopupAction != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();
                popupMenuHandle.HandlePopupAction = null;
            }
            borderImg.sprite = borderList[0];
            bSelfItem = false;
        }

        public void SetData(cmd_entity_donate_order_info info)
        {
            index.text = info.nOrder.ToString();

            playerName.text = info.szName;
            DonateNum.text = info.nClanCtrb.ToString();
            totalNum.text = info.nTotalClanCtrb.ToString();
            clanName.text = info.szClanName.ToString();

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction PopupAction = new UPopupAction();
            PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)info.nPDBID));

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
                    if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == info.nPDBID)
                    {
                        bSelfItem = true;
                        borderImg.sprite = borderList[1];
                    }
                }
            }

            Color txtColor = bTop3 ? top3TextColor[info.nOrder - 1] : Color.white;
            index.color = txtColor;
            playerName.color = txtColor;
            DonateNum.color = txtColor;
            totalNum.color = txtColor;
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