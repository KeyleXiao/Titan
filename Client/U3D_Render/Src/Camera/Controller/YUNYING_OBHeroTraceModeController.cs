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

namespace U3D_Render.CameraControl
{
    public class YUNYING_OBHeroTraceModeController : BaseCameraController, ICameraModelController
    {

        public bool m_bRotateWithMouse = true;

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<YUNYING_OBHeroTraceModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            SendCameraFocusPosToLogic(0);
            setFPS(true);
            SetupByCamParam();
            ResetExternOBCameraInfo();
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            ResetExternOBCameraInfo();
            setFPS(false);
            
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.YUNYING_OBHeroTrace;
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

        public override void OnLateUpdate()
        {
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

            float RoateSpeed = 30 + ExternOBCameraRoateSpeed;
            if (InputManager.GetKey(KeyCode.Keypad4))
            {
                this.mouseCtrl.fMouseAxisX = -Time.deltaTime * RoateSpeed;
            }
            else if (InputManager.GetKey(KeyCode.Keypad6))
            {
                this.mouseCtrl.fMouseAxisX = Time.deltaTime * RoateSpeed;
            }
            if (InputManager.GetKey(KeyCode.Keypad2))
            {
                this.mouseCtrl.fMouseAxisY = -Time.deltaTime * RoateSpeed;
            }
            else if (InputManager.GetKey(KeyCode.Keypad8))
            {
                this.mouseCtrl.fMouseAxisY = Time.deltaTime * RoateSpeed;
            }
            else if (InputManager.GetKey(KeyCode.Keypad1))
            {
                BaseCameraController.ExternTargetOffset += 0.01f;
            }
            else if (InputManager.GetKey(KeyCode.Keypad3))
            {
                BaseCameraController.ExternTargetOffset -= 0.01f;
            }
            base.OnLateUpdate();
            updateFPS();
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

        private void updateFPS()
        {
            if (InputManager.Raw_GetKey(KeyCode.LeftAlt) || InputManager.Raw_GetKey(KeyCode.RightAlt) || InputManager.Raw_GetKey(KeyCode.LeftControl) || InputManager.Raw_GetKey(KeyCode.RightControl) || soldierCam.bFPSShowCursor)
            {

                m_bRotateWithMouse = false;
            }
            else
            {
                m_bRotateWithMouse = true;
            }
            if (InputManager.GetKeyDown(KeyCode.F1))
            {
                soldierCam.SwitchLookAtOBSolider(false);
            }
            else if (InputManager.GetKeyDown(KeyCode.F2))
            {
                soldierCam.SwitchLookAtOBSolider(true);
            }
        }
    }
}

#endif