/*******************************************************************
** 文件名:	OBHeroLockModeController.cs
** 描  述:	摄像机录屏英雄锁定状态类（市场部录屏专用）
** 应  用:  	
            跟UserMode一致
            不允许任何状态自动切进来，只能利用特定接口
********************************************************************/
#if ENABLE_YUNYING_MODE
using UnityEngine;

namespace U3D_Render.CameraControl
{
    /// <summary>
    /// 暂时没用
    /// </summary>
    public class YUNYING_OBHeroLockModeController : BaseCameraController, ICameraModelController
    {
        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<YUNYING_OBHeroLockModeController>(GetCameraMode(), this);
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
            SetupByCamParam();

            DataCenter.WarFrontSightDataManager.Instance.FrontSightVisible = false;
            MouseCursorManager.Instance.MouseCursorVisible = true;
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {

        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.YUNYING_OBHeroLock;
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
            base.OnLateUpdate();
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