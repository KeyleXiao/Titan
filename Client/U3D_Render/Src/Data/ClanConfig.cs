using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.ClanConfig
{
    // 与ClanConfig.csv表里面的列对应
    public enum ClanConfigRow : int
    {
        EMT_ROW_CLAN_DISMISSING_LIFE = 2,
        EMT_ROW_CLAN_TIP = 8,               // 帮会说明
        EMT_ROW_MAX,                        // 最大
    }

    public enum EClanLevelCol
    {
        CLAN_LEVEL_COL_LEVEL = 0,
        CLAN_LEVEL_COL_MAX_KIN_COUNT = 1,
        CLAN_LEVEL_COL_MAX_PERSON_COUNT = 4,
    }

    /// <summary>
    /// ClanConfig配置类
    /// </summary>
    public class SSchemeClanConfig
    {
        public string sClanTip;         //  帮会详细说明UBB格式
        public int nDismissingLife;     // 解散帮会的生存周期
    }

    // 帮会每级的升级信息
    public class SSchemeClanLevel
    {
        public int nMaxPersonCount;                        // 人数上限
        public int nMaxKinCount;                           // 战队上限
    };

    class ClanConfig
    {
        public SSchemeClanConfig tClanConfig;
        public Dictionary<int, SSchemeClanLevel> tClanLevelMap;
        private static ClanConfig singleton = new ClanConfig();

        public static ClanConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new ClanConfig();
            }
        }

        public void Load(string stPath)
        {
            InitClanConfigCsv(stPath + "ClanConfig.csv");
            LoadClanLevelCsv(stPath + "ClanLevel.csv");
        }

        public void UnLoad()
        {
            if (tClanConfig != null)
            {
                tClanConfig = null;
            }

            if (tClanLevelMap != null)
            {
                tClanLevelMap.Clear();
                tClanLevelMap = null;
            }

            singleton = null;
        }

        // 初始化csv表
        protected void InitClanConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            tClanConfig = new SSchemeClanConfig();

            for (int i = 0; i < (int)ClanConfigRow.EMT_ROW_MAX; i++)
            {
                switch (i)
                {
                    case (int)ClanConfigRow.EMT_ROW_CLAN_DISMISSING_LIFE:
                        {
                            tClanConfig.nDismissingLife = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)ClanConfigRow.EMT_ROW_CLAN_TIP:
                        {
                            tClanConfig.sClanTip = reader.GetString(i, 1, string.Empty);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        private void LoadClanLevelCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            tClanLevelMap = new Dictionary<int, SSchemeClanLevel>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeClanLevel clanLevel = new SSchemeClanLevel();
                int level = reader.GetInt(i, (int)EClanLevelCol.CLAN_LEVEL_COL_LEVEL, 0);
                clanLevel.nMaxKinCount = reader.GetInt(i, (int)EClanLevelCol.CLAN_LEVEL_COL_MAX_KIN_COUNT, 0);
                clanLevel.nMaxPersonCount = reader.GetInt(i, (int)EClanLevelCol.CLAN_LEVEL_COL_MAX_PERSON_COUNT, 0);
                tClanLevelMap[level] = clanLevel;
            }
        }
    }
}
