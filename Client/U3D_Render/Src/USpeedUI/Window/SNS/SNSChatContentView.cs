/*******************************************************************
** 文件名:	SNSChatContentView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/12
** 版  本:	1.0
** 描  述:	SNS聊听内容
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using System.Collections;
using USpeedUI.Chat;

namespace USpeedUI.SNS
{
	[Serializable]
    public class SNSMsgContentItem : IItemHeight
    {
		public DataCenter.SNSDataManager.SSNSChatMessageInfo info;

        public float Height { get; set; }
    }

    public class SNSMsgContentItemComponent : ListViewItem, IListViewItemHeight
	{
		public Transform leftTrans;
		public Transform rightTrans;
        public float MinHeight;
        public float MaxWidth;
        public float TimeHeight;
        public float MarginY;

        private int m_nMsgID = 0;
		private float m_Height = 0f;

		public float Height
		{
			get
			{
				return m_Height;
			}
		}

		public float DefaultHeight
		{
			get
			{
				return 0f;
			}
		}

		public void SetData(SNSMsgContentItem data)
        {
            m_nMsgID = data.info.msg.dwMsgID;

			Transform curTrans = null;
			if (isSelf(data.info.msg.dwFromAccountID))
			{
				curTrans = rightTrans;
				leftTrans.gameObject.SetActive(false);
				rightTrans.gameObject.SetActive(true);
			}
			else
			{
				curTrans = leftTrans;
				leftTrans.gameObject.SetActive(true);
				rightTrans.gameObject.SetActive(false);
			}

			Transform timeTrans = curTrans.FindChild("Time");
			if (timeTrans == null)
				return;

			Transform bubbleTrans = curTrans.FindChild("Bubble");
			if (bubbleTrans == null)
				return;
			
			Transform contentTrans = curTrans.FindChild("Bubble/Content");
			if (contentTrans == null)
				return;

			timeTrans.GetComponent<Text>().text = Api.convertTimeStamp2DateTime(data.info.msg.dwTime).ToString("yyyy-MM-dd HH:mm");

			// 设置content的最大宽度
			RectTransform contentRect = contentTrans as RectTransform;
			contentRect.sizeDelta = new Vector2(MaxWidth, contentRect.sizeDelta.y);
			//contentRect.anchoredPosition = new Vector2(contentRect.anchoredPosition.x, 0);

			float contentHeight = 0;
			ChatContentItem item = contentTrans.GetComponent<ChatContentItem>();
			if (item != null)
			{
				ChatContentItemData itemData = new ChatContentItemData();
				itemData.objList = data.info.objList;
				item.SetData(itemData);

				contentHeight = item.Height;
			}
			else
			{
				Debug.LogError("item == null, This GameObject have no ChatContentItem component.");
			}

			// 设置内容的大小
			float contentWidth = contentRect.sizeDelta.x;
			contentRect.sizeDelta = new Vector2(contentWidth, contentHeight);

			// 气泡与内容之间的边缘大小
			Vector2 Margin;
			Margin.x = Math.Abs(contentRect.anchoredPosition.x);
			Margin.y = Math.Abs(contentRect.anchoredPosition.y);

			// 计算气泡的大小
			RectTransform bubbleRect = bubbleTrans as RectTransform;
			bubbleRect.sizeDelta = new Vector2(contentWidth + Margin.x * 2, contentHeight + Margin.y * 2);
			float bubbleHeight = bubbleRect.sizeDelta.y;


			// 设置时间的坐标(时间显示在聊天内容下面),坐标为负数
			float timePosY = bubbleRect.anchoredPosition.y + (-bubbleHeight);
			RectTransform timeRect = timeTrans as RectTransform;
			timeRect.anchoredPosition = new Vector2(timeRect.anchoredPosition.x, timePosY);

			// 计算总高度
			m_Height = Math.Abs(bubbleRect.anchoredPosition.y) + bubbleHeight + TimeHeight + MarginY;
			if (m_Height < MinHeight)
			{
				m_Height = MinHeight;
			}

			// 设置总高度
			RectTransform rect2 = curTrans as RectTransform;
			rect2.sizeDelta = new Vector2(rect2.sizeDelta.x, m_Height);

			// 设置DefautItem高度
			RectTransform rect3 = transform as RectTransform;
			rect3.sizeDelta = new Vector2(rect3.sizeDelta.x, m_Height);

            m_Height = 0;
		}

        private bool isSelf(int dwAccount)
        {
            // 是否是自己发的消息
            return LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID == dwAccount; 
        }
    }

    public class SNSChatContentView : ListViewCustomHeight<SNSMsgContentItemComponent, SNSMsgContentItem>
    {
        // Sets component data with specified item.
        protected override void SetData(SNSMsgContentItemComponent component, SNSMsgContentItem item)
        {
            component.SetData(item);
        }

        // leave coloring functions empty
        protected override void HighlightColoring(SNSMsgContentItemComponent component)
        {
        }

        protected override void SelectColoring(SNSMsgContentItemComponent component)
        {
        }

        protected override void DefaultColoring(SNSMsgContentItemComponent component)
        {
        }
    }
}
