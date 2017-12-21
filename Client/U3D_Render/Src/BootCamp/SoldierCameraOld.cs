///*******************************************************************
//** 文件名:	SoldierCamera.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	谭强均
//** 日  期:	2015/6/18
//** 版  本:	1.0
//** 描  述:	相机控制，这个脚本只能控制主相机，而且与场景无关，只要场景中有一个主相机，这个脚本就能找到主相机，并进行控制
//** 应  用:   只能控制住相机，即 Camera.main相机
// *          
//**************************** 修改记录 ******************************
//** 修改人:    
//** 日  期: 
//** 描  述:
//********************************************************************/
//using UnityEngine;
//using System.Collections.Generic;
//using System.Collections;
//using System.IO;
//using ImageEffects;
//using GameLogic;
//using DataCenter;
//using USpeedUI;
//using System;

//public class SoldierCameraOld:MonoBehaviour
//{
//	public Transform target;

//    [System.NonSerialized]
//	public U3D_Render.EntityView soldier;

//    public float CurDistance
//    {
//        get
//        {
//            return apprDistance;
//        }
//    }

//    public Camera CurrentCamera
//    {
//        get
//        {
//            return m_Camera;
//        }
//    }

//    public Vector3 userDesireAngle
//    {
//        set
//        {
//            m_userDesireAngle = value;
//            if (curCameraModeParam != null)
//            {
//                m_userDesireAngle.x = Mathf.Clamp(m_userDesireAngle.x, curCameraModeParam.pitchAngleRange.x, curCameraModeParam.pitchAngleRange.y);
//            }
//        }
//        get
//        {
//            return m_userDesireAngle;
//        }
//    }

//    public float userDesireDistance
//    {
//        set
//        {
//            m_userDesireDistance = value;
//        }
//        get
//        {
//            return m_userDesireDistance;
//        }
//    }
    



//    public static SoldierCamera MainInstance = null;

//    public bool isGodEye_VMode = false;  //是否是V视角
//    private float godEyeStartTime = 0.0f;
//    private const float GOD_EYE_TIME = 10.0f;

//    #region 镜头模式分类及切换

//    //这里按优先级分类了各种不同的镜头模式，越靠前的优先级越高
//    public enum CameraControlMode//(public)
//    {
//        CameraAnim,
//        GodEyeControl,
//        FollowTarget,
//        SniperscopeControl,
//        FPSMode,
//        FloatingControl,
//        RidingControl,
//        UserControl,
//        Count
//    }
//    public class CameraParam
//    {
//        public float curDistance = -1000.0f;
//        public Vector2 distanceRange = Vector2.zero;
//        public float curPitchAngle=-1000.0f;
//        public Vector2 pitchAngleRange=Vector2.zero;
//        public float targetOffset=-1000.0f;
//        public float FOV = -1000.0f;
//    }
    
//    private bool[] enableCameraControlMode;
//    public CameraParam[] cameraControlModeParam;
//    private CameraControlMode curCameraMode = CameraControlMode.Count;
//    public CameraParam curCameraModeParam;
//    public CameraControlMode cameraMode //= CameraControlMode.UserControl;//(public)
//    {
//        get
//        {
//            return curCameraMode;
//        }
//    }
//    public float rayTraceAngle = 60.0f; //60度外不模糊选取
    
//    public void switchMode()
//    {
//        if (cameraControlModeParam == null)
//        {
//            return;
//        }
//        for (int i = 0; i < (int)CameraControlMode.Count; i++)
//        {
//            if (enableCameraControlMode[i])
//            {
//                if ((int)curCameraMode != i)
//                {
//                    if (curCameraMode == CameraControlMode.FloatingControl || curCameraMode == CameraControlMode.RidingControl)
//                    {
//                        LeaveFloatingMode();
//                    }
//                    else if (curCameraMode == CameraControlMode.SniperscopeControl)
//                    {
//                        setSniperscope(false);
//                    }
//                    else if (curCameraMode == CameraControlMode.FPSMode)
//                    {
//                        setFPS(false);
//                    }


//                    if (curCameraMode < CameraControlMode.Count)
//                    {
//                        curCameraModeParam.FOV = m_userDesireFov;
//                        curCameraModeParam.curPitchAngle = m_userDesireAngle.x;
//                        curCameraModeParam.curDistance = m_userDesireDistance;
//                        cameraControlModeParam[(int)curCameraMode] = curCameraModeParam;
//                    }

//                    curCameraMode = (CameraControlMode)i;
//                    curCameraModeParam = cameraControlModeParam[(int)curCameraMode];
//                    if (curCameraMode != CameraControlMode.GodEyeControl)
//                        SendCameraFocusPosToLogic(0);
//                    if (curCameraMode == CameraControlMode.FloatingControl || curCameraMode == CameraControlMode.RidingControl)
//                    {
//                        EnterFloatingMode();
//                    }
//                    else if (curCameraMode == CameraControlMode.SniperscopeControl)
//                    {
//                        setSniperscope(true);
//                    }
//                    else if (curCameraMode == CameraControlMode.FPSMode)
//                    {
//                        setFPS(true);
//                    }

//                    if (curCameraMode == CameraControlMode.FPSMode)
//                    {
//                        rayTraceAngle = 30.0f;
//                    }
//                    else
//                    {
//                        rayTraceAngle = 60.0f;
//                    }


//                    if (curCameraMode == CameraControlMode.UserControl || curCameraMode == CameraControlMode.RidingControl || curCameraMode == CameraControlMode.FloatingControl || curCameraMode == CameraControlMode.FPSMode || curCameraMode == CameraControlMode.SniperscopeControl)
//                    {
//                        m_userDesireFov = curCameraModeParam.FOV;
//                        m_userDesireAngle.x = curCameraModeParam.curPitchAngle;
//                        m_userDesireDistance = curCameraModeParam.curDistance;

//                        if (CurCameraProperty != null)
//                        {
//                            CurCameraProperty.NormalFOV = (int)curCameraModeParam.FOV;
//                            CurCameraProperty.NormalDistance = curCameraModeParam.curDistance;
//                            CurCameraProperty.NormalHeight = curCameraModeParam.targetOffset;
//                            CurCameraProperty.InitedEulerAngle.x = curCameraModeParam.curPitchAngle;
//                        }
//                    }
//                }
//                return;
//            }
//        }
//    }

//    public void setRidingCameraParam(float distance, Vector2 angleRange)
//    {
//        if (distance <= 0.001f)
//        {
//            distance = m_userDesireDistance;
//        }
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].curDistance = distance;
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].distanceRange = new Vector2(distance,distance);
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].curPitchAngle = Mathf.Clamp(m_userDesireAngle.x, angleRange.x, angleRange.y);
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].pitchAngleRange = angleRange;
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].FOV = m_userDesireFov;
//        cameraControlModeParam[(int)CameraControlMode.RidingControl].targetOffset = curCameraModeParam.targetOffset;
//    }

//    public void enableMode(CameraControlMode mode, bool enable)
//    {
//        //Debug.Log("switch mode!!");
//        if (enableCameraControlMode == null || enableCameraControlMode.Length<(int)CameraControlMode.Count)
//        {
//            Reset();
//        }
//        int index = (int)mode;
//        enableCameraControlMode[index] = enable;
//        switchMode();
//    }

//    public void enableMode(CameraControlMode mode, bool enable, CameraParam param)
//    {
//        int index = (int)mode;

//        //下面赋值参数，默认赋值的不覆盖
//        if (param.curDistance > -999.0f)
//        {
//            cameraControlModeParam[index].curDistance = param.curDistance;
//        }

//        if (param.distanceRange.sqrMagnitude > 0.0001f)
//        {
//            cameraControlModeParam[index].distanceRange = param.distanceRange;
//        }

//        if (param.targetOffset>-999.0f)
//        {
//            cameraControlModeParam[index].targetOffset=param.targetOffset;
//        }

//        if (param.curPitchAngle > -999.0f)
//        {
//            cameraControlModeParam[index].curPitchAngle = param.curPitchAngle;
//        }

//        if (param.pitchAngleRange.sqrMagnitude > 0.0001f)
//        {
//            cameraControlModeParam[index].pitchAngleRange = param.pitchAngleRange;
//        }

//        if (param.FOV > -999.0f)
//        {
//            cameraControlModeParam[index].FOV = param.FOV;
//        }

//        enableMode(mode, enable);
//    }

//    private bool canUserControlCamera()
//    {
//        //下面开始每帧update流程
//        switch (curCameraMode)
//        {
//            case CameraControlMode.UserControl:
//            case CameraControlMode.RidingControl:
//            case CameraControlMode.FloatingControl:
//            case CameraControlMode.FPSMode:
//            case CameraControlMode.SniperscopeControl:
//            case CameraControlMode.FollowTarget:
//                return true;
//            default:
//                return false;
//        }
//    }

//    //不需要按鼠标右键就直接旋转镜头
//    private bool canMouseRotateCamera()
//    {
//        if (curCameraMode == CameraControlMode.SniperscopeControl || (curCameraMode == CameraControlMode.FPSMode && m_bRotateWithMouse))
//        {
//            return true;
//        }
//        return false;
//    }


//    //读取摄像机参数配置
//    private int currentSceneType = 0;
//    private int currentHeroType = 0;
//    public void updateCameraParam(int sceneType, int heroType)
//    {
//        if (sceneType >= 0)
//        {
//            currentSceneType = sceneType;
//        }
//        if (heroType >= 0)
//        {
//            currentHeroType = heroType;
//        }


//        CameraParam[] cameraInfo = ASpeedGame.Data.CameraConfig.CameraConfig.Instance.GetCameraConfigInfo(sceneType, heroType);
//        if (cameraInfo != null)
//        {
//            if (cameraControlModeParam == null)
//            {
//                cameraControlModeParam = new CameraParam[(int)CameraControlMode.Count];

//                for (int i = 0; i < (int)CameraControlMode.Count; i++)
//                {
//                    cameraControlModeParam[i] = new CameraParam();
//                }
//            }
//            cameraControlModeParam[(int)CameraControlMode.UserControl] = cameraInfo[0];
//            cameraControlModeParam[(int)CameraControlMode.FloatingControl] = cameraInfo[1];
//            cameraControlModeParam[(int)CameraControlMode.FPSMode] = cameraInfo[2];
//            cameraControlModeParam[(int)CameraControlMode.SniperscopeControl] = cameraInfo[3];            
//        }

//        if (curCameraMode < CameraControlMode.Count)
//        {
//            curCameraModeParam = cameraControlModeParam[(int)curCameraMode];
//            m_userDesireFov = curCameraModeParam.FOV;
//            m_userDesireAngle.x = curCameraModeParam.curPitchAngle;
//            m_userDesireDistance = curCameraModeParam.curDistance;

//            if (CurCameraProperty != null)
//            {
//                CurCameraProperty.NormalFOV = (int)curCameraModeParam.FOV;
//                CurCameraProperty.NormalDistance = curCameraModeParam.curDistance;
//                CurCameraProperty.NormalHeight = curCameraModeParam.targetOffset;
//                CurCameraProperty.InitedEulerAngle.x = curCameraModeParam.curPitchAngle;
//            }
//        }
//        else
//        {
//            enableMode(CameraControlMode.UserControl, true);
//            switchMode();
//        }

//    }

//    #endregion

//    #region 镜头每帧更新控制


//    public void LateUpdate()
//    {
//        if (soldier == null || m_Camera == null || curCameraMode >= CameraControlMode.Count)
//        {
//            return;
//        }

//        if (curCameraMode == CameraControlMode.CameraAnim)
//        {
//            updateCameraAnim();
//            return;
//        }

//        if (curCameraMode == CameraControlMode.FPSMode)
//        {
//            updateFPS();
//        }

//        deltaTime = Time.deltaTime;
//        if (deltaTime > maxDetlaTime)
//        {
//            deltaTime = maxDetlaTime;
//        }

//        //切换上帝模式
//        if(FuntionShortCutCtrl.CheckFunctionShortcutKeyUp(FuntionShortCutSlotIndex.LockView))
//        {
//            if (!isGodEye_VMode && curCameraMode != CameraControlMode.GodEyeControl)
//            {
//                SwitchGodEye(soldier);
//                isGodEye_VMode = true;
//                godEyeStartTime = Time.time;
//            }
//            else if(isGodEye_VMode && curCameraMode == CameraControlMode.GodEyeControl)
//            {
//                LeaveGodEyeMode();
//                isGodEye_VMode = false;
//            }
//        }

//        if (curCameraMode == CameraControlMode.GodEyeControl && soldier.gameObject!=null)
//        {
//            if (isGodEye_VMode && Time.time - godEyeStartTime >=GOD_EYE_TIME)
//            {
//                isGodEye_VMode = false;
//                LeaveGodEyeMode();
//            }
//            else if( isGodEye_VMode && Time.time - godEyeStartTime < GOD_EYE_TIME)
//            {
//                ChangeGodEyePos(soldier.gameObject.transform.position);
//                if (mouseCtrl.cameraRotateButton)
//                {
//                    mouseCtrl.RotateWithMouse = true;
//                    MouseCursorManager.Instance.MouseCursorVisible = true;
//                    Vector3 angle = GetGodEyeAngle();
//                    float newY = angle.y + mouseCtrl.fMouseAxisX * Time.deltaTime * 30;
//                    float newX = angle.x - mouseCtrl.fMouseAxisY * Time.deltaTime * 30;

//                    m_userDesireAngle.Set(newX, newY, angle.z);
//                    ChangeGodEyeAngle(m_userDesireAngle);
//                }
//            }
//           else  if (!isGodEye_VMode && (InputManager.GetKeyDown(KeyCode.W) || InputManager.GetKeyDown(KeyCode.S) || InputManager.GetKeyDown(KeyCode.A) || InputManager.GetKeyDown(KeyCode.D)
//                || InputManager.GetKeyDown(KeyCode.UpArrow) || InputManager.GetKeyDown(KeyCode.DownArrow) || InputManager.GetKeyDown(KeyCode.LeftArrow) || InputManager.GetKeyDown(KeyCode.RightArrow)))
//            {
//                if (EntityFactory.MainHeroView.StateMachine.GetState() != EntityState.Dead) //非死亡状态才推出上帝视角
//                {
//                    LeaveGodEyeMode();
//                    isGodEye_VMode = false;
//                }
//            }
//            else if (isGodEye_DeadMode && !isGodEye_UIMode)
//            {
//                ChangeGodEyePos(soldier.gameObject.transform.position);
//            }
//            if (!isGodEye_UIMode)
//            {
//                if (mouseCtrl.cameraRotateButton && !UISystem.Instance.IsMouseOverUI())
//                {
//                    mouseCtrl.RotateWithMouse = true;
//                    MouseCursorManager.Instance.MouseCursorVisible = true;
//                    Vector3 angle = GetGodEyeAngle();
//                    float newY = angle.y + mouseCtrl.fMouseAxisX * Time.deltaTime * 30;
//                    float newX = angle.x - mouseCtrl.fMouseAxisY * Time.deltaTime * 30;

