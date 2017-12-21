/*******************************************************************
** 文件名:	DialogInfoBase.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-26
** 版  本:	1.8.0
** 描  述:	消息框对象 显示的基础信息
		
********************************************************************/

using UnityEngine;

namespace UIWidgets {
	/// <summary>
	/// DialogInfoBase.
	/// </summary>
	abstract public class DialogInfoBase : MonoBehaviour {

		/// <summary>
		/// Sets the info.
		/// </summary>
		/// <param name="title">Title.</param>
		/// <param name="message">Message.</param>
		/// <param name="icon">Icon.</param>
		public abstract void SetInfo(string title, string message, Sprite icon);
	}
}
