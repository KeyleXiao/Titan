using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using U3D_Render;
using SPELL;
using ASpeedGame.Data.HeroTypeConfig;

namespace DataCenter
{
    /// <summary>
    /// 死亡回放数据管理中心
    /// </summary>
    public class WarDeathReplayDataManager
    {
        //伤害类型
        //public enum HurtType
        //{
        //    HurtType_Skill,      //技能伤害
        //    HurtType_Effect,     //效果伤害
        //}

        //主角承受伤害数据项
        public class HurtDataItem
        {
            public int uidOperator;
            public int skillID;
            public int hurtNum;
            public uint hurtTime;
        }

        public class SkillDamageItem
        {
            public int skillID;
            public int damageNum;
            public int spellEffectType;
            public int skillIcon;
            public string szSkillName = "";
            public string szSkillDesc = "";
        }

        //死亡回放数据项，按攻击对象汇总，相同类型怪物或小兵则汇总数据
        public class DeathReplayData
        {
            public ENTITY_TYPE entityType;
            public int uidOperator;
            public ulong pdbID;
            public int skinID;
            public int monsterID;
            //技能伤害表，按技能ID存伤害
            public Dictionary<int, int> skillDamageList = new Dictionary<int, int>();
            public List<SkillDamageItem> skillInfoList = new List<SkillDamageItem>();
            public int totalDamage
            {
                get 
                {
                    int totalNum = 0;
                    foreach (int v in skillDamageList.Values)
                        totalNum += v;
                    return totalNum;
                } 
            }
            public bool bMurder;
            public string roleName;
            public string skinName;
            public string heroDesc;
            //角色的个人物品信息
            public List<cmd_war_persongoodsinfo> goodsInfo;
            //角色属性信息
            public WarRecordTableDataManager.WarPersonInfo personInfo;
        }

        //主角伤害承受数据表，最大数据条目个数由DamageCalculationsNum决定
        private LinkedList<HurtDataItem> m_ActorHurtDataList;
        //死亡回放数据表
        private Dictionary<int, DeathReplayData> m_DeathReplayDataList;
        public int MurderSkinID { get { return m_MurderSkinID; } set { m_MurderSkinID = value; } }
        //击杀者皮肤ID
        private int m_MurderSkinID;
        //死亡回放数据统计数目定义
        private const int DamageCalculationsNum = 30;
        private const uint DamageCalculationsTime = 60000;
        public uint StartTime { get { return m_startTime; } }
        private uint m_startTime = 0;
        public uint EndTime { get { return m_endTime; } }
        private uint m_endTime = 0;
        public int TotalHurtNum
        {
            get 
            {
                int num = 0;
                foreach (DeathReplayData data in m_DeathReplayDataList.Values)
                    num += data.totalDamage;
                return num;
            }
        }

        public void Init()
        {
            m_ActorHurtDataList = new LinkedList<HurtDataItem>();
            m_DeathReplayDataList = new Dictionary<int, DeathReplayData>();
        }

        public void Clear()
        {
            m_ActorHurtDataList.Clear();
            m_DeathReplayDataList.Clear();
            m_MurderSkinID = 0;
            m_startTime = 0;
            m_endTime = 0;
        }

        /// <summary>
        /// 添加主角承受伤害数据
        /// </summary>
        /// <param name="operatorView"></param>
        /// <param name="damageData"></param>
        public void AddHurtData(EntityView operatorView, cmd_creature_damage damageData)
        {
            if (m_ActorHurtDataList.Count > DamageCalculationsNum - 1)
                m_ActorHurtDataList.RemoveFirst();
            if (!(operatorView.CampFlag == SPELL.CampFlag.CampFlag_Enemy))
                return;

            HurtDataItem data = new HurtDataItem();
            data.uidOperator = Convert.ToInt32(damageData.uidOperator);
            data.skillID = damageData.nID;
            data.hurtNum = damageData.nDeltaHP;
            data.hurtTime = GameLogicAPI.GetUnityTickCount();
            m_ActorHurtDataList.AddLast(data);
        }

        public void ClearDeathReplayData()
        {
            m_ActorHurtDataList.Clear();
            m_DeathReplayDataList.Clear();
            m_MurderSkinID = 0;
            m_startTime = 0;
            m_endTime = 0;
        }

        /// <summary>
        /// 获取死亡回放数据信息，返回主角死前8次承受伤害汇总后的角色排名信息(PS:击杀者强制排第一，即使伤害不是最高)
        /// </summary>
        /// <returns></returns>
        public List<DeathReplayData> GetDeathReplayInfo()
        {
            List<DeathReplayData> DeathReplayDataList = new List<DeathReplayData>();
            List<int> enemyIDList = SummaryAndSortData();
            foreach (int enemyID in enemyIDList)
            {
                DeathReplayData damageData;
                if (m_DeathReplayDataList.TryGetValue(enemyID, out damageData))
                    DeathReplayDataList.Add(damageData);
            }
            return DeathReplayDataList;
        }

