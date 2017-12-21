using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.WarStatisticMap
{
    // 与WarStatisticMap.csv表里面的列对应
    public enum WarStatisticMapConfigRow
    {
        EMT_ROW_MATCHTYPE_ID,   // 比赛类型ID
        EMT_ROW_MATCHTYPE_NAME, // 玩法名称
        EMT_ROW_MATCHTYPE_HAVERANK,   // 是否统计排位
        EMT_ROW_MAX,            // 最大
    }

    public class SSchemeWarStatisticMapConfig
    {
        public String strName;      // 玩法名称
        public int  nMatchTypeID;  // 对应的比赛类型ID
        public bool isHaveRank;     // 是否统计排位
    }

    class WarStatisticMapConfig
    {
        private Dictionary<int, SSchemeWarStatisticMapConfig> m_mapConfig;
        public Dictionary<int, SSchemeWarStatisticMapConfig> MapConfig { get { return m_mapConfig; } }

        private static WarStatisticMapConfig singleton;
        public static WarStatisticMapConfig Instance
        {
            get
            {
                if (singleton == null)
                    singleton = new WarStatisticMapConfig();

                return singleton;
            }
        }

        public void Load(string stPath)
        {
            InitConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (m_mapConfig != null)
                m_mapConfig = null;
            singleton = null;
        }

        // 初始化csv表
        protected void InitConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            m_mapConfig = new Dictionary<int, SSchemeWarStatisticMapConfig>();

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeWarStatisticMapConfig sConfig = new SSchemeWarStatisticMapConfig();
                int nMatchTypeID = reader.GetInt(i, (int)WarStatisticMapConfigRow.EMT_ROW_MATCHTYPE_ID, 0);
                sConfig.nMatchTypeID = nMatchTypeID;
                sConfig.strName = reader.GetString(i, (int)WarStatisticMapConfigRow.EMT_ROW_MATCHTYPE_NAME, "");
                sConfig.isHaveRank = (reader.GetInt(i, (int)WarStatisticMapConfigRow.EMT_ROW_MATCHTYPE_HAVERANK, 0) > 0);

                m_mapConfig.Add(nMatchTypeID, sConfig);
            }
        }

        public SSchemeWarStatisticMapConfig getWarStatMapConfig(int nMatchType)
        {
            if(!m_mapConfig.ContainsKey(nMatchType))
            {
                return null;
            }

            return m_mapConfig[nMatchType];
        }
    }
}
