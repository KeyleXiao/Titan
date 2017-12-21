/*******************************************************************
** 文件名:	ChatItem.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	李有红
** 日  期:	2017/8/28
** 版  本:	1.0
** 描  述:	聊天框实现
** 应  用:  
*****************************************************************/
//#define FUNCTION_AUTO_ADD_CHAT_OPEN  

using System;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using Stage_ManagerDef;
using System.Collections;
using UIWidgets;
using USpeedUI.UExtensions;
using ASpeedGame.Data.ChatSettingConfig;
using USpeedUI.Chat;
using GameLogic;
using Data.ChatEmoticonConfig;

namespace USpeedUI.ChatBox
{
    public partial class ChatBoxWndView : UIBaseWndView, ITimerHanlder
    {
        public CanvasGroup contentCanvasGroup;
        public CanvasGroup borderCanvasGroup;
        public CanvasGroup channelCanvasGroup;
        public Image resizerImage;
        public ChatBoxResizer resizer;

        // 聊天内容
        public ChatBoxContentView contentView;
        public float hideContentTime;
        public ChatMinimumIcon minimumIcon;

        // 最小化按钮
        private CanvasGroup minimunCanvasGroup;

        // 聊天外框
        public float hideFrameTime;

        // 频道选择
        public Dropdown channelDropdown;
        private EMChatChannelType[] m_dropDownIndexToChannel;
        private int m_curSelChannel;
        public UButtonGroup channelMaskButtonGroup;
        public RectTransform channelMaskButtonFrame;
        private ResNode channelMaskItemPrefabRes;

        // 输入框
        public ChatBoxInputField inputField;
        private bool m_isInputFieldFocus;

        //输入法
        public Dropdown imeDropdown;
        public Image chineseIcon;
        public Image enIcon;

        //表情
        public Button emoticonBtn;
        public ChatEmoticonPanel emoticonPanel;

        /*FPS英雄鼠标切换提示*/
        public Image fpsSwitchHint;

        private Vector2 fpsHintInitPosition;

        // 鼠标是否在窗口内
        private float m_nextCheckMouseInWndTime;
        private bool m_disableFade;
        // 是否隐藏外框
        private bool m_hideFrame;
        private float m_hideFrameAlpha;
        private float m_showFrameTime = 0f;

        // 是否隐藏内容
        private bool m_hideContent;
        private float m_hideContentAlpha;
        private float m_showContentTime;

        // 聊天框的位置
        private Vector3 m_UpPos;
        private Vector3 m_DownPos;

        // 自定义频道
        public ChatDiyChannelGroup m_DiyChannelGroup;
        private ChatChannelGroupItem m_DiyChannelGroupItem = null;
        private int m_DiyChannelMask = 0;
        public int DiyChannelMask { get { return m_DiyChannelMask; } }

        // 聊天框初始大小
        Vector2 m_InitSize = Vector2.zero;
        // 聊天框主城非活动时候的大小
        public float m_UnActiveWidth;
        public float m_UnActiveHeight;

        // 是否最小化
        private bool m_isMin;

        // 主城弹窗状态,<窗口ID,是否可见>
        private Dictionary<int, bool> m_PopupWndStatus;

        // 主城全屏状态,<窗口ID,是否可见>
        private Dictionary<int, bool> m_fullWndStatus;

        // 频道名字缓存
        static private Dictionary<EMChatChannelType, String> m_dicChannnelName = new Dictionary<EMChatChannelType, String>();

        // 聊天框当前状态
        private ChatBoxBase m_ChatBoxState = null;

        // 当前房间类型
        public int RoomMatchType { set { m_nMatchType = value; } }
        private int m_nMatchType;

        public enum EChatTimer
        {
            EChatTimer_ShowChatMsg = 1,     // 显示聊听消息定时器
#if FUNCTION_AUTO_ADD_CHAT_OPEN
            EChatTimer_AddChatMsg = 2,     // 加入聊听消息定时器
#endif
        }


        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            // 从脚本配置中获取各个频道名字，缓存起来，与频道宏定义做个映射，方便读取
            string[] channelName = { "SystemChannelName",
                                      "WorldChannelName",
                                      "MatchroomChannelName",
                                      "TeammateChannelName",
                                      "WarSceneChannelName",
                                      "PrivateChannelName",
                                      "KinChannelName",
                                      "ClanChannelName",
                                      "NearbyChannelName",
                                      "ZoneChannelName",
                                      "CampChannelName",
                                   };
            for (EMChatChannelType type = EMChatChannelType.CHAT_CHANNEL_SYSTEM; type < EMChatChannelType.CHAT_CHANNEL_MAX; ++type)
            {
                m_dicChannnelName[type] = ULocalizationService.Instance.Get("UIView", "ChatBox", channelName[(int)type]);
            }