        /// <summary>
        /// 汇总并排序伤害总量，返回按伤害总量排血的角色ID表
        /// </summary>
        /// <returns></returns>
        private List<int> SummaryAndSortData()
        {
            m_DeathReplayDataList.Clear();
            List<int> enemyOrderList = new List<int>();

            m_endTime = GameLogicAPI.GetUnityTickCount();

            bool tempFlag = true;

            //先汇总8次承受伤害数据
            foreach (HurtDataItem hurtData in m_ActorHurtDataList)
            {
                if (m_endTime - hurtData.hurtTime > DamageCalculationsTime)
                    continue;
                if (tempFlag)
                {
                    //保存符合条件的第一次受伤时间
                    m_startTime = hurtData.hurtTime;
                    tempFlag = false;
                }

                DeathReplayData damageData = new DeathReplayData();
                EntityView ev = EntityFactory.getEntityViewByID(hurtData.uidOperator);
                if (ev == null || ev.Property == null)
                    continue;
                int skinID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
                if (m_DeathReplayDataList.ContainsKey(skinID))
                {
                    damageData = m_DeathReplayDataList[skinID];
                    if (damageData.skillDamageList.ContainsKey(hurtData.skillID))
                        damageData.skillDamageList[hurtData.skillID] += hurtData.hurtNum;
                    else
                    {
                        damageData.skillDamageList.Add(hurtData.skillID, hurtData.hurtNum);
                    }
                    m_DeathReplayDataList[skinID] = damageData;
                }
                else
                {
                    damageData.uidOperator = hurtData.uidOperator;
                    damageData.pdbID = LogicDataCenter.recordTableDataManager.getWarPersonPDBIDByUID(damageData.uidOperator);
                    damageData.skinID = skinID;
                    EntityView view = EntityFactory.getEntityViewByID(Convert.ToInt32(hurtData.uidOperator));
                    if (view != null && view.Property != null)
                    {
                        damageData.entityType = view.Type;
                        if (view.Type == ENTITY_TYPE.TYPE_MONSTER)
                        {
                            /////////////////////////////////////////////
                            damageData.monsterID = view.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                        }
                        damageData.roleName = view.Property.CreatureName;
                        damageData.skinName = view.Property.CreatureName;
                        if(view.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            int heroId = view.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                            SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(heroId);
                            if (config != null)
                                damageData.heroDesc = config.strAgainstWay;
                        }
                    }
                    WarRecordTableDataManager.WarPlayerVsInfo vsInfo = LogicDataCenter.recordTableDataManager.GetWarPlayerVsInfo(damageData.pdbID);
                    if(null != vsInfo)
                        damageData.skinName = vsInfo.szVocationName;
                    damageData.skillDamageList.Add(hurtData.skillID, hurtData.hurtNum);
                    damageData.goodsInfo = LogicDataCenter.warMobaGoodsDataManager.GetPlayerGoodsInfo(damageData.pdbID);
                    damageData.personInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID(damageData.pdbID);
                    m_DeathReplayDataList.Add(skinID, damageData);
                }
            }

            //再排汇总后的敌方角色的技能伤害表
            foreach (DeathReplayData data in m_DeathReplayDataList.Values)
            {
                foreach (KeyValuePair<int,int> kvp in data.skillDamageList)
                {
                    SkillDamageItem item = new SkillDamageItem();
                    item.skillID = kvp.Key;
                    item.damageNum = kvp.Value;
                    IntPtr Ptr = GameLogicAPI.getSpellData(kvp.Key);
                    if (Ptr != IntPtr.Zero)
                    {
                        SPELL.SPELL_DATA spellData = IntPtrHelper.toData<SPELL.SPELL_DATA>(Ptr);
                        item.spellEffectType = spellData.nSpellEffectType;
                        item.skillIcon = spellData.nIcon;
                        IntPtr strPtr = GameLogicAPI.getSpellStrData(kvp.Key, (int)SPELL_DATA_ID.SPDATA_NAME);
                        if (strPtr != null)
                        {
                            item.szSkillName = IntPtrHelper.Ptr2Str(strPtr);
                        }
                        strPtr = GameLogicAPI.getSpellStrData(kvp.Key, (int)SPELL_DATA_ID.SPDATA_DESC);
                        if (strPtr != null)
                        {
                            item.szSkillDesc = IntPtrHelper.Ptr2Str(strPtr);
                        }
                    }
                    //如果是普通攻击
                    if((item.spellEffectType & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_NormalAttack) != 0 )
                    {
                        //判断表里是否已经有普通攻击了，如果有就直接加值
                        SkillDamageItem temp = data.skillInfoList.Find(x => (x.spellEffectType & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_NormalAttack) != 0);
                        if (null != temp)
                            temp.damageNum += item.damageNum;
                        else
                            data.skillInfoList.Add(item);
                    }
                    else
                        data.skillInfoList.Add(item);
                }
                data.skillInfoList.Sort((x, y) => -x.damageNum.CompareTo(y.damageNum));
            }

            //再排序
            var dicSort = from objDic in m_DeathReplayDataList orderby objDic.Value.totalDamage descending select objDic;
            foreach (KeyValuePair<int, DeathReplayData> kvp in dicSort)
                enemyOrderList.Add(kvp.Key);
            //返回角色ID表
            return enemyOrderList;
        }

    }
}
