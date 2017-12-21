using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;

namespace USpeedUI
{
    // 窗口视图基类
    public abstract class UIBaseWndView : MonoBehaviour, IUIWndView//, IBeginDragHandler, IDragHandler, IEndDragHandler
    {
        // 对应窗口逻辑类
        protected IUIWnd m_wnd;
        // transform组件
        protected RectTransform m_transform;

        #region 拖动相关
        private bool m_canDrag = false;
        protected DraggableHandle m_DraggableHandle;
        protected bool canDrag
        {
            get { return m_canDrag; }
            set
            {
                if (value != m_canDrag)
                {
                    m_canDrag = value;
                    if(m_canDrag)
                    {
                        m_DraggableHandle = gameObject.AddComponent<DraggableHandle>();
                        m_DraggableHandle.Drag(this.GetTransform());
                        m_DraggableHandle.OnBeginDragEvent += OnBeginDrag;
                        m_DraggableHandle.OnDragEvent += OnDrag;
                        m_DraggableHandle.OnEndDragEvent += OnEndDrag;
                    }
                    else
                    {
                        m_DraggableHandle.OnBeginDragEvent -= OnBeginDrag;
                        m_DraggableHandle.OnDragEvent -= OnDrag;
                        m_DraggableHandle.OnEndDragEvent -= OnEndDrag;

                        gameObject.RemoveComponent<DraggableHandle>();
                        m_DraggableHandle = null;
                    }
                }
            }
        }
        protected bool m_draging;
        #endregion

        #region 窗口顶置
        private bool m_canBringToFrom = false;
        protected BringToFront m_BringToFront;
        /// <summary>
        /// 设置当前窗口是否可以顶置
        /// </summary>
        protected bool canBringToFrom
        {
            get
            {
                return m_canBringToFrom;
            }
            set
            {
                if(value != m_canBringToFrom)
                {
                    m_canBringToFrom = value;
                    if (m_canBringToFrom)
                    {
                        //if (m_wnd.GetLayer() == WndLayerDef.WND_LAYER_POPUP01)
                            m_BringToFront = gameObject.AddComponent<BringToFront>();
                        //else
                        //    Trace.LogWarning("目前只有View处于WND_LAYER_MIDDLE层时才可以顶置。");
                    }
                    else
                        gameObject.RemoveComponent<BringToFront>();
                }
            }
        }
        #endregion

        #region View显示时全屏效果
        //默认效果
        private Type m_DefaultImageEffect = typeof(ImageEffects.BlurAndDownScreen);
        private Type m_CurrentImageEffect = null;
        protected bool m_UseImageEffect = false;

        protected Type imageEffect
        {
            get
            {
                return m_CurrentImageEffect;
            }
            set
            {
                if(!value.IsSubclassOf(typeof(ImageEffects.ImageEffectNode)))
                {
                    Trace.LogWarning("设置的类型不是ImageEffectNode 的子类");
                    return;
                }
                //移除旧的效果
                if (m_CurrentImageEffect != value)
                    DestroyImageEffect();

                m_CurrentImageEffect = value;

                ToggleImageEffect();
            }
        }
        protected bool canUseImageEffect
        {
            get
            {
                return m_UseImageEffect;
            }
            set
            {
                //return;
                m_UseImageEffect = value;
                ToggleImageEffect();
            }
        }
        protected void ToggleImageEffect()
        {
            if (IsVisible() && canUseImageEffect)
            {
                //CreateImageEffect();
                UISystem.Instance.ActiveCommonBlurEffect(m_wnd, true);
            }
            else if(!IsVisible() && canUseImageEffect)
            {
                //DestroyImageEffect();
                UISystem.Instance.ActiveCommonBlurEffect(m_wnd, false);
            }
        }
        protected void CreateImageEffect()
        {
            UEffectParamBase param = new UEffectImageParam(_eAttachImageEffect: imageEffect == null ? m_DefaultImageEffect : imageEffect, _tfAttachParent: this.GetTransform());
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectImage, ref param);
        }
        protected void DestroyImageEffect()
        {
            UEffectParamBase param = new UEffectImageParam(_eAttachImageEffect: imageEffect == null ? m_DefaultImageEffect : imageEffect, _tfAttachParent: this.GetTransform());
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectImage, ref param);
        }

        /// <summary>
        /// 在SetVisible时处理是否使用全屏效果
        /// </summary>
        protected void ProcessImageEffect()
        {
            if(canUseImageEffect)
            {
                ToggleImageEffect();
            }
        }
        #endregion

        // IUIWndView
        // 初始化
        public virtual bool Init(IUIWnd wnd)
        {
            m_wnd = wnd;
            USpeedUILayerManager.RegisterWndViewForLayer(this);
            return true;
        }

        // 销毁
        public virtual void Destroy()
        {
            //移除Drag事件
            canDrag = false;
            canUseImageEffect = false;
            //移除该View用到的动态Sprite
            USpriteManager.Instance.DeleteSprite(m_wnd.GetID());
			USpeedUILayerManager.UnRegisterWndViewForLayer(this);
        }

        // 获取对应的IUIWnd
        public virtual IUIWnd GetUIWnd()
        {
            return m_wnd;
        }

        // 获取RectTransform
        public virtual RectTransform GetTransform()
        {
            return m_transform = m_transform ?? GetComponent<RectTransform>();
        }

        // 是否可见
        public virtual bool IsVisible()
        {
            return gameObject.activeSelf;
        }

        // 设置是否可见
        public virtual void SetVisible(bool visible)
        {
            m_wnd.SetVisible(visible);

            SetRendererVisible(visible);
            //ProcessImageEffect();
        }

        // IBeginDragHandler
        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            if (m_canDrag && m_draging == false && eventData.button == PointerEventData.InputButton.Left)
            {
                m_draging = true;
            }
        }

        // IDragHandler
        public virtual void OnDrag(PointerEventData eventData)
        {

        }

        // IEndDragHandler
        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;
            }
        }

        // MonoBehaviour
        public virtual void Awake()
        {
            m_transform = GetComponent<RectTransform>();
        }

        public virtual void Start()
        {
        }

        public virtual void Update()
        {
        }
        /// <summary>
        /// 设置view的渲染状态
        /// </summary>
        internal virtual void SetRendererVisible(bool _bVisible)
        {
            gameObject.SetActive(_bVisible);
            ProcessImageEffect();
        }
    }
}
