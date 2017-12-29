/*******************************************************************
** 文件名:	BaseCameraController.cs
** 描  述:	摄像机状态基类
** 应  用:  	
            处理每个摄像机都需要做的工作：
            1、Reset函数：重置参数，以及重置需要用到的组件
            2、NavNormally？
            3、advanceByEffect：光效对摄像机的控制
            4、GetInputNormally：获取输入
            5、UpdateTransformNormally：计算摄像机具体参数【摄像机旋转逻辑不明确，与mouseCtrl有强烈耦合】
            6、CollectNormally：导航数据采集
********************************************************************/
using GameLogic;
using System.Collections.Generic;
using UnityEngine;
using USpeedUI;

namespace U3D_Render.CameraControl
{
    public abstract class BaseCameraController : ICameraModelController
    {
#if ENABLE_YUNYING_MODE
        public static float ExternOBCameraMoveSpeed = 0;
        public static float ExternOBCameraRoateSpeed = 0;
        public static float ExternOBCamerFOV = 0;

        public static float MoveSpeedUpdateDealt = 0.5f;
        public static float RoateSpeedUpdateDealt = 0.5f;
        public static float FOVUpdateDealt = 0.5f;

        public static float ExternTargetOffset = 0.0f;

        public static void ResetExternOBCameraInfo()
        {
            ExternOBCameraMoveSpeed = 0;
            ExternOBCameraRoateSpeed = 0;
            ExternOBCamerFOV = 0;
        }
#endif
        public SoldierCamera soldierCam
        { 
            set
            {
                m_soldierCam = value;
            }
            get { return m_soldierCam; }
        }

        public SoldierCamera m_soldierCam;

        private LayerMask hitLayer = -1;

        public List<CameraMode> AllowChangeList = new List<CameraMode>();

        //SoldierCamera数据
        protected EntityView soldier { get { return soldierCam.soldier; } }
        protected SoldierCamera.CameraParam curCameraModeParam { get { return soldierCam.curCameraModeParam; } }
        protected SoldierController soldierController { get { return soldierCam.soldierController; } }
        protected MouseCtrl mouseCtrl { get { return soldierCam.mouseCtrl; } }
        protected CameraProperty CurCameraProperty { get { return soldierCam.CurCameraProperty; } }
        protected Camera m_Camera { get { return soldierCam.CurrentCamera; } }
        protected Transform target { get { return soldierCam.target; } }

        protected Transform camTransform { get { return soldierCam.getCameraTransform(); } }
        #region 相机逻辑计算属性
        //private float distance = 10.0f;//Shake Distance
        //private Quaternion rotation; 
        protected Vector3 cPos;
        protected float deltaTime = 0.03f;
        protected Quaternion originalSoldierRotation;
        //private float crouchDistance = 2.3f;
        #endregion

        public const float maxDetlaTime = 0.1f; //镜头每次移动的最大deltaTime时间，避免镜头剧烈移动

        //当前摄像机模式
        public CameraMode curCameraMode = CameraMode.UserControl;

        /// <summary>
        /// 镜头Transform控制参数，
        /// </summary>

        //镜头用户操作当前值
        protected Vector3 m_userCurPos
        {
            get { return soldierCam.m_userCurPos; }
            set { soldierCam.m_userCurPos = value; }
        }
        protected Vector3 m_userCurTran
        {
            get { return soldierCam.m_userCurTran; }
            set { soldierCam.m_userCurTran = value; }
        }
        protected Vector3 m_userCurAngle
        {
            get { return soldierCam.m_userCurAngle; }
            set { soldierCam.m_userCurAngle = value; }
        }
        protected Vector3 m_userCurDir
        {
            get { return soldierCam.m_userCurDir; }
            set { soldierCam.m_userCurDir = value; }
        }
        protected float m_userCurFov
        {
            get { return soldierCam.m_userCurFov; }
            set { soldierCam.m_userCurFov = value; }
        }
        protected float m_userCurDistance
        {
            get { return soldierCam.m_userCurDistance; }
            set { soldierCam.m_userCurDistance = value; }
        }

        protected Vector3 apprPos
        {
            get { return soldierCam.m_userDesirePos; }
            set { soldierCam.m_userDesirePos = value; }
        }
        protected float apprFov
        {
            get { return soldierCam.apprFov; }
            set { soldierCam.apprFov = value; }
        }
        protected Vector3 apprAngle
        {
            get { return soldierCam.apprAngle; }
            set { soldierCam.apprAngle = value; }
        }
        protected Vector3 apprTran
        {
            get { return soldierCam.apprTran; }
            set { soldierCam.apprTran = value; }
        }
        protected Vector3 apprDir
        {
            get { return soldierCam.apprDir; }
            set { soldierCam.apprDir = value; }
        }
        protected float apprDistance
        {
            get { return soldierCam.apprDistance; }
            set { soldierCam.apprDistance = value; }
        }

