using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;
using USpeedUI;

namespace ASpeedGame.Data.HeroTalentConfig
{
    // 与HeroTalentConfig.csv表里面的列对应
    public enum HeroTalentConfig : int
    {
        EMT_HEROID,                     // 英雄id
        EMT_TALENTID,                   // 天赋id
        EMT_LEVELPLIMIT,                // 学习等级
        EMT_TALENTICONID,               // 天赋图标
		EMT_RECOMMAND_SHOW,				// 推荐显示
        EMT_ACTIVETYPE,                 // 激活类型
        EMT_ACTIVESTARLEVEL,            // 激活升星等级
        EMT_ONLYACTIVE,                 // 是否互斥:同等级只激活一个
        EMT_SHOWTYPE,                   // 显示类型
        EMT_SLOTID,                     // 影响技能槽位
        EMT_TALENTNAME,                 // 天赋名称
        EMT_TALENTPRIZENAME,            // 天赋奖励名称
        EMT_TALENTACTIVEDESC,           // 天赋描述
        EMT_TALENTDISABLEDESC,          // 天赋未激活描述
        EMT_TALENTDESCINWAR,            // 天赋场内描述
        EMT_TALENTTERMID,               // 天赋前置条件
        EMT_WAR_TYPE,                   // 战场类型ID
    }

    public enum TalentActiveType
    {
        ETAT_DefualtActive = 0,
        ETAT_SystemActive
    };

    /// <summary>
    /// 选人提示类
    /// </summary>
    public class SSchemeHeroTalentConfig
    {
        public int nHeroId;                     // 英雄id
        public int nTalentId;                   //  天赋id
        public int nLevelLimit;                 // 学习等级
        public int nTalentIconId;               // 天赋图标
        public int nActiveType;                 // 激活类型
        public int nActiveStarLv;            	// 激活等级
        public int nOnlyActive;                 // 是否互斥:同等级只激活一个
        public int nShowType;                   // 显示类型
        public int nSlotId;                     // 影响技能槽位
        public string sTalentName;              // 天赋名称
        public string sTalentPrizeName;         // 天赋名称
        public string sTalenActiveDescInStatic; // 天赋描述
        public string sTalenDisableDescInStatic;// 天赋未激活描述id组
        public string sTalenDescInWar;          // 天赋场内描述   
        public int    nTalentTermId;            // 天赋前置条件id  
		public int nRecommandShow;				// 推荐显示
    }

    public class HeroDicTalentConfig
    {
       public Dictionary<int,SSchemeHeroTalentConfig> dicTalentConfig = new  Dictionary<int,SSchemeHeroTalentConfig>();
    }

    class HeroTalentConfigPage
    {
        public Hashtable allHeroTalentConfigPageTable;
        private static HeroTalentConfigPage singleton = new HeroTalentConfigPage();

        // 最大升星等级
        private int m_MaxStarActiveLv = 0;

