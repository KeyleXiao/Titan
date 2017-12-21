using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Data.WarBgmConfig;

/// <summary>
/// 处理声音播放的规则需求
/// </summary>
public class SoundPlayHandler : Singleton<SoundPlayHandler>
{
    private static int currentBGM = 0;
    private static int prevBGM = 0;
    //循环标志，战场BGM需要循环
    private static int cycleFlag = 0;
    //当前BGM循环规则表，仅战场内需要
    private static List<SSchemeWarBgmConfig> BGMList = new List<SSchemeWarBgmConfig>();
    private static Dictionary<int, int> EntityRandomSoundMap = new Dictionary<int, int>();

    public static void Init()
    {
        SoundManager.SetSoundDeleteHandler(SoundDeleteHandle);
    }

    public static void Clear()
    {
        ClearData();
        SoundManager.RemoveSoundDeleteHandler(SoundDeleteHandle);
    }

    public static void ClearData()
    {
        currentBGM = 0;
        prevBGM = 0;
        cycleFlag = 0;
        BGMList.Clear();
        EntityRandomSoundMap.Clear();
    }


    #region SceneSound
    /// <summary>
    /// 场景音效播放接口(读取BgmRules脚本)
    /// </summary>
    /// <param name="ID">BGM配置ID</param>
    public static void PlaySceneSound(int ID)
    {
        if (currentBGM > 0)
            SoundManager.DeleteSound(currentBGM);

        if (!WarBgmConfig.Instance.GetWarBgmConfig(ID, ref BGMList))
            return;

        if (BGMList.Count <= 0)
            return;

        cycleFlag = 0;

        int soundID = BGMList[cycleFlag].soundID;
        SoundManager.CreateWarSceneSound(soundID, BGMList[cycleFlag].playCount, BGMList[cycleFlag].playDelay, BGMList[cycleFlag].playInterval);
        currentBGM = soundID;
    }

    /// <summary>
    /// UI场景音效播放接口(针对部分UI场景BGM调用)
    /// </summary>
    /// <param name="soundID"></param>
    public static void PlayUISceneSound(int soundID)
    {
        if (currentBGM == soundID)
            return;

        if (currentBGM > 0)
        {
            prevBGM = currentBGM;
            SoundManager.DeleteSound(currentBGM);
        }

        SoundManager.CreateUISound(soundID);
        currentBGM = soundID;
    }

    /// <summary>
    /// 停止场景BGM
    /// </summary>
    public static void StopSceneSound()
    {
        BGMList.Clear();
        SoundManager.DeleteSound(currentBGM);
        currentBGM = 0;
    }

    /// <summary>
    /// 停止UI场景BGM
    /// </summary>
    /// /// <param name="bReplayPrev">是否回播之前的BGM</param>
    public static void StopUISceneSound(bool bReplayPrev = true)
    {
        SoundManager.DeleteSound(currentBGM);

        if (bReplayPrev && prevBGM > 0)
        {
            SoundManager.CreateUISound(prevBGM);
            currentBGM = prevBGM;
            prevBGM = 0;
        }
        else
        {
            currentBGM = 0;
        }
    }

    /// <summary>
    /// 战场内切换下一首BGM
    /// </summary>
    private static void CutToNextSceneSound()
    {
        if (BGMList.Count < 1)
            return;

        if (cycleFlag == BGMList.Count - 1)
            cycleFlag = 0;
        else
            cycleFlag++;

        int soundID = BGMList[cycleFlag].soundID;
        SoundManager.CreateWarSceneSound(soundID, BGMList[cycleFlag].playCount, BGMList[cycleFlag].playDelay, BGMList[cycleFlag].playInterval);
        currentBGM = soundID;
    }
    #endregion

    #region RandomSound
    public static void PlaySkillRandomSound(int nSoundID, int entityID, GameObject entity)
    {
        if(EntityRandomSoundMap.ContainsKey(entityID) && EntityRandomSoundMap[entityID] > 0)
        {
            return;
        }

        int nRandomSoundID = ASpeedGame.Data.Scheme.SchemeRandomSound.Instance.GetRandomSound(nSoundID);
        if(nRandomSoundID < 1)
        {
            return;
        }

        Sound sound = SoundManager.CreateSound(entity, nRandomSoundID, entityID);
        if(sound != null)
        {
            EntityRandomSoundMap[entityID] = nRandomSoundID;
        }
    }

    public static void PlayHeroRandomSound(int nHeroID, int entityID, GameObject entityGO, ASpeedGame.Data.Scheme.EMHeroSoundType eType)
    {
        if (EntityRandomSoundMap.ContainsKey(entityID) && EntityRandomSoundMap[entityID] > 0)
        {
            return;
        }

        int nRandomSoundID = ASpeedGame.Data.Scheme.SchemeHeroSound.Instance.GetRandomSound(nHeroID, eType);
        if (nRandomSoundID < 1)
        {
            return;
        }
        Sound sound = SoundManager.CreateSound(entityGO, nRandomSoundID, entityID);
        if(sound != null)
        {
            EntityRandomSoundMap[entityID] = nRandomSoundID;
        }
    }

    private static void DeleteRandomSound(int nSoundID, int entityID)
    {
        if(EntityRandomSoundMap.ContainsKey(entityID) && EntityRandomSoundMap[entityID] == nSoundID)
        {
            EntityRandomSoundMap[entityID] = 0;
        }
    }
    #endregion


    private static void SoundDeleteHandle(SoundDeleteEventArgs e)
    {
        if (null == e)
            return;

        switch (e.soundType)
        {
            case SoundSystem.EMSoundType.Environment:
                {
                    if (GameLogicAPI.isInWarScene() > 0)
                        CutToNextSceneSound();
                }
                break;
            case SoundSystem.EMSoundType.Skill:
                {
                    if(e.entityID > 0)
                        DeleteRandomSound(e.soundID, e.entityID);
                }
                break;
            default:
                break;
        }

    }

}

