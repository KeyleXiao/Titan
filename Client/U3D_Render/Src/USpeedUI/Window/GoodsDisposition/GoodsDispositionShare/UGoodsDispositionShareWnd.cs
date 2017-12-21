using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.GoodsDisposition
{
    class UGDS_SubWnd_GoodsDisposition : UIBaseSubWnd<UGDS_SubView_GoodsDisposition>
    {
        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/GoodsDispositionShare/SubView/GoodsDisposition";
        }

        public override int GetWndType()
        {
            return (int)UGoodsDispositionShareWnd.ELevel1_SubWnd.GoodsDisposition;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch(msgID)
            {
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO:
                    {
                        UEquipSchemeMsgData cmdData = msgData as UEquipSchemeMsgData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetData(cmdData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    class UGDS_SubWnd_DispositionFilled : UIBaseSubWnd<UGDS_SubView_DispositionFilled>
    {
        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/GoodsDisposition/CGFrame/SubLevel2/DispositionFilled";
        }

        public override int GetWndType()
        {
            return (int)UGoodsDispositionShareWnd.ELevel1_SubWnd.DispositionFilled;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SAVE_DISPOSITIONINFO:
                    {
                        UEquipSchemeMsgData cmdData = msgData as UEquipSchemeMsgData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.SetDispositionFilled(cmdData.nHeroID);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

    }


    public class UGoodsDispositionShareWnd : UIPopupWnd<UGoodsDispositionShareView>
    {        
        //子窗口列表
        private List<IUISubWnd> m_SubWndList;
        /// <summary>
        /// 这个界面估计不会很大，所以直接把数据存到这了，有需要再开个数据单独存放
        /// </summary>
        protected GameLogic.cmd_war_equipscheme m_nCurrentEquipScheme;
        /// <summary>
        /// 当前分享配置用于的英雄ID
        /// </summary>
        protected int m_nHeroID;

        /// <summary>
        /// 当前分享的英雄物品信息
        /// </summary>
        protected UEquipSchemeMsgData m_nCurrentShareEquipSchemeData;


        public enum ELevel1_SubWnd
        {
            GoodsDisposition,
            DispositionFilled,
        }

        public override WndID GetID()
        {
            return WndID.WND_ID_GOODSDISPOSITION_SHARE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/GoodsDispositionShare/GoodsDispositionShareView";
        }

        public override bool Init()
        {
            m_SubWndList = new List<IUISubWnd>();
            m_SubWndList.Add(new UGDS_SubWnd_GoodsDisposition());
            m_SubWndList.Add(new UGDS_SubWnd_DispositionFilled());

            //子窗口初始化
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.Init();
            }

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO, this);

            return base.Init();
        }

        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO, this);
            base.Destroy();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch(msgID)
            {
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SAVE_DISPOSITIONINFO:
                    {
                        //m_nCurrentEquipScheme.
                        if (DataCenter.GoodsDispositionDataManager.Instance.AddHeroGoodsDispositionFromShare(m_nCurrentShareEquipSchemeData.nHeroID, m_nCurrentShareEquipSchemeData.scheme))
                            SetVisible(false);
                        else
                        {
                            if(m_wndView != null)
                            {
                                GetSubWnd(ELevel1_SubWnd.DispositionFilled).SetVisible(true);

                                GetSubWnd(ELevel1_SubWnd.DispositionFilled).OnMessage(msgID, m_nCurrentShareEquipSchemeData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO:
                    {
                        SetVisible(true);
                        GetSubWnd(ELevel1_SubWnd.DispositionFilled).SetVisible(false);

                        GetSubWnd(ELevel1_SubWnd.GoodsDisposition).OnMessage(msgID, data);
                        UEquipSchemeMsgData cmdData = data as UEquipSchemeMsgData;
                        if (m_wndView != null && cmdData != null)
                        {
                            m_nCurrentShareEquipSchemeData = cmdData;
                            m_wndView.SetTitleData(String.Format(ULocalizationService.Instance.Get("UIView", "EquipScheme", "EquipSchemeLink"), cmdData.scheme.szAuthorName));
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_CLOSE:
                    {
                        if(m_wndView != null)
                        {
                            SetVisible(false);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private IUISubWnd GetSubWnd(ELevel1_SubWnd wndType)
        {
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    return wnd;
            }
            return null;
        }

        public void InitSubView(ELevel1_SubWnd _eSubWnd,RectTransform _rtParent,bool _bVisible)
        {
            GetSubWnd(_eSubWnd).SetContainer(_rtParent);
            GetSubWnd(_eSubWnd).LoadView();
            GetSubWnd(_eSubWnd).SetVisible(_bVisible);          
        }
    }
}
