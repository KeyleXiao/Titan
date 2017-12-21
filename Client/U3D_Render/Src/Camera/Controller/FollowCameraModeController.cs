/*******************************************************************
** 文件名:	FollowCameraModeController.cs
** 描  述:	摄像机跟随状态类
** 应  用:  	
            处理跟随状态的摄像机：
                1、指定对应的镜头参数camParam
********************************************************************/

namespace U3D_Render.CameraControl
{
    /// <summary>
    /// 暂时没用
    /// </summary>
    public class FollowCameraModeController : BaseCameraController, ICameraModelController
    {



        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<FollowCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.CameraAnim);
            AllowChangeList.Add(CameraMode.DeadControl);
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.FPSMode);
            AllowChangeList.Add(CameraMode.UserControl);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            SendCameraFocusPosToLogic(0);
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
            return CameraMode.FollowTarget;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if (isInnerSwitch)
                return false;
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
        }
    }
}
