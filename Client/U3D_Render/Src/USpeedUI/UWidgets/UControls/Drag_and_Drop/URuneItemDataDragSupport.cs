using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
namespace USpeedUI.UWidgets
{
    public class URuneItemDataDragSupport : DragSupport<TreeNode<URuneTypeTreeItemData>>
    {
        /// <summary>
        /// The drag info.
        /// </summary>
        [SerializeField]
        public URuneSlotItemComponent DragInfo;
        public GameObject runePageWndView;
        private GameObject draggingRune;
        private RectTransform draggingTransform;
        public int nDelth;
        public virtual void Start()
        {
            if (DragInfo != null)
            {
                DragInfo.gameObject.SetActive(false);
            }
            draggingTransform = runePageWndView.transform as RectTransform;
        }

        protected override void InitDrag(PointerEventData eventData)
        {

            Data = GetComponent<URuneTypeTreeItemComponent>().Node;
          
            ShowDragInfo();
        }
        protected virtual void ShowDragInfo()
        { }
        protected virtual void HideDragInfo()
        { }

        public override void Dropped(bool success)
        {
            HideDragInfo();
            base.Dropped(success);
        }
        public  override void OnBeginDrag(PointerEventData eventData)
        {
            nDelth = GetComponent<URuneTypeTreeItemComponent>().ItemDepth;
            if(nDelth<=0)
            {
                return;
            }

            //Debug.Log("开始拖拽事件：" + GetComponent<URuneTypeTreeItemComponent>().Item.nRuneId);
            draggingRune = new GameObject("RuneIcon");

            draggingRune.transform.SetParent(runePageWndView.transform, false);
            draggingRune.transform.SetAsLastSibling();
            var image = draggingRune.AddComponent<Image>();
            CanvasGroup group = draggingRune.AddComponent<CanvasGroup>();
            group.blocksRaycasts = false;
            image.sprite = this.transform.FindChild("Icon").GetComponent<Image>().sprite;
            image.SetNativeSize();
            draggingRune.GetComponent<RectTransform>().sizeDelta = new Vector2(40f, 45f);
            if(draggingRune!=null)
            {
                var rt = draggingRune.GetComponent<RectTransform>();
                Vector3 globalMousePos;
                if(RectTransformUtility.ScreenPointToWorldPointInRectangle(draggingTransform,eventData.position,eventData.pressEventCamera,out globalMousePos))
                {
                    rt.SetPosition(globalMousePos);
                    rt.rotation = draggingTransform.rotation;
                }
            }

        }

        public override void OnDrag(PointerEventData eventData)
        {
            if (nDelth <= 0)
            {
                return;
            }

            //Debug.Log("OnDrag:拖动中事件" + eventData.position);

            if(draggingRune==null)
            {
                return;
            }

            var rt = draggingRune.GetComponent<RectTransform>();
            Vector3 globalMousePos;
            if (RectTransformUtility.ScreenPointToWorldPointInRectangle(draggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
            {
                rt.SetPosition(globalMousePos);
                rt.rotation = draggingTransform.rotation;
            }
        }

        public override void OnEndDrag(PointerEventData eventData)
        {
            if (nDelth <= 0)
            {
                return;
            }

            //Debug.Log("OnEndDrag:拖动结束时间" + eventData.position);
            {
                if (draggingRune == null)
                {
                    return;
                }

                ResNode.DestroyRes(ref draggingRune);
            }
        }
    }
}
