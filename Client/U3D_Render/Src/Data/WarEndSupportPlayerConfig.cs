/*******************************************************************
** 文件名:	PlayerCrownWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-12-12
** 版  本:	1.0
** 描  述:	点赞配置
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.WarSupportPlayerConfig
{
    public class SSchemeWarSupportPlayerConfig
    {
        public int nVoteID;
        public string sVoteDesc;
        public int nVoteIconID;
		public int[] labelIDList;
        public int nParam1;
    }

    public class SSchemeWarSupportPositionConfig
    {
        public int nSortID;
        public int nPlayerCount;
        public float fUIPositionOffsetX;
        public Vector3 playerOffsetVec3;
    }

	public class SSchemeLabelConfig
	{
		public int nLabelID;
		public string labelName;
		public int nEffectID;
		public int nIconID;
	}
    class WarSupportPlayerConfigPage
    {
        public Hashtable allWarSupportPlayerConfigTable;
        public Hashtable allWarSupportPositionConfigTable;
		public Hashtable allLabelConfigTable;

        private static WarSupportPlayerConfigPage singleton = new WarSupportPlayerConfigPage();
        public static WarSupportPlayerConfigPage Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new WarSupportPlayerConfigPage();
            }
        }

        public void Load(string stPath1, string stPath2, string stPath3)
        {
            InitWarSupportPlayerConfigCsv(stPath1);
            InitWarSupportPositionConfigCsv(stPath2);
			InitLabelConfigCsv(stPath3);
		}

        public void UnLoad()
        {
            if (allWarSupportPlayerConfigTable != null)
            {
                allWarSupportPlayerConfigTable.Clear();
                allWarSupportPlayerConfigTable = null;
            }

            if (allWarSupportPositionConfigTable != null)
            {
                allWarSupportPositionConfigTable.Clear();
                allWarSupportPositionConfigTable = null;
            }

			if(allLabelConfigTable != null)
			{
				allLabelConfigTable.Clear();
				allLabelConfigTable = null;
			}


            singleton = null;
        }

        // 初始化csv表
        protected void InitWarSupportPlayerConfigCsv(string stPath)
        {     
            ScpReader reader = new ScpReader(stPath, true, 2);
            allWarSupportPlayerConfigTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;
                SSchemeWarSupportPlayerConfig sConfig = new SSchemeWarSupportPlayerConfig();
                sConfig.nVoteID = reader.GetInt(i, nCol++, 0);
                sConfig.sVoteDesc = reader.GetString(i, nCol++, "");
                sConfig.nVoteIconID = reader.GetInt(i, nCol++, 0);

				// 读取标签ID列表
				string[] strList = reader.GetString(i, nCol++, "").Split(';');
				if(strList.Length > 0)
					sConfig.labelIDList = new int[strList.Length];
				else 
					sConfig.labelIDList = new int[0];
				for(int j = 0; j < strList.Length; ++j)
				{
					int result;
					if(!int.TryParse(strList[j], out result))
					{
						Debug.LogError("配置文件出错,解析字符串为int类型出错,path=" + stPath);
						result = -1;
					}

					sConfig.labelIDList[j] = result;	
				}

                sConfig.nParam1 = reader.GetInt(i, nCol++, 0);

                allWarSupportPlayerConfigTable[sConfig.nVoteID] = sConfig;   
            }        
        }

        // 初始化csv表
        protected void InitWarSupportPositionConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath, true, 2);
            allWarSupportPositionConfigTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;
                SSchemeWarSupportPositionConfig sConfig = new SSchemeWarSupportPositionConfig();
                sConfig.nSortID = reader.GetInt(i, nCol++, 0);
                sConfig.nPlayerCount = reader.GetInt(i, nCol++, 0);
                sConfig.fUIPositionOffsetX = reader.GetFloat(i, nCol++, 0.0f);
                sConfig.playerOffsetVec3.x = reader.GetFloat(i, nCol++, 0.0f);
                sConfig.playerOffsetVec3.y = reader.GetFloat(i, nCol++, 0.0f);
                sConfig.playerOffsetVec3.z = reader.GetFloat(i, nCol++, 0.0f);

                if (allWarSupportPositionConfigTable.ContainsKey(sConfig.nSortID))
                {
                    (allWarSupportPositionConfigTable[sConfig.nSortID] as List<SSchemeWarSupportPositionConfig>).Add(sConfig);

                }
                else
                {
                    List<SSchemeWarSupportPositionConfig> positionList = new List<SSchemeWarSupportPositionConfig>();
                    positionList.Add(sConfig);
                    allWarSupportPositionConfigTable[sConfig.nSortID] = positionList;
                }  
            }
        }

		// 初始化Label.csv
		protected void InitLabelConfigCsv(string stPath)
		{
			ScpReader reader = new ScpReader(stPath, true, 2);
			allLabelConfigTable = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
			{
				int nCol = 0;
				SSchemeLabelConfig sConfig = new SSchemeLabelConfig();
				sConfig.nLabelID = reader.GetInt(i, nCol++, 0);
				sConfig.labelName = reader.GetString(i, nCol++, "");
				sConfig.nEffectID = reader.GetInt(i, nCol++, 0);
				sConfig.nIconID = reader.GetInt(i, nCol++, 0);

				allLabelConfigTable[sConfig.nLabelID] = sConfig;
			}

		}

        public SSchemeWarSupportPlayerConfig GetWarSupportPlayerConfig(int nVoteID)
        {
            if (allWarSupportPlayerConfigTable == null || !allWarSupportPlayerConfigTable.ContainsKey(nVoteID))
            {
                return null;
            }

            return allWarSupportPlayerConfigTable[nVoteID] as SSchemeWarSupportPlayerConfig;
        }

        public SSchemeWarSupportPositionConfig GetWarSupportPositionConfig(int nSortID, int nPlayerCount)
        {
            if (allWarSupportPositionConfigTable == null || !allWarSupportPositionConfigTable.ContainsKey(nSortID))
            {
                return null;
            }

            List<SSchemeWarSupportPositionConfig> positionList = allWarSupportPositionConfigTable[nSortID] as List<SSchemeWarSupportPositionConfig>;
            if (positionList == null)
                return null;

            foreach (SSchemeWarSupportPositionConfig item in positionList)
            {
                if (item.nPlayerCount == nPlayerCount)
                    return item;
            }

            return null;
        }

		public SSchemeLabelConfig GetLabelConfig(int nLabelID)
		{
			if (allLabelConfigTable == null || !allLabelConfigTable.ContainsKey(nLabelID))
			{
				return null;
			}

			return allLabelConfigTable[nLabelID] as SSchemeLabelConfig;
		}
    }
}
