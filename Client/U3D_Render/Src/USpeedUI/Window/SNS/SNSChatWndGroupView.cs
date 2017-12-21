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
    public class SNSChatWndGroupView : MonoBehaviour
    {
        public GameObject ChatWndDefaultITem;
        private Dictionary<string, GameObject> m_subChatWndList = new Dictionary<string, GameObject>();

        public void Init()
        {

        }

        public void onOpenChatWnd(int nSessionID)
        {
			if((nSessionID <= 0) || (nSessionID == LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID))
			{
				Debug.LogError("打开聊天框失败, 传入的SessionID为0或为自己的UserID, nSessionID=" + nSessionID);
				return;
			}

            // 显示窗口，没有就创建窗口
            string sessionName = LogicDataCenter.snsDataManager.getSessionName(nSessionID);
            GameObject subChatWndObj = null;
            if (!m_subChatWndList.TryGetValue(sessionName, out subChatWndObj))
            {
                subChatWndObj = ResNode.InstantiateRes(ChatWndDefaultITem);
                subChatWndObj.SetActive(true);
                subChatWndObj.transform.SetParent(ChatWndDefaultITem.transform.parent, false);
                m_subChatWndList.Add(sessionName, subChatWndObj);
				subChatWndObj.GetComponent<SNSChatWndView>().Init();
                subChatWndObj.GetComponent<SNSChatWndView>().SessionName = sessionName;
                subChatWndObj.GetComponent<SNSChatWndView>().SessionID = nSessionID;
            }
            subChatWndObj.SetActive(true);

            // 更新消息显示
            subChatWndObj.GetComponent<SNSChatWndView>().onUpdateChatMessage();
            // 更新聊听框的好友信息
            subChatWndObj.GetComponent<SNSChatWndView>().onUpdateBuddyInfo();

            // 移除DIDA提示消息
            subChatWndObj.GetComponent<SNSChatWndView>().removeNewMsgDIDA();
        }


        public void onAddMsgData(int nSessionID)
        {
            // 如果窗口已经打开了，则更新到窗口
            if (isChatWndAlreadyOpen(nSessionID))
            {
                GameObject subChatWndObj;

                string sessionName = LogicDataCenter.snsDataManager.getSessionName(nSessionID);
                if (m_subChatWndList.TryGetValue(sessionName, out subChatWndObj))
                {
                    subChatWndObj.GetComponent<SNSChatWndView>().onUpdateChatMessage();
                }
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

                SNSNewChatMsgInfo newChatInfo = new SNSNewChatMsgInfo();
                newChatInfo.nSessionID = nSessionID;
                msg.sCustomData = newChatInfo;
                msg.callBack = (CustomerInfo) =>
                {
                    onOpenChatWnd(nSessionID);
                };

                LogicDataCenter.didaDataManager.AddItem(msg);

                // 添加DIDA到窗口缓存
                DIDARemoveData removeMsg = new DIDARemoveData();
                removeMsg.nItemID = msg.nItemID;
                removeMsg.sCustomData = newChatInfo;
                addNewMsgDIDA(nSessionID, removeMsg);
            }
        }

        public void onAddBlackListByOther(int dwUserID)
        {
            // 如果有该窗口，则销毁该窗口
            GameObject subChatWndObj;
            string sessionName = LogicDataCenter.snsDataManager.getSessionName(dwUserID);
            if (m_subChatWndList.TryGetValue(sessionName, out subChatWndObj))
            {
                LogicDataCenter.snsDataManager.SessionManager.delSession(sessionName);
				m_subChatWndList.Remove(sessionName);
				ResNode.DestroyRes(ref subChatWndObj);
			}
        }

        private bool isChatWndAlreadyOpen(int nSessionID)
        {
            GameObject obj;

            string sessionName = LogicDataCenter.snsDataManager.getSessionName(nSessionID);
            if (m_subChatWndList.TryGetValue(sessionName, out obj))
            {
                return obj.activeSelf;
            }
            else
            {
                return false;
            }
        }

        private void addNewMsgDIDA(int nSessionID, DIDARemoveData removeData)
        {
            // 显示窗口，没有就创建窗口
            string sessionName = LogicDataCenter.snsDataManager.getSessionName(nSessionID);
            GameObject subChatWndObj = null;
            if (!m_subChatWndList.TryGetValue(sessionName, out subChatWndObj))
            {
                subChatWndObj = ResNode.InstantiateRes(ChatWndDefaultITem);
                subChatWndObj.SetActive(false);
                subChatWndObj.transform.SetParent(ChatWndDefaultITem.transform.parent, false);
                m_subChatWndList.Add(sessionName, subChatWndObj);
				subChatWndObj.GetComponent<SNSChatWndView>().Init();
				subChatWndObj.GetComponent<SNSChatWndView>().SessionName = sessionName;
                subChatWndObj.GetComponent<SNSChatWndView>().SessionID = nSessionID;
            }
            // 增加DIDA提示消息
            subChatWndObj.GetComponent<SNSChatWndView>().addNewMsgDIDA(removeData);
        }
    }

    public class SNSChatWndView : MonoBehaviour
    {
        public SNSChatContentView ChatContentView;
        public SNSChatHistoryView ChatHistroyView;
        public InputField ChatInputField;
        public Image BuddyAvatar;
        public Text BuddyName;
        public Text BuddyStatus;
        public Text BuddyLevel;
		public Button SendBtn;

        // 每一页的聊听消息记录数目
        public int PerPageNum = 10;

        // 历史消息页数状态
        public Text PageStatus;

        private string m_SessionName = "";
        public string SessionName { set { m_SessionName = value; } get { return m_SessionName; } }
        public int SessionID { set; get; }

        // 历史消息总数
        private int m_HistoryMsgCount = 0;
        // 历史消息当前页数
        private int m_CurPage = 0;
        // 当前页数
        private int m_MaxPage = 0;

        // 是否按下了Ctrl键
        private bool m_isLeftAltDown = false;

        // 缓存新消息DIDA
        private List<DIDARemoveData> m_NewMsgDIDAList = new List<DIDARemoveData>();

		public void Init()
		{
			UTooltipTrigger sendBtnToolTipTrigger = SendBtn.gameObject.GetComponent<UTooltipTrigger>();
			string sendBtnTips = ULocalizationService.Instance.Get("UIView", "SNS", "SendBtnTips");
			UBB.toHtml(ref sendBtnTips);
			sendBtnToolTipTrigger.SetText(UTooltipParamName.BodyText, sendBtnTips);
		}

		public void OnScrollValueChange(float v)
		{
			Debug.Log(v);
		}

		public void Update()
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

        /// <summary>
        /// 关闭聊听窗口
        /// </summary>
        public void onCloseWnd()
        {
            gameObject.SetActive(false);
        }

        /// <summary>
        /// 关闭聊听历史窗口
        /// </summary>
        public void onCloseChatHistroyWnd()
        {
            ChatHistroyView.gameObject.SetActive(false);
        }

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
                // 加载聊听历史记录
                m_HistoryMsgCount = LogicDataCenter.snsDataManager.SessionManager.loadAllHistroyMsg(m_SessionName);

                // 计算最大页数
                m_MaxPage = m_HistoryMsgCount / PerPageNum;
                if (m_HistoryMsgCount % PerPageNum != 0)
                {
                    m_MaxPage += 1;
                }
                // 获取最新一页的内容
                m_CurPage = m_MaxPage;

				StartCoroutine(lateUpdateHistoryMsg());
			}

		}

        /// <summary>
        /// 最小化聊听窗口
        /// </summary>
        public void onMinWnd()
        {
            gameObject.SetActive(false);
        }

        public void onUpdateChatMessage()
        {
            // 获取所有聊听记录
            var unReadMsgList = LogicDataCenter.snsDataManager.SessionManager.getUnReadMsg(m_SessionName);


            foreach (var item in unReadMsgList)
            {
                SNSMsgContentItem data = new SNSMsgContentItem();
                data.info = item;
                ChatContentView.DataSource.Add(data);
            }

			ChatContentView.GetScrollRect().normalizedPosition = new Vector2(0f, 0f);
		}

		public void onUpdateBuddyInfo()
        {
            DataCenter.SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(SessionID);

            BuddyAvatar.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_PLAYERFRAME, 2, 1);
            BuddyName.text = buddyInfo.Info.szName;
            BuddyLevel.text = ULocalizationService.Instance.Get("UIView", "SNS", "Level") + buddyInfo.Info.nLevel;
            BuddyStatus.text = LogicDataCenter.snsDataManager.getBuddyStatusDesc(buddyInfo);

			if(buddyInfo.Info.nStatus == (byte)EMSNS_BUDDY_STATUS.EMSNS_BUDDY_STATUS_OFFLINE)
			{
				BuddyAvatar.AddNewMaterial(EUIShaderType.EST_Gray);
			}
			else
			{
				BuddyAvatar.ResetToDefaultMaterial();
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

		private IEnumerator lateUpdateHistoryMsg()
		{
			// 延迟下更新,不然会报错
			yield return new WaitForSeconds(0.1f);

			onUpdateHistoryMsg();
		}

        private void onUpdateHistoryMsg()
        {
            // 更新内容
            var msgList = LogicDataCenter.snsDataManager.SessionManager.getHistoryMsg(m_SessionName, m_CurPage, PerPageNum);
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

                ChatHistroyView.DataSource.Add(item);
            }

            ChatHistroyView.DataSource.EndUpdate();

            // 更新页数状态
            updatePageStatus();
        }

        private void updatePageStatus()
        {
            PageStatus.text = m_CurPage + "/" + m_MaxPage;
        }

        public void addNewMsgDIDA(DIDARemoveData removeData)
        {
            if (removeData == null)
            {
                return;
            }

            m_NewMsgDIDAList.Add(removeData);
        }

        public void removeNewMsgDIDA()
        {
            foreach (var item in m_NewMsgDIDAList)
            {
                LogicDataCenter.didaDataManager.RemoveItem(item);
            }
            m_NewMsgDIDAList.Clear();
        }

        public void onClickSendBtn()
        {
            string sendContent = ChatInputField.text;
            if (sendContent.Trim().Length == 0)
            {
                return;
            }

            // 对方是否把自己加入黑名单
            if (LogicDataCenter.snsDataManager.getBuddyRelationStatus(SessionID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST_BY_OTHER)
            {
                GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_BLACKLIST_BY_OTHER);
                return;
            }

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

            LogicDataCenter.snsDataManager.reqSendChatData(SessionID, sendContent, (int)0xFFFFFF);

            ChatInputField.text = "";
        }

        public void onClickEmotionBtn()
        {
            GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");

        }

        public void onClickFontBtn()
        {
            GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");

        }
    }

}
