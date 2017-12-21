/*******************************************************************
** 文件名:	ImageSetting_Main.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板

********************************************************************/
using UnityEngine;
using System;
using Effect;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GameLogic;
/* 增加一个配置所需要做的事情:
 * 1.在ImageSetting_Enum增加对应的枚举
 * 2.新建对应的文件用于放对应的函数,如:ImageSetting_Texture就放贴图相关的配置
 * 3.在ImageSetting_Data中的LoadConfig和ExportConfig到处对应的配置
 * 4.在SetImageQuality__XX和Apply中添加对应的函数调用
 */
/// <summary>
/// 画面设置
/// </summary>
public partial class ImageSetting :MonoBehaviour
{
    public const string ConfigFileName = "ImageSetting.csv";

    private static GameObject MonoInstance = null;
    private static bool bInit = false;
    public static void Init()
    {
        //InitUGUI();
       
        if(bInit)
        {
            return;
        }
        bInit = true;
        //如果加载配置失败，则使用默认的配置
        bool bLoadConfig = LoadConfig();
        if (!MonoInstance)
        {
            MonoInstance = new GameObject();
            MonoInstance.AddComponent<ImageSetting>().hideFlags = HideFlags.NotEditable;
            MonoInstance.name = "ImageSetting";
            DontDestroyOnLoad(MonoInstance);
        }
        InitResolution();

        //有开启自动调整,参数设置为预设的值
        if (bLoadConfig /*&& AutoAdjustQuality > 0*/)
        {
            SetImageQuality(currData.currImageQuality);
        }
        else
        {
            //加载配置失败就使用默认配置
            if (bLoadConfig)
            {
                //没有开启自动调整，直接应用参数,采用玩家的值
                Apply(true);

            }
            else
            {
                SetDefaultSetting();
            }

        }

        ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled = currData.AdvanceImageEffectEnabled == 1;
        //ApplyUIData();
        GameFrameManager.FpsMonitor.SubScribeProfileEvent(OnFPSChange);
        SceneResInfoManager.RegisterActiveSceneChangedEvent(ActiveSceneChaned);
        SceneResInfoManager.RegsiterGetImageSettignData(GetData);
    }

    public static ImageSettingData GetData()
    {
        return currData;
    }

    public static void Clear()
    {
        bInit = false;
        ClearResolution();
        //ClearUGUI();
        
        GameFrameManager.FpsMonitor.UnSubScribeProfileEvent(OnFPSChange);
        //清除的时候导出一下配置
        ExportConfig();

        if (MonoInstance)
        {
            ResNode.DestroyRes(ref MonoInstance);
            MonoInstance = null;
        }
    }

    public static void TraceSettings()
    {
        Trace.Log("电脑信息:");
        //string hostname = System.Net.Dns.GetHostName(); //主机
        //System.Net.IPHostEntry ipEntry = System.Net.Dns.GetHostEntry(hostname);//网卡IP地址集合
        //string IP = ipEntry.AddressList[0].ToString();//取一个IP
       // Trace.Log("IP:" + IP);
        Trace.Log("isEditor:" + Application.isEditor);
        Trace.Log("CPU:" + SystemInfo.processorType);
        Trace.Log("显卡:" + SystemInfo.graphicsDeviceName);
        Trace.Log("渲染器:" + SystemInfo.graphicsDeviceVersion);
        Trace.Log("显存:" + SystemInfo.graphicsMemorySize);
        Trace.Log("ShaderAPI:" + SystemInfo.graphicsShaderLevel);
        Trace.Log("SupportRT:" + SystemInfo.supportedRenderTargetCount);
        Trace.Log("内存:" + SystemInfo.systemMemorySize);
        Trace.Log("系统:" + SystemInfo.operatingSystem);
        Trace.Log("ScreenResolutio:" + currData.currResolution);
    }

    static void OnFPSChange(int duration, float avgFPS)
    {
        AdjustLogic(avgFPS);
    }

    [DllImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern bool GetWindowRect(IntPtr hWnd, ref RECT lpRect);
    [DllImport("user32.dll", CharSet = CharSet.Auto)]
    private static extern int SetWindowPos(IntPtr hWnd, int hWndInsertAfter, int x, int y, int Width, int Height, int flags); 

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int Left;                             //最左坐标
        public int Top;                             //最上坐标
        public int Right;                           //最右坐标
        public int Bottom;                        //最下坐标
    }
    const int HWND_TOP = 0;
    const int SWP_NOSIZE = 1;
    float checkTime = 0.5f;
    float currCheckTime = 10.0f;
    public void LateUpdate()
    {
        if(Application.isEditor)
        {
            return;
        }
        currCheckTime += Time.deltaTime;
        if(currCheckTime < checkTime)
        {
            return;
        }
        currCheckTime = 0.0f;
        if (currData.currWindowType == WindowType.FullScreen && isSupportFullScreen)
        {
            RECT ret = default(RECT);
            GetWindowRect(Initialize.m_WindowHwnd, ref ret);
            if(ret.Top >= 1 || ret.Left >= 1)
            {
                SetWindowPos(Initialize.m_WindowHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
            }
          
        }
    }


    
}