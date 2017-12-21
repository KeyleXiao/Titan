/*******************************************************************
** 文件名:	UBuddySearchListView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/4/11
** 版  本:	1.0
** 描  述:	好友系统主面板搜索好友结果列表
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections.Generic;
using DataCenter;
using UnityEngine.EventSystems;
using USpeedUI.UWidgets;
using USpeedUI;

namespace UIWidgets {
	public class UBuddySearchListItemComponent : ListViewItem, IResizableItem {
		/// <summary>
		/// Gets the objects to resize.
		/// </summary>
		/// <value>The objects to resize.</value>
		public GameObject[] ObjectsToResize {
			get {
				return new GameObject[] {Icon.transform.parent.gameObject, Text.gameObject};
			}
		}

		/// <summary>
		/// The icon.
		/// </summary>
		[SerializeField]
		public Image Icon;

		/// <summary>
		/// The text.
		/// </summary>
		[SerializeField]
		public Text Text;

		/// <summary>
		/// Set icon native size.
		/// </summary>
		public bool SetNativeSize = true;

		UBuddySearchListItem item;

		/// <summary>
		/// Gets the current item.
		/// </summary>
		/// <value>Current item.</value>
		public UBuddySearchListItem Item {
			get {
				return item;
			}
		}
		
		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="newItem">Item.</param>
		public virtual void SetData(UBuddySearchListItem newItem)
		{
			item = newItem;
			if (item==null)
			{
				if (Icon!=null)
				{
					Icon.sprite = null;
				}
				Text.text = string.Empty;
			}
			else
			{
				if (Icon!=null)
				{
					Icon.sprite = item.Icon;
				}

				Text.text = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(item.UserID).Info.szName;
			}

			if ((SetNativeSize) && (Icon!=null))
			{
				Icon.SetNativeSize();
			}

			//set transparent color if no icon
			if (Icon!=null)
			{
				Icon.color = (Icon.sprite==null) ? Color.clear : Color.white;
			}
		}

		/// <summary>
		/// Called when item moved to cache, you can use it free used resources.
		/// </summary>
		public override void MovedToCache()
		{
			if (Icon!=null)
			{
				Icon.sprite = null;
			}
		}

		public override void OnPointerClick(PointerEventData eventData)
		{
			base.OnPointerClick(eventData);

			SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
			msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
			msgData.SessionID = item.UserID;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);


			// 隐藏结果窗口
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_HIDE_SEARCH_BUDDY_WND, null);
		}
	}
}