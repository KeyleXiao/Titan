using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using DataCenter;
using USpeedUI.UExtensions;

namespace USpeedUI.UWidgets
{
    public class URuneItemDataDropSupport : MonoBehaviour, IDropSupport<TreeNode<URuneTypeTreeItemData>>, IDropHandler,IPointerEnterHandler,IPointerExitHandler
    {
        URuneSlotItemComponent _source;
        public URuneSlotItemComponent Source
        {
            get
            {
                if (_source == null)
                {
                    _source = GetComponent<URuneSlotItemComponent>();
                }
                return _source;
            }
        }

        public bool CanReceiveDrop(TreeNode<URuneTypeTreeItemData> data, PointerEventData eventData)
        {
            throw new NotImplementedException();
        }

        public void Drop(TreeNode<URuneTypeTreeItemData> data, PointerEventData eventData)
        {
            throw new NotImplementedException();
        }

        public void DropCanceled(TreeNode<URuneTypeTreeItemData> data, PointerEventData eventData)
        {
            throw new NotImplementedException();
        }

        public  void OnDrop(PointerEventData eventData)
        {
            // Debug.Log("放下事件：" + eventData.position);
            var originalObj = eventData.pointerDrag;
            if(originalObj==null)
            {
                return;
            }

            bool bActive = this.transform.FindChild("Sprite").gameObject.activeSelf;          
            if (bActive)
            {
                return;
            }
          
            if (_source == null)
            {
                _source = GetComponent<URuneSlotItemComponent>();
            }

            var dragData =  eventData.pointerDrag.GetComponent<URuneTypeTreeItemComponent>();
            if(dragData == null)
            {
                return;
            }

            if(_source.nRuneSlotType != dragData.Item.nRuneType)
            {
                return;
            }

            UTooltipTrigger dragTooltipTrigger = originalObj.gameObject.GetComponent<UTooltipTrigger>();
            if (dragTooltipTrigger == null)
            {
                Debug.LogError("DragUTooltipTrigger == null");
                return;
            }

            UTooltipTrigger  slotTooltipTrigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if(slotTooltipTrigger==null)
            {
                slotTooltipTrigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                slotTooltipTrigger.backgroundTint = Color.grey;
                slotTooltipTrigger.IsUpStay = false;
            }

            slotTooltipTrigger.enabled=true;
            slotTooltipTrigger.SetText(UTooltipParamName.BodyText, dragTooltipTrigger.parameterizedTextFields[0].value);
            int nRuneId = dragData.Item.nRuneId;
            this.transform.FindChild("Sprite").gameObject.SetActive(true);
            this.transform.FindChild("Sprite").gameObject.GetComponent<Image>().sprite = eventData.pointerDrag.transform.FindChild("Icon").GetComponent<Image>().sprite;
      
            LogicDataCenter.runePageDataManager.SetRunePagePointsInfo(_source.nRuneSlotId,nRuneId,true);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_PUTRUNE, null);

            eventData.pointerDrag.GetComponent<URuneItemDataDragSupport>().OnEndDrag(eventData);
        }

        public  void OnPointerEnter(PointerEventData eventData)
        {
            bool bActive = this.transform.FindChild("Sprite").gameObject.activeSelf;
            if (!bActive && eventData.pointerDrag==null)
            {
                return;
            }
          
            this.transform.FindChild("High").gameObject.SetActive(true);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            this.transform.FindChild("High").gameObject.SetActive(false);
        }
    }
}
