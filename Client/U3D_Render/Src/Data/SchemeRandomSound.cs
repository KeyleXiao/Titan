using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.Scheme
{
    public class RandomSoundNode
    {
        public int nMainSoundID;
        public List<int> randomSoundList;

        public void Clear()
        {
            randomSoundList.Clear();
            randomSoundList = null;
        }

        public void PushRandomData(string szList)
        {
            if (string.IsNullOrEmpty(szList))
                return;

            if (null == randomSoundList)
            {
                randomSoundList = new List<int>();
            }

            string[] arry = szList.Split(';');
            {
                foreach(string val in arry)
                {
                    int i;
                    if (!int.TryParse(val, out i))
                    {
                        i = 0;
                    }
                    randomSoundList.Add(i);
                }
            }
        }

        public int RandomSound()
        {
            int id = 0;
            if(null != randomSoundList && randomSoundList.Count > 0)
            {
                Random rd = new Random();
                id = randomSoundList[rd.Next(0, randomSoundList.Count)];
            }
            return id;
        }
    }

    public class SchemeRandomSound
    {
        public static readonly string schemeFileName = "RandomSound.csv";

        private Dictionary<int, RandomSoundNode> m_dicRandomSoundNodeMap = null;

        private static SchemeRandomSound m_instance = new SchemeRandomSound();

        public static SchemeRandomSound Instance
        {
            get { return m_instance; }
        }

        SchemeRandomSound()
        {
            m_dicRandomSoundNodeMap = new Dictionary<int, RandomSoundNode>();
        }

        public void UnLoad()
        {
            foreach (var node in m_dicRandomSoundNodeMap.Values)
            {
                node.Clear();
            }

            m_dicRandomSoundNodeMap.Clear();
            m_dicRandomSoundNodeMap = null;
        }

        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read sheme fail! file=" + strFilePath);
                return;
            }
            m_dicRandomSoundNodeMap.Clear();
            int nRecordCount = reader.GetRecordCount();
            for (int row = 0; row < nRecordCount; row++)
            {
                int nID = reader.GetInt(row, 0, 0);
                if (m_dicRandomSoundNodeMap.ContainsKey(nID))
                {
                    Trace.LogWarning("scheme randomSoundNode id repeat! row=" + row);
                    continue;
                }

                string randomArry = reader.GetString(row, 1, "");

                RandomSoundNode node = new RandomSoundNode();
                node.nMainSoundID = nID;

                node.PushRandomData(randomArry);

                m_dicRandomSoundNodeMap.Add(nID, node);
            }
        }

        public int GetRandomSound(int nID)
        {
            if (!m_dicRandomSoundNodeMap.ContainsKey(nID) || null == m_dicRandomSoundNodeMap[nID])
                return 0;

            return m_dicRandomSoundNodeMap[nID].RandomSound();
        }
    }
}
