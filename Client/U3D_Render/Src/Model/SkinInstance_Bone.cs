/*******************************************************************
** 文件名:	SkinInstance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤实例，外部通过这个来使用这个皮肤
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 这个是皮肤实例，外部通过这个来使用这个皮肤
/// </summary>
public partial class SkinInstance                        
{
    private Dictionary<string, Transform> m_BoneMap = new Dictionary<string, Transform>();

    /// <summary>
    /// 预加载的骨骼名
    /// </summary>
    private string[] preLoadBones = new string[] { "Wound", "W_RA", "W_LA", "Root", "Main" };
    public Transform GetBonesTransform(string boneName)
    {
        Transform foundTranform = null;
        if(null == m_BoneMap)
        {
            return null;
        }
        //先取一下，看能不能去到
        if(!m_BoneMap.TryGetValue(boneName, out foundTranform))
        {
            foundTranform = getTransformByName(boneName);
            if(foundTranform)
            {
                m_BoneMap.Add(boneName, foundTranform);
            }
        }
        return foundTranform;
    }

    private void BuildBonesMap()
    {
        if(null == m_BoneMap)
        {
            m_BoneMap = new Dictionary<string, Transform>();
        }

        int length = preLoadBones.Length;
        for (int i = 0; i < length;i++ )
        {
            string s = preLoadBones[i];
            Transform t = getTransformByName(s);
            if (t)
            {
                m_BoneMap.Add(s, t);
            }
        }
    }

    private void ClearBonesMap()
    {
        m_BoneMap.Clear();
        m_BoneMap = null;
    }

    private Transform getTransformByName(string boneName)
    {
        if (!SkinObj)
        {
            return null;
        }
        Transform root = SkinObj.transform;

        //先看看用到的骨骼里有没这根骨骼 
        if (SkinnedMeshRenderList.Count >　0)
        {
            foreach (SkinnedMeshRenderer smr in SkinnedMeshRenderList)
            {
                foreach (Transform t in smr.bones)
                {
                    if (t == null)
                    {
                        Trace.Log("Mesh Error:" + root.name + "'s SkinnedMeshRender" + smr.name + " has empty bones");
                    }
                    else if (t.name.Equals(boneName))
                    {
                        return t;
                    }
                }
            }
        }
      
        //SkinnedMeshRender里找不到再遍历层次树里有没这骨骼
        Transform[] bones = root.GetComponentsInChildren<Transform>();
        foreach (Transform t in bones)
        {
            if (t == null)
            {
                Trace.Log("Mesh Error:" + root.name + "'s SkinnedMeshRender" + root.name + " has empty bones");
            }
            else if (t.name.Equals( boneName))
            {
                return t;
            }
        }

        return null;
    }
}
