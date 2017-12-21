/*******************************************************************
** 文件名:	SceneResInfo_OcclusCulling.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息,遮挡剔出部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfo : SceneInfo
{
    /// <summary>
    /// 遮挡剔出碰撞盒
    /// </summary>
    public Collider OcclusCullingCollider = null;

    public bool isOccluder = false;

    [System.NonSerialized]
    public int OcclusVisible = 0;

    [System.NonSerialized]
    public int NotCullDataIndex = -1;

    [System.NonSerialized]
    public bool occlusFadingOut = false;

    [System.NonSerialized]
    public bool occlusFadingIn = false;

    [System.NonSerialized]
    public Effect.SceneFadeInOutMaterialFloat occlusFadInstance = null;

    [System.NonSerialized]
    public Material[] OcclusInstaneMaterial = null;



    private bool bCullEnable = true;

    private int LastCullVisible = int.MaxValue;


    void ClearOcclusCulling()
    {
        OcclusInstaneMaterial = null;
    }

    public void UpdateOcclusFadeOut(Material[] matList)
    {
        if (null != matList)
        {
            int length = hostRender.sharedMaterials.Length;
            if (matList.Length != length)
            {
                Debug.LogError("OCC-SceneResInfo:cout != length" + name);
                return;
            }
            hostRender.materials = matList;
        }
    }

    public void UpdateOcclusFadeIn(Material[] matList)
    {
        //淡入的话要立刻更新，要不然的话，会看不见淡入的效果
        UpdateVisible();
        if (null != matList)
        {
            int length = hostRender.sharedMaterials.Length;
            if (matList.Length != length)
            {
                Debug.LogError("OCC-SceneResInfo:cout != length" + name);
                return;
            }
            hostRender.materials = matList;
        }
    }

    public void DisableCullingCheck()
    {
        if (!bCullEnable)
        {
            return;
        }
        bCullEnable = false;
        if (OcclusCullingCollider)
        {
            OcclusCullingCollider.enabled = false;
        }
    }

    public void EnableCullingCheck()
    {
        if (bCullEnable)
        {
            return;
        }

        bCullEnable = true;
        if (OcclusCullingCollider)
        {
            OcclusCullingCollider.enabled = true;
        }
    }

    public void ApplyCullVisible(bool NotFade = false)
    {
        if(LastCullVisible != OcclusVisible)
        {
            LastCullVisible = OcclusVisible;
            if(NotFade)
            {
                UpdateVisible();
            }
            else
            {
                hostResManager.PushToOccFadeList(this);
            }
            
        }
    }

    public void Cull()
    {
        if (OcclusVisible == 0)
        {
            return ;
        }
        if(OcclusCullingCollider)
        {

            OcclusVisible = 0;
        }
    }

    public void NotCull()
    {
        if(OcclusVisible == 1)
        {
            return;
        }
        if (OcclusCullingCollider)
        {

            OcclusVisible = 1;
        }
    }



    bool getOcclusVisible()
    {
        if (!hostResManager)
        {
            return false;
        }
        return OcclusVisible == 1 || !OcclusCullingCollider ||
               !SceneResInfoManager.GoablEnabledOcclusCulling || !hostResManager.enabledOcclusCulling || isOccluder;
    }
}
