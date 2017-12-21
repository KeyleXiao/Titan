/*******************************************************************
** 文件名:	IObservable.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	观察对象的接口
		
********************************************************************/


namespace UIWidgets {
	/// <summary>
	/// OnChange.
	/// </summary>
	public delegate void OnChange();

	/// <summary>
	/// IObservable.
	/// </summary>
	public interface IObservable {

		/// <summary>
		/// Occurs when data changed.
		/// </summary>
		event OnChange OnChange;
	}
}
