using ASpeedGame.Data.HeroTalentConfig;
using ASpeedGame.Data.PrizeUnlockConfig;
using GameLogic;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using USpeedUI;
using Data.SideButtonConfig;
using USpeedUI.PlayerFrame;
using War;
using UnityEngine;
using Data.ActorPrizeConfig;
using ASpeedGame.Data.RunePage;

namespace DataCenter
{
    public enum HeroDetailReqType
    {
        HDRT_INVALID = 0,       // 无效
        HDRT_INIT,              // 初始化
        HDRT_REFRESH,           // 更新
        HDRT_LIFEHERO,          // 本命英雄获取
    }

    public struct PDBIDandHeroID : IEquatable<PDBIDandHeroID>
    {
        public int nPDBID;
        public int nHeroID;

        public PDBIDandHeroID(int pdbid, int heroid)
        {
            this.nPDBID = pdbid;
            this.nHeroID = heroid;
        }

        public bool Equals(PDBIDandHeroID other)
        {
            return nPDBID == other.nPDBID && nHeroID == other.nHeroID;
        }
    }

    // 玩家界面
    public class ActorPlayerInfo
    {
        public cmd_player_info_return playerInfo;
        public List<cmd_entity_hero_info> strongHeroInfoList;
    }

    // 英雄界面
    public class ActorHeroInfo
    {
        public Dictionary<int, cmd_entity_hero_info> dicEntityHeroInfo;
        public Dictionary<int, cmd_entity_nohanve_hero> dicEntityNoHaveHeroInfo;
    }

    // 符文界面
    public class OtherActorRuneInfo
    {
        public cmd_Rune_Info_Total totalRune;
        public List<cmd_Rune_Single_PageInfo> totalRunePage;
    }

    // 英雄详细信息界面
    public class ActorHeroDetailInfo
    {
        public cmd_entity_hero_detail_info heroDetailInfo;
        public cmd_count cmdCount;
        public List<cmd_match_room_hero_skill_desc> heroSkillDesc;
    }

    // 增益卡相关UI
    #region
    public class BenefitCardInfo
    {
        private int m_nBenefitCardType;      // 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
        private int m_nBenefitCardSubType;   // 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
        private long m_nBenefitValue;        // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间；为2时，表示剩余使用次数
        private int m_nMultiple;			 // 增益卡的增益倍数

        private string m_CardName;           // 增益卡名字

        public int CardType { get { return m_nBenefitCardType; } }

        static private Dictionary<int, string> m_nameList = new Dictionary<int, string>{
            {(int)BENEFITCARDTYPE.BENEFITCARDTYPE_GOLD, "金币"},
            {(int)BENEFITCARDTYPE.BENEFITCARDTYPE_ACTOR_EXP, "角色经验"},
            {(int)BENEFITCARDTYPE.BENEFITCARDTYPE_HERO_EXP, "英雄经验"},
        };

        public BenefitCardInfo(int type, int subType, long value, int multiple)
        {
            m_nBenefitCardType = type;
            m_nBenefitCardSubType = subType;
            m_nBenefitValue = value;
            m_nMultiple = multiple;

            if (!m_nameList.TryGetValue(m_nBenefitCardType, out m_CardName))
            {
                m_CardName = "未知卡";
            }
        }

        /// <summary>
        /// 根据时间戳计算相差几天几小时，返回字符串
        /// </summary>
        /// <param name="timeStamp"></param>
        /// <returns></returns>
        private string getReaminTime(long timeStamp)
        {
            DateTime dtEnd = Api.convertTimeStamp2DateTime(timeStamp);

            TimeSpan span = dtEnd - System.DateTime.Now;

			int remainDays = Math.Max(span.Days, 0);
			int remainHours = Math.Max(span.Hours, 0);

            string result = String.Format("{0}天{1}小时", remainDays, remainHours);

            return result;
        }

        public bool isValid()
        {
            bool bValid = true;
            if (m_nBenefitCardSubType == (int)BENEFITCARDSUBTYPE.BENEFITCARDSUBTYPE_PERIOD)
            {
                int now = Api.convertDateTime2TimeStamp(DateTime.Now);
                bValid = now < m_nBenefitValue;
            }
            else if (m_nBenefitCardSubType == (int)BENEFITCARDSUBTYPE.BENEFITCARDSUBTYPE_USECOUNT)
            {
                bValid = m_nBenefitValue > 0;
            }

            return bValid;
        }

