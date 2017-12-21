using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using Data.ActorPrizeConfig;
using War;

namespace Data.PointShopConfig
{
    // 商品类型
    public enum PointShopGoodsType
    {
        Goods_Hero = 1,
        Goods_Skin,
        Goods_Wing,
        Goods_Card,
        Goods_Rune,
        Goods_Chest,
        Goods_Special,
        Goods_BenefitCard,
        Goods_Xp,
        Goods_GemStone,
        Goods_Other,

        Goods_Max
    }

    // 商城消费途径
    public enum PointShopConsumpApproach
    {
        Approach_Invalid,
        Approach_Actor_Rename,				// 玩家改名
        Approach_Gemstone_Appraisal,		// 宝石抛光
        Approach_MysteryShop_Reflush,		// 神秘商店刷新
        Approach_TaskOpt_Reflush,           // 任务操作刷新

        Approach_Max
    };

    enum EMSchemePointShopGoodsCol
    {
        GOODS_SELLID = 0,
        GOODS_NAME,
        GOODS_ISVALID,
        GOODS_SHOWINSHOP,
        GOODS_TYPE,
        GOODS_PRIZEID,
        GOODS_LABEL,
        GOODS_POSITIONWEIGHT,
        GOODS_VIPLVLIMIT,
        GOODS_ACTORLVLIMIT,
        GOODS_VALIDBEGINTIME,
        GOODS_VALIDENDTIME,
        GOODS_NUMPRICEID,
        GOODS_POINTPRICE,
        GOODS_MONEYPRICE,
        GOODS_MONEYUSETYPE,
        GOODS_DISCOUNTRATE,
        GOODS_DISCOUNTBEGINTIME,
        GOODS_DISCOUNTENDTIME,
        GOODS_LIMITNUM,
        GOODS_LIMITTYPE,
        GOODS_LIMITFLUSHTIME,
        GOODS_PLUSDESC,
        GOODS_TOOLTIP,
        GOODS_OPENSERVER,
        GOODS_CONSUMPAPPROACH,

        GOODS_MAX
    };

    public class SSchemePointShopGoodsConfig
    {
        public int nSellID;             // 出售ID
        public String strGoodsName;     // 名称
        public bool bShowInShop;        // 是否在商城显示
        public int nGoodsType;          // 物品类型
        public int nPrizeID;            // 奖励ID
        public int[] nGoodsLabel;         // 商品标签
        public int nPositionWeight;     // 位置权重
        public int nNeedVipLevel;       // 需要VIP等级
        public int nNeedActorLvMin;     // 最低角色等级
        public int nNeedActorLvMax;     // 最高角色等级
        public int nBeginTime;          // 开始时间
        public int nEndTime;            // 结束时间
        public int nNumLimitID;         // 动态限制价格ID
        public int nPointPrice;         // 点券价格
        public int nMoneyPrice;         // 金币价格
        public int nMoneyUseType;       // 金币使用类型
        public int nDiscountRate;       // 折扣率
        public int nDiscountBeginTime;  // 折扣开始时间
        public int nDiscountEndTime;    // 折扣结束时间
        public String strPlusDesc;      // 附加描述
        public String strTooltip;       // 附加描述
        public int nConsumpApproach;    // 消费途径
    }

    public class SSchemePointShopADConfig
    {
        public int nID;                 // 广告ID
        public String strTitle;         // 标题
        public String strDesc;          // 描述
        public int nLocType;            // 位置
        public List<int> nSellIDList;   // 出售ID
        public String strPicName;       // 广告图名称
    }

    /// <summary>
    /// 增益卡信息脚本
    /// </summary>
    public class SSchemePointShopBenefitCardConfig
    {
        public String strDesc;                  // 描述
        public int nBenefitCardType;			// 1为金币增益卡；2为角色经验增益卡；3为英雄经验增益卡
        public int nBenefitCardSubType;		    // 1为时间卡，2为次数卡
        public int nBenefitValue;				// 如果是时间卡，该参数为天数；如果是次数卡，该参数为次数
        public int nMultiple;					// 增益倍数
        public int nShopPriceID;                // 商城价格ID
    }

    public class SPointShopNumLimitPriceSub
    {
        public int nLimitNum;
        public int nLimitPointPrice;
        public int nLimitMoneyPrice;
    }

    public class SPointShopNumLimitPrice
    {
        public int nLimitPriceID;
        public List<SPointShopNumLimitPriceSub> limitStepList;
    }

