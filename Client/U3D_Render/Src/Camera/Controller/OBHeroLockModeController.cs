using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class OBHeroLockModeController : OBBaseCameraController, ICameraModelController
    {
        private float m_defaultScrollSpeed = 100.0f;

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<OBHeroLockModeController>(GetCameraMode(), this);
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
            SetupByCamParam();
            m_defaultScrollSpeed = CurCameraProperty.ScrollSpeed;

            curCameraModeParam.distanceRange = new Vector2(5, 30);
            CurCameraProperty.ScrollSpeed = soldierCam.OBCtrlMouseScrollSpeed;

            EnterOBMode();

            DataCenter.WarFrontSightDataManager.Instance.FrontSightVisible = false;
            MouseCursorManager.Instance.MouseCursorVisible = true;
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            CurCameraProperty.ScrollSpeed = m_defaultScrollSpeed;

            LeaveOBMode();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.OBHeroLock;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if(soldierCam.isOBState)
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
            if (Mathf.Abs(InputManager.GetAxis(InputAxisSet.Axis_Horizontal)) > 0.01f || Mathf.Abs(InputManager.GetAxis(InputAxisSet.Axis_Vertical)) > 0.01f)
            {
                soldierCam.EnableMode(CameraMode.OBMoveFree, true);
            }
        }

        public override void OnLateUpdate()
        {
            CurCameraProperty.ScrollSpeed = soldierCam.OBCtrlMouseScrollSpeed;

            base.OnLateUpdate();
            CalAndSendCameraFocusPos(cPos);

            ChangeOBFocusPos(cPos);
            UpdateMinimapFocusPos();
        }
    }
}
