/*******************************************************************
** 文件名:	CameraEffectUtil.cs
** 描  述:	摄像机光效播放工具类（同时只能播放一个摄像机光效，单例！！）
** 应  用:  	
            处理摄像机光效播放（路径动画，震屏效果）：与AnimCameraModeController有稍微职能重复
            1、震屏处理：获得光效的震屏数据，通过ShakeUtil实现
            2、路径动画处理：读取每一个节点，然后改变摄像机参数
********************************************************************/
using Effect;
using UnityEngine;

namespace U3D_Render.CameraControl
{
    class CameraEffectUtil
    {

        private static CameraEffectUtil _instance;
        public static CameraEffectUtil MainInstance
        {
            get
            {
                if (_instance == null)
                    _instance = new CameraEffectUtil();
                return _instance;
            }
        }

        private static CameraShakeUtil _ShakeInstance;
        public static CameraShakeUtil ShakeUtil
        {
            get
            {
                if (_ShakeInstance == null)
                    _ShakeInstance = new CameraShakeUtil();
                return _ShakeInstance;
            }
        }

        private BaseCameraController camCtrl {
            get { return SoldierCamera.MainInstance<SoldierCamera>().GetCurCameraController(); }
        }

        private SoldierCamera soldierCam
        {
            get { return SoldierCamera.MainInstance<SoldierCamera>(); }
        }


        ////////////////////////摄像机光效播放////////////////////////////
        public bool isPlayingEffect = false;
        public bool canPlayControl = true;   //什么鬼，都是true...
        //镜头特效移动摄像机的相关参数
        public Effect_CameraNode[] m_cameraAxis;
        public uint m_effectStartTick;
        public uint m_effectStopTick;
        public uint m_nextEffectNodeTick;
        public int m_nCameraAxisIndex;
        private bool m_recoverTransform;
        private bool m_remainAngle;
        private bool m_remainDistance;
        private bool m_remainFOV;
        private bool m_remainPosition;
        private bool m_remainDirection;
        private bool m_remainTran;

        //镜头特效速度
        private float m_effectDisSpeed;
        private Vector3 m_effectTranSpeed;
        private Vector3 m_effectDirSpeed;
        private Vector3 m_effectAngleSpeed;
        private float m_effectFovSpeed;
        private Vector3 m_effectPosSpeed;

        //镜头特效锁定参数(以后绝对改，不应该用这些bool)
        public bool m_effectLockPos = false;
        public bool m_effectLockTran = false;
        public bool m_effectLockDir = false;
        public bool m_effectLockDistance = false;
        public bool m_effectLockFov = false;
        public bool m_effectLockAngle = false;

        //镜头特效期望参数
        private float m_effectDesireFov = 0.0f;
        private float m_effectDesireDistance = 0.0f;
        private Vector3 m_effectDesirePos = Vector3.zero;
        private Vector3 m_effectDesireTran = Vector3.zero;
        private Vector3 m_effectDesireDir = Vector3.zero;
        private Vector3 m_effectDesireAngle = Vector3.zero;

        //镜头特效当前参数
        public Vector3 m_effectCurPos = Vector3.zero;//镜头特效带来的位置拉伸效果
        public float m_effectCurDistance = 0.0f; //镜头特效带来的距离拉伸效果
        public Vector3 m_effectCurTran = Vector3.zero; //镜头特效带来的平移拉伸效果
        public Vector3 m_effectCurDir = Vector3.zero;//镜头特效带来的方向拉伸效果
        public Vector3 m_effectCurAngle = Vector3.zero;//镜头特效带来的方向拉伸效果
        public float m_effectCurFov = 0.0f;//镜头特写带来的广角拉伸效果

        public float deltaTime;

        private uint s_nLastCheckTick;  // 上次检测时间
        private int s_nLastCheckTickInBegin;

        /// <summary>
        /// 摄像机光效参数重置
        /// </summary>
        public void Reset()
        {
            s_nLastCheckTick = (uint)Time.frameCount;
            s_nLastCheckTickInBegin = Time.frameCount;

            m_effectLockPos = false;
            m_effectLockTran = false;
            m_effectLockDistance = false;
            m_effectLockFov = false;
            m_effectLockAngle = false;
            m_effectLockDir = false;

            m_effectCurPos = Vector3.zero;
            m_effectCurFov = 0.0f;
            m_effectCurDistance = 0.0f;
            m_effectCurTran = Vector3.zero;
            m_effectCurDir = Vector3.zero;
            m_effectCurAngle = Vector3.zero;
        }