    class PointShopConfig
    {
        private static PointShopConfig m_singleton;
        public static PointShopConfig Instance
        {
            get
            {
                if (m_singleton == null)
                    m_singleton = new PointShopConfig();

                return m_singleton;
            }
        }

        private Dictionary<int, SSchemePointShopGoodsConfig> m_goodsConfig = new Dictionary<int, SSchemePointShopGoodsConfig>();
        private List<SSchemePointShopGoodsConfig>[] m_goodsTypeList;

        private List<SSchemePointShopADConfig> m_adConfig;

        private Dictionary<int, SPointShopNumLimitPrice> m_numLimitPriceConfig;

        // 增益卡
        private Dictionary<int, SSchemePointShopBenefitCardConfig> m_BenefitCardConfig;
        

        public void Load(string stPath)
        {
            LoadPointShopGoodsConfig(stPath + "PointShopGoods.csv");
            LoadPointShopADPageConfig(stPath + "PointShopADPage.csv");
            LoadPointShopLimitPriceConfig(stPath + "ShopNumLimitPrice.csv");
            LoadPointShopBenefitCardConfig(stPath + "BenefitCard.csv");
        }

        // 载入商城物品脚本
        private void LoadPointShopGoodsConfig(string stPath)
        {
            List<SSchemePointShopGoodsConfig> tempGoodsConfigList = new List<SSchemePointShopGoodsConfig>();
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIsValid = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_ISVALID, 0);
                if (nIsValid <= 0)
                    continue;
                // TODO 判断是否在本区开放

