/*******************************************************************
** 文件名:	MapGodCameraModeController.cs
** 描  述:	摄像机UI上帝视角状态类
** 应  用:  	
            处理UI上帝视角状态的摄像机：
                1、指定对应的镜头参数camParam
                2、允许拖动鼠标，允许旋转
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class MapGodCameraModeController : GodBaseCameraController, ICameraModelController
    {
        public class MapGodCamContext : CameraContext
        {
            public Vector3 pos;
            public Vector3 angle;
            public Vector3 dir;
            public Vector3 tran;
            public float distance;
            public float fov;
        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<MapGodCameraModeController>(GetCameraMode(), this);
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
            //直接结束
            if (context == null) {
                EnterGodEyeMode();
                return;
            }
            MapGodCamContext _context = context as MapGodCamContext;
            EnterGodEyeMode(_context.pos, _context.angle, _context.dir, _context.tran, _context.distance, _context.fov);
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            LeaveGodEyeMode();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.MapGodControl;
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

            if (isInnerSwitch)
                return false;
            else
                return true;
        }

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool CheckGodContext(CameraContext context)
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

        }

        public override void OnLateUpdate()
        {
            base.OnLateUpdate();
            if ((InputManager.GetKeyDown(KeyCode.W) || InputManager.GetKeyDown(KeyCode.S) || InputManager.GetKeyDown(KeyCode.A) || InputManager.GetKeyDown(KeyCode.D)
               || InputManager.GetKeyDown(KeyCode.UpArrow) || InputManager.GetKeyDown(KeyCode.DownArrow) || InputManager.GetKeyDown(KeyCode.LeftArrow) 
               || InputManager.GetKeyDown(KeyCode.RightArrow) || InputManager.GetKeyDown(KeyCode.Space)))
            {
                soldierCam.LeaveCurMode();
            }
        }
    }
}
