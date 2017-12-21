/*******************************************************************
** 文件名:	ScrollRectPaginator.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-7-1
** 版  本:	1.8.0
** 描  述:	显示页数的分页
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

namespace UIWidgets {
	/// <summary>
	/// Scroll rect page with number.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/ScrollRectPageWithNumber")]
	public class ScrollRectPageWithNumber : ScrollRectPage {
		/// <summary>
		/// The number.
		/// </summary>
		[SerializeField]
		public Text Number;

		/// <summary>
		/// Sets the page number.
		/// </summary>
		/// <param name="page">Page.</param>
		public override void SetPage(int page)
		{
			base.SetPage(page);
			if (Number!=null)
			{
				Number.text = (page + 1).ToString();
			}
		}
	}
}
