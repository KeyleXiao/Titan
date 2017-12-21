using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.MatchRankLimitConfig
{
    public class SMatchRankLimitHeroSchemeInfo
    {
        public int nMatchType;          //比赛类型
        public int nLimitHeroCount;     //限制的英雄数量
        public int nLimitHeroStarLevel; //限制的英雄星级
        public int nLimitPlayerLevel;   //限制的英雄星级

        public int nOtherFailedDesID;   //其他人失败描述ID
        public int nSelfFailedDesID;    //自己失败描述ID

        public int nOtherLevelFailedID;  //其他人等级失败描述ID
        public int nSelfLevelFailedDesID;//自己失败等级描述ID
    }

    public class MatchRankLimitConfig : Singleton<MatchRankLimitConfig>
    {
        private Dictionary<int, SMatchRankLimitHeroSchemeInfo> m_mapMatchRankLimitHero;

        public void Load(string strPath)
        {
            LoadRankLimitHeroConfig(strPath + "MatchRankLimitHero.csv");
        }

        public void Unload()
        {
            m_mapMatchRankLimitHero.Clear();
            m_mapMatchRankLimitHero = null;
        }

        private void LoadRankLimitHeroConfig(string strPath)
        {
            ScpReader reader = new ScpReader(strPath, true, 2);

            m_mapMatchRankLimitHero = new Dictionary<int, SMatchRankLimitHeroSchemeInfo>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;
                SMatchRankLimitHeroSchemeInfo node = new SMatchRankLimitHeroSchemeInfo();
                node.nMatchType             = reader.GetInt(i, nCol++, 0);
                node.nLimitHeroCount        = reader.GetInt(i, nCol++, 0);
                node.nLimitHeroStarLevel    = reader.GetInt(i, nCol++, 0);
                node.nLimitPlayerLevel      = reader.GetInt(i, nCol++, 0);
                node.nOtherFailedDesID      = reader.GetInt(i, nCol++, 0);
                node.nSelfFailedDesID       = reader.GetInt(i, nCol++, 0);
                node.nOtherLevelFailedID    = reader.GetInt(i, nCol++, 0);
                node.nSelfLevelFailedDesID  = reader.GetInt(i, nCol++, 0);
                m_mapMatchRankLimitHero[node.nMatchType] = node;
            }
        }

        public SMatchRankLimitHeroSchemeInfo getRankLimitHeroShemeInfo(int nMatchTypeID)
        {
            SMatchRankLimitHeroSchemeInfo sysConfig = null;
            if (m_mapMatchRankLimitHero.TryGetValue(nMatchTypeID, out sysConfig))
            {
                return sysConfig;
            }
                
            return null;
        }
    }
}
