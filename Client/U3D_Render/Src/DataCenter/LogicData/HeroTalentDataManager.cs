using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;

namespace DataCenter
{
    public class HeroTalentDataManager 
    {
        public class HeroTalentInfo
        {
            public int nHeroId;
            public int nTalentId;
            public bool bActive;
        }

        // 本主角的所有英雄天赋列表<英雄ID,天赋列表>
        public Dictionary<int, List<HeroTalentInfo>> dicTalentInfoData = new Dictionary<int, List<HeroTalentInfo>>();

        // 当前英雄可激活天赋的等级列表
        public int[] LevelArray = new int[0];
        // 当前英雄ID
        public int nCurHeroId = 0;

		// 所有玩家已激活的天赋列表<pdbid,已激活天赋列表>
		private Dictionary<ulong, cmd_war_active_talent_list> activeTalentInfo = new Dictionary<ulong, cmd_war_active_talent_list>();

        public void Init()
        {
            
        }

        // 初始化本主角玩家的所有英雄天赋数据
        public void InitHeroTalentInfo(IntPtr ptrParam, int nPtrLen)
        {
            if (dicTalentInfoData == null)
            {
                return;
            }

            dicTalentInfoData.Clear();

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count heroCount = helper.get<cmd_count>();
            for (int i = 0; i < heroCount.nCount; ++i)
            {
                cmd_count talentCount = helper.get<cmd_count>();
                for (int j = 0; j < talentCount.nCount; ++j)
                {
                    cmd_Hero_Talent_Info cmdInfo = helper.get<cmd_Hero_Talent_Info>();
                    int nHeroId = cmdInfo.nHeroId;
                    bool bActive = cmdInfo.nActive > 0 ? true : false;

                    HeroTalentInfo info = new HeroTalentInfo();
                    info.nHeroId = nHeroId;
                    info.nTalentId = cmdInfo.nTalentId;
                    info.bActive = bActive;

                    if (dicTalentInfoData.ContainsKey(cmdInfo.nHeroId))
                    {
                        dicTalentInfoData[nHeroId].Add(info);
                    }
                    else
                    {
                        List<HeroTalentInfo> listInfo = new List<HeroTalentInfo>();
                        listInfo.Add(info);
                        dicTalentInfoData[nHeroId] = listInfo;
                    }
                }
            }

            // 初始化战场天赋显示
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_ID_HEROTALENTVIEW_INITWARTALENT, null);
        }

        public void UpdateHeroTalentData(IntPtr ptrParam,int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nLen);
            cmd_Hero_Talent_Info data = helper.get<cmd_Hero_Talent_Info>();
            if (!dicTalentInfoData.ContainsKey(data.nHeroId))
            {
                return;
            }

            foreach (HeroTalentInfo info in dicTalentInfoData[data.nHeroId])
            {
                if (info.nTalentId == data.nTalentId)
                {
                    bool bActive = data.nActive > 0 ? true : false;
                    info.bActive = bActive;
                    break;
                }
            }

            // 更新主城天赋显示
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_ID_HEROTALENTVIEW_UPDATESTATICTALENT, null);
        }

        public void Clear()
        {
			activeTalentInfo.Clear();
            dicTalentInfoData.Clear();
        }

        // 进入LoginMoba（退出房间后，显示战场Loading界面前）
        public void onEnterLoginMobaState()
        {
            Clear();
        }

        // 进入战场
        public void onEnterBattleState()
        {
        }

        // 离开战场
        public void onLeaveBattleState()
        {
            Clear();
        }

        // 清空自己已激活天赋
        public void clearSelfActivedTalentList()
        {
            ulong pdbid = (ulong)EntityUtil.getMainHeroPdbid();
            cmd_war_active_talent_list talentList = new cmd_war_active_talent_list();
            if (activeTalentInfo.TryGetValue(pdbid, out talentList))
            {
                talentList.nCurTalent = 0;
                for(int i = 0; i < talentList.nTalentList.Length; ++i)
                {
                    talentList.nTalentList[i] = 0;
                }
            }
        }

        // 玩家天赋激活
        public void recvActiveTalentList(IntPtr ptr, int nLen)
		{
			if (ptr == null || nLen == 0)
				return;

			IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
			cmd_war_active_talent_list cmd = helper.get<cmd_war_active_talent_list>();

			if (!activeTalentInfo.ContainsKey((ulong)cmd.pdbid))
				activeTalentInfo.Add((ulong)cmd.pdbid, cmd);
			else
				activeTalentInfo[(ulong)cmd.pdbid] = cmd;

            // 判断是否当前主角
            int pdbid = EntityUtil.getMainHeroPdbid();
            if (cmd.pdbid == pdbid)
            {
                // 通知界面已激活天赋列表更新
                UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE, cmd.nCurTalent, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE, uiResult);
            }
        }

		// 获取某个玩家的已激活天赋列表
		public bool getActiveTalentList(ulong pdbid, out cmd_war_active_talent_list talentList)
		{
			if (activeTalentInfo.TryGetValue(pdbid, out talentList)) 
			{
				return true;
			}

			return false;
		}

        // 该等级的天赋已经激活了
        public bool isTalentActivedThisLv(int nLv)
        {
            ulong pdbid = (ulong)EntityUtil.getMainHeroPdbid();
            if (pdbid == 0)
            {
                return false;
            }

            cmd_war_active_talent_list talentList;
            if (!DataCenter.LogicDataCenter.heroTalentDataManager.getActiveTalentList(pdbid, out talentList))
            {
                return false;
            }

            for(int i = 0; i < LevelArray.Length; ++i)
            {
                if (LevelArray[i] == nLv)
                {
                    return talentList.nTalentList[i] > 0;
                }
            }

            return false;
        }

        // 该天赋是否激活过了
        public bool isActivedThisTalent(int nTalentID)
        {
            if (nTalentID == 0)
            {
                return false;
            }

            ulong pdbid = (ulong)EntityUtil.getMainHeroPdbid();
            if (pdbid == 0)
            {
                return false;
            }

            cmd_war_active_talent_list talentList;
            if (!DataCenter.LogicDataCenter.heroTalentDataManager.getActiveTalentList(pdbid, out talentList))
            {
                return false;
            }

            foreach (var item in talentList.nTalentList)
            {
                if (item == nTalentID)
                {
                    return true;
                }
            }

            return false;
        }

        // 是否可以激活
        public bool isCanActiveTalent(int nTalentID, int nLimitLv)
        {
            int nCurHeroLv = EntityUtil.getMainHeroLv();
            if (nCurHeroLv == 0)
            {
                Debug.LogError("isTalentCanActive,EntityUtil.getMainHeroLv() == 0");
                return false;
            }

            if (nCurHeroLv < nLimitLv || isActivedThisTalent(nTalentID))
            {
                return false;
            }

            return true;
        }

        // 获取下一次激活天赋的等级
        public int getNextCanActiveLv(int nCurLv)
		{
			foreach(int lv in LevelArray)
			{
				if (nCurLv < lv)
					return lv;
			}

			return 0;
		}
	}
}
