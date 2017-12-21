/*******************************************************************
** 文件名:	SceneResInfo_Main.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息
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

    #region  成员

    [System.NonSerialized]
    public SceneResInfoManager.QuadTreeNode hostTreeNode = null;

    [System.NonSerialized]
    public SceneResInfoManager hostResManager = null;

    /// <summary>
    /// 所属的渲染器
    /// </summary>
    public Renderer hostRender = null;

    /// <summary>
    /// 不能取hostRender.Bounds，打包之后的场景有可能取出来的是0,轴对称的Bounds，经过旋转后，是不会准确的显示出大小
    /// </summary>
    public Vector3 RenderBoundsMin = Vector3.zero;

    /// <summary>
    /// 不能取hostRender.Bounds，打包之后的场景有可能取出来的是0,轴对称的Bounds，经过旋转后，是不会准确的显示出大小
    /// </summary>
    public Vector3 RenderBoundsMax = Vector3.zero;

    /// <summary>
    /// 材质球配置链表
    /// </summary>
    public List<SceneMaterialConfig> ConfigDataList = new List<SceneMaterialConfig>();

    /// <summary>
    /// 没有优化的材质球，用一个优化计数，因为会有多个渲染器使用同一个材质球 材质球--优化计数
    /// </summary>
    public static Dictionary<Material, int> UnOptimitzedMaterials = new Dictionary<Material, int>();

    
    /// <summary>
    /// 主贴图纹理
    /// </summary>
    private const string MainTexKeyWorld = "_MainTex";

    /// <summary>
    /// GUID 与加载的资源
    /// </summary>
    private Dictionary<SceneTextureConfigData, ResNode> OtherLoadedResTable = new Dictionary<SceneTextureConfigData, ResNode>();

    private Dictionary<SceneTextureConfigData, ResNode> MainTexLoadedResTable = new Dictionary<SceneTextureConfigData, ResNode>();

    private Transform mainCameraTransform = null;

    /// <summary>
    /// 使用Render.center的位置，不使用Transform的位置，这样做的好处是，物体的坐标没必要再中心点
    /// </summary>
    private static bool CorrectRefPos = true;

   /// <summary>
   /// 参考点,各种计算所使用的点
   /// </summary>
    public Vector3 RefPos = Vector3.zero;
    #endregion

    public void LoadOtherTex()
    {
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                if (!bPreaLoaded)
                {
                    currPreLoadedTexCount++;
                }
                continue;
            }
            foreach (SceneTextureConfigData rt in r.TextureConfigList)
            {
                ResNode nod = null;

                //主贴图是不会释放的,先看看是不是主贴图,如果是主贴图的话，看看是不是加载了，是的话，就跳过
                bool isMainTex = rt.ShaderKeyWordName.Equals(MainTexKeyWorld);
                if(isMainTex)
                {
                    continue;
                }

                //如果是永远不优化内存，也只会加载一次，后面就不加载了
                OtherLoadedResTable.TryGetValue(rt, out nod);
                if (null != nod)
                {
                    continue;
                }

                nod = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Texture,rt, this, r);

                if (!ResNode.isNullOrEmpty(nod))
                {
                     OtherLoadedResTable.Add(rt, nod);        

                    //不优化内存的需要增加一下引用.避免多个引用删除
                    if(AlwaysUnOptimitzedMemory)
                    {
                        if (!UnOptimitzedMaterials.ContainsKey(r.SharedMaterial))
                        {
                            UnOptimitzedMaterials.Add(r.SharedMaterial, 1);
                        }
                        else
                        {
                            UnOptimitzedMaterials[r.SharedMaterial]++;
                        }

                    }
                }
                else
                {
                    if (!bPreaLoaded)
                    {
                        currPreLoadedTexCount++;
                    }
                }
            }
        }
    }

    public void LoadMainTex()
    {
        foreach (SceneMaterialConfig r in ConfigDataList)
        {
            if (!r.SharedMaterial)
            {
                if (!bPreaLoaded)
                {
                    currPreLoadedTexCount++;
                }
                continue;
            }
            
            foreach (SceneTextureConfigData rt in r.TextureConfigList)
            {
                if (rt.ShaderKeyWordName.Equals(MainTexKeyWorld))
                {
                    ResNode nod = null;

                    //主贴图是不会释放的,先看看是不是主贴图,如果是主贴图的话，看看是不是加载了，是的话，就跳过
                    MainTexLoadedResTable.TryGetValue(rt, out nod);
                    if (null != nod)
                    {
                        continue;
                    }
                    nod = AssetBundleManager.GetAssets(AssetType.Asset_Texture,rt);
                    if (!ResNode.isNullOrEmpty(nod))
                    {
                        ResLoadCallBack(rt, nod, r);
                        MainTexLoadedResTable.Add(rt, nod);
                    }
                    else
                    {
                        if (!bPreaLoaded)
                        {
                            currPreLoadedTexCount++;
                        }
                    }
                }
            }
        }
        
    }

}
