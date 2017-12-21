/*******************************************************************
** 文件名:	InputConvertHelper.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	键盘键位转换辅助   
** 应  用:    字符串—KeyCode数组互转
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public static class InputConvertHelper
{
    /// <summary>
    /// 键位转换对照表
    /// </summary>
    private static Dictionary<string, KeyCode[]> m_dicKeyCodeConvertMap;

    static InputConvertHelper()
    {
        m_dicKeyCodeConvertMap = new Dictionary<string, KeyCode[]>();
        InitConvertMap();
    }

    public static void Clear()
    {
        m_dicKeyCodeConvertMap.Clear();
        m_dicKeyCodeConvertMap = null;
    }

    //
    public static KeyCode[] String2KeyCodeArry(string szCode)
    {
        KeyCode[] codeArry = null;
        if (m_dicKeyCodeConvertMap == null)
            return codeArry;

        if (m_dicKeyCodeConvertMap.ContainsKey(szCode))
        {
            codeArry = m_dicKeyCodeConvertMap[szCode];
        }
        return codeArry;
    }

    public static string KeyCode2String(KeyCode keycode)
    {
        string szCode = "";
        if (m_dicKeyCodeConvertMap == null)
            return szCode;

        foreach (KeyValuePair<string, KeyCode[]> kvp in m_dicKeyCodeConvertMap)
        {
            if(kvp.Value.ToList().Exists(x => x == keycode))
            {
                szCode = kvp.Key;
            }
        }
        return szCode;
    }

    static void InitConvertMap()
    {
        m_dicKeyCodeConvertMap.Clear();

        m_dicKeyCodeConvertMap.Add("BackSpace", new KeyCode[1] { KeyCode.Backspace });
        m_dicKeyCodeConvertMap.Add("Tab", new KeyCode[1] { KeyCode.Tab });
        m_dicKeyCodeConvertMap.Add("Enter", new KeyCode[2] { KeyCode.Return, KeyCode.KeypadEnter });
        m_dicKeyCodeConvertMap.Add("Space", new KeyCode[1] { KeyCode.Space });
        m_dicKeyCodeConvertMap.Add("'", new KeyCode[1] { KeyCode.Quote });        // 单引号
        m_dicKeyCodeConvertMap.Add(",", new KeyCode[1] { KeyCode.Comma });        // ，
        m_dicKeyCodeConvertMap.Add("-", new KeyCode[2] { KeyCode.Minus, KeyCode.KeypadMinus }); // -
        m_dicKeyCodeConvertMap.Add(".", new KeyCode[2] { KeyCode.Period, KeyCode.KeypadPeriod }); // .
        m_dicKeyCodeConvertMap.Add("/", new KeyCode[2] { KeyCode.Slash, KeyCode.KeypadDivide });  // /
        m_dicKeyCodeConvertMap.Add("0", new KeyCode[2] { KeyCode.Alpha0, KeyCode.Keypad0 });
        m_dicKeyCodeConvertMap.Add("1", new KeyCode[2] { KeyCode.Alpha1, KeyCode.Keypad1 });
        m_dicKeyCodeConvertMap.Add("2", new KeyCode[2] { KeyCode.Alpha2, KeyCode.Keypad2 });
        m_dicKeyCodeConvertMap.Add("3", new KeyCode[2] { KeyCode.Alpha3, KeyCode.Keypad3 });
        m_dicKeyCodeConvertMap.Add("4", new KeyCode[2] { KeyCode.Alpha4, KeyCode.Keypad4 });
        m_dicKeyCodeConvertMap.Add("5", new KeyCode[2] { KeyCode.Alpha5, KeyCode.Keypad5 });
        m_dicKeyCodeConvertMap.Add("6", new KeyCode[2] { KeyCode.Alpha6, KeyCode.Keypad6 });
        m_dicKeyCodeConvertMap.Add("7", new KeyCode[2] { KeyCode.Alpha7, KeyCode.Keypad7 });
        m_dicKeyCodeConvertMap.Add("8", new KeyCode[2] { KeyCode.Alpha8, KeyCode.Keypad8 });
        m_dicKeyCodeConvertMap.Add("9", new KeyCode[2] { KeyCode.Alpha9, KeyCode.Keypad9 });
        m_dicKeyCodeConvertMap.Add(";", new KeyCode[1] { KeyCode.Semicolon });    // ;
        m_dicKeyCodeConvertMap.Add("=", new KeyCode[1] { KeyCode.Equals });       // =
        m_dicKeyCodeConvertMap.Add("[", new KeyCode[1] { KeyCode.LeftBracket });  // [
        m_dicKeyCodeConvertMap.Add("BackSlash", new KeyCode[1] { KeyCode.Backslash }); // \
        m_dicKeyCodeConvertMap.Add("]", new KeyCode[1] { KeyCode.RightBracket });      // ]
        m_dicKeyCodeConvertMap.Add("`", new KeyCode[1] { KeyCode.BackQuote });         // `
        m_dicKeyCodeConvertMap.Add("A", new KeyCode[1] { KeyCode.A });
        m_dicKeyCodeConvertMap.Add("B", new KeyCode[1] { KeyCode.B });
        m_dicKeyCodeConvertMap.Add("C", new KeyCode[1] { KeyCode.C });
        m_dicKeyCodeConvertMap.Add("D", new KeyCode[1] { KeyCode.D });
        m_dicKeyCodeConvertMap.Add("E", new KeyCode[1] { KeyCode.E });
        m_dicKeyCodeConvertMap.Add("F", new KeyCode[1] { KeyCode.F });
        m_dicKeyCodeConvertMap.Add("G", new KeyCode[1] { KeyCode.G });
        m_dicKeyCodeConvertMap.Add("H", new KeyCode[1] { KeyCode.H });
        m_dicKeyCodeConvertMap.Add("I", new KeyCode[1] { KeyCode.I });
        m_dicKeyCodeConvertMap.Add("J", new KeyCode[1] { KeyCode.J });
        m_dicKeyCodeConvertMap.Add("K", new KeyCode[1] { KeyCode.K });
        m_dicKeyCodeConvertMap.Add("L", new KeyCode[1] { KeyCode.L });
        m_dicKeyCodeConvertMap.Add("M", new KeyCode[1] { KeyCode.M });
        m_dicKeyCodeConvertMap.Add("N", new KeyCode[1] { KeyCode.N });
        m_dicKeyCodeConvertMap.Add("O", new KeyCode[1] { KeyCode.O });
        m_dicKeyCodeConvertMap.Add("P", new KeyCode[1] { KeyCode.P });
        m_dicKeyCodeConvertMap.Add("Q", new KeyCode[1] { KeyCode.Q });
        m_dicKeyCodeConvertMap.Add("R", new KeyCode[1] { KeyCode.R });
        m_dicKeyCodeConvertMap.Add("S", new KeyCode[1] { KeyCode.S });
        m_dicKeyCodeConvertMap.Add("T", new KeyCode[1] { KeyCode.T });
        m_dicKeyCodeConvertMap.Add("U", new KeyCode[1] { KeyCode.U });
        m_dicKeyCodeConvertMap.Add("V", new KeyCode[1] { KeyCode.V });
        m_dicKeyCodeConvertMap.Add("W", new KeyCode[1] { KeyCode.W });
        m_dicKeyCodeConvertMap.Add("X", new KeyCode[1] { KeyCode.X });
        m_dicKeyCodeConvertMap.Add("Y", new KeyCode[1] { KeyCode.Y });
        m_dicKeyCodeConvertMap.Add("Z", new KeyCode[1] { KeyCode.Z });
        m_dicKeyCodeConvertMap.Add("Delete", new KeyCode[1] { KeyCode.Delete });
        m_dicKeyCodeConvertMap.Add("*", new KeyCode[1] { KeyCode.KeypadMultiply });
        m_dicKeyCodeConvertMap.Add("+", new KeyCode[1] { KeyCode.KeypadPlus });
        m_dicKeyCodeConvertMap.Add("UpArrow", new KeyCode[1] { KeyCode.UpArrow });
        m_dicKeyCodeConvertMap.Add("DownArrow", new KeyCode[1] { KeyCode.DownArrow });
        m_dicKeyCodeConvertMap.Add("RightArrow", new KeyCode[1] { KeyCode.RightArrow });
        m_dicKeyCodeConvertMap.Add("LeftArrow", new KeyCode[1] { KeyCode.LeftArrow });
        m_dicKeyCodeConvertMap.Add("Insert", new KeyCode[1] { KeyCode.Insert });
        m_dicKeyCodeConvertMap.Add("Home", new KeyCode[1] { KeyCode.Home });
        m_dicKeyCodeConvertMap.Add("End", new KeyCode[1] { KeyCode.End });
        m_dicKeyCodeConvertMap.Add("PageUp", new KeyCode[1] { KeyCode.PageUp });
        m_dicKeyCodeConvertMap.Add("PageDown", new KeyCode[1] { KeyCode.PageDown });
        m_dicKeyCodeConvertMap.Add("F1", new KeyCode[1] { KeyCode.F1 });
        m_dicKeyCodeConvertMap.Add("F2", new KeyCode[1] { KeyCode.F2 });
        m_dicKeyCodeConvertMap.Add("F3", new KeyCode[1] { KeyCode.F3 });
        m_dicKeyCodeConvertMap.Add("F4", new KeyCode[1] { KeyCode.F4 });
        m_dicKeyCodeConvertMap.Add("F5", new KeyCode[1] { KeyCode.F5 });
        m_dicKeyCodeConvertMap.Add("F6", new KeyCode[1] { KeyCode.F6 });
        m_dicKeyCodeConvertMap.Add("F7", new KeyCode[1] { KeyCode.F7 });
        m_dicKeyCodeConvertMap.Add("F8", new KeyCode[1] { KeyCode.F8 });
        m_dicKeyCodeConvertMap.Add("F9", new KeyCode[1] { KeyCode.F9 });
        m_dicKeyCodeConvertMap.Add("F10", new KeyCode[1] { KeyCode.F10 });
        m_dicKeyCodeConvertMap.Add("F11", new KeyCode[1] { KeyCode.F11 });
        m_dicKeyCodeConvertMap.Add("F12", new KeyCode[1] { KeyCode.F12 });
        m_dicKeyCodeConvertMap.Add("NumLock", new KeyCode[1] { KeyCode.Numlock });
        m_dicKeyCodeConvertMap.Add("CapsLock", new KeyCode[1] { KeyCode.CapsLock });
        m_dicKeyCodeConvertMap.Add("ScrollLock", new KeyCode[1] { KeyCode.ScrollLock });
        m_dicKeyCodeConvertMap.Add("Shift", new KeyCode[2] { KeyCode.RightShift, KeyCode.LeftShift });
        m_dicKeyCodeConvertMap.Add("Ctrl", new KeyCode[2] { KeyCode.RightControl, KeyCode.LeftControl });
        m_dicKeyCodeConvertMap.Add("Alt", new KeyCode[2] { KeyCode.RightAlt, KeyCode.LeftAlt });
        m_dicKeyCodeConvertMap.Add("Mouse0", new KeyCode[1] { KeyCode.Mouse0 });
        m_dicKeyCodeConvertMap.Add("Mouse1", new KeyCode[1] { KeyCode.Mouse1 });
        m_dicKeyCodeConvertMap.Add("Mouse2", new KeyCode[1] { KeyCode.Mouse2 });
        m_dicKeyCodeConvertMap.Add("Mouse3", new KeyCode[1] { KeyCode.Mouse3 });
        m_dicKeyCodeConvertMap.Add("Mouse4", new KeyCode[1] { KeyCode.Mouse4 });
        m_dicKeyCodeConvertMap.Add("Mouse5", new KeyCode[1] { KeyCode.Mouse5 });
        m_dicKeyCodeConvertMap.Add("Mouse6", new KeyCode[1] { KeyCode.Mouse6 });
    }
}

