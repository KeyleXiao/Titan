using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ASpeedGame.Data.TaskViewTimeConfig
{
    public enum ETaskViewTimeConfigCol
    {
        ETVT_ID,        // 交互ID
        ETVT_TIME,      // 查看时间
    }

    public class TaskViewTimeConfig : Singleton<TaskViewTimeConfig>
    {
        private Dictionary<long, int> m_mapTaskViewTime;

        // 配置表路径
        private string m_FilePath;

        public void Load(string _strPath)
        {
            m_FilePath = ViewConfigManager.GetAccountConfigFullPath("TaskViewTimeConfig.csv");
            m_mapTaskViewTime = new Dictionary<long, int>();

            LoadTaskViewTimeConfig(m_FilePath);
        }

        public void Unload()
        {
            ExportData();

            if (m_mapTaskViewTime == null)
                return;
            m_mapTaskViewTime.Clear();
            m_mapTaskViewTime = null;
        }

        private void LoadTaskViewTimeConfig(string _strPath)
        {
            if (!File.Exists(_strPath))
                return;

            ScpReader reader = new ScpReader(_strPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                long llContactID = reader.GetLong(i, (int)ETaskViewTimeConfigCol.ETVT_ID, 0);
                int nViewTime = reader.GetInt(i, (int)ETaskViewTimeConfigCol.ETVT_TIME, 0);

                m_mapTaskViewTime[llContactID] = nViewTime;
            }
        }

        private void ExportData()
        {
            if (String.IsNullOrEmpty(m_FilePath))
                return;

            StreamWriter writer = new StreamWriter(m_FilePath, false, new System.Text.UTF8Encoding(true));
            string s = "";

            s = "交互ID" + "," + "查看时间";
            writer.WriteLine(s);

            s = "long" + "," + "int";
            writer.WriteLine(s);

            foreach (var item in m_mapTaskViewTime)
            {
                s = item.Key + "," + item.Value;
                writer.WriteLine(s);
            }

            writer.Flush();
            writer.Close();
            writer = null;
        }

        public int GetTaskViewTime(long _llContactID)
        {
            if (m_mapTaskViewTime.ContainsKey(_llContactID))
            {
                return m_mapTaskViewTime[_llContactID];
            }

            return 0;
        }

        public void SetTaskViewTime(long _llContactID, int _nViewTime)
        {
            m_mapTaskViewTime[_llContactID] = _nViewTime;
        }

    }
}
