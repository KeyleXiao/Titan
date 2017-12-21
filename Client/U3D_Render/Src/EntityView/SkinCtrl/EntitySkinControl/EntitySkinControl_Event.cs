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
    public class SkinChangedEvent:UnityEvent<SkinInstance,int>
    { 
    }

    /// <summary>
    /// 实体皮肤控制
    /// </summary>
    public partial class EntitySkinControl: MonoBehaviour
    {

        protected UnityEvent onSkinCull = new UnityEvent();
        protected UnityEvent onSkinNotCull = new UnityEvent();
        protected SkinChangedEvent onSkinChanged = new SkinChangedEvent();

        public void RegsiterSkinCullEvent(UnityAction action)
        {
            if(null == action)
            {
                return;
            }
            if(null == onSkinCull)
            {
                onSkinCull = new UnityEvent();
            }

            onSkinCull.AddListener(action);
        }
        public void UnRegsiterSkinCullEvent(UnityAction action)
        {
            if (null == action)
            {
                return;
            }
            if (null == onSkinCull)
            {
                return;
            }

            onSkinCull.RemoveListener(action);
        }
        protected void InvokeSkinCullEvent()
        {
            if (null == onSkinCull)
            {
                return;
            }
            onSkinCull.Invoke();
        }
        protected void ClearSkinCullEvent()
        {
            if (null != onSkinCull)
            {
                onSkinCull.RemoveAllListeners();
                onSkinCull = null;
            }
        }


        public void RegsiterSkinNotCullEvent(UnityAction action)
        {
            if (null == action)
            {
                return;
            }
            if (null == onSkinNotCull)
            {
                onSkinNotCull = new UnityEvent();
            }

            onSkinNotCull.AddListener(action);
        }
        public void UnRegsiterSkinNotCullEvent(UnityAction action)
        {
            if (null == action)
            {
                return;
            }
            if (null == onSkinNotCull)
            {
                return;
            }

            onSkinNotCull.RemoveListener(action);
        }
        protected void InvokeSkinNotCullEvent()
        {
            if (null == onSkinNotCull)
            {
                return;
            }
            onSkinNotCull.Invoke();
        }
        protected void ClearSkinNotCullEvent()
        {
            if (null != onSkinNotCull)
            {
                onSkinNotCull.RemoveAllListeners();
                onSkinNotCull = null;
            }
        }


        public void RegsiterSkinChangedEvent(UnityAction<SkinInstance, int> action)
        {
            if (null == action)
            {
                return;
            }
            if (null == onSkinChanged)
            {
                onSkinChanged = new SkinChangedEvent();
            }

            onSkinChanged.AddListener(action);
        }
        public void UnRegsiterSkinChangedEvent(UnityAction<SkinInstance,int> action)
        {
            if (null == action)
            {
                return;
            }
            if (null == onSkinChanged)
            {
                return;
            }

            onSkinChanged.RemoveListener(action);
        }
        protected void InvokeSkinChangedEvent(SkinInstance ins,int nPart)
        {
            if (null == onSkinChanged)
            {
                return;
            }
            onSkinChanged.Invoke(ins, nPart);
        }
        protected void ClearSkinChangedEvent()
        {
            if (null != onSkinChanged)
            {
                onSkinChanged.RemoveAllListeners();
                onSkinChanged = null;
            }
        }

    }
}
