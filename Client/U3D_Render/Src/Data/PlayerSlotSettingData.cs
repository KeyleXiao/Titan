using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using ASpeedGame.Data.SlotKeyCodeConfig;

namespace ASpeedGame.Data.PlayerSettingLocalConfig
{
    /// <summary>
    /// 玩家设置Slot数据类型
    /// </summary>

    public class PlayerSlotItem
    {
        public int slotID;
        public int useType;
        public PlayerSlotItem(int id,int type)
        {
            slotID = id;
            useType = type;
        }
    }

    /// <summary>
    /// 玩家设置本地Slot配置
    /// </summary>
    public class PlayerSlotSettingConfig
    {
        private Dictionary<int, List<PlayerSlotItem>> m_LocalDataMap = new Dictionary<int, List<PlayerSlotItem>>();
        //配置表路径
        private string m_FilePath;
        private static PlayerSlotSettingConfig singleton = new PlayerSlotSettingConfig();

        public static int UNUSEABLE_INDEX = 10000;  //预留位，先不处理
        public static int[] indexList_DefaultUseType = new int[] {2013,2016,2018 }; //对应召唤师技能（F），天赋技能（1,2）

        public static PlayerSlotSettingConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PlayerSlotSettingConfig();
            }
        }

        public void Load(string stPath)
        {
            m_FilePath = stPath;

            if (null == m_LocalDataMap)
            {
                m_LocalDataMap = new Dictionary<int, List<PlayerSlotItem>>();
            }
            InitPlayerSlotSettingConfigCsv(stPath);
        }

        public void UnLoad()
        {
            ExportData();
            foreach (List<PlayerSlotItem> dataList in m_LocalDataMap.Values)
                dataList.Clear();

            m_LocalDataMap.Clear();
            m_LocalDataMap = null;
        }

        public bool GetLocalData(int heroID, out List<PlayerSlotItem> dataList)
        {
            if (m_LocalDataMap.TryGetValue(heroID, out dataList) && dataList.Count > 0)
                return true;
            else
            {
                dataList = null;
                return false;
            }
        }

        public void ModifyLocalData(int heroID, List<PlayerSlotItem> dataList)
        {
            if (m_LocalDataMap.ContainsKey(heroID))
                m_LocalDataMap[heroID] = dataList;
            else
            {
                m_LocalDataMap.Add(heroID, dataList);
            }
        }

        private void InitPlayerSlotSettingConfigCsv(string stPath)
        {
            if (!File.Exists(stPath))
                return;

            ScpReader reader = new ScpReader(stPath, false, 0);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                //读取表中记录的数据类型(PlayerSettingDataType)
                string result = reader.GetString(i, 0, string.Empty);
                int heroID = -1;
                if (int.TryParse(result, out heroID))
                {
                    bool parseResult = true;
                    List<PlayerSlotItem> slotDataList;
                    if (!m_LocalDataMap.TryGetValue(heroID, out slotDataList))
                    {
                        //如果不存在该英雄ID
                        slotDataList = new List<PlayerSlotItem>();
                        m_LocalDataMap.Add(heroID, slotDataList);
                    }
                    string slotNumStr = reader.GetString(i, 1, string.Empty);
                    int slotNum = 0;
                    parseResult = int.TryParse(slotNumStr, out slotNum);
                    for (int n = 0; n < slotNum; n++)
                    {
                        string value = reader.GetString(i, n + 2, string.Empty);
                        string[] datas = value.Split('_');
                        int slotID = -1;
                        int slotUseType = -1;
                        parseResult = int.TryParse(datas[0], out slotID);
                        parseResult = int.TryParse(datas[1], out slotUseType);
                        PlayerSlotItem item = new PlayerSlotItem(slotID, slotUseType);
                        slotDataList.Add(item);
                        if (!parseResult)
                            Debug.LogError("PlayerSlotSettingConfig 配置错误");
                    }
                }
            }
        }

        private void ExportData()
        {
            StreamWriter writer = new StreamWriter(m_FilePath, false, new System.Text.UTF8Encoding(true));
            string s = "";
            foreach (KeyValuePair<int, List<PlayerSlotItem>> kvp in m_LocalDataMap)
            {
                s = kvp.Key.ToString() + "," + kvp.Value.Count;
                foreach (PlayerSlotItem item in kvp.Value)
                {
                    s += ("," + item.slotID+"_"+item.useType);
                }
                writer.WriteLine(s);
            }
            writer.Flush();
            writer.Close();
            writer = null;
        }
    }
}
