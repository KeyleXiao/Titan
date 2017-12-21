/*******************************************************************
** 文件名:	GodBaseCameraController.cs
** 描  述:	摄像机上帝视角状态基类
** 应  用:  	
            处理上帝视角状态的摄像机的基本操作：
                1、GetInputGodEye：处于用户输入，如旋转，拖动
                2、UpdateTransformByGodEye：更新不同上帝视角参数的摄像机位置，旋转，fov
                3、GetSceneAngleY：读取配置，获得指定场景的俯视角
********************************************************************/
using DataCenter;
using GameLogic;
using System;
using UnityEngine;
using USpeedUI;

namespace U3D_Render.CameraControl
{
    public abstract class GodBaseCameraController: BaseCameraController
    {
        public delegate void SwitchGodEyeModeHanlder(bool bGodEyeMode, Vector3 vPos);
        public static event SwitchGodEyeModeHanlder SwitchGodEyeModeEvent;



        //上帝之眼镜头模式参数
        protected float GodFov = 30.0f;
        protected Vector3 GodAngle = new Vector3(60, 0, 0);
        protected Vector3 GodDir = new Vector3(0, 0, -1);
        protected float GodDistance = 45.0f;
        protected Vector3 GodTran = new Vector3(0, 0, 0);
        protected Vector3 GodPos = new Vector3(0, 0, 0);
        protected bool GodEyeLongLock = false;
        protected bool GodEyeShortLock = false;

        private const float GodEyeFocusPosMaxCheckTime = 5.0f;
        private float GodEyeFocusPosCheckTime = GodEyeFocusPosMaxCheckTime + 1;

        public float GodTranThre1 = 0.01f;
        public float GodTranThre2 = 0.01f;
        private float GodTranSpeed = 20.0f;
        private float GodTranAcc = 1.0f;
        private float GodTranAccFactor = 20.0f;
        private float GodTranSpeedMin = 40.0f;
        private float GodTranSpeedMax = 50.0f;
        private float tmpGodAngleMin = 45.0f;
        private float tmpGodAngleMax = 45.0f;
        private float tmpGodDistanceMin = 5.0f;
        protected float tmpGodDistanceMax = 50.0f;

        public void ChangeGodEyePos(Vector3 pos)
        {
            GodPos = pos;
        }
        public void ChangeGodEyeAngle(Vector3 angle)
        {
            GodAngle = angle;
        }
        public void ChangeGodEyeFov(float fov)
        {
            GodFov = fov;
        }
        public void ChangeGodEyeDistance(float distance)
        {
            GodDistance = distance;
        }
        public void ChangeGodEyeDir(Vector3 dir)
        {
            GodDir = dir;
        }

        public void ChangeGodTran(Vector3 tran)
        {
            GodTran = tran;
        }
        public Vector3 GetGodEyePos()
        {
            return GodPos;
        }
        public Vector3 GetGodEyeAngle()
        {
            return GodAngle;
        }
        public float GetGodEyeFov()
        {
            return GodFov;
        }
        public float GetGodEyeDistance()
        {
            return GodDistance;
        }
        public Vector3 GetGodEyeDir()
        {
            return GodDir;
        }
        public Vector3 GetGodEyeTran()
        {
            return GodTran;
        }

        public void UpdateGodParams(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov)
        {
            ChangeGodEyePos(pos);
            ChangeGodEyeAngle(angle);
            ChangeGodEyeDir(dir);
            ChangeGodTran(tran);
            ChangeGodEyeDistance(distance);
            ChangeGodEyeFov(fov);
        }

        public void EnterGodEyeMode()
        {
            //关闭景深
            ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();

            //限制鼠标在窗口范围内
            //Initialize.bIsLockInWindow = true;
            // 通知小地图enter上帝视角模式
            if (SwitchGodEyeModeEvent != null)
            {
                SwitchGodEyeModeEvent(true, new Vector3());
            }
        }

        public void EnterGodEyeMode(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov, bool isMiniMap = true)
        {
            GodPos = pos;
            GodAngle = angle;
            GodDir = dir;
            GodTran = tran;
            GodDistance = distance;
            GodFov = fov;
            //关闭景深
            ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();

            //限制鼠标在窗口范围内
            //Initialize.bIsLockInWindow = true;
            // 通知小地图enter上帝视角模式
            if (SwitchGodEyeModeEvent != null)
            {
                SwitchGodEyeModeEvent(true, new Vector3());
            }
        }

        //离开上帝之眼模式
        public void LeaveGodEyeMode()
        {
            GodEyeFocusPosCheckTime = GodEyeFocusPosMaxCheckTime + 1;   
            //解除限制
            //Initialize.bIsLockInWindow = false;
            //开启景深
            ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.DepthOfFieldScatter>();

            // 通知小地图离开上帝视角模式
            if (SwitchGodEyeModeEvent != null)
            {
                SwitchGodEyeModeEvent(false, new Vector3());
            }
        }

