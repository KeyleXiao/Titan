using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.HeroTypeConfig
{
    // HeroType.csv
    public enum EHeroTypeConfig
    {
        EHTC_COL_HERO_ID,       // 英雄ID
        EHTC_COL_HERO_TYPE,     // 英雄类型
        EHTC_COL_OPER_DIFFICULTY,// 操作难度（1简单2中等3困难）
        EHTC_COL_HERO_DESC,     // 英雄描述
        EHTC_COL_HERO_LOCATION, // 英雄定位
        EHTC_COL_HERO_NAME,     // 备注英雄名字方便策划配表
        EHTC_COL_SELECT_DESC,   // 大厅选人描述
        EHTC_COL_SORT_INDEX,    // 选人排序优先级（默认为0最低1最高2其次依次往下）
        EHTC_COL_AGAINST_WAY,   // 针对方式
        EHTC_COL_PLAY_WAY,      // 玩法介绍
    }

    public class SSchemeHeroTypeConfig
    {
        public int nHeroID;
        public string strHeroType;
        public int nOperDifficulty;
        public string strHeroDesc;
        public string strHeroLocation;
        public string strHeroName;
        public string strSelectDesc;
        public int nSortIndex;
        public string strAgainstWay;
        public string strPlayWay;
    }

    class HeroTypeConfig
    {
        private static HeroTypeConfig singleton = new HeroTypeConfig();
        public static HeroTypeConfig Instance
        {
            get { return singleton; }
        }

        private Dictionary<int, SSchemeHeroTypeConfig> m_heroTypeConfig;

        public void Load(string stPath)
        {
            LoadHeroTypeConfig(stPath);
        }

        public void Unload()
        {
            m_heroTypeConfig.Clear();
        }

        private void LoadHeroTypeConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            m_heroTypeConfig = new Dictionary<int, SSchemeHeroTypeConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeHeroTypeConfig config = new SSchemeHeroTypeConfig();
                config.nHeroID = reader.GetInt(i, (int)EHeroTypeConfig.EHTC_COL_HERO_ID, 0);
                config.strHeroType = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_HERO_TYPE, "");
                config.nOperDifficulty = reader.GetInt(i, (int)EHeroTypeConfig.EHTC_COL_OPER_DIFFICULTY, 0);
                config.strHeroDesc = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_HERO_DESC, "");
                config.strHeroLocation = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_HERO_LOCATION, "");
                config.strHeroName = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_HERO_NAME, "");
                config.strSelectDesc = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_SELECT_DESC, "");
                config.nSortIndex = reader.GetInt(i, (int)EHeroTypeConfig.EHTC_COL_SORT_INDEX, 0);
                config.strAgainstWay = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_AGAINST_WAY, "");
                config.strPlayWay = reader.GetString(i, (int)EHeroTypeConfig.EHTC_COL_PLAY_WAY, "");

                m_heroTypeConfig[config.nHeroID] = config;
            }
        }

        public SSchemeHeroTypeConfig GetHeroTypeConfig(int nHeroID)
        {
            SSchemeHeroTypeConfig config;
            if (m_heroTypeConfig.TryGetValue(nHeroID, out config))
            {
                return config;
            }

            return null;
        }

    }
}
