/*******************************************************************
** 文件名:	EquipSchemeDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/4/20
** 版  本:	1.0
** 描  述:	装备数据管理
		
********************************************************************/

using ASpeedGame.Data.GameMobaSchemes;
using GameLogic;
using System;
using System.Collections.Generic;
using UnityEngine;
using USpeedUI;
using War;
using USpeedUI.Chat;

namespace DataCenter
{
	public class EquipSchemeDataManager : Singleton<EquipSchemeDataManager>
	{
		//// 装备方案ID
		//enum EWEquipSchemeType
		//{
		//	EWEST_None = 0,     // 空 
		//	EWEST_OftenUse,     // 常用方案
		//	EWEST_Diy1,         // 自定义方案一
		//	EWEST_Diy2,         // 自定义方案二
		//	EWEST_Diy3,         // 自定义方案三
		//	EWEST_Diy4,         // 自定义方案四

		//	EWEST_Max
		//};

		// <英雄ID, <方案ID,装备方案>>
		private Dictionary<int, Dictionary<byte, cmd_war_equipscheme>> m_EquipSchemeList;

		// 请求标识
		private Dictionary<int, bool> m_ReqHeroEquipScheme;

		// 新增装备方案名称
		private const string DefaultEquipSchemeName = "默认装备方案";

		public void Init()
		{
			m_EquipSchemeList = new Dictionary<int, Dictionary<byte, cmd_war_equipscheme>>();
			m_ReqHeroEquipScheme = new Dictionary<int, bool>();
		}

		public void Clear()
		{
			m_EquipSchemeList.Clear();
		}

		#region 显示层获取数据接口
		/// <summary>
		/// 获取某个英雄的某个装备方案
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="bySchemeID"></param>
		/// <param name="scheme"></param>
		/// <returns></returns>
		public bool getEquipScheme(int nHeroID, byte bySchemeID, out cmd_war_equipscheme scheme)
		{
			Dictionary<byte, cmd_war_equipscheme> schemeList;

			scheme = new cmd_war_equipscheme();

			if (!m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				return false;
			}

			if (!schemeList.TryGetValue(bySchemeID, out scheme))
			{
				return false;

			}

			return true;
		}

		/// <summary>
		/// 返回英雄的所有装备方案（战场内调用）
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <returns></returns>
		public bool getAllEquipSchemeList(int nHeroID, out List<cmd_war_equipscheme> schemeList)
		{
			Dictionary<byte, cmd_war_equipscheme> tmpSchemeList;

			schemeList = new List<cmd_war_equipscheme>();

			if (!m_EquipSchemeList.TryGetValue(nHeroID, out tmpSchemeList))
			{
				return false;
			}

			foreach (var item in tmpSchemeList)
			{
				schemeList.Add(item.Value);
			}

			// 读取大神推荐装
			List<cmd_war_equipscheme> godRecommdList = getGodRecommandEquipList(nHeroID);
			if(godRecommdList != null)
			{
				schemeList.AddRange(godRecommdList);
			}

			return true;
		}

		/// <summary>
		/// 获取英雄的自定义装备方案（主城中调用）
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="schemeList"></param>
		/// <returns>是否获取成功,未成功显示层则可以用一个Loading界面来等待一下</returns>
		public bool getDiyEquipSchemeList(int nHeroID, out List<cmd_war_equipscheme> schemeList)
		{
			Dictionary<byte, cmd_war_equipscheme> tmpSchemeList;

			schemeList = new List<cmd_war_equipscheme>();

			if (!m_EquipSchemeList.TryGetValue(nHeroID, out tmpSchemeList))
			{
				// 是否已经发送请求了
				if (!isReqHeroEquipScheme(nHeroID))
				{
					// 本地没有数据则向服务器请求
					sendReadEquipScheme(nHeroID);

					// 正在向服务器请求该英雄数据
					m_ReqHeroEquipScheme[nHeroID] = true;
				}

				return false;
			}

			// 一个装备都没有,添加一个
			if (tmpSchemeList.Count == 0)
			{
				cmd_war_equipscheme newScheme = new cmd_war_equipscheme();
				newScheme.nHeroID = nHeroID;
				newScheme.nEquipIDList = new int[UDefines.nEquipSchemeMaxEquipNum];
				newScheme.szAuthorName = "";
				newScheme.szSchemeName = DefaultEquipSchemeName;

				if (!addNewEquipScheme(nHeroID, ref newScheme))
				{
					Debug.LogError("Add new equip scheme failed.");
					return false;
				}

				tmpSchemeList = m_EquipSchemeList[nHeroID];
			}

			foreach (var item in tmpSchemeList)
			{
				if (isDiyEquipShceme(item.Value.bySchemeID))
					schemeList.Add(item.Value);
			}

			return true;
		}

