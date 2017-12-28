using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.MatchSeasonPrizeConfig
{
    public class SSchemeSeasonPrizeConfig
    {
        public int nPrizeConfigID;
        public string szSeasonDesc;
        public int nRankGrade;
        public string szGradeDesc;
        public int[] arrNormalPrize;
        public int[] arrExtraCondition;
        public int[] arrExtraPrize;
        public int nAccountScore;
        public string szNormalCondition;
        public string szExtralCondition;
    }

    public class MatchSeasonPrizeConfig
    {
        private static MatchSeasonPrizeConfig m_singleton = new MatchSeasonPrizeConfig();
        public static MatchSeasonPrizeConfig Instance { get { return m_singleton; } }

        private Dictionary<int, Dictionary<int, SSchemeSeasonPrizeConfig>> m_mapSeasonPrizeConfig;
        public Dictionary<int, Dictionary<int, SSchemeSeasonPrizeConfig>> MapSeasonPrizeConfig { get { return m_mapSeasonPrizeConfig; } }

        public void Load(string strPath)
        {
            LoadSeasonConfig(strPath + "SchemeMatchSeasonPrize.csv");
        }

        public void Unload()
        {
            m_mapSeasonPrizeConfig.Clear();
            m_mapSeasonPrizeConfig = null;
        }

        // 任务脚本
        private void LoadSeasonConfig(string strPath)
        {
            ScpReader reader = new ScpReader(strPath, true, 2);

            m_mapSeasonPrizeConfig = new Dictionary<int, Dictionary<int, SSchemeSeasonPrizeConfig>>();

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;

                SSchemeSeasonPrizeConfig node = new SSchemeSeasonPrizeConfig();

                node.nPrizeConfigID = reader.GetInt(i, nCol++, 0);
                node.szSeasonDesc = reader.GetString(i, nCol++, "");
                node.nRankGrade = reader.GetInt(i, nCol++, 0);
                node.szGradeDesc = reader.GetString(i, nCol++, "");

                string[] strNormalPrizeList = reader.GetString(i, nCol++, "").Split(';');
                if (strNormalPrizeList != null && strNormalPrizeList.Length > 0)
                {
                    node.arrNormalPrize = new int[strNormalPrizeList.Length];
                    for (int j = 0; j < strNormalPrizeList.Length; j++)
                    {
                        Int32.TryParse(strNormalPrizeList[j], out node.arrNormalPrize[j]);
                    }
                }

                string[] strExtraConditionList = reader.GetString(i, nCol++, "").Split(';');
                if (strExtraConditionList != null && strExtraConditionList.Length > 0)
                {
                    node.arrExtraCondition = new int[strExtraConditionList.Length];
                    for (int j = 0; j < strExtraConditionList.Length; j++)
                    {
                        Int32.TryParse(strExtraConditionList[j], out node.arrExtraCondition[j]);
                    }
                }

                string[] strExtraPrizeList = reader.GetString(i, nCol++, "").Split(';');
                if (strExtraPrizeList != null && strExtraPrizeList.Length > 0)
                {
                    node.arrExtraPrize = new int[strExtraPrizeList.Length];
                    for (int j = 0; j < strExtraPrizeList.Length; j++)
                    {
                        Int32.TryParse(strExtraPrizeList[j], out node.arrExtraPrize[j]);
                    }
                }

                node.nAccountScore = reader.GetInt(i, nCol++, 0);
                node.szNormalCondition = reader.GetString(i, nCol++, "");
                node.szExtralCondition = reader.GetString(i, nCol++, "");

                if (m_mapSeasonPrizeConfig.ContainsKey(node.nPrizeConfigID) == false)
                {
                    Dictionary<int, SSchemeSeasonPrizeConfig> dicPrize = new Dictionary<int, SSchemeSeasonPrizeConfig>();
                    m_mapSeasonPrizeConfig.Add(node.nPrizeConfigID, dicPrize);
                }

                m_mapSeasonPrizeConfig[node.nPrizeConfigID].Add(node.nRankGrade, node);
            }
        }

        public SSchemeSeasonPrizeConfig GetSeasonPrizeData(int nConfigID, int nRankGrade)
        {
            if (m_mapSeasonPrizeConfig.ContainsKey(nConfigID) == false)
                return null;

            Dictionary<int, SSchemeSeasonPrizeConfig> temp = m_mapSeasonPrizeConfig[nConfigID];

            if (temp.ContainsKey(nRankGrade) == false)
                return null;

            return temp[nRankGrade];
        }

    }
}
