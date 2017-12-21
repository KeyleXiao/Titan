/*******************************************************************
** 文件名:	SNSChatHistoryView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/14
** 版  本:	1.0
** 描  述:	SNS聊听历史记录
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using DataCenter;
using USpeedUI.Chat;

namespace USpeedUI.SNS
{
    [Serializable]
    public class SNSHistroyMsgItem : IItemHeight
    {
        public int nMsgID;
        public int nFromAcountID;
        public int nToAcountID;
        public string szContent;
        public string szName;
        public int nTime;

        public List<SChatMessageObjectInfo> objList;    // 解析后的对象列表

        public float Height { get; set; }
    }

    public class SNSHistroyMsgItemComponent : ListViewItem
    {
        public float MaxWidth;
        public float MarginY;
        public float MinHeight;

        private int m_nMsgID = 0;
        private float m_Height = 0;

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

        public void SetData(SNSHistroyMsgItem data)
        {
            m_nMsgID = data.nMsgID;

            string name = data.szName;
            string time = Api.convertTimeStamp2DateTime(data.nTime).ToString("yyyy-MM-dd HH:mm");

			string content = data.szContent;

            Transform nameTrans = transform.FindChild("Name");
            if (nameTrans == null)
                return;

            Transform timeTrans = transform.FindChild("Time");
            if (timeTrans == null)
                return;

            Transform contentTrans = transform.FindChild("Content");
            if (contentTrans == null)
                return;

            if (isSelf(data.nFromAcountID))
            {
                name = "<color=#40A3FEFF> " + name + "</color>";
                time = "<color=#40A3FEFF> " + time + "</color>";
            }

            nameTrans.GetComponent<Text>().text = name;
            timeTrans.GetComponent<Text>().text = time;
            //transform.FindChild("MsgContent").GetComponent<Text>().text = content;

            // 设置content的最大宽度
            RectTransform contentRect = contentTrans as RectTransform;
            contentRect.sizeDelta = new Vector2(MaxWidth, contentRect.sizeDelta.y);

            // 设置内容并获取高度
            float contentHeight = 0;
            ChatContentItem item = contentTrans.GetComponent<ChatContentItem>();
            if (item != null)
            {
                ChatContentItemData itemData = new ChatContentItemData();
                itemData.objList = data.objList;
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

            // 计算总高度
            m_Height = Math.Abs(contentRect.anchoredPosition.x) + contentHeight + MarginY;
            if (m_Height < MinHeight)
            {
                m_Height = MinHeight;
            }

            // 设置DefautItem高度
            RectTransform rect3 = transform as RectTransform;
            rect3.sizeDelta = new Vector2(rect3.sizeDelta.x, m_Height);
        }

        private bool isSelf(int dwAccount)
        {
            // 是否是自己发的消息
            return LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID == dwAccount; 
        }
    }

    public class SNSChatHistoryView : ListViewCustomHeight<SNSHistroyMsgItemComponent, SNSHistroyMsgItem>
    {
        // Sets component data with specified item.
        protected override void SetData(SNSHistroyMsgItemComponent component, SNSHistroyMsgItem item)
        {
            component.SetData(item);
        }

        // leave coloring functions empty
        protected override void HighlightColoring(SNSHistroyMsgItemComponent component)
        {
        }

        protected override void SelectColoring(SNSHistroyMsgItemComponent component)
        {
        }

        protected override void DefaultColoring(SNSHistroyMsgItemComponent component)
        {
        }
    }
}