            canDrag = false;

            //如果游戏限制输入法，则初始化相关界面
            if (ImeManager.ForbidIme)
            {
                List<string> imeList = ImeManager.GetImeNameList();
                if (imeList != null)
                {
                    foreach (var temp in imeList)
                    {
                        imeDropdown.options.Add(new Dropdown.OptionData() { text = temp });
                    }
                }
                imeDropdown.value = 0;
                chineseIcon.gameObject.SetActive(false);
                enIcon.gameObject.SetActive(true);
            }
            else//否则隐藏
            {
                imeDropdown.interactable = false;
                chineseIcon.gameObject.SetActive(false);
                enIcon.gameObject.SetActive(false);
            }

            inputField.onEndEdit.AddListener(delegate { OnEndEdit(); });

            resizer.SetWndView(this);

            minimumIcon.SetChatWndView(this);
            minimunCanvasGroup = minimumIcon.GetComponent<CanvasGroup>();
            channelMaskItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/Chat/ChatChannelGroupItem");

            m_DiyChannelMask = ChatSettingConfig.Instance.GetChatSettingIntValue(ChatSettingIndex.DiyChannelMask);

            m_DiyChannelGroup.setWndView(this);
            m_DiyChannelGroup.Init();

            emoticonPanel.SetChatWndView(this);
            emoticonPanel.Init();
            emoticonBtn.onClick.AddListener(delegate { emoticonPanel.SwitchEmotionPanelActiveState(); });

            // 设置消息显示定时器
            TimerManager.SetTimer(this, (int)EChatTimer.EChatTimer_ShowChatMsg, 0.5f);
#if FUNCTION_AUTO_ADD_CHAT_OPEN
            TimerManager.SetTimer(this, (int)EChatTimer.EChatTimer_AddChatMsg, 1f);
#endif

            // 计算聊天框位置
            m_DownPos = GetTransform().anchoredPosition;
            m_UpPos = new Vector3(m_DownPos.x, m_DownPos.y + 300, m_DownPos.z);

            // 初始化大小
            m_InitSize = new Vector2(GetTransform().sizeDelta.x, GetTransform().sizeDelta.y);


            m_isMin = false;

            inputField.onFocusChange.AddListener(OnFocus);

            fpsHintInitPosition = fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition;


            m_PopupWndStatus = new Dictionary<int, bool>();

            m_fullWndStatus = new Dictionary<int, bool>();
            //m_ModalKey = 0;

            m_ChatBoxState = new ChatBoxBase(this);

            return true;
        }

        public override void Destroy()
        {
            AssetBundleManager.DeleteAssets(ref channelMaskItemPrefabRes, true);
            base.Destroy();
        }

        public void OnTimer(int nTimerID)
        {
            if (nTimerID == (int)EChatTimer.EChatTimer_ShowChatMsg)
            {
                LogicDataCenter.chatMessageManager.ShowOneChatMessage();
            }
#if FUNCTION_AUTO_ADD_CHAT_OPEN
            else if (nTimerID == (int)EChatTimer.EChatTimer_AddChatMsg)
            {
                //if (UnityEngine.Random.Range(0, 100) > 50)
                {
                    gamelogic_show_chat_message newMsg = new gamelogic_show_chat_message();
                    newMsg.channel = (int)EMChatChannelType.CHAT_CHANNEL_WARSCENE;
                    int nCount = UnityEngine.Random.Range(0, 4);
                    string str = Time.time.ToString();
                    for(int i = 0; i < nCount; ++i)
                    {
                        str += "一一一一一一一一一一一一一一一一一一一一";
                    }
                    newMsg.message = str;
                    newMsg.senderName = "test";
                    LogicDataCenter.chatMessageManager.AddChatMessage(ref newMsg);
                }
            }
#endif
        }

        public void OnFocus(bool focus)
        {
            // 聊天框激活且当前是私聊频道
            if (focus && m_dropDownIndexToChannel[channelDropdown.value] == EMChatChannelType.CHAT_CHANNEL_PRIVATE)
            {
                if (inputField.Empty)
                {
                    addPrivateChatToInputField(LogicDataCenter.chatMessageManager.LastPrivateChatName);
                }
            }
        }

