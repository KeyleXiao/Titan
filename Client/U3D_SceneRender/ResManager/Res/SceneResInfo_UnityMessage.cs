/*******************************************************************
** 文件名:	SceneResInfo_UnityMessage.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息
** 应  用:   场景资源信息，用于做场景资源管理,接受Unity消息部分

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

    private bool bInitCheckSupportFading = false;

    private bool bInit = false;

    public bool CheckSupportFading()
    {
        if(!hostRender)
        {
            return false;
        }
        if (hostRender.sharedMaterials == null)
        {
            return false;
        }
        if(!bInitCheckSupportFading)
        {
            foreach (SceneMaterialConfig r in ConfigDataList)
            {
                //没有材质球的，要减一
                if (!r.SharedMaterial)
                {
                    HostMainTexCount-= r.TextureConfigList.Count;
                    HostTexCount -= r.TextureConfigList.Count;
                }
                else
                {
                    //支持优化的材质球
                    if (r.isSupportOptimtizedMat)
                    {
                        supportOptimitzedMatList.Add(r.SharedMaterial);
                    }
                }
            }
            bInitCheckSupportFading = true;
        }
        return supportOptimitzedMatList.Count == hostRender.sharedMaterials.Length;

    }

    public void Init(Dictionary<Material, Material> FadeMatTable, Camera sceneCamera)
    {
        if (bInit)
        {
            return;
        }

        //初始化Bounds信息
        mainCameraTransform = sceneCamera.transform;

        if (!hostRender ||
            null == ConfigDataList ||
            !mainCameraTransform
            )
        {
            enabled = false;
            return;
        }

        SetMatTable(FadeMatTable);
        ReCalculateBounds();

        if (supportOptimitzedMatList.Count != 0)
        {
            if(supportOptimitzedMatList.Count != hostRender.sharedMaterials.Length)
            {
                Debug.LogWarning(hostRender.gameObject.name + ":supportOptimitzedMatList ！= hostRender.sharedMaterials");
            }
        }
        ReAssignConfingMaterial();
        CheckSupportFading();
        InitRendering();

        if (!AlwaysUnOptimitzedMemory)
        {
            NeedToPreLoadedAllTexCount += HostMainTexCount;
            LoadMainTex();

        }
        else
        {
            NeedToPreLoadedAllTexCount += HostTexCount;
            LoadMainTex();
            LoadOtherTex();

        }
        if (AlwaysVisible)
        {
            UnOptimitzedRendering(null);
        }
        bInit = true;
    }


    public void SetMatTable(Dictionary<Material, Material> FadeMatTable)
    {
        currFadeMatTable = FadeMatTable;
    }
  
  
    public override void OnUpdate(Camera sceneCamera)
    {
        if(!enabled)
        {
            return;
        }

        //只有总是可见的物体才需要判断这个
        if(AlwaysVisible)
        {
            if (Contains_Memory() && !bOptimitzedRendering)
            {
                UnOptimitzedMemory();
            }
            else
            {
                OptimitzedMemory();
            }
        }

        CheckOcclus();
        //if (Contains_Rendering())
        //{
        //    UnOptimitzedRendering();
        //}
        //else
        //{
        //    OptimitzedRendering();
        //}

    }

    void ReCalculateBounds()
    {

        //if (hostRender.gameObject.name.Equals("moba_001_tyst28"))
        //{
        //    Debug.LogWarning(RenderBoundsMin + "," + RenderBoundsMax);
        //}
        ////算出对角线
        //Vector3 a = b.max - b.min;

        ////用Y做为标尺
        //float vol =  a.y;

        //a = Vector3.one * 10;
        //float standradlength =  a.y;

        //float scale = vol / standradlength;

        ////缩放的最大，最小值
        //scale = Mathf.Clamp(scale, 0.2f, 2.0f);

        //OptimitzedDistance *= scale;

        //如果总是可见，内存优化距离为80
        if(AlwaysVisible)
        {
            MemoryOptimitzedDistance = 80;
        }

        MemoryBounds_Min = RenderBoundsMin;
        MemoryBounds_Min.x -= MemoryOptimitzedDistance;
        MemoryBounds_Min.y -= MemoryOptimitzedDistance;
        MemoryBounds_Min.z -= MemoryOptimitzedDistance;


        MemoryBounds_Max = RenderBoundsMax;
        MemoryBounds_Max.x += MemoryOptimitzedDistance;
        MemoryBounds_Max.y += MemoryOptimitzedDistance;
        MemoryBounds_Max.z += MemoryOptimitzedDistance;

        RenderingBounds_Min = RenderBoundsMin;
        RenderingBounds_Min.x -= RenderingOptimitzedDistance;
        RenderingBounds_Min.y -= RenderingOptimitzedDistance;
        RenderingBounds_Min.z -= RenderingOptimitzedDistance;


        RenderingBounds_Max = RenderBoundsMax;
        RenderingBounds_Max.x += RenderingOptimitzedDistance;
        RenderingBounds_Max.y += RenderingOptimitzedDistance;
        RenderingBounds_Max.z += RenderingOptimitzedDistance;

        if(CorrectRefPos)
        {
            RefPos = hostRender.bounds.center;
        }
        else
        {
            RefPos = transform.position;
        }
    }

    public override void OnClear(Camera sceneCamera)
    {
        ClearMemory();
        ClearRendering();
        ClearOcclusCulling();
        hostResManager = null;
        hostTreeNode = null;
        hostRender = null;
        mainCameraTransform = null;
    }

}
