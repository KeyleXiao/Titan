/*******************************************************************
** 文件名:	EntitySkinConfig.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2015-06-30
** 版  本:	1.0
** 描  述:	怪物皮肤配置
********************************************************************/
using UnityEngine;
using System.Collections;

/// <summary>
/// 怪物皮肤配置
/// </summary>
public class MonsterSkinConfig : EntitySkinConfig
{
    /// <summary>
    /// 碰撞盒，用来和主角做碰撞检测的
    /// </summary>
    public Collider ColliderShape = null;

    /// <summary>
    /// 遮挡检测用的碰撞盒
    /// </summary>
    public Collider OcclusCollider = null;

    /// <summary>
    /// 碰撞盒的原始大小，用于皮肤缩放时.
    /// </summary>
    [System.NonSerialized]
    public Vector3 StandradColliderShapeSize = Vector3.zero;
}
