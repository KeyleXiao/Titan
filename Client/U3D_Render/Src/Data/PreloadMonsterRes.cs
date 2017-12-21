using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.PreloadMonsterRes
{

    //PreLoadSkinLayout.csv表里面对应的列
    public enum PreloadMonsterResColnum : int
    {
        EPSLC_MAP_ID,              // 地图ID
        EPSLC_SKIN_ID,       // 皮肤ID
    }

    public struct SPreloadMonsterRes
    {
        public int MapID; //地图ID
        public int[] SkinIDs;        // 皮肤ID
    }

    class PreloadMonsterRes
    {

        private static PreloadMonsterRes singleton = new PreloadMonsterRes();
        private Dictionary<int, SPreloadMonsterRes> m_ConfigMap = new Dictionary<int, SPreloadMonsterRes>();

        public static PreloadMonsterRes Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PreloadMonsterRes();
            }
        }

        public void Load(string stPath)
        {
            InitPreloadMonsterResCsv(stPath);
        }

        public void UnLoad()
        {
            singleton = null;
            m_ConfigMap = null;
        }

        private void InitPreloadMonsterResCsv(string stPath)
        {
            m_ConfigMap = new Dictionary<int, SPreloadMonsterRes>();

            ScpReader reader = new ScpReader(stPath,true,2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SPreloadMonsterRes config = new SPreloadMonsterRes();
                config.MapID = reader.GetInt(i, (int)PreloadMonsterResColnum.EPSLC_MAP_ID, 0);

                if(m_ConfigMap.ContainsKey(config.MapID))
                {
                    Debug.LogError(String.Format("配置格式错误：重复的地图ID,MapID={0}", config.MapID));
                    continue;
                }
                string strRelateSkinID = reader.GetString(i, (int)PreloadMonsterResColnum.EPSLC_SKIN_ID, "");
                string[] relateSkinIDArray = strRelateSkinID.Split(';');

                config.SkinIDs = new int[relateSkinIDArray.Length];
                for(int j = 0; j < relateSkinIDArray.Length; ++j)
                {
                    if (relateSkinIDArray[j] == "")
                        continue;

                    if (!int.TryParse(relateSkinIDArray[j], out config.SkinIDs[j]))
                    {
                        Debug.LogError(String.Format("配置格式错误：皮肤id格式错误,relateskinid={0},{1}", relateSkinIDArray[j], stPath));
                        continue;
                    }
                }

                m_ConfigMap.Add(config.MapID, config);
            }
        }

        // 获取关联的皮肤ID数组
        public int[] GetSkinIDs(int nMapID)
        {
            if (!m_ConfigMap.ContainsKey(nMapID))
                return null;

            return m_ConfigMap[nMapID].SkinIDs;
        }
    }
}
