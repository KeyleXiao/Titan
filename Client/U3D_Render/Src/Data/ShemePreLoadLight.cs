/*******************************************************************
** 文件名: SchemePreLoadLight.cs
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 7/5/2017
** 版  本: 1.0
** 描  述: 预加载光效配置 PreLoadLight.csv

    //地图ID	光效类型（1人皮肤，2怪ID，3XP，4其他）	对象ID	光效ID	是否启用（1启用）	标识（预留列，暂时无用）
    //int	    int	                                    int	    string	int	                int           
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace ASpeedGame.Data.Scheme
{

    /// <summary>
    /// 光效类型
    /// </summary>
    public enum LightType
    {
        Unknown,                        // 未知类型
        ActorSkin,                      // 人物皮肤
        Monster,                        // 怪物
        XP,                             // XP技能
        Other,                          // 其他
        Max,                            // 最大
    }

    /// <summary>
    /// 目标对象所对应的光效列表
    /// </summary>
    public class ObjectLightNode
    {
        int m_nObjID;              // 对象ID
        public int ObjID
        {
            get { return m_nObjID; }
        }
        List<int> m_lightIDList;   // 光效ID列表
        public List<int> Collection
        {
            get
            {
                return m_lightIDList;
            }
        }
        int m_nFlag;               // 标识
        public int Flag
        {
            get { return m_nFlag; }
            set { m_nFlag = value; }
        }

        public ObjectLightNode(int nObjID)
        {
            m_nObjID = nObjID;
            m_nFlag = 0;
            m_lightIDList = new List<int>();
        }

        public void Add(int nLightID)
        {
            if (m_lightIDList.Contains(nLightID)) 
                return;
            m_lightIDList.Add(nLightID);
        }

        public void Add(List<int> newlightIDList)
        {
            foreach (int lightID in newlightIDList)
            {
                Add(lightID);
            }
        }
    }


    /// <summary>
    /// 某类性下的所有目标对象列表
    /// </summary>
    public class TypeLightNode
    {
        int m_nType;               // 光效类型
        public int Type
        {
            get { return m_nType; }
        }
        /// <summary>
        /// 目标对象ID为键值
        /// </summary>
        Dictionary<int, ObjectLightNode>  m_objectList;
        public Dictionary<int, ObjectLightNode> Collection
        {
            get{
                return m_objectList;
            }
        }


        public TypeLightNode(int nType)
        {
            m_nType = nType;
            m_objectList = new Dictionary<int,ObjectLightNode>();
        }

        /// <summary>
        /// 增加目标对象节点
        /// </summary>
        /// <param name="node"></param>
        public void Add(ObjectLightNode node)
        {
            if (m_objectList.ContainsKey(node.ObjID))
            {
                return;
            }

            m_objectList[node.ObjID] = node;
        }

        /// <summary>
        /// 获取目标对象所对应数据
        /// </summary>
        /// <param name="nObjID"></param>
        /// <returns></returns>
        public ObjectLightNode get(int nObjID)
        {
            if (!m_objectList.ContainsKey(nObjID))
            {
                return null;
            }

            return m_objectList[nObjID];
        }
    }

    /// <summary>
    /// 地图下各类型所对应的光效ID列表
    /// </summary>
    public class MapLightNode
    {
        /// <summary>
        /// 地图ID
        /// </summary>
        public int nMapID;

        /// <summary>
        /// 类型ID为键值
        /// </summary>
        public Dictionary<int, TypeLightNode> dictTypeLightList = null;

        public MapLightNode()
        {
            nMapID = 0;
            dictTypeLightList = new Dictionary<int, TypeLightNode>();
        }

        /// <summary>
        /// 增加某类型下的目标对象数据
        /// </summary>
        /// <param name="nType"></param>
        /// <param name="node"></param>
        public void Add(int nType, ref ObjectLightNode node)
        {
            if (dictTypeLightList.ContainsKey(nType))
            {
                dictTypeLightList[nType].Add(node);
            }
            else 
            {
                TypeLightNode current = new TypeLightNode(nType);
                current.Add(node);

                dictTypeLightList.Add(nType, current);
            }
        }

        /// <summary>
        /// 获取目标对象所对应数据
        /// </summary>
        /// <param name="nType"></param>
        /// <param name="nObjID"></param>
        /// <returns></returns>
        public ObjectLightNode get(ASpeedGame.Data.Scheme.LightType nType, int nObjID)
        {
            int nCurType = (int)nType;
            if (!dictTypeLightList.ContainsKey(nCurType))
            {
                return null;
            }

            return dictTypeLightList[nCurType].get(nObjID);
        }

        /// <summary>
        /// 获取某类所对应的全部光效
        /// </summary>
        /// <param name="nType"></param>
        /// <returns></returns>
        public TypeLightNode get(ASpeedGame.Data.Scheme.LightType nType)
        {
            int nCurType = (int)nType;
            if (!dictTypeLightList.ContainsKey(nCurType))
            {
                return null;
            }

            return dictTypeLightList[nCurType];
        }
    }

    /// <summary>
    /// 玩家设置本地Slot配置
    /// </summary>
    public class SchemePreLoadLight
    {
        /// <summary>
        /// 配置列
        /// </summary>
        private enum Column 
        {
            COL_MAP,                // 地图ID
            COL_TYPE,               // 光效类型LightType 
            COL_OBJECT_ID,          // 对象ID 
            COL_LIGHT_ID1,          // 光效ID1 
            COL_LIGHT_ID2,          // 光效ID2
            COL_LIGHT_ID3,          // 光效ID3
            COL_LIGHT_ID4,          // 光效ID4
            COL_OPEN,               // 是否启用（1启用） 
            COL_FLAG,               // 标识
            COL_MAX,                // 最大列
        }
        /// <summary>
        /// 配置文件名称
        /// </summary>
        public static readonly string SCHEME_FILENAME = "PreLoadLight.csv";

        /// <summary>
        /// MapID为键值 
        /// </summary>
        private Dictionary<int, MapLightNode> m_dictMapLightList = null; 

        private static SchemePreLoadLight m_instance = new SchemePreLoadLight();
        public static SchemePreLoadLight Instance
        {
            get
            {
                return m_instance;   
            }
        }


        SchemePreLoadLight()
        {
            m_dictMapLightList = new Dictionary<int, MapLightNode>();
        }

        /// <summary>
        /// 卸载预加载光效配置
        /// </summary>
        public void UnLoad()
        {
            m_dictMapLightList.Clear(); 
        }

        /// <summary>
        /// 加载配置文件
        /// </summary>
        /// <param name="stPath"></param>
        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read sheme fail! file=" + strFilePath);
                return;
            }
            m_dictMapLightList.Clear();

            char [] splitChars = new char[] { ';' };

            int nRecordCount = reader.GetRecordCount();
            for (int row = 0; row < nRecordCount; row++)
            {
                // 是否启用（1启用） 
                int nOpen = reader.GetInt(row, (int)Column.COL_OPEN, 0);
                if (nOpen == 0)
                {
                    continue;
                }
                // 光效类型LightType 
                int nType = reader.GetInt(row, (int)Column.COL_TYPE, 0);
                if (nType < (int)LightType.Unknown || nType >= (int)LightType.Max)
                {
                    continue;
                }
                // 地图ID
                int nMapID = reader.GetInt(row, (int)Column.COL_MAP, 0);


                // 对象ID 
                string strObjIDList = reader.GetString(row, (int)Column.COL_OBJECT_ID, "").TrimEnd(splitChars).TrimStart(splitChars);
                if (string.IsNullOrEmpty(strObjIDList))
                {
                    continue;
                }

                try
                {
                    var objIDs = new List<int>(Array.ConvertAll(strObjIDList.Split(';'), int.Parse));
                    foreach (int nObjID in objIDs)
                    {
                        ObjectLightNode node = new ObjectLightNode(nObjID);

                        // 光效ID列共多列
                        for (Column i = Column.COL_LIGHT_ID1; i < Column.COL_OPEN; ++i)
                        {
                            string strLightIDList = reader.GetString(row, (int)i, "").TrimEnd(splitChars).TrimStart(splitChars);
                            if (string.IsNullOrEmpty(strLightIDList))
                            {
                                continue;
                            }

                            var lights = new List<int>(Array.ConvertAll(strLightIDList.Split(';'), int.Parse));
                            node.Add(lights);
                        }
                        // 标识
                        node.Flag = reader.GetInt(row, (int)Column.COL_FLAG, 0);

                        if (!m_dictMapLightList.ContainsKey(nMapID))
                        {
                            MapLightNode map = new MapLightNode();
                            map.nMapID = nMapID;

                            m_dictMapLightList.Add(nMapID, map);
                        }

                        m_dictMapLightList[nMapID].Add(nType, ref node);
                    }
                }
                catch (Exception ex)
                {
                    UnityEngine.Debug.LogError("Row=" + (row + 1).ToString() + ", error=" + ex.ToString());
                }
            }
        }

        /// <summary>
        /// 获取目标对象所对应数据
        /// </summary>
        /// <param name="nMapID"></param>
        /// <param name="nType"></param>
        /// <param name="nObjID"></param>
        /// <returns></returns>
        public ObjectLightNode get(int nMapID, ASpeedGame.Data.Scheme.LightType nType, int nObjID)
        {
            EntityUtil.formatMapID(ref nMapID);

            if (!m_dictMapLightList.ContainsKey(nMapID))
            {
                return null;
            }

            return m_dictMapLightList[nMapID].get(nType, nObjID);
        }

        /// <summary>
        /// 获取某一类所对应全部光效
        /// </summary>
        /// <param name="nMapID"></param>
        /// <param name="nType"></param>
        /// <returns></returns>
        public TypeLightNode get(int nMapID, ASpeedGame.Data.Scheme.LightType nType)
        {
            EntityUtil.formatMapID(ref nMapID);

            if (!m_dictMapLightList.ContainsKey(nMapID))
            {
                return null;
            }

            return m_dictMapLightList[nMapID].get(nType);
        }

    }
}
