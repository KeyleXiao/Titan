using System;
using System.Collections.Generic;
using System.Text;

namespace Data.WarSubFuncConfig
{
	enum EWarSubFuncType
	{
		EWarSubFuncType_XPSkill = 0,
		EWarSubFuncType_Support,

		EWarSubFuncType_Max,
	}

	public class SSchemeWarSubFuncConfig
	{
		public int nMatchType;
		public bool[] isEnableList;
	}

    class WarSubFuncConfig
	{
        private static WarSubFuncConfig m_singleton;
        public static WarSubFuncConfig Instance
        {
            get
            {
                if (m_singleton == null)
                    m_singleton = new WarSubFuncConfig();

                return m_singleton;
            }
        }

        private Dictionary<int, SSchemeWarSubFuncConfig> m_mapWarSubFunc = new Dictionary<int, SSchemeWarSubFuncConfig>();

        public void Load(string stPath)
        {
			ScpReader reader = new ScpReader(stPath, true, 2);
			for (int i = 0; i < reader.GetRecordCount(); i++)
			{
				int nIndex = 0;
				SSchemeWarSubFuncConfig sConfig = new SSchemeWarSubFuncConfig();
				sConfig.nMatchType = reader.GetInt(i, nIndex++, 0);

				int nSubFuncCount = (int)EWarSubFuncType.EWarSubFuncType_Max;
				sConfig.isEnableList = new bool[nSubFuncCount];
				for(int j = 0; j < nSubFuncCount; ++j)
				{
					sConfig.isEnableList[j] = reader.GetInt(i, nIndex++, 0) > 0 ? true : false;
				}

				m_mapWarSubFunc.Add(sConfig.nMatchType, sConfig);
			}

		}

		public void UnLoad()
		{
			if (m_mapWarSubFunc != null)
			{
				m_mapWarSubFunc.Clear();
				m_mapWarSubFunc = null;
			}
			m_singleton = null;
		}

		public bool isEnableWarSubFunc(int nMatchType, EWarSubFuncType subFuncType)
		{
			SSchemeWarSubFuncConfig sConfig;
			if (!m_mapWarSubFunc.TryGetValue(nMatchType, out sConfig))
			{
				return false;
			}

			return sConfig.isEnableList[(int)subFuncType];
		}
    }
}
