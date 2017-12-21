using System;
using System.Collections.Generic;
using System.Text;
using GameLogic;
using UnityEngine;
using USpeedUI.PointShop;
using USpeedUI;
using Data.PointShopConfig;
using Data.ActorPrizeConfig;
using War;
using UnityEngine.UI;


namespace DataCenter
{
    public class PointShopDataManager : ITimerHanlder
    {
        enum ETIMER
        {
            ETIMER_REFLUSHLEFTTIME = 0,	// 刷新卡牌剩余时间
        };

        private Dictionary<int, cmd_entity_update_pointshop_buynum_info> buyNumInfo;
        public Dictionary<int, cmd_entity_update_pointshop_buynum_info> BuyNumInfo { get { return buyNumInfo; } }
        private List<int> cardlistInfo;
        public List<int> CardListInfo { get { return cardlistInfo; } }
        private int m_leftReflushTime;
        public int LeftReflushTime { get { return m_leftReflushTime; } }
        private int m_leftReflushNum;
        public int LeftReflushNum { get { return m_leftReflushNum; } }

        public ResNode prizeIconItemRes;
        public ResNode shopMoneyItemRes;
        public ResNode singleShopGoodsItemRes;
        public ResNode squareHighlightItemRes;
        public ResNode singleSortBtnItemRes;
        public ResNode heroStarItemRes;
        public void Init()
        {
            buyNumInfo = new Dictionary<int, cmd_entity_update_pointshop_buynum_info>();
            cardlistInfo = new List<int>();

            prizeIconItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/PrizeIcon");
            shopMoneyItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/ShopMoneyItem");
            singleShopGoodsItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/PointShopSingleGoods");
            squareHighlightItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/SquareHighlightItem");
            singleSortBtnItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/PointShopSortItem");
            heroStarItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PointShop/HeroStarItem");

            TimerManager.SetTimer(this, (int)ETIMER.ETIMER_REFLUSHLEFTTIME, 1.0f);
        }
        public void Clear()
        {
            AssetBundleManager.DeleteAssets(ref shopMoneyItemRes,true);
            AssetBundleManager.DeleteAssets(ref shopMoneyItemRes, true);
            AssetBundleManager.DeleteAssets(ref singleShopGoodsItemRes, true);
            AssetBundleManager.DeleteAssets(ref squareHighlightItemRes, true);
            AssetBundleManager.DeleteAssets(ref singleSortBtnItemRes, true);
            AssetBundleManager.DeleteAssets(ref heroStarItemRes, true);

            TimerManager.KillTimer(this, (int)ETIMER.ETIMER_REFLUSHLEFTTIME);
        }
        public PrizeIconItem CreatePrizeIconItem(Transform iconContainer)
        {
            GameObject newItem = prizeIconItemRes.InstanceMainRes();
            PrizeIconItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<PrizeIconItem>();
                if (result)
                {
                    result.SetContainer(iconContainer as RectTransform);
                }
                else
                {
                    prizeIconItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public ShopMoneyItem CreateShopMoneyItem(Transform container)
        {
            GameObject newItem = shopMoneyItemRes.InstanceMainRes();
            ShopMoneyItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<ShopMoneyItem>();
                if (result)
                {
                    result.Init(container as RectTransform);
                }
                else
                {
                    shopMoneyItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public PointShopSingleGoodsItem CreateShopSingleGoodsItem(Transform container)
        {
            GameObject newItem = singleShopGoodsItemRes.InstanceMainRes();
            PointShopSingleGoodsItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<PointShopSingleGoodsItem>();
                if (result)
                {
                    result.Init(container as RectTransform);
                }
                else
                {
                    singleShopGoodsItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public SquareHighlightItem CreateSquareHighlightItem(Transform container)
        {
            GameObject newItem = squareHighlightItemRes.InstanceMainRes();
            SquareHighlightItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<SquareHighlightItem>();
                if (result)
                {
                    result.Init(container as RectTransform);
                }
                else
                {
                    squareHighlightItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public HeroStarItem CreateHeroStarItem(Transform container)
        {
            GameObject newItem = heroStarItemRes.InstanceMainRes();
            HeroStarItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<HeroStarItem>();
                if (result)
                {
                    result.Init(container as RectTransform);
                }
                else
                {
                    heroStarItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public PointShopSortItem CreateSingleSortBtnItem(Transform container, String strDesc, Comparison<SSchemePointShopGoodsConfig> comparison)
        {
            GameObject newItem = singleSortBtnItemRes.InstanceMainRes();
            PointShopSortItem result = null;
            if (newItem)
            {
                result = newItem.GetComponent<PointShopSortItem>();
                if (result)
                {
                    result.Init(container as RectTransform, strDesc, comparison);
                }
                else
                {
                    singleSortBtnItemRes.DestroyInstanceRes(ref newItem);
                }
            }

            return result;
        }

        public void UpdateBuyNumInfo(IntPtr ptrParam, int nPtrLen)
        {
            cmd_entity_update_pointshop_buynum_info data = IntPtrHelper.toData<cmd_entity_update_pointshop_buynum_info>(ptrParam);
            if (buyNumInfo.ContainsKey(data.nSellID))
            {
                buyNumInfo[data.nSellID] = data;
            }
            else
            {
                buyNumInfo.Add(data.nSellID, data);
            }

            // TODO 发送窗口消息
        }

        public void ReflushBuyNumInfo()
        {
            buyNumInfo.Clear();
        }

        public void OnBuyResult(IntPtr ptrParam, int nPtrLen)
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_POINTSHOP_BUYRESULT, 0, "", ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_BUYRESULT, uiData);
        }

        public void UpdateCardlist(IntPtr ptrParam, int nPtrLen)
        {
            cmd_entity_pointshop_update_cardlist data = IntPtrHelper.toData<cmd_entity_pointshop_update_cardlist>(ptrParam);
            m_leftReflushTime = (int)data.nLeftReflushTime;
            m_leftReflushNum = data.nLeftReflushNum;
            cardlistInfo.Clear();
            for (int i = 0; i < data.nSellID.Length; ++i)
            {
                cardlistInfo.Add(data.nSellID[i]);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST, null);
        }

        public void ReqeustBuyGoods(int nSellID, int nResType, int nBuyNum, int nClientPrice)
        {
            cmd_entity_pointshop_buygoods msgInfo = new cmd_entity_pointshop_buygoods();
            msgInfo.nSellID = nSellID;
            msgInfo.nResType = nResType;
            msgInfo.nBuyNum = nBuyNum;
            msgInfo.nClientPrice = nClientPrice;

            EntityEventHelper.Instance.SendCommand<cmd_entity_pointshop_buygoods>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_POINTSHOP_BUYGOODS, ref msgInfo);
        }

        public bool CheckCanBuyGoods(int nSellID, int nNum)
        {
            SSchemePointShopGoodsConfig goodsConfig = PointShopConfig.Instance.GetPointShopGoodsConfig(nSellID);
            if (goodsConfig == null)
                return false;

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(goodsConfig.nPrizeID);
            if (prizeConfig == null)
                return false;

            if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                int nHeroID = prizeConfig.nPrizeParam[0];
                ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                if (actorHeroInfo == null)
                    return false;

                cmd_entity_hero_info heroData;
                if (actorHeroInfo.dicEntityHeroInfo.TryGetValue(nHeroID, out heroData))
                {
                    if (heroData.bIsBuy > 0)
                        return false;
                }
            }

            if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
            {
                cmd_Hero_Skin_Info cmdSkinInfo = new cmd_Hero_Skin_Info();
                SkinInfoScheme skinConfig = new SkinInfoScheme();
                if (!GameLogicAPI.GetHeroSkinInfo(prizeConfig.nPrizeParam[1], ref skinConfig))
                    return false;

                bool bHaveSkin;
                if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(prizeConfig.nPrizeParam[0], prizeConfig.nPrizeParam[1], ref cmdSkinInfo))
                    bHaveSkin = true;
                else
                    bHaveSkin = skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL ? true : false;

                if (bHaveSkin)
                    return false;
            }

            return true;
        }

        public bool checkIsHaveSkin(int nHeroId, int nSkinId)
        {
            cmd_Hero_Skin_Info cmdSkinInfo = new cmd_Hero_Skin_Info();
            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (!GameLogicAPI.GetHeroSkinInfo(nSkinId, ref skinConfig))
                return false;

            // 拥有皮肤 或 拥有英雄的默认皮肤
            bool bHaveHero = LogicDataCenter.playerSystemDataManager.CheckIsHaveHero(nHeroId);
            bool bHaveSkin;
            if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroId, nSkinId, ref cmdSkinInfo))
                bHaveSkin = true;
            else
                bHaveSkin = (bHaveHero && skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL) ? true : false;

            if (!bHaveSkin)
                return false;

            return true;
        }

        public int CalcNumLimitPrice(int nLimitID, int nSellID, int nResType, int nBuyNum)
        {
            SPointShopNumLimitPrice limitConfig = PointShopConfig.Instance.GetShopNumLimitPriceConfig(nLimitID);
            if (limitConfig == null)
                return 0;

            int nBoughtNum = 0;
            cmd_entity_update_pointshop_buynum_info buyNumInfo = new cmd_entity_update_pointshop_buynum_info();
            if (BuyNumInfo.TryGetValue(nSellID, out buyNumInfo))
            {
                nBoughtNum = buyNumInfo.nBoughtNum;
            }

            int nIndex = 0;
            for (; nIndex < limitConfig.limitStepList.Count; ++nIndex)
            {
                int nLimitNum = limitConfig.limitStepList[nIndex].nLimitNum;
                if (nLimitNum == 0)
                    return 0;

                if (nLimitNum < 0 || nBoughtNum < nLimitNum)
                    break;
            }

            if (nIndex >= limitConfig.limitStepList.Count)
                return 0;

            long lRealPrice = 0;

            int nHasBuyNum = nBoughtNum;    // 已经购买数量
            int nToBuyNum = nBuyNum;        // 本次将要购买数量

            int nPriceTotal = 0;
            for (nIndex = 0; nIndex < limitConfig.limitStepList.Count; ++nIndex)
            {
                SPointShopNumLimitPriceSub node = limitConfig.limitStepList[nIndex];
                if (nHasBuyNum > node.nLimitNum)
                    continue;

                int nSinglePrice = ((nResType == 0) ? node.nLimitPointPrice : node.nLimitMoneyPrice);
                if (nSinglePrice <= 0)
                    return 0;

                int nDiffNum = node.nLimitNum - nHasBuyNum;
                // 当次可购买数量,如果不够将下次循环中继续扣除本次的数量
                int nCanBuyNum = (nDiffNum >= nToBuyNum ? nToBuyNum : nDiffNum);

                nPriceTotal += nSinglePrice * nCanBuyNum;

                nHasBuyNum += nCanBuyNum;
                nToBuyNum -= nCanBuyNum;
                if (nToBuyNum <= 0)
                {
                    break;
                }
            }
            lRealPrice = nPriceTotal;

            if (lRealPrice > int.MaxValue)
                return 0;

            return (int)lRealPrice;
        }

        public int CalcNumLimitBuyMax(int nLimitID, int nSellID, int nResType, int nCurRes)
        {
            SPointShopNumLimitPrice limitConfig = PointShopConfig.Instance.GetShopNumLimitPriceConfig(nLimitID);
            if (limitConfig == null)
                return 0;

            int nBoughtNum = 0;
            cmd_entity_update_pointshop_buynum_info buyNumInfo = new cmd_entity_update_pointshop_buynum_info();
            if (BuyNumInfo.TryGetValue(nSellID, out buyNumInfo))
            {
                nBoughtNum = buyNumInfo.nBoughtNum;
            }

            int nIndex = 0;
            for (; nIndex < limitConfig.limitStepList.Count; ++nIndex)
            {
                int nLimitNum = limitConfig.limitStepList[nIndex].nLimitNum;
                if (nLimitNum == 0)
                    return 0;

                if (nLimitNum < 0 || nBoughtNum < nLimitNum)
                    break;
            }

            if (nIndex >= limitConfig.limitStepList.Count)
                return 0;

            int nResult = 0;
            while (nCurRes > 0)
            {
                if (nIndex >= limitConfig.limitStepList.Count)
                    break;

                SPointShopNumLimitPriceSub config = limitConfig.limitStepList[nIndex];
                int nSinglePrice = 0;
                if (nResType == 0)
                    nSinglePrice = config.nLimitPointPrice;
                else
                    nSinglePrice = config.nLimitMoneyPrice;

                if (nSinglePrice <= 0)
                    break;

                int nCanBuyNum = config.nLimitNum - nBoughtNum;
                int nStepPrice = nSinglePrice * nCanBuyNum;
                if (nStepPrice >= nCurRes)
                {
                    nResult += nCurRes / nSinglePrice;
                    nCurRes = 0;
                }
                else
                {
                    nResult += nCanBuyNum;
                    nBoughtNum += nCanBuyNum;
                    nCurRes -= nStepPrice;
                }

                ++nIndex;
            }

            return nResult;
        }

        // 限制购买的最大数量：（1）当前最大输入3位数，（2）不同种类有不同数量限制，未限制的默认为999
        public int CalcMaxNumByPriceType(int nType)
        {
            int nCount = 0;
            switch (nType)
            {
                case (int)EMActorPrizeType.ACTOR_PRIZE_GEMSTONE:
                    {
                        nCount = (int)PLAYER_CROWN_DEF.CROWN_GEMSTONE_MAX_COUNT - LogicDataCenter.crownPageDataManager.CrownGemtoneDic.Count;
                        // 策划要求加，5没有任何意义
                        nCount = Mathf.Min(nCount, 5);
                    }
                    break;
                case (int)EMActorPrizeType.ACTOR_PRIZE_GEMSTONEPAGE:
                    {
                        nCount = (int)PLAYER_CROWN_DEF.CROWN_PAGE_MAX_COUNT - LogicDataCenter.crownPageDataManager.CrownPageDic.Count;
                    }
                    break;
                case (int)EMActorPrizeType.ACTOR_PRIZE_HERO:
                case (int)EMActorPrizeType.ACTOR_PRIZE_HEROXPSKILL:
                case (int)EMActorPrizeType.ACTOR_PRIZE_TALENT:
                case (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN:
                case (int)EMActorPrizeType.ACTOR_PRIZE_EMOJI:
                    {
                        nCount = 1;
                    }
                    break;
                default:
                    {
                        nCount = 999;
                    }
                    break;
            }
            return nCount;
        }

        public void FillGoodsPlusIcon(SSchemePointShopGoodsConfig config, float fScale, RectTransform container1, RectTransform container2)
        {
            ClearContainer(container1);
            ClearContainer(container2);

            if (config == null || config.nGoodsLabel == null)
                return;

            for (int i = 0; i < config.nGoodsLabel.Length; ++i)
            {
                int nType = config.nGoodsLabel[i];
                if (nType == 0)
                    continue;

                GameObject newObj = new GameObject("GoodsPlusIcon" + i);
                newObj.layer = LayerMask.NameToLayer(Config.LayerUI);
                Image iconImage = newObj.AddComponent<Image>();
                iconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, "GoodsPlusIcon_" + nType, WndID.WND_ID_POINTSHOP);
                iconImage.SetNativeSize();
                if (fScale != 1f)
                {
                    RectTransform newTrans = newObj.transform as RectTransform;
                    Vector2 curSize = newTrans.sizeDelta;
                    curSize *= fScale;
                    LayoutElement layoutEle = newObj.AddComponent<LayoutElement>();
                    layoutEle.preferredWidth = curSize.x;
                    layoutEle.preferredHeight = curSize.y;
                }

                if (nType <= 10)
                {
                    newObj.transform.SetParent(container1, false);
                }
                else
                {
                    newObj.transform.SetParent(container2, false);
                }
            }
        }

        void ClearContainer(RectTransform container)
        {
            for (int i = container.childCount - 1; i >= 0; --i)
            {
                var temp = container.GetChild(i).gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)ETIMER.ETIMER_REFLUSHLEFTTIME:
                    {
                        --m_leftReflushTime;
                        m_leftReflushTime = m_leftReflushTime <= 0 ? 0 : m_leftReflushTime;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
