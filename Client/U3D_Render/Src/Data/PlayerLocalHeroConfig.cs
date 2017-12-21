/*******************************************************************
** 文件名:	PlayerLocalHeroConfig.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	林建聪
** 日  期:	2017/6/19
** 版  本:	1.0
** 描  述:	英雄数据会被玩家修改的配置。
** 应  用:  
 *          
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using DataCenter;

namespace Data.PlayerLocalHeroConfig
{
    public enum ELocalHero
    {
        ELH_HeroID,
        ELH_DeSkinID,
        ELH_GemPageID,
        ELH_SummonID,
        ELH_GoodsDispositionID,
    }
    public class LocalHeroData
    {
        public int nHeroID;
        /// <summary>
        /// 什么鬼是默认皮肤ID？
        /// </summary>
        public int nDeSkinID;
        /// <summary>
        /// 英雄宝石页
        /// </summary>
        public int nGemPageID;
        /// <summary>
        /// 英雄召唤技能ID
        /// </summary>
        public int nSummonID;
        /// <summary>
        /// 英雄战场内选取的推荐ID;
        /// </summary>
        public int nGoodsDispositionID;
    }

    /// <summary>
    /// 玩家设置本地配置
    /// </summary>
    public class PlayerLocalHeroConfig:Singleton<PlayerLocalHeroConfig>, IModelConfig
    {
        private Dictionary<int, LocalHeroData> m_SkinDataMap = new Dictionary<int, LocalHeroData>();
        private Dictionary<int, LocalHeroData> m_SkinScpDataMap = new Dictionary<int, LocalHeroData>();
        //配置表路径
        private string m_FilePath;

        public void Load(string stPath)
        {
            InitPlayerLocalHeroCsv(ref m_SkinScpDataMap, ViewConfigManager.GetConfigFullPath("PlayerLocalHeroConfig.csv", true),true);
        }

        public void AccountLoad()
        {
            m_FilePath = ViewConfigManager.GetAccountConfigFullPath("PlayerLocalHeroConfig.csv");
            InitPlayerLocalHeroCsv(ref m_SkinDataMap, m_FilePath, false);
        }

        public void AccountUnload()
        {
            m_SkinDataMap.Clear();
        }

        public void UnLoad()
        {
            ExportData();


            m_SkinDataMap.Clear();
            m_SkinDataMap = null;
            m_SkinScpDataMap.Clear();
            m_SkinScpDataMap = null;
        }

        #region 新增属性时需要修改的
        private void InitPlayerLocalHeroCsv(ref Dictionary<int, LocalHeroData> _dataDiv, string _strPath,bool _bScp)
        {
            if (_bScp == false && !File.Exists(_strPath))
                return;

            ScpReader reader = new ScpReader(_strPath, true, 2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                //读取表中记录的数据类型(HeroSettingDataType)
                LocalHeroData data = new LocalHeroData();
                data.nHeroID = reader.GetInt(i, (int)ELocalHero.ELH_HeroID, 0);
                data.nDeSkinID = reader.GetInt(i, (int)ELocalHero.ELH_DeSkinID, 0);
                data.nGoodsDispositionID = reader.GetInt(i, (int)ELocalHero.ELH_GoodsDispositionID, 0);
                data.nGemPageID = reader.GetInt(i, (int)ELocalHero.ELH_GemPageID, 0);
                data.nSummonID = reader.GetInt(i, (int)ELocalHero.ELH_SummonID, 0);

                _dataDiv[data.nHeroID] = data;
            }
        }
        private int GetDataIntProperty(ELocalHero _eDataType, LocalHeroData _data)
        {
            switch (_eDataType)
            {
                case ELocalHero.ELH_HeroID:
                    return _data.nHeroID;
                case ELocalHero.ELH_DeSkinID:
                    return _data.nDeSkinID;
                case ELocalHero.ELH_GoodsDispositionID:
                    return _data.nGoodsDispositionID;
                case ELocalHero.ELH_GemPageID:
                    return _data.nGemPageID;
                case ELocalHero.ELH_SummonID:
                    return _data.nSummonID;
                default:
                    return 0;
            }
        }
        private void SetDataIntProperty(ELocalHero _eDataType, ref LocalHeroData _data, int _nPropertyData)
        {
            switch (_eDataType)
            {
                case ELocalHero.ELH_DeSkinID:
                    {
                        _data.nDeSkinID = _nPropertyData;
                    }
                    break;
                case ELocalHero.ELH_GoodsDispositionID:
                    {
                        _data.nGoodsDispositionID = _nPropertyData;
                    }
                    break;
                case ELocalHero.ELH_GemPageID:
                    {
                        if(_nPropertyData > 0)
                            _data.nGemPageID = _nPropertyData;
                    }
                    break;
                case ELocalHero.ELH_SummonID:
                    {
                        if(_nPropertyData > 0)
                            _data.nSummonID = _nPropertyData;
                    }
                    break;
                default:
                    break;
            }
        }
        #endregion


        private void ExportData()
        {
            UpdateUserDataWithScp();

            if (string.IsNullOrEmpty(m_FilePath))
                return;
            
            StreamWriter writer = new StreamWriter(m_FilePath, false, new System.Text.UTF8Encoding(true));
            string s = "";

            s = "英雄ID" + "," + "默认皮肤ID" + "," + "宝石页ID" + "," + "召唤师技能ID" + "," + "战场推荐配置ID";
            writer.WriteLine(s);
            s = "int" + "," + "int" + "," + "int" + "," + "int" + "," + "int";
            writer.WriteLine(s);
            foreach (LocalHeroData data in m_SkinDataMap.Values)
            {
                s = data.nHeroID.ToString() + "," + data.nDeSkinID + "," + data.nGemPageID + "," + data.nSummonID + "," + data.nGoodsDispositionID;

                writer.WriteLine(s);
            }
            writer.Flush();
            writer.Close();
            writer = null;
        }

        private void UpdateUserDataWithScp()
        {
            List<int> removeList = new List<int>();
            foreach (LocalHeroData data in m_SkinDataMap.Values)
            {
                if (m_SkinScpDataMap.ContainsKey(data.nHeroID))
                    continue;

                removeList.Add(data.nHeroID);

            }

            foreach (int nHeroID in removeList)
            {
                m_SkinDataMap.Remove(nHeroID);
            }

            foreach (LocalHeroData data in m_SkinScpDataMap.Values)
            {
                if (m_SkinDataMap.ContainsKey(data.nHeroID))
                    continue;

                m_SkinDataMap[data.nHeroID] = data;
            }
        }

        public bool GetIntData(ELocalHero _eDataType, int _nHeroID, out int _nDataResult,bool _bIsDefault = false)
        {
            _nDataResult = -1;
            if (_nHeroID <= 0)
            {
                
                return false;
            }
                

            bool result = false;
            LocalHeroData data = null;

            result = GetData(_nHeroID, out data, _bIsDefault);

            if (result)
            {
                _nDataResult = GetDataIntProperty(_eDataType, data);
            }

            return result;
        }
        public bool SetIntData(ELocalHero _eDataType,int _nHeroID,int _nDataResult)
        {
            if(_nHeroID <=0)
            {
                return false;
            }

            bool result = false;
            LocalHeroData data = null;

            result = GetData(_nHeroID, out data);
            if(result)
            {
                SetDataIntProperty(_eDataType,ref data, _nDataResult);
                SetData(_nHeroID,ref data);
            }

            return result;
        }

        private bool GetData(int _nHeroID, out LocalHeroData _nDataResult, bool _bIsDefault = false)
        {
            bool result = true;

            Dictionary<int, LocalHeroData> tmpDic = _bIsDefault ? m_SkinScpDataMap : m_SkinDataMap;

            //新客户端，user目录下没有临时文件，那就用默认配置初始化一下。
            if(m_SkinDataMap.Count != m_SkinScpDataMap.Count)
            {
                UpdateUserDataWithScp();
            }

            if (!tmpDic.TryGetValue(_nHeroID, out _nDataResult))
            {
                Trace.LogWarning("取不到英雄ID：" + _nHeroID + " 的LocalHeroData数据");
                result = false;
            }

            return result;
        }
        private bool SetData(int _nHeroID,ref LocalHeroData _nDataResult)
        {
            bool result = false;

            if (m_SkinDataMap.ContainsKey(_nHeroID))
            {
                m_SkinDataMap[_nHeroID] = _nDataResult;
                result = true;
            }
            else
            {
                //新客户端，user目录下没有临时文件，那就用默认配置初始化一下。
                if (m_SkinDataMap.Count <= 0)
                {
                    UpdateUserDataWithScp();
                }
                else
                Trace.LogWarning("取不到英雄ID：" + _nHeroID + " 的LocalHeroData数据");
            }


            return result;

        }
    }
}
