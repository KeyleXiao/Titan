using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using GameLogic;
/// <summary>
/// 光效行为
/// </summary>
public sealed class LightingEffectBehavior_Shadow : LightingEffectBehavior
{
    private LightingEffect_Shadow shadowConfig = null;

    class ShadowInstance
    {
        public GameObject gameObject;
        public int createTick;
        public float alpha;
        public List<Renderer> renders = new List<Renderer>();
    }

    private List<ShadowInstance> shadowInstanceLists; //残影实例列表
    private uint mStartTick; //开始时刻
    private uint mEndTick; //开始时刻
    private int shadowObjectIndex = 0; //到创建第i个阴影时刻
    private Color matColor = Color.blue; //当替换材质需要靠设置颜色来调整alpha值的时候，记录初始颜色的rgb
    private int shadowCount = 0;
    private int maxShadowCount = 0;

    public override void FillData(LightingEffect effectConfig, LightEffectParam param, LightingEffectResHandleInstance instance, Transform transformAgent)
    {
        base.FillData(effectConfig, param, instance, transformAgent);
        this.shadowConfig = effectConfig as LightingEffect_Shadow;
        if (!shadowConfig && effectConfig)
        {
            Debug.LogWarning(effectConfig.nID + "配置不匹配,创建光效失败---" + GetType().ToString());
        }
    }

    public override bool Begin()
    {
        if (!shadowConfig || null == EffectParam)
        {
            return false;
        }
        base.Begin();
        if (!AssetBundleManager.EntityAlphaShader)
        {
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = this;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
            Trace.LogWarning("AssetBundleManager.EntityAlphaShader---实体半透shader为null");
            return false;
        }

        mStartTick = GameLogicAPI.getTickCount() + (uint)shadowConfig.nStartTick;
        mEndTick = mStartTick + (uint)shadowConfig.nEndTick + (uint)shadowConfig.nShadowMaintainTick + (uint)shadowConfig.nShadowFadeoutTick;
        shadowObjectIndex = 0;
        shadowInstanceLists = new List<ShadowInstance>();
        shadowCount = 0;
        if (!EffectParam.bIsHero)
        {
            maxShadowCount = 20; //非主角最多画20个

            //计算前进一步所偏移的像素
            Vector3 projectPos = Initialize.mainCam.WorldToScreenPoint(EffectParam.srcObject.transform.position);
            Vector3 projectStepPos = Initialize.mainCam.WorldToScreenPoint(EffectParam.srcObject.transform.position + EffectParam.srcObject.transform.forward.normalized);
            Vector3 dis = projectPos - projectStepPos;
            dis.z = 0;

            //间隔5像素画一个
            float drawDis = 5.0f / dis.magnitude;
            shadowConfig.minDistance = Mathf.Max(shadowConfig.minDistance, drawDis);
        }
        return true;
    }

    public override void extendEffectLift()
    {
        mEndTick += (uint)shadowConfig.nEndTick;
    }



    private int currentTick;
    public override bool LateUpdate()
    {
        if (!Valid)
        {
            return false;
        }
        uint tick = GameLogicAPI.getTickCount();
        currentTick = (int)(tick - mStartTick);

        if (shadowConfig.nShadowMaintainTick + shadowConfig.nShadowFadeoutTick <= 0)
        {
            return false;
        }

        if (needClose == true || ((shadowConfig.nEndTick > 0) && (tick > mEndTick)))
        {
            return false;
        }

        if (tick < mStartTick)
        {
            return true;
        }

        if (shadowConfig.shadowIntervalTicks > 0)
        {
            if ((tick <= mEndTick) && (tick >= mStartTick + shadowObjectIndex * shadowConfig.shadowIntervalTicks))
            {
                createShadow();
                shadowObjectIndex++;
            }
        }
        else if (shadowObjectIndex < shadowConfig.shadowFrameTicks.Length)
        {
            if (currentTick - mStartTick >= shadowConfig.shadowFrameTicks[shadowObjectIndex])
            {
                createShadow();
                shadowObjectIndex++;
            }
        }

        updateShadow();

        return true;
    }

    public override void Close()
    {
        if (!Valid)
        {
            return ;
        }
        if (null == shadowInstanceLists)
        {
            return;
        }
        foreach (ShadowInstance shadow in shadowInstanceLists)
        {
            if (shadow.gameObject != null)
            {
                ResNode.DestroyRes(ref shadow.gameObject);
            }
        }
        shadowInstanceLists.Clear();
        shadowInstanceLists = null;
    }

