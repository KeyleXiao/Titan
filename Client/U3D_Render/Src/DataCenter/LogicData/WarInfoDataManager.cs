/*******************************************************************
** 文件名:	WarInfoDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017-3-18
** 版  本:	1.0
** 描  述:	战场信息
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;
using LegendCup_ManagedDef;

namespace DataCenter
{
    public class WarInfoDataManager
    {
        private cmd_war_info m_WarInfo = new cmd_war_info();         // 本局战场信息
        private cmd_war_info m_LastWarInfo = new cmd_war_info();     // 上一句战场信息
        private cmd_war_end_legendcup_info m_WarEndLegendCupInfo = new cmd_war_end_legendcup_info();
        // 战场持续时间(秒)
        public uint WarPlayTimeSec
        {
            get { return m_WarPlayTimeSec; }
            set { m_WarPlayTimeSec = value; }
        }
        private uint m_WarPlayTimeSec = 0;

        // 战场正式开始的时间（毫秒）
        public uint WarStartTime { get { return m_WarInfo.nWarStartTick; } }

        // 战场数据库ID
        public Int64 DBWarID { get { return m_WarInfo.lDBWarID; } }

        public void Init()
        {
            m_WarPlayTimeSec = 0;
        }

        public void Clear()
        {
            ClearData();

            m_LastWarInfo = new cmd_war_info();
        }

        // 清除本局战场内相关的数据
        public void ClearData()
        {
            m_WarPlayTimeSec = 0;

            m_WarInfo = new cmd_war_info();
            m_WarEndLegendCupInfo = new cmd_war_end_legendcup_info();
        }

        public void onLeaveBattleState()
        {
            // 缓存上一局的战场信息
            m_LastWarInfo = m_WarInfo;

            ClearData();
        }

        public void onEnterLoginState()
        {
            ClearData();

            m_LastWarInfo = new cmd_war_info();
        }

        #region 接收服务器消息
        public void recvWarInfo(IntPtr ptrParam, int nPtrLen)
        {
            cmd_war_info info = IntPtrHelper.toData<cmd_war_info>(ptrParam);

            // 本局的战场信息
            m_WarInfo = info;

            Debug.LogWarning("summerid=" + GameLogicAPI.getWarSummerID());
        }

        public void recvWarEndLegendCupInfo(IntPtr ptrParam, int nPtrLen)
        {
            cmd_war_end_legendcup_info info = IntPtrHelper.toData<cmd_war_end_legendcup_info>(ptrParam);

            m_WarEndLegendCupInfo = info;
        }
        #endregion

        #region 显示层接口
        // 获取战场开始时间
        public uint getWarStartTick()
        {
            return m_WarInfo.nWarStartTick;
        }

        // 杯赛信息
        public void showLegendCupMsgInfo()
        {
            if (m_WarEndLegendCupInfo.bIsValid == 1)
            {
                // 线下比赛
                if (m_WarEndLegendCupInfo.bIsOnlineCup == 0)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_OFFLINE_END_WAR_MSGBOX_INFO);
                    return;
                }

                switch((ERoundCompetitionType)m_WarEndLegendCupInfo.eRoundType)
                {
                    case ERoundCompetitionType.emType_KnockoutCompetition : // 淘汰赛类型
                        {
                            showKnockoutTypeMsgInfo();
                        }
                        break;
                    case ERoundCompetitionType.emType_GroupCompetition : // 小组赛类型
                        {
                            showGroupTypeMsgInfo();
                        }
                        break;
                    default:
                        break;
                }
                
            }

            m_WarEndLegendCupInfo.bIsValid = 0;
        }

        public void showKnockoutTypeMsgInfo()
        {
            if (m_WarEndLegendCupInfo.bIsLastRound == 1)
            {
                // 给出提示
                if (m_WarEndLegendCupInfo.bNodeEnd == 1)
                {
                    // 节点比赛打完
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_END_WIN_MSGBOX_INFO);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_END_FAIL_MSGBOX_INFO);
                    }
                }
                else
                {
                    // 节点比赛未打完 提示下一场比赛间隔
                    string szParam = "";
                    szParam += m_WarEndLegendCupInfo.dwNextStartInterval;
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_WIN_MSGBOX_INFO, szParam);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_FAIL_MSGBOX_INFO, szParam);
                    }
                }
            }
            else
            {
                // 本轮淘汰赛节点是否结束
                if (m_WarEndLegendCupInfo.bNodeEnd == 1)
                {
                    // 节点比赛打完 提示下一场时间
                    DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(m_WarEndLegendCupInfo.dwNextRoundStartTime);
                    beginTime = beginTime.ToLocalTime();
                    string szParam = "";
                    szParam += beginTime.Month + ";" + beginTime.Day + ";" + beginTime.Hour + ";" + beginTime.Minute;
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_KNOCKOUT_NODE_END_WIN_MSGBOX_INFO, szParam);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_KNOCKOUT_NODE_END_FAIL_MSGBOX_INFO, szParam);
                    }
                }
                else
                {
                    // 节点比赛未打完 提示下一场比赛间隔
                    string szParam = "";
                    szParam += m_WarEndLegendCupInfo.dwNextStartInterval;
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_WIN_MSGBOX_INFO, szParam);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_FAIL_MSGBOX_INFO, szParam);
                    }
                }
            }
        }

        public void showGroupTypeMsgInfo()
        {
            if (m_WarEndLegendCupInfo.bIsLastRound == 1)
            {
                // 给出提示
                if (m_WarEndLegendCupInfo.bNodeEnd == 1)
                {
                    // 节点比赛打完
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_END_WIN_MSGBOX_INFO);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_END_FAIL_MSGBOX_INFO);
                    }
                }
                else
                {
                    // 节点比赛未打完 提示下一场比赛间隔
                    string szParam = "";
                    szParam += m_WarEndLegendCupInfo.dwNextStartInterval;
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_WIN_MSGBOX_INFO, szParam);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_FAIL_MSGBOX_INFO, szParam);
                    }
                }
            }
            else
            {
                // 节点比赛打完
                if (m_WarEndLegendCupInfo.bNodeEnd == 1)
                {
                    // 节点比赛打完 提示下一场时间
                    DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(m_WarEndLegendCupInfo.dwNextRoundStartTime);
                    beginTime = beginTime.ToLocalTime();
                    string szParam = "";
                    szParam += beginTime.Month + ";" + beginTime.Day + ";" + beginTime.Hour + ";" + beginTime.Minute;

                    if (m_WarEndLegendCupInfo.bRoundLastNode == 1)
                    {
                        if (m_WarEndLegendCupInfo.bWin == 1)
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_GROUP_NODE_END_WIN_MSGBOX_INFO, szParam);
                        }
                        else
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_GROUP_NODE_END_FAIL_MSGBOX_INFO, szParam);
                        }
                    }
                    else
                    {
                        
                        if (m_WarEndLegendCupInfo.bWin == 1)
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_END_WIN_MSGBOX_INFO, szParam);
                        }
                        else
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_END_FAIL_MSGBOX_INFO, szParam);
                        }
                    }
                }
                else
                {
                    // 节点比赛未打完 提示下一场比赛间隔
                    string szParam = "";
                    szParam += m_WarEndLegendCupInfo.dwNextStartInterval;
                    if (m_WarEndLegendCupInfo.bWin == 1)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_WIN_MSGBOX_INFO, szParam);
                    }
                    else
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NODE_FAIL_MSGBOX_INFO, szParam);
                    }
                }
            }
        }

        // 获取上一局战场信息
        public cmd_war_info getLastWarInfo()
        {
            return m_LastWarInfo;
        }
        // 获取本局战场信息
        public cmd_war_info getCurWarInfo()
        {
            return m_WarInfo;
        }
        #endregion

#region 内部方法

#endregion
    }
}
