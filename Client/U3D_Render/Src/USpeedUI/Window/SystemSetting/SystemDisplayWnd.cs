using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.GameSettingConfig;
using USpeedUI.Blood;

namespace USpeedUI.SystemSetting
{
    public class SystemDisplayWnd : UIBaseSubWnd<SystemDisplayWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_DISPLAY;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/DisplayView";
        }

        public override bool Init()
        {
            base.Init();

            SetVisible(false);
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
                default:
                    break;
            }
        }
    }

    //显示项目分类
    [Serializable]
    public enum DisplayType
    {
        RoleName,
        KinName,
        ClanName,
        SelfBlood,
        BloodInfo,
        OBTopInfo,
        OBViewUI,
    }

    [Serializable]
    public class DisplayItem
    {
        public DisplayType type;
        public Text txt;
        public bool bShow;
        public int nValue;
    }

    public enum EOBViewUIType : int
    {
        EOBUI_ALL,
        EOBUI_PART,
        EOBUI_NONE,
    }


    public class SystemDisplayWndView : UIBaseSubWndView
    {
        public List<DisplayItem> m_DisplayItem = new List<DisplayItem>();
        public GameObject obPart;
        public Text roleName;
        public Text kinName;
        public Text clanName;
        public Text selfBlood;
        public Text bloodInfo;
        public Text obTopInfo;
        public Text obViewUI;

        private string[] switchDis = new string[2];
        private string[] obViewUILabel = new string[3];

        public override bool Init(IUISubWnd wnd)
        {
            switchDis[0] = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "switchOn");
            switchDis[1] = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "switchOff");
            obViewUILabel[0] = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "obViewUIAll");
            obViewUILabel[1] = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "obViewUIPart");
            obViewUILabel[2] = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "obViewUINone");
            roleName.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "roleName");
            kinName.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "kinName");
            clanName.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "clanName");
            selfBlood.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "selfBlood");
            bloodInfo.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "bloodInfo");
            obTopInfo.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "obTopInfo");
            obViewUI.text = ULocalizationService.Instance.Get("UIView", "SystemDisplayView", "obViewUI");

            InitDisplayData();

            return base.Init(wnd);
        }

        public override void SetVisible(bool visible)
        {
            if (!visible)
                RestoreSettingData();

            base.SetVisible(visible);

            obPart.SetActive(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB);
        }

        public void RestoreSettingData()
        {
            InitDisplayData();
            foreach (DisplayItem item in m_DisplayItem)
            {
                if (item.type >= DisplayType.RoleName && item.type < DisplayType.SelfBlood)
                {
                    UTopNameManager.Instance.SwitchNameShow((int)item.type, item.bShow);
                }
                else if (item.type == DisplayType.SelfBlood)
                {
                    UBloodManager.Instance.ChangePlayerBlood(item.bShow);
                }
                else if (item.type == DisplayType.BloodInfo)
                {
                    UBloodManager.Instance.ChangeBloodInfoVisible(item.bShow);
                }
                else if (item.type == DisplayType.OBTopInfo)
                {
                    UBloodManager.Instance.ChangeCanvasVisible(item.bShow);
                }
            }
        }



        #region 切换按钮点击事件
        public void OnSideBtnClick(int type)
        {
            switch (type)
            {
                case (int)DisplayType.RoleName:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.ShowRoleName, result.ToString());
                                UTopNameManager.Instance.SwitchNameShow((int)DisplayType.RoleName, item.bShow);
                            }
                        }
                    }
                    break;
                case (int)DisplayType.KinName:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.ShowKinName, result.ToString());
                                UTopNameManager.Instance.SwitchNameShow((int)DisplayType.KinName, item.bShow);
                            }
                        }
                    }
                    break;
                case (int)DisplayType.ClanName:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.ShowClanName, result.ToString());
                                UTopNameManager.Instance.SwitchNameShow((int)DisplayType.ClanName, item.bShow);
                            }
                        }
                    }
                    break;
                case (int)DisplayType.SelfBlood:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.ShowSelfBlood, result.ToString());
                                UBloodManager.Instance.ChangePlayerBlood(item.bShow);
                            }
                        }
                    }
                    break;
                case (int)DisplayType.BloodInfo:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.ShowBloodNum, result.ToString());
                                UBloodManager.Instance.ChangeBloodInfoVisible(item.bShow);
                            }
                        }
                    }
                    break;
                case (int)DisplayType.OBTopInfo:
                    {
                        foreach (DisplayItem item in m_DisplayItem)
                        {
                            if (item.type == (DisplayType)type)
                            {
                                item.txt.text = item.bShow ? switchDis[1] : switchDis[0];
                                item.bShow = !item.bShow;
                                int result = item.bShow ? 1 : 0;
                                UBloodManager.Instance.ChangeCanvasVisible(item.bShow);
                            }
                        }
                    }
                    break;
            }
        }

        public void ToggleOBViewUILeft()
        {
            foreach (DisplayItem item in m_DisplayItem)
            {
                if (item.type == DisplayType.OBViewUI)
                {
                    item.nValue--;
                    if (item.nValue < 0)
                        item.nValue = 2;

                    item.txt.text = obViewUILabel[item.nValue];
                    ChangeOBViewUI(item.nValue);
                }
            }
        }

        public void ToggleOBViewUIRight()
        {
            foreach (DisplayItem item in m_DisplayItem)
            {
                if (item.type == DisplayType.OBViewUI)
                {
                    item.nValue++;
                    if (item.nValue > 2)
                        item.nValue = 0;

                    item.txt.text = obViewUILabel[item.nValue];
                    ChangeOBViewUI(item.nValue);
                }
            }
        }
        #endregion

        #region 初始化数据
        private void InitDisplayData()
        {
            foreach (DisplayItem item in m_DisplayItem)
            {
                switch (item.type)
                {
                    case DisplayType.RoleName:
                        {
                            item.bShow = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowRoleName);
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.KinName:
                        {
                            item.bShow = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowKinName);
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.ClanName:
                        {
                            item.bShow = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowClanName);
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.SelfBlood:
                        {
                            item.bShow = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowSelfBlood);
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.BloodInfo:
                        {
                            item.bShow = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowBloodNum);
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.OBTopInfo:
                        {
                            item.bShow = true;
                            item.txt.text = item.bShow ? switchDis[0] : switchDis[1];
                        }
                        break;
                    case DisplayType.OBViewUI:
                        {
                            item.nValue = 0;
                            item.txt.text = obViewUILabel[item.nValue];
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        #endregion

        private void ChangeOBViewUI(int _viewType)
        {
            switch (_viewType)
            {
                case (int)EOBViewUIType.EOBUI_ALL:
                    {
                        ShowOBPartUI(false);
                        RectTransform rt = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_FIXED);
                        rt.gameObject.SetActive(true);
                    }
                    break;
                case (int)EOBViewUIType.EOBUI_PART:
                    {
                        ShowOBPartUI(true);
                    }
                    break;
                case (int)EOBViewUIType.EOBUI_NONE:
                    {
                        ShowOBPartUI(false);
                        RectTransform rt = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_FIXED);
                        rt.gameObject.SetActive(false);
                    }
                    break;
            }
        }

        private void ShowOBPartUI(bool _showPart)
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_GAMEHUDTOPRIGHTVIEW, !_showPart);

            UIOBHideFramePart hidePart = new UIOBHideFramePart();
            hidePart.frame = _showPart ? (int)USpeedUI.WarObserver.WarOBUIPartType.BUTTEN_LEFT_PART : 0;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_OBSERVER_HIDE_FRAME_PART, hidePart);
        }
    }
}
