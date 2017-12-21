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
    public class UWarPurchasedGoodsView : UWarGoodsStoreSubViewBase
    {
        public Text MoneyText;
        public Button RevokedBtn;
        public Button SellBtn;
        //只有当已购买物品有变化时才刷新列表
        protected bool m_bCanUpdateList = false;

        public UWarGoodsStorePurchasedGoodsTile PurchasedGoodsTile;
        public int SelectedPurchasedGoodsSlotID = -1;
        public int SelectedPurchasedGoodsID = -1;
        protected ListViewItem m_SelectedPurchasedGoodsComponent = null;


        private ObservableList<UWarGoodsStoreItem> m_PurchasedGoodsDataSource = new ObservableList<UWarGoodsStoreItem>();
        private UWarGoodsStoreItem m_nDefaultItem = new UWarGoodsStoreItem(0);

        public List<UWarGoodsStoreItemComponent> PurchasedGoodsList = new List<UWarGoodsStoreItemComponent>();

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_PurchasedGoods;
            }
        }

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            for (int i = 0; i < 7; i++)
            {
                m_PurchasedGoodsDataSource.Add(m_nDefaultItem);
            }

            UpdatePurchasedGoodsListData();// PurchasedGoodsTile.DataSource = m_PurchasedGoodsDataSource;
            AddPurchasedGoodsListListener();//PurchasedGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);
                                            //PurchasedGoodsTile.OnDeselect.AddListener(OnAllGoodsOnDeselect);

            InitButtons(_viewParent);

            return base.Init(_viewParent);
        }
        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            if (m_bCanUpdateList)
            {
                UpdatePurchasedGoodsListData(); //PurchasedGoodsTile.UpdateVisible();
                m_bCanUpdateList = false;
            }
        }
        /*
        public override void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            SelectedPurchasedGoodsSlotID = _nIndex;
            SelectedPurchasedGoodsID = PurchasedGoodsTile.DataSource[_nIndex].GoodsID;
            m_SelectedPurchasedGoodsComponent = _itemComponent;

            m_MyParent.CurrentSelectedGoods = m_PurchasedGoodsDataSource[_nIndex];

            if(SelectedPurchasedGoodsID > 0 )
            {
                if ((_itemComponent as UWarGoodsStoreItemComponent) && (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame != null)
                {
                    (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
                }
                SetSellRevokedBtnEnable(true);
            }            
        }
        */
        private void InitButtons(UWarGoodsStoreView _viewParent)
        {
            if (RevokedBtn == null || SellBtn == null)
            {
                Transform RevokedTrans = this.GetTransform.FindChild("BtnRevoked");
                if (RevokedTrans == null)
                {
                    Trace.LogError("没找到对象名为:RevokedBtn 的撤销按钮");
                }
                RevokedBtn = RevokedTrans.GetComponent<Button>();

                Transform SellTrans = this.GetTransform.FindChild("BtnSell");
                if (SellTrans == null)
                {
                    Trace.LogError("没找到对象名为:SellBtn 的出售按钮");
                }
                SellBtn = SellTrans.GetComponent<Button>();
            }

            if (RevokedBtn == null)
                Trace.LogError("对象名为:RevokedBtn 没找到Button组件");
            else
            {
                if(GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP)
                {
                    RevokedBtn.gameObject.SetActive(false);
                }
                else
                {
                    RevokedBtn.gameObject.SetActive(true);
                    RevokedBtn.onClick.AddListener(_viewParent.OnBtnRevokedClick);
                }
            }

            if (SellBtn == null)
                Trace.LogError("对象名为:SellBtn 没找到Button组件");
            else
                SellBtn.onClick.AddListener(OnSellBtnClick);

            SetCanRevoked(false);
            SetSellRevokedBtnEnable(false);
        }

        public void SetMoneyText(int _nMoney)
        {
            MoneyText.text = _nMoney.ToString();
        }

        public void SetSellRevokedBtnEnable(bool _bInteractive)
        {
            //RevokedBtn.interactable = _bInteractive;
            SellBtn.interactable = WarMobaGoodsDataManager.Instance.CanBuyOrSell && _bInteractive;
        }

        /// <summary>
        /// 设置已购买物品列表
        /// </summary>
        /// <param name="_nSlotIndex">已购买槽位ID</param>
        /// <param name="_nGoodsItem">物品信息</param>
        /// <param name="_nCanUseTimes">物品可使用次数</param>
        /// <param name="_nDisplayMaxTime">显示当前物品可使用次数,少于等于1次时不显示 </param>
        public void SetPurchasedGoods(int _nSlotIndex, UWarGoodsStoreItem _nGoodsItem)
        {
            m_PurchasedGoodsDataSource[_nSlotIndex] = _nGoodsItem == null ? m_nDefaultItem : _nGoodsItem;
            m_bCanUpdateList = true;

            SelectedFrameVisible(m_SelectedPurchasedGoodsComponent,false);
        }
        public void SetPurchasedGoods(List<GameLogic.cmd_war_persongoodsinfo> _goodsList)
        {
            for(int i= 0;i<_goodsList.Count;++i)
            {
                UWarGoodsStoreItem item = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_goodsList[i].nGoodID, false);
                SetPurchasedGoods(_goodsList[i].nSlotIndex, item);
            }
            UpdatePurchasedGoodsListData();
        }

        public void SetCanRevoked(bool _bCanRevoked)
        {
            if (RevokedBtn != null)
                RevokedBtn.interactable = _bCanRevoked;//.enabled = _bCanRevoked;
        }
        /*
        private void OnAllGoodsOnDeselect(int _nIndex, ListViewItem _itemComponent)
        {
            SetSellRevokedBtnEnable(false);
            HideSelectedFrame(_itemComponent);
        }
        */
        private void SelectedFrameVisible(ListViewItem _itemComponent,bool _bSelected)
        {
            if (_itemComponent == null)
                return;

            var tmpSelectedComponent = (_itemComponent as UWarGoodsStoreItemComponent);
            if (tmpSelectedComponent && tmpSelectedComponent.GoodsSelectedFrame != null)
            {
                tmpSelectedComponent.GoodsSelectedFrame.gameObject.SetActive(_bSelected);
                //m_SelectedPurchasedGoodsComponent = null;
            }
        }

        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {

            SelectedFrameVisible(m_SelectedPurchasedGoodsComponent, false);
        }

        protected void OnSellBtnClick()
        {
            m_MyParent.OnBtnSellClick();
            SetSellRevokedBtnEnable(false);
        }



        #region 因为要自定义布局形式，所有不用PurchasedGoodsTile
        protected void UpdatePurchasedGoodsListData()
        {
            if (PurchasedGoodsList.Count == 0 || m_PurchasedGoodsDataSource.Count == 0 || PurchasedGoodsList.Count != m_PurchasedGoodsDataSource.Count)
                return;

            for (int i = 0; i < PurchasedGoodsList.Count; ++i)
            {
                PurchasedGoodsList[i].SetData(m_PurchasedGoodsDataSource[i]);
            }
        }
        protected void AddPurchasedGoodsListListener()
        {
            for (int i = 0; i < PurchasedGoodsList.Count; ++i)
            {
                PurchasedGoodsList[i].onSelect.AddListener(OnMobaGoodsSelected);
            }
        }
        protected int GetPurchasedGoodsIndex(ListViewItem _itemComponent)
        {
            int index = -1;

            if(PurchasedGoodsList != null)
                index = PurchasedGoodsList.IndexOf((_itemComponent as UWarGoodsStoreItemComponent));

            return index;
        }
        public override void OnMobaGoodsSelected(ListViewItem _itemComponent)
        {
            OnAllGoodsOnDeselect(m_SelectedPurchasedGoodsComponent);

            int _nIndex = GetPurchasedGoodsIndex(_itemComponent);

            SelectedPurchasedGoodsSlotID = _nIndex;
            SelectedPurchasedGoodsID = m_PurchasedGoodsDataSource[_nIndex].GoodsID;
            m_SelectedPurchasedGoodsComponent = _itemComponent;

            m_MyParent.CurrentSelectedGoods = m_PurchasedGoodsDataSource[_nIndex];

            if (SelectedPurchasedGoodsID > 0)
            {
                SelectedFrameVisible(_itemComponent, true);

                SetSellRevokedBtnEnable(true);
            }
        }
        protected void OnAllGoodsOnDeselect(ListViewItem _itemComponent)
        {
            SetSellRevokedBtnEnable(false);
            SelectedFrameVisible(_itemComponent, false);
        }
        #endregion

    }
}
