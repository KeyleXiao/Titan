/*******************************************************************
** 文件名:	UI_ImageEffectNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/10
** 版  本:	1.0
** 描  述:	全屏特效脚本
** 应  用:  	全屏特效脚本,UI全品，这个渲染是在ImageEffectNode后面

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using UnityEngine;

namespace ImageEffects
{
    public class UI_ImageEffectNode : MonoBehaviour
	{

        public virtual bool ImageEffect_UI_RenderImage(RenderTexture src, RenderTexture dst)
        {
            Debug.LogWarning(ToString() + "必须实现ImageEffect_UI_RenderImage函数！");
            return false;
        }

    }
}
