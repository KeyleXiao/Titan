/*******************************************************************
** 文件名:	GfxUtil.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/26
** 版  本:	1.0
** 描  述:	ScaleForm GFx 工具类
** 应  用:  	目前有 坐标转换，视口判断，颜色转换等功能

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Scaleform;
using System.IO;

/// <summary>
/// Gfx工具集
/// </summary>
public class GfxUtil  {

    public static Vector2 UIStageToScreenScale = Vector2.zero;
    public static Vector2 UIScreenToStageScale = Vector2.zero;
    public static Vector2 UIStageToScreenDelta = Vector2.zero;
    public static Vector2 UIScreenToStageDelta = Vector2.zero;

    /// <summary>
    /// 标准UI宽
    /// </summary>
    public const int StandardMovieWidth = 1920;

    /// <summary>
    /// 标准UI高
    /// </summary>
    public const int StandardMovieHeight = 1080;

    /// <summary>
    /// 屏幕缩放，UI的标准是StandardMovieWidth*StandardMovieHeight，这个是屏幕相对于这个的缩放。
    /// </summary>
    public static Vector2 ScreenScale = Vector2.zero;

    public static float screenAspectRatio = 0;
    public static float stageAspectRatio = 0;

    private static int OldScreenWidth = 0;
    private static int OldScreenHeight = 0;

    private static Camera MainCamera = null;
    public static void CaculUIScale()
    {
        bool bNeedCacul = false;
        if (OldScreenHeight != Screen.height)
        {
            bNeedCacul = true;
            OldScreenHeight = Screen.height;
        }
        if (OldScreenWidth != Screen.width)
        {
            bNeedCacul = true;
            OldScreenWidth = Screen.width;
        }
        if (!bNeedCacul)
            return;
        float movieWidth = StandardMovieWidth;
        float movieHeight = StandardMovieHeight;

        float ScreenWidth = Screen.width;
        float ScreenHeight = Screen.height;

        UIStageToScreenScale.x= (Screen.width / movieWidth);
        UIStageToScreenScale.y = (Screen.height / movieHeight);

        UIStageToScreenDelta.x = (Screen.width - movieWidth);
        UIStageToScreenDelta.y = (Screen.height - movieHeight);

        stageAspectRatio = (movieHeight / movieWidth);
        screenAspectRatio = (Screen.height / (float)Screen.width);


        UIScreenToStageScale.x = (movieWidth / ScreenWidth);
        UIScreenToStageScale.y = (movieHeight / ScreenHeight);

        UIScreenToStageDelta.x = (movieWidth - ScreenWidth);
        UIScreenToStageDelta.y = (movieHeight - ScreenHeight);

        ScreenScale.x = (Screen.width / movieWidth);
        ScreenScale.y = (Screen.height / movieHeight);

   }

    /// <summary>
    /// 从指针取得字符串数组，这里的指针是指C++中的字符串数组
    /// </summary>
    /// <param name="argCount">数目</param>
    /// <param name="p">指针</param>
    /// <returns></returns>
    public static string[] GetStringArrayFromPtr(int argCount, System.IntPtr p)
    {
        if (argCount <= 0)
            return null;
        List<string> retList = new List<string>();
        for (int i = 0; i < argCount; i++)
        {
            IntPtr address = (IntPtr)Marshal.PtrToStructure(p, typeof(IntPtr));
            if (address != IntPtr.Zero)
            {
                string sz = Marshal.PtrToStringAnsi(address);
                retList.Add(sz);
                p = (IntPtr)((Int32)p + 4);
            }
        }
        return retList.ToArray();
    }

    /// <summary>
    /// AS到屏幕坐标，这里的AS的长宽固定为1280*720
    /// </summary>
    /// <param name="stagePoint">舞台坐标</param>
    /// <returns></returns>
    public static Vector3 StagePointToScreenPoint(Vector3 stagePoint)
    {
        
        float x = stagePoint.x;
        float y = stagePoint.y;

        float stageX = x;
        float stageY = y;


        x = (int)(stageX * UIStageToScreenScale.x);
        y = (int)(stageY * UIStageToScreenScale.y);


        switch (SFGFxMovieManager.GfxScaleType)
        {
            case ScaleModeType.SM_ExactFit:
                {
                    break;
                }
            case ScaleModeType.SM_NoBorder:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (Screen.width * stageAspectRatio), Screen.height);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (Screen.height / stageAspectRatio), Screen.width);
                    }

                    break;
                }
            case ScaleModeType.SM_NoScale:
                {
                    x = stageX;
                    x += (UIStageToScreenDelta.x * 0.5f);
                    y = stageY;
                    y += (UIStageToScreenDelta.y * 0.5f);
                    break;
                }
            case ScaleModeType.SM_ShowAll:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (Screen.height / stageAspectRatio), Screen.width);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (Screen.width * stageAspectRatio), Screen.height);
                    }

                    break;
                }
        }

        y = Screen.height - y;

        return new Vector3(x, y, 0F);
    }
    /// <summary>
    /// 屏幕坐标到AS，这里的AS的长宽固定为1280*720
    /// </summary>
    /// <param name="screenPoint">屏幕坐标</param>
    /// <returns></returns>
    public static Vector3 ScreenPointToStagePoint(Vector3 screenPoint)
    {
        float x = screenPoint.x;
        float y = screenPoint.y;

        float movieWidth = StandardMovieWidth;
        float movieHeight = StandardMovieHeight;

        float ScreenWidth = Screen.width;
        float ScreenHeight = Screen.height;

        float stageX = x;
        float stageY = y;


        x = stageX * UIScreenToStageScale.x;
        y = stageY * UIScreenToStageScale.y;


        switch (SFGFxMovieManager.GfxScaleType)
        {
            case ScaleModeType.SM_ExactFit:
                {
                    break;
                }
            case ScaleModeType.SM_NoBorder:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (movieWidth * screenAspectRatio), movieHeight);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (movieHeight / screenAspectRatio), movieWidth);
                    }
                    break;
                }
            case ScaleModeType.SM_NoScale:
                {
                    x = stageX;
                    x += (UIScreenToStageDelta.x * 0.5f);
                    y = stageY;
                    y += (UIScreenToStageDelta.y * 0.5f);
                    break;
                }
            case ScaleModeType.SM_ShowAll:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (movieHeight / screenAspectRatio), movieWidth);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (movieWidth * screenAspectRatio), movieHeight);
                    }
                    break;
                }
        }

        y = movieHeight - y;

        return new Vector3(x, y, 0F);
    }

    private static float RescaleValueWithOffset(float value, float scale, float offset)    
    {
        // remove the offset
        value -= offset * 0.5f;

        // normalize value
        value /= offset;

        // scale up to the targetScale
        value *= scale;

        // restore the offset
        value += offset * 0.5f;
        
        return value;
    }

    public static uint ColorToUINT(Color col)
    {
        //flash是ARGB形式
        byte val8;
        uint val32 = 0;

        // Alpha
        val8 = (byte)(col.a * 255);
        val32 += (uint)val8 << 24;

        // Red
        val8 = (byte)(col.r * 255);
        val32 = (uint)(val8 << 16);

        // Green
        val8 = (byte)(col.g * 255);
        val32 += (uint)val8 << 8;

        // Blue
        val8 = (byte)(col.b * 255);
        val32 += (uint)val8;

        return val32;
    }

    public static Color UINTtoColor(uint col)
    {
        uint val = col;
        Color c;
        // Alpha
        c.a = (byte)(val >> 24) / 255.0f;

        // Red
        c.r = (byte)(val >> 16) / 255.0f;

        // Green
        c.g = (byte)(val >> 8) / 255.0f;

        // Blue
        c.b = (byte)(val) / 255.0f;

        return c;
    }

    /// <summary>
    /// 由于Scaleform是没有Unity的视锥剔除，所以要判断点是否在视野内
    /// </summary>
    /// <param name="p">坐标，默认是屏幕坐标</param>
    /// <param name="isWorldPos">是否是世界坐标</param>
    /// <returns></returns>
    public static bool IsOnView(Vector3 p,bool isWorldPos)
    {
        if (null == MainCamera)
        {
            MainCamera = Initialize.mainCam;
        }
        if (!isWorldPos)
        {
            if (p.x > Screen.width || p.x  < 0 ||
                p.y > Screen.height || p.y < 0)
                return false;
            return true;
        }
        else
        {
            Vector3 ViewPos = ViewPos = MainCamera.ScreenToViewportPoint(p);
            Vector3 WorldPos = MainCamera.ScreenToWorldPoint(p);
            ViewPos = MainCamera.WorldToViewportPoint(WorldPos);
            if (ViewPos.x < 0 || ViewPos.x > 1 ||
                ViewPos.y < 0 || ViewPos.y > 1 ||
                    ViewPos.z < 0)
                return false;
            return true;
        }

    }


    /// <summary>
    /// Loads the file into memory as a Byte array.
    /// </summary>
    /// <returns>
    /// Byte array with file contents.
    /// </returns>
    /// <param name='filename'>
    /// A filename relative to the StreamingAssets folder.
    /// </param>
    static public Byte[] LoadFileToMemory(string filename)
    {
        string filePath = System.IO.Path.Combine(Application.streamingAssetsPath, filename);
#if UNITY_ANDROID
		if (filePath.Contains("jar:file://"))
		{
			WWW url = new WWW(filePath);
			
			// wait until 'download' finishes
			while(!url.isDone)
			{
			}

			return url.bytes;
		}
		else
#endif

        {
            Byte[] fileBytes = null;
            filePath = SFManager.GetScaleformContentPath() + filename;
            try
            {
                fileBytes = System.IO.File.ReadAllBytes(filePath);
                //using (FileStream fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                //{
                //    using (BinaryReader reader = new BinaryReader(fileStream))
                //    {
                //        fileBytes = reader.ReadBytes((int)fileStream.Length);
                //    }
                //}

                return fileBytes;
            }
            catch (Exception e)
            {
                Debug.Log(String.Format("Loading file failed with message = '{0}'", e.Message));
            }
        }

        return null;
    }

    /// <summary>
    /// Create and return a new movie instance with the given swfName.
    /// </summary>
    /// <returns>
    /// An instance of class T.
    /// </returns>
    /// <param name='camera'>
    /// 创建的Swf属于哪个SFCamera
    /// </param>
    /// <param name='swfName'>
    /// A path to the SWF or GFX file relative to the StreamingAssets folder.
    /// </param>
    /// <param name='fromByteStream'>
    /// If true, an attempt will be made to load this file into memory. On some platforms, this is useful when there isn't a real file path.
    /// </param>
    /// <param name='backgroundColor'>
    /// The background clear color for this movie.
    /// </param>
    /// <param name='renderTarget'>
    /// The RenderTarget to use when rendering this movie. A null value just renders to the screen.
    /// </param>
    /// <param name='scaleModeType'>
    /// Override the default ScaleModeType for this movie. Default is SM_ShowAll.
    /// </param>
    /// <typeparam name='T'>
    /// The class type that will be instanced for this movie.
    /// </typeparam> 
    public static T CreateSwf<T>(SFCamera camera,string swfName, bool fromByteStream, Color32 backgroundColor, RenderTexture renderTarget = null, ScaleModeType scaleModeType = ScaleModeType.SM_ShowAll, string movieName = null) where T : Movie
    {
        if (camera == null)
        {
            return default(T);
        }

        int depth = 1;
        bool initFirstFrame = false;
        bool useBackgroundColor = false;
        bool autoManageViewport = true;
        int ox = 0, oy = 0, width = 0, height = 0;
        Int32 length = 0;
        IntPtr unmanagedData = IntPtr.Zero;
        Byte[] swfBytes = null;

        // load the swf either from a byte array or via path
        SFMovieCreationParams creationParams;


        if (renderTarget != null)
        {
            ox = 0;
            oy = 0;
            width = renderTarget.width;
            height = renderTarget.height;

            useBackgroundColor = true;
            autoManageViewport = false;
        }
        else
        {
            SFCamera.GetViewport(ref ox, ref oy, ref width, ref height);
        }


        bool loadSucceeded = false;
        if (fromByteStream)
        {
            swfBytes = LoadFileToMemory(swfName);
            if (swfBytes != null && swfBytes.Length > 0)
            {
                loadSucceeded = true;

                length = swfBytes.Length;
                unmanagedData = new IntPtr();
                unmanagedData = Marshal.AllocCoTaskMem((int)length);
                Marshal.Copy(swfBytes, 0, unmanagedData, (int)length);
            }
            else
            {
                Debug.Log("Unable to load swf from memory!");
            }
        }
        else
        {
            // no more to do here -- pass it on to the runtime.
            loadSucceeded = true;
        }


        string fullSwfPath = SFManager.GetScaleformContentPath() + swfName;
        creationParams = new SFMovieCreationParams(fullSwfPath, depth, ox, oy, width, height,
            unmanagedData, length, initFirstFrame, renderTarget, backgroundColor, useBackgroundColor, scaleModeType,
            autoManageViewport);


        if (movieName != null)
        {
            creationParams.MovieName = movieName;
        }

        if (loadSucceeded)
        {
            // create and return the instance
            GameObject uigo = new GameObject(Path.GetFileName(creationParams.MovieName));
            uigo.transform.parent = SFGFxMovieManager.Instance.uigroup.transform;
            T instance = uigo.AddComponent<T>();
            instance.Init(SFGFxMovieManager.Instance, camera.GetSFManager(), creationParams);
            //T instance = (T)Activator.CreateInstance(typeof(T), camera, camera.GetSFManager(), creationParams);

            return instance;
        }
        else
        {
            Debug.Log(String.Format("Unable to load swf named: {0}", swfName));
            return default(T);
        }
    }


    public static bool EqualsFloat(float a, float b,float exprence = 0.0001f)
    {
        float temp = a - b;
        temp = Mathf.Abs(temp);

        return temp < exprence;
    }
}
