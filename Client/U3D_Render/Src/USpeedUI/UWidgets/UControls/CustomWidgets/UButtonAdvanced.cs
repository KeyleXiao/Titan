/*******************************************************************
** 文件名:	UButtonAdvanced.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-8-29
** 版  本:	1.0
** 描  述:	新增用于扩展的按钮
            目前已有扩展功能
            1、阻止事件往父节点冒泡
            2、鼠标按下、弹起、进入、离开回调
********************************************************************/
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    [System.Serializable]
    public class PointerUnityEvent : UnityEvent<PointerEventData>
    {

    }
    public class UButtonAdvanced : Button
    {
        /// <summary>
        /// 是否把当前事件往父节点冒泡上去
        /// </summary>
        [Tooltip("是否把当前事件往父节点冒泡上去")]
        public bool IsCanBubbleEvent = true;

        [SerializeField]
        public PointerUnityEvent onPointerDown = new PointerUnityEvent();

        [SerializeField]
        public PointerUnityEvent onPointerUp = new PointerUnityEvent();

        [SerializeField]
        public PointerUnityEvent onPointerEnter = new PointerUnityEvent();

        [SerializeField]
        public PointerUnityEvent onPointerExit = new PointerUnityEvent();

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);

            if (eventData is UPointerEventDataEx)
            {
                UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
                eventDataEx.CanBubbleEvent = IsCanBubbleEvent;
            }
        }
        public override void OnPointerEnter(PointerEventData eventData)
        {
            onPointerEnter.Invoke(eventData);
            base.OnPointerEnter(eventData);

            if (eventData is UPointerEventDataEx)
            {
                UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
                eventDataEx.CanBubbleEvent = IsCanBubbleEvent;
            }
        }
        public override void OnPointerDown(PointerEventData eventData)
        {
            onPointerDown.Invoke(eventData);
            base.OnPointerDown(eventData);

            if (eventData is UPointerEventDataEx)
            {
                UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
                eventDataEx.CanBubbleEvent = IsCanBubbleEvent;
            }
        }
        public override void OnPointerExit(PointerEventData eventData)
        {
            onPointerExit.Invoke(eventData);
            base.OnPointerExit(eventData);
            if (eventData is UPointerEventDataEx)
            {
                UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
                eventDataEx.CanBubbleEvent = IsCanBubbleEvent;
            }
        }
        public override void OnPointerUp(PointerEventData eventData)
        {
            onPointerUp.Invoke(eventData);
            base.OnPointerUp(eventData);
            if (eventData is UPointerEventDataEx)
            {
                UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
                eventDataEx.CanBubbleEvent = IsCanBubbleEvent;
            }
        }
    }
}


