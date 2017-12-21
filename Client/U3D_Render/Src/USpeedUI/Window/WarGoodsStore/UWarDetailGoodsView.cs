using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarGoodsStore
{
    public class UWarDetailGoodsView : UWarGoodsStoreSubViewBase
    {
        public Image GoodsIcon;
        public Text GoodsName;
        public Text GoodsCost;
        public Text GoodsDetailInfo;
        public Button BuyBtn;

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            if (BuyBtn == null)
            {
                Transform BuyTrans = this.GetTransform.FindChild("BtnBuy");
                if (BuyTrans == null)
                {
                    Trace.LogError("没找到对象名为:BtnBuy 的按钮");
                }
                BuyBtn = BuyTrans.GetComponent<Button>();
            }


            if (BuyBtn == null)
                Trace.LogError("对象名为:BuyBtn 没找到Button组件");
            else
            {
                BuyBtn.onClick.AddListener(_viewParent.OnBtnBuyClick);
                SetBuyBtnEnable(false);
            }
                

            return base.Init(_viewParent);
        }

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_DetailGoods;
            }
        }

        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            UpdateView(_nParam);
        }
        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            if (m_MyParent.CurrentSelectedGoods != null && _nParam.DicGoodsItemProtertyChange.ContainsKey(m_MyParent.CurrentSelectedGoods.GoodsID))
            {
                UpdateView(m_MyParent.CurrentSelectedGoods);
            }

        }

        protected void UpdateView(UWarGoodsStoreItem _nParam)
        {
            if (_nParam == null || _nParam.SchemeGoodsInfo == null)
                return;

            UWarGoodsStoreView.GetWarGoodsIcon(_nParam.SchemeGoodsInfo.GoodsIconID, GoodsIcon);
            GoodsCost.text = _nParam.GoodsCost.ToString();
            GoodsName.text = _nParam.SchemeGoodsInfo.GoodsName;

            string des = _nParam.SchemeGoodsInfo.GoodsDes;
            UBBProperty.GetInstance().getRatio(des);
            U3D_Render.Common.UBB.toHtml(ref des,UBB_FORMAT_TYPE.UGUI);
            GoodsDetailInfo.text = des;
            SetBuyBtnEnable(_nParam.GoodsCanBuy);

            if (DataCenter.LogicDataCenter.warMobaGoodsDataManager.EnableGoodsFilter)
            {
                if (_nParam.IsFiltered)
                    BuyBtn.gameObject.SetActive(false);
                else
                    BuyBtn.gameObject.SetActive(true);
            }
            else
                BuyBtn.gameObject.SetActive(true);
        }

        public void SetBuyBtnEnable(bool _bCanBuy)
        {
            BuyBtn.interactable = DataCenter.WarMobaGoodsDataManager.Instance.CanBuyOrSell && _bCanBuy;
        }
    }
}
