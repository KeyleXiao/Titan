/*******************************************************************
** 文件名:	ChatItem.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	李有红
** 日  期:	2017/8/28
** 版  本:	1.0
** 描  述:	聊天框实现-各种状态下的聊天框逻辑（主城、战场、房间、战场Loading界面等等）
** 应  用:  
*****************************************************************/

using UnityEngine;
using DataCenter;
using ASpeedGame.Data.ChatDefaultChannel;

namespace USpeedUI.ChatBox
{
    // 聊天框状态
    public enum ChatBoxState
    {
        NoneState,
        StaticState,                // 主城状态
        WarState,                   // 战场状态
        RoomState,                  // 房间状态
        WarLoadingState,            // 战场Loading状态
        StaticStateFullScreen,      // 主城全屏状态
        StaticStatePopup,           // 主城弹窗状态
    }

    public partial class ChatBoxWndView : UIBaseWndView, ITimerHanlder
    {
        // 聊天框逻辑基类
        public class ChatBoxBase
        {
            public ChatBoxWndView View = null;

            public ChatBoxBase(ChatBoxWndView view)
            {
                View = view;
            }

            virtual public void OnEnter(ChatBoxState oldState) { }

            virtual public void OnLeave() { }

            virtual public void OnUpdate() { }

            virtual public void OnNewChatMsg(){ }

            virtual public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.NoneState;
            }
        }

        // 主城默认
        public class ChatBoxStaticScene : ChatBoxBase
        {
            // 是否激活状态
            private bool m_isActive = false;

            // 激活状态大小
            private Vector2 m_ActiveSize = Vector2.zero;

            // 变为非激活状态起始时间
            private float m_PlayUnActiveTime = 0f;

            public ChatBoxStaticScene(ChatBoxWndView view) : base(view)
            {
                m_ActiveSize = View.m_InitSize;
            }

            override public void OnEnter(ChatBoxState oldState)
            {
                // 清空消息
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_MATCHROOM);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_CAMP);

                View.SetActive(true);
                View.SetHide(false);
                SetActiveState();

                if (oldState == ChatBoxState.StaticStateFullScreen)
                    return;

                // 设置需要显示的频道选项对应的频道
                View.m_dropDownIndexToChannel = new EMChatChannelType[]{
                EMChatChannelType.CHAT_CHANNEL_WORLD,
                EMChatChannelType.CHAT_CHANNEL_CLAN,
                EMChatChannelType.CHAT_CHANNEL_KIN,
                EMChatChannelType.CHAT_CHANNEL_TEAMMATE,
                EMChatChannelType.CHAT_CHANNEL_PRIVATE,
                EMChatChannelType.CHAT_CHANNEL_NEARBY,
                EMChatChannelType.CHAT_CHANNEL_ZONE,
                };
                View.refreshChannelDropDown();
                View.ChangeChannel((int)EMChatChannelType.CHAT_CHANNEL_NEARBY);