//                    m_userDesireAngle.Set(newX, newY, angle.z);
//                    ChangeGodEyeAngle(m_userDesireAngle);
//                }
//            }
//        }



//        //下面开始每帧update流程
//        switch (curCameraMode)
//        {
//            case CameraControlMode.UserControl:
//            case CameraControlMode.RidingControl:
//            case CameraControlMode.FloatingControl:
//            case CameraControlMode.FPSMode:
//            case CameraControlMode.SniperscopeControl:
//            case CameraControlMode.FollowTarget:
//                NavNormally(); //镜头自动跟随，包括切换模式时候的过渡或者摄像机
//                if (isPlayingEffect)
//                {
//                    advanceByEffect(deltaTime);
//                }
//                if (soldierController != null && canPlayControl)
//                {
//                    GetInputNormally(curCameraModeParam.pitchAngleRange);
//                }
//                Vector3 cOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
//                UpdateTransformNormally(cOffset); //计算摄像机具体参数
//                CollectNormally(); //导航数据采集
//                if (soldierController != null && soldierController.dead)
//                    CalNormalCameraFocusPos();
//                break;
//            case CameraControlMode.GodEyeControl:
//                UpdateStateByGodEye();
//                if (!isGodEye_VMode)
//                {
//                    GetInputGodEye();
//                }
//                UpdateTransformByGodEye(); //计算摄像机具体参数
//                UpdateMinimapGodEye();  // 小地图画框
//                CalGodEyeCameraFocusPos();
//                break;
//        }
//    }

//    private float dx;
//    private float dy;
//    public float k1 = 1.0f;
//    public float k2 = 1.0f;
//    public float k3 = 1.0f;
//    private float n = 1.0f;
//    public float maxn = 5.0f;
//    public float minn = 0.0f;
//    private float rx;
//    private float ry;

//    private void GetInputNormally(Vector2 angleClamp)
//    {
//        if (mouseCtrl == null)
//        {
//            return;
//        }

//        float fMouseScrollValue = 0.0f;
//        if (InputManager.IsMouseInScreen())
//        {
//            fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
//        }

//        if (Mathf.Abs(fMouseScrollValue) > 0.01f)
//        {
//            if ((Application.isEditor) && (InputManager.GetKey(KeyCode.LeftControl) || InputManager.GetKey(KeyCode.RightControl)))
//            {
//                //m_userDesireFov = CurCameraProperty.NormalFOV;
//                m_userDesireFov = m_userDesireFov - fMouseScrollValue * CurCameraProperty.ScrollSpeed;
//                m_userDesireFov = Mathf.Clamp(m_userDesireFov, 10.0f, 120.0f);
//            }
//            else if (canUserControlCamera())
//            {
//                // normalDistance -= InputManager.GetAxis("Mouse ScrollWheel") * Time.deltaTime * CurCameraProperty.ScrollSpeed;//normalDistance -= InputManager.GetAxis("Mouse ScrollWheel") * Time.deltaTime * scrollSpeed;
//                m_userDesireDistance = Mathf.Lerp(m_userDesireDistance, m_userDesireDistance - fMouseScrollValue * CurCameraProperty.ScrollSpeed, Time.deltaTime);
//                m_userDesireDistance = Mathf.Clamp(m_userDesireDistance, curCameraModeParam.distanceRange.x, curCameraModeParam.distanceRange.y);//normalDistance = Mathf.Clamp(normalDistance, minDistance, maxDistance);
//            }

//            NavPauseTime = Time.time;//如果用户操作了暂停自动导航一会
//        }

//        //m_userDesireDir = new Vector3(CurCameraProperty.NormalDirection.z, CurCameraProperty.NormalDirection.y, CurCameraProperty.NormalDirection.x);

//        //m_userDesireTran = new Vector3(0, CurCameraProperty.NormalHeight, 0);

//        if (!soldier.gameObject)
//        {
//            Trace.Warning("SoliderCamera.soldier.gameObject is null!");
//            return;
//        }
        
//        float dis = (m_userDesirePos - soldier.gameObject.transform.position).magnitude;
//        if (dis > 10.0f || curCameraMode == CameraControlMode.FPSMode)
//        {
//            m_userDesirePos = soldier.gameObject.transform.position;
//        }
//        else
//        {
//            //帧率波动距离时，摄像机会抖动，特别是左右移动时此现象明显
//            //这里对摄像机位移利用逼近方案平滑，解决抖动
//            //位移逼近方案
//            Vector3 apprMoveX = approximateVec(m_userDesirePos, soldier.gameObject.transform.position, smoothFactor.x) - m_userDesirePos;
//            Vector3 apprMoveY = approximateVec(m_userDesirePos, soldier.gameObject.transform.position, smoothFactor.y) - m_userDesirePos;
//            Vector3 apprMoveZ = approximateVec(m_userDesirePos, soldier.gameObject.transform.position, smoothFactor.z) - m_userDesirePos;
//            float moveRight = Vector3.Dot(apprMoveX, camTransform.right);
//            float moveForward = Vector3.Dot(apprMoveZ, camTransform.forward);
//            float moveUp = Vector3.Dot(apprMoveY, camTransform.up);

//            //以下代码帧率突变进行统计，为平滑参数提供参考
//            if (adaptSmooth)
//            {
//                //用标准差的方式算帧率突变的程度
//                //每m_deviationN帧算一组，该组的平均值作为下一组的平均值使用
//                //累计m_deviationN后平滑因子按(0.75f - Mathf.Sqrt(deviation)*2.0f)计算，并且和上一个平滑因子37开叠加，避免平滑因子突变
//                float deltaTime = Time.deltaTime;
//                if (m_avgDetaTime < 0.01f)
//                {
//                    m_avgDetaTime = deltaTime;
//                }

//                m_accDetaTime += deltaTime;
//                float k = deltaTime - m_avgDetaTime;
//                k = k * k;
//                m_accDeviation += k;

//                m_deviationCount++;
//                if (m_deviationCount >= m_deviationN)
//                {
//                    m_avgDetaTime = m_accDetaTime / m_deviationN;
//                    float deviation = Mathf.Sqrt(m_accDeviation / m_deviationN);
//                    //Debug.Log("dev=" + deviation);
//                    smoothFactor.x = smoothFactor.x * 0.3f + (0.75f - Mathf.Sqrt(deviation) * 2.0f) * 0.7f;
//                    if (smoothFactor.x < 0.2f) { smoothFactor.x = 0.2f; }
//                    m_accDetaTime = 0.0f;
//                    m_accDeviation = 0.0f;
//                    m_deviationCount = 0;
//                }
//            }


//            /*
//            //速度逼近方案
//            Vector3 moveVec = soldier.position - m_userDesirePos;
//            float moveRight = Vector3.Dot(moveVec, camTransform.right);
//            float moveForward = Vector3.Dot(moveVec, camTransform.forward);
//            float moveUp = Vector3.Dot(moveVec, camTransform.up);
//            float speedRight = moveRight - m_lastMoveRight;
//            float apprSpeedRight = approximateFloat(m_lastSpeedRight, speedRight, smoothFactor.x);
//            if (Mathf.Abs(speedRight) > Mathf.Abs(apprSpeedRight))
//            {
//                speedRight = apprSpeedRight;
//            }
//            if (Mathf.Abs(moveRight) > Mathf.Abs(m_lastMoveRight + speedRight))
//            {
//                moveRight = m_lastMoveRight + speedRight;
//            }
//            m_lastMoveRight = moveRight;
//            m_lastSpeedRight = speedRight;
//            */
//            Vector3 offset = moveForward * camTransform.forward + moveUp * camTransform.up + moveRight * camTransform.right;
//            m_userDesirePos += offset;
//        }
//        //m_userDesirePos = soldier.position;

//        if ((mouseCtrl.cameraRotateButton || canMouseRotateCamera()) && canCameraRotate() && !UISystem.Instance.IsMouseOverUI())
//        {
//            float _x = mouseCtrl.fMouseAxisX;
//            float _y = mouseCtrl.fMouseAxisY;


//            if (k1 < 1.0f)
//            {
//                k1 = 1.0f;
//            }

//            //Vector2 a = soldierController.aim ? aimSpeed : speed;
//            Vector2 a = CurCameraProperty.Speed;//speed;
//            dx = Mathf.Clamp(_x * a.x, -999999999.0f, 99999999.0f);
//            dy = Mathf.Clamp(_y * a.y, -999999999.0f, 99999999.0f);

//            n = k1 * Mathf.Pow(Mathf.Sqrt(_x * _x + _y * _y) / (k3 * deltaTime), k2);

//            n = Mathf.Clamp(n, minn, maxn);
//            rx = dx * n;
//            ry = dy * n;

//            if (Mathf.Abs(rx) > 0.1f || Mathf.Abs(ry) > 0.1f)
//            {
//                exitNavMode(CameraNavMode.Target); //其他操作中断镜头追踪攻击对象
//                exitNavMode(CameraNavMode.FloatingAngle); //其他操作中断自动转镜头
//            }
            
//            m_userDesireAngle.y += rx;        //x += Mathf.Clamp(_x * a.x, -maxSpeed.x, maxSpeed.x) * deltaTime;
//            m_userDesireAngle.x -= ry;        //y -= Mathf.Clamp(_y * a.y, -maxSpeed.y, maxSpeed.y) * deltaTime;
//            //这里限制仰俯角，不能去掉
//            m_userDesireAngle.x = Mathf.Clamp(m_userDesireAngle.x, angleClamp.x, angleClamp.y);//y = ClampAngle(y, yMinLimit, yMaxLimit);
//            m_userDesireAngle.y = m_userDesireAngle.y - Mathf.Floor(m_userDesireAngle.y / 360.0f) * 360.0f;
//        }

//        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.MoveCameraPositionToFront))
//        {
//            if (soldier != null)
//            {
//                if(soldier.gameObject)
//                {
//                    bool reached = false;

//                    m_userDesireAngle.y = approximateAngleFloat(m_userDesireAngle.y, soldier.gameObject.transform.eulerAngles.y, 0.5f, ref reached);
//                }

//            }
//            exitNavMode(CameraNavMode.Target); //其他操作中断自动旋转
//            exitNavMode(CameraNavMode.FloatingAngle); //其他操作中断自动旋转
//        }
//    }

//    void UpdateTransformNormally(Vector3 cPosOffset)
//    {
//        //摄像机广角
//        if (!m_effectLockFov)
//        {
//            m_userCurFov = m_userDesireFov;
//        }
//        apprFov = approximateFloat(apprFov, m_userCurFov, apprFactor);
//        float finalFov = apprFov + m_effectCurFov;
//        if (finalFov > 150.0f) finalFov = 150.0f;
//        m_Camera.fieldOfView = finalFov;

//        //根据角度计算坐标基
//        if (!m_effectLockAngle)
//        {
//            m_userCurAngle = m_userDesireAngle;
//        }
//        apprAngle = approximateAngleVec(apprAngle, m_userCurAngle, 1.0f);
//        Vector3 finalAngle = apprAngle + m_effectCurAngle;
//        Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(finalAngle), Vector3.one);
//        //Vector3 baseright = new Vector3(basem.m00, basem.m10, basem.m20);
//        //Vector3 baseup = new Vector3(basem.m01, basem.m11, basem.m21);
//        //Vector3 baseforward = new Vector3(basem.m02, basem.m12, basem.m22);

//        //摄像机方向，一般调整上面的角度就可以了，这里是据说是调整错切的
//        if (!m_effectLockDir)
//        {
//            m_userCurDir = m_userDesireDir;
//        }
//        apprDir = approximateVec(apprDir, m_userCurDir, 1.0f);
//        Vector3 normalDir = basem * apprDir;
//        normalDir = normalDir.normalized;
//        Vector3 effectedDir = basem *
//            new Vector3(m_effectCurDir.z, m_effectCurDir.y, m_effectCurDir.x);
//        effectedDir = effectedDir.normalized;
//        Vector3 finalDir = (normalDir + effectedDir).normalized;

//        //摄像机深度
//        if (!m_effectLockDistance)
//        {
//            m_userCurDistance = m_userDesireDistance;
//        }
//        apprDistance = approximateFloat(apprDistance, m_userCurDistance, apprFactor);
//        float finalDistance = apprDistance + m_effectCurDistance;
//        if (finalDistance < 1.0f) finalDistance = 1.0f;

//        //摄像机平移（相对朝向）
//        if (!m_effectLockTran)
//        {
//            m_userCurTran = m_userDesireTran;
//        }
//        apprTran = approximateVec(apprTran, m_userCurTran, apprFactor);

//        Vector3 finalTran = apprTran + m_effectCurTran;
//        finalTran = HandleCameraShake(finalTran);//震屏

//        //摄像机位置（绝对世界）
//        if (!m_effectLockPos)
//        {
//            m_userCurPos = m_userDesirePos;
//        }
//        apprPos = approximateVec(apprPos, m_userCurPos, apprFactor);
//        Vector3 finalPos = apprPos + m_effectCurPos;

//        //摄像机目标点位置
//        {
//            Vector3 transformedtran = basem * finalTran;
//            cPos = finalPos + transformedtran;
//        }

//        //设置摄像机
//        Vector3 targetPos = cPos + cPosOffset;


//        if (curCameraMode == CameraControlMode.SniperscopeControl && soldier != null && soldier.gameObject)
//        {
//            //把摄像机位置拉到枪上
//            Vector3 camPos=Vector3.zero;
//            if (m_sniperscopreWeapon)
//            {
//                camPos = m_sniperscopreWeapon.position - finalDir * m_userCurTran.magnitude * 0.5f;                
//            }
//            else
//            {
//                camPos = soldier.gameObject.transform.transform.position + m_userCurTran - finalDir * m_userCurTran.magnitude * 0.5f;
//            }


//            camTransform.SetPosition(camPos);
//            camTransform.forward = -finalDir;
//        }
//        else
//        {
//            camTransform.SetPosition(targetPos + (finalDir * finalDistance));
//            camTransform.LookAt(targetPos);

