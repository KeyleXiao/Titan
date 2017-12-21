/*******************************************************************
** 文件名:	InputManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/3
** 版  本:	1.0
** 描  述:	输入管理
** 应  用:  	对Unity的Input做一层简单的封装
 *          

**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述: 
********************************************************************/
using GameLogic;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

/// <summary>
/// 输入管理，对Unity的Input做一层简单的封装，可以禁用/启用输入
/// </summary>
public partial class InputManager : MonoBehaviour
{
    public static bool foceOpenInput = false;
    /// <summary>
    /// 是否有效
    /// </summary>
    public static bool Available
    {
        get
        {
            return m_available || foceOpenInput;
        }
        set
        {
            m_available = value;
        }
    }

    public static Vector3 MousePosition
    {
        get
        {
            return Input.mousePosition;
        }
        set
        {
            Vector3 inputCursor = Input.mousePosition;
            // flip input Cursor y (as the Reference "0" is the last scanline)
            inputCursor.y = Screen.height - 1 - inputCursor.y;
            POINTAPI p = new POINTAPI();
            GameLogicAPI.GetCursorPos(ref p);
            Vector2 renderRegionOffset;
            renderRegionOffset.x = p.x - inputCursor.x;
            renderRegionOffset.y = p.y - inputCursor.y;
            int newx = (int)(value.x + renderRegionOffset.x);
            int newy = (int)(value.y + renderRegionOffset.y);
            GameLogicAPI.SetCursorPos(newx, newy);
        }
    }

    public static bool IsMouseInScreen()
    {
        Vector3 pos = Input.mousePosition;
        bool ret= (pos.x >= 0 && pos.x < Screen.width && pos.y >= 0 && pos.y < Screen.height);
        return ret && !USpeedUI.UISystem.Instance.IsMouseOverUI();
    }

    public static Vector2 MouseScrollDelta
    {
        get
        {
            return Input.mouseScrollDelta;
        }
    }

    public static void ResetInputAxes()
    {
        Input.ResetInputAxes();
    }

    public static IMECompositionMode ImeCompositionMode
    {
        set
        {
            Input.imeCompositionMode = value;
        }
        get
        {
            return Input.imeCompositionMode;
        }
    }

    private static bool m_available = true;

    /// <summary>
    /// 通知逻辑层当前按键信息
    /// </summary>
    /// <param name="_eKeyCode"></param>
    /// <param name="_eKeyState"></param>
    private static void SendLogicKeyState(KeyCode _eKeyCode, EViewKeyState _eKeyState)
    {
        m_sendLogicKeyStateData.nKeyCode = (int)_eKeyCode;
        m_sendLogicKeyStateData.nKeyState = (int)_eKeyState;
        ViewEventHelper.Instance.SendCommand<cmd_inputmanager_keystate>(GameLogicDef.GVIEWCMD_INPUTMANAGER_KEYSTATE, ref m_sendLogicKeyStateData);
    }
    private static cmd_inputmanager_keystate m_sendLogicKeyStateData = new cmd_inputmanager_keystate();

    #region 受enabled影响的

    public static int TouchCount
    {
        get
        {
            if (!Available)
            {
                return 0;
            }
            return Input.touchCount;
        }
    }

    public static bool AnyKey
    {
        get
        {
            if (!Available)
            {
                return false;
            }
            return Input.anyKey;
        }
    }

    public static bool AnyKeyDown
    {
        get
        {
            if (!Available)
            {
                return false;
            }
            return Input.anyKeyDown;
        }
    }

    public static bool GetButton(string buttonName)
    {
        if (!Available)
        {
            return false;
        }

        return (Input.GetButton(buttonName));
    }

    public static bool GetButtonUp(string buttonName)
    {
        if (!Available)
        {
            return false;
        }

        return (Input.GetButtonUp(buttonName));
    }

    public static bool GetButtonDown(string buttonName)
    {
        if (!Available)
        {
            return false;
        }

        return (Input.GetButtonDown(buttonName));
    }

    public static bool GetKey(KeyCode Key)
    {
        if (!Available)
        {
            return false;
        }

        //bool Result = Input.GetKey(Key);
        ////如果UI本帧已经处理了这个键，那么后面来的都不通过
        //if (Result && UIProcessedKey.Contains(Key))
        //{
        //    Result = false;
        //}

        //return Result;
        return (Input.GetKey(Key));
    }