        public string getRemainTimeDesc()
        {
            string result = "";
            if (m_nBenefitCardSubType == (int)BENEFITCARDSUBTYPE.BENEFITCARDSUBTYPE_PERIOD)
            {
                string str = getReaminTime(m_nBenefitValue);

                result = String.Format("{0}倍{1}（剩余{2}）", m_nMultiple, m_CardName, str);

            }
            else if (m_nBenefitCardSubType == (int)BENEFITCARDSUBTYPE.BENEFITCARDSUBTYPE_USECOUNT)
            {
                result = String.Format("{0}倍{1}（剩余{2}次）", m_nMultiple, m_CardName, m_nBenefitValue);
            }

            return result;
        }

        public bool isSameType(int nMainType, int nSubType)
        {
            return m_nBenefitCardType == nMainType && m_nBenefitCardSubType == nSubType;
        }
    }

    public class BenefitCardInfoManager
    {
        // 剩余卡信息
        private List<BenefitCardInfo> m_remainCardList;
        public List<BenefitCardInfo> RemainCardList { get { return m_remainCardList; } }

        // 本局触发效果的卡信息
        private List<BenefitCardInfo> m_usedCardList;
        public List<BenefitCardInfo> UsedCardList { get { return m_usedCardList; } }
        
        public void init()
        {
            m_remainCardList = new List<BenefitCardInfo>();

            m_usedCardList = new List<BenefitCardInfo>();
        }

        public void addBenefitCard(int nListType, ref List<BenefitCardInfo> cardList)
        {
            if (nListType == (int)BENEFITCARDLISTTYPE.BENEFITCARDLISTTYPE_REMAIN)
            {
                m_remainCardList = cardList;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_UPDATEBOTTOMDESC, null);
            }
            else if(nListType == (int)BENEFITCARDLISTTYPE.BENEFITCARDLISTTYPE_USED)
            {
                m_usedCardList = cardList;
            }
        }

