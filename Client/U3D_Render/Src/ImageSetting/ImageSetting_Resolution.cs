/*******************************************************************
** 文件名:	ImageSetting_Resolution.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 分辨率

********************************************************************/
using UnityEngine;
using System;
using Effect;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GameLogic;

using System.Linq;
/// <summary>
/// 画面设置
/// </summary>
public partial class ImageSetting : MonoBehaviour
{
    //分辨率是比较特殊的，因为Unity会自动保存分辨率的，所以我们就无需通过配置来更改辨率了。
    //除非是配置里的分辨率与Unity自己的分辨率不一致,则使用配置里的分辨率修改unity的分辨率

    /// <summary>
    /// Unity支持的分辨率表
    /// </summary>
    private static HashSet<string> m_SupportResolutionList = new HashSet<string>();

    /// <summary>
    /// 纵横比
    /// </summary>
    private static Dictionary<int, List<string>> m_AspectRatioTable = new Dictionary<int, List<string>>();

    /// <summary>
    /// 所有支持的分辨率
    /// </summary>
    private static List<string> allSupportResolution = new List<string>();

    /// <summary>
    /// 系统分辨率
    /// </summary>
    public static string systemResolutions = string.Empty;

    /// <summary>
    /// 默认分辨率
    /// </summary>
    private static string defaultResoultions = string.Empty;
    private static WindowType defaultWindowType = WindowType.Invalid;

    /// <summary>
    /// 屏幕位置
    /// </summary>
    private static Rect screenPosition;

    private static IntPtr HWND_NORMAL = new IntPtr(-2);
    private static IntPtr HWND_TOPMOST = new IntPtr(-1);
    protected const uint SWP_SHOWWINDOW = 0x0040;
    protected const int GWL_STYLE = -16;
    protected const int NEW_LONG = 369164288;

    /// <summary>
    /// 是否支持全屏,有的显示器的分辨率，并不支持全屏，
    /// 这种情况下，就选择一个与目标显示器最接近的显示器分辨率
    /// </summary>
    public static bool isSupportFullScreen = false;

    public static string lastWindowResoultion = string.Empty;

    public static int minResolutionWidth = 1024;//1366;
    public static int minResolutionHeight = 576;//768;

    public static int RefershRate = 0;

    static void Add2K_4K_5K_Resolution(ref List<Resolution>  _allSupportResolution,int systemWidth,int systemHeight)
    {
        if(null == _allSupportResolution)
        {
            _allSupportResolution = new List<Resolution>();
        }

        if(systemHeight >= 1080 && systemWidth > 1920)
        {
            Resolution Resolution2KAndUp = new Resolution();
            Resolution2KAndUp.height = systemHeight;
            Resolution2KAndUp.width = systemWidth;
            Resolution2KAndUp.refreshRate = RefershRate;
            _allSupportResolution.Add(Resolution2KAndUp);
        }


        //Resolution Resolution2K_1 = new Resolution();
        //Resolution2K_1.height = 1080;
        //Resolution2K_1.width = 2560;
        //Resolution2K_1.refreshRate = RefershRate;
        //if (systemWidth >= Resolution2K_1.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution2K_1);
        //}
        

        //Resolution Resolution2K_2 = new Resolution();
        //Resolution2K_2.height = 1536;
        //Resolution2K_2.width = 2048;
        //Resolution2K_2.refreshRate = RefershRate;
        //if (systemWidth >= Resolution2K_2.width) //系统分辨率大于这个才可以加入进去
        //{
        //     _allSupportResolution.Add(Resolution2K_2);
        //}

        //Resolution Resolution2K_3 = new Resolution();
        //Resolution2K_3.height = 1600;
        //Resolution2K_3.width = 2560;
        //Resolution2K_3.refreshRate = RefershRate;
        //if (systemWidth >= Resolution2K_3.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution2K_3);
        //}

        //Resolution Resolution2K_4 = new Resolution();
        //Resolution2K_4.height = 1440;
        //Resolution2K_4.width = 2560;
        //Resolution2K_4.refreshRate = RefershRate;
        //if (systemWidth >= Resolution2K_4.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution2K_4);
        //}

        //Resolution Resolution4K_1 = new Resolution();
        //Resolution4K_1.height = 2160;
        //Resolution4K_1.width = 4096;
        //Resolution4K_1.refreshRate = RefershRate;
        //if (systemWidth >= Resolution4K_1.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution4K_1);
        //}

        //Resolution Resolution4K_2 = new Resolution();
        //Resolution4K_2.height = 2160;
        //Resolution4K_2.width = 3840;
        //Resolution4K_2.refreshRate = RefershRate;

        //if (systemWidth >= Resolution4K_2.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution4K_2);
        //}

        //Resolution Resolution4K_3 = new Resolution();
        //Resolution4K_1.height = 2304;
        //Resolution4K_1.width = 4096;
        //Resolution4K_1.refreshRate = RefershRate;
        //if (systemWidth >= Resolution4K_3.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution4K_3);
        //}

        //Resolution Resolution5K = new Resolution();
        //Resolution5K.height = 2880;
        //Resolution5K.width = 5120;
        //Resolution5K.refreshRate = RefershRate;
        //if (systemWidth >= Resolution5K.width) //系统分辨率大于这个才可以加入进去
        //{
        //    _allSupportResolution.Add(Resolution5K);
        //}
    }

