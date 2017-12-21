/*******************************************************************
** 文件名:	UTeammateInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-7-6
** 版  本:	1.0
** 描  述:	队伍信息界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using USpeedUI.UWidgets;
using GameLogic;
using DataCenter;
using USpeedUI.UExtensions;
using USpeedUI.PlayerFrame;
using USpeedUI.PointShop;
using USpeedUI.ChatBubble;
using USpeedUI.Chat;

namespace USpeedUI.TeammateInfo
{
    public class UTeammateInfoView : MonoBehaviour
    {
        public Image HeadIcon;
        public Image RankIcon;
        public Text Lv;
        public Text Name;
        public Image IsCaptain;
        public UPopupMenuHandle PopupMenuHandle;
        public  UPopupAction PopupAction;
        public int nPDBID;
        public int nUserID;
		
		// 聊天气泡
		public ChatBubbleItemBase ChatBubble;

        public virtual void SetData(cmd_entity_teammate_info data)
        {
            nPDBID = data.nPDBID;
            nUserID = data.nUserID;

            Lv.text = string.Format("lv.{0}", data.nLevel);
            Name.text = data.szName;
            HeadIcon.sprite = GetActorIcon(data.nHeadID);

            int nRankIconID = GameLogicAPI.getRankIconIDByGrade((int)data.byMatchType, data.nRankGrade);
            if (nRankIconID > 0)
            {
                RankIcon.gameObject.SetActive(true);
                Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_TEAMMATEINFO, 3, nRankIconID);
                if (sprite == null)
                    sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_TEAMMATEINFO, 3, 1);
                RankIcon.sprite = sprite;
            }
            else
            {
                RankIcon.gameObject.SetActive(false);
            }
        }

        public void SetCaptainFlag(bool bIsCap)
        {
            IsCaptain.gameObject.SetActive(bIsCap);
        }

        public void DisablePopupAction()
        {
            if (PopupMenuHandle != null)
            {
                this.gameObject.RemoveComponent<UPopupMenuHandle>();
            }
        }

        public virtual void SetPopupAction(bool bIsCap)
        {
            if (bIsCap)
            {
                PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(nPDBID));
                PopupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(nUserID, Name.text));
                PopupAction.Add(UPopupItemDataTeamShield.GetDescString(), new UPopupItemDataTeamShield(nPDBID));
                //PopupAction.Add(UPopupItemDataTeamReport.GetDescString(), new UPopupItemDataTeamReport(nPDBID));
            }
            else
            {
                PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(nPDBID));
                PopupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(nUserID, Name.text));
                PopupAction.Add(UPopupItemDataTeamAppointCaptain.GetDescString(), new UPopupItemDataTeamAppointCaptain(nPDBID));
                PopupAction.Add(UPopupItemDataTeamKickout.GetDescString(), new UPopupItemDataTeamKickout(nPDBID));
                PopupAction.Add(UPopupItemDataTeamShield.GetDescString(), new UPopupItemDataTeamShield(nPDBID));
                //PopupAction.Add(UPopupItemDataTeamReport.GetDescString(), new UPopupItemDataTeamReport(nPDBID));
            }

            if (PopupMenuHandle == null)
                PopupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            PopupMenuHandle.HandlePopupAction = PopupAction;
        }

        private Sprite GetActorIcon(int nHeadID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_TEAMMATEINFO, 1, nHeadID);
            if (sprite == USpriteManager.Instance.DefaultSprite)
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_TEAMMATEINFO, 1, 1);
            return sprite;
        }

		// 显示聊天气泡
		public void AddChatMessage(Chat.SChatMessageInfo info)
		{
			ChatBubble.gameObject.SetActive(true);

			ChatBubble.setData(info);
		}
    }

    public class UActorInfoView : UTeammateInfoView
    {
        public Text PointNum;
        //public Text GoldNum;
        public Text BindGoldNum;
        public Button inviteInfoBtn;

        public override void SetData(cmd_entity_teammate_info data)
        {
            base.SetData(data);

            Lv.text = data.nLevel.ToString();
        }

        public override void SetPopupAction(bool bIsCap)
        {
            if (bIsCap)
            {
                PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataTeamLeave.GetDescString(), new UPopupItemDataTeamLeave());
                PopupAction.Add(UPopupItemDataTeamDisband.GetDescString(), new UPopupItemDataTeamDisband());
            }
            else
            {
                PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataTeamLeave.GetDescString(), new UPopupItemDataTeamLeave());
            }

            if (PopupMenuHandle == null)
                PopupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            PopupMenuHandle.HandlePopupAction = PopupAction;
        }

		public void onClickGold()
		{
			PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Card;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
		}

		public void onClickPoint()
		{
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Rune;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
		}
    }

    public class UTeammateInfoWnd : UIBaseWnd<UTeammateInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_TEAMMATEINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Teammate/TeammateView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE_NAME, this);
            
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE_NAME, this);
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
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiTeammateInfoUpdateData = (UIMsgCmdData)msgData;
                        m_wndView.TeammateInfoUpdate(uiTeammateInfoUpdateData.ptrParam, uiTeammateInfoUpdateData.nPtrLen);                                                   
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiTeammateChangeCaptainData = (UIMsgCmdData)msgData;
                        m_wndView.TeammateInfoChangeCaptain(uiTeammateChangeCaptainData.ptrParam);
                    }
                    break;
                case WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiTeammateKickoutMemberData = (UIMsgCmdData)msgData;
                        m_wndView.TeammateInfoKickoutMember(uiTeammateKickoutMemberData.ptrParam);
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiTeammateLeaveTeamData = (UIMsgCmdData)msgData;
                        m_wndView.TeammateInfoLeaveTeam(uiTeammateLeaveTeamData.ptrParam);
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.TeammateInfoDisbandTeam();
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_REQUESTJOINTEAM:
                    {
                        if (m_wndView == null)
                            return;
                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;

                        m_wndView.RequestJoinTeamConfirm(uiData.nParam, uiData.strParam);
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.onRenameResultMessage((msgData as UIMsgCmdData).nParam);
                        }
                    }
                    break;
				case WndMsgID.WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE:
					{
						if (m_wndView != null)
							m_wndView.onAddTeamChatMessage(msgData as UTeamBubbleChatMessage);
					}
					break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE_NAME:
                    {
                        if (m_wndView != null)
                            m_wndView.UpdatePlayerName();
                    }
                    break;

				default:
                    break;
            }
        }
    }

    public class UTeammateInfoWndView : UIBaseWndView
    {
        // 回调委托
        public delegate void GuideCallBack();
        public UActorInfoView actorInfo;
        public UTeammateInfoView[] teammateInfo;

        private int nTeammateCount;
        private const int TEAM_MEMBER_MAXCOUNT = 5;
        private int nItemIndex = 0;
        ActorNameChangeView actorNameChangeView;
        ResNode actorNameChangeNode;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            UpdatePlayerProperty();

            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();
            AssetBundleManager.DeleteAssets(ref actorNameChangeNode, true);
            
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (e.objev == null || !e.objev.IsHero)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_SEX:
                    {
                        int nSex = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
                        actorInfo.HeadIcon.sprite = GetActorIcon(nSex + 1);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_RANKGRADE:
                case (int)ENTITY_PROPERTY.PROPERTY_RANKSCORE:
                    {
                        int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                        int nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
                        int nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
                        int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
                        IntPtr ptr = GameLogicAPI.getActorRankNameByGrade(nMatchType, nRankGrade);
                        string strRankName = "";
                        if (ptr != IntPtr.Zero)
                            strRankName = IntPtrHelper.Ptr2Str(ptr);
                        // 更新下本地的PlayerDataManager数据 这里是个坑 以前不是实时更新的
                        ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
                        if (info.playerInfo.pDbid > 0)
                        {
                            info.playerInfo.nRankScore = nRankScore;
                            info.playerInfo.nRankIconID = nRankIcon;
                            info.playerInfo.chRankName = strRankName;
                        }

                        if (nRankIcon > 0)
                        {
                            actorInfo.RankIcon.gameObject.SetActive(true);
                            actorInfo.RankIcon.sprite = GetActorRankIcon(nRankIcon);
                        }
                        else
                        {
                            actorInfo.RankIcon.gameObject.SetActive(false);
                        }
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
                        int nLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                        actorInfo.Lv.text = nLevel.ToString();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_TICKET:
                    {
                        int nPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
                        actorInfo.PointNum.text = nPoint.ToString();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_GOLD:
                    {
                        int nGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
                        actorInfo.BindGoldNum.text = nGold.ToString();
                    }
                    break;
                default:
                    break;
            }
        }

        public void UpdatePlayerName()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            actorInfo.Name.text = EntityFactory.MainHeroView.Property.CreatureName;
        }

        public void UpdatePlayerProperty()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int nSex = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
            actorInfo.HeadIcon.sprite = GetActorIcon(nSex + 1);

            int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
            int nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
            int nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
            int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
            if (nRankIcon > 0)
            {
                actorInfo.RankIcon.gameObject.SetActive(true);
                actorInfo.RankIcon.sprite = GetActorRankIcon(nRankIcon);
            }
            else
            {
                actorInfo.RankIcon.gameObject.SetActive(false);
            }

            int nLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            actorInfo.Lv.text = nLevel.ToString();

            int nPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
            actorInfo.PointNum.text = nPoint.ToString();

            int nGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
            actorInfo.BindGoldNum.text = nGold.ToString();

            string strName = EntityFactory.MainHeroView.Property.CreatureName;
            actorInfo.Name.text = strName;
        }

        public void TeammateInfoUpdate(IntPtr ptrParam, int nParam)
        {
            nItemIndex = 0;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);
            cmd_entity_teammate_info_count teamBaseData = helper.get<cmd_entity_teammate_info_count>();
            nTeammateCount = teamBaseData.nteammateCount;

            for (int i = 0; i < nTeammateCount; i++)
            {
                cmd_entity_teammate_info teammateData = helper.get<cmd_entity_teammate_info>();
                bool isCaptain = teammateData.nPDBID == teamBaseData.nCaptainPDBID;
                if (teammateData.nPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                {
                    actorInfo.SetData(teammateData);
                    actorInfo.SetCaptainFlag(isCaptain);
                    actorInfo.SetPopupAction(isCaptain);
                }
                else
                {
                    teammateInfo[nItemIndex].SetData(teammateData);
                    teammateInfo[nItemIndex].SetCaptainFlag(isCaptain);
                    teammateInfo[nItemIndex].SetPopupAction(isCaptain);
                    teammateInfo[nItemIndex].gameObject.SetActive(true);
                    nItemIndex++;                        
                }
            }

            for (int i = nItemIndex; i < TEAM_MEMBER_MAXCOUNT - 1; i++)
            {
                teammateInfo[i].gameObject.SetActive(false);
            }
        }

        public void TeammateInfoChangeCaptain(IntPtr ptr)
        {
            // 改变队长
            cmd_team_changeCaptain2view data = IntPtrHelper.toData<cmd_team_changeCaptain2view>(ptr);
            
            bool isCaptain = data.nCaptainPDBID == actorInfo.nPDBID;
            actorInfo.SetCaptainFlag(isCaptain);
            actorInfo.SetPopupAction(isCaptain);
            for (int i = 0; i < nTeammateCount - 1; i++)
            {
                if (teammateInfo[i] == null)
                    return;

                isCaptain = data.nCaptainPDBID == teammateInfo[i].nPDBID;
                teammateInfo[i].SetCaptainFlag(isCaptain);
                teammateInfo[i].SetPopupAction(isCaptain);
            }
        }

        public void TeammateInfoKickoutMember(IntPtr ptr)
        {
            // 踢出队员
            cmd_team_kickoutMember2view data = IntPtrHelper.toData<cmd_team_kickoutMember2view>(ptr);

            if (data.nKickoutPDBID == actorInfo.nPDBID)
            {
                actorInfo.DisablePopupAction();

                for (int i = 0; i < nTeammateCount - 1; i++)
                {
                    if (teammateInfo[i] == null)
                        return;

                    teammateInfo[i].gameObject.SetActive(false);
                }
            }
        }

        public void TeammateInfoLeaveTeam(IntPtr ptr)
        {
            // 离开队伍
            cmd_team_leaveTeam2view data = IntPtrHelper.toData<cmd_team_leaveTeam2view>(ptr);

            if (data.nLeaveMemberPDBID == actorInfo.nPDBID)
            {
                actorInfo.DisablePopupAction();
                actorInfo.SetCaptainFlag(false);

                for (int i = 0; i < nTeammateCount - 1; i++)
                {
                    if (teammateInfo[i] == null)
                        return;

                    teammateInfo[i].gameObject.SetActive(false);
                }
            }

        }

        public void TeammateInfoDisbandTeam()
        {
            // 解散队伍
            actorInfo.DisablePopupAction();
            actorInfo.SetCaptainFlag(false);

            for (int i = 0; i < nTeammateCount - 1; i++)
            {
                if (teammateInfo[i] == null)
                    return;

                teammateInfo[i].gameObject.SetActive(false);
            }
        }

        public void OnHeadIconClick()
        {
            UIButtonType buttonType = new UIButtonType((int)PlayerSubWndType.WND_HERO);
            buttonType.bIsRequestSelf = true;
            buttonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, buttonType);
        }

        public void OnInviteFrameClick()
        {
            // 临时邀请组队
            SChatMessageObjectInfo channelObj;
            channelObj.type = ChatItemType.ObjType_Text;
            channelObj.subType = ChatItemSubType.ObjSubType_Text_JoinTeam;
            channelObj.text = "加入我的队伍";
            channelObj.param = new Dictionary<String, String>();
            channelObj.param.Add("color", LogicDataCenter.chatMessageManager.CreateColorParam(235, 80, 30));
            channelObj.param.Add("id", "" + EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID));
            channelObj.param.Add("name", EntityFactory.MainHeroView.Property.CreatureName);
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = channelObj;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);

        }

        public void OnTeammateClanInviteJoinClick()
        {

            string strTitle = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTitleDes");
            string strMessage = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideDes");
            string strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes1");

            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
            int nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
            GuideCallBack handler = ConveneClanMemberCallBack;
            if (nKinID == 0)
            {
                strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinKinDes");
                strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes1");
                handler = OpenKinListCallBack;
            }
            else
            {
                if (nClanID == 0)
                {
                    if (nPDBID == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinClanDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes2");
                        handler = OpenClanListCallBack;
                    }
                    else
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTipKinDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes3");
                        handler = NotifyKinShaikhJoinClanCallBack;
                    }
                }
            }

            if (ConveneClanMemberCallBack == handler)
            {
                handler();
            }
            else
            {
                USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                                  title: strTitle,
                                  message: strMessage,
                                  buttons: new UIWidgets.DialogActions() { { strConfirm, () => { handler(); return true; } } },
                                  modal: true
                                  );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
                
            }
        }


        public void OpenKinListCallBack()
        {
            UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_KIN, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        }

        public void OpenClanListCallBack()
        {
            UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_CLAN, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        }

        public void ConveneClanMemberCallBack()
        {
            // 联盟邀请组队
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_TEAM_CLAN_INVITE_JOIN_TEAM);
        }
         

        public void NotifyKinShaikhJoinClanCallBack()
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
            if (nKinID == 0)
                return;

            String strMessage = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "TipShaikhJoinClanDes"), LogicDataCenter.kinDataManager.KinBaseData.szShaikhName, ChatMessageManager.ChatItemFlag);
            UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_KIN, strMessage);
        }

        private Sprite GetActorIcon(int nHeadID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, GetUIWnd().GetID(), 1, nHeadID);
            if (sprite == null)
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, GetUIWnd().GetID(), 1, 1);
            return sprite;
        }

        private Sprite GetActorRankIcon(int nRankIconID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, GetUIWnd().GetID(), 3, nRankIconID);
            if (sprite == null)
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, GetUIWnd().GetID(), 3, 1);
            return sprite;
        }
        

        public void RequestJoinTeamConfirm(int idActor, string strActorName)
        {
            // 是自己
            if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == idActor)
            {
                return;
            }
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "TeamMate", "RequestJoinTitle"),
                                    message: String.Format(ULocalizationService.Instance.Get("UIView", "TeamMate", "RequestJoinDes"), strActorName),
                                    buttons: new UIWidgets.DialogActions() { {ULocalizationService.Instance.Get("UIView", "TeamMate", "RequestJoinConfirm"), () => { ConfirmBtnClick( idActor );return true; }}, 
                                                                                {ULocalizationService.Instance.Get("UIView", "TeamMate", "RequestJoinCancel"), null} 
                                                                            } 

                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool ConfirmBtnClick(int nActorID)
        {
            LogicDataCenter.teamDataManager.RequestJoin(nActorID);
            return true;
        }

        public void OnNameChangeBtnClick()
        {
            if (actorNameChangeView == null)
            {
                actorNameChangeNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "PlayerInfo/" + "ActorNameChangeView");
                if (actorNameChangeNode == null)
                {
                    Debug.Log("actorNameChangePrefab Is Null");
                    return;
                }

                GameObject actorNameChangeObject = actorNameChangeNode.InstanceMainRes();
                if (actorNameChangeObject == null)
                {
                    return;
                }

                actorNameChangeView = actorNameChangeObject.GetComponent<ActorNameChangeView>();
                actorNameChangeObject.transform.SetParent(this.transform, false);
                actorNameChangeObject.GetComponent<RectTransform>().anchoredPosition = new Vector3(-25f,-104f,0.0f);
                actorNameChangeObject.SetActive(false);
            }

            if (actorNameChangeView == null)
            {
                return;
            }

            actorNameChangeView.OpenActorNameChangeView();
        }

        public void onRenameResultMessage(int nResult)
        {
            if (actorNameChangeView == null)
            {
                return;
            }

            actorNameChangeView.onRenameResultMessage(nResult);
        }

		// 队伍聊天气泡
		public void onAddTeamChatMessage(UTeamBubbleChatMessage msgData)
		{
			if (msgData == null)
				return;

			uint pdbid = msgData.pdbid;

			foreach(var item in teammateInfo)
			{
				if(item.nPDBID == pdbid)
				{

					item.AddChatMessage(msgData.info);
					break;
				}
			}
		}
    }
}
