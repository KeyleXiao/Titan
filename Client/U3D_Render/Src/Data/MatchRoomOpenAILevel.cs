using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.MatchRoomOpenAILevel
{
    // 与MatchRoomOpenAILevel.csv表里面的列对应
    public enum EMatchRoomOpenAILevel : int
    {
        ERC_MATCHTYPE_ID,      // 战场id
        ERC_AILEVEL_DES,       // AI等级描述
        ERC_AILEVEL_SCORE,     // AI等级分数
    }

    /// <summary>
    /// ai配置类
    /// </summary>
    public class SSchemeMatchRoomOpenAILevel
    {
        public int nMatchTypeID;                // 比赛类型ID 

        public string AILevelDes;           // AI等级描述

        public string AILevelScore;         // AI等级分数
    }

    class MatchRoomOpenAILevel
    {
        public Hashtable allOpenAILevelTable = new Hashtable();
        private static MatchRoomOpenAILevel singleton = new MatchRoomOpenAILevel();
       
        public static MatchRoomOpenAILevel Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new MatchRoomOpenAILevel();
            }
        }

        public void Load(string stPath)
        {
            InitMatchRoomOpenAILevelConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (allOpenAILevelTable != null)
            {
                allOpenAILevelTable.Clear();
                allOpenAILevelTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitMatchRoomOpenAILevelConfigCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2); 
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                
                SSchemeMatchRoomOpenAILevel sConfig = new SSchemeMatchRoomOpenAILevel();
                sConfig.nMatchTypeID = reader.GetInt(i, (int)EMatchRoomOpenAILevel.ERC_MATCHTYPE_ID, 0);
                sConfig.AILevelDes = reader.GetString(i, (int)EMatchRoomOpenAILevel.ERC_AILEVEL_DES, "");
                sConfig.AILevelScore = reader.GetString(i, (int)EMatchRoomOpenAILevel.ERC_AILEVEL_SCORE, "");

                // 配置检查
                string[] AILevelDesStr = sConfig.AILevelDes.Split(';');
                int nDescLength = AILevelDesStr.Length;

                string[] AIScoreArrayStr = sConfig.AILevelScore.Split(';');
                int nScoreLength = AIScoreArrayStr.Length;

                if (nDescLength != nScoreLength)
                {
                    Trace.LogError("配置出错 请检查 MatchRoomOpenAILevel.csv 位置：=(" + (i + 1) + ")行");
                }

                if (allOpenAILevelTable.ContainsKey(sConfig.nMatchTypeID))
                {
                    Trace.LogError("重复id 检查配置MatchRoomOpenAILevel.csv！id =" + sConfig.nMatchTypeID);
                }

                allOpenAILevelTable.Add(sConfig.nMatchTypeID, sConfig);
            }
        }

        // 根据id获得天赋效果信息
        public void GetOpenAILevel(int nMatchTypeID, ref SSchemeMatchRoomOpenAILevel sAILevelConfig)
        {
            if (!allOpenAILevelTable.ContainsKey(nMatchTypeID))
            {
                sAILevelConfig = null;
            }
            sAILevelConfig = allOpenAILevelTable[nMatchTypeID] as SSchemeMatchRoomOpenAILevel;
        }
    }
}
