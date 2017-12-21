/*******************************************************************
** 文件名:	WarReliveTimeWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	战场连击及连杀界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using UnityEngine;
using UnityEngine.UI;
using System.Collections;

namespace USpeedUI.WarComboAttack
{
    public class UWarComboAttckWnd : UIBaseWnd<UWarComboAttackWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_COMBOATTACK;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarComboAttackView/WarComboAttackView";
        }

        public override bool Init()
        {
            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWATTACK, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWKILL, this);
            //订阅战场结束事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWATTACK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWKILL, this);
            //退订战场结束事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                //case WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWATTACK:
                //    {
                //        if (m_wndView == null)
                //            return;

                //        UIMsgCmdData uiShowComboAttackData = (UIMsgCmdData)msgData;
                //        cmd_creature_combo data = IntPtrHelper.toData<cmd_creature_combo>(uiShowComboAttackData.ptrParam);
                //        m_wndView.ShowComboAttack(data.nComboCount);
                //    }
                //    break;
                case WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWKILL:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiShowKillNumData = (UIMsgCmdData)msgData;
                        m_wndView.ShowComboKill(uiShowKillNumData.nParam);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UWarComboAttackWndView : UIBaseWndView, ITimerHanlder
    {
        public GameObject ComboKill;
        public GameObject ComboAttack;
        public Image ComboKillIcon;
        public Image ComboAttackIcon;
        public Image ComboAttackHundred;
        public Image ComboAttackTen;
        public Image ComboAttackOne;
        
        enum EN_WarCombo_Timer
        {
            ENWT_AttackTimer = 0,     // 游戏时间定时器id
            ENWT_KillTimer,
        }

        public override bool Init(IUIWnd wnd)
        {
            TimerManager.SetTimer(this, (int)EN_WarCombo_Timer.ENWT_AttackTimer, 5.0f);
            TimerManager.SetTimer(this, (int)EN_WarCombo_Timer.ENWT_KillTimer, 5.0f);
                
            return base.Init(wnd);
        }

        public override void Destroy()
        {
            TimerManager.KillTimer(this, (int)EN_WarCombo_Timer.ENWT_AttackTimer);
            TimerManager.KillTimer(this, (int)EN_WarCombo_Timer.ENWT_KillTimer);

            base.Destroy();
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_WarCombo_Timer.ENWT_AttackTimer:
                    {
                        ComboAttack.SetActive(false);
                    }
                    break;
                case (int)EN_WarCombo_Timer.ENWT_KillTimer:
                    {
                        ComboKill.SetActive(false);
                    }
                    break;
                default:
                    break;
            }
        }

        public void ShowComboAttack(int nAttackNum)
        {
            int nComboAttackOffset = (int)EDynamicStateMoba.EDSM_ComboAttack_NumZeroIcon;
          
            ComboAttackOne.sprite = GetSprite(nAttackNum % 10 + nComboAttackOffset);
            ComboAttackTen.gameObject.SetActive(false);
            ComboAttackHundred.gameObject.SetActive(false);
            if (nAttackNum >= 10 && nAttackNum < 100)
            {                
                ComboAttackTen.sprite = GetSprite(nAttackNum / 10 + nComboAttackOffset);
                ComboAttackTen.gameObject.SetActive(true);
            }
            if (nAttackNum >= 100 && nAttackNum < 1000)
            {
                ComboAttackHundred.sprite = GetSprite(nAttackNum / 100 + nComboAttackOffset);
                ComboAttackTen.sprite = GetSprite((nAttackNum / 10) % 10 + nComboAttackOffset);
                ComboAttackHundred.gameObject.SetActive(true);
                ComboAttackTen.gameObject.SetActive(true);
            }
            ComboAttackIcon.sprite = GetSprite((int)EDynamicStateMoba.EDSM_ComboAttack_ScoreIcon);
            ComboAttack.SetActive(true);
            TimerManager.KillTimer(this, (int)EN_WarCombo_Timer.ENWT_AttackTimer);
            TimerManager.SetTimer(this, (int)EN_WarCombo_Timer.ENWT_AttackTimer, 5.0f);
        }

        public void ShowComboKill(int nKillNum)
        {
            int nComboKillOffset = (int)EDynamicStateMoba.EDSM_ComboAttack_OneKillIcon - 1;

            if (nKillNum >= 1 && nKillNum <= 5)
            {
                ComboKillIcon.sprite = GetSprite(nKillNum + nComboKillOffset);
                ComboKill.SetActive(true);
                TimerManager.KillTimer(this, (int)EN_WarCombo_Timer.ENWT_KillTimer);
                TimerManager.SetTimer(this, (int)EN_WarCombo_Timer.ENWT_KillTimer, 5.0f);
            }
        }

        private Sprite GetSprite(int nIndex)
        {
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_DynamicState_Moba, WndID.WND_ID_WAR_COMBOATTACK, nIndex); 
        }
    }
}
