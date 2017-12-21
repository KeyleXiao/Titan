/*******************************************************************
** 文件名:	CameraStateParam.cs
** 描  述:	摄像机状态切换数据类
** 应  用:  	
            用于保存切换状态时必要的数据
            必须继承CameraStateParam!
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    public class CameraStateParam
    {
        private CameraMode mode;
        private bool enabled;
        public void Set(CameraMode mode, bool enabled)
        {
            this.mode = mode;
            this.enabled = enabled;
        }
        public CameraStateParam() { }
        public CameraStateParam(CameraMode mode, bool enabled)
        {
            this.mode = mode;
            this.enabled = enabled;
        }
    }

    public class RidingStateParam : CameraStateParam
    {
        public float distance;
        public Vector2 angleRange;
        public RidingStateParam(float distance, Vector2 angleRange)
        {
            this.distance = distance;
            this.angleRange = angleRange;
        }
    }

    public class CamAnimStateParam : CameraStateParam
    {
        public AnimationClip clip;
        public Effect.CameraAnimOrgType animOrgType;
        public bool projectFloor;
        public Vector3 targetPos;
        public Transform sourceTransform;
        public bool animationMainCamera;

        public CamAnimStateParam(AnimationClip clip, Effect.CameraAnimOrgType animOrgType, bool projectFloor,
            Vector3 targetPos, Transform sourceTransform, bool animationMainCamera)
        {
            this.clip = clip;
            this.animOrgType = animOrgType;
            this.projectFloor = projectFloor;
            this.targetPos = targetPos;
            this.sourceTransform = sourceTransform;
            this.animationMainCamera = animationMainCamera;
        }
    }

    public class GodEyeStateParam : CameraStateParam
    {
        public Vector3 pos;
        public Vector3 angle;
        public Vector3 dir;
        public Vector3 tran;
        public float distance;
        public float fov;
        public bool isMiniMap;
        public GodEyeStateParam(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance,
            float fov, bool isMiniMap = true)
        {
            this.pos = pos;
            this.angle = angle;
            this.dir = dir;
            this.tran = tran;
            this.distance = distance;
            this.fov = fov;
            this.isMiniMap = isMiniMap;
        }
    }

    public class WarEndStateParam : CameraStateParam
    {
        public Vector3 pos;
        public int nWinFlag;
        public WarEndStateParam(Vector3 pos, int nWinFlag)
        {
            this.pos = pos;
            this.nWinFlag = nWinFlag;
        }
    }
}