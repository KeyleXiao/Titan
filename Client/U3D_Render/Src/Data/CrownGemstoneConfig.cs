/*******************************************************************
** 文件名:	PlayerCrownWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-11-29
** 版  本:	1.0
** 描  述:	皇冠配置
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.CrownGenstoneConfig
{
    // 宝石品级
    public enum EGEMSTONE_GRADE_TYPE
    {
        EGGT_OBSOLETEGRADE,		// 陈旧宝石
        EGGT_FIRSTGRADE,        // 一级宝石
        EGGT_SECONDGRADE,		// 二级宝石
        EGGT_THIRDGRADE,		// 三级宝石
        EGGT_FOURTHGRADE,		// 四级宝石
        EGGT_FIVEGRADE,			// 五级宝石
        EGGT_SIXTHTGRADE,		// 六级宝石
        EGGT_MAX,               // 最大值
    };
    
    // 与CrownPageConfig.csv表里面的列对应
    public enum ECrownGenstoneConfig : int
    {
        EMT_GEMSTONEID,                   
        EMT_GEMSTONENAME,                 
        EMT_GEMSTONEICONID,               
        //EMT_COBBLESTONEICONID,              
        //EMT_WINDOWICONID,   
        EMT_GRADEUPPER = 4,              
        EMT_EXPUPPER,
        EMT_EXPARRAY,       
        EMT_PROPERTYARRAY = 7,                  
        EMT_COBBLESTONEEXTERIORID = 14,           
        EMT_WINDOWICONEXTERIORID,        
        EMT_szExteriorIDArray = 16,         
        EMT_SEILMONEYARRAY = 23,
        EMT_APPRAISALPROCESSMAX,
        EMT_APPRAISALCOSTMONEY,
        EMT_APPRAISALCOSTTICKET,
        EMT_APPRAISALPROCESS,
    }

    public class SSchemeCrownGemstoneConfig
    {
        public int nGemstoneID;                   
        public string sGemstoneName;              
        public int nGemstoneIconID;              
        //public int nCobblestoneIconID;                
        //public int nWindowIconID;         
        public int nGradeUpper;               
        public int nExpUpper;
        public string szExpArray;
        public string[] szPropertyArray = new string[(int)EGEMSTONE_GRADE_TYPE.EGGT_MAX];       
        public int nCobblestoneExteriorID;               
        public int nWindowIconExteriorID;
        public int[] nExteriorIDArray = new int[(int)EGEMSTONE_GRADE_TYPE.EGGT_MAX];  
        public string szSeilMoneyArray ;
        public int nAppraisalProcessMax;						// 鉴定进度上限
        public int nAppraisalCostMoney;						    // 鉴定消耗金币
        public int nAppraisalCostTicket;						// 鉴定消耗点卷
        public int nAppraisalProcess;							// 鉴定增加进度
        public string[] szPropertyDescArray = new string[(int)EGEMSTONE_GRADE_TYPE.EGGT_MAX];    
    }

    public class SSchemeCrownEffectConfig
    {
        public int nPropertyID;
        public int nPropertyValue;
        public string sGemstoneName;
        public string sGemstoneDesc;
        public string szEffectArray;
        public string szValueArray;
        public int nPropertyNameType;
        public int nPropShowValue;
    }

    public class SSchemeCrownGemstoneExteriorConfig
    {
        public int nExteriorID;
        public int nFacescore;
        public float nColours;
        public float nLight;
        public float nTexture;
        public float nWeight;
        public string szModelPath;
        public float fScale;
        public float fScale2;
    }

    public class SSchemeCrownCobstoneExteriorConfig
    {
        public int nCobID;
        public string szName;
        public int nCobIconID;
        public string szDesc;
    }

    public class SSchemeCrownGemstoneRandomConfig
    {
        public int nGroupID;
        public int nOpenWinCost;
        public int nAdditionWinCost;
    }

    class CrownPageConfigPage
    {
        public Dictionary<int, SSchemeCrownGemstoneConfig> allCrownPageConfigGemstoneTable;
        public Hashtable allCrownPageConfigEffectTable;
        public Hashtable allCrownPageConfigExteriorTable;
        public Hashtable allCrownPageConfigCobTable;
        public Hashtable allCrownPageConfigGemRandomTable;
        private static CrownPageConfigPage singleton = new CrownPageConfigPage();
        public static CrownPageConfigPage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new CrownPageConfigPage();
            }
        }

        public void Load(string stPath1, string stPath2, string stPath3, string stPath4, string stPath5)
        {

            InitCrownPageConfigGemstoneCsv(stPath1);



            InitCrownPageConfigEffectCsv(stPath2);



            InitCrownPageConfigExteriorCsv(stPath3);



            InitCrownPageConfigCobCsv(stPath4);


            InitCrownPageConfigGemsRandomCsv(stPath5);

        }

        public void UnLoad()
        {
            if (allCrownPageConfigGemstoneTable != null)
            {
                allCrownPageConfigGemstoneTable.Clear();
                allCrownPageConfigGemstoneTable = null;
            }

            if (allCrownPageConfigEffectTable != null)
            {
                allCrownPageConfigEffectTable.Clear();
                allCrownPageConfigEffectTable = null;
            }

            if (allCrownPageConfigExteriorTable != null)
            {
                allCrownPageConfigExteriorTable.Clear();
                allCrownPageConfigExteriorTable = null;
            }

            if (allCrownPageConfigCobTable != null)
            {
                allCrownPageConfigCobTable.Clear();
                allCrownPageConfigCobTable = null;
            }

            if (allCrownPageConfigGemRandomTable != null)
            {
                allCrownPageConfigGemRandomTable.Clear();
                allCrownPageConfigGemRandomTable = null;
            }

            singleton = null;
        }

        // 初始化csv表
        protected void InitCrownPageConfigGemstoneCsv(string stPath)
        {

            ScpReader reader = new ScpReader(stPath, true, 2);

            allCrownPageConfigGemstoneTable = new Dictionary<int,SSchemeCrownGemstoneConfig>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {

                SSchemeCrownGemstoneConfig sConfig = new SSchemeCrownGemstoneConfig();
                sConfig.nGemstoneID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_GEMSTONEID, 0);
                sConfig.sGemstoneName = reader.GetString(i, (int)ECrownGenstoneConfig.EMT_GEMSTONENAME, "");
                sConfig.nGemstoneIconID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_GEMSTONEICONID, 0);
                //sConfig.nCobblestoneIconID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_COBBLESTONEICONID, 0);
                //sConfig.nWindowIconID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_WINDOWICONID, 0);
                sConfig.nGradeUpper = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_GRADEUPPER, 0);
                sConfig.nExpUpper = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_EXPUPPER, 0);
                sConfig.szExpArray = reader.GetString(i, (int)ECrownGenstoneConfig.EMT_EXPARRAY, "");
                string szProperty = "";
                for (int j = 1; j <= (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX; ++j)
                {
                    szProperty = reader.GetString(i, (int)ECrownGenstoneConfig.EMT_PROPERTYARRAY + j-1 , "");
                    int nIndex = j == (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX ? 0 : j;
                    sConfig.szPropertyArray[nIndex] = szProperty;
                }

                sConfig.nCobblestoneExteriorID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_COBBLESTONEEXTERIORID, 0);
                sConfig.nWindowIconExteriorID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_WINDOWICONEXTERIORID, 0);

                int nExteriorID = 0;
                for (int j = 1; j <= (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX; ++j)
                {
                    nExteriorID = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_szExteriorIDArray + j - 1, 0);
                    int nIndex = j == (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX ? 0 : j;
                    sConfig.nExteriorIDArray[nIndex] = nExteriorID;
                }

                sConfig.szSeilMoneyArray = reader.GetString(i, (int)ECrownGenstoneConfig.EMT_SEILMONEYARRAY, "");
                sConfig.nAppraisalProcessMax = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_APPRAISALPROCESSMAX, 0);						// 鉴定进度上限
                sConfig.nAppraisalCostMoney = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_APPRAISALCOSTMONEY, 0);						// 鉴定消耗金币
                sConfig.nAppraisalCostTicket = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_APPRAISALCOSTTICKET, 0);						// 鉴定消耗点卷
                sConfig.nAppraisalProcess = reader.GetInt(i, (int)ECrownGenstoneConfig.EMT_APPRAISALPROCESS, 0);							// 鉴定增加进度

                allCrownPageConfigGemstoneTable.Add(sConfig.nGemstoneID,sConfig);   
            }        
        }
        // 初始化csv表
        protected void InitCrownPageConfigEffectCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            allCrownPageConfigEffectTable = new Hashtable();
            int nIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
                SSchemeCrownEffectConfig sConfig = new SSchemeCrownEffectConfig();
                sConfig.nPropertyID = reader.GetInt(i, nIndex++, 0);
                sConfig.nPropertyValue = reader.GetInt(i, nIndex++, 0);
                sConfig.sGemstoneName = reader.GetString(i, nIndex++, "");
                sConfig.sGemstoneDesc = reader.GetString(i, nIndex++, "");
                sConfig.szEffectArray = reader.GetString(i, nIndex++, "");
                sConfig.szValueArray = reader.GetString(i, nIndex++, "");
                sConfig.nPropertyNameType = reader.GetInt(i, nIndex++, 0);
                sConfig.nPropShowValue = reader.GetInt(i, nIndex++, 0);

                allCrownPageConfigEffectTable[sConfig.nPropertyID] = sConfig;
            }
        }

        protected void InitCrownPageConfigExteriorCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            allCrownPageConfigExteriorTable = new Hashtable();
            int nIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
                SSchemeCrownGemstoneExteriorConfig sConfig = new SSchemeCrownGemstoneExteriorConfig();
                sConfig.nExteriorID = reader.GetInt(i, nIndex++, 0);
                sConfig.nFacescore = reader.GetInt(i, nIndex++, 0);
                sConfig.nColours = reader.GetFloat(i, nIndex++, 0.0f);
                sConfig.nLight = reader.GetFloat(i, nIndex++, 0.0f);
                sConfig.nTexture = reader.GetFloat(i, nIndex++, 0.0f);
                sConfig.nWeight = reader.GetFloat(i, nIndex++, 0.0f);
                sConfig.szModelPath = reader.GetString(i, nIndex++, "");
                sConfig.fScale = reader.GetFloat(i, nIndex++, 0.0f);
                sConfig.fScale2 = reader.GetFloat(i, nIndex++, 0.0f);
                allCrownPageConfigExteriorTable[sConfig.nExteriorID] = sConfig;
            }
        }

        protected void InitCrownPageConfigCobCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            allCrownPageConfigCobTable = new Hashtable();
            int nIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
                SSchemeCrownCobstoneExteriorConfig sConfig = new SSchemeCrownCobstoneExteriorConfig();
                sConfig.nCobID = reader.GetInt(i, nIndex++, 0);
                sConfig.szName = reader.GetString(i, nIndex++, "");
                sConfig.nCobIconID = reader.GetInt(i, nIndex++, 0);
                sConfig.szDesc = reader.GetString(i, nIndex++, "");

                allCrownPageConfigCobTable[sConfig.nCobID] = sConfig;
            }
        }

        protected void InitCrownPageConfigGemsRandomCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            allCrownPageConfigGemRandomTable = new Hashtable();
            int nIndex = 0;
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
                SSchemeCrownGemstoneRandomConfig sConfig = new SSchemeCrownGemstoneRandomConfig();
                sConfig.nGroupID = reader.GetInt(i, nIndex++, 0);
                sConfig.nOpenWinCost = reader.GetInt(i, nIndex++, 0);
                sConfig.nAdditionWinCost = reader.GetInt(i, nIndex++, 0);


                allCrownPageConfigGemRandomTable[sConfig.nGroupID] = sConfig;
            }
        }

        public SSchemeCrownGemstoneConfig GetCrownPageGemstoneConfig(int nGemstoneID)
        {
            if(allCrownPageConfigGemstoneTable == null||!allCrownPageConfigGemstoneTable.ContainsKey(nGemstoneID))
            {
                return null;
            }

            return allCrownPageConfigGemstoneTable[nGemstoneID] as SSchemeCrownGemstoneConfig;
        }

        public SSchemeCrownEffectConfig GetCrownPageEffectConfig(int nPropertyID)
        {
            if (allCrownPageConfigEffectTable == null || !allCrownPageConfigEffectTable.ContainsKey(nPropertyID))
            {
                return null;
            }

            return allCrownPageConfigEffectTable[nPropertyID] as SSchemeCrownEffectConfig;
        }

        public SSchemeCrownGemstoneExteriorConfig GetCrownPageExteriorConfig(int nExteriorID)
        {
            if (allCrownPageConfigExteriorTable == null || !allCrownPageConfigExteriorTable.ContainsKey(nExteriorID))
            {
                return null;
            }

            return allCrownPageConfigExteriorTable[nExteriorID] as SSchemeCrownGemstoneExteriorConfig;
        }

        public SSchemeCrownCobstoneExteriorConfig GetCrownPageCobConfig(int nCobID)
        {
            if (allCrownPageConfigCobTable == null || !allCrownPageConfigCobTable.ContainsKey(nCobID))
            {
                return null;
            }

            return allCrownPageConfigCobTable[nCobID] as SSchemeCrownCobstoneExteriorConfig;
        }

        public SSchemeCrownGemstoneRandomConfig GetCrownPageGemsRandomConfig(int nGroupID)
        {
            if (allCrownPageConfigGemRandomTable == null || !allCrownPageConfigGemRandomTable.ContainsKey(nGroupID))
            {
                return null;
            }

            return allCrownPageConfigGemRandomTable[nGroupID] as SSchemeCrownGemstoneRandomConfig;
        }
    }
}
