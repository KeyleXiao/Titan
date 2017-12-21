/*******************************************************************
** 文件名:	RenderVisibleHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;


public class RenderVisibleHelper : MonoBehaviour
{

    [System.NonSerialized]
    public U3D_Render.SkinCtrl.EntitySkinControl hostSkin;

    //public void OnBecameInvisible()
    //{
    //    if(!hostSkin)
    //    {
    //        return;
    //    }
    //    if (hostSkin.OnRenderVisibleChanged != null)
    //    {
    //        hostSkin.OnRenderVisibleChanged(false);
    //    }
    //}
    //public void OnBecameVisible()
    //{
    //    if(!hostSkin)
    //    {
    //        return;
    //    }

    //    if (hostSkin.OnRenderVisibleChanged != null)
    //    {
    //        hostSkin.OnRenderVisibleChanged(true);
    //    }
    //}
}
