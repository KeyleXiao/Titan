using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.RunePage
{


    // 与RunePage.csv表里面的列对应
    public enum Rune : int
    {
        EMT_ID,                      // 索引ID
        EMT_NAME,                    // 称号名字
        EMT_ICONID,                  // 符文图片ＩＤ
        EMT_EXCLUSIVEID,             // 符文专属id
        EMT_INLAYTYPE,               // 镶嵌类型
        EMT_PROPERTYTYPE,            // 属性类型
        EMT_LEVEL,                   // 符文等级
        EMT_DESC,                    // 符文描述
        EMT_EFFECTIDARRAY,           // 符文效果id组
        EMT_EFFECTPARAMETERS ,       // 符文效果参数组
        EMT_PROPERTYNAMETYPE,        // 符文的属性名称类型
    }

    public enum RuneType
    {
  
        // 雕文
        ENUM_RUNETYPE_GLYPH = 1,
        // 印记
        ENUM_RUNETYPE_HALLMARK,
        // 符印
        ENUM_RUNETYPE_SIGIL,
        // 精华
        ENUM_RUNETYPE_CREAM 

    };

    public enum RuneLevel
    {
        // 等级1
        ENUM_RUNELEVEL_1 = 1,
        // 等级2
        ENUM_RUNELEVEL_2,
        // 等级3
        ENUM_RUNELEVEL_3,
    };

    public enum RunePropertyType
    {
        // 生命值
        ENUM_RUNEPROPERTYTYPE_HEALTH = 1,
        // 法力值
        ENUM_RUNEPROPERTYTYPE_MANA,
        // 攻击
        ENUM_RUNEPROPERTYTYPE_ATTACK,
        // 魔法
        ENUM_RUNEPROPERTYTYPE_MAGIC,
        // 防御
        ENUM_RUNEPROPERTYTYPE_DEFENCE,
        // 功能
        ENUM_RUNEPROPERTYTYPE_FUNCTION,
        // 全部
        ENUM_RUNEPROPERTYTYPE_ALL,
    };

    public enum RunePropertyNameType
    {
        RUNEPROPERTYNAMETYPE_ADD = 1,
        RUNEPROPERTYNAMETYPE_PERSENT = 2,
        RUNEPROPERTYNAMETYPE_GROW =4,
        RUNEPROPERTYNAMETYPE_ENLARGEHUNDRED  = 8,
    }

    /// <summary>
    /// 符文类
    /// </summary>
    public class SSchemeRuneConfig
    {

        public int nId;                          // id
                        
        public string szRuneName;                // 符文名字

        public int nRuneIconId;                  // 符文图片id

        public int nExclusiveId;                 // 符文专属id

        public int nRuneInlayType;               // 符文镶嵌类型

        public int nRunePropertyType;            // 符文属性类型

        public int nRuneLevel;                   // 符文等级

        public string szRuneDESC;                // 符文描述

        public string szEffectIdArray;           // 符文属性id组

        public string szParametersValueArray;    // 符文参数值组

        public int nPropertyNameType;            // 符文的属性名称类型
    }
  
    class RunePage
    {
        public const int DefaultTypeCount = 3;
        public const int DefaultLevelCount = 6;
        public const int DefaultSameLevelCount = 3;
        public Hashtable allRunePageTable;
        private static RunePage singleton = new RunePage();
        public int[, ,] aRuneArray;  // 储存符文id,根据类型和等级划分出来，大区分类型小区分等级、为后面控制打下铺垫
        public static RunePage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new RunePage();
            }
        }

        public void Load(string stPath)
        {
            InitRunePageCsv(stPath);
        }

        public void UnLoad()
        {
            if (allRunePageTable != null)
            {
                allRunePageTable.Clear();
                allRunePageTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitRunePageCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2);
            allRunePageTable = new Hashtable();
            aRuneArray = new int[DefaultTypeCount, DefaultLevelCount, DefaultSameLevelCount]; // 目前写死
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeRuneConfig sConfig = new SSchemeRuneConfig();
                sConfig.nId = reader.GetInt(i, (int)Rune.EMT_ID, 0);
                sConfig.szRuneName = reader.GetString(i, (int)Rune.EMT_NAME, "");
                sConfig.nRuneIconId = reader.GetInt(i, (int)Rune.EMT_ICONID, 0);
                sConfig.nExclusiveId = reader.GetInt(i, (int)Rune.EMT_EXCLUSIVEID, 0);
                sConfig.nRuneInlayType = reader.GetInt(i, (int)Rune.EMT_INLAYTYPE, 0);
                sConfig.nRunePropertyType = reader.GetInt(i, (int)Rune.EMT_PROPERTYTYPE, 0);
                sConfig.nRuneLevel = reader.GetInt(i, (int)Rune.EMT_LEVEL, 0);
                sConfig.szRuneDESC = reader.GetString(i, (int)Rune.EMT_DESC, "");
                sConfig.szEffectIdArray = reader.GetString(i, (int)Rune.EMT_EFFECTIDARRAY, "");
                sConfig.szParametersValueArray = reader.GetString(i, (int)Rune.EMT_EFFECTPARAMETERS, "");
                sConfig.nPropertyNameType = reader.GetInt(i, (int)Rune.EMT_PROPERTYNAMETYPE, 0);

                if (sConfig.nId == 0)
                {
                    continue;
                }

                allRunePageTable[sConfig.nId] = sConfig;
            }
        }

        // 判断是否有该id的符文信息
        private bool IsHasRuneInfo(int nRuneId, ref SSchemeRuneConfig sRunePage)
        {
            bool res = false;
            if (allRunePageTable.ContainsKey(nRuneId))
            {
                sRunePage = allRunePageTable[nRuneId] as SSchemeRuneConfig;
                res = true;
            }
            return res;
        }

        // 根据id获得符文的信息
        public void GetRuneInfo(int nRuneId, ref SSchemeRuneConfig sRunePage)
        {
            if (!IsHasRuneInfo(nRuneId, ref sRunePage))
            {
                Trace.LogWarning("没有找到id为：" + nRuneId + "的符文");
                sRunePage = null;
            }
        }
        // 获得所有符文信息
        public Hashtable GetAllRunePageTable()
        {

            return allRunePageTable;
        }

        public int GetSingleRuneType(int nRuneId)
        {
            if (allRunePageTable.ContainsKey(nRuneId))
            {
                SSchemeRuneConfig RuneConfig = allRunePageTable[nRuneId] as SSchemeRuneConfig;
                return RuneConfig.nRuneInlayType;
            }
            return 0;
        }
    }
}
