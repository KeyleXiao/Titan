/*******************************************************************
** 文件名:	MapGodCameraModeController.cs
** 描  述:	摄像机骑乘状态类
** 应  用:  	
            处理骑乘状态的摄像机（龙母）：
                1、指定对应的镜头参数camParam
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class RidingCameraModeController:BaseCameraController,ICameraModelController
    {

        private Vector3 m_userDesireAngleRec;
        private float m_userDesireDistanceRec;


        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<RidingCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.DeadControl);
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.UserControl);
        }


        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            SendCameraFocusPosToLogic(0);
            EnterRidingMode();
            SetupByCamParam();

        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            LeaveRidingMode();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.RidingControl;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if (soldierCam.isDead)
                return false;
            if (!soldierCam.isFloating) //骑乘也属于飞行状态
                return false;
            if (soldierCam.isFps) //目前fps英雄没有骑乘状态
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

        }

        public override void OnLateUpdate()
        {
            base.OnLateUpdate();
        }

        private void EnterRidingMode()
        {
            m_userDesireAngleRec = m_userDesireAngle;
            m_userDesireDistanceRec = 0.0f;

            Vector3 angle = m_userDesireAngle;
            angle.x = soldierCam.curCameraModeParam.curPitchAngle;
            angle.x = Mathf.Clamp(angle.x, soldierCam.curCameraModeParam.pitchAngleRange.x, soldierCam.curCameraModeParam.pitchAngleRange.y);

            m_userDesireDistanceRec = m_userDesireDistance; //m_userDesireDistanceRec>0时，表示正在实行技能浮空的指定摄像机距离，m_userDesireDistance不可调
            m_userDesireDistance = soldierCam.curCameraModeParam.curDistance;

            soldierCam.enterNavMode(CameraNavMode.FloatingAngle, null, angle);
        }

        private void LeaveRidingMode()
        {
            Vector3 angle = m_userDesireAngle;
            angle.x = m_userDesireAngleRec.x;

            if (m_userDesireDistanceRec > 0.0f)
            {
                m_userDesireDistance = m_userDesireDistanceRec;
                m_userDesireDistanceRec = 0.0f; //m_userDesireDistanceRec>0时，表示正在实行技能浮空的指定摄像机距离，m_userDesireDistance不可调
            }

            soldierCam.enterNavMode(CameraNavMode.FloatingAngle, null, angle); //调整angle，利用enterNavMode将镜头导航逐渐恢复到m_userDesireAngleRec
        }
    }
}
