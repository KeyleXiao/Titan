/*******************************************************************
** 文件名:	ImageAdvanced.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	扩展Image，添加些鼠标事件
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

namespace UIWidgets
{
	/// <summary>
	/// Image Advanced. You can add callback on PointerEnter/Exit/Down/Up 
	/// </summary>
	public class ImageAdvanced : Image,
		IPointerDownHandler,
		IPointerUpHandler,
		IPointerEnterHandler,
		IPointerExitHandler
	{

		/// <summary>
		/// What to do when the event system send a pointer down Event.
		/// </summary>
		public PointerUnityEvent onPointerDown = new PointerUnityEvent();

		/// <summary>
		/// What to do when the event system send a pointer up Event.
		/// </summary>
		public PointerUnityEvent onPointerUp = new PointerUnityEvent();

		/// <summary>
		/// What to do when the event system send a pointer enter Event.
		/// </summary>
		public PointerUnityEvent onPointerEnter = new PointerUnityEvent();

		/// <summary>
		/// What to do when the event system send a pointer exit Event.
		/// </summary>
		public PointerUnityEvent onPointerExit = new PointerUnityEvent();

		/// <summary>
		/// Raises the pointer down event.
		/// </summary>
		/// <param name="eventData">Current event data.</param>
		public virtual void OnPointerDown(PointerEventData eventData)
		{
			onPointerDown.Invoke(eventData);
		}

		/// <summary>
		/// Raises the pointer up event.
		/// </summary>
		/// <param name="eventData">Current event data.</param>
		public virtual void OnPointerUp(PointerEventData eventData)
		{
			onPointerUp.Invoke(eventData);
		}

		/// <summary>
		/// Raises the pointer enter event.
		/// </summary>
		/// <param name="eventData">Current event data.</param>
		public virtual void OnPointerEnter(PointerEventData eventData)
		{
			onPointerEnter.Invoke(eventData);
		}

		/// <summary>
		/// Raises the pointer exit event.
		/// </summary>
		/// <param name="eventData">Current event data.</param>
		public virtual void OnPointerExit(PointerEventData eventData)
		{
			onPointerExit.Invoke(eventData);
		}
	}
}
