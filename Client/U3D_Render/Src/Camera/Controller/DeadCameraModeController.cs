/*******************************************************************
** 文件名:	DeadCameraModeController.cs
** 描  述:	摄像机死亡状态类(上帝视角分支)
** 应  用:  	
            处理摄像机死亡：
                1、指定对应的镜头参数camParam
                2、允许切换英雄，跟随英雄
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class DeadCameraModeController : GodBaseCameraController, ICameraModelController
    {
        //是否跟随目标(默认跟随，但自由查看上帝视野时不跟随)
        private bool m_bFollowTarget;

        public struct DeadCamContext
        {

        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<DeadCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.FPSMode);
            AllowChangeList.Add(CameraMode.UserControl);
        }

        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            SwitchTarget();
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
            return CameraMode.DeadControl;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool Enable(bool isInnerSwitch)
        {
            if (isInnerSwitch)
                return false;
            else if (soldierCam.isDead)
                return true;
            else
                return false;
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
            if (m_bFollowTarget)
            {
                if(soldierCam.soldier != null && soldierCam.soldier.gameObject)
                {
                    ChangeGodEyePos(soldierCam.soldier.gameObject.transform.position);
                }
                //if (soldierCam.mouseCtrl != null)
                //{
                //    if (soldierCam.mouseCtrl.cameraRotateButton && !UISystem.Instance.IsMouseOverUI())
                //    {
                //        soldierCam.mouseCtrl.RotateWithMouse = true;
                //        MouseCursorManager.Instance.MouseCursorVisible = true;
                //        Vector3 angle = GetGodEyeAngle();
                //        float newY = angle.y + soldierCam.mouseCtrl.fMouseAxisX * Time.deltaTime * 30;
                //        float newX = angle.x - soldierCam.mouseCtrl.fMouseAxisY * Time.deltaTime * 30;

                //        soldierCam.userDesireAngle.Set(newX, newY, angle.z);
                //        ChangeGodEyeAngle(soldierCam.userDesireAngle);
                //    }
                //}
            }
            base.OnLateUpdate();
        }

        /// <summary>
        /// 切换观察目标
        /// </summary>
        public void SwitchTarget()
        {
            float realAngle = 0;
            if(GetSceneAngleY(ref realAngle))   //根据地图和阵营获得对应的旋转角度Y轴
            {
                m_bFollowTarget = true;
                Vector3 godDir = new Vector3(0, 0, -1);
                EnterGodEyeMode(soldierCam.soldier.gameObject.transform.position, new Vector3(50.0f, realAngle, 0.0f), godDir, new Vector3(0, 0, 0), 35.0f, 30.0f, false);
            }
        }

        /// <summary>
        /// 切换上帝之眼目标，当玩家主动点击小地图，以上帝视野查看目标时，取消跟随队友
        /// </summary>
        /// <param name="pos"></param>
        public void SwitchGodEyePos(Vector3 pos)
        {
            m_bFollowTarget = false;
            ChangeGodEyePos(pos);
        }

    }
}
