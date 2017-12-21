using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System;
using U3D_Render;
using GameLogic;
using SPELL;
using War;
using USpeedUI;
using DataCenter;

public enum MouseState
{
    HeroControl = 0,             // 主角3D操控
    //FlagPosSelect,           // 设定插旗子的位置
}


public enum MouseAttachFuncType
{
    None = -1,
    CtrlAttackSign = 0,     // 攻击信号
    AltAttackSign = 1,     // 攻击信号（对于fps英雄，这个无效）
    RetreatSign = 2,    // 撤退信号
    DangerSign = 3,     // 危险信号
    OnWaySign = 4,      // 正在路上信号
    HelpMeSign = 5,     // 帮助我信号
    LostEyeSign = 6,    // 敌人不见了信号
    EmptySign = 99,      // 空白信号，占位用
}

//-----------------------------------------------------------------------------------------------------
// 鼠标管理类，以后鼠标全局控制的相关代码放这里
// 目前这里放了鼠标所对应的状态
// 根据鼠标的状态，其他类的各种操作可能需要被屏蔽，所以其他类进行鼠标操作的时候调用canXXX函数询问一下
//-----------------------------------------------------------------------------------------------------

//范围选择
public delegate void TargetSelectEventHandler(bool succeed, int targetObjectID, Vector3 targetPos);   

public class MouseCtrl : MonoBehaviour
{
    public MouseState state=MouseState.HeroControl;

    private ScreenRaycast raycast = null;
    private SkillCtrl skillCtrl = null;

    // 附着功能相关
    private MouseAttachFuncType m_currentAttachType = MouseAttachFuncType.None;
    private bool m_multiAttachFunc = false;

    private bool m_teamCommandStart = false;
    private bool m_teamCommandShow = false;
    private Vector2 m_commandStartScreenPos;
    public Vector2 CommandStartScreenPos { get { return m_commandStartScreenPos; } }
    private Vector3 m_commandStartWorldPos;
    private bool m_commandTryAttackFlag = false;

    //在此时间段内按下并抬起鼠标，算是寻路
    //public int mouseButtonTraceTick = 180;
    //private uint un_lastMouseDownTick = 0;

    public bool cameraRotateButton
    {
        get
        {
            //按下右键旋转镜头
            return m_bCameraRotateButton;
        }
    }

    private bool m_bIsMouseMoving = false;

    private bool m_bCameraRotateButton = false;

    public float fMouseAxisX;
    public float fMouseAxisY;

    /////////////////////////////////////////////////////////////////////////////////陈涛指定修改操作方式
    private uint s_nLastCheckTick = 0;  // 上次检测时间
    private float s_fCursorMoved = 0;               // 鼠标相对移动
    public float s_fLastCursorX = 0;               // 上一次更新鼠标位置
    public float s_fLastCursorY = 0;               // 上一次更新鼠标位置

    private int CheckTickCount = 30;
    private int CursorMoveDisCheck = 7;

    public bool canSkillCtrl()
    {
        //只有角色控制的时候鼠标左右键才作为放技能的快捷键
        if (m_currentAttachType != MouseAttachFuncType.None)
            return false;
        return (state == MouseState.HeroControl);
    }

