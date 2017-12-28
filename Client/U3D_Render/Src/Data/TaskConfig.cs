using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.TaskConfig
{
    // 任务配置（交互任务专属部分）
    public enum ETaskConfigCol
    {
        ETCC_TASK_ID = 0,           // 任务ID

        ETCC_TASK_TYPE = 3,         // 任务类型（1首胜任务2帮会任务3循环任务4主线5交互6日常7排位）

        ETCC_SUBTYPE_ID = 33,       // 子类（1炫耀、2互助、3交友、4心情、5一起玩）
        ETCC_SUBTYPE_NAME,          // 子类名称
        ETCC_SPECIFICTYPE_ID,       // 具体类型
        ETCC_SPECIFICTYPE_NAME,     // 具体类型名称
        ETCC_PUBLISH_DESCRIPTION,   // 发布说明
        ETCC_EMOTION_WORDS,         // 心情话语
        ECTT_TASK_LEVEL,            // 任务星级
    }

    public class SSchemeContactTaskConfig
    {
        public int nSubType;
        public string strSubTypeName;
        public int nSpecificType;
        public string strSpecificTypeName;
        public string strPublishDescription;
        public string strEmotionWords;
        public int nTaskLevel;
    }

    public class SSchemeSystemTaskConfig
    {
        public int nTaskID;
        public int nTaskType;
        public int nTaskLevel;
    }

    public class TaskConfig
    {
        private static TaskConfig m_singleton = new TaskConfig();
        public static TaskConfig Instance { get { return m_singleton; } }

        private Dictionary<int, List<SSchemeContactTaskConfig>> m_mapContactTaskConfig;
        public Dictionary<int, List<SSchemeContactTaskConfig>> MapContactTaskConfig { get { return m_mapContactTaskConfig; } }

        private Dictionary<int, SSchemeSystemTaskConfig> m_mapSystemTaskConfig;
        public Dictionary<int, SSchemeSystemTaskConfig> MapSystemTaskConfig { get { return m_mapSystemTaskConfig; } }

        public void Load(string strPath)
        {
            LoadTaskConfig(strPath + "Task.csv");
        }

        public void Unload()
        {
            m_mapContactTaskConfig.Clear();
            m_mapContactTaskConfig = null;
            m_mapSystemTaskConfig.Clear();
            m_mapSystemTaskConfig = null;
        }

        // 任务脚本
        private void LoadTaskConfig(string strPath)
        {
            ScpReader reader = new ScpReader(strPath, true, 2);

            m_mapContactTaskConfig = new Dictionary<int, List<SSchemeContactTaskConfig>>();
            m_mapSystemTaskConfig = new Dictionary<int, SSchemeSystemTaskConfig>();

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nType = reader.GetInt(i, (int)ETaskConfigCol.ETCC_TASK_TYPE, 0);
                if (nType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                {
                    int nSubType = reader.GetInt(i, (int)ETaskConfigCol.ETCC_SUBTYPE_ID, 0);
                    if (!m_mapContactTaskConfig.ContainsKey(nSubType))
                    {
                        List<SSchemeContactTaskConfig> listTask = new List<SSchemeContactTaskConfig>();
                        m_mapContactTaskConfig[nSubType] = listTask;
                    }
                    // 已有nSpecificType数据
                    int nSpecificType = reader.GetInt(i, (int)ETaskConfigCol.ETCC_SPECIFICTYPE_ID, 0);
                    int nIndex = m_mapContactTaskConfig[nSubType].FindIndex(item => item.nSpecificType == nSpecificType);
                    if (nIndex >= 0)
                        continue;

                    SSchemeContactTaskConfig taskInfo = new SSchemeContactTaskConfig();
                    taskInfo.nSubType = reader.GetInt(i, (int)ETaskConfigCol.ETCC_SUBTYPE_ID, 0);
                    taskInfo.strSubTypeName = reader.GetString(i, (int)ETaskConfigCol.ETCC_SUBTYPE_NAME, "");
                    taskInfo.nSpecificType = reader.GetInt(i, (int)ETaskConfigCol.ETCC_SPECIFICTYPE_ID, 0);
                    taskInfo.strSpecificTypeName = reader.GetString(i, (int)ETaskConfigCol.ETCC_SPECIFICTYPE_NAME, "");
                    taskInfo.strPublishDescription = reader.GetString(i, (int)ETaskConfigCol.ETCC_PUBLISH_DESCRIPTION, "");
                    taskInfo.strEmotionWords = reader.GetString(i, (int)ETaskConfigCol.ETCC_EMOTION_WORDS, "");
                    taskInfo.nTaskLevel = reader.GetInt(i, (int)ETaskConfigCol.ECTT_TASK_LEVEL, 0);
                    m_mapContactTaskConfig[nSubType].Add(taskInfo);
                }
                else
                {
                    SSchemeSystemTaskConfig systemTaskInfo = new SSchemeSystemTaskConfig();
                    systemTaskInfo.nTaskID = reader.GetInt(i, (int)ETaskConfigCol.ETCC_TASK_ID, 0);
                    systemTaskInfo.nTaskType = nType;
                    systemTaskInfo.nTaskLevel = reader.GetInt(i, (int)ETaskConfigCol.ECTT_TASK_LEVEL, 0);
                    m_mapSystemTaskConfig[systemTaskInfo.nTaskID] = systemTaskInfo;
                }
            }
        }

        public List<SSchemeContactTaskConfig> GetSubTypeData(int nSubType)
        {
            if (m_mapContactTaskConfig.ContainsKey(nSubType))
            {
                return m_mapContactTaskConfig[nSubType];
            }

            return null;
        }

        public int GetTaskLevel(int nTaskID, int nTaskType, int nSubType, int nSpecificType)
        {
            int nLevel = 0;
            if (nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
            {
                List<SSchemeContactTaskConfig> config = null;
                if (m_mapContactTaskConfig.TryGetValue(nSubType, out config))
                {
                    foreach (var item in config)
                    {
                        if (item.nSpecificType == nSpecificType)
                        {
                            nLevel = item.nTaskLevel;
                            break;
                        }
                    }
                }
            }
            else
            {
                SSchemeSystemTaskConfig sysConfig = null;
                if (m_mapSystemTaskConfig.TryGetValue(nTaskID, out sysConfig))
                {
                    nLevel = sysConfig.nTaskLevel;
                } 
            }

            return nLevel;
        }

        public SSchemeSystemTaskConfig GetSystemTaskConfig(int nTaskID)
        {
            if (m_mapSystemTaskConfig.ContainsKey(nTaskID))
            {
                return m_mapSystemTaskConfig[nTaskID];
            }

            return null;
        }

    }
}
