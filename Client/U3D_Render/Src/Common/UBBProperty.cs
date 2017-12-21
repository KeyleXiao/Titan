/*******************************************************************
** 文件名:	UBBProperty.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2015-10-15
** 版  本:	1.0
** 描  述:	

		放置UBB所需要的属性等等
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using U3D_Render.Common;
using System.Collections;
using U3D_Render;
using GameLogic;
public class UBBProperty
{
    // 存储属性类型和属性系数
    private Dictionary<string, List<ratioAppendInfo>> _propertyRatio = new Dictionary<string, List<ratioAppendInfo>>();
    // 存储冷却ID和冷却时间
    public Dictionary<int, float> _dicCoolTime = new Dictionary<int, float>();
    // 存储充能buffID和充能信息
    public Dictionary<int, chargeValue_Info> _dicChargeCount = new Dictionary<int, chargeValue_Info>();
    //　单实例
    private static UBBProperty _instance = null;
    // 属性对象
    private CreatureProperty _nProperty = null;
    // 获得单实例
    public static UBBProperty GetInstance()
    {
        if (_instance == null)
        {
            _instance = new UBBProperty();
            _instance.PropertyUBB();
        }
        return _instance;
    }
    // UBB汇总
    public void PropertyUBB()
    {
        #region 冷却[冷却_类型数值(技能或装备）=冷却ID]
        UBB.AddReplacementEx(@"\[CD(\d)=(\d+)\]", CD_Callbak, true);
        #endregion 冷却1[冷却=系数]


        #region 充能固定血量[CHP=系数]
        UBB.AddReplacementEx(@"\[CHP=(?<CHP>.+?)]", CHP);
        #endregion 充能固定血量[CHP=系数]

        #region 充能固定蓝量[CMP=系数]
        UBB.AddReplacementEx(@"\[CMP=(?<CMP>.+?)]", CMP);
        #endregion 充能固定蓝量[CMP=系数]

        #region 充能固定物攻[CPA=系数]
        UBB.AddReplacementEx(@"\[CPA=(?<CPA>.+?)]", CPA);
        #endregion 充能固定物攻 [CPA=系数]

        #region 充能固定法强[CMA=系数]
        UBB.AddReplacementEx(@"\[CMA=(?<CMA>.+?)]", CMA);
        #endregion 充能固定法强 [CMA=系数]

        #region 充能固定物防[CPD=系数]
        UBB.AddReplacementEx(@"\[CPD=(?<CPD>.+?)]", CPD);
        #endregion 充能固定物防[CPD=系数]

        #region 充能固定法防[CMD=系数]
        UBB.AddReplacementEx(@"\[CMD=(?<CMD>.+?)]", CMD);
        #endregion 充能固定法防[CMD=系数]

        #region 充能固定移动[CSD=系数]
        UBB.AddReplacementEx(@"\[CSD=(?<CSD>.+?)]", CSD);
        #endregion 充能固定移动[CSD=系数]

        #region 充能固定攻速[CPASD=系数]
        UBB.AddReplacementEx(@"\[CPASD=(?<CPASD>.+?)]", CPASD);
        #endregion 充能固定攻速[CPASD=系数]

        #region 充能固定暴击[CCRC=系数]
        UBB.AddReplacementEx(@"\[CCRC=(?<CCRC>.+?)]", CCRC);
        #endregion 充能固定暴击 [CCRC=系数]

        #region 充能固定回血[CHPRY=系数]
        UBB.AddReplacementEx(@"\[CHPRY=(?<CHPRY>.+?)]", CHPRY);
        #endregion 充能固定回血[CHPRY=系数]

        #region 充能固定回魔[CMPRY=系数]
        UBB.AddReplacementEx(@"\[CMPRY=(?<CMPRY>.+?)]", CMPRY);
        #endregion 充能固定回魔[CMPRY=系数]

        #region 充能百分比血量[PHP=系数]
        UBB.AddReplacementEx(@"\[PHP=(?<PHP>.+?)]", PHP);
        #endregion 充能百分比血量[PHP=系数]

        #region 充能百分比蓝量[PMP=系数]
        UBB.AddReplacementEx(@"\[PMP=(?<PMP>.+?)]", PMP);
        #endregion 充能百分比蓝量[PMP=系数]

        #region 充能百分比物攻[PPA=系数]
        UBB.AddReplacementEx(@"\[PPA=(?<PPA>.+?)]", PPA);
        #endregion 充能百分比物攻 [PPA=系数]

        #region 充能百分比法强[PMA=系数]
        UBB.AddReplacementEx(@"\[PMA=(?<PMA>.+?)]", PMA);
        #endregion 充能百分比法强 [PMA=系数]

        #region 充能百分比物防[PPD=系数]
        UBB.AddReplacementEx(@"\[PPD=(?<PPD>.+?)]", PPD);
        #endregion 充能百分比物防[PPD=系数]

        #region 充能百分比法防[PMD=系数]
        UBB.AddReplacementEx(@"\[PMD=(?<PMD>.+?)]", PMD);
        #endregion 充能百分比法防[PMD=系数]

        #region 充能百分比移动[PSD=系数]
        UBB.AddReplacementEx(@"\[PSD=(?<PSD>.+?)]", PSD);
        #endregion 充能百分比移动[PSD=系数]

        #region 充能百分比攻速[PPASD=系数]
        UBB.AddReplacementEx(@"\[PPASD=(?<PPASD>.+?)]", PPASD);
        #endregion 充能百分比攻速[PPASD=系数]

        #region 充能百分比暴击[PCRC=系数]
        UBB.AddReplacementEx(@"\[PRP=(?<PRP>.+?)]", PCRC);
        #endregion 充能百分比暴击 [PCRC=系数]

        #region 充能百分比回血[PHPRY=系数]
        UBB.AddReplacementEx(@"\[PHPRY=(?<PHPRY>.+?)]", PHPRY);
        #endregion 充能百分比回血[PHPRY=系数]

        #region 充能百分比回魔[PMPRY=系数]
        UBB.AddReplacementEx(@"\[PMPRY=(?<PMPRY>.+?)]", PMPRY);
        #endregion 充能百分比回魔[PMPRY=系数]

        #region 充能层数[CCOUNT=系数]
        UBB.AddReplacementEx(@"\[CCOUNT=(?<CCOUNT>.+?)]", CCOUNT);
        #endregion 充能层数[CCOUNT=系数]

        #region 物防特殊公式1减伤攻击[PDFA1=系数]
        UBB.AddReplacementEx(@"\[PDFA1=(?<PDFA1>.+?)]", PDFA1);
        #endregion 物防特殊公式[PDFA1=系数]

        #region 魔防特殊公式1减伤攻击[MDFA1=系数]
        UBB.AddReplacementEx(@"\[MDFA1=(?<MDFA1>.+?)]", MDFA1);
        #endregion 魔防特殊1公式[MDFA1=系数]

        return;
    }

    /// <summary>
    /// 冷却回调
    /// </summary>
    /// <param name="reg"></param>
    /// <param name="strText"></param>
    /// <returns></returns>
    public string CD_Callbak(RegexInfo reg, string strText)
    {
        MatchCollection matchs = reg.r.Matches(strText);
        // 没有匹配的直接返回原值
        if (matchs.Count == 0)
            return strText;

        // 先将正则出来的结果进行帅选出来进行细化定位(KEY=开始位置）
        List<Match> result = new List<Match>();
        foreach (Match m in matchs)
        {
            result.Add(m);
        }
        // 将位置将序排下顺序，后面的优先处理
        List<Match> sortList = result.OrderByDescending(p => p.Index).ToList();

        // 主角要处理 
        int nReduceFreezePer = 0;
        if(EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
        {
            // 基础百分比冷却缩减
            nReduceFreezePer = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE);
        }

        StringBuilder builder = new StringBuilder(strText);
        // 将结果中的项目通过指定的函数进行格式化并替换值
        foreach (Match node in sortList)
        {
            if (node.Groups.Count != 3)
                continue;

            string strOldText = node.Groups[0].ToString();
            string strClassID = node.Groups[1].ToString();
            string strFreezeID = node.Groups[2].ToString();
            uint dwClassID = 0;
            uint dwFreezeID = 0;
            if( uint.TryParse(strClassID, out dwClassID) && uint.TryParse(strFreezeID, out dwFreezeID) )
            {
                int nFreezeTime = GameLogicAPI.GetFreezeTime(dwClassID, dwFreezeID, 1);
                
                // 冷却时间*（1-基础百分比冷却缩减)
                float fFreezeTime = (float)nFreezeTime * (10000 - nReduceFreezePer)* 0.0000001f;
                float fFreezeTimeCmp = (float)(int)fFreezeTime;
                string strNewText = fFreezeTime > fFreezeTimeCmp ? fFreezeTime.ToString("0.0") : fFreezeTime.ToString("F0");// + ": nReduceFreezePer=" + nReduceFreezePer.ToString();

                // 本正则出来字符串在源字符串的起始位置
                int nStartPos = node.Index;
                // 正则出来字符串的长度
                int nTextLength = node.Length;
                // 替换
                builder.Replace(strOldText, strNewText, nStartPos, nTextLength);
            }
        }
        return builder.ToString();
    }

    // 获得系数
    public void getRatio(string strText)
    {
        if(string.IsNullOrEmpty(strText))
        {
            return;
        }

        _propertyRatio.Clear();
        //分隔的长字符串
        string[] lPartStr = strText.Split(']');
        //分隔的中字符串
        string mPartStr;
        //分隔的短字符组
        string[] sPartStr;
        foreach (string s in lPartStr)
        {
            if (s.IndexOf('[') >= 0)
            {
                mPartStr = s.Substring(s.IndexOf('[') + 1);
                sPartStr = mPartStr.Split('=');
                string propertyType = sPartStr[0];

                // 拆分系数 附加值
                if (comparePropertyType(propertyType) == false)
                    continue;
                string sRatioStr = sPartStr[1];

                int ratio = 0;
                int.TryParse(sRatioStr,out ratio);
                  ratioAppendInfo ratioInfo = new ratioAppendInfo();
                  ratioInfo.nRatio = ratio;
                if (ratio <= 0)
                {
                    if (!SplitRatioStrByAppendType(sRatioStr, ratioInfo))
                        continue;
                }

                if (!_propertyRatio.ContainsKey(propertyType))
                {
                    _propertyRatio[propertyType] = new List<ratioAppendInfo>();
                }

                _propertyRatio[propertyType].Add(ratioInfo);
            }
            else
            {
                continue;
            }

        }
        return;
    }

    public bool SplitRatioStrByAppendType(string sRatio,ratioAppendInfo ratioInfo)
    {
        string sSymbolsGroup = "+-*/";
        string[] sPartRatioStr;
        for (int i = 0; i < (int)EAppendValueType.EAVT_DIVIDED;++i )
        {
            if(i>=sSymbolsGroup.Length)
            {
                continue;
            }

            char sSymbol = sSymbolsGroup[i];
            sPartRatioStr = sRatio.Split(sSymbol);
            if(sPartRatioStr.Length <=1 )
            {
                continue;
            }

            int.TryParse(sPartRatioStr[0], out ratioInfo.nRatio);
            int.TryParse(sPartRatioStr[1], out ratioInfo.nAddValue);
            ratioInfo.eAddType = (EAppendValueType)(i + 1);
            return true;
        }

        return false;
    }
    // 排除不需要的key
    public bool comparePropertyType(string propertyType)
    {
        if (propertyType != "CD" && 
            propertyType != "CVALUE" && propertyType != "PVALUE" &&
            propertyType != "CPA" && propertyType != "CMA" && propertyType != "CPD" &&
            propertyType != "CMD" && propertyType != "CHP" && propertyType != "CMP" &&
            propertyType != "CSD" && propertyType != "CPASD" && 
            propertyType != "CHPRY" && propertyType != "CMPRY" && propertyType != "PPA" &&
            propertyType != "PMA" && propertyType != "PPD" && propertyType != "PMD" &&
            propertyType != "PHP" && propertyType != "PMP" && propertyType != "PSD" &&
            propertyType != "PPASD" && propertyType != "PCRC" && propertyType != "PHPRY" &&
            propertyType != "PMPRY" && propertyType != "CCOUNT" && 
            propertyType != "PDFA1" && propertyType != "MDFA1" && 
            propertyType != "PASD" &&  propertyType != "CRC"
            )
        {
            return false;
        }

        else
        {
            return true;
        }

    }
    // 获得主英雄的property
    public CreatureProperty getProperty()
    {
        return EntityFactory.MainHeroView.Property;
    }
    // 根据propertyType的key得到相应的属性值
    public float getEntityPropertyNum(string key)
    {
        int nScaleRatio = 100;
        int nScaleRatioEx = 10000;
        switch (key)
        {
            case "CPA":
            case "PA":
            case "PPA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_PA);
                }
            case "CPD":
            case "PD":
            case "PPD":
            case "PDFA1":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_PD);
                }
            case "CMA":
            case "MA":
            case "PMA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MA);
                }
            case "CMD":
            case "MD":
            case "PMD":
            case "MDFA1":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MD);
                }
            case "CHP":
            case "HP":
            case "PHP":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                }
            case "CMP":
            case "MP":
            case "PMP":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);
                }
            case "CCRC":
            case "CRC":
            case "PCRC":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_CRC) / nScaleRatio;
                }
            case "CSD":
            case "SD":
            case "PSD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_SD);
                }
            case "CPASD":
            case "PASD":
            case "PPASD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_AF) / nScaleRatio;
                }
            case "CHPRY":
            case "HPRY":
            case "PHPRY":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_HP_REPLY);
                }
            case "CMPRY":
            case "MPRY":
            case "PMPRY":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MP_REPLY);
                }
            case "LEVEL":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                }
            case "COOL":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE) / nScaleRatio;
                }
            case "MMP":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);
                }
            case "MHP":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                }
            case "BPA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PA) / nScaleRatio;
                }
            case "BMA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_MA) / nScaleRatio;
                }
            case "BPD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PD) / nScaleRatio;
                }
            case "BMD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_MD) / nScaleRatio;
                }
            case "BSD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_SD) / nScaleRatio;
                }
            case "BPASD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_AF) / nScaleRatioEx;
                }
            case "BCRC":
                {
                    return 0;
                }
            case "BCOOL":
                {
                    return 0;
                }
            case "APA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PA) / nScaleRatio;
                }
            case "AMA":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_MA) / nScaleRatio;
                }
            case "APD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PD) / nScaleRatio;
                }
            case "AMD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_MD) / nScaleRatio;
                }
            case "ASD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_SD) / nScaleRatio;
                }
            case "APASD":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_AF) / nScaleRatio - (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_AF) / nScaleRatioEx;
                }
            case "ACRC":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_CRC) / nScaleRatioEx;
                }
            case "ACOOL":
                {
                    return (float)getProperty().GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE) / nScaleRatio;
                }
            default:
                return 0;
        }
    }
    //*********接受消息部分***********//

    // 充能消息
    public void SetChargeCount(cmd_buff_charge_count_change _data)
    {

        UBBProperty.chargeValue_Info nValue_info = new UBBProperty.chargeValue_Info();
        nValue_info.chargeCount = _data.chargeCount;
        nValue_info.chargeValue = _data.nChargeValue;
        int BuffID = _data.nBuffID;
        _dicChargeCount[BuffID] = nValue_info;
    }

    //*********** 回调函数的取值部分***********//
    // 充能固定取值
    public string getConstantChargeString(string key)
    {
        if (EntityFactory.MainHeroView == null || !_propertyRatio.ContainsKey(key))
        {
            return null;
        }

        if( _propertyRatio[key].Count == 0)
        {
            return null;
        }

        int ratio = _propertyRatio[key][0].nRatio;
        _propertyRatio[key].RemoveAt(0);

        if (!_dicChargeCount.ContainsKey(ratio))
        {
            return "0";
        }
        chargeValue_Info valueInfo = _dicChargeCount[ratio];
        int value = valueInfo.chargeValue;
        int ShowNum = value;
        string appendNum = string.Format("{0:D1}", ShowNum);
        return appendNum;
    }
    // 充能百分比取值
    public string getPersnentChargeString(string key)
    {
        if (EntityFactory.MainHeroView == null || !_propertyRatio.ContainsKey(key))
        {
            return null;
        }

        if (_propertyRatio[key].Count == 0)
        {
            return null;
        }

        int ratio = _propertyRatio[key][0].nRatio;
        _propertyRatio[key].RemoveAt(0);

        if (!_dicChargeCount.ContainsKey(ratio))
        {
            return "0";
        }
        chargeValue_Info valueInfo = _dicChargeCount[ratio];
        int value = valueInfo.chargeValue;
        float ShowNum = (float)value * 100 / (getEntityPropertyNum(key) - value);
        string appendNum = string.Format("{0:F1}", ShowNum);
        return appendNum;
    }

    // 特殊公式1 减伤公式
    public string getFormula1PropertyString(string key)
    {
        if (EntityFactory.MainHeroView == null || !_propertyRatio.ContainsKey(key))
        {
            return null;
        }

        if (_propertyRatio[key].Count == 0)
        {
            return null;
        }

        ratioAppendInfo ratioInfo = _propertyRatio[key][0];
        int ratio = ratioInfo.nRatio;
        _propertyRatio[key].RemoveAt(0);
        float value = getEntityPropertyNum(key);
        int ShowNum = (int)value * ratio / 100;
        ShowNum = DoCalculateAppendProperty(ShowNum, ratioInfo.eAddType, ratioInfo.nAddValue);

        float fValue = 100-(float)10000 / (100 + ShowNum);
        string appendNum = fValue.ToString("0");

        return appendNum;
    }

    public int DoCalculateAppendProperty(int nValue,EAppendValueType eType,int nAppendValue)
    {
        int nCalculateVlaue = nValue;
        switch(eType)
        {
            case EAppendValueType.EAVT_ADD:
                {
                    nCalculateVlaue += nAppendValue;
                }
                break;
            case EAppendValueType.EAVT_MINUS:
                {
                    nCalculateVlaue -= nAppendValue;
                }
                break;
            case EAppendValueType.EAVT_TIME:
                {
                    nCalculateVlaue *= nAppendValue;
                }
                break;
            case EAppendValueType.EAVT_DIVIDED:
                {
                    nCalculateVlaue /= nAppendValue;
                }
                break;
                default:
                break;
        }

        return nCalculateVlaue;
    }

    public string CPA(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CMA(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CPD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CMD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CHP(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CMP(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CSD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CPASD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CCRC(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CHPRY(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string CMPRY(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getConstantChargeString(sf.GetMethod().Name);
    }
    public string PPA(RegexInfo reg,string strText)
    {

        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PMA(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PPD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PMD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PHP(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PMP(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PSD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PPASD(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PCRC(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PHPRY(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string PMPRY(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getPersnentChargeString(sf.GetMethod().Name);
    }
    public string CCOUNT(RegexInfo reg,string strText)
    {
        if (EntityFactory.MainHeroView == null || !_propertyRatio.ContainsKey("CCOUNT"))
        {
            return null;
        }

        if (_propertyRatio["CCOUNT"].Count == 0)
        {
            return null;
        }

        int ratio = _propertyRatio["CCOUNT"][0].nRatio;
        _propertyRatio["CCOUNT"].RemoveAt(0);

        if (!_dicChargeCount.ContainsKey(ratio))
        {
            return "0";
        }
        chargeValue_Info valueInfo = _dicChargeCount[ratio];
        int ShowNum = valueInfo.chargeCount;
        string appendNum = string.Format("{0:D1}", ShowNum);
        return appendNum;
    }

    public string PDFA1(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getFormula1PropertyString(sf.GetMethod().Name);
    }

    public string MDFA1(RegexInfo reg,string strText)
    {
        System.Diagnostics.StackFrame sf = new System.Diagnostics.StackFrame(0);
        return getFormula1PropertyString(sf.GetMethod().Name);
    }

    // 充能消息的结构体
    public struct chargeValue_Info
    {
        public int chargeCount;
        public uint uidTarget;
        public int chargeValue;
    }

    // 加减乘除类型
    public enum EAppendValueType
    {
        EAVT_INVALUED,
        EAVT_ADD,
        EAVT_MINUS,
        EAVT_TIME,
        EAVT_DIVIDED ,
    }

    // 充能消息的结构体
    public class ratioAppendInfo
    {
        public int nRatio = 0;
        public EAppendValueType eAddType = EAppendValueType.EAVT_INVALUED;
        public int nAddValue = 0;
    }
}