        /// <summary>
        ///  每次lateupadate的摄像机光效更新
        /// </summary>
        public void UpdateEffect(float deltaTime)
        {
            this.deltaTime = deltaTime;
            uint tick = GameLogicAPI.getTickCount();
            if (camCtrl == null )
            {
                return;
            }
            if (tick < m_effectStartTick)
            {
                return;
            }

            if (tick >= m_effectStopTick)
            {
                stopEffect();
                return;
            }

            if (tick >= m_nextEffectNodeTick)
            {
                m_effectDisSpeed = 0.0f;
                m_effectTranSpeed = Vector3.zero;
                m_effectDirSpeed = Vector3.zero;
                m_effectAngleSpeed = Vector2.zero;
                m_effectFovSpeed = 0.0f;
                m_effectPosSpeed = Vector3.zero;
                if (m_nCameraAxisIndex < m_cameraAxis.Length - 1)
                {
                    m_nCameraAxisIndex += 1;
                    m_nextEffectNodeTick = m_effectStartTick + (uint)m_cameraAxis[m_nCameraAxisIndex].tick;
                    Effect_CameraNode currentNode = m_cameraAxis[m_nCameraAxisIndex];
                    canPlayControl = currentNode.playerControl;
                    //if (currentNode.playerControl)
                    //{
                    //    cameraMode = CameraControlMode.UserAndEffectControl;
                    //}
                    //else
                    //{
                    //    cameraMode = CameraControlMode.EffectControl;
                    //}
                    if (m_nextEffectNodeTick <= tick)
                    {
                        m_nextEffectNodeTick = tick + 1;
                    }
                    float effectTime = (m_nextEffectNodeTick - tick) / 1000.0f;

                    //位置
                    m_effectLockPos = currentNode.lockPosition;
                    Vector3 pos = currentNode.pos - m_effectCurPos;
                    m_effectPosSpeed = pos / effectTime;
                    m_effectDesirePos = currentNode.pos;

                    //fov
                    m_effectLockFov = currentNode.lockFov;
                    float fov = currentNode.fov - m_effectCurFov;
                    m_effectFovSpeed = fov / effectTime;
                    m_effectDesireFov = currentNode.fov;

                    //距离
                    m_effectLockDistance = currentNode.lockDistance;
                    float dis = currentNode.distance - m_effectCurDistance;
                    m_effectDisSpeed = dis / effectTime;
                    m_effectDesireDistance = currentNode.distance;

                    //平移
                    m_effectLockTran = currentNode.lockTran;
                    Vector3 tran = currentNode.tran - m_effectCurTran;
                    m_effectTranSpeed = tran / effectTime;
                    m_effectDesireTran = currentNode.tran;


                    //偏移方向
                    m_effectLockDir = currentNode.lockDir;
                    Vector3 dir = currentNode.dir - m_effectCurDir;
                    m_effectDirSpeed = dir / effectTime;
                    m_effectDesireDir = currentNode.dir;


                    //角度
                    m_effectLockAngle = currentNode.lockAngle;
                    Vector3 angleDis = currentNode.angle - m_effectCurAngle;
                    m_effectAngleSpeed = angleDis / effectTime;
                    m_effectDesireAngle = currentNode.angle;


                    if (currentNode.shakeCamera == true)
                    {
                        CameraShakeContext shakeContext = new CameraShakeContext();
                        shakeContext.minShakeSpeed = currentNode.shakeParam.minShakeSpeed;
                        shakeContext.maxShakeSpeed = currentNode.shakeParam.maxShakeSpeed;
                        shakeContext.minShake = currentNode.shakeParam.minShake;
                        shakeContext.maxShake = currentNode.shakeParam.maxShake;
                        shakeContext.minShakeTimes = currentNode.shakeParam.minShakeTimes;
                        shakeContext.maxShakeTimes = currentNode.shakeParam.maxShakeTimes;

                        shakeContext.maxShakeFov = currentNode.shakeParam.maxShakeFov;
                        shakeContext.minShakeFov = currentNode.shakeParam.minShakeFov;
                        shakeContext.shakeFovSpeed = currentNode.shakeParam.shakeFovSpeed;
                        shakeContext.shakeDir = currentNode.shakeParam.shakeDir;
                        shakeContext.isDirDependRole = currentNode.shakeParam.isDependRole;
                        shakeContext.isRandom = currentNode.shakeParam.isRandom;
                        int rand = UnityEngine.Random.Range(0, 99);
                        shakeContext.maxShakeDistance = 100.0f / ((float)rand);
                        ShakeUtil.Init(shakeContext,this);
                        ShakeUtil.StartShake(1.0f);
                    }
                }
                else
                {
                    if (m_recoverTransform)
                    {
                        //所有镜头动作已经做完，最后到StopTick这段时间内开始逐渐恢复镜头
                        //m_effectPlayerControl = true;
                        m_nextEffectNodeTick = m_effectStopTick;
                        float fEffectTime = (m_effectStopTick - tick) / 1000.0f;
                        m_effectDisSpeed = -m_effectCurDistance / fEffectTime;
                        m_effectTranSpeed = -m_effectCurTran / fEffectTime;
                        m_effectDirSpeed = -m_effectCurDir / fEffectTime;
                        m_effectFovSpeed = -m_effectCurFov / fEffectTime;
                        m_effectAngleSpeed = -m_effectCurAngle / fEffectTime;
                        m_effectPosSpeed = -m_effectCurPos / fEffectTime;
                        m_effectDesireDistance = 0.0f;
                        m_effectDesireFov = 0.0f;
                        m_effectDesireTran = Vector3.zero;
                        m_effectDesireDir = Vector3.zero;
                        m_effectDesireAngle = Vector3.zero;
                        m_effectDesirePos = Vector3.zero;
                        ShakeUtil.shake = false;
                    }
                    else
                    {
                        if (m_remainAngle) { soldierCam.m_userDesireAngle += m_effectCurAngle; }
                        if (m_remainDistance) { soldierCam.m_userCurDistance += soldierCam.m_userCurDistance; }
                        if (m_remainFOV) { soldierCam.m_userDesireFov = soldierCam.m_userCurFov; }
                        if (m_remainPosition) { soldierCam.m_userDesirePos = soldierCam.m_userCurPos; }
                        if (m_remainDirection) { soldierCam.m_userDesireDir = soldierCam.m_userCurDir; }
                        if (m_remainTran) { soldierCam.m_userDesireTran = soldierCam.m_userCurTran; }
                        stopEffect();
                        return;
                    }
                }
            }


            CameraUtil.advanceValue(ref m_effectCurFov, m_effectDesireFov, m_effectFovSpeed * deltaTime);
            CameraUtil.advanceValue(ref m_effectCurDistance, m_effectDesireDistance, m_effectDisSpeed * deltaTime);
            CameraUtil.advanceVec(ref m_effectCurPos, m_effectDesirePos, m_effectPosSpeed * deltaTime);
            CameraUtil.advanceVec(ref m_effectCurTran, m_effectDesireTran, m_effectTranSpeed * deltaTime);
            CameraUtil.advanceVec(ref m_effectCurDir, m_effectDesireDir, m_effectDirSpeed * deltaTime);
            CameraUtil.advanceVec(ref m_effectCurAngle, m_effectDesireAngle, m_effectAngleSpeed * deltaTime);
        }

