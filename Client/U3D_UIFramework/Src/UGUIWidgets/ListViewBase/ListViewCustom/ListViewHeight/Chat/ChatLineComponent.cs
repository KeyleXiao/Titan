/*******************************************************************
** 文件名:	ChatLineComponent.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	ChatView 显示对象挂接的组件，这里可以根据需求更改
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;

namespace UIWidgetsSamples {
	/// <summary>
	/// ChatLine component.
	/// </summary>
	public class ChatLineComponent : ListViewItem {
		// specify components for displaying item data
		[SerializeField]
		public Text UserName;
		
		[SerializeField]
		public Text Message;
		
		[SerializeField]
		public Text Time;

		/// <summary>
		/// Display ChatLine.
		/// </summary>
		/// <param name="item">Item.</param>
		public void SetData(ChatLine item)
		{
			UserName.text = item.UserName;
			Message.text = item.Message;
			Time.text = item.Time.ToString("[HH:mm:ss]");

			// change colors depend of origin
			if (item.Type==ChatLineType.System)
			{
				UserName.color = Color.red;
				Message.color = Color.red;
			}
			else
			{
				UserName.color = Color.white;
				Message.color = Color.white;
			}

			Utilites.UpdateLayout(GetComponent<LayoutGroup>());
		}
	}
}
