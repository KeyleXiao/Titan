using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.Scheme
{
    public enum SocialSharingType
    {
        WonderfulMoment,
        WarRecord,
        RankUpgrad,
        LifeHero,
        GamePromotion,
        TypeMax,
    }

    public class SocialSharingNode
    {
        public int m_nIndex;
        public string szTitle;
        public string szDesc;

        public SocialSharingNode(int nIndex, string title = "", string desc = "")
        {
            m_nIndex = nIndex;
            szTitle = title;
            szDesc = desc;
        }
    }

    public class SchemeSocialSharing
    {
        private enum Column
        {
            COL_SHARINGTYPE,
            COL_TITLE,
            COL_DESC,
            COL_MAX,
        }

        public static readonly string schemeFileName = "SocialSharing.csv";

        private Dictionary<SocialSharingType, List<SocialSharingNode>> m_dicSocialSharingNodeList = null;

        private static SchemeSocialSharing m_instance = new SchemeSocialSharing();

        public static SchemeSocialSharing Instance
        {
            get { return m_instance; }
        }

        SchemeSocialSharing()
        {
            m_dicSocialSharingNodeList = new Dictionary<SocialSharingType, List<SocialSharingNode>>();
        }

        public void UnLoad()
        {
            foreach(var nodeList in m_dicSocialSharingNodeList.Values)
            {
                nodeList.Clear();
            }

            m_dicSocialSharingNodeList.Clear();
            m_dicSocialSharingNodeList = null;
        }

        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read sheme fail! file=" + strFilePath);
                return;
            }
            m_dicSocialSharingNodeList.Clear();
            int nRecordCount = reader.GetRecordCount();
            for (int row = 0; row < nRecordCount; row++)
            {
                int nType = reader.GetInt(row, (int)Column.COL_SHARINGTYPE, 0);
                if(nType < 0 || nType >= (int)SocialSharingType.TypeMax)
                {
                    Trace.LogWarning("scheme sharingType invalid! file=" + strFilePath);
                    continue;
                }

                string title = reader.GetString(row, (int)Column.COL_TITLE, "");
                string desc = reader.GetString(row, (int)Column.COL_DESC, "");

                AddNode((SocialSharingType)nType, title, desc);
            }
        }

        public SocialSharingNode RandomSharingNode(SocialSharingType type)
        {
            if (!m_dicSocialSharingNodeList.ContainsKey(type))
                return null;

            List<SocialSharingNode> nodeList = m_dicSocialSharingNodeList[type];
            if (null == nodeList || nodeList.Count < 1)
                return null;

            Random random = new Random();
            int index = random.Next(0, nodeList.Count);
            return nodeList[index];
        }

        private void AddNode(SocialSharingType type, string title, string desc)
        {
            List<SocialSharingNode> nodeList;
            if(!m_dicSocialSharingNodeList.TryGetValue(type, out nodeList))
            {
                nodeList = new List<SocialSharingNode>();
                m_dicSocialSharingNodeList.Add(type, nodeList);
            }

            int nIndex = nodeList.Count;
            SocialSharingNode node = new SocialSharingNode(nIndex, title, desc);

            nodeList.Add(node);
        }
    }
}
