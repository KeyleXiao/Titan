/*******************************************************************
** 文件名:	SceneResInfoManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{
    /// <summary>
    /// 用于场景中地形公告板颜色补偿
    /// </summary>
    public float TerrainBilllBoardColorCompensationFactor = 1.0f;
    public List<SceneTerrainConfigData> TerrainInfoList = new List<SceneTerrainConfigData>();

    private List<Terrain> TerrainList = new List<Terrain>();
    private List<ResNode> TerrainResNodeList = new List<ResNode>();
    private GameObject UnityTerrainRoot = null;
    private List<WindZone> WindList = new List<WindZone>();

    const string TerrainBilllBoardColorCompensationFactorKeyWord = "_TerrainBilllBoardColorCompensationFactor";
    private void LoadTerrain()
    {
        if (!UnityTerrainRoot)
        {
            UnityTerrainRoot = new GameObject();
            UnityTerrainRoot.name = "UnityTerrainRoot_" + sceneName;
            UnityTerrainRoot.transform.SetParent(SceneRoot.transform);
        }
        foreach(SceneTerrainConfigData cof in TerrainInfoList)
        {
            LoadTerrain(cof);
        }
        WindList.AddRange(GameObject.FindObjectsOfType<WindZone>());
        Shader.SetGlobalFloat(TerrainBilllBoardColorCompensationFactorKeyWord, TerrainBilllBoardColorCompensationFactor);
    }

    private void LoadTerrain(SceneTerrainConfigData terrainConfig)
    {
        if (null == terrainConfig)
        {
            return;
        }
        if (string.IsNullOrEmpty(terrainConfig.TerrainGUID))
        {
            return;
        }
        ResNode terrainResNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, terrainConfig.TerrainGUID, true);
        if (ResNode.isNullOrEmpty(terrainResNode))
        {
            Debug.LogError("地形丢失:" + terrainConfig.TerrainGUID);
            return;
        }
        Terrain terrain = terrainResNode.InstanceMainRes<Terrain>();
        terrain.transform.position = terrainConfig.TerrainPos;
        TerrainResNodeList.Add(terrainResNode);
        TerrainList.Add(terrain);
        terrain.transform.SetParent(UnityTerrainRoot.transform);
    }

    private void UnLoadTerrain()
    {
        foreach (ResNode res in TerrainResNodeList)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssets(ref temp, true);
        }
        TerrainResNodeList.Clear();
        TerrainList.Clear();
        DestroyImmediate(UnityTerrainRoot);
        WindList.Clear();
    }
    private void ApplyTerrainParams_Best()
    {
        foreach (Terrain terrain in TerrainList)
        {
            if(!terrain)
            {
                continue;
            }

            terrain.heightmapPixelError = 50;
            terrain.basemapDistance = 200;
            terrain.detailObjectDistance = 120;
            terrain.detailObjectDensity = 1;
            terrain.treeBillboardDistance = 250;
            terrain.treeCrossFadeLength = 30;
            terrain.treeDistance = 1000;
            terrain.treeMaximumFullLODCount = 20;
        }

        if (WindList.Count > 0)
        {
            foreach (WindZone wz in WindList)
            {
                wz.gameObject.SetActive(true);
            }
        }
    }

    private void ApplyTerrainParams_High()
    {
        foreach (Terrain terrain in TerrainList)
        {
            if (!terrain)
            {
                continue;
            }

            terrain.heightmapPixelError = 50;
            terrain.basemapDistance = 200;
            terrain.detailObjectDistance = 100;
            terrain.detailObjectDensity = 1.0f;
            terrain.treeBillboardDistance = 200;
            terrain.treeCrossFadeLength = 20;
            terrain.treeDistance = 1000;
            terrain.treeMaximumFullLODCount = 20;
        }

        if (WindList.Count > 0)
        {
            foreach (WindZone wz in WindList)
            {
                wz.gameObject.SetActive(true);
            }
        }
    }

    private void ApplyTerrainParams_Good()
    {
        foreach (Terrain terrain in TerrainList)
        {
            if (!terrain)
            {
                continue;
            }

            terrain.heightmapPixelError = 50;
            terrain.basemapDistance = 200;
            terrain.detailObjectDistance = 80;
            terrain.detailObjectDensity = 0.6f;
            terrain.treeBillboardDistance = 150;
            terrain.treeCrossFadeLength = 20;
            terrain.treeDistance = 500;
            terrain.treeMaximumFullLODCount = 10;
        }

        if(WindList.Count >　0)
        {
            foreach (WindZone wz in WindList)
            {
                wz.gameObject.SetActive(false);
            }
            WindList[0].gameObject.SetActive(true);
        }

    }

    private void ApplyTerrainParams_Fastest()
    {
        foreach (Terrain terrain in TerrainList)
        {
            if (!terrain)
            {
                continue;
            }

            terrain.heightmapPixelError = 50;
            terrain.basemapDistance = 200;
            terrain.detailObjectDistance = 50;
            terrain.detailObjectDensity = 0.2f;
            terrain.treeBillboardDistance = 100;
            terrain.treeCrossFadeLength = 10;
            terrain.treeDistance = 500;
            terrain.treeMaximumFullLODCount = 10;
        }
        if (WindList.Count > 0)
        {
            foreach (WindZone wz in WindList)
            {
                wz.gameObject.SetActive(false);
            }
        }
    }
}