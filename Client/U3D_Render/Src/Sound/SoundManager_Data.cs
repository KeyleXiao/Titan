/*******************************************************************
** 文件名:	SoundManager_Data.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音管理器的数据部分

**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/7/11
** 描  述: 增加声音类型区分，增加音量控制和静音设置
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using SoundSystem;

/// <summary>
/// 此枚举放置一些个别单独的、特殊的音效ID对应(后面把它们统一建配置表读取)
/// </summary>
public enum SoundIDIndex : int
{
    GameWaitingRoomReadyTimer = 431,
    GameWaitingRoomEnter = 1008,
    GameWaitingRoomHeroSelect = 1006,
    MapLogin = 1007,
}


public enum EMBGMRule : int
{
    SupportPlayer = 8,
}


public sealed partial class SoundManager
{
    public const string ConfigFileName = "SoundConfig.csv";

    /// <summary>
    /// 音效ID，音效配置
    /// </summary>
    private static Dictionary<int, SoundConfigData> m_DataTable = new Dictionary<int, SoundConfigData>();


    /// <summary>
    /// 加载声音配置
    /// </summary>
    private static bool LoadSoundConfig(ref Dictionary<int, SoundConfigData> dataSet)
    {
        if (dataSet == null)
        {
            SoundSystemLog("参数dataset为null");
            return false;
        }

        string patch = ViewConfigManager.GetConfigFullPath(ConfigFileName);

        ScpReader SoundReader = new ScpReader(patch, true, 2);

        // 遍歷整個表并存儲起來
        for (int i = 0; i < SoundReader.GetRecordCount(); ++i)
        {
            SoundConfigData data = new SoundConfigData();
            data.nID = SoundReader.GetInt(i, (int)SoundConfigCol.COL_ID, 0);
            data.soundType = (EMSoundType)SoundReader.GetInt(i, (int)SoundConfigCol.COL_Type, 2);
            data.mixerGroupType = (AudioMixerGroupType)SoundReader.GetInt(i, (int)SoundConfigCol.COL_MixerGroupType, 1);
            data.nMixerGroupIndex = SoundReader.GetInt(i, (int)SoundConfigCol.COL_MixerGroupIndex, 0);
            data.bBypassEffect = SoundReader.GetInt(i, (int)SoundConfigCol.COL_BypassEffect, 0) > 0;
            data.bBypassReverbZone = SoundReader.GetInt(i, (int)SoundConfigCol.COL_BypassReverbZone, 0) > 0;
            data.bPlayOnAwake = SoundReader.GetInt(i, (int)SoundConfigCol.COL_PlayOnAwake, 1) > 0;
            data.nPriority = Mathf.Clamp(SoundReader.GetInt(i, (int)SoundConfigCol.COL_Priority, 128), 0, 256);
            data.fPitch = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_Pitch, 1), -3.0f, 3.0f);
            data.fStereoPan = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_StereoPan, 0), -1.0f, 1.0f);
            data.fReverZoneMix = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_ReverZoneMix, 1), 0, 1.1f);
            data.b3DSound = SoundReader.GetInt(i, (int)SoundConfigCol.COL_3DSound, 1) > 0;
            data.fDopplerLv = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_DopplerLv, 1),0, 5.0f);
            data.nSpread = Mathf.Clamp(SoundReader.GetInt(i, (int)SoundConfigCol.COL_Spread, 0), 0, 360); ;
            data.audioRoffType = (AudioRolloffMode)SoundReader.GetInt(i, (int)SoundConfigCol.COL_RolloffMode, 0);
            data.fMinDistance = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_MinDistance, 1);
            data.fMaxDistance = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_MaxDistance, 15);
            data.nPlayTimes = SoundReader.GetInt(i, (int)SoundConfigCol.COL_PlayTimes, 1);
            data.fDuartion = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_Duartion, 0);
            data.fDelayPlayTime = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_DelayTime, 0);
            data.fIntervalBetweenPlayTimes = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_IntervalBetweenPlayTimes, 0);
            data.AssetBundleName = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetBundleName, "");
            data.AssetName = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetName, "");
            data.AssetGUID = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetGUID, "");
            data.AssetSize_X = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_AssetSize_X, -1);
            data.AssetSize_Y = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_AssetSize_Y, -1);
            data.szDependAssetGUID = SoundReader.GetString(i, (int)SoundConfigCol.COL_DependAssetGUID, "");
            if (dataSet.ContainsKey(data.nID))
            {
                SoundSystemLog(ConfigFileName + "拥有重复的ID！");
                dataSet[data.nID] = data;
            }
            else
            {
                CheckConfig(data);
                dataSet.Add(data.nID, data);
            }
            
        }
        SoundReader.Dispose();
        SoundReader = null;
        return true;
    }


    /// <summary>
    /// 从音效编辑器中加载配置
    /// </summary>
    /// <param name="editorData"></param>
    private static void LoadSoundConfigFromSoundSystemEditor(Dictionary<int, SoundConfigData> editorData)
    {
        m_DataTable.Clear();
        foreach (KeyValuePair<int, SoundConfigData> pai in editorData)
        {
            m_DataTable.Add(pai.Key, pai.Value);
        }
    }


    private static void CopyConfig(int id, out SoundConfigData cfgData)
    {
        cfgData = new SoundConfigData();

        if (m_DataTable.ContainsKey(id))
        {
            cfgData.AssetBundleName = m_DataTable[id].AssetBundleName;
            cfgData.AssetName = m_DataTable[id].AssetName;
            cfgData.AssetGUID = m_DataTable[id].AssetGUID;
            cfgData.AssetSize_X = m_DataTable[id].AssetSize_X;
            cfgData.AssetSize_Y = m_DataTable[id].AssetSize_Y;
            cfgData.fromReference = m_DataTable[id].fromReference;
            cfgData.priority = m_DataTable[id].priority;
            cfgData.nID = m_DataTable[id].nID;
            cfgData.soundType = m_DataTable[id].soundType;
            cfgData.bBypassEffect = m_DataTable[id].bBypassEffect;
            cfgData.bBypassReverbZone = m_DataTable[id].bBypassReverbZone;
            cfgData.bPlayOnAwake = m_DataTable[id].bPlayOnAwake;
            cfgData.nPriority = m_DataTable[id].nPriority;

            cfgData.fPitch = m_DataTable[id].fPitch;
            cfgData.fStereoPan = m_DataTable[id].fStereoPan;
            cfgData.b3DSound = m_DataTable[id].b3DSound;
            cfgData.fReverZoneMix = m_DataTable[id].fReverZoneMix;
            cfgData.fDopplerLv = m_DataTable[id].fDopplerLv;
            cfgData.nSpread = m_DataTable[id].nSpread;
            cfgData.audioRoffType = m_DataTable[id].audioRoffType;
            cfgData.fMinDistance = m_DataTable[id].fMinDistance;
            cfgData.mixerGroupType = m_DataTable[id].mixerGroupType;
            cfgData.fDuartion = m_DataTable[id].fDuartion;
            cfgData.fDelayPlayTime = m_DataTable[id].fDelayPlayTime;
            cfgData.fIntervalBetweenPlayTimes = m_DataTable[id].fIntervalBetweenPlayTimes;
        }
    }


    private static void UnLoadSoundConfig()
    {
        m_DataTable.Clear();
    }


    private static void CheckConfig(SoundConfigData cfgData)
    {
        if (cfgData == null)
            return;

        switch (cfgData.soundType)
        {
            case EMSoundType.Environment:
                {
                    cfgData.b3DSound = false;
                    cfgData.mixerGroupType = AudioMixerGroupType.BGM;
                }
                break;
            case EMSoundType.System:
                {
                    cfgData.b3DSound = false;
                    cfgData.mixerGroupType = AudioMixerGroupType.UI;
                }
                break;
            case EMSoundType.Skill:
                {
                    cfgData.mixerGroupType = AudioMixerGroupType.Normal;
                }
                break;
        }
    }
}