        public BenefitCardInfo getRemainBenefitCard(int nMainType, int nSubType)
        {
            foreach (var item in m_remainCardList)
            {
                if (item.isSameType(nMainType, nSubType))
                {
                    return item;
                }
            }

            return null;
        }

        
        public Dictionary<int, string> getRemainDesc()
        {
            Dictionary<int, string> result = new Dictionary<int, string>();

            foreach(var item in m_remainCardList)
            {
                // 已经无效，则跳过
                if (!item.isValid())
                {
                    continue;
                }

                string oldDesc;
                if (result.TryGetValue(item.CardType, out oldDesc))
                {
                    oldDesc += "\n" + item.getRemainTimeDesc();
                }
                else
                {
                    oldDesc = item.getRemainTimeDesc();
                }
                result[item.CardType] = oldDesc;
            }

            return result;
        }
    }
    #endregion

    public class PlayerSystemDataManager
    {
        private Dictionary<int, ActorPlayerInfo> actorPlayer;
        private Dictionary<int, ActorHeroInfo> actorHero;
        private Dictionary<int, OtherActorRuneInfo> actorRune;
        private Dictionary<PDBIDandHeroID, ActorHeroDetailInfo> actorHeroDetail;

        private BenefitCardInfoManager benefitCardInfoManager;
        private const int nDefaultSelfActorID = -1;
        private int m_nSelfActorID = -1;
        public int SelfAcotrID { get {  return m_nSelfActorID; } }
        private HeroDetailReqType eHeroDetailReqType = HeroDetailReqType.HDRT_INVALID;
        public bool bInReconnect = true; // 依据为大厅不选人则为不是断线重连
        private bool m_bRequestSelf;
        public bool bRequestSelf
        {
            get { return m_bRequestSelf; }

            set {
                m_bRequestSelf = value;
                if (m_bRequestSelf)
                {
                    bFake = false;
                    strCurFakeName = "";
                }
            }
        }

        public int nRequestPDBID;
        public bool bFake;
        public string strCurFakeName;
        public string sFixActorName = null;
        public int SelfGameState;       // 玩家的游戏状态

        private cmd_entity_heroupstar_unlockprize heroUpStarPrize;  // 英雄升星奖励

        private List<cmd_label_info> m_LabelList;
        // 玩家升级奖励缓存
        private int m_nWarEndActorLv = 0;
        public int WarEndActorLv { get { return m_nWarEndActorLv; } set { m_nWarEndActorLv = value; } }
        private List<int> m_listActorUnlockPrizeID;
        public List<int> ListActorUnlockPrizeID { get { return m_listActorUnlockPrizeID; } set { m_listActorUnlockPrizeID = value; } }

        public void Init()
        {
            actorPlayer = new Dictionary<int, ActorPlayerInfo>();
            actorHero = new Dictionary<int, ActorHeroInfo>();
            actorRune = new Dictionary<int, OtherActorRuneInfo>();
            actorHeroDetail = new Dictionary<PDBIDandHeroID, ActorHeroDetailInfo>();
            benefitCardInfoManager = new BenefitCardInfoManager();
            benefitCardInfoManager.init();
            heroUpStarPrize = new cmd_entity_heroupstar_unlockprize();

            bRequestSelf = true;
            if (EntityFactory.MainHeroView != null)
            {
                nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }
            bFake = false;
            strCurFakeName = "";
            m_LabelList = new List<cmd_label_info>();
            m_listActorUnlockPrizeID = new List<int>();

            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_ID:
                    {
                        int nActorID = e.data.nValue;
                        ActorHeroInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero(nActorID);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
                        checkAllHeroIsHaveCanUpStar();
                    }
                    break;
                default:
                    break;
            }
        }

        public void Clear()
        {
            actorPlayer.Clear();
            //actorHero.Clear();
            actorRune.Clear();
            actorHeroDetail.Clear();
            m_LabelList.Clear();
            m_nSelfActorID = -1;
            bRequestSelf = true;
            bFake = false;
            strCurFakeName = "";
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public ActorPlayerInfo GetActorPlayerInfo(int actorID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref actorID);

            ActorPlayerInfo info = null;
            if (actorPlayer.TryGetValue(actorID, out info))
            {
                return info;
            }
            else
            {
                info = new ActorPlayerInfo();
                info.playerInfo = new cmd_player_info_return();
                info.strongHeroInfoList = new List<cmd_entity_hero_info>();
                actorPlayer[actorID] = info;
            }

            RequestPlayerInfo(actorID);
            return info;
        }

        public ActorHeroInfo GetActorAllHero(int actorID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref actorID);

            ActorHeroInfo info = null;
            if (actorHero.TryGetValue(actorID, out info))
            {
                return info;
            }
            else
            {
                info = new ActorHeroInfo();
                info.dicEntityHeroInfo = new Dictionary<int, cmd_entity_hero_info>();
                info.dicEntityNoHaveHeroInfo = new Dictionary<int, cmd_entity_nohanve_hero>();
                actorHero[actorID] = info;
            }

            RequestAllHero(actorID);
            return info;
        }

        public OtherActorRuneInfo GetActorAllRune(int actorID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref actorID);

            OtherActorRuneInfo info = null;
            if (actorRune.TryGetValue(actorID, out info))
            {
                return info;
            }
            else
            {
                info = new OtherActorRuneInfo();
                info.totalRune = new cmd_Rune_Info_Total();
                info.totalRunePage = new List<cmd_Rune_Single_PageInfo>();
                actorRune[actorID] = info;
            }

            RequestAllRune(actorID);
            return info;
        }

        public ActorHeroDetailInfo GetActorHeroDetail(int heroID, int actorID = nDefaultSelfActorID, HeroDetailReqType eType = HeroDetailReqType.HDRT_INIT)
        {
            CheckGetSelfPDBID(ref actorID);

            ActorHeroDetailInfo info = null;
            PDBIDandHeroID multiKey = new PDBIDandHeroID(actorID, heroID);
            if (actorHeroDetail.TryGetValue(multiKey, out info))
            {
                return info;
            }
            else
            {
                info = new ActorHeroDetailInfo();
                info.heroDetailInfo = new cmd_entity_hero_detail_info();
                info.cmdCount.nCount = 0;
                info.heroSkillDesc = new List<cmd_match_room_hero_skill_desc>();
                actorHeroDetail[multiKey] = info;
            }

            RequestHeroDetailInfo(multiKey, eType);
            return info;
        }

        public void RequestPlayerInfo(int actorID)
        {
            cmd_Request_Player_Info cmdInfo = new cmd_Request_Player_Info();
            cmdInfo.nActorID = (uint)actorID;
            cmdInfo.bFake = bFake == true? 1: 0;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_Request_Player_Info>(ref cmdInfo);
            int len = Marshal.SizeOf(cmdInfo);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_PLAYER_BASE_INFO, len, "", ptr, len);
        }

        public void RequestAllHero(int actorID)
        {
            cmd_Request_Hero_Info cmdInfo = new cmd_Request_Hero_Info();
            cmdInfo.nActorID = (uint)actorID;
            cmdInfo.bFake = bFake == true ? 1 : 0;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_Request_Hero_Info>(ref cmdInfo);
            int len = Marshal.SizeOf(cmdInfo);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_HERO_INFO, len, "", ptr, len);
        }

        public void RequestAllRune(int actorID)
        {
            cmd_Request_Rune_Info cmdRequestPointAll = new cmd_Request_Rune_Info();
            cmdRequestPointAll.bIsRequestSelf = 0;
            cmdRequestPointAll.RequestFriendID = (uint)actorID;
            EntityEventHelper.Instance.SendCommand<cmd_Request_Rune_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_QUERY_OTHER_RUNE, ref cmdRequestPointAll);
        }

        public void RequestHeroDetailInfo(PDBIDandHeroID nMultiKey, HeroDetailReqType eType = HeroDetailReqType.HDRT_INIT)
        {
            // 设置详细英雄界面信息请求标志位
            eHeroDetailReqType = eType;

            cmd_entity_hero_info cmdHeroInfo = new cmd_entity_hero_info();

            // Player界面取一次
            ActorPlayerInfo playerInfo = null;
            if (actorPlayer.TryGetValue(nMultiKey.nPDBID, out playerInfo))
            {
                foreach (var value in playerInfo.strongHeroInfoList)
                {
                    if (value.nHeroID == nMultiKey.nHeroID)
                    {
                        cmdHeroInfo = value;
                        break;
                    }
                }
            }

            // Hero界面取一次
            if (cmdHeroInfo.nHeroID == 0)
            {
                ActorHeroInfo heroInfo = null;
                if (actorHero.TryGetValue(nMultiKey.nPDBID, out heroInfo))
                {
                    heroInfo.dicEntityHeroInfo.TryGetValue(nMultiKey.nHeroID, out cmdHeroInfo);
                }
            }

            // nHeroID > 0,则表示该英雄有数据；bIsBuy > 0,则表示已购买
            cmd_Request_Hero_Detail_Info reqHeroDetailInfo = new cmd_Request_Hero_Detail_Info();
            reqHeroDetailInfo.nActorID = (uint)nMultiKey.nPDBID;
            reqHeroDetailInfo.nHeroID = nMultiKey.nHeroID;
            reqHeroDetailInfo.heroInfo = cmdHeroInfo;
            
            EntityEventHelper.Instance.SendCommand<cmd_Request_Hero_Detail_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_QUERY_HERO_DETAIL_INFO, ref reqHeroDetailInfo);
        }

        public void UpdatePlayerInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_player_info_return playerInfo = helper.get<cmd_player_info_return>();
            ActorPlayerInfo info = null;
            if (actorPlayer.TryGetValue((int)playerInfo.pDbid, out info) == false)
            {
                return;
            }
            info.playerInfo = playerInfo;

            info.strongHeroInfoList.Clear();
            for (int i = 0; i < info.playerInfo.nCount; ++i)
            {
                cmd_entity_hero_info data = helper.get<cmd_entity_hero_info>();
                info.strongHeroInfoList.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, null);
        }

        public void UpdataAllHero(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_pdbid_count haveHeroInfo = helper.get<cmd_pdbid_count>();
            ActorHeroInfo info = null;
            if (actorHero.TryGetValue(haveHeroInfo.dwPDBID, out info) == false)
            {
                return;
            }

            info.dicEntityHeroInfo.Clear();
            for (int i = 0; i < haveHeroInfo.nCount; ++i)
            {
                cmd_entity_hero_info data = helper.get<cmd_entity_hero_info>();
                info.dicEntityHeroInfo[data.nHeroID] = data;
            }

            cmd_count noHaveHeroInfo = helper.get<cmd_count>();
            info.dicEntityNoHaveHeroInfo.Clear();
            for (int i = 0; i < noHaveHeroInfo.nCount; i++)
            {
                cmd_entity_nohanve_hero data1 = helper.get<cmd_entity_nohanve_hero>();
                info.dicEntityNoHaveHeroInfo[data1.nHeroID] = data1;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, null);
            checkAllHeroIsHaveCanUpStar();

            // 更新已下发详细英雄信息
            int actorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            foreach (var item in actorHeroDetail)
            {
                if (item.Key.nPDBID == actorID)
                {
                    RequestHeroDetailInfo(item.Key, HeroDetailReqType.HDRT_REFRESH);
                }
            }
        }

        public void UpdateAllRune(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_Rune_Info_Total runeInfo = helper.get<cmd_Rune_Info_Total>();
            OtherActorRuneInfo info = null;
            if (actorRune.TryGetValue(runeInfo.dwActorID, out info) == false)
            {
                return;
            }
            info.totalRune = runeInfo;

            info.totalRunePage.Clear();
            for (int i = 0; i < info.totalRune.nPageNum; ++i)
            {
                cmd_Rune_Single_PageInfo data = helper.get<cmd_Rune_Single_PageInfo>();
                info.totalRunePage.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITOTHERRUNEDATA, null);
        }

        public void UpdateHeroDetailInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            int dwPDBID = helper.get<int>();
            cmd_entity_hero_detail_info heroDetailInfo = helper.get<cmd_entity_hero_detail_info>();

            PDBIDandHeroID multiKey = new PDBIDandHeroID(dwPDBID, heroDetailInfo.nHeroID);
            ActorHeroDetailInfo info = null;
            if (actorHeroDetail.TryGetValue(multiKey, out info) == false)
            {
                return;
            }
            info.heroDetailInfo = heroDetailInfo;

            cmd_count cmdCount = helper.get<cmd_count>();
            info.cmdCount = cmdCount;
            for (int i = 0; i < cmdCount.nCount; i++)
            {
                cmd_match_room_hero_skill_desc skillDesc = helper.get<cmd_match_room_hero_skill_desc>();
                info.heroSkillDesc.Add(skillDesc);
            }
                
            cmd_Return_Hero_Detail_Info returnHeroInfo;
            returnHeroInfo.nPDBID = dwPDBID;
            returnHeroInfo.nHeroID = heroDetailInfo.nHeroID;
            UIMsgCmdData uiReturnHeroInfo = null;

            if (eHeroDetailReqType == HeroDetailReqType.HDRT_INIT)
            {
                uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, ref returnHeroInfo);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, uiReturnHeroInfo);
            }
            else if (eHeroDetailReqType == HeroDetailReqType.HDRT_REFRESH)
            {
                uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFOREFRESH, ref returnHeroInfo);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFOREFRESH, uiReturnHeroInfo);
            }
            else if (eHeroDetailReqType == HeroDetailReqType.HDRT_LIFEHERO)
            {
                uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, ref returnHeroInfo);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, uiReturnHeroInfo);
            }
        }

        public void UpgradeStartProAndExpend(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_entity_hero_diff_expend cmdHeroDetailInfo = helper.get<cmd_entity_hero_diff_expend>();
            int dwPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            PDBIDandHeroID multiKey = new PDBIDandHeroID(dwPDBID, cmdHeroDetailInfo.nHeroID);
            ActorHeroDetailInfo info = null;
            if (actorHeroDetail.TryGetValue(multiKey, out info) == false)
            {
                return;
            }

            info.heroDetailInfo.nStar = cmdHeroDetailInfo.nStarLv;
            info.heroDetailInfo.nStarExp = cmdHeroDetailInfo.nHaveCardCount;
            info.heroDetailInfo.nStarNeedExp = cmdHeroDetailInfo.nExpendCard;
            info.heroDetailInfo.fHP = cmdHeroDetailInfo.fHP;
            info.heroDetailInfo.fMA = cmdHeroDetailInfo.fMA;
            info.heroDetailInfo.fMagic = cmdHeroDetailInfo.fMagic;
            info.heroDetailInfo.fMD = cmdHeroDetailInfo.fMD;
            info.heroDetailInfo.fPA = cmdHeroDetailInfo.fPA;
            info.heroDetailInfo.fPD = cmdHeroDetailInfo.fPD;
        }

        public void UpdateHeroInfo(IntPtr ptrParam, int nPtrLen)
        {
            int actorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            int nCount = helper.get<int>();

            for (int i = 0; i < nCount; ++i)
            {
                bool bNewBuy = false;
                bool bOldBuy = false;
                
                cmd_entity_hero_info data = helper.get<cmd_entity_hero_info>();
                ActorPlayerInfo playInfo = null;
                ActorHeroInfo info = null;
                int nHeroID = data.nHeroID;
                bNewBuy = data.bIsBuy>0;
    
                if (actorHero.TryGetValue(actorID, out info) == true)
                {
                    // 英雄总览界面数据更新
                    if (info.dicEntityHeroInfo.ContainsKey(nHeroID) == true)
                    {
                        bOldBuy = info.dicEntityHeroInfo[nHeroID].bIsBuy>0;
                        info.dicEntityHeroInfo[nHeroID] = data;
                    }
                    else if (info.dicEntityNoHaveHeroInfo.ContainsKey(nHeroID) == true)
                    {
                        info.dicEntityNoHaveHeroInfo.Remove(nHeroID);
                        info.dicEntityHeroInfo[nHeroID] = data;
                    }
                }

                if ((actorPlayer.TryGetValue(actorID, out playInfo) == true))
                {
                    // 玩家界面数据更新
                    for (int j = 0; j < playInfo.strongHeroInfoList.Count; j++)
                    {
                        if (playInfo.strongHeroInfoList[j].nHeroID == nHeroID)
                        {
                            playInfo.strongHeroInfoList[j] = data;
                        }
                    }
                }

                PlayerHeroUpdateData msgData = new PlayerHeroUpdateData();
                msgData.nHeroID = nHeroID;
                msgData.nReqPDBID = actorID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, msgData);

                if (bNewBuy && !bOldBuy)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMBUYHERO, msgData);
                }
  
                // 英雄详细界面数据更新
                PDBIDandHeroID multiKey = new PDBIDandHeroID(actorID, nHeroID);
                RequestHeroDetailInfo(multiKey, HeroDetailReqType.HDRT_REFRESH);
            }
        }

        public void ActorInfoChangeUpdate(IntPtr ptrParam, int nPtrLen, int nActorID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref nActorID);
            if (!actorPlayer.ContainsKey(nActorID))
            {
                return;
            }

            cmd_actor_info_change data = IntPtrHelper.toData<cmd_actor_info_change>(ptrParam);

            switch (data.nType)
            {
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_LEVEL:
                    {
                        actorPlayer[nActorID].playerInfo.nLv = data.nValue1;
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_PKWINNUM:
                    {
                        actorPlayer[nActorID].playerInfo.nPKWinNum = data.nValue1;
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_PKTOTALNUM:
                    {
                        actorPlayer[nActorID].playerInfo.nPKTotalNum = data.nValue1;
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_EXP:
                    {
                        actorPlayer[nActorID].playerInfo.ulCurrentExp = (uint)data.nValue1;
                        actorPlayer[nActorID].playerInfo.ulNeedExp = (uint)data.nValue2;
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_NAME:
                    {
                        actorPlayer[nActorID].playerInfo.chPlayerName = data.str1;

                        if (EntityFactory.MainHeroView != null)
                        {
                            CreatureProperty property = EntityFactory.MainHeroView.Property;
                            if (property != null)
                            {
                                property.CreatureName = data.str1;
                            }
                        }
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_GLORY:
                    {
                        for (int i = 0; i < (int)ENACHIEVE.ENACHIEVE_MAX; i++)
                        {
                            actorPlayer[nActorID].playerInfo.nGlory[i] = data.nGlory[i];
                            actorPlayer[nActorID].playerInfo.llWarID[i] = data.llWarID[i];
                        }
                    }
                    break;
                case (int)ENPLAYERINFOTYPE.ENPLAYERINFOTYPE_RANKNUM:
                    {
                        actorPlayer[nActorID].playerInfo.nRankMatchNum = data.nValue1;
                        actorPlayer[nActorID].playerInfo.nRankWinNum = data.nValue2;
                    }
                    break;
                default:
                    break;
            }
        }

        public void HeroLevelChangeUpdate(int nHeroId,int nHeroLevel, int nActorID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref nActorID);
            PDBIDandHeroID multiKey = new PDBIDandHeroID(nActorID, nHeroId);
            if (!actorHeroDetail.ContainsKey(multiKey))
            {
                return;
            }

            actorHeroDetail[multiKey].heroDetailInfo.nLv = nHeroLevel;
        }

        public int GetHeroStarLv(int heroID, int acotrID = nDefaultSelfActorID)
        {
            CheckGetSelfPDBID(ref acotrID);
            int starLv = -1;
            ActorHeroInfo heroInfo;
            if (actorHero.TryGetValue(acotrID, out heroInfo))
            {
                cmd_entity_hero_info entityHeroInfo;
                if (heroInfo.dicEntityHeroInfo.TryGetValue(heroID, out entityHeroInfo))
                {
                    return entityHeroInfo.byStarLv;
                }
            }

            return starLv;
        }

        private void CheckGetSelfPDBID(ref int actorID)
        {
            if (actorID != nDefaultSelfActorID)
            {
                return;
            }

            //// 设置为请求自己
            //if (m_nSelfActorID > 0)
            //{
            //    actorID = m_nSelfActorID;
            //    return;
            //}
            
            if (EntityFactory.MainHeroView==null)
            {
                return;
            }

            m_nSelfActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            actorID = m_nSelfActorID;
            return;
        }

        public void Reset()
        {
            if (StageManager.Instance.getCurrentState() != EStage_State.ESS_GAME)
            {
                return;
            }

            Clear();
            GetActorPlayerInfo();
        }

        public BenefitCardInfoManager GetBenefitCardManager()
        {
            return benefitCardInfoManager;
        }

        public bool CheckIsHaveHero(int nHeroID, int nActorID = nDefaultSelfActorID)
        {
            ActorHeroInfo info = GetActorAllHero(nDefaultSelfActorID);
            if (info == null)
                return false;

            foreach (cmd_entity_hero_info item in info.dicEntityHeroInfo.Values)
            {
                if (item.nHeroID == nHeroID)
                {
                    if (item.bIsBuy > 0)
                        return true;
                }
            }

            return false;
        }

        public void updatePlayerGameState(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_entity_update_gamestate cmd = helper.get<cmd_entity_update_gamestate>();
            SelfGameState = cmd.nGameState;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_UPDATEGAMESTATE);
        }

        public void RecvHeroUpStarUnlockPrize(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            heroUpStarPrize = helper.get<cmd_entity_heroupstar_unlockprize>();
        }

        public void ShowHeroUpStarUnlockPrize()
        {
            if (heroUpStarPrize.nHeroID <= 0)
                return;

            List<HeroStarPrizeUnlockItem> starPrizeUnlockList = PrizeUnlockConfig.Instance.heroStarPrizeUnlockDic[heroUpStarPrize.nHeroID];
            if (starPrizeUnlockList == null)
                return;
            HeroStarPrizeUnlockItem prizeConfig = null;
            foreach (var item in starPrizeUnlockList)
            {
                if (item.nUnlockLevel == heroUpStarPrize.nStarLevel)
                {
                    prizeConfig = item;
                    break;
                }
            }
            if (prizeConfig == null)
                return;


            UPrizePopupData uiData = new UPrizePopupData();
            uiData.strPrizeTitle = String.Format(ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PrizeTitle"), heroUpStarPrize.nStarLevel);
            uiData.strPrizeDesc = "";
            uiData.nPrizeIdList = new List<int>(prizeConfig.nPrizeIdList);
            uiData.nHeroID = heroUpStarPrize.nHeroID;
            uiData.nTalentIDList = HeroTalentConfigPage.Instance.getNextPrizeTalentID(heroUpStarPrize.nHeroID, heroUpStarPrize.nStarLevel);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_OPEN, uiData);

            // 重新赋值为0 
            heroUpStarPrize.nHeroID = 0;
            heroUpStarPrize.nStarLevel = 0;
        }

        public void ShowActorUnlockPrize(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            m_nWarEndActorLv = helper.get<int>();
            int nPrizeCount = helper.get<int>();
            for (int i = 0; i < nPrizeCount; i++)
            {
                int nPrizeID = helper.get<int>();
                m_listActorUnlockPrizeID.Add(nPrizeID);
            }
        }

		public void recvLabelList(IntPtr ptrParam, int nPtrLen)
		{
			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

			cmd_count dataCount = helper.get<cmd_count>();
			if (dataCount.nCount <= 0)
				return;

			for(int i = 0; i < dataCount.nCount; ++i)
			{
				cmd_label_info info = helper.get<cmd_label_info>();

				m_LabelList.Add(info);
			}

		}

        // 玩家游戏状态改变：是否离开
        public void reqUpdateGameStateIsLeave(bool isLeave)
        {
            cmd_entity_gamestate_leave cmd = new cmd_entity_gamestate_leave();
            cmd.isLeave = isLeave ? 1 : 0;

            EntityEventHelper.Instance.SendCommand<cmd_entity_gamestate_leave>(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_GAMESTATE_LEAVE, ref cmd);
        }

        public void updateHeroCard(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nCount = helper.get<int>();
            int dwPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            for (int i = 0; i < nCount; i++)
            {
                cmd_entity_update_card_info node = helper.get<cmd_entity_update_card_info>();
                // Hero更新
                ActorHeroInfo info = null;
                if (actorHero.TryGetValue(dwPDBID, out info))
                {
                    if (info.dicEntityHeroInfo.ContainsKey(node.nCardID))
                    {
                        cmd_entity_hero_info temp = info.dicEntityHeroInfo[node.nCardID];
                        temp.nCardCount = node.nCardCount;
                        info.dicEntityHeroInfo[node.nCardID] = temp;
                    }
                    if (info.dicEntityNoHaveHeroInfo.ContainsKey(node.nCardID))
                    {
                        cmd_entity_nohanve_hero temp = info.dicEntityNoHaveHeroInfo[node.nCardID];
                        temp.nHaveCardCount = node.nCardCount;
                        info.dicEntityNoHaveHeroInfo[node.nCardID] = temp;
                    }
                }
                
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, null);
            checkAllHeroIsHaveCanUpStar();
        }

        public void checkAllHeroIsHaveCanUpStar(int nCurGold = 0)
        {
            if (EntityFactory.MainHeroView == null)
                return;
            int dwPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ActorHeroInfo heroInfo = null;
            if (!actorHero.TryGetValue(dwPDBID, out heroInfo))
                return;

            int nHaveGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
            nHaveGold = nCurGold > 0 ? nCurGold : nHaveGold;

            bool bCanUpstar = false;
            foreach (var item in heroInfo.dicEntityHeroInfo)
            {
                if (item.Value.bIsBuy <= 0)
                    continue;
                SHeroStarSchemeInfo pStarScheme = new SHeroStarSchemeInfo();
                if (!GameLogicAPI.getHeroStarConfig(item.Value.nHeroID, item.Value.byStarLv, ref pStarScheme))
                    continue;
                if (item.Value.byStarLv >= pStarScheme.nLimitLV)
                    continue;
                    
                bool bIsLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(item.Value.nHeroID);
                int nNeedCard = bIsLifeHero && pStarScheme.nCardDiscount > 0 ? pStarScheme.nCardDiscount : pStarScheme.nNeedCount;
                int nNeedGold = bIsLifeHero && pStarScheme.nGoldDiscount > 0 ? pStarScheme.nGoldDiscount : pStarScheme.nExpendGold;
                if (nNeedCard > item.Value.nCardCount || nNeedGold > nHaveGold)
                    continue;

                bCanUpstar = true;
                break;
            }

            int playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_HeroStar);
            if (bCanUpstar && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                // 满足升星条件，显示升星提示
                SideButtonListAddData msg = new SideButtonListAddData();
                msg.nItemID = (int)ESideButtonItemType.ESBT_HeroStar;
                msg.nButtonIconID = config.nIconID;
                msg.strButtonText = config.strName;
                msg.strTooltip = config.strHint;
                msg.callBack = (customData) =>
                {
                    UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_HERO);
                    ButtonType.bIsRequestSelf = true;
                    ButtonType.nRequestPDBID = dwPDBID;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(msg);
            }
            else
            {
                // 不满足升星条件， 隐藏升星提示
                SideButtonListRemoveData msg = new SideButtonListRemoveData();
                msg.nItemID = (int)ESideButtonItemType.ESBT_HeroStar;
                LogicDataCenter.sideButtonDataManager.RemoveItem(msg);
            }
        }

        public Sprite GetIconByPrize(SSchemeActorPrizeConfig prizeConfig, WndID wndID)
        {
            if (prizeConfig == null)
                return null;

            Sprite sprite = null;
            switch (prizeConfig.nPrizeType)
            {
                case (int)EMActorPrizeType.ACTOR_PRIZE_HERO:
                    {
                        int iconIndex;
                        if (wndID == WndID.WND_ID_MAILBOX)
                            iconIndex = 4;
                        else
                            iconIndex = 3;
                        sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, wndID, 1, iconIndex, prizeConfig.nPrizeParam[0]);
                    }
                    break;
                case (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD:
                    {
                        int iconIndex;
                        if (wndID == WndID.WND_ID_MAILBOX)
                            iconIndex = 4;
                        else
                            iconIndex = 7;
                        sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, wndID, 1, iconIndex, prizeConfig.nPrizeParam[0]);
                    }
                    break;
                case (int)EMActorPrizeType.ACTOR_PRIZE_RUNE:
                    {
                        SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[prizeConfig.nPrizeParam[0]] as SSchemeRuneConfig;
                        if (runeConfig != null)
                        {
                            sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, wndID, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                        }
                    }
                    break;
                case (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN:
                    {
                        SkinInfoScheme skinConfig = new SkinInfoScheme();
                        if (GameLogicAPI.GetHeroSkinInfo(prizeConfig.nPrizeParam[1], ref skinConfig))
                        {
                            sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, wndID, skinConfig.nSkinIconID, 3, prizeConfig.nPrizeParam[0]);
                        }
                    }
                    break;
                default:
                    {
                        sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, wndID, prizeConfig.nPrizeType, prizeConfig.nPrizeIcon);
                    }
                    break;
            }

            return sprite;
        }
    }
}

