using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.PreLoadSkinLayout
{

    //PreLoadSkinLayout.csv表里面对应的列
    public enum PreLoadSkinLayoutColnum : int
    {
        EPSLC_SKIN_ID,              // 皮肤ID
        EPSLC_RELATE_SKIN_ID,       // 关联的皮肤ID
    }

    public struct SPreLoadSkinLayout
    {
        public int nSkinID;                // 皮肤ID
        public int[] nRelateSkinID;        // 关联的皮肤ID
    }

    class PreLoadSkinLayout
    {

        private static PreLoadSkinLayout singleton = new PreLoadSkinLayout();
        private Dictionary<int, SPreLoadSkinLayout> m_ConfigMap = new Dictionary<int, SPreLoadSkinLayout>();

        public static PreLoadSkinLayout Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PreLoadSkinLayout();
            }
        }

        public void Load(string stPath)
        {
            InitPreLoadSkinLayoutCsv(stPath);
        }

        public void UnLoad()
        {
           
            singleton = null;
            m_ConfigMap = null;
        }

        private void InitPreLoadSkinLayoutCsv(string stPath)
        {
            m_ConfigMap = new Dictionary<int, SPreLoadSkinLayout>();

            ScpReader reader = new ScpReader(stPath,true,2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SPreLoadSkinLayout config = new SPreLoadSkinLayout();
                config.nSkinID = reader.GetInt(i, (int)PreLoadSkinLayoutColnum.EPSLC_SKIN_ID, 0);

                string strRelateSkinID = reader.GetString(i, (int)PreLoadSkinLayoutColnum.EPSLC_RELATE_SKIN_ID, "");
                string[] relateSkinIDArray = strRelateSkinID.Split(';');

                config.nRelateSkinID = new int[relateSkinIDArray.Length];
                for(int j = 0; j < relateSkinIDArray.Length; ++j)
                {
                    if (relateSkinIDArray[j] == "")
                        continue;

                    if (!int.TryParse(relateSkinIDArray[j], out config.nRelateSkinID[j]))
                    {
                        Debug.LogError(String.Format("配置格式错误：皮肤id格式错误,relateskinid={0},{1}", relateSkinIDArray[j], stPath));
                        continue;
                    }
                }

                m_ConfigMap.Add(config.nSkinID, config);
            }
        }

        // 获取关联的皮肤ID数组
        public int[] GetRelateSkinID(int nSkinID)
        {
            if (!m_ConfigMap.ContainsKey(nSkinID))
                return null;

            return m_ConfigMap[nSkinID].nRelateSkinID;
        }
    }
}
