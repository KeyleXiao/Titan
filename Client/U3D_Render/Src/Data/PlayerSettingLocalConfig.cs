using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

namespace ASpeedGame.Data.PlayerSettingLocalConfig
{
    /// <summary>
    /// 玩家设置数据类型
    /// </summary>
    public enum PlayerSettingDataType
    {
        ImageSetting,         //图像设置
        GameSetting,          //游戏设置
        shortcutsSetting,     //快捷键设置
		ChatSetting,		  //聊天设置
    }

    public class PlayerSettingData
    {
        public PlayerSettingDataType m_DataType;       
        public string m_DataName;
        public List<string> m_DataValueList = new List<string>();

        public PlayerSettingData(PlayerSettingDataType type, string name, params string[] valueList)
        {
            this.m_DataType = type;
            this.m_DataName = name;
            foreach (string s in valueList)
                this.m_DataValueList.Add(s);
        }
    }

    /// <summary>
    /// 玩家设置本地配置
    /// </summary>
    public class PlayerSettingLocalConfig
    {
        private Dictionary<PlayerSettingDataType, List<PlayerSettingData>> m_LocalDataMap = new Dictionary<PlayerSettingDataType, List<PlayerSettingData>>();
        //配置表路径
        private string m_FilePath = "";
        private static PlayerSettingLocalConfig singleton = new PlayerSettingLocalConfig();
        public static PlayerSettingLocalConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PlayerSettingLocalConfig();
            }
        }

        public void Load(string stPath)
        {
            m_FilePath = stPath;

            if(null == m_LocalDataMap)
            {
                m_LocalDataMap = new Dictionary<PlayerSettingDataType, List<PlayerSettingData>>(); 
            }
            m_LocalDataMap.Add(PlayerSettingDataType.ImageSetting, new List<PlayerSettingData>());
            m_LocalDataMap.Add(PlayerSettingDataType.GameSetting, new List<PlayerSettingData>());
            m_LocalDataMap.Add(PlayerSettingDataType.shortcutsSetting, new List<PlayerSettingData>());
            m_LocalDataMap.Add(PlayerSettingDataType.ChatSetting, new List<PlayerSettingData>());
            InitPlayerSettingLocalConfigCsv(stPath);
        }

        public void UnLoad()
        {
            ExportData();
            foreach (List<PlayerSettingData> dataList in m_LocalDataMap.Values)
                dataList.Clear();

            m_LocalDataMap.Clear();
            m_LocalDataMap = null;
        }

        public bool GetLocalData(PlayerSettingDataType dataType, out List<PlayerSettingData> dataList)
        {
            if (m_LocalDataMap.TryGetValue(dataType, out dataList) && dataList.Count > 0)
                return true;
            else
            {
                dataList = null;
                return false;
            }
        }

        public void ModifyLocalData(PlayerSettingDataType dataType, List<PlayerSettingData> dataList)
        {
            if (m_LocalDataMap.ContainsKey(dataType))
                m_LocalDataMap[dataType] = dataList;
        }

        private void InitPlayerSettingLocalConfigCsv(string stPath)
        {
            if (!File.Exists(stPath))
                return;

            ScpReader reader = new ScpReader(stPath,false, 0);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                //读取表中记录的数据类型(PlayerSettingDataType)
                string result = reader.GetString(i, 0, string.Empty);
                System.Object resultObj = null;
                if (StringHelper.StringToEnum(typeof(PlayerSettingDataType), result, out resultObj))
                {
                    PlayerSettingDataType dataType = (PlayerSettingDataType)resultObj;
                    List<PlayerSettingData> dataList;
                    if (!m_LocalDataMap.TryGetValue(dataType, out dataList))
                    {
                        //如果不存在本设置类型，则跳过
                        continue;
                    }
                    //由于不同设置类型对应的表中数据列数不一致，则分支处理
                    switch (dataType)
                    {
                        case PlayerSettingDataType.ImageSetting:
                            {
                                string name = reader.GetString(i, 1, string.Empty);
                                string value = reader.GetString(i, 2, string.Empty);
                                PlayerSettingData data = new PlayerSettingData(dataType, name, value);
                                dataList.Add(data);
                            }
                            break;
                        case PlayerSettingDataType.GameSetting:
                            {
                                string name = reader.GetString(i, 1, string.Empty);
                                string value = reader.GetString(i, 2, string.Empty);
                                PlayerSettingData data = new PlayerSettingData(dataType, name, value);
                                dataList.Add(data);
                            }
                            break;
                        case PlayerSettingDataType.shortcutsSetting:
                            {
                                string name = reader.GetString(i, 1, string.Empty);
                                string index = reader.GetString(i, 2, string.Empty);
                                string code = reader.GetString(i, 3, string.Empty);
                                string spellType = reader.GetString(i, 4, string.Empty);
                                PlayerSettingData data = new PlayerSettingData(dataType, name, index, code, spellType);
                                dataList.Add(data);
                            }
                            break;
						case PlayerSettingDataType.ChatSetting:
							{
                                string name = reader.GetString(i, 1, string.Empty);
                                string value = reader.GetString(i, 2, string.Empty);
                                PlayerSettingData data = new PlayerSettingData(dataType, name, value);
                                dataList.Add(data);
							}
							break;
                        default:
                            break;
                    }
                }
            }
        }

        private void ExportData()
        {
            if (m_FilePath == "")
                return;

            StreamWriter writer = new StreamWriter(m_FilePath, false, new System.Text.UTF8Encoding(true));
            string s = "";
            foreach(KeyValuePair<PlayerSettingDataType, List<PlayerSettingData>> kvp in m_LocalDataMap)
            {
                foreach (PlayerSettingData data in kvp.Value)
                {
                    s = data.m_DataType.ToString() + "," + data.m_DataName;
                    foreach (string st in data.m_DataValueList)
                    {
                        s += ("," + st);
                    }
                    writer.WriteLine(s);
                }
            }
            writer.Flush();
            writer.Close();
            writer = null;
        }
    }
}