    static void InitResolution()
    {
        if (Application.isEditor)
        {
            return;
        }
        //读取默认值，因为Unity有自己的默认值，这个默认值是在编辑器中的playersettings中设置的
        defaultResoultions = ConvertToString(minResolutionWidth, minResolutionHeight);
        defaultWindowType = WindowType.Window;

        int width = 0;
        int height = 0;
        GetMainMonitorSize(out width, out height);
        //系统的分辨率
        systemResolutions = ConvertToString(width,height);
		//取消全屏时用的是lastWindowResoultion，所以用配置表的值初始化他
        lastWindowResoultion = currData.currResolution;

        RefershRate = Screen.currentResolution.refreshRate;
        Debug.Log("系统分辨率:" + systemResolutions + ",刷新率:" + RefershRate);
        List<Resolution> _allSupportResolution;

        _allSupportResolution = Screen.resolutions.ToList();


        Add2K_4K_5K_Resolution(ref _allSupportResolution,width,height);

        //移除分辨率少于默认值的内容
        for(int i=_allSupportResolution.Count-1;i>=0;--i)
        {
            if(_allSupportResolution[i].width < minResolutionWidth)
            {
                _allSupportResolution.RemoveAt(i);
            }                
        }

        //将所有支持的分辨率全部插入
        foreach (Resolution res in _allSupportResolution)
        {
            InsertToAspectRatioTable(res);
        }

        //查找系统分辨率
        isSupportFullScreen = systemResolutions.Equals(FindSupportResolution(systemResolutions));

        if (!isSupportFullScreen)
        {
            systemResolutions = FindSupportResolution(systemResolutions);
        }


        //将所有支持的分辨率全部转化一遍
        foreach (Resolution res in _allSupportResolution)
        {
            string s = ConvertToString(res);
            string displayStr = s;
            //系统的分辨率，前面加一个*号
            if (s.Equals(systemResolutions))
            {
                displayStr = "*" + s;
            }

            if (!m_SupportResolutionList.Contains(s))
            {
                m_SupportResolutionList.Add(s);
                //这个是显示链表
                allSupportResolution.Add(displayStr);
            }
        }


        //当前游戏的分辨率
        string defaultResStr = ConvertToString(Screen.width, Screen.height);

        //如果配置里没有分辨率，则使用全屏的分辨率
        if (currData.currResolution.Equals(string.Empty))
        {
            currData.currResolution = systemResolutions;
            currData.currWindowType = WindowType.FullScreen;

            SetResolution(systemResolutions, true);
        }
        else
        {
            bool bFullScreen = true;
            //看看配置里的分辨率是否与Unity当前的分辨率一样
            if (IsTheSameResolutio(currData.currResolution, defaultResStr))
            {
                ////一样的话，再看看窗口类型是否一致
                //if (!IsTheSameWindowType())
                //{
                    
                //}
            }
            Resolution old = ConvertToResolution(currData.currResolution);
            Resolution sys = ConvertToResolution(systemResolutions);
            //如果配置里的分辨率比系统的要大，使用系统分辨率，并且全屏
            if(old.height * old.width > sys.width * sys.height)
            {
                currData.currResolution = systemResolutions;
                currData.currWindowType = WindowType.FullScreen;
                currData.currResolution = systemResolutions;
            }

            bFullScreen = currData.currWindowType == WindowType.FullScreen;
			//全屏的话用系统当前分辨率
            string newResolution = bFullScreen ? systemResolutions : currData.currResolution;
            currData.currResolution = newResolution;
            SetResolution(newResolution, bFullScreen);
        }

    }

    public static int GetResolutionPixels(string srcResulution)
    {
        Resolution res = ConvertToResolution(srcResulution);
        return res.width * res.height;
    }

    /// <summary>
    /// 根据给出的像素和缩放值，来匹配对应的分辨率
    /// </summary>
    /// <param name="pixels"></param>
    /// <param name="scale"></param>
    /// <returns></returns>
    public static string AdjustResolutionFromPixels(int pixels,float scale)
    {
        pixels = (int)(pixels * scale);

        float min = -1;
        string TargetStr = defaultResoultions;
        foreach (string res in m_SupportResolutionList)
        {
            Resolution rt = ConvertToResolution(res);
            float currPixels = rt.width * rt.height;
            if(currPixels < pixels)
            {
                currPixels /= pixels;
                if(min < currPixels)
                {
                    min = currPixels;
                    TargetStr = res;
                }
            }
            else
            {
                break;
            }
        }
        return TargetStr;
    }

