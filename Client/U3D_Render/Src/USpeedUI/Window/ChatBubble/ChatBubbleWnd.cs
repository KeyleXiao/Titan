using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using GameLogic;
using UnityEngine;
using UnityEngine.EventSystems;
using DataCenter;
using Stage_ManagerDef;
using System.Collections;
using UIWidgets;
using USpeedUI.UExtensions;
using Data.ChatEmoticonConfig;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.ChatSettingConfig;
using DG.Tweening;
using U3D_Render;
using USpeedUI.Chat;

namespace USpeedUI.ChatBubble
{
	public class ChatBubbleWnd : UIBaseWnd<ChatBubbleWndView>
	{
		// 获取ID
		public override WndID GetID()
		{
			return WndID.WND_ID_CHATBUBBLE;
		}

		// 获取资源路径
		public override String GetPrefabPath()
		{
			return "ChatBubble/ChatBubbleWndView";
		}

		// 获取所在层次
		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP01;
		}

		// 初始化
		public override bool Init()
		{
			base.Init();

			// 订阅消息
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_NEW_MESSAGE, this);

			return true;
		}

		// 销毁
		public override void Destroy()
		{
			base.Destroy();

			// 退订消息
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_NEW_MESSAGE, this);
		}

		// 接受消息
		public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
		{
			switch (msgID)
			{
				case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
					{
						SetVisible(true);
					}
					break;
				case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
					{
						UnloadView();
					}
					break;
				case WndMsgID.WND_MSG_CHATBUBBLE_NEW_MESSAGE:
					{
						if (m_wndView != null)
							m_wndView.onNewMessage(msgData as UChatBubbleMsgData);
					}
					break;
				default:
					break;
			}
		}
	}

	public class ChatBubbleWndView : UIBaseWndView
	{
		public GameObject DefaultItem;

		// 聊天气泡<uid, 气泡>
		private Dictionary<uint, ChatBubbleItem> m_ItemList;

		public override bool Init(IUIWnd wnd)
		{
			base.Init(wnd);

			m_ItemList = new Dictionary<uint, ChatBubbleItem>();

			return true;
		}

		public override void Destroy()
		{
			base.Destroy();

			m_ItemList.Clear();
		}

		public void onNewMessage(UChatBubbleMsgData data)
		{
			if (data == null)
				return;

			uint uid = data.uid;

			// 判断是否在视野范围内
			if (ChatBubbleItem.isEntityDisappear(uid))
				return;
			
			ChatBubbleItem tmpItem;
			if (!m_ItemList.TryGetValue(uid, out tmpItem))
			{
				tmpItem = ResNode.InstantiateRes<ChatBubbleItem>(DefaultItem);
				if (tmpItem == null)
					return;

				tmpItem.Init();

				tmpItem.transform.SetParent(this.transform, false);

                tmpItem.bg.gameObject.SetActive(data.isShowBg);

				m_ItemList.Add(uid, tmpItem);
			}

			if(tmpItem.setData(this, uid, data.info))
			{
				tmpItem.gameObject.SetActive(true);
			}
			else
			{
				removeChatBubbleItem(uid);
			}
		}

		// 移除泡泡
		public void removeChatBubbleItem(uint uid)
		{
			ChatBubbleItem tmpItem;
			if (m_ItemList.TryGetValue(uid, out tmpItem))
			{
				tmpItem.Clear();

				m_ItemList.Remove(uid);

				GameObject go = tmpItem.gameObject;
				ResNode.DestroyRes(ref go);
			}
		}
	}

	public class ChatBubbleItemBase : MonoBehaviour
	{
        public Image bg;

		// 气泡显示时间
		public float ShowTime = 5f;

		// 最大宽度
		public float MaxWidth = 200f;

		protected bool m_isClosed = true;

		// 开始显示时间
		private float m_startTime = 0;

		public virtual void Init()
		{

		}

		public virtual void Clear()
		{
			m_startTime = 0;
		}

		public virtual void Close()
		{
			gameObject.SetActive(false);
			m_isClosed = true;
		}

		public virtual void Update()
		{
			// 判断是否消失
			if (Time.time - m_startTime > ShowTime)
				Close();
		}

		public virtual void setData(SChatMessageInfo info)
		{
			ChatContentItemData itemData = new ChatContentItemData();
			itemData.objList = info.objList;
			itemData.Height = 0;

			// 设置最大宽度
			RectTransform rect = transform as RectTransform;
			rect.sizeDelta = new Vector2(MaxWidth, rect.sizeDelta.y);

			ChatContentItem contentItem = gameObject.GetComponent<ChatContentItem>();
			if (contentItem != null)
				contentItem.SetData(itemData);
			else
			{
				Debug.LogError("contentItem == null, This GameObject have no ChatContentItem component.");
			}

			m_startTime = Time.time;

			m_isClosed = false;
		}
	}

	public class ChatBubbleItem : ChatBubbleItemBase
	{
		// 玩家UID
		private uint m_UID = 0;
		EntityView m_EntityView = null;
		private CreatureProperty m_EntryProperty = null;
		private ChatBubbleWndView m_View = null;

		public override void Init()
		{
			base.Init();
		}
		
		public override void Clear()
		{
			base.Init();

			m_View = null;
			m_UID = 0;
			m_EntityView = null;
			m_EntryProperty = null;
		}

		public override void Update()
		{
			base.Update();

			// 根据实体位置来实时更新气泡位置
			if(!m_isClosed)
				updatePosition();
		}

		public override void Close()
		{
			base.Close();

			if (m_View != null)
			{
				m_View.removeChatBubbleItem(m_UID);
			}
			else
			{
				Debug.LogError("m_View == null");
			}
		}

		public bool setData(ChatBubbleWndView view, uint uid, SChatMessageInfo info)
		{
			base.setData(info);

			m_View = view;
			m_UID = uid;

			// 实体气泡,则更新位置
			m_EntityView = EntityFactory.getEntityViewByID((int)uid);
			if (m_EntityView == null || !m_EntityView.IsValid)
			{
				return false;
			}

			m_EntryProperty = m_EntityView.GetComponent<CreatureProperty>();
			if (m_EntryProperty == null)
			{
				return false;
			}

			// 设置初始位置
			updatePosition();

			return true;
		}

		// 设置位置
		private void updatePosition()
		{
			// 实体消失,则关闭气泡
			if(isEntityDisappear(m_UID))
			{
				Close();
				return;
			}

			// 是否在相机的视窗内
			if (!CameraUtil.isOnCameraView(getChatBubblePos(m_UID)))
			{
				Close();
				return;
			}

			Vector3 pos = getChatBubblePos(m_UID);

			Vector3 screenPos = WorldToUIPoint(UISystem.Instance.GetCanvas(), pos);
			//screenPos.x += 50;
			//screenPos.y += 50;

			transform.GetComponent<RectTransform>().SetPosition(screenPos);
		}

		// 获取实体位置
		private Vector3 getEntityPos(uint uid)
		{
			return m_EntityView.gameObject.transform.position;
		}

		// 获取实体位置
		private Vector3 getChatBubblePos(uint uid)
		{
			Vector3 pos = getEntityPos(m_UID);

			// 加上人物高度和0.5米偏移
			pos.y += m_EntryProperty.CreatureHeightInMeters + 0.5f;

			return pos;
		}


		private static Vector3 WorldToUIPoint(Canvas canvas, Vector3 pos)
		{
			Vector3 worldPos = Camera.main.WorldToScreenPoint(pos);
			Vector3 screenPos = canvas.worldCamera.ScreenToWorldPoint(worldPos);

			Vector3 result = new Vector3(screenPos.x, screenPos.y, canvas.GetComponent<RectTransform>().anchoredPosition3D.z);

			return result;
		}

		// 实体是否消失,不再视野范围内
		public static bool isEntityDisappear(uint uid)
		{
			EntityView ev = EntityFactory.getEntityViewByID((int)uid);

			return ev == null;
		}
	}
}