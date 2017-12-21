/*******************************************************************
** 文件名:	SoundManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音管理器

**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/7/11
** 描  述: 增加声音类型区分，增加音量控制和静音设置
********************************************************************/
using UnityEngine;
using UnityEngine.Audio;
using System.Collections;
using System.Collections.Generic;
using System;
using SoundSystem;
using ASpeedGame.Data.GameSettingConfig;


//混音组类型
public enum AudioMixerGroupType : int
{
    Normal,
    Actor,
    UI,
    BGM,
}

//混音器快照类型
public enum AudioMixerSnapshotType : int
{
    Snapshot_Default,
    Snapshot_Mute,
    Snapshot_2DAlone,
}

public class SoundDeleteEventArgs : EventArgs
{
    public int soundID;
    public int entityID;
    public EMSoundType soundType;

    public SoundDeleteEventArgs(int _soundID, int _entityID, EMSoundType _eType)
    {
        soundID = _soundID;
        entityID = _entityID;
        soundType = _eType;
    }
}

public sealed partial class SoundManager
{
    public const int UI_SOUND_BEGIN_ID = 0;
    public const int AudioMixer_Actor_PT = 2;              //普攻混音索引
    public const int MAX_MIXERGROUP_CHILDREN = 100;        //混音组子节点最大数目

    public const string MasterMixerPath = "AudioMixer/MasterMixer";   //总线路径
    public const string AudioMixer_PT_Pitch = "PT_Pitch";             //混音器导出参数_Pitch
    public const string AudioMixer_PT_Volume = "PT_Volume";           //混音器导出参数_Volume

    private static GameObject SoundParentRoot = null;
    private static GameObject SoundCacheParentRoot = null;

    private static Queue<Sound> m_CacheList = new Queue<Sound>();
    private static Dictionary<int, HashSet<Sound>> m_ActiveTable = new Dictionary<int, HashSet<Sound>>();

    #region 音量及禁音控制
    private static bool bForbid3DSound = false;
    public static bool FinalMute
    {
        get { return bAudioMute || !bGameFocus; }
    }
    private static bool bAudioMute = false;   //玩家是否主动禁音游戏
    private static bool bGameFocus = true;   //游戏是否聚焦窗口

    private static float mainVolume = 1.0f;

    private static Dictionary<EMSoundType, float> m_VolumeTable = new Dictionary<EMSoundType, float>();
    private static Dictionary<EMSoundType, float> m_CorrectTable = new Dictionary<EMSoundType, float>();
    #endregion

    #region 通道控制
    //混音器
    public static AudioMixer audioMixer;
    //当前混音快照类型
    public static AudioMixerSnapshotType m_snapshotType = AudioMixerSnapshotType.Snapshot_Default;
    //混音组列表
    private static Dictionary<int, AudioMixerGroup> m_MixerGroupTable = new Dictionary<int, AudioMixerGroup>();
    //混音快照列表
    private static Dictionary<AudioMixerSnapshotType, AudioMixerSnapshot> m_MixerSnapshotTable = new Dictionary<AudioMixerSnapshotType, AudioMixerSnapshot>();
    #endregion

    private static bool bInit = false;

    public static int getSoundCount()
    {
        int count = 0;

        if (SoundParentRoot != null)
        {
            count = SoundParentRoot.transform.childCount;
        }

        return count;
    }


    #region InitAndClear
    /// <summary>
    /// 初始化
    /// </summary>
    public static void Init()
    {
        if (bInit)
            return;

        SoundPlayHandler.Init();

        if (!LoadSoundConfig(ref m_DataTable))
        {
            Clear();
            return;
        }

        Load();
        bInit = true;
    }

    /// <summary>
    /// 编辑器初始化
    /// </summary>
    /// <param name="editorData"></param>
    public static void InitFromSystemEditor(Dictionary<int,SoundConfigData> editorData)
    {
        if (bInit)
            return;

        LoadSoundConfigFromSoundSystemEditor(editorData);

        Load();
        bInit = true;
    }

