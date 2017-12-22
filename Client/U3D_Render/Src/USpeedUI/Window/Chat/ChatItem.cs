/*******************************************************************
** 文件名:	ChatItem.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	李有红
** 日  期:	2017/5/19
** 版  本:	1.0
** 描  述:	聊听内容-富文本标签实现
** 应  用:  
*****************************************************************/

using System;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using DataCenter;
using UIWidgets;
using USpeedUI.UExtensions;
using GameLogic;
using USpeedUI.UWidgets;
using War;
using USpeedUI.PlayerFrame;
using ASpeedGame.Data.GameMobaSchemes;
using Data.ActorPrizeConfig;
using USpeedUI.UWidgets.UAnimator;
using ASpeedGame.Data.CrownGenstoneConfig;
using System.Collections.Generic;
using LegendCup_ManagedDef;

namespace USpeedUI.Chat
{
	// 聊天消息子对象类型
	public enum ChatItemType
	{
		ObjType_Null,           // 无效对象

		ObjType_Text,           // 纯文本对象
		ObjType_Image,          // 图像对象
	}

	public enum ChatItemSubType
	{
		ObjSubType_Null,                // 无效对象

		ObjSubType_Text_Text,           // 文本对象
		ObjSubType_Text_Channel,        // 频道对象
		ObjSubType_Text_Actor,          // 角色对象
		ObjSubType_Text_ShowWnd,        // 点击打开窗口对象
		ObjSubType_Text_HeroCard,       // 英雄卡牌对象
		ObjSubType_Text_ActorWarResult, // 玩家战绩对象
		ObjSubType_Text_JoinTeam,       // 加入队伍对象
		ObjSubType_Text_Prize,          // 奖励对象
		ObjSubType_Text_Gemstone,       // 宝石对象
		ObjSubType_Text_CrownPage,      // 皇冠页对象
		ObjSubType_Image_Emotion,       // 表情对象
		ObjSubType_Text_BuddyChat,     //  好友聊天对象
		ObjSubType_Text_LegendCup,      // 杯赛对象
		ObjSubType_Text_ShowClanWnd,    // 显示联盟窗口对象
		ObjSubType_Text_ShowChatWnd,    // 显示聊天窗口对象
		ObjSubType_Text_EquipScheme,    // 装备方案对象
		ObjSubType_Text_SwitchScene,    // 切换场景（换线）
		ObjSubType_Text_InvitePlayGame, // 邀请一起游戏
        ObjSubType_Text_CreateClanCupWnd, // 显示创建联盟杯赛窗体
        ObjSubType_Text_ClickInviteLegendCup,   // 点击邀请杯赛
        ObjSubType_Text_CreateInviteLegendCup,  // 创建邀请杯赛
        ObjSubType_Text_ClanMemberCupWnd, // 显示联盟杯赛窗体
	}

	public interface IChatItem
    {
        // 获取物件类型
        ChatItemType GetItemType();
        // 设置物件内容
        bool SetItemData(SChatMessageObjectInfo data);
        // 获取坐标组件
        RectTransform GetTransform();
        // 获取物件宽度
        float GetItemWidth();
        // 获取物件高度
        float GetItemHeight();
        // 分割物件
        void SplitByWidth(float width, float offsetX, out IChatItem item1, out IChatItem item2);

        // 销毁物件
        void Destroy();
        // 分割物件
        bool SplitByIndex(int nIndex, out IChatItem item1, out IChatItem item2);
        // 复制物件
        IChatItem Clone();

        // 获取输入框用换算字符数
        int GetConvertCharCount();
        // 获取指定字符处的起始位置
        float GetCharStart(int index);
        // 获取指定字符处的宽度
        float GetCharWidth(int index);
        // 获取指定字符处的实际字符串
        String GetCharString(int beginIndex, int endIndex);
        // 删除指定位置字符
        bool DeleteChar(int index, int count);
        // 插入指定位置字符
        bool InsertChar(int index, String text);

        ResNode GetResNode();

        void SetResNode(ResNode resNode);

		// 清除状态
		//void Clear();
    }

	public class ChatItem
	{
		public static string  GetChatObjSubTypeString(ChatItemSubType subType, bool bLimit)
		{
			switch (subType)
			{
				case ChatItemSubType.ObjSubType_Text_Text:
					return "text";
				case ChatItemSubType.ObjSubType_Text_Channel:
					return "channel";
				case ChatItemSubType.ObjSubType_Text_Actor:
					return "actor";
				case ChatItemSubType.ObjSubType_Text_ShowWnd:
					return "showwnd";
				case ChatItemSubType.ObjSubType_Text_HeroCard:
					return "herocard";
				case ChatItemSubType.ObjSubType_Text_ActorWarResult:
					return "warresult";
				case ChatItemSubType.ObjSubType_Text_JoinTeam:
					return "jointeam";
				case ChatItemSubType.ObjSubType_Text_Prize:
					return "prize";
				case ChatItemSubType.ObjSubType_Text_Gemstone:
					return "gemstone";
				case ChatItemSubType.ObjSubType_Text_CrownPage:
					return "crownpage";
				case ChatItemSubType.ObjSubType_Image_Emotion:
					return "emo";
				case ChatItemSubType.ObjSubType_Text_BuddyChat:
					return "buddychat";
				case ChatItemSubType.ObjSubType_Text_LegendCup:
					return "legendcup";
				case ChatItemSubType.ObjSubType_Text_ShowClanWnd:
					return "showclanwnd";
				case ChatItemSubType.ObjSubType_Text_ShowChatWnd:
					return "showchatwnd";
				case ChatItemSubType.ObjSubType_Text_EquipScheme:
					return "equipscheme";
                case ChatItemSubType.ObjSubType_Text_SwitchScene:
                    return "switchscene";
                case ChatItemSubType.ObjSubType_Text_InvitePlayGame:
                    return "inviteplaygame";
                case ChatItemSubType.ObjSubType_Text_CreateClanCupWnd:
                    return "createclancupwnd";
                case ChatItemSubType.ObjSubType_Text_ClanMemberCupWnd:
                    return "clanmembercupwnd";
                case ChatItemSubType.ObjSubType_Text_ClickInviteLegendCup:
                    return "clickinvitelegendcup";
                case ChatItemSubType.ObjSubType_Text_CreateInviteLegendCup:
                    return "createinvitelegendcup";
                    
                default:
					break;
			}

			return "";
		}

