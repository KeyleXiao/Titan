/*******************************************************************
** 文件名:	TreeViewComponentBase.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	树形列表显示对象 ,更新显示元素
		
********************************************************************/

using UnityEngine;
using System.Collections;

namespace UIWidgets {
	/// <summary>
	/// TreeView component.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/TreeViewComponent")]
	public class TreeViewComponent : TreeViewComponentBase<TreeViewItem> {
		TreeViewItem item;

		/// <summary>
		/// Gets or sets the item.
		/// </summary>
		/// <value>The item.</value>
		public TreeViewItem Item {
			get {
				return item;
			}
			set {
				if (item!=null)
				{
					item.OnChange -= UpdateView;
				}
				item = value;
				if (item!=null)
				{
					item.OnChange += UpdateView;
				}
				UpdateView();
			}
		}

		/// <summary>
		/// Sets the data.
        /// <para>WillNote:获取当前更新的节点数据更新值</para>
		/// </summary>
		/// <param name="newNode">Node.</param>
		/// <param name="depth">Depth.</param>
		public override void SetData(TreeNode<TreeViewItem> newNode, int depth)
		{
			Node = newNode;
			base.SetData(Node, depth);

			Item = (Node==null) ? null : Node.Item;
		}

		/// <summary>
		/// Updates the view.
		/// </summary>
		protected virtual void UpdateView()
		{
			if ((Icon==null) || (Text==null))
			{
				return ;
			}
				
			if (Item==null)
			{
				Icon.sprite = null;
				Text.text = string.Empty;
			}
			else
			{
				Icon.sprite = Item.Icon;
				Text.text = Item.LocalizedName ?? Item.Name;
			}
			
			if (SetNativeSize)
			{
				Icon.SetNativeSize();
			}
			
			//set transparent color if no icon
			Icon.color = (Icon.sprite==null) ? Color.clear : Color.white;
		}

		/// <summary>
		/// Called when item moved to cache, you can use it free used resources.
		/// </summary>
		public override void MovedToCache()
		{
			if (Icon!=null)
			{
				Icon.sprite = null;
			}
		}

		/// <summary>
		/// This function is called when the MonoBehaviour will be destroyed.
		/// </summary>
        protected override void OnDestroy()
		{
			if (item!=null)
			{
				item.OnChange -= UpdateView;
			}
			base.OnDestroy();
		}
	}
}