        /// <summary>
        /// 播放摄像机光效
        /// </summary>
        /// <param name="delayTicks"></param>
        /// <param name="nliveTicks"></param>
        /// <param name="cameraNodes"></param>
        /// <param name="recoverTransform"></param>
        public void playEffect(int delayTicks, int nliveTicks, Effect_CameraNode[] cameraNodes, bool recoverTransform)
        {
            if (camCtrl==null ||
                camCtrl.GetCameraMode() == CameraMode.MapGodControl ||
                camCtrl.GetCameraMode() == CameraMode.TopControl ||
                camCtrl.GetCameraMode() == CameraMode.DeadControl ||
                camCtrl.GetCameraMode() == CameraMode.EndWarControl)
            {
                return;
            }

            uint tick = GameLogicAPI.getTickCount();
            m_effectStartTick = tick + (uint)delayTicks;
            m_effectStopTick = m_effectStartTick + (uint)nliveTicks;
            m_cameraAxis = cameraNodes;
            m_nCameraAxisIndex = -1;
            m_nextEffectNodeTick = m_effectStartTick;
            m_recoverTransform = recoverTransform;

            if (!m_recoverTransform)
            {
                //如果不逐渐恢复镜头的话，停止摄像机特效时，只保留当前角度，其他的瞬间恢复。
                m_remainAngle = true;
                m_remainDistance = false;
                m_remainFOV = false;
                m_remainPosition = false;
                m_remainDirection = false;
                m_remainTran = false;
            }
            else
            {
                m_remainAngle = false;
                m_remainDistance = false;
                m_remainFOV = false;
                m_remainPosition = false;
                m_remainDirection = false;
                m_remainTran = false;
            }

            stopEffect();
            isPlayingEffect = true;
            canPlayControl = true;  //有些摄像机特效静止玩家控制摄像机

            //cameraMode = CameraControlMode.UserAndEffectControl;
        }

        /// <summary>
        /// 停止摄像机光效效果
        /// </summary>
        public void stopEffect()
        {
            if (camCtrl == null ||
                camCtrl.GetCameraMode() == CameraMode.MapGodControl ||
                camCtrl.GetCameraMode() == CameraMode.TopControl ||
                camCtrl.GetCameraMode() == CameraMode.DeadControl ||
                camCtrl.GetCameraMode() == CameraMode.EndWarControl)
            {
                return;
            }

            m_effectLockPos = false;
            m_effectLockTran = false;
            m_effectLockDistance = false;
            m_effectLockFov = false;
            m_effectLockAngle = false;
            m_effectLockDir = false;

            m_effectCurFov = 0.0f;
            m_effectCurDistance = 0.0f;
            m_effectCurTran = Vector3.zero;
            m_effectCurDir = Vector3.zero;
            m_effectCurAngle = Vector3.zero;
            m_effectCurPos = Vector3.zero;
            isPlayingEffect = false;
            canPlayControl = true;
            //cameraMode = CameraControlMode.UserControl;
            ShakeUtil.shake = false;
        }

        /// <summary>
        /// 在finishTick的时间内恢复摄像机
        /// </summary>
        /// <param name="finishTick"></param>
        public void finishEffect(uint finishTick)
        {
            uint tick = GameLogicAPI.getTickCount();
            m_nextEffectNodeTick = tick;
            m_effectStopTick = tick + finishTick;
            if (m_cameraAxis != null)
            {
                m_nCameraAxisIndex = m_cameraAxis.Length - 1;
            }
        }
    }
}
