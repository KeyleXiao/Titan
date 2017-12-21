using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Data.UISoundConfig
{
    public class SSchemeUSoundData:IModelConfigData<SSchemeUSoundData>
    {
        public string USoundFilter;
        public string USoundKey;
        public int USoundID;

        public void CopyTo(ref SSchemeUSoundData _tTargetValue)
        {
            _tTargetValue.USoundFilter = USoundFilter;
            _tTargetValue.USoundKey = USoundKey;
            _tTargetValue.USoundID = USoundID;
        }
    }

    public class UISoundConfig : Singleton<UISoundConfig>, IModelConfig
    {
        public const string ConfigName = "UISoundConfig.csv";
        protected Dictionary<string, SSchemeUSoundData> m_dicUSoundTable;

        public bool IsConfigLoad = false;

        protected char FilterKeyConnect = '_';
        private StringBuilder sb_USoundName = new StringBuilder();

        enum EUSoundConfig
        {
            USC_Filter,
            USC_Key,
            USC_ID,
        }

        public void Load(string strPath)
        {
            m_dicUSoundTable = new Dictionary<string, SSchemeUSoundData>();
            ScpReader reader = new ScpReader(strPath, true, 1);
            sb_USoundName = new StringBuilder();

            string tmpCurrentFilter = string.Empty;
            string tmpSoundName = string.Empty;

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeUSoundData sConfig = new SSchemeUSoundData();

                sConfig.USoundFilter = reader.GetString(i, (int)EUSoundConfig.USC_Filter, tmpCurrentFilter);

                if(i==0 && sConfig.USoundFilter == string.Empty)
                {
                    Trace.LogError("配置表出错!!第一行Filter不能是空的。");
                    break;
                }
                //表当前位置与上一个位置的ViewFilter一致
                else if (sConfig.USoundFilter == string.Empty)
                {
                    sConfig.USoundFilter = tmpCurrentFilter;
                }
                //表当前位置属于另一个ViewFilter
                else if (sConfig.USoundFilter != tmpCurrentFilter)
                {
                    tmpCurrentFilter = sConfig.USoundFilter;
                }

                sConfig.USoundKey = reader.GetString(i, (int)EUSoundConfig.USC_Key, string.Empty);

                sConfig.USoundID = reader.GetInt(i, (int)EUSoundConfig.USC_ID, 0);

                GetSoundName(sConfig.USoundFilter, sConfig.USoundKey,ref tmpSoundName);

                if (m_dicUSoundTable.ContainsKey(tmpSoundName))
                {
                    Trace.LogWarning("有相同的音效名称：" + sConfig.USoundFilter + ","+ sConfig.USoundKey + "表第" + i + "行");
                }
                else
                    m_dicUSoundTable.Add(tmpSoundName, sConfig);
            }
            reader.Dispose();
            reader = null;
            IsConfigLoad = true;
        }

        public void UnLoad()
        {
            if (m_dicUSoundTable != null)
            {
                m_dicUSoundTable.Clear();
                m_dicUSoundTable = null;
                IsConfigLoad = false;
            }
            if(sb_USoundName != null)
            {
                sb_USoundName.Length = 0;
                sb_USoundName = null;
            }
        }

        public void Load_Editor()
        {
            if(!IsConfigLoad)
            {
                Load(ViewConfigManager.Editor_DataScpPath + ConfigName);
            }
        }


        /// <summary>
        /// 慎用！！！别修改了返回值内容
        /// 取配置表数据
        /// </summary>
        /// <param name="_strUSoundName">音效名字</param>
        /// <returns></returns>
        public SSchemeUSoundData GetData(string _strUSoundName)
        {
            if(!string.IsNullOrEmpty(_strUSoundName) && m_dicUSoundTable != null && m_dicUSoundTable.ContainsKey(_strUSoundName))
            {
                return m_dicUSoundTable[_strUSoundName];
            }
            return null;
        }

        /// <summary>
        /// 取配置表数据
        /// </summary>
        /// <param name="_strUSoundName">配置表名字</param>
        /// <param name="_schemeUSoundData">我给你赋值，避免修改原始数据</param>
        /// <returns></returns>
        public bool GetData(string _strUSoundName, ref SSchemeUSoundData _schemeUSoundData)
        {
            if (_schemeUSoundData == null)
                return false;

            if (m_dicUSoundTable != null && m_dicUSoundTable.ContainsKey(_strUSoundName))
            {
                m_dicUSoundTable[_strUSoundName].CopyTo(ref _schemeUSoundData);
                return true;
            }
            return false;
        }

        /// <summary>
        /// 取配置表数据
        /// </summary>
        /// <param name="_strUSoundFilter">筛选字</param>
        /// <param name="_strUSoundKey">关键字</param>
        /// <param name="_schemeUSoundData">我给你赋值，避免修改原始数据</param>
        /// <returns></returns>
        public bool GetData(string _strUSoundFilter,string _strUSoundKey ,ref SSchemeUSoundData _schemeUSoundData)
        {
            if (_schemeUSoundData == null)
                return false;

            string tmpName = string.Empty;
            GetSoundName(_strUSoundFilter, _strUSoundKey, ref tmpName);
            return GetData(tmpName, ref _schemeUSoundData);
        }

        public List<string> GetSoundNameList()
        {
            if (m_dicUSoundTable == null)
                return null;

            return m_dicUSoundTable.Keys.ToList();
        }

        public void GetSoundFilterList(ref List<string> _filterList)
        {
            if (m_dicUSoundTable == null)
            {
                return;
            }
            
            foreach(SSchemeUSoundData data in m_dicUSoundTable.Values)
            {
                if (!_filterList.Contains(data.USoundFilter))
                    _filterList.Add(data.USoundFilter);
            }
        }

        public void GetSoundFilterKeyList(string _strFilter,ref List<string> _keyList)
        {
            if (m_dicUSoundTable == null || string.IsNullOrEmpty(_strFilter))
            {
                return;
            }

            foreach (SSchemeUSoundData data in m_dicUSoundTable.Values)
            {
                if (data.USoundFilter == _strFilter)
                {
                    if(!_keyList.Contains(data.USoundKey))
                        _keyList.Add(data.USoundKey);
                }
            }
        }

        public void GetSoundName(string _strFilter,string _strKey,ref string _strName)
        {
            sb_USoundName.Length = 0;
            sb_USoundName.Append(_strFilter).Append(FilterKeyConnect).Append(_strKey);
            _strName = sb_USoundName.ToString();
        }
    }
}
