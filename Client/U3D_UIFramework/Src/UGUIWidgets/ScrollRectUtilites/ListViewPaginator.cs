/*******************************************************************
** 文件名:	ScrollRectPaginator.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-7-1
** 版  本:	1.8.0
** 描  述:	列表分页工具
            专用于ListView的分页
********************************************************************/

using UnityEngine;
using System;

namespace UIWidgets {

	/// <summary>
	/// ListView's paginator. Also works with TileView's.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/ListViewPaginator")]
	public class ListViewPaginator : ScrollRectPaginator {
		/// <summary>
		/// ListView.
		/// </summary>
		[SerializeField]
		protected ListViewBase ListView;

		/// <summary>
		/// Count of items on one page.
		/// </summary>
		[SerializeField]
		protected int perPage = 1;

		/// <summary>
		/// Gets or sets the count of items on one page.
		/// </summary>
		/// <value>The per page.</value>
		public int PerPage {
			get {
				return Mathf.Max(1, perPage);
			}
			set {
				perPage = Mathf.Max(1, value);
				RecalculatePages();
			}
		}

		bool isListViewPaginatorStarted;

		/// <summary>
		/// Start this instance.
		/// </summary>
        public override void Start()
		{
			if (isListViewPaginatorStarted)
			{
				return ;
			}
			isListViewPaginatorStarted = true;

			ScrollRect = ListView.GetScrollRect();
			base.Start();
		}

		/// <summary>
		/// Handle scroll changes.
		/// </summary>
		protected override void ScrollChanged()
		{
			var page = Mathf.FloorToInt(((float)ListView.GetNearestItemIndex()) / (ListView.GetItemsPerBlock() * PerPage));
			GoToPage(page, true);
		}

		/// <summary>
		/// Recalculate the pages count.
		/// </summary>
		protected override void RecalculatePages()
		{
			var per_block = ListView.GetItemsPerBlock() * PerPage;
			Pages = (per_block==0) ? 0 : Mathf.CeilToInt(((float)ListView.GetItemsCount()) / per_block);
		}

		/// <summary>
		/// Gets the page position.
		/// </summary>
		/// <returns>The page position.</returns>
		/// <param name="page">Page.</param>
		protected override float GetPagePosition(int page)
		{
			return ListView.GetItemPosition(page * ListView.GetItemsPerBlock() * PerPage);
		}
	}
}