    /// <summary>
    /// 卸载
    /// </summary>
    public static void Clear()
    {
        DeleteAll();
        UnLoadSoundConfig();

        m_VolumeTable.Clear();
        m_CorrectTable.Clear();

        m_MixerGroupTable.Clear();
        m_MixerSnapshotTable.Clear();

        ResNode.DestroyRes(ref SoundParentRoot);
        ResNode.DestroyRes(ref SoundCacheParentRoot);
        if (audioMixer != null)
        {
            Resources.UnloadAsset(audioMixer);
        }

        SoundParentRoot = null;
        SoundCacheParentRoot = null;
        audioMixer = null;

        Initialize.onApplicationFocusChangeEvent.RemoveListener(OnFocusChange);
        SoundPlayHandler.Clear();

        bInit = false;
    }
    #endregion


    #region CreateSound
    /// <summary>
    /// 创建UI音效(2D音效)
    /// </summary>
    /// <param name="nUISoundID"></param>
    /// <returns></returns>
    public static Sound CreateUISound(int nUISoundID, bool bAsyncLoad = true)
    {
        int UISoundID = UI_SOUND_BEGIN_ID + nUISoundID;
        return CreateSound(null, UISoundID, -1, bAsyncLoad);
    }

    /// <summary>
    /// 创建音效
    /// </summary>
    /// <param name="host">挂接对象</param>
    /// <param name="nSoundID">音效ID</param>
    /// <returns></returns>
    public static Sound CreateSound(GameObject host,int nSoundID, int entityID = -1, bool bAsyncLoad = true)
    {
        Sound Result = null;

        //第一版先不cache,小于0不创建
        if (!bInit || nSoundID <= 0)
        {
            return Result;
        }
            
        if (!m_DataTable.ContainsKey(nSoundID))
        {
            SoundManager.SoundSystemLog("找不到音效配置,ID:" + nSoundID);
            return Result;
        }
        SoundConfigData config = m_DataTable[nSoundID];

        if (string.IsNullOrEmpty(config.AssetGUID) ||
            string.IsNullOrEmpty(config.AssetName) ||
            string.IsNullOrEmpty(config.AssetBundleName)
            )
        {
            SoundManager.SoundSystemLog("音效配置,ID:" + nSoundID + ",没有绑定资源!");
            return Result;
        }

        //if (config.b3DSound)
        //{
        //    if (host != null && SoldierCamera.MainInstance() != null && SoldierCamera.MainInstance().target != null)
        //    {
        //        if (Vector3.SqrMagnitude(host.transform.position - SoldierCamera.MainInstance().target.position) > config.fMaxDistance * config.fMaxDistance)
        //        {
        //            return Result;
        //        }
        //    }
        //}

        Result = GetSoundInstance();

        if (!Result.Init(host, config, entityID, bAsyncLoad))
        {
            DeleteSound(ref Result);
            return null;
        }

        Result.name = "Sound:<ID:" + nSoundID + ">";

        HashSet<Sound> list = null;
        if(m_ActiveTable.TryGetValue(nSoundID, out list))
        {
            list.Add(Result);   
        }
        else
        {
            list = new HashSet<Sound>();
            list.Add(Result);
            m_ActiveTable.Add(nSoundID, list);
        }
        return Result;
    }

