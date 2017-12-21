/*******************************************************************
** 文件名:	EffectEnumDef.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2016/05/09
** 版  本:	1.0
** 描  述:	特效枚举定义
** 应  用:  	特效枚举定义

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using UnityEngine.Events;
namespace Effect
{
    /// <summary>
    /// 特效播放优先级
    /// </summary>
    public enum EffectPlayPriority
    {
        Low,
        Middle,
        High,
    }

    /// <summary>
    /// 材质球操作运行模式
    /// </summary>
    public enum MaterialOpRunType
    {
        /// <summary>
        /// 添加到自己身上
        /// </summary>
        Self,

        /// <summary>
        /// 指定模型
        /// </summary>
        PointedModel,

        /// <summary>
        /// 实体ID
        /// </summary>
        EntityID,
    }

    public enum EffectRunMode
    {
        /// <summary>
        /// 自己运行自己
        /// </summary>
        Self,

        /// <summary>
        /// 通过外部调用运行
        /// </summary>
        OutSiedePointed
    }

    /// <summary>
    /// 值类型
    /// </summary>
    public enum ValueMode
    {
        /// <summary>
        /// 常量
        /// </summary>
        Constant,

        /// <summary>
        /// 曲线
        /// </summary>
        Curve,
    }

    /// <summary>
    /// 条带目标类型
    /// </summary>
    public enum LineRenderTargetType
    {
        /// <summary>
        /// 使用外部指定的目标
        /// </summary>
        OutSiedePointed,

        /// <summary>
        /// 使用自身的EffectNode坐标
        /// </summary>
        Self,
    }

    /// <summary>
    /// 用哪个部位的render来修改shape
    /// </summary>
    public enum ModifyParticlesShapePart
    {
        Body,
        LeftWeapon,
        RightWeapon,
    }

    public enum ModifyBlendMode
    {
        Relepace,
        Additive,
    }

    public enum ScaleType
    {
        Model,
        ParitcleSystem,
    }

    public enum ParticleSystemScaleType
    {
        WoldSpaceScale,
        LocalSpaceScale,
        ShapeScale,
    }

    public enum EffectAttackType
    {
        Source = 0,
        Target = 1,
        SourceAndTarget = 2,
        All = 3,
    }


    public enum CameraAnimOrgType
    {
        Hero = 0,
        FollowHero = 1,
        TargetPos = 2,
        SourceObject = 3,
    }
    public enum EffectNodeMask
    {
        /// <summary>
        /// 比当前等级低一级，最低级则不播
        /// </summary>
        Mask_DownLevelQuality = 1,

        /// <summary>
        /// 比当前等级高一级，最多最高级
        /// </summary>
        Mask_UpLevelQuality = 2,

        /// <summary>
        /// 启用特效里面的灯光
        /// </summary>
        Mask_EnableLihgt = 4,
    }

    public class FloatFadeFinishEvent : UnityEvent<Effect_FadeInOutMaterialFloat>
    {
    }

    public class LightFadeFinishEvent : UnityEvent<Effect_FadeInOutLightIntensity>
    {
    }
    public class ColorFadeFinishEvent : UnityEvent<Effect_FadeInOutMaterialColor>
    {
    }
    public class SceneBrightnesFadeFinishEvent : UnityEvent<Effect_ModifSceneBrightness>
    {
    }
    public class EffectQualityChangeEvent : UnityEvent
    {
    }

    [System.Serializable]
    public class ModifyInfo3
    {
        public Vector3 IsLoop = Vector3.zero ;
        public Vector3 Scale = Vector3.one;
        public Vector3 Duration = Vector3.zero;

        public AnimationCurve CurveX = AnimationCurve.EaseInOut(1, 1, 1, 1);
        public AnimationCurve CurveY = AnimationCurve.EaseInOut(1, 1, 1, 1);
        public AnimationCurve CurveZ = AnimationCurve.EaseInOut(1, 1, 1, 1);

        public ModifyBlendMode blendMode = ModifyBlendMode.Additive;

        private Vector3 currDuration = Vector3.zero;


        /// <summary>
        /// 每帧调用
        /// </summary>
        /// <param name="src"></param>
        /// <returns></returns>
        public Vector3 Evaluate(Vector3 src)
        {
            Vector3 reslut = src;
            if (Duration.x > 0)
            {
                float t = currDuration.x / Duration.x;
                if (currDuration.x > Duration.x)
                {
                    if (IsLoop.x > 0)
                    {
                        currDuration.x = 0;
                        reslut.x = EvaluateValue(CurveX, t, Scale.x, reslut.x);
                    }
                }
                else
                {
                    reslut.x = EvaluateValue(CurveX, t, Scale.x, reslut.x);
                }
                currDuration.x += Time.deltaTime;
            }

            if (Duration.y > 0)
            {
                float t = currDuration.y / Duration.y;
                if (currDuration.y > Duration.y)
                {
                    if (IsLoop.y > 0)
                    {
                        currDuration.y = 0;
                        reslut.y = EvaluateValue(CurveY, t, Scale.y, reslut.y);
                    }
                }
                else
                {
                    reslut.y = EvaluateValue(CurveY, t, Scale.y, reslut.y);
                }
                currDuration.y += Time.deltaTime;
            }

            if (Duration.z > 0)
            {
                float t = currDuration.z / Duration.z;
                if (currDuration.z > Duration.z)
                {
                    if (IsLoop.z > 0)
                    {
                        currDuration.z = 0;

                        reslut.z = EvaluateValue(CurveZ, t, Scale.z, reslut.z);
                    }
                }
                else
                {
                    reslut.z = EvaluateValue(CurveZ, t, Scale.z, reslut.z);

                }
                currDuration.z += Time.deltaTime;
            }




            return reslut;
        }

        private float EvaluateValue(AnimationCurve Curve, float t, float scale, float value)
        {
            if (blendMode == ModifyBlendMode.Additive)
            {
                value += Curve.Evaluate(t) * scale;
            }
            else
            {
                value = Curve.Evaluate(t) * scale;
            }
            return value;
        }

        public void Reset()
        {
            currDuration = Vector3.zero;
        }
    }

    [System.Serializable]
    public class ModifyInfo
    {
        public float IsLoop = 0;
        public float Scale = 1;
        public float Duration = 0;

        public AnimationCurve Curve = AnimationCurve.EaseInOut(1, 1, 1, 1);

        public ModifyBlendMode blendMode = ModifyBlendMode.Additive;

        private float currDuration = 0;


        /// <summary>
        /// 每帧调用
        /// </summary>
        /// <param name="src"></param>
        /// <returns></returns>
        public float Evaluate(float src)
        {
            float reslut = src;
            if (Duration > 0)
            {
                float t = currDuration / Duration;
                if (currDuration > Duration)
                {
                    if (IsLoop > 0)
                    {
                        currDuration = 0;
                        reslut = EvaluateValue(Curve, t, Scale, reslut);
                    }
                }
                else
                {
                    reslut = EvaluateValue(Curve, t, Scale, reslut);
                }
                currDuration += Time.deltaTime;
            }
            return reslut;
        }

        private float EvaluateValue(AnimationCurve Curve, float t, float scale, float value)
        {
            if (blendMode == ModifyBlendMode.Additive)
            {
                value += Curve.Evaluate(t) * scale;
            }
            else
            {
                value = Curve.Evaluate(t) * scale;
            }
            return value;
        }

        public void Reset()
        {
            currDuration = 0;
        }
    }
}

