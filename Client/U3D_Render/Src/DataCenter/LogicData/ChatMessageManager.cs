using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;
using USpeedUI.ChatBox;
using UnityEngine.UI;
using Data.ChatEmoticonConfig;
using ASpeedGame.Data.ShieldWorld;
using USpeedUI.UWidgets;
using USpeedUI.Chat;

namespace DataCenter
{
	public class ChatMessageManager
	{
        enum ChatMessageShowPos
        {
            CMS_ChatBox = 1,        // 聊天框
            CMS_Bubble = 2,         // 气泡

            CMS_ALL = 0x7FFFFFFF,   // 所有位置
        }

		private LinkedList<SChatMessageInfo> m_messageList;
		private List<SChatMessageInfo> m_messageShowList;
		public List<SChatMessageInfo> MessageList { get { return m_messageShowList; } }

		public bool MsgIsOverLoad { get; set; }     // 聊听消息缓存是否超过上限

		// 聊天显示物件
		private ResNode m_chatItemTextRes;           // 文本物件
		private ResNode m_chatItemImageRes;        // 表情物件

		// 表示聊天插入物件起始点的特殊字符
		public const char ChatItemFlag = (char)21;

		// 聊天频道掩码
		private int m_nChannelMask;

		// 消息缓冲队列
		private Queue<SChatMessageInfo> m_CacheMsgInfoQueue = new Queue<SChatMessageInfo>();

		// 客户端最多缓存聊天消息数
		static private int MAX_CHAT_CACHE_NUM = 100;

		// 最后一次发私聊信息的玩家
		private string m_LastPrivateChatName = "";
		public string LastPrivateChatName { set { m_LastPrivateChatName = value; } get { return m_LastPrivateChatName; } }

		public void Init()
		{
			m_messageList = new LinkedList<SChatMessageInfo>();
			m_messageShowList = new List<SChatMessageInfo>();
			m_nChannelMask = ~0;
			m_chatItemTextRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/Chat/ChatItemText");
			m_chatItemImageRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/Chat/ChatItemImage");

			MsgIsOverLoad = false;

		}

		public void Clear()
		{
			AssetBundleManager.DeleteAssets(ref m_chatItemTextRes, true);
			AssetBundleManager.DeleteAssets(ref m_chatItemImageRes, true);
		}

		public void AddPrivateChatMessage(ref gamelogic_show_private_chat_message msgInfo)
		{
			if (msgInfo.message == null || msgInfo.channel < 0 || msgInfo.channel >= (int)EMChatChannelType.CHAT_CHANNEL_MAX)
				return;

			if (msgInfo.channel != (int)EMChatChannelType.CHAT_CHANNEL_PRIVATE)
			{
				return;
			}

			// 不显示黑名单中玩家的信息
			if (isBlackList((int)msgInfo.senderUdbid))
			{
				return;
			}

			SChatMessageInfo info;
			info.sequenceID = ++SChatMessageInfo.seqGeneID;
			info.strMessage = msgInfo.message;
			info.channel = (EMChatChannelType)msgInfo.channel;
			info.senderName = msgInfo.senderName;
			info.senderPdbid = msgInfo.senderPdbid;
			info.senderUdbid = msgInfo.senderUdbid;
			info.senderKinID = msgInfo.senderKinID;
			info.targetName = msgInfo.targetName;
			info.targetPdbid = msgInfo.targetPdbid;

			// 分割解析聊天对象
			info.objList = new List<SChatMessageObjectInfo>();

			// 加入前缀
			String channelName;
			String normalColor;
			channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "PrivateChannelName");
			normalColor = CreateColorParam(171, 0, 214);

			if (channelName != null && channelName.Length > 0)
			{
				SChatMessageObjectInfo channelObj;
				channelObj.type = ChatItemType.ObjType_Text;
				channelObj.subType = ChatItemSubType.ObjSubType_Text_Channel;
				channelObj.text = "[" + channelName + "]";
				channelObj.param = new Dictionary<String, String>();
				channelObj.param.Add("color", normalColor);
				channelObj.param.Add("type", "" + (int)info.channel);
				info.objList.Add(channelObj);
			}

