using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Data.SideButtonConfig
{
    public enum SideButtonCSVCOLName
    {
        ID,
        NAME,
        LOWLV,
        HIGHLV,
        HINT,
        ICONID,
    }

    public class SSchemeSideButtonInfo
    {
        public int nID;
        public string strName;
        public int nLowLv;
        public int nHighLv;
        public string strHint;
        public int nIconID;
    }


    class SideButtonConfig : Singleton<SideButtonConfig>
    {
        private Dictionary<int, SSchemeSideButtonInfo> m_sideButtonInfoConfig = new Dictionary<int, SSchemeSideButtonInfo>();
        public Dictionary<int, SSchemeSideButtonInfo> SideButtonInfoConfig { get { return m_sideButtonInfoConfig; } }

        public void Load(string stPath)
        {
            LoadSideButtonConfig(stPath);
        }

        public void Unload()
        {
            m_sideButtonInfoConfig.Clear();
            m_sideButtonInfoConfig = null;
        }

        // 载入侧边提示脚本
        private void LoadSideButtonConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeSideButtonInfo buttonInfo = new SSchemeSideButtonInfo();
                buttonInfo.nID = reader.GetInt(i, (int)SideButtonCSVCOLName.ID, 0);
                buttonInfo.strName = reader.GetString(i, (int)SideButtonCSVCOLName.NAME, "");
                buttonInfo.nLowLv = reader.GetInt(i, (int)SideButtonCSVCOLName.LOWLV, 0);
                buttonInfo.nHighLv = reader.GetInt(i, (int)SideButtonCSVCOLName.HIGHLV, 0);
                buttonInfo.strHint = reader.GetString(i, (int)SideButtonCSVCOLName.HINT, "");
                buttonInfo.nIconID = reader.GetInt(i, (int)SideButtonCSVCOLName.ICONID, 0);

                m_sideButtonInfoConfig[buttonInfo.nID] = buttonInfo;
            }
        }

        public SSchemeSideButtonInfo GetSideButtonConfig(int nID)
        {
            SSchemeSideButtonInfo result;
            if (m_sideButtonInfoConfig.TryGetValue(nID, out result))
            {
                return result;
            }

            return null;
        }
    }
}
