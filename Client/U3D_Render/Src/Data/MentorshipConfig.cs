using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.MentorshipConfig
{
    // 与MentorshipConfig.csv表里面的列对应
    public enum MentorshipConfigRow : int
    {
        EMT_ROW_MENTORSHIP_MASTERNEEDLEVEL = 0,          // 师傅要求等级
        EMT_ROW_MENTORSHIP_PRENTICENEEDLEVEL,            // 学徒要求等级
        EMT_ROW_MENTORSHIP_FMASTERCOUNT,                 // 正式师傅个数
        EMT_ROW_MENTORSHIP_IMASTERCOUNT,                 // 见习师傅个数
        EMT_ROW_MENTORSHIP_FPRENTICECOUNT,               // 正式徒弟个数
        EMT_ROW_MENTORSHIP_IPRENTICECOUNT,               // 见习徒弟个数
        EMT_ROW_MENTORSHIP_PFWINAWARDCOUNT,              // 学徒首胜奖励次数
        EMT_ROW_MENTORSHIP_MFWINAWARDCOUNT = 8,          // 导师首胜奖励次数
        EMT_ROW_MENTORSHIP_SERVERTIMELEN = 10,           // 解散需要时长
        EMT_ROW_MENTORSHIP_WINSHIPVAL,                   // 胜利获取师徒值
        EMT_ROW_MENTORSHIP_FAILSHIPVAL,                  // 失败获取师徒值
        EMT_ROW_MENTORSHIP_WEEKSHIPVAL,                  // 周获取师徒最大值
        EMT_ROW_MENTORSHIP_MFWINPRIZEID = 17,            // 导师首胜奖励ID
        EMT_ROW_MENTORSHIP_PFWINPRIZEID,                 // 学徒首胜奖励ID
        EMT_ROW_MENTORSHIP_TIP = 19,                     //  帮会详细说明UBB格式
        EMT_ROW_MAX,                                     // 最大
    }

    /// <summary>
    /// MentorshipConfig配置类
    /// </summary>
    public class SSchemeMentorshipConfig
    {
        public int nMasterNeedLevel;					// 师傅要求等级
        public int nPrenticeNeedLevel;					// 学徒要求等级
        public int nFormalMasterCount;					// 正式师傅个数
        public int nInternMasterCount;					// 见习师傅个数
        public int nFormalPrenticeCount;			    // 正式徒弟个数
        public int nInternPrenticeCount;				// 见习徒弟个数
        public int nPFWinAwardCount;					// 学徒首胜奖励次数
        public int nMFWinAwardCount;					// 导师首胜奖励次数
        public int nServerTimeLen;						// 解散需要时长
        public int nWinShipVal;							// 胜利获取师徒值
        public int nFailShipVal;						// 失败获取师徒值
        public int nWeekShipVal;						// 周获取师徒最大值
        public int nMFWinPrizeID;                       // 导师首胜奖励ID
        public int nPFWinPrizeID;                       // 学徒首胜奖励ID
        public string sMentorshipTip;                   //  帮会详细说明UBB格式
    }

    class MentorshipConfig
    {
        public SSchemeMentorshipConfig tMentorshipConfig;
        private static MentorshipConfig singleton = new MentorshipConfig();

        public static MentorshipConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new MentorshipConfig();
            }
        }

        public void Load(string stPath)
        {
            InitMentorshipConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (tMentorshipConfig != null)
            {
                tMentorshipConfig = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitMentorshipConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            tMentorshipConfig = new SSchemeMentorshipConfig();

            for (int i = 0; i < (int)MentorshipConfigRow.EMT_ROW_MAX; i++)
            {
                switch (i)
                {
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_MASTERNEEDLEVEL: 
                        {
                            tMentorshipConfig.nMasterNeedLevel = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_PRENTICENEEDLEVEL: 
                        {
                            tMentorshipConfig.nPrenticeNeedLevel = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_FMASTERCOUNT: 
                        {
                            tMentorshipConfig.nFormalMasterCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_IMASTERCOUNT: 
                        {
                            tMentorshipConfig.nInternMasterCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_FPRENTICECOUNT: 
                        {
                            tMentorshipConfig.nFormalPrenticeCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_IPRENTICECOUNT: 
                        {
                            tMentorshipConfig.nInternPrenticeCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_PFWINAWARDCOUNT: 
                        {
                            tMentorshipConfig.nPFWinAwardCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_MFWINAWARDCOUNT: 
                        {
                            tMentorshipConfig.nMFWinAwardCount = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_SERVERTIMELEN: 
                        {
                            tMentorshipConfig.nServerTimeLen = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_WINSHIPVAL: 
                        {
                            tMentorshipConfig.nWinShipVal = reader.GetInt(i, 1, 0);
                        }
                        break;
                     case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_FAILSHIPVAL: 
                        {
                            tMentorshipConfig.nFailShipVal = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_WEEKSHIPVAL: 
                        {
                            tMentorshipConfig.nWeekShipVal = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_MFWINPRIZEID:
                        {
                            tMentorshipConfig.nMFWinPrizeID = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_PFWINPRIZEID:
                        {
                            tMentorshipConfig.nPFWinPrizeID = reader.GetInt(i, 1, 0);
                        }
                        break;
                    case (int)MentorshipConfigRow.EMT_ROW_MENTORSHIP_TIP:
                        {
                            tMentorshipConfig.sMentorshipTip = reader.GetString(i, 1, string.Empty);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
