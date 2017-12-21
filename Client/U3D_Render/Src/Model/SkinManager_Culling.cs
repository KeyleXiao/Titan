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
    /// 只有主城需要这个skinCull，战场不需要
    /// </summary>
    public static bool enabledSkinCull = false;

    private static HashSet<U3D_Render.SkinCtrl.EntitySkinControl> allSkinControl = new HashSet<U3D_Render.SkinCtrl.EntitySkinControl>();

    private static bool bInitCulling = false;

    private const int CellSize = 30;

    private static SkinQuadTreeNode quadTreeRoot = null;

    private const float cullUpdateTime = 0.1f;
    private static float currCullUpdateTime = 0.0f;

    private static int MaxSkinShowCount = 20;
    private static int MinSkinShowCount = 9;
    private static int currSkinShowCount = 1;

    /// <summary>
    /// 活动的资源结点
    /// </summary>
    private static List<SkinQuadTreeNode> ActiveParent = new List<SkinQuadTreeNode>();
    private static List<U3D_Render.SkinCtrl.EntitySkinControl> ActiveLeafList = new List<U3D_Render.SkinCtrl.EntitySkinControl>(100);

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private static List<U3D_Render.SkinCtrl.EntitySkinControl> unUsedLeafList = new List<U3D_Render.SkinCtrl.EntitySkinControl>(100);
    private static List<SkinQuadTreeNode> unUsedParent = new List<SkinQuadTreeNode>();

    private static void InitCulling()
    {
        bInitCulling = true;
        unUsedParent.Clear();
        ActiveParent.Clear();
        ActiveLeafList.Clear();
        unUsedLeafList.Clear();
        quadTreeRoot = new SkinQuadTreeNode();
        quadTreeRoot.Bounds = new Rect(-SceneResInfoManager.SceneSize * 0.5f, -SceneResInfoManager.SceneSize * 0.5f, SceneResInfoManager.SceneSize, SceneResInfoManager.SceneSize);
        BuildQuadTree(quadTreeRoot, (float)CellSize, (float)CellSize);
        currCullUpdateTime = 0;

        SceneManager.RegisterSceneLoadFinishEvent(OnSceneLoadFinish);
        SceneManager.RegisterSceneLoadStartEvent(OnSceneLoadStart);
        ImageSetting.OnSceneRenderingQualityChanged += OnSceneRenderingQualityChaned;
    }

    
    private static void OnSceneRenderingQualityChaned(SceneRenderingQuality newQuality,SceneRenderingQuality oldQuality)
    {


        currSkinShowCount = MinSkinShowCount;
        if (newQuality == SceneRenderingQuality.Fastest)
        {
            currSkinShowCount = MinSkinShowCount;
        }
        else if (newQuality == SceneRenderingQuality.Good)
        {
            currSkinShowCount = MinSkinShowCount;
        }
        else if (newQuality == SceneRenderingQuality.High)
        {
            currSkinShowCount = 14;
        }
        else if (newQuality == SceneRenderingQuality.Best)
        {
            currSkinShowCount = MaxSkinShowCount;
        }
        //算上自己
        currSkinShowCount++;

        if(newQuality != oldQuality)
        {
            ResetCulling();
        }
    }
    public static void TraceCullingList()
    {
        Debug.Log("SkinCullingStart:");
        if(!enabledSkinCull)
        {
            Debug.Log("enabledSkinCull:false");
            foreach (U3D_Render.SkinCtrl.EntitySkinControl sk in allSkinControl)
            {
                Debug.Log("pos:" + sk.transform.position + ",name:" + sk.gameObject.name + ",skinID:" + sk.currentSkinId + ",iscull:" + sk.isCull + ",hostTreeNode:" + sk.hostQuadTreeNode + ",isActiveEnabled:" + sk.isActiveAndEnabled);
            }
        }
        else
        {
            Debug.Log("ActiveLeafList:");
            foreach (U3D_Render.SkinCtrl.EntitySkinControl sk in ActiveLeafList)
            {
                Debug.Log("pos:" + sk.transform.position + ",name:" + sk.gameObject.name + ",skinID:" + sk.currentSkinId + ",iscull:" + sk.isCull + ",hostTreeNode:" + sk.hostQuadTreeNode + ",isActiveEnabled:" + sk.isActiveAndEnabled);
            }
            Debug.Log("allSkinControl:");
            foreach(U3D_Render.SkinCtrl.EntitySkinControl sk in allSkinControl)
            {
                Debug.Log("pos:" + sk.transform.position + ",name:" + sk.gameObject.name + ",skinID:" + sk.currentSkinId + ",iscull:" + sk.isCull + ",hostTreeNode:" + sk.hostQuadTreeNode + ",isActiveEnabled:" + sk.isActiveAndEnabled);
            }
        }
        Debug.Log("SkinCullingEnd");
    }

    private static void OnSceneLoadStart(string sceneName)
    {
        enabledSkinCull = GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId());
    }


    private static void OnSceneLoadFinish(string sceneName)
    {
        //enabledSkinCull = GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId());
        ResetCulling();
    }

    private static void ResetCulling()
    {
        if (!enabledSkinCull)
        {
            return;
        }
        unUsedParent.Clear();
        ActiveParent.Clear();
        ActiveLeafList.Clear();
        unUsedLeafList.Clear();
        foreach (U3D_Render.SkinCtrl.EntitySkinControl instance in allSkinControl)
        {
            instance.ResetCull();
        }
    }

    private static void ClearCulling()
    {
        SceneManager.UnRegisterSceneLoadFinishEvent(OnSceneLoadFinish);

        ImageSetting.OnSceneRenderingQualityChanged -= OnSceneRenderingQualityChaned;
        bInitCulling = false;
        ClearQuadTree(quadTreeRoot);
        quadTreeRoot = null;
        allSkinControl.Clear();
    }

    private static void UpdateCulling()
    {
        //DebugDraw();
        currCullUpdateTime += Time.deltaTime;
        if(currCullUpdateTime < cullUpdateTime)
        {
            return;
        }
        currCullUpdateTime = 0;
        //只对主城有效
        if (!enabledSkinCull)
        {
            return;
        }
        RefershQuadTreeNode();
    }
    
    private static int CompareFunc(U3D_Render.SkinCtrl.EntitySkinControl left,U3D_Render.SkinCtrl.EntitySkinControl right)
    {
        if(!left && !right)
        {
            return 0;
        }

        if(!left.isActiveAndEnabled && !right.isActiveAndEnabled)
        {
            return 0;
        }

        if(!left)
        {
            return 1;
        }

        if(!left.isActiveAndEnabled)
        {
            return 1;
        }

        if(!right)
        {
            return -1;
        }

        if(!right.isActiveAndEnabled)
        {
            return -1;
        }

        int leftproity = left.GetSkinShowProity();
        int rightproity = right.GetSkinShowProity();
        //显示的优先级在距离前面

        if(leftproity < rightproity)
        {
            return 1;
        }

        if (leftproity > rightproity)
        {
            return -1;
        }

        //然后看距离
        U3D_Render.EntityView heroView = EntityFactory.getMainPlayer();
        if (null == heroView)
        {
            return 0;
        }
        if (!heroView.StateMachine)
        {
            return 0;
        }

        if (heroView.SkinControl == right)
        {
            return 1;
        }
        if (heroView.SkinControl == left)
        {
            return -1;
        }
        Vector3 pos = heroView.StateMachine.transform.position;
        float leftdistance = GameUtil.GetSqrDistanceWithOutY(pos, left.transform.position);
        float rightdistance = GameUtil.GetSqrDistanceWithOutY(pos, right.transform.position);

        return leftdistance.CompareTo(rightdistance);
    }

    private static void GetActiveNode(ref List<U3D_Render.SkinCtrl.EntitySkinControl> UsedReslut)
    {
        UsedReslut.Clear();
        foreach (U3D_Render.SkinCtrl.EntitySkinControl sk in allSkinControl)
        {
            if (sk.isActiveAndEnabled)
            {
                UsedReslut.Add(sk);
            }
        }
    }

    private static int GetActiveNodeCount( List<U3D_Render.SkinCtrl.EntitySkinControl> UsedReslut)
    {
        int cout = 0;
        U3D_Render.EntityView heroView = EntityFactory.getMainPlayer();
        if (null == heroView)
        {
            return cout;
        }
        if (!heroView.StateMachine)
        {
            return cout;
        }
        foreach (U3D_Render.SkinCtrl.EntitySkinControl sk in UsedReslut)
        {
            Vector3 pos = heroView.StateMachine.transform.position;
            float leftdistance = GameUtil.GetSqrDistanceWithOutY(pos, sk.transform.position);
            if(leftdistance <= CellSize * CellSize)
            {
                cout++;
            }
        }
        return cout;
    }

    private static void RefershQuadTreeNode()
    {

        U3D_Render.EntityView heroView = EntityFactory.getMainPlayer();
        if(null == heroView)
        {
            return;
        }
        if(!heroView.StateMachine)
        {
            return;
        }

        Vector3 pos = heroView.StateMachine.transform.position;
        Vector2 pos2D = Vector2.zero;
        Vector2 Dir2D = Vector2.zero;

        pos2D.x = pos.x;
        pos2D.y = pos.z;

        GenerateActiveQuadParent(quadTreeRoot, 0.0f, pos2D, Vector2.zero, CellSize,
                                 ref ActiveLeafList, ref ActiveParent, ref unUsedLeafList, ref unUsedParent);
        ActiveLeafList.Sort(CompareFunc);
        //GetActiveNode(ref ActiveLeafList);

        int currShowCount = GetActiveNodeCount(ActiveLeafList);
        currShowCount = Mathf.Min(currSkinShowCount, currShowCount);
        for (int i = 0; i < currShowCount; i++)
        {
            U3D_Render.SkinCtrl.EntitySkinControl instance = ActiveLeafList[i];
            if (!instance || !instance.isActiveAndEnabled)
            {
                continue;
            }

            instance.OnNotCull();
        }

        for (int i = currShowCount; i < ActiveLeafList.Count; i++)
        {
            U3D_Render.SkinCtrl.EntitySkinControl instance = ActiveLeafList[i];
            if (!instance || !instance.isActiveAndEnabled)
            {
                continue;
            }

            instance.OnCull();
        }

        foreach (U3D_Render.SkinCtrl.EntitySkinControl instance in unUsedLeafList)
        {
            if (!instance || !instance.isActiveAndEnabled)
            {
                continue;
            }

            instance.OnCull();
        }

    }

    public static void UpdateSkinControl(U3D_Render.SkinCtrl.EntitySkinControl instance)
    {

        if (null != instance.hostQuadTreeNode)
        {
            if(instance.hostQuadTreeNode.IsPointOnParent(instance.transform.position))
            {
                return;
            }
            instance.hostQuadTreeNode.RemoveLeaf(instance);
            instance.hostQuadTreeNode = null;
        }

        //人物会有传送，插入到父节点可能不对，直接从根节点开始
        // SkinQuadTreeNode parent = instance.hostQuadTreeNode.m_Parent == null ? quadTreeRoot : instance.hostQuadTreeNode.m_Parent;
        //InsetToTree(parent, instance);
        InsetToTree(quadTreeRoot, instance);
    }

    public static void PushSkinControlToManager(U3D_Render.SkinCtrl.EntitySkinControl instance)
    {
        if(!bInitCulling || !instance)
        {
            return;
        }
        allSkinControl.Add(instance);
        InsetToTree(quadTreeRoot,instance);
    }

    public static void RemoveSkinControlToManager(U3D_Render.SkinCtrl.EntitySkinControl instance)
    {
        if (!bInitCulling || !instance)
        {
            return;
        }
        allSkinControl.Remove(instance);
        instance.hostQuadTreeNode = null;
    }

    public static void DebugDraw()
    {
        DrawBounds(quadTreeRoot, ActiveParent);
    }

    public static void DrawRect(Rect r, float y, Color c, float padding = 0.0f)
    {
        Debug.DrawLine(new Vector3(r.xMin + padding, y, r.yMin + padding), new Vector3(r.xMin + padding, y, r.yMax - padding), c);
        Debug.DrawLine(new Vector3(r.xMin + padding, y, r.yMin + padding), new Vector3(r.xMax - padding, y, r.yMin + padding), c);
        Debug.DrawLine(new Vector3(r.xMax - padding, y, r.yMax - padding), new Vector3(r.xMin + padding, y, r.yMax - padding), c);
        Debug.DrawLine(new Vector3(r.xMax - padding, y, r.yMax - padding), new Vector3(r.xMax - padding, y, r.yMin + padding), c);
    }


    static void DrawBounds(SkinQuadTreeNode p, List<SkinQuadTreeNode> ActiveList)
    {

        DrawAll(p);
        DrawActive(ActiveList);

    }

    static void DrawAll(SkinQuadTreeNode p)
    {
        if (null == p)
        {
            return;
        }
        DrawRect(p.Bounds, 5.0f, Color.white);
        if (p.ChildCount > 0)
        {
            //继续遍历剩下的结点
            foreach (SkinQuadTreeNode p1 in p.ChildNode)
            {
                DrawAll(p1);
            }
        }
    }

    static void DrawActive(List<SkinQuadTreeNode> ActiveList)
    {
        foreach (SkinQuadTreeNode p in ActiveList)
        {
            DrawRect(p.Bounds, 8.0f, Color.red);
        }

    }
}