    public override void ResetAllMembers()
    {
        Close();
        shadowInstanceLists = null;
        mStartTick = 0;
        mEndTick = 0;
        shadowObjectIndex = 0;
        matColor = Color.blue;
        shadowCount = 0;
        maxShadowCount = 0;
        currentTick = 0;
        base.ResetAllMembers();
    }

    private void createShadow()
    {
        BaseStateMachine machine = EffectParam.srcObject.GetComponent<BaseStateMachine>();
        if (machine == null)
        {
            return;
        }

        GameObject srcObject = machine.getCurrentSkin();

        if (srcObject == null)
        {
            return;
        }

        if (maxShadowCount > 0 && shadowCount >= maxShadowCount)
        {
            return;
        }

        if (EffectParam.srcObject && Initialize.mainCam)
        {
            //距离超过创建距离，不创建光效了。
            //为了保证上帝视角和切换回英雄附近都能正确看到光效，这里同时考虑光效到英雄以及光效到摄像机的距离

            Vector3 p0 = EffectParam.srcObject.transform.position;
            Vector3 p1 = Initialize.mainCam.transform.position;
            Vector3 p2 = Vector3.zero;
            if (EntityFactory.MainHero)
            {
                p2 = EntityFactory.MainHero.transform.position;
            }


            //不要Y
            p0.y = 0;
            p1.y = 0;
            p2.y = 0;

            Vector3 dis1 = p1 - p0;
            Vector3 dis2 = p2 - p0;
            float disSqr = LightingEffectFactory.CreateDistance * LightingEffectFactory.CreateDistance;

            if (EffectConfig.EnableDistanceOptimtized && (dis1.sqrMagnitude > disSqr) && (dis2.sqrMagnitude > disSqr))
            {
                return;
            }
        }


        //距离太近就不创建了
        if (shadowConfig.minDistance > 0.01f && shadowInstanceLists.Count > 0)
        {
            GameObject go = shadowInstanceLists[shadowInstanceLists.Count - 1].gameObject;
            if (go != null)
            {
                Vector3 lastPos = go.transform.position;
                Vector3 currentPos = srcObject.transform.position;
                Vector3 deta = currentPos - lastPos;
                if (deta.magnitude < shadowConfig.minDistance)
                {
                    return;
                }
            }
        }


        //复制skin
        GameObject newObject = ResNode.InstantiateRes(srcObject, true);
        newObject.transform.SetPosition(srcObject.transform.position);
        newObject.transform.rotation = srcObject.transform.rotation;
        Transform cloneTransform = newObject.transform;
        cloneTransform.parent = EntityFactory.EffectGroup.transform;
        newObject.name = srcObject.name + "(effect" + getID() + ")";

        //删除多余的组件
        deleteComponent(cloneTransform);

        CombinedBone[] cbs = cloneTransform.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone bone in cbs)
        {
            CombinedBone temp = bone;
            ResNode.DestroyRes(ref temp);
        }

        int childIndex = 0;
        while (childIndex < cloneTransform.childCount)
        {
            Transform child = cloneTransform.GetChild(childIndex);
            if (child.name == "Main" || child.name == "MAIN" || child.name == "main")
            {
                childIndex++;
            }
            else if (child.name.Length >= 5 && child.name.Substring(0, 5) == "group")
            {
                childIndex++;
            }
            else
            {
                child.parent = null;
                var temp1 = child.gameObject;
                ResNode.DestroyRes(ref temp1);
            }
        }