        public override void OnLateUpdate()
        {
            UpdateStateByGodEye();
            if (curCameraMode != CameraMode.TopControl)
            {
                GetInputGodEye();
            }
            UpdateTransformByGodEye(); //计算摄像机具体参数
            UpdateMinimapGodEye();  // 小地图画框
            CalGodEyeCameraFocusPos();
        }

        private void UpdateStateByGodEye()
        {
            if (!soldier.gameObject)
            {
                return;
            }
            if (InputManager.GetKey(KeyCode.Space)) //该功能会被强制切换上帝视角的功能屏蔽掉，但是与策划商量，先不管
            {
                GodEyeShortLock = true;
            }
            else
            {
                GodEyeShortLock = false;
            }

            if (InputManager.GetKeyUp(KeyCode.F9))
            {
                GodEyeLongLock = !GodEyeLongLock;
            }

            if (GodEyeLongLock || GodEyeShortLock)
            {
                GodPos = soldier.gameObject.transform.position;
                GodTranSpeed = 0.0f;
            }


        }

        private void GetInputGodEye()
        {
            if (mouseCtrl == null)
            {
                return;
            }

            bool bShift = false;//InputManager.GetKey(KeyCode.LeftShift) || InputManager.GetKey(KeyCode.RightShift);

            float MouseZ = 0.0f;
            if (InputManager.IsMouseInScreen())
            {
                MouseZ = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
            }
            float scrollValue = MouseZ * CurCameraProperty.ScrollSpeed * 0.1f;
            if (bShift)
            {
                scrollValue *= 5.0f;
            }

            //处理角度
            //GodAngle.x -= scrollValue*2.0f;
            ////策划不需要更改角度
            //GodAngle.x = Mathf.Clamp(GodAngle.x, tmpGodAngleMin, tmpGodAngleMax);

            GodDistance -= scrollValue;
            GodDistance = Mathf.Clamp(GodDistance, tmpGodDistanceMin, tmpGodDistanceMax);

            Vector3 dirforward = m_Camera.transform.forward;
            Vector3 dirright = m_Camera.transform.right;
            dirforward.y = 0.0f;
            dirright.y = 0.0f;
            dirforward.Normalize();
            dirright.Normalize();

            //GodPos += dirforward * _y;
            //GodPos += dirright * _x;

            Vector2 screenPos = InputManager.MousePosition;
            Vector2 percentPos;
            percentPos.x = screenPos.x / Screen.width;
            percentPos.y = screenPos.y / Screen.height;


            float deltatime = Time.deltaTime;
            float deltavalue = Mathf.Clamp(deltatime, 0.0f, 0.1f) * GodTranSpeed;
            if (!GodEyeLongLock && !GodEyeShortLock)
            {
                if ((percentPos.x > -GodTranThre2 && percentPos.x < GodTranThre1)
                    || (percentPos.x > 1.0f - GodTranThre1 && percentPos.x < 1.0f + GodTranThre2)
                    || (percentPos.y > -GodTranThre2 && percentPos.y < GodTranThre1)
                    || (percentPos.y > 1.0f - GodTranThre1 && percentPos.y < 1.0f + GodTranThre2))
                {
                    GodTranAcc = deltatime;
                }
                else
                {
                    GodTranAcc = -deltatime;
                }
                GodTranSpeed += GodTranAcc * GodTranAccFactor;
            }
            GodTranSpeed = Mathf.Clamp(GodTranSpeed, GodTranSpeedMin, GodTranSpeedMax);

            if (bShift)
            {
                deltavalue *= 5.0f;
            }

            deltavalue = Mathf.Log(GodDistance) - 1.5f;

            if (!GodEyeLongLock && !GodEyeShortLock)
            {
                Vector3 tmpPos = Vector3.zero;

                if (percentPos.x > -GodTranThre2 && percentPos.x < GodTranThre1)
                {
                    tmpPos -= dirright * deltavalue;
                }
                else if (percentPos.x > 1.0f - GodTranThre1 && percentPos.x < 1.0f + GodTranThre2)
                {
                    tmpPos += dirright * deltavalue;
                }

                if (percentPos.y > -GodTranThre2 && percentPos.y < GodTranThre1)
                {
                    tmpPos -= dirforward * deltavalue;
                }
                else if (percentPos.y > 1.0f - GodTranThre1 && percentPos.y < 1.0f + GodTranThre2)
                {
                    tmpPos += dirforward * deltavalue;
                }

                float percentGodPosX, percentGodPosY, percentValue;
                percentValue = 0.45f;
                Rect sceneRect = LogicDataCenter.warMinimapDataManager.SceneRect;
                if (sceneRect.width == 0)
                    sceneRect.width = 1;
                if (sceneRect.height == 0)
                    sceneRect.height = 1;
                percentGodPosX = ((GodPos + tmpPos).x - sceneRect.xMin) / sceneRect.width - percentValue;
                //这里Y坐标有翻转，所以乘个-1
                percentGodPosY = ((GodPos + tmpPos).z - sceneRect.yMin) * (-1) / sceneRect.height - percentValue;
                if (percentGodPosX > percentValue)
                    tmpPos.x = 0;
                else if (percentGodPosX < -(percentValue - 0.1))
                    tmpPos.x = 0;

                if (percentGodPosY > percentValue)
                    tmpPos.z = 0;
                else if (percentGodPosY < -(percentValue - 0.1))
                    tmpPos.z = 0;

                GodPos += tmpPos;
                //策划说上帝模式下不要旋转摄像机
                //if (mouseCtrl.isCameraCtrl())
                //{
                //    float _x = mouseCtrl.fMouseAxisX;
                //    float _y = mouseCtrl.fMouseAxisY;

                //    GodAngle.y += _x;
                //    //GodAngle.x += _y;
                //}
            }

        }

