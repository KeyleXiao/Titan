
using System.Collections.Generic;
using GameLogic;
using System;
using DataCenter;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;
using USpeedUI.UEffect;
using War;

namespace USpeedUI.TeamRightPart
{
    public class TeammateViewItem : TimeCountDown
    {
        public Image HeroIconImage;
        public Image SkillIconImage;
        public Slider HPBar;
        public Slider MPBar;
        public Text TimeCountText;
        public Text LevelText;
        public GameObject DieMask;
        public Image helpMeImage;

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            SkillIconImage.fillAmount = (fOriginalTime - fCurTime) / (fOriginalTime);
        }
    }

    public class BattleWarTeammateWnd : UIPopupWnd<BattleWarTeammateWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_BATTLEWAR_TEAMMATEVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/TeamRightView/BattleWarTeammateView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01; ;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
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
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATE_HEROICON, this);

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
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATE_HEROICON, this);
            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
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

                        m_wndView.SetData();
                    }
                    break;
                case WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateLifeState(msgData as UBattleWarLifeState);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_UPDATE_HEROICON:
                    {
                        if (m_wndView != null)
                            m_wndView.updateHeroIcon(msgData as UIWarUpdateHeroIcon);
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

            TeamRightInfo TeammateViewItem = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
            if (data.nType > 0)
            {
                m_wndView.SetXpSkillFireEffect(TeammateViewItem.nSlotId);
            }
            else
            {
                m_wndView.DestoryXpSkillFireEffect(TeammateViewItem.nSlotId);
            }
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            if (m_wndView == null)
            {
                return;
            }

            EntityView ev = e.objev;
			int nUID = e.data.nEntityID;

            bool isMyCamp = (ev.CampFlag == SPELL.CampFlag.CampFlag_Friend || ev.CampFlag == SPELL.CampFlag.CampFlag_Self);
            if (!isMyCamp)
                return;

            TeamRightInfo TeammateViewItem = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
            if (TeammateViewItem == null)
				return;

			int nSlotId = TeammateViewItem.nSlotId;
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
                        int nlevel = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                        Debug.LogWarning("Lv update," + ev.createinfo.szName + ":" + ev.createinfo.nLevel + "," + nlevel);

                        if (isMyCamp)
							m_wndView.SetMyTeamLevelChange(nlevel, nSlotId);
                    }
                    break;
                default:
                    break;
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
        }

         public void SetMyTeamSkillIcon(cmd_player_skill_state data)
         {
             int nUID = (int)data.uOperator;
             int nRevliveTime = (int)data.nCoolTime / 1000;
             if (!LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nUID))
             {
                 return;
             }

             TeamRightInfo teamItem = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nUID] as TeamRightInfo;
             m_wndView.SetMyTeamSkillIcon(nRevliveTime, teamItem.nSlotId, ref teamItem.bFirstLearnSkill);
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

    public class BattleWarTeammateWndView : UIBaseWndView
    {
        public TeammateViewItem defaultTeammateItem;
        private Dictionary<int, TeammateViewItem> m_dicMyTeam = new Dictionary<int, TeammateViewItem>();
        private Dictionary<int, UEffectParamBase> dicOldUEffectParam = new Dictionary<int, UEffectParamBase>();

        public void SetData()
        {
            int nMainPlayerCamp = LogicDataCenter.recordTableDataManager.GetMainPlayerCamp();
            int nHalfPlayerNum = LogicDataCenter.recordTableDataManager.GetHalfPlayerCount();
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            if (dicPersonInfo == null || dicPersonInfo.Count <= 0)
            {
                return;
            }

            defaultTeammateItem.gameObject.SetActive(false);
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                DataCenter.WarRecordTableDataManager.WarPersonInfo personInfo = kvp.Value;
                int nEntityUID = personInfo.nID;
                if (nEntityUID <= 0)
                {
                    // 若一开始没有uid的话，就先只是设置下英雄图片 等以后服务器完善再改 
                    SetTeamSlotHeroIcon(personInfo, nMainPlayerCamp, nHalfPlayerNum);
                    continue;
                }

                if (LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.ContainsKey(nEntityUID))
                {
                    continue;
                }

                if (LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable.ContainsKey(nEntityUID))
                {
                    continue;
                }

                if (personInfo.nCamp == nMainPlayerCamp)
                {
                    int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID);
                    int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                    TeammateViewItem teammateItem = GameObject.Instantiate<TeammateViewItem>(defaultTeammateItem);
                    teammateItem.gameObject.SetActive(true);
                    teammateItem.helpMeImage.gameObject.SetActive(false);
                    teammateItem.transform.SetParent(defaultTeammateItem.transform.parent, false);
                    m_dicMyTeam.Add(nSlotId, teammateItem);
                    Debug.LogWarningFormat("BattleWarTeammateWndView:SetData:{0},{1}", nSlotId, personInfo.uPDBID);
                    TeamRightInfo teamRightInfo = new TeamRightInfo();
                    teamRightInfo.nSlotId = nSlotId;
                    LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable[nEntityUID] = teamRightInfo;
                    LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nSlotId] = nEntityUID;
                    SetMyTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
                }
            }

            int nTeamPlayerNum = LogicDataCenter.teamRightWndDataManager.MyTeamUIDTable.Count + LogicDataCenter.teamRightWndDataManager.OtherTeamUIDTable.Count;
            int nTotalPlayerNum = dicPersonInfo.Count;
            if (nTeamPlayerNum == nTotalPlayerNum && nTotalPlayerNum > 0)
            {
                // 所有的玩家信息初始化完成
                LogicDataCenter.recordTableDataManager.m_bUpdateTeamInfo = false;
            }

            if (nTeamPlayerNum > nTotalPlayerNum)
            {
                Debug.LogError("nTeamPlayerNum >= nTotalPlayerNum please to check");
            }
        }

        public void OnUpdateLifeState(UBattleWarLifeState msg)
        {
            int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID((ulong)msg.lifeState.pdbid);
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                Debug.LogFormat("OnUpdateLifeState:not find TeammateViewItem by slot:{0},{1}", nSlotId, msg.lifeState.pdbid);
                return;
            }

            myTeamItem.DieMask.SetActive(msg.lifeState.nLifeSate != (int)ELifeState.ELS_Alive);
            myTeamItem.helpMeImage.gameObject.SetActive(msg.lifeState.nLifeSate == (int)ELifeState.ELS_FakeDead);
        }

        public void SetMyTeamSkillIcon(int nCoolTime, int nSlotId, ref bool bFirstLearn)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                Debug.LogFormat("SetMyTeamSkillIcon:not find TeammateViewItem by slot:{0},{1}", nSlotId);
                return;
            }
            
            if (nCoolTime <= 0)
            {
                if (bFirstLearn)
                {
                    myTeamItem.SkillIconImage.fillAmount = 1;
                    bFirstLearn = false;
                }
            }
            else
            {
                myTeamItem.StartCountDown(nCoolTime, 0.5f, 0.5f);
            }
        }

        public void SetMyTeamHPChange(int nHPMax, int nHPValue, int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                return;
            }

            if (nHPMax <= 0)
            {
                return;
            }

            myTeamItem.HPBar.value = (float)nHPValue / nHPMax;
        }

        public void SetMyTeamMPChange(int nMPMax, int nMPValue, int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                return;
            }

            if (nMPMax <= 0)
            {
                return;
            }

            myTeamItem.MPBar.value = (float)nMPValue / nMPMax;
        }

        public void SetMyTeamLevelChange(int nLevel, int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                Debug.LogFormat("SetMyTeamLevelChange:not find TeammateViewItem by slot:{0},{1}", nSlotId, nLevel);
                return;
            }

            myTeamItem.LevelText.text = nLevel.ToString();
        }

        public void ShowMyTeamReliveTime(int nReliveTime, int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                return;
            }

            myTeamItem.TimeCountText.gameObject.SetActive(true);
            myTeamItem.DieMask.SetActive(true);
            myTeamItem.TimeCountText.text = nReliveTime.ToString();
            myTeamItem.HPBar.gameObject.SetActive(false);
            myTeamItem.MPBar.gameObject.SetActive(false);

            myTeamItem.gameObject.GetComponent<TeamReliveCountDown>().StartCountDown(nReliveTime, 1.0f, 1.0f);
        }

        public void SetXpSkillFireEffect(int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                return;
            }

            if (dicOldUEffectParam.ContainsKey(nSlotId))
            {
                return;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UTeamRight_XpSkillFire, _tfAttachParent: myTeamItem.HeroIconImage.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            dicOldUEffectParam[nSlotId] = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.localPosition = new Vector3(0, 0, 0);
        }

        public void DestoryXpSkillFireEffect(int nSlotId)
        {
            TeammateViewItem myTeamItem;
            if (!m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                return;
            }

            if (!dicOldUEffectParam.ContainsKey(nSlotId))
            {
                return;
            }

            UEffectParamBase param = dicOldUEffectParam[nSlotId];
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            dicOldUEffectParam.Remove(nSlotId);
        }

        private void SetTeamSlotHeroIcon(WarRecordTableDataManager.WarPersonInfo personInfo, int nMainPlayerCamp, int nHalfPlayerNum)
        {
            if (personInfo.nCamp == nMainPlayerCamp)
            {
                int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID);
                int nLevelID = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(personInfo.uPDBID);
                SetMyTeamItemViewShow(nSlotId, personInfo.nHeroId, nLevelID);
            }
        }

        public void SetMyTeamItemViewShow(int nSlotId, int nHeroId, int nLevel)
        {
            TeammateViewItem myTeamItem;
            if (m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                myTeamItem.gameObject.SetActive(true);
                myTeamItem.HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_TEAMRIGHTVIEW, 1, 6, nHeroId);
                myTeamItem.LevelText.text = nLevel.ToString();
            }
        }

        public void updateHeroIcon(UIWarUpdateHeroIcon uiMsgData)
        {
            int nSlotId = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uiMsgData.nPdbid);
            TeammateViewItem myTeamItem;
            if (m_dicMyTeam.TryGetValue(nSlotId, out myTeamItem))
            {
                myTeamItem.HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_TEAMRIGHTVIEW, 1, 6, uiMsgData.nHeroID);
            }
        }
    }
}
