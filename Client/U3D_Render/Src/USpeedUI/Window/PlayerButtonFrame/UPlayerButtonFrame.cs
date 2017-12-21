/*******************************************************************
** 文件名:	UPlayerButtonFrame.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	所有系统的按钮
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using Stage_ManagerDef;
using DataCenter;

namespace USpeedUI.PlayerButtonFrame
{
    public enum ESimulateOpenViewType
    {
        ESOP_PLAYER_HERO,       // 其他地方模拟打开角色英雄界面
        ESOP_WAR_MAIN,          // 模拟打开匹配自定义界面
        ESOP_SHOP,              // 模拟打开商店

        ESOP_MAX,
    }

    public class UPlayerButtonFrame : UIBaseWnd<UPLayerButtonFrameView>
    {
        private UPLayerButtonFrameView view { get { return m_wndView as UPLayerButtonFrameView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYERBUTTONFRAME;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PlayerSysTem/PlayerSysButton";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        // 初始化
        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAIL_NEWMAILCOUNT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_REDPOINTTIP, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);

            return base.Init();
        }

        // 销毁
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAIL_NEWMAILCOUNT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_REDPOINTTIP, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);

            base.Destroy();
        }

        // 接收消息
        public override void OnMessage( WndMsgID msgID, UIMsgData msgData )
        {
            switch( msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP:
                    {
                        if (m_wndView != null)
                            m_wndView.SetRedPointTip(msgData as RedPointSetCount);
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN:
                    {
                        if (m_wndView != null)
                        {
                            PlayerButtonFrameSimulateOpenData data = msgData as PlayerButtonFrameSimulateOpenData;
                            m_wndView.SimulateOpenView(data.eViewType);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_MAIL_NEWMAILCOUNT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.UpdateMailRedTip();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_REDPOINTTIP:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            m_wndView.SetGemstoneRedPointCount(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_SHOW:
                    {
                        if (m_wndView != null)
                        {
                            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == false)
                                m_wndView.warHallBtn.gameObject.SetActive(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_ACCEPTWAIT_SHOW:
                case WndMsgID.WND_MSG_MSTCHTIMEINFO_HIDE:
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.warHallBtn.gameObject.SetActive(true);
                        }
                    }
                    break;

                #region 新手引导

                case WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if(cmdData != null)
                                m_wndView.AddGuideUIWidget(cmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_REMOVEGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                                m_wndView.RemoveGuideUIWidget(cmdData);
                        }
                    }
                    break;
                #endregion
                default:
                    break;
            }
        }

        public void onWingClick()
        {
            if (!(EntityFactory.MainHeroView.StateMachine is CreatureStateMachine))
            {
                return;
            }

            CreatureStateMachine fsm = EntityFactory.MainHeroView.StateMachine as CreatureStateMachine;
            fsm.useWing();
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (m_wndView == null)
                return;

            if (visible)
            {
                // 主动更新任务红点提示
                updateTaskRedTip();
                m_wndView.UpdateMailRedTip();
            }
        }

        private void updateTaskRedTip()
        {
            List<Task_List_Node> taskList = LogicDataCenter.taskDataManager.ListTaskData;
            if (taskList == null)
                return;

            int nCount = 0;
            foreach (var item in LogicDataCenter.taskDataManager.ListTaskData)
            {
                if (item.nTaskState == (int)Task_State_Type.TaskStateType_Finish)
                {
                    nCount++;
                }
            }
            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 4;
            uiData.nCount = nCount;
            m_wndView.SetRedPointTip(uiData);
        }
    }
}
