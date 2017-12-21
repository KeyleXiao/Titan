/*******************************************************************
** 文件名:	UListComponentPopup.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/22
** 版  本:	1.0
** 描  述:	弹出菜单列表显示组件
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    public class UListComponentPopupBase : Button
    {
        private RectTransform m_RectTransform;

        /// <summary>
        /// 背景图
        /// </summary>
        public Image ItemBG;
        /// <summary>
        /// 类型图标
        /// </summary>
        public Image ItemTypeImage;
        /// <summary>
        /// 显示文本
        /// </summary>
        public Text ItemTypeName;
        /// <summary>
        /// 下级节点图标
        /// </summary>
        public Image ItemSubMenuImage;

        public RectTransform GetTransform
        {
            get
            {
                if (m_RectTransform == null)
                    m_RectTransform = transform as RectTransform;
                return m_RectTransform;
            }
        }
        public float GetHeight
        {
            get
            {
                return GetTransform.sizeDelta.y;
            }
        }

        /// <summary>
        /// 这里存个引用，虽然不建议，但目前没想到好方法处理
        /// </summary>
        public UListComponentItemDataPopup ItemData { get; set; }
        public UListPopup ChildPopupList
        {
            get { return m_childPopuplist; }
            set
            {
                m_childPopuplist = value;
                if (value == null)
                    DoStateTransition(SelectionState.Normal, true);
            }
        }
        protected UListPopup m_childPopuplist;

        protected override void Awake()
        {
            base.Awake();
            if(ItemTypeImage == null)
            {
                Transform tr = transform.FindChild("ItemTypeImage");
                if (tr != null)
                {
                    ItemTypeImage = tr.GetComponent<Image>();
                }
            }
            if (ItemTypeName == null)
            {
                Transform tr = transform.FindChild("ItemTypeName");
                if (tr != null)
                {
                    ItemTypeName = tr.GetComponent<Text>();
                }
            }
            if (ItemSubMenuImage == null)
            {
                Transform tr = transform.FindChild("ItemSubMenuImage");
                if (tr != null)
                {
                    ItemSubMenuImage = tr.GetComponent<Image>();
                }
            }
            if(ItemBG == null)
            {
                Transform tr = transform.FindChild("ItemBG");
                if(tr != null)
                {
                    ItemBG = tr.GetComponent<Image>();
                }
            }
            ItemTypeImage.gameObject.SetActive(false);
            ItemSubMenuImage.gameObject.SetActive(false);
        }



        [Serializable]
        public class UListComponentPopupEnterEvent : UnityEvent<PointerEventData,UListComponentPopupBase>
        { }
        [Serializable]
        public class UListComponentPopupExitEvent : UnityEvent<PointerEventData,UListComponentPopupBase>
        { }
        [SerializeField]
        private UListComponentPopupEnterEvent m_OnEnter = new UListComponentPopupEnterEvent();

        public UListComponentPopupEnterEvent onEnter
        {
            get { return m_OnEnter; }
            set { m_OnEnter = value; }
        }

        [SerializeField]
        private UListComponentPopupExitEvent m_OnExit = new UListComponentPopupExitEvent();

        public UListComponentPopupExitEvent onExit
        {
            get { return m_OnExit; }
            set { m_OnExit = value; }
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);

            if(interactable)
                onEnter.Invoke(eventData,this);            
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            if(interactable)
                onExit.Invoke(eventData,this);
        }

        protected override void DoStateTransition(SelectionState state, bool instant)
        {

            if (ChildPopupList != null && state == SelectionState.Normal)
            {
                base.DoStateTransition(SelectionState.Highlighted, instant);
                return;
            }
            base.DoStateTransition(state, instant);

        }
    }

    public class UListComponentPopup : UListComponentPopupBase
    {
        protected override void OnDisable()
        {
            targetGraphic = ItemBG;

            base.OnDisable();
        }
        public void SetData(UListComponentItemDataPopup newData)
        {
            this.ItemTypeName.text = newData.ItemTypeName;


            if (newData.ItemChildItems == null || newData.ItemChildItems.Count <= 0)
            {
                this.onClick.AddListener(newData.ClearPopupMenuAction);
                this.onClick.AddListener(newData.ItemSelectedAction);
                if (this.ItemSubMenuImage != null)
                    this.ItemSubMenuImage.gameObject.SetActive(false);
            }             
            else
            {
                if(this.ItemSubMenuImage != null)
                    this.ItemSubMenuImage.gameObject.SetActive(true);
            }

            if (this.ItemTypeImage != null)
            {

                this.ItemTypeImage.sprite = newData.ItemPopupSprite;
                this.ItemTypeImage.gameObject.SetActive(newData.ItemPopupSprite == null ? false : true);
            }
           

            if (newData.ItemInteractable)
            {
                this.targetGraphic = ItemBG;
            }
            else
            {
                this.targetGraphic = ItemTypeName;
            }
            //DoStateTransition(this.currentSelectionState, true);
            
            this.interactable = newData.ItemInteractable;

            this.ItemData = newData;

        }
    }
}
