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
using DataCenter;
using GameLogic;

namespace USpeedUI.UWidgets
{
    public class UTileViewRoomHeroSkin : TileView<UTileViewComponentRoomHeroSkin, UTileViewItemRoomHeroSkin>
    {
        private int m_nPageCount;
        private bool bIsStartedTileViewHero = false;
        private int m_nCurPageIndex = 1 ;
        private const int m_nPageItemCount =13;
        // 最原始的所有列表信息
        private ObservableList<UTileViewItemRoomHeroSkin> SelectListHeroData = new ObservableList<UTileViewItemRoomHeroSkin>();
        // 筛选后的列表信息
        private ObservableList<UTileViewItemRoomHeroSkin> SelectListHeroDataFilter = new ObservableList<UTileViewItemRoomHeroSkin>();
        private UTileViewComponentRoomHeroSkin m_CurSelectHeroItem;
        private bool m_bSelectDown;
        public void Awake()
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
        protected override void SetData(UTileViewComponentRoomHeroSkin component, UTileViewItemRoomHeroSkin item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UTileViewComponentRoomHeroSkin component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UTileViewComponentRoomHeroSkin component)
        {
            base.SelectColoring(component);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UTileViewComponentRoomHeroSkin component)
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
            //PageIndexText.text = m_nCurPageIndex.ToString();
            ObservableList<UTileViewItemRoomHeroSkin> newData = new ObservableList<UTileViewItemRoomHeroSkin>();
            int nSlotId = 0;
            foreach (UTileViewItemRoomHeroSkin item in SelectHeroDataFilter)
            {
                if (nSlotId >= (m_nCurPageIndex - 1) * m_nPageItemCount && nSlotId < m_nCurPageIndex * m_nPageItemCount)
                {
                    newData.Add(item);
                }

                nSlotId++;
            }

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
            //PageIndexText.text = m_nCurPageIndex.ToString();
            int nSlotId = 0;
            ObservableList<UTileViewItemRoomHeroSkin> newData = new ObservableList<UTileViewItemRoomHeroSkin>();
            foreach (UTileViewItemRoomHeroSkin item in  SelectHeroDataFilter )
            {
                if (nSlotId >= (m_nCurPageIndex - 1) * m_nPageItemCount && nSlotId < m_nCurPageIndex * m_nPageItemCount)
                {
                    newData.Add(item);
                }
                nSlotId++;
            }

            this.DataSource = newData;
            ClearSelectState();
        }

        public void ResetData()
        {
            ClearSelectState();
            m_nCurPageIndex = 1;
            //PageIndexText.text = 1.ToString();

            SelectHeroData.Clear();
            SelectHeroDataFilter.Clear();
        }

        public override void OnDestroy()
        {
            ResetData();
            base.OnDestroy(); 
        }

        public ObservableList<UTileViewItemRoomHeroSkin> SelectHeroData
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

        public ObservableList<UTileViewItemRoomHeroSkin> SelectHeroDataFilter
        {
            set
            {
                SelectListHeroDataFilter = value;
                int nOffset = SelectListHeroDataFilter.Count % m_nPageItemCount == 0 ? 0 : 1;
                m_nPageCount = SelectListHeroDataFilter.Count / m_nPageItemCount + nOffset;
                //PageTotalText.text = m_nPageCount.ToString();
            }
            get
            {
                return SelectListHeroDataFilter;         
            }
        }

        public override ObservableList<UTileViewItemRoomHeroSkin> DataSource
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
            m_bSelectDown = false;
            if (m_CurSelectHeroItem != null)
            {
                m_CurSelectHeroItem.DestorySelectDownShotEffect();
                m_CurSelectHeroItem.DestorySelectOverLightEffect();
                m_CurSelectHeroItem = null;
            }

            foreach (UTileViewItemRoomHeroSkin item in SelectHeroData)
            {
                item.bSelect = false;
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

        public UTileViewComponentRoomHeroSkin SelectHeroItem
        {
            set
            {
                m_CurSelectHeroItem = value;
            }
            get
            {
                return m_CurSelectHeroItem;
            }
        }

        public void SetOnePageListItem()
        {
            ObservableList<UTileViewItemRoomHeroSkin> newData = new ObservableList<UTileViewItemRoomHeroSkin>();
            int nSlotId = 0;
            foreach (UTileViewItemRoomHeroSkin item in SelectHeroDataFilter)
            {
                if (nSlotId >= 0 && nSlotId < m_nPageItemCount)
                {
                    newData.Add(item);
                }

                nSlotId++;
            }

            this.DataSource = newData;
        }

        public void SetRoomListItemStateData(Stage_ManagerDef.EMatchRoomState eCurRoomState)
        {
            foreach (UTileViewItemRoomHeroSkin item in SelectHeroData)
            {
                item.eRoomState = eCurRoomState;
            }

            UpdateItem();
        }

        public void ResetPageIndex()
        {
            m_nCurPageIndex = 1;
            //PageIndexText.text = 1.ToString();
        }

        public int CompareMemberList(UTileViewItemRoomHeroSkin A, UTileViewItemRoomHeroSkin B)
        {
            return (A.nSkinType < B.nSkinType) ? -1 : 1;
        }

        public void SortSelectData()
        {
            SelectListHeroData.Sort(CompareMemberList);
        }

        public void UpdateRoomSkinList(int nHeroID,int nUpdateSkinID)
        {  
            cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
            if (!LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroID, nUpdateSkinID, ref skinCmd)) 
                return;

            foreach (UTileViewItemRoomHeroSkin item in this.DataSource)
            {
                if (item.nSkinId == nUpdateSkinID)
                {
                    item.bIsHaveSkin = true;
                    item.dwSkinLimitTime = skinCmd.dwDurationTime;
                }
            }

            foreach (UTileViewItemRoomHeroSkin item in this.SelectHeroData)
            {
                if (item.nSkinId == nUpdateSkinID)
                {
                    item.bIsHaveSkin = true;
                    item.dwSkinLimitTime = skinCmd.dwDurationTime;
                }
            }

            foreach (UTileViewItemRoomHeroSkin item in this.SelectHeroDataFilter)
            {
                if (item.nSkinId == nUpdateSkinID)
                {
                    item.bIsHaveSkin = true;
                    item.dwSkinLimitTime = skinCmd.dwDurationTime;
                }
            }

            UpdateItem();
        }
    }
}
