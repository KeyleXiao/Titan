using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using GameLogic;
using U3D_Render.CameraControl;
using Effect;

public partial class SoldierCamera : BaseSoldierCamera
{
    #region  镜头用户操作期望参数
    /// <summary>
    /// 当前摄像机正在使用的参数
    /// 需要继承到不同摄像机状态
    /// </summary>
    public Vector3 m_userDesirePos = Vector3.zero;
    public Vector3 m_userDesireTran = Vector3.zero;
    public Vector3 m_userDesireAngle = Vector3.zero;
    public Vector3 m_userDesireDir = Vector3.zero;
    public float m_userDesireFov = 0.0f;
    public float m_userDesireDistance = 2.3f;
    #endregion

    #region  镜头用户操作当前值
    /// <summary>
    /// 当前摄像机正在使用的参数
    /// 需要继承到不同摄像机状态
    /// </summary>
    public Vector3 m_userCurPos = Vector3.zero;
    public Vector3 m_userCurTran = Vector3.zero;
    public Vector3 m_userCurAngle = Vector3.zero;
    public Vector3 m_userCurDir = Vector3.zero;
    public float m_userCurFov = 0.0f;
    public float m_userCurDistance = 0.0f;
    public void SetUserDesireAngleX(float x) { m_userDesireAngle.x = x; }
    public void SetUserDesireAngleY(float y) { m_userDesireAngle.y = y; }

    public Vector3 userDesireAngle
    {
        set
        {
            m_userDesireAngle = value;
            if (curCameraModeParam != null)
            {
                SetUserDesireAngleX(Mathf.Clamp(m_userDesireAngle.x, curCameraModeParam.pitchAngleRange.x, curCameraModeParam.pitchAngleRange.y));
            }
        }
        get { return m_userDesireAngle; }
    }

    public float userDesireDistance
    {
        set { m_userDesireDistance = value; }
        get { return m_userDesireDistance; }
    }

    public float OBCtrlMouseScrollSpeed
    {
        get { return obCtrlMouseScrollCurSpeed; }
        set { obCtrlMouseScrollCurSpeed = Mathf.Clamp(value, obCtrlMouseScrollSpeedRange.x, obCtrlMouseScrollSpeedRange.y); }
    }
    public float OBCtrlCamMoveSpeed
    {
        get { return obCtrlCamMoveCurSpeed; }
        set { obCtrlCamMoveCurSpeed = Mathf.Clamp(value, obCtrlCamMoveSpeedRange.x, obCtrlCamMoveSpeedRange.y); }
    }

    private float obCtrlMouseScrollCurSpeed; //OB鼠标滚动速度
    private float obCtrlCamMoveCurSpeed;     //OB相机移动速度
    #endregion

    public float CurDistance
    {
        get { return apprDistance; }
    }

    /////////////////////////////////////////////////////////////////////
    #region 相机逻辑计算属性

    public Vector3 apprPos = Vector3.zero;
    public float apprFov = 60.0f;
    public Vector3 apprAngle = Vector3.zero;
    public Vector3 apprTran = Vector3.zero;
    public Vector3 apprDir = -Vector3.forward;
    public float apprDistance = 7.0f;
    public float apprFactor = 0.5f;

    public float navFactor = 0.05f;

    public float followNavFactor = 0.04f;


    #endregion
    /////////////////////////////////////////////////////////////////////
    #region  相机逻辑计算可配置属性

    [HideInInspector]
    public CameraProperty CurCameraProperty;//相机属性,从英雄预制体获取

    public void Copy(CameraProperty other)
    {
        CurCameraProperty.Copy(other);
    }

    private float obCtrlMouseScrollSpeedDefault;  //鼠标滚动速度默认值
    private float obCtrlCamMoveSpeedDefault;      //相机移动速度默认值

    private Vector2 obCtrlMouseScrollSpeedRange;  //鼠标滚动速度上下限
    private Vector2 obCtrlCamMoveSpeedRange;      //相机移动速度上下限
    #endregion
    /////////////////////////////////////////////////////////////////////

    //将镜头自动转向目标的配置
    public float m_approximageRotateSpeed = 0.0f;
    public float rotateStartSpeed = 120.0f;
    public float rotateAcceleratedSpeed = 1000.0f;
    public float rotateStartAngle = 90.0f;
    public float rotateStartMinDistance = 5.0f;
    public float rotateStartMaxDistance = 15.0f;

    private BaseCameraController camCtrl;
    public BaseCameraController GetCurCameraController() { return camCtrl; }

    private AnimCameraModeController m_animCamCtrl;
    private DeadCameraModeController m_deadCamCtrl;
    private EndWarCameraModeController m_endWarCamCtrl;
    private FloatingCameraModeController m_floatingCamCtrl;
    private FollowCameraModeController m_followCamCtrl;
    private FPSCameraModeController m_fpsCamCtrl;
    private MapGodCameraModeController m_mapGodCamCtrl;
    private RidingCameraModeController m_ridingCamCtrl;
    private SniperscopeCameraModeController m_sniperscopeCamCtrl;
    private TopCameraModeController m_topCamCtrl;
    private UserCameraModeController m_userCamCtrl;
    private OBHeroLockModeController m_ob_hero_lockCamCtrl;
    private OBMoveFreeModeController m_ob_hero_moveFreeCamCtrl;

#if ENABLE_YUNYING_MODE

    private YUNYING_OBHeroLockModeController YUNYING_m_ob_hero_lockCamCtrl;
    private YUNYING_OBHeroMoveFreeModeController YUNYING_m_ob_hero_movefreeCamCtrl;
    private YUNYING_OBHeroTraceModeController YUNYING_m_ob_hero_traceCamCtrl;

#endif
    public class CameraParam
    {
        public float curDistance = -1000.0f;
        public Vector2 distanceRange = Vector2.zero;
        public float curPitchAngle = -1000.0f;
        public Vector2 pitchAngleRange = Vector2.zero;

        public float targetOffset
        {
            get
            {
                float val = m_targetOffset;
#if ENABLE_YUNYING_MODE
                val += BaseCameraController.ExternTargetOffset;
#endif
                return val;
            }
            set
            {
                m_targetOffset = value;
            }
        }
        private float m_targetOffset = -1000.0f;
        public float FOV = -1000.0f;
        public int modeID = 0;

        public CameraParam GetCopy()
        {
            CameraParam param = new CameraParam();
            param.curDistance = this.curDistance;
            param.distanceRange = this.distanceRange;
            param.curPitchAngle = this.curPitchAngle;
            param.pitchAngleRange = this.pitchAngleRange;
            param.targetOffset = this.targetOffset;
            param.FOV = this.FOV;
            param.modeID = this.modeID;

            return param;
        }
    }

    public class CamStateChangeMsg
    {
        public CameraMode mode;
        public bool enabled;
        public CameraStateParam param;
        public CamStateChangeMsg(CameraMode mode, bool enabled, CameraStateParam param)
        {
            this.mode = mode;
            this.enabled = enabled;
            this.param = param;
        }
    }
}
