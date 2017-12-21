/*******************************************************************
** 文件名:	ListViewCustom.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	ListItem的高度可变类型的
		
********************************************************************/

using UnityEngine;
using System.Collections;

namespace UIWidgets
{
	/// <summary>
	/// Interface for ListViewCustomHeight components.
	/// </summary>
	public interface IListViewItemHeight {
		/// <summary>
		/// Gets the height.
		/// </summary>
		/// <value>The height.</value>
		float Height {
			get;
		}

		float DefaultHeight {
			get;
		}
	}
}
