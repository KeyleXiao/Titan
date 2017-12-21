/*******************************************************************
** 文件名:	SNSChatWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/12
** 版  本:	1.0
** 描  述:	SNS聊听窗口
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.Common;
using DataCenter;
using DG.Tweening;
using USpeedUI.UWidgets;
using UIWidgets;
using GameLogic;
using ASpeedGame.Data.ShieldWorld;
using UnityEngine.EventSystems;
using USpeedUI.UExtensions;

namespace USpeedUI.SNS
{
	public class SNSChatWnd: UIPopupWnd<SNSChatWndView>
	{
		public override WndID GetID()
		{
			return WndID.WND_ID_SNS_CHAT;
		}

		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP02;
		}

		public override string GetPrefabPath()
		{
			return "SNS/SNSChatWndView";
		}

		public override bool Init()
		{
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_ADD_CHAT_MESSAGE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_BLACKLIST_BY_OTHER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SELECT_SESSION, this);

			return base.Init();
		}

		public override void Destroy()
		{
			base.Destroy();
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_ADD_CHAT_MESSAGE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_BLACKLIST_BY_OTHER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SELECT_SESSION, this);
		}

		public override void OnMessage(WndMsgID msgID, UIMsgData data)
		{
			switch (msgID)
			{
				case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
					{
						// 进入登录界面,清理上一局数据
						if (m_wndView != null)
						{
							m_wndView.Clear();
						}

                        UnloadView();
                    }
					break;
				case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
					{
                        LoadView();
                    }
					break;
				case WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND:
					{
						SetVisible(true);
						if (m_wndView != null)
						{
							m_wndView.onOpenChatWnd(data as SNSOpenChatWndMsgData);
						}
					}
					break;
				case WndMsgID.WND_MSG_SNS_ADD_CHAT_MESSAGE:
					{
						if (m_wndView != null)
						{
							m_wndView.onAddMsgData(data as SNSAddChatMsgData);
						}
					}
					break;
				case WndMsgID.WND_MSG_SNS_BLACKLIST_BY_OTHER:
					{
						if (m_wndView != null)
						{
							m_wndView.onAddBlackListByOther(data as SNSBlackListByOtherMsgData);
						}
					}
					break;
                case WndMsgID.WND_MSG_SNS_SELECT_SESSION:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.onSelectSession(data as SNSSelectSessionMsgData);
                        }
                    }
                    break;
                default:
					break;
			}
		}
	}

    // 基础逻辑
	public partial class SNSChatWndView : UIBaseWndView
	{
        class SNSSession
        {
            public long SessionID = 0; // 目前该ID等于好友的UserID
            public int nCurHistoryPage = 0;
        }

        public UBuddySessionListView SessionListView;
        public Button MinBtn;
        public Button CloseBtn;

        // 会话列表<会话ID,会话>
        private Dictionary<long, SNSSession> m_SessionList = new Dictionary<long, SNSSession>();

        // 滴答列表<会话ID,会话>
        private Dictionary<long, List<DIDARemoveData>> m_DIDAList = new Dictionary<long, List<DIDARemoveData>>();

        private SNSSession CurSession {
            get {
                if(m_SessionList.ContainsKey(m_CurSessionID))
                {
                    return m_SessionList[m_CurSessionID];
                }
                return null;
            }
        }

        public override bool Init(IUIWnd wnd)
		{
			base.Init(wnd);

            UTooltipTrigger sendBtnToolTipTrigger = SendBtn.gameObject.GetComponent<UTooltipTrigger>();
            string sendBtnTips = ULocalizationService.Instance.Get("UIView", "SNS", "SendBtnTips");
            UBB.toHtml(ref sendBtnTips);
            sendBtnToolTipTrigger.SetText(UTooltipParamName.BodyText, sendBtnTips);

            MinBtn.onClick.AddListener(onMinWnd);
            CloseBtn.onClick.AddListener(onCloseWnd);
            SendBtn.onClick.AddListener(onClickSendBtn);

            return true;
		}

		public void Clear()
		{
            m_SessionList.Clear();
            m_DIDAList.Clear();
        }


        public void onOpenChatWnd(SNSOpenChatWndMsgData data)
        {
			if (data == null)
				return;

			if((data.SessionID <= 0) || (data.SessionID == LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID))
			{
				Debug.LogError("打开聊天框失败, 传入的SessionID为0或为自己的UserID, nSessionID=" + data.SessionID);
				return;
			}

            // 保存旧的Session信息
            if(CurSession != null)
            {
                CurSession.nCurHistoryPage = m_CurPage;
            }

            m_CurSessionID = data.SessionID;
            // 不存在对话
            if(!m_SessionList.ContainsKey(m_CurSessionID))
            {
                SNSSession session = new SNSSession();
                session.SessionID = m_CurSessionID;
                session.nCurHistoryPage = 0;
                m_SessionList.Add(m_CurSessionID, session);
            }

            onUpdateView();
        }

        private void onUpdateView()
        {
            // 显示窗口，没有就创建窗口
            onUpdateChatMessage();

            // 更新聊听框的好友信息
            onUpdateBuddyInfo();

            // 移除DIDA提示消息
            removeNewMsgDIDA();

            // 更新Session列表UI
            SessionListView.DataSource.BeginUpdate();
            SessionListView.DataSource.Clear();
            foreach (var session in m_SessionList)
            {
                UBuddySessionListItem item = new UBuddySessionListItem();
                item.UserID = (int)session.Value.SessionID;
                item.isCurSession = (session.Value.SessionID == m_CurSessionID);
                SessionListView.DataSource.Add(item);
            }

            SessionListView.DataSource.EndUpdate();

            // 更新历史记录
            if(ChatHistroyView.isActiveAndEnabled)
            {
                initHistoryMsg(CurSession.nCurHistoryPage);
            }
        }

        public void onAddMsgData(SNSAddChatMsgData data)
        {
			if (data == null)
				return;

			long SessionID = data.SessionID;

            // 如果窗口已经打开了，则更新到窗口
            if (IsVisible() && m_CurSessionID == SessionID)
            {
                m_CurSessionID = SessionID;
                onUpdateView();
            }
            else   // 如果窗口没打开，提示有新的消息
            {
                // 新的聊听消息
                DIDAAddData msg = new DIDAAddData();
                msg.eLayout = EDIDALayout.EDT_Left;
                msg.nDIDATime = 60;
                msg.nItemID = (int)EDIDAType.EDIDA_Chat;
                msg.bAllowRepeatItem = true;
                msg.nButtonIconID = (int)EDIDAType.EDIDA_Chat;
                msg.strTooltip = ULocalizationService.Instance.Get("UIView", "SNS", "NewChatMsgDesc");
                msg.bShowInWar = true;


                SNSNewChatMsgInfo newChatInfo = new SNSNewChatMsgInfo();
                newChatInfo.SessionID = SessionID;
                msg.sCustomData = newChatInfo;
                msg.callBack = (CustomerInfo) =>
                {
					// 显示聊天框
					SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
					msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
					msgData.SessionID = SessionID;
					UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
                };

                LogicDataCenter.didaDataManager.AddItem(msg);

                // 添加DIDA到窗口缓存
                DIDARemoveData removeMsg = new DIDARemoveData();
                removeMsg.nItemID = msg.nItemID;
                removeMsg.sCustomData = newChatInfo;
                addNewMsgDIDA(SessionID, removeMsg);
            }
        }

        public void onAddBlackListByOther(SNSBlackListByOtherMsgData data)
        {
			if (data == null)
				return;

			int dwUserID = data.dwUserID;

            // TODO 如果有该窗口，则销毁该窗口
			//SNSChatSubWndView subWndView = null;
   //         string sessionName = LogicDataCenter.snsDataManager.getSessionName(dwUserID);
   //         if (m_subChatWndList.TryGetValue(sessionName, out subWndView))
   //         {
   //             LogicDataCenter.snsDataManager.SessionManager.delSession(sessionName);
			//	m_subChatWndList.Remove(sessionName);

			//	GameObject go = subWndView.gameObject;
			//	ResNode.DestroyRes(ref go);
			//}
        }

   //     private bool isChatWndAlreadyOpen(int nSessionID)
   //     {
			//SNSChatSubWndView subWndView = null;

			//string sessionName = LogicDataCenter.snsDataManager.getSessionName(nSessionID);
   //         if (m_subChatWndList.TryGetValue(sessionName, out subWndView))
   //         {
   //             return subWndView.gameObject.activeSelf;
   //         }
   //         else
   //         {
   //             return false;
   //         }
   //     }

        private void addNewMsgDIDA(long SessionID, DIDARemoveData removeData)
        {
            if(!m_DIDAList.ContainsKey(SessionID))
            {
                m_DIDAList.Add(SessionID, new List<DIDARemoveData>());
            }
            m_DIDAList[SessionID].Add(removeData);
        }

        public void onSelectSession(SNSSelectSessionMsgData msgData)
        {
            if(msgData.dwUserID == 0)
            {
                Debug.LogError("onSelectSession,msgData.dwUserID == 0");
                return;
            }

            m_CurSessionID = msgData.dwUserID;

            onUpdateView();
        }

        // 最小化聊听窗口
        public void onMinWnd()
        {
            gameObject.SetActive(false);
        }

        // 关闭聊天框窗口
        public void onCloseWnd()
        {
            gameObject.SetActive(false);
        }
    }

    // 聊天逻辑
	public partial class SNSChatWndView : UIBaseWndView
    {
        public SNSChatContentView ChatContentView;
        public InputField ChatInputField;
        public Text BuddyName;
		public Button SendBtn;

        private long m_CurSessionID = 0;

        // 是否按下了Ctrl键
        private bool m_isLeftAltDown = false;

		public void OnScrollValueChange(float v)
		{
		}

		public override void Update()
        {
            if (Input.GetKeyDown(KeyCode.LeftAlt))
            {
				m_isLeftAltDown = true;
            }

            if (Input.GetKeyUp(KeyCode.LeftAlt))
            {
				m_isLeftAltDown = false;
            }

            if (!m_isLeftAltDown && Input.GetKeyDown(KeyCode.Return))
            {
                onClickSendBtn();
            }

            if (m_isLeftAltDown && Input.GetKeyDown(KeyCode.Return))
			{
				ChatInputField.text += "\n";
				ChatInputField.MoveTextEnd(false);
			}
		}

        public void onUpdateChatMessage()
        {
            // 获取所有聊听记录
            //var unReadMsgList = LogicDataCenter.snsDataManager.SessionManager.getUnReadMsg(m_CurSessionID);
            var allMsgList = LogicDataCenter.snsDataManager.SessionManager.getAllMsg(m_CurSessionID);

            ChatContentView.DataSource.BeginUpdate();
            ChatContentView.DataSource.Clear();
            foreach (var item in allMsgList)
            {
                SNSMsgContentItem data = new SNSMsgContentItem();
                data.info = item;
                ChatContentView.DataSource.Add(data);
            }
            ChatContentView.DataSource.EndUpdate();

			ChatContentView.GetScrollRect().normalizedPosition = new Vector2(0f, 0f);
		}

		public void onUpdateBuddyInfo()
        {
            DataCenter.SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID((int)m_CurSessionID);

            BuddyName.text = buddyInfo.Info.szName;

            if (buddyInfo.Info.nStatus == (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
            {
                BuddyName.color = Color.gray;
            }
            else
            {
                BuddyName.color = Color.white;
            }
        }

        public void removeNewMsgDIDA()
        {
            if(m_DIDAList.ContainsKey(m_CurSessionID))
            {
                foreach(var item in m_DIDAList[m_CurSessionID])
                {
                    LogicDataCenter.didaDataManager.RemoveItem(item);
                }
                m_DIDAList[m_CurSessionID].Clear();
            }
        }

        public void onClickSendBtn()
        {
            string sendContent = ChatInputField.text;
            if (sendContent.Trim().Length == 0)
            {
                return;
            }

            // 对方是否把自己加入黑名单
            //if (LogicDataCenter.snsDataManager.getBuddyRelationStatus((int)m_CurSessionID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST_BY_OTHER)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_BLACKLIST_BY_OTHER);
            //    return;
            //}

            // 处理屏蔽词
            ShieldWord.Instance.filterShieldWord(ref sendContent);

            //gamelogic_sns_msg_data data = new gamelogic_sns_msg_data();
            //data.dwFromAccountID = LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID;
            //data.dwToAccountID = SessionID;
            //data.nFontColor = 0;
            //data.nFontSize = 16;
            //data.szFont = "";
            //data.szContent = sendContent;
            //data.szFromName = LogicDataCenter.snsDataManager.MyBuddyInfo.Info.szName;
            //data.dwTime = Api.convertDateTime2TimeStamp(DateTime.Now);
            //LogicDataCenter.snsDataManager.SessionManager.addMsgData(data);

            LogicDataCenter.snsDataManager.reqSendChatData((int)m_CurSessionID, sendContent, (int)0xFFFFFF);

            ChatInputField.text = "";
        }

        public void onClickEmotionBtn()
        {
            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");

        }

        public void onClickFontBtn()
        {
            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");

        }
    }

    // 历史记录逻辑
    public partial class SNSChatWndView : UIBaseWndView
    {
        public SNSChatHistoryView ChatHistroyView;

        // 每一页的聊听消息记录数目
        public const int PerPageNum = 10;

        // 历史消息页数状态
        public Text PageStatus;

        private int m_HistoryMsgCount = 0;
        // 历史消息当前页数
        private int m_CurPage = 0;
        // 最大页数
        private int m_MaxPage = 0;


        /// <summary>
        /// 打开或关闭聊听窗口
        /// </summary>
        public void onSwitchChatHistroyWnd()
        {
            if (ChatHistroyView.gameObject.activeSelf)
            {
                ChatHistroyView.gameObject.SetActive(false);
            }
            else
            {
                ChatHistroyView.gameObject.SetActive(true);

                initHistoryMsg(0);

                //StartCoroutine(lateUpdateHistoryMsg());
            }

        }

        public void onPrePage()
        {
            m_CurPage--;
            if (m_CurPage <= 0)
            {
                m_CurPage = 1;
                return;
            }

            onUpdateHistoryMsg();
        }

        public void onNextPage()
        {
            m_CurPage++;
            if (m_CurPage > m_MaxPage)
            {
                m_CurPage = m_MaxPage;
                return;
            }

            onUpdateHistoryMsg();
        }

        //private IEnumerator lateUpdateHistoryMsg()
        //{
        //    // 延迟下更新,不然会报错
        //    yield return new WaitForSeconds(0.1f);

        //    onUpdateHistoryMsg();
        //}

        public void initHistoryMsg(int nCurPage)
        {
            // 加载聊听历史记录
            m_HistoryMsgCount = LogicDataCenter.snsDataManager.SessionManager.loadAllHistroyMsg(m_CurSessionID);

            // 计算最大页数
            m_MaxPage = m_HistoryMsgCount / PerPageNum;
            if (m_HistoryMsgCount % PerPageNum != 0)
            {
                m_MaxPage += 1;
            }

            // 使用最新一页的内容
            if(nCurPage == 0)
            {
                m_CurPage = m_MaxPage;
            }
            else
            {
                m_CurPage = nCurPage;
            }

            onUpdateHistoryMsg();
        }

        private void onUpdateHistoryMsg()
        {
            // 更新内容
            var msgList = LogicDataCenter.snsDataManager.SessionManager.getHistoryMsg(m_CurSessionID, m_CurPage, PerPageNum);
            ChatHistroyView.DataSource.BeginUpdate();
            ChatHistroyView.DataSource.Clear();
            foreach (var msg in msgList)
            {
                SNSHistroyMsgItem item = new SNSHistroyMsgItem();
                item.nFromAcountID = msg.nFromAcountID;
                item.nToAcountID = msg.nToAcountID;
                item.nMsgID = msg.nMsgID;
                item.nTime = msg.nTime;
                item.szContent = msg.szContent;
                item.szName = msg.szFromName;
                item.objList = LogicDataCenter.chatMessageManager.parseChatContent(msg.szContent, Color.white);

                ChatHistroyView.DataSource.Add(item);
            }

            ChatHistroyView.DataSource.EndUpdate();

            // 更新页数状态
            PageStatus.text = m_CurPage + "/" + m_MaxPage;
        }
    }
}
