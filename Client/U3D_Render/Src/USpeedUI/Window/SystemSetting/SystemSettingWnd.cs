//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using UnityEngine;
//using UnityEngine.UI;
//using USpeedUI;
//using DataCenter;
//using USpeedUI.TooltipAndDialog;
//using Match_ManagerDef;

//namespace USpeedUI.SystemSetting
//{
//    public class SystemSettingWnd : UIPopupWnd<SystemSettingWndView>
//    {
//        private bool bInWar;
//        public override WndID GetID()
//        {
//            return WndID.WND_ID_SYSTEMSETTING;
//        }

//        public override string GetPrefabPath()
//        {
//            return "SystemSetting/SystemSettingFrame";
//        }

//        public override WndLayerDef GetLayer()
//        {
//            return WndLayerDef.WND_LAYER_POPUP02;
//        }

//        public override bool Init()
//        {
//            m_isKeyClose = true;
//            bInWar = false;

//            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
//            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
//            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
//            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, this);
//            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME, this);
//            //UISystem.Instance.RegisterKeyMessage(KeyCode.Escape, KeyStatus.Up, this);

//            return base.Init();
//        }

//        public override void Destroy()
//        {
//            base.Destroy();
//            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
//            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
//            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
//            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, this);
//            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME, this);
//            //UISystem.Instance.UnregisterKeyMessage(KeyCode.Escape, KeyStatus.Up, this);
//        }

//        public override void OnMessage(WndMsgID msgID, UIMsgData data)
//        {
//            switch (msgID)
//            {
//                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
//                    {
//                        this.LoadView();
//                        bInWar = false;
//                    }
//                    break;
//                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
//                    {
//                        bInWar = true;
//                    }
//                    break;
//                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
//                    {
//                        bInWar = false;
//                    }
//                    break;
//                case WndMsgID.WND_MSG_SYSTEMSETTING_SHOW:
//                    {
//                        if (StageManager.Instance.getCurrentState() == Stage_ManagerDef.EStage_State.ESS_LOGIN)
//                            QuitApplication();
//                        else
//                            SetVisible(!IsVisible());
//                    }
//                    break;
//                case WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME:
//                    QuitGame();
//                    break;
//                default:
//                    break;
//            }
//        }

//        public void QuitGame()
//        {
//            if (bInWar)
//            {
//                warQuitGame();
//            }
//            else
//            {
//                QuitApplication();
//            }
                
//        }

//        private void warQuitGame()
//        {
//            EMHallRoomType eRoomType = (EMHallRoomType)GameLogicAPI.getCurRoomType();

//            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
//                  modal: true,
//                  title: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitWarAsk"),
//                  message: getWarExitDes(eRoomType),
//                  buttons: new UIWidgets.DialogActions() {
//                            { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfirmWarQuit },
//                            { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), CancelWarQuit}
//            });
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

//        }

//        public bool ConfirmWarQuit()
//        {
//            // 通知逻辑层释放连接
//            ViewEventHelper.Instance.SendCommand(GameLogic.GameLogicDef.GVIEWCMD_USER_EXIT_WAR);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_EXIT_WAR, null);
//            SoundManager.DeleteAll();

//            SetVisible(false);
//            return true;
//        }

//        public bool CancelWarQuit()
//        {
//            return true;
//        }

//        public string getWarExitDes(EMHallRoomType eRoomType)
//        {
//            string sDes = "";
//            switch (eRoomType)
//            {
//                case EMHallRoomType.HallRoomType_LegendCup:
//                    {
//                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarLegendCupQuit");
//                    }
//                    break;
//                case EMHallRoomType.HallRoomType_MacthMachine:
//                case EMHallRoomType.HallRoomType_Macth:
//                    {
//                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarMatchQuit");
//                    }
//                    break;

//                case EMHallRoomType.HallRoomType_Custom:
//                    {
//                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarCustomerQuit");
//                    }
//                    break;

//                default:
//                    {
//                        sDes = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "WarCustomerQuit");
//                    }
//                    break;
//            }

//            return sDes;
//        }

//        private bool bOpenQuitDialog = false;
//        private void QuitApplication()
//        {
//            if (bOpenQuitDialog)
//                return;

