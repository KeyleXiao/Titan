
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using ASpeedGame.Data.PlayerSettingLocalConfig;

namespace ASpeedGame.Data.ChatSettingConfig
{
    //聊天设置配置表各列项
    public enum ChatSettingTableColumn : int
    {
        Index = 0, 
        Value = 1,
    }
    //聊天设置项目，与配置表中对应，顺序一般不要随意改动
    public enum ChatSettingIndex : int
    {
        DiyChannelMask = 0,                   //自定义频道
    }

    /// <summary>
    /// 聊天设置相关配置
    /// </summary>
    public class ChatSettingConfig
    {
        //当前聊天设置配置表
        private Dictionary<ChatSettingIndex, string> currentChatSettingTable = new Dictionary<ChatSettingIndex, string>();
        //默认聊天设置配置表
        private Dictionary<ChatSettingIndex, string> defaultChatSettingTable = new Dictionary<ChatSettingIndex, string>();

        private static ChatSettingConfig singleton = new ChatSettingConfig();
        public static ChatSettingConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new ChatSettingConfig();
            }
        }

        public void Load(string stDefaultPath)
        {
            //载入默认配置
            LoadDefaultConfig(stDefaultPath);
            //载入玩家本地配置并覆盖当前相关项目
            LoadLocalAndCoverConfig();
        }

        public void UnLoad()
        {
            SaveConfig();
            if (currentChatSettingTable != null)
            {
                currentChatSettingTable.Clear();
                currentChatSettingTable = null;
            }
            if (defaultChatSettingTable != null)
            {
                defaultChatSettingTable.Clear();
                defaultChatSettingTable = null;
            }
            singleton = null;
        }

        /// <summary>
        /// 聊天设置恢复至默认
        /// </summary>
        public void RestChatSetting()
        {
            currentChatSettingTable.Clear();
            foreach (KeyValuePair<ChatSettingIndex, string> temp in defaultChatSettingTable)
            {
                currentChatSettingTable.Add(temp.Key, temp.Value);
            }
        }

        public bool GetChatSettingBoolValue(ChatSettingIndex item)
        {
            bool bValue = true;
            if (currentChatSettingTable.ContainsKey(item))
                bValue = Int32.Parse(currentChatSettingTable[item]) == 1 ? true : false;
            return bValue;
        }

        public int GetChatSettingIntValue(ChatSettingIndex item)
        {
            int nValue = 0;
            if (currentChatSettingTable.ContainsKey(item))
                nValue = Int32.Parse(currentChatSettingTable[item]);
            return nValue;
        }

        public string GetChatSettingStringValue(ChatSettingIndex item)
        {
            string stValue = "";
            if (currentChatSettingTable.ContainsKey(item))
                stValue = currentChatSettingTable[item];
            return stValue;
        }

        /// <summary>
        /// 修改聊天设置
        /// </summary>
        /// <param name="item"></param>
        /// <param name="newValue"></param>
        public void ModifyChatSetting(ChatSettingIndex item, string newValue)
        {
            if (currentChatSettingTable.ContainsKey(item))
                currentChatSettingTable[item] = newValue;
        }

        private void LoadDefaultConfig(string stPath)
        {
            //走mpk包暂不判断文件
            //if (!File.Exists(stPath))
            //{
            //    Trace.Error("配置表：ChatSettingConfig.csv 不存在！");
            //    return;
            //}

            ScpReader reader = new ScpReader(stPath, true, 2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int index = reader.GetInt(i, (int)ChatSettingTableColumn.Index, 0);
                string value = reader.GetString(i, (int)ChatSettingTableColumn.Value, "");
                if (defaultChatSettingTable.ContainsKey((ChatSettingIndex)index))
                    defaultChatSettingTable[(ChatSettingIndex)index] = value;
                else
                    defaultChatSettingTable.Add((ChatSettingIndex)index, value);
            }

            foreach (KeyValuePair<ChatSettingIndex, string> kvp in defaultChatSettingTable)
                currentChatSettingTable.Add(kvp.Key, kvp.Value);
        }

        private void LoadLocalAndCoverConfig()
        {
            List<PlayerSettingData> tempList;
            if(PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.GetLocalData(PlayerSettingDataType.ChatSetting, out tempList))
            {
                foreach(PlayerSettingData data in tempList)
                {
                    System.Object resultObj = null;
                    if(StringHelper.StringToEnum(typeof(ChatSettingIndex), data.m_DataName, out resultObj))
                    {
						ChatSettingIndex index = (ChatSettingIndex)resultObj;
                        if (currentChatSettingTable.ContainsKey(index))
                            currentChatSettingTable[index] = data.m_DataValueList[0];
                    }
                }
            }
        }

        private void SaveConfig()
        {
            List<PlayerSettingData> dataList = new List<PlayerSettingData>();
            foreach(KeyValuePair<ChatSettingIndex, string> kvp in currentChatSettingTable)
            {
                PlayerSettingData data = new PlayerSettingData(PlayerSettingDataType.ChatSetting, kvp.Key.ToString(), kvp.Value);
                dataList.Add(data);
            }
            if (dataList.Count > 0)
                PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.ModifyLocalData(PlayerSettingDataType.ChatSetting, dataList);
        }
    }
}
