using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI
{
    public partial class USpeedUILayerElement
    {

        protected Renderer[] m_EffectRenderer;
        protected Renderer[] EffectRenderer
        {
            get
            {
                CacheEffectRenderer();
                return m_EffectRenderer;
            }
        }
        protected bool m_bFindEffectRenderers;
        private void CacheEffectRenderer()
        {
            if (m_EffectRenderer == null || !m_bFindEffectRenderers)
            {
                m_EffectRenderer = this.GetComponentsInChildren<Renderer>();
                m_bFindEffectRenderers = true;
            }
        }
        protected void SetUIElementType_Effect_Enter(EUILayerElementType _eNewType)
        {
            CacheEffectRenderer();
        }
        protected void SetUIElementType_Effect_Leave(EUILayerElementType _eOldType)
        {
            if (m_EffectRenderer != null || m_bFindEffectRenderers)
            {
                m_EffectRenderer = null;
                m_bFindEffectRenderers = false;
            }
        }

        protected void SetSortingLayerName_Effect(string _strNewSortingLayerName)
        {
            foreach (Renderer renderer in EffectRenderer)
                renderer.sortingLayerName = m_SortingLayerName;
        }
        protected void SetSortingOrder_Effect(int _nNewOrder)
        {
            foreach (Renderer renderer in EffectRenderer)
            {
                renderer.sortingOrder = USpeedUILayerManager.CheckSortingOrderRange(_nNewOrder);
            }
        }
        protected void SetOnEnable_Effect()
        {
            CacheEffectRenderer();
        }
        protected void SetDisable_Effect()
        { }
    }
}