        if (shadowConfig.DisableWeapon == true) //删武器
        {
            Transform weapon = FindTransformRoot(cloneTransform, "W_RA");
            if (weapon != null)
            {
                var temp = weapon.gameObject;
                ResNode.DestroyRes(ref temp);
            }

            weapon = FindTransformRoot(cloneTransform, "W_LA");
            if (weapon != null)
            {
                var temp = weapon.gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }


        ShadowInstance newInstance = new ShadowInstance();
        newInstance.gameObject = newObject;
        newInstance.createTick = currentTick;
        newInstance.alpha = 255.0f;

        //改shader支持透明
        newInstance.renders.AddRange(newObject.GetComponentsInChildren<SkinnedMeshRenderer>());
        newInstance.renders.AddRange(newObject.GetComponentsInChildren<MeshRenderer>());
        foreach (Renderer r in newInstance.renders)
        {
            if (r != null)
            {
                if (shadowConfig.alphaMaterial != null)
                {
                    Material newMaterial = new Material(shadowConfig.alphaMaterial);
                    r.material = newMaterial;

                    if (shadowConfig.alphaAttributeType == LightingEffect_Shadow.AlphaAttributeType.Color && shadowConfig.alphaAttributeName != "")
                    {
                        matColor = r.material.GetColor(shadowConfig.alphaAttributeName);
                    }
                }
                else if (r.materials != null)
                {
                    if (r.materials.Length > 1) //多层材质叠加会变成白色
                    {
                        Material[] newMaterials = new Material[1];
                        newMaterials[0] = r.materials[0];
                        r.materials = newMaterials;
                    }
                    foreach (Material mat in r.materials)
                    {
                        mat.shader = AssetBundleManager.EntityAlphaShader;
                        if (mat.HasProperty("_Alpha"))
                        {
                            mat.SetFloat("_Alpha", shadowConfig.fStartAlpha);
                        }
                    }
                }
            }
        }

        shadowInstanceLists.Add(newInstance);
        shadowCount++;

        if (shadowConfig.minDistance > 0.01f && newInstance.gameObject != null)
        {
            newInstance.gameObject.SetActive(false);
        }

    }

    private void deleteComponent(Transform node)
    {
        foreach (Component comp in node.gameObject.GetComponents<Component>())
        {
            if (comp != node.transform)
            {
                Component temp = comp;
                ResNode.DestroyRes(ref temp);
            }
        }
    }


    public void updateShadow()
    {
        Vector3 currentPos = EffectParam.srcObject.transform.position;

        foreach (ShadowInstance shadow in shadowInstanceLists)
        {
            if (shadow.gameObject != null)
            {
                if (shadowConfig.minDistance > 0.01f)
                {
                    GameObject go = shadow.gameObject;
                    if (go.activeSelf == false)
                    {
                        Vector3 shadowPos = go.transform.position;
                        Vector3 deta = currentPos - shadowPos;
                        if (deta.magnitude > shadowConfig.minDistance)
                        {
                            go.SetActive(true);
                        }
                    }
                }


                int liveTick = currentTick - shadow.createTick;
                int fadeoutTick = liveTick - shadowConfig.nShadowMaintainTick;

                if (fadeoutTick >= shadowConfig.nShadowFadeoutTick)
                {
                    ResNode.DestroyRes(ref shadow.gameObject);
                    shadow.gameObject = null;
                    shadowCount--;
                }
                else
                {
                    float newAlpha = shadowConfig.fStartAlpha;
                    if (fadeoutTick > 0)
                    {
                        newAlpha = (1.0f - (float)(fadeoutTick) / (float)shadowConfig.nShadowFadeoutTick) * shadowConfig.fStartAlpha;
                    }

                    Vector3 shadowPos = shadow.gameObject.transform.position;
                    Vector3 deta = currentPos - shadowPos;
                    if (shadowConfig.minDistance > 0.01f && deta.magnitude < shadowConfig.minDistance) //与原模型距离太近不产生残影
                    {
                        newAlpha = 0.0f;
                    }

                    if (Mathf.Abs(newAlpha - shadow.alpha) > 0.01f)
                    {
                        shadow.alpha = newAlpha;
                        foreach (Renderer r in shadow.renders)
                        {
                            if (r == null)
                            {
                                continue;
                            }

                            if (shadowConfig.alphaMaterial != null && shadowConfig.alphaAttributeName != "")
                            {
                                if (r.material.HasProperty(shadowConfig.alphaAttributeName))
                                {
                                    if (shadowConfig.alphaAttributeType == LightingEffect_Shadow.AlphaAttributeType.Color)
                                    {
                                        matColor.a = newAlpha;
                                        r.material.SetColor(shadowConfig.alphaAttributeName, matColor);
                                    }
                                    else if (shadowConfig.alphaAttributeType == LightingEffect_Shadow.AlphaAttributeType.Int)
                                    {
                                        r.material.SetInt(shadowConfig.alphaAttributeName, (int)(newAlpha * 255));
                                    }
                                    else
                                    {
                                        r.material.SetFloat(shadowConfig.alphaAttributeName, newAlpha);
                                    }
                                }
                            }
                            else if ((r != null) && (r.materials != null))
                            {
                                foreach (Material mat in r.materials)
                                {
                                    if (mat.HasProperty("_Alpha"))
                                    {
                                        mat.SetFloat("_Alpha", newAlpha);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