//            //如果摄像机与人物之间被地形阻挡，则拉近到地形以上
//            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
//            Vector3 viewVec = camTransform.position - targetPos;
//            Ray viewRay = new Ray(targetPos, viewVec.normalized);
//            RaycastHit hitInfo;
//            bool hitGround = Physics.Raycast(viewRay, out hitInfo, viewVec.magnitude, LayerMaskOnGround);
//            if (hitGround)
//            {
//                float offset = viewVec.magnitude / 2.0f;
//                offset = offset < 0.1f ? offset : 0.1f;
//                camTransform.SetPosition(hitInfo.point + (viewVec * -offset));
//            }
//        }


//        //设置目标框
//        target.SetPosition(cPos);
//        target.eulerAngles = finalAngle;
//    }


//    //强制镜头瞬间跟上英雄，而不是利用插值平滑移动镜头
//    public void ForceUpdateTransformNormally()
//    {
//        Vector3 cOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
//        switch (curCameraMode)
//        {
//            case CameraControlMode.UserControl:
//            case CameraControlMode.RidingControl:
//            case CameraControlMode.FloatingControl:
//            case CameraControlMode.FPSMode:
//            case CameraControlMode.SniperscopeControl:
//            case CameraControlMode.FollowTarget:
//                NavNormally(); //镜头自动跟随，包括切换模式时候的过渡或者摄像机
//                if (isPlayingEffect)
//                {
//                    advanceByEffect(deltaTime);
//                }
//                if (soldierController != null && canPlayControl)
//                {
//                    GetInputNormally(curCameraModeParam.pitchAngleRange);
//                }
//                break;
//            default:
//                return;
//        }



//        //摄像机广角
//        if (!m_effectLockFov)
//        {
//            m_userCurFov = m_userDesireFov;
//        }
//        apprFov = m_userCurFov;
//        float finalFov = apprFov + m_effectCurFov;
//        if (finalFov > 150.0f) finalFov = 150.0f;
//        m_Camera.fieldOfView = finalFov;

//        //根据角度计算坐标基
//        if (!m_effectLockAngle)
//        {
//            m_userCurAngle = m_userDesireAngle;
//        }
//        apprAngle = m_userCurAngle;
//        Vector3 finalAngle = apprAngle + m_effectCurAngle;
//        Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(finalAngle), Vector3.one);
//        //Vector3 baseright = new Vector3(basem.m00, basem.m10, basem.m20);
//        //Vector3 baseup = new Vector3(basem.m01, basem.m11, basem.m21);
//        //Vector3 baseforward = new Vector3(basem.m02, basem.m12, basem.m22);

//        //摄像机方向，一般调整上面的角度就可以了，这里是据说是调整错切的
//        if (!m_effectLockDir)
//        {
//            m_userCurDir = m_userDesireDir;
//        }
//        apprDir = approximateVec(apprDir, m_userCurDir, 1.0f);
//        Vector3 normalDir = basem * apprDir;
//        normalDir = normalDir.normalized;
//        Vector3 effectedDir = basem *
//            new Vector3(m_effectCurDir.z, m_effectCurDir.y, m_effectCurDir.x);
//        effectedDir = effectedDir.normalized;
//        Vector3 finalDir = (normalDir + effectedDir).normalized;

//        //摄像机深度
//        if (!m_effectLockDistance)
//        {
//            m_userCurDistance = m_userDesireDistance;
//        }
//        apprDistance = m_userCurDistance;
//        float finalDistance = apprDistance + m_effectCurDistance;
//        if (finalDistance < 1.0f) finalDistance = 1.0f;

//        //摄像机平移（相对朝向）
//        if (!m_effectLockTran)
//        {
//            m_userCurTran = m_userDesireTran;
//        }
//        apprTran = m_userCurTran;

//        Vector3 finalTran = apprTran + m_effectCurTran;
//        finalTran = HandleCameraShake(finalTran);//震屏

//        //摄像机位置（绝对世界）
//        if (!m_effectLockPos)
//        {
//            m_userCurPos = m_userDesirePos;
//        }
//        apprPos = m_userCurPos;
//        Vector3 finalPos = apprPos + m_effectCurPos;

//        //摄像机目标点位置
//        {
//            Vector3 transformedtran = basem * finalTran;
//            cPos = finalPos + transformedtran;
//        }

//        //设置摄像机
//        Vector3 targetPos = cPos + cOffset;


//        if (curCameraMode == CameraControlMode.SniperscopeControl && null != soldier && soldier.gameObject)
//        {
//            //把摄像机位置拉到枪上
//            Vector3 camPos = Vector3.zero;
//            if (m_sniperscopreWeapon)
//            {
//                camPos = m_sniperscopreWeapon.position - finalDir * m_userCurTran.magnitude * 0.5f;
//            }
//            else
//            {
//                camPos = soldier.gameObject.transform.transform.position + m_userCurTran - finalDir * m_userCurTran.magnitude * 0.5f;
//            }


//            camTransform.SetPosition(camPos);
//            camTransform.forward = -finalDir;
//        }
//        else
//        {
//            camTransform.SetPosition(targetPos + (finalDir * finalDistance));
//            camTransform.LookAt(targetPos);

//            //如果摄像机与人物之间被地形阻挡，则拉近到地形以上
//            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
//            Vector3 viewVec = camTransform.position - targetPos;
//            Ray viewRay = new Ray(targetPos, viewVec.normalized);
//            RaycastHit hitInfo;
//            bool hitGround = Physics.Raycast(viewRay, out hitInfo, viewVec.magnitude, LayerMaskOnGround);
//            if (hitGround)
//            {
//                float offset = viewVec.magnitude / 2.0f;
//                offset = offset < 0.1f ? offset : 0.1f;
//                camTransform.SetPosition(hitInfo.point + (viewVec * -offset));
//            }
//        }


//        //设置目标框
//        target.SetPosition(cPos);
//        target.eulerAngles = finalAngle;
//    }

//    #endregion




//    /////////////////////////////////////////////////////////////////////
//    #region 暂时没用的属性
//    //public int zoomFOV = 30;
//    //public Vector2 aimSpeed  = new Vector2(70.0f, 70.0f);
//    //public Vector3 aimDirection;
//    //public Vector3 crouchDirection;
//    //public Vector3 aimCrouchDirection;
//    //public float positionLerp;
//    //public float crouchHeight;
//    //public float normalAimHeight;
//    //public float crouchAimHeight;
//    //public float minHeight;
//    //public float normalAimDistance;
//    //public float crouchAimDistance;
//    #endregion

//    #region 通过参数调节的摄像机特效，用户可控镜头

//    public float minShakeSpeed;
//    public float maxShakeSpeed;
//    public float minShake;
//    public float maxShake = 2.0f;

//    public int minShakeTimes;
//    public int maxShakeTimes;

//    public float maxShakeDistance;

//    private bool shake;
//    private float shakeSpeed = 2.0f;
//    private float cShakePos;
//    private int shakeTimes = 8;
//    private float cShake;
//    private float cShakeSpeed;
//    private int cShakeTimes;

//    private float maxShakeFov=45;
//    private float minShakeFov=45.0f;
//    private float shakeFovSpeed=0.0f;

//    private bool isRandom;
//    private bool isDirDependRole;
//    private Vector3 shakeDir;
//    private float oldCShake;

//    private float GetShakeValue(float time, float totalTime)
//    {
//        float frequence = ((this.shakeTimes * 2) * 3.141593f) / (totalTime * this.cShakeSpeed);
//        float t_min = (((2f * this.shakeTimes) - 0.5f) * 3.141593f) / frequence;
//        float sin_min = Mathf.Sin(t_min * frequence);
//        float log_val = Mathf.Log(this.minShake / (this.maxShake * Mathf.Abs(sin_min)));
//        float exponent = -t_min / log_val;
//        float t = time;
//        float left = Mathf.Exp(-t / exponent);
//        float right = Mathf.Sin(frequence * t);
//        if (Mathf.Abs(right) <= 0.001f)
//        {
//            right = 0f;
//        }
//        return ((this.maxShake * left) * right);
//    }

//    Vector3 HandleCameraShake(Vector3 tran)
//    {
//        if (this.shake)
//        {
//            if (isRandom)
//            {
//                this.cShake += this.cShakeSpeed * this.deltaTime;
//                if (Mathf.Abs(this.cShake) > this.cShakePos)
//                {
//                    this.cShakeSpeed *= -1f;
//                    this.cShakeTimes++;
//                    if (this.cShakeTimes >= this.shakeTimes)
//                    {
//                        this.shake = false;
//                    }
//                    if (this.cShake > 0.0)
//                    {
//                        this.cShake = this.cShakePos;
//                    }
//                    else
//                    {
//                        this.cShake = -this.cShakePos;
//                    }
//                }
//                tran.y += this.cShake;
//                this.m_effectCurFov += this.shakeFovSpeed * this.deltaTime;
//                if (((this.shakeFovSpeed > 0f) && (this.m_effectCurFov > this.maxShakeFov)) || ((this.shakeFovSpeed < 0f) && (this.m_effectCurFov < this.minShakeFov)))
//                {
//                    this.shakeFovSpeed *= -1f;
//                }
//                return tran;
//            }
//            else
//            {
//                uint num = GameLogicAPI.getTickCount();
//                if (num >= this.m_effectStartTick)
//                {
//                    if (num >= this.m_effectStopTick)
//                    {
//                        this.stopEffect();
//                        return tran;
//                    }
//                    uint effectStartTick = this.m_effectStartTick;
//                    if (this.m_nCameraAxisIndex >= 1)
//                    {
//                        effectStartTick = (uint)this.m_cameraAxis[this.m_nCameraAxisIndex - 1].tick;
//                    }
//                    if (num <= this.m_nextEffectNodeTick)
//                    {
//                        if (this.m_nextEffectNodeTick <= num)
//                        {
//                            this.m_nextEffectNodeTick = num + 1;
//                        }
//                        float totalTime = ((float)(this.m_nextEffectNodeTick - effectStartTick)) / 1000f;
//                        float time = ((float)(num - effectStartTick)) / 1000f;
//                        //Debug.Log(time);
//                        this.cShake = this.GetShakeValue(time, totalTime);
//                        tran += (Vector3)((this.cShake - this.oldCShake) * Vector3.Normalize(this.shakeDir + (this.isDirDependRole ? soldier.gameObject.transform.forward : Vector3.zero)));
//                        this.oldCShake = this.cShake;
//                        return tran;
//                    }
//                    this.shake = false;
//                }
//            }
//        }
//        return tran;
//    }

//    void StartShake(float distance0)
//    {
//        float proximity = distance0 / maxShakeDistance;

//        if (proximity > 1.0f) return;

//        proximity = Mathf.Clamp(proximity, 0.0f, 1.0f);

//        proximity = 1.0f - proximity;

//        cShakeSpeed = Mathf.Lerp(minShakeSpeed, maxShakeSpeed, proximity);
//        shakeTimes = (int)Mathf.Lerp(minShakeTimes, maxShakeTimes, proximity);
//        cShakeTimes = 0;
//        cShakePos = Mathf.Lerp(minShake, maxShake, proximity);
//        this.oldCShake = 0f;
//        shake = true;
//    }


//    private bool isPlayingEffect = false;
//    public void playEffect(int delayTicks, int nliveTicks, Effect_CameraNode[] cameraNodes, bool recoverTransform)
//    {
//        if (cameraMode == CameraControlMode.GodEyeControl)
//        {
//            return;
//        }

//        uint tick = GameLogicAPI.getTickCount();
//        m_effectStartTick = tick + (uint)delayTicks;
//        m_effectStopTick = m_effectStartTick + (uint)nliveTicks;
//        m_cameraAxis = cameraNodes;
//        m_nCameraAxisIndex = -1;
//        m_nextEffectNodeTick = m_effectStartTick;
//        m_recoverTransform = recoverTransform;

//        if (!m_recoverTransform)
//        {
//            //如果不逐渐恢复镜头的话，停止摄像机特效时，只保留当前角度，其他的瞬间恢复。
//            m_remainAngle = true;
//            m_remainDistance = false;
//            m_remainFOV = false;
//            m_remainPosition = false;
//            m_remainDirection = false;
//            m_remainTran = false;
//        }
//        else
//        {
//            m_remainAngle = false;
//            m_remainDistance = false;
//            m_remainFOV = false;
//            m_remainPosition = false;
//            m_remainDirection = false;
//            m_remainTran = false;
//        }

//        stopEffect();
//        isPlayingEffect = true;
//        canPlayControl = true;
//        //cameraMode = CameraControlMode.UserAndEffectControl;
//    }

//    public void stopEffect()
//    {
//        if (cameraMode == CameraControlMode.GodEyeControl)
//        {
//            return;
//        }

//        m_effectLockPos = false;
//        m_effectLockTran = false;
//        m_effectLockDistance = false;
//        m_effectLockFov = false;
//        m_effectLockAngle = false;
//        m_effectLockDir = false;

//        m_effectCurFov = 0.0f;
//        m_effectCurDistance = 0.0f;
//        m_effectCurTran = Vector3.zero;
//        m_effectCurDir = Vector3.zero;
//        m_effectCurAngle = Vector3.zero;
//        m_effectCurPos = Vector3.zero;
//        isPlayingEffect = false;
//        canPlayControl = true;
//        //cameraMode = CameraControlMode.UserControl;
//        shake = false;
//    }

//    //在finishTick的时间内恢复摄像机
//    public void finishEffect(uint finishTick)
//    {
//        uint tick = GameLogicAPI.getTickCount();
//        m_nextEffectNodeTick = tick;
//        m_effectStopTick = tick + finishTick;
//        if (m_cameraAxis != null)
//        {
//            m_nCameraAxisIndex = m_cameraAxis.Length - 1;
//        }
//    }

//    private bool canPlayControl = true;
//    private void advanceByEffect(float deltaTime)
//    {
//        uint tick = GameLogicAPI.getTickCount();

//        if (tick < m_effectStartTick)
//        {
//            return;
//        }

//        if (tick >= m_effectStopTick)
//        {
//            stopEffect();
//            return;
//        }

//        if (tick >= m_nextEffectNodeTick)
//        {
//            m_effectDisSpeed = 0.0f;
//            m_effectTranSpeed = Vector3.zero;
//            m_effectDirSpeed = Vector3.zero;
//            m_effectAngleSpeed = Vector2.zero;
//            m_effectFovSpeed = 0.0f;
//            m_effectPosSpeed = Vector3.zero;
//            if (m_nCameraAxisIndex < m_cameraAxis.Length - 1)
//            {
//                m_nCameraAxisIndex += 1;
//                m_nextEffectNodeTick = m_effectStartTick + (uint)m_cameraAxis[m_nCameraAxisIndex].tick;
//                Effect_CameraNode currentNode = m_cameraAxis[m_nCameraAxisIndex];
//                canPlayControl = currentNode.playerControl;
//                //if (currentNode.playerControl)
//                //{
//                //    cameraMode = CameraControlMode.UserAndEffectControl;
//                //}
//                //else
//                //{
//                //    cameraMode = CameraControlMode.EffectControl;
//                //}
//                if (m_nextEffectNodeTick <= tick)
//                {
//                    m_nextEffectNodeTick = tick + 1;
//                }
//                float effectTime = (m_nextEffectNodeTick - tick) / 1000.0f;

