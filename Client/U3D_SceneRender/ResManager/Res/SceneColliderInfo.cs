/*******************************************************************
** 文件名:	SceneColliderInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景碰撞信息
** 应  用:   场景碰撞信息，用于碰撞检测优化

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;

public partial class SceneColliderInfo : SceneInfo
{
    public Collider hostCollider = null;

    public Vector3 RefPos = Vector3.zero;

    public Vector3 ColliderBounds_Min = Vector3.zero;
    public Vector3 ColliderBounds_Max = Vector3.zero;

    public bool isAlwaysEnabled = false;

    public bool bOptimitized = true;

    private Vector3 OptimitzedColliderBounds_Min = Vector3.zero;
    private Vector3 OptimitzedColliderBounds_Max = Vector3.zero;
    /// <summary>
    /// 碰撞优化距离
    /// </summary>
    private float ColliderOptimitzedDistance = 16;

    private Transform mainCameraTrans = null;

    public override void OnStart(Camera sceneCamera)
    {
        mainCameraTrans = sceneCamera.transform;
        if(isAlwaysEnabled)
        {
            hostCollider.enabled = true;
        }
        ReCalculateBounds();
    }

    void UnOptimitized()
    {
        if(!bOptimitized)
        {
            return;
        }
        bOptimitized = false;
        hostCollider.enabled = true;
    }

    public void ResetCollider()
    {
        bOptimitized = true;
        hostCollider.enabled = false;
    }

    public void Optimitzed()
    {
        if(isAlwaysEnabled || bOptimitized)
        {
            return;
        }
        bOptimitized = true;
        hostCollider.enabled = false;
    }

    public override void OnUpdate(Camera sceneCamera)
    {
        if(isAlwaysEnabled)
        {
            return;
        }

        if(Contains_Collider())
        {
            UnOptimitized();
        }
        else
        {
            Optimitzed();
        }
    }

    void ReCalculateBounds()
    {

        //if (hostRender.gameObject.name.Equals("moba_001_tyst28"))
        //{
        //    Debug.LogWarning(RenderBoundsMin + "," + RenderBoundsMax);
        //}
        ////算出对角线
        //Vector3 a = b.max - b.min;

        ////用Y做为标尺
        //float vol =  a.y;

        //a = Vector3.one * 10;
        //float standradlength =  a.y;

        //float scale = vol / standradlength;

        ////缩放的最大，最小值
        //scale = Mathf.Clamp(scale, 0.2f, 2.0f);

        //OptimitzedDistance *= scale;

        OptimitzedColliderBounds_Min = ColliderBounds_Min;
        OptimitzedColliderBounds_Min.x -= ColliderOptimitzedDistance;
        OptimitzedColliderBounds_Min.y -= ColliderOptimitzedDistance;
        OptimitzedColliderBounds_Min.z -= ColliderOptimitzedDistance;


        OptimitzedColliderBounds_Max = ColliderBounds_Max;
        OptimitzedColliderBounds_Max.x += ColliderOptimitzedDistance;
        OptimitzedColliderBounds_Max.y += ColliderOptimitzedDistance;
        OptimitzedColliderBounds_Max.z += ColliderOptimitzedDistance;
        RefPos = hostCollider.bounds.center;
    }

    bool Contains_Collider()
    {
        Vector3 pos = mainCameraTrans.position;

        if (pos.x > OptimitzedColliderBounds_Min.x && pos.y > OptimitzedColliderBounds_Min.y && pos.z > OptimitzedColliderBounds_Min.z &&
            pos.x < OptimitzedColliderBounds_Max.x && pos.y < OptimitzedColliderBounds_Max.y && pos.z < OptimitzedColliderBounds_Max.z)
        {
            return true;
        }
        return false;
    }

}
