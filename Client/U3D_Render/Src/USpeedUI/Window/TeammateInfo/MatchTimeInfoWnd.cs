/*******************************************************************
** 文件名:	UMatchInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	匹配信息界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using DG.Tweening;
using UnityEngine;
using GameLogic;
using Match_ManagerDef;
using U3D_Render.Common;

namespace USpeedUI.TeammateInfo
{
    enum EN_MatchInfo_Timer
    {
        ENMT_UsedTimer = 0,     // 已用匹配时间定时器id
        ENMT_LeftTimer,         // 倒计时id
    }

    public class UMatchTimeInfoWnd : UIPopupWnd<UMatchTimeInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_MATCHTIMEINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Teammate/MatchTimeInfoView";
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

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_HIDE, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_HIDE, this);
            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                        cmd_add_match_info data = IntPtrHelper.toData<cmd_add_match_info>(cmdData.ptrParam);
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = true;
                        m_wndView.SetVisible(true);
                        m_wndView.StartTimer((int)EN_MatchInfo_Timer.ENMT_UsedTimer, data);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_LeftTimer);

                    }
                    break;
                case WndMsgID.WND_MSG_ACCEPTWAIT_SHOW:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_UsedTimer);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_LeftTimer);
                        
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE:
                    {
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_UsedTimer);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_LeftTimer);
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_CUR_MATCH_PLAYER_COUNT:
                    {
                        if (!LogicDataCenter.viewPersonStateDataManager.bIsMatching)
                        {
                            return;
                        }

                        if (m_wndView == null)
                            return;
                        // 显示玩家人数倒计时间
                        UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                        cmd_match_cur_player_count data = IntPtrHelper.toData<cmd_match_cur_player_count>(cmdData.ptrParam);
                        m_wndView.UpdateCurPlayerCount(data.nCurCount, data.nMaxCount, data.nLeftTime);
                    }
                    break;
                    
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER:
                    {
                        LogicDataCenter.viewPersonStateDataManager.bIsMatching = false;
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_UsedTimer);
                        m_wndView.KillTimer((int)EN_MatchInfo_Timer.ENMT_LeftTimer);
                        
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_SHOW:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.SetReturnMatchState();

                        m_wndView.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_RETURN_MATCH_HIDE:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                    }
                    break;
                #region 新手引导部分
                case WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONBEGIN:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.GuideActionBegin();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONEND:
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
    }

    public class UMatchTimeInfoWndView : UIBaseWndView, ITimerHanlder
    {
        public Text Title;
        public Text PlayerCountTitle;
        public Text UsedTimeTitle;
        public Text CurPlayerCountTxt;
        public Text UsedTime;
        public Button CloseBtn;
        public Image RotateIcon;

        public Button ReturnMatchBtn;
        public Text ReturnMatchBtnDesc;

        //private Tweener m_tween = null;
        private int m_CurMatchTimeSec;
        private int m_nCurPlayerCount;
        private int m_nLeftTimeSec;
        private int m_nMaxPlayerCount;

        public override bool Init(IUIWnd wnd)
        {
            //Title.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "Title");
            PlayerCountTitle.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "PlayerCountTitle");
            UsedTimeTitle.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "UsedTimeTitle");
            RotateIcon.gameObject.SetActive(false);

            return base.Init(wnd);
        }

        public void StartTimer(int nTimerID, cmd_add_match_info cmdData)
        {
            ReturnMatchBtn.gameObject.SetActive(false);

            UsedTime.text = "00 : 00";
            m_CurMatchTimeSec = 0;
            m_nCurPlayerCount = cmdData.nCurMatchPlayerCount;
            m_nMaxPlayerCount = cmdData.nMaxPlayerCount;
            CurPlayerCountTxt.text = m_nCurPlayerCount.ToString();
            // 更新当前玩家数量
            CurPlayerCountTxt.text = m_nCurPlayerCount + " / " + m_nMaxPlayerCount;
            TimerManager.SetTimer(this, nTimerID, 1.0f);
            switch (cmdData.nMatchTypeID)
            {
                case (int)EMMatchType.MatchType_MachineMatch:
                    {
                        Title.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "MachineTitle");
                    }
                    break;
                case (int)EMMatchType.MatchType_First5Match:
                case (int)EMMatchType.MatchType_Match:
                    {
                        Title.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "MatchTitle");
                    }
                    break;
                case (int)EMMatchType.MatchType_Rank:
                    {
                        Title.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "RankTitle");
                    }
                    break;
                case (int)EMMatchType.MatchType_MultiCampMatch:
                    {
                        Title.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "SurvivalTitle");
                    }
                    break;
                default:
                    break;
            }

            //if (m_tween == null)
            //{
            //    m_tween = RotateIcon.transform.DORotate(new Vector3(0.0f, 0.0f, 360.0f), 0.5f, RotateMode.FastBeyond360).SetEase(Ease.Linear).SetLoops(-1);
            //}
        }

        public void UpdateCurPlayerCount(int nPlayerCount, int nMaxPlayerCount, int nLeftTime)
        {
            m_nLeftTimeSec = nLeftTime;
            if (nLeftTime > 0)
            {
                TimerManager.KillTimer(this, (int)EN_MatchInfo_Timer.ENMT_UsedTimer);
                TimerManager.SetTimer(this, (int)EN_MatchInfo_Timer.ENMT_LeftTimer, 1.0f);
                UsedTimeTitle.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "LeftTimeTitle");
            }
            else
            {
                UsedTimeTitle.text = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "UsedTimeTitle");
            }

            m_nCurPlayerCount = nPlayerCount;
            m_nMaxPlayerCount = nMaxPlayerCount;
            // 更新当前玩家数量
            CurPlayerCountTxt.text = m_nCurPlayerCount + " / " + m_nMaxPlayerCount;
            
        }

        public void KillTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, nTimerID);

            //if (m_tween != null)
            //{
            //    m_tween.Kill();
            //    m_tween = null;

            //    RotateIcon.transform.localRotation = new Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
            //} 
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_MatchInfo_Timer.ENMT_UsedTimer:
                    {
                        m_CurMatchTimeSec++;

                        int nMin, nSec;
                        string strMin, strSec;
                        nMin = m_CurMatchTimeSec / 60;
                        nSec = m_CurMatchTimeSec % 60;
                        strMin = nMin.ToString();
                        strSec = nSec.ToString();
                        if (nMin / 10 < 1)
                            strMin = "0" + strMin;
                        if (nSec / 10 < 1)
                            strSec = "0" + strSec;
                            
                        // 更新匹配时间
                        UsedTime.text = strMin + " : " + strSec;

                    }
                    break;
                case (int)EN_MatchInfo_Timer.ENMT_LeftTimer:
                    {
                        if (m_nLeftTimeSec <= 0)
                        {
                            break;
                        }
                        m_nLeftTimeSec--;
                        int nMin, nSec;
                        string strMin, strSec;
                        nMin = m_nLeftTimeSec / 60;
                        nSec = m_nLeftTimeSec % 60;
                        strMin = nMin.ToString();
                        strSec = nSec.ToString();
                        if (nMin / 10 < 1)
                            strMin = "0" + strMin;
                        if (nSec / 10 < 1)
                            strSec = "0" + strSec;
                            
                        // 更新匹配时间
                        UsedTime.text = strMin + " : " + strSec;

                    }
                    break;
                    
                default:
                    break;
            }
        }

        public void SetReturnMatchState()
        {
            string str = "返回匹配邀请界面<br>暂未开始匹配";
            UBB.toHtml(ref str);
            ReturnMatchBtnDesc.text = str;
            ReturnMatchBtn.gameObject.SetActive(true);
        }

        public void OnClickReturnMatchButton()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_INVITE_RETURN);

            SetVisible(false);
        }

        public void OnCloseButton()
        {
            LogicDataCenter.matchDataManager.OnCancelMatch();
            //SetVisible(false);
            //KillTimer((int)EN_MatchInfo_Timer.ENMT_UsedTimer);
            //KillTimer((int)EN_MatchInfo_Timer.ENMT_LeftTimer);
        }
        #region 新手引导部分

        internal void GuideActionBegin()
        {
            if(CloseBtn != null)
            {
                CloseBtn.gameObject.SetActive(false);
            }
        }

        internal void GuideActionEnd()
        {
            if (CloseBtn != null)
            {
                CloseBtn.gameObject.SetActive(true);
            }
        }
        #endregion
    }
}
