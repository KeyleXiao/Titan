/*******************************************************************
** �ļ���:	EquipSchemeDataManager.cs 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/20
** ��  ��:	1.0
** ��  ��:	װ�����ݹ���
		
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
		//// װ������ID
		//enum EWEquipSchemeType
		//{
		//	EWEST_None = 0,     // �� 
		//	EWEST_OftenUse,     // ���÷���
		//	EWEST_Diy1,         // �Զ��巽��һ
		//	EWEST_Diy2,         // �Զ��巽����
		//	EWEST_Diy3,         // �Զ��巽����
		//	EWEST_Diy4,         // �Զ��巽����

		//	EWEST_Max
		//};

		// <Ӣ��ID, <����ID,װ������>>
		private Dictionary<int, Dictionary<byte, cmd_war_equipscheme>> m_EquipSchemeList;

		// �����ʶ
		private Dictionary<int, bool> m_ReqHeroEquipScheme;

		// ����װ����������
		private const string DefaultEquipSchemeName = "Ĭ��װ������";

		public void Init()
		{
			m_EquipSchemeList = new Dictionary<int, Dictionary<byte, cmd_war_equipscheme>>();
			m_ReqHeroEquipScheme = new Dictionary<int, bool>();
		}

		public void Clear()
		{
			m_EquipSchemeList.Clear();
		}

		#region ��ʾ���ȡ���ݽӿ�
		/// <summary>
		/// ��ȡĳ��Ӣ�۵�ĳ��װ������
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
		/// ����Ӣ�۵�����װ��������ս���ڵ��ã�
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

			// ��ȡ�����Ƽ�װ
			List<cmd_war_equipscheme> godRecommdList = getGodRecommandEquipList(nHeroID);
			if(godRecommdList != null)
			{
				schemeList.AddRange(godRecommdList);
			}

			return true;
		}

		/// <summary>
		/// ��ȡӢ�۵��Զ���װ�������������е��ã�
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="schemeList"></param>
		/// <returns>�Ƿ��ȡ�ɹ�,δ�ɹ���ʾ���������һ��Loading�������ȴ�һ��</returns>
		public bool getDiyEquipSchemeList(int nHeroID, out List<cmd_war_equipscheme> schemeList)
		{
			Dictionary<byte, cmd_war_equipscheme> tmpSchemeList;

			schemeList = new List<cmd_war_equipscheme>();

			if (!m_EquipSchemeList.TryGetValue(nHeroID, out tmpSchemeList))
			{
				// �Ƿ��Ѿ�����������
				if (!isReqHeroEquipScheme(nHeroID))
				{
					// ����û�������������������
					sendReadEquipScheme(nHeroID);

					// ����������������Ӣ������
					m_ReqHeroEquipScheme[nHeroID] = true;
				}

				return false;
			}

			// һ��װ����û��,���һ��
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
		/// ���װ������
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="scheme"></param>
		/// <returns>�Ƿ񱾵���ӳɹ�</returns>
		public bool addEquipScheme(int nHeroID, ref cmd_war_equipscheme scheme)
		{
			// �ȱ������,ͬʱ�����������������
			if (!addNewEquipScheme(nHeroID, ref scheme))
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, "װ�������Ѿ�����");
				return false;
			}

			return true;
		}

		/// <summary>
		/// ɾ��װ������
		/// </summary>
		/// <param name="nHeroID"></param>
		/// <param name="bySchemeID"></param>
		public void delEquipScheme(int nHeroID, byte bySchemeID)
		{
			_delEquipScheme(nHeroID, bySchemeID);

			sendDelEquipScheme(nHeroID, bySchemeID);
		}

		/// <summary>
		/// ����װ������
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
		/// װ�������Ƿ�ɱ��޸�
		/// </summary>
		/// <param name="bySchemeID">װ������ID</param>
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
		/// ����װ�������������������
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
		/// ��ȡ�����Ƽ�װ�����ǵ��ã�
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

		#region ��������ӿ�
		// ����װ������
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

		// ����װ������
		private void sendDelEquipScheme(int nHeroID, byte bySchemeID)
		{
			cmd_war_del_equipscheme cmd;
			cmd.nHeroID = nHeroID;
			cmd.bySchemeID = bySchemeID;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_DEL_EQUIPSCHEME, ref cmd);
		}

		// �޸�װ������
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

		// ����װ������
		private void sendReadEquipScheme(int nHeroID)
		{
			cmd_war_read_equipscheme cmd;
			cmd.nHeroID = nHeroID;

			EntityEventHelper.Instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_READ_EQUIPSCHEME, ref cmd);
		}
		#endregion

		#region ���ܷ�������Ϣ
		// ����װ�������б�
		public void recvEquipSchemeList(IntPtr ptrParam, int nParamLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParamLen);
			cmd_war_equipscheme_list cmd = helper.get<cmd_war_equipscheme_list>();

			int nHeroID = cmd.nHeroID;
			int nSchemeCount = cmd.nSchemeCount;

			// �Ѿ���ø�Ӣ�۵�װ������
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

		#region �ڲ�����
		// �Ƿ���Լ������װ������
		private bool isCanAddEquipScheme(int nHeroID)
		{
			int nCount = getDiyEquipSchemeCount(nHeroID);

			// �Զ��巽��������
			int nDiySchemeMaxCount = (int)War.EWEquipSchemeType.EWEST_Max - 2;

			if (nCount < nDiySchemeMaxCount)
			{
				return true;
			}

			return false;
		}

		// ���װ������
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

		// ɾ��װ������
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

		// ����װ������
		private void _saveEquipScheme(int nHeroID, cmd_war_equipscheme scheme)
		{

		}

		// �Ƿ��Զ��巽��
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

		// ĳ��Ӣ�۵��Զ��巽������
		private int getDiyEquipSchemeCount(int nHeroID)
		{
			int nCount = 0;

			Dictionary<byte, cmd_war_equipscheme> schemeList;
			if (m_EquipSchemeList.TryGetValue(nHeroID, out schemeList))
			{
				// ͳ���Զ��巽������
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

		// �Ƿ���������ĳ��Ӣ�۵�װ����������
		private bool isReqHeroEquipScheme(int nHero)
		{
			if (!m_ReqHeroEquipScheme.ContainsKey(nHero))
			{
				m_ReqHeroEquipScheme.Add(nHero, false);
			}

			return m_ReqHeroEquipScheme[nHero];
		}

		// ��ȡһ�����õ�װ������ID
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


		// ���һ���µķ���
		private bool addNewEquipScheme(int nHeroID, ref cmd_war_equipscheme newScheme)
		{
			EWEquipSchemeType type = getFreeSchemeID(nHeroID);
			if (type == EWEquipSchemeType.EWEST_None)
			{
				return false;
			}

			// ��ǰӢ��һ��װ��������û��,�����Ӹ�Ĭ�ϵ�
			newScheme.bySchemeID = (byte)type;

			// ��ӵ����ػ���
			_addEquipScheme(nHeroID, newScheme);

			// ��ӵ����������ݿ�
			sendAddEquipScheme(nHeroID, newScheme);

			return true;
		}

		// ��ȡ����װ������


		#endregion
	}
}