        protected float apprFactor
        {
            get { return soldierCam.apprFactor; }
            set { soldierCam.apprFactor = value; }
        }


        ////镜头用户操作期望参数
        protected Vector3 m_userDesirePos
        {
            get { return soldierCam.m_userDesirePos; }
            set { soldierCam.m_userDesirePos = value; }
        }
        protected Vector3 m_userDesireTran
        {
            get { return soldierCam.m_userDesireTran; }
            set { soldierCam.m_userDesireTran = value; }
        }
        protected Vector3 m_userDesireDir
        {
            get { return soldierCam.m_userDesireDir; }
            set { soldierCam.m_userDesireDir = value; }
        }
        protected float m_userDesireFov
        {
            get { return soldierCam.m_userDesireFov; }
            set { soldierCam.m_userDesireFov = value; }
        }
        protected float m_userDesireDistance
        {
            get { return soldierCam.m_userDesireDistance; }
            set { soldierCam.m_userDesireDistance = value; }
        }
        protected Vector3 m_userDesireAngle
        {
            get { return soldierCam.userDesireAngle; }
            set { soldierCam.m_userDesireAngle = value; }
        }

        //摄像机平滑时需要用到的统计参数
        private float m_avgDetaTime = 0.0f;
        private float m_accDetaTime = 0.0f;
        private float m_accDeviation = 0.0f;
        private int m_deviationCount = 0;
        public bool adaptSmooth = true;
        private float m_deviationN = 30.0f;

        //狙击镜模式
        private Transform m_sniperscopreWeapon = null;

        #region 基础接口

