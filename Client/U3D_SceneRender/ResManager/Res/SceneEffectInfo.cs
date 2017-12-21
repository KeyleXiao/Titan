/*******************************************************************
** 文件名:	SceneEffectInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景特效信息
** 应  用:   场景特效信息

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneEffectInfo : SceneInfo
{

    /// <summary>
    /// 是否被优化了，默认是优化了的，因为bake是优化了。
    /// </summary>
    public bool bOptimitzed = true;

    public bool isAlwaysNormal = false;

    public Vector3 EffectPosition = Vector3.zero;
    public Quaternion EffectRoation = Quaternion.identity;
    public Vector3 EffectScale = Vector3.one;
    public Transform EffectParent = null;

    public string assetGUID = string.Empty;

    private Transform mainCameraTransform = null;

    private Effect.EffectNodeBase hostNode = null;

    private ResNode EffectRes = null;
    private GameObject effectGo = null;

    public override void OnAwake(Camera sceneCamera)
    {

        EffectRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, assetGUID, true);
        

        if(null == EffectRes)
        {
            return;
        }

        hostNode = EffectRes.InstanceMainRes<Effect.EffectNodeBase>();
        if (!hostNode)
        {
            AssetBundleManager.DeleteAssets(ref EffectRes, true);
            enabled = false;
            return;
        }
        effectGo = hostNode.gameObject;
        effectGo.transform.SetParent(EffectParent);
        effectGo.transform.position = EffectPosition;
        effectGo.transform.rotation = EffectRoation;
        effectGo.transform.localScale = EffectScale;
        hostNode.PlayOnAwake = false;
        hostNode.isStaticEffect = false;
        hostNode.Init();
        hostNode.StopEffect();
        Effect.EffectNodeBase.ResigterEffectQualityChange(OnEffectQualityChange);
    }

    void OnEffectQualityChange()
    {
        if (!effectGo)
        {
            return;
        }
        if(!bOptimitzed)
        {
            hostNode.StopEffect();
            hostNode.PlayEffect();
        }
    }

    public override void OnStart(Camera sceneCamera)
    {
        //初始化Bounds信息
        mainCameraTransform = sceneCamera.transform;
        if (!hostNode)
        {
            enabled = false;
            return;
        }
    }

    public override void OnClear(Camera sceneCamera)
    {
        if (!effectGo)
        {
            return;
        }
        hostNode = null;
        Effect.EffectNodeBase.UnResigterEffectQualityChange(OnEffectQualityChange);
        GameObject.DestroyObject(effectGo);
        AssetBundleManager.DeleteAssets(ref EffectRes,true);
    }

    public override void OnUpdate(Camera sceneCamera)
    {
        if (!effectGo)
        {
            return;
        }
        if (Contains(sceneCamera))
        {
            UnOptimitzed();
        }
        else
        {
            Optimitzed();
        }

    }

    public void ResetSceneEffect()
    {
        bOptimitzed = true;
        if (!effectGo)
        {
            return;
        }

        hostNode.StopEffect();
        
    }

    /// <summary>
    /// 优化
    /// </summary>
    public void Optimitzed()
    {
        if (bOptimitzed)
        {
            return;
        }
        if (!effectGo)
        {
            return;
        }
        if(isAlwaysNormal)
        {
            return;
        }
        hostNode.StopEffect();
        
        bOptimitzed = true;
    }


    /// <summary>
    /// 不优化
    /// </summary>
    public void UnOptimitzed()
    {
        //可见才能不优化
        if (!bOptimitzed)
        {
            return;
        }

        if (!effectGo)
        {
            return;
        }
        hostNode.PlayEffect(false,-1, Vector3.zero, Vector3.zero);
        bOptimitzed = false;
    }

    /// <summary>
    /// 判断摄像机
    /// </summary>
    /// <returns></returns>
    bool Contains(Camera cam)
    {
        if (!effectGo)
        {
            return false;
        }

        //位置往前移3m，再判断
        Vector3 hostpos = transform.position + mainCameraTransform.forward * 3.0f;

        bool isOnView = isOnCameraView(cam,hostpos);
        return isOnView;
    }

    private static bool isOnCameraView(Camera cam, Vector3 pos)
    {
        if (!cam)
        {
            return false;
        }

        Vector3 ViewPos = cam.WorldToViewportPoint(pos);
        if (ViewPos.x < 0 || ViewPos.x > 1 ||
            ViewPos.y < 0 || ViewPos.y > 1 ||
                ViewPos.z < 0)
            return false;
        return true;
    }
}
