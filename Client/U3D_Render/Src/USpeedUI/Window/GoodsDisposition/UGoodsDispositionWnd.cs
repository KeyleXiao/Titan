using ASpeedGame.Data.GameGoodsLabelDesConfig;
using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.WarGoodsStore;

namespace USpeedUI.GoodsDisposition
{
    public class UGoodsDispositionWnd : UIPopupWnd<UGoodsDispositionView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_GOODSDISPOSITION;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/GoodsDisposition/GoodsDispositionView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            m_bHaveEscCallBack = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, this);


            //UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.UseWing, KeyStatus.Down, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, this);

        }
        int KeyState;
        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            switch (index)
            {
                case FuntionShortCutSlotIndex.UseWing:
                    {
                        if (status == KeyStatus.Down)
                        {
                            KeyState++;
                            SetVisible(KeyState % 2 == 1);
                        }
                    }
                    break;
            }
            return base.OnKeyMessage(index, status);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                #region 加载/卸载消息
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                    #endregion
            }
            OnCustomGoodsFrameMessage(msgID, data);
        }

        public override bool HaveEscCallBack()
        {
            return m_wndView.HaveEscCallBack();
        }

        //public override bool OnCloseKey()
        //{
        //    return m_wndView.OnCloseKey();
        //}

        #region 自定义物品配置界面
        public bool InitGoodsScheme()
        {
            m_wndView.InitGoodsScheme(GoodsDispositionDataManager.Instance.SchemeGoodsLabelDescirptionTable, GoodsDispositionDataManager.Instance.SchemeMobaGoodsDataSource);

            return true;
        }

        public void InitHeroHeadPortraitListHeroTypeItem()
        {
            m_wndView.InitHeroHeadPortraitListHeroTypeItem(GoodsDispositionDataManager.Instance.AllHeroTypeList);
        }

        protected void OnCustomGoodsFrameMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                #region 拖拽消息
                case WndMsgID.WND_MSG_GOODSDISPOSITION_BEGINDRAG:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GoodsBeginDrag(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_DRAGGING:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GoodsDragging(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_ENDDRAG:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GoodsEndDrag(cmdData.nParam);
                        }
                    }
                    break;
                #endregion

                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROPORTRAITITEMLIST:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetPlayerHeroPortraitItem(GoodsDispositionDataManager.Instance.CurrentHeroPortraitItemList);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetCurrentHero(GoodsDispositionDataManager.Instance.CurrentSelectHero);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONLIST:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetGoodsDispositionListData(GoodsDispositionDataManager.Instance.GoodsDispositionList);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTDISPOSITION:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetCurrentGoodsDisposition(GoodsDispositionDataManager.Instance.CurrentGoodsDisposition);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION:
                    {
                        GoodsDispositionDataManager.Instance.CurrentGoodsDisposition.Author = EntityFactory.MainHeroView.Property.CreatureName;
                        EquipSchemeDataManager.Instance.saveEquipScheme(GoodsDispositionDataManager.Instance.CurrentSelectHero, GoodsDispositionDataManager.Instance.CurrentGoodsDisposition.To());
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_CLOSE_GOODSDISPOSITIONLIST:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.CloseGoodsDispositionList();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTSELECTEDGOODS:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetCurrentSelectedGoods(GoodsDispositionDataManager.Instance.CurrentSelectGoodsID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetVisible(true);
                            GoodsDispositionDataManager.Instance.CurrentSelectHero = cmdData.nParam;
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_OPEN_GOODSSYNTHESISVIEW:
                    {
                        if(m_wndView != null && GoodsDispositionDataManager.Instance.CurrentSelectGoodsID >= 0)
                        {
                            m_wndView.OpenGoodsSynthesisView(GoodsDispositionDataManager.Instance.CurrentSelectGoodsID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_OPEN_DISPOSITIONFILLED:
                    {
                        UGD_DispositionFilledMsgData msgData = data as UGD_DispositionFilledMsgData;
                        if (m_wndView != null && msgData != null)
                        {
                            m_wndView.OpenDispositionFilled(msgData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_COPY_CURRENTDISPOSITION:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.OpenDispositionCopy();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONFILLEDLIST:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.UpdateDispositionFilled();
                        }
                    }
                    break;
            }
        }
        #endregion


        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (_bVisible)
            {
                //显示的时候首先重新取一次玩家英雄数据
                GoodsDispositionDataManager.Instance.SetPlayerHeroPortraitItemList();
            }
            else
            {
                if (!GoodsDispositionDataManager.Instance.IsCurrentNeedToSave(true,
                    () =>
                    {
                        UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION, null);
                        GoodsDispositionDataManager.Instance.ClearCurrentInfo();
                        return true;
                    },
                    () =>
                    {
                        GoodsDispositionDataManager.Instance.ClearCurrentInfo();
                        return true;
                    }))
                {
                    GoodsDispositionDataManager.Instance.ClearCurrentInfo();
                }
            }
        }


        public void SetView(UGoodsDispositionView _view)
        {
            m_wndView = _view;
        }
    }
}