//            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
//                  modal: true,
//                  title: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitConfirm"),
//                  message: ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitGameAsk"),
//                  closeButtonAction: () => { CancelQuit(); },
//                  modalClickAction: ()=> { CancelQuit(); },
//                  buttons: new UIWidgets.DialogActions() {
//                            { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfirmQuit },
//                            { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), CancelQuit}
//            });
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
//            bOpenQuitDialog = true;
//        }

//        private bool ConfirmQuit()
//        {
//            Initialize.Instance.ForbidQuit = false;
//            Application.Quit();
//            return true;
//        }

//        private bool CancelQuit()
//        {
//            bOpenQuitDialog = false;
//            return true;
//        }
//    }

//    public class SystemSettingWndView : UIBaseWndView
//    {
//        public Text frameLabel;
//        public Text controlBtnLabel;
//        public Text videoBtnLabel;
//        public Text audioBtnLabel;
//        public Text displayBtnLabel;
//        public Text gameBtnLabel;
//        //public Text admitBtnLabel;
//        public Text QuitBtnLabel;
//        //public Text returnBtnLabel;
//        //public Button admitBtn;
//        //public Button[] settingBtnGroup;

//        private SystemSettingWnd systemSettingWnd { get { return m_wnd as SystemSettingWnd; } }
//        //public bool isMainTown = false;

//        public override bool Init(IUIWnd wnd)
//        {
//            frameLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "frameLabel");
//            controlBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "controlBtnLabel");
//            videoBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "videoBtnLabel");
//            audioBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "audioBtnLabel");
//            displayBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "displayBtnLabel");
//            gameBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "gameBtnLabel");
//            //admitBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "admitBtnLabel");
//            QuitBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "QuitBtnLabel");
//            //returnBtnLabel.text = ULocalizationService.Instance.Get("UIView", "SystemSettingView", "returnBtnLabel");

//            U3D_Render.EntityInputHandler.MouseButtonDownEntityEvent += MouseButtonDown;
//            return base.Init(wnd);
//        }

//        public override void Destroy()
//        {
//            U3D_Render.EntityInputHandler.MouseButtonDownEntityEvent -= MouseButtonDown;
//            base.Destroy();
//        }

//        /// <summary>
//        /// 鼠标事件处理，如果FPS英雄按下鼠标左键，则隐藏ESC
//        /// </summary>
//        /// <param name="button"></param>
//        public void MouseButtonDown(MouseButton button)
//        {
//            if (button == MouseButton.MouseBtn_Left && IsVisible())
//            {
//                m_wnd.SetVisible(false);
//            }
//        }

//        public void OnControlBtnClick()
//        {
//            UIButtonType btnType = new UIButtonType((int)SystemSettingFrameButtonType.BTN_CONTROL);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTINGPANEL_OPEN, btnType);
//        }

//        public void OnVedioBtnClick()
//        {
//            UIButtonType btnType = new UIButtonType((int)SystemSettingFrameButtonType.BTN_VEDIO);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTINGPANEL_OPEN, btnType);
//        }

//        public void OnAudioBtnClick()
//        {
//            UIButtonType btnType = new UIButtonType((int)SystemSettingFrameButtonType.BTN_AUDIO);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTINGPANEL_OPEN, btnType);
//        }

//        public void OnDisplayBtnClick()
//        {
//            UIButtonType btnType = new UIButtonType((int)SystemSettingFrameButtonType.BTN_DISPLAY);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTINGPANEL_OPEN, btnType);
//        }

//        public void OnGameBtnClick()
//        {
//            UIButtonType btnType = new UIButtonType((int)SystemSettingFrameButtonType.BTN_GAME);
//            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTINGPANEL_OPEN, btnType);
//        }

//        public void OnCloseBtnClick()
//        {
//            SetVisible(false);
//        }

//        public void OnReturnGameBtnClick()
//        {
//            SetVisible(false);
//            UISystem.Instance.ShowWnd(WndID.WND_ID_SYSTEMSETTINGPANEL, false);
//        }

//        //如果在战场内，则退出战场，战场外则退出游戏
//        public void OnQuitGameBtnClick()
//        {
//            systemSettingWnd.QuitGame();
//        }
//    }
//}
