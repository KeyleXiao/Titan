/*******************************************************************
** 文件名:	ICameraModelController.cs
** 描  述:	摄像机状态机接口
** 应  用:  	
            处理摄像机状态机的固有操作
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{
    /// <summary>
    /// 相机上下文参数
    /// </summary>
    public class CameraContext
    {

    }

    /// <summary>
    /// 镜头抖动上下文
    /// </summary>
    public class CameraShakeContext
    {
        public float minShakeSpeed;
        public float maxShakeSpeed;
        public float minShake;
        public float maxShake;
        public int minShakeTimes;
        public int maxShakeTimes;
        public float maxShakeFov;
        public float minShakeFov;
        public float maxShakeDistance;
        public float shakeFovSpeed;
        public Vector3 shakeDir;
        public bool isDirDependRole;
        public bool isRandom;
    }

    /// <summary>
    /// 相机模式控制器
    /// </summary>
    public interface ICameraModelController
    {

        /// <summary>
        /// 初始化
        /// </summary>
        void Init();

        /// <summary>
        /// 设置状态允许跳转的状态
        /// </summary>
        void SetAllowChangeList();

        /// <summary>
        /// 本控制器接管
        /// </summary>
        void Enter(CameraContext context);

        /// <summary>
        /// 离开本控制器接管
        /// </summary>
        void Leave();

        /// <summary>
        /// 获取模式
        /// </summary>
        /// <returns></returns>
        CameraMode GetCameraMode();

        /// <summary>
        /// 判断此模式是否可生效
        /// </summary>
        bool Enable(bool isInnerSwitch);

        /// <summary>
        /// 判断Context是否存在且无误
        /// </summary>
        /// <param name="bEnabled"></param>
        bool CheckContext(CameraContext context);

        /// <summary>
        /// 更新相机数据
        /// </summary>
        /// <param name="context"></param>
        void UpdateData(CameraContext context);

        void OnUpdate();

        void OnLateUpdate();

        void Reset(CameraProperty property,bool smoothBlend);
    }
}
