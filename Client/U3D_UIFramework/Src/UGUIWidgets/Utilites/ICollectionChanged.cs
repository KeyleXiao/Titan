/*******************************************************************
** 文件名:	ICollectionChanged.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	对象收集事件
		
********************************************************************/


namespace UIWidgets {
	/// <summary>
	/// IObservable.
	/// </summary>
	public interface ICollectionChanged {
		
		/// <summary>
		/// Occurs when changed collection (added item, removed item, replaced item).
		/// </summary>
		event OnChange OnCollectionChange;
	}
}
