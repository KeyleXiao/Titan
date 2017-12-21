using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using GameLogic;
using USpeedUI;
using Match_ManagerDef;

namespace DataCenter
{
    //排行榜榜单类型
    public enum EMChartType
    {
        EMCHART_ROLERANK,            //角色段位榜(个人排名)
        EMCHART_KILLRANK,           //多杀榜
        EMCHART_HERODEVELOP,           //英雄养成榜
        EMCHART_CLAN,           //联盟榜
        EMCHART_KIN,            //战队榜
        EMCHART_CURRENTWEEKMASTER,
        EMCHART_CURRENTWEEKPRENTICE,
        EMCHART_LASTWEEKMASTER,
        EMCHART_LASTWEEKPRENTICE,
        EMCHART_DONATECHART,         //捐卡榜
        EMCHART_HEROCOUNT,
        EMCHART_SKIN,
        EMCHART_GEM,
    }

    //英雄类型榜分类
    public enum StaMatchType : int
    {
        EMT_TOGETHER = EMMatchType.MatchType_None,            //综合排名 EMMatchType
        EMT_RANK     = EMMatchType.MatchType_Rank,       //排位排名
    }

    public class ChartDataList<T>
    {
        //临时表，查询搜索用
        public List<T> tmpDataList = new List<T>();
        //所有数据表
        public List<T> AllDataList = new List<T>();

        public List<T> ClanMemberDataList = new List<T>();

        public List<T> FriendDataList = new List<T>();

        //个人排名
        public int PersonalRank { get { return m_PersonalRank; } set { m_PersonalRank = value; } }
        private int m_PersonalRank = -1;
        //最后一名战斗力
        public int LastScore { get { return m_LastScore; } set { m_LastScore = value; } }
        private int m_LastScore;

        public void Clear()
        {
            tmpDataList.Clear();
            tmpDataList = null;
            AllDataList.Clear();
            AllDataList = null;
            ClanMemberDataList.Clear();
            ClanMemberDataList = null;
            FriendDataList.Clear();
            FriendDataList = null;
        }
    }

    public class ChartDataManager
    {
        //Role
        public ChartDataList<cmd_Entity_HighLadder_Rank_Info> RoleRankList;
        public ChartDataList<cmd_entity_kill_order_info> KillRankList;
        //Hero
        public ChartDataList<cmd_entity_king_of_hero_order_info> KingOfHeroChartList;
        //public Dictionary<int, ChartDataList<cmd_entity_king_of_hero_order_info>> KingOfHeroAllChartData;
        public ChartDataList<cmd_entity_hero_order_info> HeroRankList;
        public List<cmd_entity_herotype_order_info> HeroTypeChartList;
        //Clan
        public ChartDataList<cmd_entity_clan_order_info> ClanChartList;
        //Kin
        public ChartDataList<cmd_entity_kin_order_info> KinChartList;
        //MentorShip
        public ChartDataList<cmd_entity_mentorship_order_info> CurWeekMasterChartList;
        public ChartDataList<cmd_entity_mentorship_order_info> CurWeekPrenticeChartList;
        public ChartDataList<cmd_entity_mentorship_order_info> LastWeekMasterChartList;
        public ChartDataList<cmd_entity_mentorship_order_info> LastWeekPrenticeChartList;
        //Donation
        public ChartDataList<cmd_entity_donate_order_info> DonateChartList;
        //Collection
        public ChartDataList<cmd_entity_herocount_order_info> HeroCountChartList;
        public ChartDataList<cmd_entity_skin_order_info> SkinChartList;
        public ChartDataList<cmd_entity_gemstone_order_info> GemStoneChartList;

        public cmd_entity_set_orderinfo PersonOrderSet { get { return m_PersonOrderSet; } }
        private cmd_entity_set_orderinfo m_PersonOrderSet;

