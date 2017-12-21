/*******************************************************************
** 文件名:	SoundConfig.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音配置

**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/7/11
** 描  述: 增加声音类型区分，增加音量控制和静音设置
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

namespace SoundSystem
{
    public enum SoundConfigCol
    {
        COL_ID = 0,
        COL_Type,
        COL_MixerGroupType,
        COL_MixerGroupIndex,
        COL_BypassEffect,
        COL_BypassReverbZone,
        COL_PlayOnAwake,
        COL_Priority,
        COL_Pitch,
        COL_StereoPan,
        COL_ReverZoneMix,
        COL_3DSound,
        COL_DopplerLv,
        COL_Spread,
        COL_RolloffMode,
        COL_MinDistance,
        COL_MaxDistance,
        COL_PlayTimes,
        COL_Duartion,
        COL_DelayTime,
        COL_IntervalBetweenPlayTimes,
        COL_AssetBundleName,
        COL_AssetName,
        COL_AssetGUID,
        COL_AssetSize_X,
        COL_AssetSize_Y,
        COL_DependAssetGUID,
    }


    public enum EMSoundType
    {
        Environment = 1,                //环境
        Skill,                          //技能
        System,                         //系统
        Speech,                         //语音
        Sigh,                           //标记
        MAX,
    }


    public class SoundConfigData : ResConfigData
    {
        /// <summary>
        /// 音效ID
        /// </summary>
        public int nID ;

        /// <summary>
        /// 声音类型
        /// </summary>
        public EMSoundType soundType;

        /// <summary>
        /// 混音组名称
        /// </summary>
        public AudioMixerGroupType mixerGroupType;

        /// <summary>
        /// 混音索引
        /// </summary>
        public int nMixerGroupIndex;

        /// <summary>
        /// 旁路效果
        /// </summary>
        public bool bBypassEffect;

        /// <summary>
        /// 路由混响
        /// </summary>
        public bool bBypassReverbZone;

        public bool bPlayOnAwake;

        /// <summary>
        /// 优先级
        /// </summary>
        [Range(0,256)]
        public int nPriority;

        /// <summary>
        /// 高音
        /// </summary>
        [Range(-3.0f,3.0f)]
        public float fPitch;

        /// <summary>
        /// 立体声
        /// </summary>
        [Range(-1.0f,1.0f)]
        public float fStereoPan;

        public bool b3DSound;

        /// <summary>
        /// 全局混响
        /// </summary>
        [Range(0,1.1f)]
        public float fReverZoneMix;

        /// <summary>
        /// 多普勒
        /// </summary>
        [Range(0,5.0f)]
        public float fDopplerLv;
        
        /// <summary>
        /// 穿透
        /// </summary>
        [Range(0,360)]
        public int nSpread;

        /// <summary>
        /// 3D声音衰减类型
        /// </summary>
        public AudioRolloffMode audioRoffType;

        /// <summary>
        /// 3D声音最小距离
        /// </summary>
        public float fMinDistance;

        /// <summary>
        /// 3D声音最大距离
        /// </summary>
        public float fMaxDistance;

        /// <summary>
        /// 播放次数，《=0，表示循环
        /// </summary>
        public int nPlayTimes;

        /// <summary>
        /// 持续时间，《=0，表示，这个选项无用。
        /// </summary>
        public float fDuartion;

        /// <summary>
        /// 延迟多少秒播放
        /// </summary>
        public float fDelayPlayTime;

        /// <summary>
        /// 两次播放之间的时间间隔
        /// </summary>
        public float fIntervalBetweenPlayTimes;

        /// <summary>
        /// 依赖资源，仅编辑器用到
        /// </summary>
        public GameObject dependAsset;

        /// <summary>
        /// 依赖资源GUID
        /// </summary>
        public string szDependAssetGUID;
    }
}
