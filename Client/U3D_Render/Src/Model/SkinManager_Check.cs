/*******************************************************************
** 文件名:	SkinManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤管理器
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

/// <summary>
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager 
{
    /// <summary>
    /// 检查这个皮肤是否合法
    /// </summary>
    /// <param name="sk"></param>
    /// <returns></returns>
    public static bool CheckSkinValid(Skin sk)
    {
        switch(sk.SkinType)
        {
            case ENTITY_TYPE.TYPE_MONSTER:
                return CheckMonsterSkin(sk);
            case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                return CheckPlayerSkin(sk);
            case ENTITY_TYPE.TYPE_TANK:
                return CheckTankSkin(sk);
            default:
                return false;
        }
    }

    private static bool CheckMonsterSkin(Skin skin)
    {
        if(skin.bChecked)
        {
            return skin.CheckedReslut ;
        }
        if(null == skin.ResSkinObj || !skin.ResSkinObj.IsValid())
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            return false;
        }
        skin.SkinCheckObj = skin.ResSkinObj.InstanceMainRes();
        if (!skin.SkinCheckObj)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;

            Debug.LogWarning("CheckMonsterSkin:skin.SkinObj 为 null!!");
            return false;
        }

        if (skin.SkinType != ENTITY_TYPE.TYPE_MONSTER)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "类型与皮肤类型不一致" + "ID:" + skin.nSkinID +  "," +
            "目标类型" + skin.SkinType.ToString() + ",需要类型：" + ENTITY_TYPE.TYPE_MONSTER.ToString() + "，请请[--策划--]检查配置文件");
            return false;
        }

        GameObject go = skin.SkinCheckObj;

        //先检查skinnedmeshrender,对于animator来说，如果skinnedmeshrender丢失的话，打包的时候会崩溃
        //所以要检查每一个预制体的skinned是否正常
        SkinnedMeshRenderer[] allSkinneds = go.GetComponentsInChildren<SkinnedMeshRenderer>(true);
        foreach (SkinnedMeshRenderer sk in allSkinneds)
        {
            if (!sk.sharedMesh)
            {
                skin.bChecked = true;
                skin.CheckedReslut = false;
                skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
                skin.SkinCheckObj = null;
                Debug.LogWarning("ID:" + skin.nSkinID + ",名字:" + go.name + "丢失mesh，请[策划]检查资源");
                return false;
            }
        }

        Transform SkinDataTransform = go.transform.FindChild("SkinDataObj");
        GameObject SkinDataObj = null;
        if (null != SkinDataTransform)
        {
            SkinDataObj = SkinDataTransform.gameObject;
        }
        else
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应皮肤预制体找不到SkinDataObj" + "ID:" + skin.nSkinID + ",名字:" + go.name + "请请[--策划--]检查预制体!");
            return false;
        }

        CreatureProperty cp = SkinDataObj.GetComponent<CreatureProperty>();

        if (!cp)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应的皮肤上的SkinDataObj没有CreatureProperty组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }

        MonsterSkinConfig msp = SkinDataObj.GetComponent<MonsterSkinConfig>();

        if (!msp)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应的皮肤上的SkinDataObj没有MonsterSkinConfig组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }
        skin.skinConfig = msp;
        Animator HostAnimator = null;
        if (!skin.isStaticSkin)
        {
            HostAnimator = go.GetComponent<Animator>();
            if (!HostAnimator)
            {
                skin.bChecked = true;
                skin.CheckedReslut = false;
                skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
                skin.SkinCheckObj = null;
                Debug.LogWarning("皮肤错误:" + "对应的皮肤上的MonsterStateMachineParams的HostAnimator成员为null，ID：" + skin.nSkinID + ",名字:" + go.name +
                        "，请请[--策划--]在Unity中检查对应的皮肤预制");
                return false;
            }

            if (HostAnimator.runtimeAnimatorController == null || HostAnimator.avatar == null)
            {
                skin.bChecked = true;
                skin.CheckedReslut = false;
                skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
                skin.SkinCheckObj = null;
                Debug.LogWarning("皮肤错误:" + "对应的皮肤上的Animator中Controller或者avator为null，ID：" + skin.nSkinID + ",名字:" + go.name +
                            "，请请[--策划--]在Unity中检查对应的皮肤预制");
                return false;
            }
        }
        //Editor模式下检查资源
        if (Application.isEditor)
        {
            SkinnedMeshRenderer[] renders = skin.SkinCheckObj.GetComponentsInChildren<SkinnedMeshRenderer>();
            foreach (SkinnedMeshRenderer r in renders)
            {
                if (!r.sharedMesh)
                {
                    Debug.LogWarning(skin.ResSkinObj.AssetName + "Mesh资源丢失!请请[--策划--]检查资源或预制体");
                }

                if (!r.sharedMaterial)
                {
                    Debug.LogWarning(skin.ResSkinObj.AssetName + "材质资源丢失!请请[--策划--]检查资源或预制体");
                }
            }
        }
        skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
        skin.bChecked = true;
        skin.CheckedReslut = true;
        skin.SkinCheckObj = null;
        return true;
    }

    private static bool CheckPlayerSkin(Skin skin)
    {
        if (skin.bChecked)
        {
            return skin.CheckedReslut;
        }
        if (null == skin.ResSkinObj || !skin.ResSkinObj.IsValid())
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            return false;
        }
        skin.SkinCheckObj = skin.ResSkinObj.InstanceMainRes();
        if (!skin.SkinCheckObj)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            Debug.LogWarning("CheckPlayerSkin:skin.SkinObj 为 null!!");
            return false;
        }

        if (skin.SkinType != ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "类型与皮肤类型不一致" + "ID:" + skin.nSkinID + ",名字:" + "," +
            "目标类型" + skin.SkinType.ToString() + ",需要类型：" + ENTITY_TYPE.TYPE_PLAYER_ROLE.ToString() + "，请请[--策划--]检查配置文件");
            return false;
        }

        GameObject go = skin.SkinCheckObj;

        //先检查skinnedmeshrender,对于animator来说，如果skinnedmeshrender丢失的话，打包的时候会崩溃
        //所以要检查每一个预制体的skinned是否正常
        SkinnedMeshRenderer[] allSkinneds = go.GetComponentsInChildren<SkinnedMeshRenderer>(true);
        foreach (SkinnedMeshRenderer sk in allSkinneds)
        {
            if(!sk.sharedMesh)
            {
                skin.bChecked = true;
                skin.CheckedReslut = false;
                skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
                skin.SkinCheckObj = null;
                Debug.LogWarning("ID:" + skin.nSkinID + ",名字:" + go.name  + "丢失mesh，请[策划]检查资源");
                return false;
            }
        }

        Transform SkinDataTransform = go.transform.FindChild("SkinDataObj");
        GameObject SkinDataObj = null;
        if (null != SkinDataTransform)
        {
            SkinDataObj = SkinDataTransform.gameObject;
        }
        else
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应皮肤预制体找不到SkinDataObj" + "ID:" + skin.nSkinID + ",名字:" + go.name + "请请[--策划--]检查预制体!");
            return false;
        }

        CreatureProperty cp = SkinDataObj.GetComponent<CreatureProperty>();

        if (!cp)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应的皮肤上的SkinDataObj没有CreatureProperty组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }

        PersonSkinConfig msp = SkinDataObj.GetComponent<PersonSkinConfig>();

        if (!msp)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:" + "对应的皮肤上的SkinDataObj没有PersonSkinConfig组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }
        skin.skinConfig = msp;
        Animator ani = go.GetComponent<Animator>();
        if (!ani)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:对应的皮肤上没有Animator组件，ID：" + skin.nSkinID + ",名字:" + go.name + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }
        if (ani.runtimeAnimatorController == null || ani.avatar == null)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:对应的皮肤上的Animator中Controller或者avator为null，ID：" + skin.nSkinID + ",名字:" + go.name +
                        "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }

        CharacterController con = SkinDataObj.GetComponent<CharacterController>();
        if (!con)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:对应的皮肤上的SkinDataObj没有CharacterController组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }

        CameraProperty cpp = SkinDataObj.GetComponent<CameraProperty>();
        if (!cpp)
        {
            skin.bChecked = true;
            skin.CheckedReslut = false;
            skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
            skin.SkinCheckObj = null;
            Debug.LogWarning("皮肤错误:对应的的SkinDataObj皮肤上没有CameraProperty组件，ID：" + skin.nSkinID + ",名字:" + go.name +
                 "，请请[--策划--]在Unity中检查对应的皮肤预制");
            return false;
        }

        //Editor模式下检查资源
        if (Application.isEditor)
        {
            SkinnedMeshRenderer[] renders = skin.SkinCheckObj.GetComponentsInChildren<SkinnedMeshRenderer>();
            foreach (SkinnedMeshRenderer r in renders)
            {
                if (!r.sharedMesh)
                {
                    Trace.Warning(skin.ResSkinObj.AssetName + "Mesh资源丢失!请请[--策划--]检查资源或预制体");
                }

                if (!r.sharedMaterial)
                {
                    Trace.Warning(skin.ResSkinObj.AssetName + "材质资源丢失!请请[--策划--]检查资源或预制体");
                }
            }
        }

        skin.ResSkinObj.DestroyInstanceRes(ref skin.SkinCheckObj);
        skin.bChecked = true;
        skin.CheckedReslut = true;
        skin.SkinCheckObj = null;
        return true;
    }

    private static bool CheckTankSkin(Skin sk)
    {
        return false;
    }
}


