using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class RenderingPlugin : MonoBehaviour
{
    public static RenderingPlugin Singleton = null;


//    // Native plugin rendering events are only called if a plugin is used
//    // by some script. This means we have to DllImport at least
//    // one function in some active script.
//    // For this example, we'll call into plugin's SetTimeFromUnity
//    // function and pass the current time so the plugin can animate.


//#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
//    [DllImport("RenderingPlugin")]
//#elif UNITY_IPHONE
//    [DllImport("__Internal")]
//#endif
//    private static extern void SetTimeFromUnity(float t);

//#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
//    [DllImport("RenderingPlugin")]
//#elif UNITY_IPHONE
//    [DllImport("__Internal")]
//#endif
//    private static extern void ReleaseMinmipOfTexture(IntPtr pTexture);

//#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
//    [DllImport("RenderingPlugin")]
//#elif UNITY_IPHONE
//    [DllImport("__Internal")]
//#endif
//    public static extern IntPtr CreateTexture(int width, int height, int levels, int usage, int format, int pool, int lightmapformat, IntPtr data, int datalen, int lockflag);

//#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
//    [DllImport("RenderingPlugin")]
//#elif UNITY_IPHONE
//    [DllImport("__Internal")]
//#endif
//    public static extern void ReleaseTexture(IntPtr pTexture);

    public static IntPtr CreateTexture(int width, int height, int levels, int usage, int format, int pool, int lightmapformat, IntPtr data, int datalen, int lockflag)
    {
        return IntPtr.Zero;
    }
    public static void ReleaseTexture(IntPtr pTexture)
    {

    }
    public void Awake()
    {
        Singleton = this;

        //// Set time for the plugin
        //SetTimeFromUnity(Time.timeSinceLevelLoad);

    }

    public void OnDestroy()
    {
        Singleton = null;
    }

    public void ReleaseTexture(Texture tex)
    {
        //if (tex == null) return;
        //ReleaseMinmipOfTexture(tex.GetNativeTexturePtr());
    }


    //IEnumerator Start()
    //{
    //    yield return StartCoroutine(CallPluginAtEndOfFrames());
    //}

    //private IEnumerator CallPluginAtEndOfFrames()
    //{
    //    while (true)
    //    {
    //        // Wait until all frame rendering is done
    //        yield return new WaitForEndOfFrame();

    //        // Set time for the plugin
    //        SetTimeFromUnity(Time.timeSinceLevelLoad);

    //        // Issue a plugin event with arbitrary integer identifier.
    //        // The plugin can distinguish between different
    //        // things it needs to do based on this ID.
    //        // For our simple plugin, it does not matter which ID we pass here.
    //        GL.IssuePluginEvent(1);
    //    }
    //}



}
