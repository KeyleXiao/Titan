using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.WarGoodsStore
{
    public class UWarSearchResultGoodsView : UWarGoodsStoreSubViewBase
    {
        public UWarGoodsStoreAllGoodsTile AllGoodsTile;
        //临时的查询字符
        private string m_tmpSearchTextInput;

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            return base.Init(_viewParent);
        }
        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_SearchGoods;
            }
        }

        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            if (AllGoodsTile.SelectedItem != null && m_MyParent.CurrentSelectedGoods.GoodsID != AllGoodsTile.SelectedItem.GoodsID)
                AllGoodsTile.Deselect(AllGoodsTile.SelectedIndex);
        }

        public override void OnMobaGoodsBuy(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.SetWndBuyGoods(AllGoodsTile.DataSource[_nIndex].GoodsID);
            HideOptions();
        }

        public override void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.CurrentSelectedGoods = AllGoodsTile.DataSource[_nIndex];

            (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }

        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            AllGoodsTile.UpdateVisible();
        }

        private void OnAllGoodsOnDeselect(int _nIndex, ListViewItem _itemComponent)
        {
            (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(false);
        }

        public void OnSearchValueChange(EWarGoodsStoreSearchFilter _eFilter, string _strValue)
        {
            if (_strValue.Length == 0)
            {
                //SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.WarGoodsStoreMobaGoodsDataSource);
                HideOptions();
                return;
            }


            m_tmpSearchTextInput = _strValue;
            AllGoodsTile.Multiple = false;
            SetAllGoodsTileDataSource(GetFilterResult(_eFilter));
            if(AllGoodsTile.DataSource.Count > 0)
            {
                ShowOptions();
                AllGoodsTile.SelectedIndex = 0;
            }
            else
            {
                HideOptions();
            }
            m_tmpSearchTextInput = null;

        }

        public void OnSearchValueEndEdit(string _strValue)
        {
            //HideOptions();
        }

        protected void ShowOptions()
        {
            //AllGoodsTile.gameObject.SetActive(true);
            GetRectTransform.gameObject.SetActive(true);
        }

        protected void HideOptions()
        {
            GetRectTransform.gameObject.SetActive(false);
        }


        protected ObservableList<UWarGoodsStoreItem> GetFilterResult(EWarGoodsStoreSearchFilter _eFilter)
        {
            if (_eFilter == EWarGoodsStoreSearchFilter.ESF_Startswith)
            {
                return LogicDataCenter.warMobaGoodsDataManager.WarGoodsStoreMobaGoodsDataSource.Where(Startswith).ToObservableList();
            }
            else
            {
                return LogicDataCenter.warMobaGoodsDataManager.WarGoodsStoreMobaGoodsDataSource.Where(Contains).ToObservableList();
            }

        }

        private void SetAllGoodsTileDataSource(ObservableList<UWarGoodsStoreItem> _datSource)
        {
            AllGoodsTile.SelectedIndex = -1;

            AllGoodsTile.OnDeselect.RemoveListener(OnAllGoodsOnDeselect);
            AllGoodsTile.OnSelect.RemoveListener(OnMobaGoodsSelected);
            AllGoodsTile.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
            AllGoodsTile.onRightClick.RemoveListener(OnMobaGoodsBuy);


            int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            PersonSkinModelItem ModelItem = new PersonSkinModelItem();
            GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref ModelItem);

            //移除英雄限制的装备
            var list = _datSource.Select(d => d).Where(s => (s.SchemeGoodsInfo.GoodsVocationLimit != 0 && s.SchemeGoodsInfo.GoodsVocationLimit == ModelItem.nTraits)).ToList();
            foreach (var i in list)
            {
                _datSource.Remove(i);
            }


            AllGoodsTile.DataSource = _datSource;

            AllGoodsTile.OnDeselect.AddListener(OnAllGoodsOnDeselect);
            AllGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);
            AllGoodsTile.onDoubleClick.AddListener(OnMobaGoodsBuy);
            AllGoodsTile.onRightClick.AddListener(OnMobaGoodsBuy);
        }

        /// <summary>
        /// Determines whether the beginnig of value matches the Input.
        /// </summary>
        /// <param name="value">Value.</param>
        /// <returns>true if beginnig of value matches the Input; otherwise, false.</returns>
        public virtual bool Startswith(UWarGoodsStoreItem value)
        {
            if (LogicDataCenter.warMobaGoodsDataManager.EnableGoodsFilter && value.IsFiltered)
                return false;

            if (value != null && value.SchemeGoodsInfo != null && !string.IsNullOrEmpty(value.SchemeGoodsInfo.GoodsName))
                return value.SchemeGoodsInfo.GoodsName.StartsWith(m_tmpSearchTextInput);
            return false;
        }

        /// <summary>
        /// Returns a value indicating whether Input occurs within specified value.
        /// </summary>
        /// <param name="value">Value.</param>
        /// <returns>true if the Input occurs within value parameter; otherwise, false.</returns>
        public virtual bool Contains(UWarGoodsStoreItem value)
        {
            if (LogicDataCenter.warMobaGoodsDataManager.EnableGoodsFilter && value.IsFiltered)
                return false;

            if (value != null && value.SchemeGoodsInfo != null && !string.IsNullOrEmpty(value.SchemeGoodsInfo.GoodsName))
                return value.SchemeGoodsInfo.GoodsName.Contains(m_tmpSearchTextInput);
            return false;
        }
    }
}