//                //位置
//                m_effectLockPos = currentNode.lockPosition;
//                Vector3 pos = currentNode.pos - m_effectCurPos;
//                m_effectPosSpeed = pos / effectTime;
//                m_effectDesirePos = currentNode.pos;

//                //fov
//                m_effectLockFov = currentNode.lockFov;
//                float fov = currentNode.fov - m_effectCurFov;
//                m_effectFovSpeed = fov / effectTime;
//                m_effectDesireFov = currentNode.fov;

//                //距离
//                m_effectLockDistance = currentNode.lockDistance;
//                float dis = currentNode.distance - m_effectCurDistance;
//                m_effectDisSpeed = dis / effectTime;
//                m_effectDesireDistance = currentNode.distance;

//                //平移
//                m_effectLockTran = currentNode.lockTran;
//                Vector3 tran = currentNode.tran - m_effectCurTran;
//                m_effectTranSpeed = tran / effectTime;
//                m_effectDesireTran = currentNode.tran;


//                //偏移方向
//                m_effectLockDir = currentNode.lockDir;
//                Vector3 dir = currentNode.dir - m_effectCurDir;
//                m_effectDirSpeed = dir / effectTime;
//                m_effectDesireDir = currentNode.dir;


//                //角度
//                m_effectLockAngle = currentNode.lockAngle;
//                Vector3 angleDis = currentNode.angle - m_effectCurAngle;
//                m_effectAngleSpeed = angleDis / effectTime;
//                m_effectDesireAngle = currentNode.angle;


//                if (currentNode.shakeCamera == true)
//                {
//                    minShakeSpeed = currentNode.shakeParam.minShakeSpeed;
//                    maxShakeSpeed = currentNode.shakeParam.maxShakeSpeed;
//                    minShake = currentNode.shakeParam.minShake;
//                    maxShake = currentNode.shakeParam.maxShake;
//                    minShakeTimes = currentNode.shakeParam.minShakeTimes;
//                    maxShakeTimes = currentNode.shakeParam.maxShakeTimes;

//                    maxShakeFov = currentNode.shakeParam.maxShakeFov;
//                    minShakeFov = currentNode.shakeParam.minShakeFov;
//                    shakeFovSpeed = currentNode.shakeParam.shakeFovSpeed;
//                    this.shakeDir = currentNode.shakeParam.shakeDir;
//                    this.isDirDependRole = currentNode.shakeParam.isDependRole;
//                    this.isRandom = currentNode.shakeParam.isRandom;
//                    int rand = UnityEngine.Random.Range(0, 99);
//                    maxShakeDistance = 100.0f / ((float)rand);
//                    StartShake(1.0f);
//                }
//            }
//            else
//            {
//                if (m_recoverTransform)
//                {
//                    //所有镜头动作已经做完，最后到StopTick这段时间内开始逐渐恢复镜头
//                    //m_effectPlayerControl = true;
//                    m_nextEffectNodeTick = m_effectStopTick;
//                    float fEffectTime = (m_effectStopTick - tick) / 1000.0f;
//                    m_effectDisSpeed = -m_effectCurDistance / fEffectTime;
//                    m_effectTranSpeed = -m_effectCurTran / fEffectTime;
//                    m_effectDirSpeed = -m_effectCurDir / fEffectTime;
//                    m_effectFovSpeed = -m_effectCurFov / fEffectTime;
//                    m_effectAngleSpeed = -m_effectCurAngle / fEffectTime;
//                    m_effectPosSpeed = -m_effectCurPos / fEffectTime;
//                    m_effectDesireDistance = 0.0f;
//                    m_effectDesireFov = 0.0f;
//                    m_effectDesireTran = Vector3.zero;
//                    m_effectDesireDir = Vector3.zero;
//                    m_effectDesireAngle = Vector3.zero;
//                    m_effectDesirePos = Vector3.zero;
//                    shake = false;
//                }
//                else
//                {
//                    if (m_remainAngle) { m_userDesireAngle += m_effectCurAngle; }
//                    if (m_remainDistance) { m_userCurDistance += m_userCurDistance; }
//                    if (m_remainFOV) { m_userDesireFov = m_userCurFov; }
//                    if (m_remainPosition) { m_userDesirePos = m_userCurPos; }
//                    if (m_remainDirection) { m_userDesireDir = m_userCurDir; }
//                    if (m_remainTran) { m_userDesireTran = m_userCurTran; }
//                    stopEffect();
//                    return;
//                }
//            }
//        }


//        advanceValue(ref m_effectCurFov, m_effectDesireFov, m_effectFovSpeed * deltaTime);
//        advanceValue(ref m_effectCurDistance, m_effectDesireDistance, m_effectDisSpeed * deltaTime);
//        advanceVec(ref m_effectCurPos, m_effectDesirePos, m_effectPosSpeed * deltaTime);
//        advanceVec(ref m_effectCurTran, m_effectDesireTran, m_effectTranSpeed * deltaTime);
//        advanceVec(ref m_effectCurDir, m_effectDesireDir, m_effectDirSpeed * deltaTime);
//        advanceVec(ref m_effectCurAngle, m_effectDesireAngle, m_effectAngleSpeed * deltaTime);
//    }

//    private void advanceValue(ref float cur, float dest, float advance)
//    {
//        float delta = dest - cur;
//        if (advance * advance > delta * delta)
//        {
//            advance = delta;
//        }
//        cur += advance;
//    }

//    private void advanceVec(ref Vector3 cur, Vector3 dest, Vector3 advance)
//    {
//        Vector3 delta = dest - cur;
//        if (advance.sqrMagnitude > delta.sqrMagnitude)
//        {
//            advance = delta;
//        }
//        cur += advance;
//    }


//    #endregion

//    //狙击镜效果
//    private Transform m_sniperscopreWeapon = null;
//    public void setSniperscope(bool enable)
//    {
//        if (enable)
//        {
//            //if (targetPos.sqrMagnitude>0.01f)
//            //{
//            //    Vector3 recPos = camTransform.position;
//            //    Vector3 recAngle = camTransform.eulerAngles;
//            //    camTransform.SetPosition( soldier.transform.position + m_userCurTran);
//            //    camTransform.LookAt(targetPos);
//            //    m_fSniperscopeAngleOffset = camTransform.eulerAngles - recAngle;
//            //    camTransform.SetPosition(recPos);
//            //    camTransform.eulerAngles = recAngle;
//            //}
//            m_sniperscopreWeapon = null;
//            if (EntityFactory.MainHeroView != null)
//            {
//                if (EntityFactory.MainHeroView.SkinControl!=null)
//                {
//                    if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance!=null)
//                    {
//                        if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig!=null)
//                        {
//                            if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender!=null)
//                            {
//                                EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender.enabled=false;
//                                CreatureStateMachine state = EntityFactory.MainHeroView.StateMachine as CreatureStateMachine;
//                                if (state != null)
//                                {
//                                    m_sniperscopreWeapon = state.SkinConfig.weaponObject;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        else
//        {
//            if (EntityFactory.MainHeroView.SkinControl != null)
//            {
//                if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance != null)
//                {
//                    if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig != null)
//                    {
//                        if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender != null)
//                        {
//                            EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender.enabled = true;
//                        }
//                    }
//                }
//            }

//        }
//        if (mouseCtrl != null && mouseCtrl.RotateWithMouse != enable)
//        {
//            mouseCtrl.RotateWithMouse = enable;
//        }
//        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,
//    new UIMsgCmdData((int)WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, Convert.ToInt32(enable), string.Empty, IntPtr.Zero, 0));

//    }
//    public bool isSniperscope()
//    {
//        return m_bSniperscope;
//    }
//    private bool m_bSniperscope = false;
//    private Vector3 m_fSniperscopeAngleOffset = Vector3.zero;
//    private float m_fSniperScopreFovOffset = 0.0f;


//    #region FPSMode

//    private bool m_bRotateWithMouse = true;
//    private bool m_bFPSShowCursor = false;

//    public void setFPS(bool enable)
//    {
//        m_bRotateWithMouse = enable;
//        WarFrontSightDataManager.Instance.FrontSightVisible = enable;
//        Cursor.lockState = enable ? CursorLockMode.Locked : CursorLockMode.None;
//        if(mouseCtrl != null)
//            mouseCtrl.RotateWithMouse = enable;
//    }

//    public bool bFPSShowCursor
//    {
//        get { return m_bFPSShowCursor; }
//        set { m_bFPSShowCursor = value; }
//    }

//    private void updateFPS()
//    {
//        if (InputManager.Raw_GetKey(KeyCode.LeftAlt) || InputManager.Raw_GetKey(KeyCode.RightAlt) || bFPSShowCursor)
//        {

//            m_bRotateWithMouse = false;
//        }
//        else
//        {
//            m_bRotateWithMouse = true;
//        }
//    }

//    public bool bRotateWithMouse
//    {
//        get { return (curCameraMode==CameraControlMode.FPSMode && m_bRotateWithMouse); }
//    }
//    #endregion

//    /////////////////////////////////////////////////////////////////////
//    #region 相机逻辑计算属性
//    //private float distance = 10.0f;//Shake Distance
//    private Transform camTransform;
//    //private Quaternion rotation; 
//    private Vector3 cPos;
//    private float deltaTime;
//    private Quaternion originalSoldierRotation;
//    //private float crouchDistance = 2.3f;

//    //镜头特效移动摄像机的相关参数
//    private Effect_CameraNode[] m_cameraAxis;
//    private uint m_effectStartTick;
//    private uint m_effectStopTick;
//    private uint m_nextEffectNodeTick;
//    private int m_nCameraAxisIndex;
//    private bool m_recoverTransform;
//    private bool m_remainAngle;
//    private bool m_remainDistance;
//    private bool m_remainFOV;
//    private bool m_remainPosition;
//    private bool m_remainDirection;
//    private bool m_remainTran;

//    //镜头特效速度
//    private float m_effectDisSpeed;
//    private Vector3 m_effectTranSpeed;
//    private Vector3 m_effectDirSpeed;
//    private Vector3 m_effectAngleSpeed;
//    private float m_effectFovSpeed;
//    private Vector3 m_effectPosSpeed;

//    //镜头特效锁定参数
//    private bool m_effectLockPos = false;
//    private bool m_effectLockTran = false;
//    private bool m_effectLockDir = false;
//    private bool m_effectLockDistance = false;
//    private bool m_effectLockFov = false;
//    private bool m_effectLockAngle = false;

//    //镜头特效期望参数
//    private float m_effectDesireFov = 0.0f;
//    private float m_effectDesireDistance = 0.0f;
//    private Vector3 m_effectDesirePos = Vector3.zero;
//    private Vector3 m_effectDesireTran = Vector3.zero;
//    private Vector3 m_effectDesireDir = Vector3.zero;
//    private Vector3 m_effectDesireAngle = Vector3.zero;

//    //镜头特效当前参数
//    private Vector3 m_effectCurPos = Vector3.zero;//镜头特效带来的位置拉伸效果
//    private float m_effectCurDistance = 0.0f; //镜头特效带来的距离拉伸效果
//    private Vector3 m_effectCurTran = Vector3.zero; //镜头特效带来的平移拉伸效果
//    private Vector3 m_effectCurDir = Vector3.zero;//镜头特效带来的方向拉伸效果
//    private Vector3 m_effectCurAngle = Vector3.zero;//镜头特效带来的方向拉伸效果
//    private float m_effectCurFov = 0.0f;//镜头特写带来的广角拉伸效果

//    //镜头用户操作期望参数
//    private Vector3 m_userDesirePos = Vector3.zero;
//    private Vector3 m_userDesireTran = Vector3.zero;
//    private Vector3 m_userDesireAngle = Vector3.zero;
//    private Vector3 m_userDesireDir = Vector3.zero;
//    private float m_userDesireFov = 0.0f;
//    private float m_userDesireDistance = 2.3f;

//    //镜头用户操作当前值
//    private Vector3 m_userCurPos = Vector3.zero;
//    private Vector3 m_userCurTran = Vector3.zero;
//    private Vector3 m_userCurAngle = Vector3.zero;
//    private Vector3 m_userCurDir = Vector3.zero;
//    private float m_userCurFov = 0.0f;
//    private float m_userCurDistance = 0.0f;

//    private Vector3 apprPos = Vector3.zero;
//    private float apprFov = 60.0f;
//    private Vector3 apprAngle = Vector3.zero;
//    private Vector3 apprTran = Vector3.zero;
//    private Vector3 apprDir = -Vector3.forward;
//    private float apprDistance = 7.0f;

//    public float navFactor = 0.05f;
//    public float apprFactor = 0.5f;
//    public float followNavFactor = 0.04f;


//#endregion
//    /////////////////////////////////////////////////////////////////////
//    #region  相机逻辑计算可配置属性
//    //以下属性以替换为注释部分
//    //private Vector2 speed = new Vector2(270.0f, 270.0f);   //=>CameraProperty.Speed;
//    //private Vector2 maxSpeed = new Vector2(270.0f, 270.0f);//=>CameraProperty.MaxSpeed;
//    //private int yMinLimit = -45;//=>CameraProperty.PitchAngleClamp;
//    //private int yMaxLimit = 45;//=>CameraProperty.PitchAngleClamp;
//    //private int normalFOV = 60;//=>CameraProperty.NormalFOV;
//    //private float lerpSpeed = 8.0f;//=>CameraProperty.FOVLerpSpeed;
//    //private Vector3 normalDirection = new Vector3(-1, 0, 0);//=>CameraProperty.NormalDirection;
//    //private float normalHeight;//=>CameraProperty.NormalHeight;
//    //private float maxHeight;//=>CameraProperty.MaxHeight;
//    //private float minDistance;//=>CameraProperty.DistanceClamp;
//    //private float maxDistance;//=>CameraProperty.DistanceClamp;
//    //private float scrollSpeed = 10.0f;//=>CameraProperty.ScrollSpeed;
//    [HideInInspector]
//    public CameraProperty CurCameraProperty;//相机属性,从英雄预制体获取
//    #endregion
//    /////////////////////////////////////////////////////////////////////

