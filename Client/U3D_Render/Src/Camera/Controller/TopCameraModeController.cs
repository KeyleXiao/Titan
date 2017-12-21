/*******************************************************************
** 文件名:	TopCameraModeController.cs
** 描  述:	摄像机顶视角状态类
** 应  用:  	
            处理顶视角状态的摄像机：
                1、指定对应的镜头参数camParam
                2、开启旋转
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    class TopCameraModeController :GodBaseCameraController, ICameraModelController
    {
        private const float GOD_EYE_TIME = 10.0f;
        private float godEyeStartTime;  //顶视角开始时间
        public struct TopCamContext
        {
        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<TopCameraModeController>(GetCameraMode(), this);
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
            this.godEyeStartTime = Time.time;
            SwitchGodEye(soldierCam.soldier);
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
            return CameraMode.TopControl;
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
            if (soldierCam.isFps)
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
            if (Time.time - godEyeStartTime >= GOD_EYE_TIME)
            {
                soldierCam.LeaveCurMode();
            }
            else if (Time.time - godEyeStartTime < GOD_EYE_TIME)
            {
                ChangeGodEyePos(soldierCam.soldier.gameObject.transform.position);
                if (soldierCam.mouseCtrl && soldierCam.mouseCtrl.cameraRotateButton)
                {
                    soldierCam.mouseCtrl.RotateWithMouse = true;
                    MouseCursorManager.Instance.MouseCursorVisible = true;
                    Vector3 angle = GetGodEyeAngle();
                    float newY = angle.y + soldierCam.mouseCtrl.fMouseAxisX * Time.deltaTime * 30;
                    float newX = angle.x - soldierCam.mouseCtrl.fMouseAxisY * Time.deltaTime * 30;

                    m_userDesireAngle = new Vector3(newX, newY, angle.z);
                    ChangeGodEyeAngle(m_userDesireAngle);
                }
            }
        }
        

        private void SwitchGodEye(U3D_Render.EntityView theSoldier)
        {
            Vector3 godDir = new Vector3(0, 0, -1);
            float realAngle =(float)m_userDesireAngle.y; 
            EnterGodEyeMode(theSoldier.gameObject.transform.position, new Vector3(50.0f, realAngle, 0.0f), godDir, new Vector3(0, 0, 0), 35.0f, 30.0f, false);
        }
    }
}
