/*******************************************************************
** 文件名:	UBuddySearchListView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/4/11
** 版  本:	1.0
** 描  述:	好友系统主面板搜索好友结果列表
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using System.Collections.Generic;
using System.Linq;
using System;

namespace UIWidgets {

	public class UBuddySearchListView : ListViewCustom<UBuddySearchListItemComponent, UBuddySearchListItem> {
		///// <summary>
		///// Awake this instance.
		///// </summary>
		//protected override void Awake()
		//{
		//	Start();
		//}

		[System.NonSerialized]
		bool isStartedListViewIcons = false;

		//protected Comparison<UBuddySearchListItem> ItemsComparison =
			//(x, y) => (x.LocalizedName ?? x.Name).CompareTo(y.LocalizedName ?? y.Name);

		/// <summary>
		/// Start this instance.
		/// </summary>
		public override void Start()
		{
			if (isStartedListViewIcons)
			{
				return ;
			}
			isStartedListViewIcons = true;

			base.Start();
			//SortFunc = list => list.OrderBy(item => item.LocalizedName ?? item.Name);
			//DataSource.Comparison = ItemsComparison;
		}

		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="component">Component.</param>
		/// <param name="item">Item.</param>
		protected override void SetData(UBuddySearchListItemComponent component, UBuddySearchListItem item)
		{
			component.SetData(item);
		}

		/// <summary>
		/// Set highlights colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void HighlightColoring(UBuddySearchListItemComponent component)
		{
			base.HighlightColoring(component);
			component.Text.color = HighlightedColor;
		}

		/// <summary>
		/// Set select colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void SelectColoring(UBuddySearchListItemComponent component)
		{
			base.SelectColoring(component);
			component.Text.color = SelectedColor;
		}

		/// <summary>
		/// Set default colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void DefaultColoring(UBuddySearchListItemComponent component)
		{
			base.DefaultColoring(component);
			component.Text.color = DefaultColor;
		}
	}
}