//    public float maxDetlaTime = 0.1f; //镜头每次移动的最大deltaTime时间，避免镜头剧烈移动
//    public Vector3 smoothFactor = new Vector3(0.2f,1.0f,1.0f);

//	private SoldierController soldierController=null;
//    private MouseCtrl mouseCtrl=null;
//    private LayerMask hitLayer = -1;



//    /// <summary>
//    /// 是否可以更新相机逻辑
//    /// </summary>
//    //private bool CanUpdateCamera = true;


//     /////////////////////////////////////////////////////////////////////////////////陈涛指定修改操作方式
//    private uint s_nLastCheckTick;  // 上次检测时间
//    //private bool s_bCursorMoving = true;            // 鼠标是否正在移动
//    //private float s_fCursorMoved = 0;               // 鼠标相对移动
//    //private float s_fLastCursorX = 0;               // 上一次更新鼠标位置
//    //private float s_fLastCursorY = 0;               // 上一次更新鼠标位置

//    //private int CheckTickCount = 30;
//    //private int CursorMoveDisCheck = 7;

//    private int s_nLastCheckTickInBegin;
//    //private float ClearIdleInBeginStateTime = 4.0f;

//    /// <summary>
//    /// 登录后，判断是站立，是则，镜头随鼠标移动
//    /// </summary>
//    //private bool IsIdleInBegin = true;

//    /// <summary>
//    /// 判断当前鼠标是否要锁定
//    /// </summary>
//    //private bool IsCurMouseLock = false;

//    private Camera m_Camera;

//    //private Vector3 vec3SendLogicFocusPos;//通知逻辑层相机焦点坐标
//    public int SendLogicFocusPosRange = 10;//每次相机焦点坐标更新在这个范围内不发，超出才发

//    public Vector2 FloatingPitchAngleClamp;//浮空时镜头限制

//    public Transform getCameraTransform()
//    {
//        return camTransform;
//    }


//    public void Copy(CameraProperty other)
//    {
//        CurCameraProperty.Copy(other);
//    }

//    //public void onSceneLoaded(ASpeedGame.Data.CameraConfig.CameraConfigInfo cameraInfo)
//    //{
//    //    if (CurCameraProperty != null && !CurCameraProperty.bFPSPriority)
//    //    {
//    //        this.FPSTargetOffset = cameraInfo.FPSTargetOffset;
//    //    }
//    //}


//    public void Reset(bool smoothblend=true)
//    {
//        cShakeTimes = 0;
//        cShake = 0.0f;
//        cShakeSpeed = shakeSpeed;

//        if (CurCameraProperty == null || CurCameraProperty.configType == 0)
//        {
//            CurCameraProperty = gameObject.GetComponent<CameraProperty>();
//        }

//        //if (CurCameraProperty != null)
//        //{
//        //    if (CurCameraProperty.bFPSPriority)
//        //    {
//        //        this.FPSTargetOffset = CurCameraProperty.FPSTargetOffset;
//        //    }
//        //}


//        enableCameraControlMode = new bool[(int)CameraControlMode.Count];
//        for (int i = 0; i < (int)CameraControlMode.Count; i++)
//        {
//            enableCameraControlMode[i] = false;
//        }
//        enableMode(CameraControlMode.UserControl, true);


//        if (target == null || soldier == null)
//        {
//            //ResNode.DestroyRes(ref this);
//            return;
//        }

//        if (!soldier.gameObject)
//        {
//            return;
//        }
//        target.parent = null;
//        if (!m_Camera)
//        {
//            m_Camera = Initialize.mainCam;
//        }
//        camTransform = m_Camera.transform;
//        Vector3 newEulerAngle = new Vector3(CurCameraProperty.InitedEulerAngle.x, m_userDesireAngle.y, CurCameraProperty.InitedEulerAngle.z);
//        camTransform.eulerAngles = newEulerAngle;
//        m_userDesireAngle = newEulerAngle;

//        originalSoldierRotation = soldier.gameObject.transform.rotation;

//        soldierController = soldier.GetComponent<SoldierController>();
//        mouseCtrl = soldier.GetComponent<MouseCtrl>();

//        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property!=null)
//        {
//            this.enableMode(CameraControlMode.FPSMode,EntityFactory.MainHeroView.Property.FPSMode);
//        }
        

//        if (CurCameraProperty == null)// || soldierController == null || mouseCtrl==null)
//        {
//            Trace.LogError("摄像机获取CameraProperty组件");
//            //Trace.LogError("不能从英雄预制体获取SoldierController or CameraProperty or MouseCtrl 组件");
//        }

//        m_userDesireDistance = CurCameraProperty.NormalDistance;
//        m_userDesireFov = CurCameraProperty.NormalFOV;
//        m_userDesireDir = new Vector3(CurCameraProperty.NormalDirection.z, CurCameraProperty.NormalDirection.y, CurCameraProperty.NormalDirection.x);
//        m_userDesireTran = new Vector3(0, CurCameraProperty.NormalHeight, 0);

//        /////////////////////////////////
//        s_nLastCheckTick = (uint)Time.frameCount;
//        s_nLastCheckTickInBegin = Time.frameCount;

//        m_effectLockPos = false;
//        m_effectLockTran = false;
//        m_effectLockDistance = false;
//        m_effectLockFov = false;
//        m_effectLockAngle = false;
//        m_effectLockDir = false;

//        m_effectCurPos = Vector3.zero;
//        m_effectCurFov = 0.0f;
//        m_effectCurDistance = 0.0f;
//        m_effectCurTran = Vector3.zero;
//        m_effectCurDir = Vector3.zero;
//        m_effectCurAngle = Vector3.zero;

//        shake = false;

//        m_userCurPos = soldier.gameObject.transform.position;
//        m_userCurTran = m_userDesireTran;
//        m_userCurAngle = m_userDesireAngle;
//        m_userCurDir = m_userDesireDir;
//        m_userCurFov = m_userDesireFov;
//        m_userCurDistance = m_userDesireDistance;

//        apprPos = m_userCurPos;
//        apprAngle = m_userCurAngle;
//        apprDir = m_userCurDir;
//        if (!smoothblend)
//        {
//            apprTran = m_userCurTran;
//            apprFov = m_userCurFov;
//            apprDistance = m_userCurDistance;
//        }
//    }


//    public void Awake()
//    {
//        m_Camera = Initialize.mainCam;
//        MainInstance = this;
//        SceneManager.SceneLoadFinish += SceneLoadFinish;
//    }

//    public void OnDestroy()
//    {
//        SceneManager.SceneLoadFinish -= SceneLoadFinish;
//    }

//    void SceneLoadFinish(object sender, SceneLoadFinishEventArgs e)
//    {

//        m_Camera = Initialize.mainCam;
//        if (!m_Camera)
//        {
//            return;
//        }
//        camTransform = m_Camera.transform;
//    }

//	public void Start () 
//	{
//        Reset();
//        //当前相机不与Player和ScreenRaycast层碰撞
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
//        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
//	}




//    float approximateFloat(float cur, float dest, float factor)
//    {
//        float delta = dest - cur;
//        if (Mathf.Abs(delta) < 0.01f)
//            return dest;
//        float ret = Mathf.Lerp(cur, dest, factor);
//        return ret;
//    }
    
//    //将镜头自动转向目标的配置
//    public float m_approximageRotateSpeed = 0.0f;
//    public float rotateStartSpeed = 120.0f;
//    public float rotateAcceleratedSpeed = 1000.0f;
//    public float rotateStartAngle = 90.0f;
//    public float rotateStartMinDistance = 5.0f;
//    public float rotateStartMaxDistance = 15.0f;


//    float approximateAngleFloat(float cur, float dest, float factor, ref bool reached, float angleThroad=0.01f)
//    {
//        reached = false;
//        float delta = Mathf.DeltaAngle(cur, dest);
//        if (Mathf.Abs(delta) < angleThroad)
//        {
//            reached = true;
//            return dest;
//        }

//        float ret;
//        if (factor < 0.01f)
//        {
//            if (delta * m_approximageRotateSpeed < 0.0f)
//            {
//                m_approximageRotateSpeed = -m_approximageRotateSpeed;
//            }
//            float step = m_approximageRotateSpeed * Time.deltaTime;
//            if (delta * rotateAcceleratedSpeed < 0.0f)
//            {
//                rotateAcceleratedSpeed = -rotateAcceleratedSpeed;
//            }
//            m_approximageRotateSpeed = m_approximageRotateSpeed + rotateAcceleratedSpeed * Time.deltaTime;
            
//            if (Mathf.Abs(Mathf.DeltaAngle(cur, dest)) < Mathf.Abs(step))
//            {
//                ret = dest;
//                reached = true;
//            }
//            else
//            {
//                ret = cur + step;
//            }
            
//        }
//        else 
//        {
//            ret = Mathf.LerpAngle(cur, dest, factor);
//        }
//        while (ret > 360) ret -= 360;
//        while (ret < 0) ret += 360;
//        return ret;
//    }

//    Vector3 approximateVec(Vector3 cur, Vector3 dest, float factor)
//    {
//        Vector3 delta = dest - cur;
//        if (delta.sqrMagnitude < 0.001f)
//            return dest;
//        Vector3 ret = Vector3.Lerp(cur, dest, factor);
//        return ret;
//    }

//    Vector3 approximateAngleVec(Vector3 cur, Vector3 dest, float factor)
//    {
//        Vector3 delta = dest - cur;
//        if (delta.sqrMagnitude < 0.001f)
//            return dest;
//        Vector3 ret;
//        ret.x = Mathf.LerpAngle(cur.x, dest.x, factor);
//        ret.y = Mathf.LerpAngle(cur.y, dest.y, factor);
//        ret.z = Mathf.LerpAngle(cur.z, dest.z, factor);
//        return ret;
//    }





//    //摄像机平滑时需要用到的统计参数
//    private float m_avgDetaTime = 0.0f;
//    private float m_accDetaTime = 0.0f;
//    private float m_accDeviation = 0.0f;
//    private int m_deviationCount = 0;
//    public bool adaptSmooth = true;
//    private float m_deviationN = 30.0f;

//    private bool canCameraRotate()
//    {
//        if ((soldierController != null) && ((soldierController.bSkillingPauseMoveRotate) || (soldierController.bStopRotate)))
//        {
//            return false;
//        }
//        return true;
//    }





//    private CameraControlMode lastCameraMode;


//    #region GodEye
//    //上帝之眼镜头模式参数
//    private float GodFov = 30.0f;
//    private Vector3 GodAngle = new Vector3(60, 0, 0);
//    private Vector3 GodDir = new Vector3(0, 0, -1);
//    private float GodDistance = 45.0f;
//    private Vector3 GodTran = new Vector3(0, 0, 0);
//    private Vector3 GodPos = new Vector3(0, 0, 0);
//    private bool GodEyeLongLock = false;
//    private bool GodEyeShortLock = false;

//    private const float GodEyeFocusPosMaxCheckTime = 5.0f;
//    private float GodEyeFocusPosCheckTime = GodEyeFocusPosMaxCheckTime+1;


//    void UpdateStateByGodEye()
//    {
//        if (!soldier.gameObject)
//        {
//            return;
//        }
//        if (InputManager.GetKey(KeyCode.Space)) //该功能会被强制切换上帝视角的功能屏蔽掉，但是与策划商量，先不管
//        {
//            GodEyeShortLock = true;
//        }
//        else
//        {
//            GodEyeShortLock = false;
//        }

//        if (InputManager.GetKeyUp(KeyCode.F9))
//        {
//            GodEyeLongLock = !GodEyeLongLock;
//        }

//        if (GodEyeLongLock || GodEyeShortLock)
//        {
//            GodPos = soldier.gameObject.transform.position;
//            GodTranSpeed = 0.0f;
//        }


//    }

//    //上帝之眼模式更新镜头参数
//    void UpdateTransformByGodEye()
//    {
//        //摄像机广角
//        apprFov = approximateFloat(apprFov, GodFov, apprFactor);
//        float finalFov = apprFov;
//        if (finalFov > 150.0f) finalFov = 150.0f;
//        m_Camera.fieldOfView = finalFov;

//        //根据角度计算坐标基
//        apprAngle = approximateAngleVec(apprAngle, GodAngle, 1.0f);
//        Vector3 finalAngle = apprAngle;
//        Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(finalAngle), Vector3.one);

//        //摄像机方向
//        Vector3 finaldir =  basem * GodDir;
//        finaldir = finaldir.normalized;
//        apprDir = approximateVec(apprDir, finaldir, 1.0f);
//        Vector3 finalDir = apprDir.normalized;

//        //摄像机深度
//        apprDistance = approximateFloat(apprDistance, tmpGodDistanceMax, apprFactor);
//        float finalDistance = apprDistance;
//        if (finalDistance < 1.0f) finalDistance = 1.0f;

//        //摄像机平移（相对朝向）
//        apprTran = approximateVec(apprTran, GodTran, apprFactor);
//        Vector3 finalTran = apprTran;

//        //摄像机位置（绝对世界）
//        apprPos = approximateVec(apprPos, GodPos, apprFactor);
//        Vector3 finalPos = apprPos;

//        //摄像机目标点位置
//        Vector3 transformedtran = basem * finalTran;
//        cPos = finalPos + transformedtran;

//        //设置摄像机

//        //刚在小地图点击/拖动，触发上帝视角时，需要根据地面调整摄像机距离

//            Ray ray = new Ray(cPos + (finalDir * finalDistance), -finalDir);
//            RaycastHit hitInfo;
//            //计算离地面的距离，然后调整
//            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
//            bool hit = Physics.Raycast(ray, out hitInfo, 200.0f, LayerMaskOnGround);
//            if (hit)
//            {
//                cPos = hitInfo.point;
//        }

//        camTransform.SetPosition( cPos + (finalDir * finalDistance));

//        camTransform.LookAt(cPos);

//        //设置目标框
//        target.SetPosition(cPos);
//        target.eulerAngles = finalAngle;
//    }


//    public float GodTranThre1 = 0.01f;
//    public float GodTranThre2 = 0.01f;
//    private float GodTranSpeed = 20.0f;
//    private float GodTranAcc = 1.0f;
//    private float GodTranAccFactor = 20.0f;
//    private float GodTranSpeedMin = 40.0f;
//    private float GodTranSpeedMax = 50.0f;
//    private float tmpGodAngleMin = 45.0f;
//    private float tmpGodAngleMax = 45.0f;
//    private float tmpGodDistanceMin = 5.0f;
//    private float tmpGodDistanceMax = 50.0f;
//    private void GetInputGodEye()
//    {
//        if (mouseCtrl == null)
//        {
//            return;
//        }

