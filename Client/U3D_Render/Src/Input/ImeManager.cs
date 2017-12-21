using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections;
using Microsoft.Win32;
using UnityEngine;
using USpeedUI;

/// <summary>
/// 输入法管理类
/// </summary>
public sealed class ImeManager : MonoBehaviour
{
    //系统版本
    public enum SystemVersion
    {
        SysVer_UnKnown,     //未知
        SysVer_7minus,      //win7以下
        SysVer_7,           //win7
        Sysver_8plus,       //win8+(包括win8)
    }

    /// <summary>
    /// 是否限制输入法(XP目前不限制输入法)
    /// </summary>
    public static bool ForbidIme 
    { 
        get { return bForbidIme; } 
    }
    private static bool bForbidIme;

    /// <summary>
    /// 当前输入法状态
    /// </summary>
    public static bool EnableIme 
    { 
        get { return bEnableIme; } 
        set { bEnableIme = value; } 
    }
    private static bool bEnableIme;

    //激活Flag
    public const uint KLF_ACTIVATE = 1;
    //缓存输入法句柄
    private static IntPtr cacheImeIntPtr;
    //上一个输入法
    private static IntPtr PrevImeIntPtr;
    //默认玩家输入法
    private static IntPtr defaultPlayerImeIntPtr;
    //默认英文输入法
    private static IntPtr defaultEnImeIntPtr;
    //当前输入法
    private static IntPtr currentImeIntPtr;
    //输入法名称句柄匹配表
    private static Dictionary<string, IntPtr> ImeList;
    //英文名称
    private static string szEnName;
    //IME名称
    private static string szImeName;
    //当前系统版本
    private static SystemVersion systemVersion;

    private static bool bInit;

    public void Start()
    {
        bForbidIme = true;
        bEnableIme = false;

        cacheImeIntPtr = IntPtr.Zero;
        PrevImeIntPtr = IntPtr.Zero;
        defaultEnImeIntPtr = IntPtr.Zero;
        currentImeIntPtr = IntPtr.Zero;
        defaultPlayerImeIntPtr = IntPtr.Zero;

        if (!CheckSystemVersion())
            return;

        InitImes();

        Initialize.onApplicationFocusChangeEvent.AddListener(OnFocusChange);

        bInit = true;
    }

    public void OnDestroy()
    {
        bEnableIme = true;
        bForbidIme = false;

        Initialize.onApplicationFocusChangeEvent.RemoveListener(OnFocusChange);
        GameLogicAPI.ActivateKeyboardLayout(defaultPlayerImeIntPtr, KLF_ACTIVATE);

        cacheImeIntPtr = IntPtr.Zero;
        PrevImeIntPtr = IntPtr.Zero;
        defaultEnImeIntPtr = IntPtr.Zero;
        currentImeIntPtr = IntPtr.Zero;
        defaultPlayerImeIntPtr = IntPtr.Zero;

        szEnName = "";
        szImeName = "";

        ImeList.Clear();
        ImeList = null;

        bInit = false;
    }

