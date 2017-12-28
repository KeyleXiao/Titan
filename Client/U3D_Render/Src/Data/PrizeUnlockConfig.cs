using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.PrizeUnlockConfig
{
    // 与PrizeUnlockConfig.csv表里面的列对应
    /// <summary>
    /// 选人提示类
    /// </summary>
    public class SSchemePrizeUnlockConfig
    {
        public int nUnlockType;                // 角色解锁or英雄等级解锁or英雄星级解锁
        public int nParam_1;                   // 参数1（英雄ID）
        public int nParam_2;                   // 参数2（解锁等级）
        public int nParam_3;                   // 参数3（奖励id列表）
    }

    public enum UnlockPrizeType
    {
        UNLOCKTYPE_ACTOR =1,
        UNLOCKTYPE_HEROLEVEL,
        UNLOCKTYPE_HEROSTAR,
    }

    public class ActorPrizeUnlockItem
    {
        public int nUnlockLevel;
        public bool bUnlock;
        public int[] nPrizeIdList;
        public int nPrizeIconId;
    }

    public class HeroLevelPrizeUnlockItem
    {
        public int nUnlockLevel;
        public bool bUnlock;
        public int[] nPrizeIdList;
        public int nPrizeIconId;
    }

    public class HeroStarPrizeUnlockItem
    {
        public int nUnlockLevel;
        public bool bUnlock;
        public int[] nPrizeIdList;
        public int nPrizeIconId;
        public string strUnlockStarDesc;
    }

    //public class HeroDicUnlockConfig
    //{
    //    public Dictionary<int, List<HeroPrizeUnlockItem>> dicHeroConfig = new Dictionary<int, List<HeroPrizeUnlockItem>>();
    //}

    //public class ActorDicUnlockConfig
    //{
    //    public List<ActorPrizeUnlockItem> listActorConfig = new List<ActorPrizeUnlockItem>();
    //}

    class PrizeUnlockConfig
    {
        //public ActorDicUnlockConfig ActorPrizeUnlockConfig = new ActorDicUnlockConfig();
        //public HeroDicUnlockConfig HeroPrizeUnlockConfig = new HeroDicUnlockConfig();

        public List<ActorPrizeUnlockItem> actorPrizeUnlockList = new List<ActorPrizeUnlockItem>();
        public Dictionary<int, List<HeroLevelPrizeUnlockItem>> heroLevelPrizeUnlockDic = new Dictionary<int, List<HeroLevelPrizeUnlockItem>>();
        public Dictionary<int, List<HeroStarPrizeUnlockItem>> heroStarPrizeUnlockDic = new Dictionary<int, List<HeroStarPrizeUnlockItem>>();

        private static PrizeUnlockConfig singleton = new PrizeUnlockConfig();
        public static PrizeUnlockConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PrizeUnlockConfig();
            }
        }

        public void Load(string stPath)
        {
            InitPrizeUnlockConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (actorPrizeUnlockList != null)
            {
                actorPrizeUnlockList.Clear();
                actorPrizeUnlockList = null;
            }
            if (heroLevelPrizeUnlockDic != null)
            {
                heroLevelPrizeUnlockDic.Clear();
                heroLevelPrizeUnlockDic = null;
            }
            if (heroStarPrizeUnlockDic != null)
            {
                heroStarPrizeUnlockDic.Clear();
                heroStarPrizeUnlockDic = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitPrizeUnlockConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            int nIndex = 0;
            int nHeroID = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
                nIndex++;
                nIndex++;
                int nUnlockType = reader.GetInt(i, nIndex++, 0);

                if (nUnlockType == (int)UnlockPrizeType.UNLOCKTYPE_ACTOR)
                {
                    ActorPrizeUnlockItem item = new ActorPrizeUnlockItem();
                    nHeroID = reader.GetInt(i, nIndex++, 0);
                    item.nUnlockLevel = reader.GetInt(i, nIndex++, 0);
                    string[] strPrizeID = reader.GetString(i, nIndex++, "").Split(';');
                    if (strPrizeID != null && strPrizeID.Length > 0)
                    {
                        item.nPrizeIdList = new int[strPrizeID.Length];
                        for (int j = 0; j < strPrizeID.Length; j++)
                        {
                            //item.nPrizeIdList[j] = Convert.ToInt32(strPrizeID[j]);
                            Int32.TryParse(strPrizeID[j], out item.nPrizeIdList[j]);
                        }
                    }
                    item.nPrizeIconId = reader.GetInt(i, nIndex++, 0);
                    item.bUnlock = false;
                    actorPrizeUnlockList.Add(item);
                }
                else if (nUnlockType == (int)UnlockPrizeType.UNLOCKTYPE_HEROLEVEL)
                {
                    HeroLevelPrizeUnlockItem item = new HeroLevelPrizeUnlockItem();
                    nHeroID = reader.GetInt(i, nIndex++, 0);
                    item.nUnlockLevel = reader.GetInt(i, nIndex++, 0);
                    string[] strPrizeID = reader.GetString(i, nIndex++, "").Split(';');
                    if (strPrizeID != null && strPrizeID.Length > 0)
                    {
                        item.nPrizeIdList = new int[strPrizeID.Length];
                        for (int j = 0; j < strPrizeID.Length; j++)
                        {
                            //item.nPrizeIdList[j] = Convert.ToInt32(strPrizeID[j]);
                            Int32.TryParse(strPrizeID[j], out item.nPrizeIdList[j]);
                        }
                    }                
                    item.nPrizeIconId = reader.GetInt(i, nIndex++, 0);
                    item.bUnlock = false;

                    if (!heroLevelPrizeUnlockDic.ContainsKey(nHeroID))
                    {
                        List<HeroLevelPrizeUnlockItem> listHero = new List<HeroLevelPrizeUnlockItem>();
                        heroLevelPrizeUnlockDic[nHeroID] = listHero;
                    }
                    heroLevelPrizeUnlockDic[nHeroID].Add(item);
                }
                else if (nUnlockType == (int)UnlockPrizeType.UNLOCKTYPE_HEROSTAR)
                {
                    HeroStarPrizeUnlockItem item = new HeroStarPrizeUnlockItem();
                    nHeroID = reader.GetInt(i, nIndex++, 0);
                    item.nUnlockLevel = reader.GetInt(i, nIndex++, 0);
                    string[] strPrizeID = reader.GetString(i, nIndex++, "").Split(';');
                    if (strPrizeID != null && strPrizeID.Length > 0)
                    {
                        item.nPrizeIdList = new int[strPrizeID.Length];
                        for (int j = 0; j < strPrizeID.Length; j++)
                        {
                            //item.nPrizeIdList[j] = Convert.ToInt32(strPrizeID[j]);
                            Int32.TryParse(strPrizeID[j], out item.nPrizeIdList[j]);
                        }
                    }
                    item.nPrizeIconId = reader.GetInt(i, nIndex++, 0);
                    nIndex++;
                    item.strUnlockStarDesc = reader.GetString(i, nIndex++, "");
                    item.bUnlock = false;

                    if (!heroStarPrizeUnlockDic.ContainsKey(nHeroID))
                    {
                        List<HeroStarPrizeUnlockItem> listHero = new List<HeroStarPrizeUnlockItem>();
                        heroStarPrizeUnlockDic[nHeroID] = listHero;
                    }
                    heroStarPrizeUnlockDic[nHeroID].Add(item);
                }
            }
        }
    }
}
