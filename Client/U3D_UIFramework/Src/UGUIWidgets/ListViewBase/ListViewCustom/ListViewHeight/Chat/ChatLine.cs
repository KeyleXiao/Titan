/*******************************************************************
** 文件名:	ChatLine.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	ChatView DataSource类型，这里可以根据需求更改
		
********************************************************************/

using UnityEngine;
using System.Collections;
using System;
using UIWidgets;

namespace UIWidgetsSamples {

	/// <summary>
	/// Origin of chat line.
	/// </summary>
	public enum ChatLineType {
		System,
		User,
	}

	/// <summary>
	/// Chat line.
	/// </summary>
	[Serializable]
	public class ChatLine : IItemHeight {
		[SerializeField]
		public string UserName;
		[SerializeField]
		public string Message;
		[SerializeField]
		public DateTime Time;

		[SerializeField]
		public ChatLineType Type;

		public float Height {
			get;
			set;
		}
	}
}
