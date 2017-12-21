using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.Scheme
{
    public enum EMHeroSoundType
    {
        EHeroSound_Relive,
    }

    public class SchemeHeroSoundNode
    {
        public int nHeroID;
        public Dictionary<EMHeroSoundType, List<int>> dicHeroSoundTable;

        public SchemeHeroSoundNode(int _heroID)
        {
            nHeroID = _heroID;
            dicHeroSoundTable = new Dictionary<EMHeroSoundType, List<int>>();
        }

        public void PushSoundData(params string[] arry)
        {
            for(int i = 0; i < arry.Length; i++)
            {
                if (!Enum.IsDefined(typeof(EMHeroSoundType), i))
                    break;

                if (arry[i] == "")
                    continue;

                var list = new List<int>(Array.ConvertAll(arry[i].Split(';'), int.Parse));
                dicHeroSoundTable.Add((EMHeroSoundType)i, list);
            }
        }

        public int RandomSound(EMHeroSoundType eType)
        {
            int id = 0;
            if (dicHeroSoundTable.ContainsKey(eType))
            {
                Random rd = new Random();
                id = dicHeroSoundTable[eType][rd.Next(0, dicHeroSoundTable[eType].Count)];
            }
            return id;
        }
    }

    public class SchemeHeroSound
    {
        public static readonly string schemeFileName = "HeroSound.csv";

        private Dictionary<int, SchemeHeroSoundNode> m_dicHeroSoundNodeMap = null;

        private static SchemeHeroSound m_instance = new SchemeHeroSound();

        public static SchemeHeroSound Instance
        {
            get { return m_instance; }
        }

        SchemeHeroSound()
        {
            m_dicHeroSoundNodeMap = new Dictionary<int, SchemeHeroSoundNode>();
        }

        public void UnLoad()
        {
            m_dicHeroSoundNodeMap.Clear();
            m_dicHeroSoundNodeMap = null;
        }

        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read sheme fail! file=" + strFilePath);
                return;
            }
            m_dicHeroSoundNodeMap.Clear();
            int nRecordCount = reader.GetRecordCount();
            for (int row = 0; row < nRecordCount; row++)
            {
                int nHeroID = reader.GetInt(row, 0, 0);
                if (m_dicHeroSoundNodeMap.ContainsKey(nHeroID))
                {
                    Trace.LogWarning("scheme id repeat! " + schemeFileName + " row=" + row);
                    continue;
                }

                string randomArry = reader.GetString(row, 1, "");

                SchemeHeroSoundNode node = new SchemeHeroSoundNode(nHeroID);
                node.PushSoundData(randomArry);

                m_dicHeroSoundNodeMap.Add(nHeroID, node);
            }
        }

        public int GetRandomSound(int nHeroID, EMHeroSoundType eType)
        {
            if (!m_dicHeroSoundNodeMap.ContainsKey(nHeroID) || null == m_dicHeroSoundNodeMap[nHeroID])
                return 0;

            return m_dicHeroSoundNodeMap[nHeroID].RandomSound(eType);
        }
    }
}
