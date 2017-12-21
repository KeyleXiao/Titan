using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarGoodsStore
{
    public class UWarDispositionGoodsView: UWarGoodsStoreSubViewBase
    {
        //public Toggle ToggleListView = null;
        public UWarGoodsStoreDispositionList DispositionListView = null;
        public RectTransform DispositionListContainer = null;

        protected int m_nMainHeroID;
        private enum ETimerHandlerID
        {
            GoodsDispositionList,
        }

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            bool result = base.Init(_viewParent);

            //ToggleListView.isOn = false;
            DispositionListView.gameObject.SetActive(false);
            //ToggleListView.onValueChanged.AddListener(OnToggleListView);
            WarMobaGoodsDataManager.InitHeroDispositionListEvent += SetDispositionListViewData;
            return result;
        }

        protected override void OnDestroy()
        {
            WarMobaGoodsDataManager.InitHeroDispositionListEvent -= SetDispositionListViewData;

            base.OnDestroy();
        }

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_DispositionGoods;
            }
        }

        int modalKey;
        public void OnToggleListView(bool _bOpen)
        {
            if(_bOpen)
            {
                DispositionListView.gameObject.SetActive(true);
                modalKey = ModalHelper.Open(DispositionListView,color:new Color(0,0,0,0), onClick: () => 
                {
                    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_TOGGLEDISPOSITIONSTATE,
                        new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_TOGGLEDISPOSITIONSTATE, 0, string.Empty, IntPtr.Zero, 0));
                },_tModalTarget: DispositionListContainer);
            }
            else
            {
                DispositionListView.gameObject.SetActive(false);
                ModalHelper.Close(modalKey);
            }
        }

        protected void SetDispositionListViewData()
        {
            DispositionListView.DataSource = WarMobaGoodsDataManager.Instance.HeroDispositionList;

            DispositionListView.OnSelect.AddListener(OnDispositionSelect);

            //新需求，需要在主界面上显示推荐物品按钮，直接这里发通知好了
            for(int i=1;i< DispositionListView.DataSource.Count;++i)
            {                
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM,
                    new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM, 
                    DispositionListView.DataSource[i].DispositionID, 
                    DispositionListView.DataSource[i].DispositionName,IntPtr.Zero,0));
            }
        }

        protected void OnDispositionSelect(int _index,UWarGoodsStoreDispositionItemComponent _component)
        {
            if(DispositionListView.DataSource != null)
            {
                if (DispositionListView.DataSource[_index].DispositionID == 0)
                {
                    WarMobaGoodsDataManager.Instance.HeroCurrentGoodsDisposition = WarMobaGoodsDataManager.Instance.HeroDefaultGoodsDisposition;
                }
                else
                {
                    WarMobaGoodsDataManager.Instance.HeroCurrentGoodsDisposition = DispositionListView.DataSource[_index].DispositionGoods;
                }
                //ToggleListView.isOn = false;
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_CHANGEGOODSDISPOSITION, null);

                UWGS_SelectedGoodsDispositionMsgData msgData = new UWGS_SelectedGoodsDispositionMsgData();
                msgData.SelectedDisposition = DispositionListView.DataSource[_index];
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SELECTEDRECOMMENDDISPOSITION, msgData);
            }

        }

        /// <summary>
        /// 用于主界面上推荐物品按钮标签点击处理
        /// </summary>
        /// <param name="_index"></param>
        public void OnDispositionSelect(int _index)
        {
            OnDispositionSelect(_index, null);
        }
    }
}