        public void SetFpsSwitchHint(int fpsMode)
        {
            if (fpsMode == 1)
            {
                fpsSwitchHint.gameObject.SetActive(true);
            }
            else
            {
                fpsSwitchHint.gameObject.SetActive(false);
            }
        }

        public void CloseEmoticonPanel()
        {
            emoticonPanel.SwitchEmotionPanelActiveState();
        }

        public void UpdateEmoticonGroupLockState(UChatEmoticonGroupState groupState)
        {
            if (groupState == null)
                return;

            emoticonPanel.UpdateEmoticonGroupLockState(groupState.unlockEmotionGroup);
        }

        // 设置聊天框为激活状态
        private void setActiveChatFrame()
        {
            channelCanvasGroup.alpha = 1;
            minimunCanvasGroup.alpha = 1;
            borderCanvasGroup.alpha = 1;
            SetContentScrollViewBarAlpha(1);
            setResizerImageAlpha(1f);
            GetTransform().sizeDelta = m_InitSize;
        }

        // 需要等段时间再设置verticalScrollbar的位置,不然不生效
        private IEnumerator showLastChatContent()
        {
            yield return new WaitForSeconds(0.1f);

            if (contentView != null)
            {
                contentView.GetScrollRect().verticalScrollbar.value = 0;
            }
        }

        public override void Update()
        {
            base.Update();

            m_ChatBoxState.OnUpdate();
        }

        private void SetFrameHide(bool bHide)
        {
            m_hideFrame = bHide;
            if (bHide == false)
            {
                borderCanvasGroup.alpha = 1f;
                minimunCanvasGroup.alpha = 1f;
                channelCanvasGroup.alpha = 1f;

                setResizerImageAlpha(1f);

                SetContentScrollViewBarAlpha(1f);

                SetContentHide(false);
            }
        }

        // 隐藏外框
        private void SetFrameHideEx(bool bHide)
        {
            if (bHide)
            {
                borderCanvasGroup.alpha = 0f;
                minimunCanvasGroup.alpha = 0f;
                channelCanvasGroup.alpha = 0f;
                setResizerImageAlpha(0f);
                SetContentScrollViewBarAlpha(0f);
            }
            else
            {
                borderCanvasGroup.alpha = 1f;
                minimunCanvasGroup.alpha = 1f;
                channelCanvasGroup.alpha = 1f;
                setResizerImageAlpha(1f);
                SetContentScrollViewBarAlpha(1f);
            }
        }

        private void SetHide(bool bHide)
        {
            if (bHide)
            {
                borderCanvasGroup.alpha = 0f;
                minimunCanvasGroup.alpha = 0f;
                channelCanvasGroup.alpha = 0f;
                contentCanvasGroup.alpha = 0f;
                setResizerImageAlpha(0f);
                SetContentScrollViewBarAlpha(0f);
            }
            else
            {
                borderCanvasGroup.alpha = 1f;
                minimunCanvasGroup.alpha = 1f;
                channelCanvasGroup.alpha = 1f;
                contentCanvasGroup.alpha = 1f;
                setResizerImageAlpha(1f);
                SetContentScrollViewBarAlpha(1f);
            }
        }

        private void SetActive(bool bActive)
        {
            borderCanvasGroup.gameObject.SetActive(bActive);
            minimunCanvasGroup.gameObject.SetActive(bActive);
            channelCanvasGroup.gameObject.SetActive(bActive);
            contentCanvasGroup.gameObject.SetActive(bActive);
            resizerImage.gameObject.SetActive(bActive);
            SetContentScrollViewBar(bActive);
        }

        public void ChangeIme(ChatBoxChangeImeData imeData)
        {
            if (imeData.bIsInImeList && !imeData.name.Equals(ULocalizationService.Instance.Get("UIView", "ChatBox", "EN")))
            {
                chineseIcon.gameObject.SetActive(true);
                enIcon.gameObject.SetActive(false);
            }
            else
            {
                chineseIcon.gameObject.SetActive(false);
                enIcon.gameObject.SetActive(true);
            }
        }

        private void SetContentHide(bool bHide)
        {
            m_hideContent = bHide;
            if (bHide == false)
            {
                contentCanvasGroup.alpha = 1f;
                m_showContentTime = Time.time;
            }
        }

