using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using DataCenter;
using USpeedUI.UWidgets;
using System.Runtime.InteropServices;
using U3D_Render.Common;
using USpeedUI.HeroDetail;
using Data.PersonModel;

namespace USpeedUI.PlayerFrame
{
    public class UHeroPreviewFrame : UIBaseSubWnd<UViewHeroPreviewFrame>
    {
        public override int GetWndType()
        {
            return (int)PlayerSubWndType.WND_HERO;
        }

        public override String GetPrefabPath()
        {
            return "HeroPreviewFrame/HeroPreviewFrame";
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (m_wndView == null)
                return;
                
            if (visible)
            {
                int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
                ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero(nActorID);
                if (info.dicEntityHeroInfo.Count != 0)
                {
                    OnRecvHeroInfo();
                }
            }
            else
            {
               // UISystem.Instance.ShowWnd(WndID.WND_ID_HERODETAIL, false);
                UISystem.Instance.ShowWnd(WndID.WND_ID_UPGRADESTARHINT, false);
            }
        }

        public override bool Init()
        {
            SetVisible(false);
            return base.Init();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO:
                    {
                        if (m_wndView != null)
                        {
                            OnRecvHeroInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                            OnRecvHeroInfo(uiData.nParam);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnRecvHeroInfo(int nCurrentGold = 0)
        {
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero(nActorID);
            m_wndView.ShowHeroHead(info.dicEntityHeroInfo, info.dicEntityNoHaveHeroInfo, nCurrentGold);
        }
    }

   public class UViewHeroPreviewFrame : UIBaseSubWndView
    {
        public UTileViewHero HeroList;

        // 显示英雄头像星级及卡牌数量
        public void ShowHeroHead(Dictionary<int, cmd_entity_hero_info> dicEntityHeroInfo, Dictionary<int, cmd_entity_nohanve_hero> dicEntityNoHaveHeroInfo, int nCurrentGold = 0)
        {
            HeroList.DataSource.Clear();
            UIWidgets.ObservableList<UTileViewItemHero> dataSource = new UIWidgets.ObservableList<UTileViewItemHero>();

            // 遍历已拥有的英雄加到表中
            foreach (KeyValuePair<int, cmd_entity_hero_info> kvp in dicEntityHeroInfo)
            {
                cmd_entity_hero_info cmdInfo = kvp.Value;
                int nHeroID = cmdInfo.nHeroID;

                SHeroStarSchemeInfo pStarScheme = new SHeroStarSchemeInfo();
                if (!GameLogicAPI.getHeroStarConfig(nHeroID, cmdInfo.byStarLv, ref pStarScheme))
                    continue;
                bool bIsLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(nHeroID);
                int nNeedCard = bIsLifeHero && pStarScheme.nCardDiscount > 0 ? pStarScheme.nCardDiscount : pStarScheme.nNeedCount;
                int nNeedGold = bIsLifeHero && pStarScheme.nGoldDiscount > 0 ? pStarScheme.nGoldDiscount : pStarScheme.nExpendGold;
                if (nNeedCard <= 0 || nNeedGold <= 0)
                    continue;

                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (!GameLogicAPI.GetPersonModel((UInt16)nHeroID, ref modelData))
                    continue;

                UTileViewItemHero item = new UTileViewItemHero();
                item.nHeroID = nHeroID;
                item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, nHeroID);
                if (item.headIcon == null)
                    item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, 10);
                item.stName = cmdInfo.chHeroName;
                item.stTitle = modelData.szHeroTitle;
                item.nStarLv = cmdInfo.byStarLv;
                item.bHaveHero = (cmdInfo.bIsBuy > 0);
                item.bWeekFree = (cmdInfo.bIsWeekFree > 0);

                item.nStarLvLimit = pStarScheme.nLimitLV;
                item.fExpValue = (float)cmdInfo.nCardCount / nNeedCard;
                item.stStarExp = cmdInfo.nCardCount.ToString() + (cmdInfo.byStarLv >= pStarScheme.nLimitLV ? "" : "/" + nNeedCard.ToString());
                int nGold = nCurrentGold == 0 ? EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD) : nCurrentGold;

                // 已购买、未满级、卡够、钱够
                item.bUpGrade = (cmdInfo.bIsBuy > 0 && cmdInfo.byStarLv < pStarScheme.nLimitLV && item.fExpValue >= 1 && nGold >= nNeedGold);

                item.nUnlockLevel = modelData.nUnlockLevel;
                item.bFpsHero =  modelData.nPlayMode > 0;

                dataSource.Add(item);
            }

            // 遍历未拥有的英雄遍历到表中
            foreach (KeyValuePair<int, cmd_entity_nohanve_hero> kvp in dicEntityNoHaveHeroInfo)
            {
                cmd_entity_nohanve_hero cmdInfo = kvp.Value;
                int nHeroID = cmdInfo.nHeroID;

                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (!GameLogicAPI.GetPersonModel((UInt16)nHeroID, ref modelData))
                    continue;

                SHeroStarSchemeInfo pStarScheme = new SHeroStarSchemeInfo();
                if (!GameLogicAPI.getHeroStarConfig(nHeroID, 1, ref pStarScheme))
                    continue;

                bool bIsLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(nHeroID);
                int nNeedCard = bIsLifeHero && pStarScheme.nCardDiscount > 0 ? pStarScheme.nCardDiscount : pStarScheme.nNeedCount;
                int nNeedGold = bIsLifeHero && pStarScheme.nGoldDiscount > 0 ? pStarScheme.nGoldDiscount : pStarScheme.nExpendGold;
                if (nNeedCard <= 0 || nNeedGold <= 0)
                    continue;

                UTileViewItemHero item = new UTileViewItemHero();
                item.nHeroID = nHeroID;
                item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, nHeroID);
                if (item.headIcon == null)
                    item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, 10);
                item.stName = cmdInfo.chHeroName;
                item.stTitle = modelData.szHeroTitle;
                item.nStarLv = 1;
                item.bHaveHero = false;
                item.bWeekFree = false;

