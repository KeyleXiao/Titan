using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.RecommendPrize
{
    // 与 RecommendPrize.csv表里面的列对应
    /// <summary>
    /// RecommendPrize 配置类
    /// </summary>
    public class SSchemeRecommendPrize
    {
        public int targetID;             // 目标ID
        public int totalNumGames;         // 总局数量
        public List<int> prizeList;       // 奖励ID列表
    }

    public enum ECOLUMNType : int
    {
        TARGET_ID,
        TOTAL_NUMBER_OF_GAMES,
        PRIZE_ID,
    }

    class RecommendPrizeConfig : Singleton<RecommendPrizeConfig>
    {
        private List<SSchemeRecommendPrize> m_listConfig = new List<SSchemeRecommendPrize>();
        public List<SSchemeRecommendPrize> ConfigList
        {
            get
            {
                return m_listConfig;
            }
        }

        public void Load(string stPath)
        {
            InitRecommendPrizeCsv(stPath + "RecommendPrize.csv");
        }

        public void UnLoad()
        {
            m_listConfig.Clear();
        }
        
        private void InitRecommendPrizeCsv(string stPath)
        {
            char[] charSeparator = new char[] { ';' };
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); ++i)
            {
                SSchemeRecommendPrize config = new SSchemeRecommendPrize();
                config.targetID = reader.GetInt(i, (int)ECOLUMNType.TARGET_ID, 0);
                config.totalNumGames = reader.GetInt(i, (int)ECOLUMNType.TOTAL_NUMBER_OF_GAMES, 0);
                string szPrizes = reader.GetString(i, (int)ECOLUMNType.PRIZE_ID, "");
                config.prizeList = ScpReader.ConvertStringToIntArray(szPrizes, charSeparator);
                m_listConfig.Add(config);
            }
        }
    }
}
