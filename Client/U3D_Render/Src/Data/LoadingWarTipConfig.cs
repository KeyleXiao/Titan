using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.LoadingWarTipConfig
{
    public enum ELoadingTipType
    {
        ELTT_NONE,
        ELTT_FIRST,        // 1类
        ELTT_SECOND,       // 2类

        ELTT_MAX,
    }

    // 与LoadingWarTip.csv表里面的列对应
    public enum ELoadingWarTip
    {
        ELWT_ID,            // 提示ID
        ELWT_TYPE,          // 提示类型
        ELWT_TIP,           // 提示内容
        ELWT_TITLE,         // 提示标题
        
        ELWT_MAX,           // 提示内容
    }

    public class SSchemeLoadingWarTip
    {
        public int nTipID;              // 提示ID
        public string sTipContent;      // 提示内容
        public string sTipTitle;        // 提示标题
        public int nTipType;            // 提示类型
    }

    class LoadingWarTipConfig : Singleton<LoadingWarTipConfig>
    {
        private List<SSchemeLoadingWarTip> m_listFirstTypeTip;
        private List<SSchemeLoadingWarTip> m_listSecondTypeTip;

        public void Load(string stPath)
        {
            LoadLoadingWarTipConfig(stPath + "LoadingWarTip.csv");
        }

        public void Unload()
        {
            m_listFirstTypeTip.Clear();
            m_listFirstTypeTip = null;
            m_listSecondTypeTip.Clear();
            m_listSecondTypeTip = null;
        }

        // 比赛加载界面提示
        private void LoadLoadingWarTipConfig(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);

            m_listFirstTypeTip = new List<SSchemeLoadingWarTip>();
            m_listSecondTypeTip = new List<SSchemeLoadingWarTip>();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeLoadingWarTip tipInfo = new SSchemeLoadingWarTip();
                tipInfo.nTipID = reader.GetInt(i, (int)ELoadingWarTip.ELWT_ID, 0);
                tipInfo.nTipType = reader.GetInt(i, (int)ELoadingWarTip.ELWT_TYPE, 0);
                tipInfo.sTipContent = reader.GetString(i, (int)ELoadingWarTip.ELWT_TIP, "");
                tipInfo.sTipTitle = reader.GetString(i, (int)ELoadingWarTip.ELWT_TITLE, "");

                if (tipInfo.nTipType == (int)ELoadingTipType.ELTT_FIRST)
                {
                    m_listFirstTypeTip.Add(tipInfo);
                }
                else if (tipInfo.nTipType == (int)ELoadingTipType.ELTT_SECOND)
                {
                    m_listSecondTypeTip.Add(tipInfo);
                }
            }
        }

        //public int GetTipCount()
        //{
        //    return m_listTipConfig.Count;
        //}

        //public SSchemeLoadingWarTip GetTipConfig(int nConfigIndex)
        //{
        //    SSchemeLoadingWarTip tipConfig = null;
        //    if (nConfigIndex >= 0 && nConfigIndex < m_listTipConfig.Count)
        //        tipConfig = m_listTipConfig[nConfigIndex];

        //    return tipConfig;
        //}

        public SSchemeLoadingWarTip GetRandTipConfig(ELoadingTipType eType = ELoadingTipType.ELTT_FIRST)
        {
            SSchemeLoadingWarTip tipConfig = null;

            do{
                List<SSchemeLoadingWarTip> listConfig = null;
                if (eType == ELoadingTipType.ELTT_FIRST)
                    listConfig = m_listFirstTypeTip;
                else if (eType == ELoadingTipType.ELTT_SECOND)
                    listConfig = m_listSecondTypeTip;
                if (listConfig == null || listConfig.Count <= 0)
                    break;

                System.Random rd = new System.Random((int)(UnityEngine.Time.unscaledTime));
                int nTipHintIndex = rd.Next() % listConfig.Count;
                tipConfig = listConfig[nTipHintIndex];

            }while (false);

            return tipConfig;
        }

    }
}