//        bool bShift = false;//InputManager.GetKey(KeyCode.LeftShift) || InputManager.GetKey(KeyCode.RightShift);

//        float MouseZ = 0.0f;
//        if (InputManager.IsMouseInScreen())
//        {
//            MouseZ = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
//        }
//        float scrollValue = MouseZ * CurCameraProperty.ScrollSpeed * 0.1f;
//        if (bShift)
//        {
//            scrollValue *= 5.0f;
//        }

//        //处理角度
//        //GodAngle.x -= scrollValue*2.0f;
//        ////策划不需要更改角度
//        //GodAngle.x = Mathf.Clamp(GodAngle.x, tmpGodAngleMin, tmpGodAngleMax);

//        GodDistance -= scrollValue;
//        GodDistance = Mathf.Clamp(GodDistance, tmpGodDistanceMin, tmpGodDistanceMax);

//        Vector3 dirforward = m_Camera.transform.forward;
//        Vector3 dirright = m_Camera.transform.right;
//        dirforward.y = 0.0f;
//        dirright.y = 0.0f;
//        dirforward.Normalize();
//        dirright.Normalize();

//        //GodPos += dirforward * _y;
//        //GodPos += dirright * _x;

//        Vector2 screenPos = InputManager.MousePosition;
//        Vector2 percentPos;
//        percentPos.x = screenPos.x / Screen.width;
//        percentPos.y = screenPos.y / Screen.height;


//        float deltatime = Time.deltaTime;
//        float deltavalue = Mathf.Clamp(deltatime, 0.0f, 0.1f) * GodTranSpeed;
//        if (!GodEyeLongLock && !GodEyeShortLock)
//        {
//            if ((percentPos.x > -GodTranThre2 && percentPos.x < GodTranThre1)
//                || (percentPos.x > 1.0f - GodTranThre1 && percentPos.x < 1.0f + GodTranThre2)
//                || (percentPos.y > -GodTranThre2 && percentPos.y < GodTranThre1)
//                || (percentPos.y > 1.0f - GodTranThre1 && percentPos.y < 1.0f + GodTranThre2))
//            {
//                GodTranAcc = deltatime;
//            }
//            else
//            {
//                GodTranAcc = -deltatime;
//            }
//            GodTranSpeed += GodTranAcc * GodTranAccFactor;
//        }
//        GodTranSpeed = Mathf.Clamp(GodTranSpeed, GodTranSpeedMin, GodTranSpeedMax);

//        if (bShift)
//        {
//            deltavalue *= 5.0f;
//        }

//        deltavalue = Mathf.Log(GodDistance) - 1.5f;

//        if (!GodEyeLongLock && !GodEyeShortLock)
//        {
//            Vector3 tmpPos = Vector3.zero;

//            if (percentPos.x > -GodTranThre2 && percentPos.x < GodTranThre1)
//            {
//                tmpPos -= dirright * deltavalue;
//            }
//            else if (percentPos.x > 1.0f - GodTranThre1 && percentPos.x < 1.0f + GodTranThre2)
//            {
//                tmpPos += dirright * deltavalue;
//            }

//            if (percentPos.y > -GodTranThre2 && percentPos.y < GodTranThre1)
//            {
//                tmpPos -= dirforward * deltavalue;
//            }
//            else if (percentPos.y > 1.0f - GodTranThre1 && percentPos.y < 1.0f + GodTranThre2)
//            {
//                tmpPos += dirforward * deltavalue;
//            }

//            float percentGodPosX, percentGodPosY, percentValue;
//            percentValue = 0.45f;
//            Rect sceneRect = LogicDataCenter.warMinimapDataManager.SceneRect;
//            if (sceneRect.width == 0)
//                sceneRect.width = 1;
//            if (sceneRect.height == 0)
//                sceneRect.height = 1;
//            percentGodPosX = ((GodPos + tmpPos).x - sceneRect.xMin) / sceneRect.width - percentValue;
//            //这里Y坐标有翻转，所以乘个-1
//            percentGodPosY = ((GodPos + tmpPos).z - sceneRect.yMin) * (-1) / sceneRect.height - percentValue;
//            if (percentGodPosX > percentValue)
//                tmpPos.x = 0;
//            else if (percentGodPosX < -(percentValue - 0.1))
//                tmpPos.x = 0;

//            if (percentGodPosY > percentValue)
//                tmpPos.z = 0 ;
//            else if (percentGodPosY < -(percentValue - 0.1))
//                tmpPos.z = 0; 

//            GodPos += tmpPos;
//            //策划说上帝模式下不要旋转摄像机
//            //if (mouseCtrl.isCameraCtrl())
//            //{
//            //    float _x = mouseCtrl.fMouseAxisX;
//            //    float _y = mouseCtrl.fMouseAxisY;

//            //    GodAngle.y += _x;
//            //    //GodAngle.x += _y;
//            //}
//        }

//    }

//    //切换上帝之眼模式
//    public void SwitchGodEye(U3D_Render.EntityView  theSoldier)
//    {
//        if (cameraMode != CameraControlMode.GodEyeControl || isGodEye_UIMode)   //UI小地图上帝模式，同样可以进入
//        {
//            SMinimapInfo m_sCurMinimapInfo = new SMinimapInfo();
//            int nAngle = 270;
//            if (GameLogicAPI.getMinimapInfo(SceneManager.Instance.m_curMapID, ref m_sCurMinimapInfo))
//            {
//                string[] strRotation = ((string)(m_sCurMinimapInfo.szDefaultRotation)).Split(';');
//                for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
//                {
//                    int nCamp = Convert.ToInt32(strRotation[idx]);
//                    if (EntityFactory.MainHeroView.createinfo.byCamp == nCamp)
//                    {
//                        nAngle = Convert.ToInt32(strRotation[idx + 1]) + (360 - m_sCurMinimapInfo.nOriRotation);
//                        break;
//                    }
//                }
//                Vector3 godDir = new Vector3(0, 0, -1);
//                float realAngle = isGodEye_DeadMode ? nAngle : (float)m_userDesireAngle.y;  //如果是死亡上帝视角，就用回原来的
//                EnterGodEyeMode(theSoldier.gameObject.transform.position, new Vector3(50.0f, realAngle, 0.0f), godDir, new Vector3(0, 0, 0), 35.0f, 30.0f,false);
//            }
//        }
//    }

//    public void EnterWarEndMode(Vector3 pos)
//    {
//        SMinimapInfo m_sCurMinimapInfo = new SMinimapInfo();
//        int nAngle = 270;
//        if (GameLogicAPI.getMinimapInfo(SceneManager.Instance.m_curMapID, ref m_sCurMinimapInfo))
//        {
//            string[] strRotation = ((string)(m_sCurMinimapInfo.szDefaultRotation)).Split(';');
//            for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
//            {
//                int nCamp = Convert.ToInt32(strRotation[idx]);
//                if (EntityFactory.MainHeroView.createinfo.byCamp == nCamp)
//                {
//                    nAngle = Convert.ToInt32(strRotation[idx + 1]) + (360 - m_sCurMinimapInfo.nOriRotation);
//                    break;
//                }
//            }
//            Vector3 godDir = new Vector3(0, 0, -1);
//            float realAngle = isGodEye_DeadMode ? nAngle : (float)m_userDesireAngle.y;  //如果是死亡上帝视角，就用回原来的
//            EnterGodEyeMode(pos, new Vector3(50.0f, realAngle, 0.0f), godDir, new Vector3(0, 0, 0), 35.0f, 30.0f, false);
//        }
//    }

//    //进入上帝之眼EnterGodEyeMode(pos,angle,new Vector3(0,0,-1),new Vector3(0,0,0),20.0f,60.0f);
//    private bool isGodEye_UIMode = false;
//    public void EnterGodEyeMode(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov, bool isMiniMap = true)
//    {        
//        GodPos = pos;
//        GodAngle = angle;
//        GodDir = dir;
//        GodTran = tran;
//        GodDistance = distance;
//        GodFov = fov;
//        isGodEye_UIMode = isMiniMap;
//        enableMode(CameraControlMode.GodEyeControl,true);
//        //关闭景深
//        ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();
//        //限制鼠标在窗口范围内
//        //Initialize.bIsLockInWindow = true;
//        // 通知小地图enter上帝视角模式
//        if (SwitchGodEyeModeEvent != null)
//        {
//            SwitchGodEyeModeEvent(true, new Vector3());
//        }
//    }

//    //离开上帝之眼模式
//    public void LeaveGodEyeMode()
//    {
//        if (cameraMode != CameraControlMode.GodEyeControl)
//            return;
//        GodEyeFocusPosCheckTime = GodEyeFocusPosMaxCheckTime + 1;
//        //解除限制
//        //Initialize.bIsLockInWindow = false;
//        enableMode(CameraControlMode.GodEyeControl, false);
//        //开启景深
//        ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.DepthOfFieldScatter>();
//        // 通知小地图离开上帝视角模式
//        if (SwitchGodEyeModeEvent != null)
//        {
//            SwitchGodEyeModeEvent(false, new Vector3());
//        }
//        isGodEye_VMode = false;
//    }

//    // 小地图上帝之眼（矩形框）
//    public void UpdateMinimapGodEye()
//    {
//        if (SwitchGodEyeModeEvent != null)
//        {
//            SwitchGodEyeModeEvent(true, GodPos);
//        }
//    }

//    //更新上帝之眼参数
//    public void ChangeGodEyePos(Vector3 pos)
//    {
//        GodPos = pos;
//    }
//    public void ChangeGodEyeAngle(Vector3 angle)
//    {
//        GodAngle = angle;
//    }
//    public void ChangeGodEyeFov(float fov)
//    {
//        GodFov = fov;
//    }
//    public void ChangeGodEyeDistance(float distance)
//    {
//        GodDistance = distance;
//    }
//    public Vector3 GetGodEyePos()
//    {
//        return GodPos;
//    }
//    public Vector3 GetGodEyeAngle()
//    {
//        return GodAngle;
//    }
//    public float GetGodEyeFov()
//    {
//        return GodFov;
//    }
//    public float GetGodEyeDistance()
//    {
//        return GodDistance;
//    }

//    public delegate void SwitchGodEyeModeHanlder(bool bGodEyeMode, Vector3 vPos);
//    public static event SwitchGodEyeModeHanlder SwitchGodEyeModeEvent; 

//#endregion


//    #region FloatingMode
//    private Vector3 m_userDesireAngleRec;
//    private float m_userDesireDistanceRec;
//    private void EnterFloatingMode()
//    {
//        m_userDesireAngleRec=m_userDesireAngle;
//        m_userDesireDistanceRec = 0.0f;

//        Vector3 angle = m_userDesireAngle;
//        angle.x = curCameraModeParam.curPitchAngle;
//        angle.x = Mathf.Clamp(angle.x, curCameraModeParam.pitchAngleRange.x, curCameraModeParam.pitchAngleRange.y);

//        m_userDesireDistanceRec = m_userDesireDistance; //m_userDesireDistanceRec>0时，表示正在实行技能浮空的指定摄像机距离，m_userDesireDistance不可调
//        m_userDesireDistance = curCameraModeParam.curDistance;

//        enterNavMode(CameraNavMode.FloatingAngle, null, angle);
//    }
//    private void LeaveFloatingMode()
//    {
//        Vector3 angle = m_userDesireAngle;
//        angle.x = m_userDesireAngleRec.x;

//        if (m_userDesireDistanceRec > 0.0f)
//        {
//            m_userDesireDistance = m_userDesireDistanceRec;
//            m_userDesireDistanceRec = 0.0f; //m_userDesireDistanceRec>0时，表示正在实行技能浮空的指定摄像机距离，m_userDesireDistance不可调
//        }

//        enterNavMode(CameraNavMode.FloatingAngle, null, angle); //调整angle，利用enterNavMode将镜头导航逐渐恢复到m_userDesireAngleRec
//    }
//    #endregion

//    #region Nav+Collect



//    public struct int3
//    {
//        public int x;
//        public int y;
//        public int z;
//    }

//    public enum CameraNavMode //各种不同的转镜头，优先级由低到高，低的无法打断高的，进入高的自动覆盖低的
//    {
//        None=0,
//        Follow = 1,
//        Target = 2,
//        FloatingAngle=3,
//        Collect = 4,
//        Nav=5,
//    }

//    private int m_navModeFollowCount = 0; //Follow模式计数器，应对多个地方进入此状态冲突
//    private Transform m_navTragetTransform; //跟随对象转镜头的对象
//    private Vector3 m_navTargetAngle = Vector3.zero; //转向指定角度
//    public void enterNavMode(CameraNavMode enterNavMode, Transform refTransform, Vector3 refAngle)
//    {
//        if (enterNavMode == CameraNavMode.Follow)
//        {
//            m_navModeFollowCount++;
//        }
//        if (NavMode > enterNavMode) return; //正在进行优先级更高的操作，直接挡掉了        
//        if (enterNavMode == CameraNavMode.Target)
//        {
//            if (refTransform)
//            {
//                if (m_navTragetTransform == refTransform)
//                {
//                    //已经跟踪在旋转了，直接退了吧
//                    return;
//                }
//                Vector3 direction = refTransform.position - soldier.gameObject.transform.transform.position;
//                if (direction.magnitude > rotateStartMaxDistance || direction.magnitude < rotateStartMinDistance)
//                {
//                    return; //距离限制不开启
//                }

//                if (ScreenRaycast.isInScreen(refTransform.position))
//                {
//                    return; //在屏幕内的不开启
//                }


//                direction.y = 0;
//                direction.Normalize();
//                float angle = Mathf.Rad2Deg * (Mathf.Atan2(direction.x, direction.z));

//                float detaAngle = Mathf.DeltaAngle(m_userDesireAngle.y, angle);
//                if (Mathf.Abs(detaAngle) <= rotateStartAngle) //小于rotateStartAngle度不开启
//                {
//                    return;
//                }

//                if (enterNavMode != NavMode) //刚进入此模式，从初速度开始旋转
//                {
//                    m_approximageRotateSpeed = rotateStartSpeed;
//                }

//                m_navTragetTransform = refTransform;
//            }
//            else
//            {
//                if (NavMode == enterNavMode)
//                {
//                    exitNavMode(NavMode);
//                }
//                return;
//            }
//        }
//        else if (enterNavMode == CameraNavMode.FloatingAngle)
//        {
//            if (refAngle.sqrMagnitude > 0.01f)
//            {
//                m_navTargetAngle = refAngle;
//            }
//            else
//            {
//                exitNavMode(enterNavMode);
//            }
//        }
//        NavMode = enterNavMode;
//    }
//    public void exitNavMode(CameraNavMode navMode)
//    {        
//        if (navMode == CameraNavMode.Follow)
//        {
//            m_navModeFollowCount--;
//            if (m_navModeFollowCount <= 0)
//            {
//                m_navModeFollowCount = 0;
//            }
//        }

