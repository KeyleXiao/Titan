/*******************************************************************
** 文件名:	AssetBundleManager_Prefab.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，预制体部分

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
    /*
     * 预制体跟其他的资源不一样，很特殊
     * 所以单独开出来，重新搞一搞。
     * 预制体只会打包贴图，fbx，ainmatorcontroller，animationClip
     */

    public enum PrefabConfigCol
    {
        COL_AssetBundleName,
        COL_AssetName,
        COL_AssetGUID,
        COL_UnionIndex,
        COL_StartIndex,
        COL_DataLength,
        COL_TextureCount,
        COL_FBXCount,
        COL_ClipCount,
        COL_ControllerCount,
        COL_ShaderCount,
        COL_MaterialCount,
    }

    /// <summary>
    /// [资源GUID-配置表],所有的依赖配置表.预制体的所有引用，都是从这个表中获取，依赖资源的配置更新，也更新这个表即可
    /// </summary>
    private static Dictionary<string, ResConfigData> m_DependenciesResConfigTable = new Dictionary<string, ResConfigData>();

    /// <summary>
    /// [资源GUID-配置表],所有的依赖配置表.预制体的所有引用，都是从这个表中获取，依赖资源的配置更新，也更新这个表即可
    /// </summary>
    public static Dictionary<string, ResConfigData> DependenciesResConfigTable
    {
        get
        {
            return m_DependenciesResConfigTable;
        }
    }


    /// <summary>
    /// 预制体GUID以及对应的依赖[资源配置]表
    /// </summary>
    private static Dictionary<string, PrefabDependenicesResConfigData> m_PrefabDependenciesConfigTable = new Dictionary<string, PrefabDependenicesResConfigData>();

    public static Dictionary<string, PrefabDependenicesResConfigData> PrefabDependenciesConfigTable
    {
        get
        {
            return m_PrefabDependenciesConfigTable;
        }
    }

    /// <summary>
    /// 包名-[资源名-ResConfigData] 预制体[配置]表
    /// </summary>
    private static Dictionary<string, Dictionary<string, UnionResConfigData>> m_PrefabConfigTable = new Dictionary<string, Dictionary<string, UnionResConfigData>>();

    /// <summary>
    /// 包名-[资源名-ResConfigData] 预制体[配置]表
    /// </summary>
    public static Dictionary<string, Dictionary<string, UnionResConfigData>> PrefabConfigTable
    {
        get
        {
            return m_PrefabConfigTable;
        }
    }


    /// <summary>
    ///  GUID-预制体配置表,同时用于做校验,移动资源的时候有可能会换包，所以要校验一下，避免冗余的资源信息
    /// </summary>
    private static Dictionary<int, UnionResConfigData> m_PrefabGUIDConfigTable = new Dictionary<int, UnionResConfigData>();


    /// <summary>
    /// 预制体GUID与他所依赖的[资源]表
    /// </summary>
    private static Dictionary<int, PrefabDependenciesResNode> m_PrefabDependenciesResNodeTable = new Dictionary<int, PrefabDependenciesResNode>();



    public static string PrefabConfigFileName = "GameObject.gocof";
    public static string PrefabDependenicesFileName = "GameObject_Depen.gocof";

    public static void LoadPrefabConfig(Dictionary<string, Dictionary<string, UnionResConfigData>> result = null,
                                        Dictionary<string, PrefabDependenicesResConfigData> depedenceResult = null,
                                        Dictionary<string, ResConfigData> DependenicesResConfigReslut = null)
    {
        ClearPrefabConfig();

        LoadDependenciesResConfig(DependenicesResConfigReslut);
        LoadPrefabSelfConfig(result, depedenceResult);
        CorrectDependencies();
    }

    private static void CorrectDependencies()
    {
        if (m_PrefabDependenciesConfigTable.Count <= 0)
        {
            return;
        }

        List<int> tempTextureList = new List<int>(50);
        List<int> tempMatList = new List<int>(50);
        List<int> tempAniList = new List<int>(50);
        List<int> tempFBXList = new List<int>(50);
        List<int> tempAniControllerList = new List<int>(50);

        List<int> tempDepList = new List<int>(100);
        foreach (PrefabDependenicesResConfigData keyValue in m_PrefabDependenciesConfigTable.Values)
        {
            tempTextureList.Clear();
            tempMatList.Clear();
            tempAniList.Clear();
            tempFBXList.Clear();
            tempAniControllerList.Clear();

            tempDepList.Clear();
            PrefabDependenicesResConfigData dependenciesConfig = keyValue;

            foreach (string resGUID in dependenciesConfig.TextureResConfigDatas)
            {
                int guid = GUIDToInstanceID(resGUID);
                tempTextureList.Add(guid);
            }

            tempDepList.Clear();
            foreach (string resGUID in dependenciesConfig.MaterialResConfigDatas)
            {
                tempDepList.AddRange(tempTextureList);

                int guid = GUIDToInstanceID(resGUID);
                tempMatList.Add(guid);

                PushDependence(guid, tempDepList);
                
            }

            tempDepList.Clear();
            foreach (string resGUID in dependenciesConfig.AnimatonClipResConfigDatas)
            {
                tempDepList.AddRange(tempMatList);

                int guid = GUIDToInstanceID(resGUID);
                tempAniList.Add(guid);

                PushDependence(guid, tempDepList);
            }

            tempDepList.Clear();
            foreach (string resGUID in dependenciesConfig.FBXResConfigDatas)
            {
                tempDepList.AddRange(tempMatList);
                tempDepList.AddRange(tempAniList);

                int guid = GUIDToInstanceID(resGUID);

                ResConfigData res = GetDependenicesConfig(resGUID);
                tempFBXList.Add(guid);

                PushDependence(guid, tempDepList);
            }

            tempDepList.Clear();
            foreach (string resGUID in dependenciesConfig.AnimatorControllerResConfigDatas)
            {
                tempDepList.AddRange(tempMatList);
                tempDepList.AddRange(tempAniList);
                tempDepList.AddRange(tempFBXList);

                int guid = GUIDToInstanceID(resGUID);
                tempAniControllerList.Add(guid);

                PushDependence(guid, tempDepList);
            }
        }
    }

    private static void LoadDependenciesResConfig(Dictionary<string, ResConfigData>  Result = null)
    {
        string patch = AssetBundleManager.GetDependenicesResConfigPath();
        if (!File.Exists(patch))
        {
            if(Application.isPlaying)
            {
                Debug.LogError("找不到预制体资源表！！！-" + patch);
            }
            return;
        }

        string stSchemeAllText = DeCodeAssetFile(patch);

        if (stSchemeAllText.Equals(string.Empty))
        {
            return;
        }

        if (null == Result)
        {
            Result = m_DependenciesResConfigTable;
        }

        ////去掉\n,因为ScpReader以"\r"分割
        //stSchemeAllText = stSchemeAllText.Replace("\n", "");
        ////再干掉最后的"\r"
        //stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        AssetBundleConfigReader ResReader = new AssetBundleConfigReader(stSchemeAllText, "DependenicesResConfig", 0);
        for (int i = 0; i < ResReader.GetRecordCount(); ++i)
        {
            ResConfigData depconf = new ResConfigData();
            depconf.AssetBundleName = StringHelper.CopyString(ResReader.GetString(i, (int)AssetConfigCol.COL_AssetBundleName, string.Empty));
            depconf.AssetName = StringHelper.CopyString(ResReader.GetString(i, (int)AssetConfigCol.COL_AssetName, string.Empty));
            depconf.AssetGUID = StringHelper.CopyString(ResReader.GetString(i, (int)AssetConfigCol.COL_AssetGUID, string.Empty));
            depconf.AssetSize_X = ResReader.GetFloat(i, (int)AssetConfigCol.COL_AssetSize_X, -1.0f);
            depconf.AssetSize_Y = ResReader.GetFloat(i, (int)AssetConfigCol.COL_AssetSize_Y, -1.0f);
            if(m_DependenciesResConfigTable.ContainsKey(depconf.AssetGUID))
            {
                Debug.LogWarning(StringHelper.BuildString("依赖资源表有重复的配置:" , depconf.AssetGUID,depconf.AssetName,depconf.AssetBundleName));
                continue;
            }
            else
            {
                m_DependenciesResConfigTable.Add(StringHelper.CopyString(depconf.AssetGUID), depconf);
            }
        }

        ResReader.Dispose();
        ResReader = null;
    }


    /// <summary>
    /// 加载预制体配置
    /// </summary>
    private static void LoadPrefabSelfConfig(Dictionary<string, Dictionary<string, UnionResConfigData>> result = null,
                                        Dictionary<string, PrefabDependenicesResConfigData> depedenceResult = null)
    {
        string patch = AssetBundleManager.GetPrefabConfigPath();
        if (!File.Exists(patch))
        {
            if (Application.isPlaying)
            {
                Debug.LogError("找不到预制体配置！！！-" + patch);
            }
            return;
        }
        string stSchemeAllText = DeCodeAssetFile(patch);

        if (stSchemeAllText.Equals(string.Empty))
        {
            return;
        }

        if (result == null || depedenceResult == null)
        {
            result = m_PrefabConfigTable;
            depedenceResult = m_PrefabDependenciesConfigTable;
        }

        ////去掉\n,因为ScpReader以"\r"分割
        //stSchemeAllText = stSchemeAllText.Replace("\n", "");
        ////再干掉最后的"\r"
        //stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);

        AssetBundleConfigReader PrefabReader = new AssetBundleConfigReader(stSchemeAllText, "PrefabConfig", 0);


        bool isNeedLoadDependenices = false; //是否需要加载依赖配置
        UnionResConfigData currLoadingPrefab = null; //当前正在加载的Prefab
        int texcount = 0; //贴图数目
        int fbxcount = 0; //fbx数目
        int clipcount = 0; //clip数目
        int controllercount = 0; //animatorcontroller数目
        int shadercount = 0;//shader数目
        int materialcount = 0; //材质球数目

        string lowerPrefabCommonPatch = PrefabCommonPatch.ToLower();
        for (int i = 0; i < PrefabReader.GetRecordCount(); ++i)
        {

            isNeedLoadDependenices = !(texcount == 0 && fbxcount == 0 &&
                            clipcount == 0 && controllercount == 0 &&
                            shadercount == 0 && materialcount == 0);
            if (!isNeedLoadDependenices)
            {
                UnionResConfigData prefabConfig = new UnionResConfigData();
                prefabConfig.AssetBundleName = PrefabReader.GetString(i, (int)PrefabConfigCol.COL_AssetBundleName, string.Empty);
                prefabConfig.AssetName = PrefabReader.GetString(i, (int)PrefabConfigCol.COL_AssetName, string.Empty);
                prefabConfig.AssetGUID = PrefabReader.GetString(i, (int)PrefabConfigCol.COL_AssetGUID, string.Empty);
                prefabConfig.unionIndx = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_UnionIndex,-1);
                prefabConfig.startIndex = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_StartIndex, -1);
                prefabConfig.dataLength = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_DataLength, -1);


                if(!prefabConfig.AssetBundleName.EndsWith(PrefabVariant))
                {
                    Debug.LogWarning("预制体配置中有冗余的配置:" + prefabConfig.AssetBundleName + ",名字:" + prefabConfig.AssetName + ",重新生成资源可解决此问题 -- not gameobject");
                    continue;
                }

                texcount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_TextureCount, 0);
                fbxcount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_FBXCount, 0);
                clipcount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_ClipCount, 0);
                controllercount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_ControllerCount, 0);
                shadercount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_ShaderCount, 0);
                materialcount = PrefabReader.GetInt(i, (int)PrefabConfigCol.COL_MaterialCount, 0);

                isNeedLoadDependenices = !(texcount == 0 && fbxcount == 0 && 
                                            clipcount == 0 && controllercount == 0 &&
                                            shadercount == 0 && materialcount == 0);

                if (Application.isPlaying)
                {
                    if (isOnRunTimeMode)
                    {
                        if (prefabConfig.unionIndx < 0 || prefabConfig.startIndex < 0 || prefabConfig.dataLength <= 0)
                        {
                            Debug.LogWarning("无效的预制体配置中有冗余的配置:" + prefabConfig.AssetBundleName + ",名字:" + prefabConfig.AssetName + ",重新生成资源可解决此问题--uniondex < 0");
                            i += texcount;
                            i += fbxcount;
                            i += clipcount;
                            i += controllercount;
                            i += shadercount;
                            i += materialcount;
                            texcount = 0;
                            fbxcount = 0;
                            clipcount = 0;
                            controllercount = 0;
                            shadercount = 0;
                            materialcount = 0;
                            isNeedLoadDependenices = false;
                            currLoadingPrefab = null;
                            continue;
                        }
                    }
                }

                currLoadingPrefab = prefabConfig;
                int InstanceID = GUIDToInstanceID(prefabConfig.AssetGUID);
                if (m_PrefabGUIDConfigTable.ContainsKey(InstanceID))
                {
                    Debug.LogWarning("重复的预制体配置,GUID:" + prefabConfig.AssetGUID +
                        "资源名:" + prefabConfig.AssetName);
                    i += texcount;
                    i += fbxcount;
                    i += clipcount;
                    i += controllercount;
                    i += shadercount;
                    i += materialcount;
                    texcount = 0;
                    fbxcount = 0;
                    clipcount = 0;
                    controllercount = 0;
                    shadercount = 0;
                    materialcount = 0;
                    isNeedLoadDependenices = false;
                    currLoadingPrefab = null;
                    continue;
                }
                else
                {
                    m_PrefabGUIDConfigTable.Add(InstanceID, prefabConfig);
                }

                //把预制体资源先加载进去
                Dictionary<string, UnionResConfigData> assetTable = null;
                if (result.TryGetValue(prefabConfig.AssetBundleName, out assetTable))
                {
                    if (assetTable.ContainsKey(prefabConfig.AssetName))
                    {
                        Debug.LogWarning("预制体:" + prefabConfig.AssetBundleName + "有重复的依赖资源:" + prefabConfig.AssetName);
                        continue;
                    }
                    assetTable.Add(prefabConfig.AssetName, prefabConfig);

                }
                else
                {
                    assetTable = new Dictionary<string, UnionResConfigData>();
                    assetTable.Add(prefabConfig.AssetName, prefabConfig);
                    result.Add(prefabConfig.AssetBundleName, assetTable);

                    //通用预制体
                    if (prefabConfig.AssetBundleName.Contains(lowerPrefabCommonPatch))
                    {
                        PrefabCommonAssetBundleNames.Add(prefabConfig.AssetBundleName);
                    }
                }
                //直接destroy GameObject类型的资源

                //没有依赖的，给一个空配置即可
                if(!isNeedLoadDependenices)
                {
                    depedenceResult.Add(currLoadingPrefab.AssetGUID, new PrefabDependenicesResConfigData());
                }
                continue;
            }

            /*开始加载配置依赖表*/




            //1.先创建类
            PrefabDependenicesResConfigData dependenicesConfig = null;
            if (!depedenceResult.TryGetValue(currLoadingPrefab.AssetGUID, out dependenicesConfig))
            {
                dependenicesConfig = new PrefabDependenicesResConfigData();
                depedenceResult.Add(currLoadingPrefab.AssetGUID, dependenicesConfig);
            }

            //先是贴图
            if (texcount > 0)
            {
                texcount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i, 0, string.Empty));
                PushPrefabUnionRes(guid, dependenicesConfig.TextureResConfigDatas);
                continue;
            }

            //再加载FBX
            if (fbxcount > 0)
            {
                fbxcount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i, 0, string.Empty));
                PushPrefabUnionRes(guid, dependenicesConfig.FBXResConfigDatas);
                continue;
            }

            //再加载clip
            if (clipcount > 0)
            {
                clipcount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i,0, string.Empty));
                PushPrefabUnionRes(guid, dependenicesConfig.AnimatonClipResConfigDatas);

                continue;
            }

            //再加载controller
            if (controllercount > 0)
            {
                controllercount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i,0, string.Empty));
                PushPrefabUnionRes(guid, dependenicesConfig.AnimatorControllerResConfigDatas);

                continue;
            }

            //再加载shader
            if (shadercount > 0)
            {
                shadercount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i, 0, string.Empty));
                PushPrefabUnionRes(guid, dependenicesConfig.ShaderResConfigDatas);
                continue;
            }

            //再加载material
            if (materialcount > 0)
            {
                materialcount--;
                string guid = StringHelper.CopyString(PrefabReader.GetString(i, 0, string.Empty));

                PushPrefabUnionRes(guid, dependenicesConfig.MaterialResConfigDatas);
                continue;
            }
        }

        PrefabReader.Dispose();
        PrefabReader = null;
    }

    private static void PushPrefabUnionRes(string guid,List<string> list)
    {
        if(null == list)
        {
            return;
        }
        ResConfigData current = GetDependenicesConfig(guid);;
        if (isOnRunTimeMode && Application.isPlaying)
        {
            if (!IsUnionDependenicesAssetBundle(current.AssetBundleName))
            {
                list.Add(StringHelper.CopyString(guid));
            }
            else
            {
                //是合并的资源包，只取其中一个即可，多余的不要
                bool isAlreadExits = false;

                foreach (string s in list)
                {
                    ResConfigData cof = GetDependenicesConfig(s);
                    if (cof.AssetBundleName.Equals(current.AssetBundleName))
                    {
                        isAlreadExits = true;
                        break;
                    }
                }

                if(!isAlreadExits)
                {
                    current.fromReference = true;
                    list.Add(guid);
                }
            }

        }
        else
        {
            list.Add(StringHelper.CopyString(guid));
        }
    }

    private static ResConfigData GetDependenicesConfig(string guid)
    {
        ResConfigData reslut = null;
        m_DependenciesResConfigTable.TryGetValue(guid, out reslut);
        return reslut;
    }

    public static string GetPrefabConfigPath()
    {

        string fullPatch = FullAssetPackagePathRoot + PrefabConfigFileName;

        return fullPatch;
    }

    public static string GetDependenicesResConfigPath()
    {

        string fullPatch = FullAssetPackagePathRoot + PrefabDependenicesFileName;

        return fullPatch;
    }

    public static void PushDependenicesRes(ResConfigData res)
    {
        if(m_DependenciesResConfigTable.ContainsKey(res.AssetGUID))
        {
            m_DependenciesResConfigTable[res.AssetGUID] = res;
        }
        else
        {
            m_DependenciesResConfigTable.Add(res.AssetGUID, res);
        }
    }

    public static void PushPrefabConfig(UnionResConfigData prefabConfig, List<string> TextureData, List<string> FBXData,
                                        List<string> ClipData, List<string> ControllerData, List<string> ShaderData,
                                        List<string> MaterialData)
    {
        if(null == prefabConfig)
        {
            return;
        }
        /*先添加到预制体表里去*/

        Dictionary<string, UnionResConfigData> assetTable = null;

        UnionResConfigData cof;
        int InstanceID = GUIDToInstanceID(prefabConfig.AssetGUID);
        //如果这个预制体已经存在了,那么移除原来的预制体,再添加进去，存在多份一样的预制体信息
        if (m_PrefabGUIDConfigTable.TryGetValue(InstanceID, out cof))
        {
            //移除原来的
            m_PrefabConfigTable[cof.AssetBundleName].Remove(m_PrefabGUIDConfigTable[InstanceID].AssetName);

        }
        else
        {
            m_PrefabGUIDConfigTable.Add(InstanceID, prefabConfig);
        }

        if (m_PrefabConfigTable.TryGetValue(prefabConfig.AssetBundleName, out assetTable))
        {
            //对于已经存在的资源,则采取更新对应的资源
            if (assetTable.ContainsKey(prefabConfig.AssetName))
            {
                assetTable[prefabConfig.AssetName] = prefabConfig;
            }
            else
            {
                assetTable.Add(prefabConfig.AssetName, prefabConfig);
            }


        }
        else
        {
            assetTable = new Dictionary<string, UnionResConfigData>();
            assetTable.Add(prefabConfig.AssetName, prefabConfig);
            m_PrefabConfigTable.Add(prefabConfig.AssetBundleName, assetTable);
        }

        bool isNeedToWriteDependenices = !(TextureData.Count == 0 && FBXData.Count == 0 && ClipData.Count == 0 && ControllerData.Count == 0 && ShaderData.Count == 0 && MaterialData.Count == 0);

        if (!isNeedToWriteDependenices)
        {
            //如果他不需要写入配置，那么检查一下原来是不是有，有的话直接移除
            if (m_PrefabDependenciesConfigTable.ContainsKey(prefabConfig.AssetGUID))
            {
                m_PrefabDependenciesConfigTable.Remove(prefabConfig.AssetGUID);
            }
            return;
        }

        //1.先创建类
        PrefabDependenicesResConfigData dependenicesConfig = null;
        if (!m_PrefabDependenciesConfigTable.TryGetValue(prefabConfig.AssetGUID, out dependenicesConfig))
        {
            dependenicesConfig = new PrefabDependenicesResConfigData();
            m_PrefabDependenciesConfigTable.Add(prefabConfig.AssetGUID, dependenicesConfig);
        }
        else
        {
            //已经存在的，则清楚资源，更新
            dependenicesConfig.TextureResConfigDatas.Clear();
            dependenicesConfig.FBXResConfigDatas.Clear();
            dependenicesConfig.AnimatonClipResConfigDatas.Clear();
            dependenicesConfig.AnimatorControllerResConfigDatas.Clear();
            dependenicesConfig.ShaderResConfigDatas.Clear();
            dependenicesConfig.MaterialResConfigDatas.Clear();

        }
        //先贴图
        for (int i = 0; i < TextureData.Count; i++)
        {
            if (null != TextureData[i])
            {
                dependenicesConfig.TextureResConfigDatas.Add(TextureData[i]);
            }
        }

        //再fbx
        for (int i = 0; i < FBXData.Count; i++)
        {
            if (null != FBXData[i])
            {
                dependenicesConfig.FBXResConfigDatas.Add(FBXData[i]);
            }
        }

        //再clip
        for (int i = 0; i < ClipData.Count; i++)
        {
            if (null != ClipData[i])
            {
                dependenicesConfig.AnimatonClipResConfigDatas.Add(ClipData[i]);
            }
        }

        //再controller
        for (int i = 0; i < ControllerData.Count; i++)
        {
            if (null != ControllerData[i])
            {
                dependenicesConfig.AnimatorControllerResConfigDatas.Add(ControllerData[i]);
            }
        }

        //再shader
        for (int i = 0; i < ShaderData.Count; i++)
        {
            if (null != ShaderData[i])
            {
                dependenicesConfig.ShaderResConfigDatas.Add(ShaderData[i]);
            }
        }

        //再材质球
        for (int i = 0; i < MaterialData.Count; i++)
        {
            if (null != MaterialData[i])
            {
                dependenicesConfig.MaterialResConfigDatas.Add(MaterialData[i]);
            }
        }
    }

    public static bool SupplmentConfig(string assetBundleName,int unionIndex,
                                       int startIndex,int dataLength)
    {
        if(m_PrefabConfigTable.Count <= 0)
        {
            Debug.LogError("补充配置失败，配置表为null");
            return false;
        }

        Dictionary<string, UnionResConfigData> assetTable = null;
        if (!m_PrefabConfigTable.TryGetValue(assetBundleName, out assetTable))
        {
            //Debug.LogError("补充配置失败，找不到对应的资源表:" + assetBundleName);
            return false;
        }

        foreach(UnionResConfigData conf in assetTable.Values)
        {
            conf.unionIndx = unionIndex;
            conf.startIndex = startIndex;
            conf.dataLength = dataLength;
        }
        return true;
    }

    public static void DeletePrefabConfig(string prefabGUID)
    {
        UnionResConfigData cof;
        int InstanceID = GUIDToInstanceID(prefabGUID);
        //如果这个预制体已经存在了,那么移除原来的预制体,再添加进去，存在多份一样的预制体信息
        if (m_PrefabGUIDConfigTable.TryGetValue(InstanceID, out cof))
        {
            //移除原来的
            m_PrefabConfigTable[cof.AssetBundleName].Remove(m_PrefabGUIDConfigTable[InstanceID].AssetName);

        }

    }

    /// <summary>
    /// 清楚预制体配置
    /// </summary>
    public static void ClearPrefabConfig()
    {
        m_PrefabDependenciesConfigTable.Clear();
        m_PrefabConfigTable.Clear();
        m_PrefabGUIDConfigTable.Clear();
        m_DependenciesResConfigTable.Clear();
    }
}
