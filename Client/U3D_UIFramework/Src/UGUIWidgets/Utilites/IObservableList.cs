/*******************************************************************
** 文件名:	IObservableList.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	观察对象接口 ，集合了对象数据改变事件
		
********************************************************************/

using System.Collections.Generic;
using System.Collections;
using System;

namespace UIWidgets {
	/// <summary>
	/// IObservableList.
	/// </summary>
	public interface IObservableList<T> : IList<T>, IObservable, ICollectionChanged, ICollectionItemChanged, IDisposable {

		/// <summary>
		/// Maintains performance while items are added/removed/changed by preventing the widgets from drawing until the EndUpdate method is called.
		/// </summary>
		void BeginUpdate();

		/// <summary>
		/// Ends the update and raise OnChange if something was changed.
		/// </summary>
		void EndUpdate();
	}
}
