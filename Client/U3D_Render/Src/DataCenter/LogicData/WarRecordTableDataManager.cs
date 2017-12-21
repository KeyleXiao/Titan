/*******************************************************************
** 文件名:	WarRecordTableDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	战绩表数据管理
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using GameLogic;
using USpeedUI;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;
using System.Runtime.InteropServices;
using USpeedUI.WarBroadcastMessageMgr;
using War;
using UnityEngine;

namespace DataCenter
{
    public class WarRecordTableDataManager
    {
        // 自己英雄阵营
        public int m_nMainHeroCamp;
        // 是否第一次请求
        public bool isOneRequest = true;
        // TAB界面是否打开
        public bool IsWarRecordTableVisible = false;
        // 队伍界面是否需要更新uid 全部人都有uid之后关闭这个变量
        public bool m_bUpdateTeamInfo = true;

        private int m_ModelHalfPlayerCount = 5;

        private int m_MainPlayerCamp = 0;

        private SWar2ViewMsgAce m_SelfCampACE;

        public SWar2ViewMsgAce SelfCampACE
        {
            get
            {
                return m_SelfCampACE;
            }

            set
            {
                m_SelfCampACE = value;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_BATTLEWAR_RECORDTABLE_ACE, null);
            }
        }

        private int m_uidDevilKing = 0;
        public int CurDevilKing
        {
            get { return m_uidDevilKing; }
            set { m_uidDevilKing = value; }
        }
        // 存储逻辑成的装备数据 pdPID为索引
        //public Dictionary<ulong, List<cmd_war_persongoodsinfo>> m_dicEquipInfoArray = new Dictionary<ulong, List<cmd_war_persongoodsinfo>>();

        public class WarPersonInfo
        {
            public int nHeroId;
            public int nCamp;
            public int nLevel;
            public int nID;
            public int nKillNum;
            public int nIndex;
            public int nDeadNum;
            public int nAssists;
            public int nMobaWarMoney;
            public int bIsHero;
            public string playerName;
            public ulong uPDBID;
            public int nSummerIconID;  // 召唤师技能图标id
            public string sSummerDesc; // 召唤师技能描述
            public bool bIsAI;        // 是否是ai
            public int nAliveContAssist;// 不死连助攻
            public int nSkinID;
            public int nSex;
            public int nPlayerType;
            public int nDestroyTower;  // 推塔数
        }

        public class WarPlayerVsInfo
        {
            public uint uPdbid;
            public int nCampIndex;
            public int nHeroID;
            public int nCamp;
            public int nSex;
            public string sRankDesc;
            public int nHeroGameNum;
            public float fHeroWinRate;
            public bool bComputer;
            public string szVocationName;
            public int nSelfCamp;
        }

        // 存储逻辑层的数据 pdPid为索引 
        public Dictionary<ulong, WarPersonInfo> recordInfoArry = new Dictionary<ulong, WarPersonInfo>();

        // 存储逻辑层的数据 pdPid为索引 
        private Dictionary<ulong, WarPlayerVsInfo> dicPlayersVsInfo = new Dictionary<ulong, WarPlayerVsInfo>();

        #region war end Info
        // 英雄升级数据
        cmd_entity_statichero_upgrade m_CmdUpgradeInfo;
        public bool m_HeroIsUpgrade;
        // 战后奖励
        cmd_entity_return_player_awark m_cmdPlayerAwake;
        List<cmd_entity_hero_card> m_listHeroCard = new List<cmd_entity_hero_card>();

        // 炫耀相关玩家数据
        public struct swar_flaunt_result
        {
            public cmd_entity_end_player_data data;
            public Int64 dbWarID;
        }
        // 战后所有玩家的数据
        Dictionary<ulong, swar_flaunt_result> endRecordInfoArray = new Dictionary<ulong, swar_flaunt_result>();

        #endregion war end Info

        public void Init()
        {
            m_nMainHeroCamp = 0;
        }

        public int MainHeroCamp
        {
            get { return m_nMainHeroCamp; }
            set { m_nMainHeroCamp = value; }
        }

        public int getMainHeroCamp()
        {
            EntityView objMaster = EntityFactory.getMainPlayer();

            if (MainHeroCamp == 0)
            {
                MainHeroCamp = (objMaster != null ? objMaster.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP) : 0);
            }
            return MainHeroCamp;
        }

        public cmd_entity_statichero_upgrade HeroUpgradeInfo
        {
            get { return m_CmdUpgradeInfo; }
            set { m_CmdUpgradeInfo = value; }
        }

        public void setStaticHeroUpgradeInfo(cmd_entity_statichero_upgrade HeroUpgradeInfo, bool bIsUpgrade)
        {
            m_CmdUpgradeInfo = HeroUpgradeInfo;
            m_HeroIsUpgrade = bIsUpgrade;
            LogicDataCenter.playerSystemDataManager.HeroLevelChangeUpdate(HeroUpgradeInfo.nHeroID, HeroUpgradeInfo.nCurrntLv);
        }

        // 接收战场结束时玩家获得的奖励信息
        public void recvWarEndAwake(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_entity_return_player_awark obj = helper.get<cmd_entity_return_player_awark>();
            m_cmdPlayerAwake = obj;
            int nCount = obj.nCount;
            for (int i = 0; i < nCount; ++i)
            {
                cmd_entity_hero_card cmdInfo = helper.get<cmd_entity_hero_card>();
                m_listHeroCard.Add(cmdInfo);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO, null);
        }

        public cmd_entity_return_player_awark getPlayerAwake()
        {
            return m_cmdPlayerAwake;
        }

        public List<cmd_entity_hero_card> getListCardInfo()
        {
            return m_listHeroCard;
        }

        public bool checkIsShowAwarkUI()
        {
            if (m_cmdPlayerAwake.nActorGetExp != 0 || m_cmdPlayerAwake.nHeroExp != 0 || m_cmdPlayerAwake.nGold != 0 || m_cmdPlayerAwake.nSuccessScore != 0)
            {
                return true;
            }

            if (m_listHeroCard.Count != 0)
            {
                return true;
            }
            return false;
        }

        public void onLeaveBattleState()
        {
            // 缓存本局战绩
            getMainHeroWarStat(out DataCenter.LogicDataCenter.lifeHeroDataManager.LastWarStat);
            LogicDataCenter.lifeHeroDataManager.IsFromWarExit = true;

            isOneRequest = true;
            IsWarRecordTableVisible = false;
            recordInfoArry.Clear();

            m_HeroIsUpgrade = false;
            m_listHeroCard.Clear();
            endRecordInfoArray.Clear();
            m_nMainHeroCamp = 0;
            ClearLoadSenecePlayerVsData();
        }

        /// 实现相对阵营中 杀敌数 等级 助攻数更新
        public void UpdatePlayerInfo(IntPtr recordInfo, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(recordInfo, nPtrLen);
            cmd_war_personInfo_count obj = helper.get<cmd_war_personInfo_count>();

            for (int i = 0; i < obj.nCount; ++i)
            {
                cmd_war_personinfo data = helper.get<cmd_war_personinfo>();
                ulong uPdBid = data.uPDBID;
                if (uPdBid == 0) continue;
                int uCamp = data.nCamp;

                // 如果找到这个元素就清空重新把元素加入进去
                if (recordInfoArry.ContainsKey(uPdBid))
                {
                    // 找到的更新
                    WarPersonInfo tmpRecordInfo = recordInfoArry[uPdBid];
                    doPersonAliveAssistBrodcast(tmpRecordInfo, data.nAliveContAssist);
                    tmpRecordInfo.nLevel = data.nLevel;
                    tmpRecordInfo.nID = data.uID;
                    tmpRecordInfo.nKillNum = data.nKillNum;
                    tmpRecordInfo.nDeadNum = data.nDeadNum;
                    tmpRecordInfo.nAssists = data.nAssists;
                    tmpRecordInfo.nMobaWarMoney = data.nKillMonsterNum;
                    tmpRecordInfo.bIsHero = data.isProtag;
                    tmpRecordInfo.nAliveContAssist = data.nAliveContAssist;
                    tmpRecordInfo.nDestroyTower = data.nDestroyTower;
                    if (tmpRecordInfo.nSummerIconID <= 0)
                    {
                        tmpRecordInfo.nSummerIconID = data.nSummerIconID;
                        tmpRecordInfo.sSummerDesc = data.szSummerDesc;
                    }

                    recordInfoArry[uPdBid] = tmpRecordInfo;
                }
                else
                {
                    WarPersonInfo warPersonInfo = new WarPersonInfo();
                    FillWarPersonInfo(ref warPersonInfo, data);
                    recordInfoArry.Add(data.uPDBID, warPersonInfo);
                }
            }
            if (this.IsWarRecordTableVisible == true)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, null);
            }

            if (m_bUpdateTeamInfo)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATETEAMINFO, null);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_UPDATEPLAYERINFO, null);
        }

        private void doPersonAliveAssistBrodcast(WarPersonInfo info, int nUpdateAliveAssist)
        {
            if (nUpdateAliveAssist <= 0 || info.nAliveContAssist >= nUpdateAliveAssist)
            {
                return;
            }

            string sDesc = null;
            bool bChatShow = false;
            int nSoundID = 0;
            int nAssistType = 0;
            if (nUpdateAliveAssist == 5 || nUpdateAliveAssist == 10)
            {
                nSoundID = (int)WarKillSoundType.WKST_SELFCONTINUOUSASSIST + nUpdateAliveAssist / 5 - 1;
                sDesc = "连续助攻" + nUpdateAliveAssist + "次!";
                nAssistType = (int)BroadcastType.EMBT_ALIVEASSIST_2;
                if (info.nID != EntityFactory.MainHeroID)
                {
                    return;
                }
            }
            else if ((nUpdateAliveAssist - 15) % 5 == 0)
            {
                nSoundID = (int)WarKillSoundType.WKST_SELFGODLIKEASSIST;
                sDesc = "已完成了神助攻";
                nAssistType = (int)BroadcastType.EMBT_ALIVEASSIST_1;
                bChatShow = true;
            }
            else
            {
                return;
            }

            PlayerAliveAssistData data = new PlayerAliveAssistData();
            data.nHeroID = info.nHeroId;
            data.nPlayerName = info.playerName;
            data.szDesc = sDesc;
            data.bChatShow = bChatShow;
            data.nSoundID = nSoundID;
            data.nAssistType = nAssistType;
            data.nAssistCount = nUpdateAliveAssist;
            data.bSameCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP) == info.nCamp;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_ALIVEASSIST, data);
        }

        protected bool FillWarPersonInfo(ref WarPersonInfo warPersonInfo, cmd_war_personinfo recordInfo)
        {
            warPersonInfo.nHeroId = recordInfo.nHeroID;
            warPersonInfo.nCamp = recordInfo.nCamp;
            warPersonInfo.nLevel = recordInfo.nLevel;
            warPersonInfo.nID = recordInfo.uID;
            warPersonInfo.nKillNum = recordInfo.nKillNum;
            warPersonInfo.nIndex = GetAlwaysMyTeamLeftIndex(recordInfo.nIndex, recordInfo.nCamp);
            warPersonInfo.nDeadNum = recordInfo.nDeadNum;
            warPersonInfo.nAssists = recordInfo.nAssists;
            warPersonInfo.nMobaWarMoney = recordInfo.nKillMonsterNum;
            warPersonInfo.uPDBID = recordInfo.uPDBID;
            warPersonInfo.playerName = recordInfo.szName;
            warPersonInfo.bIsHero = recordInfo.isProtag;
            warPersonInfo.bIsAI = recordInfo.nIsAi > 0;
            warPersonInfo.nSummerIconID = recordInfo.nSummerIconID;
            warPersonInfo.sSummerDesc = recordInfo.szSummerDesc;
            warPersonInfo.nAliveContAssist = recordInfo.nAliveContAssist;
            warPersonInfo.nSkinID = recordInfo.nSkinID;
            warPersonInfo.nSex = recordInfo.nSex;
            warPersonInfo.nPlayerType = recordInfo.nPlayerType;
            warPersonInfo.nDestroyTower = recordInfo.nDestroyTower;

            return true;
        }

        // 设置isOneRequest  只设置一次
        public void setIsOneRequest()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT, null);

            this.isOneRequest = false;
        }

        // 进入战场后请求基本数据
        public void OnTabClick()
        {
            IntPtrHelper helper = new IntPtrHelper();
            int nIsOneRequest = Convert.ToInt16(isOneRequest);
            IntPtr ptr = helper.toPtr<int>(ref nIsOneRequest);
            int len = Marshal.SizeOf(nIsOneRequest);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_PLAYER_BASE_INFO, len, "", ptr, len);
        }

        // 更新boss刷新时间
        public void UpdateBossTimer(int nType, int nTime)
        {
            BossCdTime bossCdTime = new BossCdTime();
            bossCdTime.nIndex = nType;
            bossCdTime.nTime = nTime;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME, bossCdTime);

        }

        // 根据pdbid找对应的人物数据
        public WarPersonInfo getWarPersonInfoByPDBID(ulong pDBID)
        {
            if (recordInfoArry.ContainsKey(pDBID))
            {
                return recordInfoArry[pDBID];
            }

            return null;
        }

        public WarPersonInfo getWarPersonInfoByUID(int nUID)
        {
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nID == nUID)
                {
                    return info;
                }
            }

            return null;
        }

        public ulong getWarPersonPDBIDByUID(int nUID)
        {
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nID == nUID)
                {
                    return info.uPDBID;
                }
            }

            return 0;
        }

        // 根据pdbid找对应的索引
        public int getIndexAcordPDBID(ulong pDBID)
        {
            if (recordInfoArry.ContainsKey(pDBID))
            {
                return recordInfoArry[pDBID].nIndex;
            }

            Debug.LogWarning("Can't find index by pdbid, pdbid=" + pDBID);

            return -1;
        }

        // 根据pdbid找到玩家使用英雄的等级
        public int getHeroLvAcordPDBID(ulong pDBID)
        {
            if (recordInfoArry.ContainsKey(pDBID))
            {
                return recordInfoArry[pDBID].nLevel;
            }
            return -1;
        }

        // 接收战后统计数据
        public void recvWarEndPlayerData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            Int64 dbWarID = helper.getInt64();

            cmd_count cmdCount = helper.get<cmd_count>();
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_entity_end_player_data cmdInfo = helper.get<cmd_entity_end_player_data>();
                ulong pDbid = cmdInfo.pDbid;

                swar_flaunt_result result;
                result.data = cmdInfo;
                result.dbWarID = dbWarID;

                endRecordInfoArray[pDbid] = result;
            }

        }

        // 获得战后玩家数据
        public Dictionary<ulong, swar_flaunt_result> getWarEndPlayerData()
        {
            return endRecordInfoArray;
        }

        // 根据类型获得排名
        public int getMetalScoreLevel(War.ERecord_Data_Type dataType, ref int nSelfSocre)
        {
            ulong ulSelfID = (ulong)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            nSelfSocre = 0;
            if (endRecordInfoArray.ContainsKey(ulSelfID))
            {
                nSelfSocre = endRecordInfoArray[ulSelfID].data.gzAllData[(int)dataType];
            }

            int nLevel = 1;
            foreach (KeyValuePair<ulong, swar_flaunt_result> kvp in endRecordInfoArray)
            {
                ulong ulPdbid = kvp.Key;
                if (ulPdbid == ulSelfID)
                    continue;
                if (nLevel >= (int)UMedalScore.EN_PlayerGrade.ENPLAYERGRADE_MAX)
                    break;

                if (dataType != War.ERecord_Data_Type.ERDT_DEADNUM)
                {
                    nLevel = kvp.Value.data.gzAllData[(int)dataType] > nSelfSocre ? (++nLevel) : nLevel;
                }
                else if (dataType == War.ERecord_Data_Type.ERDT_DEADNUM)
                {
                    nLevel = kvp.Value.data.gzAllData[(int)dataType] < nSelfSocre ? (++nLevel) : nLevel;
                }
            }
            return nLevel;
        }

        // 计算某个类型数据的场均平均分
        public int getMeanScoreOneType(War.ERecord_Data_Type dataType)
        {
            int nSumScore = 0;
            foreach (KeyValuePair<ulong, swar_flaunt_result> kvp in endRecordInfoArray)
            {
                ulong ulPdbid = kvp.Key;
                nSumScore += kvp.Value.data.gzAllData[(int)dataType];
            }
            int nPlayerCount = endRecordInfoArray.Count();
            if (nPlayerCount == 0)
                return 0;
            return nSumScore / nPlayerCount;
        }

        // 获得金钱，杀人， 死亡， 助攻最多的次数
        public void getDataTop(ref int nScoreTop, ref int nMoneyTop, ref int nKillTop, ref int nDietop, ref int nAssistTop)
        {
            //foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in recordInfoArry)
            foreach (var item in endRecordInfoArray)
            {
                //int nIndex = item.Value.data.;
                cmd_entity_end_player_data warInfo = item.Value.data;
                if (nScoreTop < warInfo.nScore)
                    nScoreTop = warInfo.nScore;
                if (nMoneyTop < warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_MONEY])
                    nMoneyTop = warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_MONEY];
                if (nKillTop < warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_KILLNUM])
                    nKillTop = warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_KILLNUM];
                if (nDietop < warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_DEADNUM])
                    nDietop = warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_DEADNUM];
                if (nAssistTop < warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_ASSIST])
                    nAssistTop = warInfo.gzAllData[(int)ERecord_Data_Type.ERDT_ASSIST];
            }
        }

        // 获得战绩最好成绩
        public int[] getDataTopEx()
        {
            int[] result = new int[(int)War.ERecord_Data_Type.ERDT_MAX1];
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.swar_flaunt_result> kvp in endRecordInfoArray)
            {
                int[] gzAllData = kvp.Value.data.gzAllData;
                for (int i = 0; i < (int)War.ERecord_Data_Type.ERDT_MAX1; ++i)
                {
                    if (gzAllData[i] > result[i])
                        result[i] = gzAllData[i];
                }
            }

            return result;
        }

        /// 更新玩家对战信息
        public void UpdatePlayersVsInfo(cmd_match_load_scene_playerinfo data)
        {
            // 如果找到这个元素就清空重新把元素加入进去
            if (dicPlayersVsInfo.ContainsKey(data.idActor))
            {
                return;
            }

            WarPlayerVsInfo warPersonVsInfo = new WarPlayerVsInfo();
            warPersonVsInfo.uPdbid = data.idActor;
            warPersonVsInfo.nHeroID = data.nHeroID;
            warPersonVsInfo.nCamp = data.nCamp;
            warPersonVsInfo.nSex = data.cmdPlayerVsDetails.nSex;
            warPersonVsInfo.nHeroGameNum = data.cmdPlayerVsDetails.nHeroGameNum;
            warPersonVsInfo.fHeroWinRate = data.cmdPlayerVsDetails.fHeroWinRate;
            warPersonVsInfo.sRankDesc = data.cmdPlayerVsDetails.szGradeName;
            warPersonVsInfo.nCampIndex = data.nCampIndex;
            warPersonVsInfo.bComputer = data.nIsComputer > 0;
            warPersonVsInfo.szVocationName = data.szVocationName;
            warPersonVsInfo.nSelfCamp = data.nSelfCamp;

            dicPlayersVsInfo.Add(data.idActor, warPersonVsInfo);

            return;
        }

        public WarPlayerVsInfo GetWarPlayerVsInfo(ulong idActor)
        {
            WarPlayerVsInfo info = null;
            dicPlayersVsInfo.TryGetValue(idActor, out info);

            return info;
        }

        public Dictionary<ulong, WarPlayerVsInfo> GetWarDicPlayerVsInfo()
        {
            return dicPlayersVsInfo;
        }

        public int GetAlwaysMyTeamLeftIndex(int nIndex, int nCamp)
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return -1;

            // 观战者的阵营为0
            m_MainPlayerCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
            if (m_MainPlayerCamp == 0)
            {
                return nIndex;
            }

            return GetAlwaysMyTeamLeftIndex(nIndex, nCamp, m_MainPlayerCamp);
        }

        public int GetAlwaysMyTeamLeftIndex(int nIndex, int nCamp, int nCompareCamp)
        {
            int nRealIndex = nIndex;

            if (nCamp == nCompareCamp)
            {
                if (nRealIndex >= m_ModelHalfPlayerCount)
                {
                    nRealIndex -= m_ModelHalfPlayerCount;
                }
            }
            else
            {
                if (nRealIndex < m_ModelHalfPlayerCount)
                {
                    nRealIndex += m_ModelHalfPlayerCount;
                }
            }

            return nRealIndex;
        }

        public int GetMainPlayerCamp()
        {
            return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
        }

        public int GetHalfPlayerCount()
        {
            return m_ModelHalfPlayerCount;
        }

        public void ClearLoadSenecePlayerVsData()
        {
            dicPlayersVsInfo.Clear();
        }

        public int getWarSameCampPersonKillNum(int nCamp)
        {
            int nKillNum = 0;
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nCamp == nCamp)
                {
                    nKillNum += info.nKillNum;
                }
            }

            return nKillNum;
        }

        public int getWarOtherCampPersonKillNum(int nCamp)
        {
            int nKillNum = 0;
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nCamp != nCamp)
                {
                    nKillNum += info.nKillNum;
                }
            }

            return nKillNum;
        }

        public int GetTeamTotalMoney(int nCamp)
        {
            int money = 0;
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nCamp == nCamp)
                {
                    money += info.nMobaWarMoney;
                }
            }

            return money;
        }

        public int GetTeamTotalDestroyTower(int nCamp)
        {
            int destroyTower = 0;
            foreach (WarPersonInfo info in recordInfoArry.Values)
            {
                if (info.nCamp == nCamp)
                {
                    destroyTower += info.nDestroyTower;
                }
            }

            return destroyTower;
        }

        // 获取本地玩家的战绩
        public bool getMainHeroWarStat(out cmd_entity_end_player_data data)
        {
            data = new cmd_entity_end_player_data();
            foreach (var item in endRecordInfoArray)
            {
                if (item.Value.data.bIsSelf > 0)
                {
                    data = item.Value.data;
                    return true;
                }
            }

            return false;
        }

        // 结算界面是否自动显示分享战绩窗口
        public bool isWarEndShowShareWnd()
        {
            return false;

            cmd_entity_end_player_data warStat;
            if (!getMainHeroWarStat(out warStat))
                return false;


            if (warStat.nScore >= 750)
                return true;

            if (warStat.nUpgradeRank > 0)
                return true;

            return false;
        }

        // 战场玩家信息更新
        public void updatePersonInfo(ulong pdbid, EWarPersonInfoUpdateType updateType, IntPtrVaArgParseHelper helper)
        {
            WarPersonInfo person;
            if (recordInfoArry.TryGetValue(pdbid, out person) == false)
            {
                Debug.LogWarning("recordInfoArry.ContainsKey(pdbid) == false, pdbid=" + pdbid);
                return;
            }

            switch (updateType)
            {
                case EWarPersonInfoUpdateType.EWPTUT_VOCATION:
                    {
                        person.nHeroId = helper.getInt();
                        person.nSkinID = helper.getInt();

                        Debug.Log("战场人物数据更新：英雄ID=" + person.nHeroId + ",皮肤=" + person.nSkinID);

                        // 更新头像
                        UIWarUpdateHeroIcon uiMsgData = new UIWarUpdateHeroIcon();
                        uiMsgData.msgID = (int)WndMsgID.WND_MSG_WAR_UPDATE_HEROICON;
                        uiMsgData.nHeroID = person.nHeroId;
                        uiMsgData.nSkinID = person.nSkinID;
                        uiMsgData.nPdbid = pdbid;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_UPDATE_HEROICON, uiMsgData);
                    }
                    break;
                default: break;
            }
        }
    }
}