    /// <summary>
    /// 创建战场BGM专用方法,由于需要覆盖音效部分配置，所以单独出来
    /// </summary>
    /// <param name="nSoundID">ID</param>
    /// <param name="nPlayCount">播放次数</param>
    /// <param name="nPlayDelay">延迟时间</param>
    /// <param name="nInterval">两次播放间隔时间</param>
    /// <returns></returns>
    public static Sound CreateWarSceneSound(int nSoundID, int nPlayCount, float nPlayDelay, float nInterval)
    {
        Sound Result = null;

        //第一版先不cache,小于0不创建
        if (!bInit || nSoundID < 0)
        {
            return Result;
        }

        if (!m_DataTable.ContainsKey(nSoundID))
        {
            SoundManager.SoundSystemLog("找不到音效配置,ID:" + nSoundID);
            return Result;
        }

        SoundConfigData config;
        CopyConfig(nSoundID, out config);

        config.nPlayTimes = nPlayCount;
        config.fDelayPlayTime = nPlayDelay;
        config.fIntervalBetweenPlayTimes = nInterval;

        if (string.IsNullOrEmpty(config.AssetGUID) ||
            string.IsNullOrEmpty(config.AssetName) ||
            string.IsNullOrEmpty(config.AssetBundleName)
            )
        {
            SoundManager.SoundSystemLog("音效配置,ID:" + nSoundID + ",没有绑定资源!");
            return Result;
        }


        Result = GetSoundInstance();

        if (!Result.Init(null, config))
        {
            DeleteSound(ref Result);
            return null;
        }

        Result.name = "Sound:<ID:" + nSoundID + ">";

        HashSet<Sound> list = null;
        if (m_ActiveTable.TryGetValue(nSoundID, out list))
        {
            list.Add(Result);
        }
        else
        {
            list = new HashSet<Sound>();
            list.Add(Result);
            m_ActiveTable.Add(nSoundID, list);
        }
        return Result;
    }
    #endregion


    public delegate void SoundDeleteEventHanlder(SoundDeleteEventArgs e);
    public static event SoundDeleteEventHanlder SoundDeleteEvent;


    public static void SetSoundDeleteHandler(SoundDeleteEventHanlder soundDeleteHandler)
    {
        SoundDeleteEvent -= soundDeleteHandler;
        SoundDeleteEvent += soundDeleteHandler;
    }
    public static void RemoveSoundDeleteHandler(SoundDeleteEventHanlder soundDeleteHandler)
    {
        SoundDeleteEvent -= soundDeleteHandler;
    }

    #region DeleteSound
    /// <summary>
    /// 删除所有音效
    /// </summary>
    public static void DeleteAll()
    {
        SoundPlayHandler.ClearData();
        foreach (HashSet<Sound> temp in m_ActiveTable.Values)
        {
            foreach (Sound instance in temp)
            {
                if (instance)
                {
                    instance.Clear();
                    GameObject.DestroyImmediate(instance.gameObject);
                }

            }
            temp.Clear();
        }

        foreach (Sound temp in m_CacheList)
        {
            if (temp)
            {
                temp.Clear();
                GameObject.DestroyImmediate(temp.gameObject);
            }
        }
        m_CacheList.Clear();
        m_ActiveTable.Clear();
    }

    /// <summary>
    /// 删除音效(供逻辑调用，主动删除音效)
    /// </summary>
    /// <param name="SoundID"></param>
    public static void DeleteSound(int SoundID)
    {
        if (m_ActiveTable.ContainsKey(SoundID))
        {
            HashSet<Sound> instance = m_ActiveTable[SoundID];
            List<Sound> tempList = new List<Sound>(instance);
            foreach (Sound ins in tempList)
            {
                Sound temp = ins;
                DeleteSound(ref temp);
            }
            m_ActiveTable.Remove(SoundID);
        }
    }

