using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using DataCenter;
using USpeedUI.Chat;

namespace USpeedUI.ChatBox
{
	#region 聊天框聊天信息:ChatBoxContent
	[Serializable]
	public class ChatBoxContentItemData : IItemHeight
	{
		public SChatMessageInfo info;

		public float Height { get; set; }
	}

	public class ChatBoxContentItem : ListViewItem, IListViewItemHeight
	{
		private int m_curSequenceID = -1;
		private float m_curWidth = 0f;
		private float m_Height = 0f;

		public float Height
		{
			get
			{
				return m_Height;
			}
		}

		// 用于计算maxVisibleItems,节省性能
		public float DefaultHeight
		{
			get
			{
				return 12f;
			}
		}

		public void SetData(ChatBoxContentItemData data)
		{
			// 重复SetData的问题先这么写，以后再看
			RectTransform selfTrans = this.RectTransform;
			if (m_curSequenceID == data.info.sequenceID && m_curWidth == selfTrans.rect.width)
				return;
			m_curSequenceID = data.info.sequenceID;
			m_curWidth = selfTrans.rect.width;

			ChatContentItem item = gameObject.GetComponent<ChatContentItem>();
			if(item != null)
			{
				ChatContentItemData itemData = new ChatContentItemData();
				itemData.objList = data.info.objList;
				item.SetData(itemData);

				m_Height = item.Height;
			}
			else
			{
				Debug.LogError("item == null, This GameObject have no ChatContentItem component.");
			}
		}
	}

	public class ChatBoxContentView : ListViewCustomHeight<ChatBoxContentItem, ChatBoxContentItemData>
	{
		public override void Start()
        {
            base.Start();

            DestroyGameObjects = true;
            ForceAutoHeightCalculation = false;
        }
		
		// Sets component data with specified item.
		protected override void SetData(ChatBoxContentItem item, ChatBoxContentItemData data)
		{
			item.SetData(data);
        }

		// leave coloring functions empty
		protected override void HighlightColoring(ChatBoxContentItem component)
		{
		}

		protected override void SelectColoring(ChatBoxContentItem component)
		{
		}

		protected override void DefaultColoring(ChatBoxContentItem component)
		{
		}

		/// <summary>
		/// 移除缓存的第一个元素，以保证缓存列表与新的消息列表保持一致
		/// </summary>
		public void removeFirstComponentsItem()
		{
			if (components.Count > 0)
			{
				components.RemoveAt(0);
			}
		}
	}

	#endregion
}