    //更形鼠标是否处在移动状态
    private void updateMouseMoving()
    {
        s_fCursorMoved += Mathf.Abs(fMouseAxisX - s_fLastCursorX);
        s_fCursorMoved += Mathf.Abs(fMouseAxisY - s_fLastCursorY);
        s_fLastCursorX = fMouseAxisX;
        s_fLastCursorY = fMouseAxisY;

        int now = Time.frameCount;

        if (now - (int)s_nLastCheckTick > CheckTickCount)// 检测频率可修改
        {
            s_nLastCheckTick = (uint)now;
            m_bIsMouseMoving = s_fCursorMoved > CursorMoveDisCheck;              // 鼠标是否移动，8这个是个参数
            s_fCursorMoved = 0;
        }

        if (m_teamCommandStart)
        {
            Vector2 vec = (Vector2)getMousePosition() - m_commandStartScreenPos;
            if (vec.sqrMagnitude > 15f * 15f)
            {
                if (m_teamCommandShow == false)
                {
                    // 打开窗口
                    UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAPSIGNAL, true);
                    m_teamCommandShow = true;
                }

                MouseAttachFuncType funcType = MouseAttachFuncType.None;
                if (vec.x > 0)
                {
                    if (vec.y > 0)
                    {
                        if (Mathf.Abs(vec.x) > Mathf.Abs(vec.y))
                            funcType = MouseAttachFuncType.OnWaySign;
                        else
                            funcType = MouseAttachFuncType.DangerSign;
                    }
                    else
                    {
                        if (Mathf.Abs(vec.x) > Mathf.Abs(vec.y))
                            funcType = MouseAttachFuncType.OnWaySign;
                        else
                            funcType = MouseAttachFuncType.HelpMeSign;
                    }
                }
                else
                {
                    if (vec.y > 0)
                    {
                        if (Mathf.Abs(vec.x) > Mathf.Abs(vec.y))
                            funcType = MouseAttachFuncType.LostEyeSign;
                        else
                            funcType = MouseAttachFuncType.DangerSign;
                    }
                    else
                    {
                        if (Mathf.Abs(vec.x) > Mathf.Abs(vec.y))
                            funcType = MouseAttachFuncType.LostEyeSign;
                        else
                            funcType = MouseAttachFuncType.HelpMeSign;
                    }
                }

                if (m_currentAttachType != funcType)
                {
                    SetAttachType(funcType);

                    MapSignalSetSelectSignal uiData = new MapSignalSetSelectSignal();
                    uiData.nType = (int)funcType;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL, uiData);
                }
            }
            else
            {
                if (m_teamCommandShow && m_currentAttachType != MouseAttachFuncType.EmptySign)
                {
                    SetAttachType(MouseAttachFuncType.EmptySign);

                    MapSignalSetSelectSignal uiData = new MapSignalSetSelectSignal();
                    uiData.nType = (int)MouseAttachFuncType.EmptySign;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL, uiData);
                }
            }
        }
    }

    public static MouseCtrl Instance = null;

    public void Awake()
    {
        Instance = this;
    }

    public void Start()
    {
       // loadMouseTexture();
        raycast = gameObject.GetComponent<ScreenRaycast>();
        switchToState(MouseState.HeroControl,true);

        Initialize.onApplicationFocusChangeEvent.AddListener(OnApplicationFocusChangeEvent);
    }

    public bool OutputUIFocus = false; //是否输出ui选中状态和鼠标点

    public void setFPSRotate(bool freeRotate, bool FontSight)
    {
        RotateWithMouse = freeRotate;
        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,
        //            new UIMsgCmdData((int)WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, Convert.ToInt32(showSniperscope), string.Empty, IntPtr.Zero, 0));
        WarFrontSightDataManager.Instance.FrontSightVisible = FontSight;
    }

    public bool RotateWithMouse
    {
        get 
        {
            return m_bFreeRotate;
        }
        set
        {
            if (m_bFreeRotate != value)
            {
                if (value)
                {
                    InputManager.MousePosition = new Vector3(Screen.width / 2, Screen.height / 2, 0);
                }
                LockCursor = value;
                m_bFreeRotate = value;
            }
            MouseCursorManager.Instance.MouseCursorVisible = !value;

        }
    }
    private bool m_bFreeRotate=false;

    public void Update()
    {
        if (InputManager.GetKeyDown(KeyCode.BackQuote) && (InputManager.GetKey(KeyCode.LeftControl)||InputManager.GetKey(KeyCode.RightControl)))
        {
            RotateWithMouse = !RotateWithMouse;
        }

        if (SoldierCamera.MainInstance() != null && SoldierCamera.MainInstance<SoldierCamera>().bRotateWithMouse != RotateWithMouse)
        {
            if (SoldierCamera.MainInstance<SoldierCamera>().bRotateWithMouse)
            {
                setFPSRotate(true, true);
            }
            else
            {
                if (SoldierCamera.MainInstance().cameraMode == CameraMode.FPSMode)
                {
                    setFPSRotate(false, true);
                }
                else if (SoldierCamera.MainInstance().cameraMode!= CameraMode.SniperscopeControl)
                {
                    setFPSRotate(false, false);
                }
            }
        }
        fMouseAxisX = InputManager.GetAxis(InputAxisSet.Axis_MouseX);
        fMouseAxisY = InputManager.GetAxis(InputAxisSet.Axis_MouseY);

        if (skillCtrl == null)
        {
            skillCtrl = transform.GetComponent<SkillCtrl>();
        }

        if ((InputManager.GetMouseButton(1) || RotateWithMouse) && canSkillCtrl())
        {
            m_bCameraRotateButton = true;
        }
        else
        {
            m_bCameraRotateButton = false;
        }
        updateMouseMoving();

        //分状态对按键处理
        updateFunction();

        //if (Screen.lockCursor != m_lockCursor)
        //{
        //    Screen.lockCursor = m_lockCursor;
        //}
    }

    public void OnApplicationFocus(bool focusStatus)
    {
        // 失去焦点时清除鼠标附着物,修复alt+tab切换窗口后切回游戏会插旗的问题
        if (focusStatus == false && GetAttachType() != (int)MouseAttachFuncType.None)
            SetAttachType(MouseAttachFuncType.None);
    }

    public POINTAPI MousePosotion;
    public bool bPickingMovePoint = false;
    private uint pickingMovePointEndTick = 0;
    private float pickingMoveDis = 0.0f;
    public uint pickMovePointTickThread = 500; //鼠标按下时间阈值，这段时间内按下抬起右键自动寻路有效
    public float pickMovePointDisThread = 0.5f; //鼠标移动距离阈值，按下抬起的时间内累计位移不超过限制右键自动寻路有效
	//private    SceneEffectParamBase m_autoMoveEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_Common_AutoMoveTargetPoint, null);

    private void updateFunction()
    {
        uint currentTick = GameLogicAPI.getTickCount();

        //检测鼠标的动作，更新鼠标状态
        //if (state == MouseState.HeroControl)
        //{

        if (bPickingMovePoint)
        {
            Vector2 mouseAxis=new Vector2(fMouseAxisX,fMouseAxisY);
            pickingMoveDis += mouseAxis.magnitude;

            if (skillCtrl && !skillCtrl.enable)
            {
                bPickingMovePoint = false; //屏蔽技能操作时，右键寻路功能也关闭
            }

            if ((skillCtrl && skillCtrl.isSelecting()) || pickingMoveDis > pickMovePointDisThread || currentTick > pickingMovePointEndTick)
            {
                bPickingMovePoint = false;
            }
        }

        if ((InputManager.GetMouseButtonUp(1)) && (bPickingMovePoint)) //右键寻路
        {            
            bPickingMovePoint = false;            
            CreatureStateMachine stateMachine = transform.GetComponent<CreatureStateMachine>();
            if (stateMachine != null)
            {
                Vector3 pickPos=Vector3.zero;
                Vector3 selectPoint = getMousePosition();
                    
                if (!UISystem.Instance.IsMouseOverUI())
                {
                    //执行了实体右键弹出菜单就不执行寻路
                    if (EntityRightMenu())
                        return;

                    // TODO : 去除右键按场景地点自动寻路功能
                    //RaycastHit hitInfo;
                    //Ray castRay = Initialize.mainCam.ScreenPointToRay(selectPoint);

                    //int LayerMaskOnGournd = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding));

                    //bool isHit = Physics.Raycast(castRay, out hitInfo, 100, LayerMaskOnGournd);

                    //if (isHit)
                    //{
                    //    if (m_autoMoveEffectParam.ReturnComponent == null)
                    //        SceneEffectManager.Instance.CreateSceneEffect(ref m_autoMoveEffectParam);
                    //    {
                    //        m_autoMoveEffectParam.ReturnComponent.transform.position = hitInfo.point;
                    //        m_autoMoveEffectParam.ReturnComponent.ReplayEffect();
                    //    }

                    //    stateMachine.AutoMoveTo(hitInfo.point, false);
                    //}
                }
            }
        }
        else if (InputManager.GetMouseButtonDown(1) && GetAttachType() == (int)MouseAttachFuncType.None)
        {
            bPickingMovePoint = true;
            pickingMoveDis = 0.0f;
            pickingMovePointEndTick = currentTick + pickMovePointTickThread;
        }

        {
            CheckAttachFunc();

            //应策划要求，暂时关闭Alt+鼠标标记地图功能
            //if (InputManager.GetMouseButtonDown(0) && InputManager.GetKey(KeyCode.LeftAlt))
            //{
            //    if (!UISystem.Instance.IsMouseOverUI())
            //        OnTeamCommand(true);
            //}
            //if (InputManager.GetMouseButtonUp(0) || InputManager.GetKeyUp(KeyCode.LeftAlt))
            //{
            //    OnTeamCommand(false);
            //}

            if (m_currentAttachType != MouseAttachFuncType.None && InputManager.GetMouseButtonUp(0)) //左键插旗，抬键才算，不然会跟普通攻击冲突
            {
                //GFxMinimap minimap = SFGFxMovieManager.Instance.GetWindow("GFxMinimap") as GFxMinimap;
                //if (minimap != null)
                {
                    //如果点到UI上，则不插旗
                    if (!UISystem.Instance.IsMouseOverUI())
                    {
                        Vector3 hitPoint = new Vector3();
                        if (MouseRaycastToGround(ref hitPoint))
                        {
                            DoMouseAttachFunc(hitPoint.x, hitPoint.y, hitPoint.z);
                        }
                    }
                }
            }
        }

        switch(m_currentAttachType)
        {
            case MouseAttachFuncType.CtrlAttackSign:
                MouseCursorManager.Instance.MouseCursorType = EMouseCursorType.MCT_MouseAttack;
                break;
            case MouseAttachFuncType.AltAttackSign:
                MouseCursorManager.Instance.MouseCursorType = EMouseCursorType.MCT_MouseAttack;
                break;
            case MouseAttachFuncType.RetreatSign:
                MouseCursorManager.Instance.MouseCursorType = EMouseCursorType.MCT_MouseSign;
                break;
            default:
                if (MouseCursorManager.Instance.MouseCursorType == EMouseCursorType.MCT_MouseAttack
                    || MouseCursorManager.Instance.MouseCursorType == EMouseCursorType.MCT_MouseSign)
                    MouseCursorManager.Instance.MouseCursorType = EMouseCursorType.MCT_Normal;
                break;
        }

        // AICommand模块
        if (InputManager.GetMouseButtonDown(0) && InputManager.GetKey(KeyCode.X))
        {
            if (!UISystem.Instance.IsMouseOverUI())
            {
                OnAICommand();
            }
        }

        if (!RotateWithMouse)
        {
            if (LockCursor && !InputManager.GetMouseButton(1))
            {
                LockCursor = false;
            }
            else if (!LockCursor && InputManager.GetMouseButton(1) && !UISystem.Instance.IsMouseOverUI())
            {
                LockCursor = true;
            }
        }
    }

    bool IsValidMapSignalTarget(EntityView target)
    {
        if (target.gameObject == null)
            return false;

        if (target.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (target.CampFlag == CampFlag.CampFlag_Enemy)
                return true;
        }

        if (target.Type == ENTITY_TYPE.TYPE_MONSTER)
        {
            if (target.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER || target.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE)
                return true;

            if (target.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_WILD_MONSTER)
                return true;
        }

        return false;
    }

    bool MouseRaycastToGround(ref Vector3 hitPoint)
    {
        //获得当前鼠标点的位置，做一条从鼠标点发出的射线，和场景进行射线检测
        Vector3 selectPoint = InputManager.MousePosition;
        Ray castRay = Initialize.mainCam.ScreenPointToRay(selectPoint);

        int LayerMaskOnFlag = -1;
        //因为一个人物角色有3个动作分身，Hero框套在他们上面
        LayerMaskOnFlag = LayerMaskOnFlag & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));

        RaycastHit hitInfo;
        bool isHit = Physics.Raycast(castRay, out hitInfo, 100.0f, LayerMaskOnFlag); //什么都拾取

        if (isHit)
        {
            hitPoint = hitInfo.point;
            int LayerMaskOnPos = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding)); //只取场景中的物件层
            RaycastHit groundHitInfo;
            bool hitGround = Physics.Raycast(castRay, out groundHitInfo, 100.0f, LayerMaskOnPos);
            if (!hitGround || (groundHitInfo.point - hitPoint).sqrMagnitude > 1.0f)
            {
                //点到是非物件层的东西，做一次向地面投影吧
                Ray PointToGroundRay = new Ray(hitPoint, new Vector3(0, -1, 0));
                RaycastHit projectInfo;
                bool hitPorject = Physics.Raycast(PointToGroundRay, out projectInfo, 100, LayerMaskOnPos);
                if (hitPorject)
                {
                    hitPoint = projectInfo.point;
                }
            }

            // Trace.Log("Flag on " + hitPoint.ToString());
            return true;
        }

        return false;
    }

    public void OnTeamCommand(bool bStart)
    {
        if (GameLogicAPI.isInWarScene() <= 0)
            return;

        if (m_teamCommandStart == bStart)
            return;

        m_teamCommandStart = bStart;
        if (bStart)
        {
            if (!MouseRaycastToGround(ref m_commandStartWorldPos))
            {
                m_teamCommandStart = false;
                return;
            }
            m_commandStartScreenPos = getMousePosition();
            m_teamCommandShow = false;
        }
        else
        {
            // 关闭窗口
            if (m_teamCommandShow)
            {
                if (m_currentAttachType != MouseAttachFuncType.None)
                    DoMouseAttachFunc(m_commandStartWorldPos.x, m_commandStartWorldPos.y, m_commandStartWorldPos.z);

                UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAPSIGNAL, false);
                m_teamCommandShow = false;
            }
            else
            {
                if (m_commandTryAttackFlag)
                {
                    DoMouseAttachFunc(m_commandStartWorldPos.x, m_commandStartWorldPos.y, m_commandStartWorldPos.z);
                    m_commandTryAttackFlag = false;
                }
            }
        }
    }

    void CheckAttachFunc()
    {
        if (m_multiAttachFunc)
        {
            if ((isAttackSign(m_currentAttachType) && FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.AttackSign))
                || (m_currentAttachType == MouseAttachFuncType.RetreatSign && (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.DangerSign))))
            {
                SetAttachType(MouseAttachFuncType.None);
            }
        }

        if (InputManager.GetKeyUp(KeyCode.LeftAlt) || InputManager.GetKeyUp(KeyCode.LeftControl))
        {
            SetAttachType(MouseAttachFuncType.None);
        }

        if (InputManager.GetMouseButtonDown(1)) //右键取消
        {
            SetAttachType(MouseAttachFuncType.None);
        }

        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.AttackSign) ||  InputManager.GetKeyDown(KeyCode.LeftControl))
            {
            SetAttachType(MouseAttachFuncType.CtrlAttackSign);
        }

        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.AttackSign) || InputManager.GetKeyDown(KeyCode.LeftAlt) )
        {
            SetAttachType(MouseAttachFuncType.AltAttackSign);
        }

        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.DangerSign))
        {
            SetAttachType(MouseAttachFuncType.DangerSign);
        }
    }

    public void SetAttachType(MouseAttachFuncType type)
    {
        if (m_currentAttachType == type)
        {
            return;
        }

        if (m_currentAttachType == type && (isAttackSign(type) || type == MouseAttachFuncType.RetreatSign))
            type = MouseAttachFuncType.None;
        
        if (skillCtrl)
        {
            if (skillCtrl.isSelecting())
            {
                return;
            }
            else
            {
                skillCtrl.resetSkill();
            }
        }
        m_currentAttachType = type;
        m_multiAttachFunc = false;
        
        bPickingMovePoint = false;
        if (m_currentAttachType == MouseAttachFuncType.None)
        {
        }
        else
        {
            LockCursor = false;
        }
    }

    public int GetAttachType()
    {
        return (int)m_currentAttachType;
    }

    private bool isAttackSign(MouseAttachFuncType type)
    {
        return (type == MouseAttachFuncType.CtrlAttackSign || type == MouseAttachFuncType.AltAttackSign);
    }

    public void switchToState(MouseState newState, bool force=false)
    {
        if (!force && state==newState)
        {
            return;
        }

        if (newState == MouseState.HeroControl)
        {
            MouseCursorManager.Instance.MouseCursorType = EMouseCursorType.MCT_Normal;
            SetAttachType(MouseAttachFuncType.None);
            raycast.SetDefaultSelection();
        }
        state = newState;
        //Trace.LogWarning("current Mouse ctrl state=" + state);
    }

    public void OnDestroy()
    {
        //switchToState(MouseState.UIControl);
        //Screen.lockCursor = false;
        //if (m_autoMoveEffectParam != null)
        //    SceneEffectManager.Instance.DestroySceneEffect(ref m_autoMoveEffectParam);
        LockCursor = false;
        Instance = null;
    }

    public void OnDisable()
    {
       // Screen.lockCursor = false;
        LockCursor = false;
    }

    /// <summary>
    /// 是否把鼠标锁定
    /// </summary>
    /// <param name="val">true:锁定，false:解锁</param>
    /// 
    private bool m_lockCursor;
    void LockMouse(bool val)
    {
        m_lockCursor = val;
    }

    public Vector3 getMousePosition()
    {
        return InputManager.MousePosition;
    }

    private bool m_bLockCursor=false;
    public bool LockCursor
    {
//#if UNITY_3_5 || UNITY_4_0 || UNITY_4_1 || UNITY_4_2 || UNITY_4_3 || UNITY_4_5 || UNITY_4_6
//    get { return Screen.lockCursor; }
//    set 
//    { 
//        if (value == true)
//        {
//            lockMousePosition = InputManager.MousePosition;
//        }
//        Screen.lockCursor = value; 
//    }
//#else
        get
        {
            return m_bLockCursor;
            //return ((Cursor.lockState == CursorLockMode.Locked) ? true : false);
        }
        set
        {
            if (!Initialize.isOnFocus)
                return;
            if (value != m_bLockCursor)
            {
 
                //if (SoldierCamera.MainInstance<SoldierCamera>().cameraMode != SoldierCamera.CameraControlMode.FPSMode)
                //    Cursor.lockState = CursorLockMode.None;

                if (value == true)
                {
                    int ret = GameLogicAPI.GetCursorPos(ref MousePosotion);

                    RECTAPI rec;
                    rec.left = MousePosotion.x;
                    rec.right = MousePosotion.x + 1;
                    rec.bottom = MousePosotion.y + 1;
                    rec.top = MousePosotion.y;
                    IntPtr p = Marshal.AllocHGlobal(Marshal.SizeOf(rec));
                    Marshal.StructureToPtr(rec, p, false);
                    GameLogicAPI.ClipCursor(p);
                    Marshal.FreeHGlobal(p);
                }
                else
                {
                    //
                    if (Application.isEditor)
                        GameLogicAPI.ClipCursor(IntPtr.Zero);
                    else
                        Initialize.Instance.DoMouseLockInModeType();
                }
            }
            m_bLockCursor = value;

            //Cursor.visible = !value;

        }
//#endif
    }

    // 小地图点击响应
    public bool OnMinimapClick(float xPos, float yPos, float zPos)
    {
        //if (InputManager.GetKey(KeyCode.LeftAlt) && m_teamCommandStart == false)
        //{
        //    m_teamCommandStart = true;
        //    m_teamCommandShow = false;
        //    m_commandStartScreenPos = getMousePosition();
        //    m_commandStartWorldPos.x = xPos;
        //    m_commandStartWorldPos.z = zPos;
        //    m_commandTryAttackFlag = true;
        //    return true;
        //}
        //关闭fps英雄的alt点解小地图发信号功能
        if (EntityFactory.MainHeroView != null) {
            PersonSkinModelItem item = new PersonSkinModelItem();
            int heroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            GameLogicAPI.GetPersonModel((UInt16)heroID, ref item);
            if (m_currentAttachType == MouseAttachFuncType.AltAttackSign && item.nPlayMode==1)
            {
                return false;
            }
        }
        if (GetAttachType() > (int)MouseAttachFuncType.None)
        {
            DoMouseAttachFunc(xPos, yPos, zPos);
            return true;
        }

        return false;
    }

    // 是否在拖动地图信息标示
    public bool IsTeamCommandStart()
    {
        return m_teamCommandStart;
    }

    //插旗
    public void DoMouseAttachFunc(float xPos, float yPos, float zPos)
    {
        int targetID = 0;
        if (isAttackSign(m_currentAttachType) || m_currentAttachType == MouseAttachFuncType.RetreatSign)
        {
            if (raycast.targetID > 0)
            {
                EntityView evTarget = EntityFactory.getEntityViewByID(raycast.targetID);
                if (evTarget != null && IsValidMapSignalTarget(evTarget))
                {
                    targetID = raycast.targetID;
                }
            }

            if (targetID == 0)
            {
                Vector3 hitPoint = new Vector3(xPos, yPos, zPos);
                IList<EntityView> list = MapFinderManager.Nearest(hitPoint, 24, SPELL.CampFlag.CampFlag_Unknow);
                if (list != null)
                {
                    float curDistance = float.MaxValue;
                    EntityView curObj = null;
                    foreach (EntityView view in list)
                    {
                        if (IsValidMapSignalTarget(view))
                        {
                            float mag = (view.gameObject.transform.position - hitPoint).sqrMagnitude;
                            if (curDistance > mag)
                            {
                                curDistance = mag;
                                curObj = view;
                            }
                        }
                    }

                    if (curDistance < 16f)
                    {
                        targetID = curObj.ID;
                    }
                }
            }
        }

        cmd_war_mini_map_info data = new cmd_war_mini_map_info();
        switch (m_currentAttachType)
        {
            case MouseAttachFuncType.CtrlAttackSign:
                data.nType = (int)EMinimapClickType.EMCT_AttackSignal;
                break;
            case MouseAttachFuncType.AltAttackSign:
                data.nType = (int)EMinimapClickType.EMCT_AttackSignal;
                break;
            case MouseAttachFuncType.RetreatSign:
                data.nType = (int)EMinimapClickType.EMCT_RetreatSignal;
                break;
            case MouseAttachFuncType.DangerSign:
                data.nType = (int)EMinimapClickType.EMCT_DangerSignal;
                break;
            case MouseAttachFuncType.OnWaySign:
                data.nType = (int)EMinimapClickType.EMCT_OnWaySignal;
                break;
            case MouseAttachFuncType.HelpMeSign:
                data.nType = (int)EMinimapClickType.EMCT_HelpMeSignal;
                break;
            case MouseAttachFuncType.LostEyeSign:
                data.nType = (int)EMinimapClickType.EMCT_LostEyeSignal;
                break;
            default:
                return;
        }

        // 给逻辑层发命令 主角ID 目标位置 信号类型
        data.x = xPos;
        data.y = yPos;
        data.z = zPos;
        data.nEntityID = targetID;
        EntityEventHelper.Instance.SendCommand<cmd_war_mini_map_info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_MINIMAP_INFO, ref data);

        if (isAttackSign(m_currentAttachType) && (FuntionShortCutCtrl.CheckFunctionShortcutKeyPressing(FuntionShortCutSlotIndex.AttackSign) || InputManager.GetKey(KeyCode.LeftAlt) || InputManager.GetKey(KeyCode.LeftControl))
            || m_currentAttachType == MouseAttachFuncType.RetreatSign && (FuntionShortCutCtrl.CheckFunctionShortcutKeyPressing(FuntionShortCutSlotIndex.DangerSign)))
        {
            // 功能继续
            m_multiAttachFunc = true;
        }
        else if ((InputManager.GetKey(KeyCode.LeftAlt))
            && (m_currentAttachType == MouseAttachFuncType.OnWaySign || m_currentAttachType == MouseAttachFuncType.DangerSign
                || m_currentAttachType == MouseAttachFuncType.HelpMeSign || m_currentAttachType == MouseAttachFuncType.LostEyeSign))
        {
            // 战术标记发完后还按着alt的话保持附个空标记占位，以免出普攻
            SetAttachType(MouseAttachFuncType.EmptySign);
        }
        else
        {
            SetAttachType(MouseAttachFuncType.None);
        }
    }

    private void OnAICommand()
    {
        int targetID = 0;
        if (raycast.targetID > 0)
        {
            EntityView evTarget = EntityFactory.getEntityViewByID(raycast.targetID);
            EntityView evMainHeroView = EntityFactory.MainHeroView;
            if (evTarget != null && evTarget.IsValid)
            {
                if (evTarget.CampFlag == CampFlag.CampFlag_Friend && evTarget.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                {
                    // 1.发送服务器要(1)控制或(2)解除或   暂时没有该功能//(3)移交targetID的友方英雄单位
                    if (evTarget.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_AI_SCHEME_ID) > 0)
                    {
                        targetID = raycast.targetID;
                        if (!LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(targetID))
                        {
                            return;
                        }
                        TeamRightInfo teamRightInfo = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[targetID] as TeamRightInfo;

                        int groundLightEffectID = 0;
                        const int helpmeSignLightEffectID = 13;
                        const int dangerSignLightEffectID = 12;
                        if (LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[teamRightInfo.nSlotId])
                        {
                            cmd_war_ai_command_break_follow data = new cmd_war_ai_command_break_follow();
                            data.uidTarget = targetID;
                            EntityEventHelper.Instance.SendCommand<cmd_war_ai_command_break_follow>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_AI_COMMAND_BREAK_FOLLOW, ref data);

                            groundLightEffectID = dangerSignLightEffectID;
                        }
                        else
                        {
                            cmd_war_ai_command_follow data = new cmd_war_ai_command_follow();
                            data.uidTarget = targetID;
                            EntityEventHelper.Instance.SendCommand<cmd_war_ai_command_follow>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_AI_COMMAND_FOLLOW, ref data);

                            groundLightEffectID = helpmeSignLightEffectID;
                        }
                        LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[teamRightInfo.nSlotId] = !LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[teamRightInfo.nSlotId];

                        // 添加地面光效
                        Vector3 pos = new Vector3();
                        if (MouseRaycastToGround(ref pos))
                        {
                            int LayerMaskOnPos = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding));
                            Ray PointToGroundRay = new Ray(pos + new Vector3(0, 0.5f, 0), new Vector3(0, -1, 0));
                            RaycastHit projectInfo;
                            Physics.Raycast(PointToGroundRay, out projectInfo, 100, LayerMaskOnPos);
                            pos = projectInfo.point;

                            LightEffectParam param = new LightEffectParam();
                            param.nSrcEntityID = evMainHeroView.ID;
                            param.sourcePos = pos;
                            param.targetPos = pos;
                            LightingEffectManager lem = evMainHeroView.LightingEffectManager;
                            if (lem != null && lem)
                                lem.AddLighting(groundLightEffectID, param);
                        }
                    }
                }
            }
        }
    }

    protected void OnApplicationFocusChangeEvent(bool isFocus)
    {
        if (RotateWithMouse)
        {
            LockCursor = true;
            Cursor.lockState = CursorLockMode.Locked;
        }
            
    }

    #region Entity 右键菜单逻辑
    protected int nPopupMenuTargetID = -1;
    /// <summary>
    /// 实体右键菜单
    /// </summary>
    protected bool EntityRightMenu()
    {
        bool result = false;

        if (!GameLogicAPI.isEnableEntityRightMenu())
            return result;

        raycast.SetFriendTargetSelection();

        if (raycast.targetID > 0)
        {
            result = true;
            //停掉射线检测
            raycast.enabled = false; 

            nPopupMenuTargetID = raycast.targetID;
            //通知弹出列表菜单处理东西
            LogicDataCenter.popupMenuDataManager.SendPopupMenuEntityID(nPopupMenuTargetID,()=> 
            {
                if(raycast != null)
                {
                    raycast.enabled = true;
                    //重置鼠标选择规则
                    raycast.SetDefaultSelection();
                }
            });            
        }         

        if(!result)
        {
            //没检测到实体的话直接重置鼠标选择规则
            raycast.SetDefaultSelection();
        }

        return result;
    }
    #endregion
}