    /// <summary>
    /// 删除音效实例(音效播放完成后自己删除)
    /// </summary>
    /// <param name="instance"></param>
    public static void DeleteSound(ref Sound instance)
    {
        if (!instance)
        {
            return;
        }

        if (SoundDeleteEvent != null && instance.ConfigInstance != null)
        {
            SoundDeleteEvent(new SoundDeleteEventArgs(instance.ConfigInstance.nID, instance.EntityID, instance.ConfigInstance.soundType));
        }

        HashSet<Sound> list = null;
        if (instance.Valid)
        {
            if (m_ActiveTable.TryGetValue(instance.ConfigInstance.nID, out list))
            {
                list.Remove(instance);

                if (list == null || list.Count < 1)
                    m_ActiveTable.Remove(instance.ConfigInstance.nID);
            }
        }

        //如果删除的是BGM，则需要淡出
        if (instance.ConfigInstance != null && instance.ConfigInstance.soundType == EMSoundType.Environment)
        {
            instance.FadeOut();
        }
        else   //负责直接释放
        {
            instance.Clear();
            EnqueueCacheList(instance);
        };

        instance = null;
    }
    #endregion 


    #region SetVolAndMute
    /// <summary>
    /// 设置音效禁音状态(玩家主动设置)
    /// </summary>
    /// <param name="val"></param>
    public static void SetAudioMute(bool val)
    {
        if (bAudioMute == val)
            return;
        bAudioMute = val;
        ToggleMute();
    }

    /// <summary>
    /// 禁用3D音效(用于打开全屏界面，进入全屏场景UI等等)
    /// </summary>
    /// <param name="bForbid"></param>
    public static void Forbid3DSound(bool bForbid)
    {
        bForbid3DSound = bForbid;

        if (m_snapshotType == AudioMixerSnapshotType.Snapshot_Mute)
            return;

        if (bForbid3DSound)
        {
            SnapshotTransition(AudioMixerSnapshotType.Snapshot_2DAlone);
        }
        else
        {
            SnapshotTransition(AudioMixerSnapshotType.Snapshot_Default);
        }
    }

    /// <summary>
    /// 设置主音量
    /// </summary>
    /// <param name="val"></param>
    public static void SetMainVolume(float val)
    {
        if (mainVolume == val)
            return;
        mainVolume = val;
        foreach (HashSet<Sound> temp in m_ActiveTable.Values)
        {
            foreach (Sound sound in temp)
            {
                if(sound != null && sound.ConfigInstance != null)
                    sound.SetVolume(GetSoundVolume(sound.ConfigInstance.soundType));
            }
        }
    }

    /// <summary>
    /// 根据类型设置音效音量
    /// </summary>
    /// <param name="type"></param>
    /// <param name="val"></param>
    public static void SetSoundVolume(EMSoundType type, float val)
    {
        if (m_ActiveTable == null || m_VolumeTable[type] == val)
        {
            return;
        }
        m_VolumeTable[type] = val;

        foreach (HashSet<Sound> temp in m_ActiveTable.Values)
        {
            foreach (Sound sound in temp)
            {
                if (sound.ConfigInstance != null && sound.ConfigInstance.soundType == type)
                {
                    sound.SetVolume(GetSoundVolume(type));
                }
            }
        }   
    }
    #endregion


    /// <summary>
    /// 缓存
    /// </summary>
    /// <param name="Instance">声音实例</param>
    public static void EnqueueCacheList(Sound Instance)
    {
        if (!Instance)
            return;

        Instance.transform.SetParent(SoundCacheParentRoot.transform);
        Instance.gameObject.SetActive(false);

        m_CacheList.Enqueue(Instance);
    }

    /// <summary>
    /// 获取音效音量大小
    /// </summary>
    /// <param name="type">音效类型</param>
    /// <returns></returns>
    public static float GetSoundVolume(EMSoundType type)
    {
        if (m_VolumeTable.ContainsKey(type) && m_CorrectTable.ContainsKey(type))
        {
            return m_VolumeTable[type] * m_CorrectTable[type] * mainVolume;
        }
        else
            return 1.0f * mainVolume;
    }

