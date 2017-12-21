using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using ASpeedGame.Data.PlayerSettingLocalConfig;

namespace ASpeedGame.Data.GameSettingConfig
{
    //游戏设置配置表各列项
    public enum GameSettingTableColumn : int
    {
        Index = 0, 
        Value = 1,
    }
    //游戏设置项目，与配置表中对应，顺序一般不要随意改动
    public enum GameSettingIndex : int
    {
        IPAddress = 0,                   //IP地址
        UserName,                        //登陆用户名
        MouseLockInWindow,               //鼠标锁定在窗口之内
        EnableMultiThread,               //开启多线程
        MuteAudio,                       //禁音
        AutoSpellCasting,                //智能施法
        ShowBloodNum,                    //显示血量数值
        ShowSelfBlood,                   //显示自身血条
        ShowFriendSoldierBlood,        //显示友方小兵血条
        ShowRoleName,                    //显示角色名
        ShowKinName,                     //显示战队名
        ShowClanName,                    //显示帮会名
        MainVolume,                      //主音量
        BGMVolume,                       //BGM音量
        SkillSoundVolume,                //技能音量
        SystemSoundVolume,               //系统音量
        SpeechSoundVolume,               //语音音量
        OpenScrenRec,                    //视频录制
        CameraSpeed,                     //相机移动速度
        MouseSensitivity,                //鼠标灵敏度
    }

    /// <summary>
    /// 游戏设置相关配置
    /// </summary>
    public class GameSettingConfig
    {
        //当前游戏设置配置表
        private Dictionary<GameSettingIndex,string> currentGameSettingTable = new Dictionary<GameSettingIndex,string>();
        //默认游戏设置配置表
        private Dictionary<GameSettingIndex, string> defaultGameSettingTable = new Dictionary<GameSettingIndex,string>();

