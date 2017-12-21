using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using UnityEngine.EventSystems;
using USpeedUI.UWidgets;
using USpeedUI.UExtensions;
using Mentorship_ManagerDef;
using ASpeedGame.Data.MentorshipConfig;
using USpeedUI.UEffect;
using UIWidgets;
using Data.ActorPrizeConfig;
using U3D_Render.Common;

namespace USpeedUI.Social
{
    // 师徒值显示类型
    public enum EMMentorshipValType
    {
        ShipValType_Master = 0,
        ShipValType_Prentice,
    }

    public class MentorshipPersonWnd : UIBaseSubWnd<MentorshipPersonWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)MentorshipSubWndType.WND_MENTORSHIP_PERSON;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewMentorshipPerson";
        }

        // 初始化
        public override bool Init()
        {
            SetVisible(false);

            return base.Init();
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                RequestMentorshipPersonInfo();
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvPersonBaseData();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvMasterList();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEFPRENTICELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvFormalPrenticeList();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEIPRENTICELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvInternPrenticeList();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERFPRENTICELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvMasterFormalPrenticeList();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEMASTERIPRENTICELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvMasterInternPrenticeList();
                    }
                    break;
                default:
                    break;
            }
        }

        public void RequestMentorshipPersonInfo()
        {
            cmd_mentorship_request_persondata cmdData = new cmd_mentorship_request_persondata();
            cmdData.nPDBID = 0;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_persondata>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA, ref cmdData);
        }
    }

    public class MasterListItem : MonoBehaviour, IPointerDownHandler
    {
        public Image frameIcon;
        public Image fMasterIcon;
        public Text name;
        public Text posDesc;

        private int m_index;
        private int m_MentorshipID;
        public int MentorshipID { get { return m_MentorshipID; } }
        private int m_DesPDBID;
        public int DesPDBID { get { return m_DesPDBID; } }
        private string m_MasterName;
        public string MasterName { get { return m_MasterName; } }
        private MentorshipPersonWndView wndView;

        private UPopupMenuHandle popupMenuHandle;

        public void SetWndView(MentorshipPersonWndView view, int index)
        {
            wndView = view;
            m_index = index;
        }

        public void Clear()
        {
            fMasterIcon.enabled = false;
            name.text = "";
            m_MentorshipID = 0;
            m_DesPDBID = 0;
            m_MasterName = "";
            posDesc.text = "";

            if (popupMenuHandle != null)
                popupMenuHandle.HandlePopupAction = null;

            if (m_index == 0)
            {
                string szLeaveLineDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "LeaveLineNameDesc");
                szLeaveLineDesc = szLeaveLineDesc.Replace("{1}", "暂无导师");
                name.text = szLeaveLineDesc;  
            }
        }

        public void SetData(cmd_mentorship_personshipinfo data)
        {
            m_MentorshipID = data.nMentorshipID;
            m_DesPDBID = data.nPDBID;
            m_MasterName = data.szName;

            if (data.nRelationType == (int)EMMentorshipType.emMentorshipType_Formal)
            {
                fMasterIcon.enabled = true;
                fMasterIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MentorshipMaster, "MasterIcon_1", WndID.WND_ID_SOCIALCONTACT);

                posDesc.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "FormalPosDesc");
            }
            else
            {
                fMasterIcon.enabled = false;
                posDesc.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "InnerPosDesc");
            }

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction popupAction = new UPopupAction();
            popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(m_DesPDBID));

            if (data.nIsOnline == 0)
            {
                string szLeaveLineDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "LeaveLineNameDesc");
                szLeaveLineDesc = szLeaveLineDesc.Replace("{1}", data.szName);
                name.text = szLeaveLineDesc;                
            }
            else
            {
                name.text = data.szName;
                popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(m_MasterName));
                if (data.nRelationType != (int)EMMentorshipType.emMentorshipType_Formal && !LogicDataCenter.mentorshipDataManager.IsFormalMasterFull())
                {
                    popupAction.Add(UPopupItemDataApplyFormalPrentice.GetDescString(), new UPopupItemDataApplyFormalPrentice(m_DesPDBID));
                }
            }

            if (data.nSeverPDBID == 0 || data.nPDBID == data.nSeverPDBID)
            {
                popupAction.Add(UPopupItemDataDismissMentorship.GetDescString(), new UPopupItemDataDismissMentorship(m_MentorshipID)); 
            }

            popupMenuHandle.HandlePopupAction = popupAction;

            gameObject.SetActive(true);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (wndView == null)
                return;

            if (m_MentorshipID > 0)
            {
                wndView.OnSelectMaster(m_MentorshipID, m_MasterName, this);
            }
            else
            {
                UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, (int)MentorshipFrameBtnType.BTN_MASTER, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, uiCmdData);
            }
        }

        private void OnDismissMentorhsip()
        {
            if (m_MentorshipID > 0)
            {
                GameLogic.cmd_mentorship_request_dismiss cmdData = new GameLogic.cmd_mentorship_request_dismiss();
                cmdData.nMentorshipID = m_MentorshipID;
                ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_dismiss>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTDISMISS, ref cmdData);
            }
        }
    }

    public class PrenticeListItem : MonoBehaviour, IPointerDownHandler
    {
        public Image headIcon;
        public Text name;

        private int m_MentorshipID;
        public int MentorshipID { get { return m_MentorshipID; } }
        private int m_DesPDBID;
        public int DesPDBID { get { return m_DesPDBID; } }
        private string m_PrenticeName;
        public string PrenticeName { get { return m_PrenticeName; } }
        private bool m_bSelfPrentice;
        private MentorshipPersonWndView wndView;
        private UPopupMenuHandle popupMenuHandle;
        private UEffectParamBase headIconMatParam;

        public void SetWndView(MentorshipPersonWndView view)
        {
            wndView = view;
            headIconMatParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Gray);
            (headIconMatParam as UEffectMaterialParam).GraphicsTarget = headIcon;
        }

        public void Clear()
        {
            m_MentorshipID = 0;
            m_DesPDBID = 0;
            m_PrenticeName = "";
            m_bSelfPrentice = false;
            if (popupMenuHandle != null)
                popupMenuHandle.HandlePopupAction = null;

            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref headIconMatParam);
            string szLeaveLineDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "LeaveLineNameDesc");
            szLeaveLineDesc = szLeaveLineDesc.Replace("{1}", "暂无");
            name.text = szLeaveLineDesc;
        }

        public void SetData(int mentorshipID, int nPDBID, string szName, int nSeverPDBID, int nOnline, int nRelationType, bool bSelfPrentice)
        {
            m_MentorshipID = mentorshipID;
            m_DesPDBID = nPDBID;
            m_PrenticeName = szName;
            m_bSelfPrentice = bSelfPrentice;

            headIcon.enabled = true;
            headIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_SOCIALCONTACT, 1, 1);

            if (popupMenuHandle == null)
                popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

            UPopupAction popupAction = new UPopupAction();
            popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(m_DesPDBID));

            if (nOnline == 0)
            {
                string szLeaveLineDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "LeaveLineNameDesc");
                szLeaveLineDesc = szLeaveLineDesc.Replace("{1}", szName);
                name.text = szLeaveLineDesc;
            }
            else
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectMaterial, ref headIconMatParam);
                name.text = szName;
                popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(m_PrenticeName));

                if (m_bSelfPrentice && nRelationType != (int)EMMentorshipType.emMentorshipType_Formal && !LogicDataCenter.mentorshipDataManager.IsFormalPrenticeFull())
                {
                    popupAction.Add(UPopupItemDataPromotePrentice.GetDescString(), new UPopupItemDataPromotePrentice(m_MentorshipID, m_DesPDBID));
                }
            }

            if (m_bSelfPrentice && (nSeverPDBID == 0 || nPDBID == nSeverPDBID))
            {
                popupAction.Add(UPopupItemDataDismissMentorship.GetDescString(), new UPopupItemDataDismissMentorship(m_MentorshipID));
            }

            popupMenuHandle.HandlePopupAction = popupAction;

            gameObject.SetActive(true);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (wndView == null)
                return;

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            if (m_MentorshipID <= 0)
            {
                int nSelfLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

                if (nSelfLevel >= MentorshipConfig.Instance.tMentorshipConfig.nMasterNeedLevel)
                {
                    UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, (int)MentorshipFrameBtnType.BTN_PRENTICE, "", IntPtr.Zero, 0);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MENTORSHIP_OPNESUBPAGE, uiCmdData);
                }
                else
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MENTORSHIP_MASTER_NEED_LEVEL);
                }
            }
        }

        private void OnDismissMentorhsip()
        {
            if (m_bSelfPrentice && m_MentorshipID > 0)
            {
                GameLogic.cmd_mentorship_request_dismiss cmdData = new GameLogic.cmd_mentorship_request_dismiss();
                cmdData.nMentorshipID = m_MentorshipID;
                ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_dismiss>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTDISMISS, ref cmdData);
            }
        }

        private void OnPromotePrentice()
        {
            if (m_bSelfPrentice && m_MentorshipID > 0 && m_DesPDBID > 0)
            {
                GameLogic.cmd_mentorship_request_promote cmdData = new GameLogic.cmd_mentorship_request_promote();
                cmdData.nMentorshipID = m_MentorshipID;
                cmdData.nDesPDBID = m_DesPDBID;
                ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_promote>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTPROMOTE, ref cmdData);
            }
        }
    }

    public class MentorshipPersonWndView : UIBaseSubWndView
    {
        public MasterListItem[] listMasterItem;
        public Image masterItemSelectImage;

        public PrenticeListItem[] listFPrenticeItem;

        public PrenticeListItem[] listIPrenticeItem;

        public Text masterTitleText;

        public Text mastserSumShipText;
        public Text prenticeSumShipText;
        public Text masterSumShipVal;
        public Text prenticeSumShipVal;

        public Transform masterSumShipTrans;
        public Transform prenticeSumShipTrans;

        public Text awardTitleText;
        //public Text awardDestText;
        public Text prenticeFWinTitleText;
        public Text prenticeFWinValText;
        public Text prenticeFWinDescText;

        public Text masterFWinTitleText;
        public Text masterFWinValText;
        public Text masterFWinDescText;

        public Text fPrenticeTitleText;
        public Text fMasterNameText;

        public Text iPrenticeTitleText;
        public Text iMasterNameText;

        public Image masterFWinPrizeImg;
        public Image prenticeFWinPrizeImg;
        public Button masterFWinPrizeBtn;
        public Button prenticeFWinPrizeBtn;
        public Text masterFWinPrizeBtnDes;
        public Text prenticeFWinPrizeBtnDes;
        public Text masterFWinPrizeCount;
        public Text prenticeFWinPrizeCount;
  
        private UTooltipTrigger mFWinAwardTooltipTrigger;
        private UTooltipTrigger pFWinAwardTooltipTrigger;

        private UTooltipTrigger masterShipTooltipTrigger;
        private UTooltipTrigger prenticeShipTooltipTrigger;

        private int m_curSelMasterShipID;
        private string m_curSelMasterName;

        // 初始化
        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            masterTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MasterTitleDesc");
            awardTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "AwardTitleDesc");

            prenticeFWinTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PFWinTitleDesc");
            prenticeFWinDescText.text = "--" + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PFWinAwardDesc");

            masterFWinTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MFWinTitleDesc");
            masterFWinDescText.text = "--" + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MFWinAwardDesc");

            fPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MyFPrenticeTitleDesc");
            iPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MyIPrenticeTitleDesc");

            masterItemSelectImage.enabled = false;

            OnShipValDropdownChanged();

            ShowMentorshipAwardData();

            UpdateBtnState(true);

            m_curSelMasterShipID = 0;
            m_curSelMasterName = "";

            for (int i = 0; i < listMasterItem.Length; ++i)
            {
                listMasterItem[i].SetWndView(this, i);
                listMasterItem[i].Clear();
            }

            for (int i = 0; i < listFPrenticeItem.Length; ++i)
            {
                listFPrenticeItem[i].SetWndView(this);
                listFPrenticeItem[i].Clear();
            }

            for (int i = 0; i < listIPrenticeItem.Length; ++i)
            {
                listIPrenticeItem[i].SetWndView(this);
                listIPrenticeItem[i].Clear();
            }

            return true;
        }

        private void ShowMentorshipAwardData()
        {
            masterFWinPrizeBtnDes.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "RecvFirstPrizeBtnDesc");
            prenticeFWinPrizeBtnDes.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "RecvFirstPrizeBtnDesc");

            if (masterFWinPrizeImg != null && mFWinAwardTooltipTrigger == null)
            {
                mFWinAwardTooltipTrigger = masterFWinPrizeImg.transform.GetComponentInChildren<UTooltipTrigger>(true);
            }

            if (prenticeFWinPrizeImg != null && pFWinAwardTooltipTrigger == null)
            {
                pFWinAwardTooltipTrigger = prenticeFWinPrizeImg.transform.GetComponentInChildren<UTooltipTrigger>(true);
            }

            SSchemeActorPrizeConfig configMaster = new SSchemeActorPrizeConfig();
            configMaster = ActorPrizeConfig.Instance.GetActorPrizeConfig(MentorshipConfig.Instance.tMentorshipConfig.nMFWinPrizeID);
            if (configMaster != null)
            {
                masterFWinPrizeImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_SOCIALCONTACT, configMaster.nPrizeType, configMaster.nPrizeIcon);

                String szMasterPrizeDesc = configMaster.strPrizeDesc;
                UBB.toHtml(ref szMasterPrizeDesc, UBB_FORMAT_TYPE.UGUI);
                mFWinAwardTooltipTrigger.SetText(UTooltipParamName.BodyText, szMasterPrizeDesc);
            }

            SSchemeActorPrizeConfig configPrentice = new SSchemeActorPrizeConfig();
            configPrentice = ActorPrizeConfig.Instance.GetActorPrizeConfig(MentorshipConfig.Instance.tMentorshipConfig.nPFWinPrizeID);
            if (configPrentice != null)
            {
                prenticeFWinPrizeImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_SOCIALCONTACT, configPrentice.nPrizeType, configPrentice.nPrizeIcon);

                String szPrenticePrizeDesc = configPrentice.strPrizeDesc;
                UBB.toHtml(ref szPrenticePrizeDesc, UBB_FORMAT_TYPE.UGUI);
                pFWinAwardTooltipTrigger.SetText(UTooltipParamName.BodyText, szPrenticePrizeDesc);
            }
        }

        private void UpdateBtnState(bool bInit)
        {
            int nMFWinAwardPreCount = (bInit ? 0 : LogicDataCenter.mentorshipDataManager.MentorshipPersonData.nMFWinAwardPreCount);
            masterFWinPrizeCount.text = nMFWinAwardPreCount.ToString();

            bool bBtnVisible = (nMFWinAwardPreCount > 0 ? true : false);

            Color clr = new Color();
            string htmlString = (bBtnVisible ? "#ffffff" : "#bebebe");
            ColorUtility.TryParseHtmlString(htmlString, out clr);
            masterFWinPrizeBtn.interactable = bBtnVisible;
            masterFWinPrizeBtnDes.color = clr;

            int nPFWinAwardPreCount = (bInit ? 0 : LogicDataCenter.mentorshipDataManager.MentorshipPersonData.nPFWinAwardPreCount);
            prenticeFWinPrizeCount.text = nPFWinAwardPreCount.ToString();

            bool bPBtnVisible = (nPFWinAwardPreCount > 0 ? true : false);
            Color clrPrentice = new Color();
            string htmlPrentice = (bPBtnVisible ? "#ffffff" : "#bebebe");
            ColorUtility.TryParseHtmlString(htmlPrentice, out clrPrentice);
            prenticeFWinPrizeBtn.interactable = bPBtnVisible;
            prenticeFWinPrizeBtnDes.color = clrPrentice;
        }

        public void OnShipValDropdownChanged()
        {
            if (masterShipTooltipTrigger == null)
            {
                masterShipTooltipTrigger = masterSumShipTrans.transform.GetComponentInChildren<UTooltipTrigger>(true);
            }

            if (prenticeShipTooltipTrigger == null)
            {
                prenticeShipTooltipTrigger = prenticeSumShipTrans.transform.GetComponentInChildren<UTooltipTrigger>(true);
            }

            String szMasterDesc = "";
            String szPrenticeDesc = "";

            int masterSumShip = 0;
            int prenticeSumShip = 0;
            cmd_mentorship_person_baseinfo personData = LogicDataCenter.mentorshipDataManager.MentorshipPersonData;
            if (personData.nPDBID > 0)
            {
                masterSumShip = personData.nMSumShipVal;
                prenticeSumShip = personData.nPSumShipVal;

                szMasterDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MWeekShipTitleDesc") + "：" + personData.nMWeekShipVal.ToString() + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MLastWeekShipTitleDesc") + "：" + personData.nMLastWeekShipVal.ToString() + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MLastWeekOrderDesc") + "：" + personData.nMLastSVOrder.ToString();

                szPrenticeDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PWeekShipTitleDesc") + "：" + personData.nPWeekShipVal.ToString() + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PLastWeekShipTitleDesc") + "：" + personData.nPLastWeekShipVal.ToString() + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PLastWeekOrderDesc") + "：" + personData.nPLastSVOrder.ToString();
            }
            else
            {
                szMasterDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MWeekShipTitleDesc") + "：0" + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MLastWeekShipTitleDesc") + "：0" + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MLastWeekOrderDesc") + "：0";

                szPrenticeDesc = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PWeekShipTitleDesc") + "：0" + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PLastWeekShipTitleDesc") + "：0" + "\n"
                    + ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PLastWeekOrderDesc") + "：0";
            }

            mastserSumShipText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MSumShipTitleDesc");
            prenticeSumShipText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PSumShipTitleDesc");
            masterSumShipVal.text = masterSumShip.ToString();
            prenticeSumShipVal.text = prenticeSumShip.ToString();

            UBB.toHtml(ref szMasterDesc, UBB_FORMAT_TYPE.UGUI);
            masterShipTooltipTrigger.SetText(UTooltipParamName.BodyText, szMasterDesc);

            UBB.toHtml(ref szPrenticeDesc, UBB_FORMAT_TYPE.UGUI);
            prenticeShipTooltipTrigger.SetText(UTooltipParamName.BodyText, szPrenticeDesc); 
        }

        public void OnRecvPersonBaseData()
        {
            cmd_mentorship_person_baseinfo personData = LogicDataCenter.mentorshipDataManager.MentorshipPersonData;
            if (personData.nPDBID <= 0)
                return;

            OnShipValDropdownChanged();

            UpdateBtnState(false);

            int nPFWinCount = MentorshipConfig.Instance.tMentorshipConfig.nPFWinAwardCount;
            int nMFWinCount = MentorshipConfig.Instance.tMentorshipConfig.nMFWinAwardCount;

            string szPFWinDes = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "PFWinAwardValDesc");
            szPFWinDes = szPFWinDes.Replace("{0}", personData.nPFWinAwardCount.ToString() );
            szPFWinDes = szPFWinDes.Replace("{1}", nPFWinCount.ToString());
            prenticeFWinValText.text = szPFWinDes;

            string szMFWinDes = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MFWinAwardValDesc");
            szMFWinDes = szMFWinDes.Replace("{0}", personData.nMFWinAwardCount.ToString());
            szMFWinDes = szMFWinDes.Replace("{1}", nMFWinCount.ToString());
            masterFWinValText.text = szMFWinDes;
        }

        public void OnRecvMasterList()
        {
            foreach (MasterListItem item in listMasterItem)
            {
                item.Clear();
            }

            OnSelectMaster(0, "", null);
            int i = 0;
            List<cmd_mentorship_personshipinfo> tmpMasterList = LogicDataCenter.mentorshipDataManager.MasterList;
            foreach (cmd_mentorship_personshipinfo item in tmpMasterList)
            {
                listMasterItem[i].SetData(item);
                i += 1;
            }
        }

        public void OnRecvFormalPrenticeList()
        {
            fPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MyFPrenticeTitleDesc");
            fMasterNameText.text = "";

            foreach (PrenticeListItem item in listFPrenticeItem)
            {
                item.Clear();
            }

            int i = 0;
            List<cmd_mentorship_personshipinfo> tmpFPrenticeList = LogicDataCenter.mentorshipDataManager.FPrenticeList;
            foreach (cmd_mentorship_personshipinfo item in tmpFPrenticeList)
            {
                listFPrenticeItem[i].SetData(item.nMentorshipID, item.nPDBID, item.szName, item.nSeverPDBID, item.nIsOnline, item.nRelationType, true);
                i += 1;
            }
        }

        public void OnRecvInternPrenticeList()
        {
            iPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "MyIPrenticeTitleDesc");
            iMasterNameText.text = "";
            foreach (PrenticeListItem item in listIPrenticeItem)
            {
                item.Clear();
            }

            int i = 0;
            List<cmd_mentorship_personshipinfo> tmpIPrenticeList = LogicDataCenter.mentorshipDataManager.IPrenticeList;
            foreach (cmd_mentorship_personshipinfo item in tmpIPrenticeList)
            {
                listIPrenticeItem[i].SetData(item.nMentorshipID, item.nPDBID, item.szName, item.nSeverPDBID, item.nIsOnline, item.nRelationType, true);
                i += 1;
            }
        }

        public void OnRecvMasterFormalPrenticeList()
        {
            fPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "FPrenticeTitleDesc");
            fMasterNameText.text = m_curSelMasterName;
            foreach (PrenticeListItem item in listFPrenticeItem)
            {
                item.Clear();
            }

            int i = 0;
            List<cmd_mentorship_prenticeinfo> tmpMasterFPrenticeList = LogicDataCenter.mentorshipDataManager.MasterFPrenticeList;
            foreach (cmd_mentorship_prenticeinfo item in tmpMasterFPrenticeList)
            {
                listFPrenticeItem[i].SetData(0, item.nPDBID, item.szName, 0, item.nIsOnline, item.nRelationType, false);
                i += 1;
            }
        }

        public void OnRecvMasterInternPrenticeList()
        {
            iPrenticeTitleText.text = ULocalizationService.Instance.Get("UIView", "MentorshipPerson", "IPrenticeTitleDesc");
            iMasterNameText.text = m_curSelMasterName;
            foreach (PrenticeListItem item in listIPrenticeItem)
            {
                item.Clear();
            }

            int i = 0;
            List<cmd_mentorship_prenticeinfo> tmpMasterIPrenticeList = LogicDataCenter.mentorshipDataManager.MasterIPrenticeList;
            foreach (cmd_mentorship_prenticeinfo item in tmpMasterIPrenticeList)
            {
                listIPrenticeItem[i].SetData(0, item.nPDBID, item.szName, 0, item.nIsOnline, item.nRelationType, false);
                i += 1;
            }
        }

        public void OnSelectMaster(int nMentorshipID, string szMasterName, MasterListItem selItem)
        {
            m_curSelMasterShipID = nMentorshipID;
            m_curSelMasterName = szMasterName;

            if (selItem != null)
            {
                masterItemSelectImage.rectTransform.SetPosition(selItem.gameObject.transform.position);
                masterItemSelectImage.enabled = true;

                /*
                if (selItem.DesPDBID > 0)
                {
                    cmd_mentorship_prenticelist_head cmdData = new cmd_mentorship_prenticelist_head();
                    cmdData.nPDBID = selItem.DesPDBID;
                    cmdData.nCount = 0;
                    ViewEventHelper.Instance.SendCommand<cmd_mentorship_prenticelist_head>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST, ref cmdData);
                }*/
            }
            else
            {
                masterItemSelectImage.enabled = false;
            }
        }

        public void OnRecvMasterFirstAwardBtnClick()
        {
            if (LogicDataCenter.mentorshipDataManager.MentorshipPersonData.nMFWinAwardPreCount <= 0)
                return;

            cmd_mentorship_request_firstaward cmdData = new cmd_mentorship_request_firstaward();
            cmdData.nPost = (int)EMMentorshipPost.emMentorshipPost_Prentice;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_firstaward>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD, ref cmdData);
        }

        public void OnRecvPrenticeFirstAwardBtnClick()
        {
            if (LogicDataCenter.mentorshipDataManager.MentorshipPersonData.nPFWinAwardPreCount <= 0)
                return;

            cmd_mentorship_request_firstaward cmdData = new cmd_mentorship_request_firstaward();
            cmdData.nPost = (int)EMMentorshipPost.emMentorshipPost_Master;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_firstaward>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD, ref cmdData);
        }
    }
}
