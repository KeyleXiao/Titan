/*******************************************************************
** 文件名:	Skin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤结点
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

public class BakeMeshNode
{
    public Mesh OrginalMesh = null;
    public Mesh BakedMesh = null;
    public int RefCount = 0;

    public bool IsValid()
    {
        return OrginalMesh && BakedMesh;
    }
}

/// <summary>
/// 皮肤结点，通过这个来控制皮肤的所有功能
/// </summary>
public partial class Skin
{
    /// <summary>
    /// 不通过缓存去释放，适用于战场中的模型切换
    /// </summary>
    public bool NotReleaseFromCache = false;

    public float currentCacheTime = 0.0f;

    /// <summary>
    /// 皮肤ID
    /// </summary>
    public int nSkinID = -9999;

    /// <summary>
    /// 是否有效
    /// </summary>
    public bool isValid = false;

    /// <summary>
    /// 皮肤类型，指能够用在哪种实体上
    /// </summary>
    public ENTITY_TYPE SkinType = ENTITY_TYPE.TYPE_MONSTER;

    public float fSkinScale = 1.0f;

    /// <summary>
    /// 资源
    /// </summary>
    public ResNode ResSkinObj = null;

    /// <summary>
    /// 用于检查的皮肤obj
    /// </summary>
    public GameObject SkinCheckObj = null;

    /// <summary>
    /// 是否进行了皮肤检查
    /// </summary>
    public bool bChecked = false;

    /// <summary>
    /// 皮肤的检查结果
    /// </summary>
    public bool CheckedReslut = false;

    /// <summary>
    /// 是否是静态皮肤，静态皮肤没有动画的
    /// </summary>
    public bool isStaticSkin = false;

    /// <summary>
    /// SkinObj的config,一遍用来读取通用数据
    /// </summary>
    public EntitySkinConfig skinConfig = null;

    /// <summary>
    /// 别调用我哦~
    /// </summary>
    public Queue<SkinInstance> Main_CacheList = new Queue<SkinInstance>();

    /// <summary>
    /// 别调用我哦~
    /// </summary>
    public List<SkinInstance> AllSkin = new List<SkinInstance>(20);

    public List<BakeMeshNode> AllBakeMesh = new List<BakeMeshNode>();

    

    public void OnCache()
    {
        if (!isValid)
        {
            return ;
        }
        foreach(SkinInstance go in AllSkin)
        {
            SkinManager.CacheSkinInstance(go);
        }
    }

    public int AddRef()
    {
        ReferenceCount++;
        return ReferenceCount;
    }

    public int AddRef(int refCount)
    {
        ReferenceCount += refCount;
        return ReferenceCount;
    }

    public int Release()
    {
        ReferenceCount--;
        return ReferenceCount;
    }

    private int ReferenceCount = 0;

    public SkinInstance Instantiate_Main()
    {
        if (!isValid)
        {
            return null;
        }
        SkinInstance go = null;
        if (Main_CacheList.Count > 0)
        {
            go = Main_CacheList.Dequeue();
            while(SkinInstance.isNullOrEmpry(go))
            {
                if(Main_CacheList.Count <= 0)
                {
                    CreateSkinObject(this, ref go);
                    break;
                }
                go = Main_CacheList.Dequeue();
            }

            go.isBeCached = false;
            go.SkinObj.SetActive(true);
        }
        else
        {
            CreateSkinObject(this, ref go);
        }
        return go;
    }

    public void Clear(bool fullUnLoad)
    {
        foreach (SkinInstance g in AllSkin)
        {
           GameObject temp = g.SkinObj;
           ResSkinObj.DestroyInstanceRes(ref temp);
           g.Clear();
        }

        foreach(BakeMeshNode bk in AllBakeMesh)
        {
            SkinInstance.ClearBakeMesh(bk);
        }
        currentCacheTime = 0.0f;
        AllBakeMesh.Clear();
        AllSkin.Clear();
        //m_SkinInstance.Clear();
        //m_SkinDataObj.Clear();
        //m_OverrideControllerTable.Clear();
        Main_CacheList.Clear();
        ReferenceCount = 0;
        if (fullUnLoad)
        {
            ClearSaveOrginalData();
            SkinCheckObj = null;
            ResSkinObj = null;
            bChecked = false;
            CheckedReslut = false;
            isAsync = false;
            isValid = false;
        }

       
    }

    /// <summary>
    /// 创建一个皮肤
    /// </summary>
    /// <param name="sk"></param>
    /// <returns></returns>
    public static void CreateSkinObject(Skin sk,ref SkinInstance result/*, bool bBuildSkinInstance,
                                                RuntimeAnimatorController srcController = null*/)
    {
        if(!sk.isValid)
        {
            return ;
        }
        GameObject newSkinObj = null;
        if (!sk.CheckedReslut)
        {
            return ;
        }

        newSkinObj = sk.ResSkinObj.InstanceMainRes();
       
        if (EntityFactory.SkinCacheGroup!=null)
        {
            newSkinObj.transform.SetParent(EntityFactory.SkinCacheGroup.transform);
        }    
        newSkinObj.SetActive(true);

        //一定要放在最后才能取到数据
        SkinInstance.BuildSkinInstance(sk, newSkinObj, ref result);
        sk.SaveOrginalData(result);
        sk.AllSkin.Add(result);
        
        //if (bBuildSkinInstance)
        //{
        //    sk.m_SkinDataObj.Add(newSkinObj, newSkinObj.transform.FindChild("SkinDataObj"));
        //    //一定要放在最后才能取到数据
        //    BuildSkinInstance(sk, newSkinObj);
        //}
    }



    #region 异步

    public bool isAsync = false;

    #endregion
}