    public static bool GetKeyUp(KeyCode Key)
    {
        if (!Available)
        {
            return false;
        }

        //bool Result = Input.GetKeyUp(Key);
        ////如果UI本帧已经处理了这个键，那么后面来的都不通过
        //if (Result && UIProcessedKey.Contains(Key))
        //{
        //    Result = false;
        //}

        //return Result;
        return (Input.GetKeyUp(Key));
    }

    public static bool GetKeyDown(KeyCode Key)
    {
        if (!Available)
        {
            return false;
        }

        bool Result = Input.GetKeyDown(Key);
        if(Result)
        {
            SendLogicKeyState(Key, EViewKeyState.EVKS_Down);

        //    //如果UI本帧已经处理了这个键，那么后面来的都不通过
        //    if (UIProcessedKey.Contains(Key))
        //    {
        //        Result = false;
        //    }
        }
        //return Result;
        return (Input.GetKeyDown(Key));
    }

    public static float GetAxis(string axisName)
    {
        if (!Available)
        {
            return 0.0f;
        }
        return Input.GetAxis(axisName);
    }

    public static float GetAxisRaw(string axisName)
    {
        if (!Available)
        {
            return 0.0f;
        }

        return Input.GetAxisRaw(axisName);

    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool GetMouseButton(int button)
    {
        if (!Available)
        {
            return false;
        }

        if(UIProcessedMouse.Contains(button))
        {
            return false;
        }
        bool result = Input.GetMouseButton(button);
        if (result && USpeedUI.UISystem.Instance.IsMouseOverUI() && button != 1)
        {
            UIProcessedMouse.Add(button);
            result = false;
        }
        return result;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool GetMouseButtonUp(int button)
    {
        if (!Available)
        {
            return false;
        }
        if (UIProcessedMouse.Contains(button))
        {
            return false;
        }
        bool result = Input.GetMouseButtonUp(button);

        if (result)
        {
            if(button != 1 && USpeedUI.UISystem.Instance.IsMouseOverUI())
            {
                UIProcessedMouse.Add(button);
                result = false;
            }
        }
        
        return result;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool GetMouseButtonDown(int button)
    {
        if (!Available)
        {
            return false;
        }
        if (UIProcessedMouse.Contains(button))
        {
            return false;
        }
        bool result = Input.GetMouseButtonDown(button);
        if (result && USpeedUI.UISystem.Instance.IsMouseOverUI())
        {
            UIProcessedMouse.Add(button);
            result = false;
        }
        return result;
    }

    public static string[] GetJoystickNames()
    {
        if (!Available)
        {
            return new string[0];
        }

        return Input.GetJoystickNames();
    }

    public static Touch GetTouch(int index)
    {
        if (!Available)
        {
            return default(Touch);
        }
        return Input.GetTouch(index);
    }

    #endregion

    #region 不受enabled影响的，跟以前的是一样的

    public static bool Raw_AnyKey
    {
        get
        {
            return Input.anyKey;
        }
    }

    public static bool Raw_AnyKeyDown
    {
        get
        {
            return Input.anyKeyDown;
        }
    }

    public static bool Raw_GetButton(string buttonName)
    {
        return Input.GetButton(buttonName);
    }

    public static bool Raw_GetButtonUp(string buttonName)
    {
        return Input.GetButtonUp(buttonName);
    }

    public static bool Raw_GetButtonDown(string buttonName)
    {
        return Input.GetButtonDown(buttonName);
    }

    public static bool Raw_GetKey(KeyCode Key)
    {

        return Input.GetKey(Key);
    }

    public static bool Raw_GetKeyUp(KeyCode Key)
    {
        return Input.GetKeyUp(Key);
    }

    public static bool Raw_GetKeyDown(KeyCode Key)
    {
        return Input.GetKeyDown(Key);
    }

    public static float Raw_GetAxis(string axisName)
    {
        return Input.GetAxis(axisName);
    }

    public static float Raw_GetAxisRaw(string axisName)
    {
        return Input.GetAxisRaw(axisName);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool Raw_GetMouseButton(int button)
    {
        return Input.GetMouseButton(button);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool Raw_GetMouseButtonUp(int button)
    {
        return Input.GetMouseButtonUp(button);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="button">0左键，1右，2中</param>
    /// <returns></returns>
    public static bool Raw_GetMouseButtonDown(int button)
    {
        return Input.GetMouseButtonDown(button);
    }

    public static string[] Raw_GetJoystickNames()
    {
        return Input.GetJoystickNames();
    }

    public static Touch Raw_GetTouch(int index)
    {
        return Input.GetTouch(index);
    }
    #endregion
}