        public static HeroTalentConfigPage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new HeroTalentConfigPage();
            }
        }

        public void Load(string stPath)
        {
            InitHeroTalentConfigPageCsv(stPath);
        }

        public void UnLoad()
        {
            if (allHeroTalentConfigPageTable != null)
            {
                allHeroTalentConfigPageTable.Clear();
                allHeroTalentConfigPageTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitHeroTalentConfigPageCsv(string stPath)
        {     
            ScpReader reader = new ScpReader(stPath, true, 2);
            allHeroTalentConfigPageTable = new Hashtable();
            int nIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeHeroTalentConfig sConfig = new SSchemeHeroTalentConfig();
                sConfig.nHeroId = reader.GetInt(i, (int)HeroTalentConfig.EMT_HEROID, 0);
                sConfig.nTalentId = reader.GetInt(i, (int)HeroTalentConfig.EMT_TALENTID, 0);
                sConfig.nLevelLimit = reader.GetInt(i, (int)HeroTalentConfig.EMT_LEVELPLIMIT, 0);
                sConfig.nTalentIconId = reader.GetInt(i, (int)HeroTalentConfig.EMT_TALENTICONID, 0);
                sConfig.nRecommandShow = reader.GetInt(i, (int)HeroTalentConfig.EMT_RECOMMAND_SHOW, 0);
                sConfig.nActiveType = reader.GetInt(i, (int)HeroTalentConfig.EMT_ACTIVETYPE, 0);
                sConfig.nActiveStarLv = reader.GetInt(i, (int)HeroTalentConfig.EMT_ACTIVESTARLEVEL, 0);
                sConfig.nOnlyActive = reader.GetInt(i, (int)HeroTalentConfig.EMT_ONLYACTIVE, 0);
                sConfig.nShowType = reader.GetInt(i, (int)HeroTalentConfig.EMT_SHOWTYPE, 0);
                sConfig.nSlotId = reader.GetInt(i, (int)HeroTalentConfig.EMT_SLOTID, 0);
                sConfig.sTalentName = reader.GetString(i, (int)HeroTalentConfig.EMT_TALENTNAME, "");
                sConfig.sTalentPrizeName = reader.GetString(i, (int)HeroTalentConfig.EMT_TALENTPRIZENAME, "");
                sConfig.sTalenActiveDescInStatic = reader.GetString(i, (int)HeroTalentConfig.EMT_TALENTACTIVEDESC, "");
                sConfig.sTalenDisableDescInStatic = reader.GetString(i, (int)HeroTalentConfig.EMT_TALENTDISABLEDESC, "");
                sConfig.sTalenDescInWar = reader.GetString(i, (int)HeroTalentConfig.EMT_TALENTDESCINWAR, "");
                sConfig.nTalentTermId = reader.GetInt(i, (int)HeroTalentConfig.EMT_TALENTTERMID, 0);

                if(sConfig.sTalenDisableDescInStatic == String.Empty)
                {
                    string str = String.Format(ULocalizationService.Instance.Get("UIView", "TalentView", "TalentUnActiveDesc"), sConfig.nActiveStarLv);
                    sConfig.sTalenDisableDescInStatic = sConfig.sTalenActiveDescInStatic + str;
                }

                if (sConfig.nActiveType == 0 && sConfig.nActiveStarLv == 0)
                {
                    string str = ULocalizationService.Instance.Get("UIView", "TalentView", "TalentUnActiveDescEx");
                    sConfig.sTalenDisableDescInStatic = sConfig.sTalenActiveDescInStatic + str;
                }

                if (allHeroTalentConfigPageTable.ContainsKey(sConfig.nHeroId))
                {
                    HeroDicTalentConfig dicConig = allHeroTalentConfigPageTable[sConfig.nHeroId] as HeroDicTalentConfig;
                    dicConig.dicTalentConfig[sConfig.nTalentId] = sConfig;
                }
                else
                {
                    HeroDicTalentConfig dicConig = new HeroDicTalentConfig();
                    dicConig.dicTalentConfig[sConfig.nTalentId] = sConfig;
                    allHeroTalentConfigPageTable[sConfig.nHeroId] = dicConig;
                }

                if (m_MaxStarActiveLv < sConfig.nActiveStarLv)
                    m_MaxStarActiveLv = sConfig.nActiveStarLv;
            }        
        } 
    
        public SSchemeHeroTalentConfig GetHeroTalentConfig(int nHeroId,int nTalentId)
        {
            if(allHeroTalentConfigPageTable == null||!allHeroTalentConfigPageTable.ContainsKey(nHeroId))
            {
                return null;
            }

            HeroDicTalentConfig dicConig = allHeroTalentConfigPageTable[nHeroId] as HeroDicTalentConfig;
            if(!dicConig.dicTalentConfig.ContainsKey(nTalentId))
            {
                return null;
            }

            return dicConig.dicTalentConfig[nTalentId];
        }

        public HeroDicTalentConfig GetHeroTalentDicConfig(int nHeroId)
        {
            if (allHeroTalentConfigPageTable == null || !allHeroTalentConfigPageTable.ContainsKey(nHeroId))
            {
                return null;
            }

            HeroDicTalentConfig dicConig = allHeroTalentConfigPageTable[nHeroId] as HeroDicTalentConfig;
            return dicConig;
        }

        // 返回英雄升星等级奖励的天赋列表
        public List<SSchemeHeroTalentConfig> getHeroStarPrizeTalent(int nHeroID, int nLevel)
        {
            List<SSchemeHeroTalentConfig> result = new List<SSchemeHeroTalentConfig>();

            HeroDicTalentConfig dicConig = allHeroTalentConfigPageTable[nHeroID] as HeroDicTalentConfig;
            foreach(var item in dicConig.dicTalentConfig)
            {
                if(item.Value.nActiveStarLv == nLevel && item.Value.nActiveType == (int)TalentActiveType.ETAT_DefualtActive)
                {
                    result.Add(item.Value);
                }
            }

            return result;
        }

        // 返回下次升星等级奖励的天赋配置列表
        public List<SSchemeHeroTalentConfig> getNextPrizeTalent(int nHeroID, int nStarLv)
        {
            List<SSchemeHeroTalentConfig> result = new List<SSchemeHeroTalentConfig>();

            for(int i = nStarLv+1; i <= m_MaxStarActiveLv; ++i)
            {
                List<SSchemeHeroTalentConfig> talentList = getHeroStarPrizeTalent(nHeroID, i);
                if(talentList.Count > 0)
                {
                    return talentList;
                }
            }

            return result;
        }

        // 返回下次升星等级奖励的天赋ID列表
        public List<int> getNextPrizeTalentID(int nHeroID, int nStarLv)
        {
            List<int> result = new List<int>();

            List<SSchemeHeroTalentConfig> talentList = getHeroStarPrizeTalent(nHeroID, nStarLv);

            return talentList.Select(v => v.nTalentId).ToList();
        }
    }
}
