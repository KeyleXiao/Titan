using UnityEngine;
using System.Collections;

/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Area : LightingEffectBehavior
{
    private LightingEffect_Area areaConfig = null;
    private uint mStartTick = 0;
    private uint mNextCreateTick = 0;
    private uint mEndTick = 0;
    private int mEffectLeftCount = 0;

    Transform sourceTransform;


    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.areaConfig = effectConfig as LightingEffect_Area;
        if (!areaConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!areaConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        uint tick = GameLogicAPI.getTickCount();
        mStartTick = tick + (uint)areaConfig.effectStartTick;
        mNextCreateTick = mStartTick;
        mEndTick = mStartTick + (uint)areaConfig.effectTotalTick;
        mEffectLeftCount = areaConfig.effectCount;

        //将光效绑定到指定位置
        sourceTransform = null;
        if (areaConfig.posBindBone != null && areaConfig.posBindBone != "")
        {
            sourceTransform = FindTransformEx(EffectParam.nSrcEntityID, areaConfig.posBindBone);
        }

        return true;
    }

    private bool createEffect()
    {
        LightEffectParam newParam = new LightEffectParam();
        EffectParam.ColoneTo(ref newParam);

        float maxDistance = EffectParam.fDistance;
        float effectRad = EffectParam.fAttackRange;

        Vector3 newSourcePos = newParam.srcObject.transform.position;
        if (sourceTransform != null)
        {
            newSourcePos = sourceTransform.position;
        }


        Vector3 areaTargetPos = EffectParam.targetPos;
        Vector3 newTargetPos = areaTargetPos;
        //构造一个范围内的随机目标位置
        if (areaConfig.projectOnGround == true)
        {  //构造一个投影到地面的圆形
            int angle = Random.Range(0, 360);
            float _rad = angle * Mathf.PI / 180.0f;
            Vector3 offset = new Vector3(Mathf.Sin(_rad), 0, Mathf.Cos(_rad));
            float dis = Random.Range(0.0f, effectRad);
            offset *= dis;
            newTargetPos += offset;

            RaycastHit _ClosestHitInfo_Down;
            RaycastHit _ClosestHitInfo_Up;
            float rayDistance = maxDistance;
            Ray PointToGroundRayDown = new Ray(newTargetPos, new Vector3(0, -1, 0));
            Ray PointToGroundRayUp = new Ray(newTargetPos, new Vector3(0, 1, 0));
            bool DownHit = Physics.Raycast(PointToGroundRayDown, out _ClosestHitInfo_Down, rayDistance, -1);
            bool UpHit = Physics.Raycast(PointToGroundRayUp, out _ClosestHitInfo_Up, rayDistance, -1);
            Vector3 recordPos = newTargetPos;
            float hitDistance = maxDistance;
            if (DownHit)
            {
                hitDistance = (_ClosestHitInfo_Down.point - recordPos).magnitude;
                newTargetPos = _ClosestHitInfo_Down.point;
            }

            if (UpHit)
            {
                if ((_ClosestHitInfo_Up.point - recordPos).magnitude < hitDistance)
                {
                    hitDistance = (_ClosestHitInfo_Up.point - recordPos).magnitude;
                    newTargetPos = _ClosestHitInfo_Up.point;
                }
            }

            newParam.targetPos = newTargetPos;

        }
        else if (effectRad > 0.01f)
        { //构造一个圆锥
            Vector3 normal = EffectParam.targetPos - newSourcePos;
            Vector3 tangent = new Vector3(0, 0, 1);
            Vector3.OrthoNormalize(ref normal, ref tangent);
            int angle = Random.Range(0, 360);
            Quaternion rotation = Quaternion.AngleAxis((float)angle, normal);
            tangent = rotation * tangent;
            float dis = Random.Range(0.0f, effectRad);
            tangent *= dis;
            newTargetPos += tangent;
            if (areaConfig.extentToMaxDistance)
            {//将目标点拉远，延长用更容易造成与地面的碰撞
                Vector3 dir = newTargetPos - newSourcePos;
                dir.Normalize();
                newTargetPos = newSourcePos + dir * maxDistance;
            }
            newParam.targetPos = newTargetPos;
        }
        else
        {
            if (areaConfig.extentToMaxDistance)
            {//将目标点拉远，延长用更容易造成与地面的碰撞
                Vector3 dir = newTargetPos - newSourcePos;
                dir.Normalize();
                newTargetPos = newSourcePos + dir * maxDistance;
            }
            newParam.targetObject = EffectParam.targetObject;
            newParam.targetPos = EffectParam.targetPos;
        }

        if (areaConfig.customOffset.sqrMagnitude > 0.001f)
        {
            //有此设置时，变更起始点，变为相对于newParam.targetPos，偏移customOffet
            Vector3 offsetNormal = EffectParam.targetPos - newSourcePos;
            offsetNormal.y = 0;
            offsetNormal.Normalize();
            Vector3 offsetTangent = new Vector3(0, 1, 0);
            Vector3 offsetBinormal = Vector3.Cross(offsetTangent, offsetNormal);
            //Vector3.OrthoNormalize(ref offsetNormal, ref offsetTangent,ref offsetBinormal);
            Vector3 offSet = newParam.targetPos;
            offSet += offsetNormal * areaConfig.customOffset.z;
            offSet += offsetTangent * areaConfig.customOffset.y;
            offSet += offsetBinormal * areaConfig.customOffset.x;
            newParam.sourcePos = offSet;
            newParam.nEffectFlag |= (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos;
        }

        LightingEffectManager leManager = EffectParam.srcObject.GetComponent<LightingEffectManager>();
        if (leManager != null)
        {
            ////测试用代码，用来标记检测发射区域是否正确
            //GameObject checkPoint = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            //checkPoint.transform.localScale = new Vector3(0.2f, 0.2f, 0.2f);
            //checkPoint.transform.SetPosition( newParam.targetPos;
            //checkPoint.collider.enabled = false;
            //checkPoint.name = "Point" + mEffectLeftCount.ToString();

            leManager.AddLighting(areaConfig.subEffectID, newParam);
        }

        return true;
    }

    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();

        if (needClose == true || (areaConfig.effectTotalTick > 0) && (tick > mEndTick))
        {
            return false;
        }

        if (tick < mStartTick)
        {
            return true;
        }

        if (tick >= mNextCreateTick)
        {
            mNextCreateTick = tick + (uint)areaConfig.effectIntervalTick;
            createEffect();
            mEffectLeftCount--;
        }

        if (mEffectLeftCount == 0)
        {
            needClose = true;
            return false;
        }

        return true;
    }

    public override void Close()
    {
        base.Close();
    }

    public override void ResetAllMembers()
    {
        mStartTick = 0;
        mNextCreateTick = 0;
        mEndTick = 0;
        mEffectLeftCount = 0;
        sourceTransform = null;
        base.ResetAllMembers();
    }

    //设置创建的光效个数
    public void SetEffectCount(int c)
    {
        this.mEffectLeftCount = c;
    }


}
