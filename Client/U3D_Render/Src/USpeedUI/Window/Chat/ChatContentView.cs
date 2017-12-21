/*******************************************************************
** 文件名:	ChatContentView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	李有红
** 日  期:	2017/5/19
** 版  本:	1.0
** 描  述:	聊听内容-富文本标签位置自适应逻辑
** 应  用:  
*****************************************************************/

using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using DataCenter;

namespace USpeedUI.Chat
{


	public struct SChatMessageObjectInfo
	{
		public ChatItemType type;                         // 对象类型
		public ChatItemSubType subType;                   // 对象子类型
		public String text;                             // 对象正文
		public Dictionary<String, String> param;        // 参数列表
	}

	public struct SChatMessageInfo
	{
		static public int seqGeneID = 0;
		public int sequenceID;                          // 流水号
		public EMChatChannelType channel;               // 频道
		public String strMessage;                       // 消息原文
		public String senderName;                       // 发送者名字
		public uint senderPdbid;                        // 发送者ID
		public uint senderUdbid;                        // 发送者账号ID
		public int senderKinID;                         // 发送者战队ID
		public String targetName;                       // 接收者名字，用于私聊
		public uint targetPdbid;                        // 接受者ID，用于私聊
		public List<SChatMessageObjectInfo> objList;    // 解析后的对象列表
	}

	public struct ChatContentItemData
	{
		public List<SChatMessageObjectInfo> objList;    // 解析后的对象列表
		public float Height;
	}

	public class ChatContentItem : MonoBehaviour
    {
        private struct LineObjCache
        {
            public float width;
            public float height;
            public RectTransform trans;
        }

        private float m_curWidth = 0f;
        private float m_Height = 0f;

        private List<IChatItem> currChatBoxItem = new List<IChatItem>();
        public float Height
        {
            get
            {
                return m_Height;
            }
        }

        public void SetData(ChatContentItemData data)
        {
            RectTransform selfTrans = this.transform as RectTransform;
            m_curWidth = selfTrans.rect.width;

            // 删除现有子对象
            for (int i = 0; i < currChatBoxItem.Count; i++)
            {
                IChatItem itm =  currChatBoxItem[i];
				itm.Destroy();
				//itm.Clear();
                GameObject temp = itm.GetTransform().gameObject;
                ResNode.DestroyRes(ref temp);
            }

            currChatBoxItem.Clear();
            float width = selfTrans.rect.width;
            float offsetX = 0f;
            float offsetY = 0f;

			// TODO: 如果使用了自动布局,可以不计算位置
            List<LineObjCache> curLineObject = new List<LineObjCache>();
            if (data.objList != null)
            {
                foreach (var obj in data.objList)
                {
                    CreateSubItem(obj, width, ref offsetX, ref offsetY, curLineObject);
                }
            }
            AlignLineObject(ref offsetX, ref offsetY, curLineObject);

            data.Height = -offsetY;
            selfTrans.sizeDelta = new Vector2(selfTrans.sizeDelta.x, data.Height);

            m_Height = data.Height;
         
        }

        private void CreateSubItem(SChatMessageObjectInfo info, float width, ref float curOffsetX, ref float curOffsetY, List<LineObjCache> curLineObject)
        {
            if (width < 0f || curLineObject == null)
                return;

                ResNode itemPrefab = LogicDataCenter.chatMessageManager.GetChatItemPrefabByObjType(info.type);
                if (itemPrefab == null)
                    return;

                GameObject itemObj = itemPrefab.InstanceMainRes();
                if (itemObj == null)
                    return;

                IChatItem subItem = itemObj.GetComponent<IChatItem>();
                if (subItem == null)
                {
                    itemPrefab.DestroyInstanceRes(ref itemObj);
                    return;
                }
                subItem.SetResNode(itemPrefab);
                currChatBoxItem.Add(subItem);
                itemObj.transform.SetParent(this.transform, false);


            if (subItem.SetItemData(info) == false)
                return;

            for (int i = 0; i < 100; ++i)
            {
                IChatItem nextItem;
                subItem.SplitByWidth(width, curOffsetX, out subItem, out nextItem);
                if (subItem != null)
                {
                    curOffsetX += subItem.GetItemWidth();
                    AddLineObject(subItem, curLineObject);
                }

                if (nextItem == null)
                    break;

                AlignLineObject(ref curOffsetX, ref curOffsetY, curLineObject);
                subItem = nextItem;
				currChatBoxItem.Add(subItem);
			}
		}

        private void AddLineObject(IChatItem item, List<LineObjCache> curLineObject)
        {
            LineObjCache objCache;
            objCache.width = item.GetItemWidth();
            objCache.height = item.GetItemHeight();
            objCache.trans = item.GetTransform();
            objCache.trans.sizeDelta = new Vector2(objCache.width, objCache.height);
            curLineObject.Add(objCache);
        }

        private void AlignLineObject(ref float curOffsetX, ref float curOffsetY, List<LineObjCache> curLineObject)
        {
            float maxHeight = 0f;
            for (int i = 0; i < curLineObject.Count; ++i)
            {
                maxHeight = Mathf.Max(maxHeight, curLineObject[i].height);
            }

            const float yMargin = 2f;
            curOffsetX = 0f;
            curOffsetY -= (yMargin + maxHeight);
            float offX = 0f;
            for (int i = 0; i < curLineObject.Count; ++i)
            {
                curLineObject[i].trans.anchoredPosition = new Vector2(offX, curOffsetY);
                offX += curLineObject[i].width;
            }

            curLineObject.Clear();
        }
    }
}
