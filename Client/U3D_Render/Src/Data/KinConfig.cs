using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.KinConfig
{

    // 与KinConfig.csv表里面的列对应
    public enum KinConfigRow : int
    {
        EMT_ROW_CRETE_NEED_LEVEL,    // 创建战队需要的等级
        EMT_ROW_CRETE_NEED_MONEY,    // 创建战队需要的资金
        EMT_ROW_APPLY_NEED_LEVEL,    // 申请加入需要的等级
        EMT_ROW_JOIN_KIN_FREEZE,     // 加入战队冷却
        EMT_ROW_KIN_DETAIL_DES = 8,  // 战队详细说明
        EMT_ROW_MAX,        // 最大
        
    }

    public enum EKinLevelCol
    {
        KIN_LEVEL_COL_LEVEL = 0,
        KIN_LEVEL_COL_MAX_PERSON_COUNT = 1,
    }

    /// <summary>
    /// ai配置类
    /// </summary>
    public class SSchemeKinConfig
    {
        public int nCreateNeedLevel;   // 创建需要的等级

        public int nCreateNeedMoney;   // 创建需要的金币

        public int nApplyNeedLevel;   //  申请加入需要的等级

        public int nJoinKinFreeze;   //  加入战队冷却

        public string sKinDetailDes;   //  战队详细说明UBB格式
        
    }

    // 战队每级的升级信息
    public class SSchemeKinLevel
    {
        public int nMaxPersonCount;                        // 人数上限
    };

    class KinConfig
    {
        public SSchemeKinConfig tKinConfig;
        public Dictionary<int, SSchemeKinLevel> tKinLevelMap;
        private static KinConfig singleton = new KinConfig();
       
        public static KinConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new KinConfig();
            }
        }

        public void Load(string stPath)
        {
            InitKinConfigCsv(stPath + "KinConfig.csv");
            LoadKinLevelCsv(stPath + "KinLevel.csv");
        }

        public void UnLoad()
        {
            if (tKinConfig != null)
            {
                tKinConfig = null;
            }

            if (tKinLevelMap != null)
            {
                tKinLevelMap.Clear();
                tKinLevelMap = null;
            }

            singleton = null;
        }

        // 初始化csv表
        protected void InitKinConfigCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            tKinConfig = new SSchemeKinConfig();

            for (int i = 0; i < (int)KinConfigRow.EMT_ROW_MAX; i++)
            {
                switch (i)
                {
                    case (int)KinConfigRow.EMT_ROW_CRETE_NEED_LEVEL: // 创建战队需要的等级
                        {
                            tKinConfig.nCreateNeedLevel = reader.GetInt(i, 1, 0);
                        }
                        break;

                    case (int)KinConfigRow.EMT_ROW_CRETE_NEED_MONEY:   // 创建需要的金币
                        {
                            tKinConfig.nCreateNeedMoney = reader.GetInt(i, 1, 0);
                        }
                        break;

                    case (int)KinConfigRow.EMT_ROW_APPLY_NEED_LEVEL:   // 申请加入需要的等级
                        {
                            tKinConfig.nApplyNeedLevel = reader.GetInt(i, 1, 0); 
                        }
                        break;
                    case (int)KinConfigRow.EMT_ROW_JOIN_KIN_FREEZE:   // 加入战队冷却
                        {
                            tKinConfig.nJoinKinFreeze = reader.GetInt(i, 1, 0); 
                        }
                        break;
                    case (int)KinConfigRow.EMT_ROW_KIN_DETAIL_DES:   // 战队详细说明
                        {
                            tKinConfig.sKinDetailDes = reader.GetString(i, 1, string.Empty); 
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        private void LoadKinLevelCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            tKinLevelMap = new Dictionary<int, SSchemeKinLevel>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeKinLevel kinLevel = new SSchemeKinLevel();
                int level = reader.GetInt(i, (int)EKinLevelCol.KIN_LEVEL_COL_LEVEL, 0);
                kinLevel.nMaxPersonCount = reader.GetInt(i, (int)EKinLevelCol.KIN_LEVEL_COL_MAX_PERSON_COUNT, 0);
                tKinLevelMap[level] = kinLevel;
            }
        }
    }
}
