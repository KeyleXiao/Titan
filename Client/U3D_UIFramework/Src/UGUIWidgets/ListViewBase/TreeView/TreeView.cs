/*******************************************************************
** 文件名:	TreeView.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	树形列表显示对象 ,更新显示元素
		
********************************************************************/
using UnityEngine;

namespace UIWidgets {
	/// <summary>
	/// TreeView.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/TreeView")]
	public class TreeView : TreeViewCustom<TreeViewComponent,TreeViewItem> 
    {

		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="component">Component.</param>
		/// <param name="item">Item.</param>
		protected override void SetData(TreeViewComponent component, ListNode<TreeViewItem> item)
		{
			component.SetData(item.Node, item.Depth);
		}
		
		/// <summary>
		/// Set highlights colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void HighlightColoring(TreeViewComponent component)
		{
			base.HighlightColoring(component);
			component.Text.color = HighlightedColor;
		}
		
		/// <summary>
		/// Set select colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void SelectColoring(TreeViewComponent component)
		{
			base.SelectColoring(component);
			component.Text.color = SelectedColor;
		}
		
		/// <summary>
		/// Set default colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void DefaultColoring(TreeViewComponent component)
		{
			if (component==null)
			{
				return ;
			}
			base.DefaultColoring(component);
			if (component.Text!=null)
			{
				component.Text.color = DefaultColor;
			}
		}
	}
}