        private bool detectMouseInWnd()
        {
            if (!this)
            {
                return false;
            }

            //检测鼠标是否在鼠标内
            if (Time.time > m_nextCheckMouseInWndTime)
            {
                m_nextCheckMouseInWndTime = Time.time + 0.3f;

                RectTransform rectTrans = transform as RectTransform;
                if (RectTransformUtility.RectangleContainsScreenPoint(rectTrans, InputManager.MousePosition, UISystem.Instance.GetCamera()))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            return false;
        }

        // 检测主城是否有弹窗
        private bool isHavePopupWnd()
        {
            foreach (var item in m_PopupWndStatus)
            {
                if (item.Value)
                    return true;
            }

            return false;
        }

        private bool isHaveFullWnd()
        {
            foreach (var item in m_fullWndStatus)
            {
                if (item.Value)
                    return true;
            }

            return false;
        }
        public void ChangeChannel(int channel)
        {
            EMChatChannelType target = (EMChatChannelType)channel;
            for (int i = 0; i < m_dropDownIndexToChannel.Length; ++i)
            {
                if (m_dropDownIndexToChannel[i] == target)
                {
                    channelDropdown.value = i;
                    break;
                }
            }
        }

 

        // 设置聊天框大小
        private void SetSize(Vector2 size)
        {
            GetTransform().sizeDelta = size;
        }

        private void ClearChannelMaskItem()
        {
            // 清除频道
            int childCount = channelMaskButtonFrame.childCount;
            GameObject[] childArray = new GameObject[childCount];
            for (int i = 0; i < childCount; ++i)
            {
                Transform child = channelMaskButtonFrame.GetChild(i);
                childArray[i] = child.gameObject;
            }
            foreach (var obj in childArray)
            {
                ChatChannelGroupItem item = obj.GetComponent<ChatChannelGroupItem>();
                if (item != null)
                    item.Clear();
                var temp = obj;
                channelMaskItemPrefabRes.DestroyInstanceRes(ref temp);
            }
        }

        private ChatChannelGroupItem AddChannelMaskItem(ChatChannelGroupType type)
        {
            GameObject itemObj = channelMaskItemPrefabRes.InstanceMainRes();
            if (itemObj == null)
                return null;

            ChatChannelGroupItem item = itemObj.GetComponent<ChatChannelGroupItem>();
            if (item == null)
                return null;

            item.Init(this, type);

            // 增加Tooltips
            addTooltipsForChannelGroup(ref itemObj, type);

            return item;
        }

        public void SetChatWndViewPositionChange(ChatPositionChangeType chatPositionChangeType)
        {
            if (chatPositionChangeType == ChatPositionChangeType.CPCY_MOVEUP)
            {
                GetTransform().anchoredPosition = m_UpPos;
            }

            if (chatPositionChangeType == ChatPositionChangeType.CPCY_MOVEDOWN)
            {
                GetTransform().anchoredPosition = m_DownPos;
            }
        }

        public bool OnPressReturn()
        {
            if (m_isInputFieldFocus == false)
            {
                // 隐藏频道选择
                channelDropdown.Hide();

                inputField.Select();
            }

            return true;
        }

        public void StepChangeChannel(bool bMinus)
        {
            int nCurChannel = channelDropdown.value;
            if (bMinus && nCurChannel > 0)
            {
                channelDropdown.value = nCurChannel - 1;
            }
            if (!bMinus && nCurChannel < channelDropdown.options.Count - 1)
            {
                channelDropdown.value = nCurChannel + 1;
            }
        }

        private void OnEndEdit()
        {
            if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.KeypadEnter))
            {
                OnSendBtnClick();
            }
        }

        // 切换频道响应
        public void OnChannelChanged()
        {
            EMChatChannelType newChannel = m_dropDownIndexToChannel[channelDropdown.value];
            bool bBan = false;
            switch (newChannel)
            {
                case EMChatChannelType.CHAT_CHANNEL_WORLD:
                    break;
                case EMChatChannelType.CHAT_CHANNEL_MATCHROOM:
                    {
                        if (StageManager.Instance.getCurrentState() != EStage_State.ESS_MATCHROOM)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_MATCHROOM_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_TEAMMATE:
                    {
                        //if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID) <= 0)
                        if (GameLogicAPI.getTeamID() <= 0)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_TEAMMATE_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_CAMP:
                    {
                        if (StageManager.Instance.getCurrentState() != EStage_State.ESS_BATTLE &&
                            StageManager.Instance.getCurrentState() != EStage_State.ESS_MATCHROOM)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_CAMP_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_WARSCENE:
                    {
                        if (StageManager.Instance.getCurrentState() != EStage_State.ESS_BATTLE)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_WARSCENE_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_KIN:
                    {
                        if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN) <= 0)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_KIN_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_CLAN:
                    {
                        if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) <= 0)
                        {
                            bBan = true;
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_BAN_CLAN_CHANNEL);
                        }
                    }
                    break;
                case EMChatChannelType.CHAT_CHANNEL_PRIVATE:
                    {
                        addPrivateChatToInputField(LogicDataCenter.chatMessageManager.LastPrivateChatName);
                    }
                    break;
                default:
                    break;
            }

            if (bBan)
                StartCoroutine(DelayRevertChannel());
            else
                m_curSelChannel = channelDropdown.value;
        }

        IEnumerator DelayRevertChannel()
        {
            yield return new WaitForEndOfFrame();
            channelDropdown.value = m_curSelChannel;
        }

        public void OnImeChanged()
        {
            int index = imeDropdown.value;
            ImeManager.SwitchIme(imeDropdown.options[index].text);
            if (imeDropdown.value != 0)
            {
                chineseIcon.gameObject.SetActive(true);
                enIcon.gameObject.SetActive(false);
            }
            else
            {
                chineseIcon.gameObject.SetActive(false);
                enIcon.gameObject.SetActive(true);
            }
            inputField.Select();
            ImeManager.EnableIme = true;
        }

        public void OnSendBtnClick()
        {
            string strInput = inputField.GetCharString(0, -1);

			//替换表情字符
			ChatEmoticonConfig.Instance.ReplaceChatEmoticonChar(ref strInput);

            if (strInput.StartsWith("/"))  // 私聊
            {
                string targetName;
                string msg;
                parsePrivateChat(strInput, out targetName, out msg);
                if (targetName.Length > 0)
                {
                    if (msg.Length > 0)
                        UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_PRIVATE, msg, targetName, 0);

                    LogicDataCenter.chatMessageManager.LastPrivateChatName = targetName;
                }
            }
            else
            {
                EMChatChannelType channelType = m_dropDownIndexToChannel[channelDropdown.value];
                if (channelType != EMChatChannelType.CHAT_CHANNEL_PRIVATE)
                {
                    UIUtil.SendChatMessage(channelType, strInput);
                }
            }

            inputField.ClearContent();
        }

        public void OnChatMessageUpdate()
        {
            if (!contentView)
                return;

            List<SChatMessageInfo> msgList = LogicDataCenter.chatMessageManager.MessageList;
            if (msgList == null)
                return;

            //if (LogicDataCenter.chatMessageManager.MsgIsOverLoad)
            //{
                //contentView.removeFirstComponentsItem();
            //}

            contentView.DataSource.BeginUpdate();

            contentView.DataSource.Clear();
            for (int i = 0; i < msgList.Count; ++i)
            {
                ChatBoxContentItemData data = new ChatBoxContentItemData();
                data.info = msgList[i];
                contentView.DataSource.Add(data);
            }

            contentView.DataSource.EndUpdate();

            contentView.GetScrollRect().normalizedPosition = new Vector2(0f, 0f);

            m_ChatBoxState.OnNewChatMsg();
        }

        public void ToggleHideWindow()
        {
            if (contentCanvasGroup.gameObject.activeSelf)
            {
                contentCanvasGroup.gameObject.SetActive(false);
                borderCanvasGroup.gameObject.SetActive(false);
                resizerImage.gameObject.SetActive(false);
                channelCanvasGroup.gameObject.SetActive(false);
                minimunCanvasGroup.alpha = 1f;

                m_isMin = true;
            }
            else
            {
                contentCanvasGroup.gameObject.SetActive(true);
                borderCanvasGroup.gameObject.SetActive(true);
                resizerImage.gameObject.SetActive(true);

                // 战场内不显示频道选择按钮组
                if (GameLogicAPI.isInWarScene() > 0)
                    channelCanvasGroup.gameObject.SetActive(false);
                else
                    channelCanvasGroup.gameObject.SetActive(true);

                m_isMin = false;
            }
        }

        public void SetMinState(bool isMin)
        {
            if(isMin)
            {
                contentCanvasGroup.gameObject.SetActive(false);
                borderCanvasGroup.gameObject.SetActive(false);
                resizerImage.gameObject.SetActive(false);
                channelCanvasGroup.gameObject.SetActive(false);
                minimunCanvasGroup.gameObject.SetActive(true);
                minimunCanvasGroup.alpha = 1f;

                m_isMin = true;
            }
            else
            {
                contentCanvasGroup.gameObject.SetActive(true);
                borderCanvasGroup.gameObject.SetActive(true);
                resizerImage.gameObject.SetActive(true);
                channelCanvasGroup.gameObject.SetActive(true);
                minimunCanvasGroup.gameObject.SetActive(true);
                minimunCanvasGroup.alpha = 1f;

                m_isMin = false;

            }
        }

        public void OnInputfieldAddItem(ChatBoxAddInputFieldItemData msgData)
        {
            ResNode itemPrefab = LogicDataCenter.chatMessageManager.GetChatItemPrefabByObjType(msgData.objInfo.type);
            if (itemPrefab == null)
                return;

            GameObject itemObj = itemPrefab.InstanceMainRes();
            if (itemObj == null)
                return;

            IChatItem subItem = itemObj.GetComponent<IChatItem>();
            subItem.SetResNode(itemPrefab);

            if (subItem == null)
            {
                itemPrefab.DestroyInstanceRes(ref itemObj);
                return;
            }

            if (subItem.SetItemData(msgData.objInfo) == false)
            {
                itemPrefab.DestroyInstanceRes(ref itemObj);
                return;
            }

            if (msgData.bClearInputField)
            {
                inputField.ClearContent();
            }

            if (msgData.bActiveInputField)
            {
                inputField.ActivateInputField();
            }

            inputField.selectionFocusPosition = inputField.caretPosition;
            inputField.InsertItem(itemObj, subItem, -1);

            // 自动发送
            if (msgData.bAutoSend)
            {
                OnSendBtnClick();
            }
        }

        public void SetFrameFade(bool bFade)
        {
            m_disableFade = !bFade;
        }

        public void SetForceConditionHide(ChatForceConditionHide data)
        {
            this.SetVisible(!data.bForceHide);
        }

        /// <summary>
        /// 设置聊听内容的滚动条的透明度
        /// </summary>
        /// <param name="a">透明度值</param>
        private void SetContentScrollViewBarAlpha(float a)
        {
            Transform scrollBarVertical = contentCanvasGroup.transform.FindChild("Scroll View").FindChild("Scrollbar Vertical");

            // 滚动条
            Scrollbar scrollBar = scrollBarVertical.GetComponent<Scrollbar>();
            ColorBlock oldColorBlock = scrollBar.colors;
            Color tmpColor1 = scrollBar.colors.normalColor;
            tmpColor1.a = a;
            oldColorBlock.normalColor = tmpColor1;
            scrollBar.colors = oldColorBlock;

            // 滚动条背景
            Image scrollBg = scrollBarVertical.GetComponent<Image>();
            Color tmpColor2 = scrollBg.color;
            tmpColor2.a = a;
            scrollBg.color = tmpColor2;
        }

        private void SetContentScrollViewBar(bool bActive)
        {
            Transform scrollBarVertical = contentCanvasGroup.transform.FindChild("Scroll View").FindChild("Scrollbar Vertical");
            scrollBarVertical.gameObject.SetActive(bActive);
        }

        /// <summary>
        /// 更新聊听内容的滚动条的透明度
        /// </summary>
        /// <param name="delta">时间间隔</param>
        private void UpdateContentScrollViewBarAlpha(float delta)
        {
            Transform scrollBarVertical = contentCanvasGroup.transform.FindChild("Scroll View").FindChild("Scrollbar Vertical");

            // 滚动条
            Scrollbar scrollBar = scrollBarVertical.GetComponent<Scrollbar>();
            ColorBlock oldColorBlock = scrollBar.colors;
            Color tmpColor1 = scrollBar.colors.normalColor;
            tmpColor1.a -= delta * 0.5f;
            if (tmpColor1.a < m_hideFrameAlpha)
                tmpColor1.a = m_hideFrameAlpha;
            oldColorBlock.normalColor = tmpColor1;
            scrollBar.colors = oldColorBlock;

            // 滚动条背景
            Image scrollBg = scrollBarVertical.GetComponent<Image>();
            Color tmpColor2 = scrollBg.color;
            tmpColor2.a -= delta * 0.5f;
            if (tmpColor2.a < m_hideFrameAlpha)
                tmpColor2.a = m_hideFrameAlpha;
            scrollBg.color = tmpColor2;
        }

        private void setResizerImageAlpha(float alpha)
        {
            Color curColor = resizerImage.color;
            curColor.a = alpha;
            resizerImage.color = curColor;
        }

        /// <summary>
        /// 设置自定义频道中接收哪些消息
        /// </summary>
        /// <param name="channelMask"></param>
        public void updateDiyChannelMask(int channelMask)
        {
            m_DiyChannelMask = channelMask;
            // 更新到本地脚本
            ChatSettingConfig.Instance.ModifyChatSetting(ChatSettingIndex.DiyChannelMask, channelMask.ToString());

            m_DiyChannelGroupItem.setChannelMask(m_DiyChannelMask);
        }

        /// <summary>
        /// 更新自定义频道窗口
        /// </summary>
        public void updateDiyChannelWnd()
        {
            if (!m_DiyChannelGroup.gameObject.activeSelf)
            {
                openDiyChannelWnd();
            }
            else
            {
                closeDiyChannelWnd();
            }
        }

        /// <summary>
        /// 关闭自定义频道窗口
        /// </summary>
        public void closeDiyChannelWnd()
        {
            m_DiyChannelGroup.gameObject.SetActive(false);

            LogicDataCenter.chatMessageManager.SetChannelMask(m_DiyChannelMask);
        }

        /// <summary>
        /// 隐藏自定义频道窗口
        /// </summary>
		public void hideDiyChannelWnd()
        {
            m_DiyChannelGroup.gameObject.SetActive(false);
        }

        /// <summary>
        /// 打开自定义频道窗口
        /// </summary>
        public void openDiyChannelWnd()
        {
            // 计算位置
            Vector3 diyWndNewPos = Vector3.zero;
            diyWndNewPos.x = gameObject.GetComponent<RectTransform>().sizeDelta.x;
            diyWndNewPos.y = gameObject.GetComponent<RectTransform>().sizeDelta.y;
            m_DiyChannelGroup.transform.SetLocalPosition(diyWndNewPos);

            m_DiyChannelGroup.gameObject.SetActive(true);
        }

        /// <summary>
        /// 处理私聊消息，私聊格式：斜杠+名字+空格+聊听内容
        /// </summary>
        /// <param name="strInput">聊听框的内容</param>
        /// <param name="targetName">私聊目标</param>
        /// <param name="msg">私聊消息</param>
        private void parsePrivateChat(string strInput, out string targetName, out string msg)
        {
            targetName = "";
            msg = "";

            // 获取第一个空格
            int firstBlank = strInput.IndexOf(' ');
            if (firstBlank < 0)
            {
                return;
            }

            //截取[名字]
            targetName = strInput.Substring(1, firstBlank - 1);

            // 截取[空格+聊听内容]
            msg = strInput.Substring(firstBlank, strInput.Length - firstBlank);

            // 去掉[空格]，留下[聊听内容]
            msg = msg.Substring(1);
        }

        /// <summary>
        /// 选择私聊
        /// </summary>
        public void selectPrivateChat(ChatBoxPrivateChat msgData)
        {
            // 输入框设置私聊格式
            addPrivateChatToInputField(msgData.name);

            // 查找私聊频道的下标
            int index = 0;
            foreach (EMChatChannelType type in m_dropDownIndexToChannel)
            {
                if (type == EMChatChannelType.CHAT_CHANNEL_PRIVATE)
                {
                    break;
                }
                index++;
            }
            channelDropdown.value = index;
        }
        /// <summary>
        /// 输入框设置私聊格式
        /// </summary>
        /// <param name="name"></param>
        public void addPrivateChatToInputField(string name)
        {
            if (string.IsNullOrEmpty(name))
                return;

            SChatMessageObjectInfo channelObj;
            channelObj.type = ChatItemType.ObjType_Text;
            channelObj.subType = ChatItemSubType.ObjSubType_Text_Text;
            channelObj.text = "/" + name + " ";
            channelObj.param = new Dictionary<String, String>();
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = channelObj;
            uiData.bClearInputField = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
        }

        /// <summary>
        /// 给频道增加tooltips
        /// </summary>
        /// <param name="ob"></param>
        private void addTooltipsForChannelGroup(ref GameObject ob, ChatChannelGroupType groupType)
        {
            if (groupType < ChatChannelGroupType.ChatChannel_All || groupType > ChatChannelGroupType.ChatChannel_WarScene)
            {
                return;
            }
            string[] channelTipsName = { "AllTips", "ClanTips", "KinTips", "TeamTips", "AreaTips", "RoomTips", "PersonalTips", "PublicTips", "DiyTips", "WarSceneTips" };

            string tooltipsText = ULocalizationService.Instance.Get("UIView", "ChatBox", channelTipsName[(int)groupType]);


            UTooltipTrigger channelTooltipTrigger = ob.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            channelTooltipTrigger.SetText(UTooltipParamName.BodyText, tooltipsText);
            channelTooltipTrigger.backgroundTint = Color.cyan;
        }

        /// <summary>
        /// 获取频道的名称
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        static public string getChannelName(EMChatChannelType type)
        {
            string name = "";
            if (!m_dicChannnelName.TryGetValue(type, out name))
            {
                return "";
            }

            return name;
        }

        private void refreshChannelDropDown()
        {
            channelDropdown.options.Clear();
            foreach (EMChatChannelType type in m_dropDownIndexToChannel)
            {
                channelDropdown.options.Add(new Dropdown.OptionData() { text = getChannelName(type) });
            }
            // 刷新Dropdown
            channelDropdown.RefreshShownValue();
        }

        public void clearAllMsgData()
        {
            LogicDataCenter.chatMessageManager.clearAllChatMsgData();
        }

        public void DelayChatMessageUpdate()
        {
            StartCoroutine(_DelayChatMessageUpdate());
        }

        private IEnumerator _DelayChatMessageUpdate()
        {
            yield return new WaitForEndOfFrame();

            OnChatMessageUpdate();
        }
        public void onPopWndVisible(UCommonPopupWndVisible msgData)
        {
            if (m_PopupWndStatus.ContainsKey(msgData.nWndID))
            {
                m_PopupWndStatus[msgData.nWndID] = msgData.isVisible;
            }
            else
            {
                m_PopupWndStatus.Add(msgData.nWndID, msgData.isVisible);
            }

            if (isHavePopupWnd())
            {
                if(m_ChatBoxState.GetChatBoxState() == ChatBoxState.StaticState)
                {
                    OnEnter(ChatBoxState.StaticStatePopup);
                }
            }
            else
            {
                if(m_ChatBoxState.GetChatBoxState() == ChatBoxState.StaticStatePopup)
                {
                    OnLeave(ChatBoxState.StaticStatePopup);
                    OnEnter(ChatBoxState.StaticState);
                }
            }
        }

        public void onFullWndVisible(UCommonFullWndVisible msgData)
        {

            if (m_fullWndStatus.ContainsKey(msgData.nWndID))
            {
                m_fullWndStatus[msgData.nWndID] = msgData.isVisible;
            }
            else
            {
                m_fullWndStatus.Add(msgData.nWndID, msgData.isVisible);
            }

            if (isHaveFullWnd())
            {
                if(m_ChatBoxState.GetChatBoxState() == ChatBoxState.StaticState || 
                    m_ChatBoxState.GetChatBoxState() == ChatBoxState.StaticStatePopup)
                {
                    OnEnter(ChatBoxState.StaticStateFullScreen);
                }
            }
            else
            {
                if(m_ChatBoxState.GetChatBoxState() == ChatBoxState.StaticStateFullScreen)
                {
                    OnLeave(ChatBoxState.StaticStateFullScreen);
                    OnEnter(ChatBoxState.StaticState);
                }
            }
        }

        public void OnLeave(ChatBoxState state)
        {
            if(m_ChatBoxState.GetChatBoxState() == state)
            {
                m_ChatBoxState.OnLeave();
            }
        }
        public void OnEnter(ChatBoxState newState)
        {
            ChatBoxState oldState = m_ChatBoxState.GetChatBoxState();

            switch (newState)
            {
                case ChatBoxState.StaticState:
                    {
                        m_ChatBoxState = new ChatBoxStaticScene(this);
                    }
                    break;
                case ChatBoxState.StaticStateFullScreen:
                    {
                        m_ChatBoxState = new ChatBoxStaticSceneFullScreen(this);
                    }
                    break;
                case ChatBoxState.WarLoadingState:
                    {
                        m_ChatBoxState = new ChatBoxWarLoading(this);
                    }
                    break;
                case ChatBoxState.WarState:
                    {
                        m_ChatBoxState = new ChatBoxWarScene(this);
                    }
                    break;
                case ChatBoxState.RoomState:
                    {
                        m_ChatBoxState = new ChatBoxMatchRoom(this);
                    }
                    break;
                case ChatBoxState.StaticStatePopup:
                    {
                        m_ChatBoxState = new ChatBoxStaticScenePopup(this);
                    }
                    break;
                default:
                    {
                        Debug.LogWarning(String.Format("Error ChatBoxState,{0}", newState));
                    }
                    break;
            }

            m_ChatBoxState.OnEnter(oldState);
        }
    }
}