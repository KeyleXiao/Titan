/*******************************************************************
** 文件名:	SceneResInfo_Memory.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息,内存部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfo : SceneInfo, IAsyncResLoadHandler
{
    /// <summary>
    /// 这个实例有多少个maintex
    /// </summary>
    public int HostMainTexCount = 0;

    /// <summary>
    /// 这个实例有多少张贴图
    /// </summary>
    public int HostTexCount = 0;

    /// <summary>
    /// 优化距离，这个是根据物体的包围盒，再加上这个距离来做的。
    /// </summary>
    private float MemoryOptimitzedDistance = 30.0f;

    /// <summary>
    /// 是否被优化了，默认是已经优化了的，因为bake是优化了。
    /// </summary>
    public bool bOptimitzedMenory = true;

    private Vector3 MemoryBounds_Min = Vector3.zero;
    private Vector3 MemoryBounds_Max = Vector3.zero;
    private Dictionary<Material, Material> currFadeMatTable = null;

    static string EmissionMapKey = "_EmissionMap";
    static string MetallicMapKey = "_MetallicGlossMap";
    static string SpecMapKey = "_SpecGlossMap";
    private static void ResetEmissonTexture(Material mat)
    {
        if(!mat)
        {
            return;
        }
        mat.SetTexture(EmissionMapKey, Texture2D.blackTexture);
    }

    private static void ResetSurfaceDefineTexture(Material mat)
    {
        if (!mat)
        {
            return;
        }
        mat.SetTexture(MetallicMapKey, AssetBundleManager.CommonBlack_AlphaZeroTexture);
        mat.SetTexture(SpecMapKey, AssetBundleManager.CommonBlack_AlphaZeroTexture);
    }
    /// <summary>
    /// 优化内存
    /// </summary>
    public void OptimitzedMemory()
    {
        if (bOptimitzedMenory || AlwaysUnOptimitzedMemory)
        {
            return;
        }

        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                continue;
            }
            if (UnOptimitzedMaterials.ContainsKey(r.SharedMaterial))
            {
                int refCount = --UnOptimitzedMaterials[r.SharedMaterial];
                if (refCount <= 0)
                {
                    Material instanceFadeInMat = null;
                    Material instanceFadeOutMat = null;
                    Material orginalInstanceMat = null;

                    if (currFadeMatTable != null && currFadeMatTable.ContainsKey(r.SharedMaterial))
                    {
                        instanceFadeInMat = currFadeMatTable[r.SharedMaterial];
                    }

                    Orginal_AlphaInstanceTable.TryGetValue(r.SharedMaterial, out orginalInstanceMat);
                    foreach (SceneTextureConfigData rt in r.TextureConfigList)
                    {
                        if (!rt.ShaderKeyWordName.Equals(MainTexKeyWorld))
                        {
                            if (rt.ShaderKeyWordVariantName.Count != 0)
                            {
                                foreach (string var in rt.ShaderKeyWordVariantName)
                                {
                                    if (instanceFadeInMat)
                                    {
                                        instanceFadeInMat.DisableKeyword(var);
                                    }
                                    if (instanceFadeOutMat)
                                    {
                                        instanceFadeOutMat.DisableKeyword(var);
                                    }
                                    if (orginalInstanceMat)
                                    {
                                        orginalInstanceMat.DisableKeyword(var);
                                    }
                                    r.SharedMaterial.DisableKeyword(var);
                                }

                            }
                            if (instanceFadeInMat)
                            {
                                instanceFadeInMat.SetTexture(rt.ShaderKeyWordName, null);
                            }
                            if (instanceFadeOutMat)
                            {
                                instanceFadeOutMat.SetTexture(rt.ShaderKeyWordName, null);
                            }
                            if (orginalInstanceMat)
                            {
                                orginalInstanceMat.SetTexture(rt.ShaderKeyWordName, null);
                            }

                            r.SharedMaterial.SetTexture(rt.ShaderKeyWordName, null);
                        }
                    }
                    UnOptimitzedMaterials.Remove(r.SharedMaterial);
                    ResetEmissonTexture(r.SharedMaterial);
                    ResetSurfaceDefineTexture(r.SharedMaterial);
                }
            }

        }
        

        foreach (ResNode res in OtherLoadedResTable.Values)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssetsAsync(ref temp, this,false);
        }
        OtherLoadedResTable.Clear();
        bOptimitzedMenory = true;
    }

    public void ClearMemory()
    {
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                continue;
            }
            if (UnOptimitzedMaterials.ContainsKey(r.SharedMaterial))
            {
                int refCount = --UnOptimitzedMaterials[r.SharedMaterial];
                if (refCount <= 0)
                {
                    foreach (SceneTextureConfigData rt in r.TextureConfigList)
                    {
                        if (rt.ShaderKeyWordVariantName.Count != 0)
                        {
                            foreach (string variant in rt.ShaderKeyWordVariantName)
                            {
                                r.SharedMaterial.DisableKeyword(variant);
                            }

                        }

                        r.SharedMaterial.SetTexture(rt.ShaderKeyWordName, null);
                    }
                    UnOptimitzedMaterials.Remove(r.SharedMaterial);
                    ResetEmissonTexture(r.SharedMaterial);
                    ResetSurfaceDefineTexture(r.SharedMaterial);
                }
            }
            else
            {
                foreach (SceneTextureConfigData rt in r.TextureConfigList)
                {
                    if (rt.ShaderKeyWordVariantName.Count != 0)
                    {
                        foreach (string variant in rt.ShaderKeyWordVariantName)
                        {
                            r.SharedMaterial.DisableKeyword(variant);
                        }

                    }

                    r.SharedMaterial.SetTexture(rt.ShaderKeyWordName, null);
                }
                UnOptimitzedMaterials.Remove(r.SharedMaterial);
                ResetEmissonTexture(r.SharedMaterial);
                ResetSurfaceDefineTexture(r.SharedMaterial);

            }

        }
        foreach (ResNode res in OtherLoadedResTable.Values)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssetsAsync(ref temp, this, true);
        }
        OtherLoadedResTable.Clear();

        foreach (ResNode res in MainTexLoadedResTable.Values)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssets(ref temp, true);
        }
        MainTexLoadedResTable.Clear();
        ConfigDataList.Clear();
    }

    public void ResetMemory()
    {
        bOptimitzedMenory = true;
        currFadeMatTable = null;
        if (OtherLoadedResTable.Count <= 0 || AlwaysUnOptimitzedMemory)
        {
            return;
        }
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                continue;
            }
            if (UnOptimitzedMaterials.ContainsKey(r.SharedMaterial))
            {
                int refCount = --UnOptimitzedMaterials[r.SharedMaterial];
                if (refCount <= 0)
                {
                    foreach (SceneTextureConfigData rt in r.TextureConfigList)
                    {
                        if (!rt.ShaderKeyWordName.Equals(MainTexKeyWorld))
                        {
                            Material orginalInstanceMat = null;

                            Orginal_AlphaInstanceTable.TryGetValue(r.SharedMaterial, out orginalInstanceMat);
                            if (rt.ShaderKeyWordVariantName.Count != 0)
                            {
                                foreach (string var in rt.ShaderKeyWordVariantName)
                                {
                                    if (orginalInstanceMat)
                                    {
                                        orginalInstanceMat.DisableKeyword(var);
                                    }
                                    r.SharedMaterial.DisableKeyword(var);
                                }

                            }
                            if (orginalInstanceMat)
                            {
                                orginalInstanceMat.SetTexture(rt.ShaderKeyWordName, null);
                            }

                            r.SharedMaterial.SetTexture(rt.ShaderKeyWordName, null);
                        }
                    }
                    ResetEmissonTexture(r.SharedMaterial);
                    ResetSurfaceDefineTexture(r.SharedMaterial);
                    UnOptimitzedMaterials.Remove(r.SharedMaterial);
                }
            }
           
        }
        foreach (ResNode res in OtherLoadedResTable.Values)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssetsAsync(ref temp, this, false);
        }
        OtherLoadedResTable.Clear();
    }

    /// <summary>
    /// 不优化内存
    /// </summary>
    public void UnOptimitzedMemory()
    {
        //可见才能不优化
        if (!bOptimitzedMenory || bOptimitzedRendering || AlwaysUnOptimitzedMemory)
        {
            return;
        }
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                continue;
            }

            if (!UnOptimitzedMaterials.ContainsKey(r.SharedMaterial))
            {
                UnOptimitzedMaterials.Add(r.SharedMaterial, 1);
            }
            else
            {
                UnOptimitzedMaterials[r.SharedMaterial]++;
            }

            foreach (SceneTextureConfigData rt in r.TextureConfigList)
            {
                if (!rt.ShaderKeyWordName.Equals(MainTexKeyWorld))
                {
                    ResNode re = null;
                    if (!OtherLoadedResTable.ContainsKey(rt))
                    {
                        re = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Texture, rt, this, r);
                        if (!ResNode.isNullOrEmpty(re))
                        {
                            OtherLoadedResTable.Add(rt, re);
                        }
                    }
                }
            }
            
        }

        bOptimitzedMenory = false;
    }


    /// <summary>
    /// 资源加载回调
    /// </summary>
    /// <param name="cof"></param>
    /// <param name="res"></param>
    /// <param name="UserDataObj"></param>
    public void ResLoadCallBack(ResConfigData cof, ResNode res,System.Object UserDataObj)
    {
        SceneTextureConfigData rt = cof as SceneTextureConfigData;
        SceneMaterialConfig r = UserDataObj as SceneMaterialConfig;
        if (null == rt)
        {
            return;
        }

        if (!bPreaLoaded)
        {
            currPreLoadedTexCount++;
        }

        //加载资源失败了，回调为null,计数器也要+1
        if (ResNode.isNullOrEmpty(res))
        {
            return;
        }

        if (!r.SharedMaterial)
        {
            return;
        }

        Texture tex = res.ConvertRes<Texture>();
        Material instanceFadeMat = null;
        Material orginalInstanceMat = null;
        if (currFadeMatTable != null && currFadeMatTable.ContainsKey(r.SharedMaterial))
        {
            instanceFadeMat = currFadeMatTable[r.SharedMaterial];
        }


        Orginal_AlphaInstanceTable.TryGetValue(r.SharedMaterial, out orginalInstanceMat);

        if (instanceFadeMat)
        {
            instanceFadeMat.SetTexture(rt.ShaderKeyWordName, tex);
        }
        if (orginalInstanceMat)
        {
            orginalInstanceMat.SetTexture(rt.ShaderKeyWordName, tex);
        }

        r.SharedMaterial.SetTexture(rt.ShaderKeyWordName, tex);

        if (rt.ShaderKeyWordVariantName.Count != 0)
        {
            foreach (string variant in rt.ShaderKeyWordVariantName)
            {
                if (instanceFadeMat)
                {
                    instanceFadeMat.EnableKeyword(variant);
                }
                if (orginalInstanceMat)
                {
                    orginalInstanceMat.EnableKeyword(variant);
                }
                r.SharedMaterial.EnableKeyword(variant);
            }

        }
    }

    /// <summary>
    /// 判断摄像机的位置
    /// </summary>
    /// <returns></returns>
    bool Contains_Memory()
    {

        Vector3 pos = mainCameraTransform.position;
        return Contains_Memory(pos);
    }

    /// <summary>
    /// 根据给的位置来判断
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    bool Contains_Memory(Vector3 pos)
    {
        
        if (pos.x > MemoryBounds_Min.x && pos.y > MemoryBounds_Min.y && pos.z > MemoryBounds_Min.z &&
            pos.x < MemoryBounds_Max.x && pos.y < MemoryBounds_Max.y && pos.z < MemoryBounds_Max.z)
        {
            return true;
        }
        return false;
    }


    /// <summary>
    /// 初始化阶段的判断，一开始，只需要加载周围5m内的物体即可
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    bool Contains_Start(Vector3 pos)
    {
        Vector3 tempBoundsMin = MemoryBounds_Min;
        Vector3 tempBoundsMax = MemoryBounds_Max;

        tempBoundsMax.x -= MemoryOptimitzedDistance;
        tempBoundsMax.y -= MemoryOptimitzedDistance;
        tempBoundsMax.z -= MemoryOptimitzedDistance;

        tempBoundsMin.x += MemoryOptimitzedDistance;
        tempBoundsMin.y += MemoryOptimitzedDistance;
        tempBoundsMin.z += MemoryOptimitzedDistance;

        float distance = 5;
        tempBoundsMax.x += distance;
        tempBoundsMax.y += distance;
        tempBoundsMax.z += distance;

        tempBoundsMin.x -= distance;
        tempBoundsMin.y -= distance;
        tempBoundsMin.z -= distance;

        if (pos.x > tempBoundsMin.x && pos.y > tempBoundsMin.y && pos.z > tempBoundsMin.z &&
            pos.x < tempBoundsMax.x && pos.y < tempBoundsMax.y && pos.z < tempBoundsMax.z)
        {
            return true;
        }
        return false;
    }

    void ReAssignConfingMaterial()
    {
        if(AssetBundleManager.isOnRunTimeMode)
        {
            return;
        }
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            r.SharedMaterial = hostResManager.GetRenderMaterial(r.SharedMaterial);
        }
    }
}
