using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class OBMoveFreeModeController : OBBaseCameraController, ICameraModelController
    {
        public class OBMoveFreeContext : CameraContext
        {
            public Vector3 pos;
            public Vector3 angle;
            public Vector3 dir;
            public Vector3 tran;
            public float distance;
            public float fov;
        }

        private float moveSpeed = 6;         //基础移动速度
        private Vector3 inputMoveDelta;      //输入数据
        private bool m_bRotateWithMouse;     //鼠标是否可以旋转

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<OBMoveFreeModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.UserControl);
            AllowChangeList.Add(CameraMode.FPSMode);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            if (context != null)
            {
                OBMoveFreeContext obContext = context as OBMoveFreeContext;
                GodEyeStateParam godEyeParam = new GodEyeStateParam(obContext.pos, obContext.angle, new Vector3(0, 0, -1), Vector3.zero, 8, 60);
                UpdateFocusPos(godEyeParam);
            }

            EnterOBMode();
            this.soldierController.SetStopMove(true);
            DataCenter.WarFrontSightDataManager.Instance.FrontSightVisible = false;
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            LeaveOBMode();
            this.soldierController.SetStopMove(false);
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.OBMoveFree;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if (soldierCam.isOBState)
                return true;

            return false;
        }

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool CheckContext(CameraContext context)
        {
            return true;
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
            base.OnUpdate();
        }

        public override void OnLateUpdate()
        {
            if (soldierController != null && CameraEffectUtil.MainInstance.canPlayControl)
            {
                GetInputNormally(curCameraModeParam.pitchAngleRange);
            }

            Vector3 cOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
            UpdateTransformOBMode(cOffset); //计算摄像机具体参数

            DisposeInputMoveDelta();

            soldierCam.target.SetPosition(camTransform.position);

            CalAndSendCameraFocusPos(camTransform.position);
            ChangeOBFocusPos(camTransform.position);
            UpdateMinimapFocusPos();
        }

        protected override bool canMouseRotateCamera()
        {
            return m_bRotateWithMouse;
        }

        //更新聚焦点
        public void UpdateFocusPos(GodEyeStateParam GodEyeParam) 
        {
            if (null == GodEyeParam)
                return;

            m_userDesirePos = GodEyeParam.pos;
            m_userDesireAngle = GodEyeParam.angle;
            m_userDesireDir = GodEyeParam.dir;
            m_userCurDistance = 8;

            UpdateCamera();
        }

        //刷新相机位置(切换聚焦目标)
        public void UpdateCamera()
        {
            Vector3 cPosOffset = new Vector3(0, curCameraModeParam.targetOffset, 0);
            //摄像机广角
            if (!CameraEffectUtil.MainInstance.m_effectLockFov)
            {
                m_userCurFov = m_userDesireFov;
            }
            apprFov = CameraUtil.approximateFloat(apprFov, m_userCurFov, apprFactor);
            float finalFov = apprFov + CameraEffectUtil.MainInstance.m_effectCurFov;
            if (finalFov > 150.0f) finalFov = 150.0f;
            m_Camera.fieldOfView = finalFov;

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
            apprDistance = CameraUtil.approximateFloat(apprDistance, m_userCurDistance, 1.0f);
            float finalDistance = apprDistance + CameraEffectUtil.MainInstance.m_effectCurDistance;
            if (finalDistance < 1.0f) finalDistance = 1.0f;

            //摄像机平移（相对朝向）
            if (!CameraEffectUtil.MainInstance.m_effectLockTran)
            {
                m_userCurTran = m_userDesireTran;
            }
            apprTran = CameraUtil.approximateVec(apprTran, m_userCurTran, 1.0f);

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

        //更新相机位置
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
            m_Camera.fieldOfView = finalFov;

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

        void DisposeInputMoveDelta()
        {
            float FinalMoveSpeed = moveSpeed + soldierCam.OBCtrlCamMoveSpeed;
            Vector2 moveDir = new Vector2(InputManager.GetAxis(InputAxisSet.Axis_Horizontal), InputManager.GetAxis(InputAxisSet.Axis_Vertical));
            if(moveDir == Vector2.zero)
            {
                //如果键盘无输入，再看看鼠标在窗口边界上的移动
                moveDir = GetMouseBoundaryMove();
            }
            inputMoveDelta = moveDir.x * camTransform.right + moveDir.y * new Vector3(camTransform.forward.x, 0, camTransform.forward.z);
            inputMoveDelta.Normalize();
            inputMoveDelta *= FinalMoveSpeed * Time.deltaTime;
            camTransform.position += inputMoveDelta;

            if (InputManager.IsMouseInScreen())
            {
                float fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
                fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
                if (Mathf.Abs(fMouseScrollValue) > 0.01f)
                {
                    if (fMouseScrollValue < 0)
                        fMouseScrollValue = -1;
                    else
                        fMouseScrollValue = 1;
                    camTransform.position += camTransform.forward * fMouseScrollValue * soldierCam.OBCtrlCamMoveSpeed;
                }
            }
        }

        void setFPS(bool enable)
        {
            m_bRotateWithMouse = enable;
            DataCenter.WarFrontSightDataManager.Instance.FrontSightVisible = enable;
            Cursor.lockState = enable ? CursorLockMode.Locked : CursorLockMode.None;
            if (soldierCam.mouseCtrl != null)
                soldierCam.mouseCtrl.RotateWithMouse = enable;
        }
    }
}
