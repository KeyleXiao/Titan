using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

namespace USpeedUI.WarGoodsStore
{


    public class UWarGoodsStoreWnd : UIPopupWnd<UWarGoodsStoreView>
    {
        private GameMobaSchemesManager.MobaGoodsSchemesEventArgs m_MobaGoodsSchemesEventArgs;
        //记录当前金钱数
        private int m_nMoney;

        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_GOODSSTORE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarGoodsStore/WarGoodsStore";
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            GameMobaSchemesManager.MobaGoodsSchemesInitEndEvent += GetMobaGoodsFromSchemes;
            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);

            //WillNote::测试用,可删
            //SetVisible(true);
            
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED, this);


            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.OpenGoodStore, KeyStatus.Down, this);



            return base.Init();
        }

        public override void Destroy()
        {
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
            GameMobaSchemesManager.MobaGoodsSchemesInitEndEvent -= GetMobaGoodsFromSchemes;

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.OpenGoodStore, KeyStatus.Down, this);


            base.Destroy();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
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
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        SetPurchasedGoodsSlot();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE:
                    {
                        SetVisible(!IsVisible());
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL:
                    {
                        if (m_wndView == null || !(msgData is UIMsgCmdData))
                        {
                            return;
                        }
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        WarMobaGoodsDataManager.Instance.CanBuyOrSell = Convert.ToBoolean(cmdData.nParam);
                        m_wndView.SetBuyBtnEnable(WarMobaGoodsDataManager.Instance.CanBuyOrSell);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        bool bCanRevoked = Convert.ToBoolean(cmdData.nParam);
                        m_wndView.SetCanRevoked(bCanRevoked);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_CHANGEGOODSDISPOSITION:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.SetCurrentGoodsDisposition(WarMobaGoodsDataManager.Instance.HeroCurrentGoodsDisposition);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGED:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetSearchValueChanged(cmdData.nParam, cmdData.strParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGEEND:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetSearchValueEndEdit(cmdData.strParam);
                        }
                    }
                    break;

                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SELECTEDRECOMMENDDISPOSITION:
                    {
                        UWGS_SelectedGoodsDispositionMsgData cmdData = msgData as UWGS_SelectedGoodsDispositionMsgData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetSelectedGoodsDispositionInfo(cmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_GET_TOGGLEDISPOSITIONSTATE:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            bool result = cmdData.nParam == 1 ? true : false;
                            m_wndView.GetToggleDisposition(result);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_TOGGLEDISPOSITIONSTATE:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            bool result = cmdData.nParam == 1 ? true : false;
                            m_wndView.SetToggleState(result);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetGoodsStoreTabItem(cmdData.nParam,cmdData.strParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_DISPOSITIONTABITEM_SELECTED:
                    {
                        UIMsgCmdData cmdData = msgData as UIMsgCmdData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetGoodsStoreTabItemSelected(cmdData.nParam);
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null)
                return false;
            if (index == FuntionShortCutSlotIndex.OpenGoodStore && status == KeyStatus.Down)
            {
                SetVisible(!IsVisible());
            }

            return true;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            onMoneyChange();
        }

        private void GetMobaGoodsFromSchemes(GameMobaSchemesManager.MobaGoodsSchemesEventArgs arg)
        {
            if (m_wndView == null)
            {
               // Trace.LogWarning("WillTrace::暂时加个打印，在接受物品配置表时，界面还没初始化，看看有没有延后初始化。");
                m_MobaGoodsSchemesEventArgs = arg;
            }
            else
            {
                //Trace.LogWarning("WillTrace::暂时加个打印，在接受物品配置表时,界面已初始化。");

                m_wndView.InitMobaGoodsScheme(arg);

            }
        }

        /// <summary>
        /// view主动申请初始化物品配置
        /// </summary>
        /// <returns>true:配置先来，subview还没初始化；false:配置没来，subview已经初始化，等配置来了会走GetMobaGoodsFromSchemes()方法初始化</returns>
        public bool InitMobaGoodsScheme()
        {
            if(m_MobaGoodsSchemesEventArgs != null)
            {
                //Trace.LogWarning("WillTrace::暂时加个打印，在接受物品配置表时,界面还没初始化，现在进行延后初始化。");

                m_wndView.InitMobaGoodsScheme(m_MobaGoodsSchemesEventArgs);

                return true;
            }
            return false;
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (e.objev == null)
            {
                return;
            }

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT:
                    {
                        if (e.objev.IsHero)
                        {
                            m_nMoney = e.data.nValue;

                            onMoneyChange();
                        }
                    }
                    break;
            }

        }

        private void onMoneyChange()
        {
            if (IsVisible())
            {
                LogicDataCenter.warMobaGoodsDataManager.onMoneyChange(m_nMoney);
                m_wndView.onMoneyChange(m_nMoney);
            }
        }

        internal void onBtnBuyClick(int _nBuyGoodsID)
        {
            if (!WarMobaGoodsDataManager.Instance.CanBuyOrSell)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_GOODS_CANNOTBUY_STATE);
                return;
            }

            if (_nBuyGoodsID <= 0)
                return;

            cmd_war_personbuygoods buyGoods = new cmd_war_personbuygoods();
            buyGoods.nGoodID = _nBuyGoodsID;


            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_war_personbuygoods>(ref buyGoods);
            int len = Marshal.SizeOf(buyGoods);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_BUY_GOOD, len, "", ptr, len);

        }

        internal void onBtnRevokedClick()
        {
            if (WarMobaGoodsDataManager.Instance.CanBuyOrSell)
            {
                cmd_war_revocation_operation data = new cmd_war_revocation_operation();
                data.uUID = (uint)EntityFactory.MainHeroID;
                IntPtrHelper helper = new IntPtrHelper();
                IntPtr ptr = helper.toPtr<cmd_war_revocation_operation>(ref data);
                int len = Marshal.SizeOf(data);
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_REVOCATION_OPERATION, len, "", ptr, len);
            }
        }

        internal void onBtnSellClick(int _nGoodsID,int _nSlotIndex)
        {
            if (WarMobaGoodsDataManager.Instance.CanBuyOrSell && _nGoodsID > 0 && _nSlotIndex != -1)
            {
                cmd_war_personsellgoods data = new cmd_war_personsellgoods();
                data.nGoodID = _nGoodsID;
                data.nSlotIndex = _nSlotIndex;

                IntPtrHelper helper = new IntPtrHelper();
                IntPtr ptr = helper.toPtr<cmd_war_personsellgoods>(ref data);
                int len = Marshal.SizeOf(data);
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_SELL_GOOD, len, "", ptr, len);
            }
        }

        #region 购买物品
        /// <summary>
        /// view 显示已购买物品
        /// </summary>
        /// <param name="data"></param>
        private void SetPurchasedGoodsSlot()
        {
            LogicDataCenter.warMobaGoodsDataManager.onPurchasedChange();
            //foreach(var purchasedGoods in LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo())
            {
                //Trace.Log("显示已购买物品:" + purchasedGoods.nGoodID);
                m_wndView.SetPurchasedGoods(LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo());
            }

            m_wndView.SetTreeSynthesisPurchasedIcon(LogicDataCenter.warMobaGoodsDataManager.GetPurchasedGoodsIDList());
        }

        #endregion
    }
}
