/*******************************************************************
** 文件名:	UMediatorTeamRight.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-1
** 版  本:	1.0
** 描  述:	战场右侧队伍
		
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
namespace USpeedUI.TeamRightPart
{
    public class TeamRightWnd : UIPopupWnd<TeamRightWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_TEAMRIGHTVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/TeamRightView/TeamRightView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01; ;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
           // UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMUID, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATETEAMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
           // UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMUID, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATETEAMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
			EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMDATA:
                    {
                        // 这个消息无用 以后处理掉一系列消息和结构
                        //return;
                        //if (m_wndView == null)
                        //{
                        //    return;
                        //}

                        //UIMsgCmdData uiTeamRight = (UIMsgCmdData)msgData;

                        //InitMyTeamRightToShow(uiTeamRight.ptrParam, uiTeamRight.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                            this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiMaxSkillData = (UIMsgCmdData)msgData;
                        cmd_player_skill_state data = IntPtrHelper.toData<cmd_player_skill_state>(uiMaxSkillData.ptrParam);
                        SetMyTeamSkillIcon(data);
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMUID:
                    {
                        UIMsgCmdData uiUIDData = (UIMsgCmdData)msgData;
                        SaveUIDAndPBUID(uiUIDData.ptrParam, uiUIDData.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();

                        LogicDataCenter.recordTableDataManager.m_bUpdateTeamInfo = true;
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                        cmd_creature_ShowPromptUI data = IntPtrHelper.toData<cmd_creature_ShowPromptUI>(uiData.ptrParam);
                        SetXpSkillFireEffect(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                        cmd_creature_ShowPromptUI data = IntPtrHelper.toData<cmd_creature_ShowPromptUI>(uiData.ptrParam);
                        SetXpSkillFireEffect(data);
                    }
                    break;
                case WndMsgID.WND_MSG_TEAMMATEINFO_UPDATETEAMINFO:
                    {
                        if (m_wndView == null)
                            return;

                        UpdateTeamRightViewToShow();
                    }
                    break;
				case WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME:
					{
						UIWarShowReliveTimeData data = msgData as UIWarShowReliveTimeData;
						if (m_wndView == null)
                            return;

						ShowTeamReliveTime(data.uid, data.nReliveTime);
					}
					break;
				default:
                    break;
            }
        }

        private void SetXpSkillFireEffect(cmd_creature_ShowPromptUI data)
        {
            int nUID = (int)data.uid;
            if (!LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nUID))
            {
                return;
            }

            TeamRightInfo teamRightInfo = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
            if (data.nType > 0)
            {
                m_wndView.SetXpSkillFireEffect(teamRightInfo.nSlotId);
            }
            else
            {
                m_wndView.DestoryXpSkillFireEffect(teamRightInfo.nSlotId);
            }
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
			//int nMyCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
            if (m_wndView == null)
            {
                return;
            }

            EntityView ev = e.objev;
			int nUID = e.data.nEntityID;

			bool isMyCamp = (ev.CampFlag == SPELL.CampFlag.CampFlag_Friend || ev.CampFlag == SPELL.CampFlag.CampFlag_Self);

			TeamRightInfo teamRightInfo = null;
			if (isMyCamp)
			{
				teamRightInfo = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
			}
			else
			{
				teamRightInfo = LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable[nUID] as TeamRightInfo;
			}

			if (teamRightInfo == null)
				return;

			int nSlotId = teamRightInfo.nSlotId;

			switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_HP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                    {
						int nMaxHP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
						int nHP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);

						if(isMyCamp)
							m_wndView.SetMyTeamHPChange(nMaxHP, nHP, nSlotId);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                    {
						int nMaxMP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);
						int nMP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);

						if(isMyCamp)
							m_wndView.SetMyTeamMPChange(nMaxMP, nMP, nSlotId);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
						Debug.LogWarning("Lv update," + ev.createinfo.szName + ":" + ev.createinfo.nLevel);

                        int nlevel = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

						if (isMyCamp)
							m_wndView.SetMyTeamLevelChange(nlevel, nSlotId);
						else
							m_wndView.SetOtherTeamLevelChagne(nlevel, nSlotId);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_AI_SCHEME_ID:
                    {
                        bool bIsAI = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_AI_SCHEME_ID) > 0 ? true : false;

						if(isMyCamp)
							m_wndView.SetMyTeamAICommandBtn(bIsAI, nSlotId);
                    }
                    break;
                default:
                    break;
            }
        }

        public void InitMyTeamRightToShow(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count nCountdata = helper.get<cmd_count>();
            int nMySlotID = 0;
            int nOtherSlotID = 0;
            int nMainPlayerCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
            for (int i = 0; i < nCountdata.nCount; ++i)
            {
                cmd_entity_camp_palyer_uidinfo data = helper.get<cmd_entity_camp_palyer_uidinfo>();
                if (data.nCamp == nMainPlayerCamp)
                {
                    TeamRightInfo teamRightInfo = new TeamRightInfo();
                    teamRightInfo.nSlotId = nMySlotID;


                    LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[(int)data.uUID] = teamRightInfo;
                    LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nMySlotID] = (int)data.uUID;
                    int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(data.uPDBID);
                    m_wndView.SetMyTeamItemViewShow(nMySlotID, data.nHeroID, nLevelID);

                    bool bIsAI = data.nIsAI > 0 ? true : false;
                    m_wndView.SetMyTeamAICommandBtn(bIsAI, teamRightInfo.nSlotId);

                    nMySlotID++;
                }

                else
                {
                    TeamRightInfo teamRightInfo = new TeamRightInfo();
                    teamRightInfo.nSlotId = nOtherSlotID;

                    LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable[(int)data.uUID] = teamRightInfo;
                    int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(data.uPDBID);
                    m_wndView.SetOtherTeamItemViewShow(nOtherSlotID, data.nHeroID, nLevelID);
                    nOtherSlotID++;
                }
            }
        }

		// 更新玩家的TeamRight界面显示
        public void UpdateTeamRightViewToShow()
        {
            int nMainPlayerCamp = LogicDataCenter.recordTableDataManager.GetMainPlayerCamp();
            int nHalfPlayerNum = LogicDataCenter.recordTableDataManager.GetHalfPlayerCount();
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            if(dicPersonInfo == null || dicPersonInfo.Count<=0)
            {
                return;
            }

            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                DataCenter.WarRecordTableDataManager.WarPersonInfo personInfo =  kvp.Value;
                int nEntityUID = personInfo.nID;
                if(nEntityUID<=0)
                {
                    // 若一开始没有uid的话，就先只是设置下英雄图片 等以后服务器完善再改 
                    SetTeamSlotHeroIcon(personInfo, nMainPlayerCamp, nHalfPlayerNum);
                    continue;
                }

                if( LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nEntityUID))
                {
                    continue;
                }

                if (LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable.ContainsKey(nEntityUID))
                {
                    continue;
                }

                if (personInfo.nCamp == nMainPlayerCamp)
                {
                    TeamRightInfo teamRightInfo = new TeamRightInfo();
                    int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID);
                    int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                    teamRightInfo.nSlotId = nSlotId;

                    LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nEntityUID] = teamRightInfo;
                    LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nSlotId] = nEntityUID;
                    m_wndView.SetMyTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
                    m_wndView.SetMyTeamAICommandBtn(personInfo.bIsAI, nSlotId);
                }
                else
                {
                    TeamRightInfo teamRightInfo = new TeamRightInfo();
                 
                    int nSlotId =nHalfPlayerNum*2- LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID)-1;
                    teamRightInfo.nSlotId = nSlotId;

                    LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable[nEntityUID] = teamRightInfo;
                    int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                    m_wndView.SetOtherTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
                }
            }

            int nTeamPlayerNum = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.Count + LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable.Count;
            int nTotalPlayerNum = dicPersonInfo.Count;
            if (nTeamPlayerNum == nTotalPlayerNum && nTotalPlayerNum>0)
            {
				// 所有的玩家信息初始化完成
                LogicDataCenter.recordTableDataManager.m_bUpdateTeamInfo = false;
            }

            if (nTeamPlayerNum > nTotalPlayerNum)
            {
                Debug.LogError("nTeamPlayerNum >= nTotalPlayerNum please to check");
            }
        }

        private void SetTeamSlotHeroIcon(WarRecordTableDataManager.WarPersonInfo personInfo, int nMainPlayerCamp, int nHalfPlayerNum)
        {
            if (personInfo.nCamp == nMainPlayerCamp)
            {
                int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID);
                int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                m_wndView.SetMyTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
            }
            else
            {
                int nSlotId = nHalfPlayerNum * 2 - LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID) - 1;
                int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                m_wndView.SetOtherTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
            }
        }

         public void ShowTeamReliveTime(int nUID, int nReliveTime)
         {
            TeamRightInfo teamRightInfo;
            if (LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nUID))
            {
                 teamRightInfo = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
                 if (teamRightInfo!=null)
                 {
                     m_wndView.ShowMyTeamReliveTime(nReliveTime, teamRightInfo.nSlotId);
                 }
            
            }
            else
            {
                //if ((int)data.uReliveUID == EntityFactory.MainHeroID)
                //{
                //    return;
                //}
                if (LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable.ContainsKey(nUID))
                {
                    teamRightInfo = LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable[nUID] as TeamRightInfo;
                    if (teamRightInfo != null)
                    {
                        m_wndView.ShowOtherTeamReliveTime(nReliveTime, teamRightInfo.nSlotId);
                    }
                }
            }
         }

         public void SetMyTeamSkillIcon(cmd_player_skill_state data)
         {
             int nUID = (int)data.uOperator;
             int nRevliveTime = (int)data.nCoolTime / 1000;
             if (!LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nUID))
             {
                 return;
             }

             TeamRightInfo teamRightInfo = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
             m_wndView.SetMyTeamSkillIcon(nRevliveTime, teamRightInfo.nSlotId, ref teamRightInfo.bFirstLearnSkill);
         }

         public void SendTeamRightClickPlayerUID(int nSlotId)
         {
             if (!LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable.ContainsKey(nSlotId))
             {
                 return;
             }

             SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
             if (sc != null)
             {
                 sc.TeamRightClickPlayerUID = (int)LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nSlotId];
             }
         }

         public void SaveUIDAndPBUID(IntPtr ptrParam, int nPtrLen)
         {
             IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
             cmd_count nCountdata = helper.get<cmd_count>();
             int nMyCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
             int nSlotIndex = 0;
             for (int i = 0; i < nCountdata.nCount; ++i)
             {
                 cmd_Send_UID_And_PBUID data = helper.get<cmd_Send_UID_And_PBUID>();
                 // 敌方和自己不存
                 if (data.nCamp!= nMyCamp )//|| EntityFactory.MainHeroID == data.uUID)
                 {
                     continue;
                 }

                 int nUID = (int)data.uUID;
                 LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nSlotIndex] = nUID;
                 nSlotIndex++;
             }
         }
    }
}