        public BaseCameraController()
        {
            this.soldierCam = BaseSoldierCamera.MainInstance<SoldierCamera>();
            Init();
            SetAllowChangeList();
        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        abstract public void Init();


        abstract public void SetAllowChangeList();


        /// <summary>
        /// 本控制器接管
        /// </summary>
        /// <param name="context"></param>
        virtual public void Enter(CameraContext context)
        {

        
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        virtual public void Leave()
        {

        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        virtual public CameraMode GetCameraMode()
        {
            return CameraMode.UserControl;
        }
        

        /// <summary>
        /// 更新相机数据
        /// </summary>
        /// <param name="context"></param>
        virtual public void UpdateData(CameraContext context)
        {

        }

        virtual public void Reset(CameraProperty camProperty,bool smoothBlend)
        {
            if(soldier==null)
                return;
            Vector3 newEulerAngle = new Vector3(camProperty.InitedEulerAngle.x, m_userDesireAngle.y, camProperty.InitedEulerAngle.z);
            camTransform.eulerAngles = newEulerAngle;
            m_userDesireAngle = newEulerAngle;
            m_userDesireDistance = camProperty.NormalDistance;
            m_userDesireFov = camProperty.NormalFOV;
            m_userDesireDir = new Vector3(camProperty.NormalDirection.z, camProperty.NormalDirection.y, camProperty.NormalDirection.x);
            m_userDesireTran = new Vector3(0, camProperty.NormalHeight, 0);

            /////////////////////////////////


            m_userCurPos = soldier.gameObject.transform.position;
            m_userCurTran = m_userDesireTran;
            m_userCurAngle = m_userDesireAngle;
            m_userCurDir = m_userDesireDir;
            m_userCurFov = m_userDesireFov;
            m_userCurDistance = m_userDesireDistance;

            apprPos = m_userCurPos;
            apprAngle = m_userCurAngle;
            apprDir = m_userCurDir;

            if (!smoothBlend)
            {
                apprTran = m_userCurTran;
                apprFov = m_userCurFov;
                apprDistance = m_userCurDistance;
            }

            CameraEffectUtil.MainInstance.Reset();
            CameraEffectUtil.ShakeUtil.Reset();
        }

        virtual public void OnUpdate()
        {

        }

        virtual public void OnLateUpdate()
        {
            soldierCam.NavNormally(); //镜头自动跟随，包括切换模式时候的过渡或者摄像机
            if (CameraEffectUtil.MainInstance.isPlayingEffect)
            {
                CameraEffectUtil.MainInstance.UpdateEffect(deltaTime);
            }
            if (soldierController != null && CameraEffectUtil.MainInstance.canPlayControl)
            {
                GetInputNormally(curCameraModeParam.pitchAngleRange);
            }
            Vector3 cOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
            UpdateTransformNormally(cOffset); //计算摄像机具体参数
            soldierCam.CollectNormally(); //导航数据采集
            if (soldierController != null && soldierController.dead)    //纳尼？？
                CalNormalCameraFocusPos();
        }

        virtual protected bool canMouseRotateCamera()
        {
            return false;
        }

        #endregion

        protected void SetupByCamParam()
        {
            m_userDesireFov = curCameraModeParam.FOV;
            soldierCam.SetUserDesireAngleX(curCameraModeParam.curPitchAngle);
            m_userDesireDistance = curCameraModeParam.curDistance;

            if (CurCameraProperty != null)
            {
                CurCameraProperty.NormalFOV = (int)curCameraModeParam.FOV;
                CurCameraProperty.NormalDistance = curCameraModeParam.curDistance;
                CurCameraProperty.NormalHeight = curCameraModeParam.targetOffset;
                CurCameraProperty.InitedEulerAngle.x = curCameraModeParam.curPitchAngle;

                m_userDesireTran = new Vector3(0, CurCameraProperty.NormalHeight, 0);
            }
        }

        public void SetLateDealtaTime()
        {
            deltaTime = Time.deltaTime;
            if (deltaTime > maxDetlaTime)
            {
                deltaTime = maxDetlaTime;
            }
        }

        public void SendCameraFocusPosToLogic(int _bIsGodMode, float _fPosX = 0, float _fPosY = 0, float _fPosZ = 0)
        {
            if (EntityFactory.MainHeroID <= 0)
            {
                return;
            }

            cmd_entity_master_eye_focus data;
            data.isGodMode = _bIsGodMode;
            data.x = _fPosX;
            data.y = _fPosY;
            data.z = _fPosZ;

            IntPtrHelper helper = new IntPtrHelper();
            System.IntPtr ptr = helper.toPtr<cmd_entity_master_eye_focus>(ref data);
            int len = System.Runtime.InteropServices.Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_MASTER_EYE_FOCUS, len, "", ptr, len);

        }

        private void CalNormalCameraFocusPos()
        {
            soldierCam.NormalFocusPosCheckTime += deltaTime;
            if (soldierCam.NormalFocusPosCheckTime > 5.0f)
            {
                if (curCameraMode == CameraMode.DeadControl)  
                    SendCameraFocusPosToLogic(1, cPos.x, cPos.y, cPos.z);
                soldierCam.NormalFocusPosCheckTime = 0;
            }
        }

        void UpdateTransformNormally(Vector3 cPosOffset)
        {

            //摄像机广角
            if (!CameraEffectUtil.MainInstance.m_effectLockFov)
            {
                m_userCurFov = m_userDesireFov;
            }
            apprFov = CameraUtil.approximateFloat(apprFov, m_userCurFov, apprFactor);
            float finalFov = apprFov + CameraEffectUtil.MainInstance.m_effectCurFov;
            if (finalFov > 150.0f) finalFov = 150.0f;
            m_Camera.fieldOfView = finalFov;

#if ENABLE_YUNYING_MODE
            m_Camera.fieldOfView += ExternOBCamerFOV;
#endif

            //根据角度计算坐标基
            if (!CameraEffectUtil.MainInstance.m_effectLockAngle)
            {
                m_userCurAngle = m_userDesireAngle;
            }
            apprAngle = CameraUtil.approximateAngleVec(apprAngle, m_userCurAngle, 1.0f);
            Vector3 finalAngle = apprAngle + CameraEffectUtil.MainInstance.m_effectCurAngle;
            Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(finalAngle), Vector3.one);
            //Vector3 baseright = new Vector3(basem.m00, basem.m10, basem.m20);
            //Vector3 baseup = new Vector3(basem.m01, basem.m11, basem.m21);
            //Vector3 baseforward = new Vector3(basem.m02, basem.m12, basem.m22);

            //摄像机方向，一般调整上面的角度就可以了，这里是据说是调整错切的
            if (!CameraEffectUtil.MainInstance.m_effectLockDir)
            {
                m_userCurDir = m_userDesireDir;
            }
            apprDir = CameraUtil.approximateVec(apprDir, m_userCurDir, 1.0f);
            Vector3 normalDir = basem * apprDir;
            normalDir = normalDir.normalized;
            Vector3 effectedDir = basem *
                new Vector3(CameraEffectUtil.MainInstance.m_effectCurDir.z, CameraEffectUtil.MainInstance.m_effectCurDir.y, CameraEffectUtil.MainInstance.m_effectCurDir.x);
            effectedDir = effectedDir.normalized;
            Vector3 finalDir = (normalDir + effectedDir).normalized;

            //摄像机深度
            if (!CameraEffectUtil.MainInstance.m_effectLockDistance)
            {
                m_userCurDistance = m_userDesireDistance;
            }
            apprDistance = CameraUtil.approximateFloat(apprDistance, m_userCurDistance, apprFactor);
            float finalDistance = apprDistance + CameraEffectUtil.MainInstance.m_effectCurDistance;
            if (finalDistance < 1.0f) finalDistance = 1.0f;

            //摄像机平移（相对朝向）
            if (!CameraEffectUtil.MainInstance.m_effectLockTran)
            {
                m_userCurTran = m_userDesireTran;
            }
            apprTran = CameraUtil.approximateVec(apprTran, m_userCurTran, apprFactor);

            Vector3 finalTran = apprTran + CameraEffectUtil.MainInstance.m_effectCurTran;
            finalTran = CameraEffectUtil.ShakeUtil.HandleCameraShake(finalTran);//震屏

            //摄像机位置（绝对世界）
            if (!CameraEffectUtil.MainInstance.m_effectLockPos)
            {
                m_userCurPos = m_userDesirePos;
            }
            apprPos = CameraUtil.approximateVec(apprPos, m_userCurPos, apprFactor);
            Vector3 finalPos = apprPos + CameraEffectUtil.MainInstance.m_effectCurPos;

            //摄像机目标点位置
            {
                Vector3 transformedtran = basem * finalTran;
                cPos = finalPos + transformedtran;
            }

            //设置摄像机
            Vector3 targetPos = cPos + cPosOffset;


            if (curCameraMode == CameraMode.SniperscopeControl && soldier != null && soldier.gameObject)
            {
                //把摄像机位置拉到枪上
                Vector3 camPos = Vector3.zero;
                if (m_sniperscopreWeapon)
                {
                    camPos = m_sniperscopreWeapon.position - finalDir * m_userCurTran.magnitude * 0.5f;
                }
                else
                {
                    camPos = soldier.gameObject.transform.transform.position + m_userCurTran - finalDir * m_userCurTran.magnitude * 0.5f;
                }


                camTransform.SetPosition(camPos);
                camTransform.forward = -finalDir;
            }
            else
            {
                camTransform.SetPosition(targetPos + (finalDir * finalDistance));
                camTransform.LookAt(targetPos);

                //如果摄像机与人物之间被地形阻挡，则拉近到地形以上
                int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
                Vector3 viewVec = camTransform.position - targetPos;
                Ray viewRay = new Ray(targetPos, viewVec.normalized);
                RaycastHit hitInfo;
                bool hitGround = Physics.Raycast(viewRay, out hitInfo, viewVec.magnitude, LayerMaskOnGround);
                if (hitGround)
                {
                    float offset = viewVec.magnitude / 2.0f;
                    offset = offset < 0.1f ? offset : 0.1f;
                    camTransform.SetPosition(hitInfo.point + (viewVec * -offset));
                }
            }


            //设置目标框
            soldierCam.target.SetPosition(cPos);
            soldierCam.target.eulerAngles = finalAngle;
        }

        //....
        private float dx;
        private float dy;
        public float k1 = 1.0f;
        public float k2 = 1.0f;
        public float k3 = 1.0f;
        private float n = 1.0f;
        public float maxn = 5.0f;
        public float minn = 0.0f;
        private float rx;
        private float ry;

        //处理用户输入
        protected void GetInputNormally(Vector2 angleClamp)
        {
            if (mouseCtrl == null)
            {
                return;
            }

            float fMouseScrollValue = 0.0f;
            if (InputManager.IsMouseInScreen())
            {
                fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
            }

            if (Mathf.Abs(fMouseScrollValue) > 0.01f)
            {
                if ((Application.isEditor) && (InputManager.GetKey(KeyCode.LeftControl) || InputManager.GetKey(KeyCode.RightControl)))
                {
                    //m_userDesireFov = CurCameraProperty.NormalFOV;
                    m_userDesireFov = m_userDesireFov - fMouseScrollValue * CurCameraProperty.ScrollSpeed;
                    m_userDesireFov = Mathf.Clamp(m_userDesireFov, 10.0f, 120.0f);
                }
                else if (canUserControlCamera())
                {
                    // normalDistance -= InputManager.GetAxis("Mouse ScrollWheel") * Time.deltaTime * CurCameraProperty.ScrollSpeed;//normalDistance -= InputManager.GetAxis("Mouse ScrollWheel") * Time.deltaTime * scrollSpeed;
                    m_userDesireDistance = Mathf.Lerp(m_userDesireDistance, m_userDesireDistance - fMouseScrollValue * CurCameraProperty.ScrollSpeed, Time.deltaTime);
                    m_userDesireDistance = Mathf.Clamp(m_userDesireDistance, curCameraModeParam.distanceRange.x, curCameraModeParam.distanceRange.y);//normalDistance = Mathf.Clamp(normalDistance, minDistance, maxDistance);
                }

                soldierCam.NavPauseTime = Time.time;//如果用户操作了暂停自动导航一会
            }

            //m_userDesireDir = new Vector3(CurCameraProperty.NormalDirection.z, CurCameraProperty.NormalDirection.y, CurCameraProperty.NormalDirection.x);

            //m_userDesireTran = new Vector3(0, CurCameraProperty.NormalHeight, 0);

            if (!soldier.gameObject)
            {
                Trace.Warning("SoliderCamera.soldier.gameObject is null!");
                return;
            }

            float dis = (m_userDesirePos - soldier.gameObject.transform.position).magnitude;
            if (dis > 10.0f || curCameraMode == CameraMode.FPSMode)
            {
                m_userDesirePos = soldier.gameObject.transform.position;
            }
            else
            {
                //帧率波动距离时，摄像机会抖动，特别是左右移动时此现象明显
                //这里对摄像机位移利用逼近方案平滑，解决抖动
                //位移逼近方案
                Vector3 apprMoveX = CameraUtil.approximateVec(m_userDesirePos, soldier.gameObject.transform.position, soldierCam.smoothFactor.x) - m_userDesirePos;
                Vector3 apprMoveY = CameraUtil.approximateVec(m_userDesirePos, soldier.gameObject.transform.position, soldierCam.smoothFactor.y) - m_userDesirePos;
                Vector3 apprMoveZ = CameraUtil.approximateVec(m_userDesirePos, soldier.gameObject.transform.position, soldierCam.smoothFactor.z) - m_userDesirePos;
                float moveRight = Vector3.Dot(apprMoveX, camTransform.right);
                float moveForward = Vector3.Dot(apprMoveZ, camTransform.forward);
                float moveUp = Vector3.Dot(apprMoveY, camTransform.up);

                //以下代码帧率突变进行统计，为平滑参数提供参考
                if (adaptSmooth)
                {
                    //用标准差的方式算帧率突变的程度
                    //每m_deviationN帧算一组，该组的平均值作为下一组的平均值使用
                    //累计m_deviationN后平滑因子按(0.75f - Mathf.Sqrt(deviation)*2.0f)计算，并且和上一个平滑因子37开叠加，避免平滑因子突变
                    float deltaTime = Time.deltaTime;
                    if (m_avgDetaTime < 0.01f)
                    {
                        m_avgDetaTime = deltaTime;
                    }

                    m_accDetaTime += deltaTime;
                    float k = deltaTime - m_avgDetaTime;
                    k = k * k;
                    m_accDeviation += k;

                    m_deviationCount++;
                    if (m_deviationCount >= m_deviationN)
                    {
                        m_avgDetaTime = m_accDetaTime / m_deviationN;
                        float deviation = Mathf.Sqrt(m_accDeviation / m_deviationN);
                        soldierCam.smoothFactor.x = soldierCam.smoothFactor.x * 0.3f + (0.75f - Mathf.Sqrt(deviation) * 2.0f) * 0.7f;
                        if (soldierCam.smoothFactor.x < 0.2f) { soldierCam.smoothFactor.x = 0.2f; }
                        m_accDetaTime = 0.0f;
                        m_accDeviation = 0.0f;
                        m_deviationCount = 0;
                    }
                }


                /*
                //速度逼近方案
                Vector3 moveVec = soldier.position - m_userDesirePos;
                float moveRight = Vector3.Dot(moveVec, camTransform.right);
                float moveForward = Vector3.Dot(moveVec, camTransform.forward);
                float moveUp = Vector3.Dot(moveVec, camTransform.up);
                float speedRight = moveRight - m_lastMoveRight;
                float apprSpeedRight = approximateFloat(m_lastSpeedRight, speedRight, smoothFactor.x);
                if (Mathf.Abs(speedRight) > Mathf.Abs(apprSpeedRight))
                {
                    speedRight = apprSpeedRight;
                }
                if (Mathf.Abs(moveRight) > Mathf.Abs(m_lastMoveRight + speedRight))
                {
                    moveRight = m_lastMoveRight + speedRight;
                }
                m_lastMoveRight = moveRight;
                m_lastSpeedRight = speedRight;
                */
                Vector3 offset = moveForward * camTransform.forward + moveUp * camTransform.up + moveRight * camTransform.right;
                m_userDesirePos += offset;
            }
            //m_userDesirePos = soldier.position;

            if ((mouseCtrl.cameraRotateButton || canMouseRotateCamera()) && canCameraRotate() && !UISystem.Instance.IsMouseOverUI())
            {
                float _x = mouseCtrl.fMouseAxisX;
                float _y = mouseCtrl.fMouseAxisY;


                if (k1 < 1.0f)
                {
                    k1 = 1.0f;
                }

                //Vector2 a = soldierController.aim ? aimSpeed : speed;
                Vector2 a = CurCameraProperty.Speed  * CurCameraProperty.SpeedScale;//speed;
                dx = Mathf.Clamp(_x * a.x, -999999999.0f, 99999999.0f);
                dy = Mathf.Clamp(_y * a.y, -999999999.0f, 99999999.0f);

                n = k1 * Mathf.Pow(Mathf.Sqrt(_x * _x + _y * _y) / (k3 * deltaTime), k2);

                n = Mathf.Clamp(n, minn, maxn);
                rx = dx * n;
                ry = dy * n;

                if (Mathf.Abs(rx) > 0.1f || Mathf.Abs(ry) > 0.1f)
                {
                    soldierCam.exitNavMode(CameraNavMode.Target); //其他操作中断镜头追踪攻击对象
                    soldierCam.exitNavMode(CameraNavMode.FloatingAngle); //其他操作中断自动转镜头
                }

                soldierCam.SetUserDesireAngleY(m_userDesireAngle.y + rx);        //x += Mathf.Clamp(_x * a.x, -maxSpeed.x, maxSpeed.x) * deltaTime;
                soldierCam.SetUserDesireAngleX(m_userDesireAngle.x - ry);         //y -= Mathf.Clamp(_y * a.y, -maxSpeed.y, maxSpeed.y) * deltaTime;
                                                  //这里限制仰俯角，不能去掉
                soldierCam.SetUserDesireAngleX(Mathf.Clamp(m_userDesireAngle.x, angleClamp.x, angleClamp.y));//y = ClampAngle(y, yMinLimit, yMaxLimit);
                soldierCam.SetUserDesireAngleY(m_userDesireAngle.y - Mathf.Floor(m_userDesireAngle.y / 360.0f) * 360.0f);
            }

            if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.MoveCameraPositionToFront))
            {
                if (soldier != null)
                {
                    if (soldier.gameObject)
                    {
                        bool reached = false;

                        soldierCam.SetUserDesireAngleY(CameraUtil.approximateAngleFloat(soldierCam,m_userDesireAngle.y, soldier.gameObject.transform.eulerAngles.y, 0.5f, ref reached));
                    }

                }
                soldierCam.exitNavMode(CameraNavMode.Target); //其他操作中断自动旋转
                soldierCam.exitNavMode(CameraNavMode.FloatingAngle); //其他操作中断自动旋转
            }
        }

        public bool canCameraRotate()
        {
            if ((soldierController != null) && ((soldierController.bSkillingPauseMoveRotate) || (soldierController.bStopRotate)))
            {
                return false;
            }
            return true;
        }

        private bool canUserControlCamera()
        {
            //下面开始每帧update流程
            switch (curCameraMode)
            {
                case CameraMode.UserControl:
                case CameraMode.RidingControl:
                case CameraMode.FloatingControl:
                case CameraMode.FPSMode:
                case CameraMode.SniperscopeControl:
                case CameraMode.FollowTarget:
                case CameraMode.OBHeroLock:
#if ENABLE_YUNYING_MODE
                case CameraMode.YUNYING_OBHeroLock:
                case CameraMode.YUNYING_OBHeroTrace:
#endif
                    return true;
                default:
                    return false;
            }
        }

        /// <summary>
        /// 判断此模式是否可生效
        /// </summary>
        public abstract bool Enable(bool isInnerSwitch);

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        public abstract bool CheckContext(CameraContext context);
    }
}
