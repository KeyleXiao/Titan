/*******************************************************************
** 文件名:	ResNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	资源结点
** 应  用:  	资源结点类，所有的资源加载都是这个

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public sealed partial class ResNode 
{

    //public static T InstantiateScriptable<T>(ScriptableObject go) where T : ScriptableObject
    //{
    //    if (!go)
    //    {
    //        return null;
    //    }

    //    return GameObject.Instantiate(go)as T;
    //}

    //public static ScriptableObject InstantiateScriptable(ScriptableObject go)
    //{
    //    if (!go)
    //    {
    //        return null;
    //    }

    //    return GameObject.Instantiate(go);
    //}

    //public void DestroyInstanceConfig<T>(ref T obj) where T : ScriptableObject
    //{
    //    if(!obj)
    //    {
    //        return;
    //    }
    //    GameObject.Destroy(obj);
    //    obj = null;
    //}

    //private void ClearInstanceConfig()
    //{

    //}

    //public T InstanceMainConfig<T>() where T : ScriptableObject
    //{
    //    if (m_bCleared)
    //    {
    //        return null;
    //    }
    //    T res = GameObject.Instantiate(assetObj) as T;
    //    return res;
    //}

    //public List<T> InstanceAllConfigs<T>() where T : ScriptableObject
    //{

    //    List<T> reslut = new List<T>();
    //    if (m_bCleared)
    //    {
    //        return reslut;
    //    }

    //    if (!IsValid())
    //    {
    //        return reslut;
    //    }
    //    for (int i = 0; i < assetObjList.Count; i++)
    //    {
    //        T res = GameObject.Instantiate(assetObj) as T;
    //        if (res)
    //        {
    //            reslut.Add(res);
    //        }
    //    }
    //    return reslut;
    //}

}