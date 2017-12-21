/*******************************************************************
** 文件名:	SelectListener.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-19
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/

using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using System.Collections;

namespace UIWidgets
{
	/// <summary>
	/// SelectEvent.
	/// </summary>
	[System.Serializable]
	public class SelectEvent : UnityEvent<BaseEventData> {

	}

	/// <summary>
	/// Deselect listener.
	/// </summary>
	public class SelectListener : MonoBehaviour, ISelectHandler, IDeselectHandler
	{
		/// <summary>
		/// The OnSelect event.
		/// </summary>
		public SelectEvent onSelect = new SelectEvent();

		/// <summary>
		/// The OnDeselect event.
		/// </summary>
		public SelectEvent onDeselect = new SelectEvent();

		/// <summary>
		/// Raises the select event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnSelect(BaseEventData eventData)
		{
			onSelect.Invoke(eventData);
		}

		/// <summary>
		/// Raises the deselect event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnDeselect(BaseEventData eventData)
		{
			onDeselect.Invoke(eventData);
		}
	}
}
