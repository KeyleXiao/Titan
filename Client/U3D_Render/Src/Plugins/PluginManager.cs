using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;


public class PluginManager : MonoBehaviour
{
    public enum RenderTime
    {
        EndOfFrame = 0,
        PreCamera,
        PostCamera
    };

    public static PluginManager Singleton = null;

    /// <summary>
    /// When to trigger Scaleform rendering during the frame. 
    /// Scaleform rendering can be performed at the end of the frame (most common use case), or before/after a certain camera. This can be used to render Scaleform before/after a particular object in the	scene. 
    /// </summary>
    public RenderTime WhenToRender = RenderTime.EndOfFrame;

    public void Awake()
    {
        Singleton = this;
    }

//    public void Start()
//    {
//#if UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO
//        GL.IssuePluginEvent(0);
//#elif UNITY_IPHONE  || UNITY_ANDROID
//            UnityRenderEvent(0);
//#endif
//        GL.InvalidateState();

//        if (WhenToRender == RenderTime.EndOfFrame)
//        {
//            StartCoroutine(CallPluginAtEndOfFrame());
//        }
//    }

//    public void OnDestroy()
//    {
//        // This is used to initiate RenderHALShutdown, which must take place on the render thread.
//#if UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO || UNITY_WP8
//        GL.IssuePluginEvent(2);
//#endif

//        Singleton = null;
//    }

//    /// <summary>
//    /// Used with EndOfFrame render layers, pumps Scaleform once per frame at the end of the frame.
//    /// </summary>
//    /// <returns></returns>
//    protected IEnumerator CallPluginAtEndOfFrame()
//    {
//        while (true)
//        {
//            yield return new WaitForEndOfFrame();
//            if (WhenToRender == RenderTime.EndOfFrame)
//            {
//                PumpPluginRender();
//            }
//        }
//    }

//    /// <summary>
//    /// Issues a call to perform Scaleform rendering. Rendering is multi-threaded on Windows and single threaded on iOS/Android.
//    /// </summary>
//    private void PumpPluginRender()
//    {
//#if UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO
//        GL.IssuePluginEvent(1);
//#elif UNITY_IPHONE  || UNITY_ANDROID
//        UnityRenderEvent(1);
//#endif
//    }

//    /// <summary>
//    /// Used with PreCamera render layers, pumps Scaleform once prior to the rendering the camera to which the Scaleform instance is attached. Can be used to render Scaleform before a particular camera renders.
//    /// </summary>
//    public void OnPluginPreRender()
//    {
//        if (WhenToRender == RenderTime.PreCamera)
//        {
//            PumpPluginRender();
//        }
//    }

//    /// <summary>
//    /// Used with PostCamera render layers, pumps Scaleform once after rendering the camera to which the Scaleform instance is attached. Can be used to render Scaleform after a particular camera renders.
//    /// </summary>
//    public void OnPluginPostRender()
//    {
//        GL.InvalidateState();

//        if (WhenToRender == RenderTime.PostCamera)
//        {
//            PumpPluginRender();            
//        }
//    }

//    public void Update()
//    {
//        Camera cam = Initialize.mainCam;
//        if (cam != null)
//        {
//            if (cam.gameObject.GetComponent<CameraPlugin>() == null)
//            {
//                cam.gameObject.AddComponent<CameraPlugin>();
//            }
//        }
//    }
}
