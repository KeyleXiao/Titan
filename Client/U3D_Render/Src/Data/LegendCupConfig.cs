using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.LegendCupConfig
{
    public enum LegendCupTipType
    {
        LCTT_None,
        LCTT_OnLine_CupList,
        LCTT_OnLine_CupCreate,
        LCTT_OnLine_CupRegist,
        LCTT_OnLine_Cup32In,
        LCTT_OnLine_Cup16In,
        LCTT_OffLine_CupList,
        LCTT_OffLine_CupCreate,
        LCTT_OffLine_CupRegist,
        LCTT_OffLine_Cup32In,
        LCTT_OffLine_Cup16In,

        LCTT_MAX,
    }

    public struct SCupTypeKey : IEquatable<SCupTypeKey>
    {
        public int nCupConfigID;
        public int nRoundIndex;

        public SCupTypeKey(int configID, int roundIndexID)
        {
            this.nCupConfigID = configID;
            this.nRoundIndex = roundIndexID;
        }

        public bool Equals(SCupTypeKey other)
        {
            return nCupConfigID == other.nCupConfigID && nRoundIndex == other.nRoundIndex;
        }
    }

    // 与LegendCup.csv表里面的列对应
    public enum ECupConfigCol
    {
        ECCC_COL_CUP_CUPID,         // 比赛ID
        ECCC_COL_CUP_NAME,          // 比赛名称
        ECCC_COL_CUP_NEEDROUND,     // 需要轮数
        ECCC_COL_CUP_MAXKINNUM,     // 最大队伍数量
        ECCC_COL_CUP_CONFIGID,      // 配置ID
        ECCC_COL_CUP_DEADLINE,      // 报名截止时间

        ECCC_COL_CUP_CREATENEEDMONEY = 9,   // 创建需要金币
        ECCL_COL_CUP_REGISTNEEDMONEY,       // 注册需要金币
        ECCL_COL_CUP_REGISTHIGHESTMONEY,       // 报名最高金币
        ECCC_COL_CUP_PRIZECONFIGID,         // 奖励配置ID
        ECCC_COL_CUP_ISFREE,         // 免费标示
        ECCC_COL_CUP_MINI_START_KINNUM,     // 最小开始比赛战队数量
        ECCC_COL_CUP_VIEW_NAME,     // 配置显示名称

        ECCC_COL_CUP_MAX,                   // 最大
    }

    // 与LegendCupDetail.csv表里面的列对应
    public enum ECupDetailCol
    {
        ECDC_COL_CUP_CONFIGID,            // 比赛配置序号
        ECDC_COL_CUP_ROUNDINDEX,          // 轮数索引
        ECDC_COL_CUP_COMPETITIONTYPE,     // 该轮类型（0空1淘汰2小组）
        ECDC_COL_CUP_GROUPCOUNT,          // 小组数量
        ECDC_COL_CUP_MEMBERCOUNT,         // 组内成员数量
        ECDC_COL_CUP_WINMEMBERCOUNT,      // 晋级成员数量

        ECDC_COL_CUP_STARTDAYGAP,         // 开始时间天数间隔
        ECDC_COL_CUP_BEGINTIME,           // 创建房间提前进场时间点
        ECDC_COL_CUP_STARTTIME,           // 每天开赛时间点
        ECDC_COL_CUP_BOTYPE,              // 比赛类型(1BO1一把定胜负2BO3三局两胜3BO5五局三胜)
        ECDC_COL_CUP_BOTIMEGAP,           // bo赛的时间间隔(分钟)
        ECDC_COL_CUP_ROUNDENDGAP,         // 本轮结算时间天数间隔
        ECDC_COL_CUP_ROUND_RESET_ENDTIME,  // 本轮可重置时间点
        ECDC_COL_CUP_ROUNDENDTIME,        // 本轮结算时间点
        ECDC_COL_CUP_ROUNDDESC,           // 本轮名称

        ECDC_COL_CUP_MAX,                 // 最大
    }

    // 与LegendCupTipConfig.csv表里面的列对应
    public enum ECupTipCol
    {
        ECTC_COL_TIP_ID,         // 说明ID
        ECTC_COL_TIP_TITLE,      // 说明标题
        ECTC_COL_TIP_CONTENT,    // 说明内容

        ECTC_COL_TIP_MAX,            // 最大
    }

    // 与LegendCupPrizeConfig.csv表里面的列对应
    public enum ECupPrizeCol
    {
        ECPC_COL_PRIZE_CONFIGID,    // 配置ID
        ECPC_COL_PRIZE_STARTPOS,    // 名次奖励开始区间
        ECPC_COL_PRIZE_ENDPOS,      // 名次奖励结束区间
        ECPC_COL_PRIZE_PRIZEIDLIST, // 奖励ID列表
        ECPC_COL_PRIZE_BONUSRATIO,  // 奖金分配比例
        ECPC_COL_PRIZE_DESC,        // 描述

        ECPC_COL_PRIZE_MAX,         // 最大
    }

    // 与LegendCupTypeInfo.csv表里面的列对应
    public enum ECupTypeCol
    {
        ECTC_COL_TYPE_CREATETYPE,       // 创建的类型（1、系统 2、 个人 3、 联盟）
        ECTC_COL_TYPE_FREECUPID,        // 免费比赛ID列表
        ECTC_COL_TYPE_CHARGECUPID,      // 收费比赛ID列表
        ECTC_COL_TYPE_ISSHOW,           // 创建类型是否显示

        ECTC_COL_TYPE_MAX,         // 最大
    }

    // 与LegendCupLimit.csv表里面的列对应
    public enum ECupLimitCol
    {
        ECLC_COL_LIMIT_CONFIGID,        // 配置ID
        ECLC_COL_LIMIT_COUNT,           // 人数限制

        ECLC_COL_LIMIT_MAX,             // 最大
    }

    public class SSchemeLegendCupConfig
    {
        public int nConfigID;               // 配置ID
        public string sCupConfigName;       // 比赛名称
        public int nNeedRoundNum;           // 需要轮数
        public int nMaxKinNum;              // 最大队伍数量
        public int nCupDetailID;            // 配置ID
        public int[] nDeadline;             // 报名截止时间
        public int nCreateNeedMoney;        // 创建需要金币
        public int nRegistNeedMoney;        // 报名需要金币
        public int nRegistHighestMoney;     // 报名最高金币
        public int nPrizeConfigID;          // 奖励配置ID
        public bool bIsFree;                // 是否免费
        public int nMiniStartCount;         // 最小
        public string sCupConfigViewName;   // 配置显示名称
    }

    public class SSchemeLegendCupDetailConfig
    {
        public int nCupConfigID;         // 比赛配置序号
        public int nRoundIndex;          // 轮数索引
        public int nCompetitionType;     // 该轮类型（0空1淘汰2小组）
        public int nGroupCount;          // 小组数量
        public int nMemberCount;         // 组内成员数量
        public int nWinMemberCount;      // 晋级成员数量

        public int nStartDayGap;         // 开始时间天数间隔
        public int[] nBeginTime;         // 创建房间提前进场时间点
        public int[] nStartTime;         // 每天开赛时间点
        public int nBoType;              // 比赛类型(1BO1一把定胜负2BO3三局两胜3BO5五局三胜)
        public int nBoTimeGap;           // bo赛的时间间隔(分钟)
        public int nRoundEndGap;         // 本轮结算时间天数间隔
        public int[] nRoundResetEndTime; // 本轮结算时间点
        public int[] nRoundEndTime;      // 本轮结算时间点
        public string sRoundDes;         // 本轮名称
    }

    public class SSchemeLegendCupTipConfig
    {
        public int nTipID;                  // 说明ID
        public string sCupTitle;            // 说明标题
        public string sCupContent;          // 说明内容
    }

    public class SSchemeLegendCupPrizeConfig
    {
        public int nConfigID;
        public int nStartPos;
        public int nEndPos;
        public int[] nPrizeIDList;
        public int nBonusRatio;
        public string sDesc;
    }

    public class SSchemeLegendCupTypeConfig
    {
        public int nCreateType;
        public int[] nFreeCupConfigIDList;
        public int[] nChargeCupconfigIDList;
        public int nIsShow;
    }

    public class SSchemeLegendCupLimitConfig
    {
        public int nConfigID;
        public int nCountLimit;
    }

    class LegendCupConfig
    {
        private static LegendCupConfig singleton = new LegendCupConfig();

        public static LegendCupConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new LegendCupConfig();
            }
        }

        private Dictionary<int, SSchemeLegendCupConfig> m_cupConfig;
        private Dictionary<SCupTypeKey, SSchemeLegendCupDetailConfig> m_cupDetailConfig;
        private Dictionary<int, SSchemeLegendCupTipConfig> m_cupTipConfig;
        private Dictionary<int, List<SSchemeLegendCupPrizeConfig>> m_cupPrizeConfig;
        private List<SSchemeLegendCupTypeConfig> m_cupTypeConfig;
        private Dictionary<int, SSchemeLegendCupLimitConfig> m_cupLimitConfig;
        
        public void Load(string stPath)
        {
            LoadLegendCupConfig(stPath + "LegendCup.csv");
            LoadLegendCupDetailConfig(stPath + "LegendCupDetail.csv");
            LoadLegendCupTipConfig(stPath + "LegendCupTipConfig.csv");
            LoadLegendCupPrizeConfig(stPath + "LegendCupPrizeConfig.csv");
            LoadLegendCupTypeConfig(stPath + "LegendCupTypeInfo.csv");
            LoadLegendCupLimitConfig(stPath + "LegendCupLimit.csv");
        }

        public void Unload()
        {
            m_cupConfig.Clear();
            m_cupDetailConfig.Clear();
            m_cupTipConfig.Clear();
            m_cupPrizeConfig.Clear();
            m_cupTypeConfig.Clear();
            m_cupLimitConfig.Clear();
            m_cupConfig = null;
            m_cupDetailConfig = null;
            m_cupTipConfig = null;
            m_cupPrizeConfig = null;
            m_cupTypeConfig = null;
            m_cupLimitConfig = null;
        }

        // 比赛脚本
        private void LoadLegendCupConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupConfig = new Dictionary<int,SSchemeLegendCupConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupConfig cupInfo = new SSchemeLegendCupConfig();
                cupInfo.nConfigID = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_CUPID, 0);
                cupInfo.sCupConfigName = reader.GetString(i, (int)ECupConfigCol.ECCC_COL_CUP_NAME, "");
                cupInfo.nNeedRoundNum = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_NEEDROUND, 0);
                cupInfo.nMaxKinNum = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_MAXKINNUM, 0);
                cupInfo.nCupDetailID = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_CONFIGID, 0);
                string[] strDeadline = reader.GetString(i, (int)ECupConfigCol.ECCC_COL_CUP_DEADLINE, "").Split(';');
                if (strDeadline != null && strDeadline.Length > 0)
                {
                    cupInfo.nDeadline = new int[strDeadline.Length];
                    for (int j = 0; j < strDeadline.Length; ++j)
                    {
                        cupInfo.nDeadline[j] = Convert.ToInt32(strDeadline[j]);
                    }
                }
                cupInfo.nCreateNeedMoney = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_CREATENEEDMONEY, 0);
                cupInfo.nRegistNeedMoney = reader.GetInt(i, (int)ECupConfigCol.ECCL_COL_CUP_REGISTNEEDMONEY, 0);
                cupInfo.nRegistHighestMoney = reader.GetInt(i, (int)ECupConfigCol.ECCL_COL_CUP_REGISTHIGHESTMONEY, 0);
                cupInfo.nPrizeConfigID = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_PRIZECONFIGID, 0);
                cupInfo.bIsFree = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_ISFREE, 0) > 0;
                cupInfo.nMiniStartCount = reader.GetInt(i, (int)ECupConfigCol.ECCC_COL_CUP_MINI_START_KINNUM, 0);
                cupInfo.sCupConfigViewName = reader.GetString(i, (int)ECupConfigCol.ECCC_COL_CUP_VIEW_NAME, "");

                m_cupConfig[cupInfo.nConfigID] = cupInfo;
            }
        }

        // 比赛详细脚本
        private void LoadLegendCupDetailConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupDetailConfig = new Dictionary<SCupTypeKey, SSchemeLegendCupDetailConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupDetailConfig cupTypeInfo = new SSchemeLegendCupDetailConfig();
                cupTypeInfo.nCupConfigID = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_CONFIGID, 0);
                cupTypeInfo.nRoundIndex = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_ROUNDINDEX, 0);
                cupTypeInfo.nCompetitionType = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_COMPETITIONTYPE, 0);
                cupTypeInfo.nGroupCount = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_GROUPCOUNT, 0);
                cupTypeInfo.nMemberCount = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_MEMBERCOUNT, 0);
                cupTypeInfo.nWinMemberCount = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_WINMEMBERCOUNT, 0);
                cupTypeInfo.nStartDayGap = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_STARTDAYGAP, 0);
                cupTypeInfo.nBoType = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_BOTYPE, 0);
                cupTypeInfo.nBoTimeGap = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_BOTIMEGAP, 0);
                cupTypeInfo.nRoundEndGap = reader.GetInt(i, (int)ECupDetailCol.ECDC_COL_CUP_ROUNDENDGAP, 0);
                cupTypeInfo.sRoundDes = reader.GetString(i, (int)ECupDetailCol.ECDC_COL_CUP_ROUNDDESC, "");

                string[] strBeginTime = reader.GetString(i, (int)ECupDetailCol.ECDC_COL_CUP_BEGINTIME, "").Split(';');
                if (strBeginTime != null && strBeginTime.Length > 0)
                {
                    cupTypeInfo.nBeginTime = new int[strBeginTime.Length];
                    for (int j = 0; j < strBeginTime.Length; ++j)
                    {
                        cupTypeInfo.nBeginTime[j] = Convert.ToInt32(strBeginTime[j]);
                    }
                }

                string[] strStartTime = reader.GetString(i, (int)ECupDetailCol.ECDC_COL_CUP_STARTTIME, "").Split(';');
                if (strStartTime != null && strStartTime.Length > 0)
                {
                    cupTypeInfo.nStartTime = new int[strStartTime.Length];
                    for (int p = 0; p < strStartTime.Length; ++p)
                    {
                        cupTypeInfo.nStartTime[p] = Convert.ToInt32(strStartTime[p]);
                    }
                }

                string[] strResetEndTime = reader.GetString(i, (int)ECupDetailCol.ECDC_COL_CUP_ROUND_RESET_ENDTIME, "").Split(';');
                if (strResetEndTime != null && strResetEndTime.Length > 0)
                {
                    cupTypeInfo.nRoundResetEndTime = new int[strResetEndTime.Length];
                    for (int q = 0; q < strResetEndTime.Length; ++q)
                    {
                        cupTypeInfo.nRoundResetEndTime[q] = Convert.ToInt32(strResetEndTime[q]);
                    }
                }

                string[] strRoundEndTime = reader.GetString(i, (int)ECupDetailCol.ECDC_COL_CUP_ROUNDENDTIME, "").Split(';');
                if (strRoundEndTime != null && strRoundEndTime.Length > 0)
                {
                    cupTypeInfo.nRoundEndTime = new int[strRoundEndTime.Length];
                    for (int q = 0; q < strRoundEndTime.Length; ++q)
                    {
                        cupTypeInfo.nRoundEndTime[q] = Convert.ToInt32(strRoundEndTime[q]);
                    }
                }

                SCupTypeKey typeKey = new SCupTypeKey(cupTypeInfo.nCupConfigID, cupTypeInfo.nRoundIndex);
                m_cupDetailConfig[typeKey] = cupTypeInfo;
            }
        }

        // 比赛说明脚本
        private void LoadLegendCupTipConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupTipConfig = new Dictionary<int, SSchemeLegendCupTipConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupTipConfig tipInfo = new SSchemeLegendCupTipConfig();
                tipInfo.nTipID = reader.GetInt(i, (int)ECupTipCol.ECTC_COL_TIP_ID, 0);
                tipInfo.sCupTitle = reader.GetString(i, (int)ECupTipCol.ECTC_COL_TIP_TITLE, "");
                tipInfo.sCupContent = reader.GetString(i, (int)ECupTipCol.ECTC_COL_TIP_CONTENT, "");

                m_cupTipConfig[tipInfo.nTipID] = tipInfo;
            }
        }

        // 比赛奖励脚本
        private void LoadLegendCupPrizeConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupPrizeConfig = new Dictionary<int, List<SSchemeLegendCupPrizeConfig>>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupPrizeConfig prizeInfo = new SSchemeLegendCupPrizeConfig();
                prizeInfo.nConfigID = reader.GetInt(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_CONFIGID, 0);
                prizeInfo.nStartPos = reader.GetInt(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_STARTPOS, 0);
                prizeInfo.nEndPos = reader.GetInt(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_ENDPOS, 0);
                string[] strPrizeIDList = reader.GetString(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_PRIZEIDLIST, "").Split(';');
                if (strPrizeIDList != null && strPrizeIDList.Length > 0)
                {
                    prizeInfo.nPrizeIDList = new int[strPrizeIDList.Length];
                    for (int j = 0; j < strPrizeIDList.Length; j++)
                    {
                        prizeInfo.nPrizeIDList[j] = Convert.ToInt32(strPrizeIDList[j]);
                    }
                }
                prizeInfo.nBonusRatio = reader.GetInt(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_BONUSRATIO, 0);
                prizeInfo.sDesc = reader.GetString(i, (int)ECupPrizeCol.ECPC_COL_PRIZE_DESC, "");

                if (m_cupPrizeConfig.ContainsKey(prizeInfo.nConfigID))
                {
                    m_cupPrizeConfig[prizeInfo.nConfigID].Add(prizeInfo);
                }
                else
                {
                    List<SSchemeLegendCupPrizeConfig> temp = new List<SSchemeLegendCupPrizeConfig>();
                    temp.Add(prizeInfo);
                    m_cupPrizeConfig[prizeInfo.nConfigID] = temp;
                }
            }
        }

        // 比赛类型脚本
        private void LoadLegendCupTypeConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupTypeConfig = new List<SSchemeLegendCupTypeConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupTypeConfig typeInfo = new SSchemeLegendCupTypeConfig();
                typeInfo.nCreateType = reader.GetInt(i, (int)ECupTypeCol.ECTC_COL_TYPE_CREATETYPE, 0);
                typeInfo.nIsShow = reader.GetInt(i, (int)ECupTypeCol.ECTC_COL_TYPE_ISSHOW, 0);

                string[] strFreeTypeIDList = reader.GetString(i, (int)ECupTypeCol.ECTC_COL_TYPE_FREECUPID, "").Split(';');
                if (strFreeTypeIDList != null && strFreeTypeIDList.Length > 0)
                {
                    typeInfo.nFreeCupConfigIDList = new int[strFreeTypeIDList.Length];
                    for (int j = 0; j < strFreeTypeIDList.Length; j++)
                    {
                        typeInfo.nFreeCupConfigIDList[j] = Convert.ToInt32(strFreeTypeIDList[j]);
                    }
                }

                string[] strChargeTypeIDList = reader.GetString(i, (int)ECupTypeCol.ECTC_COL_TYPE_CHARGECUPID, "").Split(';');
                if (strChargeTypeIDList != null && strChargeTypeIDList.Length > 0)
                {
                    typeInfo.nChargeCupconfigIDList = new int[strChargeTypeIDList.Length];
                    for (int j = 0; j < strChargeTypeIDList.Length; j++)
                    {
                        typeInfo.nChargeCupconfigIDList[j] = Convert.ToInt32(strChargeTypeIDList[j]);
                    }
                }

                m_cupTypeConfig.Add(typeInfo);
            }
        }

        // 比赛限制脚本
        private void LoadLegendCupLimitConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_cupLimitConfig = new Dictionary<int, SSchemeLegendCupLimitConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLegendCupLimitConfig limitInfo = new SSchemeLegendCupLimitConfig();
                limitInfo.nConfigID = reader.GetInt(i, (int)ECupLimitCol.ECLC_COL_LIMIT_CONFIGID, 0);
                limitInfo.nCountLimit = reader.GetInt(i, (int)ECupLimitCol.ECLC_COL_LIMIT_COUNT, 0);

                m_cupLimitConfig.Add(limitInfo.nConfigID, limitInfo);
            }
        }


        public SSchemeLegendCupConfig GetCupConfig(int nConfigID)
        {
            SSchemeLegendCupConfig result;
            if (m_cupConfig.TryGetValue(nConfigID, out result))
            {
                return result;
            }

            return null;
        }

        public SSchemeLegendCupDetailConfig GetCupDetailConfig(SCupTypeKey sCupTypeKey)
        {
            SSchemeLegendCupDetailConfig result;
            if (m_cupDetailConfig.TryGetValue(sCupTypeKey, out result))
            {
                return result;
            }

            return null;
        }

        public List<SSchemeLegendCupDetailConfig> GetCupDetailConfigList(int nConfigID)
        {
            List<SSchemeLegendCupDetailConfig> configList = new List<SSchemeLegendCupDetailConfig>();
            foreach (var item in m_cupDetailConfig)
            {
                if (item.Value.nCupConfigID == nConfigID)
                {
                    configList.Add(item.Value);
                }
            }

            return configList;
        }

        public SSchemeLegendCupTipConfig GetCupTipConfig(int nTipID)
        {
            SSchemeLegendCupTipConfig result;
            if (m_cupTipConfig.TryGetValue(nTipID, out result))
            {
                return result;
            }

            return null;
        }

        public List<SSchemeLegendCupPrizeConfig> GetCupPrizeConfig(int nConfigID)
        {
            List<SSchemeLegendCupPrizeConfig> result;
            if (m_cupPrizeConfig.TryGetValue(nConfigID, out result))
            {
                return result;
            }

            return null;
        }

        public List<SSchemeLegendCupTypeConfig> GetCupTypeConfigList()
        {
            return m_cupTypeConfig;    
        }

        public SSchemeLegendCupTypeConfig GetCupTypeConfig(int nCreateType)
        {
            foreach (SSchemeLegendCupTypeConfig item in m_cupTypeConfig)
            {
                if (item.nCreateType == nCreateType)
                {
                    return item;
                }
            }

            return null;
        }

        public SSchemeLegendCupLimitConfig GetCupLimitConfig(int nConfigID)
        {
            if (m_cupLimitConfig.ContainsKey(nConfigID))
            {
                return m_cupLimitConfig[nConfigID];
            }

            return null;
        }
    }
}
