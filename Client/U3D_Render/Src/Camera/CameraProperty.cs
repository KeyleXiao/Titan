/*******************************************************************
** 文件名:	CameraProperty.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/22
** 版  本:	1.0
** 描  述:	英雄相机属性表
** 应  用:  	用于不同英雄可以使用不同的相机属性,
 *          相机逻辑从这里获取属性

**************************** 修改记录 ******************************
** 修改人: 谭强均
** 日  期: 2015/4/16
** 描  述: 增加copy函数
********************************************************************/
using UnityEngine;

/// <summary>
/// 摄像机属性，新增的属性记得写到Copy函数中
/// </summary>
public class CameraProperty:MonoBehaviour
{
    /// <summary>
    /// 相机转动速度X:yaw速度，Y：pitch速度
    /// </summary>
    public Vector2 Speed = new Vector2(135.0f, 135.0f);
    
    /// <summary>
    /// 相机转动最大速度X:yaw速度，Y：pitch速度
    /// </summary>
    public Vector2 MaxSpeed = new Vector2(270.0f, 270.0f);

    /// <summary>
    /// 相机转动速度缩放比例
    /// </summary>
    public float SpeedScale = 1.0f;
    ///// <summary>
    ///// 相机Pitch角度限制，x：最小值，y：最大值
    ///// </summary>
    //public Vector2 PitchAngleClamp = new Vector2(-45.0f, 45.0f);
    //public Vector2 FloatingPitchAngleClamp = new Vector2(-28.0f, 28.0f);

    /// <summary>
    /// 相机FOV角度
    /// </summary>
    public int NormalFOV = 60;

    /// <summary>
    /// FOV角变化速度
    /// </summary>
    public int FOVLerpSpeed = 8;

    /// <summary>
    /// 相机偏移方向
    /// </summary>
    public Vector3 NormalDirection = new Vector3(-1, 0, 0);

    /// <summary>
    /// 相机偏离角色pos高度
    /// </summary>
    public float NormalHeight = 2.0f;

    /// <summary>
    /// 相机偏离角色pos最大高度
    /// </summary>
    public float MaxHeight = 2.0f;

    ///// <summary>
    ///// 相机与角色距离范围,X:最小距离,Y:最大距离
    ///// </summary>
    //public Vector2 DistanceClamp = new Vector2(0.8f,6.0f);

    /// <summary>
    /// 相机距离拉扯速度
    /// </summary>
    public float ScrollSpeed = 100.0f;

    /// <summary>
    /// 初始相机距离
    /// </summary>
    public float NormalDistance = 7.0f;

    /// <summary>
    /// 射线检测小于该距离时，镜头不移动
    /// </summary>
    public float RayCastHitMinDistance = 0.2f;

    /// <summary>
    /// 进入战场后，以这个角度初始化相机角度
    /// </summary>
    public Vector3 InitedEulerAngle = Vector3.zero;

    /// <summary>
    /// 从另一个相机参数过度到本相机参数需要多少时间，秒
    /// </summary>
    //[HideInInspector]
    public float TranslationDuration = 1.0f;

    /// <summary>
    /// 主角控制模式
    /// </summary>
    //[HideInInspector]
    public int soldiermode = 0;

    ///// <summary>
    ///// FPS模式镜头位移
    ///// </summary>
    //public Vector3 FPSTargetOffset = new Vector3(0.0f,1.0f,0.0f);


    /// <summary>
    /// 记录配置是从哪边设置过来的，默认和人物预制体的是0，场景的是1
    /// </summary>
    public int configType = 0;

    /// <summary>
    /// 拷贝属性。新加的变量记得写到这个cpoy里！！！！
    /// </summary>
    /// <param name="dst"></param>
    public void Copy(CameraProperty src)
    {
        Speed = src.Speed;
        MaxSpeed = src.MaxSpeed;
        //PitchAngleClamp = src.PitchAngleClamp;
        //FloatingPitchAngleClamp = src.FloatingPitchAngleClamp;
        NormalFOV = src.NormalFOV;
        FOVLerpSpeed = src.FOVLerpSpeed;
        NormalDirection = src.NormalDirection;
        NormalHeight = src.NormalHeight;
        MaxHeight = src.MaxHeight;
        //DistanceClamp = src.DistanceClamp;
        ScrollSpeed = src.ScrollSpeed;
        NormalDistance = src.NormalDistance;
        RayCastHitMinDistance = src.RayCastHitMinDistance;
        InitedEulerAngle = src.InitedEulerAngle;
        TranslationDuration = src.TranslationDuration;
        soldiermode = src.soldiermode;
        //FPSTargetOffset = src.FPSTargetOffset;
    }
}

