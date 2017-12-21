/*******************************************************************
** 文件名:	IItemHeight.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	数据源单个数据所占的高度
********************************************************************/


namespace UIWidgets {
	/// <summary>
	/// Interface for ListViewCustomHeight items.
	/// </summary>
	public interface IItemHeight {
		/// <summary>
		/// Gets or sets the height of item.
		/// </summary>
		/// <value>The height.</value>
		float Height {
			get; set;
		}
	}
}
