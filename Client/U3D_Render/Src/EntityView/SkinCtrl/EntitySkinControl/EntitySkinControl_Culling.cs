/*******************************************************************
** 文件名:	EntitySkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using UnityEngine.Events;

namespace U3D_Render.SkinCtrl
{
    /// <summary>
    /// 实体皮肤控制
    /// </summary>
    public partial class EntitySkinControl: MonoBehaviour
    {
        /// <summary>
        /// 是否被裁剪
        /// </summary>
        public bool isCull = true;

        [System.NonSerialized]
        public SkinManager.SkinQuadTreeNode hostQuadTreeNode = null;


        protected virtual void ClearCull()
        {
            ClearSkinCullEvent();
            ClearSkinNotCullEvent();
            ClearSkinChangedEvent();
            //hostQuadTreeNode = null;
            isCull = true;
        }

        public virtual void ResetCull()
        {
            isCull = true;
        }

        public virtual void OnCull()
        {

        }

        public virtual void OnNotCull()
        { 

        }

        /// <summary>
        /// 取得显示优先级
        /// </summary>
        /// <returns></returns>
        public int GetSkinShowProity()
        {
            return 1;
        }
    }
}
