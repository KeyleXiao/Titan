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
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE:
                    {
                        //更新下显示相关
                        if (m_wndView != null)
                        {
                            m_wndView.OnMatchOpenOrClose();
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
                            //m_wndView.AddGuideUIWidget(cmdData);
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
                                //m_wndView.RemoveGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONBEGIN:
                    {
                        if(m_wndView != null)
                        {
                            //m_wndView.GuideActionBegin();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONEND:
                    {
                        if(m_wndView != null)
                        {
                            //m_wndView.GuideActionEnd();
                        }
                    }
                    break;
                #endregion

                default:
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (m_wndView != null && _bVisible)
            {
                //m_wndView.UpdateAllItems();
                //m_wndView.ViewAnim.PlayForward();
            }
        }
    }

    public class UWarMainModeItem : UCustomUIBehaviour, IPointerClickHandler, IPointerEnterHandler, IPointerExitHandler
    {
        public Image HeadIcon;
        public Image ModeIcon;
        public Text ModeDesText;
        public Image SelectIcon;

        public float JumpDuration;
        public Vector2 StartJumpPos;
        public Vector2 EndJumpPos;

        private int m_nModeID;
        private UWarMainWndView m_wndView;

        protected override void Awake()
        {
            this.GetRectTransform.anchoredPosition = new Vector3(StartJumpPos.x, StartJumpPos.y, 0);
        }

        public void SetData(UWarMainWndView view, int nModeID, string strMode, string strModeDesc)
        {
            m_wndView = view;
            m_nModeID = nModeID;

            HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarMainMode, "mode_head_" + nModeID, WndID.WND_ID_WAR_MAIN_FRAME);
            HeadIcon.gameObject.SetActive(HeadIcon.sprite != null);

            ModeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarMainMode, "mode_base_" + nModeID, WndID.WND_ID_WAR_MAIN_FRAME);
            ModeIcon.gameObject.SetActive(ModeIcon.sprite != null);

            ModeDesText.text = strModeDesc;
            ModeDesText.gameObject.SetActive(false);

            SelectIcon.gameObject.SetActive(false);
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (m_wndView == null)
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
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            ModeDesText.gameObject.SetActive(false);
            SelectIcon.gameObject.SetActive(false);
        }

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

        //public UViewAnim ViewAnim;

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

            WarMainModeItemsInit();

            //if (ViewAnim == null)
            //    ViewAnim = GetComponent<UViewAnim>();

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

        public void OnMatchOpenOrClose()
        {
            WarMainModeItemsInit();
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

            LogicDataCenter.matchDataManager.OnMachineVSModeBtnClick();
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
            m_wnd.SetVisible(false);
        }

        //#region 新手引导部分
        //internal void AddGuideUIWidget(UGuideWidgetMsgData _msgData)
        //{
        //    ListMode.DataSource.BeginUpdate();
        //    for (int i = 0; i < ListMode.DataSource.Count; ++i)
        //    {
        //        if(ListMode.DataSource[i].nModeID == (int)_msgData.oUserData)
        //        {
        //            ListMode.DataSource[i].bUseGuideWidget = true;
        //            ListMode.DataSource[i].GuideWidgetData = new UGuideWidgetMsgData(_msgData);
                    
        //            break;
        //        }
        //    }
        //    ListMode.DataSource.EndUpdate();
        //    ListMode.UpdateItems();
        //}
        //internal void RemoveGuideUIWidget(UGuideWidgetMsgData _msgData)
        //{
        //    for (int i = 0; i < ListMode.DataSource.Count; ++i)
        //    {
        //        if (ListMode.DataSource[i].nModeID == (int)_msgData.oUserData)
        //        {
        //            ListMode.DataSource[i].bUseGuideWidget = false;
        //            ListMode.DataSource[i].GuideWidgetData = new UGuideWidgetMsgData(_msgData);
                    
        //            break;
        //        }
        //    }
        //}

        //internal void GuideActionBegin()
        //{
        //    Transform backBtnTF = GetTransform().FindChild("BackBtn");
        //    if(backBtnTF != null)
        //    {
        //        backBtnTF.gameObject.SetActive(false);
        //    }
        //}

        //internal void GuideActionEnd()
        //{
        //    Transform backBtnTF = GetTransform().FindChild("BackBtn");
        //    if (backBtnTF != null)
        //    {
        //        backBtnTF.gameObject.SetActive(true);
        //    }
        //}
        //#endregion
    }
}
