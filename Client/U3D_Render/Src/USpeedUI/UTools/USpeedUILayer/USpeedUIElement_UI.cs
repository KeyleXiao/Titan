using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using USpeedUI.UWidgets;
using USpeedUI.UExtensions;

namespace USpeedUI
{
    public partial class USpeedUILayerElement
    {
        protected Canvas m_UICanvasRenderer;
        protected Canvas UICanvasRenderer
        {
            get
            {
                if(m_UICanvasRenderer == null)
                    CacheUICanvasRenderer();
                return m_UICanvasRenderer;
            }
        }
        [SerializeField]
        protected bool m_bIsUIRequirRaycaster = false;
        public bool IsUIRequirRaycaster
        {
            get { return m_bIsUIRequirRaycaster; }
            set
            {
                GraphicRaycaster raycaster = this.gameObject.GetComponent<GraphicRaycaster>();
                m_bIsUIRequirRaycaster = value;
                if (m_bIsUIRequirRaycaster && raycaster == null)
                {
                    gameObject.AddComponent<GraphicRaycaster>();
                }
                else if (!m_bIsUIRequirRaycaster && raycaster != null)
                {
                    gameObject.RemoveComponent<GraphicRaycaster>();
                }

            }
        }


        private void CacheUICanvasRenderer()
        {
            bool IsActiveObject = gameObject.activeSelf;
            if (!IsActiveObject)
            {
                gameObject.SetActive(true);
            }

            if (m_UICanvasRenderer == null)
            {
                m_UICanvasRenderer = GetComponent<Canvas>();
                if (m_UICanvasRenderer == null)
                    m_UICanvasRenderer = gameObject.AddComponent<Canvas>();
            }

            m_UICanvasRenderer.overrideSorting = true;
            m_UICanvasRenderer.enabled = false;
            m_UICanvasRenderer.enabled = true;

            if (!IsActiveObject)
            {
                gameObject.SetActive(false);
            }
        }
        protected void SetUIElementType_UI_Enter(EUILayerElementType _eNewType)
        {
            CacheUICanvasRenderer();
        }
        protected void SetUIElementType_UI_Leave(EUILayerElementType _eOldType)
        {
            bool IsActiveObject = gameObject.activeSelf;
            if (!IsActiveObject)
            {
                gameObject.SetActive(true);
            }
            GraphicRaycaster raycaster = this.gameObject.GetComponent<GraphicRaycaster>();
            if (raycaster != null)
            {
                gameObject.RemoveComponent<GraphicRaycaster>();
                m_bIsUIRequirRaycaster = false;
            }
            if (m_UICanvasRenderer != null)
            {
                gameObject.RemoveComponent<Canvas>();
                m_UICanvasRenderer = null;
            }

            if (!IsActiveObject)
            {
                gameObject.SetActive(false);
            }
        }
        protected void SetSortingLayerName_UI(string _strNewSortingLayerName)
        {
            UICanvasRenderer.overrideSorting = true;
            UICanvasRenderer.sortingLayerName = m_SortingLayerName;
        }
        protected void SetSortingOrder_UI(int _nNewOrder)
        {
            UICanvasRenderer.overrideSorting = true;
            UICanvasRenderer.sortingOrder = USpeedUILayerManager.CheckSortingOrderRange(_nNewOrder);
        }
        protected void SetOnEnable_UI()
        {
            CacheUICanvasRenderer();
        }
        protected void SetDisable_UI()
        { }
    }
}