		public static void GetChatObjStringSubType(String type, bool bLimit, out ChatItemType objType, out ChatItemSubType subType)
		{
			objType = ChatItemType.ObjType_Null;
			subType = ChatItemSubType.ObjSubType_Null;
			switch (type)
			{
				case "text":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_Text;
					}
					break;
				case "channel":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_Channel;
					}
					break;
				case "actor":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_Actor;
					}
					break;
				case "showwnd":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_ShowWnd;
					}
					break;
				case "herocard":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_HeroCard;
					}
					break;
				case "warresult":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_ActorWarResult;
					}
					break;
				case "jointeam":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_JoinTeam;
					}
					break;
				case "prize":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_Prize;
					}
					break;
				case "gemstone":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_Gemstone;
					}
					break;
				case "crownpage":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_CrownPage;
					}
					break;
				case "emo":
					{
						objType = ChatItemType.ObjType_Image;
						subType = ChatItemSubType.ObjSubType_Image_Emotion;
					}
					break;
				case "buddychat":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_BuddyChat;
					}
					break;
				case "legendcup":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_LegendCup;
					}
					break;
				case "showclanwnd":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_ShowClanWnd;
					}
					break;
				case "showchatwnd":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_ShowChatWnd;
					}
					break;
				case "equipscheme":
					{
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_EquipScheme;
					}
					break;
                case "switchscene":
                    {
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_SwitchScene;
                    }
                    break;
                case "inviteplaygame":
                    {
						objType = ChatItemType.ObjType_Text;
						subType = ChatItemSubType.ObjSubType_Text_InvitePlayGame;
                    }
                    break;
                case "createclancupwnd":
                    {
                        objType = ChatItemType.ObjType_Text;
                        subType = ChatItemSubType.ObjSubType_Text_CreateClanCupWnd;
                    }
                    break;
                case "clanmembercupwnd":
                    {
                        objType = ChatItemType.ObjType_Text;
                        subType = ChatItemSubType.ObjSubType_Text_ClanMemberCupWnd;
                    }
                    break;
                    
                case "clickinvitelegendcup":
                    {
                        objType = ChatItemType.ObjType_Text;
                        subType = ChatItemSubType.ObjSubType_Text_ClickInviteLegendCup;
                    }
                    break;
                case "createinvitelegendcup":
                    {
                        objType = ChatItemType.ObjType_Text;
                        subType = ChatItemSubType.ObjSubType_Text_CreateInviteLegendCup;
                    }
                    break;
                default:
					break;
			}
		}
	}

    public class ChatSubItemText_Base
    {

        protected ChatItemText m_obj;
        protected SChatMessageObjectInfo m_itemData;
        protected Color m_defaultColor = Color.white;
        protected Text m_UIText;

        protected ChatItemText m_prevObj;
        public ChatItemText PrevObj { get { return m_prevObj; } }
        protected ChatItemText m_nextObj;
        public ChatItemText NextObj { get { return m_nextObj; } }

        private ResNode resNode;

        public virtual void SetResNode(ResNode rn)
        {
            resNode = rn;
        }

        public virtual ResNode GetResNode()
        {
            return resNode;
        }

        public virtual bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            m_obj = obj;
            m_UIText = uiText;
            m_itemData = data;

            if (data.param != null && data.param.ContainsKey("color"))
            {
                string colorParam = data.param["color"];
                int r, g, b;
                LogicDataCenter.chatMessageManager.ParseColorParam(colorParam, out r, out g, out b);
                float factor = 1.0f / 255.0f;
                m_defaultColor = new Color(r * factor, g * factor, b * factor);
            }

			m_UIText.text = data.text;
			m_UIText.color = m_defaultColor;
			m_UIText.raycastTarget = true;

            return true;
        }

        // 获取坐标组件
        public virtual RectTransform GetTransform()
        {
            return m_UIText.rectTransform;
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            return m_UIText.preferredWidth;
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            return m_UIText.preferredHeight;
        }

        // 分割物件
        public virtual void SplitByWidth(float width, float offsetX, out IChatItem item1, out IChatItem item2)
        {
            float leftWidth = width - offsetX;
            String strText = m_UIText.text;
            if (strText.Length <= 0)
            {
                // 无效就全返回null
                item1 = null;
                item2 = null;
                return;
            }

            if (m_UIText.preferredWidth < leftWidth)
            {
                // 足够长就直接返回
                item1 = m_obj;
                item2 = null;
                return;
            }

            String strCache = "" + strText[0];
            m_UIText.text = strCache;
            if (m_UIText.preferredWidth > leftWidth)
            {
                // 长度不满足最低限度
                if (offsetX > 0f)
                {
                    // 作为下一行返回
                    m_UIText.text = strText;

                    item1 = null;
                    item2 = m_obj;
                }
                else
                {
                    // 整行宽度都塞不下一个字，强行放一个进去
                    m_UIText.text = strCache;
                    item1 = m_obj;

                    if (strText.Length > 1)
                    {
                        // 创建分割物件
                        item2 = CreateSplitItem(strText.Substring(strCache.Length));
                    }
                    else
                    {
                        item2 = null;
                    }
                }
                return;
            }

            // 寻找切割点
            int strIndex = 1;
            while (strIndex < strText.Length)
            {
                strCache += strText[strIndex++];
                m_UIText.text = strCache;
                if (m_UIText.preferredWidth > leftWidth)
                {
                    break;
                }
            }

            strCache = strCache.Remove(strCache.Length - 1);
            m_UIText.text = strCache;

            // 创建分割物件
            item1 = m_obj;
            item2 = CreateSplitItem(strText.Substring(strCache.Length));
            return;
        }

        private ChatItemText CreateSplitItem(String text)
        {
            GameObject nextObj = ResNode.InstantiateRes(m_obj.gameObject);
            nextObj.transform.SetParent(m_obj.transform.parent, false);
            ChatItemText nextItem = nextObj.GetComponent<ChatItemText>();
            SChatMessageObjectInfo info = m_itemData;
            info.text = text;
            nextItem.IsSplit = true;
            nextItem.SetItemData(info);

            m_nextObj = nextItem;
            nextItem.SubItem.m_prevObj = m_obj;
            return nextItem;
        }

        // 分割物件
        public virtual bool SplitByIndex(int nIndex, out IChatItem item1, out IChatItem item2)
        {
            String curText = m_UIText.text;
            if (nIndex <= 0 || nIndex >= curText.Length)
            {
                item1 = item2 = null;
                return false;
            }

            String splitText = curText.Substring(nIndex);
            curText = curText.Remove(nIndex);
            m_UIText.text = curText;
            item1 = m_obj;
            item2 = CreateSplitItem(splitText);
            return true;
        }
        
        // 复制物件
        public virtual IChatItem Clone()
        {
            ChatItemText newItem = resNode.InstanceMainRes<ChatItemText>();

			newItem.SetResNode(resNode);

            newItem.SetItemData(m_itemData);

            return newItem;
        }

        // 获取输入框用换算字符数
        public virtual int GetConvertCharCount() { return 1; }
        // 获取指定字符处的起始位置
        public virtual float GetCharStart(int index) { return 0f; }
        // 获取指定字符处的宽度
        public virtual float GetCharWidth(int index) { return GetItemWidth(); }

        // 获取指定字符处的实际字符串
        public virtual String GetCharString(int beginIndex, int endIndex)
        {
            String typeName = ChatItem.GetChatObjSubTypeString(m_itemData.subType, false);
            if (typeName == "")
                return "";
            String strResult = "" + ChatMessageManager.ChatItemFlag;
            strResult += String.Format("[{0}", typeName);
            if (m_itemData.param != null)
            {
                foreach (var data in m_itemData.param)
                {
                    strResult += String.Format(" {0}={1}", data.Key, data.Value);
                }
            }
            strResult += String.Format("]{0}[/{1}]", m_UIText.text, typeName);
            return strResult;
        }

        // 删除指定位置字符
        public virtual bool DeleteChar(int index, int count) { return false; }
        // 插入指定位置字符
        public virtual bool InsertChar(int index, String text) { return false; }

        public virtual void OnDestroy()
        {
        }

        public virtual void OnUpdate()
        {
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
        }
    }

    class ChatSubItemText_Text : ChatSubItemText_Base
    {
        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);
			m_UIText.raycastTarget = false;
			return true;
        }

        // 获取输入框用换算字符数
        public override int GetConvertCharCount()
        {
            return m_UIText.text.Length;
        }

        // 获取指定字符处的起始位置
        public override float GetCharStart(int index)
        {
            if (index <= 0 || index >= m_UIText.text.Length)
                return 0f;

            var settings = m_UIText.GetGenerationSettings(Vector2.zero);
            return m_UIText.cachedTextGeneratorForLayout.GetPreferredWidth(m_UIText.text.Substring(0, index), settings) / m_UIText.pixelsPerUnit;
        }

        // 获取指定字符处的宽度
        public override float GetCharWidth(int index)
        {
            if (index < 0 || index >= m_UIText.text.Length)
                return 0f;

            var settings = m_UIText.GetGenerationSettings(Vector2.zero);
            return m_UIText.cachedTextGeneratorForLayout.GetPreferredWidth(m_UIText.text.Substring(index, 1), settings) / m_UIText.pixelsPerUnit;
        }

        // 获取指定字符处的实际字符串
        public override String GetCharString(int beginIndex, int endIndex)
        {
            String text = m_UIText.text;
            if (beginIndex < 0)
                beginIndex = 0;
            if (endIndex >= text.Length)
                endIndex = text.Length - 1;
            if (beginIndex > endIndex)
                endIndex = beginIndex;

            return text.Substring(beginIndex, endIndex - beginIndex + 1);
        }

        // 删除指定位置字符
        public override bool DeleteChar(int index, int count)
        {
            String text = m_UIText.text;
            if (index < 0 || count <= 0 || index + count > text.Length)
                return false;

            m_UIText.text = text.Remove(index, count);
            m_itemData.text = m_UIText.text;
            return true;
        }

        // 插入指定位置字符
        public override bool InsertChar(int index, String insertText)
        {
            String text = m_UIText.text;
            if (index < 0 || index > text.Length)
                return false;

            m_UIText.text = text.Insert(index, insertText);
            m_itemData.text = m_UIText.text;
            return true;
        }
    }

    class ChatSubItemText_Actor : ChatSubItemText_Base
    {
        private int m_nActorID = 0;
        private int m_nUserID = 0;
        private int m_nKinID = 0;
        private UPopupMenuHandle m_popupMenuHandle = null;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id"))
                return false;

            if (EntityFactory.MainHeroView != null)
            {
                // 忽略自身
                int nActorID = Convert.ToInt32(m_itemData.param["id"]);
                int nUserID = Convert.ToInt32(m_itemData.param["userID"]);
                int nKinID = Convert.ToInt32(m_itemData.param["kinID"]);
                int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                if (nActorID != nHeroID)
                {
                    m_nActorID = nActorID;
                    m_nUserID = nUserID;
                    m_nKinID = nKinID;
                    //popupMenuHandle = obj.gameObject.GetComponent<UPopupMenuHandle>();
					//if(popupMenuHandle == null)
					m_popupMenuHandle = obj.gameObject.AddComponent<UPopupMenuHandle>();

                    if (m_popupMenuHandle != null)
                    {
                        UPopupAction popupAction = new UPopupAction();
                        popupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(m_nActorID));
                        popupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(m_nActorID));
                        popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(m_nActorID));
                        popupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(m_nUserID, uiText.text));
                        popupAction.Add(UPopupItemDataAddBlackList.GetDescString(), new UPopupItemDataAddBlackList(m_nUserID, uiText.text));
                        popupAction.Add(UPopupItemDataCopyName.GetDescString(), new UPopupItemDataCopyName(m_UIText.text));
                        popupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "PrivateChat"), new UListComponentItemDataPopup(OnPrivateChat));
                        popupAction.Add(UPopupItemDataApplyInternMaster.GetDescString(), new UPopupItemDataApplyInternMaster(m_nActorID));
                        popupAction.Add(UPopupItemDataApplyInternPrentice.GetDescString(), new UPopupItemDataApplyInternPrentice(m_nActorID));

                        if (nHeroID == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                        {
                            popupAction.Add(UPopupItemDataKinInvite.GetDescString(), new UPopupItemDataKinInvite(m_nActorID));
                        }

                        if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN) == 0)
                        {
                            popupAction.Add(UPopupItemDataKinRequest.GetDescString(), new UPopupItemDataKinRequest(m_nKinID));
                        }

                        m_popupMenuHandle.HandlePopupAction = popupAction;
                    }
                }
            }
            return true;
        }

		public override void OnDestroy()
		{
			if(m_popupMenuHandle != null)
			{
				m_obj.gameObject.RemoveComponent<UPopupMenuHandle>();
				m_popupMenuHandle = null;
			}
			m_nActorID = 0;
			m_nKinID = 0;
			m_nUserID = 0;
		}

		public override void OnPointerEnter(PointerEventData eventData)
        {
            if (m_nActorID == 0)
                return;

            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (m_nActorID == 0)
                return;
        }

        private void OnPrivateChat()
        {
            // 通知触发私聊
            ChatBoxPrivateChat msgData = new ChatBoxPrivateChat();
            msgData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT;
            msgData.name = m_UIText.text;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT, msgData);
        }
    }

    class ChatSubItemText_Channel : ChatSubItemText_Base
    {
        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button == PointerEventData.InputButton.Left)
            {
                if (m_itemData.param != null && m_itemData.param.ContainsKey("type"))
                {
                    int nChannelType = Convert.ToInt32(m_itemData.param["type"]);
                    ChatBoxChangeChannelData data = new ChatBoxChangeChannelData();
                    data.msgID = (int)WndMsgID.WND_MSG_CHATBOX_CHANGECHANNEL;
                    data.channel = nChannelType;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGECHANNEL, data);
                }
            }
        }
    }

    class ChatSubItemText_ShowWnd : ChatSubItemText_Base
    {
        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			if (eventData.button == PointerEventData.InputButton.Left)
            {
                if (m_itemData.param != null && m_itemData.param.ContainsKey("type"))
                {
                    String strWndType = m_itemData.param["type"];
                    if (strWndType == "clandonate")
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCARD, null);
                }
            }
        }
    }

    class ChatSubItemText_HeroCard : ChatSubItemText_Base
    {
        private int m_nCardID = 0;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id"))
                return false;

            m_nCardID = Convert.ToInt32(m_itemData.param["id"]);

            // 先直接当成英雄ID取，如果以后能获取卡片信息了再改
            PersonSkinModelItem modelData = new PersonSkinModelItem();
            if (GameLogicAPI.GetPersonModel((UInt16)m_nCardID, ref modelData))
            {
                m_UIText.text = "[" + modelData.szHeroName + "]";
                m_defaultColor = new Color(0.92f, 0.63f, 0.17f);
                m_UIText.color = m_defaultColor;
            }

            return true;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			if (eventData.button == PointerEventData.InputButton.Left)
            {
            }
        }
    }

    class ChatSubItemText_ActorWarResult : ChatSubItemText_Base
    {
        private UTooltipTrigger m_tooltip = null;
        private int m_nActorID = 0;
        private long m_nWarID = 0;
        private bool m_bRequestingInfo = false;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id") || !m_itemData.param.ContainsKey("war"))
                return false;

            m_nActorID = Convert.ToInt32(m_itemData.param["id"]);
            m_nWarID = Convert.ToInt64(m_itemData.param["war"]);

            // 创建tooltip
            //if (!obj.IsSplit)
            {
				//m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
				//if (m_tooltip == null)
				//m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_WarResultShow);

				m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_WarResultShow);

				if (m_tooltip != null)
                    m_tooltip.backgroundTint = Color.grey;
            }
            //else
            //{
                //m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
            //}

            return true;
        }

		public override void OnDestroy()
		{
			if(m_tooltip != null)
			{
				m_obj.gameObject.RemoveComponent<UTooltipTrigger>();
				m_tooltip = null;
			}
			m_nActorID = 0;
			m_nWarID = 0;
			m_bRequestingInfo = false;
		}

		public override void OnUpdate()
        {
            if (m_bRequestingInfo)
            {
				if (m_tooltip == null)
					return;

                WarStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(m_nWarID, EGetSingleWarType.EGSWT_PlayerRecord);
                if (info.actorInfo.Count <= 0)
                {
                    return;
                }

                if (info.mainInfo.nWarID != 0)
                {
                    m_bRequestingInfo = false;

                    for (int i = 0; i < info.actorInfo.Count; ++i)
                    {
                        if (info.actorInfo[i].nActorID == m_nActorID)
                        {
                            cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];
                            m_tooltip.SetText("Title", ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowTitle"));
                            m_tooltip.SetImage("HeroIcon", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CHATBOX, 1, 3, actorInfo.nHeroID));
                            m_tooltip.SetImage("MVPIcon", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarResult_MVPIcon", WndID.WND_ID_CHATBOX));
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_SCORE) > 0)
                                m_tooltip.TurnSectionOn("MVPIcon");
                            else
                                m_tooltip.TurnSectionOff("MVPIcon");
                            m_tooltip.SetText("ActorName", actorInfo.szActorName);

                            bool bCamp1Win = false;
                            if (info.mainInfo.nEndBecause == 1 || info.mainInfo.nEndBecause == 4)
                                bCamp1Win = true;
                            if ((actorInfo.nCamp == 1 && bCamp1Win) || (actorInfo.nCamp != 1 && !bCamp1Win))
                                m_tooltip.SetImage("ResultImage", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarResult_Win", WndID.WND_ID_CHATBOX));
                            else
                                m_tooltip.SetImage("ResultImage", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarResult_Lose", WndID.WND_ID_CHATBOX));
                            if (actorInfo.nWinType <= 0)
                            {
                                m_tooltip.TurnSectionOff("ResultTypeImage");
                            }
                            else
                            {
                                m_tooltip.TurnSectionOn("ResultTypeImage");
                                m_tooltip.SetImage("ResultTypeImage", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarResultType_" + actorInfo.nWinType, WndID.WND_ID_CHATBOX));
                            }

                            int lastTime = info.mainInfo.nEndTime - info.mainInfo.nBeginTime;
                            lastTime /= 60;
                            int nLastHour = lastTime / 60;
                            int nLastMin = lastTime % 60;
                            String lastTimeStr = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLastTime") + ":";
                            if (nLastHour > 0)
                                lastTimeStr += nLastHour + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarHour");
                            lastTimeStr += nLastMin + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMinute");
                            m_tooltip.SetText("LastTime", lastTimeStr);

                            m_tooltip.SetText("AchiDescText", ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowAchiDesc"));

                            int nIndex = 1;
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_OUTPUT) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopDamage, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_KILLCOUNT) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopKill, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_ASSCOUNT) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopAssist, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_DAMAGE) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopTank, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_DESTORYTOWER) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopTowerKill, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_MONEY) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopRich, ref nIndex);
                            if ((actorInfo.nGlory & (int)EWarMedal.EWM_KILLMONSTER) > 0)
                                AddAchiIcon((int)AchiIconType.Icon_TopFarm, ref nIndex);
                            if (actorInfo.nContinueKillMax >= 8)
                                AddAchiIcon((int)AchiIconType.Icon_Godlike, ref nIndex);
                            if (actorInfo.nMultiKillMax == 3)
                                AddAchiIcon((int)AchiIconType.Icon_TripleKill, ref nIndex);
                            if (actorInfo.nMultiKillMax == 4)
                                AddAchiIcon((int)AchiIconType.Icon_FourKill, ref nIndex);
                            if (actorInfo.nMultiKillMax == 5)
                                AddAchiIcon((int)AchiIconType.Icon_FiveKill, ref nIndex);
                            for (int n = nIndex; n <= 12; ++n)
                                m_tooltip.SetImage("AchiImage" + n, null);

                            m_tooltip.SetText("EquipDescText", ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowEquipDesc"));

                            for (int n = 0; n < 6; ++n)
                            {
                                String strItem = "EquipIcon" + (n + 1);
                                SSchemeMobaGoods goodsInfo;
                                if (!LogicDataCenter.goodsSchemesDataManager.GetMobaGoods_Out(actorInfo.nEquipID[n], out goodsInfo))
                                {
                                    m_tooltip.TurnSectionOff(strItem);
                                    continue;
                                }

                                int nEquipIcon = goodsInfo.GoodsIconID;
                                if (nEquipIcon > 0)
                                {
                                    Sprite equipSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_CHATBOX, 1, nEquipIcon);
                                    m_tooltip.SetImage(strItem, equipSprite);
                                    m_tooltip.TurnSectionOn(strItem);
                                }
                                else
                                {
                                    m_tooltip.TurnSectionOff(strItem);
                                }
                            }

                            m_tooltip.SetText("StatisticText1", String.Format(ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowStatisticDesc1"), actorInfo.nKillPersonNum));
                            m_tooltip.SetText("StatisticText2", String.Format(ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowStatisticDesc2"), actorInfo.nDieCount));
                            m_tooltip.SetText("StatisticText3", String.Format(ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowStatisticDesc3"), actorInfo.nContinueKillMax));
                            m_tooltip.SetText("StatisticText4", String.Format(ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowStatisticDesc4"), actorInfo.nMultiKillMax));
                            m_tooltip.SetText("StatisticText5", String.Format(ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowStatisticDesc5"), actorInfo.nGetMoney));

                            break;
                        }
                    }
                }
            }
        }

        private void AddAchiIcon(int nType, ref int nCurIndex)
        {
            Sprite achiSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarAchiType_" + nType, WndID.WND_ID_CHATBOX);
            m_tooltip.SetImage("AchiImage" + nCurIndex, achiSprite);
            ++nCurIndex;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;

            m_bRequestingInfo = true;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_WAR);
            int nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ButtonType.bIsRequestSelf = (nActorID == m_nActorID);
            ButtonType.nRequestPDBID = m_nActorID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);

            WarStatisticViewSingleWarData uiData = new WarStatisticViewSingleWarData();
            uiData.nEntryType = (int)ViewSingleWarType.VSWT_CHATWND;
            uiData.lWarID = m_nWarID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR, uiData);
        }
    }

    class ChatSubItemText_CrownGemstoneInfo : ChatSubItemText_Base
    {
        private UTooltipTrigger m_tooltip = null;
        private int m_nGemstoneID = 0;
        private int m_nGemstoneGrade = 0;
		private int m_nGemstoneExp = 0;
        private bool m_bRequestingInfo = false;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("gemstoneID") || !m_itemData.param.ContainsKey("gemstoneGrade") || !m_itemData.param.ContainsKey("gemstoneExp"))
                return false;

            m_nGemstoneID = Convert.ToInt32(m_itemData.param["gemstoneID"]);
            m_nGemstoneGrade = Convert.ToInt32(m_itemData.param["gemstoneGrade"]);
            m_nGemstoneExp = Convert.ToInt32(m_itemData.param["gemstoneExp"]);
            // 创建tooltip
            //if (!obj.IsSplit)
            {
                //m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
                //if (m_tooltip == null)
					//m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_GemstoneItem);
				m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_GemstoneItem);

				if(m_tooltip != null)
                    m_tooltip.backgroundTint = Color.grey;
            }
            //else
            //{
                //m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
            //}

            return true;
        }

		public override void OnDestroy()
		{
			if(m_tooltip != null)
			{
				m_obj.gameObject.RemoveComponent<UTooltipTrigger>();
				m_tooltip = null;
			}
			m_nGemstoneID = 0;
			m_nGemstoneGrade = 0;
			m_nGemstoneExp = 0;
			m_bRequestingInfo = false;
		}

		public override void OnUpdate()
        {
            if (!m_bRequestingInfo)
                return;

            if (m_tooltip == null)
                return;

            m_bRequestingInfo = false;
            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(m_nGemstoneID);
            if (config == null)
                return;

            if (m_nGemstoneGrade < 0 || m_nGemstoneGrade >= UDefines.sGemstoneGradeDesc.Length)
                return;

            //string sDesc = "[" + UDefines.sGemstoneGradeDesc[m_nGemstoneGrade] + config.sGemstoneName + "]";
            //m_tooltip.SetText(UTooltipParamName.BodyText, sDesc);

            m_tooltip.SetText("GemstoneName", config.sGemstoneName);
            m_tooltip.SetText("GemstoneGrade", UDefines.sGemstoneGradeDesc[m_nGemstoneGrade]);

            Sprite gemstoneSprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID);
            m_tooltip.SetImage("GemstoneIcon", gemstoneSprite);

            string sGemstonePropertyHeadText = "";
            string sGemstonePropertyDescText = "";
            if (m_nGemstoneGrade > 0 && m_nGemstoneGrade < config.szPropertyDescArray.Length)
            {
                string[] sCrownPropertyIDArray = ((string)(config.szPropertyArray[m_nGemstoneGrade])).Split(';');
                int nPropertyID = 0;
                string sPropertyDesc = "";
                int nTotalPower = 0;
                foreach (string sPropertyID in sCrownPropertyIDArray)
                {
                    int.TryParse(sPropertyID, out nPropertyID);
                    SSchemeCrownEffectConfig effectConfig = CrownPageConfigPage.Instance.GetCrownPageEffectConfig(nPropertyID);
                    if (effectConfig == null)
                        continue;

                    sPropertyDesc += effectConfig.sGemstoneDesc + "\n";
                    nTotalPower += effectConfig.nPropertyValue;
                }

                sGemstonePropertyHeadText = "威力 :" + nTotalPower;
                sGemstonePropertyDescText = sPropertyDesc;
            }

            m_tooltip.SetText("GemstonePropertyDesc", sGemstonePropertyDescText);
            m_tooltip.SetText("GemstonePropertyHead", sGemstonePropertyHeadText);

            if (m_nGemstoneExp >= config.nExpUpper || m_nGemstoneGrade == (int)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
            {
                //GemstoneExpImage.fillAmount = 1;
                m_tooltip.SetText("GemstoneExp", config.nExpUpper + "/" + config.nExpUpper); 
            }
            else
            {
                string[] expArray = config.szExpArray.Split(';');
                int nNextExp = 0;
                int nGrade = m_nGemstoneGrade;
                if (nGrade - 1 >= 0 && nGrade - 1 < expArray.Length)
                {
                    int.TryParse(expArray[nGrade - 1], out nNextExp);
                    nNextExp = nNextExp <= 0 ? 1 : nNextExp;
                   // GemstoneExpImage.fillAmount = ((float)data.nExp / nNextExp);
                }

                m_tooltip.SetText("GemstoneExp", m_nGemstoneExp + "/" + nNextExp);
            }

            int nExteriorID = 0;
            if (m_nGemstoneGrade >= 0 && m_nGemstoneGrade < config.nExteriorIDArray.Length)
                nExteriorID = config.nExteriorIDArray[m_nGemstoneGrade];

            string sGemstoneExteriorHeadText = "";
            string sGemstoneExteriorDescText = "";
            SSchemeCrownGemstoneExteriorConfig exteriorConfig = CrownPageConfigPage.Instance.GetCrownPageExteriorConfig(nExteriorID);
            if (exteriorConfig != null)
            {
                string sSymbols = "\n";
                sGemstoneExteriorHeadText = "颜值 :" + exteriorConfig.nFacescore;
                string sExteriorDesc = "色泽 :" + exteriorConfig.nColours + sSymbols
                + "透光 :" + exteriorConfig.nLight + sSymbols + "重量 :" + exteriorConfig.nWeight;
                sGemstoneExteriorDescText = sExteriorDesc;
            }

            m_tooltip.SetText("GemstoneExteriorDesc", sGemstoneExteriorDescText);
            m_tooltip.SetText("GemstoneExteriorHead", sGemstoneExteriorHeadText);
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;

            m_bRequestingInfo = true;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            
        }

		// 分享到聊听框
		public static void ShareToInputField(string actorName, int nFaceValue, int nPowerValue, int pdbid)
		{
			SChatMessageObjectInfo channelObj;
			channelObj.type = ChatItemType.ObjType_Text;
			channelObj.subType = ChatItemSubType.ObjSubType_Text_CrownPage;
			channelObj.text = String.Format(ULocalizationService.Instance.Get("UIView", "CrownGem", "GemPageLink"), actorName);
			channelObj.param = new Dictionary<string, string>();
			channelObj.param.Add("color", LogicDataCenter.chatMessageManager.CreateColorParam(235, 80, 30));
			channelObj.param.Add("facevalue", "" + nFaceValue);
			channelObj.param.Add("powervalue", "" + nPowerValue);
			channelObj.param.Add("pdbid", "" + pdbid);

			ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
			uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
			uiData.objInfo = channelObj;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
		}
    }

    class ChatSubItemText_CrownPageInfo : ChatSubItemText_Base
    {
        private UTooltipTrigger m_tooltip = null;
        private int m_nFaceValue = 0;
        private int m_nPowerValue = 0;
        private int m_nPDBID = 0;
        private bool m_bRequestingInfo = false;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("facevalue") || !m_itemData.param.ContainsKey("powervalue") || !m_itemData.param.ContainsKey("pdbid"))
                return false;

            m_nFaceValue = Convert.ToInt32(m_itemData.param["facevalue"]);
            m_nPowerValue = Convert.ToInt32(m_itemData.param["powervalue"]);
            m_nPDBID = Convert.ToInt32(m_itemData.param["pdbid"]);
            // 创建tooltip
            //if (!obj.IsSplit)
            {
                //m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
				//if (m_tooltip == null)
					//m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);

				m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
				if (m_tooltip != null)
					m_tooltip.backgroundTint = Color.grey;
			}
            //else
            //{
                //m_tooltip = obj.gameObject.GetComponent<UTooltipTrigger>();
            //}

            return true;
        }

		public override void OnDestroy()
		{
			if(m_tooltip != null)
			{
				m_obj.gameObject.RemoveComponent<UTooltipTrigger>();
				m_tooltip = null;
			}

			m_nFaceValue = 0;
			m_nPowerValue = 0;
			m_nPDBID = 0;
			m_bRequestingInfo = false;
		}

		public override void OnUpdate()
        {
            if (!m_bRequestingInfo)
                return;

            if (m_tooltip == null)
                return;

            m_bRequestingInfo = false;
            m_tooltip.SetText(UTooltipParamName.BodyText, "总颜值:"+m_nFaceValue+"\n"+"总威力:"+m_nPowerValue);
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;

            m_bRequestingInfo = true;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			if (m_nPDBID <= 0)
                return;

            CreatureProperty Property = EntityFactory.MainHeroView.Property;
            if (Property == null)
                return;

            if (m_nPDBID == Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
            {
                LogicDataCenter.playerSystemDataManager.bRequestSelf = true;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW, null);
            }
            else
            {
                LogicDataCenter.playerSystemDataManager.bRequestSelf = false;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW, null);

                cmd_Crown_Request_Other_Page msgInfo = new cmd_Crown_Request_Other_Page();
                msgInfo.RequestFriendID = (uint)m_nPDBID;
                EntityEventHelper.Instance.SendCommand<cmd_Crown_Request_Other_Page>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DOTHER_PAGE, ref msgInfo);
            }
        }
    }

    class ChatSubItemText_JoinTeam : ChatSubItemText_Base
    {
        private int m_nActorID;
        private String m_strActorName;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id") || !m_itemData.param.ContainsKey("name"))
                return false;

            m_nActorID = Convert.ToInt32(m_itemData.param["id"]);
            m_strActorName = m_itemData.param["name"];

            return true;
        }

		public override void OnDestroy()
		{
			m_nActorID = 0;
			m_strActorName = "";
		}

		public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			if (eventData.button == PointerEventData.InputButton.Left)
            {
                UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM, 0, "", IntPtr.Zero, 0);
                uiData.msgID = (int)WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM;
                uiData.nParam = m_nActorID;
                uiData.strParam = m_strActorName;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM, uiData);
            }
        }
    }

    class ChatSubItemText_ActorPrize : ChatSubItemText_Base
    {
        private UTooltipTrigger m_tooltip = null;
		private int m_nPrizeID = 0;

		public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id"))
                return false;

            m_nPrizeID = Convert.ToInt32(m_itemData.param["id"]);

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_nPrizeID);
            if (prizeConfig != null)
            {
                String strColor = "white";
                m_defaultColor = Color.white;
                if (prizeConfig.nPrizeGrade == 1)
                {
                    strColor = "green";
                    m_defaultColor = Color.green;
                }
                else if (prizeConfig.nPrizeGrade == 2)
                {
                    strColor = "blue";
                    m_defaultColor = Color.blue;
                }
                else if (prizeConfig.nPrizeGrade == 3)
                {
                    strColor = "purple";
                    m_defaultColor = new Color(0.5f, 0f, 0.5f);
                }
                else if (prizeConfig.nPrizeGrade == 4)
                {
                    strColor = "yellow";
                    m_defaultColor = Color.yellow;
                }
                else if (prizeConfig.nPrizeGrade == 5)
                {
                    strColor = "red";
                    m_defaultColor = Color.red;
                }
                else if (prizeConfig.nPrizeGrade == 6)
                {
                    strColor = "orange";
                    m_defaultColor = new Color(1f, 0.647f, 0f);
                }
                m_UIText.color = m_defaultColor;

				// 分割物件不重置文字
				//if (!obj.IsSplit)
				{ 
					//UTooltipTrigger pageTooltipTrigger = obj.gameObject.GetComponent<UTooltipTrigger>();
					//if(pageTooltipTrigger == null)
					//pageTooltipTrigger = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
					m_tooltip = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
					if(m_tooltip != null)
					{
						m_tooltip.backgroundTint = Color.grey;
						m_tooltip.SetText(UTooltipParamName.BodyText, " <color=" + strColor + "><size=25>" + prizeConfig.strPrizeName + "</size></color>\n<size=20>" + prizeConfig.strPrizeDesc + "</size>");
						//m_UIText.text = "[" + prizeConfig.strPrizeName + "]";
					}
                }

				// 不是分隔物
				if (!obj.IsSplit)
				{
					m_UIText.text = "[" + prizeConfig.strPrizeName + "]";
				}
            }

            return true;
        }

		public override void OnDestroy()
		{
			if(m_tooltip != null)
			{
				m_obj.gameObject.RemoveComponent<UTooltipTrigger>();
				m_tooltip = null;
			}

			m_nPrizeID = 0;
		}

		public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }
    }

    public class ChatItemText : MonoBehaviour, IChatItem, IPointerEnterHandler, IPointerExitHandler, IPointerDownHandler, IPointerClickHandler
    {
        
        private ChatSubItemText_Base m_subItem;
        public ChatSubItemText_Base SubItem { get { return m_subItem; } }

        private bool m_bIsSplit;
        public bool IsSplit { get { return m_bIsSplit; } set { m_bIsSplit = value; } }

        private float m_fCanClickTime;
        private float m_fClickIntervalTime = 5f;
        public float ClickIntervalTime { get { return m_fClickIntervalTime; } set { m_fClickIntervalTime = value; } }

        private ResNode resNode;

		//public virtual void Clear()
		//{
			//m_bIsSplit = false;
			//m_fCanClickTime = 0;
		//}

        public virtual void SetResNode(ResNode rn)
        {
            resNode = rn;
        }

        public virtual ResNode GetResNode()
        {
            return resNode;
        }
        // 获取物件类型
        public virtual ChatItemType GetItemType()
        {
            return ChatItemType.ObjType_Text;
        }

        // 设置物件内容
        public virtual bool SetItemData(SChatMessageObjectInfo data)
        {
            switch (data.subType)
            {
                case ChatItemSubType.ObjSubType_Text_Text:
                    m_subItem = new ChatSubItemText_Text();
                    break;
                case ChatItemSubType.ObjSubType_Text_Actor:
                    m_subItem = new ChatSubItemText_Actor();
                    break;
                case ChatItemSubType.ObjSubType_Text_Channel:
                    m_subItem = new ChatSubItemText_Channel();
                    break;
                case ChatItemSubType.ObjSubType_Text_ShowWnd:
                    m_subItem = new ChatSubItemText_ShowWnd();
                    break;
                case ChatItemSubType.ObjSubType_Text_HeroCard:
                    m_subItem = new ChatSubItemText_HeroCard();
                    break;
                case ChatItemSubType.ObjSubType_Text_ActorWarResult:
                    m_subItem = new ChatSubItemText_ActorWarResult();
                    break;
                case ChatItemSubType.ObjSubType_Text_JoinTeam:
                    m_subItem = new ChatSubItemText_JoinTeam();
                    break;
                case ChatItemSubType.ObjSubType_Text_Prize:
                    m_subItem = new ChatSubItemText_ActorPrize();
                    break;
                case ChatItemSubType.ObjSubType_Text_Gemstone:
                    m_subItem = new ChatSubItemText_CrownGemstoneInfo();
                    break;
                case ChatItemSubType.ObjSubType_Text_CrownPage:
                    m_subItem = new ChatSubItemText_CrownPageInfo();
                    break;
                case ChatItemSubType.ObjSubType_Text_BuddyChat:
                    m_subItem = new ChatSubItemText_BuddyChat();
                    break;
                case ChatItemSubType.ObjSubType_Text_LegendCup:
                    m_subItem = new ChatSubItemText_LegendCup();
                    break;
                case ChatItemSubType.ObjSubType_Text_ShowClanWnd:
                    m_subItem = new ChatSubItemText_ShowClanWnd();
                    break;
                case ChatItemSubType.ObjSubType_Text_ShowChatWnd:
                    m_subItem = new ChatSubItemText_ShowChatWnd();
                    break;
                case ChatItemSubType.ObjSubType_Text_EquipScheme:
                    m_subItem = new ChatSubItemText_EquipScheme();
                    break;
                case ChatItemSubType.ObjSubType_Text_SwitchScene:
                    m_subItem = new ChatSubItemText_SwitchScene();
                    break;
                case ChatItemSubType.ObjSubType_Text_InvitePlayGame:
                    m_subItem = new ChatSubItemText_InvitePlayGame();
                    break;
                case ChatItemSubType.ObjSubType_Text_CreateClanCupWnd:
                    m_subItem = new ChatSubItemText_ShowCreateClanCupWnd();
                    break;
                case ChatItemSubType.ObjSubType_Text_ClanMemberCupWnd:
                    m_subItem = new ChatSubItemText_ShowClanWnd();
                    break;
                    
                case ChatItemSubType.ObjSubType_Text_ClickInviteLegendCup:
                    m_subItem = new ChatSubItemText_ClickInviteLegendCup();
                    break;
                case ChatItemSubType.ObjSubType_Text_CreateInviteLegendCup:
                    m_subItem = new ChatSubItemText_CreateInviteLegendCup();
                    break;
                default:
                    return false;
            }

			m_subItem.SetResNode(resNode);

            Text uiText = GetComponent<Text>();
            return m_subItem.Init(this, uiText, data);
        }

        // 获取坐标组件
        public virtual RectTransform GetTransform()
        {
            return m_subItem.GetTransform();
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            return m_subItem.GetItemWidth();
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            return m_subItem.GetItemHeight();
        }

        // 分割物件
        public virtual void SplitByWidth(float width, float offsetX, out IChatItem item1, out IChatItem item2)
        {
            m_subItem.SplitByWidth(width, offsetX, out item1, out item2);
        }

        // 销毁物件
        public virtual void Destroy()
        {
			m_bIsSplit = false;
			m_fCanClickTime = 0;

            m_subItem.OnDestroy();
            var temp = gameObject;
            ResNode.DestroyRes(ref temp);
        }

        // 分割物件
        public virtual bool SplitByIndex(int nIndex, out IChatItem item1, out IChatItem item2)
        {
            return m_subItem.SplitByIndex(nIndex, out item1, out item2);
        }

        // 复制物件
        public virtual IChatItem Clone()
        {
            return m_subItem.Clone();
        }

        // 获取输入框用换算字符数
        public virtual int GetConvertCharCount()
        {
            return m_subItem.GetConvertCharCount();
        }

        // 获取指定字符处的起始位置
        public virtual float GetCharStart(int index)
        {
            return m_subItem.GetCharStart(index);
        }

        // 获取指定字符处的宽度
        public virtual float GetCharWidth(int index)
        {
            return m_subItem.GetCharWidth(index);
        }

        // 获取指定字符处的实际字符串
        public virtual String GetCharString(int beginIndex, int endIndex)
        {
            return m_subItem.GetCharString(beginIndex, endIndex);
        }

        // 删除指定位置字符
        public virtual bool DeleteChar(int index, int count)
        {
            return m_subItem.DeleteChar(index, count);
        }

        // 插入指定位置字符
        public virtual bool InsertChar(int index, String insertText)
        {
            return m_subItem.InsertChar(index, insertText);
        }

        public void Update()
        {
            m_subItem.OnUpdate();
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            ChatItemText curObj = this;
            while (curObj.m_subItem.PrevObj != null)
                curObj = curObj.m_subItem.PrevObj;

            while (curObj != null)
            {
                curObj.m_subItem.OnPointerEnter(eventData);
                curObj = curObj.m_subItem.NextObj;
            }
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            ChatItemText curObj = this;
            while (curObj.m_subItem.PrevObj != null)
                curObj = curObj.m_subItem.PrevObj;

            while (curObj != null)
            {
                curObj.m_subItem.OnPointerExit(eventData);
                curObj = curObj.m_subItem.NextObj;
            }
        }

        public void OnPointerDown(PointerEventData eventData)
        {
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (Time.unscaledTime < m_fCanClickTime)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_CLICK_RICHTEXT_ITEM_COOLDOWN);
                return;
			}

            m_fCanClickTime = Time.unscaledTime + m_fClickIntervalTime;

            m_subItem.OnPointerClick(eventData);
        }
    }

    class ChatSubItemText_BuddyChat : ChatSubItemText_Base
    {
 
        private int m_nUserID = 0;
        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("userid"))
                return false;


            if (!int.TryParse(m_itemData.param["userid"], out m_nUserID))
            {
                m_nUserID = 0;
            }

            return true;
        }

		public override void OnDestroy()
		{

			m_nUserID = 0;
		}

		public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			// 触发好友聊天
			SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
            msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
            msgData.SessionID = m_nUserID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
        }
    }

    // 杯赛对象
    class ChatSubItemText_LegendCup : ChatSubItemText_Base
    {
        private long m_nLegendCupID = 0;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("cupid"))
                return false;

            if (!long.TryParse(m_itemData.param["cupid"], out m_nLegendCupID))
            {
                m_nLegendCupID = 0;
            }

            return true;
        }

		public override void OnDestroy()
		{
			m_nLegendCupID = 0;
		}

		public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			// 打开杯赛
			cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
            cupdata.nLegendCupID = m_nLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
        }
    }

	// 显示联盟窗口对象
	class ChatSubItemText_ShowClanWnd : ChatSubItemText_Base
	{
		public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
		{
			base.Init(obj, uiText, data);

			return true;
		}

		public override void OnPointerEnter(PointerEventData eventData)
		{
			m_UIText.color = Color.red;
		}

		public override void OnPointerExit(PointerEventData eventData)
		{
			m_UIText.color = m_defaultColor;
		}

		public override void OnPointerClick(PointerEventData eventData)
		{
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			// 显示联盟窗口
			UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_CLAN, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
		}
	}

    // 显示联盟窗口对象
    class ChatSubItemText_ShowCreateClanCupWnd : ChatSubItemText_Base
    {
        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            return true;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (GameLogicAPI.isInWarScene() > 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
                return;
            }

            // 显示联盟杯赛创建
            cmd_legendcup_req_create_cup_type cupType = new cmd_legendcup_req_create_cup_type();
            cupType.byLegendCupType = (byte)ELegendCupType.emCupType_Clan;	// 杯赛创建类型
            cupType.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_create_cup_type>(GameLogicDef.GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ, ref cupType);
        }
    }

	// 装备方案对象
	class ChatSubItemText_EquipScheme : ChatSubItemText_Base
	{
		int m_nHeroID = 0;
		string m_authorName = "";
		string m_schemeName = "";
		string m_schemeTitle = "";
		int[] m_equipList = null;


		public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
		{
			base.Init(obj, uiText, data);

			if (m_itemData.param == null
				|| !m_itemData.param.ContainsKey("heroid")
				|| !m_itemData.param.ContainsKey("author")
				|| !m_itemData.param.ContainsKey("name")
				|| !m_itemData.param.ContainsKey("title")
				|| !m_itemData.param.ContainsKey("equips")
				)
			{
				Debug.LogError("Params is invalid.");
				return false;
			}

			if(!int.TryParse(m_itemData.param["heroid"], out m_nHeroID)) 
			{
				m_nHeroID = 0;
			}
			m_authorName = m_itemData.param["author"];
			m_schemeName = m_itemData.param["name"];
			m_schemeTitle = m_itemData.param["title"];
			string equipStr = m_itemData.param["equips"];

			string[] equiplist = equipStr.Split(',');
			if (equiplist.Length != UDefines.nEquipSchemeMaxEquipNum)
			{
				Debug.LogError("equiplist.Length != UDefines.nEquipSchemeMaxEquipNum");
				return false;
			}

			m_equipList = new int[UDefines.nEquipSchemeMaxEquipNum];
			for (int i = 0; i < equiplist.Length; ++i)
			{
				if(!int.TryParse(equiplist[i], out m_equipList[i]))
				{
					m_equipList[i] = 0;
					Debug.LogError("equip id is invalid. equip id=" + equiplist[i]);
				}
			}

			return true;
		}

		public override void OnDestroy()
		{
			m_authorName = "";
			m_schemeName = "";
			m_schemeTitle = "";
			m_equipList = null;
		}

		public override void OnPointerEnter(PointerEventData eventData)
		{
			m_UIText.color = Color.red;
		}

		public override void OnPointerExit(PointerEventData eventData)
		{
			m_UIText.color = m_defaultColor;
		}

		public override void OnPointerClick(PointerEventData eventData)
		{
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			if (m_nHeroID <= 0)
			{
				Debug.LogError("m_nHeroID <= 0");
				return;
			}

			// 显示装备方案
			UEquipSchemeMsgData msgData = new UEquipSchemeMsgData();
			msgData.nHeroID = m_nHeroID;
			msgData.msgID = (int)WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO;
			msgData.scheme.nHeroID = m_nHeroID;
			msgData.scheme.nEquipIDList = m_equipList;
			msgData.scheme.szAuthorName= m_authorName;
			msgData.scheme.szSchemeName = m_schemeName;
			msgData.scheme.szTitle = m_schemeTitle;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO, msgData);
		}

		public static void ShareToInputField(int nHeroID, string authorName, string schemeName, string title, string equipString)
		{
			SChatMessageObjectInfo channelObj;
			channelObj.type = ChatItemType.ObjType_Text;
			channelObj.subType = ChatItemSubType.ObjSubType_Text_EquipScheme;
			channelObj.text = String.Format(ULocalizationService.Instance.Get("UIView", "EquipScheme", "EquipSchemeLink"),authorName);
			channelObj.param = new Dictionary<string, string>();
			channelObj.param.Add("heroid", nHeroID.ToString());
			channelObj.param.Add("author", authorName);
			channelObj.param.Add("name", schemeName);
			channelObj.param.Add("title", title);
			channelObj.param.Add("equips", equipString);

			ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
			uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
			uiData.objInfo = channelObj;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
		}
	}

    // 切换场景（换线）
    class ChatSubItemText_SwitchScene : ChatSubItemText_Base
    {
        int m_nWorldID = 0;
        int m_nServerID = 0;
        int m_nSceneID = 0;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null
                || !m_itemData.param.ContainsKey("worldid")
                || !m_itemData.param.ContainsKey("serverid")
                || !m_itemData.param.ContainsKey("sceneid")
                )
            {
                Debug.LogError("Params is invalid.");
                return false;
            }


            m_nWorldID = int.Parse(m_itemData.param["worldid"]);
            m_nServerID = int.Parse(m_itemData.param["serverid"]);
            m_nSceneID = int.Parse(m_itemData.param["sceneid"]);

            return true;
        }

        public override void OnDestroy()
        {
            m_nWorldID = 0;
            m_nServerID = 0;
            m_nSceneID = 0;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (GameLogicAPI.isInWarScene() > 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
                return;
            }

            gamelogic_migrate_switch_scene cmd = new gamelogic_migrate_switch_scene();
            cmd.nGameWorldID = m_nWorldID;
            cmd.nServerID = m_nServerID;
            cmd.nSceneID = m_nSceneID;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MIGRATE_SWITCH_SCENE, ref cmd);
        }

    }

    // 邀请一起游戏
    class ChatSubItemText_InvitePlayGame : ChatSubItemText_Base
    {
        int m_nRoomID = 0;
        string m_InviterName = "";
        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null
                || !m_itemData.param.ContainsKey("roomid")
                || !m_itemData.param.ContainsKey("inviter")
                )
            {
                Debug.LogError("Params is invalid.");
                return false;
            }


            if (!int.TryParse(m_itemData.param["roomid"], out m_nRoomID))
                Debug.LogError("Parse roomid failed. roomid=" + m_itemData.param["roomid"]);

            m_InviterName = m_itemData.param["inviter"];

            return true;
        }

        public override void OnDestroy()
        {
            m_nRoomID = 0;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (GameLogicAPI.isInWarScene() > 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
                return;
            }

            if(GameLogicAPI.getCurrentRoomID() > 0)
            {
                return;
            }

            cmd_customer_invite_respon respon = new cmd_customer_invite_respon();
            respon.nRoomID = m_nRoomID;
            respon.szPlayerName = m_InviterName;
            ViewEventHelper.Instance.SendCommand<cmd_customer_invite_respon>(GameLogicDef.GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON, ref respon);
        }

    }

    // 显示聊天窗口对象
    class ChatSubItemText_ShowChatWnd : ChatSubItemText_Base
	{
		int m_nUserID = 0;

		public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
		{
			base.Init(obj, uiText, data);

			if (m_itemData.param == null || !m_itemData.param.ContainsKey("userid"))
				return false;

			if (!int.TryParse(m_itemData.param["userid"], out m_nUserID))
			{
				m_nUserID = 0;
			}

			return true;
		}

		public override void OnDestroy()
		{
			m_nUserID = 0;
		}

		public override void OnPointerEnter(PointerEventData eventData)
		{
			m_UIText.color = Color.red;
		}

		public override void OnPointerExit(PointerEventData eventData)
		{
			m_UIText.color = m_defaultColor;
		}

		public override void OnPointerClick(PointerEventData eventData)
		{
			if (GameLogicAPI.isInWarScene() > 0)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
				return;
			}

			// 弹出好友聊天框
			SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
			msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
			msgData.SessionID = m_nUserID;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
		}
	}

    // 点击时邀请加入杯赛
    class ChatSubItemText_ClickInviteLegendCup : ChatSubItemText_Base
    {
        long m_lLegendCupID = 0;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null|| !m_itemData.param.ContainsKey("cupid"))
            {
                Debug.LogError("Params is invalid.");
                return false;
            }

            if (!long.TryParse(m_itemData.param["cupid"], out m_lLegendCupID))
                Debug.LogError("Parse cupid failed. cupid = " + m_lLegendCupID);

            return true;
        }

        public override void OnDestroy()
        {
            m_lLegendCupID = 0;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (GameLogicAPI.isInWarScene() > 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
                return;
            }

            if (GameLogicAPI.getCurrentRoomID() > 0)
            {
                return;
            }

            if (m_lLegendCupID <= 0)
            {
                return;
            }

            cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
            cupdata.nLegendCupID = m_lLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
        }
    }

    // 创建时邀请加入杯赛
    class ChatSubItemText_CreateInviteLegendCup : ChatSubItemText_Base
    {
        long m_lLegendCupID = 0;

        public override bool Init(ChatItemText obj, Text uiText, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("cupid"))
            {
                Debug.LogError("Params is invalid.");
                return false;
            }

            if (!long.TryParse(m_itemData.param["cupid"], out m_lLegendCupID))
                Debug.LogError("Parse cupid failed. cupid = " + m_lLegendCupID);

            return true;
        }

        public override void OnDestroy()
        {
            m_lLegendCupID = 0;
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            m_UIText.color = Color.red;
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            m_UIText.color = m_defaultColor;
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (GameLogicAPI.isInWarScene() > 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_NOTUSE_IN_WAR);
                return;
            }

            if (GameLogicAPI.getCurrentRoomID() > 0)
            {
                return;
            }

            if (m_lLegendCupID <= 0)
            {
                return;
            }

            cmd_legendcup_view_single_cup cupdata = new cmd_legendcup_view_single_cup();
            cupdata.nLegendCupID = m_lLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP, ref cupdata);
        }
    }

	class ChatSubItemImage_Base
    {
        protected ChatItemImage m_obj;
        protected SChatMessageObjectInfo m_itemData;
        protected RawImage m_UIImage;

		//protected Sprite[] m_animSprite;
		//protected int m_animCurFrame;
		//protected float m_animInterval;
		//protected float m_animCurTime;
		private ResNode resNode;

		public virtual bool Init(ChatItemImage obj, RawImage uiImage, SChatMessageObjectInfo data)
        {
            m_obj = obj;
            m_UIImage = uiImage;
            m_itemData = data;

            return true;
        }


		public virtual void SetResNode(ResNode rn)
		{
			resNode = rn;
		}

		public virtual ResNode GetResNode()
		{
			return resNode;
		}

		// 获取坐标组件
		public virtual RectTransform GetTransform()
        {
            return m_UIImage.rectTransform;
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            if(m_UIImage.texture != null)
                return m_UIImage.uvRect.width * m_UIImage.texture.width;
            return 1f;
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            if (m_UIImage.texture != null)
                return m_UIImage.uvRect.height * m_UIImage.texture.height;
            return 1f;
        }

        // 分割物件
        public virtual void SplitByWidth(float width, float offsetX, out IChatItem item1, out IChatItem item2)
        {
            float leftWidth = width - offsetX;
            if (!m_UIImage)
            {
                // 无效就全返回null
                item1 = null;
                item2 = null;
                return;
            }

            if (m_UIImage.rectTransform.sizeDelta.x < leftWidth)
            {
                item1 = m_obj;
                item2 = null;
            }
            else
            {
                item1 = null;
                item2 = m_obj;
            }

            return;
        }

        // 销毁物件
        public virtual void OnDestroy()
        {
        }

        // 分割物件
        public virtual bool SplitByIndex(int nIndex, out IChatItem item1, out IChatItem item2)
        {
            item1 = m_obj;
            item2 = null;
            return false;
        }

        // 复制物件
        public virtual IChatItem Clone()
        {
			//ChatBoxItemImage newItem = m_obj.Clone() as ChatBoxItemImage;

			ChatItemImage newItem = resNode.InstanceMainRes<ChatItemImage>();

			newItem.SetResNode(resNode);

            newItem.SetItemData(m_itemData);

            return newItem;
        }

        // 获取输入框用换算字符数
        public virtual int GetConvertCharCount()
        {
            return 1;
        }

        // 获取指定字符处的起始位置
        public virtual float GetCharStart(int index)
        {
            return 0f;
        }

        // 获取指定字符处的宽度
        public virtual float GetCharWidth(int index)
        {
            return m_UIImage.rectTransform.sizeDelta.x;
        }

        // 获取指定字符处的实际字符串
        public virtual String GetCharString(int beginIndex, int endIndex)
        {
            String strResult = "" + ChatMessageManager.ChatItemFlag;
            String typeName = ChatItem.GetChatObjSubTypeString(m_itemData.subType, false);
            strResult += String.Format("[{0}", typeName);
            if (m_itemData.param != null)
            {
                foreach (var data in m_itemData.param)
                {
                    strResult += String.Format(" {0}={1}", data.Key, data.Value);
                }
            }
            strResult += String.Format("][/{0}]", typeName);
            return strResult;
        }

        // 删除指定位置字符
        public virtual bool DeleteChar(int index, int count)
        {
            return false;
        }

        // 插入指定位置字符
        public virtual bool InsertChar(int index, String text)
        {
            return false;
        }

        public virtual void OnUpdate()
        {
            //if (m_animSprite != null && m_animSprite.Length > 0 && m_animInterval > 0f && Time.deltaTime > 0f)
            //{
            //    m_animCurTime += Time.deltaTime;
            //    if (m_animCurTime > m_animInterval)
            //    {
            //        int addFrame = (int)(m_animCurTime / m_animInterval);
            //        m_animCurTime -= addFrame * m_animInterval;
            //        m_animCurFrame += addFrame;
            //        if (m_animCurFrame > m_animSprite.Length)
            //            m_animCurFrame = m_animCurFrame % m_animSprite.Length;

            //        m_UIImage.sprite = m_animSprite[m_animCurFrame];
            //    }
            //}
        }
    }

    class ChatSubItemImage_Emotion : ChatSubItemImage_Base
    {
		UFlipBook m_flipBookAnimator = null;
		UTooltipTrigger m_tipTrigger = null;
		private int emotionID = 0;
        private string animKey = "";
        private float width;
        private float height;

        public override bool Init(ChatItemImage obj, RawImage uiImage, SChatMessageObjectInfo data)
        {
            base.Init(obj, uiImage, data);

            if (data.param == null || data.param.ContainsKey("animkey") == false)
                return false;
            
            animKey = data.param["animkey"];

            if (data.param.ContainsKey("id"))
                emotionID = Convert.ToInt32(data.param["id"]);

            if (emotionID > 0)
            {
                string tipdesc = Data.ChatEmoticonConfig.ChatEmoticonConfig.Instance.GetEmoticonTipDesc(emotionID);
                if (tipdesc != string.Empty)
                {
					//m_tipTrigger = obj.gameObject.GetComponent<UTooltipTrigger>();
					//if(m_tipTrigger == null)
					//m_tipTrigger = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_ChatEmotion);

					m_tipTrigger = obj.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_ChatEmotion);

					if (m_tipTrigger != null)
						m_tipTrigger.SetText("bodyTxt", tipdesc);
                }
            }

            //UFlipBook flipBookAnimator = obj.gameObject.GetComponent<UFlipBook>();
			//if(flipBookAnimator == null)
				//flipBookAnimator = obj.gameObject.AddComponent<UFlipBook>();
			m_flipBookAnimator = obj.gameObject.AddComponent<UFlipBook>();

			if(m_flipBookAnimator != null)
				m_flipBookAnimator.Init(animKey);
            
            return true;
        }

		public override void OnDestroy()
		{
			if(m_flipBookAnimator != null)
			{
				m_obj.gameObject.RemoveComponent<UFlipBook>();
				m_flipBookAnimator = null;
			}

			if(m_tipTrigger != null)
			{
				m_obj.gameObject.RemoveComponent<UTooltipTrigger>();
				m_tipTrigger = null;
			}
			emotionID = 0;
			animKey = "";
		}

	}

	public class ChatItemImage : MonoBehaviour, IChatItem
    {
        private ChatSubItemImage_Base m_subItem;

        private ResNode resNode;

		public virtual void Clear()
		{

		}

        public virtual void SetResNode(ResNode rn)
        {
            resNode = rn;
        }

        public virtual ResNode GetResNode()
        {
            return resNode;
        }

        // 获取物件类型
        public virtual ChatItemType GetItemType()
        {
            return ChatItemType.ObjType_Image;
        }

        // 设置物件内容
        public virtual bool SetItemData(SChatMessageObjectInfo data)
        {
            switch (data.subType)
            {
                case ChatItemSubType.ObjSubType_Image_Emotion:
                    m_subItem = new ChatSubItemImage_Emotion();
                    break;
                default:
                    return false;
            }

			m_subItem.SetResNode(resNode);

            RawImage uiImage = GetComponent<RawImage>();
            return m_subItem.Init(this, uiImage, data);
        }

        // 获取坐标组件
        public virtual RectTransform GetTransform()
        {
            return m_subItem.GetTransform();
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            return m_subItem.GetItemWidth();
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            return m_subItem.GetItemHeight();
        }

        // 分割物件
        public virtual void SplitByWidth(float width, float offsetX, out IChatItem item1, out IChatItem item2)
        {
            m_subItem.SplitByWidth(width, offsetX, out item1, out item2);
        }

        // 销毁物件
        public virtual void Destroy()
        {
            m_subItem.OnDestroy();
            var temp1 = gameObject;
            ResNode.DestroyRes(ref temp1);
        }

        // 分割物件
        public virtual bool SplitByIndex(int nIndex, out IChatItem item1, out IChatItem item2)
        {
            return m_subItem.SplitByIndex(nIndex, out item1, out item2);
        }

        // 复制物件
        public virtual IChatItem Clone()
        {
            return m_subItem.Clone();
        }

        // 获取输入框用换算字符数
        public virtual int GetConvertCharCount()
        {
            return m_subItem.GetConvertCharCount();
        }

        // 获取指定字符处的起始位置
        public virtual float GetCharStart(int index)
        {
            return m_subItem.GetCharStart(index);
        }

        // 获取指定字符处的宽度
        public virtual float GetCharWidth(int index)
        {
            return m_subItem.GetCharWidth(index);
        }

        // 获取指定字符处的实际字符串
        public virtual String GetCharString(int beginIndex, int endIndex)
        {
            return m_subItem.GetCharString(beginIndex, endIndex);
        }

        // 删除指定位置字符
        public virtual bool DeleteChar(int index, int count)
        {
            return m_subItem.DeleteChar(index, count);
        }

        // 插入指定位置字符
        public virtual bool InsertChar(int index, String text)
        {
            return m_subItem.InsertChar(index, text);
        }

        public void Update()
        {
            m_subItem.OnUpdate();
        }
    }
}
