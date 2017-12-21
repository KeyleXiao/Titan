/*******************************************************************
** 文件名:	ICollectionItemChanged.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	对象数据改变事件
		
********************************************************************/

namespace UIWidgets {
	/// <summary>
	/// IObservable.
	/// </summary>
	public interface ICollectionItemChanged {
		
		/// <summary>
		/// Occurs when changed data of item in collection.
		/// </summary>
		event OnChange OnCollectionItemChange;
	}
}
