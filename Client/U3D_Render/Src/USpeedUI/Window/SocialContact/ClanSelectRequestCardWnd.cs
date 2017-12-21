using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using UIWidgets;

namespace USpeedUI.ClanSelectRequestCard
{
    public class ClanSelectRequestCardWnd : UIBaseWnd<ClanSelectRequestCardWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_CLAN_SELECTREQUESTCARD;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanSelectRequestCard";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            SetVisible(false);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (m_wndView != null)
            {
                m_wndView.OnClanSelfHero();
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                default:
                    break;
            }
        }
    }

    public class ClanSelectRequestCardWndView : UIBaseWndView
    {
        public Text selectRequestCardTitle;
        public Text confirmBtnDesc;
        public UTileClanSelfHero tileClanRequestCard;

        public override bool Init(IUIWnd wnd)
        {
            selectRequestCardTitle.text = ULocalizationService.Instance.Get("UIView", "ClanSelectRequestCard", "SelectTitle");
            confirmBtnDesc.text = ULocalizationService.Instance.Get("UIView", "ClanSelectRequestCard", "ConfirmBtn");
            return base.Init(wnd);
        }

        public void OnClanSelfHero()
        {
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.Count <= 0)
                return;

            tileClanRequestCard.DataSource.Clear();
            tileClanRequestCard.DataSource.BeginUpdate();
            foreach (var item in info.dicEntityHeroInfo)
            {
                cmd_clan_request_selfhero data = new cmd_clan_request_selfhero();
                data.nHeroID = item.Value.nHeroID;
                data.nHeroStar = item.Value.byStarLv;
                data.nCardCount = item.Value.nCardCount;
                bool bIsLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(item.Value.nHeroID);
                data.nNeedCardCount = GameLogicAPI.getHeroStarNeedCard(item.Value.nHeroID, item.Value.byStarLv, bIsLifeHero);

                UTileItemClanSelfHero tileItem = new UTileItemClanSelfHero();
                tileItem.data = data;
                tileClanRequestCard.DataSource.Add(tileItem);
            }
            tileClanRequestCard.DataSource.EndUpdate();
        }

        public void onCloseSelfHeroBtnClick()
        {
            // 帮会自己英雄窗口关闭
            m_wnd.SetVisible(false);
        }

        public void onEnterSelfHeroBtnClick()
        {
            if (tileClanRequestCard.SelectedItem == null)
            {
                return;
            }

            cmd_clan_request_card cmdData;
            cmdData.nHeroID = tileClanRequestCard.SelectedItem.data.nHeroID;
            // 请求支援
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_REQUEST_CARD, ref cmdData);

            // 关闭自身
            m_wnd.SetVisible(false);
        }
    }
}
