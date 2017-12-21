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
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#region
///// <summary>
///// 输入按键
///// </summary>
//public enum InputKeyCode
//{
//    #region 物品栏
//    /// <summary>
//    /// 物品栏1
//    /// </summary>
//    Item_1,

//    /// <summary>
//    /// 物品栏2
//    /// </summary>
//    Item_2,

//    /// <summary>
//    /// 物品栏3
//    /// </summary>
//    Item_3,

//    /// <summary>
//    /// 物品栏4
//    /// </summary>
//    Item_4,

//    /// <summary>
//    /// 物品栏5
//    /// </summary>
//    Item_5,

//    /// <summary>
//    /// 物品栏6
//    /// </summary>
//    Item_6,
//    #endregion

//    #region 技能
//    /// <summary>
//    /// 技能1
//    /// </summary>
//    Skill_Q,

//    /// <summary>
//    /// 技能2
//    /// </summary>
//    Skill_E,

//    /// <summary>
//    /// 技能3
//    /// </summary>
//    Skill_R,

//    /// <summary>
//    /// 技能5
//    /// </summary>
//    Skill_F,

//    /// <summary>
//    /// 技能6
//    /// </summary>
//    Skill_B,

//    /// <summary>
//    /// 技能7
//    /// </summary>
//    Skill_X,

//    /// <summary>
//    /// 升级技能Q
//    /// </summary>
//    Skill_Update_Q,

//    /// <summary>
//    /// 升级技能E
//    /// </summary>
//    Skill_Update_E,

//    /// <summary>
//    /// 升级技能F
//    /// </summary>
//    Skill_Update_F,

//    /// <summary>
//    /// 升级技能R
//    /// </summary>
//    Skill_Update_R,
//    #endregion

//    #region 鼠标
//    /// <summary>
//    /// 左键
//    /// </summary>
//    Mouse_Left,

//    /// <summary>
//    /// 右键
//    /// </summary>
//    Mouse_Right,

//    /// <summary>
//    /// 中键
//    /// </summary>
//    Mouse_Middle,
//    #endregion

//    #region 按键
    
//    W,
//    S,
//    A,
//    D,
//    Shift,
//    Space,

//    /// <summary>
//    /// 回车，大回车
//    /// </summary>
//    Enter_Right,

//    /// <summary>
//    /// 小键盘的回车
//    /// </summary>
//    Enter_Left,

//    /// <summary>
//    /// 七龙珠
//    /// </summary>
//    DragonBall,

//    /// <summary>
//    /// 属性面板
//    /// </summary>
//    PlayerFloor,

//    /// <summary>
//    /// 系统设置面板
//    /// </summary>
//    SystemSettings,

//    /// <summary>
//    /// 战场信息
//    /// </summary>
//    WarInfo,

//    /// <summary>
//    /// 打开log文件
//    /// </summary>
//    OpenLogFile,

//    /// <summary>
//    /// 暂停scaleform更新
//    /// </summary>
//    Pause_Scaleform_Update,

//    /// <summary>
//    /// 开启特效
//    /// </summary>
//    Open_Effects,

//    /// <summary>
//    /// 开启飘字
//    /// </summary>
//    Open_FloatFont,

//    /// <summary>
//    /// 打开商店
//    /// </summary>
//    Open_Store,

//    /// <summary>
//    /// 自杀
//    /// </summary>
//    KillSelf,

//    /// <summary>
//    /// 锁定摄像机镜头
//    /// </summary>
//    LockCameraView,


//    #endregion
//}
#endregion

/// <summary>
/// 按键状态
/// </summary>
public enum KeyStatus
{
    None,
    Down,
    Pressing,
    Up,
    Max
}

public enum MouseButton
{
    MouseBtn_Left,
    MouseBtn_Right,
    MouseBtn_Middle,
}

public class InputAxisSet
{
    /// <summary>
    /// 鼠标滚轮
    /// </summary>
    public const string Axis_MouseScrollWheel = "Mouse ScrollWheel";

    /// <summary>
    /// 水平轴
    /// </summary>
    public const string Axis_Horizontal = "Horizontal";

    /// <summary>
    /// 垂直轴
    /// </summary>
    public const string Axis_Vertical = "Vertical";

    /// <summary>
    /// Mouse X
    /// </summary>
    public const string Axis_MouseX = "Mouse X";

    /// <summary>
    /// Mouse Y
    /// </summary>
    public const string Axis_MouseY = "Mouse Y";
}

/// <summary>
/// 输入管理，对Unity的Input做一层简单的封装，可以禁用/启用输入
/// </summary>
public partial class InputManager : MonoBehaviour
{
    public static GameObject InstanceGo { get { return MonoInstance; } }
    private static GameObject MonoInstance = null;
    private static InputManager inputInstance = null;
    //private static List<KeyCode> AllKeyEnums = new List<KeyCode>();
    //private static HashSet<KeyCode> UIProcessedKey = new HashSet<KeyCode>();
    private static HashSet<int> UIProcessedMouse = new HashSet<int>();
    private static WaitForEndOfFrame waitEndOfFrame = new WaitForEndOfFrame();
    //是否开启右键点击上报功能
    public static bool OpenReportOnRightBtnDown { set { bOpenReportOnRightBtnDown = value; } }
    private static bool bOpenReportOnRightBtnDown = false;

    /// <summary>
    /// UI注册的弹起按键
    /// </summary>
    //private static List<KeyCode> UIRegsiterKey_Up = new List<KeyCode>();

