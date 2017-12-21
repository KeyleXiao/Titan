/*******************************************************************
** 文件名:	SceneMaterialConfig.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景材质球信息
** 应  用:   场景材质球信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[System.Serializable]
public class SceneMaterialConfig
{
    /// <summary>
    /// 材质球
    /// </summary>
    public Material SharedMaterial = null;

    /// <summary>
    /// 材质球上对应的纹理配置
    /// </summary>
    public List<SceneTextureConfigData> TextureConfigList = new List<SceneTextureConfigData>();

    /// <summary>
    /// 主贴图数目
    /// </summary>
    public int mainTexCount = 0;


    /// <summary>
    /// 是否支持优化的材质球
    /// </summary>
    public bool isSupportOptimtizedMat = false;

    /// <summary>
    /// 本身就是优化的shader
    /// </summary>
    public bool isOrginalOptimitizedShader = false;
}
