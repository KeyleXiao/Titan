/*******************************************************************
** 文件名:	UHeroDetail.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-11-21
** 版  本:	1.0
** 描  述:	英雄详细信息界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.PlayerFrame;
using USpeedUI.UWidgets;
using U3D_Render;

namespace USpeedUI.HeroDetail
{
    public class HeroDetailViewData
    {
        private static HeroDetailViewData m_singleton;
        public static HeroDetailViewData Instance
        {
            get
            {
                if (m_singleton == null)
                    m_singleton = new HeroDetailViewData();

                return m_singleton;
            }
        }

        public ActorHeroDetailInfo info;
        public int nReqPDBID;
        public int nHeroID;
        public EHeroDetailPartsType eType = EHeroDetailPartsType.EHDPT_PANDECT;

        public void Clear()
        {
            nHeroID = 0;
            nReqPDBID = 0;
        }
    }

    public class UHeroDetail : UIFillWnd<UViewHeroDetail>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_HERODETAIL;
        }

        public override String GetPrefabPath()
        {
            return "HeroDetailInfo/HeroDetailInfo";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (m_wndView == null)
                return;
            if (_bVisible)
            {
                SetSkinModelScene();
            }
            else
            {
                SceneManager.Instance.UnLoadSelectHeroScene();
            }
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFOREFRESH, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILXPUPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_UPDATESTATICTALENT, this);
            
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
                       
            return true;
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFOREFRESH, this);      
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILXPUPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_UPDATESTATICTALENT, this);

            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO:
                    {
                        HeroDetailViewData.Instance.Clear();
                        UIMsgCmdData uiInitHeroInfo = (UIMsgCmdData)msgData;
                        cmd_Return_Hero_Detail_Info initHeroInfo = IntPtrHelper.toData<cmd_Return_Hero_Detail_Info>(uiInitHeroInfo.ptrParam);
                        SetHeroDetailData(initHeroInfo.nHeroID, initHeroInfo.nPDBID);

                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFOREFRESH:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRefreshHeroInfo = (UIMsgCmdData)msgData;
                        cmd_Return_Hero_Detail_Info refreshHeroInfo = IntPtrHelper.toData<cmd_Return_Hero_Detail_Info>(uiRefreshHeroInfo.ptrParam);
                        if (refreshHeroInfo.nHeroID != HeroDetailViewData.Instance.nHeroID ||refreshHeroInfo.nPDBID != HeroDetailViewData.Instance.nReqPDBID)
                            return;

                        SetHeroDetailData(refreshHeroInfo.nHeroID, refreshHeroInfo.nPDBID);
                        m_wndView.OnBuyOrChangeHeroSuccess();
                    }
                    break;
                case WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiHeroInfo = (UIMsgCmdData)msgData;
                        cmd_entity_hero_diff_expend cmdData = IntPtrHelper.toData<cmd_entity_hero_diff_expend>(uiHeroInfo.ptrParam);
                        m_wndView.OnRecvHeroDiffInfo(cmdData);
                    }
                    break;
                case WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT:
                    {
                        if (m_wndView == null)
                            return;

                        if (!this.IsVisible())
                            return;

                        UIMsgCmdData uiHeroUpStarInfo = (UIMsgCmdData)msgData;
                        cmd_Return_HeroDetail_Upgrade_Success cmdUpStarData = IntPtrHelper.toData<cmd_Return_HeroDetail_Upgrade_Success>(uiHeroUpStarInfo.ptrParam);
                        m_wndView.ShowUpStarSucess(cmdUpStarData);
                    }
                    break;
                case WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.OnBuyOrChangeHeroSuccess();
                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_HERODETAILXPUPDATE:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.UpdateXPSkillList();
                    }
                    break;
                case WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO:
                    {
                        if (m_wndView != null)
                        {
                            // 购买卡牌，卡牌数据主动更新
                            int nHeroID = HeroDetailViewData.Instance.nHeroID;
                            IntPtrHelper helper = new IntPtrHelper();
                            cmd_get_hero_detail_info data = new cmd_get_hero_detail_info();
                            data.nHeroID = nHeroID;
                            data.nStarLv = 1;
                            data.nStaticLv = 1;
                            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
                            {
                                data.nStarLv = info.dicEntityHeroInfo[nHeroID].byStarLv;
                                data.nStaticLv = info.dicEntityHeroInfo[nHeroID].nLv;
                            }
                            IntPtr ptr = helper.toPtr<cmd_get_hero_detail_info>(ref data);
                            int len = Marshal.SizeOf(data);

                            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO, len, "", ptr, len);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD:
                    {
                        if (m_wndView != null && IsVisible())
                        {
                            UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                            m_wndView.UpdateUpStarGoldView(uiData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_ID_HEROTALENTVIEW_UPDATESTATICTALENT:
                    {
                        if (m_wndView != null && IsVisible())
                        {
                            m_wndView.OnUpdateHeroTalent();
                        }
                    }
                    break;
                #region 新手引导相关
                case WndMsgID.WND_MSG_HERODETAIL_GUIDE_ACTIONBEGIN:
                    {
                        UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;

                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GuideActionBegin(cmdData);
                        }

                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_GUIDE_ACTIONEND:
                    {
                        UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;

                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GuideActionEnd(cmdData);
                        }
                    }
                    break;
                #endregion
                default:
                    break;
            }
        }

        private void SetSkinModelScene()
        {
            SceneManager.Instance.LoadSelectHeroScene(SetRoomShowAfterLoadScene);
        }

        public void SetRoomShowAfterLoadScene(SceneResInfoManager info)
        {
            if (m_wndView)
            {
                m_wndView.CommomFrame.SkinModelFrame.SelectTransForm = info.SceneRoot.transform;
                m_wndView.CommomFrame.UpstarBtnFrame.SelectTransForm = info.SceneRoot.transform;
                m_wndView.EffectFrame.SelectTransForm = info.SceneRoot.transform;

                m_wndView.SetHeroSkinModel(HeroDetailViewData.Instance.nHeroID);
            }
        }

        private void SetHeroDetailData(int nHeroID,int nRePDBIU)
        {
            HeroDetailViewData.Instance.info = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(nHeroID, nRePDBIU);
            HeroDetailViewData.Instance.nHeroID = nHeroID;
            HeroDetailViewData.Instance.nReqPDBID = nRePDBIU;
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            if (m_wndView == null)
            {
                return;
            }

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_SKIN:
                    {
                        m_wndView.OnBuyOrChangeHeroSuccess();
                    }
                    break;
                default:
                    break;
            } 
        }

        public override bool OnCloseKey()
        {
            base.OnCloseKey();
            UnloadView();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_CLOSE);

            return true;
        }

    }
}