        public void Init()
        {
            RoleRankList = new ChartDataList<cmd_Entity_HighLadder_Rank_Info>();
            KillRankList = new ChartDataList<cmd_entity_kill_order_info>();

            //KingOfHeroAllChartData = new Dictionary<int, ChartDataList<cmd_entity_king_of_hero_order_info>>();
            KingOfHeroChartList = new ChartDataList<cmd_entity_king_of_hero_order_info>();
            HeroRankList = new ChartDataList<cmd_entity_hero_order_info>();
            HeroTypeChartList = new List<cmd_entity_herotype_order_info>();

            ClanChartList = new ChartDataList<cmd_entity_clan_order_info>();

            KinChartList = new ChartDataList<cmd_entity_kin_order_info>();

            CurWeekMasterChartList = new ChartDataList<cmd_entity_mentorship_order_info>();
            CurWeekPrenticeChartList = new ChartDataList<cmd_entity_mentorship_order_info>();
            LastWeekMasterChartList = new ChartDataList<cmd_entity_mentorship_order_info>();
            LastWeekPrenticeChartList = new ChartDataList<cmd_entity_mentorship_order_info>();

            DonateChartList = new ChartDataList<cmd_entity_donate_order_info>();

            HeroCountChartList = new ChartDataList<cmd_entity_herocount_order_info>();
            SkinChartList = new ChartDataList<cmd_entity_skin_order_info>();
            GemStoneChartList = new ChartDataList<cmd_entity_gemstone_order_info>();
        }

        public void Clear()
        {
            RoleRankList.Clear();
            RoleRankList = null;
            KillRankList.Clear();
            KillRankList = null;

            KingOfHeroChartList.Clear();
            KingOfHeroChartList = null;
            HeroRankList.Clear();
            HeroRankList = null;
            HeroTypeChartList.Clear();
            HeroTypeChartList = null;

            ClanChartList.Clear();
            ClanChartList = null;

            KinChartList.Clear();
            KinChartList = null;

            CurWeekMasterChartList.Clear();
            CurWeekMasterChartList = null;
            CurWeekPrenticeChartList.Clear();
            CurWeekPrenticeChartList = null;
            LastWeekMasterChartList.Clear();
            LastWeekMasterChartList = null;
            LastWeekPrenticeChartList.Clear();
            LastWeekPrenticeChartList = null;

            DonateChartList.Clear();
            DonateChartList = null;

            HeroCountChartList.Clear();
            HeroCountChartList = null;
            SkinChartList.Clear();
            SkinChartList = null;
            GemStoneChartList.Clear();
            GemStoneChartList = null;
        }


