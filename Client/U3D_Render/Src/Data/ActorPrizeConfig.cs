using System;
using System.Collections.Generic;
using System.Text;

namespace Data.ActorPrizeConfig
{
    public class SSchemeActorPrizeConfig
    {
        public int nPrizeID;            // 奖励ID
        public string strPrizeName;     // 名称
        public int nPrizeIcon;          // 图标
        public int nPrizeGrade;         // 品质
        public string strPrizeDesc;     // 描述
        public int nBroadcastFlag;      // 广播标志
        public int nPrizeType;          // 类型
        public int nCakeTrailEffectID;        //圆饼拖尾光效ID
        public int nPedestalEffectID;           //底座光效ID
        public int nPedestalGlowEffectID;    //底座外发光光效ID
        public int[] nPrizeParam;       // 参数
    }

    public class SSchemePrizeChestConfig
    {
        public int nChestId;            // 宝箱ID
        public int nChestFlag;          // 宝箱标志位
        public string strChestName;     // 宝箱名称
        public int nShowPrizeID;        // 对应奖励ID
        public int nChestIcon;          // 宝箱图标
        public string strChestDesc;     // 宝箱描述
        public int nCanBuy;             // 是否能购买
        public int nAlwaysShow;         // 是否一直显示
        public int nChestModelID;       // 模型ID
        public string strNormalAction;  // 常态动作
        public int nCamEffectID;      //镜头光效
        public int nNormalEffect;       // 常态光效
        public string strOpenAction;    // 打开动作
        public int nOpenEffect;         // 打开光效
        public string strShowAction;    // 展示动作
        public int nShowEffect;         // 展示光效
        public int nNotHaveEffect;      // 未持有光效
        public int nPrizeGroupID;       // 奖励组ID
        public int nBgEffectID;          //宝箱背景光效
    }

    class ActorPrizeConfig
    {
        private static ActorPrizeConfig m_singleton;
        public static ActorPrizeConfig Instance
        {
            get
            {
                if (m_singleton == null)
                    m_singleton = new ActorPrizeConfig();

                return m_singleton;
            }
        }

        private Dictionary<int, SSchemeActorPrizeConfig> m_actorPrizeConfig = new Dictionary<int, SSchemeActorPrizeConfig>();
        private Dictionary<int, SSchemePrizeChestConfig> m_prizeChestConfig = new Dictionary<int, SSchemePrizeChestConfig>();
        public Dictionary<int, SSchemePrizeChestConfig> PrizeChestConfig { get { return m_prizeChestConfig; } }

        public void Load(string stPath)
        {
            LoadActorPrizeConfig(stPath + "ActorPrize.csv");
            LoadPrizeChestConfig(stPath + "PrizeChestInfo.csv");
        }

        // 载入奖励脚本
        private void LoadActorPrizeConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIndex = 0;
                SSchemeActorPrizeConfig sConfig = new SSchemeActorPrizeConfig();
                sConfig.nPrizeID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strPrizeName = reader.GetString(i, (int)nIndex++, "");
                sConfig.nPrizeIcon = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPrizeGrade = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strPrizeDesc = reader.GetString(i, (int)nIndex++, "");
                sConfig.nBroadcastFlag = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPrizeType = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nCakeTrailEffectID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPedestalEffectID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPedestalGlowEffectID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPrizeParam = new int[5];
                for (int j = 0; j < 5; ++j)
                {
                    sConfig.nPrizeParam[j] = reader.GetInt(i, (int)nIndex++, 0);
                }

                m_actorPrizeConfig[sConfig.nPrizeID] = sConfig;
            }
        }

        // 载入宝箱脚本
        private void LoadPrizeChestConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIndex = 0;
                SSchemePrizeChestConfig sConfig = new SSchemePrizeChestConfig();
                sConfig.nChestId = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nChestFlag = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strChestName = reader.GetString(i, (int)nIndex++, "");
                sConfig.nShowPrizeID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nChestIcon = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strChestDesc = reader.GetString(i, (int)nIndex++, "");
                sConfig.nCanBuy = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nAlwaysShow = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nChestModelID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strNormalAction = reader.GetString(i, (int)nIndex++, "");
                sConfig.strOpenAction = reader.GetString(i, (int)nIndex++, "");
                sConfig.strShowAction = reader.GetString(i, (int)nIndex++, "");
                sConfig.nCamEffectID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nNormalEffect = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nOpenEffect = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nShowEffect = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nNotHaveEffect = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nPrizeGroupID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nBgEffectID = reader.GetInt(i, (int)nIndex++, 0);

                m_prizeChestConfig[sConfig.nChestId] = sConfig;
            }
        }

        public SSchemeActorPrizeConfig GetActorPrizeConfig(int nPrizeID)
        {
            SSchemeActorPrizeConfig result;
            if (m_actorPrizeConfig.TryGetValue(nPrizeID, out result))
            {
                return result;
            }

            return null;
        }

        public SSchemePrizeChestConfig GetPrizeChestConfig(int nChestID)
        {
            SSchemePrizeChestConfig result;
            if (m_prizeChestConfig.TryGetValue(nChestID, out result))
            {
                return result;
            }

            return null;
        }
    }
}
