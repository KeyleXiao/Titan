/*******************************************************************
** 文件名:	SchemeOBControl.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB操作配置信息
** 应  用:  
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace ASpeedGame.Data.Scheme
{
    public class SchemeOBControl
    {
        // Group * keyCodeEachTypeMax + Index
        //OB操作快捷键类型
        public enum EOBCtrlKeyCodeType : int 
        {
            EOBCtrl_view_blue = 100,     //观察双阵营  
            EOBCtrl_view_red,
            EOBCtrl_view_all,
            EOBCtrl_player_camp1_1 = 200,//观察蓝方1号
            EOBCtrl_player_camp1_2,
            EOBCtrl_player_camp1_3,
            EOBCtrl_player_camp1_4,
            EOBCtrl_player_camp1_5,
            EOBCtrl_player_camp2_1 = 205, //观察红方1号
            EOBCtrl_player_camp2_2,
            EOBCtrl_player_camp2_3,
            EOBCtrl_player_camp2_4,
            EOBCtrl_player_camp2_5,
            EOBCtrl_lock_nearbyHero = 210,//锁定最近英雄
        }

        //OB操作参数类型
        public enum EOBCtrlParamType : int
        {
            EOBCtrlParam_MouseScroll_Speed, //鼠标滚动速度
            EOBCtrlParam_CameraMove_Speed,  //相机移动速度
        }

        private enum Column_OBCtrl : int
        {
            COL_TYPE,
            COL_GROUP,
            COL_INDEX,
            COL_LABEL,
            COL_KEYCODE,
            COL_DEFAULT,
            COL_MIN,
            COL_MAX,
        }

        public class SchemeOBCtrlKeyCodeNode
        {
            public int nGroup;
            public int nIndex;
            public string szLabel;
            public string szKeyCode;
            public List<UnityEngine.KeyCode[]> keyCodeList = new List<UnityEngine.KeyCode[]>();

            public SchemeOBCtrlKeyCodeNode(int _group, int _index, string _label, string _keycode)
            {
                nGroup = _group;
                nIndex = _index;
                szLabel = _label;
                szKeyCode = _keycode;

                string[] codeArry = _keycode.Split(';');
                foreach(string szCode in codeArry)
                {
                    if (string.IsNullOrEmpty(szCode))
                        continue;

                    UnityEngine.KeyCode[] codes = InputConvertHelper.String2KeyCodeArry(szCode);
                    if(codes != null && codes.Length > 0)
                    {
                        keyCodeList.Add(codes);
                    }
                }
            }
        }

        public class SchemeOBCtrlParamNode
        {
            public int nIndex;
            public float fDefaultValue;
            public float fMinValue;
            public float fMaxValue;

            public SchemeOBCtrlParamNode(int _index, float _default, float _min, float _max)
            {
                nIndex = _index;
                fDefaultValue = _default;
                fMinValue = _min;
                fMaxValue = _max;
            }
        }

        public Dictionary<int, List<SchemeOBCtrlKeyCodeNode>> SchemeOBCtrlKeyCodeNodeMap
        {
            get { return m_dicOBCtrlKeyCodeNodeMap; }
        }

        public static readonly string schemeFileName = "OBCtrlConfig.csv";
        public static readonly int keyCodeEachTypeMax = 100;        //每种类型键位最大个数

        //OB操作键位节点列表
        private Dictionary<int, List<SchemeOBCtrlKeyCodeNode>> m_dicOBCtrlKeyCodeNodeMap = null;
        //OB操作索引—键位对照表
        private Dictionary<int, List<UnityEngine.KeyCode[]>> m_dicOBCtrlKeyCodeMap = null;
        //OB操作参数节点列表
        private Dictionary<int, SchemeOBCtrlParamNode> m_dicOBCtrlParamNodeMap = null;

        private static SchemeOBControl m_instance = new SchemeOBControl();
        public static SchemeOBControl Instance
        {
            get { return m_instance; }
        }

        SchemeOBControl()
        {
            m_dicOBCtrlKeyCodeNodeMap = new Dictionary<int, List<SchemeOBCtrlKeyCodeNode>>();
            m_dicOBCtrlParamNodeMap = new Dictionary<int, SchemeOBCtrlParamNode>();
            m_dicOBCtrlKeyCodeMap = new Dictionary<int, List<UnityEngine.KeyCode[]>>();
        }

        public void UnLoad()
        {
            m_dicOBCtrlKeyCodeNodeMap.Clear();
            m_dicOBCtrlKeyCodeNodeMap = null;
            m_dicOBCtrlParamNodeMap.Clear();
            m_dicOBCtrlParamNodeMap = null;
            m_dicOBCtrlKeyCodeMap.Clear();
            m_dicOBCtrlKeyCodeMap = null;
        }

        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if(reader == null)
            {
                Trace.LogError("read scheme fail! file:" + strFilePath);
                return;
            }

            m_dicOBCtrlKeyCodeNodeMap.Clear();
            m_dicOBCtrlParamNodeMap.Clear();
            int nCount = reader.GetRecordCount();
            for(int row = 0; row < nCount; row++)
            {
                int nType = reader.GetInt(row, (int)Column_OBCtrl.COL_TYPE, 0);
                if (nType == 1)
                {
                    LoadKeyCodeNode(reader, row);
                }
                else if (nType == 2)
                {
                    LoadParamNode(reader, row);
                }
                else
                    Trace.LogError("invalid type! row = " + row + "file:" + strFilePath);
            }
        }

        /// <summary>
        /// 获取快捷键组合列表
        /// </summary>
        /// <param name="keycodeType">键位类型</param>
        /// <param name="keycodeList">快捷键组合列表</param>
        /// <returns></returns>
        public bool GetKeyCodeList(EOBCtrlKeyCodeType keycodeType, out List<UnityEngine.KeyCode[]> keycodeList)
        {
            if (m_dicOBCtrlKeyCodeMap.TryGetValue((int)keycodeType, out keycodeList))
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// 获取OB参数节点
        /// </summary>
        /// <param name="paramType"></param>
        /// <returns></returns>
        public SchemeOBCtrlParamNode GetOBCtrlParamNode(EOBCtrlParamType paramType)
        {
            if (m_dicOBCtrlParamNodeMap.ContainsKey((int)paramType))
            {
                return m_dicOBCtrlParamNodeMap[(int)paramType];
            }
            return null;
        }

        //加载快捷键信息
        private void LoadKeyCodeNode(ScpReader reader, int row)
        {
            int nGroup = reader.GetInt(row, (int)Column_OBCtrl.COL_GROUP, 0);
            int nIndex = reader.GetInt(row, (int)Column_OBCtrl.COL_INDEX, 0);

            List<SchemeOBCtrlKeyCodeNode> nodeList;
            if (m_dicOBCtrlKeyCodeNodeMap.TryGetValue(nGroup, out nodeList))
            {
                int tempIndex = nodeList.FindIndex(x => x.nIndex == nIndex);
                if (tempIndex > -1)
                {
                    Trace.Log("scheme repeat! row = " + row + "file:" + schemeFileName);
                    return;
                }
            }

            string szLabel = reader.GetString(row, (int)Column_OBCtrl.COL_LABEL, "");
            string szKeyCode = reader.GetString(row, (int)Column_OBCtrl.COL_KEYCODE, "");

            if (!m_dicOBCtrlKeyCodeNodeMap.ContainsKey(nGroup))
            {
                m_dicOBCtrlKeyCodeNodeMap.Add(nGroup, new List<SchemeOBCtrlKeyCodeNode>());
            }

            SchemeOBCtrlKeyCodeNode node = new SchemeOBCtrlKeyCodeNode(nGroup, nIndex, szLabel, szKeyCode);
            m_dicOBCtrlKeyCodeNodeMap[nGroup].Add(node);

            List<UnityEngine.KeyCode[]> keyCodeList = new List<UnityEngine.KeyCode[]>();
            keyCodeList.AddRange(node.keyCodeList);
            m_dicOBCtrlKeyCodeMap.Add(nGroup * keyCodeEachTypeMax + nIndex, keyCodeList);
        }

        //加载参数信息
        private void LoadParamNode(ScpReader reader, int row)
        {
            int nIndex = reader.GetInt(row, (int)Column_OBCtrl.COL_INDEX, 0);

            if (m_dicOBCtrlParamNodeMap.ContainsKey(nIndex))
            {
                Trace.Log("scheme repeat! row = " + row + "file:" + schemeFileName);
                return;
            }

            float fDefault = reader.GetFloat(row, (int)Column_OBCtrl.COL_DEFAULT, 0f);
            float fMin = reader.GetFloat(row, (int)Column_OBCtrl.COL_MIN, 0f);
            float fMax = reader.GetFloat(row, (int)Column_OBCtrl.COL_MAX, 0f);

            SchemeOBCtrlParamNode node = new SchemeOBCtrlParamNode(nIndex, fDefault, fMin, fMax);
            m_dicOBCtrlParamNodeMap.Add(nIndex, node);
        }
    }
}
