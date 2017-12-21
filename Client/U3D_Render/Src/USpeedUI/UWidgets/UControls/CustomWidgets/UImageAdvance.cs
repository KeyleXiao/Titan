/*******************************************************************
** 文件名:	UImageAdvance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/06/14
** 版  本:	1.0
** 描  述:	该类方便用于不规则Image的检测
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine.UI;
using UnityEngine;

public class UImageAdvance : Image
{
    Collider2D[] ImageColliders;
    protected override void Awake()
    {
        base.Awake();
        ImageColliders = GetComponents<Collider2D>();
        
        if (ImageColliders != null && ImageColliders.Length > 0)
            raycastTarget = true;
        
    }
    override public bool IsRaycastLocationValid(Vector2 screenPoint, Camera eventCamera)
    {
        bool isRay = base.IsRaycastLocationValid(screenPoint, eventCamera);
        bool colliderResult = false;
        Vector3 local;

        if(isRay && ImageColliders != null && ImageColliders.Length > 0)
        {
            foreach (var collider in ImageColliders)
            {
                if (collider != null)
                {
                    RectTransformUtility.ScreenPointToWorldPointInRectangle(rectTransform, screenPoint, eventCamera, out local);
                    colliderResult = collider.OverlapPoint(local);     

                    if(colliderResult)    
                        return colliderResult;
                }
            }
            return colliderResult;
        }

        return isRay;
    }



}
