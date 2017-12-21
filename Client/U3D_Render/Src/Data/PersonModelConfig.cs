using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Data.PersonModel
{
    public enum EMPersonModelConfig : int
    {
        EMPMC_HEROID,
        EMPMC_HEROTYPE,
        EMPMC_OPERATIONTYPE,
        EMPMC_HEROCHARACTER,
        EMPMC_SEX,
        EMPMC_DEFAULTSKINID,
        EMPMC_HERONAME,
        EMPMC_HEROTITLE,
        EMPMC_MINIMAPICON,
        EMPMC_VIEWDISTANCE,
        EMPMC_HEROSWITCH,
        EMPMC_TOTALSKINID,
        EMPMC_HEROUNLOCKLV,
    }

    public class SSchemePersonModelData
    {
        public int nHeroID;
        public int nHeroType;
        public int nOperationType;
        public int nHeroCharacter;
        public int nSex;
        public int nDefaultSkinID;
        public int nMiniMapIcon;
        public int nViewDistance;
        public int nHeroSwitch;
        public int nHeroUnLockLv;
        public string szTotalSkinID;
        public string szHeroName;
        public string szHeroTitle;

        public void CopyTo(ref SSchemePersonModelData _sOtherData)
        {
            _sOtherData.nHeroID = this.nHeroID;
            _sOtherData.nHeroType = this.nHeroType;
            _sOtherData.nOperationType = this.nOperationType;
            _sOtherData.nHeroCharacter = this.nHeroCharacter;
            _sOtherData.nSex = this.nSex;
            _sOtherData.nDefaultSkinID = this.nDefaultSkinID;
            _sOtherData.nMiniMapIcon = this.nMiniMapIcon;
            _sOtherData.nViewDistance = this.nViewDistance;
            _sOtherData.nHeroSwitch = this.nHeroSwitch;
            _sOtherData.nHeroUnLockLv = this.nHeroUnLockLv;
            _sOtherData.szTotalSkinID = this.szTotalSkinID;
            _sOtherData.szHeroName = this.szHeroName;
            _sOtherData.szHeroTitle = this.szHeroTitle;
        }
    }

    class PersonModelConfig : Singleton<PersonModelConfig>
    {
        public List<SSchemePersonModelData> PersonModelConfigs { get { return m_personModelConfigs; } }
        private List<SSchemePersonModelData> m_personModelConfigs;

        public void Load(string strPath)
        {
            m_personModelConfigs = new List<SSchemePersonModelData>();

            ScpReader reader = new ScpReader(strPath, true, 2);

            for(int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemePersonModelData data = new SSchemePersonModelData();
                data.nHeroID = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_HEROID, 0);
                data.nHeroType = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_HEROTYPE, 0);
                data.nOperationType = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_OPERATIONTYPE, 0);
                data.nHeroCharacter = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_HEROCHARACTER, 0);
                data.nSex = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_SEX, 0);
                data.nDefaultSkinID = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_DEFAULTSKINID, 0);
                data.szHeroName = reader.GetString(i, (int)EMPersonModelConfig.EMPMC_HERONAME,"");
                data.szHeroTitle = reader.GetString(i, (int)EMPersonModelConfig.EMPMC_HEROTITLE, "");
                data.nMiniMapIcon = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_MINIMAPICON, 0);
                data.nViewDistance = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_VIEWDISTANCE, 0);
                data.nHeroSwitch = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_HEROSWITCH, 0);
                data.szTotalSkinID = reader.GetString(i, (int)EMPersonModelConfig.EMPMC_TOTALSKINID,"");
                data.nHeroUnLockLv = reader.GetInt(i, (int)EMPersonModelConfig.EMPMC_HEROUNLOCKLV, 0);
                m_personModelConfigs.Add(data);
            }
            reader.Dispose();
            reader = null;
        }

        public void UnLoad()
        {
            if (m_personModelConfigs == null)
                return;

            m_personModelConfigs.Clear();
            m_personModelConfigs = null;
        }

        public bool GetData_Out(int _nHeroID,out SSchemePersonModelData _ssData)
        {
            if (PersonModelConfigs.Count <= 0)
            {
                _ssData = null;
                return false;
            }
                

            for (int i = 0; i < PersonModelConfigs.Count; ++i)
            {
                if (PersonModelConfigs[i].nHeroID == _nHeroID)
                {
                    _ssData = PersonModelConfigs[i];
                    return true;
                }                    
            }

            _ssData = null;
            return false;
        }

        public bool GetData_Ref(int _nHeroID,ref SSchemePersonModelData _ssData)
        {
            if (PersonModelConfigs.Count <= 0)
            {
                return false;
            }


            for (int i = 0; i < PersonModelConfigs.Count; ++i)
            {
                if (PersonModelConfigs[i].nHeroID == _nHeroID)
                {
                    PersonModelConfigs[i].CopyTo(ref _ssData);
                    return true;
                }
            }
            
            return false;
        }
    }
}
