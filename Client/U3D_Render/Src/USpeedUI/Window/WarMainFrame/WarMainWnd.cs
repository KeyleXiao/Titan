using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using DataCenter;
using U3D_Render.USpeedUI.UWidgets;
using USpeedUI.UWidgets;
using War;
using Match_ManagerDef;
using USpeedUI.UWidgets.UAnimator;
using UnityEngine.EventSystems;
using USpeedUI.UEffect;
using DG.Tweening;
using USpeedUI.UExtensions;

namespace USpeedUI.WarMain
{
    public class WarMainWnd : UIFillWnd<UWarMainWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_MAIN_FRAME;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "WarWnd/WarMainFrame";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 预先载入
            //LoadView();
            SetVisible(false);

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_CLOSE, this);
            // 订阅进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE, this);
            // 退订进入战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {

                case WndMsgID.WND_MSG_WAR_MAIN_OPEN:
                    {
                        //// 隐藏其他同级窗体
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.WarMainModeItemsInit();
                        }
                    }
                    break;

                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                    }
                    break;

                case WndMsgID.WND_MSG_WAR_MAIN_CLOSE:
                    {
                        // 隐藏其他同级窗体
                        OnCloseKey();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE:
                    {
                        //更新下显示相关
                        if (m_wndView != null)
                        {
                            m_wndView.WarMainModeItemsInit();
                        }
                    }
                    break;
                    
				#region 新手引导部分

                case WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET:
                    {
                        if(m_wndView == null)
                        {
                            LoadView();
                            SetVisible(false);
                        }
                        UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                        if (cmdData != null)
                        {
                            m_wndView.AddGuideUIWidget(cmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_REMOVEGUIDEWIDGET:
                    {
                        if(m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.RemoveGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONBEGIN:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.GuideActionBegin();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONEND:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.GuideActionEnd();
                        }
                    }
                    break;
                #endregion

                default:
                    break;
            }
        }

        // ESC退出
        public override bool OnCloseKey()
        {
            base.OnCloseKey();

            if (m_wndView != null)
            {
                m_wndView.OnBackBtnClick();
            }

            return true;
        }
    }

    public class UWarMainModeItem : UCustomUIBehaviour, IPointerClickHandler, IPointerEnterHandler, IPointerExitHandler
    {
        public Image HeadIcon;
        public Image ModeIcon;
        public Text ModeDesText;
        public Image SelectIcon;
        public RectTransform GuideModalTrans;
        //public LoginVideo GameModeVideo;

        public float JumpDuration;
        public Vector2 StartJumpPos;
        public Vector2 EndJumpPos;

        /// <summary>
        /// 新手引导添加控件标示
        /// </summary>
        public bool bUseGuideWidget;
        public UGuideWidgetMsgData GuideWidgetData;

        private int m_nModeID;
        private UWarMainWndView m_wndView;

        //private UEffectParamBase m_DefaultParam;
        private UEffectParamBase m_EnterParam;

        protected override void Awake()
        {
            this.GetRectTransform.anchoredPosition = new Vector3(StartJumpPos.x, StartJumpPos.y, 0);
        }

        public void SetData(UWarMainWndView view, int nModeID, string strMode, string strModeDesc)
        {
            m_wndView = view;
            m_nModeID = nModeID;

            if(HeadIcon != null)
            {
                HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarMainMode, "mode_head_" + nModeID, WndID.WND_ID_WAR_MAIN_FRAME);
                HeadIcon.gameObject.SetActive(HeadIcon.sprite != null);
            }

            ModeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarMainMode, "mode_base_" + nModeID, WndID.WND_ID_WAR_MAIN_FRAME);
            ModeIcon.gameObject.SetActive(ModeIcon.sprite != null);

            ModeDesText.text = strModeDesc;
            ModeDesText.gameObject.SetActive(false);

            SelectIcon.gameObject.SetActive(false);

            //ShowDefaultEffect();
        }

        public void Clear()
        {
            //if (m_DefaultParam != null)
            //{
            //    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_DefaultParam);
            //    m_DefaultParam = null;
            //}
            if (m_EnterParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EnterParam);
                m_EnterParam = null;
            }
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (m_wndView == null)
                return;

            if (eventData.button != PointerEventData.InputButton.Left)
                return;

            switch (m_nModeID)
            {
                case (int)EWarModeDef.MODE_SpeedVS:
                    {
                        m_wndView.OnMatchModeBtnClick();
                    }
                    break;
                case (int)EWarModeDef.MODE_MachineVS:
                    {
                        m_wndView.OnMachineVSModeBtnClick();
                    }
                    break;
                case (int)EWarModeDef.MODE_RankVS:
                    {
                        m_wndView.OnRankModeBtnClick();
                    }
                    break;
                case (int)EWarModeDef.MODE_CustomerVS:
                    {
                        m_wndView.OnCustomModeBtnClick();
                    }
                    break;
                case (int)EWarModeDef.MODE_SurviveVS:
                    {
                        m_wndView.OnSurviveVSModeBtnClick();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            ModeDesText.gameObject.SetActive(true);
            SelectIcon.gameObject.SetActive(true);

            ShowEnterEffect();
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            ModeDesText.gameObject.SetActive(false);
            SelectIcon.gameObject.SetActive(false);

            if (m_EnterParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EnterParam);
                m_EnterParam = null;
            }
        }

        //private void ShowDefaultEffect()
        //{
        //    if (m_DefaultParam != null)
        //    {
        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_DefaultParam);
        //        m_DefaultParam = null;
        //    }

        //    UEffectPrefabType prefabType = UEffectPrefabType.UEPT_None;
        //    switch (m_nModeID)
        //    {
        //        case (int)EWarModeDef.MODE_MachineVS:
        //            {
        //                prefabType = UEffectPrefabType.UEPT_WarMain_ItemMachine;
        //            }
        //            break;
        //        case (int)EWarModeDef.MODE_RankVS:
        //            {
        //                prefabType = UEffectPrefabType.UEPT_WarMain_ItemRank;
        //            }
        //            break;
        //        case (int)EWarModeDef.MODE_CustomerVS:
        //            {
        //                prefabType = UEffectPrefabType.UEPT_WarMain_ItemCustome;
        //            }
        //            break;
        //        case (int)EWarModeDef.MODE_SurviveVS:
        //            {
        //                prefabType = UEffectPrefabType.UEPT_WarMain_ItemSurvive;
        //            }
        //            break;
        //        default:
        //            break;
        //    }
        //    if (prefabType != UEffectPrefabType.UEPT_None)
        //    {
        //        m_DefaultParam = new UEffectPrefabParam(_eType: prefabType, _tfAttachParent: this.gameObject.transform);
        //        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_DefaultParam);
        //    }
        //}

        private void ShowEnterEffect()
        {
            if (m_EnterParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EnterParam);
                m_EnterParam = null;
            }

            //if (m_nModeID != (int)EWarModeDef.MODE_SpeedVS)
            //{
            m_EnterParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_ItemEnter, _tfAttachParent: ModeIcon.gameObject.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_EnterParam);
            //}
        }

        #region 新手引导部分
        public void SetGuideWidgetData(bool openGuide, UGuideWidgetMsgData widgetMsg)
        {
            bUseGuideWidget = openGuide;
            GuideWidgetData = widgetMsg;

            if (!bUseGuideWidget)
            {
                gameObject.RemoveComponent<Guide.UGuideWidget>();
            }
            else
            {
                Guide.UGuideWidget guideWidget = gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = gameObject.AddComponent<Guide.UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)GuideWidgetData.nGuideID;
                guideWidget.GuideStepID = GuideWidgetData.nGuideStepID;
                guideWidget.GuideEffectID = GuideWidgetData.nGuideEffectID;
                guideWidget.IsForceGuide = GuideWidgetData.bForeceGuide;
                guideWidget.ModalTrans = guideWidget.IsForceGuide ? GuideModalTrans : null;
            }
        }
        #endregion

        protected override void OnDisable()
        {
            this.GetRectTransform.DOPause();
            this.GetRectTransform.anchoredPosition = new Vector3(StartJumpPos.x, StartJumpPos.y, 0);
        }

        protected override void OnEnable()
        {
            this.GetRectTransform.DOJumpAnchorPos(new Vector3(EndJumpPos.x, EndJumpPos.y, 0), 10, 1, JumpDuration);
        }

    }

    public class UWarMainWndView : UIBaseWndView
    {
        public Image BaseEffect;

        public Text TitleDesc;

        public UWarMainModeItem[] WarMainModeItems;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            UEffectParamBase materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, BaseEffect, true);
            if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref materialEffectParam))
            {
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.20f));
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_Vibrancy", 0f);
            }

            TitleDesc.text = "选择模式";

            return true;
        }

        public void WarMainModeItemsInit()
        {
            for (int i = (int)EWarModeDef.MODE_SpeedVS; i < (int)EWarModeDef.MODE_MAX && i-1 < WarMainModeItems.Length; ++i)
            {
                int nModeID = i;
                string strMode = ULocalizationService.Instance.Get("UIView", "WarMain", "Mode" + i);
                string strModeDesc = ULocalizationService.Instance.Get("UIView", "WarMain", "ModeDes" + i);
                WarMainModeItems[i-1].SetData(this, i, strMode, strModeDesc);
            }
        }

        public void OnMatchModeBtnClick()
        {
            UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_INVITE_SHOW, (int)EMMatchType.MatchType_Match, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, cmdData);

            //if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
            //    return;
            //}
            //LogicDataCenter.matchDataManager.OnMatchModeBtnClick();
        }

        public void OnMachineVSModeBtnClick()
        {
            UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_INVITE_SHOW, (int)EMMatchType.MatchType_MachineMatch, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, cmdData);

            //if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
            //    return;
            //}

            //if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
            //    return;
            //}

            //LogicDataCenter.matchDataManager.OnMachineVSModeBtnClick();
        }

        public void OnRankModeBtnClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_DETIAL_SHOW);

            //if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
            //    return;
            //}
            //LogicDataCenter.matchDataManager.OnRankModeBtnClick();
        }

        public void OnCustomModeBtnClick()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }

            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }

            //// 隐藏其他同级窗体
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
            //UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);

            // 打开大厅界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_WARHILL_PRESSDOWN, null);

            OnBackBtnClick();
        }

        public void OnSurviveVSModeBtnClick()
        {
            //UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_INVITE_SHOW, (int)EMMatchType.MatchType_MultiCampMatch, "", IntPtr.Zero, 0);
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, cmdData);

            //if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
            //    return;
            //}

            //if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
            //    return;
            //}
            
            //LogicDataCenter.matchDataManager.OnSurviveVSModeBtnClick();

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "生存模式",
                                   message: "生存模式正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnClanTeamModeBtnClick()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }

            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "联盟匹配",
                                   message: "联盟匹配正在开发中....",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnRecreationModeBtnClick()
        {
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }

            if (LogicDataCenter.viewPersonStateDataManager.bDelayJoinMatchRoom)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION);
                return;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "娱乐模式",
                                   message: "娱乐模式娱乐模式正在开发中",
                                   buttons: new UIWidgets.DialogActions() { { "确认", null } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnBackBtnClick()
        {
            for (int i = (int)EWarModeDef.MODE_SpeedVS; i < (int)EWarModeDef.MODE_MAX && i - 1 < WarMainModeItems.Length; ++i)
            {
                WarMainModeItems[i - 1].Clear();
            }

            SetVisible(false);
        }

        #region 新手引导部分
        internal void AddGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (_msgData == null || (int)_msgData.oUserData < 1)
                return;

            if(WarMainModeItems.Length >= (int)_msgData.oUserData && WarMainModeItems[(int)_msgData.oUserData - 1] != null)
            {
                WarMainModeItems[(int)_msgData.oUserData - 1].SetGuideWidgetData(true, new UGuideWidgetMsgData(_msgData));
            }
        }
        internal void RemoveGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            {
                if (_msgData == null || (int)_msgData.oUserData < 1)
                    return;

                if (WarMainModeItems.Length >= (int)_msgData.oUserData && WarMainModeItems[(int)_msgData.oUserData - 1] != null)
                {
                    WarMainModeItems[(int)_msgData.oUserData - 1].SetGuideWidgetData(false, new UGuideWidgetMsgData(_msgData));
                }
            }
        }

        internal void GuideActionBegin()
        {
            Transform backBtnTF = GetTransform().FindChild("BackBtn");
            if (backBtnTF != null)
            {
                backBtnTF.gameObject.SetActive(false);
            }
        }

        internal void GuideActionEnd()
        {
            Transform backBtnTF = GetTransform().FindChild("BackBtn");
            if (backBtnTF != null)
            {
                backBtnTF.gameObject.SetActive(true);
            }
        }
        #endregion
    }
}
