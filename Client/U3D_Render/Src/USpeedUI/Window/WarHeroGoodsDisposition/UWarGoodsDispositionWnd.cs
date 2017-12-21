using Data.PlayerLocalHeroConfig;
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
    public class UWarGoodsDispositionWnd : UIPopupWnd<UWarGoodsdDispositionView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WarGoodsDisposition;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarGoodsDisposition/WarGoodsDispositionView";
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {

            this.m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);

            return base.Init();
        }
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            base.Destroy();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch(msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }


    public class UWarGoodsdDispositionView : UIBaseWndView,ITimerHanlder
    {
        public Text ViewTitle = null;
        public ListView DispositionListView = null;
        public Button CloseBtn = null;
        public RectTransform SubView01 = null;
        public RectTransform SubView02 = null;
        //子界面内容
        public Text DispositionNameTxt = null;
        public Text DelayTimeTxt = null;
        public Button ModifyBtn = null;
        public Button ConfirmBtn = null;

        private int nDelayCount = 10;
        private int DispositionIndex = 0;

        public override bool Init(IUIWnd wnd)
        {
            bool result = base.Init(wnd);
            WarMobaGoodsDataManager.InitHeroDispositionListEvent += SetDispositionListViewData;
            if(CloseBtn != null)
            {
                CloseBtn.onClick.AddListener(OnCloseBtnClick);
            }
            if(ModifyBtn != null)
            {
                ModifyBtn.onClick.AddListener(OnModifyBtnClick);
            }
            if(ConfirmBtn != null)
            {
                ConfirmBtn.onClick.AddListener(OnConfirmBtnClick);
            }
            return result;
        }

        protected void OnCloseBtnClick()
        {
            m_wnd.SetVisible(false);
        }

        private void SetDispositionListViewData()
        {
            //根据该英雄本地是否有修改过配置ID显示不同子界面
            int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            int DispositionID = -1;
            bool bShowSubView01 = true;
            if (nHeroID > 0)
            {
                PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_GoodsDispositionID, nHeroID, out DispositionID);
            }

            //增加物品推荐配置列表数据
            UWarGoodsStoreDispositionItem disposition = null;
            DispositionListView.DataSource = new ObservableList<string>();
            DispositionListView.DataSource.BeginUpdate();
            for (int i=0;i< WarMobaGoodsDataManager.Instance.HeroDispositionList.Count;++i)
            {
                disposition = WarMobaGoodsDataManager.Instance.HeroDispositionList[i];
                string item = disposition.DispositionName + disposition.DispositionTitle;
                if (disposition.DispositionID == DispositionID)
                    DispositionIndex = i;
                DispositionListView.DataSource.Add(item);
            }
            DispositionListView.DataSource.EndUpdate();
            DispositionListView.OnSelectString.AddListener(OnDispositionSelect);

            //处理子界面显示
            bShowSubView01 = (DispositionID <= 0);

            SwitchSubView(bShowSubView01);

            if(bShowSubView01)
                DispositionListView.Select(0);
            else
            {
                string str = ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "GoodsDispositionView") + DispositionListView.DataSource[DispositionIndex];
                DispositionNameTxt.text = str;
            }

            TimerManager.SetTimer(this, 0, 1, 10);
            m_wnd.SetVisible(true);
        }

        protected void OnDispositionSelect(int _nIndex,string _strItem)
        {
            //暂时不需要，现在要改为选择某个推荐ID，设置商店顶部的按钮组状态
            //if (WarMobaGoodsDataManager.Instance.HeroDispositionList[_nIndex].DispositionID == 0)
            //{
            //    WarMobaGoodsDataManager.Instance.HeroCurrentGoodsDisposition = WarMobaGoodsDataManager.Instance.HeroDefaultGoodsDisposition;
            //}
            //else
            //{
            //    WarMobaGoodsDataManager.Instance.HeroCurrentGoodsDisposition = WarMobaGoodsDataManager.Instance.HeroDispositionList[_nIndex].DispositionGoods;
            //}
            //UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_CHANGEGOODSDISPOSITION, null);

            //UWGS_SelectedGoodsDispositionMsgData msgData = new UWGS_SelectedGoodsDispositionMsgData();
            //msgData.SelectedDisposition = WarMobaGoodsDataManager.Instance.HeroDispositionList[_nIndex];
            //UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SELECTEDRECOMMENDDISPOSITION, msgData);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM_SELECTED, 
                new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM_SELECTED, 
                WarMobaGoodsDataManager.Instance.HeroDispositionList[_nIndex].DispositionID, string.Empty, IntPtr.Zero, 0));

            //存下玩家英雄配置表里
            int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_GoodsDispositionID, nHeroID, WarMobaGoodsDataManager.Instance.HeroDispositionList[_nIndex].DispositionID);


            m_wnd.SetVisible(false);
        }

        public override void Destroy()
        {
            WarMobaGoodsDataManager.InitHeroDispositionListEvent -= SetDispositionListViewData;

            base.Destroy();
        }

        private void SwitchSubView(bool _bShowSubView01)
        {
            if(SubView01 != null)
            {
                SubView01.gameObject.SetActive(_bShowSubView01);
            }
            if (SubView02 != null)
            {
                SubView02.gameObject.SetActive(!_bShowSubView01);
            }
        }

        private void OnModifyBtnClick()
        {
            TimerManager.KillTimer(this, 0);
            SwitchSubView(true);
        }
        private void OnConfirmBtnClick()
        {
            DispositionListView.Select(DispositionIndex);
            m_wnd.SetVisible(false);
        }

        public void OnTimer(int nTimerID)
        {
            nDelayCount--;

            DelayTimeTxt.text = "(" + nDelayCount + ")";
            if (nDelayCount <= 0)
            {
                DispositionListView.Select(DispositionIndex);
                m_wnd.SetVisible(false);
            }
        }
    }
}
