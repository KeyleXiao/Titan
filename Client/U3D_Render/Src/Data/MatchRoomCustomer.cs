using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;

namespace ASpeedGame.Data.CreateMatchRoomCustomer
{
    // 与MatchRoomCustomer.csv表里面的列对应
    public enum CreateRoomCustomer : int
    {
        ERC_ID,                 // 战场id
        ERC_MAPDES,             // 地图描述
        ERC_MAPDETAILDES,       // 详细描述
        ERC_PICTRUE_ID,         // 对应图片ID
        ERC_PERSONCOUNT_DES,    // 对阵人物描述列表
        ERC_MATCHTYPE_LIST,     // 对应比赛类型
        ERC_ISOPEN,          // 是否开启
    }

    /// <summary>
    /// ai配置类
    /// </summary>
    public class SSchemeMatchRoomCustomer
    {
        public int nID;                         // ID

        public string MapDes ;                  // 地图描述

        public string MapDetailDes;             // 详细描述

        public int PictrueID;                   // 图片ID

        public string strPersonCountListDes;    // 可选对阵数量描述

        public string strMatchTypeList;         // 比赛类型对应

        public bool bIsOpen;                    // 是否开启

    }

    class MatchRoomCustomer
    {
        public Hashtable allMatchRoomCustomerTable = new Hashtable();
        private static MatchRoomCustomer singleton = new MatchRoomCustomer();
       
        public static MatchRoomCustomer Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new MatchRoomCustomer();
            }
        }

        public void Load(string stPath)
        {
            InitMatchRoomCustomerConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (allMatchRoomCustomerTable != null)
            {
                allMatchRoomCustomerTable.Clear();
                allMatchRoomCustomerTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitMatchRoomCustomerConfigCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);
            ScpReader reader = new ScpReader(stPath, true, 2); 
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                
                SSchemeMatchRoomCustomer sConfig = new SSchemeMatchRoomCustomer();
                sConfig.nID = reader.GetInt(i, (int)CreateRoomCustomer.ERC_ID, 0);
                sConfig.MapDes = reader.GetString(i, (int)CreateRoomCustomer.ERC_MAPDES, "");
                sConfig.MapDetailDes = reader.GetString(i, (int)CreateRoomCustomer.ERC_MAPDETAILDES, "");
                sConfig.PictrueID = reader.GetInt(i, (int)CreateRoomCustomer.ERC_PICTRUE_ID, 0);
                sConfig.strPersonCountListDes = reader.GetString(i, (int)CreateRoomCustomer.ERC_PERSONCOUNT_DES, "");
                sConfig.strMatchTypeList = reader.GetString(i, (int)CreateRoomCustomer.ERC_MATCHTYPE_LIST, "");

                // 配置检查
                string[] PersonCountStr = sConfig.strPersonCountListDes.Split(';');
                int nDescArrayLength = PersonCountStr.Length;

                string[] AIIDArrayStr = sConfig.strMatchTypeList.Split(';');
                int nArrayLength = AIIDArrayStr.Length;

                if (nDescArrayLength != nArrayLength)
                {
                    Trace.LogError("配置出错 请检查 MatchRoomCustomer.csv 位置：=(" + (i + 1) + ")行");
                }
                sConfig.bIsOpen = reader.GetInt(i, (int)CreateRoomCustomer.ERC_ISOPEN, 0) == 1 ? true : false;
                if (allMatchRoomCustomerTable.ContainsKey(sConfig.nID))
                {
                    Trace.LogError("重复id 检查配置MatchRoomCustomer.csv！id =" + sConfig.nID);
                    
                }
                allMatchRoomCustomerTable.Add(sConfig.nID, sConfig);
            }
        }
    }
}
