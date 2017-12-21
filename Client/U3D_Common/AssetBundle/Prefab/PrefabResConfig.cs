/*******************************************************************
** 文件名:	PrefabResConfigData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	资源配置
** 应  用:  	资源配置类，通过配置去找资源,预制体资源配置

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 预制体的依赖配置
/// </summary>
[System.Serializable]
public class PrefabDependenicesResConfigData
{
    /// <summary>
    /// 贴图依赖
    /// </summary>
    public List<string> TextureResConfigDatas = new List<string>();

    /// <summary>
    /// FBX依赖
    /// </summary>
    public List<string> FBXResConfigDatas = new List<string>();

    /// <summary>
    /// AnimatorController依赖
    /// </summary>
    public List<string> AnimatorControllerResConfigDatas = new List<string>();

    /// <summary>
    /// AnimatonClip依赖
    /// </summary>
    public List<string> AnimatonClipResConfigDatas = new List<string>();

    /// <summary>
    /// Shader依赖,暂时没有实际用途
    /// </summary>
    [Obsolete("暂时没有实际用途")]
    public List<string> ShaderResConfigDatas = new List<string>();

    /// <summary>
    /// Material依赖
    /// </summary>
    public List<string> MaterialResConfigDatas = new List<string>();

    /// <summary>
    /// 资源优先级
    /// </summary>
    public int priority = 0;
}

/// <summary>
/// 预制体资源依赖
/// </summary>
public class PrefabDependenciesResNode
{
    /// <summary>
    /// 贴图依赖
    /// </summary>
    public List<ResNode> TextureResNode = new List<ResNode>();

    /// <summary>
    /// FBX依赖
    /// </summary>
    public List<ResNode> FBXResNode = new List<ResNode>();

    /// <summary>
    /// AnimatorController依赖
    /// </summary>
    public List<ResNode> AnimatorControllerResNode = new List<ResNode>();

    /// <summary>
    /// AnimatonClip依赖
    /// </summary>
    public List<ResNode> AnimatonClipResNode = new List<ResNode>();

    /// <summary>
    /// Shader依赖
    /// </summary>
    public List<ResNode> ShaderResNode = new List<ResNode>();

    /// <summary>
    /// Material依赖
    /// </summary>
    public List<ResNode> MaterialResNode = new List<ResNode>();

}