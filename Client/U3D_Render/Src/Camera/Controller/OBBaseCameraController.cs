using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using USpeedUI;


namespace U3D_Render.CameraControl
{
    public abstract class OBBaseCameraController : BaseCameraController
    {
        public delegate void SwitchOBModeHanlder(bool bOBMode, Vector3 vPos);
        public static event SwitchOBModeHanlder SwitchOBModeEvent;

        protected Vector3 obFocusPos = new Vector3(0, 0, 0);

        public void ChangeOBFocusPos(Vector3 pos)
        {
            obFocusPos = pos;
        }

        public void EnterOBMode()
        {
            obFocusPos = cPos;

            if (SwitchOBModeEvent != null)
            {
                SwitchOBModeEvent(true, obFocusPos);
            }
        }

        public void EnterOBMode(Vector3 pos)
        {
            obFocusPos = pos;

            if (SwitchOBModeEvent != null)
            {
                SwitchOBModeEvent(true, obFocusPos);
            }
        }

        public void LeaveOBMode()
        {
            if (SwitchOBModeEvent != null)
            {
                SwitchOBModeEvent(false, new Vector3());
            }
        }

        // 小地图上帝之眼（矩形框）
        public void UpdateMinimapFocusPos()
        {
            if (SwitchOBModeEvent != null)
            {
                SwitchOBModeEvent(true, obFocusPos);
            }
        }

        public override void OnUpdate()
        {
            //UpdateFocusEntity();
        }

        public override void OnLateUpdate()
        {
            base.OnLateUpdate();
        }

        protected void UpdateFocusEntity()
        {
            if (InputManager.GetKeyUp(KeyCode.Mouse0) && !UnityEngine.EventSystems.EventSystem.current.IsPointerOverGameObject()) //左键切换下一个队友        
            {
                if (!UISystem.Instance.IsMouseOverUI())
                {
                    U3D_Render.EntityView ev = DataCenter.LogicDataCenter.warOBDataManager.GetRondomEntityView();
                    if (!U3D_Render.EntityView.isNullOrEmpty(ev))
                    {
                        soldierCam.SwitchOBSoliderTarget(ev);
                        if (soldierCam.cameraMode != CameraMode.OBHeroLock)
                        {
                            soldierCam.EnableOBMode(true);
                        }
                    }
                }
            }
        }

        float marginDetal = 0;
        /// <summary>
        /// 获取鼠标在窗口边界上的移动
        /// </summary>
        /// <returns></returns>
        protected Vector2 GetMouseBoundaryMove()
        {
            Vector2 moveDir = Vector2.zero;
            //if (!InputManager.IsMouseInScreen())
            //    return moveDir;

            Vector3 mousePos = InputManager.MousePosition;

            if (mousePos.y < marginDetal)
                moveDir.y = -1;
            if (mousePos.y > Screen.height - marginDetal)
                moveDir.y = 1;

            if (mousePos.x < marginDetal)
                moveDir.x = -1;
            if (mousePos.x > Screen.width - marginDetal)
                moveDir.x = 1;

            return moveDir;
        }

        float GodEyeFocusPosCheckTime = 0.0f;
        //上报聚焦位置
        protected void CalAndSendCameraFocusPos(Vector3 focusPos)
        {
            GodEyeFocusPosCheckTime += deltaTime;
            if (GodEyeFocusPosCheckTime > 1.0f)
            {
                GameLogic.cmd_entity_master_eye_focus data;
                data.isGodMode = 1;
                data.x = focusPos.x;
                data.y = focusPos.y;
                data.z = focusPos.z;

                IntPtrHelper helper = new IntPtrHelper();
                System.IntPtr ptr = helper.toPtr<GameLogic.cmd_entity_master_eye_focus>(ref data);
                int len = System.Runtime.InteropServices.Marshal.SizeOf(data);

                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)GameLogic.EntityLogicDef.ENTITY_CMD_MASTER_EYE_FOCUS, len, "", ptr, len);

                GodEyeFocusPosCheckTime = 0;
            }
        }
    }
}
