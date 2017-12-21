/*******************************************************************
** 文件名:	DialogButtonComponent.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-26
** 版  本:	1.8.0
** 描  述:	消息框对象 显示按钮类型
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;

namespace UIWidgets {
	/// <summary>
	/// DialogButtonComponent.
	/// Control how button name will be displayed.
	/// </summary>
	public class DialogButtonComponent : MonoBehaviour {
		/// <summary>
		/// The name.
		/// </summary>
		[SerializeField]
		public Text Name;

		/// <summary>
		/// Sets the button name.
		/// </summary>
		/// <param name="name">Name.</param>
		public virtual void SetButtonName(string name)
		{
			Name.text = name;
		}
	}
}
