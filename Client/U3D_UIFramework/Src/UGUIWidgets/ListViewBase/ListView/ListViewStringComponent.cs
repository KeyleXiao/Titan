/*******************************************************************
** 文件名:	ListViewStringComponent.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	列表显示对象之纯文本类型
		    SetData()方法在显示对象数据发生改变时执行
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections;

namespace UIWidgets {

	/// <summary>
	/// List view item component.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/ListViewStringComponent")]
	public class ListViewStringComponent : ListViewItem
	{
		/// <summary>
		/// The Text component.
		/// </summary>
		public Text Text;

		public virtual void SetData(string text)
		{
			Text.text = text.Replace("\\n", "\n");
		}
	}
}