        private static GameSettingConfig singleton = new GameSettingConfig();
        public static GameSettingConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new GameSettingConfig();
            }
        }

        public void Load(string stDefaultPath)
        {
            //载入默认配置
            LoadDefaultConfig(stDefaultPath);
            //载入设备相关配置数据
            LoadDefaultDeviceConfig();
            //载入玩家本地配置并覆盖当前相关项目
            LoadLocalAndCoverConfig();

            Initialize.onApplicationFocusChangeEvent.RemoveListener(OnGameFoucusChange);
            Initialize.onApplicationFocusChangeEvent.AddListener(OnGameFoucusChange);
        }

        public void UnLoad()
        {
            Initialize.onApplicationFocusChangeEvent.RemoveListener(OnGameFoucusChange);

            SaveConfig();
            RecoverDeviceConfig();

            if (currentGameSettingTable != null)
            {
                currentGameSettingTable.Clear();
                currentGameSettingTable = null;
            }
            if (defaultGameSettingTable != null)
            {
                defaultGameSettingTable.Clear();
                defaultGameSettingTable = null;
            }
            singleton = null;
        }

        /// <summary>
        /// 游戏设置恢复至默认
        /// </summary>
        public void RestGameSetting()
        {
            currentGameSettingTable.Clear();
            foreach (KeyValuePair<GameSettingIndex, string> temp in defaultGameSettingTable)
            {
                currentGameSettingTable.Add(temp.Key, temp.Value);
            }

            RecoverDeviceConfig();
        }

        public bool GetGameSettingBoolValue(GameSettingIndex item)
        {
            int nValue = 0;
            if (currentGameSettingTable.ContainsKey(item))
            {
                int.TryParse(currentGameSettingTable[item], out nValue);
            }
            return nValue == 1 ? true : false;
        }

        public int GetGameSettingIntValue(GameSettingIndex item)
        {
            int nValue = 0;
            if (currentGameSettingTable.ContainsKey(item))
            {
                int.TryParse(currentGameSettingTable[item], out nValue);
            }
            return nValue;
        }

        public string GetGameSettingStringValue(GameSettingIndex item)
        {
            string stValue = "";
            if (currentGameSettingTable.ContainsKey(item))
                stValue = currentGameSettingTable[item];
            return stValue;
        }

        public float GetGameSettingFloatValue(GameSettingIndex item)
        {
            float fValue = 0;
            if (currentGameSettingTable.ContainsKey(item))
            {
                float.TryParse(currentGameSettingTable[item], out fValue);
            }
            return fValue;
        }

        public float GetGameSettingDefaultFloatValue(GameSettingIndex item)
        {
            float fValue = 0;
            if (defaultGameSettingTable.ContainsKey(item))
            {
                float.TryParse(defaultGameSettingTable[item], out fValue);
            }
            return fValue;
        }

        /// <summary>
        /// 修改游戏设置
        /// </summary>
        /// <param name="item"></param>
        /// <param name="newValue"></param>
        public void ModifyGameSetting(GameSettingIndex item, string newValue)
        {
            if (currentGameSettingTable.ContainsKey(item))
                currentGameSettingTable[item] = newValue;
        }

        private void LoadDefaultConfig(string stPath)
        {
            //走mpk包暂不判断文件
            //if (!File.Exists(stPath))
            //{
            //    Trace.Error("配置表：GameSettingConfig.csv 不存在！");
            //    return;
            //}

            ScpReader reader = new ScpReader(stPath, true, 2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int index = reader.GetInt(i, (int)GameSettingTableColumn.Index, 0);
                string value = reader.GetString(i, (int)GameSettingTableColumn.Value, "");
                if (defaultGameSettingTable.ContainsKey((GameSettingIndex)index))
                    defaultGameSettingTable[(GameSettingIndex)index] = value;
                else
                    defaultGameSettingTable.Add((GameSettingIndex)index, value);
            }

            foreach (KeyValuePair<GameSettingIndex, string> kvp in defaultGameSettingTable)
                currentGameSettingTable.Add(kvp.Key, kvp.Value);
        }

        private void LoadLocalAndCoverConfig()
        {
            List<PlayerSettingData> tempList;
            if(PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.GetLocalData(PlayerSettingDataType.GameSetting, out tempList))
            {
                foreach(PlayerSettingData data in tempList)
                {
                    System.Object resultObj = null;
                    if(StringHelper.StringToEnum(typeof(GameSettingIndex), data.m_DataName, out resultObj))
                    {
                        GameSettingIndex index = (GameSettingIndex)resultObj;
                        if (currentGameSettingTable.ContainsKey(index))
                            currentGameSettingTable[index] = data.m_DataValueList[0];
                    }
                }
            }
        }

        private void SaveConfig()
        {
            List<PlayerSettingData> dataList = new List<PlayerSettingData>();
            foreach(KeyValuePair<GameSettingIndex, string> kvp in currentGameSettingTable)
            {
                PlayerSettingData data = new PlayerSettingData(PlayerSettingDataType.GameSetting, kvp.Key.ToString(), kvp.Value);
                dataList.Add(data);
            }
            if (dataList.Count > 0)
                PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.ModifyLocalData(PlayerSettingDataType.GameSetting, dataList);
        }

        private void LoadDefaultDeviceConfig()
        {
            uint defaultMouseSpeed = 10;
            IntPtr defaultPtr = new IntPtr();
            try
            {
                if (GameLogicAPI.SystemParametersInfo((uint)ESystemParameters.SPI_GETMOUSESPEED, 0, out defaultPtr, 0))
                {
                    defaultMouseSpeed = (uint)defaultPtr;
                }
            }
            catch
            {
                Trace.LogWarning("Mouse Sensitivity Get failed!");
            }
            defaultGameSettingTable.Add(GameSettingIndex.MouseSensitivity, defaultMouseSpeed.ToString());
            currentGameSettingTable.Add(GameSettingIndex.MouseSensitivity, defaultMouseSpeed.ToString());
        }

        private void RecoverDeviceConfig()
        {
            string stValue;
            if (defaultGameSettingTable.TryGetValue(GameSettingIndex.MouseSensitivity, out stValue))
            {
                uint nValue = 10;
                if (uint.TryParse(stValue, out nValue))
                {
                    if (nValue < 1 || nValue > 20)
                        nValue = 10;
                }

                IntPtr defaultPtr = new IntPtr(nValue);
                try
                {
                    GameLogicAPI.SystemParametersInfo((uint)ESystemParameters.SPI_SETMOUSESPEED, 0, defaultPtr, 0);
                }
                catch
                {
                    Trace.LogWarning("Mouse Sensitivity Set failed!");
                }
            }
        }

        private void OnGameFoucusChange(bool bFocus)
        {
            if (bFocus)
            {
                string szValue;
                if(currentGameSettingTable.TryGetValue(GameSettingIndex.MouseSensitivity, out szValue))
                {
                    int nValue;
                    if(int.TryParse(szValue, out nValue))
                    {
                        //鼠标灵敏度范围1-20
                        if (nValue < 1 || nValue > 20)
                            nValue = 10;

                        try
                        {
                            IntPtr valuePtr = new IntPtr(nValue);
                            GameLogicAPI.SystemParametersInfo((uint)ESystemParameters.SPI_SETMOUSESPEED, 0, valuePtr, 0);
                        }
                        catch
                        {
                            Trace.LogWarning("Mouse Sensitivity Set failed!");
                        }
                    }
                }
            }
            else
            {
                string szValue;
                if (defaultGameSettingTable.TryGetValue(GameSettingIndex.MouseSensitivity, out szValue))
                {
                    int nValue;
                    if (int.TryParse(szValue, out nValue))
                    {
                        if (nValue < 1 || nValue > 20)
                            nValue = 10;

                        try
                        {
                            IntPtr valuePtr = new IntPtr(nValue);
                            GameLogicAPI.SystemParametersInfo((uint)ESystemParameters.SPI_SETMOUSESPEED, 0, valuePtr, 0);
                        }
                        catch
                        {
                            Trace.LogWarning("Mouse Sensitivity Set failed!");
                        }
                    }
                }
            }
        }
    }
}
