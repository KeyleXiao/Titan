/*******************************************************************
** 文件名:	TreeNodeToggle.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-16
** 版  本:	1.8.0
** 描  述:	树形列表节点显示对象的 可展开对象
            
		
********************************************************************/

using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections;

namespace UIWidgets {
	/// <summary>
	/// Tree node toggle.
	/// </summary>
	public class TreeNodeToggle : UIBehaviour, IPointerUpHandler, IPointerDownHandler, IPointerClickHandler {
		/// <summary>
		/// OnClick event.
		/// </summary>
		public UnityEvent OnClick = new UnityEvent();

		#region IPointerUpHandler implementation
		/// <summary>
		/// Raises the pointer up event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnPointerUp(PointerEventData eventData)
		{
			
		}
		#endregion

		#region IPointerDownHandler implementation
		/// <summary>
		/// Raises the pointer down event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnPointerDown(PointerEventData eventData)
		{
			
		}
		#endregion

		/// <summary>
		/// Raises the pointer click event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnPointerClick(PointerEventData eventData)
		{
			if (eventData.button!=PointerEventData.InputButton.Left)
			{
				return;
			}

			OnClick.Invoke();
		}
	}
}
