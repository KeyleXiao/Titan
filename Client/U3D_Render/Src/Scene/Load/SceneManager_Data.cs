using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;
using UnityEngine.Events;

public class SceneProgressEvent : UnityEvent<float>
{
}
public class SceneLoadFinishEvent : UnityEvent<string>
{
}
public class SceneLoadStartEvent : UnityEvent<string>
{
}
public class SceneTransitionEvent : UnityEvent<string>
{
}

public class SceneActiveChangeEvent : UnityEvent<string>
{
}


public partial class SceneManager : MonoBehaviour
{
    private static SceneProgressEvent m_SceneProgressCallBack = new SceneProgressEvent();

    /// <summary>
    /// 场景加载完成,这个时候场景已经加载完成了，可以加载下一个场景必要的东西
    /// </summary>
    private static SceneLoadFinishEvent m_SceneLoadFinishCallBack = new SceneLoadFinishEvent();

    /// <summary>
    /// 场景加载开始
    /// </summary>
    private static SceneLoadStartEvent m_SceneLoadStartCallBack = new SceneLoadStartEvent();

    /// <summary>
    /// 场景过渡完成,这个事件之后，玩家就能真正的看见场景了。在过渡开始事件中，禁用的输入可以恢复，让玩家操控，让玩家觉得场景加载完了。
    /// </summary>
    private static SceneTransitionEvent m_SceneTransitionFinishCallBack = new SceneTransitionEvent();

    /// <summary>
    /// 场景过渡开始，这个是紧接着场景加载完成之后的事件，表示场景开始过渡了，其实就是加载完成之后，再等一定的时间，让帧数稳定而已，这里面可以禁用输入之类的，不让玩家操作，让玩家觉得还在加载场景
    /// 这个消息其实有点多余，但是为了配套，还是仍上来吧
    /// </summary>
    private static SceneTransitionEvent m_SceneTransitionStartCallBack = new SceneTransitionEvent();

    /// <summary>
    /// 改变激活场景
    /// </summary>
    private static SceneTransitionEvent m_SceneActiveChangeCallBack = new SceneTransitionEvent();

    #region SceneProgress
    public static void RegisterSceneProgressEvent(UnityAction<float> ev)
    {
        if(!SceneManager.Instance)
        {
            ClearSceneProgressEvent();
            return;
        }
        if (null != m_SceneProgressCallBack)
        {
            if (null != ev)
            {
                m_SceneProgressCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneProgressEvent(UnityAction<float> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneProgressEvent();
            return;
        }
        if (null != m_SceneProgressCallBack)
        {
            if (null != ev)
            {
                m_SceneProgressCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneProgressEvent(float prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneProgressEvent();
            return;
        }

        if (null != m_SceneProgressCallBack)
        {
            m_SceneProgressCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneProgressEvent()
    {
        if (null != m_SceneProgressCallBack)
        {
            m_SceneProgressCallBack.RemoveAllListeners();
        }
    }
    #endregion 

    #region SceneLoadFinish
    public static void RegisterSceneLoadFinishEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadFinishEvent();
            return;
        }
        if (null != m_SceneLoadFinishCallBack)
        {
            if (null != ev)
            {
                m_SceneLoadFinishCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneLoadFinishEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadFinishEvent();
            return;
        }
        if (null != m_SceneLoadFinishCallBack)
        {
            if (null != ev)
            {
                m_SceneLoadFinishCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneLoadFinishEvent(string prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadFinishEvent();
            return;
        }
        if (null != m_SceneLoadFinishCallBack)
        {
            m_SceneLoadFinishCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneLoadFinishEvent()
    {
        if (null != m_SceneLoadFinishCallBack)
        {
            m_SceneLoadFinishCallBack.RemoveAllListeners();
        }
    }
    #endregion 

    #region SceneLoadStart
    public static void RegisterSceneLoadStartEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadStartEvent();
            return;
        }
        if (null != m_SceneLoadStartCallBack)
        {
            if (null != ev)
            {
                m_SceneLoadStartCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneLoadStartEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadStartEvent();
            return;
        }
        if (null != m_SceneLoadStartCallBack)
        {
            if (null != ev)
            {
                m_SceneLoadStartCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneLoadStartEvent(string prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneLoadStartEvent();
            return;
        }
        if (null != m_SceneLoadStartCallBack)
        {
            m_SceneLoadStartCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneLoadStartEvent()
    {
        if (null != m_SceneLoadStartCallBack)
        {
            m_SceneLoadStartCallBack.RemoveAllListeners();
        }
    }
    #endregion 

    #region SceneTransitionStart
    public static void RegisterSceneTransitionStartEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionStartEvent();
            return;
        }
        if (null != m_SceneTransitionStartCallBack)
        {
            if (null != ev)
            {
                m_SceneTransitionStartCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneTransitionStartEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionStartEvent();
            return;
        }
        if (null != m_SceneTransitionStartCallBack)
        {
            if (null != ev)
            {
                m_SceneTransitionStartCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneTransitionStartEvent(string prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionStartEvent();
            return;
        }
        if (null != m_SceneTransitionStartCallBack)
        {
            m_SceneTransitionStartCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneTransitionStartEvent()
    {
        if (null != m_SceneTransitionStartCallBack)
        {
            m_SceneTransitionStartCallBack.RemoveAllListeners();
        }
    }
    #endregion 

    #region SceneTransitionFinsh
    public static void RegisterSceneTransitionFinshEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionEvent();
            return;
        }
        if (null != m_SceneTransitionFinishCallBack)
        {
            if (null != ev)
            {
                m_SceneTransitionFinishCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneTransitionFinshEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionEvent();
            return;
        }
        if (null != m_SceneTransitionFinishCallBack)
        {
            if (null != ev)
            {
                m_SceneTransitionFinishCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneTransitionFinshEvent(string prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneTransitionEvent();
            return;
        }
        if (null != m_SceneTransitionFinishCallBack)
        {
            m_SceneTransitionFinishCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneTransitionEvent()
    {
        if (null != m_SceneTransitionFinishCallBack)
        {
            m_SceneTransitionFinishCallBack.RemoveAllListeners();
        }
    }
    #endregion 

    #region SceneActiveChange
    public static void RegisterSceneActiveChangeEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneActiveChangeEvent();
            return;
        }
        if (null != m_SceneActiveChangeCallBack)
        {
            if (null != ev)
            {
                m_SceneActiveChangeCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterSceneActiveChangeEvent(UnityAction<string> ev)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneActiveChangeEvent();
            return;
        }
        if (null != m_SceneActiveChangeCallBack)
        {
            if (null != ev)
            {
                m_SceneActiveChangeCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeSceneActiveChangeEvent(string prog)
    {
        if (!SceneManager.Instance)
        {
            ClearSceneActiveChangeEvent();
            return;
        }
        if (null != m_SceneActiveChangeCallBack)
        {
            m_SceneActiveChangeCallBack.Invoke(prog);
        }
    }

    public static void ClearSceneActiveChangeEvent()
    {
        if (null != m_SceneActiveChangeCallBack)
        {
            m_SceneActiveChangeCallBack.RemoveAllListeners();
        }
    }
    #endregion 
}

