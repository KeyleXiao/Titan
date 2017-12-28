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
using ASpeedGame.Data.PersonModelTransFormInfo;
using USpeedUI.UEffect;
using Effect;

public enum HEROSKIN_RENDER_TYPE
{
    SCENE,
    CAMERA,
}
/// <summary>
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager
{

    public static void OnUpdate()
    {
        UpdateSkinCache();
        ProcessSkinAsyncLoad();
        UpdateCulling();
    }

    public static void Init()
    {
        InitCulling();
    }

    public static void Clear()
    {
        ClearCulling();
        m_SkinConfigMap.Clear();
        m_SkinConfigMap = null;

        UnLoadAllSkin();
        Resources.UnloadUnusedAssets();
    }

    /// <summary>
    /// 加载皮肤
    /// </summary>
    /// <param name="ite">实体信息</param>
    /// <returns></returns>
    public static bool LoadSkin(EntityViewItem ite)
    {
        SkinModelInfo info = GetSkinInfoFromEntityViewItem(ite);

        if (m_SkinConfigMap.ContainsKey(info.nSkinID))
        {
            Trace.LogWarning("加载皮肤模型重复,ID:" + info.nSkinID + ",路径:" + info.strPatch + "，请请[--策划--]删除重复的ID");
            return true;
        }
        m_SkinConfigMap.Add(info.nSkinID, info);
        return true;
    }

    /// <summary>
    /// 取得皮肤
    /// </summary>
    /// <param name="SkinID"></param>
    /// <param name="sk"></param>
    /// <returns></returns>
    public static SkinInstance GetSkin(int SkinID)
    {
        if (null == m_SkinConfigMap)
        {
            return null;
        }

        Skin Reslut = null;
        Reslut = GetSkinFromCache(SkinID);
        if(Reslut != null)
        {
            Reslut.AddRef();
            m_SkinMap.Add(Reslut.nSkinID, Reslut);
            return Reslut.Instantiate_Main();
        }

        //先看看这个皮肤是不是有这个皮肤
        if (m_SkinConfigMap.ContainsKey(SkinID))
        {
            //再看看这个皮肤是否被加载了
            if (m_SkinMap.ContainsKey(SkinID))
            {
                Reslut = m_SkinMap[SkinID];
                Reslut.AddRef();
                return Reslut.Instantiate_Main();
            }

            //没有就加载皮肤
            SkinModelInfo info = m_SkinConfigMap[SkinID];
            Skin sk = LoadSkinSynchronous(info);
            if(null != sk && sk.isValid)
            {
                return sk.Instantiate_Main();
            }
            return null;
        }
        else
        {
            Trace.LogWarning("找不到皮肤配置,ID:" + SkinID);
        }
        return null;
    }

    /// <summary>
    /// 加载模型
    /// </summary>
    /// <param name="SkinID">模型ID</param>
    /// <param name="strPrefabPath">模型路径</param>
    /// <returns>是否成功</returns>
    private static Skin LoadSkinSynchronous(SkinModelInfo info)
    {
        Skin reslut = null;
        //已经有这个皮肤，不加载了
        if (m_SkinMap.TryGetValue(info.nSkinID,out reslut))
        {
            return reslut;
        }


        ResNode obj = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, info.strPatch);

        if (null != obj)
        {
            reslut = BuildSkin(info, obj, obj.isAsync);
            if(reslut != null)
            {
                reslut.AddRef();
                SkinAsyncLoadingObj asyncObj = null;
                if (m_AsyncLoadSkinRequestTable.TryGetValue(info.nSkinID, out asyncObj))
                {
                    reslut.AddRef(asyncObj.ReferenceCount);
                    asyncObj.ReferenceCount = 0;
                }
            }
            return reslut;
        }
        else
        {
            obj = null;
            Trace.Warning("加载皮肤模型失败,ID:" + info.nSkinID + ",路径:" + info.strPatch + ",请[--策划--]检查配置");
            return reslut;
        }
    }

    public static void DeleteSkin(ref SkinInstance instance)
    {
        if(null == instance)
        {
            return;
        }

        if(instance.nSkinID <= 0)
        {
            return;
        }

        if(!instance.isLoadFinish)
        {
            Debug.LogWarning("异步加载的皮肤，使用了同步卸载接口,可能会出现一些问题:" + instance.nSkinID);
        }
        SkinManager.CacheSkinInstance(instance);

        ReleaseSkin(instance.nSkinID);
        instance = null;
    }

    private static void ReleaseSkin(int SkinID)
    {
        Skin sk = null;
        m_SkinMap.TryGetValue(SkinID, out sk);
        ReleaseSkin(ref sk);
    }

    private static void ReleaseSkin(ref Skin sk)
    {
        if(null == sk)
        {
            return;
        }

        int refcount = sk.Release();
        if (refcount <= 0)
        {
            UnLoadSkin(ref sk);
        }
    }

    /// <summary>
    /// 卸载异步Skin
    /// </summary>
    /// <param name="SkinID"></param>
    /// <param name="?"></param>
    /// <param name="CallBack"></param>
    private static void UnLoadSkin(ref Skin sk, bool NotReleaseFromCache = false)
    {
        if (null == sk)
        {
            return;
        }
        PushToSkinCache(sk, NotReleaseFromCache);

        int SkinID = sk.nSkinID;
        currentActiveSkinList.Remove(SkinID);
        m_SkinMap.Remove(SkinID);
       // UnLoadSkinRes(ref sk);
        sk = null;

        

    }

    private static void UnLoadSkinRes(ref Skin sk)
    {
        if (null == sk)
        {
            return;
        }
        int SkinID = sk.nSkinID;
        bool isAsync = sk.isAsync;
        ResNode res = sk.ResSkinObj;
        sk.Clear(true);

        if (isAsync)
        {
            AssetBundleManager.DeleteAssetsAsync(ref res, ResLoadCallBack, ImmdieateUnLoadSkin);
        }
        else
        {
            AssetBundleManager.DeleteAssets(ref res, ImmdieateUnLoadSkin);
        }

        sk = null;
    }

    private static Skin BuildSkin(SkinModelInfo info, ResNode obj,bool isAsync)
    {
        Skin sk = null;
        if(!m_SkinMap.TryGetValue(info.nSkinID,out sk))
        {
            sk = new Skin();
            sk.nSkinID = info.nSkinID;
            m_SkinMap.Add(info.nSkinID, sk);
        }

        if (obj != null)
        {
            sk.SkinType = info.SkinType;
            sk.fSkinScale = info.fSkinScale;
            sk.ResSkinObj = obj;
            sk.isStaticSkin = info.strPatch.Contains(staticSkinPatch);
            sk.isAsync = isAsync;
            if (!CheckSkinValid(sk))
            {
                if (!isAsync)
                {
                    AssetBundleManager.DeleteAssets(ref obj, ImmdieateUnLoadSkin);
                }
                else
                {
                    AssetBundleManager.DeleteAssetsAsync(ref obj, ResLoadCallBack, ImmdieateUnLoadSkin);
                }
                sk.isValid = false;
                obj = null;
                return sk;
            }

            sk.isValid = true;
            obj = null;
            return sk;
        }
        else
        {
            sk.isValid = false;
            Debug.LogWarning("加载皮肤模型失败,ID:" + info.nSkinID + ",路径:" + info.strPatch + ",请[--策划--]检查配置");
            return null;
        }
    }

    private static SkinModelInfo GetSkinInfoFromEntityViewItem(EntityViewItem ite)
    {
        SkinModelInfo inf = new SkinModelInfo();
        inf.nSkinID = ite.nSkinID;
        inf.strPatch = ite.szModelPath;
        inf.SkinType = (ENTITY_TYPE)ite.EntityType;
        inf.fSkinScale = ite.fSkinScale;
        return inf;
    }

    public static bool SetChangeHeroSkin(int nSkinId, int nHeroId, ref SkinInstance objLastSkin, ref int nLastSkinId, Transform transHeroFrame, HEROSKIN_RENDER_TYPE renderType = HEROSKIN_RENDER_TYPE.SCENE,PERSONMODELVIEWID viewType = PERSONMODELVIEWID.PERSONMODELVIEWID_GAMEWAITINGROOM)
    {
        SkinManager.DeleteSkin(ref objLastSkin);

        nLastSkinId = 0;
        objLastSkin = null;

        if (nSkinId < 0)
        {
            Debug.LogWarning("SetChangeHeroSkin---皮肤ID小于0");
             return false;
        }
           

        SkinInstance si = SkinManager.GetSkin(nSkinId);
        if (si == null || !si.isValid)
        {
            Trace.LogWarning("切换皮肤失败，找不到对应的皮肤ID：" + nSkinId + "，请[--策划--]检查配置文件");
            return false;
        }

        GameObject goc = si.SkinObj;

        // 动画
        if (viewType == PERSONMODELVIEWID.PERSONMODELVIEWID_SHOP)
        {
            si.MainAnimator.Play("loginr");
        }


        //effectNodeHero.PlayEffect(0, null, null);
        PersonModelTransFormInfo instance = PersonModelTransFormInfo.Instance;
        Vector3 pos = instance.GetHeroModelPosition(viewType, nHeroId);
        Vector3 scale = instance.GetHeroModelScale(viewType, nHeroId);
        Vector3 rotation = instance.GetHeroModelRotation(viewType, nHeroId);
        if (renderType == HEROSKIN_RENDER_TYPE.CAMERA)
        {
            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerUI), goc, true);
            Vector2 size = new Vector2(1280f, 720f);

            UEffectMeshObject MeshObject = new UEffectMeshObject(si.SkinObj, pos, rotation, scale, _bIsSceneObj: true);
            UEffectMeshCamera MeshCamera = new UEffectMeshCamera();
            UEffectParamBase param = new UEffectMeshParam(size, new List<UEffectMeshObject>() { MeshObject }, MeshCamera, transHeroFrame);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMesh, ref param);
        }
        else if (renderType == HEROSKIN_RENDER_TYPE.SCENE)
        {
            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerHero), goc, true);
            goc.transform.parent = transHeroFrame;
            goc.transform.localPosition = pos;
            goc.transform.localEulerAngles = rotation;
            goc.transform.localScale = scale;
        }
        nLastSkinId = nSkinId;
        objLastSkin = si;

        return true;
    }
  
    public static bool SetChangeHeroSkinEffect(int nSkinId, ref SceneEffectParamBase lastSkinEffectParam,ref int nLastSkinId, Transform parentTrans, PERSONMODELVIEWID viewType = PERSONMODELVIEWID.PERSONMODELVIEWID_GAMEWAITINGROOM, Action<object> resLoadFinished = null, object userObj = null)
    {
        if (lastSkinEffectParam != null)
        {
            SceneEffectManager.Instance.DestroySceneEffect(ref lastSkinEffectParam);
            lastSkinEffectParam = null;
            nLastSkinId = 0;
        }

        if (nSkinId < 0)
            return false;
        
        int nSelectEffectID = GameLogicAPI.getSelectEffectID(nSkinId);
        //SceneEffectParamBase effectParam = new SceneEffectParamAsync(nSelectEffectID, parentTrans, _onResLoadFinished: resLoadFinished, _resLoadUserObject: userObj);
        SceneEffectParamBase effectParam = new SceneEffectParamAsync(nSelectEffectID, parentTrans, AssetLoadPriority.Priority_Exclusive, _onResLoadFinished: resLoadFinished, _resLoadUserObject: userObj);
        SceneEffectManager.Instance.CreateSceneEffect(ref effectParam);

        lastSkinEffectParam = effectParam;
        nLastSkinId = nSkinId;

        return true;
    }

    public static string GetSkinName(int SkinID)
    {
        Skin sk = null;
        if(m_SkinMap.TryGetValue(SkinID,out sk))
        {
            if(sk.isValid)
            {
                return sk.ResSkinObj.AssetName;
            }
        }
        return "NotFindSkin";
    }

    public static void EnableSkinInstance(SkinInstance instance)
    {
        if (null == instance)
        {
            return;
        }
        if (!instance.isValid)
        {
            return;
        }
        if (instance.playEntityStaticEffect)
        {
            instance.playEntityStaticEffect.Stop();
        }

        if (instance.switchEffectWithHeroDistance)
        {
            instance.switchEffectWithHeroDistance.DisableAll();
        }
        instance.SkinObj.SetActive(true);

        BipedIKAnimator anc = instance.SkinObj.GetComponent<BipedIKAnimator>();
        if (anc != null)
        {
            if (anc.goA != null)
            {
                anc.goA.SetActive(true);
            }
            if (anc.goB != null)
            {
                anc.goB.SetActive(true);
            }
        }
    }

    public static void CacheSkinInstance(SkinInstance instance)
    {
        if (SkinInstance.isNullOrEmpry(instance))
        {
            return;
        }

        if (instance.isBeCached)
        {
            return;
        }

        instance.isBeCached = true;
        Skin sk = null;
        m_SkinMap.TryGetValue(instance.nSkinID, out sk);
        sk.Main_CacheList.Enqueue(instance);
        if (EntityFactory.SkinCacheGroup != null)
        {
            instance.SkinObj.transform.parent = EntityFactory.SkinCacheGroup.transform;
        }
        DisableSkinInstance(instance);
        instance.SkinObj.transform.localScale = instance.PrefabScale;
        instance.SkinObj.transform.localPosition = Vector3.zero;
        instance.ReferencedSkinCtrl = null;
        if (instance.bindEffect_A)
        {
            instance.bindEffect_A.ClearEvent();
        }
        if (instance.bindEffect_B)
        {
            instance.bindEffect_B.ClearEvent();
        }
        if (instance.bindSound)
        {
            instance.bindSound.ClearEvent();
        }



    }

    public static void DisableSkinInstance(SkinInstance instance)
    {
        if (null == instance)
        {
            return;
        }
        if (!instance.isValid)
        {
            return;
        }
        Skin sk = null;
        m_SkinMap.TryGetValue(instance.nSkinID, out sk);
        sk.ApplyOrginalData(instance);
        if (instance.playEntityStaticEffect)
        {
            instance.playEntityStaticEffect.Stop();
        }

        if (instance.switchEffectWithHeroDistance)
        {
            instance.switchEffectWithHeroDistance.DisableAll();
        }
        instance.SkinObj.SetActive(false);
        instance.SetVisible(true);//重置可见性
        instance.SetBindEffectEnabled(false);//重置绑定特效
        BipedIKAnimator anc = instance.SkinObj.GetComponent<BipedIKAnimator>();
        if (anc != null)
        {
            if (anc.goA != null)
            {
                anc.goA.SetActive(false);
            }
            if (anc.goB != null)
            {
                anc.goB.SetActive(false);
            }
        }
    }

    /// <summary>
    /// 皮肤配置表
    /// </summary>
    private static Dictionary<int, SkinModelInfo> m_SkinConfigMap = new Dictionary<int, SkinModelInfo>();

    /// <summary>
    /// 已经加载的皮肤列表
    /// </summary>
    private static Dictionary<int, Skin> m_SkinMap = new Dictionary<int, Skin>();


    private static string staticSkinPatch = "Skin/Static";

    private static bool ImmdieateUnLoadSkin = true;
}