		/// <summary>
		/// 添加装备方案
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="scheme"></param>
		/// <returns>是否本地添加成功</returns>
		public bool addEquipScheme(int nHeroID, ref cmd_war_equipscheme scheme)
		{
			// 先本地添加,同时会向服务器发送请求
			if (!addNewEquipScheme(nHeroID, ref scheme))
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, "装备方案已经满了");
				return false;
			}

			return true;
		}

		/// <summary>
		/// 删除装备方案
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="bySchemeID"></param>
		public void delEquipScheme(int nHeroID, byte bySchemeID)
		{
			_delEquipScheme(nHeroID, bySchemeID);

			sendDelEquipScheme(nHeroID, bySchemeID);
		}

		/// <summary>
		/// 保存装备方案
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="scheme"></param>
		public void saveEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{
			//if (scheme.bySchemeID == (byte)EWEquipSchemeType.EWEST_None)
			//addEquipScheme(nHeroID, out scheme);
			//else
			//{
			//_addEquipScheme(nHeroID, scheme);

			//sendModifyEquipScheme(nHeroID, scheme);
			//}

			sendModifyEquipScheme(nHeroID, scheme);
		}

		/// <summary>
		/// 装备方案是否可被修改
		/// </summary>
		/// <param name="bySchemeID">装备方案ID</param>
		/// <returns></returns>
		public bool isCanModify(byte bySchemeID)
		{
			if(bySchemeID == (int)EWEquipSchemeType.EWEST_OftenUse)
			{
				return false;
			}

			return true;
		}

		/// <summary>
		/// 分享装备方案到聊听框输入框
		/// </summary>
		/// <param name="scheme"></param>
		public void shareEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{
			string equipStr = "";
			string[] strArray = new string[scheme.nEquipIDList.Length];
			for(int i = 0; i < scheme.nEquipIDList.Length; ++i)
			{
				strArray[i] = scheme.nEquipIDList[i].ToString();
			}
			equipStr = String.Join(",", strArray);

			ChatSubItemText_EquipScheme.ShareToInputField(nHeroID, scheme.szAuthorName, scheme.szSchemeName, scheme.szTitle, equipStr);
		}

		/// <summary>
		/// 获取大神推荐装（主城调用）
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <returns></returns>
		public List<cmd_war_equipscheme> getGodRecommandEquipList(int nHeroID)
		{
			List<SSchemeGodRecommdEquip> list = GameMobaSchemesManager.Instance.getGodRecommdEquipList(nHeroID);
			if (list == null)
				return null;

			List<cmd_war_equipscheme> resultList = new List<cmd_war_equipscheme>();

			byte bySchemeID = (byte)EWEquipSchemeType.EWEST_Max;
			foreach (var item in list)
			{
				cmd_war_equipscheme scheme = new cmd_war_equipscheme();
				scheme.nHeroID = nHeroID;
				scheme.bySchemeID = bySchemeID;
				scheme.szAuthorName = item.AuthorName;
				scheme.szSchemeName = item.SchemeName;
				scheme.szTitle = item.Title;
				scheme.nEquipIDList = item.nEquipList;

				resultList.Add(scheme);

				bySchemeID++;
			}

			return resultList;
		}

		#endregion

		#region 发送命令接口
		// 增加装备方案
		private void sendAddEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{
			cmd_war_add_equipscheme cmd;
			cmd.nHeroID = nHeroID;
			cmd.szAuthorName = scheme.szAuthorName;
			cmd.szSchemeName = scheme.szSchemeName;
			cmd.szTitle = scheme.szTitle;
			cmd.nEquipIDList = scheme.nEquipIDList;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_ADD_EQUIPSCHEME, ref cmd);
		}

		// 增加装备方案
		private void sendDelEquipScheme(int nHeroID, byte bySchemeID)
		{
			cmd_war_del_equipscheme cmd;
			cmd.nHeroID = nHeroID;
			cmd.bySchemeID = bySchemeID;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_DEL_EQUIPSCHEME, ref cmd);
		}

		// 修改装备方案
		private void sendModifyEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{
			cmd_war_modify_equipscheme cmd;
			cmd.nHeroID = nHeroID;
			cmd.bySchemeID = scheme.bySchemeID;
			cmd.szAuthorName = scheme.szAuthorName;
			cmd.szSchemeName = scheme.szSchemeName;
			cmd.szTitle = scheme.szTitle;
			cmd.nEquipIDList = scheme.nEquipIDList;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_MODIFY_EQUIPSCHEME, ref cmd);
		}

		// 增加装备方案
		private void sendReadEquipScheme(int nHeroID)
		{
			cmd_war_read_equipscheme cmd;
			cmd.nHeroID = nHeroID;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_READ_EQUIPSCHEME, ref cmd);
		}
		#endregion

		#region 接受服务器消息
		// 接收装备方案列表
		public void recvEquipSchemeList(IntPtr ptrParam, int nParamLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParamLen);
			cmd_war_equipscheme_list cmd = helper.get<cmd_war_equipscheme_list>();

			int nHeroID = cmd.nHeroID;
			int nSchemeCount = cmd.nSchemeCount;

			// 已经获得该英雄的装备方案
			m_ReqHeroEquipScheme[nHeroID] = false;

			if (!m_EquipSchemeList.ContainsKey(nHeroID))
				m_EquipSchemeList.Add(nHeroID, new Dictionary<byte, cmd_war_equipscheme>(0, null));

			for (int i = 0; i < nSchemeCount; ++i)
			{
				cmd_war_equipscheme scheme = helper.get<cmd_war_equipscheme>();

				_addEquipScheme(nHeroID, scheme);
			}
		}

		#endregion

		#region 内部方法
		// 是否可以继续添加装备方案
		private bool isCanAddEquipScheme(int nHeroID)
		{
			int nCount = getDiyEquipSchemeCount(nHeroID);

			// 自定义方案最大个数
			int nDiySchemeMaxCount = (int)War.EWEquipSchemeType.EWEST_Max - 2;

			if (nCount < nDiySchemeMaxCount)
			{
				return true;
			}

			return false;
		}

		// 添加装备方案
		private bool _addEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{
			Dictionary<byte, cmd_war_equipscheme> schemeList;
			if (!m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				schemeList = new Dictionary<byte, cmd_war_equipscheme>();
				m_EquipSchemeList.Add(nHeroID, schemeList);
			}

			if (!schemeList.ContainsKey(scheme.bySchemeID))
			{
				schemeList.Add(scheme.bySchemeID, scheme);
			}
			else
			{
				schemeList[scheme.bySchemeID] = scheme;
			}

			return true;
		}

		// 删除装备方案
		private void _delEquipScheme(int nHeroID, byte bySchemeID)
		{
			Dictionary<byte, cmd_war_equipscheme> schemeList;
			if (m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				if (schemeList.ContainsKey(bySchemeID))
				{
					schemeList.Remove(bySchemeID);
				}
			}
		}

		// 保存装备方案
		private void _saveEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{

		}

		// 是否自定义方案
		private bool isDiyEquipShceme(byte bySchemeID)
		{
			switch (bySchemeID)
			{
				case (byte)War.EWEquipSchemeType.EWEST_Diy1:
				case (byte)War.EWEquipSchemeType.EWEST_Diy2:
				case (byte)War.EWEquipSchemeType.EWEST_Diy3:
				case (byte)War.EWEquipSchemeType.EWEST_Diy4:
					return true;
				default:
					return false;
			}
		}

		// 某个英雄的自定义方案个数
		private int getDiyEquipSchemeCount(int nHeroID)
		{
			int nCount = 0;

			Dictionary<byte, cmd_war_equipscheme> schemeList;
			if (m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				// 统计自定义方案个数
				foreach (var item in schemeList)
				{
					if (isDiyEquipShceme(item.Value.bySchemeID))
					{
						nCount++;
					}
				}
			}

			return nCount;
		}

		// 是否正在请求某个英雄的装备方案数据
		private bool isReqHeroEquipScheme(int nHero)
		{
			if (!m_ReqHeroEquipScheme.ContainsKey(nHero))
			{
				m_ReqHeroEquipScheme.Add(nHero, false);
			}

			return m_ReqHeroEquipScheme[nHero];
		}

		// 获取一个可用的装备方案ID
		private EWEquipSchemeType getFreeSchemeID(int nHeroID)
		{
			Dictionary<byte, cmd_war_equipscheme> schemeList;
			if (!m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				schemeList = new Dictionary<byte, cmd_war_equipscheme>();
				m_EquipSchemeList.Add(nHeroID, schemeList);
			}

			for (byte i = (int)EWEquipSchemeType.EWEST_Diy1; i < (int)EWEquipSchemeType.EWEST_Max; ++i)
			{
				if (!schemeList.ContainsKey(i))
				{
					return (EWEquipSchemeType)i;
				}
			}

			return EWEquipSchemeType.EWEST_None;
		}


		// 添加一个新的方案
		private bool addNewEquipScheme(int nHeroID, ref cmd_war_equipscheme newScheme)
		{
			EWEquipSchemeType type = getFreeSchemeID(nHeroID);
			if (type == EWEquipSchemeType.EWEST_None)
			{
				return false;
			}

			// 当前英雄一个装备方案都没有,则增加个默认的
			newScheme.bySchemeID = (byte)type;

			// 添加到本地缓存
			_addEquipScheme(nHeroID, newScheme);

			// 添加到服务器数据库
			sendAddEquipScheme(nHeroID, newScheme);

			return true;
		}

		// 读取大神装备方案


		#endregion
	}
}