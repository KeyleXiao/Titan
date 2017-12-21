/*******************************************************************
** 文件名:	SniperscopeCameraModeController.cs
** 描  述:	摄像机狙击状态类
** 应  用:  	
            处理狙击状态的摄像机：
                1、指定对应的镜头参数camParam
                2、开启狙击镜UI
********************************************************************/
using System;
using UnityEngine;
using USpeedUI;

namespace U3D_Render.CameraControl
{
    public class SniperscopeCameraModeController : BaseCameraController, ICameraModelController
    {
        private Transform m_sniperscopreWeapon = null;

        //unuseful
        private Vector3 m_fSniperscopeAngleOffset = Vector3.zero;
        private float m_fSniperScopreFovOffset = 0.0f;


        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<SniperscopeCameraModeController>(GetCameraMode(), this);
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
            OpenSniperscope();
            SetupByCamParam();
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            CloseSniperscope();
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.SniperscopeControl;
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

        protected override bool canMouseRotateCamera()
        {
            return true;
        }

        //开启狙击镜
        private void OpenSniperscope()
        {
            m_sniperscopreWeapon = null;
            if (EntityFactory.MainHeroView != null)
            {
                if (EntityFactory.MainHeroView.SkinControl != null)
                {
                    if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance != null)
                    {
                        if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig != null)
                        {
                            if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender != null)
                            {
                                EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender.enabled = false;
                                CreatureStateMachine state = EntityFactory.MainHeroView.StateMachine as CreatureStateMachine;
                                if (state != null)
                                {
                                    m_sniperscopreWeapon = state.SkinConfig.weaponObject;
                                }
                            }
                        }
                    }
                }
            }
            if (soldierCam.mouseCtrl != null && soldierCam.mouseCtrl.RotateWithMouse != true)
            {
                soldierCam.mouseCtrl.RotateWithMouse = true;
                Cursor.lockState = CursorLockMode.Locked;

            }
            UISystem.Instance.SendWndMessage(
                WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,
                new UIMsgCmdData((int)WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,
                Convert.ToInt32(true), string.Empty, IntPtr.Zero, 0));
        }

        //关闭狙击镜
        private void CloseSniperscope()
        {
            if (EntityFactory.MainHeroView.SkinControl != null)
            {
                if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance != null)
                {
                    if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig != null)
                    {
                        if (EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender != null)
                        {
                            EntityFactory.MainHeroView.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender.enabled = true;
                        }
                    }
                }
            }
            if (soldierCam.mouseCtrl != null && soldierCam.mouseCtrl.RotateWithMouse != false)
            {
                soldierCam.mouseCtrl.RotateWithMouse = false;
                Cursor.lockState = CursorLockMode.None;

            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY,
        new UIMsgCmdData((int)WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, Convert.ToInt32(false), string.Empty, IntPtr.Zero, 0));

        }
    }
}
