/*******************************************************************
** 文件名:	SoundConfig.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音实例

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
using DG.Tweening;

[RequireComponent(typeof(AudioSource))]
public sealed class Sound : MonoBehaviour, IAsyncResLoadHandler
{
    public int ConfigID
    {
        get
        {
            if (m_Config == null)
                return -1;
            return m_Config.nID;
        }
    }

    public int EntityID
    {
        get { return m_entityID; }
    }

    public ResNode resNode
    {
        get
        {
           return m_ResNode;
        }
    }

    public SoundConfigData ConfigInstance
    {
        get
        {
            return m_Config;
        }
    }

    public bool Valid
    {
        get
        {
            return m_Valid;
        }
    }

    public bool isResourceLoadFinish
    {
        get
        {
            if(ResNode.isNullOrEmpty(m_ResNode))
            {
                return true;
            }
            return m_ResNode.isLoadFinish;
        }
    }

    private ResNode m_ResNode = null;
    private SoundConfigData m_Config;
    private AudioSource m_AudioInstance = null;
    private GameObject m_host = null;
    private int m_entityID = -1;

    private float currAudioLength = 0.0f;
    private int currPlayTimes = 0;
    private float orginalVolume = 0.0f;

    private bool isFinish = true;
    private bool bPlay = false;
    private bool m_Valid = true;
    private bool bAsyncLoad = true;


    #region Public
    /// <summary>
    /// 资源加载回调
    /// </summary>
    /// <param name="cof"></param>
    /// <param name="res"></param>
    /// <param name="UserDataObj"></param>
    public void ResLoadCallBack(ResConfigData cof, ResNode res, System.Object UserDataObj)
    {
        if(!m_Valid)
        {
            return;
        }
        SoundConfigData config = cof as SoundConfigData;
        Sound ins = UserDataObj as Sound;
        if(null == config ||
            !ins)
        {
            return;
        }

        m_ResNode = res;

        if(ApplyConfig())
        {
            isFinish = false;

            if (config.bPlayOnAwake || bPlay)
            {
                Play();
            }
        }  
    }

    /// <summary>
    /// 初始化
    /// </summary>
    /// <param name="host">挂载物体</param>
    /// <param name="config">音效配置</param>
    /// <returns></returns>
    public bool Init(GameObject host, SoundConfigData config, int enityID = -1, bool _AsyncLoad = true)
    {
        Clear();

        m_entityID = enityID;
        m_Config = config;
        m_host = host;
        m_AudioInstance = GetComponent<AudioSource>();
        orginalVolume = m_AudioInstance.volume;
        //m_AudioInstance.hideFlags = HideFlags.HideAndDontSave;

        m_Valid = true;
        if (_AsyncLoad)
        {
            m_ResNode = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Audio, config, this, this);
        }
        else
        {
            m_ResNode = AssetBundleManager.GetAssets(AssetType.Asset_Audio, config);
        }

        if (null == m_ResNode)
        {
            m_Valid = false;
            return false;
        }

        bAsyncLoad = _AsyncLoad;
        if (!_AsyncLoad && m_ResNode.IsValid())
        {
            if (ApplyConfig())
            {
                isFinish = false;

                if (config.bPlayOnAwake || bPlay)
                {
                    Play();
                }
            }
        }

        return true;
    }

    public void Clear()
    {
        if (!m_Valid)
        {
            return;
        }
        bPlay = false;
        m_Config = null;
        //顺序不能乱
        StopAllCoroutines();
        if (m_AudioInstance)
        {
            m_AudioInstance.clip = null;
            m_AudioInstance.outputAudioMixerGroup = null;
            m_AudioInstance.Stop();
            m_AudioInstance.volume = orginalVolume;
        }

        //删除资源
        if (bAsyncLoad)
        {
            AssetBundleManager.DeleteAssetsAsync(ref m_ResNode, this, false);
        }
        else
        {
            AssetBundleManager.DeleteAssets(ref m_ResNode, false);
        }

        m_entityID = -1;
        m_ResNode = null;
        m_AudioInstance = null;
        m_host = null;
        currPlayTimes = 0;
        isFinish = true;
        m_Valid = false;
        bAsyncLoad = true;
    }

    // 设置音量
    public void SetVolume(float vol)
    {
        if (m_AudioInstance != null)
        {
            m_AudioInstance.volume = vol;
        }
    }

    // 淡出
    public void FadeOut()
    {
        if (!m_AudioInstance)
        {
            Clear();
            SoundManager.EnqueueCacheList(this);
            return;
        }

        m_AudioInstance.DOFade(0, 1f).OnComplete(OnFadeOutEnd);
    }

    public void Start()
    {
        GameUtil.SetLayer(gameObject.layer, gameObject, true);
    }

    public void LateUpdate()
    {
        if (!bPlay || !m_host)
        {
            return;
        }
        if (null != m_Config && m_Config.b3DSound)
        {
            m_AudioInstance.transform.SetPosition(m_host.transform.position);
        }
    }

    public void OnDestroy()
    {
        Clear();
    }
    #endregion


    #region Private
    bool ApplyConfig()
    {
        if (null == m_ResNode || !m_ResNode.IsValid())
        {
            return false;
        }

        AudioClip clip = m_ResNode.ConvertRes<AudioClip>();
        m_AudioInstance.clip = clip;

        //混音组
        AudioMixerGroupType groupType = m_Config.mixerGroupType;
        if (m_Config.mixerGroupType == AudioMixerGroupType.Normal)
        {
            if(m_host != null && m_host == EntityFactory.MainHero)
            {
                groupType = AudioMixerGroupType.Actor;
                if(m_Config.nMixerGroupIndex == SoundManager.AudioMixer_Actor_PT)
                {
                    SoundManager.RandomPTEffect();
                }
            }
        }
        AudioMixerGroup mixerGroup = SoundManager.GetAudioMixerGroup(groupType, m_Config.nMixerGroupIndex);
        if (mixerGroup != null)
            m_AudioInstance.outputAudioMixerGroup = mixerGroup;

        m_AudioInstance.bypassEffects = m_Config.bBypassEffect;
        m_AudioInstance.bypassReverbZones = m_Config.bBypassReverbZone;
        m_AudioInstance.playOnAwake = false;
        m_AudioInstance.priority = m_Config.nPriority;
        m_AudioInstance.pitch = m_Config.fPitch;
        m_AudioInstance.panStereo = m_Config.fStereoPan;
        m_AudioInstance.spatialBlend = m_Config.b3DSound ? 1 : 0;
        m_AudioInstance.loop = m_Config.nPlayTimes == 1 ? false : true;

        m_AudioInstance.reverbZoneMix = m_Config.fReverZoneMix;
        m_AudioInstance.dopplerLevel = m_Config.fDopplerLv;
        m_AudioInstance.spread = m_Config.nSpread;

        m_AudioInstance.minDistance = m_Config.fMinDistance;
        m_AudioInstance.maxDistance = m_Config.fMaxDistance;
        m_AudioInstance.rolloffMode = m_Config.audioRoffType;

        m_AudioInstance.volume = SoundManager.GetSoundVolume(m_Config.soundType);

        currAudioLength = clip.length;
        if (null != m_Config && m_Config.b3DSound && m_host != null)
        {
            m_AudioInstance.transform.SetPosition(m_host.transform.position);
        }

        return true;
    }

    void Play()
    {
        bPlay = true;

        if(isFinish)
        {
            return;
        }
        //有延迟播放
        if (m_Config.fDelayPlayTime >= 0.001f)
        {
            m_AudioInstance.PlayDelayed(m_Config.fDelayPlayTime);
        }
        else
        {
            m_AudioInstance.Play();
        }

       
        //等待播放完成
        StartCoroutine(WaitForPlayTimesFinsh());

        //如果有持续时间
        if (m_Config.fDuartion >= 0.001f)
        {
            StartCoroutine(WaitForDuartionFinish());
        }
    }

    void Finish()
    {
        Sound temp = this;
        SoundManager.DeleteSound(ref temp);
    }

    void OnFadeOutEnd()
    {
        Clear();
        SoundManager.EnqueueCacheList(this);
    }

    void SetPosition(Vector3 pos)
    {
        transform.SetPosition(pos);
    }
    #endregion 


    /// <summary>
    /// 等待播放次数完成
    /// </summary>
    /// <returns></returns>
    IEnumerator WaitForPlayTimesFinsh()
    {
        //有延迟播放
        if (m_Config.fDelayPlayTime >= 0.03f)
        {
            yield return new WaitForSeconds(m_Config.fDelayPlayTime);
        }

        while (true)
        {
            yield return new WaitForSeconds(currAudioLength);
            currPlayTimes++;
            
            //如果有播放次数限制
            if (m_Config.nPlayTimes > 0)
            {
                if (currPlayTimes >= m_Config.nPlayTimes)
                {
                    Finish();
                    yield break;
                }
            }


            //如果有播放间隔
            if (m_Config.fIntervalBetweenPlayTimes >= 0.001f)
            {
                m_AudioInstance.Stop();
                yield return new WaitForSeconds(m_Config.fIntervalBetweenPlayTimes);
                m_AudioInstance.Play();
            }
        }
    }

    /// <summary>
    /// 等待持续时间播放完成
    /// </summary>
    /// <returns></returns>
    IEnumerator WaitForDuartionFinish()
    {
        yield return new WaitForSeconds(m_Config.fDuartion);
        Finish();
    }
}
