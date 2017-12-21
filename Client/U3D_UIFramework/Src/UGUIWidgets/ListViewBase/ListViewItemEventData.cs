/*******************************************************************
** 文件名:	ListViewItemEventData.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	列表显示对象的 选择事件
		
********************************************************************/

using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;

namespace UIWidgets {
	/// <summary>
	/// List view item event data.
	/// </summary>
	public class ListViewItemEventData : BaseEventData {
		/// <summary>
		/// The new selected object.
		/// </summary>
		public GameObject NewSelectedObject;

		/// <summary>
		/// Initializes a new instance of the <see cref="UIWidgets.ListViewItemEventData"/> class.
		/// </summary>
		/// <param name="eventSystem">Event system.</param>
		public ListViewItemEventData(EventSystem eventSystem) : base(eventSystem)
		{
		}
	}
}