                SSchemePointShopGoodsConfig sConfig = new SSchemePointShopGoodsConfig();
                sConfig.nSellID = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_SELLID, 0);
                sConfig.strGoodsName = reader.GetString(i, (int)EMSchemePointShopGoodsCol.GOODS_NAME, "");
                sConfig.bShowInShop = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_SHOWINSHOP, 0) > 0;
                sConfig.nGoodsType = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_TYPE, 0);
                sConfig.nPrizeID = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_PRIZEID, 0);
                string[] strGoodsLabel = reader.GetString(i, (int)EMSchemePointShopGoodsCol.GOODS_LABEL, "").Split(';');
                if (strGoodsLabel != null && strGoodsLabel.Length > 0 && strGoodsLabel[0].Length > 0)
                {
                    sConfig.nGoodsLabel = new int[strGoodsLabel.Length];
                    for (int j = 0; j < strGoodsLabel.Length; ++j)
                    {
                        sConfig.nGoodsLabel[j] = Convert.ToInt32(strGoodsLabel[j]);
                    }
                }
                sConfig.nPositionWeight = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_POSITIONWEIGHT, 0);
                sConfig.nNeedVipLevel = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_VIPLVLIMIT, 0);
                string[] strLvLimit = reader.GetString(i, (int)EMSchemePointShopGoodsCol.GOODS_ACTORLVLIMIT, "").Split(';');
                if (strLvLimit != null && strLvLimit.Length > 0 && strLvLimit[0].Length > 0)
                {
                    sConfig.nNeedActorLvMin = Convert.ToInt32(strLvLimit[0]);
                    if (strLvLimit.Length > 1)
                        sConfig.nNeedActorLvMax = Convert.ToInt32(strLvLimit[1]);
                    else
                        sConfig.nNeedActorLvMax = 999;
                }
                else
                {
                    sConfig.nNeedActorLvMin = 0;
                    sConfig.nNeedActorLvMax = 999;
                }
                // TODO 时间格式还没定
                sConfig.nBeginTime = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_VALIDBEGINTIME, 0);
                sConfig.nEndTime = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_VALIDENDTIME, 0);
                sConfig.nNumLimitID = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_NUMPRICEID, 0);
                sConfig.nPointPrice = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_POINTPRICE, 0);
                sConfig.nMoneyPrice = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_MONEYPRICE, 0);
                sConfig.nMoneyUseType = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_MONEYUSETYPE, 0);
                sConfig.nDiscountRate = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_DISCOUNTRATE, 0);
                // TODO 时间格式还没定
                sConfig.nDiscountBeginTime = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_DISCOUNTBEGINTIME, 0);
                sConfig.nDiscountEndTime = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_DISCOUNTENDTIME, 0);
                sConfig.strPlusDesc = reader.GetString(i, (int)EMSchemePointShopGoodsCol.GOODS_PLUSDESC, "");
                sConfig.strTooltip = reader.GetString(i, (int)EMSchemePointShopGoodsCol.GOODS_TOOLTIP, "");
                sConfig.nConsumpApproach = reader.GetInt(i, (int)EMSchemePointShopGoodsCol.GOODS_CONSUMPAPPROACH, 0);
                m_goodsConfig[sConfig.nSellID] = sConfig;
                tempGoodsConfigList.Add(sConfig);
            }

            m_goodsTypeList = new List<SSchemePointShopGoodsConfig>[(int)PointShopGoodsType.Goods_Max];
            for (int i = 1; i < (int)PointShopGoodsType.Goods_Max; ++i)
            {
                m_goodsTypeList[i] = new List<SSchemePointShopGoodsConfig>();
            }

            foreach (var config in tempGoodsConfigList)
            {
                if (config.nGoodsType > 0 && config.nGoodsType < (int)PointShopGoodsType.Goods_Max)
                {
                    m_goodsTypeList[config.nGoodsType].Add(config);
                }
                else
                {
                    Debug.LogError("PointShopGoods.csv 物品类型不合法 出售ID=" + config.nSellID);
                }
            }

            for (int i = 1; i < (int)PointShopGoodsType.Goods_Max; ++i)
            {
                InsertionSort(m_goodsTypeList[i], (x, y) => { if (x.nPositionWeight == y.nPositionWeight) return 0; else return (x.nPositionWeight > y.nPositionWeight) ? -1 : 1; });
            }
        }

        // 载入商城广告页脚本
        private void LoadPointShopADPageConfig(string stPath)
        {
            m_adConfig = new List<SSchemePointShopADConfig>();

            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemePointShopADConfig config = new SSchemePointShopADConfig();
                int nIndex = 0;
                config.nID = reader.GetInt(i, nIndex++, 0);
                config.strTitle = reader.GetString(i, nIndex++, "");
                config.strDesc = reader.GetString(i, nIndex++, "");
                config.nLocType = reader.GetInt(i, nIndex++, 0);
                String strSellID = reader.GetString(i, nIndex++, "");
                string[] sellIDList = strSellID.Split(';');
                if (sellIDList != null)
                {
                    config.nSellIDList = new List<int>();
                    for (int j = 0; j < sellIDList.Length; ++j)
                    {
                        int nSellID = Convert.ToInt32(sellIDList[j]);
                        config.nSellIDList.Add(nSellID);
                    }
                }

                config.strPicName = reader.GetString(i, nIndex++, "");

                m_adConfig.Add(config);
            }
        }

        // 载入商城随数量变动价格脚本
        private void LoadPointShopLimitPriceConfig(string stPath)
        {
            m_numLimitPriceConfig = new Dictionary<int, SPointShopNumLimitPrice>();

            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIndex = 0;
                int nLimitID = reader.GetInt(i, nIndex++, 0);

                SPointShopNumLimitPriceSub subInfo = new SPointShopNumLimitPriceSub();
                subInfo.nLimitNum = reader.GetInt(i, nIndex++, 0);
                subInfo.nLimitPointPrice = reader.GetInt(i, nIndex++, 0);
                subInfo.nLimitMoneyPrice = reader.GetInt(i, nIndex++, 0);

                if(subInfo.nLimitNum <= 0)
                {
                    Trace.LogError("Shop limit num is invalid digit!! num=" + subInfo.nLimitNum.ToString() + ", row=" + (i + 1).ToString());
                    return;
                }
                if(subInfo.nLimitPointPrice<=0 && subInfo.nLimitMoneyPrice <= 0)
                {
                    Trace.LogError("Shop limit price is invalid digit!! ticket="+ subInfo.nLimitPointPrice.ToString()+ ", Money="+ subInfo.nLimitMoneyPrice.ToString()+ ", row="+ (i + 1).ToString() );
                    return;
                }

                SPointShopNumLimitPrice limitPrice = null;
                m_numLimitPriceConfig.TryGetValue(nLimitID, out limitPrice);
                if (limitPrice != null)
                {
                    limitPrice.limitStepList.Add(subInfo);
                }
                else
                {
                    limitPrice = new SPointShopNumLimitPrice();
                    limitPrice.nLimitPriceID = nLimitID;
                    limitPrice.limitStepList = new List<SPointShopNumLimitPriceSub>();
                    limitPrice.limitStepList.Add(subInfo);

                    m_numLimitPriceConfig.Add(nLimitID, limitPrice);
                }
            }
        }

        /// <summary>
        /// 载入增益卡脚本
        /// </summary>
        public void LoadPointShopBenefitCardConfig(string stPath)
        {
            m_BenefitCardConfig = new Dictionary<int, SSchemePointShopBenefitCardConfig>();

            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIndex = 0;
                int cardIndex = reader.GetInt(i, nIndex++, 0);

                SSchemePointShopBenefitCardConfig config = new SSchemePointShopBenefitCardConfig();
                config.strDesc = reader.GetString(i, nIndex++, "");
                config.nBenefitCardType = reader.GetInt(i, nIndex++, 0);
                config.nBenefitCardSubType = reader.GetInt(i, nIndex++, 0);
                config.nBenefitValue = reader.GetInt(i, nIndex++, 0);
                config.nMultiple = reader.GetInt(i, nIndex++, 0);
                config.nShopPriceID = reader.GetInt(i, nIndex++, 0);

                m_BenefitCardConfig.Add(config.nShopPriceID, config);
            }
        }
        
        public SSchemePointShopBenefitCardConfig GetBenefitCardConfig(int nSellID)
        {
            SSchemePointShopBenefitCardConfig config;
            if (m_BenefitCardConfig.TryGetValue(nSellID, out config))
            {
                return config;
            }
            return null;
        }

        public List<SSchemePointShopADConfig> GetADConfig()
        {
            return m_adConfig;
        }

        public SSchemePointShopGoodsConfig GetPointShopGoodsConfig(int nSellID)
        {
            SSchemePointShopGoodsConfig result;
            if (m_goodsConfig.TryGetValue(nSellID, out result))
            {
                return result;
            }

            return null;
        }

        public int GetPointShopSellIDByHeroID(int nHeroID)
        {
            if (m_goodsTypeList == null || m_goodsTypeList[(int)PointShopGoodsType.Goods_Hero] == null)
                return 0;

            List<SSchemePointShopGoodsConfig> heroSellList = m_goodsTypeList[(int)PointShopGoodsType.Goods_Hero];
            foreach (var config in heroSellList)
            {
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
                if (prizeConfig == null)
                    continue;

                if (prizeConfig.nPrizeType != (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
                    continue;

                if (prizeConfig.nPrizeParam[0] == nHeroID)
                    return config.nSellID;
            }

            return 0;
        }

        public int GetPointShopSellIDBySkinID(int nHeroID, int nSkinID)
        {
            if (m_goodsTypeList == null || m_goodsTypeList[(int)PointShopGoodsType.Goods_Skin] == null)
                return 0;

            List<SSchemePointShopGoodsConfig> skinSellList = m_goodsTypeList[(int)PointShopGoodsType.Goods_Skin];
            foreach (var config in skinSellList)
            {
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
                if (prizeConfig == null)
                    continue;

                if (prizeConfig.nPrizeType != (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
                    continue;

                if (prizeConfig.nPrizeParam[0] == nHeroID && prizeConfig.nPrizeParam[1] == nSkinID)
                    return config.nSellID;
            }

            return 0;
        }

        public SPointShopNumLimitPrice GetShopNumLimitPriceConfig(int nLimitID)
        {
            SPointShopNumLimitPrice result;
            if (m_numLimitPriceConfig.TryGetValue(nLimitID, out result))
            {
                return result;
            }

            return null;
        }

        public List<SSchemePointShopGoodsConfig> GetGoodsListByType(PointShopGoodsType goodsType)
        {
            return m_goodsTypeList[(int)goodsType];
        }

        // 网上随便找的排序函数。c#自带的函数里没找到能稳定排序的
        public static void InsertionSort<T>(IList<T> list, Comparison<T> comparison, bool bReverse = false)
        {
            if (list == null || comparison == null)
                return;

            int count = list.Count;
            for (int j = 1; j < count; j++)
            {
                T key = list[j];

                int i = j - 1;
                if (!bReverse)
                {
                    for (; i >= 0 && comparison(list[i], key) > 0; i--)
                    {
                        list[i + 1] = list[i];
                    }
                }
                else
                {
                    for (; i >= 0 && comparison(list[i], key) < 0; i--)
                    {
                        list[i + 1] = list[i];
                    }
                }
                list[i + 1] = key;
            }
        }
    }
}
