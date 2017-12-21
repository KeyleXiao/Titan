/*******************************************************************
** 文件名:	AssetBundleManager_Prefab_Dependenices.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，预制体部分_预制体依赖部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Runtime.InteropServices;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    private static void ReleaseDependenicesAssetBundleIfNeed(ResNode prefabResNdode)
    {
        if(null == prefabResNdode)
        {
            return;
        }

        if(!prefabResNdode.IsValid())
        {
            return;
        }

        PrefabDependenciesResNode dependenciesRes = null;

        if(m_PrefabDependenciesResNodeTable.TryGetValue(prefabResNdode.instanceID,out dependenciesRes))
        {
            foreach (ResNode resGUID in dependenciesRes.TextureResNode)
            {
                ReleaseAssetBundleIfNeed(resGUID);
            }

            foreach (ResNode resGUID in dependenciesRes.MaterialResNode)
            {
                ReleaseAssetBundleIfNeed(resGUID);
            }

            foreach (ResNode resGUID in dependenciesRes.FBXResNode)
            {
                ReleaseAssetBundleIfNeed(resGUID);
            }

            foreach (ResNode resGUID in dependenciesRes.AnimatonClipResNode)
            {
                ReleaseAssetBundleIfNeed(resGUID);
            }

            foreach (ResNode resGUID in dependenciesRes.AnimatorControllerResNode)
            {
                ReleaseAssetBundleIfNeed(resGUID);
            }
        }
    }

    private static PrefabDependenciesResNode GetPrefabDependenciesResNode(int instanceID)
    {      
        PrefabDependenciesResNode dependenciesRes = null;
        m_PrefabDependenciesResNodeTable.TryGetValue(instanceID, out dependenciesRes);
        return dependenciesRes;

    }

    /// <summary>
    /// 加载预制体的资源依赖
    /// </summary>
    /// <param name="pefabConfig"></param>
    /// <returns></returns>
    private static bool LoadDependenices(ResConfigData pefabConfig)
    {
        PrefabDependenciesResNode dependenciesRes;
        if (LoadDependenicesResource(pefabConfig, out dependenciesRes))
        {
            int InstanceID = GUIDToInstanceID(pefabConfig.AssetGUID);
            m_PrefabDependenciesResNodeTable.Add(InstanceID, dependenciesRes);
            return true;
        }
        UnLoadDependenicesResource(dependenciesRes);
        return false;
    }

    /// <summary>
    /// 卸载预制体的资源依赖
    /// </summary>
    /// <param name="prefabGUID"></param>
    private static void UnLoadDependenices(int prefabGUID)
    {
        //if (prefabGUID.Equals(string.Empty))
        //{
        //    return;
        //}

        if (prefabGUID < 0)
        {
            return;
        }


        //资源不存在,可以撤了
        PrefabDependenciesResNode dependenciesRes = null;
        if (!m_PrefabDependenciesResNodeTable.TryGetValue(prefabGUID, out dependenciesRes))
        {
            return;
        }


        UnLoadDependenicesResource(dependenciesRes);

        dependenciesRes = null;
        m_PrefabDependenciesResNodeTable[prefabGUID] = null;
        m_PrefabDependenciesResNodeTable.Remove(prefabGUID);
    }

    /// <summary>
    /// 只加载预制体的资源依赖
    /// </summary>
    /// <param name="pefabConfig"></param>
    /// <returns></returns>
    private static bool LoadDependenicesResource(ResConfigData pefabConfig,out PrefabDependenciesResNode dependenciesRes)
    {

        int guid = GUIDToInstanceID( pefabConfig.AssetGUID);
        dependenciesRes = null;

        //取得依赖配置表
        PrefabDependenicesResConfigData dependenciesConfig = null;
        if (!m_PrefabDependenciesConfigTable.TryGetValue(pefabConfig.AssetGUID, out dependenciesConfig))
        {
            Debug.LogWarning("预制体找不到对应的依赖配置:" + pefabConfig.AssetBundleName + "[" + pefabConfig.AssetName + "]");
            return false;
        }


        if (m_PrefabDependenciesResNodeTable.TryGetValue(guid, out dependenciesRes))
        {
            Debug.LogWarning("!!!!!!!!!!!!!!天哪撸，出超级大bug了。资源系统出大问题了。这个问题，赶紧找程序解决!!!!!!!!:" + pefabConfig.AssetBundleName + "[" + pefabConfig.AssetName + "]");
            return false;
        }

        dependenciesRes = new PrefabDependenciesResNode();

        /*加载顺序是不能乱的，要是乱的话。你可以试试,只要有一个加载失败就退出*/



        //先加载贴图,贴图要区分是Sprite还是普通的Texture
        foreach (string resGUID in dependenciesConfig.TextureResConfigDatas)
        {
            ResNode currRes = null;
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[贴图]");
                return false;
            }
            if (IsUIAssetBundle( tex.AssetBundleName))
            {
                currRes = GetSpriteResource(tex);
            }
            else
            {
                currRes = GetTextureResource(tex);
            }

            if (null != currRes && currRes.IsValid())
            {
                dependenciesRes.TextureResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[贴图],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }

        }

        //再加载材质
        foreach (string resGUID in dependenciesConfig.MaterialResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[Material]");
                return false;
            }

            ResNode currRes = GetMaterialResource(tex);
            if (null != currRes && currRes.IsValid())
            {
                dependenciesRes.MaterialResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[Material],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }

        //再加载AnimatorClip
        foreach (string resGUID in dependenciesConfig.AnimatonClipResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[AnimatorClip]");
                return false;
            }
            ResNode currRes = GetAnimationClipResource(tex);
            if (null != currRes && currRes.IsValid())
            {
                dependenciesRes.AnimatonClipResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[AnimatorClip],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }

        //再加载FBX
        foreach (string resGUID in dependenciesConfig.FBXResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[FBX]");
                return false;
            }
            ResNode currRes = GetFBXResource(tex);
            if (null != currRes && currRes.IsValid())
            {
                dependenciesRes.FBXResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[FBX],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }

        }



        //再加载AnimatorController
        foreach (string resGUID in dependenciesConfig.AnimatorControllerResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[AnimatorController]");
                return false;
            }

            ResNode currRes = GetAnimatorControllerResource(tex);
            if (null != currRes && currRes.IsValid())
            {
                dependenciesRes.AnimatorControllerResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[AnimatorController],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false; 
            }
        }

        return true;
    }

    private static void UnLoadDependenicesResource(PrefabDependenciesResNode dependenciesRes)
    {
        if (null == dependenciesRes)
        {
            return;
        }
        /*卸载顺序是不能乱的，要是乱的话。你可以试试*/

        bool Immediate = true;


        //再卸载AnimatorController
        foreach (ResNode tex in dependenciesRes.AnimatorControllerResNode)
        {
            ResNode temp = tex;
            DeleteAnimatorControllerResource(ref temp, Immediate);
        }

        //再卸载FBX
        foreach (ResNode tex in dependenciesRes.FBXResNode)
        {
            ResNode temp = tex;
            DeleteFBXResource(ref temp, Immediate);
        }

        //再卸载AnimatorClip
        foreach (ResNode tex in dependenciesRes.AnimatonClipResNode)
        {
            ResNode temp = tex;
            DeleteAnimationClipResource(ref temp, Immediate);
        }

        //再卸载材质
        foreach (ResNode tex in dependenciesRes.MaterialResNode)
        {
            ResNode temp = tex;
            DeleteMaterialResource(ref temp, Immediate);
        }

        //先卸载贴图,分一下是Sprite还是普通贴图
        foreach (ResNode tex in dependenciesRes.TextureResNode)
        {
            ResNode temp = tex;
            if (tex.AssetBundleName.EndsWith(AssetBundleManager.UIVariant))
            {
                DeleteSpriteResource(ref temp, Immediate);
            }
            else
            {
                DeleteTextureResource(ref temp, Immediate);
            }
        }
    }

    #region 异步加载
    private static bool LoadDependenicesAsync(ResConfigData pefabConfig)
    {
        PrefabDependenciesResNode dependenciesRes = null;
        List<ResNode> listNode = null;
        bool b = LoadDependenicesResourceAsync(pefabConfig, out dependenciesRes, out listNode);
        if (b)
        {
            int InstanceID = GUIDToInstanceID(pefabConfig.AssetGUID);
            m_PrefabDependenciesResNodeTable.Add(InstanceID, dependenciesRes);
            m_DependenicesRequestTable.Add(InstanceID, listNode);
            return true;
        }
        else
        {
            UnLoadDependenicesResourceAsync(dependenciesRes);
            return false;
        }

   
        
    }

    private static void UnLoadDependenicesAsync(int prefabGUID)
    {
        //if (prefabGUID.Equals(string.Empty))
        //{
        //    return;
        //}

        if(prefabGUID < 0)
        {
            return;
        }

        //资源不存在,可以撤了
        PrefabDependenciesResNode dependenciesRes = null;
        if (!m_PrefabDependenciesResNodeTable.TryGetValue(prefabGUID, out dependenciesRes))
        {
            return;
        }


        UnLoadDependenicesResourceAsync(dependenciesRes);

        dependenciesRes = null;
        m_PrefabDependenciesResNodeTable[prefabGUID] = null;
        m_PrefabDependenciesResNodeTable.Remove(prefabGUID);

        //对于预制体依赖，直接卸载异步的依赖资源的即可
        if(m_DependenicesRequestTable.ContainsKey(prefabGUID))
        {
            m_DependenicesRequestTable.Remove(prefabGUID);
        }
    }

    private static bool LoadDependenicesResourceAsync(ResConfigData pefabConfig,out PrefabDependenciesResNode dependenciesRes,
                                                      out List<ResNode> RequestResList)
    {
        RequestResList = null;
        dependenciesRes = null;

        //取得依赖配置表
        PrefabDependenicesResConfigData dependenciesConfig = null;
        int prefabID = GUIDToInstanceID(pefabConfig.AssetGUID);
        if (!m_PrefabDependenciesConfigTable.TryGetValue(pefabConfig.AssetGUID, out dependenciesConfig))
        {
            Debug.LogWarning("预制体找不到对应的依赖配置:" + pefabConfig.AssetBundleName + "[" + pefabConfig.AssetName + "]");
            return false;
        }

        /*先来检测一下，看看是不是已经存在了资源加载，因为这是有BUG的。
        * 理论上来说，没有被加载到，就不会有依赖资源被保存起来，如果发现目标资源没有被加载，但是
        * 他的依赖资源被加载的情况，这就是bug，主要问题就出在释放的时候，没有释放掉对应的依赖资源
        */


        if (m_PrefabDependenciesResNodeTable.TryGetValue(prefabID, out dependenciesRes))
        {
            Debug.LogWarning("!!!!!!!!!!!!!!天哪撸，出超级大bug了。资源系统出大问题了。这个问题，赶紧找程序解决!!!!!!!!:" + pefabConfig.AssetBundleName + "[" + pefabConfig.AssetName + "]");
            return false;
        }

        RequestResList = new List<ResNode>();
        dependenciesRes = new PrefabDependenciesResNode();
        List<ResNode> TempDependenceList = new List<ResNode>();
        /*加载顺序是不能乱的，要是乱的话。你可以试试,只要有一个加载失败就退出*/

        //先加载贴图,分 一下是Sprite还是普通图
        foreach (string resGUID in dependenciesConfig.TextureResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[贴图]");
                return false;
            }


            ResNode currRes = null;
            bool b = IsUIAssetBundle(tex.AssetBundleName);

            if (b)
            {
                currRes = GetSpriteResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            }
            else
            {
                currRes = GetTextureResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            }

            if (null != currRes)
            {
                RequestResList.Add(currRes);
                dependenciesRes.TextureResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[贴图],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }

        //再加载材质
        foreach (string resGUID in dependenciesConfig.MaterialResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[Material]");
                return false;
            }
            ResNode currRes = GetMaterialResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            if (null != currRes)
            {

                RequestResList.Add(currRes);
                dependenciesRes.MaterialResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[Material],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }


        //再加载AnimatorClip
        foreach (string resGUID in dependenciesConfig.AnimatonClipResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[AnimatorClip]");
                return false;
            }
            ResNode currRes = GetAnimationClipResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            if (null != currRes)
            {

                RequestResList.Add(currRes);
                dependenciesRes.AnimatonClipResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[AnimatorClip],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }


        //再加载FBX
        foreach (string resGUID in dependenciesConfig.FBXResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[FBX]");
                return false;
            }
            ResNode currRes = GetFBXResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            if (null != currRes)
            {

                RequestResList.Add(currRes);
                dependenciesRes.FBXResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[FBX],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }

        }


        //再加载AnimatorController
        foreach (string resGUID in dependenciesConfig.AnimatorControllerResConfigDatas)
        {
            ResConfigData tex = GetDependenicesConfig(resGUID);
            if (null == tex)
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，依赖表中找不到资源，类型:[AnimatorController]");
                return false;
            }
            ResNode currRes = GetAnimatorControllerResourceAsync(tex, AsyncLoadDependenicesCallBack, null, pefabConfig.priority);
            if (null != currRes)
            {
                RequestResList.Add(currRes);
                dependenciesRes.AnimatorControllerResNode.Add(currRes);
            }
            else
            {
                Debug.LogWarning(pefabConfig.AssetName + "-预制体依赖资源加载失败，类型:[AnimatorController],资源路径:" + tex.AssetBundleName + "--" + tex.AssetName);
                return false;
            }
        }
        return true;
    }

    private static void UnLoadDependenicesResourceAsync(PrefabDependenciesResNode dependenciesRes)
    {
        if(null == dependenciesRes)
        {
            return;
        }

        /*卸载顺序是不能乱的，要是乱的话。你可以试试*/
        bool Immediate = true;

        //再卸载AnimatorController
        foreach (ResNode tex in dependenciesRes.AnimatorControllerResNode)
        {
            ResNode temp = tex;

            DeleteAnimatorControllerResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
        }

        //再卸载FBX
        foreach (ResNode tex in dependenciesRes.FBXResNode)
        {
            ResNode temp = tex;

            DeleteFBXResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
        }

        //再卸载AnimatorClip
        foreach (ResNode tex in dependenciesRes.AnimatonClipResNode)
        {
            ResNode temp = tex;

            DeleteAnimationClipResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
        }

        //再卸载材质
        foreach (ResNode tex in dependenciesRes.MaterialResNode)
        {
            ResNode temp = tex;
            DeleteMaterialResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
        }

        //先卸载贴图
        foreach (ResNode tex in dependenciesRes.TextureResNode)
        {
            ResNode temp = tex;
            if (IsUIAssetBundle( tex.AssetBundleName))
            {
                DeleteSpriteResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
            }
            else
            {
                DeleteTextureResourceAsync(ref temp, AsyncLoadDependenicesCallBack, Immediate);
            }

        }
    }

    private static void AsyncLoadDependenicesCallBack(ResConfigData config, ResNode res,
                                                        System.Object userData)
    {
        if(null != res && !res.IsValid())
        {
            Debug.LogWarning(config.AssetName + "-预制体依赖资源加载失败，类型:"+ res.assetType.ToString() + ",资源路径:" + config.AssetBundleName + "--" + config.AssetName);
        }
    }
    #endregion
}
