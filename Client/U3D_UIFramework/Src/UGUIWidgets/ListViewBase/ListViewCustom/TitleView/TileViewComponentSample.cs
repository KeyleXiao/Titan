/*******************************************************************
** 文件名:	TileViewComponentSample.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	TitleView例子显示对象
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System;
using UIWidgets;

namespace UIWidgetsSamples {
	/// <summary>
	/// ListViewIcons item component.
	/// </summary>
	public class TileViewComponentSample : ListViewItem {
		/// <summary>
		/// The icon.
		/// </summary>
		[SerializeField]
		public Image Icon;
		
		/// <summary>
		/// The text.
		/// </summary>
		[SerializeField]
		public Text Name;

		/// <summary>
		/// Set icon native size.
		/// </summary>
		public bool SetNativeSize = true;

		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="item">Item.</param>
		public void SetData(TileViewItemSample item)
		{
			{
				if (Icon!=null)
				{
					Icon.sprite = item.Icon;
				}
				if (Name!=null)
				{
					Name.text = item.Name;
				}
			}

			if (Icon!=null)
			{
				if (SetNativeSize)
				{
					Icon.SetNativeSize();
				}
				
				//set transparent color if no icon
				Icon.color = (Icon.sprite==null) ? Color.clear : Color.white;
			}
		}
	}
}
