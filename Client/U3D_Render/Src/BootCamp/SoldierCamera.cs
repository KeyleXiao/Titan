/*******************************************************************
** 文件名:	SoldierCamera.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/18
** 版  本:	1.0
** 描  述:	相机控制，这个脚本只能控制主相机，而且与场景无关，只要场景中有一个主相机，这个脚本就能找到主相机，并进行控制
** 应  用:   只能控制住相机，即 Camera.main相机
 *          
**************************** 修改记录 ******************************
** 修改人:    
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using GameLogic;
using U3D_Render.CameraControl;
using Effect;
using UnityEngine.Events;
using ASpeedGame.Data.Scheme;


public class SoldierCameraModeChangeEvent : UnityEvent<CameraMode>
{
}
public partial class SoldierCamera : BaseSoldierCamera
{

    //////////////////////////////事件队列相关//////////////////////////////
    public delegate void SoliderCameraStateChangeHandler(bool enabled, CameraStateParam obj);
    public SoliderCameraStateChangeHandler[] stateChangeEventsList;
    public List<CamStateChangeMsg> stateChangeMsgsQueue = new List<CamStateChangeMsg>();

    //////////////////////////////相关组件//////////////////////////////

    [System.NonSerialized]
    public U3D_Render.EntityView soldier;       //当前摄像机观察的对象

    public override float GetSoliderHeightInMeters()
    {
        if(!U3D_Render.EntityView.isNullOrEmpty(soldier))
        {
            return soldier.Property.CreatureHeightInMeters;
        }
        return 0;
    }

    public override GameObject GetSoliderGo()
    {
        if (!U3D_Render.EntityView.isNullOrEmpty(soldier))
        {
            return soldier.gameObject;
        }
        return null;
    }

    
    public SoldierController soldierController = null;
    public MouseCtrl mouseCtrl = null;



    //////////////////////////相关参数/////////////////////////////////
    public CameraParam[] cameraControlModeParam;
    public CameraParam curCameraModeParam;

    public float maxDetlaTime = 0.1f; //镜头每次移动的最大deltaTime时间，避免镜头剧烈移动
    public Vector3 smoothFactor = new Vector3(0.2f, 1.0f, 1.0f);
    //private Vector3 vec3SendLogicFocusPos;//通知逻辑层相机焦点坐标
    public int SendLogicFocusPosRange = 10;//每次相机焦点坐标更新在这个范围内不发，超出才发
    public Vector2 FloatingPitchAngleClamp;//浮空时镜头限制
    public Texture cameraView;

    public bool drawCameraView = false;
    public bool isDebug = false;

    private float rayTraceAngle = 60.0f; //60度外不模糊选取
    public float GetRayTraceAngle()
    {
        if (cameraMode == CameraMode.FPSMode)
            return 30.0f;
        else
            return rayTraceAngle;
    }

    //是否处于大视角观察模式
    public bool InGodViewMode
    {
        get { return cameraMode == CameraMode.MapGodControl || cameraMode == CameraMode.DeadControl || cameraMode == CameraMode.TopControl || cameraMode == CameraMode.OBMoveFree; }
    }

    #region 镜头模式切换
    private void SwitchCamMode(CameraMode mode, CameraContext context = null)
    {
        if (cameraControlModeParam == null || curCameraModeParam == null)
            return;
        CameraMode oldMode = cameraMode;
        cameraMode = mode;  //保证外部拿到的cameraMode是当前的?

        if (camCtrl != null)
        {
            if (camCtrl.GetCameraMode() == mode)    //同一个状态不能重复进入
                return;
            curCameraModeParam.FOV = m_userDesireFov;
            curCameraModeParam.curPitchAngle = m_userDesireAngle.x;
            curCameraModeParam.curDistance = m_userDesireDistance;
            cameraControlModeParam[(int)oldMode] = curCameraModeParam;
            camCtrl.Leave();
        }
        camCtrl = CameraUtil.GetCamCtrl(mode);
        camCtrl.curCameraMode = mode;
        curCameraModeParam = cameraControlModeParam[(int)mode];
        camCtrl.Enter(context);
    }

    //内部状态切换
    private void RefreshCameraMode(CameraContext context = null)
    {
        if (!Reset(false))
            return;
        for (int i = 0; i < camCtrl.AllowChangeList.Count; i++)
        {
            BaseCameraController canCtrl = CameraUtil.GetCamCtrl(camCtrl.AllowChangeList[i]);
            if (canCtrl.Enable(true) && canCtrl.CheckContext(context))
            {
                SwitchCamMode(camCtrl.AllowChangeList[i], context);
                break;
            }
        }

    }

    //外部状态强制切换
    private void HandleModeForceSwitch(CameraMode mode, CameraContext context = null)
    {
        if (!Reset(false))
            return;
        BaseCameraController camCtrl = CameraUtil.GetCamCtrl(mode);
        if (camCtrl.Enable(false) && camCtrl.CheckContext(context))
        {
            SwitchCamMode(mode, context);
        }
    }

#if ENABLE_YUNYING_MODE
    //OB模式专用（编辑器专用：状态强制切换）
    private void YUNYING_HandleModeForceSwitchOBMode(CameraMode mode)
    {
        if (!Reset(false))
            return;
        SwitchCamMode(mode, null);
    }
#endif
    #endregion

    #region monoBehavior

    public void Awake()
    {
        m_Camera = Initialize.mainCam;
        _instance = this;

        SceneManager.RegisterSceneLoadFinishEvent(SceneLoadFinish);

        m_animCamCtrl = new AnimCameraModeController();
        m_deadCamCtrl = new DeadCameraModeController();
        m_endWarCamCtrl = new EndWarCameraModeController();
        m_floatingCamCtrl = new FloatingCameraModeController();
        m_followCamCtrl = new FollowCameraModeController();
        m_fpsCamCtrl = new FPSCameraModeController();
        m_mapGodCamCtrl = new MapGodCameraModeController();
        m_ridingCamCtrl = new RidingCameraModeController();
        m_sniperscopeCamCtrl = new SniperscopeCameraModeController();
        m_topCamCtrl = new TopCameraModeController();
        m_userCamCtrl = new UserCameraModeController();
        m_ob_hero_lockCamCtrl = new OBHeroLockModeController();
        m_ob_hero_moveFreeCamCtrl = new OBMoveFreeModeController();


#if ENABLE_YUNYING_MODE
        YUNYING_m_ob_hero_lockCamCtrl = new YUNYING_OBHeroLockModeController();
        YUNYING_m_ob_hero_movefreeCamCtrl = new YUNYING_OBHeroMoveFreeModeController();
        YUNYING_m_ob_hero_traceCamCtrl = new YUNYING_OBHeroTraceModeController();
#endif

        camCtrl = m_userCamCtrl;
        cameraMode = CameraMode.UserControl;
        stateChangeEventsList = new SoliderCameraStateChangeHandler[(int)CameraMode.MaxCount];
        stateChangeEventsList[(int)CameraMode.CameraAnim] = this.CameraAminationHandler;
        stateChangeEventsList[(int)CameraMode.DeadControl] = this.CameraDeadHandler;
        stateChangeEventsList[(int)CameraMode.EndWarControl] = this.CameraWarEndHandler;
        stateChangeEventsList[(int)CameraMode.FloatingControl] = this.CameraFloatingHandler;
        stateChangeEventsList[(int)CameraMode.FollowTarget] = null;
        stateChangeEventsList[(int)CameraMode.FPSMode] = this.CameraFpsHandler;
        stateChangeEventsList[(int)CameraMode.MapGodControl] = this.CameraMapGodEyeHandler;
        stateChangeEventsList[(int)CameraMode.RidingControl] = this.CameraRidingHandler;
        stateChangeEventsList[(int)CameraMode.SniperscopeControl] = this.CameraSniperscopeHandler;
        stateChangeEventsList[(int)CameraMode.TopControl] = null;
        stateChangeEventsList[(int)CameraMode.UserControl] = null;
        stateChangeEventsList[(int)CameraMode.OBHeroLock] = this.CameraOBHeroLockHandler;
        stateChangeEventsList[(int)CameraMode.OBMoveFree] = this.CameraOBMoveFreeHandler;


        #if ENABLE_YUNYING_MODE

        stateChangeEventsList[(int)CameraMode.YUNYING_OBHeroLock] = null;
        stateChangeEventsList[(int)CameraMode.YUNYING_OBHeroTrace] = null;
        stateChangeEventsList[(int)CameraMode.YUNYING_OBMoveFree] = null;

        #endif
    }
    private LayerMask hitLayer = -1;
    public void Start()
    {
        Reset(true);
        //当前相机不与Player和ScreenRaycast层碰撞
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        hitLayer = hitLayer.value & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
    }
    public void OnDestroy()
    {
        ClearCameraModeChangeEvent();
        SceneManager.UnRegisterSceneLoadFinishEvent(SceneLoadFinish);
    }

    void SceneLoadFinish(string sceneName)
    {

        m_Camera = Initialize.mainCam;
        if (!m_Camera)
        {
            return;
        }
        camTransform = m_Camera.transform;
    }

    void InitOBScheme()
    {
        SchemeOBControl.SchemeOBCtrlParamNode mouseNode = SchemeOBControl.Instance.GetOBCtrlParamNode(SchemeOBControl.EOBCtrlParamType.EOBCtrlParam_MouseScroll_Speed);
        if (mouseNode != null)
        {
            obCtrlMouseScrollCurSpeed = obCtrlMouseScrollSpeedDefault = mouseNode.fDefaultValue;
            obCtrlMouseScrollSpeedRange = new Vector2(mouseNode.fMinValue, mouseNode.fMaxValue);
        }

        SchemeOBControl.SchemeOBCtrlParamNode moveNode = SchemeOBControl.Instance.GetOBCtrlParamNode(SchemeOBControl.EOBCtrlParamType.EOBCtrlParam_CameraMove_Speed);
        if (moveNode != null)
        {
            obCtrlCamMoveCurSpeed = obCtrlCamMoveSpeedDefault = moveNode.fDefaultValue;
            obCtrlCamMoveSpeedRange = new Vector2(moveNode.fMinValue, moveNode.fMaxValue);
        }
    }
    #endregion

    #region 角色状态控制变量
    //主角是否处于死亡状态
    public bool isDead = false;

    //主角是否处于浮空状态
    public bool isFloating
    {
        get
        {
            if (EntityFactory.MainHero == null)
                return false;

            CreatureStateMachine stateMachine = EntityFactory.MainHero.GetComponent<CreatureStateMachine>();
            if (stateMachine == null)
                return false;

            return stateMachine.GetState() == EntityState.Floating;
        }
    }

    //主角是否FPS英雄
    public bool isFps
    {
        get
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return isSkillAutoMove;

            return EntityFactory.MainHeroView.Property.FPSMode || isSkillAutoMove;
        }
    }

    //是否自动前进，自动前进也要开启fps模式...
    private bool isSkillAutoMove
    {
        get
        {
            if (soldierController != null)
            {
                return soldierController.skillautoForward;
            }
            return false;
        }
    }

    //当前是否为裁判模式
    public bool isOBState
    {
        get
        {
            return StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB;
        }
    }

    public bool isOBCameraMode
    {
        get
        {
            return camCtrl.GetCameraMode() == CameraMode.OBHeroLock || camCtrl.GetCameraMode() == CameraMode.OBMoveFree;
        }
    }


    //生存模式
    public bool isSurvivalMode
    {
        get { return GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP; }
    }
    #endregion

    #region 镜头每帧更新控制
    public void Update()
    {
        if (camCtrl == null)
            return;
        camCtrl.OnUpdate();
    }

    public void LateUpdate()
    {
        if (soldier == null || m_Camera == null || cameraMode >= CameraMode.MaxCount)
        {
            return;
        }
        //处理事件队列
        for(int i = 0; i < stateChangeMsgsQueue.Count; i++)
        {
            int mode = (int)stateChangeMsgsQueue[i].mode;
            if(stateChangeEventsList[mode]!=null)
                stateChangeEventsList[mode](stateChangeMsgsQueue[i].enabled, stateChangeMsgsQueue[i].param);
        }
        stateChangeMsgsQueue.Clear();

        //切换上帝模式（快捷键目前没有回调方法）陈伍宏说先关闭(9/20)
        //if (FuntionShortCutCtrl.CheckFunctionShortcutKeyUp(FuntionShortCutSlotIndex.LockView))
        //{
        //    if (camCtrl.GetCameraMode() != CameraMode.TopControl)
        //    {
        //        HandleModeForceSwitch(CameraMode.TopControl);
        //        //**//enableMode(CameraMode.TopControl, true);
        //    }
        //    else
        //        RefreshCameraMode();
        //    //**//enableMode(CameraMode.TopControl, false);
        //}

