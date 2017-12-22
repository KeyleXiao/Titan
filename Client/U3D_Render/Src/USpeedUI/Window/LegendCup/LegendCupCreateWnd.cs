using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using ASpeedGame.Data.LegendCupConfig;
using GameLogic;
using U3D_Render.Common;
using LegendCup_ManagedDef;
using ASpeedGame.Data.ShieldWorld;
using DataCenter;

namespace USpeedUI.LegendCup
{
    public class LegendCupCreateWnd : UIPopupWnd<LegendCupCreateWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_MSGBOXCREATE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupCreateView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_CLOSE, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_CLOSE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            UIMsgCmdData uiData = (UIMsgCmdData)data;
                            cmd_legendcup_recv_create_cup_type_info cmdData = IntPtrHelper.toData<cmd_legendcup_recv_create_cup_type_info>(uiData.ptrParam);
                            m_wndView.OnLegendCupCreateWndOpen(cmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_CLOSE:
                    {
                        if (m_wndView != null)
                            UnloadView();
                    }
                    break;
                default:
                    break;
            }
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

        public override bool OnCloseKey()
        {
            base.OnCloseKey();
            UnloadView();

            return true;
        }
    }

    public class LegendCupCreateWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text LegendCupNameDes;
        public Text CupConfigNameDes;
        public Text DeadlineDes;
        public Text PasswordDes;
        public Text RegistGoldDes;
        public Text ContriBonusDes;
        public Text CupIsOnlineDes;
        public InputField LegendCupNameIF;
        public InputField PassWordIF;
        public InputField RegistGoldIF;
        public InputField ContriBonusIF;
        public Dropdown CupConfigNameDd;
        public Dropdown DeadlineDd;
        public Dropdown CupIsOnlineDd;
        public Text CreateGoldDes;
        public Text CreateGold;

        public Text SubTitleDes;
        public Text CreateTipContent;

        public Text ConfirmBtnDes;
        public Text CancelBtnDes;

        private SSchemeLegendCupConfig m_curCupConfig;
        private int[] m_nCupIDList;
        private byte m_nCreateType;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "Title");
            LegendCupNameDes.text =  String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "LegendCupNmae"));
            CupConfigNameDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CupConfigName"));
            DeadlineDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "Deadline"));
            PasswordDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "Password"));
            RegistGoldDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "RegistGold"));
            ContriBonusDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "ContriBonus"));
            CupIsOnlineDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CupIsOnline"));
            CreateGoldDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CreateGold"));

            LegendCupNameIF.placeholder.GetComponent<Text>().text  = "";
            PassWordIF.placeholder.GetComponent<Text>().text = "";
            RegistGoldIF.placeholder.GetComponent<Text>().text = "0";
            ContriBonusIF.placeholder.GetComponent<Text>().text = "0";
            CreateGold.text = "0";

            CupIsOnlineDd.onValueChanged.RemoveListener(OnChangedCupIsOnlineDd);
            CupIsOnlineDd.options.Clear();
            CupIsOnlineDd.options.Add(new Dropdown.OptionData(ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CupIsOnlineOpt1")));
            CupIsOnlineDd.options.Add(new Dropdown.OptionData(ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CupIsOnlineOpt2")));
            CupIsOnlineDd.captionText.text = CupIsOnlineDd.options.ElementAt(0).text;
            CupIsOnlineDd.onValueChanged.AddListener(OnChangedCupIsOnlineDd);

            SSchemeLegendCupTipConfig tipConfig = LegendCupConfig.Instance.GetCupTipConfig((int)LegendCupTipType.LCTT_OnLine_CupCreate);
            if (tipConfig != null)
            {
                SubTitleDes.text = tipConfig.sCupTitle;
                UBB.toHtml(ref tipConfig.sCupContent, UBB_FORMAT_TYPE.UGUI);
                CreateTipContent.text = tipConfig.sCupContent;
            }

            ConfirmBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "ConfirmBtn");
            CancelBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "CancelBtn");

            return true;
        }

        public void OnLegendCupCreateWndOpen(cmd_legendcup_recv_create_cup_type_info typeInfo)
        {
            m_nCreateType = typeInfo.byLegendCupType;
            SSchemeLegendCupTypeConfig typeConfig = LegendCupConfig.Instance.GetCupTypeConfig((int)typeInfo.byLegendCupType);
            if (typeConfig == null)
                return;
            m_nCupIDList = typeInfo.bIsFreeCreate > 0 ? typeConfig.nFreeCupConfigIDList : typeConfig.nChargeCupconfigIDList;

            CupConfigNameDd.onValueChanged.RemoveListener(OnChangedCupConfigDd);
            CupConfigNameDd.options.Clear();
            SSchemeLegendCupConfig cupConfig = null;
            for (int i = 0; i < m_nCupIDList.Length; i++)
            {
                cupConfig = LegendCupConfig.Instance.GetCupConfig(m_nCupIDList[i]);
                if (cupConfig == null)
                    continue;

                CupConfigNameDd.options.Add(new Dropdown.OptionData(cupConfig.sCupConfigViewName));
            }
            CupConfigNameDd.onValueChanged.AddListener(OnChangedCupConfigDd);

            // 默认选择要求：战队杯赛第一组第一个，联盟杯赛选符合人数限制(都不符合选最后一个)
            int nSelectIndex = 0;
            if (typeInfo.byLegendCupType == (int)ELegendCupType.emCupType_Clan)
            {
                int nClanMemberCount = LogicDataCenter.clanDataManager.ClanBaseData.nMemberCount;

                nSelectIndex = -1;
                for (int i = 0; i < m_nCupIDList.Length; i++)
                {
                    cupConfig = LegendCupConfig.Instance.GetCupConfig(m_nCupIDList[i]);
                    if (cupConfig == null)
                        continue;
                    SSchemeLegendCupLimitConfig limitConfig = LegendCupConfig.Instance.GetCupLimitConfig(cupConfig.nConfigID);
                    if (limitConfig == null)
                        continue;

                    if (nClanMemberCount >= limitConfig.nCountLimit)
                    {
                        nSelectIndex = i;
                        break;
                    }
                }
                if (nSelectIndex < 0)
                {
                    nSelectIndex = m_nCupIDList.Length - 1;
                }
            }

            // 设置下拉框默认选中项
            if (nSelectIndex < 0 || nSelectIndex >= CupConfigNameDd.options.Count)
            {
                return;                
            }
            CupConfigNameDd.captionText.text = CupConfigNameDd.options.ElementAt(nSelectIndex).text;
            m_curCupConfig = LegendCupConfig.Instance.GetCupConfig(m_nCupIDList[nSelectIndex]);
            
            DeadlineDd.options.Clear();
            for (int i = 0; i < m_curCupConfig.nDeadline.Length; i++)
            {
                DeadlineDd.options.Add(new Dropdown.OptionData(m_curCupConfig.nDeadline[i] + ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "Day")));
            }
            DeadlineDd.captionText.text = DeadlineDd.options.First().text;

            CreateGold.text = m_curCupConfig.nCreateNeedMoney.ToString();
            RegistGoldIF.placeholder.GetComponent<Text>().text = m_curCupConfig.nRegistNeedMoney.ToString();

            // 联盟杯赛不设密码
            if (typeInfo.byLegendCupType == (int)ELegendCupType.emCupType_Clan)
            {
                PassWordIF.DeactivateInputField();
            }
            else
            {
                PassWordIF.ActivateInputField();
            }

            // 免费杯赛不设费用
            if (typeInfo.bIsFreeCreate > 0)
            {
                RegistGoldIF.DeactivateInputField();
            }
            else
            {
                RegistGoldIF.ActivateInputField();
            }
        }

        public void OnChangedCupConfigDd(int nIndex)
        {
            DeadlineDd.options.Clear();
            m_curCupConfig = LegendCupConfig.Instance.GetCupConfig(m_nCupIDList[nIndex]);
            for (int i = 0; i < m_curCupConfig.nDeadline.Length; i++)
            {
                DeadlineDd.options.Add(new Dropdown.OptionData(m_curCupConfig.nDeadline[i] + ULocalizationService.Instance.Get("UIView", "LegendCupCreate", "Day")));
            }
            DeadlineDd.captionText.text = DeadlineDd.options.First().text;

            CreateGold.text = m_curCupConfig.nCreateNeedMoney.ToString(); 
        }

        public void OnChangedCupIsOnlineDd(int nIndex)
        {
            SSchemeLegendCupTipConfig tipConfig = null;
            if (nIndex == 0)
                tipConfig = LegendCupConfig.Instance.GetCupTipConfig((int)LegendCupTipType.LCTT_OnLine_CupCreate);
            else if (nIndex == 1)
                tipConfig = LegendCupConfig.Instance.GetCupTipConfig((int)LegendCupTipType.LCTT_OffLine_CupCreate);
            if (tipConfig != null)
            {
                SubTitleDes.text = tipConfig.sCupTitle;
                UBB.toHtml(ref tipConfig.sCupContent, UBB_FORMAT_TYPE.UGUI);
                CreateTipContent.text = tipConfig.sCupContent;
            }
        }

        public void OnClickConfirmBtn()
        {
            // 名字
            if (LegendCupNameIF.text == string.Empty)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_ENTER_ANY_CUP_NAME);
                return;
            }
            if (!ShieldWord.Instance.IsStringLenRight(LegendCupNameIF.text))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CUP_NAME_LEN_NOT_CORRECT);
                return;
            }
            if (ShieldWord.Instance.IsHaveShieldWord(LegendCupNameIF.text))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CUP_NAME_CONTAIN_SHIELD_WORD);
                return;
            }

            // 钱
            int registGold = 0;
            if (Int32.TryParse(RegistGoldIF.text, out registGold) == false)
            {
                Int32.TryParse(RegistGoldIF.placeholder.GetComponent<Text>().text, out registGold);
            }
            if (registGold < m_curCupConfig.nRegistNeedMoney)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CREATE_REGISTMONEY_NOTENOUGH);
                return;
            }
            if (registGold > m_curCupConfig.nRegistHighestMoney)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_OVER_HIGHEST_REGISTER_MONEY, m_curCupConfig.nRegistHighestMoney.ToString());
                return;
            }

            cmd_legendcup_req_cup_create createData = new cmd_legendcup_req_cup_create();
            createData.byCupConfigID = (byte)m_curCupConfig.nConfigID;
            createData.byCupIsOnine = (byte)(CupIsOnlineDd.value == 0 ? 1 : 0);
            createData.byCreateCupType = m_nCreateType;
            createData.nCreaterPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            createData.nDayInterval = m_curCupConfig.nDeadline[DeadlineDd.value];//(天)
            createData.nRegistGold = Convert.ToInt32(String.IsNullOrEmpty(RegistGoldIF.text) ? RegistGoldIF.placeholder.GetComponent<Text>().text : RegistGoldIF.text);
            createData.nContriBonus = Convert.ToInt32(String.IsNullOrEmpty(ContriBonusIF.text) ? ContriBonusIF.placeholder.GetComponent<Text>().text : ContriBonusIF.text);
            createData.szLegendCupName = LegendCupNameIF.text;
            createData.szPassword = PassWordIF.text;
            createData.nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
            createData.byTicketAllocateMode = (byte)ETicketAllocateMode.ETAM_SystemSet;

            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_cup_create>(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE, ref createData);
        }

        public void OnClickCancelBtn()
        {
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }

        public void SetIntegerPositive(InputField _input)
        {
            if (_input == null)
                return;

            if (_input.text == "-")
                _input.text = "";
        }
    }
}