                View.ClearChannelMaskItem();
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_All);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Personal);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Public);
                View.m_DiyChannelGroupItem = View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Diy);
                View.m_DiyChannelGroupItem.setChannelMask(View.m_DiyChannelMask);

                // 默认选择全部
                View.channelMaskButtonGroup.SetItemSelectedItem(0, true);

                View.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEDOWN);

                //// 设置初始大小,TODO-lyh
                //View.SetSize(new Vector2(420f, 235f));
                //View.resizer.resizeMinWidth = 420f;
                //View.resizer.resizeMinHeight = 235f;
            }

            public override void OnUpdate()
            {
                // 拖拽状态,不处理
                if (View.resizer.IsDraging)
                    return;

                // 最小化状态,不处理
                if (View.m_isMin)
                    return;

                // 输入框激活状态是否变化
                if (View.m_isInputFieldFocus != View.inputField.isFocused)
                {
                    View.m_isInputFieldFocus = View.inputField.isFocused;
                    InputManager.Available = !View.m_isInputFieldFocus;
                    ImeManager.ActiveIme(View.m_isInputFieldFocus);
                }

                if (View.isHavePopupWnd())
                    return;

                //检测鼠标是否在聊天窗内
                bool isMouseInWnd = View.detectMouseInWnd();

                if (m_isActive) // 激活状态
                {
                    if (isMouseInWnd || View.inputField.isFocused)
                    {
                        // 取消3s后切换为非激活状态
                        m_PlayUnActiveTime = 0f;
                    }
                    else
                    {
                        // 开始3s后切换为非激活状态
                        if (m_PlayUnActiveTime < float.Epsilon)
                            m_PlayUnActiveTime = Time.time;
                    }

                    // 3s后切换为非激活状态
                    if (m_PlayUnActiveTime > 0f && Time.time - m_PlayUnActiveTime > 3f)
                    {
                        m_PlayUnActiveTime = 0;

                        // 切换聊天框为非激活状态
                        setUnActiveState();
                    }
                }
                else    // 非激活状态
                {
                    if (isMouseInWnd || View.inputField.isFocused)
                    {
                        // 切换聊天框为激活状态
                        SetActiveState();
                    }
                }
            }

            override public void OnLeave()
            {

            }

            override public void OnNewChatMsg()
            {
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.StaticState;
            }

            private void SetActiveState()
            {
                // 显示频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(true);
                View.channelCanvasGroup.alpha = 1;

                // 显示最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(true);
                View.minimunCanvasGroup.alpha = 1;

                // 放大聊听框
                View.SetSize(m_ActiveSize);

                m_isActive = true;

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = View.fpsHintInitPosition;
            }

            // 变为非激活状态
            private void setUnActiveState()
            {
                // 保存激活时候的大小
                m_ActiveSize = new Vector2(View.GetTransform().sizeDelta.x, View.GetTransform().sizeDelta.y);

                // 隐藏频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(false);
                // 隐藏最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(false);
                // 缩小聊天框
                View.SetSize(new Vector2(View.m_UnActiveWidth, View.m_UnActiveHeight));

                m_isActive = false;

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = new Vector2(View.fpsHintInitPosition.x, 
                    View.fpsHintInitPosition.y - View.minimumIcon.GetComponent<RectTransform>().sizeDelta.y);
            }
        }

        // 主城全屏界面显示
        public class ChatBoxStaticSceneFullScreen : ChatBoxBase
        {
            public ChatBoxStaticSceneFullScreen(ChatBoxWndView view) : base(view)
            {

            }

            override public void OnEnter(ChatBoxState oldState)
            {
                // 清空消息
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_MATCHROOM);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_CAMP);

                View.SetMinState(true);
            }

            override public void OnLeave()
            {
                View.SetMinState(false);
            }

            override public void OnUpdate()
            {
            }

            override public void OnNewChatMsg()
            {
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.StaticStateFullScreen;
            }
        }

        // 主城有弹窗显示
        public class ChatBoxStaticScenePopup : ChatBoxBase
        {
            public ChatBoxStaticScenePopup(ChatBoxWndView view) : base(view)
            {

            }

            override public void OnEnter(ChatBoxState oldState)
            {
                // 清空消息
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_MATCHROOM);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_CAMP);

                // 缩小聊天框
                View.SetSize(new Vector2(View.m_UnActiveWidth, View.m_UnActiveHeight));

                // 隐藏频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(false);
                // 隐藏最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(false);

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = new Vector2(View.fpsHintInitPosition.x,
                    View.fpsHintInitPosition.y - View.minimumIcon.GetComponent<RectTransform>().sizeDelta.y);
            }

            public override void OnUpdate()
            {
                // 输入框激活状态是否变化
                if (View.m_isInputFieldFocus != View.inputField.isFocused)
                {
                    View.m_isInputFieldFocus = View.inputField.isFocused;
                    InputManager.Available = !View.m_isInputFieldFocus;
                    ImeManager.ActiveIme(View.m_isInputFieldFocus);
                }
            }

            override public void OnLeave()
            {
            }

            override public void OnNewChatMsg()
            {
                View.SetContentHide(false);
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.StaticStatePopup;
            }
        }

        // 房间
        public class ChatBoxMatchRoom : ChatBoxBase
        {
            private bool m_isActive = false;

            private Vector2 m_ActiveSize = Vector2.zero;

            private float m_PlayUnActiveTime = 0f;

            public ChatBoxMatchRoom(ChatBoxWndView view) : base(view)
            {
                m_ActiveSize = View.m_InitSize;
            }

            override public void OnEnter(ChatBoxState oldState)
            {
                // 清空消息
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_ZONE);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_NEARBY);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_TEAMMATE);

                View.contentView.Clear();

                View.SetActive(true);
                View.SetHide(false);
                SetActiveState();

                // 设置需要显示的频道选项对应的频道
                View.m_dropDownIndexToChannel = new EMChatChannelType[]{
                EMChatChannelType.CHAT_CHANNEL_WORLD,
                EMChatChannelType.CHAT_CHANNEL_CLAN,
                EMChatChannelType.CHAT_CHANNEL_KIN,
                EMChatChannelType.CHAT_CHANNEL_MATCHROOM,
                EMChatChannelType.CHAT_CHANNEL_CAMP,
                EMChatChannelType.CHAT_CHANNEL_PRIVATE,
                };
                View.refreshChannelDropDown();

                View.ChangeChannel(ChatDefaultChannel.Instance.getDefaultChannelInRoom(View.m_nMatchType));


                View.ClearChannelMaskItem();
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_All);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Personal);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Public);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Room);
                View.m_DiyChannelGroupItem = View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Diy);
                View.m_DiyChannelGroupItem.setChannelMask(View.m_DiyChannelMask);

                // 默认选择房间
                View.channelMaskButtonGroup.SetItemSelectedItem(3, true);

                View.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEDOWN);

                // 延迟刷新聊天内容
                View.DelayChatMessageUpdate();
            }

            public override void OnUpdate()
            {
                // 拖拽状态,不处理
                if (View.resizer.IsDraging)
                    return;

                // 最小化状态,不处理
                if (View.m_isMin)
                    return;

                // 输入框激活状态是否变化
                if (View.m_isInputFieldFocus != View.inputField.isFocused)
                {
                    View.m_isInputFieldFocus = View.inputField.isFocused;
                    InputManager.Available = !View.m_isInputFieldFocus;
                    ImeManager.ActiveIme(View.m_isInputFieldFocus);
                }

                //检测鼠标是否在聊天窗内
                bool isMouseInWnd = View.detectMouseInWnd();

                if (m_isActive) // 激活状态
                {
                    if (isMouseInWnd || View.inputField.isFocused)
                    {
                        // 取消3s后切换为非激活状态
                        m_PlayUnActiveTime = 0f;
                    }
                    else
                    {
                        // 开始3s后切换为非激活状态
                        if (m_PlayUnActiveTime < float.Epsilon)
                            m_PlayUnActiveTime = Time.time;
                    }

                    // 3s后切换为非激活状态
                    if (m_PlayUnActiveTime > 0f && Time.time - m_PlayUnActiveTime > 3f)
                    {
                        m_PlayUnActiveTime = 0;

                        // 切换聊天框为非激活状态
                        setUnActiveState();
                    }
                }
                else    // 非激活状态
                {
                    if (isMouseInWnd || View.inputField.isFocused)
                    {
                        // 切换聊天框为激活状态
                        SetActiveState();
                    }
                }
            }

            override public void OnLeave()
            {
                View.contentView.Clear();
            }

            override public void OnNewChatMsg()
            {
                View.SetContentHide(false);
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.RoomState;
            }

            private void SetActiveState()
            {
                // 显示频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(true);
                View.channelCanvasGroup.alpha = 1;

                // 显示最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(true);
                View.minimunCanvasGroup.alpha = 1;

                // 放大聊听框
                View.SetSize(m_ActiveSize);

                m_isActive = true;

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = View.fpsHintInitPosition;
            }

            // 变为非激活状态
            private void setUnActiveState()
            {
                // 保存激活时候的大小
                m_ActiveSize = new Vector2(View.GetTransform().sizeDelta.x, View.GetTransform().sizeDelta.y);

                // 隐藏频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(false);
                // 隐藏最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(false);
                // 缩小聊天框
                View.SetSize(new Vector2(View.m_UnActiveWidth, View.m_UnActiveHeight));

                m_isActive = false;

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = new Vector2(View.fpsHintInitPosition.x,
                    View.fpsHintInitPosition.y - View.minimumIcon.GetComponent<RectTransform>().sizeDelta.y);
            }
        }

        // 战场Loading界面
        public class ChatBoxWarLoading : ChatBoxBase
        {
            private float m_HideTime = 0;

            public ChatBoxWarLoading(ChatBoxWndView view) : base(view)
            {
            }

            override public void OnEnter(ChatBoxState oldState)
            {
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_ZONE);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_NEARBY);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_TEAMMATE);

                View.SetActive(true);
                View.SetHide(false);

                // 设置需要显示的频道选项对应的频道
                View.m_dropDownIndexToChannel = new EMChatChannelType[]{
                EMChatChannelType.CHAT_CHANNEL_WORLD,
                EMChatChannelType.CHAT_CHANNEL_CLAN,
                EMChatChannelType.CHAT_CHANNEL_KIN,
                EMChatChannelType.CHAT_CHANNEL_MATCHROOM,
                EMChatChannelType.CHAT_CHANNEL_CAMP,
                EMChatChannelType.CHAT_CHANNEL_PRIVATE,
                };
                View.refreshChannelDropDown();

                View.ChangeChannel((int)EMChatChannelType.CHAT_CHANNEL_CAMP);


                View.ClearChannelMaskItem();
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_All);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Personal);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Public);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Room);
                View.m_DiyChannelGroupItem = View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_Diy);
                View.m_DiyChannelGroupItem.setChannelMask(View.m_DiyChannelMask);

                // 默认选择房间
                View.channelMaskButtonGroup.SetItemSelectedItem(3, true);

                // 放大聊听框
                View.SetSize(View.m_InitSize);

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = View.fpsHintInitPosition;

                // 调整聊天框层级
                View.transform.SetAsLastSibling();
                View.gameObject.SetActive(true);
                (View.m_wnd as ChatBoxWnd).resetAllSortingOrder();

                View.SetHide(false);

                View.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEDOWN);

                m_HideTime = Time.time;

                View.DelayChatMessageUpdate();
            }

            override public void OnLeave()
            {

            }

            override public void OnUpdate()
            {
                if (View.detectMouseInWnd())
                {
                    m_HideTime = Time.time;
                }

                // 时间超过3s，隐藏外框
                if (m_HideTime > 0f && m_HideTime + 3f <= Time.time)
                {
                    View.SetHide(true);
                    m_HideTime = 0f;
                }

                // 输入框激活状态是否变化
                if (View.m_isInputFieldFocus != View.inputField.isFocused)
                {
                    if (View.m_isInputFieldFocus)
                    {
                        m_HideTime = Time.time;
                    }
                    else
                    {
                        View.SetHide(false);
                    }

                    View.m_isInputFieldFocus = View.inputField.isFocused;
                    InputManager.Available = !View.m_isInputFieldFocus;
                    ImeManager.ActiveIme(View.m_isInputFieldFocus);
                }
            }

            override public void OnNewChatMsg()
            {
                View.SetHide(false);
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.WarLoadingState;
            }

            // 变为激活状态
            private void setActiveStatus()
            {
                // 显示频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(true);
                View.channelCanvasGroup.alpha = 1;

                // 显示最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(true);
                View.minimunCanvasGroup.alpha = 1;

                // 放大聊听框
                View.SetSize(View.m_InitSize);

                if(View.IsVisible())
                    View.StartCoroutine(View.showLastChatContent());

                // fps英雄鼠标操作提示位置
                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = View.fpsHintInitPosition;
            }
        }

        // 战场
        public class ChatBoxWarScene : ChatBoxBase
        {
            public ChatBoxWarScene(ChatBoxWndView view) : base(view)
            {

            }

            override public void OnEnter(ChatBoxState oldState)
            {
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_SYSTEM);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_ZONE);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_NEARBY);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_TEAMMATE);

                View.SetActive(true);
                View.SetHide(false);
                View.setActiveChatFrame();

                // 显示频道组选择框
                View.channelCanvasGroup.gameObject.SetActive(true);
                // 显示最小化按钮
                View.minimunCanvasGroup.gameObject.SetActive(true);

                // 设置需要显示的频道选项对应的频道
                View.m_dropDownIndexToChannel = new EMChatChannelType[]{
                EMChatChannelType.CHAT_CHANNEL_WORLD,
                EMChatChannelType.CHAT_CHANNEL_CLAN,
                EMChatChannelType.CHAT_CHANNEL_KIN,
                EMChatChannelType.CHAT_CHANNEL_WARSCENE,
                EMChatChannelType.CHAT_CHANNEL_NEARBY,
                EMChatChannelType.CHAT_CHANNEL_CAMP,
                EMChatChannelType.CHAT_CHANNEL_PRIVATE,
                };
                View.refreshChannelDropDown();
                View.ChangeChannel((int)EMChatChannelType.CHAT_CHANNEL_CAMP);

                View.m_hideFrameAlpha = 0f;
                View.m_hideContentAlpha = 0f;

                View.ClearChannelMaskItem();
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_All);
                View.AddChannelMaskItem(ChatChannelGroupType.ChatChannel_WarScene);

                // 默认选择区域
                View.channelMaskButtonGroup.SetItemSelectedItem(1, true);

                View.SetChatWndViewPositionChange(ChatPositionChangeType.CPCY_MOVEUP);

                View.fpsSwitchHint.GetComponent<RectTransform>().anchoredPosition = View.fpsHintInitPosition;

                // 隐藏频道选择按钮组
                View.channelCanvasGroup.gameObject.SetActive(false);

                // 设置初始大小
                //View.SetSize(new Vector2(320f, 290f));
                //View.resizer.resizeMinWidth = 320f;
                //View.resizer.resizeMinHeight = 290f;
            }

            override public void OnLeave()
            {
                // 清空消息
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_WARSCENE);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_CAMP);
                LogicDataCenter.chatMessageManager.RemoveChannelChatMessage(EMChatChannelType.CHAT_CHANNEL_NEARBY);

                View.contentView.Clear();

                // 显示频道选择按钮组
                View.channelCanvasGroup.gameObject.SetActive(true);
                View.setActiveChatFrame();
            }

            override public void OnUpdate()
            {
                bool isMouseInWnd = View.detectMouseInWnd();

                if (View.m_hideFrame)
                {
                    if(isMouseInWnd)
                    {
                        View.SetContentHide(false);
                    }

                    if (/*(isMouseInWnd && !isFightState()) ||*/ View.inputField.isFocused || View.m_disableFade)
                    {
                        View.SetFrameHide(false);
                    }
                    else if (View.contentCanvasGroup.gameObject.activeSelf)
                    {
                        if (View.borderCanvasGroup.alpha > View.m_hideFrameAlpha)
                        {
                            View.emoticonPanel.gameObject.SetActive(false);
                            View.borderCanvasGroup.alpha -= Time.deltaTime * 0.5f;
                            if (View.borderCanvasGroup.alpha < View.m_hideFrameAlpha)
                                View.borderCanvasGroup.alpha = View.m_hideFrameAlpha;

                            View.minimunCanvasGroup.alpha = View.borderCanvasGroup.alpha;
                            View.channelCanvasGroup.alpha = View.borderCanvasGroup.alpha;

                            View.UpdateContentScrollViewBarAlpha(Time.deltaTime);

                            View.setResizerImageAlpha(View.borderCanvasGroup.alpha);

                            View.m_showContentTime = Time.time;
                        }
                        else
                        {
                            if (View.m_hideContent)
                            {
                                if (View.contentCanvasGroup.alpha > View.m_hideContentAlpha)
                                {
                                    View.contentCanvasGroup.alpha -= Time.deltaTime * 0.5f;
                                    if (View.contentCanvasGroup.alpha < View.m_hideContentAlpha)
                                        View.contentCanvasGroup.alpha = View.m_hideContentAlpha;
                                }
                            }
                            else
                            {
                                if (View.m_showContentTime + View.hideContentTime < Time.time)
                                    View.SetContentHide(true);
                            }
                        }
                    }
                }
                else
                {
                    if (isMouseInWnd == false && View.inputField.isFocused == false)
                    {
                        // 聊听外框消失延迟
                        if (View.m_showFrameTime > 0f)
                        {
                            if (View.m_showFrameTime + View.hideFrameTime < Time.time)
                            {
                                View.SetFrameHide(true);
                                View.m_showFrameTime = 0f;
                            }
                        }
                        else
                        {
                            View.m_showFrameTime = Time.time;
                        }
                    }
                }

                // 输入框激活状态是否变化
                if (View.m_isInputFieldFocus != View.inputField.isFocused)
                {
                    View.m_isInputFieldFocus = View.inputField.isFocused;
                    InputManager.Available = !View.m_isInputFieldFocus;
                    ImeManager.ActiveIme(View.m_isInputFieldFocus);
                }
            }

            override public void OnNewChatMsg()
            {
                View.SetContentHide(false);
                View.m_showContentTime = Time.time;
            }

            override public ChatBoxState GetChatBoxState()
            {
                return ChatBoxState.WarState;
            }


            // 是否战斗状态
            private bool isFightState()
            {
                return EntityViewCommandHandler.mainHeroState == (int)ENTITY_STATE.ENTITY_STATE_FIGHT;
            }
        }
    }
}