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
using USpeedUI.Chat;

namespace USpeedUI.ChatBox
{
    public class ChatBoxWnd : UIBaseWnd<ChatBoxWndView>
    {
        private bool isEnterWarLoading = false;

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_CHATBOX;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Chat/ChatBoxView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }


        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_UPDATEMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGECHANNEL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGEIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_MOVEPOSUP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_MOVEPOSDOWN, this); 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CLOSEEMOTICONPANEL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_UPDATELOCKSTATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_UPDATE_MAIN_HERO_VOCATION, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_TOGGLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_RESETSORTORDER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBOX_SENDCHATMESSAGE, this);

            // 订阅按键消息
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey1, KeyStatus.Down, this);
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey2, KeyStatus.Down, this);
            return true;
        }
        
        // 销毁
        public override void Destroy()
        {
            base.Destroy(); 

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_UPDATEMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGECHANNEL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CHANGEIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_MOVEPOSUP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_MOVEPOSDOWN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_CLOSEEMOTICONPANEL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_UPDATELOCKSTATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_UPDATE_MAIN_HERO_VOCATION, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_TOGGLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_RESETSORTORDER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBOX_SENDCHATMESSAGE, this);

            // 退订按键消息
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey1, KeyStatus.Down, this);
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey2, KeyStatus.Down, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        SetVisible(false);
						if (m_wndView != null)
							m_wndView.clearAllMsgData();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                            m_wndView.OnEnter(ChatBoxState.StaticState);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnLeave(ChatBoxState.StaticState);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            UIMsgCmdData uiRoomConfirm = (UIMsgCmdData)msgData;
                            cmd_match_hall_add_room data = IntPtrHelper.toData<cmd_match_hall_add_room>(uiRoomConfirm.ptrParam);

                            m_wndView.RoomMatchType = data.nMatchTypeID;
                            m_wndView.OnEnter(ChatBoxState.RoomState);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE:
                    {
                        if (m_wndView != null)
                        {
                            //if(isEnterWarLoading == false)
                            {
                                m_wndView.OnLeave(ChatBoxState.RoomState);
                            }

                            //if(isEnterWarLoading)
                            //{
                            //    m_wndView.OnEnter(ChatBoxState.WarLoadingState);

                            //    isEnterWarLoading = false;
                            //}
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        // 因为WND_MSG_COMMON_LOGINMOBA比WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE更早，所以用这种方式
                        isEnterWarLoading = true;

                        if (m_wndView != null)
                            m_wndView.OnEnter(ChatBoxState.WarLoadingState);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                            m_wndView.OnEnter(ChatBoxState.WarState);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnLeave(ChatBoxState.WarState);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_UPDATEMESSAGE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnChatMessageUpdate();
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_CHANGECHANNEL:
                    {
                        ChatBoxChangeChannelData data = msgData as ChatBoxChangeChannelData;
                        if (m_wndView != null)
                            m_wndView.ChangeChannel(data.channel);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM:
                    {
                        if (m_wndView != null)
                            m_wndView.OnInputfieldAddItem(msgData as ChatBoxAddInputFieldItemData);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE:
                    {
                        if (m_wndView != null)
                            m_wndView.SetForceConditionHide(msgData as ChatForceConditionHide);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_CHANGEIME:
                    {
                        if (m_wndView != null)
                            m_wndView.ChangeIme(msgData as ChatBoxChangeImeData);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT:
                    {
                        if (m_wndView != null)
                            m_wndView.selectPrivateChat(msgData as ChatBoxPrivateChat);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_MOVEPOSUP:
                    {
                        if (m_wndView != null)
                            m_wndView.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEUP);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_MOVEPOSDOWN:
                    {
                        if (m_wndView != null)
                            m_wndView.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEDOWN);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_CLOSEEMOTICONPANEL:
                    {
                        if (m_wndView != null)
                            m_wndView.CloseEmoticonPanel();
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_UPDATELOCKSTATE:
                    {
                        if (m_wndView != null)
                            m_wndView.UpdateEmoticonGroupLockState(msgData as UChatEmoticonGroupState);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_UPDATE_MAIN_HERO_VOCATION:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetFpsSwitchHint(((OperationType)msgData).operationType);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE:
                    {
                        if (m_wndView != null)
                            m_wndView.onPopWndVisible(msgData as UCommonPopupWndVisible);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE:
                    {
                        if (m_wndView != null)
                            m_wndView.onFullWndVisible(msgData as UCommonFullWndVisible);
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_TOGGLE:
                    {
                        if (m_wndView != null)
                            m_wndView.ToggleHideWindow();
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_RESETSORTORDER:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.transform.SetAsLastSibling();
                            m_wndView.gameObject.SetActive(true);
                            resetAllSortingOrder();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_CHATBOX_SENDCHATMESSAGE:
                    {
                        if (m_wndView != null)
                            m_wndView.onSendChatMessage(msgData as USendChatMessage);
                    }
                    break;
                default:
                    break;
            }
        }

        // 接受按键消息
        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null)
            {
                return base.OnKeyMessage(index, status);
            }

            if (status == KeyStatus.Down)
            {
                if ((index == FuntionShortCutSlotIndex.ChatBoxActiveKey1 || index == FuntionShortCutSlotIndex.ChatBoxActiveKey2))
                {
                    return m_wndView.OnPressReturn();
                }
            }

            return base.OnKeyMessage(index, status);
        }

        //重新算该层级下所有界面的层级数
        public void resetAllSortingOrder()
        {
            USpeedUILayer layer = USpeedUILayerManager.GetUILayer(GetLayer());
            if (layer != null)
            {
                RectTransform LayerTrans = layer.GetRectTransform;
                if (LayerTrans != null)
                {
                    int count = LayerTrans.childCount;
                    for (int i = 0; i < count; ++i)
                    {
                        USpeedUILayerElement element = LayerTrans.GetChild(i).GetComponent<USpeedUILayerElement>();
                        if (element != null)
                            LayerTrans.GetChild(i).GetComponent<USpeedUILayerElement>().SetSortingOrder();
                    }
                }
            }
        }
    }

    public enum ChatChannelGroupType
    {
        ChatChannel_All,
        ChatChannel_Clan,
        ChatChannel_Kin,
        ChatChannel_Team,
        ChatChannel_Zone,
        ChatChannel_Room,
        ChatChannel_Personal,  // 个人频道
        ChatChannel_Public,    // 公共频道
        ChatChannel_Diy,       // 自定义频道
        ChatChannel_WarScene,  // 战场频道
    }

    public enum ChatPositionChangeType
    {
        CPCY_MOVEUP = 1,
        CPCY_MOVEDOWN = 2,
    }

    #region 自定义频道
    public class ChatDiyChannelGroup : MonoBehaviour
    {
        public GameObject m_ChannelGroup;

        private ChatBoxWndView m_wndView = null;
        private ResNode m_diyChannelItemPrefabRes = null;

        static private Dictionary<String, int> m_ChannelNameMap = new Dictionary<string, int>{
                {"System",(int)EMChatChannelType.CHAT_CHANNEL_SYSTEM},
                {"World",(int)EMChatChannelType.CHAT_CHANNEL_WORLD},
                {"MatchRoom",(int)EMChatChannelType.CHAT_CHANNEL_MATCHROOM},
                {"TeamMate",(int)EMChatChannelType.CHAT_CHANNEL_TEAMMATE},
                {"WarScene",(int)EMChatChannelType.CHAT_CHANNEL_WARSCENE},
                {"Private",(int)EMChatChannelType.CHAT_CHANNEL_PRIVATE},
                {"Kin",(int)EMChatChannelType.CHAT_CHANNEL_KIN},
                {"Clan",(int)EMChatChannelType.CHAT_CHANNEL_CLAN},
                {"Nearby",(int)EMChatChannelType.CHAT_CHANNEL_NEARBY},
                {"Zone",(int)EMChatChannelType.CHAT_CHANNEL_ZONE},
                {"Camp",(int)EMChatChannelType.CHAT_CHANNEL_CAMP},
            };

        public void Init()
        {

            m_diyChannelItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/Chat/ChatDiyChannelItem");
            if (m_diyChannelItemPrefabRes == null)
            {
                return;
            }


            foreach (var item in m_ChannelNameMap)
            {
                String channelName = item.Key;
                GameObject ob = m_diyChannelItemPrefabRes.InstanceMainRes();
                ob.transform.SetParent(m_ChannelGroup.transform);
                ob.name = item.Key;
                ob.transform.FindChild("ChannelName").GetComponent<Text>().text = ChatBoxWndView.getChannelName((EMChatChannelType)item.Value);


                Toggle tg = ob.GetComponent<Toggle>();

                // 初始化复选框状态
                initChannelGroup(channelName, ref tg);

                // 设置复选框状态变化回调
                tg.onValueChanged.AddListener(delegate(bool check)
                {
                    this.onValueChanged(check, ob);
                });
            }
        }

        public void onValueChanged(bool check, GameObject sender)
        {
            int uChannelMask = m_wndView.DiyChannelMask;
            int channel = m_ChannelNameMap[sender.name];

            if (check)
            {
                // 添加频道
                uChannelMask |= ((int)1 << channel);
            }
            else
            {
                // 删除频道
                int tmpMask = (1 << channel);
                tmpMask ^= ~0;
                uChannelMask &= tmpMask;
            }

            if (m_wndView == null)
            {
                return;
            }

            m_wndView.updateDiyChannelMask(uChannelMask);
        }

        private void initChannelGroup(string name, ref Toggle tg)
        {
            int uChannelMask = m_wndView.DiyChannelMask;
            int channel = m_ChannelNameMap[name];
            if ((uChannelMask & (1u << channel)) > 0)
            {
                tg.isOn = true;
            }
        }

        public void setWndView(ChatBoxWndView wndView)
        {
            m_wndView = wndView;
        }

        public void Destroy()
        {
            AssetBundleManager.DeleteAssets(ref m_diyChannelItemPrefabRes, true);
        }
    }
    #endregion

    public class ChatChannelGroupItem : MonoBehaviour
    {
        public Text itemText;
        private ChatBoxWndView m_wndView;
        private UButtonGroupItem m_buttonGroupItem;
        private int m_nChannelMask;
        private ChatChannelGroupType m_ChannelGroupType;

		private static Color m_defaultColor = Color.white;
		private static Color m_hightLightColor = new Color(255f / 255f, 202f / 255f, 39f / 255f);

		public void Clear()
		{
			m_buttonGroupItem.Clear();
			itemText.color = m_defaultColor;
		}

        public void Init(ChatBoxWndView wndView, ChatChannelGroupType nType)
        {
            m_wndView = wndView;
            m_ChannelGroupType = nType;
            transform.SetParent(wndView.channelMaskButtonFrame, false);
			m_buttonGroupItem = GetComponent<UButtonGroupItem>();
			if (m_buttonGroupItem == null)
			{
				Debug.LogError("ChatBoxWndView::Init::m_buttonGroupItem == null");
				return;
			}

            m_buttonGroupItem.Group = wndView.channelMaskButtonGroup;
            m_buttonGroupItem.onSelectedChanged.AddListener(onSelectedChanged);
            m_buttonGroupItem.onDoubleClick.AddListener(onDoubleClick);
            m_buttonGroupItem.onRightClick.AddListener(onRightClick);
            m_nChannelMask = 0;


            switch (nType)
            {
                case ChatChannelGroupType.ChatChannel_Clan:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "ClanChannelName");
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_CLAN);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Kin:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "KinChannelName");
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_KIN);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Team:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "TeammateChannelName");
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_TEAMMATE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Zone:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "WarSceneChannelName");
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_TEAMMATE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_ZONE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Room:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "MatchroomChannelName");
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_MATCHROOM);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_CAMP);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Personal:
                    itemText.text = "个人";
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_CLAN);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_KIN);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_PRIVATE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_TEAMMATE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_CAMP);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Public:
                    itemText.text = "公共";
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_WORLD);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_ZONE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_NEARBY);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                case ChatChannelGroupType.ChatChannel_Diy:
                    itemText.text = "自定义";
                    break;
                case ChatChannelGroupType.ChatChannel_WarScene:
                    itemText.text = "战场";
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_CAMP);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_WARSCENE);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_NEARBY);
                    m_nChannelMask |= (1 << (int)EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                    break;
                default:
                    itemText.text = ULocalizationService.Instance.Get("UIView", "ChatBox", "ChatChannelGroupAll");
                    m_nChannelMask = ~0;
                    break;
            }
        }

        public void onSelectedChanged(bool bActive)
        {
            if (!bActive)
			{
				itemText.color = m_defaultColor;

				// 隐藏自定义频道窗口
				if (m_ChannelGroupType == ChatChannelGroupType.ChatChannel_Diy)
				{
					m_wndView.hideDiyChannelWnd();
				}
			}
			else
			{
				itemText.color = m_hightLightColor;
				LogicDataCenter.chatMessageManager.SetChannelMask(m_nChannelMask);
			}
        }

        /// <summary>
        /// 关闭或打开自定义频道窗口
        /// </summary>
        /// <param name="bActive"></param>
        private void updateDiyChannelWnd(bool bActive)
        {
            if (bActive && m_ChannelGroupType == ChatChannelGroupType.ChatChannel_Diy)
            {
                m_wndView.updateDiyChannelWnd();
            }
        }

        public void onDoubleClick(UButtonGroupItem item, bool bActive)
        {
            //updateDiyChannelWnd(bActive);
        }

        public void onRightClick(UButtonGroupItem item, bool bActive)
        {
            updateDiyChannelWnd(bActive);
        }

        public void setChannelMask(int channelMask)
        {
            m_nChannelMask = channelMask;
        }
    }

    public class ChatMinimumIcon : MonoBehaviour, IPointerClickHandler
    {
        public UTooltipTrigger tooltip;
        private ChatBoxWndView m_chatWndView;

        public void Start()
        {
            tooltip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "ChatBox", "ChatMinimumTip"));
        }

        public void SetChatWndView(ChatBoxWndView wndView)
        {
            m_chatWndView = wndView;
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            if (m_chatWndView == null)
                return;

            if (eventData.button == PointerEventData.InputButton.Left)
            {
                m_chatWndView.contentView.GetScrollRect().normalizedPosition = new Vector2(0f, 0f);
            }
            else if (eventData.button == PointerEventData.InputButton.Right)
            {
                m_chatWndView.ToggleHideWindow();
            }
        }
    }

    public class ChatEmoticonPanel : MonoBehaviour
    {
        public GameObject[] emoticonGroupGO;
        public UButtonGroup emoticonTabGroup;
        public RectTransform[] emoticonTabPageRTList;

        private Dictionary<int, ChatEmoticonGroup> m_chatEmoticonGroupList;
        private ChatBoxWndView m_chatWndView;

        public void Init()
        {
            InitData();
            this.gameObject.SetActive(false);
            emoticonTabGroup.onButtonGroupSelectedChange.RemoveListener(OnTabGroupChanged);
            emoticonTabGroup.onButtonGroupSelectedChange.AddListener(OnTabGroupChanged);
        }

        public void SetChatWndView(ChatBoxWndView wndView)
        {
            m_chatWndView = wndView;
        }

        public void SwitchEmotionPanelActiveState()
        {
            this.gameObject.SetActive(!this.gameObject.activeInHierarchy);

            // 表情栏打开的时候不隐藏聊天框
            m_chatWndView.bAutoHide = !gameObject.activeInHierarchy;
        }

        public void UpdateEmoticonGroupLockState(List<int> unLockGroup)
        {
            if (unLockGroup == null)
                return;

            foreach(KeyValuePair<int, ChatEmoticonGroup> kvp in m_chatEmoticonGroupList)
            {
                bool bOwn = unLockGroup.Contains(kvp.Key);
                kvp.Value.SetLockState(!bOwn);
            }
        }

        public void OnTabGroupChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            if (newItem == null)
                return;

            if(oldItem == null)
            {
                oldItem = emoticonTabGroup.GetItemAt(0);
                if (oldItem == null)
                    return;
            }

            emoticonTabPageRTList[oldItem.GroupIndex].gameObject.SetActive(false);
            emoticonTabPageRTList[newItem.GroupIndex].gameObject.SetActive(true);

            oldItem.GetComponent<Image>().color = new Color(1, 1, 1, 0);
            newItem.GetComponent<Image>().color = new Color(1, 1, 1, 1);
        }

        private void InitData()
        {
            m_chatEmoticonGroupList = new Dictionary<int, ChatEmoticonGroup>();

            Dictionary<int, List<SSchemeChatEmoticonData>> chatEmoticonMap = ChatEmoticonConfig.Instance.ChatEmoticonSerialList;
            if (chatEmoticonMap == null)
                return;

            foreach(KeyValuePair<int, List<SSchemeChatEmoticonData>> kvp in chatEmoticonMap)
            {
                InitEmoticonGroup(kvp.Key, kvp.Value);
            }
        }

        private void InitEmoticonGroup(int serialID, List<SSchemeChatEmoticonData> emoticonList)
        {
            int index = serialID == 1 ? 0 : 1;
            if (emoticonList == null || emoticonList.Count < 1)
                return;

            if (emoticonGroupGO[index] == null || emoticonTabPageRTList[index] == null)
                return;

            GameObject groupGO = ResNode.InstantiateRes(emoticonGroupGO[index]);
            if (groupGO == null)
                return;

            ChatEmoticonGroup emoGroup = groupGO.GetComponent<ChatEmoticonGroup>();
            if (emoGroup == null)
                return;

            groupGO.transform.SetParent(emoticonTabPageRTList[index], false);
            groupGO.SetActive(true);

            if (emoticonList[0] == null)
                return;

            if (!emoGroup.Init(serialID, emoticonList[0].nEmoticonSellID, true))
                return;

            foreach(SSchemeChatEmoticonData emoData in emoticonList)
            {
                emoGroup.AddItem(emoData);
            }

            if (!m_chatEmoticonGroupList.ContainsKey(serialID))
                m_chatEmoticonGroupList.Add(serialID, emoGroup);

            emoGroup.SetLockState(LogicDataCenter.emojiDataManager.CheckEmojiGroupLocked(serialID));
        }
    }
}
