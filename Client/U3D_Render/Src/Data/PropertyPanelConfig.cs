using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.PropertyPanelConfig
{
    public enum EPropertyPanelCol
    {
        EPPC_HERO,              // 英雄ID
        EPPC_PROPERTY,          // 属性ID
        EPPC_LEVEL,             // 属性区间
        EPPC_COLOR,             // 颜色
        EPPC_DESC,              // 描述

        EPPC_MAX,
    }

    public class SSchemePropertyPanel
    {
        public int nHeroID;
        public int nPropertyID;
        public float fPropertyLevel;
        public string strColor;
        public string strDesc;
    }

    public class PropertyPanelConfig : Singleton<PropertyPanelConfig>
    {
        private Dictionary<int, List<SSchemePropertyPanel> > m_dicPropertyPanel;

        public void Load(string stPath)
        {
            LoadPropertyPanelConfig(stPath + "PropertyPanel.csv");
        }

        public void Unload()
        {
            m_dicPropertyPanel.Clear();
            m_dicPropertyPanel = null;
        }

        private void LoadPropertyPanelConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_dicPropertyPanel = new Dictionary<int, List<SSchemePropertyPanel> >();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemePropertyPanel node = new SSchemePropertyPanel();
                node.nHeroID = reader.GetInt(i, (int)EPropertyPanelCol.EPPC_HERO, 0);
                node.nPropertyID = reader.GetInt(i, (int)EPropertyPanelCol.EPPC_PROPERTY, 0);
                node.fPropertyLevel = reader.GetFloat(i, (int)EPropertyPanelCol.EPPC_LEVEL, 0.0f);
                node.strColor = reader.GetString(i, (int)EPropertyPanelCol.EPPC_COLOR, "");
                node.strDesc = reader.GetString(i, (int)EPropertyPanelCol.EPPC_DESC, "");

                if (!m_dicPropertyPanel.ContainsKey(node.nHeroID))
                {
                    List<SSchemePropertyPanel> listConfig = new List<SSchemePropertyPanel>();
                    m_dicPropertyPanel.Add(node.nHeroID, listConfig);
                }
                m_dicPropertyPanel[node.nHeroID].Add(node);
            }

        }

        // 有则返回，无则返回0号
        public List<SSchemePropertyPanel> GetPropertyPanelConfigList(int nHeroID)
        {
            if (m_dicPropertyPanel.ContainsKey(nHeroID))
            {
                return m_dicPropertyPanel[nHeroID];
            }
            else
            {
                return m_dicPropertyPanel[0];
            }
        }

        public SSchemePropertyPanel GetPropertyPanelConfigList(int nHeroID, float fPropValue, int nPropIndex)
        {
            List<SSchemePropertyPanel> Porplist = GetPropertyPanelConfigList(nHeroID);

            if (Porplist == null)
                return null;

            if (nPropIndex <= 0)
                return null;

            List<SSchemePropertyPanel> filtConfig = new List<SSchemePropertyPanel>();
            foreach (var item in Porplist)
            {
                if (item.nPropertyID == nPropIndex)
                {
                    filtConfig.Add(item);
                }
            }

            int i = 0;
            for (i = 0; i < filtConfig.Count - 1; ++i)
            {
                if (fPropValue >= filtConfig[i].fPropertyLevel && fPropValue < filtConfig[i + 1].fPropertyLevel)
                {
                    return filtConfig[i];
                }
            }

            if (fPropValue >= filtConfig[i].fPropertyLevel)
                return filtConfig[i];

            return null;
        }
    }
}
