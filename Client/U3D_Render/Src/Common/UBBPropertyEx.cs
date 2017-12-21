/*******************************************************************
** 文件名:	UBBPropertyEx.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	16/3/2017
** 版  本:	1.0
** 描  述:	实体属性UBB格式化

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
using ASpeedGame.Data.PropertyPanelConfig;


namespace U3D_Render.Common
{
    /// <summary>
    /// 角色属性UBB
    /// </summary>
    public class UBBPropertyEx
    {
        public delegate string PropEventHanlder(Match match);

        enum RETURN_TYPE
        {
            Int,
            Float
        }
        struct PropNode
        {
            public string key;
            public RETURN_TYPE returnType;
            public PropEventHanlder handler;    // 处理函数为null时将用默认FormatPropertyValue函数，否则用附加的函数处理
            public int precision;               // 显示的精度
        }
        Dictionary<string, PropNode> m_nodeList = new Dictionary<string, PropNode>();

        Dictionary<string, int> m_dicPropertyLevel = new Dictionary<string, int>();


        /// <summary>
        /// 构造函数
        /// </summary>
        public UBBPropertyEx()
        {
            Initialize();
            AttachUBBLogic();
        }

        /// <summary>
        /// 初始化属性UBB
        /// </summary>
        public void Initialize()
        {
            #region 附加物攻[PA=系数]
            AddKeyFunction("PA", RETURN_TYPE.Int, null);
            #endregion

            #region 附加法强[MA=系数]
            AddKeyFunction("MA");
            #endregion

            #region 附加物防[PD=系数]
            AddKeyFunction("PD");
            #endregion

            #region 附加法防[MD=系数]
            AddKeyFunction("MD");
            #endregion

            #region 附加当前血量[HP=系数]
            AddKeyFunction("HP");
            #endregion

            #region 附加当前蓝量[MP=系数]
            AddKeyFunction("MP");
            #endregion

            #region 附加最大血量[MHP=系数]
            AddKeyFunction("MHP");
            #endregion

            #region 附加最大蓝量[MMP=系数]
            AddKeyFunction("MMP");
            #endregion

            #region 附加等级[LEVEL=系数]
            AddKeyFunction("LEVEL");
            #endregion

            #region 附加损失生命[LHP=系数]
            AddKeyFunction("LHP");
            #endregion

            #region 附加损失生命百分比[LSHP=系数]
            AddKeyFunction("LSHP");
            #endregion

            #region 附加损失蓝量[LMP=系数]
            AddKeyFunction("LMP");
            #endregion

            #region 附加损失蓝量百分比[LSMP=系数]
            AddKeyFunction("LSMP");
            #endregion

            #region 附加物防[CRC=系数]
            AddKeyFunction("CRC");
            #endregion

            #region 附加法强[SD=系数]
            AddKeyFunction("SD");
            #endregion

            #region 附加物攻[PASD=系数]
            AddKeyFunction("PASD", RETURN_TYPE.Int);
            #endregion

            #region 附加法防[COOL=系数]
            AddKeyFunction("COOL");
            #endregion

            #region 附加基础物攻[BPA=系数]
            AddKeyFunction("BPA", RETURN_TYPE.Float, precision:1);
            #endregion

            #region 附加基础法强[BMA=系数]
            AddKeyFunction("BMA", RETURN_TYPE.Float, precision:1);
            #endregion

            #region 附加基础物防[BPD=系数]
            AddKeyFunction("BPD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加基础法防[BMD=系数]
            AddKeyFunction("BMD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加基础物防[BCRC=系数]
            AddKeyFunction("BCRC", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加基础法强[BSD=系数]
            AddKeyFunction("BSD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加基础物攻[BPASD=系数]
            AddKeyFunction("BPASD", RETURN_TYPE.Int);
            #endregion

            #region 附加基础法防[BCOOL=系数]
            AddKeyFunction("BCOOL", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外物攻[APA=系数]
            AddKeyFunction("APA", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外法强[AMA=系数]
            AddKeyFunction("AMA", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外物防[APD=系数]
            AddKeyFunction("APD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外法防[AMD=系数]
            AddKeyFunction("AMD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外物防[ACRC=系数]
            AddKeyFunction("ACRC", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外法强[ASD=系数]
            AddKeyFunction("ASD", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 附加额外物攻[APASD=系数]
            AddKeyFunction("APASD", RETURN_TYPE.Int);
            #endregion

            #region 附加额外法防[ACOOL=系数]
            AddKeyFunction("ACOOL", RETURN_TYPE.Float, precision: 1);
            #endregion

            #region 快捷键[SCUT=技能ID]
            AddKeyFunction("SCUT", RETURN_TYPE.Int, SCUT);
            #endregion

            #region 快捷键[SKILLSLOT=槽位ID]
            AddKeyFunction("SKILLSLOT", RETURN_TYPE.Int, SKILLSLOT);
            #endregion

            #region 百分比普攻系数[PAF=系数]
            AddKeyFunction("PAF", RETURN_TYPE.Int);
            #endregion

            #region 攻击速度[_PASD=属性]
            AddKeyFunction("_PASD", RETURN_TYPE.Int, null);

            #region 物理防御穿透[PDP=系数]
            AddKeyFunction("PDP");
            #endregion

            #region 百分比物理防御穿透[PPDP=系数]
            AddKeyFunction("PPDP");
            #endregion

            #region 法术防御穿透[MDP=系数]
            AddKeyFunction("MDP");
            #endregion

            #region 充能百分比法术防御穿透[PMDP=系数]
            AddKeyFunction("PMDP");
            #endregion

            #region 充能百分比物理吸血[PPDR=系数]
            AddKeyFunction("PPDR");
            #endregion

            #region 充能百分比法术吸血[PMDR=系数]
            AddKeyFunction("PMDR");
            #endregion

            #region 充能百分比法术吸血[HPRY=系数]
            AddKeyFunction("HPRY");
            #endregion

            #region 充能百分比法术吸血[MPRY=系数]
            AddKeyFunction("MPRY");
            #endregion

            #endregion
        }

        /// <summary>
        /// 增加属性节点 
        /// </summary>
        /// <param name="key">属性</param>
        /// <param name="handler">处理函数为null时将用默认FormatPropertyValue函数，否则用附加的函数处理</param>
        /// <returns></returns>
        private void AddKeyFunction(string propName, RETURN_TYPE returnType = RETURN_TYPE.Int, PropEventHanlder handler = null, int precision = 0)
        {
            PropNode node = new PropNode();
            node.key = propName;
            node.returnType = returnType;
            node.handler = handler;
            node.precision = precision;
            m_nodeList.Add(propName, node);
        }

        /// <summary>
        /// 将附加到UBB逻辑中
        /// </summary>
        private void AttachUBBLogic()
        {
            // 属性等级解析
            InitDicPropertyLevel();
            UBB.AddReplacementEx(@"\[(PROPLEVEL)\s*=\s*(\d*)([+*\-/]?)(\d*)\]", PropertyLevel_Callback, true);

            // 生成统一的pattern
            string pattern = CreatePattern();
            if (pattern == "")
                return;

            UBB.AddReplacementEx(pattern, Property_Callbak, true);
        }

        private void InitDicPropertyLevel()
        {
            m_dicPropertyLevel["PA"] = 1;
            m_dicPropertyLevel["PD"] = 2;
            m_dicPropertyLevel["PASD"] = 3;
            m_dicPropertyLevel["CRC"] = 4;
            m_dicPropertyLevel["MA"] = 5;
            m_dicPropertyLevel["MD"] = 6;
            m_dicPropertyLevel["COOL"] = 7;
            m_dicPropertyLevel["SD"] = 8;
            m_dicPropertyLevel["PAF"] = 9;
        }
        /// <summary>
        /// 属性等级解析
        /// </summary>
        /// <param name="reg"></param>
        /// <param name="strText"></param>
        /// <returns></returns>
        public string PropertyLevel_Callback(RegexInfo reg, string strText)
        {
            // 取得属性值
            string strPropPattern = @"\[(PA|PD|PASD|CRC|MA|MD|COOL|SD|PAF)\s*=\s*(\d*)([+*\-/]?)(\d*)\]";
            Regex re = new Regex(strPropPattern);
            MatchCollection matchs = re.Matches(strText);
            if (matchs.Count == 0)
                return strText;

            float fPropValue = 0;
            string strKey = "";
            do
            {
                Match node = matchs[0];
                if (node.Groups.Count == 0)
                    break;
                strKey = node.Groups[1].ToString();// 正则键值
                if (!m_nodeList.ContainsKey(strKey))
                    break;
                float fPropVal = getPropertyValue(strKey);

                string strOldValue = node.Groups[2].ToString();// 这个值为20或20*5之类的形式
                float fOldValue = 1.0f;
                float.TryParse(strOldValue, out fOldValue);
                fPropValue = fPropVal * fOldValue / 100.0f;
            } while (false);
            if (String.IsNullOrEmpty(strKey))
                return strText;

            // 取得配置区间值
            if (EntityFactory.MainHeroView == null)
                return strText;

	        int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);

            int nIndex = m_dicPropertyLevel[strKey];

            SSchemePropertyPanel propConfig = PropertyPanelConfig.Instance.GetPropertyPanelConfigList(nHeroID, fPropValue, nIndex);
            if (propConfig == null)
            {
                //Debug.LogError(String.Format("propConfig == null,{0},{1}, {2}", nHeroID, fValue, nPropIndex));
                return strText;
            }

            if (nIndex <= 0)
                return strText;

            // 取得匹配值
            MatchCollection matchLevel = reg.r.Matches(strText);
            if (matchLevel.Count == 0)
                return strText;

            string strNewText = String.Format("<color='#{0}'>（{1}）</color>", propConfig.strColor, propConfig.strDesc);

            Match levelNode = matchLevel[0];
            string strOldText = levelNode.Groups[0].ToString();
            int nStartPos = levelNode.Index;
            int nTextLength = levelNode.Length;

            StringBuilder builder = new StringBuilder(strText);
            // 替换
            builder.Replace(strOldText, strNewText, nStartPos, nTextLength);

            return builder.ToString();
        }

        /// <summary>
        /// 组装Pattern
        /// </summary>
        /// <remarks>\[(MA|PA)\s*=\s*(\d*)([+*\-/]?)(\d*)\]</remarks>
        /// <returns></returns>
        private string CreatePattern()
        {
            if (m_nodeList.Count == 0)
                return "";

            string pattern = @"\[";
            string body = "";
            foreach (KeyValuePair<string, PropNode> node in m_nodeList)
            {
                if (body != "")
                {
                    body += "|";
                }
                else
                {
                    body += "(";
                }
                body += node.Key;
            }
            if (!body.Equals(""))
            {
                body += ")";
            }

            pattern += body;
            pattern += @"\s*=\s*(\d*)([+*\-/]?)(\d*)\]";

            return pattern;
        }

        #region 获取属性对应数值 ****注意：凡是直接取属性的，只在前面加一个下划线前缀*****
        /// <summary>
        /// 获取属性对应数值
        /// </summary>
        /// <param name="propName">属性名称</param>
        /// <returns></returns>
        private float getPropertyValue(string propName)
        {
            // 获得主英雄的property
            if (EntityFactory.MainHeroView == null)
                return .0f;
            CreatureProperty property = EntityFactory.MainHeroView.Property;
            if (property == null)
                return .0f;

            int nScaleRatio = 100;
            int nScaleRatioEx = 10000;
            switch (propName)
            {
                case "CPA":
                case "PA":
                case "PPA":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PA);
                    }
                case "CPD":
                case "PD":
                case "PPD":
                case "PDFA1":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PD);
                    }
                case "CMA":
                case "MA":
                case "PMA":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MA);
                    }
                case "CMD":
                case "MD":
                case "PMD":
                case "MDFA1":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MD);
                    }
                case "CHP":
                case "HP":
                case "PHP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                    }
                case "CMP":
                case "MP":
                case "PMP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);
                    }
                case "CCRC":
                case "CRC":
                case "PCRC":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CRC) / nScaleRatio;
                    }
                case "CSD":
                case "SD":
                case "PSD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SD);
                    }
                case "_PASD":        // 攻击速度(放大ZOOM_IN_MULTIPLE倍)
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PASD) / nScaleRatio;
                    }
                case "CPASD":
                case "PASD":
                case "PPASD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_AF) / nScaleRatio;
                    }
                case "CHPRY":
                case "HPRY":
                case "PHPRY":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP_REPLY);
                    }
                case "CMPRY":
                case "MPRY":
                case "PMPRY":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP_REPLY);
                    }
                case "LEVEL":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                    }
                case "COOL":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE) / nScaleRatio;
                    }
                case "MMP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);
                    }
                case "MHP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                    }
                case "BPA":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PA) / nScaleRatio;
                    }
                case "BMA":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_MA) / nScaleRatio;
                    }
                case "BPD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PD) / nScaleRatio;
                    }
                case "BMD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_MD) / nScaleRatio;
                    }
                case "BSD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_SD) / nScaleRatio;
                    }
                case "BPASD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_AF) / nScaleRatioEx;
                    }
                case "BCRC":
                    {
                        return 0.0f;
                    }
                case "BCOOL":
                    {
                        return 0.0f;
                    }
                case "APA":
                    {
                        float fBasePA = (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PA) / 100.0f; // 基础物理攻击 
                        float fAppendPA = ((float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PA)) / 100.0f; // 附加物理攻击
                        float fAppendPPA = ((float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PCT_PA)) / 10000.0f;// 附加百分比物理攻击
                        //Trace.Log("APA: 基础物理攻击=" + property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_PA).ToString() 
                        //    + ", 附加物理攻击="+ property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PA).ToString()
                        //    + ",附加百分比物理攻击="+ property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PCT_PA).ToString());
                        return (fAppendPA + (fBasePA + fAppendPA) * fAppendPPA);
                    }
                case "AMA":     // AMA = fAppendMA + (fBaseMA + fAppendMA) * fAppendPMA
                    {
                        float fBaseMA = (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_MA); // 基础法术攻 
                        float fAppendMA = ((float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_MA))/100.0f; // 附加法术攻击
                        float fAppendPMA = ((float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PCT_MA))/10000.0f;// 附加百分比法术攻
                        return (fAppendMA + (fBaseMA + fAppendMA) * fAppendPMA);
                    }
                case "APD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PD) / nScaleRatio;
                    }
                case "AMD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_MD) / nScaleRatio;
                    }
                case "ASD":
                    {
                        float fCurentValue = (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SD);
                        float fBaseSD = (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_SD) / nScaleRatio;
                        return fCurentValue - fBaseSD;
                    }
                case "APASD":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_AF) / nScaleRatio - (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_BASE_AF) / nScaleRatioEx;
                    }
                case "ACRC":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_CRC) / nScaleRatioEx;
                    }
                case "ACOOL":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE) / nScaleRatio;
                    }
                case "PAF":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_AF);
                    }
                case "PDP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PDP);
                    }
                case "PPDP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PCT_PDP) / nScaleRatio;
                    }
                case "MDP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MDP);
                    }
                case "PMDP":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_APPEND_PCT_MDP) / nScaleRatio;
                    }
                case "PPDR":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_PDRAINS) / nScaleRatio;
                    }
                case "PMDR":
                    {
                        return (float)property.GetNumProp(ENTITY_PROPERTY.PROPERTY_PCT_MDRAINS) / nScaleRatio;
                    }
                default:
                    return 0.0f;
            }
        }
        #endregion


        /// <summary>
        /// 属性格式化
        /// </summary>
        /// <param name="reg"></param>
        /// <param name="strText"></param>
        /// <returns></returns>
        public string Property_Callbak(RegexInfo reg, string strText)
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

            StringBuilder builder = new StringBuilder(strText);
            // 将结果中的项目通过指定的函数进行格式化并替换值
            foreach (Match node in sortList)
            {
                if (node.Groups.Count == 0)
                    continue;

                // 获取到键值
                string strKey = node.Groups[1].ToString();
                // 对象容器中没有注册此键值
                if (!m_nodeList.ContainsKey(strKey))
                    continue;

                // 正则出来字符串
                string strOldText = node.Groups[0].ToString();

                PropNode prop = m_nodeList[strKey];
                string strNewText = "";
                // 处理值
                if (prop.handler != null)
                    strNewText = prop.handler(node);
                else
                    strNewText = FormatPropertyValue(node, prop);

                // 有变化将进行替换
                if (!strNewText.Equals(strOldText))
                {
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


        /// <summary>
        /// 格式化属性值 
        /// </summary>
        /// <param name="match"></param>
        /// <returns></returns>
        private string FormatPropertyValue(Match match, PropNode propNode)
        {
            if (match == null)
                return "";

            // 正则键值
            string strKeyText = match.Groups[1].ToString();

            // 这个值为20或20*5之类的形式
            string strOldValue1 = match.Groups[2].ToString();
            float foldValue = 1.0f;
            float.TryParse(strOldValue1, out foldValue);
            // 获取属性值
            float fPropVal = getPropertyValue(strKeyText);

            // 取得属性转换值
            float fShowNum = fPropVal * foldValue / 100.0f;
            // 处理附加数值，进行合并
            string strOpt = match.Groups[3].ToString();
            if (!string.IsNullOrEmpty(strOpt))
            {
                // 附加数值
                string strOldValue2 = match.Groups[4].ToString();
                float addition = 0.0f;
                if (float.TryParse(strOldValue2, out addition))
                {
                    // 进行附加数值运算
                    switch (strOpt)
                    {
                        case "+":
                            {
                                fShowNum += addition;
                            }
                            break;
                        case "-":
                            {
                                fShowNum -= addition;
                            }
                            break;
                        case "*":
                            {
                                fShowNum *= addition;
                            }
                            break;
                        case "/":
                            {
                                // 处理0，除数不能为0
                                if (addition >= -float.Epsilon && addition <= float.Epsilon)
                                {
                                    addition = 1.0f;
                                }
                                fShowNum /= addition;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            return propNode.returnType == RETURN_TYPE.Int ? string.Format("{0:D1}", (int)fShowNum) : string.Format("{0:F" + propNode.precision + "}", fShowNum);
        }


        /////////////////////////////////////////////////////////////////////////////
        public string SCUT(Match match) 
        {
            string strValue = match.Groups[2].ToString();
            int nSpellID = 0;
            if (!int.TryParse(strValue, out nSpellID))
                return "";

            // 获取槽位序号
            int nSlotIndex = GameLogicAPI.GetSlotIndexOfSpell(nSpellID);
            if (nSlotIndex == -1)
                return "";

            // 获取快捷键
            return ASpeedGame.Data.SlotKeyCodeConfig.SlotKeyCodeConfig.Instance.GetSlotKeyCode(ASpeedGame.Data.SlotKeyCodeConfig.SlotType.Skill, nSlotIndex).ToUpper();
        }

        public string SKILLSLOT(Match match)
        {
            string strValue = match.Groups[2].ToString();
            int nSkillSlotIndex = 0;
            if (!int.TryParse(strValue, out nSkillSlotIndex))
                return "";

            // 获取快捷键
            return ASpeedGame.Data.SlotKeyCodeConfig.SlotKeyCodeConfig.Instance.GetSlotKeyCode(ASpeedGame.Data.SlotKeyCodeConfig.SlotType.Skill, nSkillSlotIndex).ToUpper();
        }
    }

}