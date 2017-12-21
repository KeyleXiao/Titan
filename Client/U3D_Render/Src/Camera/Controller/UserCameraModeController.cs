/*******************************************************************
** 文件名:	UserCameraModeController.cs
** 描  述:	摄像机第三人称用户状态类
** 应  用:  	
            处理第三人称的摄像机：
                1、指定对应的镜头参数camParam
********************************************************************/

namespace U3D_Render.CameraControl
{
    public class UserCameraModeController : BaseCameraController, ICameraModelController
    {


        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<UserCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.CameraAnim);
            AllowChangeList.Add(CameraMode.DeadControl);
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.FloatingControl);
            AllowChangeList.Add(CameraMode.RidingControl);
            AllowChangeList.Add(CameraMode.TopControl);
            AllowChangeList.Add(CameraMode.FollowTarget);
            AllowChangeList.Add(CameraMode.MapGodControl);
            AllowChangeList.Add(CameraMode.FPSMode);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context) {
            SendCameraFocusPosToLogic(0);
            SetupByCamParam();

            DataCenter.WarFrontSightDataManager.Instance.FrontSightVisible = false;
            MouseCursorManager.Instance.MouseCursorVisible = true;
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave(){

        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode(){
            return CameraMode.UserControl;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            //吃鸡地图死亡后可以User模式跟随
            if (soldierCam.isSurvivalMode && soldierCam.isDead)
                return true;

            if (soldierCam.isDead)
                return false;
            if (soldierCam.isFloating)
                return false;
            if (soldierCam.isFps)
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
        public override void UpdateData(CameraContext context){

        }

        public override void OnUpdate(){

        }

        public override void OnLateUpdate(){
            base.OnLateUpdate();
        }


    }
}
