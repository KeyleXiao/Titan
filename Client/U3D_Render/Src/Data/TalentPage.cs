using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.TalentPage
{


    // 与TalentPage.csv表里面的列对应
    public enum Talent : int
    {
        EMT_ID,         // 索引id
        EMT_NAME,       // 称号名字
        EMT_ICONID,     // 天赋图片ｉｄ
        EMT_TYPE,       // 称号类型
        EMT_Level,      // 天赋等级
        EMT_AllowNum,   // 天赋限制点
        EMT_MaxAddNum,  // 最大加点数
        EMT_EffectArray,       // 天赋效果组
    }

    // 与TalentPageEffect.csv表里面的列对应
    public enum TalentEffect : int
    {
        EMT_EFFECCT_ID,          // 效果id 
        EMT_EFFECCT_ARRAY,       // 效果组
        EMT_EFFECT_DESC,         // 天赋描述
    }

    // 与TalentPageLevel.csv表里面的列对应
    public enum TalentCountLevel : int
    {
        EMT_LEVEL_ID,             // 等级数 
        EMT_LEVEL_USECOUNT,       // 等级可使用数
    }

    public enum TalentType
    {
        // 攻击系
        ENUM_TALENTTYPE_ATTACK = 1,
        // 诡诈系
        ENUM_TALENTTYPE_TRICK,
        // 辅助系
        ENUM_TALENTTYPE_HELP
    };

    public enum TalentLevel
    {
        // 等级1
        ENUM_TalentLevel_1 = 1,
        // 等级2
        ENUM_TalentLevel_2,
        // 等级3
        ENUM_TalentLevel_3,
        // 等级4
        ENUM_TalentLevel_4,
        // 等级5
        ENUM_TalentLevel_5,
        // 等级6
        ENUM_TalentLevel_6,

        ENUM_TalentLevel_Max
    };
    /// <summary>
    /// 天赋类
    /// </summary>
    public class SSchemeTalentConfig
    {

        public int nId;              // id

        public string szTalentName;  // 天赋名字

        public int nTalentIconId;    // 天赋图片ｉｄ

        public int nTalentType;      // 类型

        public int nTalentLevel;     // 天赋等级

        public int nTalentAllowNum;  // 天赋限制点

        public int nMaxAddNum;       //　最大加点数

        public string szTalentArray; // 天赋效果组


    }
    /// <summary>
    /// 天赋效果类
    /// </summary>
    public class SSchemeTalentEffectConfig
    {
        public int nEffectArrayId; // 效果id 

        public string szEffectArray;  // 效果组

        public string szTalentDESC;   // 天赋描述
    }

    /// <summary>
    /// 天赋等级类
    /// </summary>
    public class SSchemeTalentLevelConfig
    {
        public int nTalentLevelId;        // 等级数 

        public int nTalentLevelUseCount;  // 等级可使用数
    }

    class TalentPage
    {
        public const int DefaultTypeCount = 3;
        public const int DefaultLevelCount = 6;
        public const int DefaultSameLevelCount = 3;
        public Hashtable allTalentPageTable;
        private static TalentPage singleton = new TalentPage();
        public int[, ,] aTalentArray;  // 储存天赋id,根据类型和等级划分出来，大区分类型小区分等级、为后面控制打下铺垫
        public static TalentPage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new TalentPage();
            }
        }

        public void Load(string stPath)
        {
            InitTalentPageCsv(stPath);
        }

        public void UnLoad()
        {
            if (allTalentPageTable != null)
            {
                allTalentPageTable.Clear();
                allTalentPageTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitTalentPageCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            allTalentPageTable = new Hashtable();
            aTalentArray = new int[DefaultTypeCount, DefaultLevelCount, DefaultSameLevelCount]; // 目前写死
            int nCurType = 0;
            int nCurLevel = 0;
            int nSameLevelIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeTalentConfig sConfig = new SSchemeTalentConfig();
                sConfig.nId = reader.GetInt(i, (int)Talent.EMT_ID, 0);
                sConfig.szTalentName = reader.GetString(i, (int)Talent.EMT_NAME, "");
                sConfig.nTalentIconId = reader.GetInt(i, (int)Talent.EMT_ICONID, 0);
                sConfig.nTalentType = reader.GetInt(i, (int)Talent.EMT_TYPE, 0);
                sConfig.nTalentLevel = reader.GetInt(i, (int)Talent.EMT_Level, 0);
                sConfig.nTalentAllowNum = reader.GetInt(i, (int)Talent.EMT_AllowNum, 0);
                sConfig.nMaxAddNum = reader.GetInt(i, (int)Talent.EMT_MaxAddNum, 0);
                sConfig.szTalentArray = reader.GetString(i, (int)Talent.EMT_EffectArray, "");

                if (sConfig.nId == 0)
                {
                    continue;
                }

                nSameLevelIndex++;
                if (nCurType != sConfig.nTalentType)
                {
                    nCurType = sConfig.nTalentType;
                    nCurLevel = sConfig.nTalentLevel;
                    nSameLevelIndex = 0;
                }
                if (nCurLevel != sConfig.nTalentLevel)
                {
                    // button等级层次变化的时候
                    nSameLevelIndex = 0;
                    nCurLevel = sConfig.nTalentLevel;
                }
                aTalentArray[sConfig.nTalentType - 1, sConfig.nTalentLevel - 1, nSameLevelIndex] = sConfig.nId;
                allTalentPageTable[sConfig.nId] = sConfig;
            }
        }

        public void GetSameTypeLevelId(int nTalentType, int nTalentLevel, ref int[] nOtherTalentId)
        {
            for (int i = 0; i < 3; i++)
            {
                if (aTalentArray[nTalentType - 1, nTalentLevel - 1, i] == 0)
                {
                    continue;
                }

            }

        }

        // 判断是否有该id的天赋信息
        private bool IsHasTalentInfo(int nTalentId, ref SSchemeTalentConfig sTalentPage)
        {
            bool res = false;
            if (allTalentPageTable.ContainsKey(nTalentId))
            {
                sTalentPage = allTalentPageTable[nTalentId] as SSchemeTalentConfig;
                res = true;
            }
            return res;
        }

        // 根据id获得天赋的信息
        public void GetTalentInfo(int nTalentId, ref SSchemeTalentConfig sTalentPage)
        {
            if (!IsHasTalentInfo(nTalentId, ref sTalentPage))
            {
                Trace.LogWarning("没有找到id为：" + nTalentId + "的天赋");
                sTalentPage = null;
            }
        }
        // 获得所有天赋信息
        public Hashtable GetAllTalentPageTable()
        {

            return allTalentPageTable;
        }

        public int GetSingleTalentType(int nTalentId)
        {
            if (allTalentPageTable.ContainsKey(nTalentId))
            {
                SSchemeTalentConfig talentConfig = allTalentPageTable[nTalentId] as SSchemeTalentConfig;
                return talentConfig.nTalentType;
            }
            return 0;
        }


    }

    class TalentPageEffect
    {
        public Hashtable allTalentEffectTable;
        private static TalentPageEffect singleton = new TalentPageEffect();

        public static TalentPageEffect Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new TalentPageEffect();
            }
        }

        public void Load(string stPath)
        {
            InitTalentEffectCsv(stPath);
        }

        public void UnLoad()
        {
            if (allTalentEffectTable != null)
            {
                allTalentEffectTable.Clear();
                allTalentEffectTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitTalentEffectCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            allTalentEffectTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeTalentEffectConfig sConfig = new SSchemeTalentEffectConfig();
                sConfig.nEffectArrayId = reader.GetInt(i, (int)TalentEffect.EMT_EFFECCT_ID, 0);
                sConfig.szEffectArray = reader.GetString(i, (int)TalentEffect.EMT_EFFECCT_ARRAY, "");
                sConfig.szTalentDESC = reader.GetString(i, (int)TalentEffect.EMT_EFFECT_DESC, "");

                if (sConfig.nEffectArrayId == 0)
                {
                    continue;
                }
                allTalentEffectTable[sConfig.nEffectArrayId] = sConfig;
            }
        }
        // 判断是否有该id的天赋效果信息
        private bool IsHasTalentEffectInfo(int nTalentEffectId, ref SSchemeTalentEffectConfig sTalentEffect)
        {
            bool res = false;
            if (allTalentEffectTable.ContainsKey(nTalentEffectId))
            {
                sTalentEffect = allTalentEffectTable[nTalentEffectId] as SSchemeTalentEffectConfig;
                res = true;
            }
            return res;
        }

        // 根据id获得天赋效果信息
        public void GetTalentEffectInfo(int nTalentEffectId, ref SSchemeTalentEffectConfig sTalentEffect)
        {
            if (!IsHasTalentEffectInfo(nTalentEffectId, ref sTalentEffect))
            {
                Trace.LogWarning("没有找到id为：" + nTalentEffectId + "的天赋");
                sTalentEffect = null;
            }
        }
        // 获得所有天赋效果信息
        public Hashtable GetAllTalentEffectTable()
        {
            return allTalentEffectTable;
        }

    }

    class TalentPageLevel
    {
        public Hashtable allTalentLevelTable;
        private static TalentPageLevel singleton = new TalentPageLevel();

        public static TalentPageLevel Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new TalentPageLevel();
            }
        }

        public void Load(string stPath)
        {
            InitTalentLevelCsv(stPath);
        }

        public void UnLoad()
        {
            if (allTalentLevelTable != null)
            {
                allTalentLevelTable.Clear();
                allTalentLevelTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitTalentLevelCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            allTalentLevelTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeTalentLevelConfig sConfig = new SSchemeTalentLevelConfig();
                sConfig.nTalentLevelId = reader.GetInt(i, (int)TalentCountLevel.EMT_LEVEL_ID, 0);
                sConfig.nTalentLevelUseCount = reader.GetInt(i, (int)TalentCountLevel.EMT_LEVEL_USECOUNT, 0);

                if (sConfig.nTalentLevelId == 0)
                {
                    continue;
                }

                allTalentLevelTable[sConfig.nTalentLevelId] = sConfig;
            }
        }
        // 判断是否有该id的天赋效果信息
        private bool IsHasTalentLevelInfo(int nTalentLevelId, ref SSchemeTalentLevelConfig sTalentLevel)
        {
            bool res = false;
            if (allTalentLevelTable.ContainsKey(nTalentLevelId))
            {
                sTalentLevel = allTalentLevelTable[nTalentLevelId] as SSchemeTalentLevelConfig;
                res = true;
            }
            return res;
        }

        // 根据id获得天赋效果信息
        public void GetTalentLevelInfo(int nTalentLevelId, ref SSchemeTalentLevelConfig sTalentLevel)
        {
            if (!IsHasTalentLevelInfo(nTalentLevelId, ref sTalentLevel))
            {
                Trace.LogWarning("没有找到id为：" + nTalentLevelId + "的天赋等级");
                sTalentLevel = null;
            }
        }

        // 根据id获得天赋效果信息
        public int GetTalentLevelUseCount(int nTalentLevelId)
        {
            SSchemeTalentLevelConfig sTalentLevel = new SSchemeTalentLevelConfig();
            if (!IsHasTalentLevelInfo(nTalentLevelId, ref sTalentLevel))
            {
                Trace.LogWarning("没有找到id为：" + nTalentLevelId + "的天赋等级");
                sTalentLevel = null;
                return 0;
            }

            return sTalentLevel.nTalentLevelUseCount;
        }

        // 获得所有天赋效果信息
        public Hashtable GetAllTalentLevelTable()
        {
            return allTalentLevelTable;
        }

    }
}
