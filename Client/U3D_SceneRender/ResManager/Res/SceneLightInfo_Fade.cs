/*******************************************************************
** 文件名:	SceneLightInfo_Fade.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景灯光信息
** 应  用:   场景灯光信息，用于做场景资源管理,渐变部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;
public partial class SceneLightInfo : SceneInfo
{
    
    private SceneFadeInOutLightIntensity m_FadeInOutLightIntensity = null;

    private float OrginalIntensity = 0.0f;

    private bool isFadeing = false;
    void InitFade()
    {
        OrginalIntensity = HostLight.intensity;
        m_FadeInOutLightIntensity = gameObject.AddComponent<SceneFadeInOutLightIntensity>();
        m_FadeInOutLightIntensity.enabled = false;
        m_FadeInOutLightIntensity.RegisterFinishEvent(OnFadeInOutFinish);
        m_FadeInOutLightIntensity.FadeOutDuartion = 0.5f;
        m_FadeInOutLightIntensity.HostLight = HostLight;
    }

    void FadeIn()
    {
        if(isFadeing)
        {
            return;
        }
        isFadeing = true;

        if(!m_FadeInOutLightIntensity)
        {
            OnFadeInOutFinish(null);
            return;
        }
        HostLight.enabled = true;
        HostLight.intensity = 0;
        m_FadeInOutLightIntensity.enabled = true;
        m_FadeInOutLightIntensity.FadeOutFrom = 0;
        m_FadeInOutLightIntensity.FadeOutTo = OrginalIntensity;
        m_FadeInOutLightIntensity.Play();
    }

    void FadeOut()
    {
        if (isFadeing)
        {
            return;
        }
        isFadeing = true;
        if (!m_FadeInOutLightIntensity)
        {
            OnFadeInOutFinish(null);
            return;
        }
        HostLight.enabled = true;
        HostLight.intensity = OrginalIntensity;
        m_FadeInOutLightIntensity.enabled = true;
        m_FadeInOutLightIntensity.FadeOutFrom = OrginalIntensity;
        m_FadeInOutLightIntensity.FadeOutTo = 0;
        m_FadeInOutLightIntensity.Play();
    }

    void ClearFade()
    {
        if (m_FadeInOutLightIntensity)
        {
            ResNode.DestroyRes(ref m_FadeInOutLightIntensity);
        }
    }

    public void OnFadeInOutFinish(Effect.SceneFadeInOutLightIntensity ins)
    {
        isFadeing = false;

        if(m_FadeInOutLightIntensity)
        {
            m_FadeInOutLightIntensity.enabled = false;
        }

        if(OptimitzedLight)
        {
            HostLight.enabled = false;
        }
    }
}
