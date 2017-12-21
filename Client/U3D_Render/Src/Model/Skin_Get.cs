/*******************************************************************
** 文件名:	Skin_Get.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤结点与外部通讯部分
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/19
** 描  述:  增加缓存皮肤函数
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;


public partial class Skin
{
    ///// <summary>
    ///// 别调用我哦~ 皮肤与OverrideController
    ///// </summary>
    //public Dictionary<GameObject, AnimatorOverrideController> m_OverrideControllerTable = new Dictionary<GameObject, AnimatorOverrideController>();

    ///// <summary>
    ///// 别调用我哦~ 皮肤与SkinDataObj 
    ///// </summary>
    //public Dictionary<GameObject, Transform> m_SkinDataObj = new Dictionary<GameObject, Transform>();

    ///// <summary>
    ///// 别调用我哦~ 皮肤与SkinInstance 
    ///// </summary>
    //public Dictionary<GameObject, SkinInstance> m_SkinInstance = new Dictionary<GameObject, SkinInstance>();

    //public AnimatorOverrideController GetOverrideController(GameObject go)
    //{
    //    if(!isValid)
    //    {
    //        return null;
    //    }

    //    if (SkinType != ENTITY_TYPE.TYPE_PLAYER_ROLE)
    //    {
    //        return null;
    //    }

    //    AnimatorOverrideController co = null;
    //    if (!go)
    //    {
    //        Debug.LogError("AnimatorOverrideController 失败,参数为空");
    //        return co;
    //    }
    //    if (!m_OverrideControllerTable.TryGetValue(go, out co))
    //    {
    //        Trace.LogWarning("找不到对应的AnimatorOverrideController--" + ResSkinObj.AssetName);
    //    }
    //    return co;
    //}

    //public Transform GetSkinDataObj(GameObject go)
    //{
    //    if (!isValid)
    //    {
    //        return null;
    //    }
    //    Transform ana = null;
    //    if (!go)
    //    {
    //        Debug.LogError("SkinDataObj 失败,参数为空");
    //        return ana;
    //    }
    //    if (!m_SkinDataObj.TryGetValue(go, out ana))
    //    {
    //        Debug.LogWarning("找不到动SkinDataObj:" + go.name);
    //    }
    //    return ana;
    //}

    //public SkinInstance GetSkinInstance(GameObject go)
    //{
    //    if (!isValid)
    //    {
    //        return null;
    //    }
    //    SkinInstance ana = null;
    //    if(!go)
    //    {
    //        Debug.LogError("SkinInstance 失败,参数为空");
    //        foreach(GameObject g in AllSkin)
    //        {
    //            Debug.LogWarning(g);
    //        }

    //        Debug.LogWarning("GetAllInstanceResList:");
    //        foreach (GameObject g in ResSkinObj.GetAllInstanceResList)
    //        {
    //            Debug.LogWarning(g);
    //        }

    //        Debug.LogWarning("GetActiveInstanceResList:");
    //        foreach (GameObject g in ResSkinObj.GetActiveInstanceResList)
    //        {
    //            Debug.LogWarning(g);
    //        }

    //        Debug.LogWarning("GetUnActiveInstanceResList:");
    //        foreach (GameObject g in ResSkinObj.GetUnActiveInstanceResList)
    //        {
    //            Debug.LogWarning(g);
    //        }
    //        return ana;
    //    }
    //    if (!m_SkinInstance.TryGetValue(go, out ana))
    //    {
    //        Debug.LogWarning("找不到动SkinInstance:" + go.name);
    //    }
    //    if(ana.SkinObj != go)
    //    {
    //        Debug.LogError("Skin Instance no matched!" + ana.SkinObj + "," + go);
    //    }
    //    return ana;
    //}
}