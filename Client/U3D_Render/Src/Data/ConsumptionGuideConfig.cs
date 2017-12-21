using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.ConsumptionGuideConfig
{
    // ConsumptionGuideConfig.csv
    public enum EConsumptionGuideConfig
    {
        ECGC_COL_ID,    // 引导ID
        ECGC_COL_TYPE,  // 引导类型（1勋章2金币3元宝4终结技5宝石6宝箱）
        ECGC_COL_NAME,  // 引导名称
        ECGC_COL_TITLE, // 引导标题
        ECGC_COL_INDEX, // 引导项索引
        ECGC_COL_DESC,  // 引导项描述
        ECGC_COL_WND,   // 引导项链接界面（1完成任务 2联盟捐赠 3晋升解锁 4开宝箱 5商城勋章购买 6商城物品购买 7商城充值 ）
    }

    public class SSchemeGuideWnd
    {
        public int nGuideWnd;
        public string nGuideDesc;
    }

    public class SSchemeGuideType
    {
        public int nTypeId;
        public string strGuideName;
        public string strGuideTitle;
        public List<SSchemeGuideWnd> guideWndList;
    }

    public class ConsumptionGuideConfig
    {
        private static ConsumptionGuideConfig singleton = new ConsumptionGuideConfig();
        public static ConsumptionGuideConfig Instance
        {
            get { return singleton; }
        }

        private Dictionary<int, SSchemeGuideType> m_guideTypeDic;

        public void Load(string stPath)
        {
            LoadGuideTypeConfig(stPath);
        }

        public void Unload()
        {
            m_guideTypeDic.Clear();
        }

        private void LoadGuideTypeConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            m_guideTypeDic = new Dictionary<int, SSchemeGuideType>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nTypeId = reader.GetInt(i, (int)EConsumptionGuideConfig.ECGC_COL_TYPE, 0);
                string strGuideName = reader.GetString(i, (int)EConsumptionGuideConfig.ECGC_COL_NAME, "");
                string strGuideTitle = reader.GetString(i, (int)EConsumptionGuideConfig.ECGC_COL_TITLE, "");
                string strGuideDesc = reader.GetString(i, (int)EConsumptionGuideConfig.ECGC_COL_DESC, "");
                int nWndId = reader.GetInt(i, (int)EConsumptionGuideConfig.ECGC_COL_WND, 0);

                if (!m_guideTypeDic.ContainsKey(nTypeId))
                {
                    SSchemeGuideType guideType = new SSchemeGuideType();
                    guideType.nTypeId = nTypeId;
                    guideType.strGuideName = strGuideName;
                    guideType.strGuideTitle = strGuideTitle;
                    guideType.guideWndList = new List<SSchemeGuideWnd>();
                    m_guideTypeDic.Add(nTypeId, guideType);
                }
                SSchemeGuideWnd guideWnd = new SSchemeGuideWnd();
                guideWnd.nGuideDesc = strGuideDesc;
                guideWnd.nGuideWnd = nWndId;
                m_guideTypeDic[nTypeId].guideWndList.Add(guideWnd);
            }
        }

        public Dictionary<int, SSchemeGuideType> GetGuideTypeDic()
        {
            return m_guideTypeDic;
        }

        public SSchemeGuideType GetGuideType(int nTypeId)
        {
            SSchemeGuideType scheme = null;

            if (m_guideTypeDic.TryGetValue(nTypeId, out scheme))
            {
                return scheme;
            }

            return null;
        }
    }
}
