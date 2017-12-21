/*******************************************************************
** 文件名:	UPunishTimeInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	显示惩罚时间相关
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using GameLogic;
using UnityEngine;
using Match_ManagerDef;
using U3D_Render.Common;

namespace USpeedUI.TeammateInfo
{
    enum EN_Punish_Timer
    {
        ENMT_CountDownTimer = 0,     // 倒计时计数器
    }

    public class UPunishTimeInfoWnd : UIPopupWnd<UPunishTimeInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_PUNISH_TIMEINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Teammate/PunishTimeInfoView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PUNISH_TIMEINFO_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PUNISH_TIMEINFO_HIDE, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PUNISH_TIMEINFO_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PUNISH_TIMEINFO_HIDE, this);

            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_Punish_Timer.ENMT_CountDownTimer);
                    }
                    break;
                case WndMsgID.WND_MSG_PUNISH_TIMEINFO_SHOW:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;

                        m_wndView.SetVisible(true);
                        m_wndView.KillTimer((int)EN_Punish_Timer.ENMT_CountDownTimer);
                        m_wndView.StartTimer((int)EN_Punish_Timer.ENMT_CountDownTimer, uiData.ptrParam, uiData.nPtrLen);
                    }
                    break;

                case WndMsgID.WND_MSG_PUNISH_TIMEINFO_HIDE:
                    {
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_Punish_Timer.ENMT_CountDownTimer);
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    public class UPunishTimeInfoWndView : UIBaseWndView, ITimerHanlder
    {
        public Text Title;
        public Text ConfirmText;
        public GameObject TeamFrame;
        public Text TeamPunishDes;
        public Text TeamPlayerName;
        public Text TeamPlayerPunishTime;

        public GameObject SelfFrame;
        public Text SelfPunishDes;
        public Text SelfPlayerPunishTime;
        private byte m_SelfPunishType;

        public Button CloseBtn;
        private Dictionary<string, int> m_dicLeftTimeShow = new Dictionary<string, int>();
        private bool m_isTeamRequest;
        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "Title");
            ConfirmText.text = ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn");
            return base.Init(wnd);
        }

        public void StartTimer(int nTimerID, IntPtr ptr, int nLen)
        {
            m_dicLeftTimeShow.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_match_request_punish_head head = helper.get<cmd_match_request_punish_head>();
            m_isTeamRequest = head.bIsTeamRequest == 0 ? false : true;
            for (int i = 0; i < head.nCount; i++)
            {
                // 提示显示
                cmd_match_request_punish_info data = helper.get<cmd_match_request_punish_info>();
                m_dicLeftTimeShow[data.szName] = data.nLeftTime;

                if (!m_isTeamRequest)
                {
                    m_SelfPunishType = data.byPunishType;
                }
            }

            ShowWndInfo();

            TimerManager.SetTimer(this, nTimerID, 1.0f);
        }

        public void ShowWndInfo()
        {
            // 显示哪个模块
            if (m_isTeamRequest)
            {
                TeamFrame.gameObject.SetActive(true);
                SelfFrame.gameObject.SetActive(false);
                ShowTeamInfo();
            }
            else
            {
                TeamFrame.gameObject.SetActive(false);
                SelfFrame.gameObject.SetActive(true);
                ShowSelfInfo();
            }


        }

        public void ShowTeamInfo()
        {
            if (m_dicLeftTimeShow.Count <= 0)
            {
                OnCloseButton();
                return;
            }

            string teamPlayerDes = "";
            string teamTimeDes = "";
            foreach (KeyValuePair<string, int> data in m_dicLeftTimeShow)
            {
                teamPlayerDes += data.Key + "<br>";
                teamTimeDes += LogicDataCenter.matchDataManager.GetLeftTimeStr(m_dicLeftTimeShow[data.Key]) + "<br>";
            }

            UBB.toHtml(ref teamPlayerDes);
            UBB.toHtml(ref teamTimeDes);
            TeamPlayerName.text = teamPlayerDes; 
            TeamPlayerPunishTime.text = teamTimeDes;
            TeamPunishDes.text = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "TeamMatchReqDes");

        }

        public void ShowSelfInfo()
        {
            if (m_dicLeftTimeShow.Count <= 0)
            {
                OnCloseButton();
                return;
            }

            foreach (KeyValuePair<string, int> data in m_dicLeftTimeShow)
            {
                SelfPlayerPunishTime.text  = LogicDataCenter.matchDataManager.GetLeftTimeStr(m_dicLeftTimeShow[data.Key]);
            }

            string strPunishDes = "";
            switch ((EPunishmentType)m_SelfPunishType)
            {
                case EPunishmentType.emPunishmentType_Plug:     // 外挂惩罚：使用外挂惩罚
                    {
                        strPunishDes = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "MatchReqPunishPlugDes");
                    }
                    break;
                case EPunishmentType.emPunishmentType_Match:    // 撮合惩罚：撮合退出相关
                    {
                        strPunishDes = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "MatchCancelPunishDes");
                    }
                    break;
                case EPunishmentType.emPunishmentType_War:      // 战场惩罚：挂机或者逃跑
                    {
                        strPunishDes = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarPunishDes");
                    }
                    break;
                case EPunishmentType.emPunishmentType_Accuse:   // 举报惩罚：玩家举报
                    {
                        strPunishDes = ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "AccusePunishDes");
                    }
                    break;
                default:
                    break;

            }
            UBB.toHtml(ref strPunishDes);
            SelfPunishDes.text = strPunishDes;
            
        }

        public void KillTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, nTimerID);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_Punish_Timer.ENMT_CountDownTimer:
                    {
                        List<string> removeKey = new List<string>();
                        List<string> listKey = new List<string>();
                        listKey.AddRange(m_dicLeftTimeShow.Keys);
                        foreach (string key in listKey)
                        {

                            m_dicLeftTimeShow[key]--;
                            if (m_dicLeftTimeShow[key] <= 0)
                            {
                                removeKey.Add(key);
                            }
                        }

                        for (int n = 0; n < removeKey.Count; ++n)
                        {
                            m_dicLeftTimeShow.Remove(removeKey[n]);
                        }

                        ShowWndInfo();

                    }
                    break;
                default:
                    break;
            }
        }

        public void OnCloseButton()
        {
            SetVisible(false);
        }
    }
}
