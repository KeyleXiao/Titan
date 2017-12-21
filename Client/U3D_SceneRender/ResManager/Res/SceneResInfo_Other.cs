/*******************************************************************
** 文件名:	SceneResInfo_Rendering.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息,渲染部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
public class SceneResFadeInFinishEvent : UnityEvent<SceneResInfo>
{
}

public class SceneResFadeOutFinishEvent : UnityEvent<SceneResInfo>
{
}


public partial class SceneResInfo : SceneInfo
{
    private SceneResFadeInFinishEvent FadeInFinish = new SceneResFadeInFinishEvent();

    private SceneResFadeOutFinishEvent FadeOutFinish = new SceneResFadeOutFinishEvent();

    #region FadeInFinish
    public void RegisterFadeInFinishEvent(UnityAction<SceneResInfo> ev)
    {
        if (null != FadeInFinish)
        {
            if (null != ev)
            {
                FadeInFinish.AddListener(ev);
            }
        }
    }

    public void UnRegisterFadeInFinishEvent(UnityAction<SceneResInfo> ev)
    {
        if (null != FadeInFinish)
        {
            if (null != ev)
            {
                FadeInFinish.RemoveListener(ev);
            }
        }
    }

    public void InvokeFadeInFinishEvent()
    {
        if (null != FadeInFinish)
        {
            FadeInFinish.Invoke(this);
        }
    }

    public void ClearFadeInFinishEvent()
    {
        if (null != FadeInFinish)
        {
            FadeInFinish.RemoveAllListeners();
        }
    }
    #endregion

    #region FadeOutFinish
    public void RegisterFadeOutFinishEvent(UnityAction<SceneResInfo> ev)
    {
        if (null != FadeOutFinish)
        {
            if (null != ev)
            {
                FadeOutFinish.AddListener(ev);
            }
        }
    }

    public void UnRegisterFadeOutFinishEvent(UnityAction<SceneResInfo> ev)
    {
        if (null != FadeOutFinish)
        {
            if (null != ev)
            {
                FadeOutFinish.RemoveListener(ev);
            }
        }
    }

    public void InvokeFadeOutFinishEvent()
    {

        if (null != FadeOutFinish)
        {
            FadeOutFinish.Invoke(this);
        }
    }

    public void ClearFadeOutFinishEvent()
    {
        if (null != FadeOutFinish)
        {
            FadeOutFinish.RemoveAllListeners();
        }
    }
    #endregion 
}
