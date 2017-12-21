using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.HeroAIConfig
{

    
    
    // 与HeroAIConfig.csv表里面的列对应
    public enum HeroAI : int
    {
        EMT_BATTLEFIELDID,  // 战场id
        EMT_HEROID,         // 英雄id
        EMT_AI_IDLIST,      // AIID列表
        EMT_AI_RANKLIST,    // AI段位列表
        EMT_AI_DESLIST,     // AI描述列表
        EMT_AI_ISOPEN,      // 是否开启
    }

    /// <summary>
    /// ai配置类
    /// </summary>
    public class SSchemeHeroAIConfig
    {
        public int nBattleFieldID;   // 战场ID

        public int nHeroId;          // id

        public string szAIIDArray;      // AI ID列表
		
		public string szAIRankArray;    // Rank列表

        public string szAIDESArray;     // AI ID描述列表

        public bool bIsOpen;            // 是否开启

    }

    class HeroAIConfig
    {
        public int KEY_ID_BASE = 1000000;
        public Hashtable allHeroAIConfigTable;
        private static HeroAIConfig singleton = new HeroAIConfig();
       
        public static HeroAIConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new HeroAIConfig();
            }
        }

        public void Load(string stPath)
        {
            InitHeroAIConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (allHeroAIConfigTable != null)
            {
                allHeroAIConfigTable.Clear();
                allHeroAIConfigTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitHeroAIConfigCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            allHeroAIConfigTable = new Hashtable();
            
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeHeroAIConfig sConfig = new SSchemeHeroAIConfig();
                sConfig.nBattleFieldID = reader.GetInt(i, (int)HeroAI.EMT_BATTLEFIELDID, 0);
                sConfig.nHeroId = reader.GetInt(i, (int)HeroAI.EMT_HEROID, 0);
                sConfig.szAIIDArray = reader.GetString(i, (int)HeroAI.EMT_AI_IDLIST, "");
                sConfig.szAIRankArray = reader.GetString(i, (int)HeroAI.EMT_AI_RANKLIST, "");
                sConfig.szAIDESArray = reader.GetString(i, (int)HeroAI.EMT_AI_DESLIST, "");
                sConfig.bIsOpen = reader.GetInt(i, (int)HeroAI.EMT_AI_ISOPEN, 0) == 1? true: false;

                // 配置检查
                string szAIDescList = sConfig.szAIDESArray;
                string[] AIDescArrayStr = szAIDescList.Split(';');
                int nAIDescArrayLength = AIDescArrayStr.Length;

                string szAIIDList = sConfig.szAIIDArray;
                string[] AIIDArrayStr = szAIDescList.Split(';');
                int nAIIDArrayLength = AIIDArrayStr.Length;

                if (nAIDescArrayLength != nAIIDArrayLength)
                {
                    Trace.LogError("配置出错 请检查 HeroAIConfig.csv 位置：=(" + (i + 1) + ")行");
                }

                int keyID = sConfig.nBattleFieldID * KEY_ID_BASE + sConfig.nHeroId;
                allHeroAIConfigTable[keyID] = sConfig;
            }
        }

        private bool IsHasHeroAIInfo(int nConfigWarID, int nHeroID, ref SSchemeHeroAIConfig sHeroAIConfig)
        {
            bool res = false;
            int key = nConfigWarID * KEY_ID_BASE + nHeroID;
            if (allHeroAIConfigTable.ContainsKey(key))
            {
                sHeroAIConfig = allHeroAIConfigTable[key] as SSchemeHeroAIConfig;
                res = true;
            }
            return res;
        }

        // 根据id获得天赋效果信息
        public void GetHeroAIInfo(int nConfigWarID,int nHeroID, ref SSchemeHeroAIConfig sHeroAIConfig)
        {
            if (!IsHasHeroAIInfo(nConfigWarID, nHeroID, ref sHeroAIConfig))
            {
                sHeroAIConfig = null;
            }
        }
    }
}
