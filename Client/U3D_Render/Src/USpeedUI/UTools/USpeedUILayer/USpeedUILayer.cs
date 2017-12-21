/*******************************************************************
** 文件名:	USpeedUILayer.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-7-29
** 版  本:	1.0
** 描  述:	UI层级组件
		    目前主要用于UISystem和其下面的BottomLayer、MiddleLayer和TopLayer，向UILayerManager注册组件
            各个Layer分Canvas和GraphicRaycaster有注意射线检测优化和事件优化
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI
{
    [RequireComponent(typeof(GraphicRaycaster))]
    public class USpeedUILayer : UCustomUIBehaviour
    {
        protected WndLayerDef m_UILayerDef = WndLayerDef.WND_LAYER_NONE;
        public WndLayerDef UILayerDef
        {
            get { return m_UILayerDef; }
            set
            {
                if (m_UILayerDef != value)
                {
                    UnRegisterUILayer();
                    m_UILayerDef = value;
                    RegisterUILayer();
                }
            }
        }

        protected Canvas m_layerCanvas;
        public Canvas LayerCanvas
        {
            get
            {
                if (m_layerCanvas == null)
                    m_layerCanvas = GetComponent<Canvas>();
                return m_layerCanvas;
            }
        }

        protected GraphicRaycaster m_GraphicRaycaster;
        public GraphicRaycaster LayerGraphicRaycaster
        {
            get
            {
                if (m_GraphicRaycaster == null)
                {
                    m_GraphicRaycaster = GetComponent<GraphicRaycaster>();
                }
                return m_GraphicRaycaster;
            }
        }

        private FieldInfo m_RaycasterLayerMaskFieldInfo = null;
        public LayerMask RaycasterLayerMask
        {
            get
            {
                if (m_RaycasterLayerMaskFieldInfo == null)
                {
                    Type type = LayerGraphicRaycaster.GetType();

                    m_RaycasterLayerMaskFieldInfo = type.GetField("m_BlockingMask", BindingFlags.NonPublic | BindingFlags.Instance);
                }

                if (m_RaycasterLayerMaskFieldInfo == null)
                    return -1;

                object o = m_RaycasterLayerMaskFieldInfo.GetValue(m_GraphicRaycaster);

                if (o == null)
                    return -1;

                return (LayerMask)o;

            }
            set
            {
                if (m_RaycasterLayerMaskFieldInfo == null)
                {
                    Type type = LayerGraphicRaycaster.GetType();

                    m_RaycasterLayerMaskFieldInfo = type.GetField("m_BlockingMask", BindingFlags.NonPublic | BindingFlags.Instance);
                }

                if (m_RaycasterLayerMaskFieldInfo == null)
                    return;

                m_RaycasterLayerMaskFieldInfo.SetValue(m_GraphicRaycaster, value, BindingFlags.Instance | BindingFlags.NonPublic, null, null);
            }
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            RegisterUILayer();
        }

        protected override void OnDisable()
        {
            base.OnDisable();
            UnRegisterUILayer();
        }

        public void RegisterUILayer()
        {
            USpeedUILayerManager.RegisterUILayer(UILayerDef, this);
        }
        public void UnRegisterUILayer()
        {
            USpeedUILayerManager.UnRegisterUILayer(UILayerDef, this);
        }


    }
}
