/*******************************************************************
** 文件名:	Tab.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	Tab 显示对象数据基类
		
********************************************************************/

using UnityEngine;
using System;

namespace UIWidgets
{	
	/// <summary>
	/// Tab.
	/// </summary>
	[Serializable]
	public class Tab
	{
		/// <summary>
		/// Text of button for this tab.
		/// </summary>
		public string Name;
		
		/// <summary>
		/// The tab object.
		/// </summary>
		public GameObject TabObject;
	}
}
