/*******************************************************************
** 文件名:	AnimCameraModeController.cs
** 描  述:	摄像机动画状态类
** 应  用:  	
            处理摄像机动画（开场动画）：
            1、通过播放clip来实现
            2、对于fov，通过将fov的变化烘焙到camFOVGo的z轴
********************************************************************/
using Effect;
using UnityEngine;

namespace U3D_Render.CameraControl
{
    class AnimCameraModeController : BaseCameraController, ICameraModelController
    {
        
        private Vector3 m_cameraAminOrgPosition = Vector3.zero;
        private Vector3 m_cameraAminOrgAngle = Vector3.zero;
        private Vector3 m_cameraAminRecPosition = Vector3.zero;
        private bool m_cameraAminFollowHero = false;
        private bool m_cameraAnimProjectFloor = false;
        private bool m_cameraAnimMainCamera = false;
        private uint m_cameraAnimStopTick = 0;

        private Transform m_camRecTrans = null;
        private Transform m_camAnimTrans = null;
        private Transform m_camFOVTrans = null;
        private GameObject animGo;
        public class AnimCamContext:CameraContext
        {
            public AnimationClip clip;
            public CameraAnimOrgType animOrgType;
            public bool projectFloor;
            public Vector3 targetPos;
            public Transform sourceTransform;
            public bool animationMainCamera;
        }

        /// <summary>
        /// 初始化CameraController
        /// </summary>
        public override void Init()
        {
            CameraUtil.InitCameraMode<AnimCameraModeController>(GetCameraMode(), this);
        }

        /// <summary>
        /// 设置该状态可以转移的状态
        /// </summary>
        public override void SetAllowChangeList()
        {
            AllowChangeList.Add(CameraMode.CameraAnim);
            AllowChangeList.Add(CameraMode.DeadControl);
            AllowChangeList.Add(CameraMode.EndWarControl);
            AllowChangeList.Add(CameraMode.FollowTarget);
            AllowChangeList.Add(CameraMode.FPSMode);
            AllowChangeList.Add(CameraMode.UserControl);
        }


        /// <summary>
        /// 本控制器接管
        /// </summary>
        public override void Enter(CameraContext context)
        {
            AnimCamContext _context = context as AnimCamContext;

            SetupDatas(_context);
            Animation anim = animGo.AddComponent<Animation>();
            if(anim != null)
            {
                anim.AddClip(_context.clip, _context.clip.name);
                anim.Play(_context.clip.name);
            }
            SendCameraFocusPosToLogic(0);
        }

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        public override void Leave()
        {
            if (m_camRecTrans)
            {
                var temp = m_camRecTrans.gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        public override CameraMode GetCameraMode()
        {
            return CameraMode.CameraAnim;
        }

        /// <summary>
        /// 判断是否可以切换到该模式
        /// </summary>
        /// <param name="isInnerSwitch"></param>
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
        public override bool CheckContext(CameraContext context)
        {
            AnimCamContext _context = context as AnimCamContext;
            if (_context != null && _context.clip != null)
                return true;
            else
            {
                CameraUtil.LogError("_context.clip is null! : " + _context);
                return false;
            }
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
            updateCameraAnim();
        }

        private void SetupDatas(AnimCamContext context)
        {

            GameObject camGo = new GameObject("CameraAnimRef");
            m_camRecTrans = camGo.transform;
            animGo = new GameObject("Anim");
            animGo.transform.parent = m_camRecTrans;
            m_camAnimTrans = animGo.transform;
            GameObject camFOVGo = new GameObject("camFOV");
            m_camFOVTrans = camFOVGo.transform;
            m_camFOVTrans.parent = animGo.transform;

            m_camRecTrans.position = Vector3.zero;
            m_cameraAminFollowHero = false;
            m_cameraAnimProjectFloor = context.projectFloor;
            m_cameraAnimMainCamera = context.animationMainCamera;

            if (context.animOrgType == CameraAnimOrgType.Hero || context.animOrgType == CameraAnimOrgType.FollowHero)
            {
                if (soldier != null && soldier.gameObject)
                {

                    m_camRecTrans.position = soldier.gameObject.transform.position;
                    m_camRecTrans.eulerAngles = soldier.gameObject.transform.eulerAngles;

                    if (context.animOrgType == CameraAnimOrgType.FollowHero)
                    {
                        m_cameraAminFollowHero = true;
                    }
                }
            }
            else if (context.animOrgType == CameraAnimOrgType.TargetPos)
            {
                m_camRecTrans.position = context.targetPos;
                if (soldier != null)
                {
                    if (soldier.gameObject)
                    {
                        m_camRecTrans.eulerAngles = soldier.gameObject.transform.eulerAngles;
                    }

                }
            }
            else if (context.animOrgType == CameraAnimOrgType.SourceObject)
            {
                m_camRecTrans.position = context.sourceTransform.position;
                m_camRecTrans.eulerAngles = context.sourceTransform.eulerAngles;
            }

            if (m_cameraAnimProjectFloor)
            {
                m_camRecTrans.position = CameraUtil.ProjectToFloor(m_camRecTrans.position);
            }

            m_cameraAminOrgPosition = m_camRecTrans.position;

            if (context.clip.legacy == false)
            {
                Debug.Log("Camera Animation clip " + context.clip.name + " need to be legacy!");
                context.clip.legacy = true;
            }

            m_cameraAnimStopTick = ((uint)(context.clip.length * 1000.0f)) + GameLogicAPI.getTickCount();
        }
        private void updateCameraAnim()
        {
            Transform camTrans = camTransform;
            if (m_cameraAnimMainCamera)
            {
                camTrans = Camera.main.transform;
            }
            camTrans.position = m_camAnimTrans.position;
            Vector3 angle = m_camAnimTrans.eulerAngles;
            camTrans.eulerAngles = angle;
            float focal = m_camFOVTrans.localScale.z;
            float fov = Mathf.Atan(30.0f / focal) / Mathf.PI * 180.0f;
            m_Camera.fieldOfView = fov;

            if (m_cameraAminFollowHero && soldier.gameObject)
            {
                m_camRecTrans.position = soldier.gameObject.transform.position;
                if (m_cameraAnimProjectFloor)
                {
                    m_camRecTrans.position = CameraUtil.ProjectToFloor(m_camRecTrans.position);
                }
            }

            uint tick = GameLogicAPI.getTickCount();
            if (tick > m_cameraAnimStopTick)
            {
                StopCameraAnim();
            }
        }

        public void StopCameraAnim()
        {
            soldierCam.LeaveCurMode();
        }
    }
}
