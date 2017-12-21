using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.GameSettingConfig;
using ASpeedGame.Data.SlotKeyCodeConfig;
using Match_ManagerDef;
using GameLogic;

namespace USpeedUI.SystemSetting
{
    //系统设置子窗口类型
    public enum SystemSettingSubWndType
    {
        WND_CONTROL_DEFAULT,
        WND_CONTROL_OB,
        WND_VEDIO,
        WND_AUDIO,
        WND_DISPLAY,
        WND_GAME,
    }
    
    //系统设置按钮标签类型
    public enum SystemSettingFrameButtonType
    {
        BTN_CONTROL,
        BTN_VEDIO,
        BTN_AUDIO,
        BTN_DISPLAY,
        BTN_GAME,
    }

    public class SystemSettingPanelWnd : UIPopupWnd<SystemSettingPanelWndView>
    {
        //子窗口列表
        private List<IUISubWnd> m_subWndList = new List<IUISubWnd>();
        //当前窗口类型
        private SystemSettingSubWndType m_currentWndType;
        //当前按钮类型
        private SystemSettingFrameButtonType m_currentPageType;

        private bool bInWar;

        public override WndID GetID()
        {
            return WndID.WND_ID_SYSTEMSETTINGPANEL;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/SystemSettingPanel";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            bInWar = false;

            //添加子窗口
            m_subWndList.Add(new SystemControlWnd());
            m_subWndList.Add(new SystemOBCtrlWnd());
            m_subWndList.Add(new SystemVedioWnd());
            m_subWndList.Add(new SystemAudioWnd());
            m_subWndList.Add(new SystemDisplayWnd());
            m_subWndList.Add(new SystemGameWnd());

            SetVisible(false);

            //注册系统设置面板显示事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, this);
            //注册系统设置切页事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SWITCH, this);
            //注册系统设置保存事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SAVE, this);
            //注册系统设置恢复默认事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE, this);
            //注册退出游戏事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME, this);
            //注册重置技能控制页面事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_RESTORE, this);
            //注册刷新技能控制页面事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_REFRESH, this);
            //注册应用图像设置数据事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this); 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SWITCH, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_RESTORE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_REFRESH, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

            m_subWndList.Clear();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_SYSTEMSETTING_SHOW:
                    {
                        if (StageManager.Instance.getCurrentState() == Stage_ManagerDef.EStage_State.ESS_LOGIN)
                            QuitApplication();
                        else
                            SetVisible(!IsVisible());
                    }
                    break;
                //切页，分发消息到子页面
                case WndMsgID.WND_MSG_SYSTEMSETTING_SWITCH:
                    foreach (IUISubWnd wnd in m_subWndList)
                    {
                        wnd.OnMessage(msgID, data);
                    }
                    break;
                //保存按钮
                case WndMsgID.WND_MSG_SYSTEMSETTING_SAVE:
                    foreach (IUISubWnd wnd in m_subWndList)
                    {
                        wnd.OnMessage(msgID, data);
                    }
                    break;
                //重置按钮
                case WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE:
                    foreach (IUISubWnd wnd in m_subWndList)
                    {
                        wnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME:
                    {
                        QuitGame();
                    }
                    break;
                    //接受图像设置数据，并应用到UI上
                case WndMsgID.WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING:
                    {
                        IUISubWnd vedioWnd = GetSubWnd(SystemSettingSubWndType.WND_VEDIO);
                        if (vedioWnd != null)
                            vedioWnd.OnMessage(msgID, data);
                    }
                    break;
                //重置技能控制页面
                case WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_RESTORE:
                    {
                        IUISubWnd controlWnd = GetSubWnd(SystemSettingSubWndType.WND_CONTROL_DEFAULT);
                        if (controlWnd != null)
                            controlWnd.OnMessage(msgID, data);
                    }
                    break;
                //刷新技能控制页面
                case WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_REFRESH:
                    {
                        IUISubWnd controlWnd = GetSubWnd(SystemSettingSubWndType.WND_CONTROL_DEFAULT);
                        if (controlWnd != null)
                            controlWnd.OnMessage(msgID, data);
                    }
                    break;
                //载入窗口后，默认切换到控制页面
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        this.LoadView();
                        SwitchWndPage(SystemSettingFrameButtonType.BTN_CONTROL);

                        bInWar = false;
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        bInWar = true;
                        if (m_wndView != null)
                        {
                            IUISubWnd sysChartWnd = GetSubWnd(SystemSettingSubWndType.WND_GAME);
                            if (sysChartWnd != null)
                                sysChartWnd.OnMessage(msgID, data);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        bInWar = false;
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if (m_wndView != null)
                        {
                            IUISubWnd sysChartWnd = GetSubWnd(SystemSettingSubWndType.WND_GAME);
                            if (sysChartWnd != null)
                                sysChartWnd.OnMessage(msgID, data);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if(visible)
                ShowSubWnd(m_currentWndType);
        }

        #region 子页面管理
        //获取当前页面Wnd类型
        public SystemSettingSubWndType GetCurrentWndType()
        {
            return m_currentWndType;
        }

        //获取指定的页面Wnd
        private IUISubWnd GetSubWnd(SystemSettingSubWndType wndType)
        {
            foreach (var wnd in m_subWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    return wnd;
            }
            return null;
        }

        //打开指定的页面Wnd
        private void ShowSubWnd(SystemSettingSubWndType wndType)
        {
            if(wndType == SystemSettingSubWndType.WND_CONTROL_DEFAULT || wndType == SystemSettingSubWndType.WND_CONTROL_OB)
            {
                if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB && StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAR)
                {
                    wndType = SystemSettingSubWndType.WND_CONTROL_OB;
                }
                else
                {
                    wndType = SystemSettingSubWndType.WND_CONTROL_DEFAULT;
                }
            }

            foreach (var wnd in m_subWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    wnd.SetVisible(true);
                else
                    wnd.SetVisible(false);
            }
            m_currentWndType = wndType;
        }

        //切换页面Wnd
        public void SwitchWndPage(SystemSettingFrameButtonType pageType)
        {
            if (m_wndView == null)
                return;
            USystemSettingData data = new USystemSettingData();
            data.wndType = m_currentWndType;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SWITCH, data);
            switch (pageType)
            {
                case SystemSettingFrameButtonType.BTN_CONTROL:
                    {
                        if(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB && StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAR)
                        {
                            ShowSubWnd(SystemSettingSubWndType.WND_CONTROL_DEFAULT);
                        }
                        else
                        {
                            ShowSubWnd(SystemSettingSubWndType.WND_CONTROL_OB);
                        }
                    }
                    break;
                case SystemSettingFrameButtonType.BTN_VEDIO:
                    {
                        ShowSubWnd(SystemSettingSubWndType.WND_VEDIO);
                    }
                    break;
                case SystemSettingFrameButtonType.BTN_AUDIO:
                    {
                        ShowSubWnd(SystemSettingSubWndType.WND_AUDIO);
                    }
                    break;
                case SystemSettingFrameButtonType.BTN_DISPLAY:
                    {
                        ShowSubWnd(SystemSettingSubWndType.WND_DISPLAY);
                    }
                    break;
                case SystemSettingFrameButtonType.BTN_GAME:
                    {
                        ShowSubWnd(SystemSettingSubWndType.WND_GAME);
                    }
                    break;
                default:
                    break;
            }
            m_currentPageType = pageType;
        }

        //初始化子窗口的父容器
        public void InitSubWndContainer(Transform container)
        {
            foreach (var wnd in m_subWndList)
            {
                wnd.SetContainer(container);
                wnd.Init();
            }
        }
        #endregion 

        /// <summary>
        /// 退出游戏，分战场退出和场外退出两种情况
        /// </summary>
        public void QuitGame()
        {
            if (bInWar)
            {
                warQuitGame();
            }
            else
            {
                QuitApplication();
            }

        }

        #region 战场内退出游戏
        /// <summary>
        /// 战场内退出游戏
        /// </summary>
        private void warQuitGame()
        {
            EMHallRoomType eRoomType = (EMHallRoomType)GameLogicAPI.getCurRoomType();

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                  modal: true,
                  title: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitWarAsk"),
                  message: getWarExitDes(eRoomType),
                  buttons: new UIWidgets.DialogActions() {
                            { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfirmWarQuit },
                            { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), CancelWarQuit}
            });
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

        }