        //接收角色段位榜(个人榜单)数据
        public void OnRecvRequestRoleRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            RoleRankList.AllDataList.Clear();
            RoleRankList.FriendDataList.Clear();
            RoleRankList.ClanMemberDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_Entity_HighLadder_Rank_Info data = helper.get<cmd_Entity_HighLadder_Rank_Info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.idActor == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        RoleRankList.PersonalRank = data.nOrderID;

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)data.dwClanID)
                        RoleRankList.ClanMemberDataList.Add(data);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)data.dwUserID))
                    RoleRankList.FriendDataList.Add(data);

                RoleRankList.AllDataList.Add(data);
            }

            if(RoleRankList.AllDataList.Count > 0)
                RoleRankList.LastScore = RoleRankList.AllDataList[head.nCount - 1].nRankScore;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTROLERANKDATA, null);
        }

        //接收多杀榜数据
        public void OnRecvRequestKillRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            KillRankList.AllDataList.Clear();
            KillRankList.FriendDataList.Clear();
            KillRankList.ClanMemberDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_kill_order_info data = helper.get<cmd_entity_kill_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.nPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        KillRankList.PersonalRank = data.nOrderID;

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)data.dwClanID)
                        KillRankList.ClanMemberDataList.Add(data);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)data.dwUserID))
                    KillRankList.FriendDataList.Add(data);

                KillRankList.AllDataList.Add(data);
            }

            if (KillRankList.AllDataList.Count > 0)
                KillRankList.LastScore = KillRankList.AllDataList[head.nCount - 1].n5KillNum;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTKILLRANKDATA, null);
        }





        //接收英雄王者榜数据
        public void OnRecvRequestKingOfHeroRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head cmdHead = helper.get<cmd_Entity_Rank_Info_head>();

            KingOfHeroChartList.AllDataList.Clear();
            for (int i = 0; i < cmdHead.nCount; ++i)
            {
                cmd_entity_king_of_hero_order_info cmdNode = helper.get<cmd_entity_king_of_hero_order_info>();
                KingOfHeroChartList.AllDataList.Add(cmdNode);
            }

            UIMsgCmdData uiMsgData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA, cmdHead.nOtherCondition, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUEST_KING_OF_HERO_CHARTDATA, uiMsgData);
        }

        //接收英雄养成榜数据
        public void OnRecvRequestHeroRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            HeroRankList.AllDataList.Clear();
            HeroRankList.FriendDataList.Clear();
            HeroRankList.ClanMemberDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_hero_order_info data = helper.get<cmd_entity_hero_order_info>();
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if ((int)data.dwPlayerID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        HeroRankList.PersonalRank = (data.nOrder);

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)data.dwClanID)
                        HeroRankList.ClanMemberDataList.Add(data);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)data.dwUserID))
                    HeroRankList.FriendDataList.Add(data);

                HeroRankList.AllDataList.Add(data);
            }
            if (HeroRankList.AllDataList.Count > 0)
                HeroRankList.LastScore = (int)HeroRankList.AllDataList[head.nCount - 1].nHeroScore;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTHERORANKDATA, null);
        }

        //接收英雄类型榜数据
        public void OnRecvRequestHeroTypeRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            HeroTypeChartList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_herotype_order_info data = helper.get<cmd_entity_herotype_order_info>();
                HeroTypeChartList.Add(data);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROTYPECHARTDATA, null);
        }





        //接收联盟榜数据
        public void OnRecvRequestClanRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            ClanChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_clan_order_info data = helper.get<cmd_entity_clan_order_info>();
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if ((int)data.dwClanID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN))
                        ClanChartList.PersonalRank = (int)data.nOrderID; 
                }
                ClanChartList.AllDataList.Add(data);
            }
            if (ClanChartList.AllDataList.Count > 0)
                ClanChartList.LastScore = (int)ClanChartList.AllDataList[head.nCount - 1].dwTotalFightCapacity;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTCLANCHARTDATA, null);
        }





        //接收战队榜数据
        public void OnRecvRequestKinRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            KinChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_kin_order_info data = helper.get<cmd_entity_kin_order_info>();
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if ((int)data.dwKinID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN))
                        KinChartList.PersonalRank = (int)data.nOrderID; 
                }
                KinChartList.AllDataList.Add(data);
            }
            if (KinChartList.AllDataList.Count > 0)
                KinChartList.LastScore = (int)KinChartList.AllDataList[head.nCount - 1].dwTotalFightCapacity;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTKINCHARTDATA, null);
        }





        //接收本周导师榜数据
        public void OnRecvRequestCurWeekMasterRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            CurWeekMasterChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_mentorship_order_info data = helper.get<cmd_entity_mentorship_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        CurWeekMasterChartList.PersonalRank = data.nOrder;
                }
                CurWeekMasterChartList.AllDataList.Add(data);
            }
            if (CurWeekMasterChartList.AllDataList.Count > 0)
                CurWeekMasterChartList.LastScore = (int)CurWeekMasterChartList.AllDataList[head.nCount - 1].dwShipVal;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKMASTERCHARTDATA, null);
        }

        //接收本周学徒榜数据
        public void OnRecvRequestCurWeekPrenticeRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            CurWeekPrenticeChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_mentorship_order_info data = helper.get<cmd_entity_mentorship_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        CurWeekPrenticeChartList.PersonalRank = data.nOrder;
                }
                CurWeekPrenticeChartList.AllDataList.Add(data);
            }
            if (CurWeekPrenticeChartList.AllDataList.Count > 0)
                CurWeekPrenticeChartList.LastScore = (int)CurWeekPrenticeChartList.AllDataList[head.nCount - 1].dwShipVal;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTCURWEEKPRENTICECHARTDATA, null);
        }

        //接收上周导师榜数据
        public void OnRecvRequestLastWeekMasterRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            LastWeekMasterChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_mentorship_order_info data = helper.get<cmd_entity_mentorship_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        LastWeekMasterChartList.PersonalRank = data.nOrder;
                }
                LastWeekMasterChartList.AllDataList.Add(data);
            }
            if (LastWeekMasterChartList.AllDataList.Count > 0)
                LastWeekMasterChartList.LastScore = (int)LastWeekMasterChartList.AllDataList[head.nCount - 1].dwShipVal;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKMASTERCHARTDATA, null);
        }

        //接收上周学徒榜数据
        public void OnRecvRequestLastWeekPrenticeRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            LastWeekPrenticeChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_mentorship_order_info data = helper.get<cmd_entity_mentorship_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        LastWeekPrenticeChartList.PersonalRank = data.nOrder;
                }
                LastWeekPrenticeChartList.AllDataList.Add(data);
            }
            if (LastWeekPrenticeChartList.AllDataList.Count > 0)
                LastWeekPrenticeChartList.LastScore = (int)LastWeekPrenticeChartList.AllDataList[head.nCount - 1].dwShipVal;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTLASTWEEKPRENTICECHARTDATA, null);
        }






        //接收捐卡榜数据
        public void OnRecvRequestDonateRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            DonateChartList.AllDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_donate_order_info data = helper.get<cmd_entity_donate_order_info>();
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if ((int)data.nPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        DonateChartList.PersonalRank = (int)data.nOrder; ;
                }
                DonateChartList.AllDataList.Add(data);
            }
            if (DonateChartList.AllDataList.Count > 0)
                DonateChartList.LastScore = DonateChartList.AllDataList[head.nCount - 1].nTotalClanCtrb;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTDONATECHARTDATA, null);
        }





        // 接收英雄数量榜数据
        public void OnRecvRequestHeroCountRankData(IntPtr ptr, int nLen)

        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head cmdHead = helper.get<cmd_Entity_Rank_Info_head>();

            HeroCountChartList.AllDataList.Clear();
            HeroCountChartList.FriendDataList.Clear();
            HeroCountChartList.ClanMemberDataList.Clear();
            for (int i = 0; i < cmdHead.nCount; ++i)
            {
                cmd_entity_herocount_order_info cmdNode = helper.get<cmd_entity_herocount_order_info>();
                // 如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (cmdNode.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                    {
                        HeroCountChartList.PersonalRank = cmdNode.nOrder;
                    }

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)cmdNode.dwClanID)
                        HeroCountChartList.ClanMemberDataList.Add(cmdNode);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)cmdNode.dwUserID))
                    HeroCountChartList.FriendDataList.Add(cmdNode);

                HeroCountChartList.AllDataList.Add(cmdNode);
            }

            if (HeroCountChartList.AllDataList.Count > 0)
                HeroCountChartList.LastScore = HeroCountChartList.AllDataList[cmdHead.nCount - 1].dwHeroCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTHEROCOUNTCHARTDATA, null);
        }

        // 接收皮肤榜数据
        public void OnRecvRequestSkinRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head cmdHead = helper.get<cmd_Entity_Rank_Info_head>();

            SkinChartList.AllDataList.Clear();
            SkinChartList.FriendDataList.Clear();
            SkinChartList.ClanMemberDataList.Clear();
            for (int i = 0; i < cmdHead.nCount; ++i)
            {
                cmd_entity_skin_order_info cmdNode = helper.get<cmd_entity_skin_order_info>();
                // 如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (cmdNode.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                    {
                        SkinChartList.PersonalRank = cmdNode.nOrder;
                    }

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)cmdNode.dwClanID)
                        SkinChartList.ClanMemberDataList.Add(cmdNode);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)cmdNode.dwUserID))
                    SkinChartList.FriendDataList.Add(cmdNode);

                SkinChartList.AllDataList.Add(cmdNode);
            }

            if (SkinChartList.AllDataList.Count > 0)
                SkinChartList.LastScore = SkinChartList.AllDataList[cmdHead.nCount - 1].dwSkinCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTSKINCHARTDATA, null);
        }

        //接收宝石榜数据
        public void OnRecvRequestGemStoneRankData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head head = helper.get<cmd_Entity_Rank_Info_head>();
            GemStoneChartList.AllDataList.Clear();
            GemStoneChartList.FriendDataList.Clear();
            GemStoneChartList.ClanMemberDataList.Clear();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_entity_gemstone_order_info data = helper.get<cmd_entity_gemstone_order_info>();
                //如果有自己数据，存一下排名
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    if (data.dwPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                        GemStoneChartList.PersonalRank = data.nOrder;

                    int m_clanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                    if (m_clanID > 0 && m_clanID == (int)data.dwClanID)
                        GemStoneChartList.ClanMemberDataList.Add(data);
                }

                if (LogicDataCenter.snsDataManager.isBuddyRelation((int)data.dwUserID))
                    GemStoneChartList.FriendDataList.Add(data);

                GemStoneChartList.AllDataList.Add(data);
            }
            if (GemStoneChartList.AllDataList.Count > 0)
                GemStoneChartList.LastScore = (int)GemStoneChartList.AllDataList[head.nCount - 1].dwFaceScore;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUESTGEMSTONECHARTDATA, null);
        }



		
		public void OnRecvPersonOrderSetInfo(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_PersonOrderSet = helper.get<cmd_entity_set_orderinfo>();
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_UPDATE_PERSON_ORDER_SETTING, chartSetting);
        }

        public void requestModifyPersonOrderSet(cmd_entity_set_orderinfo data)
        {
            cmd_entity_set_orderinfo sendData = new cmd_entity_set_orderinfo();
            sendData = data;

            ViewEventHelper.Instance.SendCommand<cmd_entity_set_orderinfo>(GameLogicDef.GVIEWCMD_SET_ORDER_PERSONINFO, ref sendData);
        }

        //搜索指定信息
        public void SearchChartInfo(EMChartType chartType, string content, USpeedUI.Chart.EMChartSortType SortType = USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
        {
            switch (chartType)
            {
                case EMChartType.EMCHART_ROLERANK:
                    {
                        IEnumerable<cmd_Entity_HighLadder_Rank_Info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in RoleRankList.AllDataList
                                        where item.szPlayerName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in RoleRankList.ClanMemberDataList
                                        where item.szPlayerName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in RoleRankList.FriendDataList
                                        where item.szPlayerName.Contains(content)
                                        select item;
                        }
                        RoleRankList.tmpDataList.Clear();
                        foreach (cmd_Entity_HighLadder_Rank_Info item in ListQuery)
                        {
                            RoleRankList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_KILLRANK:
                    {
                        IEnumerable<cmd_entity_kill_order_info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in KillRankList.AllDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in KillRankList.ClanMemberDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in KillRankList.FriendDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        KillRankList.tmpDataList.Clear();
                        foreach (cmd_entity_kill_order_info item in ListQuery)
                        {
                            KillRankList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_HERODEVELOP:
                    {
                        IEnumerable<cmd_entity_hero_order_info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in HeroRankList.AllDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in HeroRankList.ClanMemberDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in HeroRankList.FriendDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        HeroRankList.tmpDataList.Clear();
                        foreach (cmd_entity_hero_order_info item in ListQuery)
                        {
                            HeroRankList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_CLAN:
                    {
                        IEnumerable<cmd_entity_clan_order_info> ListQuery = from item in ClanChartList.AllDataList
                                                                                 where item.szKinName.Contains(content)
                                                                                 select item;
                        ClanChartList.tmpDataList.Clear();
                        foreach (cmd_entity_clan_order_info item in ListQuery)
                        {
                            ClanChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_KIN:
                    {
                        IEnumerable<cmd_entity_kin_order_info> ListQuery = from item in KinChartList.AllDataList
                                                                                 where item.szKinName.Contains(content)
                                                                                 select item;
                        KinChartList.tmpDataList.Clear();
                        foreach (cmd_entity_kin_order_info item in ListQuery)
                        {
                            KinChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_CURRENTWEEKMASTER:
                    {
                        IEnumerable<cmd_entity_mentorship_order_info> ListQuery = from item in CurWeekMasterChartList.AllDataList
                                                                                 where item.szName.Contains(content)
                                                                                 select item;
                        CurWeekMasterChartList.tmpDataList.Clear();
                        foreach (cmd_entity_mentorship_order_info item in ListQuery)
                        {
                            CurWeekMasterChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_CURRENTWEEKPRENTICE:
                    {
                        IEnumerable<cmd_entity_mentorship_order_info> ListQuery = from item in CurWeekPrenticeChartList.AllDataList
                                                                                  where item.szName.Contains(content)
                                                                                  select item;
                        CurWeekPrenticeChartList.tmpDataList.Clear();
                        foreach (cmd_entity_mentorship_order_info item in ListQuery)
                        {
                            CurWeekPrenticeChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_LASTWEEKMASTER:
                    {
                        IEnumerable<cmd_entity_mentorship_order_info> ListQuery = from item in LastWeekMasterChartList.AllDataList
                                                                                  where item.szName.Contains(content)
                                                                                  select item;
                        LastWeekMasterChartList.tmpDataList.Clear();
                        foreach (cmd_entity_mentorship_order_info item in ListQuery)
                        {
                            LastWeekMasterChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_LASTWEEKPRENTICE:
                    {
                        IEnumerable<cmd_entity_mentorship_order_info> ListQuery = from item in LastWeekPrenticeChartList.AllDataList
                                                                                  where item.szName.Contains(content)
                                                                                  select item;
                        LastWeekPrenticeChartList.tmpDataList.Clear();
                        foreach (cmd_entity_mentorship_order_info item in ListQuery)
                        {
                            LastWeekPrenticeChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_DONATECHART:
                    {
                        IEnumerable<cmd_entity_donate_order_info> ListQuery = from item in DonateChartList.AllDataList
                                                                                  where item.szName.Contains(content)
                                                                                  select item;
                        DonateChartList.tmpDataList.Clear();
                        foreach (cmd_entity_donate_order_info item in ListQuery)
                        {
                            DonateChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_HEROCOUNT:
                    {
                        IEnumerable<cmd_entity_herocount_order_info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in HeroCountChartList.AllDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in HeroCountChartList.ClanMemberDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in HeroCountChartList.FriendDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        HeroCountChartList.tmpDataList.Clear();
                        foreach (cmd_entity_herocount_order_info item in ListQuery)
                        {
                            HeroCountChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_SKIN:
                    {
                        IEnumerable<cmd_entity_skin_order_info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in SkinChartList.AllDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in SkinChartList.ClanMemberDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in SkinChartList.FriendDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        SkinChartList.tmpDataList.Clear();
                        foreach (cmd_entity_skin_order_info item in ListQuery)
                        {
                            SkinChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                case EMChartType.EMCHART_GEM:
                    {
                        IEnumerable<cmd_entity_gemstone_order_info> ListQuery;
                        if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_DISTRICT)
                        {
                            ListQuery = from item in GemStoneChartList.AllDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else if (SortType == USpeedUI.Chart.EMChartSortType.EMCHARTSORT_CLAN)
                        {
                            ListQuery = from item in GemStoneChartList.ClanMemberDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        else
                        {
                            ListQuery = from item in GemStoneChartList.FriendDataList
                                        where item.szName.Contains(content)
                                        select item;
                        }
                        GemStoneChartList.tmpDataList.Clear();
                        foreach (cmd_entity_gemstone_order_info item in ListQuery)
                        {
                            GemStoneChartList.tmpDataList.Add(item);
                        }
                    }
                    break;
                default:
                    break;
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECVREQUSETSEARCHDATA, null);
        }
    }
}