//        if (navMode == CameraNavMode.Target)
//        {
//            m_navTragetTransform = null;
//        }

//        if (navMode == CameraNavMode.FloatingAngle)
//        {
//            m_navTragetTransform = null;
//        }

//        if (navMode != NavMode)
//        {
//            return;
//        }
//        else
//        {
//            NavMode = CameraNavMode.None;
//        }
//    }

 
//    public CameraNavMode NavMode = CameraNavMode.None;
//    private float NavPauseTime = 0.0f;
//    public float NavPauseInterval = 2.0f;
//    public float NavTravelTime = 1.0f;
//    public bool NavLog = false;
//    public const float NavDefRange=10.0f;

//    private GameObject NavNodeGroup;

//    private List<CameraNavNode> matchnodes = new List<CameraNavNode>();
//    private List<CameraNavNode> blendnodes = new List<CameraNavNode>();
//    private bool bNavRun = false;
//    //导航协程(对协程不甚了解或者不了解yield return 系列函数用法慎改)
//    IEnumerator NavEnumerator()
//    {
//        bNavRun = true;

//        matchnodes.Clear();
//        Vector3 currentpos = m_userDesirePos;

//        if (NavNodeGroup != null)
//        {
//            for(int index=0;index < NavNodeGroup.transform.childCount;index++)
//            {
//                if (NavMode != CameraNavMode.Nav) //处于采集模式不导航，歇会退出协程
//                {
//                    yield return new WaitForSeconds(1.0f);
//                    break;
//                }

//                float time = Time.time; //用户操作期间暂停自动导航NavPauseInterval秒
//                if (time - NavPauseTime < NavPauseInterval) //用户操作中等待NavPauseInterval秒继续协程
//                {
//                    yield return new WaitForEndOfFrame();
//                    continue;
//                }

//                //下面逐个每间隔nexttime时间去导航info点遍历一次
//                Transform tchild = NavNodeGroup.transform.GetChild(index);
//                CameraNavNode node = tchild.GetComponent<CameraNavNode>();
//                if (node != null)
//                {
//                    Vector3 vec = node.pos - currentpos;
//                    float mag = vec.magnitude;
//                    if (mag < node.range * 0.5f)
//                    {
//                        matchnodes.Add(node);
//                    }
//                }

//                float nexttime = NavTravelTime;
//                if (NavNodeGroup.transform.childCount > 0)
//                {
//                    nexttime = NavTravelTime / NavNodeGroup.transform.childCount;
//                }
//                yield return new WaitForSeconds(nexttime);

//            }
//        }
//        else //sleep 1秒
//        {
//            yield return new WaitForSeconds(1.0f);
//        }

//        blendnodes.Clear();
//        for (int k = 0; k < matchnodes.Count; k++)
//        {
//            CameraNavNode node = matchnodes[k];
//            if (node == null)
//            {
//                continue;
//            }
//            blendnodes.Add(node);
//        }

//        if (blendnodes.Count > 0)
//        {
//            Vector3 blendpos = new Vector3(0, 0, 0);
//            Vector3 blendangle = new Vector3(0, 0, 0);
//            Vector3 blenddir = new Vector3(0, 0, 0);
//            Vector3 blendtran = new Vector3(0, 0, 0);
//            float blenddistance = 0.0f;
//            float blendfov = 0.0f;

//            //下面先计算总权重
//            float totalweight = 0.0f;
//            for (int k = 0; k < blendnodes.Count; k++)
//            {
//                CameraNavNode node = blendnodes[k];
//                Vector3 vec = node.pos - currentpos;
//                float mag=vec.magnitude;
//                float weight = 0.0f;
//                if (node.range > 0.01f && mag < node.range * 0.5f)
//                {
//                    weight = 1.0f - mag / node.range * 0.5f;
//                }
//                totalweight += weight;
//            }

//            //按距离加权混合
//            for (int k = 0; k < blendnodes.Count; k++)
//            {
//                CameraNavNode node = blendnodes[k];
//                Vector3 vec = node.pos - currentpos;
//                float mag = vec.magnitude;
//                float weight = 0.0f;
//                if (node.range > 0.01f && mag < node.range * 0.5f)
//                {
//                    weight = 1.0f - mag / node.range * 0.5f;
//                }
//                float factor = weight / totalweight;

//                blendpos += node.pos * factor;
//                blendangle += node.angle * factor;
//                blenddir += node.dir * factor;
//                blendtran += node.tran * factor;
//                blenddistance += node.distance * factor;
//                blendfov += node.fov * factor;
//            }

//            if (NavLog)
//            {
//                string msg = "nav:";
//                for (int k = 0; k < blendnodes.Count; k++)
//                {
//                    CameraNavNode node = blendnodes[k];
//                    if (node == null)
//                    {
//                        continue;
//                    }
//                    msg += blendnodes[k].name + ",";
//                }
//                Debug.Log(msg);
//            }
          
//            for (int k = 0; k < 60; k++) //通过60帧逼近目标值，因为navfactor因此逼近效果只依赖于帧数
//            {
//                m_userDesireAngle = approximateAngleVec(m_userDesireAngle, blendangle, navFactor);
//                m_userDesireDir = approximateVec(m_userDesireDir, blenddir, navFactor);
//                m_userDesireTran = approximateVec(m_userDesireTran, blendtran, navFactor);
//                m_userDesireDistance = approximateFloat(m_userDesireDistance, blenddistance, navFactor);
//                m_userDesireFov = approximateFloat(m_userDesireFov, blendfov, navFactor);

//                yield return new WaitForEndOfFrame();
//            }

    
//        }
//        else  //sleep 1秒
//        {
//            yield return new WaitForSeconds(1.0f);
//        }


//        bNavRun = false;
//    }

  

//    //自动导航镜头
//    private void NavNormally()
//    {
//        if (m_Camera != Initialize.mainCam)
//        {
//            return;
//        }
//        if (soldier != null && soldier.gameObject)
//        {
//            return;
//        }
//        if (NavMode == CameraNavMode.Follow)
//        {
//            bool reached = false;
//            m_userDesireAngle.y = approximateAngleFloat(m_userDesireAngle.y, soldier.gameObject.transform.eulerAngles.y, followNavFactor, ref reached);
//        }
  

//        if (NavMode == CameraNavMode.Nav)
//        {
//            if (!bNavRun)
//            {
//                StartCoroutine(NavEnumerator());
//            }
//        }
//        else
//        {
//            NavPauseTime = Time.time;
//        }

//        if (NavMode == CameraNavMode.Target)
//        {
//            if (soldier != null && m_navTragetTransform)
//            {
//                bool reached = false;
//                if (ScreenRaycast.isInScreen(m_navTragetTransform.position,0.1f))
//                {
//                    reached = true; //比较正中就停止了
//                }
//                else
//                {
//                    Vector3 direction = m_navTragetTransform.position - soldier.gameObject.transform.transform.position;
//                    direction.y = 0;
//                    direction.Normalize();
//                    float angle = Mathf.Rad2Deg * (Mathf.Atan2(direction.x, direction.z));
//                    m_userDesireAngle.y = approximateAngleFloat(m_userDesireAngle.y, angle, 0.0f, ref reached,10.0f);

//                }
//                if (reached)
//                {
//                    //转到想要的角度
//                    exitNavMode(NavMode);
//                }

//            }
//            else
//            {
//                //自己或者目标已被删除
//                exitNavMode(NavMode);
//            }
//        }
//        else if (NavMode == CameraNavMode.FloatingAngle)
//        {
//            bool reached = false;
//            m_userDesireAngle.x = approximateAngleFloat(m_userDesireAngle.x, m_navTargetAngle.x, followNavFactor*2.0f, ref reached);
//            if (reached)
//            {
//                exitNavMode(NavMode);
//            }
//        }

//        if (mouseCtrl && mouseCtrl.cameraRotateButton && canCameraRotate())
//        {            
//            NavPauseTime = Time.time;//如果用户操作了暂停自动导航一会         
//        }
//    }

//    private bool bCollectRun = false;
//    //导航协程(对协程不甚了解或者不了解yield return 系列函数用法慎改)
//    IEnumerator CollectEnumerator()
//    {
//        bCollectRun = true;

//        Vector3 currentpos = m_userDesirePos;
//        CameraNavNode retnode = null;
//        if (NavNodeGroup != null)
//        {
//            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
//            {
//                if (NavMode != CameraNavMode.Collect)  //处于采集模式不导航
//                {
//                    yield return new WaitForSeconds(1.0f);
//                    break;
//                }

//                //下面逐个每间隔nexttime时间去导航info点遍历一次
//                Transform tchild = NavNodeGroup.transform.GetChild(index);
//                CameraNavNode node = tchild.GetComponent<CameraNavNode>();
//                if (node != null)
//                {
//                    Vector3 vec = node.pos - currentpos;
//                    float mag = vec.magnitude;
//                    if (mag < node.range * 0.5f)
//                    {
//                        retnode = node;
//                        break;
//                    }
//                }

//                float nexttime = NavTravelTime;
//                if (NavNodeGroup.transform.childCount > 0)
//                {
//                    nexttime = NavTravelTime / NavNodeGroup.transform.childCount;
//                }
//                yield return new WaitForSeconds(nexttime);
//            }

//        }

//        if (NavMode == CameraNavMode.Collect)
//        {
//            if (retnode != null) //如果已经有数据，加权到旧数据
//            {
//                CameraNavNode info = retnode;
//                if (!info.locked)
//                {
//                    float oldcount = info.count;
//                    float newcount = oldcount + 1.0f;
//                    //info.pos = (info.pos * oldcount + m_userDesirePos) / newcount;
//                    info.angle = (info.angle * oldcount + m_userDesireAngle) / newcount;
//                    info.dir = (info.dir * oldcount + m_userDesireDir) / newcount;
//                    info.dir.Normalize();
//                    info.tran = (info.tran * oldcount + m_userDesireTran) / newcount;
//                    info.distance = (info.distance * oldcount + m_userDesireDistance) / newcount;
//                    info.fov = (info.fov * oldcount + m_userDesireFov) / newcount;
//                    info.count++;
//                    //info.range = info.range;
//                }

//                if (NavLog)
//                {
//                    Debug.Log("collect old node"+info.name);
//                }
//            }
//            else //否则添加新数据
//            {
//                GameObject go = BuildNavNode(currentpos, m_userDesireAngle, m_userDesireDir, m_userDesireTran, m_userDesireDistance, m_userDesireFov);
//                CameraNavNode info = go.AddComponent<CameraNavNode>();
//                info.pos = currentpos;
//                info.angle = m_userDesireAngle;
//                info.dir = m_userDesireDir;
//                info.tran = m_userDesireTran;
//                info.distance = m_userDesireDistance;
//                info.fov = m_userDesireFov;
//                info.count = 1;
//                info.locked = false;
//                info.range = NavDefRange;

//                if (NavLog)
//                {
//                    Debug.Log("collect new node" + info.name);
//                }
//            }
//        }

        
//        yield return new WaitForSeconds(1.0f);

//        bCollectRun = false;
//    }

//    //自动采集镜头
//    private void CollectNormally()
//    {
//        if (m_Camera != Initialize.mainCam)
//        {
//            return;
//        }

//        if (NavMode != CameraNavMode.Collect) //处于采集模式不导航
//        {
//            return;
//        }
//        else
//        {
//            if (!bCollectRun)
//            {
//                StartCoroutine(CollectEnumerator());
//            }
//        }
//    }

//    private GameObject BuildNavNode(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov)
//    {
//        if (NavNodeGroup == null)
//        {
//            NavNodeGroup = new GameObject("NavNodeGroup");
//        }


//        GameObject go = new GameObject();
//        go.transform.parent = NavNodeGroup.transform;
//        string name = "navnode_" + pos.ToString();
//        go.name = name;


//        GameObject dirgo = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
//        dirgo.name = "viewdir";
//        dirgo.transform.parent = go.transform;
//        dirgo.transform.localEulerAngles = new Vector3(90.0f, 30.0f, 0.0f);
//        dirgo.transform.localScale = new Vector3(0.1f, 1.0f, 0.1f);
//        dirgo.transform.localPosition = new Vector3(0.0f, 0, 0.0f);
//        Collider dirc = dirgo.GetComponent<Collider>();
//        if (dirc != null)
//        {
//            ResNode.DestroyRes(ref dirc);
//        }

//        GameObject eyego = GameObject.CreatePrimitive(PrimitiveType.Sphere);
//        eyego.name = "eye";
//        eyego.transform.parent = go.transform;
//        eyego.transform.localEulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
//        eyego.transform.localScale = new Vector3(1, 1, 1);
//        eyego.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
//        Collider eyec = eyego.GetComponent<Collider>();
//        if (eyec != null)
//        {
//            ResNode.DestroyRes(ref eyec);
//        }

//        GameObject lookatgo = GameObject.CreatePrimitive(PrimitiveType.Capsule);
//        lookatgo.name = "lookat";
//        lookatgo.transform.parent = go.transform;
//        //lookatgo.transform.localEulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
//        lookatgo.transform.localScale = new Vector3(1.0f, 2.0f, 1.0f);
//        //lookatgo.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
//        Collider lookatc = lookatgo.GetComponent<Collider>();
//        if (lookatc != null)
//        {
//            ResNode.DestroyRes(ref lookatc);
//        }

//        GameObject rangego = GameObject.CreatePrimitive(PrimitiveType.Quad);
//        rangego.name = "range";
//        rangego.transform.parent = go.transform;
//        rangego.transform.localEulerAngles = new Vector3(90.0f, 0.0f, 0.0f);
//        rangego.transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
//        rangego.transform.localPosition = new Vector3(0.0f, -1.0f, 0.0f);
//        Collider rangec = rangego.GetComponent<Collider>();
//        if (rangec != null)
//        {
//            ResNode.DestroyRes(ref rangec);
//        }
//        rangego.SetActive(false);

//        GameObject dirago = new GameObject();
//        dirago.transform.parent = eyego.transform;
//        dirago.transform.localEulerAngles = new Vector3(90.0f, 30.0f, 0.0f);
//        dirago.transform.localScale = new Vector3(0.5f, 1.0f, 0.5f);
//        dirago.transform.localPosition = new Vector3(0, 0, 0);

