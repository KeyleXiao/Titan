using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI.TooltipAndDialog;
using UIWidgets;
using UnityEngine.UI;
using UnityEngine;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using DataCenter;
using GameLogic;
using System.Text.RegularExpressions;
using U3D_Render.Common;
using Data.SideButtonConfig;

namespace USpeedUI.AntiAddict
{
    public class UAntiAddictWnd : UIBaseWnd<UAntiAddictWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_ANTIADDICT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "AntiAddict/AntiAddictView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            m_bHaveEscCallBack = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWSIDEBTN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWRESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWTIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWSIDEBTN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWRESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWTIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        // 订阅新手引导步骤更改事件（新手引导之前不弹防沉迷）
                        GUIDE.GuideManager.onUpdateNoviceGuideTypeEvent -= OnNoviceDataCome;
                        GUIDE.GuideManager.onUpdateNoviceGuideTypeEvent += OnNoviceDataCome;
                    }
                    break;
                case WndMsgID.WND_MSG_ANTIADDICT_SHOWSIDEBTN:
                    {
                        if (!bCheckShowAntiWnd())
                            return;

                        SetVisible(true);
                        if (m_wndView == null)
                            return;

                        m_wndView.ShowGuideFrame(ULocalizationService.Instance.Get("UIView", "AntiAddict", "GuideContentTip"));
                    }
                    break;
                case WndMsgID.WND_MSG_ANTIADDICT_SHOWRESULT:
                    {
                        if (!bCheckShowAntiWnd())
                            return;

                        SetVisible(true);
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData cmdData = (UIMsgCmdData)data;
                        OnMsgRegistResult(cmdData.ptrParam);
                    }
                    break;
                case WndMsgID.WND_MSG_ANTIADDICT_SHOWTIP:
                    {
                        if (!bCheckShowAntiWnd())
                            return;

                        SetVisible(true);
                        if (m_wndView == null)
                            return;

                        string strContent = LogicDataCenter.sideButtonDataManager.StrAddictTip;
                        LogicDataCenter.sideButtonDataManager.StrAddictTip = null;
                        m_wndView.ShowTipFrame(strContent);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        GUIDE.GuideManager.onUpdateNoviceGuideTypeEvent -= OnNoviceDataCome;
                        UnloadView();
                    }
                    break;
            #region 新手引导部分

                case WndMsgID.WND_MSG_ANTIADDICT_GUIDE_ACTIONBEGIN:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_ANTIADDICT_GUIDE_ACTIONEND:
                    {
                        SetVisible(true);
                    }
                    break;
			#endregion
                default:
                    break;
            }
        }

        private void AddSideButtonAntiAddict()
        {
            int playerLv = 0;
            if (EntityFactory.MainHeroView != null)
                playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_AntiAddict);
            if (playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_AntiAddict;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_ANTIADDICT_SHOWSIDEBTN, null);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(addMsg);
            }
        }

        private void RemoveSideButtonAntiAddict()
        {
            SideButtonListRemoveData msg = new SideButtonListRemoveData();
            msg.nItemID = (int)ESideButtonItemType.ESBT_AntiAddict;
            LogicDataCenter.sideButtonDataManager.RemoveItem(msg);
        }

        private void OnMsgRegistResult(IntPtr ptrParam)
        {
            gamelogic_antiaddict_regist_result data = IntPtrHelper.toData<gamelogic_antiaddict_regist_result>(ptrParam);
            if (data.ret > 0)
            {
                RemoveSideButtonAntiAddict();
                m_wndView.registFrame.gameObject.SetActive(false);
            }
                
            m_wndView.ShowResultFrame(data);
        }

        private bool bCheckShowAntiWnd()
        {
            // 小于新手引导某步骤
            if (GUIDE.GuideManager.Instance.IsCurrentGuideLessEqual(ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_LIFEHEROCULTURE))
            {
                return false;
            }
            // 在战场中
            if (GameLogicAPI.isInWarScene() > 0)
            {
                return false;
            }

            return true;
        }

        public override bool HaveEscCallBack()
        {
            if (m_wndView != null && m_wndView.tipFrame.ConfirmBtn.interactable)
            {
                return false;
            }

            return true;
        }

        private void OnNoviceDataCome(GUIDE.UpdateNoviceGuideTypeEventArgs e)
        {
            // 新手引导时间内不显示防沉迷
            if (!bCheckShowAntiWnd())
                return;

            // 游戏开始、战场出来都执行
            if (GameLogicAPI.isRealNameAuthed())
                return;
            AddSideButtonAntiAddict();

            SetVisible(true);

            // 战场出来,是否沉迷提醒
            string str = LogicDataCenter.sideButtonDataManager.StrAddictTip;
            if (str != null)
            {
                LogicDataCenter.sideButtonDataManager.StrAddictTip = null;
                m_wndView.ShowTipFrame(str);
            }

            // 游戏开始,初始提示注册
            if (!LogicDataCenter.sideButtonDataManager.Login2StaticGame)
                return;
            LogicDataCenter.sideButtonDataManager.Login2StaticGame = false;
            m_wndView.ShowGuideFrame(ULocalizationService.Instance.Get("UIView", "AntiAddict", "GuideContentStart"));

            GUIDE.GuideManager.onUpdateNoviceGuideTypeEvent -= OnNoviceDataCome;
        }
    }

    public class DropDownEx_AntiAddict : Dropdown
    {
        public Image DropArrow;
        public Sprite[] DropSprites;
        protected override GameObject CreateDropdownList(GameObject template)
        {
            DropArrow.sprite = DropSprites[1];
            return base.CreateDropdownList(template);
        }
        protected override void DestroyDropdownList(GameObject dropdownList)
        {
            base.DestroyDropdownList(dropdownList);
            DropArrow.sprite = DropSprites[0];
        }
    }

    public class InputFieldEx_AntiAddict : InputField
    {
        public override void OnSelect(BaseEventData eventData)
        {
            base.OnSelect(eventData);
            //placeholder.gameObject.SetActive(false);
        }

        public override void OnDeselect(BaseEventData eventData)
        {
            base.OnDeselect(eventData);
            //if (text == "")
            //    placeholder.gameObject.SetActive(true);
            //else
            //    placeholder.gameObject.SetActive(false);
        }
    }

    public class BaseAntiFrame : UIBehaviour
    {
        public static UAntiAddictWndView AntiAddictView;

        public virtual void OnClickCloseBtn()
        {
            this.gameObject.SetActive(false);

            if (checkIsAllInvisible())
            {
                AntiAddictView.GetUIWnd().SetVisible(false);
            }
        }

        public virtual void OnClickConfirmBtn()
        {
            this.gameObject.SetActive(false);

            if (checkIsAllInvisible())
            {
                AntiAddictView.GetUIWnd().SetVisible(false);
            }
        }

        public bool checkIsAllInvisible()
        {
            bool bAllInVisible = true;
            do
            {
                if (AntiAddictView.guideFrame.gameObject.activeSelf)
                {
                    bAllInVisible = false;
                    break;
                }
                if (AntiAddictView.registFrame.gameObject.activeSelf)
                {
                    bAllInVisible = false;
                    break;
                }
                if (AntiAddictView.resultFrame.gameObject.activeSelf)
                {
                    bAllInVisible = false;
                    break;
                }
                if (AntiAddictView.tipFrame.gameObject.activeSelf)
                {
                    bAllInVisible = false;
                    break;
                }
            } while (false);

            return bAllInVisible;
        }
    }

    public class AntiGuideFrame : BaseAntiFrame
    {
        public Text Title;
        public Text GotoBtnDesc;
        public Text WaitBtnDesc;
        public Text Content;

        protected override void Awake()
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "GuideTitle");
            GotoBtnDesc.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "GotoBtnDesc");
            WaitBtnDesc.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "WaitBtnDesc");
        }

        public void SetData(UAntiAddictWndView view, string strContent)
        {
            UBB.toHtml(ref strContent);
            Content.text = strContent;
        }

        public void OnClickGotoBtn()
        {
            this.gameObject.SetActive(false);
            AntiAddictView.registFrame.ClearIF();
            AntiAddictView.registFrame.gameObject.SetActive(true);
        }

        public void OnClickWaitBtn()
        {
            this.gameObject.SetActive(false);

            if (checkIsAllInvisible())
            {
                AntiAddictView.GetUIWnd().SetVisible(false);
            }
        }
    }

    public class AntiRegistFrame : BaseAntiFrame
    {
        public Text Title;
        public Text SubmitBtnDesc;

        public InputFieldEx_AntiAddict NameIF;
        public InputFieldEx_AntiAddict IDIF;
        //public DropDownEx_AntiAddict IDType;
        public Text IDType;

        protected override void Awake()
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "RegistTitle");
            SubmitBtnDesc.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "SubmitBtnDesc");
            NameIF.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "InputNameDesc");
            IDIF.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "InputIDDesc");

            //IDType.options.Clear();
            //IDType.options.Add(new Dropdown.OptionData("居民身份证"));
            //IDType.options.Add(new Dropdown.OptionData("护照"));
            //IDType.options.Add(new Dropdown.OptionData("军人身份证"));
            //IDType.captionText.text = IDType.options.First().text;
            IDType.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "IDCard");
        }

        public void ClearIF()
        {
            NameIF.text = "";
            IDIF.text = "";
        }

        public void OnClickSubmitBtn()
        {
            if (!Regex.IsMatch(IDIF.text, @"^(\d{6})((18|19|20)?\d{2})((0[1-9])|(1[0-2]))((0[1-9])|([12]\d)|(3[01]))(\d{3})([0-9]|X)$"))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ANTIADDICT_IDTYOE_WRONG);
                return;
            }

            gameview_antiaddict_regist registData = new gameview_antiaddict_regist();
            registData.entityName = NameIF.text;
            registData.card = IDIF.text;
            ViewEventHelper.Instance.SendCommand<gameview_antiaddict_regist>(GameLogicDef.GVIEWCMD_ANTIADDICT_REGIST, ref registData);
        }
    }

    public class AntiResultFrame : BaseAntiFrame
    {
        public Text Title;
        public Text Content;
        public Text ConfirmBtn;

        protected override void Awake()
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "ResultTitle");
            ConfirmBtn.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "ConfirmBtnDesc");
        }

        public void SetData(UAntiAddictWndView view, gamelogic_antiaddict_regist_result result)
        {
            Content.text = result.szRetDes;
            if (result.ret > 0)
            {
                view.tipFrame.Clear();
            }
        }
    }

    public class AntiTipFrame : BaseAntiFrame, ITimerHanlder
    {
        public Text Title;
        public Text Content;
        public Text ConfirmBtnDesc;
        public Button ConfirmBtn;
        public Button CloseBtn;

        private UAntiAddictWndView m_wndView;
        private int nCountTime;

        protected override void Awake()
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "TipTitle");
        }

        public void SetData(UAntiAddictWndView view, string strContent)
        {
            m_wndView = view;

            Content.text = strContent;
            TimerManager.KillTimer(this, 1);
            TimerManager.SetTimer(this, 1, 1);
            nCountTime = 60;
            ConfirmBtnDesc.text = nCountTime.ToString();
            ConfirmBtn.interactable = false;
            CloseBtn.gameObject.SetActive(false);
        }

        public void OnTimer(int nTimerID)
        {
            nCountTime--;
            ConfirmBtnDesc.text = nCountTime.ToString();
            // 5秒后弹出填写沉迷信息的窗口,没有实名认证过的
            if (nCountTime == 55 && !GameLogicAPI.isRealNameAuthed())
            {
                m_wndView.guideFrame.SetData(m_wndView, ULocalizationService.Instance.Get("UIView", "AntiAddict", "GuideContentTip"));
                m_wndView.guideFrame.gameObject.SetActive(true);
            }
            if (nCountTime <= 0)
            {
                TimerManager.KillTimer(this, 1);
                ConfirmBtnDesc.text = ULocalizationService.Instance.Get("UIView", "AntiAddict", "ConfirmBtnDesc");
                ConfirmBtn.interactable = true;
                CloseBtn.gameObject.SetActive(true);
            }
        }

        public override void OnClickConfirmBtn()
        {
            base.OnClickConfirmBtn();
            if (!GameLogicAPI.isRealNameAuthed())
            {
                m_wndView.guideFrame.SetData(m_wndView, ULocalizationService.Instance.Get("UIView", "AntiAddict", "GuideContentTip"));
                m_wndView.guideFrame.gameObject.SetActive(true);
            }
        }

        public void Clear()
        {
            TimerManager.KillTimer(this, 1);
            ConfirmBtn.interactable = true;
            CloseBtn.gameObject.SetActive(true);
            this.gameObject.SetActive(false);
        }
    }

    public class UAntiAddictWndView : UIBaseWndView
    {
        public AntiGuideFrame guideFrame;
        public AntiRegistFrame registFrame;
        public AntiResultFrame resultFrame;
        public AntiTipFrame tipFrame;

        public override void Awake()
        {
            BaseAntiFrame.AntiAddictView = this;

            base.Awake();
        }

        public void ShowGuideFrame(string strContent)
        {
            guideFrame.SetData(this, strContent);
            guideFrame.gameObject.SetActive(true);
        }

        public void ShowResultFrame(gamelogic_antiaddict_regist_result result)
        {
            resultFrame.SetData(this, result);
            resultFrame.gameObject.SetActive(true);

            Debug.Log(String.Format("Auth real name result is [{0}]", result.szRetDes));
        }

        public void ShowTipFrame(string strContent)
        {
            tipFrame.SetData(this, strContent);
            tipFrame.gameObject.SetActive(true);
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);
            if (!visible)
            {
                tipFrame.Clear();
            }
        }
    }
}
