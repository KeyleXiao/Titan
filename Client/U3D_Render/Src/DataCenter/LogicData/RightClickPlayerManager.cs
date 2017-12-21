using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI.UWidgets;
using GameLogic;
using U3D_Render;

namespace DataCenter
{
    // 右键玩家弹出菜单
    public class RightClickPlayerManager
    {
        // 查看玩家
        private PopupMenuDataManager.UPopupItemRegisterInfo m_viewPlayerRegistInfo;
        // 邀请组队
        private PopupMenuDataManager.UPopupItemRegisterInfo m_teamInviteRegistInfo;
        // 请求组队
        private PopupMenuDataManager.UPopupItemRegisterInfo m_teamRequestRegistInfo;
        // 邀请加入战队
        private PopupMenuDataManager.UPopupItemRegisterInfo m_kinInviteRegistInfo;
        // 邀请加入联盟
        private PopupMenuDataManager.UPopupItemRegisterInfo m_clanInviteRegistInfo;
        // 添加/删除好友
        private PopupMenuDataManager.UPopupItemRegisterInfo m_AddOrDelBuddyInfo;
		// 加入黑名单
		private PopupMenuDataManager.UPopupItemRegisterInfo m_AddBlackListInfo;
        // 好友聊听
		private PopupMenuDataManager.UPopupItemRegisterInfo m_BuddyChatInfo;


		// 私聊
		private PopupMenuDataManager.UPopupItemRegisterInfo m_PrivateChatInfo;

        //PK
        private PopupMenuDataManager.UPopupItemRegisterInfo m_PKInfo;

		public void Init()
        {
            // 查看玩家信息
            m_viewPlayerRegistInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_ViewPlayer, ProcessViewPlayer);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_viewPlayerRegistInfo);

