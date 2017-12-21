using System;
using System.Collections;
using System.Collections.Generic;

namespace Data.WarBgmConfig
{
    //战场BGM播放配置
    public class SSchemeWarBgmConfig
    {
        public int soundID;                       //音效ID
        public int playCount;                    //播放次数
        public float playDelay;                  //播放延时
        public float playInterval;               //两次播放之间时间间隔

        public void Copy(ref SSchemeWarBgmConfig cfg)
        {
            cfg.soundID = soundID;
            cfg.playCount = playCount;
            cfg.playDelay = playDelay;
            cfg.playInterval = playInterval;
        }
    }

    class WarBgmConfig
    {
        public Dictionary<int, List<SSchemeWarBgmConfig>> warBgmConfigDic;
        private static WarBgmConfig singleton = new WarBgmConfig();
        public static WarBgmConfig Instance
        {
            get
            {
                if (singleton == null)
                    singleton = new WarBgmConfig();

                return singleton;
            }
        }

        public void Load(string stPath)
        {
            InitWarBgmConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (warBgmConfigDic != null)
            {
                warBgmConfigDic.Clear();
                warBgmConfigDic = null;
            }
            singleton = null;
        }

        public bool GetWarBgmConfig(int configID, ref List<SSchemeWarBgmConfig> configList)
        {
            if (warBgmConfigDic.ContainsKey(configID))
            {
                foreach(SSchemeWarBgmConfig cfg in warBgmConfigDic[configID])
                {
                    SSchemeWarBgmConfig config = new SSchemeWarBgmConfig();
                    cfg.Copy(ref config);
                    configList.Add(config);
                }
                return true;
            }
            return false;
        }

        private void InitWarBgmConfigCsv(string path)
        {
            ScpReader reader = new ScpReader(path, true, 2);
            warBgmConfigDic = new Dictionary<int,List<SSchemeWarBgmConfig>>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;
                int configID = reader.GetInt(i, nCol++, 0);
                SSchemeWarBgmConfig sConfig = new SSchemeWarBgmConfig();
                sConfig.soundID = reader.GetInt(i, nCol++, 0);
                sConfig.playCount = reader.GetInt(i, nCol++, 0);
                sConfig.playDelay = reader.GetFloat(i, nCol++, 0);
                sConfig.playInterval = reader.GetFloat(i, nCol++, 0);
                List<SSchemeWarBgmConfig> warBgmList;
                if (!warBgmConfigDic.TryGetValue(configID, out warBgmList))
                {
                    warBgmList = new List<SSchemeWarBgmConfig>();
                    warBgmConfigDic.Add(configID, warBgmList);
                }
                warBgmConfigDic[configID].Add(sConfig);
            }
            reader.Dispose();
            reader = null;
        }
    }
}
