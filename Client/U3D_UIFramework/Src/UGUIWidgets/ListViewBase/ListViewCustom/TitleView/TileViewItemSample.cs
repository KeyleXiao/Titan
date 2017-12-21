/*******************************************************************
** 文件名:	TileViewItemSample.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	TitleView例子数据对象
********************************************************************/

using UnityEngine;

namespace UIWidgetsSamples {
	[System.Serializable]
	/// <summary>
	/// ListViewIcons item description.
	/// </summary>
	public class TileViewItemSample {
		/// <summary>
		/// The icon.
		/// </summary>
		[SerializeField]
		public Sprite Icon;
		
		/// <summary>
		/// The name.
		/// </summary>
		[SerializeField]
		public string Name;
	}
}
