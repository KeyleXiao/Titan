/*******************************************************************
** 文件名:	USpeedUILayerElement.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-7-29
** 版  本:	1.0
** 描  述:	UI层级元素
		    主要用于不能通过调整Z轴而达到前后遮挡关系的UILayer下的元素，可用于Graphic、特效

********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;

namespace USpeedUI
{
    [Serializable]
    public partial class USpeedUILayerElement : UCustomUIBehaviour
    {
        [SerializeField]
        public enum EUILayerElementType
        {
            ELET_None,
            ELET_UI,
            ELET_Effect,
            ELET_Max,
        }

        [SerializeField]
        protected EUILayerElementType m_UILayerElementType;
        public EUILayerElementType UILayerElementType
        {
            get { return m_UILayerElementType; }
            set
            {
                if(m_UILayerElementType != value)
                {
                    SetUIElementType_Leave(m_UILayerElementType);
                    m_UILayerElementType = value;
                    SetUIElementType_Enter(m_UILayerElementType);
                }
            }
        }


        /// <summary>
        /// 当前Element所在的UILayer
        /// </summary>
        [NonSerialized]
        protected USpeedUILayer m_UILayer;
        public USpeedUILayer UILayer
        {
            get
            {
                if (m_UILayer == null)
                    CacheUILayer();
                return m_UILayer;
            }
        }
        /// <summary>
        /// 当前Element所在的View
        /// </summary>
        protected IUIWndView m_UIWndView;
        public IUIWndView UIWndView
        {
            get
            {
                if(m_UIWndView == null)
                    CacheUILayer();
                return m_UIWndView;
            }
        }

        /// <summary>
        /// 当前Element所用到的SortingLayer
        /// </summary>
        [SerializeField]
        protected string m_SortingLayerName = Config.SortingLayerDefault;
        public string SortingLayerName
        {
            get { return m_SortingLayerName; }
            set
            {
                m_SortingLayerName = value;
                SetSortingLayerName(m_SortingLayerName);
            }
        }

        /// <summary>
        /// 当前Element SortingLayer的序号
        /// </summary>
        [SerializeField]
        protected int m_SortingLayerOrder = 0;
        public int SortingLayerOrder
        {
            get { return m_SortingLayerOrder; }
            set
            {
                m_SortingLayerOrder = value;
                SetSortingOrder();
            }
        }

        /// <summary>
        /// 当前Element需要相对父节点Element的序号
        /// </summary>
        [SerializeField]
        protected int m_SortingLayerOrderOffset = 0;
        public int SortingLayerOrderOffset
        {
            get { return m_SortingLayerOrderOffset; }
            set
            {
                m_SortingLayerOrderOffset = value;
                if (CanSortingOrder())
                {

                    SetSortingOrder();
                }
            }
        }

        /// <summary>
        /// 当前Element父节点的序号
        /// </summary>
        protected int m_sortingLayerOrderParentOffset = 0;
        public int SortingLayrOrderParentOffset
        {
            get
            {
                int m_sortingLayerOrderParentOffset = 0;
                //WillToDo: 这里有空可以优化下，不一定需要查找该GameObject的所有父节点
                var t = gameObject.transform.parent;
                while (t != null)
                {
                    var parentLayerElement = t.GetComponent<USpeedUILayerElement>();
                    if (parentLayerElement != null && parentLayerElement.isActiveAndEnabled)
                    {
                        m_sortingLayerOrderParentOffset += parentLayerElement.SortingLayerOrderOffset;
                        //element 父节点是element不是view，则取父节点的Index
                        if(t.GetComponent<IUIWndView>() == null)
                            m_nParentLayerElementSiblingIndex = t.GetSiblingIndex();
                    }
                    //element 父节点直到view才是element，则取view节点下与当前element相关的节点
                    if (t.parent != null && t.parent.GetComponent<IUIWndView>() != null)
                        m_nParentLayerElementSiblingIndex = t.GetSiblingIndex();
                    t = t.parent;
                }

                return m_sortingLayerOrderParentOffset;
            }
            
        }

        [SerializeField]
        protected bool m_bIsUseSortingLayerOffset = true;
        public bool IsUseSortingLayerOffset
        {
            get { return m_bIsUseSortingLayerOffset; }
            set { m_bIsUseSortingLayerOffset = value; }
        }

        [SerializeField]
        protected int m_nSiblingIndex = 0;
        protected int? m_nParentLayerElementSiblingIndex = null;
        /// <summary>
        /// 获取当前节点带USpeedUILayerElement在父节点下的对象层级顺序
        /// 一个View的层级关系
        /// --view01
        ///   --view01_child01
        ///     --view01_child01_01
        ///     --view01_child01_02
        ///         --view01_child01_02_01
        ///   --view01_child02
        ///   --view01_child03
        ///     --view01_child03_01
        ///     
        /// e.g.1： 
        ///     view01_child03_01 有 UILayerElement，
        ///     view01_child03_01 的父节点 view01_child03 !UILayerElement && !IUIWndView，则取 view01_child03.GetSiblingIndex()
        /// e.g.2:
        ///     view01_child01_02_01 有 UILayerElement,
        ///     view01_child01_02_01 的父节点直到 view01 才有UILayerElement，则取 view01_child01_02_01 所有父节点里面 view01 的子节点(view01_child01).GetSiblingIndex()
        /// </summary>
        public int SortingLayerSiblingIndex
        {
            get
            {
                if (m_nParentLayerElementSiblingIndex != null)
                    m_nSiblingIndex = (int)m_nParentLayerElementSiblingIndex;
                else
                    m_nSiblingIndex = GetTransform.GetSiblingIndex();
                return m_nSiblingIndex;
            }
        }

        private void CacheUILayer()
        {
            if (gameObject == null)
                return;
            List<USpeedUILayer> list = new List<USpeedUILayer>();

            gameObject.GetComponentsInParent(false, list);
            if (list.Count > 0)
                m_UILayer = list[0];

            List<IUIWndView> listView = new List<IUIWndView>();
            gameObject.GetComponentsInParent(false,listView);
            if (listView.Count > 0)
                m_UIWndView = listView[0];
        }

        private bool CanSortingOrder()
        {
            if (m_UICanvasRenderer != null || (m_EffectRenderer != null && m_EffectRenderer.Length > 0))
                return true;
            return false;
        }

        protected override void OnEnable()
        {
            RegisterUIElement();
            SetOnEnable();
        }

        protected override void OnDisable()
        {
            UnRegisterUIElement();
            SetDisable();
            base.OnDisable();
        }

        protected override void OnCanvasHierarchyChanged()
        {
            //USpeedUILayer currentLayer = m_UILayer;
            //CacheUILayer();

            //if (currentLayer != m_UILayer)
            //{
            //    USpeedUILayerManager.UnRegisterElementForLayer(currentLayer, this);
            //    USpeedUILayerManager.RegisterElementForLayer(UILayer, this);
            //}

            UnRegisterUIElement();
            CacheUILayer();
            RegisterUIElement();

            base.OnCanvasHierarchyChanged();
        }
        //只有在运行的时候才会执行
        protected override void OnTransformParentChanged()
        {
            base.OnTransformParentChanged();

            if (!IsActive())
                return;
            CacheUILayer();
            RegisterUIElement();
        }
        //只有在运行的时候才会执行
        protected override void OnBeforeTransformParentChanged()
        {
            base.OnBeforeTransformParentChanged();
            UnRegisterUIElement();
        }

        public void RegisterUIElement()
        {
            USpeedUILayerManager.RegisterElementForLayer(UILayer, this);
        }
        public void UnRegisterUIElement()
        {
            USpeedUILayerManager.UnRegisterElementForLayer(UILayer, this);
        }

        /// <summary>
        /// 编辑模式下在各Layer之间移动不会执行OnBeforeTransformParentChanged()和OnTransformParentChanged()
        /// 所以加个方法主动执行下
        /// </summary>
        public void TransformParentChanged()
        {
            if (!Application.isEditor || !IsActive())
                return;

            UnRegisterUIElement();
            CacheUILayer();
            RegisterUIElement();
        }


        #region 分类型处理
        public void SetSortingOrder()
        {
            if (!IsUseSortingLayerOffset)
                return;

            int _nNewOrder = USpeedUILayerManager.CalculateLayerElementOrder(this);//SortingLayerOrder + SortingLayerOrderOffset + SortingLayrOrderParentOffset;

            switch (UILayerElementType)
            {

                case EUILayerElementType.ELET_UI:
                    SetSortingOrder_UI(_nNewOrder);
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetSortingOrder_Effect(_nNewOrder);
                    break;
                default:
                    break;
            }
            //Debug.LogWarning("注册UIElement：" + this.gameObject.name + ",层级为：" + _nNewOrder);
        }

        protected void SetUIElementType_Enter(EUILayerElementType _eNewType)
        {
            switch (_eNewType)
            {
                case EUILayerElementType.ELET_UI:
                    SetUIElementType_UI_Enter(_eNewType);
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetUIElementType_Effect_Enter(_eNewType);
                    break;
                default:
                    break;
            }
        }
        protected void SetUIElementType_Leave(EUILayerElementType _eOldType)
        {
            switch (_eOldType)
            {
                case EUILayerElementType.ELET_UI:
                    SetUIElementType_UI_Leave(_eOldType);
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetUIElementType_Effect_Leave(_eOldType);
                    break;
                default:
                    break;
            }
        }

        protected void SetSortingLayerName(string _strNewSortingLayerName)
        {
            switch (UILayerElementType)
            {
                case EUILayerElementType.ELET_UI:
                    SetSortingLayerName_UI(_strNewSortingLayerName);
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetSortingLayerName_Effect(_strNewSortingLayerName);
                    break;
                default:
                    break;
            }
        }

        protected void SetOnEnable()
        {
            switch (UILayerElementType)
            {
                case EUILayerElementType.ELET_UI:
                    SetOnEnable_UI();
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetOnEnable_Effect();
                    break;
                default:
                    break;
            }

            SetSortingOrder();
        }

        protected void SetDisable()
        {
            switch (UILayerElementType)
            {
                case EUILayerElementType.ELET_UI:
                    SetDisable_UI();
                    break;
                case EUILayerElementType.ELET_Effect:
                    SetDisable_Effect();
                    break;
                default:
                    break;
            }
        }
        #endregion
    }
}