            // 邀请组队
            m_teamInviteRegistInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_TeamInvite, ProcessTeamInivite);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_teamInviteRegistInfo);

            // 请求组队
            m_teamRequestRegistInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_TeamRequest, ProcessTeamRequest);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_teamRequestRegistInfo);

            // 战队邀请
            m_kinInviteRegistInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_KinInvite, ProcessKinInivite);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_kinInviteRegistInfo);

            // 联盟邀请
            m_clanInviteRegistInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_ClanInvite, ProcessClanInivite);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_clanInviteRegistInfo);

            // 添加/删除好友
            m_AddOrDelBuddyInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_AddOrDelBuddy, processAddOrDelBuddy);
			PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_AddOrDelBuddyInfo);

            // 加入黑名单
			m_AddBlackListInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_AddBlackList, processAddBlackList);
			PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_AddBlackListInfo);

            // 好友聊听
            m_BuddyChatInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_BuddyChat, processBuddyChat);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_BuddyChatInfo);

            // 私聊
            m_PrivateChatInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_PrivateChat, processPrivateChat);
			PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_PrivateChatInfo);

            // PK
            m_PKInfo = new PopupMenuDataManager.UPopupItemRegisterInfo(PopupMenuDataManager.EPopupItemInfoKey.EInfo_PK, processPK);
            PopupMenuDataManager.Instance.RegisterPopupupItem(ref m_PKInfo);
		}

        // 查看玩家
        private void ProcessViewPlayer(int nEntityID)
        {
            uint nPDBID = GameLogicAPI.getPlayerPDBID(nEntityID);
            if (nPDBID == 0)
                return;

            UListComponentItemDataPopup item = new UPopupItemDataGetPlayerDetail((int)nPDBID, _strItemTypeName : UPopupItemDataGetPlayerDetail.GetDescString());
            PopupMenuDataManager.Instance.SetPopupItemData(m_viewPlayerRegistInfo, ref item);
        }

        // 邀请组队
        private void ProcessTeamInivite(int nEntityID)
        {
            uint nPDBID = GameLogicAPI.getPlayerPDBID(nEntityID);
            if (nPDBID == 0)
                return;

            EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
            if (ev == null)
                return;

            // 对方已有队伍不能邀请
            int nTeamID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID);
            if (nTeamID > 0)
                return;
                
            UListComponentItemDataPopup item = new UPopupItemDataTeamInvite((int)nPDBID, _strItemTypeName: UPopupItemDataTeamInvite.GetDescString());
            PopupMenuDataManager.Instance.SetPopupItemData(m_teamInviteRegistInfo, ref item);
        }

        // 请求组队
        private void ProcessTeamRequest(int nEntityID)
        {
            uint nPDBID = GameLogicAPI.getPlayerPDBID(nEntityID);
            if (nPDBID == 0)
                return;

            EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
            if (ev == null)
                return;

            int nTargetTeamID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID);
            int nSelfTeamID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID);

            // 自身无队伍，目标有队伍才能请求
            if (nTargetTeamID > 0 && nSelfTeamID <= 0)
            {
                UListComponentItemDataPopup item = new UPopupItemDataTeamRequest((int)nPDBID, _strItemTypeName: UPopupItemDataTeamRequest.GetDescString());
                PopupMenuDataManager.Instance.SetPopupItemData(m_teamRequestRegistInfo, ref item);
            }
        }

        // 邀请加入战队
        private void ProcessKinInivite(int nEntityID)
        {
            uint nPDBID = GameLogicAPI.getPlayerPDBID(nEntityID);
            if (nPDBID == 0)
                return;

            if (!LogicDataCenter.kinDataManager.IsKinBaseDataValid())
                return;

            if (LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                return;
                
            UListComponentItemDataPopup item = new UPopupItemDataKinInvite((int)nPDBID, _strItemTypeName: UPopupItemDataKinInvite.GetDescString());
            PopupMenuDataManager.Instance.SetPopupItemData(m_kinInviteRegistInfo, ref item);
        }

        // 邀请加入联盟
        private void ProcessClanInivite(int nEntityID)
        {
            uint nPDBID = GameLogicAPI.getPlayerPDBID(nEntityID);
            if (nPDBID == 0)
                return;

            if (!LogicDataCenter.clanDataManager.IsClanBaseDataValid())
                return;

            if (LogicDataCenter.clanDataManager.ClanBaseData.nShaikhID != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                return;

            UListComponentItemDataPopup item = new UPopupItemDataClanInvite(nEntityID, (int)nPDBID, _strItemTypeName: UPopupItemDataClanInvite.GetDescString());
            PopupMenuDataManager.Instance.SetPopupItemData(m_clanInviteRegistInfo, ref item);
        }

        #region SNS
        // 处理加/删好友菜单项
        private void processAddOrDelBuddy(int nEntityID)
		{
			uint nUserID = GameLogicAPI.getPlayerUserID(nEntityID);
			if (nUserID == 0)
				return;

			EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
			if (ev == null)
				return;

			string szName = ev.Property.CreatureName;
			if (string.IsNullOrEmpty(szName))
				return;

			// 构造弹出菜单列表
			UListComponentItemDataPopup item;
			if (LogicDataCenter.snsDataManager.isBuddyRelation((int)nUserID))
			{
				// 删除好友
				item = new UPopupItemDataDelBuddy((int)nUserID, szName, _strItemTypeName:
				UPopupItemDataDelBuddy.GetDescString());
			}
			else
			{
				// 添加好友
				item = new UPopupItemDataAddFriend((int)nUserID, szName, _strItemTypeName:
				UPopupItemDataAddFriend.GetDescString());
			}

			// 设置弹出菜单列表
			PopupMenuDataManager.Instance.SetPopupItemData(m_AddOrDelBuddyInfo, ref item);
		}

		// 处理加入黑名单菜单项
		private void processAddBlackList(int nEntityID)
		{
			uint nUserID = GameLogicAPI.getPlayerUserID(nEntityID);
			if (nUserID == 0)
				return;

			EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
			if (ev == null)
				return;

			string szName = ev.Property.CreatureName;
			if (string.IsNullOrEmpty(szName))
				return;

			// 加入黑名单
			if (!LogicDataCenter.snsDataManager.isBlackListRelation((int)nUserID))
			{
				UListComponentItemDataPopup item = new UPopupItemDataAddBlackList((int)nUserID, szName, _strItemTypeName:
				UPopupItemDataAddBlackList.GetDescString());

				PopupMenuDataManager.Instance.SetPopupItemData(m_AddBlackListInfo, ref item);
			}
		}

        // 好友聊听
        private void processBuddyChat(int nEntityID)
        {
            uint nUserID = GameLogicAPI.getPlayerUserID(nEntityID);
            if (nUserID == 0)
                return;

            // 好友聊听
            UListComponentItemDataPopup item = new UPopupItemDataBuddyChat((int)nUserID, _strItemTypeName:
                UPopupItemDataBuddyChat.GetDescString());

            PopupMenuDataManager.Instance.SetPopupItemData(m_BuddyChatInfo, ref item);
        }
        #endregion

        #region Chat
        // 处理私聊菜单项
        private void processPrivateChat(int nEntityID)
		{
			U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
			if (ev == null)
				return;

			string szName = ev.Property.CreatureName;
			if (string.IsNullOrEmpty(szName))
				return;

			// 构造弹出菜单列表
			UListComponentItemDataPopup item = new UPopupItemDataPrivateChat(szName, _strItemTypeName:
				UPopupItemDataPrivateChat.GetDescString());

			PopupMenuDataManager.Instance.SetPopupItemData(m_PrivateChatInfo, ref item);
		}
        #endregion

        
        #region PK
        private void processPK(int nEntityID)
        {
            U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(nEntityID);
            if (ev == null)
                return;

            if (GameLogicAPI.isForbitPK() > 0)
                return;

            // 构造弹出菜单列表
            UListComponentItemDataPopup item = new UPopupItemDataPK(nEntityID, _strItemTypeName:
                UPopupItemDataPK.GetDescString());

            PopupMenuDataManager.Instance.SetPopupItemData(m_PKInfo, ref item);
        }
        #endregion
    }
}
