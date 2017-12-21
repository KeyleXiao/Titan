/*******************************************************************
** 文件名:	Effect_CameraAnimation.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2016/05/09
** 版  本:	1.0
** 描  述:	Effect_CameraAnimation
** 应  用:  	Effect_CameraAnimation

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
namespace Effect
{
    [System.Serializable]
    //摄像机震动配置参数
    public class Effect_ShakeCamera
    {
        //[Desc("震动次数")]
        //public int shakeTimes;

        //[Desc("震动方向")]
        //public Vector3 shakeDirection;

        //[Desc("初始震动速率")]
        //public float initSpeed;

        //[Desc("同一次震动中的加速度")]
        //public float shakeAcceleration;

        //[Desc("下一次震动的速度衰减")]
        //public float speedAcceleration;

        //[Desc("初始震动幅度")]
        //public float initAmplitude;

        //[Desc("下一次震动幅度衰减")]
        //public float amplitudeAcceleration;

        [Desc("是否基于角色面朝方向")]
        public bool isDependRole;
        [Desc("是否采样随机抖动（以前的）")]
        public bool isRandom = true;
        [Desc("震动最大幅度")]
        public float maxShake = 2f;
        [Desc("震动最大FOV")]
        public float maxShakeFov = 45f;
        [Desc("震动最大速度")]
        public float maxShakeSpeed;
        [Desc("震动最多次数")]
        public int maxShakeTimes;
        [Desc("震动最小幅度")]
        public float minShake;
        [Desc("震动最小FOV")]
        public float minShakeFov = 45f;
        [Desc("震动最小速度")]
        public float minShakeSpeed;
        [Desc("震动最少次数")]
        public int minShakeTimes;
        [Desc("震动方向(x:屏幕空间的水平方向,y:屏幕空间的垂直方向,z:镜头正对的方向)")]
        public Vector3 shakeDir = Vector3.up;
        [Desc("震动FOV变化速度")]
        public float shakeFovSpeed;
    }

    [System.Serializable]
    //某个关键帧的摄像机属性
    public class Effect_CameraNode
    {
        [Desc("到第n毫秒，所达到的效果（角度，距离等），请注意是截止点的时间，例如第一个效果的tick参数不应该是0而应该是第一个效果结束的时间点")]
        public int tick;

        [Desc("摄像机位置锁定")]
        public bool lockPosition;

        [Desc("摄像机离主角Fov不变")]
        public bool lockFov;

        [Desc("摄像机离主角距离不变")]
        public bool lockDistance;

        [Desc("摄像机离主角平移不变")]
        public bool lockTran;

        [Desc("摄像机离主角偏移方向不变")]
        public bool lockDir;

        [Desc("摄像机绕主角旋转角度不变")]
        public bool lockAngle;

        [Desc("摄像机离主角距离")]
        public float distance;

        [Desc("摄像机离观察点位置")]
        public Vector3 pos;

        [Desc("摄像机离观察点平移")]
        public Vector3 tran;

        [Desc("摄像机离主角方向偏移")]
        public Vector3 dir;

        [Desc("摄像机绕主角旋转角度")]
        public Vector3 angle;

        [Desc("摄像机视锥角，可用于镜头拉近拉远")]
        public float fov;

        [Desc("摄像机特效时，玩家是否能操控摄像机和角色")]
        public bool playerControl = true;

        [Desc("摄像机是否震动")]
        public bool shakeCamera = false;

        [Desc("摄像机震动时的配置参数，展开后配置Effect_ShakeCamera")]
        public Effect_ShakeCamera shakeParam;
    }

    public class Effect_CameraAnimation : MonoBehaviour
    {
        [Desc("以动画形式制作的镜头动画效果")]
        public AnimationClip animationClip = null;

        [Desc("摄像机移动关键帧节点，可以有多个，展开后配置EffectCameraNode")]
        public Effect_CameraNode[] cameraAxis;
    }

}