        //上帝之眼模式更新镜头参数
        private void UpdateTransformByGodEye()
        {
            //摄像机广角
            apprFov = CameraUtil.approximateFloat(apprFov, GodFov, apprFactor);
            float finalFov = apprFov;
            if (finalFov > 150.0f) finalFov = 150.0f;
            m_Camera.fieldOfView = finalFov;

            //根据角度计算坐标基
            apprAngle = CameraUtil.approximateAngleVec(apprAngle, GodAngle, 1.0f);
            Vector3 finalAngle = apprAngle;
            Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(finalAngle), Vector3.one);

            //摄像机方向
            Vector3 finaldir = basem * GodDir;
            finaldir = finaldir.normalized;
            apprDir = CameraUtil.approximateVec(apprDir, finaldir, 1.0f);
            Vector3 finalDir = apprDir.normalized;

            //摄像机深度
            apprDistance = CameraUtil.approximateFloat(apprDistance, tmpGodDistanceMax, apprFactor);
            float finalDistance = apprDistance;
            if (finalDistance < 1.0f) finalDistance = 1.0f;

            //摄像机平移（相对朝向）
            apprTran = CameraUtil.approximateVec(apprTran, GodTran, apprFactor);
            Vector3 finalTran = apprTran;

            //摄像机位置（绝对世界）
            apprPos = CameraUtil.approximateVec(apprPos, GodPos, apprFactor);
            Vector3 finalPos = apprPos;

            //摄像机目标点位置
            Vector3 transformedtran = basem * finalTran;
            cPos = finalPos + transformedtran;

            //设置摄像机

            //刚在小地图点击/拖动，触发上帝视角时，需要根据地面调整摄像机距离

            //Ray ray = new Ray(cPos + (finalDir * finalDistance), -finalDir);
            //RaycastHit hitInfo;
            ////计算离地面的距离，然后调整
            //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
            //bool hit = Physics.Raycast(ray, out hitInfo, 200.0f, LayerMaskOnGround);
            //if (hit)
            //{
            //    cPos = hitInfo.point;
            //}

            //先拉高目标点，再垂直向下投影，避免目标点在地表下
            cPos.y += 1000;
            Ray ray = new Ray(cPos, Vector3.down);
            RaycastHit hitInfo;
            //计算离地面的距离，然后调整
            int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
            bool hit = Physics.Raycast(ray, out hitInfo, 2000.0f, LayerMaskOnGround);
            if (hit)
            {
                cPos = hitInfo.point;
            }

            camTransform.SetPosition(cPos + (finalDir * finalDistance));

            camTransform.LookAt(cPos);

            //设置目标框
            target.SetPosition(cPos);
            target.eulerAngles = finalAngle;
        }

        // 小地图上帝之眼（矩形框）
        private void UpdateMinimapGodEye()
        {
            if (SwitchGodEyeModeEvent != null)
            {
                SwitchGodEyeModeEvent(true, GodPos);
            }
        }

        private void CalGodEyeCameraFocusPos()
        {
            GodEyeFocusPosCheckTime += deltaTime;
            if (GodEyeFocusPosCheckTime > 5.0f)
            {
                SendCameraFocusPosToLogic(1, cPos.x, cPos.y, cPos.z);
                GodEyeFocusPosCheckTime = 0;
            }
        }

        protected bool GetSceneAngleY(ref  float result)
        {
            SMinimapInfo m_sCurMinimapInfo = new SMinimapInfo();
            int nAngle = 270;
            if (GameLogicAPI.getMinimapInfo(SceneManager.Instance.m_curMapID, ref m_sCurMinimapInfo))
            {
                string[] strRotation = ((string)(m_sCurMinimapInfo.szDefaultRotation)).Split(';');
                for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
                {
                    int nCamp = Convert.ToInt32(strRotation[idx]);
                    if (EntityFactory.MainHeroView.createinfo.byCamp == nCamp)
                    {
                        nAngle = Convert.ToInt32(strRotation[idx + 1]) + (360 - m_sCurMinimapInfo.nOriRotation);
                        break;
                    }
                }
                Vector3 godDir = new Vector3(0, 0, -1);
                result = (float)nAngle; 
                return true;
            }
            return false;
        }

        public override bool Enable(bool isInnerSwitch)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        public override bool CheckContext(CameraContext context)
        {
            return CheckGodContext(context);
        }

        public abstract bool CheckGodContext(CameraContext context);
    }
}
