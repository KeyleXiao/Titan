using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.SelectArrayTipPage
{
    public enum HeroType
    {
        HEROTYPE_SOLDIER = 0,
        HEROTYPE_MAGIC,
        HEROTYPE_ASSASSIN,
        HEROTYPE_SUPPORT,
        HEROTYPE_ADC,
        HEROTYPE_ALL,
    };

    public enum LimitType
    {
        LIMITTYPE_UP = 0,
        LIMITTYPE_DOWN,
        LIMITTYPE_ALL,
    };

    public enum MatchType
    {
        MatchType_One = 1,
    };
    // 与SelectArrayTipPage.csv表里面的列对应
    public enum SELECTARRAYTIP : int
    {
        EMT_ID,                      // 比赛类型
        EMT_NAME,                    // 称号名字
        EMT_TIPLIMIT,                // 选人提示阵营限制
        EMT_SOLDIER_LIMITDOWN,       // 战士下限
        EMT_SOLDIER_LIMITUP,         // 战士下限
        EMT_MAGICDOWN,               // 法师下限
        EMT_MAGICUP,                 // 法师下限
        EMT_ASSASSINDOWN,            // 刺客下限
        EMT_ASSASSINUP,              // 刺客下限ID组
        EMT_SUPPORTDOWN,             // 辅助下限
        EMT_SUPPORTUP,               // 辅助下限
        EMT_ADCDOWN,                 // 输出下限
        EMT_ADCNUP,                  // 输出下限
    }

    /// <summary>
    /// 选人提示类
    /// </summary>
    public class SSchemeSelectArrayTipConfig
    {
        public int nId;                     // 比赛类型
        public string sName;               // 称号名字
        public int nTiplimit;              // 选人提示阵营限制
        public int nSoldier_limitDown;     // 战士下限
        public int nSoldier_limitUp;       // 战士下限
        public int nMagicDown;              // 法师下限
        public int nMagicUp;                // 法师下限
        public int nAssassinDown;           // 刺客下限
        public int nAssassinUp;             // 刺客下限id组
        public int nSupportDown;            // 辅助下限
        public int nSupportUp;              // 辅助下限
        public int nAdcDown;                // 输出下限
        public int nAdcUp;                 // 输出下限
    }

    public class SelectTeamLimitInfo
    {
        public string[] sTypeLimitWord = new string[(int)LimitType.LIMITTYPE_ALL];
        public int[] nTypeLimitCount = new int[(int)LimitType.LIMITTYPE_ALL];
    }

    class SelectArrayTipPage
    {
        public Hashtable allSelectArrayTipPageTable;
        private static SelectArrayTipPage singleton = new SelectArrayTipPage();
        private Dictionary<HeroType, int> dicTypeTeamSelectCount = new Dictionary<HeroType, int>((int)HeroType.HEROTYPE_ALL);
        private Dictionary<HeroType, SelectTeamLimitInfo> dicTypeInitInfo = new Dictionary<HeroType, SelectTeamLimitInfo>((int)HeroType.HEROTYPE_ALL);
        public static SelectArrayTipPage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new SelectArrayTipPage();
            }
        }

        public void Load(string stPath)
        {
            InitSelectArrayTipPageCsv(stPath);
        }

        public void UnLoad()
        {
            if (allSelectArrayTipPageTable != null)
            {
                allSelectArrayTipPageTable.Clear();
                allSelectArrayTipPageTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitSelectArrayTipPageCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);        
            ScpReader reader = new ScpReader(stPath, true, 2);
            allSelectArrayTipPageTable = new Hashtable();
            int nIndex = (int)SELECTARRAYTIP.EMT_ID;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeSelectArrayTipConfig sConfig = new SSchemeSelectArrayTipConfig();
                sConfig.nId = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.sName = reader.GetString(i, (int)nIndex++, "");
                sConfig.nTiplimit = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nSoldier_limitDown = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nSoldier_limitUp = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nMagicDown = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nMagicUp = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nAssassinDown = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nAssassinUp = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nSupportDown = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nSupportUp = reader.GetInt(i, (int)nIndex++, 0);              
                sConfig.nAdcDown = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.nAdcUp = reader.GetInt(i, (int)nIndex++, 0);
                allSelectArrayTipPageTable[sConfig.nId] = sConfig;
            }

            InitSelectArrayTypeWord((int)MatchType.MatchType_One);
        }

        private void InitSelectArrayTypeWord(int nMatchId)
        {
            if(allSelectArrayTipPageTable == null||!allSelectArrayTipPageTable.ContainsKey(nMatchId))
            {
                return ;
            }

            SSchemeSelectArrayTipConfig sConfig = allSelectArrayTipPageTable[nMatchId] as SSchemeSelectArrayTipConfig;
            dicTypeInitInfo.Clear();
            string UpWord = "";      
            string DownWord = "";
            int nUpCount = 0;
            int nDownCount = 0;
            for (int nTipTypeIndex = (int)HeroType.HEROTYPE_SOLDIER; nTipTypeIndex < (int)HeroType.HEROTYPE_ALL; nTipTypeIndex++)
            {
                SelectTeamLimitInfo LimitInfo = new SelectTeamLimitInfo();
                HeroType nHeroType = (HeroType)nTipTypeIndex;
                switch (nHeroType)
                {
                    case HeroType.HEROTYPE_ADC:
                        {
                            DownWord = "输出英雄不足";
                            UpWord = "输出英雄过多";
                            nUpCount = sConfig.nAdcUp;
                            nDownCount = sConfig.nAdcDown;
                        }
                        break;
                    case HeroType.HEROTYPE_ASSASSIN:
                        {
                            DownWord = "刺客英雄不足";
                            UpWord = "刺客英雄过多";
                            nUpCount = sConfig.nAssassinUp;
                            nDownCount = sConfig.nAssassinDown;
                        }
                        break;
                    case HeroType.HEROTYPE_SUPPORT:
                        {
                            DownWord = "辅助英雄不足";
                            UpWord = "辅助英雄过多";
                            nUpCount = sConfig.nSupportUp;
                            nDownCount = sConfig.nSupportDown;
                        }
                        break;
                    case HeroType.HEROTYPE_MAGIC:
                        {
                            DownWord = "法师英雄不足";
                            UpWord = "法师英雄过多";
                            nUpCount = sConfig.nMagicUp;
                            nDownCount = sConfig.nMagicDown;
                        }
                        break; break;
                    case HeroType.HEROTYPE_SOLDIER:
                        {
                            DownWord = "战士英雄不足";
                            UpWord = "战士英雄过多";
                            nUpCount = sConfig.nSoldier_limitUp;
                            nDownCount = sConfig.nSoldier_limitDown;
                        }
                        break;
                    default:
                        break;
                }

                LimitInfo.sTypeLimitWord[(int)LimitType.LIMITTYPE_UP] = UpWord;
                LimitInfo.sTypeLimitWord[(int)LimitType.LIMITTYPE_DOWN] = DownWord;
                LimitInfo.nTypeLimitCount[(int)LimitType.LIMITTYPE_UP] = nUpCount;
                LimitInfo.nTypeLimitCount[(int)LimitType.LIMITTYPE_DOWN] = nDownCount;
                dicTypeInitInfo[nHeroType] = LimitInfo;
            }
        }

        public bool IsTeamSelectArrayNeedShow(ref string sTipDesc, int nTipTypeIndex)
        {
            HeroType nHeroType = (HeroType)nTipTypeIndex;
            if (!dicTypeInitInfo.ContainsKey(nHeroType))
            {
                return false;
            }

            if (!dicTypeTeamSelectCount.ContainsKey(nHeroType))
            {
                dicTypeTeamSelectCount[nHeroType] = 0;
            }

            if ((dicTypeTeamSelectCount[nHeroType] < dicTypeInitInfo[nHeroType].nTypeLimitCount[(int)LimitType.LIMITTYPE_DOWN]))
            {
                sTipDesc = dicTypeInitInfo[nHeroType].sTypeLimitWord[(int)LimitType.LIMITTYPE_DOWN];
                return true;
            }
            else if (dicTypeTeamSelectCount[nHeroType] > dicTypeInitInfo[nHeroType].nTypeLimitCount[(int)LimitType.LIMITTYPE_UP])
            {
                sTipDesc = dicTypeInitInfo[nHeroType].sTypeLimitWord[(int)LimitType.LIMITTYPE_UP];
                return true;
            }

            return false;
        }

        public void SetSelectTypeInfo(Dictionary<int, int> dicCampTeamHeroTip)
        {
            dicTypeTeamSelectCount.Clear();
            foreach (int nHeroType in dicCampTeamHeroTip.Values)
            {
                HeroType heroType = (HeroType)nHeroType;
                if (heroType < HeroType.HEROTYPE_SOLDIER || heroType>HeroType.HEROTYPE_ALL-1)
                {
                    continue;
                }

                if (dicTypeTeamSelectCount.ContainsKey(heroType))
                {
                    dicTypeTeamSelectCount[heroType]++;
                }
                else
                {
                    dicTypeTeamSelectCount[heroType] = 1;
                }             
            }            
        }

        public int GetHeroType(int nHeroId)
        {
            PersonSkinModelItem modelData = new PersonSkinModelItem();
            if (!GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref modelData))
            {
                return -1;
            }

            return modelData.nType;
        }

        public int GetHeroTypeCount()
        {
            if(dicTypeTeamSelectCount == null)
            {
                return 0;
            }

            return dicTypeTeamSelectCount.Count;
        }

        public int GetHeroCampTipLimit(int nMatchId)
        {
            if (allSelectArrayTipPageTable == null || !allSelectArrayTipPageTable.ContainsKey(nMatchId))
            {
                return -1;
            }

            SSchemeSelectArrayTipConfig sConfig = allSelectArrayTipPageTable[nMatchId] as SSchemeSelectArrayTipConfig;
            if(sConfig == null)
            {
                return -1;
            }

            return sConfig.nTiplimit;
        }
    }
}
