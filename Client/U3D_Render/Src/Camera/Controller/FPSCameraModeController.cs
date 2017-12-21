/*******************************************************************
** 文件名:	FPSCameraModeController.cs
** 描  述:	摄像机第一人称视角状态类
** 应  用:  	
            处理FPS状态的摄像机：
                1、指定对应的镜头参数camParam
                2、锁鼠标
********************************************************************/
using DataCenter;
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class FPSCameraModeController : BaseCameraController,ICameraModelController
    {

        public bool m_bRotateWithMouse = true;

        public struct FPSCamContext
        {

        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<FPSCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.CameraAnim);
            AllowChangeList.Add(CameraMode.MapGodControl);
            AllowChangeList.Add(CameraMode.DeadControl);
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.SniperscopeControl);
            AllowChangeList.Add(CameraMode.RidingControl);
            AllowChangeList.Add(CameraMode.FloatingControl);
            AllowChangeList.Add(CameraMode.UserControl);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            SendCameraFocusPosToLogic(0);
            setFPS(true);
            SetupByCamParam();

        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            setFPS(false);
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.FPSMode;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if (soldierCam.isDead)
                return false;
            if (soldierCam.isFloating)
                return false;
            if (!soldierCam.isFps)
                return false;
            if (isInnerSwitch)
                return true;
            else
                return true;
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

        public override  void OnUpdate()
        {

        }

        public override void OnLateUpdate()
        {
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
        }
    }
}
