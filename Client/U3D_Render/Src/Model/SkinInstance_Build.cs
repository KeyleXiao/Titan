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
    public static void BuildSkinInstance(Skin sk, GameObject instanceSkin, ref SkinInstance skinInstance)
    {
        if (!sk.isValid)
        {
            return;
        }
        switch (sk.SkinType)
        {
            case ENTITY_TYPE.TYPE_MONSTER:
                BuildMonsterSkinInstance(sk, instanceSkin, ref skinInstance);
                return;
            case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                BuildPlayerSkinInstance(sk, instanceSkin, ref skinInstance);
                return;
            case ENTITY_TYPE.TYPE_TANK:
                BuildTankSkinInstance(sk, instanceSkin, ref skinInstance);
                return;
            default:
                return;
        }
    }



    private static void BuildMonsterSkinInstance(Skin skin, GameObject instanceSkin, ref SkinInstance si)
    {
        if (!skin.isValid)
        {
            return;
        }

        if (null == si)
        {
            si = new SkinInstance();
            si.m_nSkinID = skin.nSkinID;
        }

        si.m_SkinType = skin.SkinType;
        Transform SkinDataObj = instanceSkin.transform.FindChild("SkinDataObj");
        SkinDataObj.localPosition = Vector3.zero;
        SkinDataObj.localRotation = Quaternion.identity;
        SkinDataObj.localScale = Vector3.one;
        MonsterSkinConfig msp = SkinDataObj.GetComponent<MonsterSkinConfig>();
        si.m_SkinDataObj = SkinDataObj;
        if (msp.ColliderShape)
        {
            msp.StandradColliderShapeSize = msp.ColliderShape.transform.localScale;
        }

        if (msp.OcclusCollider)
        {
            msp.OcclusCollider.isTrigger = true;
            msp.gameObject.layer = LayerMask.NameToLayer(Config.LayerWater);
        }

        si.m_entitySkinConfig = msp;
        si.m_SkinObj = instanceSkin;
        si.m_creatureProperty = SkinDataObj.GetComponent<CreatureProperty>();
        si.m_ConfigScale = skin.fSkinScale;
        si.m_PrefabScale = instanceSkin.transform.localScale;
        if (!skin.isStaticSkin)
        {
            si.m_MainAnimator = instanceSkin.GetComponent<Animator>();
        }

        bool enableAnimator = true;
        if (msp.BodyNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = msp.BodyRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                msp.BodyRender = MakeBakeMeshRender(smr, nod);
                enableAnimator = false;
            }

        }

        if (msp.RightWeaponNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = msp.RightWeaponRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                msp.RightWeaponRender = MakeBakeMeshRender(smr, nod);
                enableAnimator = false;
            }
        }
        if (msp.LeftWeaponNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = msp.LeftWeaponRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                msp.LeftWeaponRender = MakeBakeMeshRender(smr, nod);
                enableAnimator = false;
            }

        }

        if (msp.BackBodyNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = msp.BackBodyRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                msp.BackBodyRender = MakeBakeMeshRender(smr, nod);
                enableAnimator = false;
            }

        }

        if (!skin.isStaticSkin)
        {
            if (!enableAnimator)
            {
                si.MainAnimator.enabled = false;
                si.m_MainAnimator = null;
            }

        }

        si.m_switchEffectWithHeroDistance = SkinDataObj.GetComponent<Effect.SwitchEffectWithHeroDistance>();
        si.m_playEntityStaticEffect = SkinDataObj.GetComponent<Effect.Effect_PlayEntityStaticEffect>();

        si.SkinnedMeshRenderList.AddRange(instanceSkin.GetComponentsInChildren<SkinnedMeshRenderer>());
        si.MeshRenderList.AddRange(instanceSkin.GetComponentsInChildren<MeshRenderer>());


        int length = si.SkinnedMeshRenderList.Count;
        //移除skindataobj下的物体
        for (int k = 0; k < length; k++)
        {
            si.SkinnedMeshRenderList[k].motionVectors = false;
            si.SkinnedMeshRenderList[k].skinnedMotionVectors = false;
            Transform t = si.SkinnedMeshRenderList[k].transform;
            if (t.IsChildOf(SkinDataObj.transform))
            {
                si.SkinnedMeshRenderList.RemoveAt(k);
            }
            else
            {
                si.AllRenders.Add(si.SkinnedMeshRenderList[k]);
            }
        }

        length = si.MeshRenderList.Count;
        //移除skindataobj下的物体
        for (int k = 0; k < length; k++)
        {
            si.MeshRenderList[k].motionVectors = false;
            Transform t = si.MeshRenderList[k].transform;
            if (t.IsChildOf(SkinDataObj.transform))
            {
                si.MeshRenderList.RemoveAt(k);
            }
            else
            {
                si.AllRenders.Add(si.MeshRenderList[k]);
            }
        }

        //必须放到最后
        if (si.playEntityStaticEffect)
        {
            si.playEntityStaticEffect.Init();
        }
        si.Init();
        return;
    }


    private static void BuildPlayerSkinInstance(Skin skin, GameObject instanceSkin, ref SkinInstance si)
    {
        if (!skin.isValid)
        {
            return;
        }
        if (null == si)
        {
            si = new SkinInstance();
            si.m_nSkinID = skin.nSkinID;
        }
        si.m_SkinType = skin.SkinType;
        Transform SkinDataObj = instanceSkin.transform.FindChild("SkinDataObj");
        SkinDataObj.localPosition = Vector3.zero;
        SkinDataObj.localRotation = Quaternion.identity;
        SkinDataObj.localScale = Vector3.one;
        PersonSkinConfig psc = SkinDataObj.GetComponent<PersonSkinConfig>();

        si.m_SkinDataObj = SkinDataObj;
        si.m_SkinObj = instanceSkin;
        si.m_characterController = SkinDataObj.GetComponent<CharacterController>();
        si.m_characterController.enabled = false;
        si.m_entitySkinConfig = psc;
        si.m_cameraProperty = SkinDataObj.GetComponent<CameraProperty>();
        if (psc.AttackAnimatorLayer > 0)
        {
            si.m_MainAnimator = instanceSkin.GetComponent<Animator>();
            si.m_AnimatorB = si.MainAnimator;

            si.m_overrideController = new AnimatorOverrideController();
            si.orginalAnimatorController = si.MainAnimator.runtimeAnimatorController;
            si.overrideController.runtimeAnimatorController = si.orginalAnimatorController;

            si.MainAnimator.runtimeAnimatorController = si.overrideController;
        }
        else
        {
            Debug.LogError("BuildPlayerSkinInstance--- psc.AttackAnimatorLayer <= 0");
        }
        //else
        //{
        //    Debug.Log("--------------皮肤调试信息-----------------------" + instanceSkin.name);
        //    GameObject goa = CreateSkinObject(skin, false, psc.controllerA);
        //    GameObject gob = CreateSkinObject(skin, false, psc.controllerB);

        //    Transform skina = goa.transform.FindChild("SkinDataObj");
        //    if (skina != null)
        //    {
        //        var temp = skina.gameObject;
        //        ResNode.DestroyRes(ref temp);
        //    }

        //    Transform skinb = gob.transform.FindChild("SkinDataObj");
        //    if (skinb != null)
        //    {
        //        var temp = skinb.gameObject;
        //        ResNode.DestroyRes(ref temp);
        //    }

        //    Renderer[] rsa = goa.GetComponentsInChildren<Renderer>();
        //    foreach (Renderer ra in rsa)
        //    {
        //        var temp = ra;
        //        ResNode.DestroyRes(ref temp);
        //    }

        //    Renderer[] rsb = gob.GetComponentsInChildren<Renderer>();
        //    foreach (Renderer rb in rsb)
        //    {
        //        var temp = rb;
        //        ResNode.DestroyRes(ref temp);
        //    }

        //    si.MainAnimator = goa.GetComponent<Animator>();

        //    si.AnimatorB = gob.GetComponent<Animator>();

        //    Animator anoldc = instanceSkin.GetComponent<Animator>();
        //    if (anoldc != null)
        //    {
        //        ResNode.DestroyRes(ref anoldc);
        //    }

        //    si.bipedIKAnimator = instanceSkin.AddComponent<BipedIKAnimator>();
        //    si.bipedIKAnimator.goA = goa;
        //    si.bipedIKAnimator.goB = gob;
        //    si.bipedIKAnimator.Build();

        //    si.SkinObj_A = goa;
        //    si.SkinObj_B = gob;
        //}

        si.MainAnimator.applyRootMotion = false;
        si.MainAnimator.cullingMode = AnimatorCullingMode.AlwaysAnimate;//有的动作会飞出屏幕，所以需要这个

        si.AnimatorB.applyRootMotion = false;
       // si.AnimatorB.cullingMode = AnimatorCullingMode.AlwaysAnimate;

        si.m_bindSound = instanceSkin.AddComponent<BindSound>();
        si.m_bindEffect_A = instanceSkin.AddComponent<BindEffect>();

        if (si.SkinObj_B)
        {
            si.m_bindEffect_B = si.SkinObj_B.AddComponent<BindEffect>();
        }
        else
        {
            si.m_bindEffect_B = instanceSkin.AddComponent<BindEffect>();
        }
        if (psc.weaponObject)
        {
            si.m_weaponAnimator = psc.weaponObject.GetComponent<Animator>();
        }
        if (psc.weaponObject2)
        {
            si.m_weaponAnimator2 = psc.weaponObject2.GetComponent<Animator>();
        }

        si.m_creatureProperty = SkinDataObj.GetComponent<CreatureProperty>();
        si.m_sidekickEntity = SkinDataObj.GetComponent<SidekickEntity>();
        if (si.sidekickEntity)
        {
            si.sidekickEntity.init();
        }
        si.m_ConfigScale = skin.fSkinScale;
        si.m_PrefabScale = instanceSkin.transform.localScale;

        if (psc.BodyNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = psc.BodyRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                psc.BodyRender = MakeBakeMeshRender(smr, nod);
            }

        }

        if (psc.RightWeaponNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = psc.RightWeaponRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                psc.RightWeaponRender = MakeBakeMeshRender(smr, nod);
            }
        }
        if (psc.LeftWeaponNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = psc.LeftWeaponRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                psc.LeftWeaponRender = MakeBakeMeshRender(smr, nod);
            }

        }

        if (psc.BackBodyNeedToBakeMesh)
        {
            SkinnedMeshRenderer smr = psc.BackBodyRender as SkinnedMeshRenderer;
            BakeMeshNode nod = BakeMesh(smr);
            if (null != nod)
            {
                skin.AllBakeMesh.Add(nod);
                psc.BackBodyRender = MakeBakeMeshRender(smr, nod);
            }

        }

        si.m_characterShadow = instanceSkin.AddComponent<CharacterShadow>();

        si.m_characterShadow.Bake(instanceSkin.transform, psc.BodyRender);
        si.m_characterShadow.Init(Initialize.mainCam);
        //si.m_characterShadow.enabled = false;

        si.m_switchEffectWithHeroDistance = SkinDataObj.GetComponent<Effect.SwitchEffectWithHeroDistance>();
        si.m_playEntityStaticEffect = SkinDataObj.GetComponent<Effect.Effect_PlayEntityStaticEffect>();

        si.SkinnedMeshRenderList.AddRange(instanceSkin.GetComponentsInChildren<SkinnedMeshRenderer>());
        si.MeshRenderList.AddRange(instanceSkin.GetComponentsInChildren<MeshRenderer>());

        foreach (SkinnedMeshRenderer skr in si.SkinnedMeshRenderList)
        {
            RenderVisibleHelper vb = skr.gameObject.AddComponent<RenderVisibleHelper>();
            si.VisibleHelperList.Add(vb);
        }

        int length = si.SkinnedMeshRenderList.Count;
        //移除skindataobj下的物体
        for (int k = 0; k < length; k++)
        {
            si.SkinnedMeshRenderList[k].motionVectors = false;
            si.SkinnedMeshRenderList[k].skinnedMotionVectors = false;
            Transform t = si.SkinnedMeshRenderList[k].transform;
            if (t.IsChildOf(SkinDataObj.transform))
            {
                si.SkinnedMeshRenderList.RemoveAt(k);
            }
            else
            {
                si.AllRenders.Add(si.SkinnedMeshRenderList[k]);
            }
        }

        length = si.MeshRenderList.Count;
        //移除skindataobj下的物体
        for (int k = 0; k < length; k++)
        {
            si.MeshRenderList[k].motionVectors = false;
            Transform t = si.MeshRenderList[k].transform;
            if (t.IsChildOf(SkinDataObj.transform))
            {
                si.MeshRenderList.RemoveAt(k);
            }
            else
            {
                si.AllRenders.Add(si.MeshRenderList[k]);
            }
        }

        if (si.playEntityStaticEffect)
        {
            si.playEntityStaticEffect.Init();
        }
        si.Init();

        return;
    }

    private static void BuildTankSkinInstance(Skin sk, GameObject instanceSkin, ref SkinInstance si)
    {
        if (!sk.isValid)
        {
            return;
        }


        return;
    }

    #region BakedMesh


    static Dictionary<Mesh, BakeMeshNode> MeshBakerTable = new Dictionary<Mesh, BakeMeshNode>();

    private static Renderer MakeBakeMeshRender(SkinnedMeshRenderer ms, BakeMeshNode nod)
    {
        MeshRenderer mr = null;
        if (nod.IsValid())
        {
            Material[] sharedMaterial = ms.sharedMaterials;
            GameObject go = ms.gameObject;
            GameObject.DestroyImmediate(ms);
            MeshFilter mf = go.GetComponent<MeshFilter>();
            if (!mf)
            {
                mf = go.AddComponent<MeshFilter>();
            }
            mr = go.GetComponent<MeshRenderer>();
            if (!mr)
            {
                mr = go.AddComponent<MeshRenderer>();
            }

            mf.sharedMesh = nod.BakedMesh;
            mr.sharedMaterials = sharedMaterial;
        }
        return mr;
    }

    private static BakeMeshNode BakeMesh(SkinnedMeshRenderer ms)
    {
        BakeMeshNode bk = null;
        if (!ms || !ms.sharedMesh)
        {
            return bk;
        }

        if (MeshBakerTable.TryGetValue(ms.sharedMesh, out bk))
        {
            bk.RefCount++;
        }
        else
        {
            bk = new BakeMeshNode();
            bk.OrginalMesh = ms.sharedMesh;
            bk.BakedMesh = new Mesh();
            ms.BakeMesh(bk.BakedMesh);
            bk.RefCount++;
            MeshBakerTable.Add(bk.OrginalMesh, bk);
        }

        return bk;
    }

    public static void ClearBakeMesh(BakeMeshNode nod)
    {
        if (null == nod)
        {
            return;
        }

        int refCount = --nod.RefCount;
        if (refCount <= 0)
        {
            MeshBakerTable.Remove(nod.OrginalMesh);
            GameUtil.DestroyMesh(nod.BakedMesh);
            nod = null;
        }
    }

    #endregion

}
