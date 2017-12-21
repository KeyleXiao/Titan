using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.LifeHero
{
    // 与LifeHero.csv表里面的列对应
    /// <summary>
    /// LifeHeroConfig配置类
    /// </summary>
    public class SSchemeLifeHero
    {
        public int lifeHeroIndex;             //本命英雄序号,表示当前第几个本命英雄
        public string warTypeIDArray;         //满足本命英雄的比赛类型ID值数组
        public int rule;                      // 入选本英雄的规则
        public int starLv;                    // 初次获得本命英雄提升指定星级
        public int prizeID;                   // 初次获得本命英雄的奖励ID
        public int goldCount;                 // 替换需要消耗的金币数量
        public int cardCount;                 // 替换需要消耗的卡牌数量
    }

    public class SSchemeLifeHeroLight
    {
        public int heroID;                    //英雄ID
        public int effectID;                  //光效ID
        public int staticEffectID;            // 静态模型光效ID
    }

    public class SSchemeLifeHeroPrize
    {
        public int heroCount;                 // 英雄数量
        public int starLv;                    // 达到评级
        public List<int> prizeIDList;         // 奖励ID
        public string prizeDesc;              // 奖励描述
    }

    class LifeHeroConfig : Singleton<LifeHeroConfig>
    {
        private Dictionary<int, SSchemeLifeHero> lifeHeroConfigDic = new Dictionary<int, SSchemeLifeHero>();
        public Dictionary<int, SSchemeLifeHero> LifeHeroConfigDic { get { return lifeHeroConfigDic; } }
        private Dictionary<int, SSchemeLifeHeroLight> lifeHeroLightDic = new Dictionary<int, SSchemeLifeHeroLight>();
        public Dictionary<int, SSchemeLifeHeroLight> LifeHeroLightDic { get { return lifeHeroLightDic; } }
        private List<SSchemeLifeHeroPrize> lifeHeroPrizeList = new List<SSchemeLifeHeroPrize>();
        public List<SSchemeLifeHeroPrize> LifeHeroPrizeList { get { return lifeHeroPrizeList; } }
        public void Load(string stPath)
        {
            InitLifeHeroCsv(stPath + "LifeHero.csv");
            InitLifeHeroLightCsv(stPath + "LifeHeroLight.csv");
            InitLifeHeroPrizeCsv(stPath + "LifeHeroPrize.csv");
        }

        public void UnLoad()
        {
            lifeHeroConfigDic.Clear();
            lifeHeroLightDic.Clear();
            lifeHeroPrizeList.Clear();
        }
        
        private void InitLifeHeroCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); ++i)
            {
                int col = 0;
                SSchemeLifeHero lifeHero = new SSchemeLifeHero();
                lifeHero.lifeHeroIndex = reader.GetInt(i, col++, int.MinValue);
                lifeHero.warTypeIDArray = reader.GetString(i, col++, "");
                lifeHero.rule = reader.GetInt(i, col++, 0);
                lifeHero.starLv = reader.GetInt(i, col++, 5); //默认提升至5级
                lifeHero.prizeID = reader.GetInt(i, col++, 0);
                lifeHero.goldCount = reader.GetInt(i, col++, 0);
                lifeHero.cardCount = reader.GetInt(i, col++, 0);

                lifeHeroConfigDic[i] = lifeHero;
            }
        }

        private void InitLifeHeroLightCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); ++i)
            {
                SSchemeLifeHeroLight lifeHero = new SSchemeLifeHeroLight();
                lifeHero.heroID = reader.GetInt(i, 0, -1);
                lifeHero.effectID = reader.GetInt(i, 1, -1);
                lifeHero.staticEffectID = reader.GetInt(i, 2, -1);

                lifeHeroLightDic[lifeHero.heroID] = lifeHero;
            }
        }

        private void InitLifeHeroPrizeCsv(string stPath)
        {
            char[] charSeparator = new char[] { ';' };
            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); ++i)
            {
                int col = 0;
                SSchemeLifeHeroPrize lifeHero = new SSchemeLifeHeroPrize();
                int key = reader.GetInt(i, col++, 0);
                lifeHero.heroCount = reader.GetInt(i, col++, 0);
                lifeHero.starLv = reader.GetInt(i, col++, 0);
                string prizeIDs = reader.GetString(i, col++, "");
                lifeHero.prizeIDList = ScpReader.ConvertStringToIntArray(prizeIDs, charSeparator);
                lifeHero.prizeDesc = reader.GetString(i, col++, "");
                lifeHeroPrizeList.Add(lifeHero);
            }
        }

        public bool IsMeetWarType(int i, int warType)
        {
            if (i < 0 || i >= (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
            {
                Debug.LogWarning("out of index range.index = " + i);
                return false;
            }

            SSchemeLifeHero config;
            if (lifeHeroConfigDic.TryGetValue(i, out config))
            {
                return config.warTypeIDArray.Contains(warType.ToString());
            }
            else
            {
                return false;
            }
        }
    }
}
