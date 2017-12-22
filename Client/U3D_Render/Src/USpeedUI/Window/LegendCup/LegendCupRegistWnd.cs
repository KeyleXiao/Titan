using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using GameLogic;
using DataCenter;
using UIWidgets;
using ASpeedGame.Data.LegendCupConfig;
using U3D_Render.Common;
using USpeedUI.UWidgets;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;
using USpeedUI.TooltipAndDialog;
using USpeedUI.Chat;
using LegendCup_ManagedDef;

namespace USpeedUI.LegendCup
{
    public class LegendCupRegistWnd : UIPopupWnd<LegendCupRegistWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_REGIST;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupRegistView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_KINLIST, this);
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_CLOSE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_KINLIST, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_CLOSE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_REGIST_KINLIST:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnRecvKinList(cmdData.nLegendCupID);
                        }
                    }
                    break;
                //case WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST:
                //    {
                //        if (m_wndView != null)
                //        {
                //            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                //            m_wndView.OnRecvKinMemberList(cmdData.nParam);
                //        }    
                //    }
                //    break;
                case WndMsgID.WND_MSG_LEGENDCUP_REGIST_CLOSE:
                    {
                        m_wndView.OnClickCloseBtn();
                    }
                    break;

                default:
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }
            return base.OnCloseKey();
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }
    }

    public class LegendCupRegistKinMemberItem : MonoBehaviour
    {
        public Text KinMemberName;
        public Text KinMemberLevel;
        public Image KinMemberRank;
        public Image KinMemberSex;

        public void SetData(cmd_legendcup_recv_regist_memberlistnode memeberNode)
        {
            KinMemberName.text = memeberNode.szActorName;
            KinMemberLevel.text = memeberNode.nActorLevel.ToString();

            if (memeberNode.nRankIconId != 0)
            {
                KinMemberRank.gameObject.SetActive(true);
                KinMemberRank.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_LEGENDCUP_REGIST, 1, memeberNode.nRankIconId);
            }
            else
            {
                KinMemberRank.gameObject.SetActive(false);
            }

            if (memeberNode.nActorSex < (int)PERSON_SEX.SEX_MAX && memeberNode.nActorSex >= (int)PERSON_SEX.SEX_MALE)
            {
                KinMemberSex.gameObject.SetActive(true);
                KinMemberSex.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_LEGENDCUP_REGIST, memeberNode.nActorSex + 1);
            }
            else
            {
                KinMemberSex.gameObject.SetActive(false);
            }

            // 添加右键弹窗
            this.gameObject.RemoveComponent<UPopupMenuHandle>();
            UPopupMenuHandle popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            if (popupMenuHandle != null)
            {
                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)memeberNode.nPDBID));
                popupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(memeberNode.uDBID, memeberNode.szActorName));
                popupAction.Add(UPopupItemDataAddBlackList.GetDescString(), new UPopupItemDataAddBlackList(memeberNode.uDBID, memeberNode.szActorName));
                popupAction.Add(UPopupItemDataCopyName.GetDescString(), new UPopupItemDataCopyName(memeberNode.szActorName));
                popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(memeberNode.szActorName));

                popupMenuHandle.HandlePopupAction = popupAction;
            }

            this.gameObject.SetActive(true);
        }
    }

    public class LegendCupRegistWndView : UIBaseWndView , ITimerHanlder
    {
        public Text SysTitleDes;
        public Text CreaterDes;
        public Text TitleDes;
        public Text KinIndexDes;
        public Text KinNameDes;
        public Text KinLevelDes;
        public Text KinCupResultDes;
        public UListLegendCupRegist LegendCupRegistList;
        private int m_CurSelectIndex;
        private UListItemLegendCupRegist m_CurSelectItem;
        private UListComponentLegendCupRegist m_CurSelectComponent;
        private ObservableList<UListItemLegendCupRegist> m_AllDataSource;

        public Text CupConfigName;
        public Image LockIcon;
        public Image OnlineIcon;
        public UTooltipTrigger LockIconTip;
        public UTooltipTrigger OnlineIconTip;

        //public Text KinMemberDes;
        //public Text KinMemberLevelDes;
        //public Text KinMemberRankDes;
        //public RectTransform KinMemberListFrame;
        //public GameObject KinMemberTemplate;
        //private LegendCupRegistKinMemberItem[] m_kinMemberItems;

        public URichText CupRegistTip;
        public Text RegistKinNumDes;
        public Text RegistDeadlineDes;
        public Text AddCupListBtnDes;
        public Text DeleteCupListBtnDes;
        public Text AddBlackListBtnDes;
        public Text ViewBlackListBtnDes;
        public Text ReleaseCupBtnDes;
        public Text IniviteRegistBtnDes;
        public Text RegistKinNum;
        public Text RegistDeadline;
        public Button AddCupListBtn;
        public Button DeleteCupListBtn;
        public Button AddBlackListBtn;
        public Button ViewBlackListBtn;
        public Button ReleaseCupBtn;
        public Button IniviteRegistBtn;

        public Button PreEndRegisterBtn;
        public Text PreEndRegisterBtnDes;
        //public Text CompetitionBonus;
        //public URichText CompetitionAward;
        //public Text ViewKinBtnDes;
        public Text ReleaseStateDesc;

        public Text ReturnBtnDes;
        public InputField SearchInfo;
        public Text TotalBonusDes;
        public Text TotalBonusCount;
        public Text TeamPrizeDes;
        public Text PersonalPrizeDes;
        public Text PrizeTip;
        public RectTransform DetailPrizeFrame;
        public GameObject DetailPrizeTemplate;

        private SingleRegistKinInfo kinlist;
        private long nLegendCupID;
        private int nSelfKinID;
        private int nSelfPDBID;
        private bool bIsCupCreater;
        private bool bIsSelfIn;
        private bool bIsRegistState;
        private bool bIsOnline;
        private bool bIsKinShaikh;
        private int nDeadline;
        private int nDeadlineHour;
        private int nDeadlineMin;
        private string strHour;
        private string strMinute;
        private const int SinglePageKinMaxCount = 13;
        private const int SinglePagePrizeMaxCount = 6;

        // 标题光效
        public GameObject AttachPoint1;
        public GameObject AttachPoint2;
        private UEffectParamBase param1;
        private UEffectParamBase param2;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            //SysTitleDes.text = ULocalizationService.Instance.Get("UIView", "Common", "OpenMatchSystem");
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "Title");
            KinIndexDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinIndex");
            KinNameDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinName");
            KinLevelDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinLevel");
            KinCupResultDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinCupResult");
            LegendCupRegistList.UserItemCount = SinglePageKinMaxCount;

            //KinMemberDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMember");
            //KinMemberLevelDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMemberLevel");
            //KinMemberRankDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMemberRank");
            //KinMemberTemplate.SetActive(false);

            CupRegistTip.text = "<a href=CupRegistTip color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "CupRegistTip") + "</a>";
            CupRegistTip.onHrefClick.AddListener(OnClickCupRegistTipRT);
            RegistKinNumDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "RegistKinNum");
            AddCupListBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "AddCupListBtn");
            DeleteCupListBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "DeleteCupListBtn");
            AddBlackListBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "AddBlacklistBtn");
            ViewBlackListBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ViewBlacklistBtn");
            ReleaseCupBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ReleaseCupBtn");
            strHour = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "Hour");
            strMinute = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "Minute");
            ReleaseStateDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ReleaseStateDesc");
            PreEndRegisterBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "PreEndRegisterBtn");
            IniviteRegistBtnDes.text = "邀请报名";

            //CompetitionBonus.text = ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionBonus") + ":-";
            //CompetitionAward.text = "<a href=CompetitionAward color=38db47>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionAward") + "</a>";
            //CompetitionAward.onHrefClick.AddListener(OnClickCompetitionAwardRT);
            //ViewKinBtnDes.text = ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinView");

            ReturnBtnDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");
            SearchInfo.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "LegendCupList", "SearchPlaceHolder");
            TotalBonusDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Bonus");
            TeamPrizeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "TeamPrize");
            PersonalPrizeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "PersonalPrize");
            string strTip = ULocalizationService.Instance.Get("UIView", "LegendCupPrize", "PrizeTip");
            UBB.toHtml(ref strTip);
            PrizeTip.text = strTip;

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            param1 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01, _tfAttachParent: AttachPoint1.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param1);
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }
            param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02, _tfAttachParent: AttachPoint2.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);

            return true;
        }

        public void OnRecvKinList(long nCupID)
        {
            kinlist = LogicDataCenter.legendCupDataManager.GetSingleRegistKinInfo(nCupID);
            if (kinlist == null)
                return;

            nLegendCupID = nCupID;
            nSelfKinID = LogicDataCenter.kinDataManager.KinBaseData.nKinID;

            // 控制按钮状态
            if (EntityFactory.MainHeroView != null)
            {
                nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            SysTitleDes.text = cupBaseData.szCupName;
            CreaterDes.gameObject.SetActive(!String.IsNullOrEmpty(cupBaseData.szCreateActorName));
            CreaterDes.text = string.Format("举办人：{0}", cupBaseData.szCreateActorName);
            bIsCupCreater = (cupBaseData.nCreaterPDBID == nSelfPDBID);
            bIsSelfIn = (cupBaseData.nIsSelfIn > 0);
            bIsOnline = (cupBaseData.nIsOnline > 0);
            bIsRegistState = (kinlist.headInfo.nRegistLeftTime > 0);
            bIsKinShaikh = (LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID == nSelfPDBID);
            
            RegistKinNum.text = kinlist.headInfo.nCurKinCount + "/" + kinlist.headInfo.nMaxKinNum;
            if (bIsRegistState)
            {
                // 设置报名阶段按钮、倒计时
                SetRegistStateButtons();
            }
            else
            {
                // 设置确认比赛名单阶段按钮、倒计时
                SetReleaseStateButtons();
            }

            LegendCupRegistList.onRightClick.RemoveListener(OnSelectCupItem);
            LegendCupRegistList.OnSelect.RemoveListener(OnSelectCupItem);
            LegendCupRegistList.DataSource.Clear();
            
            ObservableList<UListItemLegendCupRegist> dataSource = new ObservableList<UListItemLegendCupRegist>();
            int nIndex = 1;
            foreach (cmd_legendcup_recv_regist_kinlistnode node in kinlist.nodeInfo)
            {
                UListItemLegendCupRegist item = new UListItemLegendCupRegist();
                item.nIndex = nIndex;
                item.data = node;
                dataSource.Add(item);
                nIndex++;
            }
            m_AllDataSource = new ObservableList<UListItemLegendCupRegist>(dataSource);

            LegendCupRegistList.DataSource = dataSource;
            LegendCupRegistList.OnSelect.AddListener(OnSelectCupItem);
            LegendCupRegistList.onRightClick.AddListener(OnSelectCupItem);

            //// 默认选择第一条
            //ClearKinMemberFrame();
            //if (dataSource.Count > 0)
            //{
            //    LegendCupRegistList.Select(0);
            //}

            TotalBonusCount.text = cupBaseData.nCompetitionBonus.ToString();
            // 清空DetailPrizeFrame
            int nCount = DetailPrizeFrame.childCount;
            for (int j = nCount - 1; j >= 0; j--)
            {
                Transform childTrans = DetailPrizeFrame.GetChild(j);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(cupBaseData.nCupConfigID);
            if (cupConfig == null)
                return;
            List<SSchemeLegendCupPrizeConfig> prizeConfigList = LegendCupConfig.Instance.GetCupPrizeConfig(cupConfig.nPrizeConfigID);
            if (prizeConfigList == null)
                return;
            DetailPrizeTemplate.SetActive(true);
            for (int i = 0; i < prizeConfigList.Count; i++)
            {
                GameObject detailPrizeObj = ResNode.InstantiateRes(DetailPrizeTemplate);
                if (!detailPrizeObj)
                    return;

                RectTransform rectTrans = detailPrizeObj.transform as RectTransform;
                rectTrans.SetParent(DetailPrizeFrame, false);

                LegendCupDetailPrizeItem detailPrizeItems = detailPrizeObj.GetComponent<LegendCupDetailPrizeItem>();
                if (!detailPrizeItems)
                    return;

                detailPrizeItems.SetData(prizeConfigList[i], cupBaseData.nCompetitionBonus);
            }

            if (prizeConfigList.Count < SinglePagePrizeMaxCount)
            {
                for (int j = prizeConfigList.Count; j < SinglePagePrizeMaxCount; ++j)
                {
                    GameObject detailPrizeObj = ResNode.InstantiateRes(DetailPrizeTemplate);
                    if (!detailPrizeObj)
                        return;

                    RectTransform rectTrans = detailPrizeObj.transform as RectTransform;
                    rectTrans.SetParent(DetailPrizeFrame, false);

                    LegendCupDetailPrizeItem tempItems = detailPrizeObj.GetComponent<LegendCupDetailPrizeItem>();
                    if (!tempItems)
                        return;

                    tempItems.ClearData();
                }
            }

            DetailPrizeTemplate.SetActive(false);

            CupConfigName.text = cupConfig.sCupConfigName;
            LockIconTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupList", "LockIconTip"));
            OnlineIconTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupList", "OnlineIconTip"));
            LockIcon.gameObject.SetActive(cupBaseData.nIsLock > 0);
            OnlineIcon.gameObject.SetActive(cupBaseData.nIsOnline > 0);
        }

        public void OnTimer(int nTimerID)
        {
            if (nDeadlineMin > 0)
            {
                nDeadlineMin--;
                RegistDeadline.text = nDeadlineHour + strHour + nDeadlineMin + strMinute;

                if (nDeadlineMin == 0 && nDeadlineHour == 0)
                {
                    TimerManager.KillTimer(this, 1);
                    // 报名阶段结束还要启动确认阶段定时器；确认阶段结束则直接结束
                    if (bIsRegistState)
                    {
                        bIsRegistState = false;
                        SetReleaseStateButtons();
                    }
                }
            }
            else
            {
                nDeadlineHour--;
                nDeadlineMin = 59;
                RegistDeadline.text = nDeadlineHour + strHour + nDeadlineMin + strMinute;
            }
        }

        public void OnSelectCupItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupRegistList.DataSource[index];
            m_CurSelectComponent = item as UListComponentLegendCupRegist;

            LegendCupRegistList.ForEachComponent(DealEachComponent);
            m_CurSelectComponent.SelectIcon.gameObject.SetActive(true);
            //cmd_legendcup_req_regist_memberlist reqData = new cmd_legendcup_req_regist_memberlist();
            //reqData.nLegendCupID = nLegendCupID;
            //reqData.nKinID = m_CurSelectItem.data.nKinID;
            //ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_regist_memberlist>(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST, ref reqData);
        }

        private void DealEachComponent(ListViewItem item)
        {
            (item as UListComponentLegendCupRegist).SelectIcon.gameObject.SetActive(false);
        }

        public void OnPreOffLineEndRegisterClick()
        {
            long nCurLegendCupID = nLegendCupID;
            UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "PreEndRegisterTitle"),
               message: ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "PreEndRegisterContex"),
               buttons: new UIWidgets.DialogActions(){ 
                                                        { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "PreEndRegisterConfirm"), () => { OnConfirmBtnClick(nCurLegendCupID); return true; } }, 
                                                        { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "PreEndRegisterCancel"), null } 
                                                     });

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

            
        }

        private bool OnConfirmBtnClick(long nLegendCupID)
        {
            cmd_legendcup_offline_pre_end_register addcup = new cmd_legendcup_offline_pre_end_register();
            addcup.nLegendCupID = nLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_offline_pre_end_register>(GameLogicDef.GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER, ref addcup);
            return true;
        }
        
        //public void OnRecvKinMemberList(int nKinID)
        //{
        //    SingleRegistKinMemberInfo memberlist = LogicDataCenter.legendCupDataManager.GetSingleRegistKinMemberInfo(nKinID);
        //    if (memberlist == null)
        //        return;

        //    ClearKinMemberFrame();

        //    m_kinMemberItems = new LegendCupRegistKinMemberItem[memberlist.headInfo.nMemberCount];
        //    float fItemOffsetY = -36f;
        //    for (int i = 0; i < memberlist.headInfo.nMemberCount; ++i)
        //    {
        //        GameObject memberItemObj = ResNode.InstantiateRes(KinMemberTemplate);
        //        if (!memberItemObj)
        //            return;

        //        m_kinMemberItems[i] = memberItemObj.GetComponent<LegendCupRegistKinMemberItem>();
        //        if (!m_kinMemberItems[i])
        //            return;
        //        m_kinMemberItems[i].SetData(memberlist.nodeInfo[i]);

        //        memberItemObj.name = "MemberItem" + (i + 1);
        //        RectTransform rectTrans = memberItemObj.transform as RectTransform;
        //        rectTrans.SetParent(KinMemberListFrame, false);
        //        rectTrans.anchoredPosition = new Vector2(0f, fItemOffsetY);
        //        fItemOffsetY -= 72f;
        //    }
        //    KinMemberTemplate.SetActive(false);
        //}

        //public void OnClickCompetitionAwardRT(string eventName)
        //{
        //    if (eventName == "CompetitionAward")
        //    {
        //        LegendCupIDData uiData = new LegendCupIDData();
        //        uiData.nLegendCupID = nLegendCupID;
        //        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW, uiData);
        //    }
        //}

        public void OnClickCupRegistTipRT(string eventName)
        {
            if (eventName == "CupRegistTip")
            {
                cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
                if (cupBaseData.nLegendCupID == 0)
                    return;
                    
                UIMsgCmdData uiCupTipData = null;
                if (cupBaseData.nIsOnline > 0)
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OnLine_CupRegist, "", IntPtr.Zero, 0);
                else
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OffLine_CupRegist, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, uiCupTipData);
            }
        }

        public void OnClickAddCupListBtn()
        {
            if (LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID != nSelfPDBID)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SHAIKH_CANT_REQUEST);
                return;
            }

            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "AddCupTitle"),
                        message: String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "AddCupContent"), cupBaseData.nRegistGold),
                        buttons: new UIWidgets.DialogActions() 
                        { { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ConfirmBtn"), OnConfirmAddCup }, 
                        { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "CancelBtn"), null } }
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool OnConfirmAddCup()
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return false;

            if (cupBaseData.nIsLock > 0)
            {
                LegendCupIDData uiData = new LegendCupIDData();
                uiData.nLegendCupID = nLegendCupID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PASSWORD_SHOW, uiData);
            }
            else
            {
                cmd_legendcup_add_regist_kinlist addcup = new cmd_legendcup_add_regist_kinlist();
                addcup.nLegendCupID = nLegendCupID;
                addcup.nKinID = LogicDataCenter.kinDataManager.KinBaseData.nKinID;
                addcup.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                addcup.byCreateCupType = cupBaseData.byCreateCupType;
                addcup.szPassword = "";
                ViewEventHelper.Instance.SendCommand<cmd_legendcup_add_regist_kinlist>(GameLogicDef.GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST, ref addcup);
            }

            return true;
        }

        public void OnClickDeleteCupListBtn()
        {
            if (LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID != nSelfPDBID)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SHAIKH_CANT_REQUEST);
                return;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "QuitCupTitle"),
                                    message: ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "QuitCupContent"),
                                    buttons: new UIWidgets.DialogActions() 
                                    { { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ConfirmBtn"), OnConfirmQuitCup }, 
                                    { ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "CancelBtn"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool OnConfirmQuitCup()
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return false;

            cmd_legendcup_quit_regist_kinlist deletecup = new cmd_legendcup_quit_regist_kinlist();
            deletecup.nLegendCupID = nLegendCupID;
            deletecup.nKinID = LogicDataCenter.kinDataManager.KinBaseData.nKinID;
            deletecup.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            deletecup.byCreateCupType = cupBaseData.byCreateCupType;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_quit_regist_kinlist>(GameLogicDef.GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST, ref deletecup);

            return true;
        }

        public void OnClickAddBlackListBtn()
        {
            if (m_CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_SELECT_RIGHT_KINID);
                return;
            }
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            cmd_legendcup_add_regist_blacklist addblacklist = new cmd_legendcup_add_regist_blacklist();
            addblacklist.nLegendCupID = nLegendCupID;
            addblacklist.nKinID = m_CurSelectItem.data.nKinID;
            addblacklist.nOperatorID = nSelfPDBID;
            addblacklist.byCreateCupType = cupBaseData.byCreateCupType;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_add_regist_blacklist>(GameLogicDef.GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST, ref addblacklist);
        }

        public void OnClickViewBlackListBtn()
        {
            LegendCupIDData uiData = new LegendCupIDData();
            uiData.nLegendCupID = nLegendCupID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_SHOW, uiData);
        }

        public void OnClickReleaseCupBtn()
        {
            if (bIsRegistState || bIsOnline)
                return;

            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;
            cmd_legendcup_release_single_cup cupdata = new cmd_legendcup_release_single_cup();
            cupdata.nLegendCupID = nLegendCupID;
            cupdata.nOperatorID = nSelfPDBID;
            cupdata.byCreateCupType = cupBaseData.byCreateCupType;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_release_single_cup>(GameLogicDef.GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP, ref cupdata);
        }

        public void OnClickCloseBtn()
        {
            TimerManager.KillTimer(this, 1);

            cmd_legendcup_quit_observe observeData = new cmd_legendcup_quit_observe();
            observeData.nLegendCupID = nLegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_quit_observe>(GameLogicDef.GVIEWCMD_LEGENDCUP_QUITOBSERVE, ref observeData);

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }

            m_wnd.UnloadView();
        }

        public void OnClickSearchBtn()
        {
            ClearSelectIcon();

            LegendCupRegistList.DataSource.BeginUpdate();
            LegendCupRegistList.DataSource.Clear();

            ObservableList<UListItemLegendCupRegist> allDataSource = new ObservableList<UListItemLegendCupRegist>(m_AllDataSource);
            if (String.IsNullOrEmpty(SearchInfo.text))
            {
                // 显示所有数据
                LegendCupRegistList.DataSource = allDataSource;
            }
            else
            {
                // 显示过滤数据
                IEnumerable<UListItemLegendCupRegist> ListQuery = from item in allDataSource
                                                            where item.data.szKinName.Contains(SearchInfo.text)
                                                            select item;
                ObservableList<UListItemLegendCupRegist> dataSource = new ObservableList<UListItemLegendCupRegist>();
                foreach (UListItemLegendCupRegist Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                LegendCupRegistList.DataSource = dataSource;
            }
            LegendCupRegistList.DataSource.EndUpdate();
        }

        //public void OnClickViewKinBtn()
        //{
        //    if (m_CurSelectItem == null || m_CurSelectItem.data.nKinID <= 0)
        //        return;

        //    UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, m_CurSelectItem.data.nKinID, "", IntPtr.Zero, 0);
        //    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KININFO_SHOW, uiData);
        //}

        private void SetRegistStateButtons()
        {
            // 按钮
            //AddCupListBtn.gameObject.SetActive((!bIsSelfIn) && bIsKinShaikh);
            //DeleteCupListBtn.gameObject.SetActive(bIsSelfIn && bIsKinShaikh);
            AddCupListBtn.gameObject.SetActive(!bIsSelfIn);
            DeleteCupListBtn.gameObject.SetActive(bIsSelfIn);

            if (bIsCupCreater)
            {
                AddBlackListBtn.gameObject.SetActive(true);
                ViewBlackListBtn.gameObject.SetActive(true);
                ReleaseCupBtn.gameObject.SetActive(!bIsRegistState && !bIsOnline);
                // 报名状态而且线下比赛
                PreEndRegisterBtn.gameObject.SetActive(bIsRegistState && !bIsOnline);
                IniviteRegistBtn.gameObject.SetActive(true);
            }
            else
            {
                AddBlackListBtn.gameObject.SetActive(false);
                ViewBlackListBtn.gameObject.SetActive(false);
                ReleaseCupBtn.gameObject.SetActive(false);
                PreEndRegisterBtn.gameObject.SetActive(false);
                IniviteRegistBtn.gameObject.SetActive(false);
            }

            // 时间
            RegistDeadlineDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "RegistDeadline");
            nDeadline = kinlist.headInfo.nRegistLeftTime / 60;
            nDeadline = kinlist.headInfo.nRegistLeftTime % 60 > 0 ? (nDeadline + 1) : nDeadline;
            nDeadlineHour = nDeadline / 60;
            nDeadlineMin = nDeadline % 60;
            RegistDeadline.text = nDeadlineHour + strHour + nDeadlineMin + strMinute;
            TimerManager.KillTimer(this, 1);
            TimerManager.SetTimer(this, 1, 60);

            // 描述
            ReleaseStateDesc.gameObject.SetActive(false);
        }

        private void SetReleaseStateButtons()
        {
            // 按钮
            AddCupListBtn.gameObject.SetActive(false);
            DeleteCupListBtn.gameObject.SetActive(false);
            IniviteRegistBtn.gameObject.SetActive(false);

            if (bIsCupCreater)
            {
                AddBlackListBtn.gameObject.SetActive(true);
                ViewBlackListBtn.gameObject.SetActive(true);
                ReleaseCupBtn.gameObject.SetActive(!bIsRegistState && !bIsOnline);
                PreEndRegisterBtn.gameObject.SetActive(bIsRegistState && !bIsOnline);
            }
            else
            {
                AddBlackListBtn.gameObject.SetActive(false);
                ViewBlackListBtn.gameObject.SetActive(false);
                ReleaseCupBtn.gameObject.SetActive(false);
                PreEndRegisterBtn.gameObject.SetActive(false);
            }

            // 时间
            RegistDeadlineDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "ConfirmKinLeftTime");
            nDeadline = kinlist.headInfo.nConfirmKinLeftTime / 60;
            nDeadline = kinlist.headInfo.nConfirmKinLeftTime % 60 > 0 ? (nDeadline + 1) : nDeadline;
            if (nDeadline > 0)
            {
                nDeadlineHour = nDeadline / 60;
                nDeadlineMin = nDeadline % 60;
                RegistDeadline.text = nDeadlineHour + strHour + nDeadlineMin + strMinute;
                TimerManager.KillTimer(this, 1);
                TimerManager.SetTimer(this, 1, 60);
            }
            else
            {
                RegistDeadline.text = "0" + strHour + "0" + strMinute;
            }

            // 描述
            ReleaseStateDesc.gameObject.SetActive(!bIsRegistState && !bIsOnline);
        }

        private void ClearSelectIcon()
        {
            if (m_CurSelectComponent != null)
            {
                //m_CurSelectComponent.DefaultColoring();
                m_CurSelectComponent.BgIcon.gameObject.SetActive(false);
            }

            m_CurSelectIndex = -1;
            m_CurSelectItem = null;
            m_CurSelectComponent = null;
        }

        //private void ClearKinMemberFrame()
        //{
        //    // 清空KinMemberFrame
        //    int nCount = KinMemberListFrame.childCount;
        //    for (int i = nCount - 1; i >= 0; --i)
        //    {
        //        Transform childTrans = KinMemberListFrame.GetChild(i);
        //        if (childTrans)
        //        {
        //            var temp = childTrans.gameObject;
        //            ResNode.DestroyRes(ref temp);
        //        }
        //    }
        //}

        public void OnClickInviteLegendCupBtn()
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            string strContent = "";
            if (cupBaseData.nRegistGold > 0)
            {
                strContent = String.Format("杯赛【{0}】({1}/{2})正在火热进行中，报名费用金币{3}，{4}[clickinvitelegendcup color=255+255+255 cupid={5}]点击报名[/clickinvitelegendcup]",
                            cupBaseData.szCupName,
                            cupBaseData.nCurrentKinNum,
                            cupBaseData.nMaxKinNum,
                            cupBaseData.nRegistGold,
                            ChatMessageManager.ChatItemFlag,
                            cupBaseData.nLegendCupID
                            );
            }
            else
            {
                strContent = String.Format("杯赛【{0}】({1}/{2})正在火热进行中，{3}[clickinvitelegendcup color=255+255+255 cupid={4}]点击报名[/clickinvitelegendcup]",
                            cupBaseData.szCupName,
                            cupBaseData.nCurrentKinNum,
                            cupBaseData.nMaxKinNum,
                            ChatMessageManager.ChatItemFlag,
                            cupBaseData.nLegendCupID
                            );
            }

            EMChatChannelType channelType = EMChatChannelType.CHAT_CHANNEL_WORLD;
            if (cupBaseData.byCreateCupType == (int)ELegendCupType.emCupType_Clan)
            {
                channelType = EMChatChannelType.CHAT_CHANNEL_CLAN;
            }
            UIUtil.SendChatMessage(channelType, strContent);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_TOGGLE, null);
        }

    }
}