    /// <summary>
    /// 获取混音组
    /// 查找规则：先尝试找指定组，再找指定组根节点
    /// </summary>
    /// <param name="groupType">混音组类型</param>
    /// <param name="groupIndex">索引</param>
    /// <returns></returns>
    public static AudioMixerGroup GetAudioMixerGroup(AudioMixerGroupType groupType, int groupIndex)
    {
        if (m_MixerGroupTable == null)
            return null;

        if (groupIndex < 0 || groupIndex >= MAX_MIXERGROUP_CHILDREN)
            return null;

        int key = (int)groupType * MAX_MIXERGROUP_CHILDREN + groupIndex;

        if (m_MixerGroupTable.ContainsKey(key))
            return m_MixerGroupTable[key];
        else
        {
            int groupTopKey = (int)groupType * MAX_MIXERGROUP_CHILDREN;
            if (m_MixerGroupTable.ContainsKey(groupTopKey))
                return m_MixerGroupTable[groupTopKey];
        }

        return null;
    }

    /// <summary>
    /// 声音系统Log输出
    /// </summary>
    /// <param name="msg"></param>
    public static void SoundSystemLog(string msg)
    {
        Debug.LogWarning("SoundSystem--" + msg);
    }

    public static void RandomPTEffect()
    {
        if (null == audioMixer)
            return;

        //audioMixer.SetFloat(AudioMixer_PT_Pitch, UnityEngine.Random.Range(0.8f, 1.0f));
        //audioMixer.SetFloat(AudioMixer_PT_Volume, UnityEngine.Random.Range(-5.0f, 5.0f));
    }


    #region private
    /// <summary>
    /// 初始化加载
    /// </summary>
    private static void Load()
    {
        //创建根节点
        SoundParentRoot = new GameObject();
        SoundParentRoot.name = "SoundRoot";
        GameObject.DontDestroyOnLoad(SoundParentRoot);

        SoundCacheParentRoot = new GameObject();
        SoundCacheParentRoot.name = "SoundCacheRoot";
        GameObject.DontDestroyOnLoad(SoundCacheParentRoot);

        //载入配置
        LoadGameSetting();
        LoadMixerAndSnapshot();

        //事件监听
        Initialize.onApplicationFocusChangeEvent.RemoveListener(OnFocusChange);
        Initialize.onApplicationFocusChangeEvent.AddListener(OnFocusChange);
    }

