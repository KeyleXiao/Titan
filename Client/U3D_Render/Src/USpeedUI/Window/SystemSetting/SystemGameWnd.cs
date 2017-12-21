using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.GameSettingConfig;

namespace USpeedUI.SystemSetting
{
    public class SystemGameWnd : UIBaseSubWnd<SystemGameWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_GAME;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/GameView";
        }

        public override bool Init()
        {
            base.Init();
   
            SetVisible(true);
            return true;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE:
                    if (m_wndView != null)
                        m_wndView.RestoreSettingData();
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (m_wndView != null)
                            m_wndView.InitDisplayData();
                    }
                    break;
                default:
                    break;
            }
        }      
    }

    public class SystemGameWndView : UIBaseSubWndView
    {
        public Text MouseLockState;
        public Text MouseLockDesc;
        public Slider CamSpeedSlider;
        public Slider MouseSpeedSlider;

        private string[] LockStateDesc = new string[((int)EMouseLockModelType.EMLMT_INALL+1)];

        public override bool Init(IUISubWnd wnd)
        {
            LockStateDesc[(int)EMouseLockModelType.EMLMT_OFF] = ULocalizationService.Instance.Get("UIView", "SystemGameView", "Off");
            LockStateDesc[(int)EMouseLockModelType.EMLMT_INWAR] = ULocalizationService.Instance.Get("UIView", "SystemGameView", "WarMap");
            LockStateDesc[(int)EMouseLockModelType.EMLMT_INALL] = ULocalizationService.Instance.Get("UIView", "SystemGameView", "AllMap");
            MouseLockDesc.text = ULocalizationService.Instance.Get("UIView", "SystemGameView", "MouseLockDesc");

            InitDisplayData();
            return base.Init(wnd);
        }

        public void ToggleMouseLockLeft()
        {
            int nNextState = (int)Initialize.Instance.MouseLockModelType - 1;
            DoCorrectLockStateNum(ref nNextState);

            Initialize.Instance.MouseLockModelType = (EMouseLockModelType)nNextState;
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.MouseLockInWindow, nNextState.ToString());
            ChangeMouseLockState(Initialize.Instance.MouseLockModelType);
        }

        public void ToggleMouseLockRight()
        {
            int nNextState = (int)Initialize.Instance.MouseLockModelType + 1;
            DoCorrectLockStateNum(ref nNextState);

            Initialize.Instance.MouseLockModelType = (EMouseLockModelType)nNextState;
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.MouseLockInWindow, nNextState.ToString());
            ChangeMouseLockState(Initialize.Instance.MouseLockModelType);
        }

        public void RestoreSettingData()
        {
            InitDisplayData();
        }

        public void InitDisplayData()
        {
            ChangeMouseLockState((EMouseLockModelType)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.MouseLockInWindow));
            ChangeCamSpeed(GameSettingConfig.Instance.GetGameSettingFloatValue(GameSettingIndex.CameraSpeed));
            ChangeMouseSpeed(GameSettingConfig.Instance.GetGameSettingFloatValue(GameSettingIndex.MouseSensitivity));
        }

        #region 窗口锁鼠标
        public void ChangeMouseLockState(EMouseLockModelType eType)
        {
            Initialize.Instance.MouseLockModelType = eType;
            DoMouseLockInModeType();
        }

        public void DoMouseLockInModeType()
        {
            int nLockStateIndex = (int)Initialize.Instance.MouseLockModelType;
            if (nLockStateIndex < 0 || nLockStateIndex >= LockStateDesc.Length)
            {
                return;
            }

            if (IsOutOfLockStateNum(nLockStateIndex))
            {
                return;
            }

            MouseLockState.text = LockStateDesc[nLockStateIndex];

           // UnityEngine.Debug.Log("type" + LockStateDesc[(int)Initialize.Instance.MouseLockModelType]);
            Initialize.Instance.DoMouseLockInModeType();
        }

        public bool IsOutOfLockStateNum(int nLockStateIndex)
        {
            if (nLockStateIndex < 0 || nLockStateIndex >= LockStateDesc.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void DoCorrectLockStateNum(ref int nLockStateIndex)
        {
            if (nLockStateIndex < 0)
            {
                nLockStateIndex = LockStateDesc.Length - 1;
                return;
            }

            if (nLockStateIndex >= LockStateDesc.Length)
            {
                nLockStateIndex = 0;
                return;
            }
        }


        //public bool JudgeIsMouseLock()
        //{
        //    bool bMouseLock = false;
        //    switch (m_eMouseLockState)
        //    {
        //        case EMouseLockModelType.EMLMT_OFF:
        //            {
        //                bMouseLock = false;
        //            }
        //            break;
        //        case EMouseLockModelType.EMLMT_INALL:
        //            {
        //                bMouseLock = true;
        //            }
        //            break;
        //        case EMouseLockModelType.EMLMT_INWAR:
        //            {
        //                bMouseLock = GameLogicAPI.isInWarScene() > 0;
        //            }
        //            break;

        //        default:
        //            break;
        //    }

        //    Debug.Log("type" + LockStateDesc[(int)m_eMouseLockState] + "bMouseLock" + bMouseLock);
        //    return bMouseLock;
        //}
        #endregion

        public void ApplyCamSpeedData(float fValue)
        {
            if (fValue < 0)
                return;

            if(SoldierCamera.MainInstance() != null)
            {
                CameraProperty cp = SoldierCamera.MainInstance<SoldierCamera>().CurCameraProperty;
                if(cp != null)
                {
                    cp.SpeedScale = fValue;
                }
            }
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.CameraSpeed, fValue.ToString());
        }

        public void ApplyMouseSpeedData(float fValue)
        {
            int nValue = (int)fValue;
            if (nValue < 1 || nValue > 20)
                nValue = 10;

            IntPtr valuePtr = new IntPtr(nValue);

            try
            {
                GameLogicAPI.SystemParametersInfo((uint)ESystemParameters.SPI_SETMOUSESPEED, 0, valuePtr, 0);
            }
            catch
            {
                Trace.LogWarning("Mouse Sensitivity Set failed!");
            }

            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.MouseSensitivity, nValue.ToString());
        }

        public void ApplyMouseDefaultSpeed()
        {
            float fValue = GameSettingConfig.Instance.GetGameSettingDefaultFloatValue(GameSettingIndex.MouseSensitivity);

            ChangeMouseSpeed(fValue);
        }

        public void ApplyCamDefaultSpeed()
        {
            float fValue = GameSettingConfig.Instance.GetGameSettingDefaultFloatValue(GameSettingIndex.CameraSpeed);
            ChangeCamSpeed(fValue);
        }

        private void ChangeCamSpeed(float fValue)
        {
            if (CamSpeedSlider != null)
            {
                CamSpeedSlider.value = fValue;
            }

            ApplyCamSpeedData(fValue);
        }

        private void ChangeMouseSpeed(float fValue)
        {
            if (MouseSpeedSlider != null)
            {
                MouseSpeedSlider.value = fValue;
            }
            ApplyMouseSpeedData(fValue);
        }
    }
}