    static string FindSupportResolution(string srcResolution)
    {
        string Reslut = string.Empty;

        Resolution res = ConvertToResolution(srcResolution);

        int Ratio = GetAspectRation(res);
        List<string> resList = null;

        //这个分辨率是支持的，直接返回源分辨率
        if(m_AspectRatioTable.TryGetValue(Ratio,out resList))
        {
            return srcResolution;
        }

        int dealt = int.MaxValue;
        int correctRatio = 0;
        //不支持的话，找一个最接近的纵横比
        foreach(int currAsp in m_AspectRatioTable.Keys)
        {
            int temp = Mathf.Abs(currAsp - Ratio);
            if(temp < dealt)
            {
                dealt = temp;
                correctRatio = currAsp;
            }
        }

        dealt = int.MaxValue;
        //找到之后，根据高度选则一个分辨率
        m_AspectRatioTable.TryGetValue(correctRatio, out resList);
        foreach(string current in resList)
        {
            Resolution CurrRes = ConvertToResolution(current);
            int temp = Mathf.Abs(CurrRes.height - res.height);
            if(temp < dealt)
            {
                dealt = temp;
                Reslut = current;
            }
        }

        if(Reslut.Equals(string.Empty))
        {
            Debug.LogError("找不到支持的分辨率:" + srcResolution + "采用默认分辨率");
            Reslut = defaultResoultions;
        }
        else
        {
            Debug.Log("适配分辨:" + srcResolution + "成功:" + Reslut);
        }
        return Reslut;
    }

    static void ClearResolution()
    {
        if (Application.isEditor)
        {
            return;
        }
        m_SupportResolutionList.Clear();
        allSupportResolution.Clear();
    }

    static void SetDefualtResolution()
    {
        if (Application.isEditor)
        {
            return;
        }
        SetResolution(defaultResoultions,false);
    }

    static bool IsTheSameResolutio(string str1, string str2)
    {
        str1 = str1.Replace("*", "");
        str2 = str2.Replace("*", "");
        return str1.Equals(str2);
    }

    public static List<string> GetSupportResolutionList()
    {
        return allSupportResolution;
    }

    public static int GetResolutionIndex(string resStr)
    {
        //非系统的要把*去掉
        if (!IsTheSameResolutio(resStr, systemResolutions))
        {
            resStr = resStr.Replace("*", "");
        }
        else
        {
            //系统分辨率加上个*号
            resStr = "*" + systemResolutions;
        }
        return allSupportResolution.IndexOf(resStr);
    }

    public static bool IsTheSameWindowType()
    {
        //对于窗口最大化，unity始终返回false，因为unity没有这个，需要重新设置
        if (currData.currWindowType == WindowType.WindowMaxSize)
        {
            return false;
        }

        if (Screen.fullScreen)
        {
            if (currData.currWindowType == WindowType.FullScreen)
            {
                return true;
            }
        }
        else
        {
            if (currData.currWindowType == WindowType.Window)
            {
                return true;
            }
        }
        return false;
    }


    /// <summary>
    /// 设置分辨率会同时设置窗口，unity的限制，没办法
    /// </summary>
    /// <param name="resStr"></param>
    public static void SetResolution(string resStr,bool bFullScreen,bool bforce = false)
    {
        if (Application.isEditor)
        {
            return;
        }

        resStr = resStr.Replace("*", "");
        currData.currWindowType = bFullScreen ? WindowType.FullScreen : WindowType.Window;
        if(!bforce)
        {
            if (currData.currResolution.Equals(resStr) && OldWindowType == currData.currWindowType)
            {
                return;
            }
        }

        OldWindowType = currData.currWindowType;
       
        if (!m_SupportResolutionList.Contains(resStr))
        {
            //寻找支持的分辨率
            resStr = FindSupportResolution(resStr);

            //当不是支持的分辨率时，不能全屏
            bFullScreen = false;
        }

        Resolution res = ConvertToResolution(resStr);
        SetResolution(res.width, res.height, bFullScreen);
        //全屏不记录进用户配置表，以便取消全屏时可以恢复之前设置的分辨率
        if (!bFullScreen)
        {
            currData.currResolution = resStr;
            lastWindowResoultion = currData.currResolution;
        }
    }
    /// <summary>
    /// 重设当前的分辨率
    /// </summary>
    public static void ResetCurrentResolution()
    {
        string resStr = currData.currResolution.Replace("*", "");
        bool bFullScreen = currData.currWindowType == WindowType.Window ? false : true;
        
        if(bFullScreen)
        {
            resStr = systemResolutions.Replace("*", "");
        }
        if (!m_SupportResolutionList.Contains(resStr))
        {
            //寻找支持的分辨率
            resStr = FindSupportResolution(resStr);

            //当不是支持的分辨率时，不能全屏
            bFullScreen = false;
        }

        Resolution res = ConvertToResolution(resStr);
        SetResolution(res.width, res.height, bFullScreen);

    }

