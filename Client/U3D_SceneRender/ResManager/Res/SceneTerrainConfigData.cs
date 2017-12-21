/*******************************************************************
** 文件名:	SceneTextureConfigData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景贴图配置
** 应  用:   场景贴图配置，用于加载场景的贴图

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[System.Serializable]
public class SceneTerrainConfigData 
{
    /// <summary>
    /// 贴图所对应的shader关键字
    /// </summary>
    public string TerrainGUID = string.Empty;

    /// <summary>
    /// 关键字对应的变体名
    /// </summary>
    public Vector3 TerrainPos = Vector3.zero;
}
