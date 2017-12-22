/*******************************************************************
** 文件名:	UGuideWidget.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-3-20
** 版  本:	1.0
** 描  述:	引导控件功能 
            1、用于那些引导时需要屏蔽界面其他区域
            2、用于引导时需要显示指示光标的

            使用方法：
            1、给那些引导需要处理的UI控件挂上脚本，并设置相关联的引导ID和顺序ID
            2、
********************************************************************/
using GUIDE;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;

namespace USpeedUI.Guide
{
    public class UGuideWidget : UCustomUIBehaviour, IPointerClickHandler
    {
        /// <summary>
        /// 识别响应哪个引导的时候
        /// </summary>
        [SerializeField]
        public EGuideNodeID GuideID;

        /// <summary>
        /// 一个引导ID可能需要有在多个情况下按顺序的控制引导控件
        /// </summary>
        [SerializeField]
        public int GuideStepID;

        /// <summary>
        /// 是否属于强制引导
        /// </summary>
        [SerializeField]
        public bool IsForceGuide;

        /// <summary>
        /// 是否处理点击事件
        /// </summary>
        [SerializeField]
        public bool IsAcceptClickEvent = true;

        /// <summary>
        /// 引导光效ID
        /// </summary>
        public int GuideEffectID;

        /// <summary>
        /// 用于强制引导时的模态ID
        /// </summary>
        private int? m_nModalKey;

        /// <summary>
        /// 模态的背景颜色
        /// </summary>
        [SerializeField]
        public Color? ModalColor = null;

        /// <summary>
        /// 模态的节点
        /// </summary>
        [SerializeField]
        public RectTransform ModalTrans = null;

        /// <summary>
        /// 原始的父节点
        /// </summary>
        private Transform m_tOriginalParent;

        /// <summary>
        /// 是否正在引导中
        /// </summary>
        private bool m_bShowGuide;

        /// <summary>
        /// 是否有点击
        /// </summary>
        private bool m_bWidgetClick;

        private UEffectParamBase m_uEffectParam;

        public RectTransform TestCursorEffect;
        [SerializeField]
        public TipPosition CursorEffectPosition = TipPosition.LeftMiddle;
        public RectTransform GetCursorEffect { get; set; }

        public bool IsWidgetClick()
        {
           return m_bWidgetClick;
        }

        public bool IsWidgetShow()
        {
            return m_bShowGuide;
        }

        protected void Register()
        {
            GuideManager.Instance.RegisterUGuideWidget(this);
        }

        protected void UnRegister()
        {
            GuideManager.Instance.UnRegisterUGuideWidget(this);
        }

        protected override void Start()
        {
            base.Start();
            m_tOriginalParent = GetTransform.parent;
            //ModalColor = new Color(0, 0, 0, 233f / 255f);

            GetCursorEffect = TestCursorEffect;
            m_bWidgetClick = false;

            Register();
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            Hide();
            UnRegister();
        }
        public void Show()
        {
            m_bShowGuide = true;
            if (IsForceGuide)
            {
                AddModalHelper();
            }
            AddCursorEffect();
        }

        public void Hide()
        {
            m_bShowGuide = false;
            

            RemoveModalHelper();
            RemoveCursorEffect();

        }

        /// <summary>
        /// 添加模态管理
        /// </summary>
        /// <param name="_nParam"></param>
        protected void AddModalHelper()
        {
                m_nModalKey = UIWidgets.ModalHelper.Open(this, null, ModalColor, null, ModalTrans == null ? GetRectTransform : ModalTrans);

            //GetTransform.SetParent(Utilites.FindTopmostCanvas(GetTransform), true);
            //GetTransform.SetAsLastSibling();

        }
        /// <summary>
        /// 移除模态管理
        /// </summary>
        protected void RemoveModalHelper()
        {
            if (m_nModalKey != null)
            {
                ModalHelper.Close((int)m_nModalKey);
                m_nModalKey = null;
            }
        }

        protected void AddCursorEffect()
        {
            if (GetCursorEffect == null)
            {
                //UEffectPrefabType.UEPT_GuideWidget_TalentLearnItem
                m_uEffectParam = new UEffectPrefabParam((UEffectPrefabType)GuideEffectID, GetRectTransform);
                if(UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_uEffectParam))
                    GetCursorEffect = (m_uEffectParam as UEffectPrefabParam).EffectGameObject.transform as RectTransform;

            }
            if(this.gameObject.activeInHierarchy && this.gameObject.activeSelf)
                StartCoroutine(DelayChangePos());
            //Vector2 pos = GetRectTransform.switchToRectTransform(UISystem.Instance.transform as RectTransform);

            //GetCursorEffect.localPosition = pos;

            /*
            Vector3[] Corners = new Vector3[4];
            Vector3 newPos = Vector3.zero;
            Vector2 newPivot = Vector2.zero;

            Canvas rootCanvas = UISystem.Instance.GetCanvas();
            Camera uiCamera = UISystem.Instance.GetCamera();


            UIWidgets.UTooltipExtensionMethods.GetWorldCorners(GetRectTransform.gameObject, rootCanvas, uiCamera, ref Corners);

            UIWidgets.UTooltipExtensionMethods.SetPosition(CursorEffectPosition, Corners, GetCursorEffect, rootCanvas, uiCamera, ref newPos);

            GetCursorEffect.SetPosition(newPos);

            //如果在窗口边缘则重新计算显示位置
            if (true)
            {
                UTooltipOverflow overflow;
                UIWidgets.UTooltipExtensionMethods.GetOverflow(GetCursorEffect, rootCanvas, uiCamera, out overflow);

                if (overflow.IsAny)
                {

                    UIWidgets.UTooltipExtensionMethods.SetPosition(overflow.SuggestNewPosition(CursorEffectPosition), Corners, GetCursorEffect, rootCanvas, uiCamera, ref newPos);

                    GetCursorEffect.SetPosition(newPos);
                }
            }
            //重置一下Z轴，有可能是取边框角顶点时改点Z轴有
            newPos = GetCursorEffect.localPosition;
            newPos.x += 0.5f;
            newPos.y -= 0.5f;
            newPos.z = 0;
            GetCursorEffect.localPosition = newPos;
            */
        }

        protected void RemoveCursorEffect()
        {
            if (GetCursorEffect == null)
                return;

            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_uEffectParam);
            m_uEffectParam = null;
            GetCursorEffect = null;
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (!IsAcceptClickEvent || eventData.button != PointerEventData.InputButton.Left)
                return;
            m_bWidgetClick = true;
            Hide();
        }

        IEnumerator DelayChangePos()
        {
            yield return new WaitForSeconds(0.5f);
            if(GetCursorEffect != null && IsForceGuide)
            {
                GetCursorEffect.SetParent(UISystem.Instance.transform, true);
                GetCursorEffect.GetComponent<USpeedUILayerElement>().SortingLayerName = UISystem.Instance.GetCanvas().sortingLayerName;
            }            
        }

        public void SetFromMsgData(UGuideWidgetMsgData _msgData)
        {
            this.GuideID = (EGuideNodeID)_msgData.nGuideID;
            this.GuideEffectID = _msgData.nGuideEffectID;
            this.GuideStepID = _msgData.nGuideStepID;
            this.IsForceGuide = _msgData.bForeceGuide;
            this.IsAcceptClickEvent = _msgData.bAcceptClickEvent;
        }
    }
}
