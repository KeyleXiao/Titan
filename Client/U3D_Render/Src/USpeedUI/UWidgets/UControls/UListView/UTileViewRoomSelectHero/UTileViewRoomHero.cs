/*******************************************************************
** 文件名:	UTileViewHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;
using UnityEngine.UI;
using Stage_ManagerDef;
using DataCenter;
using GameLogic;

namespace USpeedUI.UWidgets
{
    public class UTileViewRoomHero : TileView<UTileViewComponentRoomHero, UTileViewItemRoomHero>
    {
        public GameWaitingRoomView WaitingRoomView;
        private int m_nPageCount;
        public Text PageIndexText;
        public Text PageTotalText;
        private bool bIsStartedTileViewHero = false;
        private int m_nCurPageIndex = 1 ;
        private const int m_nPageItemCount =13;
        // 最原始的所有列表信息
        private ObservableList<UTileViewItemRoomHero> SelectListHeroData = new ObservableList<UTileViewItemRoomHero>();
        // 筛选后的列表信息
        private ObservableList<UTileViewItemRoomHero> SelectListHeroDataFilter = new ObservableList<UTileViewItemRoomHero>();
        private UTileViewComponentRoomHero m_CurSelectHeroItem;
        private UTileViewItemRoomHero m_CurSelectHeroItemBackups = new UTileViewItemRoomHero();
        private bool m_bSelectDown;
        public  void Awake()
        {
            Start();
        }

        /// <summary>
        /// Start this instance.
        /// </summary>
        public override void Start()
        {
            if (bIsStartedTileViewHero)
            {
                return;
            }

            bIsStartedTileViewHero = true;
            this.ScrollRect.horizontal = false;
            this.ScrollRect.vertical = false;
            base.Start();
        }


        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>
        protected override void SetData(UTileViewComponentRoomHero component, UTileViewItemRoomHero item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UTileViewComponentRoomHero component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UTileViewComponentRoomHero component)
        {
            base.SelectColoring(component);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UTileViewComponentRoomHero component)
        {
            base.DefaultColoring(component);
        }

        public void OnSelectFramePageLeftClick()
        {
            if(m_nCurPageIndex <= 1)
            {
                return;
            }

            m_nCurPageIndex--;
            PageIndexText.text = m_nCurPageIndex.ToString();
            ObservableList<UTileViewItemRoomHero> newData = new ObservableList<UTileViewItemRoomHero>();
            int nSlotId = 0;
            foreach (UTileViewItemRoomHero item in SelectHeroDataFilter)
            {
                if (nSlotId >= (m_nCurPageIndex - 1) * m_nPageItemCount && nSlotId < m_nCurPageIndex * m_nPageItemCount)
                {
                    newData.Add(item);
                }

                nSlotId++;
            }

            newData.Sort(CompareMemberList);
            this.DataSource =  newData;

            ClearSelectState();
        }

        public void OnSelectFramePageRightClick()
        {
            if (m_nCurPageIndex >= m_nPageCount)
            {
                return;
            }

            m_nCurPageIndex++;
            PageIndexText.text = m_nCurPageIndex.ToString();
            int nSlotId = 0;
            ObservableList<UTileViewItemRoomHero> newData = new ObservableList<UTileViewItemRoomHero>();
            foreach (UTileViewItemRoomHero item in  SelectHeroDataFilter )
            {
                if (nSlotId >= (m_nCurPageIndex - 1) * m_nPageItemCount && nSlotId < m_nCurPageIndex * m_nPageItemCount)
                {
                    newData.Add(item);
                }
                nSlotId++;
            }

            newData.Sort(CompareMemberList);
            this.DataSource = newData;
            ClearSelectState();
        }

        public void ResetData()
        {
            m_nCurPageIndex = 1;
            PageIndexText.text = 1.ToString();

            SelectHeroData.Clear();
            SelectHeroDataFilter.Clear();
        }

        public override void OnDestroy()
        {
            ResetData();
            base.OnDestroy(); 
        }

        public ObservableList<UTileViewItemRoomHero> SelectHeroData
        {
            set
            {
                SelectListHeroData = value;           
            }
            get
            {
                return SelectListHeroData;
            }
        }

        public ObservableList<UTileViewItemRoomHero> SelectHeroDataFilter
        {
            set
            {
                SelectListHeroDataFilter = value;
                int nOffset = SelectListHeroDataFilter.Count % m_nPageItemCount == 0 ? 0 : 1;
                m_nPageCount = SelectListHeroDataFilter.Count / m_nPageItemCount + nOffset;
                PageTotalText.text = m_nPageCount.ToString();
            }
            get
            {
                return SelectListHeroDataFilter;         
            }
        }

        public override ObservableList<UTileViewItemRoomHero> DataSource
        {
            get
            {
                return base.DataSource;
            }
            set
            {
                base.DataSource = value;
                DataSource.Sort(CompareMemberList);
            }
        }

        public void UpdateItem()
        {
            UpdateItems();
        }

        public void ClearSelectState()
        {
            if (WaitingRoomView != null && WaitingRoomView.m_roomWnd != null && !WaitingRoomView.m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_SelectHero))
                m_bSelectDown = false;
            if (m_CurSelectHeroItem != null)
            {
                m_CurSelectHeroItem.DestorySelectDownShotEffect();
                m_CurSelectHeroItem.DestorySelectOverLightEffect();
                m_CurSelectHeroItem = null;
                m_CurSelectHeroItemBackups.bSelect = false;
            }
        }

        public bool SelectDown
        {
            set
            {
                m_bSelectDown = value;
            }
            get
            {
                return m_bSelectDown;
            }
        }

        public UTileViewComponentRoomHero SelectHeroItem
        {
            set
            {
                m_CurSelectHeroItem = value;
                if (value != null)
                    m_CurSelectHeroItemBackups = value.GetData();
            }
            get
            {
                return m_CurSelectHeroItem;
            }
        }

        public UTileViewItemRoomHero SelectHeroItemBackupsData
        {
            set
            {
                m_CurSelectHeroItemBackups = value;
            }
            get
            {
                return m_CurSelectHeroItemBackups;
            }
        }

        public void SetOnePageListItem()
        {
            ObservableList<UTileViewItemRoomHero> newData = new ObservableList<UTileViewItemRoomHero>();
            int nSlotId = 0;
            foreach (UTileViewItemRoomHero item in SelectHeroDataFilter)
            {
                if (nSlotId >= 0 && nSlotId < m_nPageItemCount)
                {
                    newData.Add(item);
                }

                nSlotId++;
            }

            this.DataSource = newData;
        }

        public void ResetPageIndex()
        {
            m_nCurPageIndex = 1;
            PageIndexText.text = 1.ToString();
        }

        public void SetRoomListItemStateData(Stage_ManagerDef.EMatchRoomState eCurRoomState)
        {
            foreach (UTileViewItemRoomHero item in SelectHeroData)
            {
                item.eRoomState = eCurRoomState;
            }

            UpdateItem();
        }


        public int CompareMemberList(UTileViewItemRoomHero A, UTileViewItemRoomHero B)
        {
            int nActorLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            int nIsWeekFreeA = A.bWeekFree ? 1 : 0;
            int nIsWeekFreeB = B.bWeekFree ? 1 : 0;
            int nIsHaveA = A.bIsHaveHero ? 1 : 0;
            int nIsHaveB = B.bIsHaveHero ? 1 : 0;
            int nSortIndexA = A.nSortIndex;
            int nSortIndexB = B.nSortIndex;
            int nUnlockLevelA = (A.nUnlockLevel <= 0 || (nActorLevel >= A.nUnlockLevel && !A.bIsHaveHero)) ? Int32.MaxValue : A.nUnlockLevel;
            int nUnlockLevelB = (B.nUnlockLevel <= 0 || (nActorLevel >= B.nUnlockLevel && !B.bIsHaveHero)) ? Int32.MaxValue : B.nUnlockLevel;
            if (A.nHeroIconId == -1)
                return -1;
            else if (B.nHeroIconId == -1)
                return 1;
            //else if (A.nIsNew != B.nIsNew)
            //    return (A.nIsNew > B.nIsNew) ? -1 : 1;
            else if (nIsWeekFreeA != nIsWeekFreeB)
                return (nIsWeekFreeA > nIsWeekFreeB) ? -1 : 1;
            else if (nIsHaveA != nIsHaveB)
                return (nIsHaveA > nIsHaveB) ? -1 : 1;
            else if (A.nUnlockTime != B.nUnlockTime)
                return (A.nUnlockTime > B.nUnlockTime) ? -1 : 1;
            else if (nUnlockLevelA != nUnlockLevelB)
                return (nUnlockLevelA < nUnlockLevelB) ? -1 : 1;
            else if (nSortIndexA == 0 && nSortIndexB != 0)
                return 1;
            else if (nSortIndexB == 0 && nSortIndexA != 0)
                return -1;
            else if (nSortIndexA != nSortIndexB)
                return (nSortIndexA < nSortIndexB) ? -1 : 1;
            else
                return (A.nHeroIconId < B.nHeroIconId) ? -1 : 1;
        }

        public void SortSelectData()
        {
            SelectListHeroData.Sort(CompareMemberList);
        }

        public bool UpdateRoomHeroList(int nUpdateHeroID)
        {
            int nUnlockTime = 0;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nUpdateHeroID))
            {
                cmd_entity_hero_info heroInfo = info.dicEntityHeroInfo[nUpdateHeroID];
                nUnlockTime = (int)heroInfo.ulUnlockTime;
            }

            foreach (UTileViewItemRoomHero item in this.DataSource)
            {
                if (item.nHeroIconId == nUpdateHeroID)
                {
                    if (item.bIsHaveHero)
                        return false;

                    item.nUnlockTime = nUnlockTime;
                    item.bIsHaveHero = true;
                    if (!item.bIsLockByHero && !item.bIsLockByComputer)
                        item.Lock = false;

                    break;
                }
            }
            this.DataSource.Sort(CompareMemberList);

            foreach (UTileViewItemRoomHero item in this.SelectHeroData)
            {
                if (item.nHeroIconId == nUpdateHeroID)
                {
                    item.nUnlockTime = nUnlockTime;
                    item.bIsHaveHero = true;
                    if (!item.bIsLockByHero && !item.bIsLockByComputer)
                        item.Lock = false;

                    break;
                }
            }
            this.SelectHeroData.Sort(CompareMemberList);

            foreach (UTileViewItemRoomHero item in this.SelectHeroDataFilter)
            {
                if (item.nHeroIconId == nUpdateHeroID)
                {
                    item.nUnlockTime = nUnlockTime;
                    item.bIsHaveHero = true;
                    if (!item.bIsLockByHero && !item.bIsLockByComputer)
                        item.Lock = false;

                    break;
                }
            }
            this.SelectHeroDataFilter.Sort(CompareMemberList);

            UpdateItem();
            return true;
        }

     
    }
}
