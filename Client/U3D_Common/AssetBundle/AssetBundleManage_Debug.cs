/*******************************************************************
** 文件名:	AssetBundleManager_FBX.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，API部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
//#define OpenAssetDebug
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Runtime.InteropServices;


public sealed partial class AssetBundleManager : MonoBehaviour
{
     #region DrawDebugInfo

    public static void DrawDebugInfo()
    {
#if OpenAssetDebug
        DrawDebugInfoImpl();
#endif
    }

#if OpenAssetDebug
    public class AsssetTextureDebugNode
    {
        public string AssetPath = string.Empty;
        public string AssetName = string.Empty;

        public float MemoryUsedInMB = 0.0f;
        public int RefCount = 0;
        public bool minmap = false;

        public bool isUIPacked = false;
        public float PackedSize = 0.0f;
        public string PackedName = string.Empty;
    }

    public static bool GetTextureMemory(ResNode res, ref AsssetTextureDebugNode node)
    {
        Texture2D tex = null;
        Sprite sp = null;
        if (res.isAsync)
        {
            if (res.isLoadFinish)
            {
                tex = res.AllAssets[0] as Texture2D;
                //转换失败，看看是不是sprite
                if (!tex)
                {
                    sp = res.AllAssets[0] as Sprite;
                    if (sp)
                    {
                        tex = sp.texture;
                    }
                }
            }
        }
        else
        {
            tex = res.AllAssets[0] as Texture2D;
            //转换失败，看看是不是sprite
            if (!tex)
            {
                sp = res.AllAssets[0] as Sprite;
                if (sp)
                {
                    tex = sp.texture;
                }
            }
        }

        if (tex)
        {
            node.MemoryUsedInMB = GetTextureMemory(tex.width, tex.height, tex.format, tex.mipmapCount > 1);
            node.minmap = tex.mipmapCount > 1;
            if (sp)
            {
                node.isUIPacked = sp.packed;
                node.PackedSize = node.MemoryUsedInMB;
                node.PackedName = tex.name;
            }
        }
        return tex;
    }

    public static float GetTextureMemory(int w, int h, TextureFormat formt, bool minmap)
    {
        float memory = 0.0f;
        memory = w * h;
        memory /= 1024;
        memory /= 1024;
        switch (formt)
        {
            case TextureFormat.DXT1:
                memory *= 24;
                memory /= 8;
                memory /= 6;
                break;
            case TextureFormat.DXT5:
                memory *= 32;
                memory /= 8;
                memory /= 4;
                break;
            case TextureFormat.ARGB32:
                memory *= 32;
                memory /= 8;
                break;
            case TextureFormat.RGB24:
                memory *= 24;
                memory /= 8;
                break;
            default:
                Debug.LogError(formt);
                memory = 0;
                break;
        }
        if (minmap)
        {
            memory *= 1.3f;
        }
        return memory;
    }

    public static bool IsScenePath(string path)
    {
        return path.Contains("texture/model/scene");
    }
    public static float SceneUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> SceneAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();

    public static bool IsUIPath(string path)
    {
        return path.Contains("texture/ui");
    }
    public static float UIUsedTotal = 0.0f;
    public static float UIDebugNodeCount = 0;
    public static Dictionary<int, AsssetTextureDebugNode> UIAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();
    public static Dictionary<string, int> UIPackedCacluate = new Dictionary<string, int>();


    public static bool IsHeroPath(string path)
    {
        return path.Contains("texture/model/hero");
    }
    public static float HeroUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> HeroAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();

    private static bool IsMonsterPath(string path)
    {
        if (path.Contains("texture/model/"))
        {
            return (path.Contains("monster") || path.Contains("npc") || path.Contains("wing"));
        }
        return false;
    }
    public static float MonsterUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> MonsterAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();

    public static bool IsEffectPath(string path)
    {
        return path.Contains("texture/effect/");
    }
    public static float EffectUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> EffectAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();

    public static float CommUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> CommAssetsDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();

    public static List<int> LoadingAssetList = new List<int>();
    public static float onUnLoadingUsedTotal = 0.0f;
    public static Dictionary<int, AsssetTextureDebugNode> onUnLoadingDebugNodeList = new Dictionary<int, AsssetTextureDebugNode>();




    public static AsssetTextureDebugNode PushAssetDebugNode(int key,ResNode resNode, Dictionary<int, AsssetTextureDebugNode> table)
    {
        AsssetTextureDebugNode node = new AsssetTextureDebugNode();
        node.AssetName = resNode.AssetName;
        node.AssetPath = resNode.AssetBundleName;
        node.RefCount = resNode.GetRefCount();
        if(!GetTextureMemory(resNode, ref node))
        {
            return null;
        }
        table.Add(key, node);
        return node;
    }

    public static void GenerateTextureNodeList()
    {
        LoadingAssetList.Clear();
        SceneAssetsDebugNodeList.Clear();
        UIAssetsDebugNodeList.Clear();
        HeroAssetsDebugNodeList.Clear();
        MonsterAssetsDebugNodeList.Clear();
        EffectAssetsDebugNodeList.Clear();
        onUnLoadingDebugNodeList.Clear();
        UIPackedCacluate.Clear();
        CommAssetsDebugNodeList.Clear();
        EffectUsedTotal = 0.0f;
        onUnLoadingUsedTotal = 0.0f;
        MonsterUsedTotal = 0.0f;
        HeroUsedTotal = 0.0f;
        UIUsedTotal = 0.0f;
        SceneUsedTotal = 0.0f;
        UIDebugNodeCount = 0.0f;
        CommUsedTotal = 0.0f;

        foreach (ResNode resNode in m_TextureResTable.Values)
        {
            bool needToCacluate = true;
            if (resNode.isAsync)
            {
                needToCacluate = resNode.isLoadFinish;
            }

            if (!needToCacluate)
            {
                LoadingAssetList.Add(resNode.instanceID);
            }
            else
            {
                if (IsScenePath(resNode.AssetBundleName))
                {
                    AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, SceneAssetsDebugNodeList);
                    if (null == node)
                    {
                        continue;
                    }

                    SceneUsedTotal += node.MemoryUsedInMB;
                }
                else if (IsHeroPath(resNode.AssetBundleName))
                {
                    AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, HeroAssetsDebugNodeList);
                    if (null == node)
                    {
                        continue;
                    }
                    HeroUsedTotal += node.MemoryUsedInMB;
                }
                else if (IsMonsterPath(resNode.AssetBundleName))
                {
                    AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, MonsterAssetsDebugNodeList);
                    if (null == node)
                    {
                        continue;
                    }
                    MonsterUsedTotal += node.MemoryUsedInMB;
                }
                else if (IsEffectPath(resNode.AssetBundleName))
                {
                    AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, EffectAssetsDebugNodeList);
                    if (null == node)
                    {
                        continue;
                    }
                    EffectUsedTotal += node.MemoryUsedInMB;
                }
            }
        }

        foreach (ResNode resNode in m_TextureCacheResTable.Values)
        {
            AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, onUnLoadingDebugNodeList);
            if (null == node)
            {
                continue;
            }

            onUnLoadingUsedTotal += node.MemoryUsedInMB;
        }

        foreach (ResNode resNode in m_SpriteTable.Values)
        {
            AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, UIAssetsDebugNodeList);
            if(null == node)
            {
                continue;
            }
            if (node.isUIPacked)
            {
                if (!UIPackedCacluate.ContainsKey(node.PackedName))
                {
                    UIUsedTotal += node.PackedSize;
                    UIPackedCacluate.Add(node.PackedName, 1);
                    UIDebugNodeCount++;
                }
                else
                {
                    UIPackedCacluate[node.PackedName]++;
                }
            }
            else
            {
                UIUsedTotal += node.MemoryUsedInMB;
                UIDebugNodeCount++;
            }
        }

        foreach (ResNode resNode in m_UISpriteCacheTable.Values)
        {
            AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, UIAssetsDebugNodeList);
            if (null == node)
            {
                continue;
            }
            if (node.isUIPacked)
            {
                if (UIPackedCacluate.ContainsKey(node.PackedName))
                {
                    int cout = --UIPackedCacluate[node.PackedName];
                    if (cout <= 0)
                    {
                        onUnLoadingUsedTotal += node.PackedSize;
                        UIUsedTotal -= node.PackedSize;
                        UIPackedCacluate.Remove(node.PackedName);
                    }
                }
            }
            else
            {
                onUnLoadingUsedTotal += node.MemoryUsedInMB;
                UIUsedTotal -= node.PackedSize;
            }

        }

        foreach (ResNode resNode in m_CommonResNodeTable.Values)
        {
            AsssetTextureDebugNode node = PushAssetDebugNode(resNode.instanceID, resNode, CommAssetsDebugNodeList);
            if (null == node)
            {
                continue;
            }
           CommUsedTotal += node.MemoryUsedInMB;

        }
    }


    static bool ShowScene = false;
    static bool ShowUI = false;
    static bool ShowHero = false;
    static bool ShowMonster = false;
    static bool ShowEffect = false;
    static bool ShowonUnLoading = false;
    static bool ShowonLoading = false;
    static bool ShowCommon = false;

    static Vector2 vScene = Vector2.zero;
    static Vector2 vUI = Vector2.zero;
    static Vector2 vHero = Vector2.zero;
    static Vector2 vMonster = Vector2.zero;
    static Vector2 vEffect = Vector2.zero;
    static Vector2 vonUnLoading = Vector2.zero;
    static Vector2 vonLoading = Vector2.zero;
    static Vector2 vCommon = Vector2.zero;

    public static void DarwTableList(Dictionary<int, AssetBundleManager.AsssetTextureDebugNode> table)
    {
        GUILayout.BeginVertical("box");

        foreach (AssetBundleManager.AsssetTextureDebugNode nod in table.Values)
        {
            GUILayout.BeginVertical("box");

            GUILayout.Label("路径:" + nod.AssetPath);
            GUILayout.Label("名称:" + nod.AssetName);

            if (nod.isUIPacked)
            {
                GUILayout.Label("图集名:" + nod.PackedName);
                GUILayout.Label("图集大小:" + nod.PackedSize + "MB");

            }
            else
            {
                GUILayout.Label("内存:" + nod.MemoryUsedInMB + "MB");
            }

            GUILayout.Label("引用:" + nod.RefCount);
            GUILayout.Label("mnimap:" + nod.minmap);
            GUILayout.EndVertical();
            GUILayout.Space(5.0f);
        }

        GUILayout.EndVertical();
    }

    private static void DrawDebugInfoImpl()
    {
        if (GUILayout.Button("提取贴图信息"))
        {
            AssetBundleManager.GenerateTextureNodeList();
        }

        GUILayout.BeginVertical("box");

        GUILayout.BeginHorizontal("box");
        Color old = GUI.color;
        GUI.color = Color.red;


        float allCount = CommAssetsDebugNodeList.Count + EffectAssetsDebugNodeList.Count + onUnLoadingDebugNodeList.Count + MonsterAssetsDebugNodeList.Count + HeroAssetsDebugNodeList.Count + UIDebugNodeCount + SceneAssetsDebugNodeList.Count;
        float allUsedCount = SceneUsedTotal + UIUsedTotal + HeroUsedTotal + MonsterUsedTotal + EffectUsedTotal + CommUsedTotal + onUnLoadingUsedTotal;
        GUILayout.Label("总贴图数:" + allCount);
        GUILayout.Label("总使用内存:" + allUsedCount + "MB");
        GUI.color = old;
        GUILayout.EndHorizontal();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("通用贴图");
        GUILayout.Label("数量:" + AssetBundleManager.CommAssetsDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.CommUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.CommAssetsDebugNodeList.Count > 0)
        {
            ShowCommon = GUILayout.Toggle(ShowCommon, "详细信息:");
            if (ShowCommon)
            {
                vCommon = GUILayout.BeginScrollView(vCommon);
                DarwTableList(AssetBundleManager.CommAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }
        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("场景贴图");
        GUILayout.Label("数量:" + AssetBundleManager.SceneAssetsDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.SceneUsedTotal + "MB");
        GUILayout.EndHorizontal();
        
        if (AssetBundleManager.SceneAssetsDebugNodeList.Count > 0)
        {
            ShowScene = GUILayout.Toggle(ShowScene, "详细信息:");
            if (ShowScene)
            {
                vScene = GUILayout.BeginScrollView(vScene);
                DarwTableList(AssetBundleManager.SceneAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }
        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("UI贴图");
        GUILayout.Label("数量:" + AssetBundleManager.UIDebugNodeCount);
        GUILayout.Label("总消耗:" + AssetBundleManager.UIUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.UIDebugNodeCount > 0)
        {
            ShowUI = GUILayout.Toggle(ShowUI, "详细信息:");
            if (ShowUI)
            {
                vUI = GUILayout.BeginScrollView(vUI);
                DarwTableList(AssetBundleManager.UIAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();


        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("人物贴图");
        GUILayout.Label("数量:" + AssetBundleManager.HeroAssetsDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.HeroUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.HeroAssetsDebugNodeList.Count > 0)
        {
            ShowHero = GUILayout.Toggle(ShowHero, "详细信息:");
            if (ShowHero)
            {
                vHero = GUILayout.BeginScrollView(vHero);
                DarwTableList(AssetBundleManager.HeroAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("怪物贴图");
        GUILayout.Label("数量:" + AssetBundleManager.MonsterAssetsDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.MonsterUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.MonsterAssetsDebugNodeList.Count > 0)
        {
            ShowMonster = GUILayout.Toggle(ShowMonster, "详细信息:");
            if (ShowMonster)
            {
                vMonster = GUILayout.BeginScrollView(vMonster);
                DarwTableList(AssetBundleManager.MonsterAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("特效贴图");
        GUILayout.Label("数量:" + AssetBundleManager.EffectAssetsDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.EffectUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.EffectAssetsDebugNodeList.Count > 0)
        {
            ShowEffect = GUILayout.Toggle(ShowEffect, "详细信息:");
            if (ShowEffect)
            {
                vEffect = GUILayout.BeginScrollView(vEffect);
                DarwTableList(AssetBundleManager.EffectAssetsDebugNodeList);
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();



        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("缓存中贴图");
        GUILayout.Label("数量:" + AssetBundleManager.onUnLoadingDebugNodeList.Count);
        GUILayout.Label("总消耗:" + AssetBundleManager.onUnLoadingUsedTotal + "MB");
        GUILayout.EndHorizontal();

        if (AssetBundleManager.onUnLoadingDebugNodeList.Count > 0)
        {
            ShowonUnLoading = GUILayout.Toggle(ShowonUnLoading, "详细信息:");
            if (ShowonUnLoading)
            {
                vonUnLoading = GUILayout.BeginScrollView(vonUnLoading);
                DarwTableList(AssetBundleManager.onUnLoadingDebugNodeList);
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        GUILayout.BeginHorizontal("box");
        GUILayout.Label("正在加载的贴图");
        GUILayout.Label("数量:" + AssetBundleManager.LoadingAssetList.Count);
        GUILayout.EndHorizontal();

        if (AssetBundleManager.LoadingAssetList.Count > 0)
        {
            ShowonLoading = GUILayout.Toggle(ShowonLoading, "详细信息:");
            if (ShowonLoading)
            {
                vonLoading = GUILayout.BeginScrollView(vonLoading);
                foreach (int s in AssetBundleManager.LoadingAssetList)
                {
                    GUILayout.Label(s.ToString());
                }
                GUILayout.EndScrollView();
            }
        }

        GUILayout.EndVertical();


        GUILayout.EndVertical();
    }
#endif
    #endregion

    #region PrefabDebug

    #if OpenAssetDebug
    [Serializable]
    public sealed class LoadedAssetsNode
    {
        public string ResName = string.Empty;
        public float loadedTicks = 0;
        public float costTime = 0;
        public int bCleared = -1;
        public void Clear()
        {
            loadedTicks = 0;
            ResName = string.Empty;
            costTime = 0;
            bCleared = -1;
        }
    }

    public sealed class LoadedAssetInfo : MonoBehaviour
    {
        public List<LoadedAssetsNode> dependeincesInfo = new List<LoadedAssetsNode>();

        public LoadedAssetsNode prefabInfo = new LoadedAssetsNode();

        public void Clear()
        {
            prefabInfo.Clear();
            dependeincesInfo.Clear();
        }

        private static void FillInfo(ResNode res, LoadedAssetsNode info)
        {
            info.ResName = res.AssetName;
            info.loadedTicks = (float)res.LoadingTickes;
            info.costTime = (float)res.costLoadedTime;
            info.bCleared = res.Cleared ? 0 : 1;
        }

        public void InsertResIno(ResNode prefab, PrefabDependenciesResNode resNode)
        {
            FillInfo(prefab,prefabInfo);
            if(null == resNode)
            {
                dependeincesInfo.Clear();
                return;
            }
            foreach (ResNode resGUID in resNode.TextureResNode)
            {
                LoadedAssetsNode info = new LoadedAssetsNode();
                dependeincesInfo.Add(info);
                FillInfo(resGUID,info);
            }

            foreach (ResNode resGUID in resNode.MaterialResNode)
            {
                LoadedAssetsNode info = new LoadedAssetsNode();
                dependeincesInfo.Add(info);
                FillInfo(resGUID,info);
            }

            foreach (ResNode resGUID in resNode.FBXResNode)
            {
                LoadedAssetsNode info = new LoadedAssetsNode();
                dependeincesInfo.Add(info);
                FillInfo(resGUID,info);
            }

            foreach (ResNode resGUID in resNode.AnimatonClipResNode)
            {
                LoadedAssetsNode info = new LoadedAssetsNode();
                dependeincesInfo.Add(info);
                FillInfo(resGUID,info);
            }

            foreach (ResNode resGUID in resNode.AnimatorControllerResNode)
            {
                LoadedAssetsNode info = new LoadedAssetsNode();
                dependeincesInfo.Add(info);
                FillInfo(resGUID,info);
            }
        }
    }

    private static GameObject LoadedAssetInfoParentRoot = null;

    private static Dictionary<int, LoadedAssetInfo> m_Table = new Dictionary<int, LoadedAssetInfo>();
    #endif

    public static void PushLoadedAssetInfo(ResNode res, PrefabDependenciesResNode resNode = null)
    {
#if OpenAssetDebug
        if (!LoadedAssetInfoParentRoot)
        {
            LoadedAssetInfoParentRoot = new GameObject();
            LoadedAssetInfoParentRoot.name = "LoadedAssetInfoParentRoot";
            GameObject.DontDestroyOnLoad(LoadedAssetInfoParentRoot);
        }

        LoadedAssetInfo info = null;
        if (m_Table.TryGetValue(res.instanceID, out info))
        {
            if (!info)
            {
                GameObject go = new GameObject();
                go.name = res.AssetName;
                go.transform.SetParent(LoadedAssetInfoParentRoot.transform);
                info = go.AddComponent<LoadedAssetInfo>();
                m_Table[res.instanceID] = info;
            }

        }
        else
        {
            GameObject go = new GameObject();
            go.name = res.AssetName;
            go.transform.SetParent(LoadedAssetInfoParentRoot.transform);
            info = go.AddComponent<LoadedAssetInfo>();
            m_Table.Add(res.instanceID, info);
        }

        info.Clear();
        info.InsertResIno(res, resNode);
#endif
    }

    #endregion
}
