/*******************************************************************
** 文件名:	UPlayerButtonFrameView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	所有系统的按钮
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UnityEngine;
using UIWidgets;
using U3D_Render.Common;
using USpeedUI.PointShop;
using DataCenter;
using USpeedUI.PlayerFrame;
using Match_ManagerDef;
using GameLogic;
using USpeedUI.UExtensions;
using U3D_Render;

namespace USpeedUI.PlayerButtonFrame
{
    public class UPLayerButtonFrameView : UIBaseWndView
    {
        public Button playerBtn;
        public Button socialBtn;
        public Button taskBtn;
        public Button warHallBtn;
        public Button rankBtn;
        public Button genstoneBtn;
        public Button LegendCupBtn;
        public Button chestBtn;
        public Button shopBtn;
        public Button mailBoxBtn;
        public Button wingBtn;
        public Button PkBtn;
        public Button friendBtn;
        public Button systemBtn;
		public Button hideBtn;

        private UTooltipTrigger mailBoxTrigger;
        private UTooltipTrigger wingTipTrigger;
        private UTooltipTrigger friendTipTrigger;
        private UTooltipTrigger pkTipTrigger;
        private UTooltipTrigger systemTipTrigger;
        private UTooltipTrigger hideTipTrigger;

        private URedPointTip m_socialRedPoint;
        private URedPointTip m_chestRedPoint;
        private URedPointTip m_mailBoxRedPoint;
        private URedPointTip m_taskRedPoint;
        private URedPointTip m_gemstoneRedPoint;

        private string mailBoxBtnTip;
        private string wingBtnTip;
        private string friendBtnTip;
        private string pkBtnTip;
        private string systemBtnTip;
        private string hideBtnTip;

        private UEffect.UEffectParamBase m_nBtnWarHallEffect = null;

        public override void Destroy()
        {
            URedPointTip.DestroyRedPointTip(m_socialRedPoint);
            URedPointTip.DestroyRedPointTip(m_chestRedPoint);
            URedPointTip.DestroyRedPointTip(m_mailBoxRedPoint);
            URedPointTip.DestroyRedPointTip(m_taskRedPoint);
            URedPointTip.DestroyRedPointTip(m_gemstoneRedPoint);
            if (m_nBtnWarHallEffect != null)
            {
                UEffect.UEffectManager.Instance.DestroyEffect(UEffect.UEffectType.UET_EffectPrefab, ref m_nBtnWarHallEffect);
                m_nBtnWarHallEffect = null;
            }

            base.Destroy();
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            //SetVisible(true);

			// 设置系统按钮文字
			Text playerBtnText = playerBtn.transform.FindChild("Text").GetComponent<Text>();
			playerBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "PlayerBtn");
			Text socialBtnText = socialBtn.transform.FindChild("Text").GetComponent<Text>();
			socialBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "SocialBtn");
			Text taskBtnText = taskBtn.transform.FindChild("Text").GetComponent<Text>();
			taskBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "TaskBtn");
			Text rankBtnText = rankBtn.transform.FindChild("Text").GetComponent<Text>();
			rankBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "RankBtn");

			Text genstoneText = genstoneBtn.transform.FindChild("Text").GetComponent<Text>();
			genstoneText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "GenstoneBtn");
			Text LegendText = LegendCupBtn.transform.FindChild("Text").GetComponent<Text>();
			LegendText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "LegendCupBtn");
			Text chestBtnText = chestBtn.transform.FindChild("Text").GetComponent<Text>();
			chestBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "ChestBtn");
			Text shopBtnText = shopBtn.transform.FindChild("Text").GetComponent<Text>();
			shopBtnText.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "ShopBtn");


			mailBoxBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "mailBoxBtnTip");
            wingBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "wingBtnTip");
            friendBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "friendBtnTip");
            pkBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "pkBtnTip");
            systemBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "systemBtnTip");
            hideBtnTip = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "hideBtnTip");

            m_socialRedPoint = URedPointTip.CreateRedPointObject(socialBtn.transform as RectTransform, -29f, -12f);
            m_chestRedPoint = URedPointTip.CreateRedPointObject(chestBtn.transform as RectTransform, -29f, -12f);
            m_gemstoneRedPoint = URedPointTip.CreateRedPointObject(genstoneBtn.transform as RectTransform, -29f, -12f);
            m_taskRedPoint = URedPointTip.CreateRedPointObject(taskBtn.transform as RectTransform, -29f, -12f);
			m_mailBoxRedPoint = URedPointTip.CreateRedPointObject(mailBoxBtn.transform as RectTransform, 13f, 19f);

            mailBoxTrigger = mailBoxBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref mailBoxBtnTip);
            mailBoxTrigger.SetText(UTooltipParamName.BodyText, mailBoxBtnTip);
            wingTipTrigger = wingBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref wingBtnTip);
            wingTipTrigger.SetText(UTooltipParamName.BodyText, wingBtnTip);
            friendTipTrigger = friendBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref friendBtnTip);
            friendTipTrigger.SetText(UTooltipParamName.BodyText, friendBtnTip);
            pkTipTrigger = PkBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref pkBtnTip);
            pkTipTrigger.SetText(UTooltipParamName.BodyText, pkBtnTip);
            systemTipTrigger = systemBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref systemBtnTip);
            systemTipTrigger.SetText(UTooltipParamName.BodyText, systemBtnTip);
            hideTipTrigger = hideBtn.GetComponent<UTooltipTrigger>();
            UBB.toHtml(ref hideBtnTip);
            hideTipTrigger.SetText(UTooltipParamName.BodyText, hideBtnTip);

            if (warHallBtn != null)
            {
                if(m_nBtnWarHallEffect != null)
                {
                    UEffect.UEffectManager.Instance.DestroyEffect(UEffect.UEffectType.UET_EffectPrefab, ref m_nBtnWarHallEffect);
                    m_nBtnWarHallEffect = null;
                }
                //m_nBtnWarHallEffect = new UEffect.UEffectPrefabParam(UEffect.UEffectPrefabType.UEPT_PlayerSysButton_BtnWarHall, warHallBtn.transform);
                m_nBtnWarHallEffect = new UEffect.UEffectPrefabAsyncParam(UEffect.UEffectPrefabType.UEPT_PlayerSysButton_BtnWarHall, warHallBtn.transform);
                UEffect.UEffectManager.Instance.CreateEffect(UEffect.UEffectType.UET_EffectPrefab, ref m_nBtnWarHallEffect);
            }

            if(warHallBtn != null)
                warHallBtn.onClick.AddListener(onWarHillPressDwon);

            return true;
        }

        public void onPlayerPressDown()
        {
            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
            ButtonType.bIsRequestSelf = true;
            ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
        }

        public void onSocialPressDown()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, null);
        }

        public void onWarHillPressDwon()
        {
            if (LogicDataCenter.pkDataManager.BIsPK)
            {
                TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "PK", "StopMessage"),
                        message: ULocalizationService.Instance.Get("UIView", "PK", "SurrenderMessage"),
                        buttons: new DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), CancelPK},
                                                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                        );

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
                return;
            }

            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, null);
        }

        public void OnGuideStepFirstEnterClick()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }

            //GuideStep_StartWar_FirstEnter// 点新手引导UI进新手一号战场

            // 隐藏其他同级窗体
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
            UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);

            // 引导相关
            cmd_match_create_guide_room data = new cmd_match_create_guide_room();
            data.nMatchType = (int)EMMatchType.MatchType_NewPlayerGuide1;
            data.nGuidHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
 

            ViewEventHelper.Instance.SendCommand<cmd_match_create_guide_room>(GameLogicDef.GVIEWCMD_CREATE_GUIDE_ROOM, ref data);

            // 发送到逻辑层 引导相关记录
            cmd_guide_info guideInfo = new cmd_guide_info();
            guideInfo.nGuideID = (int)EMMatchGuideStep.GuideStep_StartWar_FirstEnter;

            ViewEventHelper.Instance.SendCommand<cmd_guide_info>(GameLogicDef.GVIEWCMD_MATCH_GUIDE_INFO, ref guideInfo);

        }

        public void OnGuideMachineBtnClick()
        {
            if (LogicDataCenter.pkDataManager.BIsPK)
            {
                TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "PK", "StopMessage"),
                        message: ULocalizationService.Instance.Get("UIView", "PK", "SurrenderMessage"),
                        buttons: new DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), CancelPK},
                                                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                        );

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
                return;
            }

            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, null);

            // 发送到逻辑层 引导相关记录
            cmd_guide_info dataInfo = new cmd_guide_info();
            dataInfo.nGuideID = (int)EMMatchGuideStep.GuideStep_RadomMachine_StartWar;

            ViewEventHelper.Instance.SendCommand<cmd_guide_info>(GameLogicDef.GVIEWCMD_MATCH_GUIDE_INFO, ref dataInfo);

        }

        public void onTaskPressDown()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, null);
        }

        private bool CancelPK()
        {
            GameLogic.cmd_creature_pk_cancel_pk data = new GameLogic.cmd_creature_pk_cancel_pk();
            data.nCancel = 1;
            EntityEventHelper.Instance.SendCommand<GameLogic.cmd_creature_pk_cancel_pk>(EntityFactory.MainHeroID, GameLogic.EntityLogicDef.ENTITY_CMD_PK_CANCEL_PK, ref data);
            return true;
        }

        public void onLeaderboardClick()
        {
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_OPEN, null);
            UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, true);
        }

        public void onMailBoxClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, null);
        }

        public void onWingClick()
        {
            (m_wnd as UPlayerButtonFrame).onWingClick();
        }

        public void onChestClick()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZECHEST, true);
        }

        public void onPKClick()
        {
            StartCoroutine(PkSelect());
        }

        public void onFriendClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SWITCH_MAINPANEL, null);
        }

        public void onSystemClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, null);
        }

        public void onShopClick()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_AD;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

		public void onGenstoneClick()
		{
			UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_CROWN);
			ButtonType.bIsRequestSelf = true;
			ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
		}

        public void onLegendCupClick()
        {
            UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SHOW, (int)ELegendCupListType.ELCLT_All, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CUPLIST_SHOW, uiData);
        }

		public void onHideClick()
		{

		}

        public void SetRedPointTip(RedPointSetCount tipInfo)
        {
            if (tipInfo == null)
                return;

            if (tipInfo.nSubType == 1)
            {
                m_socialRedPoint.SetTipCount(tipInfo.nCount);
            }
            else if (tipInfo.nSubType == 2)
            {
                m_chestRedPoint.SetTipCount(tipInfo.nCount);
            }
            else if (tipInfo.nSubType == 3)
            {
                m_mailBoxRedPoint.SetTipCount(tipInfo.nCount);
            }
            else if (tipInfo.nSubType == 4)
            {
                m_taskRedPoint.SetTipCount(tipInfo.nCount);
            }
            else if (tipInfo.nSubType == 5)
            {
                m_gemstoneRedPoint.SetTipCount(tipInfo.nCount);
            }
        }

        IEnumerator PkSelect()
        {
            yield return new WaitForEndOfFrame();
            if (EntityFactory.MainHero)
            {
                SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                if (sc != null)
                {
                    //sc.PKTargetSelection();
                    sc.doingPKSelection = true;
                }
            }
        }

        public void SimulateOpenView(ESimulateOpenViewType eSimulateOpenViewType)
        {
            switch (eSimulateOpenViewType)
            {
                case ESimulateOpenViewType.ESOP_SHOP:
                    {
                        PointShopOpenData uiData = new PointShopOpenData();
                        uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Chest;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
                    }
                    break;
                case ESimulateOpenViewType.ESOP_PLAYER_HERO:
                    {
                        UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
                        ButtonType.bIsRequestSelf = true;
                        ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
                    }
                    break;
                case ESimulateOpenViewType.ESOP_WAR_MAIN:
                    {
                        onWarHillPressDwon();
                    }
                    break;
                default:
                    break;
            }

            return;
        }

        public void UpdateMailRedTip()
        {
            // todo 邮件读取提醒

            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 3;
            uiData.nCount = LogicDataCenter.mailDataManager.MailCount;
            SetRedPointTip(uiData);
        }

        public void SetGemstoneRedPointCount(int nRedPointCount)
        {
            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 5;
            uiData.nCount = nRedPointCount;
            SetRedPointTip(uiData);
        }

        #region 新手引导
        internal void AddGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if(warHallBtn != null)
            {
                Guide.UGuideWidget guideWidget = warHallBtn.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = warHallBtn.gameObject.AddComponent<Guide.UGuideWidget>();

                guideWidget.SetFromMsgData(_msgData);

                if(_msgData.oUserData != null)
                {
                    if ((string)_msgData.oUserData == ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtnGuide"))
                        SetWarHallBtnType(false);
                    else
                        SetWarHallBtnType(true);
                }

                {
                    if (_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_2001)
                    {
                        warHallBtn.onClick.RemoveAllListeners();
                        warHallBtn.onClick.AddListener(OnGuideStepFirstEnterClick);
                    }
                    else if (_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_3001)
                    {
                        warHallBtn.onClick.RemoveAllListeners();
                        warHallBtn.onClick.AddListener(OnGuideMachineBtnClick);
                        warHallBtn.onClick.AddListener(onWarHillPressDwon);
                    }
                }
            }
        }

        internal void RemoveGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (warHallBtn != null)
            {
                Guide.UGuideWidget guideWidget = warHallBtn.gameObject.GetComponent<Guide.UGuideWidget>();
                if(guideWidget != null)
                {
                    gameObject.RemoveComponent<Guide.UGuideWidget>();

                    SetWarHallBtnType(true);
                    //Text txtTitle = warHallBtn.GetComponentInChildren<Text>();
                    //txtTitle.text = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtn");
                }
                warHallBtn.onClick.RemoveAllListeners();
                warHallBtn.onClick.AddListener(onWarHillPressDwon);                
            }
        }
        #endregion

        protected void SetWarHallBtnType(bool _bHallType)
        {
            Transform HallBtnGuide = null;
            Transform HallBtn = null;

            HallBtnGuide = warHallBtn.transform.FindChild("HallBtnGuide");
            HallBtn = warHallBtn.transform.FindChild("HallBtn");
            if(HallBtnGuide != null)
                HallBtnGuide.gameObject.SetActive(!_bHallType);
            if (HallBtn != null)
                HallBtn.gameObject.SetActive(_bHallType);
        }
    }
}