    /// <summary>
    /// UI注册的按键
    /// </summary>
    //private static List<KeyCode> UIRegsiterKey = new List<KeyCode>();

    /// <summary>
    /// UI注册的按下按键
    /// </summary>
    //private static List<KeyCode> UIRegsiterKey_Down = new List<KeyCode>();
    public static void Init()
    {
        if(!MonoInstance)
        {
            MonoInstance = new GameObject();
            MonoInstance.name = "InputManager";
            inputInstance = MonoInstance.AddComponent<InputManager>();
            inputInstance.StartCoroutine(inputInstance.WaitingEndOfFrame());
            DontDestroyOnLoad(MonoInstance);
        }
        //Type t = typeof(KeyCode);
        //string[] names = Enum.GetNames(t);
        //foreach(string s in names)
        //{
        //    if(s.Equals(KeyCode.None.ToString()))
        //    {
        //        continue;
        //    }       
        //    AllKeyEnums.Add((KeyCode)Enum.Parse(t,s));
        //}
        //UIRegsiterKey.Clear();

        //UIRegsiterKey_Down.Add(KeyCode.Escape);
        //UIRegsiterKey_Down.Add(KeyCode.Return);
        //UIRegsiterKey_Down.Add(KeyCode.KeypadEnter);
        //UIRegsiterKey_Down.Add(KeyCode.C);
        //UIRegsiterKey_Down.Add(KeyCode.F10);
        //UIRegsiterKey_Down.Add(KeyCode.P);
        //UIRegsiterKey_Up.Add(KeyCode.Tab);
        //UIRegsiterKey_Up.Add(KeyCode.Escape);
        //UIRegsiterKey_Down.Add(KeyCode.Tab);
        //UIRegsiterKey_Down.Add(KeyCode.U);
    }

    public void Awake()
    {
        ImeManager imeManager = MonoInstance.GetComponent<ImeManager>();
        if (imeManager == null)
            imeManager = MonoInstance.AddComponent<ImeManager>();
    }

    public static void Clear()
    {
        if(MonoInstance)
        {
            inputInstance.StopAllCoroutines();
            inputInstance = null;
            ResNode.DestroyRes(ref MonoInstance);
            MonoInstance = null;
            //AllKeyEnums.Clear();
            //AllKeyEnums = null;
        }
        InputConvertHelper.Clear();
    }

    IEnumerator WaitingEndOfFrame()
    {
        while(true)
        {
            yield return waitEndOfFrame;
        //    UIProcessedKey.Clear();
            UIProcessedMouse.Clear();
        }
    }

    public void Update()
    {
        //int cout = UIRegsiterKey_Up.Count;
        //for (int i = 0; i < cout;i++ )
        //{
        //    KeyCode key = UIRegsiterKey_Up[i];
        //    if (GetKeyUp(key))
        //     {
        //        if (USpeedUI.UISystem.Instance.OnKeyMessage(key, KeyStatus.Up))
        //        {
        //            UIProcessedKey.Add(key);
        //        }
        //    }
        //}

        //cout = UIRegsiterKey.Count;
        //for (int i = 0; i < cout;i++ )
        //{
        //    KeyCode key = UIRegsiterKey[i];
        //    if (GetKey(key))
        //     {
        //        if (USpeedUI.UISystem.Instance.OnKeyMessage(key, KeyStatus.Pressing))
        //        {
        //            UIProcessedKey.Add(key);
        //        }
        //    }
        //}

        //cout = UIRegsiterKey_Down.Count;
        //for (int i = 0; i < cout;i++ )
        //{
        //    KeyCode key = UIRegsiterKey_Down[i];
        //    if (GetKeyDown(key))
        //     {
        //        if (USpeedUI.UISystem.Instance.OnKeyMessage(key, KeyStatus.Down))
        //        {
        //            UIProcessedKey.Add(key);
        //        }
        //    }
        //}

        if (bOpenReportOnRightBtnDown && GetMouseButtonDown((int)MouseButton.MouseBtn_Right))
        {
            if (UIProcessedMouse.Contains((int)MouseButton.MouseBtn_Right))
                return;

            ViewEventHelper.Instance.SendCommand(GameLogic.GameLogicDef.GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN, (int)MouseButton.MouseBtn_Right);
        }

        if(null != EntityFactory.MainHeroView && EntityFactory.MainHeroView.IsFPS)
            CheckMouseButtonDown();
    }

    /// <summary>
    /// 检测鼠标按下时间
    /// </summary>
    private void CheckMouseButtonDown()
    {
        if (GetMouseButtonDown((int)MouseButton.MouseBtn_Left))
        {
            if (!UIProcessedMouse.Contains((int)MouseButton.MouseBtn_Left))
                U3D_Render.EntityInputHandler.MouseButtonDownHandle(MouseButton.MouseBtn_Left);
        }

        if (GetMouseButtonDown((int)MouseButton.MouseBtn_Right))
        {
            if (!UIProcessedMouse.Contains((int)MouseButton.MouseBtn_Right))
                U3D_Render.EntityInputHandler.MouseButtonDownHandle(MouseButton.MouseBtn_Right);
        }

        if (GetMouseButtonDown((int)MouseButton.MouseBtn_Middle))
        {
            if (!UIProcessedMouse.Contains((int)MouseButton.MouseBtn_Middle))
                U3D_Render.EntityInputHandler.MouseButtonDownHandle(MouseButton.MouseBtn_Middle);
        }
    }
}