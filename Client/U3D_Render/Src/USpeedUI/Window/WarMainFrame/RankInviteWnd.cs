using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine.UI;
using UnityEngine;
using Match_ManagerDef;
using DataCenter;
using GameLogic;
using UnityEngine.EventSystems;
using USpeedUI.UEffect;

namespace USpeedUI.WarMainFrame
{
    public class RankInviteWnd : UIFillWnd<RankInviteWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_RANK_INVITE;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "WarWnd/RankInviteView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            //return WndLayerDef.WND_LAYER_FILL;
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_INVITE_RETURN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_INVITE_RETURN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_CHANGECAPTAIN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_WSG_TEAMMATEINFO_KICKOUTMEMBER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_LEAVETEAM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_DISBANDTEAM, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RANK_INVITE_SHOW:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                            return;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            m_wndView.OnShowRankInviteView(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_RANK_INVITE_RETURN:
                    {
                        if (m_wndView == null)
                            return;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.OnReturnRankInviteView();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER:
                    {
                        UnloadView();
                    }
                    break;

                    // 以下为队伍操作
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
                default:
                    break;
            }
        }
    }

    public class IRankInviteFrame : UCustomUIBehaviour
    {
        public virtual void Init(RankInviteWndView view) { }

        public virtual void Clear() { }
    }

    // 玩家
    public class RankInvitePlayerItem : UCustomUIBehaviour
    {
        public GameObject EmptyGo;
        public Button InviteMatchTeamBtn;

        public GameObject PlayerGo;
        public Image HeadIcon;
        public Image MasterIcon;
        public Button DeleteBtn;
        public Text NameDesc;
        public GameObject RankFrame;
        public Image RankIcon;
        public Text RankDesc;

        private int m_nPDBID;
        public int PDBID { get { return m_nPDBID; } }

        public void SetSelfData()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            cmd_entity_teammate_info selfData = new cmd_entity_teammate_info();
            selfData.nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            selfData.nHeadID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_FACEID);
            selfData.nLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            selfData.isCaptain = 1;
            selfData.nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
            selfData.byMatchType = (byte)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
            selfData.nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
            selfData.szName = EntityFactory.MainHeroView.Property.CreatureName;
            SetData(true, selfData);
        }

        public void SetData(bool bSelfCaptain, cmd_entity_teammate_info data)
        {
            m_nPDBID = data.nPDBID;

            PlayerGo.SetActive(true);
            EmptyGo.SetActive(false);

            // HeadIcon

            MasterIcon.gameObject.SetActive(data.isCaptain > 0);
            DeleteBtn.gameObject.SetActive(bSelfCaptain && data.isCaptain <= 0);

            NameDesc.text = data.szName;

            int nMatchType = data.byMatchType;
            int nRankScore = data.nRankScore;
            int nRankGrade = data.nRankGrade;
            if (nRankScore <= 0)
            {
                RankFrame.SetActive(false);
                return;
            }
            RankFrame.SetActive(true);

            int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
            IntPtr ptr = GameLogicAPI.getActorRankNameByGrade(nMatchType, nRankGrade);
            string strRankName = "";
            if (ptr != IntPtr.Zero)
                strRankName = IntPtrHelper.Ptr2Str(ptr);
            RankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_WAR_MAIN_FRAME, 1, nRankIcon);
            RankDesc.text = strRankName;
        }

        public void ChangeCaptain(int nNewCaptain)
        {
            if (EntityFactory.MainHeroView == null)
                return;
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            bool bSelfCaptain = nSelfID == nNewCaptain;
            bool bPosCaptain = m_nPDBID == nNewCaptain;

            MasterIcon.gameObject.SetActive(bPosCaptain);
            DeleteBtn.gameObject.SetActive(bSelfCaptain && !bPosCaptain);
        }

        public void SetClear()
        {
            m_nPDBID = 0;
            EmptyGo.SetActive(true);
            PlayerGo.SetActive(false);
        }

        public void OnClickItem()
        {
            if (m_nPDBID <= 0 && LogicDataCenter.viewPersonStateDataManager.bIsMatching == false)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INVITE_MATCH_WND);
            }
        }

        public void OnClickDeleteBtn()
        {
            if (m_nPDBID > 0 && LogicDataCenter.viewPersonStateDataManager.bIsMatching == false)
            {
                LogicDataCenter.teamDataManager.KickoutMember(m_nPDBID);
            }
        }
    }

    public class RankInvitePlayerFrame : IRankInviteFrame
    {
        public RectTransform PlayerRect;
        public GameObject PlayerTemplate;

        private RankInvitePlayerItem[] m_playerItems;
        private const int PLAYER_ITEM_COUNT = 5;

        public override void Init(RankInviteWndView view)
        {
            base.Init(view);

            // 清空
            int nCount = PlayerRect.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = PlayerRect.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            // 初始化
            PlayerTemplate.SetActive(true);
            m_playerItems = new RankInvitePlayerItem[PLAYER_ITEM_COUNT];
            for (int i = 0; i < PLAYER_ITEM_COUNT; ++i)
            {
                GameObject playerItemGo = ResNode.InstantiateRes(PlayerTemplate);
                if (!playerItemGo)
                    return;

                playerItemGo.name = "PlayerItem" + (i + 1);
                RectTransform rectTrans = playerItemGo.transform as RectTransform;
                rectTrans.SetParent(PlayerRect, false);

                m_playerItems[i] = playerItemGo.GetComponent<RankInvitePlayerItem>();
                if (!m_playerItems[i])
                    return;
                m_playerItems[i].SetClear();
            }
            PlayerTemplate.SetActive(false);
        }

        public void SetSelfData()
        {
            m_playerItems[0].SetSelfData();

            for (int i = 1; i < PLAYER_ITEM_COUNT; i++)
            {
                m_playerItems[i].SetClear();
            }
        }

        public void TeammateInfoUpdate(IntPtr ptrParam, int nParam)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);
            cmd_entity_teammate_info_count head = helper.get<cmd_entity_teammate_info_count>();

            if (EntityFactory.MainHeroView == null)
                return;
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            bool bSelfCaptain = nSelfID == head.nCaptainPDBID;
            int nIndex = 0;
            for (int i = 0; i < head.nteammateCount; i++)
            {
                cmd_entity_teammate_info node = helper.get<cmd_entity_teammate_info>();
                m_playerItems[nIndex].SetData(bSelfCaptain, node);
                nIndex++;
            }

            for (int i = nIndex; i < PLAYER_ITEM_COUNT; i++)
            {
                m_playerItems[nIndex].SetClear();
                nIndex++;
            }
        }

        public void TeammateInfoChangeCaptain(IntPtr ptr)
        {
            cmd_team_changeCaptain2view data = IntPtrHelper.toData<cmd_team_changeCaptain2view>(ptr);

            foreach (var item in m_playerItems)
            {
                if (item.PDBID <= 0)
                    continue;

                item.ChangeCaptain(data.nCaptainPDBID);
            }
        }

        public void TeammateInfoKickoutMember(IntPtr ptr)
        {
            cmd_team_kickoutMember2view data = IntPtrHelper.toData<cmd_team_kickoutMember2view>(ptr);

            foreach (var item in m_playerItems)
            {
                if (item.PDBID == data.nKickoutPDBID)
                {
                    item.SetClear();
                    break;
                }
            }

            if (EntityFactory.MainHeroView == null)
                return;
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (nSelfID == data.nKickoutPDBID)
            {
                SetSelfData();
            }
        }

        public void TeammateInfoLeaveTeam(IntPtr ptr)
        {
            cmd_team_leaveTeam2view data = IntPtrHelper.toData<cmd_team_leaveTeam2view>(ptr);

            foreach (var item in m_playerItems)
            {
                if (item.PDBID == data.nLeaveMemberPDBID)
                {
                    item.SetClear();
                    break;
                }
            }

            SetSelfData();
        }

        public void TeammateInfoDisbandTeam()
        {
            foreach (var item in m_playerItems)
            {
                item.SetClear();
            }

            SetSelfData();
        }
    }

    // 杂项
    public class RankInviteMiscFrame : IRankInviteFrame
    {
        public Text TitleDesc;
        public Text StartBtnDesc;
        public Text HideBtnDesc;
        public URichText InviteClanRT;

        private int m_nMatchType;
        private RankInviteWndView m_wndView;

        public override void Init(RankInviteWndView view)
        {
            base.Init(view);

            m_wndView = view;

            HideBtnDesc.text = "隐藏";
            InviteClanRT.text = "<a href=InviteClan color=ffca27>" + "点击联盟同伴共同开黑" + "</a>";
            InviteClanRT.onHrefClick.AddListener(OnClickInviteClanRT);
        }

        public void SetData(int nMatchType)
        {
            m_nMatchType = nMatchType;

            IntPtr ptr = GameLogicAPI.getMatchNameByMatchType(nMatchType);
            string strMatchName = "";
            if (ptr != IntPtr.Zero)
                strMatchName = IntPtrHelper.Ptr2Str(ptr);
            TitleDesc.text = strMatchName;

            if (nMatchType == (int)EMMatchType.MatchType_Rank)
            {
                StartBtnDesc.text = "开始排位";
            }
            else if (nMatchType == (int)EMMatchType.MatchType_Match)
            {
                StartBtnDesc.text = "开始匹配";
            }
        }

        public void OnClickInviteClanRT(string eventName)
        {
            if (eventName == "InviteClan")
            {
                string strTitle = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTitleDes");
                string strMessage = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideDes");
                string strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes1");

                int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
                int nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                USpeedUI.TeammateInfo.UTeammateInfoWndView.GuideCallBack handler = ConveneClanMemberCallBack;
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

        public void OnClickStartBtn()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }

            if (m_nMatchType == (int)EMMatchType.MatchType_Rank)
            {
                LogicDataCenter.matchDataManager.OnRankModeBtnClick();
            }
            else if (m_nMatchType == (int)EMMatchType.MatchType_Match)
            {
                LogicDataCenter.matchDataManager.OnMatchModeBtnClick();
            }
        }

        public void OnClickHideBtn()
        {
            // 隐藏
            if (m_wndView != null)
            {
                m_wndView.SetVisible(false);
            }

            // 显示回到界面入口
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == false)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_SHOW);
            }
        }

        public void OnClickCloseBtn()
        {
            if (m_wndView != null)
            {
                m_wndView.SetVisible(false);
            }
        }
    }

    public class RankInviteWndView : UIBaseWndView
    {
        public Image BaseEffect;

        public RankInvitePlayerFrame PlayerFrame;
        public RankInviteMiscFrame MiscFrame;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            UEffectParamBase materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, BaseEffect, true);
            if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref materialEffectParam))
            {
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.20f));
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_Vibrancy", 0f);
            }

            PlayerFrame.Init(this);
            MiscFrame.Init(this);

            return true;
        }

        public void OnShowRankInviteView(int nMatchType)
        {
            if (EntityFactory.MainHeroView == null)
                return;
            int TeamID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID);
            if (TeamID <= 0)
            {
                PlayerFrame.SetSelfData();
            }

            MiscFrame.SetData(nMatchType);

            // 隐藏返回匹配界面
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == false)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_HIDE);
            }
        }

        public void OnReturnRankInviteView()
        {
            if (EntityFactory.MainHeroView == null)
                return;
            int TeamID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TEAMID);
            if (TeamID <= 0)
            {
                PlayerFrame.SetSelfData();
            }
        }


        public void TeammateInfoUpdate(IntPtr ptrParam, int nParam)
        {
            PlayerFrame.TeammateInfoUpdate(ptrParam, nParam);
        }

        public void TeammateInfoChangeCaptain(IntPtr ptr)
        {
            PlayerFrame.TeammateInfoChangeCaptain(ptr);
        }

        public void TeammateInfoKickoutMember(IntPtr ptr)
        {
            PlayerFrame.TeammateInfoKickoutMember(ptr);
        }

        public void TeammateInfoLeaveTeam(IntPtr ptr)
        {
            PlayerFrame.TeammateInfoLeaveTeam(ptr);
        }

        public void TeammateInfoDisbandTeam()
        {
            PlayerFrame.TeammateInfoDisbandTeam();
        }
    }
}
