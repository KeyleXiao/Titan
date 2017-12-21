/*******************************************************************
** 文件名:	UMediatorTeamRight.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-15
** 版  本:	1.0
** 描  述:  进度条统一管理
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
namespace USpeedUI.ProgressBarPart
{
    public class ProgressBarWnd : UIPopupWnd<ProgressBarWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_PROGRESSBAR;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "CommomState/ProgressBar";
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

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_PROGRESSBAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);


            this.SetVisible(true);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_PROGRESSBAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);

        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_PROGRESSBAR:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        SetVisible(true);

                        UIMsgCmdData uiWarSkil = (UIMsgCmdData)msgData;
                        
                        SetWarSkillProgress(uiWarSkil.nParam);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        StopWarSkillProgress();

                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGPROGRESSBAR:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        SetVisible(true);
                        UIMsgCmdData uiWindData = (UIMsgCmdData)msgData;
                        SetWingProgress(uiWindData.nParam);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_WINGCLEARPROGRESSBAR:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        SetVisible(false);
                        StopWarWingProgress();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        SetVisible(false);
                    }
                    break;
                default:
                    break;
            }
        }

        private void StopWarSkillProgress()
        {
            m_wndView.StopWarSkillProgress();
        }

        private void SetWarSkillProgress(int nTime)
        {
            m_wndView.SetWarSkillProgress(nTime);
        }

        private void StopWarWingProgress()
        {
            m_wndView.StopWarWingProgress();
        }

        private void SetWingProgress(int nTime)
        {
            m_wndView.SetWingProgress(nTime);
        }

        public override bool IsVisible()
        {
            if (m_wndView == null)
                return true;

            return m_wndView.skillMainProgressBar.gameObject.activeSelf || m_wndView.skillWingProgressBar.gameObject.activeSelf;
        }
    }
}
