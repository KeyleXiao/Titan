/*******************************************************************
** 文件名:	PlayerCrownWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-12-12
** 版  本:	1.0
** 描  述:	点赞配置
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.HeroXPskillConfig
{
    public enum XP_COL
    {
        HERO_ID,					// 英雄id
        SPELL_ID,                   // 必杀技技能id
        SPELL_TYPE,  				// 技能类型(通用0,专属1)
        AUTO_UNLOCK,				// 是否自动解锁(否0,是1)
        CHIP_NUM,                   // 必杀技解锁所需碎片数
        ICON,						// 必杀技图标
        NAME,                       // 必杀技名字
        LOCK_DESC,                  // 必杀技未解锁时描述
        UNLOCK_DESC,                // 必杀技解锁时描述
        GUIDE,                      // 解锁时是否引导(否0,是1)
        DRESS,                      // 解锁时是否默认装备(否0,是1)
        EFFECTS,                    // 对应光效列表可以有多个，用分号隔开
    };

    public class SSchemeHeroXPskillConfig
    {
        public int nHeroID;											// 英雄ID
        public int nXPSkillID;										// XPID
        public int nXPType;											// XP类型(0通用 1 专属)
        public bool bAutoActive;									// XP自动激活
        public int nLockXPFragmentNum;								// 解锁xp所需碎片数      
        public int nIconID;
        public string szXpName;
        public string szNoLockDesc;
        public string szLockDesc;
        public bool bActiveGuide;									// XP激活是否引导
        public List<string> effects;                                // 对应光效列表
    }

    class HeroXPskillConfig
    {
        public Hashtable allHeroXPskillConfigTable;

        private static HeroXPskillConfig singleton = new HeroXPskillConfig();
        public static HeroXPskillConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new HeroXPskillConfig();
            }
        }

        public void Load(string stPath1)
        {
            InitHeroXPskillConfigCsv(stPath1);
        }

        public void UnLoad()
        {
            if (allHeroXPskillConfigTable != null)
            {
                allHeroXPskillConfigTable.Clear();
                allHeroXPskillConfigTable = null;
            }

            singleton = null;
        }

        // 初始化csv表
        protected void InitHeroXPskillConfigCsv(string stPath)
        {     
            ScpReader reader = new ScpReader(stPath, true, 2);
            allHeroXPskillConfigTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeHeroXPskillConfig sConfig = new SSchemeHeroXPskillConfig();
                sConfig.nHeroID             = reader.GetInt(i, (int)XP_COL.HERO_ID, 0);     // 英雄id
                sConfig.nXPSkillID          = reader.GetInt(i, (int)XP_COL.SPELL_ID, 0);    // 必杀技技能id
                sConfig.nXPType             = reader.GetInt(i, (int)XP_COL.SPELL_TYPE, 0);  // 技能类型(通用0,专属1)
                sConfig.bAutoActive         = reader.GetInt(i, (int)XP_COL.SPELL_TYPE, 0)>0;// 是否自动解锁(否0,是1)
                sConfig.nLockXPFragmentNum  = reader.GetInt(i, (int)XP_COL.CHIP_NUM, 0);    // 必杀技解锁所需碎片数
                sConfig.nIconID             = reader.GetInt(i, (int)XP_COL.ICON, 0);        // 必杀技图标
                sConfig.szXpName            = reader.GetString(i, (int)XP_COL.NAME, "");    // 必杀技名字
                sConfig.szLockDesc          = reader.GetString(i, (int)XP_COL.LOCK_DESC, "");   // 必杀技未解锁时描述
                sConfig.szNoLockDesc        = reader.GetString(i, (int)XP_COL.UNLOCK_DESC, ""); // 必杀技解锁时描述
                sConfig.bActiveGuide        = reader.GetInt(i, (int)XP_COL.GUIDE, 0)>0;     // 解锁时是否引导(否0,是1)

                string strEffect = reader.GetString(i, (int)XP_COL.EFFECTS, "");
                if(strEffect.Equals(""))
                    sConfig.effects = new List<System.String>();
                else
                    sConfig.effects = new List<System.String>(strEffect.Split(new Char[] { ';' }));   // 对应光效列表可以有多个，用分号隔开

                if (sConfig.nHeroID == 0)
                {
                    for (int nHeroID = 1; nHeroID < (int)VOCATION.VOCATION_MAX; ++nHeroID)
                    {
                        sConfig.nHeroID = nHeroID;
                        updateXpConfigToCache(sConfig);
                    }
                }
                else
                {
                    updateXpConfigToCache(sConfig);
                }	
            }        
        }

        public void updateXpConfigToCache(SSchemeHeroXPskillConfig sConfig)
        {
            if (allHeroXPskillConfigTable.ContainsKey(sConfig.nHeroID))
            {
                (allHeroXPskillConfigTable[sConfig.nHeroID] as List<SSchemeHeroXPskillConfig>).Add(sConfig);

            }
            else
            {
                List<SSchemeHeroXPskillConfig> heroXPskillList = new List<SSchemeHeroXPskillConfig>();
                heroXPskillList.Add(sConfig);
                allHeroXPskillConfigTable[sConfig.nHeroID] = heroXPskillList;
            }  
        }

        public SSchemeHeroXPskillConfig GetHeroXPskillConfig(int nHeroID,int nXPSkillID)
        {
            if (allHeroXPskillConfigTable == null || !allHeroXPskillConfigTable.ContainsKey(nHeroID))
            {
                return null;
            }

            List<SSchemeHeroXPskillConfig> heroXPskillList = allHeroXPskillConfigTable[nHeroID] as List<SSchemeHeroXPskillConfig>;
            foreach (SSchemeHeroXPskillConfig item in heroXPskillList)
            {
                if (item.nXPSkillID == nXPSkillID)
                    return item;
            }

            return null;
        }


        public List<SSchemeHeroXPskillConfig> GetHeroXPskillConfigList(int nHeroID)
        {
            if (allHeroXPskillConfigTable == null || !allHeroXPskillConfigTable.ContainsKey(nHeroID))
            {
                return null;
            }

            List<SSchemeHeroXPskillConfig> heroXPskillList = allHeroXPskillConfigTable[nHeroID] as List<SSchemeHeroXPskillConfig>;
            return heroXPskillList;
        }
    }
}
