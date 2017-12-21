/*******************************************************************
** 文件名:	RunePageDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-8-8
** 版  本:	1.0
** 描  述:	皮肤数据管理
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ASpeedGame.Data.RunePage;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI;
using UnityEngine;
using ASpeedGame.Data.HeroXPskillConfig;
namespace DataCenter
{
    public class HeroXPSkillDataManager
    {
        private Dictionary<int, List<cmd_Hero_XP_Skill_Info>> dicXPSkillInfoData = new Dictionary<int, List<cmd_Hero_XP_Skill_Info>>();
        private Dictionary<int, List<cmd_Hero_XP_Fragment_Info>> dicXPFragmentInfoData = new Dictionary<int, List<cmd_Hero_XP_Fragment_Info>>();

		// 更新XP技能信息
        public void UpdateHeroXPSkillInfo(IntPtr ptrParam, int nPtrLen)
        {
            if (dicXPSkillInfoData == null)
            {
                dicXPSkillInfoData = new Dictionary<int, List<cmd_Hero_XP_Skill_Info>>();
            }

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count heroCount = helper.get<cmd_count>();

            if (heroCount.nCount > 1)
                dicXPSkillInfoData.Clear();

            for (int i = 0; i < heroCount.nCount; ++i)
            {
                cmd_Hero_Additional_Info headHead = helper.get<cmd_Hero_Additional_Info>();

                int nHeroID = headHead.nHeroID;
                if (dicXPSkillInfoData.ContainsKey(nHeroID))
                {
                    dicXPSkillInfoData[nHeroID].Clear();
                }
                else
                {
                    dicXPSkillInfoData[nHeroID] = new List<cmd_Hero_XP_Skill_Info>();
                }

                for (int j = 0; j < headHead.nAdditionalCount; ++j)
                {
                    cmd_Hero_XP_Skill_Info cmdInfo = helper.get<cmd_Hero_XP_Skill_Info>();
                    dicXPSkillInfoData[nHeroID].Add(cmdInfo);
                }
            }

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_HERODETAIL_HERODETAILXPUPDATE, null);
        }

		// 更新XP碎片信息
		public void UpdateHeroXPFragmentInfo(IntPtr ptrParam, int nPtrLen)
        {
            if (dicXPFragmentInfoData == null)
            {
				dicXPFragmentInfoData = new Dictionary<int, List<cmd_Hero_XP_Fragment_Info>>();
            }

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count heroCount = helper.get<cmd_count>();

            if (heroCount.nCount > 1)
                dicXPFragmentInfoData.Clear();

            for (int i = 0; i < heroCount.nCount; ++i)
            {
                cmd_Hero_Additional_Info headHead = helper.get<cmd_Hero_Additional_Info>();

                int nHeroID = headHead.nHeroID;
                if (dicXPFragmentInfoData.ContainsKey(nHeroID))
                {
                    dicXPFragmentInfoData[nHeroID].Clear();
                }
                else
                {
                    dicXPFragmentInfoData[nHeroID] = new List<cmd_Hero_XP_Fragment_Info>();
                }

                for (int j = 0; j < headHead.nAdditionalCount; ++j)
                {
                    cmd_Hero_XP_Fragment_Info cmdInfo = helper.get<cmd_Hero_XP_Fragment_Info>();
                    dicXPFragmentInfoData[nHeroID].Add(cmdInfo);
                }
            }
        }

        public Dictionary<int, List<cmd_Hero_XP_Skill_Info>> HeroXPSkillData
        {
            set { dicXPSkillInfoData = value; }
            get { return dicXPSkillInfoData; }
        }

        public bool GetHeroXPSkillInfo(int nHeroID, int nXPSkillID, ref cmd_Hero_XP_Skill_Info  data)
        {
            if (!dicXPSkillInfoData.ContainsKey(nHeroID))
            {
				return false;
            }

            foreach (cmd_Hero_XP_Skill_Info item in dicXPSkillInfoData[nHeroID])
            {
                if (item.nXPSkillID == nXPSkillID)
                {
                    data = item;
                    return true;
                }
            }

            return false;
        }

        public List<cmd_Hero_XP_Skill_Info> GetHeroXPSkillList(int nHeroID)
        {
            if (!dicXPSkillInfoData.ContainsKey(nHeroID))
            {
                return null;
            }

            return dicXPSkillInfoData[nHeroID];
        }

        public int GetHeroXPSkillFragmentCount(int nHeroID, int nXpSkillID)
        {
            if (!dicXPFragmentInfoData.ContainsKey(nHeroID))
            {
                return 0;
            }


            foreach (cmd_Hero_XP_Fragment_Info item in dicXPFragmentInfoData[nHeroID])
            {
                if (item.nXPSkillID == nXpSkillID)
                {

                    return item.nXPFragmentCount;
                }
            }

            return 0;
        }

        public bool CheckIsHaveXPSkill(int nHeroID, int nXPSkillID)
        {
            List<cmd_Hero_XP_Skill_Info> info = null;
            if (!dicXPSkillInfoData.TryGetValue(nHeroID, out info))
            {
                return false;
            }

            if (info == null)
                return false;

            foreach (cmd_Hero_XP_Skill_Info item in info)
            {
                if (item.nXPSkillID == nXPSkillID)
                {
                    return true;
                }
            }

            return false;
        }

        public void Clear()
        {
            dicXPSkillInfoData.Clear();
            dicXPFragmentInfoData.Clear();
        }

        public bool GetHeroXpGuideVisile(int nHeroID = 0, int nXpSkillID = 0)
        {
            if (nHeroID == 0 && nXpSkillID == 0)
            {
                foreach (List<cmd_Hero_XP_Skill_Info> info in dicXPSkillInfoData.Values)
                {
                    foreach (cmd_Hero_XP_Skill_Info item in info)
                    {
                        if (item.nGuide > 0)
                        {
                            SSchemeHeroXPskillConfig config = HeroXPskillConfig.Instance.GetHeroXPskillConfig(item.nHeroID, item.nXPSkillID);
                            if (config != null && config.bActiveGuide)
                                return true;
                        }
                    }
                }

                return false;
            }
            else if (nHeroID != 0 && nXpSkillID == 0)
            {
                List<cmd_Hero_XP_Skill_Info> info = null;
                if (!dicXPSkillInfoData.TryGetValue(nHeroID, out info))
                {
                    return false;
                }

                if (info == null)
                    return false;

                foreach (cmd_Hero_XP_Skill_Info item in info)
                {
                    if (item.nGuide > 0)
                    {
                        SSchemeHeroXPskillConfig config = HeroXPskillConfig.Instance.GetHeroXPskillConfig(item.nHeroID, item.nXPSkillID);
                        if (config != null && config.bActiveGuide)
                            return true;
                    }
                }

                return false;
            }
            else if (nHeroID != 0 && nXpSkillID != 0)
            {
                List<cmd_Hero_XP_Skill_Info> info = null;
                if (!dicXPSkillInfoData.TryGetValue(nHeroID, out info))
                {
                    return false;
                }

                if (info == null)
                    return false;

                foreach (cmd_Hero_XP_Skill_Info item in info)
                {
                    if (item.nXPSkillID == nXpSkillID && item.nGuide > 0)
                    {
                        SSchemeHeroXPskillConfig config = HeroXPskillConfig.Instance.GetHeroXPskillConfig(item.nHeroID, item.nXPSkillID);
                        if (config != null && config.bActiveGuide)
                            return true;
                    }
                }

                return false;
            }

            return false;
        }
    }
}