			// 自己是发送者
			int nSelfActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
			if (nSelfActorID == info.senderPdbid && info.targetName != null && info.targetName.Length > 0)
			{
				SChatMessageObjectInfo splitObjInfo;
				splitObjInfo.type = ChatItemType.ObjType_Text;
				splitObjInfo.subType = ChatItemSubType.ObjSubType_Text_Text;
				splitObjInfo.text = "你对 ";
				splitObjInfo.param = null;
				info.objList.Add(splitObjInfo);

				SChatMessageObjectInfo nameObjInfo;
				if (info.targetPdbid > 0)
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Actor;
					nameObjInfo.param = new Dictionary<String, String>();
					nameObjInfo.param.Add("color", CreateColorParam(255, 255, 127));
					nameObjInfo.param.Add("id", "" + info.targetPdbid);
					nameObjInfo.param.Add("userID", "" + info.senderUdbid);
					nameObjInfo.param.Add("kinID", "" + info.senderKinID);
				}
				else
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Text;
					nameObjInfo.param = null;
				}
				nameObjInfo.text = info.targetName;
				info.objList.Add(nameObjInfo);

				SChatMessageObjectInfo splitObjInfo2;
				splitObjInfo2.type = ChatItemType.ObjType_Text;
				splitObjInfo2.subType = ChatItemSubType.ObjSubType_Text_Text;
				splitObjInfo2.text = " 说：";
				splitObjInfo2.param = null;
				info.objList.Add(splitObjInfo2);
			}
			else if (nSelfActorID == info.targetPdbid && info.senderName != null && info.senderName.Length > 0)   // 自己是接收者
			{
				SChatMessageObjectInfo nameObjInfo;
				if (info.senderPdbid > 0)
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Actor;
					nameObjInfo.param = new Dictionary<String, String>();
					nameObjInfo.param.Add("color", CreateColorParam(255, 255, 127));
					nameObjInfo.param.Add("id", "" + info.senderPdbid);
					nameObjInfo.param.Add("userID", "" + info.senderUdbid);
					nameObjInfo.param.Add("kinID", "" + info.senderKinID);
				}
				else
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Text;
					nameObjInfo.param = null;
				}
				nameObjInfo.text = info.senderName;
				info.objList.Add(nameObjInfo);

				SChatMessageObjectInfo splitObjInfo2;
				splitObjInfo2.type = ChatItemType.ObjType_Text;
				splitObjInfo2.subType = ChatItemSubType.ObjSubType_Text_Text;
				splitObjInfo2.text = " 对你说：";
				splitObjInfo2.param = null;
				info.objList.Add(splitObjInfo2);
			}

			// 加入聊天内容
			addChatContent(ref info, normalColor);

			m_CacheMsgInfoQueue.Enqueue(info);
		}

		// 添加一条聊天内容
		public void AddChatMessage(ref gamelogic_show_chat_message msgInfo)
		{
			if (msgInfo.message == null || msgInfo.channel < 0 || msgInfo.channel >= (int)EMChatChannelType.CHAT_CHANNEL_MAX)
				return;

			// 不显示黑名单中玩家的信息
			if (isBlackList((int)msgInfo.senderUdbid))
			{
				return;
			}



			SChatMessageInfo info;
			info.sequenceID = ++SChatMessageInfo.seqGeneID;
			info.strMessage = msgInfo.message;
			info.channel = (EMChatChannelType)msgInfo.channel;
			info.senderName = msgInfo.senderName;
			info.senderPdbid = msgInfo.senderPdbid;
			info.senderUdbid = msgInfo.senderUdbid;
			info.senderKinID = msgInfo.senderKinID;
			info.targetPdbid = 0;
			info.targetName = "";

			// 分割解析聊天对象
			info.objList = new List<SChatMessageObjectInfo>();

			// 加入前缀
			String channelName;
			String normalColor;
			switch (info.channel)
			{
				case EMChatChannelType.CHAT_CHANNEL_SYSTEM:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "SystemChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_WORLD:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "WorldChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_MATCHROOM:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "MatchroomChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_WARSCENE:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "WarSceneChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_TEAMMATE:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "TeammateChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_PRIVATE:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "PrivateChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_KIN:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "KinChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_CLAN:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "ClanChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_NEARBY:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "NearbyChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_ZONE:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "ZoneChannelName");
					break;
				case EMChatChannelType.CHAT_CHANNEL_CAMP:
					channelName = ULocalizationService.Instance.Get("UIView", "ChatBox", "CampChannelName");
					break;
				default:
					channelName = "";
					break;
			}

			normalColor = CreateColorParam(UDefines.ChatChannelColor((int)info.channel));
			if (channelName != null && channelName.Length > 0)
			{
				SChatMessageObjectInfo channelObj;
				channelObj.type = ChatItemType.ObjType_Text;
				channelObj.subType = ChatItemSubType.ObjSubType_Text_Channel;
				channelObj.text = "[" + channelName + "]";
				channelObj.param = new Dictionary<String, String>();
				channelObj.param.Add("color", normalColor);
				channelObj.param.Add("type", "" + (int)info.channel);
				info.objList.Add(channelObj);
			}

			if (info.senderName != null && info.senderName.Length > 0)
			{
				SChatMessageObjectInfo nameObjInfo;
				if (info.senderPdbid > 0)
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Actor;
					nameObjInfo.param = new Dictionary<String, String>();
					nameObjInfo.param.Add("color", CreateColorParam(255, 255, 127));
					nameObjInfo.param.Add("id", "" + info.senderPdbid);
					nameObjInfo.param.Add("userID", "" + info.senderUdbid);
					nameObjInfo.param.Add("kinID", "" + info.senderKinID);
				}
				else
				{
					nameObjInfo.type = ChatItemType.ObjType_Text;
					nameObjInfo.subType = ChatItemSubType.ObjSubType_Text_Text;
					nameObjInfo.param = null;
				}
				nameObjInfo.text = info.senderName;
				info.objList.Add(nameObjInfo);

				SChatMessageObjectInfo splitObjInfo;
				splitObjInfo.type = ChatItemType.ObjType_Text;
				splitObjInfo.subType = ChatItemSubType.ObjSubType_Text_Text;
				splitObjInfo.text = ": ";
				splitObjInfo.param = null;
				info.objList.Add(splitObjInfo);
			}

			// 加入聊天内容
			addChatContent(ref info, normalColor);

            // 默认显示在聊天框
            int showPos = (int)ChatMessageShowPos.CMS_ChatBox;

            // 大表情不显示在聊天框
            if(isHasLargeEmotion(info))
            {
                showPos &= ~(int)ChatMessageShowPos.CMS_ChatBox;
            }

            // 玩家发送的消息才显示聊天气泡
            if (msgInfo.senderUID > 0 || msgInfo.senderPdbid > 0)
            {
                showPos |= (int)ChatMessageShowPos.CMS_Bubble;
            }

            if((showPos & (int)ChatMessageShowPos.CMS_ChatBox) > 0)
            {
                m_CacheMsgInfoQueue.Enqueue(info);
            }
            if((showPos & (int)ChatMessageShowPos.CMS_Bubble) > 0)
            {
                showChatBubble(msgInfo);
            }
         
        }

		public void ShowOneChatMessage()
		{
			if (m_CacheMsgInfoQueue.Count <= 0)
			{
				return;
			}

			SChatMessageInfo msgInfo = m_CacheMsgInfoQueue.Dequeue();

			// 聊天对象放入列表
			m_messageList.AddLast(msgInfo);
			if (m_messageList.Count > MAX_CHAT_CACHE_NUM)
			{
				m_messageList.RemoveFirst();
				MsgIsOverLoad = true;
			}
			FillMessageShowList();
		}

		private static bool GetOneChatParam(String message, ref int curIndex, out String typeName, out Dictionary<String, String> param, out String content)
		{
			typeName = "";
			param = null;
			content = "";
			if (message[curIndex] != '[')
				return false;

			int endIndex = message.IndexOf(']', curIndex);
			if (endIndex < 0)
				return false;

			String subStr = message.Substring(curIndex + 1, endIndex - curIndex - 1);
			int paramIndex = subStr.IndexOf(' ');
			if (paramIndex < 0)
			{
				typeName = subStr;
			}
			else if (paramIndex == 0)
			{
				return false;
			}
			else
			{
				typeName = subStr.Substring(0, paramIndex);
				param = new Dictionary<String, String>();
				for (int i = 0; i < 100; ++i)
				{
					int startIndex = paramIndex + 1;
					paramIndex = subStr.IndexOf(' ', startIndex);
					String strSingleParam;
					if (paramIndex < 0)
					{
						strSingleParam = subStr.Substring(startIndex);
					}
					else
					{
						strSingleParam = subStr.Substring(startIndex, paramIndex - startIndex);
					}

					int equalIndex = strSingleParam.IndexOf('=');
					if (equalIndex < 0)
						return false;

					String key = strSingleParam.Substring(0, equalIndex);
					String val = strSingleParam.Substring(equalIndex + 1);
					if (!param.ContainsKey(key))
						param.Add(key, val);

					if (paramIndex < 0)
						break;
				}
			}

			int endContentIndex = message.IndexOf("[/" + typeName + "]", endIndex);
			if (endContentIndex < 0)
				return false;

			if (endContentIndex - endIndex - 1 > 0)
				content = message.Substring(endIndex + 1, endContentIndex - endIndex - 1);
			curIndex = endContentIndex + typeName.Length + 3;

			return true;
		}

		private void FillMessageShowList()
		{
			m_messageShowList.Clear();
			foreach (var info in m_messageList)
			{
				if ((m_nChannelMask & (1 << (int)info.channel)) > 0)
					m_messageShowList.Add(info);
			}

			// 通知UI
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_UPDATEMESSAGE, null);
		}

		public void SetChannelMask(int mask)
		{
			if (m_nChannelMask == mask)
				return;

			m_nChannelMask = mask;
			FillMessageShowList();

			// 发送到服务器
			gameview_chat_set_channelmask msgInfo = new gameview_chat_set_channelmask();
			msgInfo.nChannelMask = (int)mask;
			ViewEventHelper.Instance.SendCommand<gameview_chat_set_channelmask>(GameLogicDef.GVIEWCMD_CHAT_SET_CHANNELMASK, ref msgInfo);
		}

		public String CreateColorParam(int r, int g, int b)
		{
			return String.Format("{0}+{1}+{2}", r, g, b);
		}

		public String CreateColorParam(Vector3 v)
		{
			return String.Format("{0}+{1}+{2}", (int)v.x, (int)v.y, (int)v.z);
		}

		public String CreateColorParam(Color c)
		{
			return String.Format("{0}+{1}+{2}", (int)c.r * 250, (int)c.g * 250, (int)c.b * 250);
		}

		public void ParseColorParam(String param, out int r, out int g, out int b)
		{
			r = 0;
			g = 0;
			b = 0;
			if (param == null)
				return;

			String[] single = param.Split('+');
			if (single.Length != 3)
				return;

			r = Convert.ToInt32(single[0]);
			g = Convert.ToInt32(single[1]);
			b = Convert.ToInt32(single[2]);
		}

		public Color ParseColorParam(Vector3 v)
		{
			return new Color(v.x / 255f, v.y / 255f, v.z / 255f);
		}

		public ResNode GetChatItemPrefabByObjType(ChatItemType type)
		{
			switch (type)
			{
				case ChatItemType.ObjType_Text:
					return m_chatItemTextRes;
				case ChatItemType.ObjType_Image:
					return m_chatItemImageRes;
				default:
					break;
			}

			Debug.LogWarning("ChatMessageManager GetChatItemPrefabByObjType unknown infotype" + type);
			return null;
		}



		/// <summary>
		/// 清空频道聊听消息
		/// </summary>
		/// <param name="channel">频道类型</param>
		public void RemoveChannelChatMessage(EMChatChannelType channel)
		{
			var msg = m_messageList.First;
			while (msg != null)
			{
				var nextMsg = msg.Next;
				if (msg.Value.channel == channel)
				{
					m_messageList.Remove(msg);
				}
				msg = nextMsg;
			}
		}

		public void clearAllChatMsgData()
		{
			m_messageList.Clear();
			m_messageShowList.Clear();
            m_CacheMsgInfoQueue.Clear();
        }

		/// <summary>
		/// 解析聊天内容-富文本
		/// </summary>
		/// <param name="strMessage">要解析的聊天内容</param>
		/// <param name="color">默认颜色</param>
		/// <returns></returns>
		public List<SChatMessageObjectInfo> parseChatContent(string strMessage, Color color)
		{
			string normalColor = CreateColorParam(color);

			return parseChatContent(strMessage, normalColor);
		}

		/// <summary>
		/// 解析聊天内容-富文本
		/// </summary>
		/// <param name="strMessage">要解析的聊天内容</param>
		/// <param name="normalColor">默认颜色,字符串格式("255+255+255"）</param>
		/// <returns></returns>
		public static List<SChatMessageObjectInfo> parseChatContent(string strMessage, string normalColor, bool bShieldWord = true)
		{
            if (strMessage == null)
                return new List<SChatMessageObjectInfo>();

			List<SChatMessageObjectInfo> objList = new List<SChatMessageObjectInfo>();

			int curIndex = 0;
			while (true)
			{
				if (curIndex >= strMessage.Length)
					break;

				int beginIndex = strMessage.IndexOf(ChatItemFlag, curIndex);
				if (beginIndex < 0)
				{
					string text = strMessage.Substring(curIndex);

					// 处理屏蔽词
                    if(bShieldWord)
                    {
					    ShieldWord.Instance.filterShieldWord(ref text);
                    }

					SChatMessageObjectInfo txtObj;
					txtObj.type = ChatItemType.ObjType_Text;
					txtObj.subType = ChatItemSubType.ObjSubType_Text_Text;
					txtObj.text = text;
					txtObj.param = new Dictionary<String, String>();
					txtObj.param.Add("color", normalColor);
					objList.Add(txtObj);
					break;
				}
				else
				{
					if (beginIndex > curIndex)
					{
						SChatMessageObjectInfo txtObj;
						txtObj.type = ChatItemType.ObjType_Text;
						txtObj.subType = ChatItemSubType.ObjSubType_Text_Text;
						txtObj.text = strMessage.Substring(curIndex, beginIndex - curIndex);
						txtObj.param = new Dictionary<String, String>();
						txtObj.param.Add("color", normalColor);
						objList.Add(txtObj);
					}

					curIndex = beginIndex + 1;
					String typeName;
					Dictionary<String, String> param;
					String content;
					if (GetOneChatParam(strMessage, ref curIndex, out typeName, out param, out content) == false)
						continue;

					ChatItemType objType;
					ChatItemSubType subType;
					ChatItem.GetChatObjStringSubType(typeName, true, out objType, out subType);
					if (objType != ChatItemType.ObjType_Null)
					{
						SChatMessageObjectInfo msgObj;
						msgObj.type = objType;
						msgObj.subType = subType;
						msgObj.text = content;
						msgObj.param = param;
						objList.Add(msgObj);
					}
				}
			}

			return objList;
		}

		/// <summary>
		/// 加入聊听内容
		/// </summary>
		/// <param name="info">聊听消息对象</param>
		/// <param name="normalColor">默认颜色</param>
		private void addChatContent(ref SChatMessageInfo info, String normalColor)
		{
            // 系统发言不过滤关键字
            bool bShieldWord = (info.senderPdbid != 0);

			List<SChatMessageObjectInfo> tmpList = parseChatContent(info.strMessage, normalColor, bShieldWord);

			info.objList.AddRange(tmpList);
		}

		// 判断是否在黑名单中
		private bool isBlackList(int dwUserID)
		{
            if (dwUserID <= 0)
                return false;

			if (LogicDataCenter.snsDataManager.getBuddyRelationStatus(dwUserID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST)
			{
				return true;
			}

			return false;
		}

		private void showChatBubble(gamelogic_show_chat_message msgInfo)
		{
			string normalColor = CreateColorParam(UDefines.ChatChannelColor((int)msgInfo.channel));
			SChatMessageInfo chatMsgInfo = new SChatMessageInfo();
            chatMsgInfo.channel = (EMChatChannelType)msgInfo.channel;
            chatMsgInfo.senderKinID = msgInfo.senderKinID;
            chatMsgInfo.senderName = msgInfo.senderName;
            chatMsgInfo.senderPdbid = msgInfo.senderPdbid;
            chatMsgInfo.senderUdbid = msgInfo.senderUdbid;
            chatMsgInfo.objList = new List<SChatMessageObjectInfo>();
			chatMsgInfo.strMessage = msgInfo.message;

			// 实体气泡
			if (msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_NEARBY
				||msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_KIN
				||msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_CLAN
                ||msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_CAMP
                ||msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_WARSCENE)
			{
				if (msgInfo.senderUID == 0)
				{
					return;
				}

				// 战场内不显示联盟和战队的气泡
				if (msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_KIN || msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_CLAN)
				{
					if (GameLogicAPI.isInWarScene() > 0)
						return;
				}

				// 解析内容
				addChatContent(ref chatMsgInfo, normalColor);

				UChatBubbleMsgData msgData = new UChatBubbleMsgData();
				msgData.msgID = (int)WndMsgID.WND_MSG_CHATBUBBLE_NEW_MESSAGE;
				msgData.uid = msgInfo.senderUID;
				msgData.info = chatMsgInfo;

				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_NEW_MESSAGE, msgData);
			}
			else if (msgInfo.channel == (int)EMChatChannelType.CHAT_CHANNEL_TEAMMATE)	// 队伍聊天气泡
			{
				if (msgInfo.senderPdbid == 0)
				{
					Debug.LogWarning("msgInfo.senderPdbid == 0");
					return;
				}

				// 解析内容
				addChatContent(ref chatMsgInfo, normalColor);

				UTeamBubbleChatMessage msgData = new UTeamBubbleChatMessage();
				msgData.msgID = (int)WndMsgID.WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE;
				msgData.pdbid = msgInfo.senderPdbid;
				msgData.info = chatMsgInfo;

				// 发送到队伍UI上显示聊天气泡
				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE, msgData);
			}
		}

        private bool isHasLargeEmotion(SChatMessageInfo info)
        {
            foreach(var item in info.objList)
            {
                if(item.type == ChatItemType.ObjType_Image && item.subType == ChatItemSubType.ObjSubType_Image_Emotion)
                {
                    int type = -1;
                    string strType = "";
                    if(item.param.TryGetValue("type", out strType) == false)
                    {
                        continue;
                    }

                    if(int.TryParse(strType, out type))
                    {
                        if(type == (int)EMChatEmoticonType.Emoticon_Large)
                        {
                            return true;
                        }
                    }
                }
            }

            return false;
        }
	}
}