    public static Resolution GetCurrentResolution()
    {
        if (Application.isEditor)
        {
            return Screen.currentResolution;
        }

        return ConvertToResolution(currData.currResolution);
    }

    private static void SetResolution(int _nNewWidth, int _nNewHeight, bool bFullScreen)
    {
        //全屏
        if (bFullScreen && isSupportFullScreen)
        {
            Screen.SetResolution(_nNewWidth, _nNewHeight, false);
            screenPosition = new Rect(0, 0, _nNewWidth, _nNewHeight);

            //设定一个新的窗口风格
            GameLogicAPI.SetWindowLong(Initialize.m_WindowHwnd, GWL_STYLE, NEW_LONG);
            GameLogicAPI.SetWindowPos(Initialize.m_WindowHwnd, HWND_NORMAL, (int)screenPosition.x, (int)screenPosition.y, (int)screenPosition.width, (int)screenPosition.height, SWP_SHOWWINDOW);

            GameLogicAPI.SetWindowLong(Initialize.m_WindowHwnd, GWL_STYLE, NEW_LONG);
            GameLogicAPI.SetWindowPos(Initialize.m_WindowHwnd, HWND_NORMAL, (int)screenPosition.x, (int)screenPosition.y, (int)screenPosition.width, (int)screenPosition.height, SWP_SHOWWINDOW);

            GameLogicAPI.SetWindowLong(Initialize.m_WindowHwnd, GWL_STYLE, NEW_LONG);
            GameLogicAPI.SetWindowPos(Initialize.m_WindowHwnd, HWND_NORMAL, (int)screenPosition.x, (int)screenPosition.y, (int)screenPosition.width, (int)screenPosition.height, SWP_SHOWWINDOW);
        }
        //非全屏
        else
        {
            Screen.SetResolution(_nNewWidth, _nNewHeight, false);
            GameLogicAPI.SetWindowPos(Initialize.m_WindowHwnd, HWND_NORMAL, 0, 0, 0, 0, 0x0001 | 0x0002);
            GameLogicAPI.SetWindowPos(Initialize.m_WindowHwnd, HWND_NORMAL, 0, 0, 0, 0, 0x0001 | 0x0002 | 0x0020);
        }

        if (USpeedUI.UISystem.Instance)
        {
            USpeedUI.UISystem.Instance.SetResolution(_nNewWidth, _nNewHeight);
        }
        SceneResInfoManager.API_OnResolutionChanged(_nNewWidth, _nNewHeight);
    }

    private static string ConvertToString(Resolution re)
    {
        string st = string.Empty;
        st += re.width;
        st += "X";
        st += re.height;
        return st;
    }

    private static string ConvertToString(int width, int height)
    {
        string st = string.Empty;
        st += Mathf.Abs(width);
        st += "X";
        st += Mathf.Abs(height);
        return st;
    }

    private static Resolution ConvertToResolution(string str)
    {
        if(Application.isEditor)
        {
            return Screen.currentResolution;
        }
        string[] sp = str.Split('X');

        if(sp.Length != 2)
        {
            return ConvertToResolution(defaultResoultions);
        }
        Resolution res = new Resolution();
        int width = 0;
        int height = 0;
        if(!int.TryParse(sp[0],out width) || !int.TryParse(sp[1],out height))
        {
            return ConvertToResolution(defaultResoultions);
        }

        res.width = width;
        res.height = height;
        res.refreshRate = RefershRate;
        return res;
    }

    private static void InsertToAspectRatioTable(Resolution res)
    {   int IntRoat = GetAspectRation(res);

        List<string> resList = null;
        if(!m_AspectRatioTable.TryGetValue(IntRoat,out resList))
        {
            resList = new List<string>();
            m_AspectRatioTable.Add(IntRoat, resList);
        }
        string strRes = ConvertToString(res);
        if(!resList.Contains(strRes))
        {
            resList.Add(strRes);
        }

    }

    private static int GetAspectRation(Resolution res)
    {
        float roat = (float)res.width / (float)res.height;
        int IntRoat = (int)(roat * 100);

        return IntRoat;
    }

    [DllImport("game.dll")]
    public static extern void GetMainMonitorSize(out int width,out int height);
}