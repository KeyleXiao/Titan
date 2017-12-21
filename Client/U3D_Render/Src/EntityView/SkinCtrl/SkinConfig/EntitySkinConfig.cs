/*******************************************************************
** 文件名:	EntitySkinConfig.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2015-06-30
** 版  本:	1.0
** 描  述:	实体皮肤配置
********************************************************************/
using UnityEngine;
using System.Collections;

public class SkinLODRenderNode
{

    public Mesh HighQualityMesh = null;
    public Mesh MediumQualityMesh = null;
    public Mesh LowQualityMesh = null;
    public bool NeedToBakeMesh = false;
}

public class EntitySkinConfig : MonoBehaviour
{
    /// <summary>
    /// 右手武器
    /// </summary>
    public Renderer RightWeaponRender = null;
    public bool RightWeaponNeedToBakeMesh = false;

    /// <summary>
    /// 左手武器
    /// </summary>
    public Renderer LeftWeaponRender = null;
    public bool LeftWeaponNeedToBakeMesh = false;

    /// <summary>
    /// 身体
    /// </summary>
    public Renderer BodyRender = null;
    public bool BodyNeedToBakeMesh = false;

    /// <summary>
    /// 背部
    /// </summary>
    public Renderer BackBodyRender = null;
    public bool BackBodyNeedToBakeMesh = false;

    /// <summary>
    /// 翅膀
    /// </summary>
    public Transform chibangguadian = null;

    /// <summary>
    /// 优化距离,小于0不优化
    /// </summary>
    public float OptimitzedDistance = 40;

    /// <summary>
    /// 皮肤缓存数目,这个请根据战场中的怪物数量来填写！！
    /// </summary>
    public int CacheCout = 6;

    /// <summary>
    /// 靠近主角是是否优化
    /// </summary>
    public bool OptimizedNearMainHero = false;
}