#if ENABLE_YUNYING_MODE
       if (Input.GetKeyDown(KeyCode.Alpha1)){
           YUNYING_HandleModeForceSwitchOBMode(CameraMode.YUNYING_OBHeroLock);
        }
        else if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            YUNYING_HandleModeForceSwitchOBMode(CameraMode.YUNYING_OBHeroTrace);
        }
        else if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            YUNYING_HandleModeForceSwitchOBMode(CameraMode.YUNYING_OBMoveFree);
        }
#endif
        camCtrl.SetLateDealtaTime();
        camCtrl.OnLateUpdate();
    }


    //强制镜头瞬间跟上英雄，而不是利用插值平滑移动镜头
    public void ForceUpdateTransformNormally()
    {
    }

    #endregion

    #region SoliderCamera外部接口
    //读取摄像机参数配置
    private int currentSceneType = 0;
    private int currentHeroType = 0;
    public void updateCameraParam(int sceneType, int heroType)
    {
        if (sceneType >= 0)
        {
            currentSceneType = sceneType;
        }
        if (heroType >= 0)
        {
            currentHeroType = heroType;
        }
        CameraParam[] cameraInfo = ASpeedGame.Data.CameraConfig.CameraConfig.Instance.GetCameraConfigInfo(sceneType, heroType);
        if (cameraInfo != null)
        {
            if (cameraControlModeParam == null)
            {
                cameraControlModeParam = new CameraParam[(int)CameraMode.MaxCount];

                for (int i = 0; i < (int)CameraMode.MaxCount; i++)
                {
                    cameraControlModeParam[i] = cameraInfo[0].GetCopy();
                    cameraControlModeParam[i].modeID = i;
                }
            }
            cameraControlModeParam[(int)CameraMode.UserControl] = cameraInfo[0].GetCopy();
            cameraControlModeParam[(int)CameraMode.FloatingControl] = cameraInfo[1].GetCopy();
            cameraControlModeParam[(int)CameraMode.FPSMode] = cameraInfo[2].GetCopy();
            cameraControlModeParam[(int)CameraMode.SniperscopeControl] = cameraInfo[3].GetCopy();
        }

        if (cameraMode < CameraMode.MaxCount)
        {
            curCameraModeParam = cameraControlModeParam[(int)cameraMode];
            m_userDesireFov = curCameraModeParam.FOV;
            SetUserDesireAngleX(curCameraModeParam.curPitchAngle);
            m_userDesireDistance = curCameraModeParam.curDistance;

            if (CurCameraProperty != null)
            {
                CurCameraProperty.NormalFOV = (int)curCameraModeParam.FOV;
                CurCameraProperty.NormalDistance = curCameraModeParam.curDistance;
                CurCameraProperty.NormalHeight = curCameraModeParam.targetOffset;
                CurCameraProperty.InitedEulerAngle.x = curCameraModeParam.curPitchAngle;
            }
        }
        else
        {
            SwitchCamModeDefault();
        }
    }
    //外部接口，切换一个默认状态
    public void SwitchCamModeDefault()
    {
        if(!GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID) && isOBState)
        {
            if (!isOBCameraMode)
            {
                EnableOBMode(true);
                return;
            }
        }
        SwitchCamMode(CameraMode.UserControl);
    }
    //外部接口，控制状态开关
    public void EnableMode(CameraMode mode, bool enabled, CameraStateParam param = null)
    {
        if (param != null)
        {
            param.Set(mode, enabled);
        }
        CamStateChangeMsg msg = new CamStateChangeMsg(mode, enabled, param);
        stateChangeMsgsQueue.Add(msg);
    }
    //外部接口，离开当前状态
    public void LeaveCurMode()
    {
        if (cameraMode == this.camCtrl.GetCameraMode())
        {
            RefreshCameraMode();
        }
    }

    private static bool isInit = false;
    public bool Reset(bool isForce, bool smoothblend = true)
    {
        if (isForce)
        {
            isInit = false;
        }
        if (isInit)
            return true;
        if (target == null)
            return false;

        if (CurCameraProperty == null || CurCameraProperty.configType == 0)
        {
            CurCameraProperty = gameObject.GetComponent<CameraProperty>();
            if (CurCameraProperty == null)// || soldierController == null || mouseCtrl==null)
            {
                Trace.LogError("摄像机获取CameraProperty组件");
                return false;
            }
            CurCameraProperty.SpeedScale = ASpeedGame.Data.GameSettingConfig.GameSettingConfig.Instance.GetGameSettingFloatValue(ASpeedGame.Data.GameSettingConfig.GameSettingIndex.CameraSpeed);
        }

        if (soldier == null || !soldier.gameObject || EntityFactory.MainHero == null)
            return false;
        target.parent = null;

        soldierController = EntityFactory.MainHero.GetComponent<SoldierController>();
        mouseCtrl = EntityFactory.MainHero.GetComponent<MouseCtrl>();
        if (!m_Camera)
        {
            m_Camera = Initialize.mainCam;
        }
        camTransform = m_Camera.transform;

        if (camCtrl != null)
        {
            camCtrl.Reset(CurCameraProperty, smoothblend);
            if (!smoothblend)
            {
                apprTran = m_userCurTran;
                apprFov = m_userCurFov;
                apprDistance = m_userCurDistance;
            }
        }

        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
        {
            isInit = true;
            return true;
        }
        return false;
    }

    public void UpdateGodEyePos(GodEyeStateParam param)
    {
        if (isOBState)
        {
            UpdateOBFocusPos(param);
            return;
        }

        if(!isSurvivalMode)
            UpdateGodEye(param);
    }

    public void EnterOB_yunying(CameraMode camMode)
    {
#if ENABLE_YUNYING_MODE
        YUNYING_HandleModeForceSwitchOBMode(camMode);
#endif
    }


    #region 镜头光效效果  ##通过参数调节的摄像机光效，用户可控镜头

    public float minShakeSpeed;
    public float maxShakeSpeed;
    public float minShake;
    public float maxShake = 2.0f;

    public int minShakeTimes;
    public int maxShakeTimes;

    public float maxShakeDistance;

    public void playEffect(int delayTicks, int nliveTicks, Effect_CameraNode[] cameraNodes, bool recoverTransform)
    {
        CameraEffectUtil.MainInstance.playEffect(delayTicks, nliveTicks, cameraNodes, recoverTransform);
    }

    private void stopEffect()
    {
        CameraEffectUtil.MainInstance.stopEffect();
    }

    //在finishTick的时间内恢复摄像机
    public void finishEffect(uint finishTick)
    {
        CameraEffectUtil.MainInstance.finishEffect(finishTick);
    }

    #endregion

    #endregion

    ///////////////////////////摄像机模式切换接口//////////////////////////////

    #region 狙击镜Mode接口

    private void CameraSniperscopeHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            HandleModeForceSwitch(CameraMode.SniperscopeControl);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.SniperscopeControl)
                RefreshCameraMode();
        }
    }
    #endregion

    #region RidingMode接口

    private void CameraRidingHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            RidingStateParam param = obj as RidingStateParam;
            EnableRidingMode(param.distance, param.angleRange);
        }
        else
        {
            DisableRidingMode();
        }
    }
    private void EnableRidingMode(float distance, Vector2 angleRange)
    {
        setRidingCameraParam(distance, angleRange);
        HandleModeForceSwitch(CameraMode.RidingControl);
    }

    private void DisableRidingMode()
    {
        if (camCtrl.GetCameraMode() == CameraMode.RidingControl)
            RefreshCameraMode();
    }

    private void setRidingCameraParam(float distance, Vector2 angleRange)
    {
        if (distance <= 0.001f)
        {
            distance = m_userDesireDistance;
        }
        cameraControlModeParam[(int)CameraMode.RidingControl].curDistance = distance;
        cameraControlModeParam[(int)CameraMode.RidingControl].distanceRange = new Vector2(distance, distance);
        cameraControlModeParam[(int)CameraMode.RidingControl].curPitchAngle = Mathf.Clamp(m_userDesireAngle.x, angleRange.x, angleRange.y);
        cameraControlModeParam[(int)CameraMode.RidingControl].pitchAngleRange = angleRange;
        cameraControlModeParam[(int)CameraMode.RidingControl].FOV = m_userDesireFov;
        cameraControlModeParam[(int)CameraMode.RidingControl].targetOffset = curCameraModeParam.targetOffset;
    }
    #endregion

    #region FPSMode接口

    private bool m_bFPSShowCursor = false;
    public bool bFPSShowCursor
    {
        get { return m_bFPSShowCursor; }
        set { m_bFPSShowCursor = value; }
    }
    public bool bRotateWithMouse
    {
        get
        {
            if (cameraMode == CameraMode.FPSMode)
            {
                return ((FPSCameraModeController)camCtrl).m_bRotateWithMouse;
            }
            return false;
        }
    }

    private void CameraFpsHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            HandleModeForceSwitch(CameraMode.FPSMode);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.FPSMode)
                RefreshCameraMode();
        }
    }

    #endregion

    #region 战场结束模式接口

    public bool isWin = true;
    private void CameraWarEndHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            WarEndStateParam param = obj as WarEndStateParam;
            EnterWarEndMode(param.pos, param.nWinFlag);
        }
        else
        {
            StopWarEnd();
        }
    }

    //开启战场结束模式
    private void EnterWarEndMode(Vector3 pos, int nWinFlag)
    {
        isWin = nWinFlag > 0 ? true : false;
        EndWarCameraModeController.EndWarCamContext context = new EndWarCameraModeController.EndWarCamContext();
        context.targetPos = pos;
        context.curPos = soldier.gameObject.transform.position;
        HandleModeForceSwitch(CameraMode.EndWarControl, context);
    }

    private void StopWarEnd()
    {
        if (camCtrl.GetCameraMode() != CameraMode.EndWarControl)
            return;
        RefreshCameraMode();
        //**//enableMode(CameraMode.CameraAnim, false);
    }
    #endregion

    #region Top模式接口
    //...
    #endregion

    #region GodEye模式接口
    public delegate void SwitchLookAtSoliderHanlder(U3D_Render.EntityView soliderTransform);
    public static event SwitchLookAtSoliderHanlder SwitchLookAtSoliderEvent;
    private void UpdateGodEye(GodEyeStateParam param)
    {
        if (cameraMode != CameraMode.MapGodControl && cameraMode != CameraMode.DeadControl)
        {
            EnableMode(CameraMode.MapGodControl, true, param);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.MapGodControl)
            {
                ((MapGodCameraModeController)camCtrl).ChangeGodEyePos(param.pos);
            }
            else if (camCtrl.GetCameraMode() == CameraMode.DeadControl)
            {
                ((DeadCameraModeController)camCtrl).SwitchGodEyePos(param.pos);
            }
        }
    }
    //进入上帝之眼EnterGodEyeMode(pos,angle,new Vector3(0,0,-1),new Vector3(0,0,0),20.0f,60.0f);
    private void CameraMapGodEyeHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            if (obj == null)
                EnterMapGodEyeMode();
            else
            {
                GodEyeStateParam param = obj as GodEyeStateParam;
                EnterMapGodEyeMode(param.pos, param.angle, param.dir, param.tran, param.distance, param.fov, param.isMiniMap);
            }
        }
        else
        {
            LeaveGodEyeMode();
        }
    }

    private void EnterMapGodEyeMode(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov, bool isMiniMap)
    {
        if (null == camCtrl)
            return;

        if (camCtrl.GetCameraMode() != CameraMode.MapGodControl)
        {
            MapGodCameraModeController.MapGodCamContext context = new MapGodCameraModeController.MapGodCamContext();
            context.pos = pos;
            context.angle = angle;
            context.dir = dir;
            context.tran = tran;
            context.distance = distance;
            context.fov = fov;
            HandleModeForceSwitch(CameraMode.MapGodControl, context);
        }
        else
        {
            ((MapGodCameraModeController)camCtrl).UpdateGodParams(pos, angle, dir, tran, distance, fov);
        }
    }

    private void EnterMapGodEyeMode()
    {
        if (null == camCtrl)
            return;

        if (camCtrl.GetCameraMode() != CameraMode.MapGodControl)
        {
            HandleModeForceSwitch(CameraMode.MapGodControl);
        }
    }

    private void LeaveGodEyeMode()
    {
        if (null == camCtrl)
            return;

        if (camCtrl.GetCameraMode() == CameraMode.MapGodControl)
            RefreshCameraMode();
    }
    #endregion

    #region 死亡模式接口
    private const float NormalFocusPosMaxCheckTime = 5.0f;
    public float NormalFocusPosCheckTime = NormalFocusPosMaxCheckTime + 1;

    private void CameraDeadHandler(bool enabled, CameraStateParam stateParam)
    {
        if (enabled)
        {
            if (isSurvivalMode)
            {
                HandleModeForceSwitch(CameraMode.UserControl);
            }
            else
            {

                HandleModeForceSwitch(CameraMode.DeadControl);
            }
        }
        else
        {
            if (EntityFactory.MainHeroView != null)
            {
                this.soldier = EntityFactory.MainHeroView;
                if (SwitchLookAtSoliderEvent != null)
                    SwitchLookAtSoliderEvent(EntityFactory.MainHeroView);

                NormalFocusPosCheckTime = NormalFocusPosMaxCheckTime + 1;   //旧的死亡模式
            }
            RefreshCameraMode();
        }
    }

    public void SwitchLookAtSolider(U3D_Render.EntityView soliderTransform)
    {
        if (soliderTransform != null && isDead)    //主角死的时候才会切换
        {
            this.soldier = soliderTransform;
            if (SwitchLookAtSoliderEvent != null)
                SwitchLookAtSoliderEvent(soliderTransform);

            if (cameraMode == CameraMode.DeadControl)
            {
                ((DeadCameraModeController)camCtrl).SwitchTarget();
            }

            NormalFocusPosCheckTime = NormalFocusPosMaxCheckTime + 1;   //旧的死亡模式
        }
    }
    #endregion

    #region floating模式接口
    private void CameraFloatingHandler(bool enabled, CameraStateParam obj)
    {
        if (enabled)
        {
            HandleModeForceSwitch(CameraMode.FloatingControl);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.FloatingControl)
                RefreshCameraMode();
        }
    }
    #endregion

    #region CameraAmination模式接口
    private void CameraAminationHandler(bool enabled,CameraStateParam obj)
    {
        if (enabled)
        {
            CamAnimStateParam param = obj as CamAnimStateParam;
            PlayCameraAnim(param.clip, param.animOrgType, param.projectFloor, param.targetPos, param.sourceTransform,param.animationMainCamera);
        }
        else
        {
            StopCameraAnim();
        }
    }
    private void PlayCameraAnim(AnimationClip clip, CameraAnimOrgType animOrgType, bool projectFloor, Vector3 targetPos, Transform sourceTransform, bool animationMainCamera)
    {
        if (camCtrl.GetCameraMode() == CameraMode.CameraAnim)
        {
            StopCameraAnim();
        }

        AnimCameraModeController.AnimCamContext animCamContext = new AnimCameraModeController.AnimCamContext();
        animCamContext.clip = clip;
        animCamContext.animOrgType = animOrgType;
        animCamContext.projectFloor = projectFloor;
        animCamContext.targetPos = targetPos;
        animCamContext.sourceTransform = sourceTransform;
        animCamContext.animationMainCamera = animationMainCamera;

        HandleModeForceSwitch(CameraMode.CameraAnim, animCamContext);
        //**//enableMode(CameraMode.CameraAnim, true);
    }
    private void StopCameraAnim()
    {
        if (camCtrl.GetCameraMode() != CameraMode.CameraAnim)
            return;
        RefreshCameraMode();
        //**//enableMode(CameraMode.CameraAnim, false);
    }
    #endregion

    #region OB模式接口
    private bool bFirstEnterOB = true;
    public void EnableOBMode(bool enable)
    {
        if (bFirstEnterOB)
        {
            InitOBScheme();
            bFirstEnterOB = false;
        }

        if(enable)
        {
            EnableMode(CameraMode.OBHeroLock, true);
        }
        else
        {
            SwitchOBSoliderTarget(EntityFactory.MainHeroView);
            LeaveCurMode();
        }
    }
    public void SwitchOBSoliderTarget(U3D_Render.EntityView newSoldier)
    {
        if (newSoldier != null)
        {
            this.soldier = newSoldier;
        }

        if (cameraMode == CameraMode.OBMoveFree)
        {
            GodEyeStateParam godEyeParam = new GodEyeStateParam(this.soldier.gameObject.transform.position, CurrentCamera.transform.eulerAngles, new Vector3(0, 0, -1), Vector3.zero, 15, 60);
            ((OBMoveFreeModeController)camCtrl).UpdateFocusPos(godEyeParam);
            ((OBMoveFreeModeController)camCtrl).UpdateCamera();
        }
    }

    private void CameraOBHeroLockHandler(bool enabled, CameraStateParam stateParam)
    {
        if (enabled)
        {
            HandleModeForceSwitch(CameraMode.OBHeroLock);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.OBHeroLock)
                RefreshCameraMode();
        }
    }

    private void CameraOBMoveFreeHandler(bool enabled, CameraStateParam stateParam)
    {
        if (enabled)
        {
            if (stateParam != null)
            {
                OBMoveFreeModeController.OBMoveFreeContext context = new OBMoveFreeModeController.OBMoveFreeContext();
                context.pos = this.soldier.gameObject.transform.position;
                context.angle = CurrentCamera.transform.eulerAngles;
                context.dir = new Vector3(0, 0, -1);
                context.tran = Vector3.zero;
                context.distance = 8;
                context.fov = 60;
                HandleModeForceSwitch(CameraMode.OBMoveFree, context);
            }
            else
                HandleModeForceSwitch(CameraMode.OBMoveFree);
        }
        else
        {
            if (camCtrl.GetCameraMode() == CameraMode.OBMoveFree)
                RefreshCameraMode();
        }
    }

    private void UpdateOBFocusPos(GodEyeStateParam GodEyeParam)
    {
        GodEyeParam.pos = new Vector3(GodEyeParam.pos.x, 5, GodEyeParam.pos.z);
        if (camCtrl.GetCameraMode() != CameraMode.OBMoveFree)
        {
            CameraOBMoveFreeHandler(true, GodEyeParam);
            return;
        }

        ((OBMoveFreeModeController)camCtrl).UpdateFocusPos(GodEyeParam);
    }