    /// <summary>
    /// 通过本地配置文件加载声音设置信息，主要包括各个类型声音的音量大小、禁音状态
    /// </summary>
    private static void LoadGameSetting()
    {
        bAudioMute = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.MuteAudio);
        mainVolume = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.MainVolume) / 100;

        m_VolumeTable.Clear();
        m_VolumeTable.Add(EMSoundType.Environment, (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.BGMVolume) / 100);
        m_VolumeTable.Add(EMSoundType.Skill, (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SkillSoundVolume) / 100);
        m_VolumeTable.Add(EMSoundType.System, (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SystemSoundVolume) / 100);
        m_VolumeTable.Add(EMSoundType.Speech, (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SpeechSoundVolume) / 100);

        m_CorrectTable.Clear();
        m_CorrectTable.Add(EMSoundType.Environment, 0.6f);
        m_CorrectTable.Add(EMSoundType.Skill, 0.65f);
        m_CorrectTable.Add(EMSoundType.System, 0.7f);
        m_CorrectTable.Add(EMSoundType.Speech, 1.0f);
    }

    /// <summary>
    /// 载入混音器和快照组
    /// </summary>
    private static void LoadMixerAndSnapshot()
    {
        if (audioMixer == null)
        {
            audioMixer = Resources.Load<AudioMixer>(MasterMixerPath);
            if (audioMixer == null)
            {
                SoundSystemLog("找不到混音组！");
                return;
            }

            LoadAudioMixerGroup(audioMixer);
            LoadAudioMixerSnapShot(audioMixer);
        }
    }

    /// <summary>
    /// 载入混音组
    /// </summary>
    /// <param name="mixer"></param>
    private static void LoadAudioMixerGroup(AudioMixer mixer)
    {
        if (m_MixerGroupTable == null || mixer == null)
            return;

        foreach (int index in Enum.GetValues(typeof(AudioMixerGroupType)))
        {
            string groupName = ((AudioMixerGroupType)index).ToString();

            AudioMixerGroup[] groups = mixer.FindMatchingGroups(groupName);
            if (groups.Length > 0)
            {
                for(int i = 0; i < groups.Length; i++)
                {
                    if(groups[i] != null)
                    {
                        if(!m_MixerGroupTable.ContainsKey(index * MAX_MIXERGROUP_CHILDREN + i))
                            m_MixerGroupTable.Add(index * MAX_MIXERGROUP_CHILDREN + i, groups[i]);
                    }
                }
            }
            else
            {
                SoundSystemLog("找不到混音组,请修复！名称:" + groupName);
            }
        }
    }

    /// <summary>
    /// 载入混音快照
    /// </summary>
    /// <param name="mixer"></param>
    private static void LoadAudioMixerSnapShot(AudioMixer mixer)
    {
        if (m_MixerSnapshotTable == null || mixer == null)
            return;

        foreach (int shot in Enum.GetValues(typeof(AudioMixerSnapshotType)))
        {
            if (m_MixerSnapshotTable.ContainsKey((AudioMixerSnapshotType)shot))
                continue;

            string shotName = ((AudioMixerSnapshotType)shot).ToString();
            AudioMixerSnapshot shapshot = mixer.FindSnapshot(shotName);
            if (shapshot != null)
            {
                m_MixerSnapshotTable.Add((AudioMixerSnapshotType)shot, shapshot);
            }
            else
            {
                SoundSystemLog("找不到混音器快照，请修复! 名称:" + shotName);
            }
        }

        //载入快照后如果当前禁音刷一次配置
        if(bAudioMute)
        {
            SnapshotTransition(AudioMixerSnapshotType.Snapshot_Mute);
        }
    }

    /// <summary>
    /// 快照转换
    /// </summary>
    /// <param name="shotType">转换类型</param>
    /// <param name="transitionTime">过渡时间</param>
    private static void SnapshotTransition(AudioMixerSnapshotType shotType, float transitionTime = 0.2f)
    {
        if (m_snapshotType == shotType)
            return;

        if (m_MixerSnapshotTable.ContainsKey(shotType))
        {
            if(m_MixerSnapshotTable[shotType] != null)
            {
                m_MixerSnapshotTable[shotType].TransitionTo(transitionTime);
                m_snapshotType = shotType;
            }
        }
    }

    /// <summary>
    /// 切换禁音状态
    /// </summary>
    private static void ToggleMute()
    {
        if (FinalMute)
        {
            SnapshotTransition(AudioMixerSnapshotType.Snapshot_Mute);
        }
        else
        {
            if (bForbid3DSound)
                SnapshotTransition(AudioMixerSnapshotType.Snapshot_2DAlone);
            else
                SnapshotTransition(AudioMixerSnapshotType.Snapshot_Default);
        }
    }

    /// <summary>
    /// 窗口聚失焦
    /// </summary>
    /// <param name="bFocus"></param>
    private static void OnFocusChange(bool bFocus)
    {
        if (bGameFocus != bFocus)
        {
            bGameFocus = bFocus;
            ToggleMute();
        }
    }

    /// <summary>
    /// 取得声音实例
    /// </summary>
    /// <returns></returns>
    private static Sound GetSoundInstance()
    {
        Sound Reslut = null;
        if(m_CacheList.Count <= 0)
        {
            GameObject host = new GameObject();
            Reslut = host.AddComponent<Sound>();
            UnityEngine.Object.DontDestroyOnLoad(host);
            host.transform.SetParent(SoundParentRoot.transform);

        }
        else
        {
            Reslut = m_CacheList.Dequeue();
            Reslut.gameObject.SetActive(true);
            Reslut.transform.SetParent(SoundParentRoot.transform);
        }
        return Reslut;
    }
    #endregion
}