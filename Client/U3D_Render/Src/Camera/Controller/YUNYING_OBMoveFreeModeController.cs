/*******************************************************************
** 文件名:	OBHeroTraceModeController.cs
** 描  述:	摄像机录屏英雄追踪状态类（市场部录屏专用）
** 应  用:  	
            跟FPSMode一致
            不允许任何状态自动切进来，只能利用特定接口
********************************************************************/
#if ENABLE_YUNYING_MODE
using DataCenter;
using UnityEngine;
using GameLogic;
namespace U3D_Render.CameraControl
{
    public class YUNYING_OBHeroMoveFreeModeController : BaseCameraController, ICameraModelController
    {


        private float moveSpeed = 5;
        private Vector3 inputMoveDelta;
        bool m_bRotateWithMouse;
        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<YUNYING_OBHeroMoveFreeModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
        }

        AudioListener al = null;
        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            ResetExternOBCameraInfo();
            this.soldierController.SetStopMove(true);
            this.soldierCam.target.gameObject.SetActive(false);
            if(!al)
            {
                al = this.soldierCam.CurrentCamera.gameObject.AddComponent<AudioListener>();
            }
            al.enabled = true;
            
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            this.soldierController.SetStopMove(false);
            this.soldierCam.target.gameObject.SetActive(true);
            al.enabled = false;
            ResetExternOBCameraInfo();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.YUNYING_OBMoveFree;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            return false;
        }

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool CheckContext(CameraContext context)
        {
            return false;
        }

        /// <summary>
        /// 更新相机数据
        /// </summary>
        /// <param name="context"></param>
        public override void UpdateData(CameraContext context)
        {

        }

        public override void OnUpdate()
        {

        }
        float GodEyeFocusPosCheckTime = 0.0f;
        public override void OnLateUpdate()
        {
            if(Input.GetKey(KeyCode.KeypadMinus))
            {
                ExternOBCameraMoveSpeed -= MoveSpeedUpdateDealt;
            }
            if (Input.GetKey(KeyCode.KeypadPlus))
            {
                ExternOBCameraMoveSpeed += MoveSpeedUpdateDealt;
            }

            if (Input.GetKey(KeyCode.PageDown))
            {
                ExternOBCamerFOV -= FOVUpdateDealt;
            }
            if (Input.GetKey(KeyCode.PageUp))
            {
                ExternOBCamerFOV += FOVUpdateDealt;
            }
            
            if (Input.GetKey(KeyCode.KeypadDivide))
            {
                ExternOBCameraRoateSpeed -= RoateSpeedUpdateDealt;
            }
            if (Input.GetKey(KeyCode.KeypadMultiply))
            {
                ExternOBCameraRoateSpeed += RoateSpeedUpdateDealt;
            }


            float FinalMoveSpeed = moveSpeed + ExternOBCameraMoveSpeed;
            float RoateSpeed = 30 + ExternOBCameraRoateSpeed;
            if (soldierController != null && CameraEffectUtil.MainInstance.canPlayControl)
            {
                setFPS(false);
                if (InputManager.GetKey(KeyCode.Keypad4))
                {
                    setFPS(true);
                    this.mouseCtrl.fMouseAxisX = -Time.deltaTime * RoateSpeed;
                }
                else if (InputManager.GetKey(KeyCode.Keypad6))
                {
                    setFPS(true);
                    this.mouseCtrl.fMouseAxisX = Time.deltaTime * RoateSpeed;
                }
                GetInputOBMode(curCameraModeParam.pitchAngleRange);
            }
            Vector3 cOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
            UpdateTransformOBMode(cOffset); //计算摄像机具体参数
            inputMoveDelta = InputManager.GetAxis(InputAxisSet.Axis_Horizontal)* camTransform.right + InputManager.GetAxis(InputAxisSet.Axis_Vertical)*camTransform.forward;
            inputMoveDelta.Normalize();
            inputMoveDelta *= FinalMoveSpeed * Time.deltaTime;
            camTransform.position += inputMoveDelta;
            if (InputManager.GetKey(KeyCode.Keypad8))
            {
                camTransform.position += Vector3.up * FinalMoveSpeed * Time.deltaTime / 3;
            }
            else if (InputManager.GetKey(KeyCode.Keypad2))
            {
                camTransform.position -= Vector3.up * FinalMoveSpeed * Time.deltaTime / 3;
            }
            if (InputManager.GetKeyDown(KeyCode.F1))
            {
                soldierCam.SwitchLookAtOBSolider(false);
                UpdateCamera(cOffset);
            }
            else if (InputManager.GetKeyDown(KeyCode.F2))
            {
                soldierCam.SwitchLookAtOBSolider(true);
                UpdateCamera(cOffset);
            }

            GodEyeFocusPosCheckTime += deltaTime;
            if (GodEyeFocusPosCheckTime > 1.0f)
            {
                cmd_entity_master_eye_focus data;
                data.isGodMode = 1;
                data.x = camTransform.position.x;
                data.y = camTransform.position.y;
                data.z = camTransform.position.z;

                IntPtrHelper helper = new IntPtrHelper();
                System.IntPtr ptr = helper.toPtr<cmd_entity_master_eye_focus>(ref data);
                int len = System.Runtime.InteropServices.Marshal.SizeOf(data);

                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_MASTER_EYE_FOCUS, len, "", ptr, len);

                GodEyeFocusPosCheckTime = 0;
            }

        }
        protected override bool canMouseRotateCamera()
        {
            return m_bRotateWithMouse;
        }

        public void setFPS(bool enable)
        {
            m_bRotateWithMouse = enable;
            WarFrontSightDataManager.Instance.FrontSightVisible = enable;
            Cursor.lockState = enable ? CursorLockMode.Locked : CursorLockMode.None;
            if (soldierCam.mouseCtrl != null)
                soldierCam.mouseCtrl.RotateWithMouse = enable;
        }

        void UpdateCamera(Vector3 cPosOffset)
        {

            //摄像机广角
            if (!CameraEffectUtil.MainInstance.m_effectLockFov)
            {
                m_userCurFov = m_userDesireFov;
            }
            apprFov = CameraUtil.approximateFloat(apprFov, m_userCurFov, apprFactor);
            float finalFov = apprFov + CameraEffectUtil.MainInstance.m_effectCurFov;
            if (finalFov > 150.0f) finalFov = 150.0f;
            m_Camera.fieldOfView = finalFov + ExternOBCamerFOV;

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

        private void GetInputOBMode(Vector2 pitchAngleRange)
        {
            GetInputNormally(pitchAngleRange);
        }

        void UpdateTransformOBMode(Vector3 cPosOffset)
        {

            //摄像机广角
            if (!CameraEffectUtil.MainInstance.m_effectLockFov)
            {
                m_userCurFov = m_userDesireFov;
            }
            apprFov = CameraUtil.approximateFloat(apprFov, m_userCurFov, apprFactor);
            float finalFov = apprFov + CameraEffectUtil.MainInstance.m_effectCurFov;
            if (finalFov > 150.0f) finalFov = 150.0f;
            m_Camera.fieldOfView = finalFov + ExternOBCamerFOV;

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
            camTransform.forward = -finalDir;
        }
    }
}
#endif
