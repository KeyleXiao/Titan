using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Data.SceneEffectNodeData
{
    enum ESceneEffectNodeConfig : int
    {
        SEN_ID,
        SEN_Duration,
        SEN_Path,
    }
    
    /// <summary>
    /// 脚本配置文件数据
    /// </summary>
    public class SSchemeSceneEffectNodeData
    {
        /// <summary>  效果ID </summary>
        public int eSceneEffectID;
        /// <summary> 资源路径 </summary>
        public string strSceneEffectPath;
        /// <summary> 持续时间ms </summary>
        public int nSceneEffectDuration;
    }

    public class SceneEffectNodeData : Singleton<SceneEffectNodeData>, IModelConfig
    {
        protected Dictionary<int, SSchemeSceneEffectNodeData> m_dicSchemeSceneEffectNodeData;

        public void Load(string strPath)
        {
            m_dicSchemeSceneEffectNodeData = new Dictionary<int, SSchemeSceneEffectNodeData>();

            ScpReader reader = new ScpReader(strPath, true, 1);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeSceneEffectNodeData sConfig = new SSchemeSceneEffectNodeData();
                sConfig.eSceneEffectID = reader.GetInt(i, (int)ESceneEffectNodeConfig.SEN_ID, 0);
                sConfig.nSceneEffectDuration = reader.GetInt(i, (int)ESceneEffectNodeConfig.SEN_Duration, 0);
                sConfig.strSceneEffectPath = reader.GetString(i, (int)ESceneEffectNodeConfig.SEN_Path, string.Empty);

                if (m_dicSchemeSceneEffectNodeData.ContainsKey(sConfig.eSceneEffectID))
                {
                    Trace.LogWarning("有相同的场景特效ID" + sConfig.eSceneEffectID + "表第" + i + "行");
                }
                else
                    m_dicSchemeSceneEffectNodeData.Add(sConfig.eSceneEffectID, sConfig);
            }
            reader.Dispose();
            reader = null;
        }

        public void UnLoad()
        {
            if (m_dicSchemeSceneEffectNodeData != null)
            {
                m_dicSchemeSceneEffectNodeData.Clear();
                m_dicSchemeSceneEffectNodeData = null;
            }
        }

        public SSchemeSceneEffectNodeData GetData(int _eEffectID)
        {
            if (m_dicSchemeSceneEffectNodeData != null)
            {
                if (m_dicSchemeSceneEffectNodeData.ContainsKey(_eEffectID))
                {
                    return m_dicSchemeSceneEffectNodeData[_eEffectID];
                }
            }
            return null;
        }
    }
}
