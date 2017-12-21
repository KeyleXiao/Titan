using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;

namespace Data.CreateHeroConfig
{
    public class SSchemeCreateHeroConfig
    {
        public int nSexID;      //性别ID
        public string strHeroIDs; //英雄ID列表
    }
    class CreateHeroConfig : Singleton<CreateHeroConfig>, IModelConfig
    {
        private Dictionary<PERSON_SEX, SSchemeCreateHeroConfig> m_dicCreateHeroConfig;

        public void Load(string strPath)
        {
            m_dicCreateHeroConfig = new Dictionary<PERSON_SEX, SSchemeCreateHeroConfig>();

            ScpReader reader = new ScpReader(strPath, true);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nIndex = 0;
                SSchemeCreateHeroConfig sConfig = new SSchemeCreateHeroConfig();
                sConfig.nSexID = reader.GetInt(i, (int)nIndex++, 0);
                sConfig.strHeroIDs = reader.GetString(i, (int)nIndex++, string.Empty);
                
                m_dicCreateHeroConfig.Add((PERSON_SEX)sConfig.nSexID, sConfig);
            }
            reader.Dispose();
            reader = null;
        }

        public void UnLoad()
        {
            if(m_dicCreateHeroConfig != null)
            {
                m_dicCreateHeroConfig.Clear();
                m_dicCreateHeroConfig = null;
            }
        }

        public List<int> GetSexCreateHeroList(PERSON_SEX _eSex)
        {
            if (_eSex == PERSON_SEX.SEX_MAX || m_dicCreateHeroConfig == null || !m_dicCreateHeroConfig.ContainsKey(_eSex))
                return null;

            string strHeroList = m_dicCreateHeroConfig[_eSex].strHeroIDs;
            return ScpReader.ConvertStringToIntArray(strHeroList, new char[] { ';' });
        }
    }
}