//        GameObject dirbgo = new GameObject();
//        dirbgo.transform.parent = eyego.transform;
//        dirbgo.transform.localEulerAngles = new Vector3(90.0f, -30.0f, 0.0f);
//        dirbgo.transform.localScale = new Vector3(0.5f, 1.0f, 0.5f);
//        dirbgo.transform.localPosition = new Vector3(0, 0, 0);
   

//        GameObject diraago = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
//        diraago.transform.parent = dirago.transform;
//        diraago.transform.localPosition = new Vector3(0.5f, 0, 0);
//        Collider dirac = diraago.GetComponent<Collider>();
//        if (dirac != null)
//        {
//            ResNode.DestroyRes(ref dirac);
//        }

//        GameObject dirbbgo = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
//        dirbbgo.transform.parent = dirbgo.transform;
//        dirbbgo.transform.localPosition = new Vector3(-0.5f, 0, 0);
//        Collider dirbc = dirbbgo.GetComponent<Collider>();
//        if (dirbc != null)
//        {
//            ResNode.DestroyRes(ref dirbc);
//        }

//        //Vector3 gridpos = new Vector3(grid.x * NodeSize, pos.y + 1.0f, grid.z * NodeSize);
//        //go.transform.SetPosition( gridpos;

//        go.transform.SetPosition( pos);

//        return go;
//    }

 

//    public void SaveNavInfosToFile(string path)
//    { 
//        StreamWriter sw = new StreamWriter(path,false);
//        sw.WriteLine ("#,posx,posy,posz,anglex,angley,anglez,dirx,diry,dirz,tranx,trany,tranz,distance,fov,count,range,locked");

//        if (NavNodeGroup != null)
//        {
//            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
//            {
//                Transform tchild = NavNodeGroup.transform.GetChild(index);
//                CameraNavNode info = tchild.GetComponent<CameraNavNode>();
//                if (info == null)
//                {
//                    continue;
//                }
//                string sline = "node,";
//                sline += info.pos.x + ",";
//                sline += info.pos.y + ",";
//                sline += info.pos.z + ",";
//                sline += info.angle.x + ",";
//                sline += info.angle.y + ",";
//                sline += info.angle.z + ",";
//                sline += info.dir.x + ",";
//                sline += info.dir.y + ",";
//                sline += info.dir.z + ",";
//                sline += info.tran.x + ",";
//                sline += info.tran.y + ",";
//                sline += info.tran.z + ",";
//                sline += info.distance + ",";
//                sline += info.fov + ",";
//                sline += info.count + ",";
//                sline += info.range + ",";
//                int locked = info.locked ? 1 : 0;
//                sline += locked + ",";

//                sw.WriteLine(sline);
//            }
//        }

//        sw.Close();
//    }

//    public void LoadNavInfoFromFile(string path)
//    {
//        string text = File.ReadAllText(path);
//        LoadNavInfoFromText(text, "SoldierCamera");
//    }

//    public void LoadNavInfoFromText(string text,string tag)
//    {
//        if (NavNodeGroup != null)
//        {
//            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
//            {
//                Transform tchild = NavNodeGroup.transform.GetChild(index);
//                var temp = tchild.gameObject;
//                ResNode.DestroyRes(ref temp);
//            }
//        }


//        ScpReader reader = new ScpReader(text,tag);

//        for (int i = 0; i < reader.GetRecordCount(); i++)
//        {
//            string cmd = reader.GetString(i, 0, "");
//            if (cmd != "node")
//            {
//                continue;
//            }

//            Vector3 pos;
//            pos.x = reader.GetFloat(i, 1, 0.0f);
//            pos.y = reader.GetFloat(i, 2, 0.0f);
//            pos.z = reader.GetFloat(i, 3, 0.0f);
//            Vector3 angle;
//            angle.x = reader.GetFloat(i, 4, 0.0f);
//            angle.y = reader.GetFloat(i, 5, 0.0f);
//            angle.z = reader.GetFloat(i, 6, 0.0f);
//            Vector3 dir;
//            dir.x = reader.GetFloat(i, 7, 0.0f);
//            dir.y = reader.GetFloat(i, 8, 0.0f);
//            dir.z = reader.GetFloat(i, 9, 0.0f);
//            Vector3 tran;
//            tran.x = reader.GetFloat(i, 10, 0.0f);
//            tran.y = reader.GetFloat(i, 11, 0.0f);
//            tran.z = reader.GetFloat(i, 12, 0.0f);
//            float distance;
//            distance = reader.GetFloat(i, 13, 0.0f);
//            float fov;
//            fov = reader.GetFloat(i, 14, 0.0f);
//            int count;
//            count = reader.GetInt(i, 15, 0);
//            float range;
//            range = reader.GetFloat(i, 16, NavDefRange);
//            range = Mathf.Clamp(range, 0.0f, 100.0f);
//            bool locked;
//            locked = reader.GetInt(i, 17, 0)!=0;

//            GameObject go = BuildNavNode(pos, angle,dir,tran,distance,fov);
//            CameraNavNode info = go.AddComponent<CameraNavNode>();
//            info.pos = pos;
//            info.angle = angle;
//            info.dir = dir;
//            info.tran = tran;
//            info.distance = distance;
//            info.fov = fov;
//            info.count = 1;
//            info.range = range;
//            info.locked = locked;

//        }

//        reader.Dispose();

 
//    }

   
//    #endregion

//    public Texture cameraView;
//    public bool drawCameraView = false;

//    private void CalGodEyeCameraFocusPos()
//    {
//        GodEyeFocusPosCheckTime += deltaTime;
//        if (GodEyeFocusPosCheckTime > 5.0f)
//        {
//            SendCameraFocusPosToLogic(1, cPos.x, cPos.y, cPos.z);
//            GodEyeFocusPosCheckTime = 0;
//        }
//    }
//    private void SendCameraFocusPosToLogic(int _bIsGodMode,float _fPosX = 0,float _fPosY = 0,float _fPosZ = 0)
//    {
//        if (EntityFactory.MainHeroID <= 0)
//        {
//            return;
//        }

//        cmd_entity_master_eye_focus data;
//        data.isGodMode = _bIsGodMode;
//        data.x = _fPosX;
//        data.y = _fPosY;
//        data.z = _fPosZ;

//        IntPtrHelper helper = new IntPtrHelper();
//        System.IntPtr ptr = helper.toPtr<cmd_entity_master_eye_focus>(ref data);
//        int len = System.Runtime.InteropServices.Marshal.SizeOf(data);

//        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID,(int)EntityLogicDef.ENTITY_CMD_MASTER_EYE_FOCUS, len, "", ptr, len);

//    }


//    #region 镜头转移到自己/队友身上

//    public delegate void SwitchLookAtSoliderHanlder(U3D_Render.EntityView soliderTransform);
//    public static event SwitchLookAtSoliderHanlder SwitchLookAtSoliderEvent;

//    private const float NormalFocusPosMaxCheckTime = 5.0f;
//    private float NormalFocusPosCheckTime = NormalFocusPosMaxCheckTime + 1;
//    private bool isGodEye_DeadMode = false;       //上帝视角：是否正在注视主角（死亡了会切换）

//    public void switchLookAtSolider(U3D_Render.EntityView soliderTransform)
//    {
//        if (soliderTransform != null && EntityFactory.MainHeroView.StateMachine.GetState() == EntityState.Dead)    //主角死的时候才会切换
//        {
//            SoldierCamera.MainInstance.soldier = soliderTransform;
//            if (SwitchLookAtSoliderEvent != null)
//                SwitchLookAtSoliderEvent(soliderTransform);

//            if (EntityFactory.MainHero != null && soliderTransform == EntityFactory.MainHeroView)
//            {
//                if (isGodEye_DeadMode)
//                {
//                    LeaveGodEyeMode();
//                    isGodEye_DeadMode = false;
//                }
//                SendCameraFocusPosToLogic(0);

//            }
//            else
//            {
//                if (!isGodEye_DeadMode || isGodEye_UIMode)
//                {
//                    SwitchGodEye(soliderTransform);
//                    isGodEye_DeadMode = true;
//                }
//            }
//            NormalFocusPosCheckTime = NormalFocusPosMaxCheckTime + 1;
//        }
//    }

//    private void CalNormalCameraFocusPos()
//    {
//        NormalFocusPosCheckTime += deltaTime;
//        if (NormalFocusPosCheckTime > 5.0f)
//        {
//            if(isGodEye_DeadMode)
//                SendCameraFocusPosToLogic(1, cPos.x, cPos.y, cPos.z);
//            NormalFocusPosCheckTime = 0;
//        }
//    }
//    #endregion

//    #region CameraAmination

//    public bool bPlayingCameraAnim = false;
//    private Vector3 m_cameraAminOrgPosition = Vector3.zero;
//    private Vector3 m_cameraAminOrgAngle = Vector3.zero;
//    private Vector3 m_cameraAminRecPosition = Vector3.zero;
//    private bool m_cameraAminFollowHero = false;
//    private bool m_cameraAnimProjectFloor = false;
//    private bool m_cameraAnimMainCamera = false;
//    private uint m_cameraAnimStopTick = 0;
    
//    private Transform m_camRecTrans = null;
//    private Transform m_camAnimTrans = null;
//    private Transform m_camFOVTrans = null;

//    public void PlayCameraAnim(AnimationClip clip, CameraAnimOrgType animOrgType, bool projectFloor, Vector3 targetPos, Transform sourceTransform, bool animationMainCamera)
//    {
//        if (bPlayingCameraAnim)
//        {
//            StopCameraAnim();
//        }

//        GameObject camGo = new GameObject("CameraAnimRef");
//        m_camRecTrans = camGo.transform;        
//        GameObject animGo = new GameObject("Anim");
//        animGo.transform.parent = m_camRecTrans;
//        m_camAnimTrans = animGo.transform;
//        GameObject camFOVGo = new GameObject("camFOV");
//        m_camFOVTrans = camFOVGo.transform;
//        m_camFOVTrans.parent = animGo.transform;

//        m_camRecTrans.position = Vector3.zero;
//        m_cameraAminFollowHero = false;
//        m_cameraAnimProjectFloor = projectFloor;
//        m_cameraAnimMainCamera = animationMainCamera;

//        if (animOrgType == CameraAnimOrgType.Hero || animOrgType == CameraAnimOrgType.FollowHero)
//        {
//            if (soldier != null && soldier.gameObject)
//            {

//                m_camRecTrans.position = soldier.gameObject.transform.position;
//                m_camRecTrans.eulerAngles = soldier.gameObject.transform.eulerAngles;

//                if (animOrgType == CameraAnimOrgType.FollowHero)
//                {
//                    m_cameraAminFollowHero = true;
//                }
//            }
//        }
//        else if (animOrgType == CameraAnimOrgType.TargetPos)
//        {
//            m_camRecTrans.position = targetPos;
//            if (soldier != null)
//            {
//                if (soldier.gameObject)
//                {
//                    m_camRecTrans.eulerAngles = soldier.gameObject.transform.eulerAngles;
//                }
                
//            }
//        }
//        else if (animOrgType == CameraAnimOrgType.SourceObject)
//        {
//            m_camRecTrans.position = sourceTransform.position;
//            m_camRecTrans.eulerAngles = sourceTransform.eulerAngles;
//        }

//        if (m_cameraAnimProjectFloor)
//        {
//            m_camRecTrans.position = ProjectToFloor(m_camRecTrans.position);
//        }

//        m_cameraAminOrgPosition = m_camRecTrans.position;

//        if (clip.legacy == false)
//        {
//            Debug.Log("Camera Animation clip "+clip.name+" need to be legacy!");
//            clip.legacy = true;
//        }

//        Animation anim=animGo.AddComponent<Animation>();
//        anim.AddClip(clip, clip.name);
//        anim.Play(clip.name);

//        m_cameraAnimStopTick = ((uint)(clip.length * 1000.0f)) + GameLogicAPI.getTickCount();

//        bPlayingCameraAnim = true;

//        enableMode(CameraControlMode.CameraAnim, true);
//    }


//    public void StopCameraAnim()
//    {
//        if (!bPlayingCameraAnim)
//        {
//            return;
//        }
//        if (m_camRecTrans)
//        {
//            var temp = m_camRecTrans.gameObject;
//            ResNode.DestroyRes(ref temp);
//        }
//        bPlayingCameraAnim = false;
//        enableMode(CameraControlMode.CameraAnim, false);
//    }

//    private void updateCameraAnim()
//    {
//        Transform camTrans = camTransform;
//        if (m_cameraAnimMainCamera)
//        {
//            camTrans = Camera.main.transform;
//        }
//        camTrans.position = m_camAnimTrans.position;
//        Vector3 angle = m_camAnimTrans.eulerAngles;
//        camTrans.eulerAngles = angle;
//        float focal=m_camFOVTrans.localScale.z;
//        float fov=Mathf.Atan (30.0f/focal)/Mathf.PI*180.0f;
//        m_Camera.fieldOfView = fov;

//        if (m_cameraAminFollowHero && soldier.gameObject)
//        {
//            m_camRecTrans.position = soldier.gameObject.transform.position;
//            if (m_cameraAnimProjectFloor)
//            {
//                m_camRecTrans.position = ProjectToFloor(m_camRecTrans.position);
//            }
//        }

//        uint tick = GameLogicAPI.getTickCount();
//        if (tick > m_cameraAnimStopTick)
//        {
//            StopCameraAnim();
//        }
//    }


//    private Vector3 ProjectToFloor(Vector3 posBeforeProject)
//    {
//        Vector3 posAfterProject = posBeforeProject;
//        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
//        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
//        LayerMaskOnGround = LayerMaskOnGround | (1 << LayerMask.NameToLayer(Config.LayerBuilding));

//        Ray upRay = new Ray(posAfterProject + new Vector3(0, 0.5f, 0), new Vector3(0, 1, 0));
//        RaycastHit hitInfo;
//        //先向上取10米，并且确保不会撞到顶，如果是在石头内部的话，需要确保石头法向向外，这样就不会判定为hit到。
//        bool hitUp = Physics.Raycast(upRay, out hitInfo, 10.0f, LayerMaskOnGround);
//        if (hitUp)
//        {
//            posAfterProject = hitInfo.point;
//        }
//        else
//        {
//            posAfterProject = posAfterProject + new Vector3(0, 10.0f, 0);
//        }
//        Ray downRay = new Ray(posAfterProject - new Vector3(0, 0.1f, 0), new Vector3(0, -1, 0));
//        bool hitGround = Physics.Raycast(downRay, out hitInfo, 100.0f, LayerMaskOnGround);
//        if (hitGround)
//        {
//            posAfterProject = hitInfo.point;
//        }
//        else
//        {
//            posAfterProject = posBeforeProject;
//        }

//        return posAfterProject;
//    }
//    #endregion
//}