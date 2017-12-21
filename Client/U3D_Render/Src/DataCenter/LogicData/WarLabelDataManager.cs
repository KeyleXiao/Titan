/*******************************************************************
** 文件名:	WarLabelDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017-3-18
** 版  本:	1.0
** 描  述:	标签功能
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;
using ASpeedGame.Data.WarSupportPlayerConfig;

namespace DataCenter
{
    public class WarLabelDataManager
    {
		// 点赞数据
		private Dictionary<ulong, cmd_update_support_vote_person> m_dicPlayersSupportInfo;

		// 玩家本次标签数据（玩家pdbid-(标签ID-个数)）
		private Dictionary<uint, Dictionary<int, int>> m_dicLabelData;

		// 最新的标签
		private Queue<cmd_war_add_label> m_LastLabelList;

		public void Init()
		{
			m_dicPlayersSupportInfo = new Dictionary<ulong, cmd_update_support_vote_person>();
			m_dicLabelData = new Dictionary<uint, Dictionary<int, int>>();
			m_LastLabelList = new Queue<cmd_war_add_label>();
		}

		public void Clear()
		{
			m_dicPlayersSupportInfo.Clear();
			m_dicLabelData.Clear();
			m_LastLabelList.Clear();
		}

		// 清除战场内相关的数据
		public void ClearMobaData()
		{
			m_dicPlayersSupportInfo.Clear();
			m_dicLabelData.Clear();
			m_LastLabelList.Clear();
		}

		public Dictionary<ulong, cmd_update_support_vote_person> GetWarPlayersSupportInfo()
		{
			return m_dicPlayersSupportInfo;
		}

		public int GetWarPlayersSupportCount()
		{
			return m_dicPlayersSupportInfo.Count;
		}

		public int getPlayerSupportNum(uint pdbid)
		{
			Dictionary<int, int> labelInfo;
			if (!m_dicLabelData.TryGetValue(pdbid, out labelInfo))
			{
				return 0;
			}

			return labelInfo.Sum(x => x.Value);
		}


		// 获取最新的标签
		public bool getLastPlayerLabel(uint nPdbid, out cmd_war_add_label lastLabel, out int nLabelCount)
		{
			lastLabel = new cmd_war_add_label();
			nLabelCount = 0;

			if (m_LastLabelList.Count <= 0)
				return false;

			lastLabel = m_LastLabelList.Peek();
			if (lastLabel.targetPdbid != nPdbid)
				return false;

			lastLabel = m_LastLabelList.Dequeue();
			nLabelCount = m_dicLabelData[lastLabel.targetPdbid][lastLabel.nLabelID];

			return true;
		}

		#region 接受消息
		public void FilterSupportPlayer(IntPtr ptrParam, int nPtrLen)
        {
			if (nPtrLen <= 0)
				return;

			ClearMobaData();

			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

			cmd_count countData = helper.get<cmd_count>();
            int nCount = countData.nCount;
            for (int i = 0; i < nCount; i++)
            {
                cmd_update_support_vote_person data = helper.get<cmd_update_support_vote_person>();
                m_dicPlayersSupportInfo[data.uPDBIU] = data;
            }

            SortPlayersSupportInfo();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FILTER_SUPPORT_PLAYER, null);
        }

		public void recvAddLabel(cmd_war_add_label data)
		{
			if (data.targetPdbid <= 0)
			{
				Debug.LogError("WarLabelDataManger::recvAddLabel, data.targetPdbid <= 0");
				return;
			}


			uint uTargetPdbid = data.targetPdbid;
			int nLabelID = data.nLabelID;

			// 获取某个玩家本次的标签信息
			Dictionary<int, int> outLabelInfo;
			if (!m_dicLabelData.TryGetValue(uTargetPdbid, out outLabelInfo))
			{
				outLabelInfo = new Dictionary<int, int>();
				m_dicLabelData.Add(uTargetPdbid, outLabelInfo);
			}

			// 判断该玩家是否有该标签信息
			if (!outLabelInfo.ContainsKey(nLabelID))
			{
				outLabelInfo.Add(nLabelID, 0);
			}

			outLabelInfo[nLabelID]++;

			// 保存最新的标签
			m_LastLabelList.Enqueue(data);

			UAddLabelMsgData msgData = new UAddLabelMsgData();
			msgData.msgID = (int)WndMsgID.WND_MSG_WAR_HAS_ADD_LABEL;
			msgData.nLabelID = data.nLabelID;
			msgData.operatorName = data.szOperatorName;
			msgData.targetName = data.szTargetName;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_HAS_ADD_LABEL, msgData);
		}
		#endregion 接受消息

		private void SortPlayersSupportInfo()
        {
			// 对点赞评选类型排序,从低到高
			int nSortID = 1;
			m_dicPlayersSupportInfo.Values.OrderBy(x => x.nVoteID).ToList().ForEach((x) => {
				x.nSortID = nSortID++;
				m_dicPlayersSupportInfo[x.uPDBIU] = x;
			});
        }
	}
}