#if ENABLE_YUNYING_MODE

    public void SwitchLookAtOBSolider(bool isNext)
    {
        U3D_Render.EntityView newSoldier = EntityFactory.geNextPlayer(isNext);
        if (newSoldier != null)    //主角死的时候才会切换
        {
            this.soldier = newSoldier;
        }
    }

#endif
    #endregion

    /////////////////////////////////////////////////////////////////////
    #region 暂时没用的属性

    /// <summary>
    /// 是否可以更新相机逻辑
    /// </summary>
    //private bool CanUpdateCamera = true;


    /////////////////////////////////////////////////////////////////////////////////陈涛指定修改操作方式
    //private bool s_bCursorMoving = true;            // 鼠标是否正在移动
    //private float s_fCursorMoved = 0;               // 鼠标相对移动
    //private float s_fLastCursorX = 0;               // 上一次更新鼠标位置
    //private float s_fLastCursorY = 0;               // 上一次更新鼠标位置

    //private int CheckTickCount = 30;
    //private int CursorMoveDisCheck = 7;

    //private float ClearIdleInBeginStateTime = 4.0f;

    /// <summary>
    /// 登录后，判断是站立，是则，镜头随鼠标移动
    /// </summary>
    //private bool IsIdleInBegin = true;

    /// <summary>
    /// 判断当前鼠标是否要锁定
    /// </summary>
    //private bool IsCurMouseLock = false;

    //public int zoomFOV = 30;
    //public Vector2 aimSpeed  = new Vector2(70.0f, 70.0f);
    //public Vector3 aimDirection;
    //public Vector3 crouchDirection;
    //public Vector3 aimCrouchDirection;
    //public float positionLerp;
    //public float crouchHeight;
    //public float normalAimHeight;
    //public float crouchAimHeight;
    //public float minHeight;
    //public float normalAimDistance;
    //public float crouchAimDistance;

    //public void enableMode(CameraMode mode, bool enable, CameraContext context = null)
    //{
    //    //Debug.Log("switch mode!!");
    //    if (enableCameraControlMode == null || enableCameraControlMode.Length < (int)CameraControlMode.Count)
    //    {
    //        Reset();
    //    }
    //    //SwitchCamMode(mode, enable, context);
    //}

    //根据当前的模式开启列表选择下一个可使用的CameraMode
    //private int GetNextUsableMode()
    //{
    //    for (int i = 0; i < (int)CameraControlMode.Count; i++)
    //    {
    //        if (enableCameraControlMode[i])
    //            return i;
    //    }
    //    return (int)CameraMode.UserControl;
    //}

    //public void enableMode(CameraMode mode, bool enable, CameraParam param)
    //{
    //    int index = (int)mode;

    //    //下面赋值参数，默认赋值的不覆盖
    //    if (param.curDistance > -999.0f)
    //    {
    //        cameraControlModeParam[index].curDistance = param.curDistance;
    //    }

    //    if (param.distanceRange.sqrMagnitude > 0.0001f)
    //    {
    //        cameraControlModeParam[index].distanceRange = param.distanceRange;
    //    }

    //    if (param.targetOffset > -999.0f)
    //    {
    //        cameraControlModeParam[index].targetOffset = param.targetOffset;
    //    }

    //    if (param.curPitchAngle > -999.0f)
    //    {
    //        cameraControlModeParam[index].curPitchAngle = param.curPitchAngle;
    //    }

    //    if (param.pitchAngleRange.sqrMagnitude > 0.0001f)
    //    {
    //        cameraControlModeParam[index].pitchAngleRange = param.pitchAngleRange;
    //    }

    //    if (param.FOV > -999.0f)
    //    {
    //        cameraControlModeParam[index].FOV = param.FOV;
    //    }

    //    enableMode(mode, enable);
    //}

    //public void onSceneLoaded(ASpeedGame.Data.CameraConfig.CameraConfigInfo cameraInfo)
    //{
    //    if (CurCameraProperty != null && !CurCameraProperty.bFPSPriority)
    //    {
    //        this.FPSTargetOffset = cameraInfo.FPSTargetOffset;
    //    }
    //}
    #endregion

}