        public bool ConfirmWarQuit()
        {
            // 通知逻辑层释放连接
            if(GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP)
            {
                EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_EXIT_WAR);
            }
            else
            {
                ViewEventHelper.Instance.SendCommand(GameLogic.GameLogicDef.GVIEWCMD_USER_EXIT_WAR);
                SoldierCamera.MainInstance<SoldierCamera>().LeaveCurMode();
            }

            SetVisible(false);
            return true;
        }

        public bool CancelWarQuit()
        {
            return true;
        }

        public string getWarExitDes(EMHallRoomType eRoomType)
        {
            string sDes = "";
            switch (eRoomType)
            {
                case EMHallRoomType.HallRoomType_LegendCup:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarLegendCupQuit");
                    }
                    break;
                case EMHallRoomType.HallRoomType_MacthMachine:
                case EMHallRoomType.HallRoomType_Macth:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarMatchQuit");
                    }
                    break;

                case EMHallRoomType.HallRoomType_Custom:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarCustomerQuit");
                    }
                    break;

                default:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarCustomerQuit");
                    }
                    break;
            }

            return sDes;
        }
        #endregion

        #region 战场外退出游戏
        private bool bOpenQuitDialog = false;
        private void QuitApplication()
        {
            if (bOpenQuitDialog)
                return;

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                  modal: true,
                  title: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitConfirm"),
                  message: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitGameAsk"),
                  closeButtonAction: () => { CancelQuit(); },
                  modalClickAction: () => { CancelQuit(); },
                  buttons: new UIWidgets.DialogActions() {
                            { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfirmQuit },
                            { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), CancelQuit}
            });
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            bOpenQuitDialog = true;
        }

        private bool ConfirmQuit()
        {
            Initialize.Instance.ForbidQuit = false;
            Application.Quit();
            return true;
        }

        private bool CancelQuit()
        {
            bOpenQuitDialog = false;
            return true;
        }
       #endregion
    }

    public class SystemSettingPanelWndView : UIBaseWndView
    {
        public Transform contentFrame;

        private SystemSettingPanelWnd panelWnd { get { return m_wnd as SystemSettingPanelWnd;  } }
        private bool bRestWndOpen = false;
        private bool bSaveWndOpen = false;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            //初始化父容器
            if (panelWnd != null)
                panelWnd.InitSubWndContainer(contentFrame);

            return true;
        }

        #region 子页面Button点击事件
        public void OnControlClick(bool bActive)
        {
            if (!bActive)
                return;

            panelWnd.SwitchWndPage(SystemSettingFrameButtonType.BTN_CONTROL);
        }

        public void OnVedioClick(bool bActive)
        {
            if (!bActive)
                return;

            panelWnd.SwitchWndPage(SystemSettingFrameButtonType.BTN_VEDIO);
        }

        public void OnAudioClick(bool bActive)
        {
            if (!bActive)
                return;

            panelWnd.SwitchWndPage(SystemSettingFrameButtonType.BTN_AUDIO);
        }

        public void OnDisplayClick(bool bActive)
        {
            if (!bActive)
                return;

            panelWnd.SwitchWndPage(SystemSettingFrameButtonType.BTN_DISPLAY);
        }

        public void OnGameClick(bool bActive)
        {
            if (!bActive)
                return;

            panelWnd.SwitchWndPage(SystemSettingFrameButtonType.BTN_GAME);
        }
        #endregion

        #region 设置界面按钮
        public void OnCloseBtnClick()
        {
            SetVisible(false);
        }

        public void OnQuitGameBtnClick()
        {
            panelWnd.QuitGame();
        }

        public void OnRestoreBtnClick()
        {
            if (bRestWndOpen)
                return;

            //重置游戏设置数据
            //xmlNode.AllRestoreDefaultGameSettings();
            GameSettingConfig.Instance.RestGameSetting();
            SlotKeyCodeConfig.Instance.RestSlotData();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE, null);

            // 恢复默认按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "系统设置",
                                   message: "已恢复系统默认设置",
                                   closeButtonAction: () => { CloseRestWnd(); },
                                   buttons: new UIWidgets.DialogActions() { { "确认", CloseRestWnd } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            bRestWndOpen = true;
        } 

        public void OnSaveBtnClick()
        {
            if (bSaveWndOpen)
                return;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SAVE, null);
            //保存游戏设置更改的数据
            //xmlNode.SaveGameSettingsDirty();
            //SetVisible(false);

            // 确定按钮点击
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                   title: "系统设置",
                                   message: "保存成功",
                                   closeButtonAction: () => { CloseSaveWnd(); },
                                   buttons: new UIWidgets.DialogActions() { { "确认", CloseSaveWnd } }
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            bSaveWndOpen = true;
        }
        #endregion

        private bool CloseRestWnd()
        {
            bRestWndOpen = false;
            return true;
        }

        private bool CloseSaveWnd()
        {
            bSaveWndOpen = false;
            return true;
        }
    }
}
