/*******************************************************************
** 文件名:	RunePageDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-15
** 版  本:	1.0
** 描  述:	单个天赋页界面详细数据模块 缓存数据
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using ASpeedGame.Data.RunePage;
namespace DataCenter
{

    public class RuneItemInfo 
    {
        public int nRuneId;
        public int nRuneNum;              
    };

    public class RuneTypeNameInfo
    {
        public double typeValue;
        public int nType;
    };

    public class RunePageDataItem
    {

        public bool bIsChanged = false; 
        public int nCreateIndex = 0;
        public int nSaveIndex = 0;
        public Dictionary<int, int> dRuneTempSlotInfo;          // 暂时记录的点  位置->对应一个符文id
        public string sRecordName = "";                         // 缓存记录名字
        public string sTempName = "";                           // 缓存记录名字
        public Dictionary<int, int> dRuneRecordSlotPoints;          //缓存记录点数



        public Dictionary<string, RuneTypeNameInfo> dRuneTypeNameTempCount;          // 记录一种类型的名称的镶嵌个数，做统计用
        public Dictionary<string, RuneTypeNameInfo> dRuneTypeNameRecordCount;

        public RunePageDataItem()
        {
            dRuneTempSlotInfo = new Dictionary<int, int>();
            dRuneRecordSlotPoints = new Dictionary<int, int>();
            dRuneTypeNameTempCount = new Dictionary<string, RuneTypeNameInfo>();
            dRuneTypeNameRecordCount = new Dictionary<string, RuneTypeNameInfo>();
        }

        public void RescoverPointsData()
        {
            // 恢复数据,缓存->临时数据
            if (!bIsChanged)
            {
                return;
            }

            dRuneTempSlotInfo.Clear();
            foreach (int nRuneIdKey in dRuneRecordSlotPoints.Keys)
            {
                int nPoints = dRuneRecordSlotPoints[nRuneIdKey];
                // 本地记录缓存
                dRuneTempSlotInfo[nRuneIdKey] = nPoints;
            }

            dRuneTypeNameTempCount.Clear();
            foreach (string sRuneName in dRuneTypeNameRecordCount.Keys)
            {
                double nValue = dRuneTypeNameRecordCount[sRuneName].typeValue;
                int nType = dRuneTypeNameRecordCount[sRuneName].nType;

                if (dRuneTypeNameTempCount.ContainsKey(sRuneName))
                {
                    dRuneTypeNameTempCount[sRuneName].typeValue = nValue;
                    dRuneTypeNameTempCount[sRuneName].nType = nType;
                }
                else
                {
                    RuneTypeNameInfo runeTypeNameInfo = new RuneTypeNameInfo();
                    runeTypeNameInfo.typeValue = nValue;
                    runeTypeNameInfo.nType = nType;
                    dRuneTypeNameTempCount[sRuneName] = runeTypeNameInfo;
                }
            }

            sTempName = sRecordName;
            return;
        }

        public void ResetData()
        {
            // 重置数据
            dRuneTempSlotInfo.Clear();
            bIsChanged = true;
            foreach (string sRuneName in dRuneTypeNameTempCount.Keys)
            {
                dRuneTypeNameTempCount[sRuneName].typeValue = 0;
            }
           
        }

        public void SetSaveData()
        {
            // 覆盖缓存 临时数据->缓存
            if (!bIsChanged)
            {
                return;
            }

            dRuneRecordSlotPoints.Clear();
            foreach (int nRuneIdKey in dRuneTempSlotInfo.Keys)
            {
                int nPoints = dRuneTempSlotInfo[nRuneIdKey];
                // 本地记录缓存
                dRuneRecordSlotPoints[nRuneIdKey] = nPoints;
            }

            dRuneTypeNameRecordCount.Clear();
            foreach (string sRuneName in dRuneTypeNameTempCount.Keys)
            {
                double nValue = dRuneTypeNameTempCount[sRuneName].typeValue;
                int nType = dRuneTypeNameTempCount[sRuneName].nType;
                // 本地记录缓存
                if(dRuneTypeNameRecordCount.ContainsKey(sRuneName))
                {
                    dRuneTypeNameRecordCount[sRuneName].typeValue = nValue;
                    dRuneTypeNameRecordCount[sRuneName].nType = nType;
                }
                else
                {
                    RuneTypeNameInfo runeTypeNameInfo = new RuneTypeNameInfo();
                    runeTypeNameInfo.typeValue = nValue;
                    runeTypeNameInfo.nType = nType;
                    dRuneTypeNameRecordCount[sRuneName] = runeTypeNameInfo;
                }
            }

            sRecordName = sTempName;

            return;
        }      
    }
}
