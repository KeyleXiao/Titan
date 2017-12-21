/*******************************************************************
** 文件名:	DraggableHandle.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-20
** 版  本:	1.8.0
** 描  述:	可拖拽对象ID
		
********************************************************************/

using UnityEngine;
using UnityEngine.EventSystems;
using System;

namespace UIWidgets
{
    /// <summary>
    /// Draggable handle.
    /// </summary>
    public class DraggableHandle : MonoBehaviour, IDragHandler, IInitializePotentialDragHandler,IBeginDragHandler,IEndDragHandler
    {
        RectTransform drag;
        Canvas canvas;
        RectTransform canvasRect;

        public PointerEventData.InputButton DragInputButton = PointerEventData.InputButton.Left;

        #region 代理方法
        public delegate void HandleOnDrag(PointerEventData args);
        public event HandleOnDrag OnDragEvent;

        public delegate void HandleBeginDrag(PointerEventData args);
        public event HandleBeginDrag OnBeginDragEvent;

        public delegate void HandleEndDrag(PointerEventData args);
        public event HandleEndDrag OnEndDragEvent;
        #endregion

        /// <summary>
        /// Set the specified draggable object.
        /// </summary>
        /// <param name="newDrag">New drag.</param>
        public void Drag(RectTransform newDrag)
        {
            drag = newDrag;
        }

        /// <summary>
        /// Raises the initialize potential drag event.
        /// </summary>
        /// <param name="eventData">Event data.</param>
        public virtual void OnInitializePotentialDrag(PointerEventData eventData)
        {
            canvasRect = Utilites.FindTopmostCanvas(transform) as RectTransform;
            canvas = canvasRect.GetComponent<Canvas>();
        }

        /// <summary>
        /// Raises the drag event.
        /// </summary>
        /// <param name="eventData">Event data.</param>
        public virtual void OnDrag(PointerEventData eventData)
        {
            if (eventData.button != DragInputButton)
                return;

            if (canvas == null)
            {
                throw new MissingComponentException(gameObject.name + " not in Canvas hierarchy.");
            }
            Vector2 cur_pos;
            Vector2 prev_pos;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(drag, eventData.position, eventData.pressEventCamera, out cur_pos);
            RectTransformUtility.ScreenPointToLocalPointInRectangle(drag, eventData.position - eventData.delta, eventData.pressEventCamera, out prev_pos);

            var new_pos = new Vector3(
                drag.localPosition.x + (cur_pos.x - prev_pos.x),
                drag.localPosition.y + (cur_pos.y - prev_pos.y),
                drag.localPosition.z);

            drag.localPosition = new_pos;

            //分发OnDrag消息
            if(OnDragEvent != null)
                OnDragEvent.Invoke(eventData);
        }

        public void OnEndDrag(PointerEventData eventData)
        {
            if(eventData.button == DragInputButton)
            {
                //分发OnEndDrag消息
                if (OnEndDragEvent != null)
                    OnEndDragEvent.Invoke(eventData);
            }
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            if(eventData.button == DragInputButton)
            {
                //分发BeginDrag消息
                if (OnBeginDragEvent != null)
                    OnBeginDragEvent.Invoke(eventData);
            }
        }
    }
}
