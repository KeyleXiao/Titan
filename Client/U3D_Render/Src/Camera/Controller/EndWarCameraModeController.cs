/*******************************************************************
** 文件名:	EndWarCameraModeController.cs
** 描  述:	摄像机结束战场状态类（上帝模式的一种分支）
** 应  用:  	
            处理摄像机结束战场：
                1、通过插值移动摄像机
                2、不同角色的摄像机移动速度一致
                3、指定对应的镜头参数camParam
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class EndWarCameraModeController : GodBaseCameraController, ICameraModelController
    {
        private const float ENDWAR_GOD_DISTANCE = 30;
        private const float MOVE_TIME = 8;
        private float moveFactor = 0.02f;
        private float oldGodDistance;
        private bool isArrive = false;
        public class EndWarCamContext : CameraContext
        {
            public Vector3 curPos = Vector3.zero;
            public Vector3 targetPos = Vector3.zero;
        }

        private Vector3 curPos = Vector3.zero;
        private Vector3 targetPos = Vector3.zero;

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<EndWarCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.FPSMode);
            AllowChangeList.Add(CameraMode.UserControl);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            isArrive = false;
            EndWarCamContext _context = context as EndWarCamContext;
            this.targetPos = _context.targetPos;
            this.curPos = _context.curPos;
            float realAngle = 0;
            if (GetSceneAngleY(ref realAngle))   //根据地图和阵营获得对应的旋转角度Y轴
            {
                Vector3 godDir = new Vector3(0, 0, -1);
                oldGodDistance = tmpGodDistanceMax;
                tmpGodDistanceMax = ENDWAR_GOD_DISTANCE;
                //如果输了，镜头翻转，否则会被高台挡住基地爆炸
                if (!soldierCam.isWin)
                    realAngle += 180;
                EnterGodEyeMode(_context.curPos, new Vector3(50.0f, realAngle, 0.0f), godDir, new Vector3(0, 0, 0), 100.0f, 30.0f, false);
            }
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            isArrive = true;
            tmpGodDistanceMax = oldGodDistance;
            LeaveGodEyeMode();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.EndWarControl;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
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
            return context != null;
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
            if(isArrive)
                return;

            //移动摄像机
            if (Vector3.Distance(curPos, targetPos) < 0.1f)
            {
                this.curPos = this.targetPos;
                isArrive = true;
            }
            else
            {
                this.curPos = Vector3.Lerp(this.curPos, this.targetPos, moveFactor);
            }
            ChangeGodEyePos(this.curPos);
            base.OnLateUpdate();
        }
    }
}
