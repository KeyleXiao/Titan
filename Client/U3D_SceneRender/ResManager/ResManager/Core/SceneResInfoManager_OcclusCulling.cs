/*******************************************************************
** 文件名:	SceneResInfoManager_OcclusCulling.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器,四叉树部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{


    public static bool GoablEnabledOcclusCulling = false;
    public GameObject ScenereOcclusColliderRoot = null;

    private static int RayCount = 200;
    private static int MaxPreRayCheckCount = 3;
    private static int DvidFrameCount = 10;
    private static float DealyRestTime = (FadeTime + 0.3f + 2) / DvidFrameCount;
    private static float RayCameraFov = 10;

    public bool enabledOcclusCulling = false;

    private float[] ScreenWidthPoints;
    private float[] ScreenHeightPoints;
    private int ScreenPixels = 0;
    private int currUpdateIndex = 0;

    private Camera RayCamera = null;
    private Dictionary<Transform, SceneResInfo> transform_ResTable = new Dictionary<Transform, SceneResInfo>();
    private RaycastHit hit;
    private List<SceneResInfo> NotCullList = new List<SceneResInfo>();
    private List<float> NotCullTimer = new List<float>();
    private Queue<int> NotCullEmptyIndexList = new Queue<int>();

    private List<SceneResInfo> currentFadeList = new List<SceneResInfo>();
    private Queue<Effect.SceneFadeInOutMaterialFloat> m_UnActiveOcclusFadeFadeInOutFloatList = new Queue<Effect.SceneFadeInOutMaterialFloat>();
    private Dictionary<Effect.SceneFadeInOutMaterialFloat, SceneResInfo> m_ActiveFadeSceneResTable = new Dictionary<Effect.SceneFadeInOutMaterialFloat, SceneResInfo>();
    private Dictionary<Material, Queue<Material>> m_OcclusMaterialTable = new Dictionary<Material, Queue<Material>>();

    #region Init
    bool OcclusCulingEnabled()
    {
        return GoablEnabledOcclusCulling && enabledOcclusCulling;
    }

    void OcclusCullingInit()
    {
        if (!GoablEnabledOcclusCulling)
        {
            ResNode.DestroyRes(ref ScenereOcclusColliderRoot);
            return;
        }

        InitScreenInfo();
        BuildOcclusCacheData();
    }


    void OcclusCullingClear()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (RayCamera)
        {
            var temp = RayCamera.gameObject;
            ResNode.DestroyRes(ref temp);
        }
        ClearOcclusCacheData();
        transform_ResTable.Clear();
    }



    void InitScreenInfo()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (!RayCamera)
        {

            GameObject go = new GameObject();
            go.name = "OcclusCullingCamera";
            go.transform.SetParent(SceneCamera.transform);
            go.transform.localPosition = Vector3.zero;
            RayCamera = go.AddComponent<Camera>();

            RayCamera.CopyFrom(SceneCamera);
            RayCamera.fieldOfView = 60 + RayCameraFov;

            RayCamera.enabled = false;
        }
        ScreenWidthPoints = null;
        ScreenHeightPoints = null;

        ScreenPixels = Mathf.FloorToInt(Screen.width * Screen.height / 4f);
        ScreenWidthPoints = new float[ScreenPixels];
        ScreenHeightPoints = new float[ScreenPixels];
        for (int i = 0; i < ScreenPixels; i++)
        {
            ScreenWidthPoints[i] = HaltonSequence(i, 2);
            ScreenHeightPoints[i] = HaltonSequence(i, 3);
        }

        ClearOcclusCacheData();
        CullAll();
    }


    void BuildOcclusCacheData()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        for (int i = 0; i < 100; i++)
        {
            Effect.SceneFadeInOutMaterialFloat ke = CreateNewFadeInstance();
            m_UnActiveOcclusFadeFadeInOutFloatList.Enqueue(ke);
        }
    }

    void ClearOcclusCacheData()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        CullAll();

        foreach (Effect.SceneFadeInOutMaterialFloat ke in m_ActiveFadeSceneResTable.Keys)
        {
            ke.Stop();
            ke.enabled = false;
            m_UnActiveOcclusFadeFadeInOutFloatList.Enqueue(ke);
        }
        foreach (Effect.SceneFadeInOutMaterialFloat ke in m_UnActiveOcclusFadeFadeInOutFloatList)
        {
            ke.ClearMaterialInstance();
            ke.ClearFinishEvent();
            var temp = ke;
            ResNode.DestroyRes(ref temp);
        }

        foreach (Queue<Material> matList in m_OcclusMaterialTable.Values)
        {
            foreach (Material mat in matList)
            {
                DestroyImmediate(mat);
            }

        }
        m_OcclusMaterialTable.Clear();


        m_UnActiveOcclusFadeFadeInOutFloatList.Clear();
        m_ActiveFadeSceneResTable.Clear();
        currentFadeList.Clear();
        NotCullList.Clear();
        NotCullTimer.Clear();
        NotCullEmptyIndexList.Clear();
        currUpdateIndex = 0;

    }

    private float HaltonSequence(int index, int b)
    {
        float res = 0f;
        float f = 1f / b;
        int i = index;
        while (i > 0)
        {
            res = res + f * (i % b);
            i = Mathf.FloorToInt(i / b);
            f = f / b;
        }
        return res;
    }

    #endregion

    #region Fade
    public void PushToOccFadeList(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }
        currentFadeList.Add(res);
    }

    void OcclusFadingFinish(Effect.SceneFadeInOutMaterialFloat instance)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        CollectFadingMaterial(m_ActiveFadeSceneResTable[instance]);
        m_ActiveFadeSceneResTable[instance].occlusFadingOut = false;
        m_ActiveFadeSceneResTable[instance].occlusFadingIn = false;
        m_ActiveFadeSceneResTable[instance].occlusFadInstance = null;
        m_ActiveFadeSceneResTable.Remove(instance);
        m_UnActiveOcclusFadeFadeInOutFloatList.Enqueue(instance);
        instance.enabled = false;
        instance.ClearMaterialInstance();
        instance.ClearFinishEvent();
    }

    Material[] GetFadingMaterial(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return null;
        }

        if (res.OcclusInstaneMaterial != null)
        {
            return res.OcclusInstaneMaterial;
        }
        int matCount = res.OrginalMaterList.Length;
        Material[] rest = new Material[matCount];
        for (int i = 0; i < matCount; i++)
        {
            Queue<Material> matList = null;
            Material orginal = res.OrginalMaterList[i];
            Material result = null;
            if (m_OcclusMaterialTable.TryGetValue(orginal, out matList))
            {
                if (matList.Count > 0)
                {
                    result = matList.Dequeue();
                }
                else
                {
                    result = SceneResInfo.CreateFadeingMaterial(orginal);
                }
            }
            else
            {
                result = SceneResInfo.CreateFadeingMaterial(orginal);
                matList = new Queue<Material>();
                m_OcclusMaterialTable.Add(orginal, matList);
            }
            rest[i] = result;
        }
        res.OcclusInstaneMaterial = rest;
        return rest;
    }

    void CollectFadingMaterial(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (res.OcclusInstaneMaterial == null)
        {
            return;
        }

        int matCount = res.OrginalMaterList.Length;
        for (int i = 0; i < matCount; i++)
        {
            if (!res.OcclusInstaneMaterial[i])
            {
                continue;
            }
            Queue<Material> matList = null;
            Material orginal = res.OrginalMaterList[i];
            if (m_OcclusMaterialTable.TryGetValue(orginal, out matList))
            {
                matList.Enqueue(res.OcclusInstaneMaterial[i]);
            }
            else
            {
                Debug.LogError("CollectFadingMaterial!!!");
            }
        }

        res.OcclusInstaneMaterial = null;
    }

    Effect.SceneFadeInOutMaterialFloat CreateNewFadeInstance()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return null;
        }

        Effect.SceneFadeInOutMaterialFloat ef = gameObject.AddComponent<Effect.SceneFadeInOutMaterialFloat>();
        ef.enabled = false;
        ef.PropertyName = SceneResInfo.CutOutKeyWorld;
        return ef;
    }

    Effect.SceneFadeInOutMaterialFloat GetFadeInstance(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return null;
        }

        Effect.SceneFadeInOutMaterialFloat ef = null;
        if (m_UnActiveOcclusFadeFadeInOutFloatList.Count > 0)
        {
            ef = m_UnActiveOcclusFadeFadeInOutFloatList.Dequeue();
        }
        else
        {
            ef = CreateNewFadeInstance();
        }

        res.occlusFadInstance = ef;
        ef.ClearFinishEvent();
        ef.RegisterFinishEvent(OcclusFadingFinish);
        ef.RegisterFinishEvent(res.OnFadeFinish);
        m_ActiveFadeSceneResTable.Add(ef, res);
        return ef;
    }

    void ApplyFade()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        foreach (SceneResInfo res in currentFadeList)
        {
            if (res.hostTreeNode == null)
            {
                continue;
            }

            if (res.OcclusVisible == 0)
            {
                FadeOutFromOcclusCulling(res);
            }
            else
            {
                FadeInFromOcclusCulling(res);
            }
        }
        currentFadeList.Clear();
    }

    void FadeOutFromOcclusCulling(SceneResInfo res, bool testLog = false)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (!res || res.hostTreeNode == null)
        {
            return;
        }
        QuadTreeNode parent = res.hostTreeNode;

        if (!parent.bOpmitizedRendering)
        {
            if (!res.CheckSupportFading())
            {
                return;
            }
            Material[] matList = GetFadingMaterial(res);
            if (null == matList)
            {
                return;
            }
            res.UpdateOcclusFadeOut(matList);
            if (!parent.isDistanceFadingIn)
            {
                if (!res.occlusFadingIn)
                {
                    if (!res.occlusFadingOut)
                    {
                        res.occlusFadingOut = true;
                        res.occlusFadingIn = false;

                        Effect.SceneFadeInOutMaterialFloat occlusFade = GetFadeInstance(res);
                        occlusFade.enabled = true;

                        occlusFade.SetMaterialInstance(matList);
                        occlusFade.FadeOutFrom = 0.0f;
                        occlusFade.FadeOutTo = 1.0f;
                        occlusFade.FadeOutDuartion = FadeTime;
                        occlusFade.Play();

                    }
                }
                else
                {
                    res.occlusFadingIn = false;
                    res.occlusFadingOut = true;

                    res.occlusFadInstance.SetMaterialInstance(matList);
                    res.occlusFadInstance.FadeOutDuartion = res.occlusFadInstance.FadeTimer;
                    res.occlusFadInstance.FadeOutFrom = res.occlusFadInstance.currentFloat;
                    res.occlusFadInstance.FadeOutTo = 1;
                    res.occlusFadInstance.FadeTimer = 0;
                    res.occlusFadInstance.Update();
                    if (res.occlusFadInstance)
                    {
                        res.occlusFadInstance.ReStartFadeOutFinish_Constant();
                    }

                }
            }
            else
            {
                res.occlusFadingIn = false;
                res.occlusFadingOut = true;
                parent.isDistanceFadingIn = false;

                Effect.SceneFadeInOutMaterialFloat distanceFade = Batches_TreeNodeFadeTable[parent];
                Effect.SceneFadeInOutMaterialFloat occlusFade = GetFadeInstance(res);
                occlusFade.enabled = true;


                occlusFade.SetMaterialInstance(matList);
                occlusFade.FadeOutDuartion = distanceFade.FadeTimer;
                occlusFade.FadeOutFrom = distanceFade.currentFloat;
                occlusFade.FadeOutTo = 1;
                occlusFade.FadeTimer = 0;
                occlusFade.Update();
                if (occlusFade)
                {
                    occlusFade.Play();
                }


                distanceFade.enabled = false;
                distanceFade.Stop();
            }
        }

    }

    void FadeInFromOcclusCulling(SceneResInfo res, bool testLog = false)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (!res || res.hostTreeNode == null)
        {
            return;
        }
        QuadTreeNode parent = res.hostTreeNode;

        if (!parent.bOpmitizedRendering)
        {
            if (!res.CheckSupportFading())
            {
                return;
            }

            Material[] matList = GetFadingMaterial(res);
            if (null == matList)
            {
                return;
            }
            res.UpdateOcclusFadeIn(matList);
            if (!parent.isDistanceFadingIn)
            {
                if (!res.occlusFadingOut)
                {
                    if (!res.occlusFadingIn)
                    {
                        res.occlusFadingIn = true;
                        res.occlusFadingOut = false;

                        Effect.SceneFadeInOutMaterialFloat occlusFade = GetFadeInstance(res);
                        occlusFade.SetMaterialInstance(matList);
                        occlusFade.FadeOutDuartion = FadeTime;
                        occlusFade.FadeOutFrom = 1;
                        occlusFade.FadeOutTo = 0;
                        occlusFade.enabled = true;
                        occlusFade.Play();

                    }
                }
                else
                {
                    res.occlusFadingIn = true;
                    res.occlusFadingOut = false;

                    Effect.SceneFadeInOutMaterialFloat occlusFade = res.occlusFadInstance;
                    occlusFade.SetMaterialInstance(matList);
                    occlusFade.FadeOutDuartion = occlusFade.FadeTimer;
                    occlusFade.FadeOutFrom = occlusFade.currentFloat;
                    occlusFade.FadeOutTo = 0;
                    occlusFade.FadeTimer = 0;



                    occlusFade.Update();

                    if (occlusFade)
                    {
                        occlusFade.ReStartFadeOutFinish_Constant();
                    }
                }
            }
            else
            {
                res.occlusFadingIn = true;
                res.occlusFadingOut = false;
                parent.isDistanceFadingIn = false;

                Effect.SceneFadeInOutMaterialFloat distanceFade = Batches_TreeNodeFadeTable[parent];
                Effect.SceneFadeInOutMaterialFloat occlusFade = GetFadeInstance(res);
                occlusFade.enabled = true;


                occlusFade.SetMaterialInstance(matList);
                occlusFade.FadeOutDuartion = distanceFade.FadeTimer;
                occlusFade.FadeOutFrom = distanceFade.currentFloat;
                occlusFade.FadeOutTo = 0;
                occlusFade.FadeTimer = 0;
                occlusFade.Update();

                if (occlusFade)
                {
                    occlusFade.Play();
                }


                distanceFade.enabled = false;
                distanceFade.Stop();

            }
        }
    }
    #endregion

    #region Cull
    void CullAll()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        foreach (SceneResInfo res in transform_ResTable.Values)
        {
            res.NotCullDataIndex = -1;
            res.occlusFadingIn = false;
            res.occlusFadingOut = false;
            res.occlusFadInstance = null;
            CollectFadingMaterial(res);
            res.ResetMaterial();
            res.Cull();
            res.EnableCullingCheck();
            res.ApplyCullVisible(true);
        }
    }

    void NotCullAll()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        foreach (SceneResInfo res in transform_ResTable.Values)
        {
            res.NotCullDataIndex = -1;
            res.occlusFadingIn = false;
            res.occlusFadingOut = false;
            res.occlusFadInstance = null;
            CollectFadingMaterial(res);
            res.ResetMaterial();
            res.NotCull();
            res.EnableCullingCheck();
            res.ApplyCullVisible(true);

        }
    }


    void DoNotCull(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        res.DisableCullingCheck();

        if (res.NotCullDataIndex > 0)
        {
            NotCullTimer[res.NotCullDataIndex] = 0;
            return;
        }



        if (NotCullEmptyIndexList.Count > 0)
        {
            int index = NotCullEmptyIndexList.Dequeue();
            res.NotCullDataIndex = index;
            NotCullList[index] = res;
            NotCullTimer[index] = 0;
        }
        else
        {
            res.NotCullDataIndex = NotCullList.Count;
            NotCullList.Add(res);
            NotCullTimer.Add(0);
        }


        res.NotCull();

    }

    void DoCull(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (res.NotCullDataIndex >= 0)
        {
            NotCullEmptyIndexList.Enqueue(res.NotCullDataIndex);
            res.NotCullDataIndex = -1;
        }
        res.Cull();
        res.EnableCullingCheck();
    }



    void UpdateVisibleNode(int index)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        SceneResInfo res = NotCullList[index];
        if (res)
        {
            if (res.NotCullDataIndex > 0)
            {
                NotCullTimer[res.NotCullDataIndex] += Time.deltaTime;
                float alvieTime = NotCullTimer[res.NotCullDataIndex];

                bool NeedToClear = false;
                if (alvieTime > DealyRestTime)
                {
                    DoCull(res);
                    NeedToClear = true;
                }
                res.ApplyCullVisible();
                if (NeedToClear)
                {
                    NotCullList[index] = null;
                }
            }
            res.EnableCullingCheck();
        }
    }

    void UpdateOcclusingCulling()
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        //if(Input.GetKeyUp(KeyCode.J))
        //{
        //    GoablEnabledOcclusCulling = false;
        //    ClearOcclusCacheData();
        //    NotCullAll();
        //}
        //if (Input.GetKeyUp(KeyCode.K))
        //{
        //    GoablEnabledOcclusCulling = true;
        //    ClearOcclusCacheData();
        //    CullAll();
        //}

        if (!OcclusCulingEnabled() || !SceneCamera.isActiveAndEnabled ||
            !RayCamera || transform_ResTable.Count <= 0)
        {
            return;
        }

        int clipCount = 0;
        int Remain = 0;
        int startIndex = 0;
        int EndIndx = 0;

        clipCount = RayCount / DvidFrameCount;
        Remain = RayCount % DvidFrameCount;
        startIndex = currUpdateIndex * clipCount;
        EndIndx = (currUpdateIndex + 1) * clipCount + (currUpdateIndex == (DvidFrameCount - 1) ? Remain : 0);
        for (int i = startIndex; i < EndIndx; i++)
        {
            Vector3 p = new Vector3(ScreenWidthPoints[i], ScreenHeightPoints[i], 0);
            Ray ray = RayCamera.ViewportPointToRay(p);

            Vector3 pEnd = ray.origin + ray.direction * currResInfoCellSize * 2 * 0.9f;

            for (int j = 0; j < MaxPreRayCheckCount; j++)
            {
                if (Physics.Linecast(ray.origin, pEnd, out hit, 1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)))
                {
                    SceneResInfo res;
                    transform_ResTable.TryGetValue(hit.transform, out res);
                    if (res)
                    {
                        if (!res.isOccluder)
                        {
                            DoNotCull(res);
                        }
                        else
                        {
                            break;
                        }
                    }

                }
            }

        }


        clipCount = NotCullList.Count / DvidFrameCount;
        Remain = NotCullList.Count % DvidFrameCount;
        startIndex = currUpdateIndex * clipCount;
        EndIndx = (currUpdateIndex + 1) * clipCount + (currUpdateIndex == (DvidFrameCount - 1) ? Remain : 0);
        for (int i = startIndex; i < EndIndx; i++)
        {
            UpdateVisibleNode(i);

        }

        ApplyFade();
        currUpdateIndex++;
        if (currUpdateIndex >= DvidFrameCount)
        {
            currUpdateIndex = 0;
        }
    }

    public void OnQuadTreeCull(SceneResInfo res)
    {
        if (!GoablEnabledOcclusCulling)
        {
            return;
        }

        if (res.NotCullDataIndex >= 0)
        {
            NotCullList[res.NotCullDataIndex] = null;
            NotCullEmptyIndexList.Enqueue(res.NotCullDataIndex);
            res.NotCullDataIndex = -1;
        }
        if (res.occlusFadInstance)
        {
            res.occlusFadInstance.enabled = false;
            res.occlusFadInstance.Stop();
            m_ActiveFadeSceneResTable.Remove(res.occlusFadInstance);
            m_UnActiveOcclusFadeFadeInOutFloatList.Enqueue(res.occlusFadInstance);
        }
        res.occlusFadingOut = false;
        res.occlusFadingIn = false;
        res.occlusFadInstance = null;
        CollectFadingMaterial(res);
    }
    #endregion


}
