/*******************************************************************
** 文件名:	SNSDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016/12/7
** 版  本:	1.0
** 描  述:	好友系统数据管理
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ASpeedGame.Data.RunePage;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI.PlayerFrame;
using USpeedUI;
using UnityEngine;
using U3D_Render.Common;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UWidgets;
using U3D_Render;
using USpeedUI.Chat;

namespace DataCenter
{
	public class SNSDataManager
	{
		public struct SSNSChatMessageInfo
		{
			public gamelogic_sns_msg_data msg;

			public List<SChatMessageObjectInfo> objList;    // 解析后的对象列表
		}

		[Serializable]
		public class HistroyMsg
		{
			public int nMsgID = 0;
			public int nFromAcountID = 0;
			public int nToAcountID = 0;
			public string szFromName = "";
			public string szContent = "";
			public int nTime = 0;
		}

		public class BuddyInfo : IComparable
		{
			public gamelogic_sns_buddy Info = new gamelogic_sns_buddy();

			public int CompareTo(object o)
			{
				if (this.Info.nStatus != (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE
					&& ((BuddyInfo)o).Info.nStatus == (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
					return -1;
				else if (this.Info.nStatus == (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE
                    && ((BuddyInfo)o).Info.nStatus != (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
					return 1;
				else
					return 0;
			}
		}

		public class BuddyRelation
		{
			public gamelogic_sns_buddyrelation Info = new gamelogic_sns_buddyrelation();
		}

		public class BuddyGroup
		{
			public gamelogic_sns_buddygroup Info = new gamelogic_sns_buddygroup();
			public int Priority = 0;
			public bool isExpanded = false;
		}

		public class PermissionSetting
		{
			public bool isDontShowOffline = false;
		}



		public PermissionSetting SettingMgr { get; set; }

		// 好友分组 <好友分组ID，好友分组信息>
		private Dictionary<int, BuddyGroup> m_dicBuddyGroup;

		// 聊听会话管理
		private SNSSessionManager m_sessionManager;
		public SNSSessionManager SessionManager { get { return m_sessionManager; } }

		// 所有好友
		private Dictionary<int, BuddyInfo> m_dicAllBuddy;

		// 好友关系
		private Dictionary<int, BuddyRelation> m_dicBuddyRelation;

		// 自己的信息
		private int m_selfUserID;
		public BuddyInfo MyBuddyInfo
		{
			get
			{
				BuddyInfo buddy;
				if (m_dicAllBuddy == null || !m_dicAllBuddy.TryGetValue(m_selfUserID, out buddy))
				{
					return new BuddyInfo();
				}
				return buddy;
			}
		}

		// 自己的状态是否为离开
		public bool IsLeave
		{
			get
			{
				return MyBuddyInfo.Info.nStatus == (byte)ACTOR_GAME_STATE.GAME_STATE_LEAVE;
			}
		}

		// 搜索好友
		private SNSSearchBuddyMgr searchBuddyMgr;

		public void Init()
		{
			m_dicBuddyGroup = new Dictionary<int, BuddyGroup>();
			m_dicAllBuddy = new Dictionary<int, BuddyInfo>();
			m_dicBuddyRelation = new Dictionary<int, BuddyRelation>();

			m_sessionManager = new SNSSessionManager();

			m_selfUserID = 0;

			SettingMgr = new PermissionSetting();

			searchBuddyMgr = new SNSSearchBuddyMgr();

			initBuddyGroup();
		}

		public void Clear()
		{
			m_dicAllBuddy.Clear();
			m_dicBuddyGroup.Clear();
			m_dicBuddyRelation.Clear();
			m_sessionManager.Clear();
		}

		#region 客户端收到消息
		// 收到聊听消息
		public void onRecvMsgData(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_msg_data msgData = helper.get<gamelogic_sns_msg_data>();

			SessionManager.addMsgData(msgData);
		}

		// 收到好友关系变更消息
		public void onRecvUpdateRelationship(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_update_relationship data = helper.get<gamelogic_sns_update_relationship>();

            switch(data.nStatus)
            {
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY:
                    {
				        onBecomeBuddy(data.dwUserID, data.szActorName);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST:
                    {
                        BuddyRelation outRelation;
                        if (m_dicBuddyRelation.TryGetValue(data.dwUserID, out outRelation))
                        {
                            outRelation.Info.dwBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST;
                        }

                        // 获取黑名单玩家的信息
                        reqBuddyInfo(data.dwUserID);
                        reqBuddyRelation(data.dwUserID);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST_BY_OTHER:
                    {
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_DELETE_OTHER:
                    {
                        updateBuddyRelationStatus(data.dwUserID, data.nStatus);

                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_DELETED_BY_OTHER:
                    {
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_RECV_INVITE:
                    {
                        showAskAddBuddy(data.dwUserID, data.szActorName);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_SEND_INVITE:
                    {
				        updateBuddyRelationStatus(data.dwUserID, data.nStatus);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_REJECT_OTHER:
                    {
				        updateBuddyRelationStatus(data.dwUserID, data.nStatus);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_REJECTED_BY_OTHER:
                    {
                        //UIDialogData toolTipData = new UIDialogData(
                        //    title: ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndTitle"),
                        //    message: data.szActorName + ULocalizationService.Instance.Get("UIView", "SNS", "RefuseAddBuddyDesc"),
                        //    buttons: new UIWidgets.DialogActions() {
                        //                    {
                        //                        ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndBtn"),() => {;return true;}
                        //                    }
                        //    }
                        //);
                        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);

                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_REFUSE_ADD_BUDDY, data.szActorName);
                    }
                    break;
                case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY:
                    {
                        reqBuddyInfo(data.dwUserID);
                        reqBuddyRelation(data.dwUserID);
                        reqUserInfo(data.dwUserID);
                    }
                    break;
            }

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到好友在线状态
		public void onRecvBuddyStatus(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddy_status status = helper.get<gamelogic_sns_buddy_status>();

			updateBuddyStatus(status);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_BUDDY_STATUS_UPDATE, null);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到好友列表
		public void onRecvBuddyList(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddy_list_head head = helper.get<gamelogic_sns_buddy_list_head>();
			for (int i = 0; i < head.nCount; ++i)
			{
				gamelogic_sns_buddy buddy = helper.get<gamelogic_sns_buddy>();
				updateBuddy(buddy);
			}
		}

		// 收到好友分组列表
		public void onRecvBuddyGroupList(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddygroup_list_head head = helper.get<gamelogic_sns_buddygroup_list_head>();
			for (int i = 0; i < head.nCount; ++i)
			{
				gamelogic_sns_buddygroup buddyGroup = helper.get<gamelogic_sns_buddygroup>();

				updateBuddyGroup(buddyGroup);
			}

			// 通知UI
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到好友关系列表
		public void onRecvBuddyRelationList(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddyrelation_list_head head = helper.get<gamelogic_sns_buddyrelation_list_head>();
			for (int i = 0; i < head.nCount; ++i)
			{
				gamelogic_sns_buddyrelation buddyRelation = helper.get<gamelogic_sns_buddyrelation>();

				updateBuddyRelation(buddyRelation);

				switch (buddyRelation.nStatus)
				{
					case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_RECV_INVITE:
						{
							showAskAddBuddy(buddyRelation.dwUserID2, buddyRelation.szRemark);
						}
						break;
					case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST:
						{
							BuddyRelation outRelation;
							if (m_dicBuddyRelation.TryGetValue(buddyRelation.dwUserID2, out outRelation))
							{
								outRelation.Info.dwBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST;
							}

							// 请求用户信息
							reqUserInfo(buddyRelation.dwUserID2);
						}
						break;
					case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY:
					case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY:
						{
							// 请求用户信息
							reqUserInfo(buddyRelation.dwUserID2);
						}
						break;
					default:
						break;
				}
			}

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到一个好友关系
		public void onRecvBuddyRelation(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddyrelation buddyRelation = helper.get<gamelogic_sns_buddyrelation>();

			updateBuddyRelation(buddyRelation);

			switch (buddyRelation.nStatus)
			{
				case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_RECV_INVITE:
					{
						showAskAddBuddy(buddyRelation.dwUserID2, buddyRelation.szRemark);
					}
					break;
				//case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_REJECTED_BY_OTHER:
					//{
						//UIDialogData toolTipData = new UIDialogData(
						//		 title: ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndTitle"),
						//		 message: buddyRelation.szRemark + ULocalizationService.Instance.Get("UIView", "SNS", "RefuseAddBuddyDesc"),
						//		 buttons: new UIWidgets.DialogActions() {
						//			{
						//				ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndBtn"),() => {;return true;}
						//			}
						//	}
						//);

						//UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
					//}
					//break;
				case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST:
					{
						BuddyRelation outRelation;
						if (m_dicBuddyRelation.TryGetValue(buddyRelation.dwUserID2, out outRelation))
						{
							outRelation.Info.dwBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST;
						}

						// 请求用户信息
						reqUserInfo(buddyRelation.dwUserID2);
					}
					break;
				case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY:
				case (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY:
					{
						// 请求用户信息
						reqUserInfo(buddyRelation.dwUserID2);
					}
					break;
				default:
					break;
			}

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到一个好友信息
		public void onRecvBuddy(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddy buddy = helper.get<gamelogic_sns_buddy>();

			updateBuddy(buddy);

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
        }

        // 收到好友备注信息
        public void onRecvBuddyRemark(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddy_remark remark = helper.get<gamelogic_sns_buddy_remark>();

			updateBuddyRemark(remark);

		}

		// 收到自己的信息
		public void onRecvBuddySelf(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddy buddy = helper.get<gamelogic_sns_buddy>();

			updateMyBuddyInfo(buddy);

			initSettingMgr();

			reqUserInfo(buddy.nUserID);
		}

		// 收到用户信息
		public void onRecvUserInfo(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_userinfo userInfo = helper.get<gamelogic_sns_userinfo>();

			updateUserInfo(userInfo);

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}

		// 收到好友信息变更
		public void onRecvBuddyInfoChange(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_buddyinfo_change_notify notify = helper.get<gamelogic_sns_buddyinfo_change_notify>();

			switch (notify.nType)
			{
				case (int)EMSNS_BUDDYINFO_UPDATE_TYPE.EMSNS_BUDDYINFO_UPDATE_TYPE_MOOD:
					{
						reqUserInfo(notify.dwUserID);
					}
					break;
				case (int)EMSNS_BUDDYINFO_UPDATE_TYPE.EMSNS_BUDDYINFO_UPDATE_TYPE_BUDDYRELATION:
					{
						reqBuddyRelation(notify.dwUserID);
					}
					break;
			}
		}

		// 收到邀请一起游戏
		public void onRecvInvitePlayGame(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
			gamelogic_sns_invite_playgame_notify notify = helper.get<gamelogic_sns_invite_playgame_notify>();

			// 显示“邀请游戏”滴答
			DIDAAddData msg = new DIDAAddData();
			msg.eLayout = EDIDALayout.EDT_Left;
			msg.nDIDATime = 60;
			msg.nItemID = (int)EDIDAType.EDIDA_InvitePlayGame;
			msg.bAllowRepeatItem = true;
			msg.nButtonIconID = (int)EDIDAType.EDIDA_InvitePlayGame;
			msg.strTooltip = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "InvitePlayGame");

			SNSInvitePlayGameInfo askInfo = new SNSInvitePlayGameInfo();
			askInfo.nRoomID = notify.nRoomID;
			askInfo.inviterName = notify.szInviterName;
            askInfo.dwInviteActorID = notify.dwInviteActorID;
			msg.sCustomData = askInfo;
			msg.callBack = (CustomerInfo) =>
			{
				SNSInvitePlayGameInfo customerInfo = CustomerInfo as SNSInvitePlayGameInfo;
				UIDialogData toolTipData = new UIDialogData(
					title: ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "InvitePlayGame"),
					message: notify.szInviterName + ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "InvitePlayGameDesc"),
					buttons: new UIWidgets.DialogActions() {
							{ULocalizationService.Instance.Get("UIView", "Common", "Agree"),() => {invitePlayGameCallBack(customerInfo, true);return true;}},
							{ULocalizationService.Instance.Get("UIView", "Common", "Refuse"),() => {invitePlayGameCallBack(customerInfo, false);return true;}}
				});

				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
			};

			LogicDataCenter.didaDataManager.AddItem(msg);
		}

        // 收到好友场景改变
        public void onRecvUpdateScene(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            gamelogic_sns_update_scene_notify notify = helper.get<gamelogic_sns_update_scene_notify>();

            BuddyInfo buddy;
            if (m_dicAllBuddy.TryGetValue(notify.nUserID, out buddy))
            {
                buddy.Info.dwWorldID = notify.nWroldID;
                buddy.Info.dwServerID = 0;
                buddy.Info.dwSceneID = notify.nSceneID;

			    // 通知UI层,刷新右键菜单中的场景数据
			    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);

                Debug.Log("Buddy switch scene. nWorldID=" + notify.nWroldID + ",nSceneID=" + notify.nSceneID);
            }
        }
		#endregion 客户端收到消息

		// 初始化好友分组
		private void initBuddyGroup()
		{
			// 添加我的好友分组
			BuddyGroup buddyGroup = new BuddyGroup();
			buddyGroup.Priority = 0;
			buddyGroup.Info.nBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_MY_BUDDY;
			buddyGroup.Info.nRelationStatus = (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_NORMAL;
			buddyGroup.Info.szBuddyGroupName = ULocalizationService.Instance.Get("UIView", "SNS", "MyBuddy");
			m_dicBuddyGroup.Add(buddyGroup.Info.nBuddyGroupID, buddyGroup);

			// 添加附近的人分组
			buddyGroup = new BuddyGroup();
			buddyGroup.Priority = 98;
			buddyGroup.Info.nBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_NEARBY;
			buddyGroup.Info.nRelationStatus = (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_TEMP;
			buddyGroup.Info.szBuddyGroupName = ULocalizationService.Instance.Get("UIView", "SNS", "Nearby");
			m_dicBuddyGroup.Add(buddyGroup.Info.nBuddyGroupID, buddyGroup);

			// 最近一起游戏
			//buddyGroup = new BuddyGroup();
			//buddyGroup.Priority = 99;
			//buddyGroup.Info.nBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_LastPlaying;
			//buddyGroup.Info.nRelationStatus = (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_TEMP;
			//buddyGroup.Info.szBuddyGroupName = ULocalizationService.Instance.Get("UIView", "SNS", "LastPlaying");
			//m_dicBuddyGroup.Add(buddyGroup.Info.nBuddyGroupID, buddyGroup);

			// 添加黑名单分组
			buddyGroup = new BuddyGroup();
			buddyGroup.Priority = 100;
			buddyGroup.Info.nBuddyGroupID = (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST;
			buddyGroup.Info.nRelationStatus = (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_NORMAL;
			buddyGroup.Info.szBuddyGroupName = ULocalizationService.Instance.Get("UIView", "SNS", "BlackList");
			m_dicBuddyGroup.Add(buddyGroup.Info.nBuddyGroupID, buddyGroup);

		}
		// 添加一个好友分组
		public void updateBuddyGroup(gamelogic_sns_buddygroup newInfo)
		{
			BuddyGroup oldBuddyGroup;
			if (m_dicBuddyGroup.TryGetValue(newInfo.nBuddyGroupID, out oldBuddyGroup))
			{
				oldBuddyGroup.Info = newInfo;
			}
			else
			{
				BuddyGroup newBuddyGroup = new BuddyGroup();
				newBuddyGroup.Info = newInfo;
				m_dicBuddyGroup.Add(newInfo.nBuddyGroupID, newBuddyGroup);
			}

		}
		// 获取好友分组列表
		public List<BuddyGroup> getBuddyGroupList()
		{
			List<BuddyGroup> result = new List<BuddyGroup>();
			foreach (var item in m_dicBuddyGroup)
			{
				if (item.Value.Info.nRelationStatus == (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_NORMAL
					|| item.Value.Info.nRelationStatus == (int)EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP.EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP_TEMP)
				{
					result.Add(item.Value);
				}
			}

			return result.OrderBy(x => x.Priority).ToList();
		}
		// 获取在线好友
		public List<BuddyInfo> getOnlineBuddy()
		{
			List<BuddyInfo> buddyList = new List<BuddyInfo>();
			foreach (var item in m_dicAllBuddy)
			{
				BuddyInfo info = item.Value;
				if (info.Info.nStatus == (int)ACTOR_GAME_STATE.GAME_STATE_IDLE
                    && getBuddyRelationStatus(info.Info.nUserID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY
					&& isSameGameWorld(info.Info.nUserID))
				{
					buddyList.Add(info);
				}
			}

			return buddyList;
		}
		// 获取附近的人
		public List<BuddyInfo> getNearbyPlayer()
		{
			List<BuddyInfo> buddyList = new List<BuddyInfo>();
			foreach (var item in m_dicAllBuddy)
			{
				BuddyInfo info = item.Value;
				if (info.Info.nStatus != (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE
                    && getBuddyRelationStatus(info.Info.nUserID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY
					&& isSameGameWorld(info.Info.nUserID))
				{
					buddyList.Add(info);
				}
			}

			return buddyList;
		}


		// 处理好友关系
		private void processBuddyRelation(int dwUserID, EMSNS_BUDDY_RELATIONSHIP relationStatus, string strRemark)
		{
			switch (relationStatus)
			{
				//case EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_RECV_INVITE:
					//{
						//showAskAddBuddy(dwUserID, strRemark);
					//}
					//break;
				//case EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_REJECTED_BY_OTHER:
				//	{
				//		UIDialogData toolTipData = new UIDialogData(
				//			title: ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndTitle"),
				//			message: strRemark + ULocalizationService.Instance.Get("UIView", "SNS", "RefuseAddBuddyDesc"),
				//			buttons: new UIWidgets.DialogActions() {
				//					{
				//						ULocalizationService.Instance.Get("UIView", "SNS", "RefuseWndBtn"),() => {;return true;}
				//					}
				//			}
				//		);

				//		UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
				//	}
				//	break;
				case EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST:
					{

					}
					break;
			
			}
		}
		// 弹出添加好友请求框
		private void showAskAddBuddy(int dwUserID, string strRemark)
		{
			// 收到添加好友请求
			DIDAAddData msg = new DIDAAddData();
			msg.eLayout = EDIDALayout.EDT_Left;
			msg.nDIDATime = 60;
			msg.nItemID = (int)EDIDAType.EDIDA_Friend;
			msg.bAllowRepeatItem = true;
			msg.nButtonIconID = (int)EDIDAType.EDIDA_Friend;
			msg.strTooltip = ULocalizationService.Instance.Get("UIView", "SNS", "AskAddBuddy");
            msg.bShowInWar = true;

			SNSAskAddBuddyInfo askInfo = new SNSAskAddBuddyInfo();
			askInfo.dwUserID = dwUserID;
			askInfo.askBuddyName = strRemark;
			msg.sCustomData = askInfo;
			msg.callBack = (CustomerInfo) =>
			{
				SNSAskAddBuddyInfo customerInfo = CustomerInfo as SNSAskAddBuddyInfo;
				UIDialogData toolTipData = new UIDialogData(
					title: ULocalizationService.Instance.Get("UIView", "SNS", "AskAddBuddy"),
					message: strRemark + ULocalizationService.Instance.Get("UIView", "SNS", "AskAddBuddyDesc"),
					buttons: new UIWidgets.DialogActions() {
							{ULocalizationService.Instance.Get("UIView", "SNS", "AgreeBtnDesc"),() => {addBuddyReqCallBack(customerInfo, true);return true;}},
							{ULocalizationService.Instance.Get("UIView", "SNS", "RefuseBtnDes"),() => {addBuddyReqCallBack(customerInfo, false);return true;}}
				});

				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
			};

			LogicDataCenter.didaDataManager.AddItem(msg);
		}

		// 添加好友请求回调
		private void addBuddyReqCallBack(SNSAskAddBuddyInfo info, bool isAgree)
		{
			gameview_sns_rsp_add_buddy data = new gameview_sns_rsp_add_buddy();
			data.dwOtherAccountID = info.dwUserID;
			data.nFlag = isAgree ? (byte)EMSNS_RSP_ADDBUDDY_TYPE.EMSNS_RSP_ADDBUDDY_TYPEE_AGGRE : (byte)EMSNS_RSP_ADDBUDDY_TYPE.EMSNS_RSP_ADDBUDDY_TYPEE_REFUSE;
			data.szActorName = info.askBuddyName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_RSP_ADD_BUDDY, ref data);
		}
		// 更新自己的信息
		private void updateMyBuddyInfo(gamelogic_sns_buddy buddy)
		{
			m_selfUserID = buddy.nUserID;

			BuddyInfo info = new BuddyInfo();
			info.Info = buddy;
            if(m_dicAllBuddy.ContainsKey(m_selfUserID))
            {
                m_dicAllBuddy[m_selfUserID] = info;
            }
            else
            {
			    m_dicAllBuddy.Add(m_selfUserID, info);
            }
		}
		// 更新好友备注
		private void updateBuddyRemark(gamelogic_sns_buddy_remark remark)
		{
			BuddyRelation relation;
			if (m_dicBuddyRelation.TryGetValue(remark.dwUserID2, out relation))
			{
				relation.Info.szRemark = remark.szRemark;
			}

			// 用于搜索好友
			searchBuddyMgr.updateRemark(remark.dwUserID2, remark.szRemark);

			// 通知UI层
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}
		// 是否显示在好友列表
		private bool isShowBuddyGroup(byte nStatus)
		{
			if (nStatus == (byte)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST
				 || nStatus == (byte)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY
				 || nStatus == (byte)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY)
			{
				return true;
			}

			return false;
		}

		// 初始化设置
		private void initSettingMgr()
		{
			LogicDataCenter.snsDataManager.SettingMgr.isDontShowOffline = (MyBuddyInfo.Info.nPermission & (1 << (int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_DONT_SHOW_OFFLINE_BUDDY)) > 0;
		}

		// 邀请一起游戏回调
		private void invitePlayGameCallBack(SNSInvitePlayGameInfo info, bool isAgree)
		{
            if (!isAgree)
                return;

			cmd_customer_invite_respon respon = new cmd_customer_invite_respon();
			respon.nRoomID = info.nRoomID;
			respon.szPlayerName = info.inviterName;
            respon.dwInviteActorID = info.dwInviteActorID;
			ViewEventHelper.Instance.SendCommand<cmd_customer_invite_respon>(GameLogicDef.GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON, ref respon);
		}

		// 好友当前是否在同一个区
		public bool isSameGameWorld(int nUserID)
		{
			BuddyInfo buddyInfo = getBuddyInfoByUserID(nUserID);

			if (buddyInfo.Info.nUserID <= 0)
			{
				return false;
			}

			return MyBuddyInfo.Info.dwWorldID == buddyInfo.Info.dwWorldID;
		}

		// 更新好友关系，没有则添加
		public void updateBuddyRelation(gamelogic_sns_buddyrelation data)
		{
			BuddyRelation relation;
			if (m_dicBuddyRelation.TryGetValue(data.dwUserID2, out relation))
			{
				relation.Info = data;
			}
			else
			{
				relation = new BuddyRelation();
				relation.Info = data;
				m_dicBuddyRelation.Add(data.dwUserID2, relation);
			}

			// 用于搜索好友
			searchBuddyMgr.updateRemark(data.dwUserID2, data.szRemark);
		}
		// 更新好友关系状态
		public void updateBuddyRelationStatus(int dwUserID, byte relationStatus)
		{
			BuddyRelation oldRelation;
			if (m_dicBuddyRelation.TryGetValue(dwUserID, out oldRelation))
			{
				oldRelation.Info.nStatus = relationStatus;
			}
			else
			{
				Debug.LogWarning("updateBuddyRelationStatus, 没有这个好友 userID=" + dwUserID);
			}
		}
		// 更新好友，没有则添加
		public void updateBuddy(gamelogic_sns_buddy newInfo)
		{

			BuddyInfo buddy;
			if (m_dicAllBuddy.TryGetValue(newInfo.nUserID, out buddy))
			{
				buddy.Info = newInfo;
			}
			else
			{
				buddy = new BuddyInfo();
				buddy.Info = newInfo;
				m_dicAllBuddy.Add(newInfo.nUserID, buddy);
			}

			// 用于搜索好友
			searchBuddyMgr.updateActorName(newInfo.nUserID, newInfo.szName);
		}
		// 更新好友在线状态
		public void updateBuddyStatus(gamelogic_sns_buddy_status buddyStatus)
		{
			BuddyInfo info;
			if (m_dicAllBuddy.TryGetValue(buddyStatus.dwUserID, out info))
			{
				info.Info.nStatus = buddyStatus.nStatus;
				info.Info.dwLastLogout = buddyStatus.dwLastLogout;
				info.Info.szStatusStr = buddyStatus.szStatusStr;
			}
		}
		// 更新用户信息
		public void updateUserInfo(gamelogic_sns_userinfo info)
		{
			BuddyInfo buddy;
			if (m_dicAllBuddy.TryGetValue(info.dwUserID, out buddy))
			{
				buddy.Info.szMood = info.szMood;
				buddy.Info.nPermission = info.nPermission;
			}
			else
			{
				buddy = new BuddyInfo();
				buddy.Info.szMood = info.szMood;
				buddy.Info.nPermission = info.nPermission;
				m_dicAllBuddy.Add(info.dwUserID, buddy);
			}
		}
		// 更新好友的分组
		public void updateBuddyNewGroup(int dwUserID, int nBuddyGroupID)
		{
			BuddyRelation relation;
			if (m_dicBuddyRelation.TryGetValue(dwUserID, out relation))
			{
				relation.Info.dwBuddyGroupID = nBuddyGroupID;
			}
		}
		// 获取某个分组的所有好友
		public List<BuddyInfo> getBuddyListByBuddyGroupID(int nBuddyGroupID)
		{
			var accountList = m_dicBuddyRelation.Where(item => item.Value.Info.dwBuddyGroupID == nBuddyGroupID).Select(item => item.Key).ToList();
			List<BuddyInfo> result = new List<BuddyInfo>();
			foreach (var nUserID in accountList)
			{
				if (nUserID <= 0)
				{
					continue;
				}

				BuddyRelation relation;
				if (!m_dicBuddyRelation.TryGetValue(nUserID, out relation))
				{
					continue;
				}
				if (!isShowBuddyGroup(relation.Info.nStatus))
				{
					continue;
				}

				BuddyInfo buddy;
				if (!m_dicAllBuddy.TryGetValue(nUserID, out buddy))
				{
					continue;
				}

				// 不显示离线好友
				if (SettingMgr.isDontShowOffline && buddy.Info.nStatus == (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
				{
					continue;
				}

				result.Add(buddy);
			}

			// 排序,在线的玩家排前面
			result.Sort();

			return result;
		}
		// 获取备注名
		public string getBuddyRemark(int nUserID)
		{
			BuddyRelation relation = null;
			if (m_dicBuddyRelation.TryGetValue(nUserID, out relation))
			{
				return relation.Info.szRemark;
			}
			return "";
		}
		// 获取亲密度
		public int getBuddyIntamicy(int nUserID)
		{
			BuddyRelation relation = null;
			if (m_dicBuddyRelation.TryGetValue(nUserID, out relation))
			{
				return relation.Info.nIntimacy;
			}
			return 0;
		}
		// 获取好友分组信息
		public BuddyGroup getBuddyGroup(int nBuddyGroupID)
		{
			BuddyGroup buddyGroup = null;
			if (m_dicBuddyGroup.TryGetValue(nBuddyGroupID, out buddyGroup))
			{
				return buddyGroup;
			}
			return new BuddyGroup();
		}
		// 根据UserID获取好友信息
		public BuddyInfo getBuddyInfoByUserID(int nUserID)
		{
			BuddyInfo result;
			if (m_dicAllBuddy.TryGetValue(nUserID, out result))
			{
				return result;
			}

			return new BuddyInfo();
		}
		// 获取好友状态描述
		public string getBuddyStatusDesc(BuddyInfo buddy)
		{
			// 其他区且在线,则显示大区名字
			if (!isSameGameWorld(buddy.Info.nUserID) && buddy.Info.nStatus != (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
			{
				return String.Format("<color=#FFFFFF>{0}</color>", buddy.Info.szWorldName);
			}

            string strColor = UDefines.GetGameStateColor(buddy.Info.nStatus);
            string strState = UDefines.GetGameStateDesc(buddy.Info.nStatus);
            string result = String.Format("<color=#{0}>{1}</color>", strColor, ULocalizationService.Instance.Get("UIView", "Common", strState));
            switch (buddy.Info.nStatus)
			{
				case (byte)ACTOR_GAME_STATE.GAME_STATE_OFFLINE:
					{
						DateTime lastLogoutTime = Api.convertTimeStamp2DateTime(buddy.Info.dwLastLogout);
						TimeSpan span = DateTime.Now - lastLogoutTime;
						string desc = ULocalizationService.Instance.Get("UIView", "Common", "ActorGameState_OffLine");
						if (span.Days > 0)
						{
							desc += span.Days + ULocalizationService.Instance.Get("UIView", "Common", "Days");
                        }
						else if (span.Hours > 0)
						{
							desc += span.Hours + ULocalizationService.Instance.Get("UIView", "Common", "Hour");
                        }

                        int nMin = span.Minutes < 0 ? 0 : span.Minutes;
                        desc += nMin + ULocalizationService.Instance.Get("UIView", "Common", "Minute");
                        result = String.Format("<color=#{0}>{1}</color>", strColor, desc);
					}
					break;
                default:break;
			}

			return result;
		}
		// 获取自己的状态描述
		public string getSelfStatusDesc()
		{
			return getBuddyStatusDesc(MyBuddyInfo);
		}
		// 根据好友ID获取session名字
		//public string getSessionName(int nSessionID)
		//{
		//	return SessionManager.getSessionName(MyBuddyInfo.Info.nUserID, nSessionID);
		//}
		// 是否是系统默认的分组
		static public bool isDefaultBuddyGroup(int nBuddyGroupID)
		{
			if (nBuddyGroupID >= (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_MY_BUDDY && nBuddyGroupID <= (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_RETAIN5)
			{
				return true;
			}
			return false;
		}
		// 获取与好友的关系
		public int getBuddyRelationStatus(int dwUserID)
		{
            if(dwUserID > 0)
            {
                BuddyRelation relation;
                if (m_dicBuddyRelation.TryGetValue(dwUserID, out relation))
                {
                    return relation.Info.nStatus;
                }
            }

			return (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NONE;
		}
		// 获取好友在哪个分组
		public int getBuddyGroupID(int dwUserID)
		{
			BuddyRelation relation;
			if (m_dicBuddyRelation.TryGetValue(dwUserID, out relation))
			{
				return relation.Info.dwBuddyGroupID;
			}

			return (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_MY_BUDDY;
		}

		// 获取某个分组中在线的好友个数
		public int getOnlineBuddyNum(int nBuddyGroupID)
		{
			int count = 0;
			foreach (var buddy in LogicDataCenter.snsDataManager.getBuddyListByBuddyGroupID(nBuddyGroupID))
			{
				if (buddy.Info.nStatus != (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
				{
					count++;
				}
			}

			return count;
		}
		// 获取某个分组中的好友个数
		public int getBuddyNum(int nBuddyGroupID)
		{
			return LogicDataCenter.snsDataManager.getBuddyListByBuddyGroupID(nBuddyGroupID).Count;
		}
		// 设置某个分组是否展开
		public void setBuddyGroupExpand(int buddyGroupID, bool isExpanded)
		{
			m_dicBuddyGroup[buddyGroupID].isExpanded = isExpanded;
		}
		// 判断是否好友
		public bool isBuddyRelation(int nOtherUserID)
		{
			return getBuddyRelationStatus(nOtherUserID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY;
		}
		// 判断是否是黑名单
		public bool isBlackListRelation(int nOtherUserID)
		{
			return getBuddyRelationStatus(nOtherUserID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST;
		}
		// 成为好友后给出提示
		private void onBecomeBuddy(int nOtherUserID, string otherName)
		{
			// 请求信息
			reqBuddyInfo(nOtherUserID);
			reqBuddyRelation(nOtherUserID);
			reqUserInfo(nOtherUserID);
		}
		// 是否可以搜索
		private bool IsCanSearch(int userID)
		{
			return getBuddyRelationStatus(userID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BLACKLIST
				|| getBuddyRelationStatus(userID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_BUDDY
				|| getBuddyRelationStatus(userID) == (int)EMSNS_BUDDY_RELATIONSHIP.EMSNS_BUDDY_RELATIONSHIP_NEARBY;
		}
		// 根据过滤字符串搜索符合条件的好友信息
		public List<int> searchBuddy(string filterStr)
		{
			List<int> result = new List<int>();
			foreach (var buddy in m_dicAllBuddy)
			{
				if (!IsCanSearch(buddy.Key))
					continue;

				if (searchBuddyMgr.IsMatch(buddy.Key, filterStr))
				{
					result.Add(buddy.Key);
				}
			}

			return result;
		}
		// 判断过滤字符串是否匹配该好友
		public bool isMatchBuddy(int nUserID, string filterStr)
		{
			return searchBuddyMgr.IsMatch(nUserID, filterStr);
		}

		#region 客户端发出请求
		// 登陆SNS服务器
		public void loginSNSServer(ActorPlayerInfo playerInfo)
		{
			gameview_sns_login data = new gameview_sns_login();
			data.nPdbid = playerInfo.playerInfo.pDbid;
			data.nLevel = playerInfo.playerInfo.nLv;
			data.nHeadID = playerInfo.playerInfo.usHeadID;
			data.nMatchNum = playerInfo.playerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MATCHNUM];
			data.nWinNum = playerInfo.playerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_WINNUM];
			data.nRankIconID = playerInfo.playerInfo.nRankIconID;
			data.szClan = playerInfo.playerInfo.chClanName;
			data.szName = playerInfo.playerInfo.chPlayerName;
			data.szRankName = playerInfo.playerInfo.chRankName;
			data.szTitle = playerInfo.playerInfo.chMostGlory;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_LOGIN, ref data);
		}
		// 请求修改好友名字
		public void reqChangeBuddyRemark(int dwAcountID, string actorName, string remark)
		{
			gameview_sns_update_remark data = new gameview_sns_update_remark();
			data.dwUserID = dwAcountID;
			data.szRemark = remark;
			data.szActorName = actorName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_REMARK, ref data);
		}
		// 请求删除好友
		public void reqDelBuddy(int dwUserID, string buddyName)
		{
			gameview_sns_update_buddy data = new gameview_sns_update_buddy();
			data.dwAddAccountID = dwUserID;
			data.nOptType = (int)EMSNS_BUDDY_UPDATE_TYPE.EMSNS_BUDDY_UPDATE_TYPE_DEL;
			data.szActorName = buddyName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDY, ref data);
		}
		// 请求添加好友
		public void reqAddBuddy(int dwUserID, string buddyName)
		{
            // 判断是否已经是好友关系
            if(isBuddyRelation(dwUserID))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_ALREADY_BUDDY_RELATION);
                return;
            }

			gameview_sns_update_buddy data = new gameview_sns_update_buddy();
			data.dwAddAccountID = dwUserID;
			data.nOptType = (int)EMSNS_BUDDY_UPDATE_TYPE.EMSNS_BUDDY_UPDATE_TYPE_ADD;
			data.szActorName = buddyName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDY, ref data);
		}
		// 请求加入黑名单
		public void reqAddBlackList(int dwUserID, string buddyName)
		{
			gameview_sns_update_buddy data = new gameview_sns_update_buddy();
			data.dwAddAccountID = dwUserID;
			data.nOptType = (int)EMSNS_BUDDY_UPDATE_TYPE.EMSNS_BUDDY_UPDATE_TYPE_ADD_BLACKLIST;
			data.szActorName = buddyName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDY, ref data);
		}
        // 请求加入好友战队
        public void reqJoinBuddyKin(int pdbid)
        {
            gameview_sns_data_to_social data = new gameview_sns_data_to_social();
            data.nMsgType = (int)ESNSView2SocialMsg.SNSView2Social_JoinBuddyKin;
            data.nReverse1 = pdbid;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_SEND_DATA_TO_SOCIAL, ref data);
        }
        // 请求删除好友分组
        public void reqDelBuddyGroup(int dwBuddyGroupID)
		{
			gameview_sns_update_buddygroup data = new gameview_sns_update_buddygroup();
			data.dwBuddyGroupID = dwBuddyGroupID;
			data.nOptType = (int)EMSNS_BUDDYGROUP_UPDATE_TYPE.EMSNS_BUDDYGROUP_UPDATE_TYPE_DEL;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDYGROUP, ref data);
		}
		// 请求新增好友分组
		public void reqAddBuddyGroup(string name)
		{
			gameview_sns_update_buddygroup data = new gameview_sns_update_buddygroup();
			data.nOptType = (int)EMSNS_BUDDYGROUP_UPDATE_TYPE.EMSNS_BUDDYGROUP_UPDATE_TYPE_ADD;
			data.szName = name;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDYGROUP, ref data);
		}
		// 请求修改好友分组名字
		public void reqChangeBuddyGroupName(int dwBuddyGroupID, string name)
		{
			gameview_sns_update_buddygroup data = new gameview_sns_update_buddygroup();
			data.dwBuddyGroupID = dwBuddyGroupID;
			data.nOptType = (int)EMSNS_BUDDYGROUP_UPDATE_TYPE.EMSNS_BUDDYGROUP_UPDATE_TYPE_UPDATE_NAME;
			data.szName = name;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_UPDATE_BUDDYGROUP, ref data);
		}
		// 发送聊听消息
		public void reqSendChatData(long SessionID, string content, int fontColor)
		{
			gameview_sns_send_msg_data data = new gameview_sns_send_msg_data();
			data.dwFromAccountID = MyBuddyInfo.Info.nUserID;
			data.dwToAccountID = (int)SessionID;
			data.szFromName = MyBuddyInfo.Info.szName;
			data.szContent = content;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_SEND_MSG_DATA, ref data);
		}
		// 请求好友信息
		public void reqBuddyInfo(int dwUserID)
		{
			// 请求新好友的信息
			gameview_sns_req_buddy_info cmd = new gameview_sns_req_buddy_info();
			cmd.dwUserID = dwUserID;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_BUDDY_INFO, ref cmd);
		}
		// 请求好友关系信息
		public void reqBuddyRelation(int dwUserID)
		{
			// 请求和新好友的关系数据
			gameview_sns_req_buddyrelation cmd2 = new gameview_sns_req_buddyrelation();
			cmd2.dwUserID = dwUserID;
			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_BUDDY_RELATION, ref cmd2);
		}
		// 请求用户信息
		public void reqUserInfo(int dwUserID)
		{
			gameview_sns_req_userinfo cmd = new gameview_sns_req_userinfo();
			cmd.dwUserID = dwUserID;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_USER_INFO, ref cmd);
		}
		// 更新权限
		public void reqChangePermission()
		{
			gameview_sns_req_change_permission cmd = new gameview_sns_req_change_permission();
			cmd.nPermission = MyBuddyInfo.Info.nPermission;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_CHANGE_PERMISSION, ref cmd);
		}
		// 更新心情
		public void reqChangeMood(string newMood)
		{
			gameview_sns_req_change_mood cmd = new gameview_sns_req_change_mood();
			cmd.szMood = newMood;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_CHANGE_MOOD, ref cmd);
		}
		// 移动好友分组
		public void reqChangeBuddyGroup(int dwUserID, int dwBuddyGroupID)
		{
			gameview_sns_req_change_buddygroup cmd = new gameview_sns_req_change_buddygroup();
			cmd.dwUserID = dwUserID;
			cmd.dwBuddyGroupID = dwBuddyGroupID;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_CHANGE_BUDDYGROUP, ref cmd);

			// 先客户端发生变化
			updateBuddyNewGroup(dwUserID, dwBuddyGroupID);
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
		}
		// 更新在线状态
		public void reqUpdateBuddyStatus(byte nStatus, string strReverse)
		{
            gameview_sns_req_update_buddystatus cmd = new gameview_sns_req_update_buddystatus();
            cmd.nStatus = nStatus;
            cmd.szReverse = strReverse;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_UPDATE_BUDDYSTATUS, ref cmd);

            // 更新自己本地的状态
            MyBuddyInfo.Info.nStatus = nStatus;
			MyBuddyInfo.Info.szStatusStr = strReverse;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_SELF_STATUS);
		}
		// 请求一起游戏
		public void reqInvitePlayGame(int nRoomID, int nUserID)
		{
			gameview_sns_req_invite_play_game cmd = new gameview_sns_req_invite_play_game();
			cmd.nUserID = nUserID;
			cmd.nRoomID = nRoomID;
			cmd.szInviterName = MyBuddyInfo.Info.szName;

			ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_INVITE_PLAYGAME, ref cmd);
		}

        // 请求一起组队
        public void reqInviteMatchTeam(int nUserID)
        {
            gameview_sns_req_invite_match_team cmd = new gameview_sns_req_invite_match_team();
            cmd.nUserID = nUserID;
            cmd.szInviterName = MyBuddyInfo.Info.szName;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_INVITE_MATCH_TEAM, ref cmd);
        }

		#endregion 客户端发出请求

		// SNS 聊听会话
		private class SNSSession
		{
			// 聊听消息
			private List<SSNSChatMessageInfo> m_AllMsgList = new List<SSNSChatMessageInfo>();
			// 未读消息
			private List<SSNSChatMessageInfo> m_UnReadMsgList = new List<SSNSChatMessageInfo>();
			// 聊听历史消息记录
			private List<HistroyMsg> m_HistroyMsgList = new List<HistroyMsg>();
			// 聊听会话名称，标识着和不同好友的会话
			private long m_SessionID;

			public SNSSession(long sid)
			{
                m_SessionID = sid;
			}

			// 添加一条聊听消息
			public void addMsgData(gamelogic_sns_msg_data data)
			{
				// 解析富文本
				List<SChatMessageObjectInfo> objList = LogicDataCenter.chatMessageManager.parseChatContent(data.szContent, Color.white);

				SSNSChatMessageInfo info = new SSNSChatMessageInfo();
				info.msg = data;
				info.objList = objList;

				m_AllMsgList.Add(info);
				m_UnReadMsgList.Add(info);
			}

			// 存到本地数据库
			public void addMsgToDB(long sid, gamelogic_sns_msg_data data)
			{
				HistroyMsg histroyMsg = new HistroyMsg();
				histroyMsg.nFromAcountID = (int)data.dwFromAccountID;
				histroyMsg.nMsgID = (int)data.dwMsgID;
				histroyMsg.nTime = (int)data.dwTime;
				histroyMsg.nToAcountID = (int)data.dwToAccountID;
				histroyMsg.szContent = data.szContent ?? "";
				histroyMsg.szFromName = data.szFromName ?? "";

				HistroyMsgDBHelper.WriteToFile(sid, histroyMsg);
			}

			// 获取所有聊听消息
			public List<SSNSChatMessageInfo> getAllMsg()
			{
				return m_AllMsgList;
			}

			// 获取未读消息
			public List<SSNSChatMessageInfo> getUnReadMsg()
			{
				List<SSNSChatMessageInfo> tmpList = new List<SSNSChatMessageInfo>(m_UnReadMsgList);

				m_UnReadMsgList.Clear();

				return tmpList;
			}

			// 获取某一页的聊听记录
			public List<HistroyMsg> getHistoryMsg(int nPage, int nPerPageNum)
			{
				List<HistroyMsg> result = new List<HistroyMsg>();

				// nPage最小为1
				if (nPage <= 0)
				{
					nPage = 1;
				}

				// 起算该页的起点
				int start = (nPage - 1) * nPerPageNum;

				// 计算该页的个数，考虑最后一页可能没有nPerPageNum条
				int remainCnt = m_HistroyMsgList.Count - start;
				int count = (remainCnt > nPerPageNum) ? nPerPageNum : remainCnt;

				result = m_HistroyMsgList.GetRange(start, count);

				return result;
			}

			// 从本地缓存中读取所有历史记录
			public int loadAllHistroyMsg()
			{
				// 查询所有历史记录
				m_HistroyMsgList = HistroyMsgDBHelper.ReadFromFile(m_SessionID);

				return m_HistroyMsgList.Count;
			}
		}

		// SNS 聊听会话管理器
		public class SNSSessionManager
		{
			// 所有的聊听会话
			private Dictionary<long, SNSSession> m_dicSession = new Dictionary<long, SNSSession>();

			public void Clear()
			{
				m_dicSession.Clear();
			}

			// 添加一条聊听消息
			public void addMsgData(gamelogic_sns_msg_data data)
			{
				// 判断是否自己发的消息
				long sid = data.dwFromAccountID;
				if (data.dwFromAccountID == LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nUserID)
				{
					sid = data.dwToAccountID;
				}
				//string sessionName = getSessionName(data.dwFromAccountID, data.dwToAccountID);

				SNSSession session = getSession(sid);
				session.addMsgData(data);

                // 存到客户端记录中去
                session.addMsgToDB(sid, data);

                // 通知UI
                SNSAddChatMsgData msg = new SNSAddChatMsgData();
				msg.SessionID = sid;
				UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_ADD_CHAT_MESSAGE, msg);
			}

			// 获取所有聊听消息
			public List<SSNSChatMessageInfo> getAllMsg(long sid)
			{
				return getSession(sid).getAllMsg();
			}

			// 获取未读聊听消息
			public List<SSNSChatMessageInfo> getUnReadMsg(long sid)
			{
				return getSession(sid).getUnReadMsg();
			}

			public List<HistroyMsg> getHistoryMsg(long sid, int nPage, int nPerPageNum)
			{
				return getSession(sid).getHistoryMsg(nPage, nPerPageNum);
			}

			public int loadAllHistroyMsg(long sid)
			{
				return getSession(sid).loadAllHistroyMsg();
			}

			// 获取一个好友会话
			private SNSSession getSession(long sid)
			{
				SNSSession session;
				if (!m_dicSession.TryGetValue(sid, out session))
				{
					session = new SNSSession(sid);
					m_dicSession.Add(sid, session);
				}

				return session;
			}

			//public string getSessionName(int userID1, int userID2)
			//{
			//	int smallID = userID1;
			//	int bigID = userID2;

			//	if (smallID > bigID)
			//	{
			//		smallID = userID2;
			//		bigID = userID1;
			//	}

			//	return "Buddy" + smallID + bigID;
			//}

   //         public string getSessionName(int nUserID)
   //         {
   //             return "Buddy" + nUserID;
   //         }

			// 删除一个会话
			public bool delSession(long sid)
			{
				SNSSession session;
				if (!m_dicSession.TryGetValue(sid, out session))
				{
					m_dicSession.Remove(sid);
					return true;
				}

				return false;
			}
		}

		// 聊听历史存到客户端硬盘中
		private class HistroyMsgDBHelper
		{
            static private readonly string ChatLogFolderName = "ChatLog";
            static private string ChatLogFolderFullPath
            {
                get
                {
                    string path = ViewConfigManager.GetAccountConfigFullPath(ChatLogFolderName);
                    //没有目录的话创建一个
                    if (!System.IO.Directory.Exists(path))
                    {
                        System.IO.Directory.CreateDirectory(path);
                    }
                    return path+"/";
                }
            }

            // 直接存到二进制文件中
            public static void WriteToFile(long sid, HistroyMsg msg)
			{
				using (BinaryWriter writer = new BinaryWriter(File.Open(String.Format("{0}{1}{2}", ChatLogFolderFullPath , sid , ".bin"), FileMode.OpenOrCreate)))
				{
					writer.Seek(0, SeekOrigin.End);
					writer.Write(msg.nMsgID);
					writer.Write(msg.nFromAcountID);
					writer.Write(msg.nToAcountID);
					writer.Write(msg.szFromName);
					writer.Write(msg.szContent);
					writer.Write(msg.nTime);
				}
			}

			// 直接从二进制文件中读取出来
			public static List<HistroyMsg> ReadFromFile(long sid)
			{
				List<HistroyMsg> result = new List<HistroyMsg>();

				using (BinaryReader br = new BinaryReader(File.Open(String.Format("{0}{1}{2}", ChatLogFolderFullPath, sid, ".bin"), FileMode.OpenOrCreate)))
				{
					while (br.BaseStream.Position < br.BaseStream.Length)
					{
						HistroyMsg msg = new HistroyMsg();
						msg.nMsgID = br.ReadInt32();
						msg.nFromAcountID = br.ReadInt32();
						msg.nToAcountID = br.ReadInt32();
						msg.szFromName = br.ReadString();
						msg.szContent = br.ReadString();
						msg.nTime = br.ReadInt32();

						result.Add(msg);
					}
				}

				return result;
			}
		}

		// 搜索好友
		private class SNSSearchBuddyMgr
		{
			// 好友名字相关缓存,用于搜索好友
			private class BuddyNameCache
			{
				public string ActorName = "";
				public string ActorNamePinYin = "";
				public string ActorNameFirstPinYin = "";
				public string Remark = "";
				public string RemarkPinYin = "";
				public string RemarkFirstPinYin = "";

				public bool Contains(string str)
				{
					if (ActorName.IndexOf(str) >= 0)
						return true;
					if (ActorNamePinYin.IndexOf(str) >= 0)
						return true;
					if (ActorNameFirstPinYin.IndexOf(str) >= 0)
						return true;
					if (Remark.IndexOf(str) >= 0)
						return true;
					if (RemarkPinYin.IndexOf(str) >= 0)
						return true;
					if (RemarkFirstPinYin.IndexOf(str) >= 0)
						return true;

					return false;
				}
			}

			private Dictionary<int, BuddyNameCache> m_AllBuddyNameCache = new Dictionary<int, BuddyNameCache>();

			public void updateActorName(int userID, string name)
			{
				BuddyNameCache nameCache;
				if (!m_AllBuddyNameCache.TryGetValue(userID, out nameCache))
				{
					nameCache = new BuddyNameCache();
					m_AllBuddyNameCache.Add(userID, nameCache);
				}

				nameCache.ActorName = name;
				nameCache.ActorNamePinYin = PinYinConverter.Get(name).ToLower();
				nameCache.ActorNameFirstPinYin = PinYinConverter.GetFirst(name).ToLower();
			}

			public void updateRemark(int userID, string remark)
			{
				BuddyNameCache nameCache;
				if (!m_AllBuddyNameCache.TryGetValue(userID, out nameCache))
				{
					nameCache = new BuddyNameCache();
					m_AllBuddyNameCache.Add(userID, nameCache);
				}

				nameCache.Remark = remark;
				nameCache.RemarkPinYin = PinYinConverter.Get(remark).ToLower();
				nameCache.RemarkFirstPinYin = PinYinConverter.GetFirst(remark).ToLower();
			}

			public bool IsMatch(int userID, string filterStr)
			{
				BuddyNameCache nameCache;
				if (m_AllBuddyNameCache.TryGetValue(userID, out nameCache))
				{
					if (nameCache.Contains(filterStr))
					{
						return true;
					}
				}

				return false;
			}
		}
	}
}
