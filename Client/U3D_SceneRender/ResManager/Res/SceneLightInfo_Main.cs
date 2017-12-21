/*******************************************************************
** 文件名:	SceneLightInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景灯光信息
** 应  用:   场景灯光信息，用于做场景资源管理，主要部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;
public partial class SceneLightInfo : SceneInfo
{
    public Light HostLight = null;
    public bool isBakedLight = false;
    public bool OptimitzedLight = true;
    public bool AlwaysOptimitzed = false;
    public bool lastOptimitzedLight = true;
    /// <summary>
    /// 灯光优化距离,与灯光范围有关，小于灯光范围取灯光范围，最大取90
    /// </summary>
    private float LightOptimitzedDistance = 90;

    public Transform mainCameraTrans = null;

    public override void OnStart(Camera sceneCamera)
    {
        mainCameraTrans = sceneCamera.transform;
        LightOptimitzedDistance = Mathf.Min(HostLight.range + 20, LightOptimitzedDistance);
        //InitFade();
        HostLight.enabled = true;
    }

    public override void OnClear(Camera sceneCamera)
    {
        //ClearFade();
    }
    public void ResetLight()
    {
        //OptimitzedLight = true;
        //HostLight.enabled = false;

    }
    //public void InfoUpdate()
    //{
    //    if(Contains())
    //    {
    //        UnOptimitzed();
    //    }
    //    else
    //    {
    //        Optimitzed();
    //    }
    //}

    //public bool Contains()
    //{
    //    if (!mainCameraTrans)
    //    {
    //        mainCameraTrans = Initialize.mainCam.transform;
    //    }

    //    //只判断XZ平面
    //    Vector3 startPos = mainCameraTrans.position;
    //    startPos.y = 0;

    //    Vector3 endPos = transform.position;
    //    endPos.y = 0;

    //    float dis = Vector3.SqrMagnitude(startPos - endPos);

    //    if (dis > LightOptimitzedDistance * LightOptimitzedDistance)
    //    {
    //        return false;
    //    }
    //    return true;
    //}

    public void UnOptimitzed()
    {
        //if (!OptimitzedLight || AlwaysOptimitzed)
        //{
        //    return;
        //}
        //OptimitzedLight = false;
        //FadeIn();
    }

    public void Optimitzed()
    {
        //if (OptimitzedLight)
        //{
        //    return;
        //}
        //OptimitzedLight = true;
        //FadeOut();
    }

}