    public void Update()
    {
        if (!bForbidIme || !Initialize.isOnFocus)
            return;

        currentImeIntPtr = GameLogicAPI.GetKeyboardLayout(0);
        if(bEnableIme)
        {
            if (currentImeIntPtr != PrevImeIntPtr)
            {
                PrevImeIntPtr = currentImeIntPtr;
                szImeName = "";
                bool isInImeList = false;
                foreach (KeyValuePair<string, IntPtr> temp in ImeList)
                {
                    if (temp.Value == currentImeIntPtr)
                    {
                        szImeName = temp.Key;
                        isInImeList = true;
                    }
                }
                ChatBoxChangeImeData imeData = new ChatBoxChangeImeData();
                imeData.bIsInImeList = isInImeList;
                imeData.name = szImeName;
                imeData.bIsIme = szImeName == szEnName ? false : true;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGEIME, imeData);
            }
        }
            //屏蔽输入法切换
        else
        {
            //if (currentIme != defaultIme)
                GameLogicAPI.ActivateKeyboardLayout(defaultEnImeIntPtr, KLF_ACTIVATE);
        }
    }

    //激活/关闭输入法
    public static void ActiveIme(bool bEnable)
    {
        if (!bInit || !bForbidIme)
            return;

        bEnableIme = bEnable;
        if (bEnableIme)
        {
            GameLogicAPI.ActivateKeyboardLayout(cacheImeIntPtr, KLF_ACTIVATE);
            currentImeIntPtr = GameLogicAPI.GetKeyboardLayout(0);
            szImeName = "";
            bool isInImeList = false;

            foreach (KeyValuePair<string, IntPtr> temp in ImeList)
            {
                if (temp.Value == currentImeIntPtr)
                {
                    szImeName = temp.Key;
                    isInImeList = true;
                }
            }
            ChatBoxChangeImeData imeData = new ChatBoxChangeImeData();
            imeData.bIsInImeList = isInImeList;
            imeData.name = szImeName;
            imeData.bIsIme = szImeName == szEnName ? false : true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGEIME, imeData);
        }
        else
        {
            cacheImeIntPtr = currentImeIntPtr;
            GameLogicAPI.ActivateKeyboardLayout(defaultEnImeIntPtr, KLF_ACTIVATE);
            ChatBoxChangeImeData imeData = new ChatBoxChangeImeData();
            imeData.bIsInImeList = false;
            imeData.name = szEnName;
            imeData.bIsIme = false;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGEIME, imeData);
        }
    }

    //切换输入法方法(UI点击切换)
    public static void SwitchIme(string ImeName)
    {
        if (!bForbidIme)
            return;

        if (ImeName == szEnName)
        {
            GameLogicAPI.ActivateKeyboardLayout(defaultEnImeIntPtr, KLF_ACTIVATE);
            cacheImeIntPtr = defaultEnImeIntPtr;
            return;
        }
        if (ImeList.ContainsKey(ImeName))
        {
            GameLogicAPI.ActivateKeyboardLayout(ImeList[ImeName], KLF_ACTIVATE);
            cacheImeIntPtr = GameLogicAPI.GetKeyboardLayout(0);
        }
    }

    public static void OnFocusChange(bool bWindowFocus)
    {
        if (bWindowFocus)
        {
            GameLogicAPI.ActivateKeyboardLayout(currentImeIntPtr, KLF_ACTIVATE);
        }
        else
        {
            GameLogicAPI.ActivateKeyboardLayout(defaultPlayerImeIntPtr, KLF_ACTIVATE);
        }
    }

    public static List<string> GetImeNameList()
    {
        if (!bInit)
            return null;

        if (null == ImeList || ImeList.Count == 0)
            return null;

        List<string> nameList = new List<string>();
        foreach (string name in ImeList.Keys)
        {
            nameList.Add(name);
        }
        return nameList;
    }

    //初始化统输入法列表
    private static void InitImes()
    {
        if (ImeList == null)
            ImeList = new Dictionary<string,IntPtr>();

        defaultPlayerImeIntPtr = GameLogicAPI.GetKeyboardLayout(0);
        cacheImeIntPtr = PrevImeIntPtr = defaultPlayerImeIntPtr;

        if (systemVersion == SystemVersion.SysVer_7minus)
        {
            bForbidIme = false;
            return;
        }
        else if(systemVersion == SystemVersion.SysVer_7)
        {
            //默认载入中文简体--美式键盘
            GameLogicAPI.LoadKeyboardLayout("00000804", KLF_ACTIVATE);
        }
        else if(systemVersion == SystemVersion.Sysver_8plus)
        {
            GameLogicAPI.LoadKeyboardLayout("00000409", KLF_ACTIVATE);
        }

        defaultEnImeIntPtr = GameLogicAPI.GetKeyboardLayout(0);
        szImeName = szEnName = ULocalizationService.Instance.Get("UIView", "ChatBox", "EN");

        //加入输入法列表中
        ImeList.Add(szEnName, defaultEnImeIntPtr);

        IntPtr[] keyboardlist = new IntPtr[5];
        //获取输入法列表
        int count = GameLogicAPI.GetKeyboardLayoutList(5, keyboardlist);
        //遍历输入法列表信息，存储到imeList中
        for (int i = 0; i < (int)count; i++)
        {
            if (keyboardlist[i] == IntPtr.Zero)
                continue;

            //////////////////////////////////Win8////////////////////////////////////////
            if (systemVersion == SystemVersion.Sysver_8plus)
            {
                if (keyboardlist[i] != defaultEnImeIntPtr)
                {
                    ImeList.Add(ULocalizationService.Instance.Get("UIView", "ChatBox", "CH"), keyboardlist[i]);
                    break;
                }
            }
            //////////////////////////////Win7////////////////////////////////////////////
            else
            { 
                uint layout = (uint)keyboardlist[i];
                string RegKey = String.Format("System\\CurrentControlSet\\Control\\Keyboard Layouts\\{0:X8}", layout);
                RegistryKey rk = Registry.LocalMachine.OpenSubKey(RegKey);

                if (rk == null)
                    continue;

                string ImeName = (string)rk.GetValue("layout text");

                if (ImeName == null)
                    continue;

                if (ImeList.ContainsKey(ImeName))
                    ImeList[ImeName] = keyboardlist[i];
                else
                    ImeList.Add(ImeName, keyboardlist[i]);
            }
        }
    }

    /// <summary>
    /// 检测系统
    /// </summary>
    private static bool CheckSystemVersion()
    {
        systemVersion = SystemVersion.SysVer_UnKnown;

        Version currentVersion = Environment.OSVersion.Version;

        if(null == currentVersion)
            return false;

        if (currentVersion.CompareTo(new Version("6.2")) >= 0)
        {
            systemVersion = SystemVersion.Sysver_8plus;
        }
        else
        {
            if (currentVersion.CompareTo(new Version("6.1")) > 0)
                systemVersion = SystemVersion.SysVer_7;
            else
                systemVersion = SystemVersion.SysVer_7minus;
        }
        return true;
    }
}
