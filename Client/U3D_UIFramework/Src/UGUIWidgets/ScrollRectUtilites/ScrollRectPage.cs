/*******************************************************************
** 文件名:	ScrollRectPaginator.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-7-1
** 版  本:	1.8.0
** 描  述:	分页显示
********************************************************************/

using UnityEngine;
using UnityEngine.EventSystems;

namespace UIWidgets {
	/// <summary>
	/// ScrollRectPage.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/ScrollRectPage")]
	public class ScrollRectPage : MonoBehaviour, IPointerDownHandler, IPointerUpHandler, IPointerClickHandler, ISubmitHandler {
		/// <summary>
		/// The page number.
		/// </summary>
		[HideInInspector]
		public int Page;

		/// <summary>
		/// OnPageSelect event.
		/// </summary>
		[SerializeField]
		public ScrollRectPageSelect OnPageSelect = new ScrollRectPageSelect();

		/// <summary>
		/// Sets the page number.
		/// </summary>
		/// <param name="page">Page.</param>
		public virtual void SetPage(int page)
		{
			Page = page;
		}

		#region IPointerClickHandler implementation
		/// <summary>
		/// Called by a BaseInputModule when an OnPointerClick event occurs.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public virtual void OnPointerClick(PointerEventData eventData)
		{
			OnPageSelect.Invoke(Page);
		}
		#endregion

		#region IPointerDownHandler implementation
		/// <summary>
		/// Called by a BaseInputModule when an OnPointerDown event occurs.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public virtual void OnPointerDown(PointerEventData eventData)
		{
		}
		#endregion

		#region IPointerUpHandler implementation
		/// <summary>
		/// Called by a BaseInputModule when an OnPointerUp event occurs.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public virtual void OnPointerUp(PointerEventData eventData)
		{
		}
		#endregion

		#region ISubmitHandler implementation
		/// <summary>
		/// Called by a BaseInputModule when an OnSubmit event occurs.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnSubmit(BaseEventData eventData)
		{
			OnPageSelect.Invoke(Page);
		}
		#endregion
	}
}