                item.nStarLvLimit = pStarScheme.nLimitLV;
                item.fExpValue = (float)cmdInfo.nHaveCardCount / nNeedCard;
                item.stStarExp = cmdInfo.nHaveCardCount.ToString() + "/" + nNeedCard.ToString();
                item.bUpGrade = false;

                item.nUnlockLevel = modelData.nUnlockLevel;
                item.bFpsHero = modelData.nPlayMode > 0;

                dataSource.Add(item);
            }
            dataSource.Sort(CompareMemberList);
            HeroList.DataSource = dataSource;

            //增加监听点击头像的事件
            HeroList.OnSelect.RemoveAllListeners();
            HeroList.OnSelect.AddListener(ItemSelected);
        }

        // 当玩家点击头像时
        void ItemSelected(int index, UIWidgets.ListViewItem component)
        {
            string stName = HeroList.DataSource[index].stName;
            int nHeroID = HeroList.DataSource[index].nHeroID;
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;

            ActorHeroDetailInfo info = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(nHeroID, nActorID);
            if (info.heroDetailInfo.nHeroID != 0)
            {
                cmd_Return_Hero_Detail_Info returnHeroInfo;
                returnHeroInfo.nPDBID = nActorID;
                returnHeroInfo.nHeroID = nHeroID;

                UIMsgCmdData uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, ref returnHeroInfo);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, uiReturnHeroInfo);
            }
        }

        private int CompareMemberList(UTileViewItemHero A, UTileViewItemHero B)
        {
            int nActorLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            int nIsWeekFreeA = A.bWeekFree ? 1 : 0;
            int nIsWeekFreeB = B.bWeekFree ? 1 : 0;
            int nIsHaveHeroA = A.bHaveHero ? 1 : 0;
            int nIsHaveHeroB = B.bHaveHero ? 1 : 0;
            int nUnlockLevelA = (A.nUnlockLevel <= 0 || (nActorLevel >= A.nUnlockLevel && !A.bHaveHero)) ? Int32.MaxValue : A.nUnlockLevel;
            int nUnlockLevelB = (B.nUnlockLevel <= 0 || (nActorLevel >= B.nUnlockLevel && !B.bHaveHero)) ? Int32.MaxValue : B.nUnlockLevel;
            if (nIsHaveHeroA != nIsHaveHeroB)
                return (nIsHaveHeroA > nIsHaveHeroB) ? -1 : 1;
            else if (nUnlockLevelA != nUnlockLevelB)
                return (nUnlockLevelA < nUnlockLevelB) ? -1 : 1;
            else if (A.nStarLv != B.nStarLv)
                return (A.nStarLv > B.nStarLv) ? -1 : 1;
            else if (nIsWeekFreeA != nIsWeekFreeB)
                return (nIsWeekFreeA > nIsWeekFreeB) ? -1 : 1;
            else
                return (A.nHeroID < B.nHeroID) ? -1 : 1;
        }
    }
}
