using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarGoodsStore
{
    public class UWarCanSynthesizedGoodsView : UWarGoodsStoreSubViewBase
    {
        public UWarGoodsStoreAllGoodsTile CanSynthesizedGoodsTile;
        private ObservableList<UWarGoodsStoreItem> m_CanSynthesizedGoodsTileDataSource = new ObservableList<UWarGoodsStoreItem>();
        public Button BtnPrevious;
        public Button BtnNexts;

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            InitButtons(_viewParent);
            return base.Init(_viewParent);
        }
        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_CanSyntheszedGoods;
            }
        }

        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            List<string> CanSynthesizedGoodsIDList = _nParam.SchemeGoodsInfo.GoodsCanComposeList.Split(MobaSchemesConfig.separator).ToList();
            if(CanSynthesizedGoodsIDList != null && CanSynthesizedGoodsIDList.Count > 0)
            {
                int nGoodsID = -1;
                //移除旧的侦听
                CanSynthesizedGoodsTile.OnSelect.RemoveListener(OnMobaGoodsSelected);
                CanSynthesizedGoodsTile.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
                CanSynthesizedGoodsTile.onRightClick.RemoveListener(OnMobaGoodsBuy);
                CanSynthesizedGoodsTile.OnEndScrolling.RemoveListener(OnGoodsTileScrollEnd);

                m_CanSynthesizedGoodsTileDataSource.Clear();

                foreach (var strGoodsID in CanSynthesizedGoodsIDList)
                {
                    if (string.IsNullOrEmpty(strGoodsID))
                        continue;
                    nGoodsID = Convert.ToInt32(strGoodsID);
                    UWarGoodsStoreItem item = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(nGoodsID);
                    if (item != null)
                        m_CanSynthesizedGoodsTileDataSource.Add(item);
                }

                CanSynthesizedGoodsTile.DataSource = m_CanSynthesizedGoodsTileDataSource;
                //添加新的侦听
                CanSynthesizedGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);
                CanSynthesizedGoodsTile.onDoubleClick.AddListener(OnMobaGoodsBuy);
                CanSynthesizedGoodsTile.onRightClick.AddListener(OnMobaGoodsBuy);
                CanSynthesizedGoodsTile.OnEndScrolling.AddListener(OnGoodsTileScrollEnd);

                RecalculatePages();
                CurrentPage = 0;
            }
        }
        public override void OnMobaGoodsBuy(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.SetWndBuyGoods(CanSynthesizedGoodsTile.DataSource[_nIndex].GoodsID);
        }

        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            CanSynthesizedGoodsTile.UpdateVisible();
        }

        public override void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.CurrentSelectedGoods = CanSynthesizedGoodsTile.DataSource[_nIndex];
        }

        protected void OnGoodsTileScrollEnd()
        {
            SetCurrentPageByNearIndex();
            SetBtnInteraByPages();
        }
        protected void InitButtons(UWarGoodsStoreView _viewParent)
        {
            if (BtnPrevious == null)
            {
                Transform BtnPreviousTrans = this.GetTransform.FindChild("BtnPrevious");
                if (BtnPreviousTrans == null)
                {
                    Trace.LogError("没找到对象名为:BtnPrevious 的按钮");
                }
                BtnPrevious = BtnPreviousTrans.GetComponent<Button>();
            }


            if (BtnPrevious == null)
                Trace.LogError("对象名为:BuyBtn 没找到Button组件");
            else
                BtnPrevious.onClick.AddListener(Prev);


            if (BtnNexts == null)
            {
                Transform BtnNextsTrans = this.GetTransform.FindChild("BtnNexts");
                if (BtnNextsTrans == null)
                {
                    Trace.LogError("没找到对象名为:BtnNexts 的按钮");
                }
                BtnNexts = BtnNextsTrans.GetComponent<Button>();
            }


            if (BtnNexts == null)
                Trace.LogError("对象名为:BuyBtn 没找到Button组件");
            else
                BtnNexts.onClick.AddListener(Next);
        }

        #region 控制转页部分
        public int PerPage = 4;
        private int Pages = 0;
        private int RemainderPages = 0;//滑动产生的页面余数
        protected int currentPage;
        public int CurrentPage
        {
            get
            {
                return currentPage;
            }
            set
            {
                GoToPage(value);
            }
        }
        protected void RecalculatePages()
        {
            var per_block = CanSynthesizedGoodsTile.GetItemsPerBlock() * PerPage;
            Pages = (per_block == 0) ? 0 : Mathf.CeilToInt(((float)CanSynthesizedGoodsTile.GetItemsCount()) / per_block);
        }
        protected float GetPagePosition(int page)
        {
           return CanSynthesizedGoodsTile.GetItemPosition(page * CanSynthesizedGoodsTile.GetItemsPerBlock() * PerPage);
        }
        protected void GoToPage(int page)
        {
            var endPosition = GetPagePosition(page);
            if (IsHorizontal())
            {
                endPosition *= -1;
            }
            if (IsHorizontal())
            {
                CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition = new Vector2(endPosition, CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition.y);
            }
            else
            {
                CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition = new Vector2(CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition.x, endPosition);
            }
            //改变Scroll位置后会触发上面OnGoodsTileScrollEnd()
            //currentPage = page;
            // ChangeBtnInteraByPage(page);
        }
        private void SetCurrentPageByNearIndex()
        {
            int nearIndex = CanSynthesizedGoodsTile.GetNearestItemIndex();
            currentPage = nearIndex / PerPage;
            RemainderPages = nearIndex % PerPage;
        }
        private void SetBtnInteraByPages()
        {
            if (BtnPrevious != null)
            {
                BtnPrevious.interactable = !(currentPage == 0 && RemainderPages==0);
                //BtnPrevious.interactable = (page != 0);
            }
            if (BtnNexts != null)
            {
                BtnNexts.interactable = (currentPage != (Pages - 1));
            }
        }
        private bool IsHorizontal()
        {
            var rect = CanSynthesizedGoodsTile.ScrollRect.content.rect;
            return rect.width >= rect.height;
        }

        public virtual void Next()
        {
            if (CurrentPage == (Pages - 1))
            {
                return;
            }
            CurrentPage += 1;
        }

        public virtual void Prev()
        {
            if (CurrentPage == 0&&RemainderPages==0)
            {
                return;
            }
            else if (RemainderPages == 0)
            {
                CurrentPage -= 1;
            }else
            {
                CurrentPage = CurrentPage;
            }
        }
        #endregion
    }
}
