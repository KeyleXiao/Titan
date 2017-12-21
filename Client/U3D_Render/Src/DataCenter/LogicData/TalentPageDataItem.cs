/*******************************************************************
** 文件名:	TalentPageDataManager.cs 
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
using ASpeedGame.Data.TalentPage;
namespace DataCenter
{
    public class TalentPageDataItem
    {

        public bool bIsChanged = false; 
        public int nCreateIndex = 0;
        public int nSaveIndex = 0;
        public Dictionary<int, int> dTalentTempPoints;          // 暂时记录的点
        public string sRecordName = "";                         // 缓存记录名字
        public string sTempName = "";                           // 缓存记录名字
        public Dictionary<int, int> dTalentRecordPoints;        //缓存记录点数
        public int nAttackPoints = 0;                           // 攻击加点情况
        public int nTrickPoints = 0;
        public int nHelperPoints = 0;
        public int nTotalLimitCountRecord = 30;                 //不变值,重置时调取,初始最大可加点值
        public int nLeftLimitCount = 0;                         // 剩余点数
        public int nLeftLimitCountRecord = 0;                   // 缓存剩余值
        public int nAttackPointsRecord = 0;                     // 系加点情况
        public int nTrickPointsRecord = 0;
        public int nHelperPointsRecord = 0;

        public TalentPageDataItem()
        {
            dTalentTempPoints = new Dictionary<int, int>();
            dTalentRecordPoints = new Dictionary<int, int>();
        }

        public void RescoverPointsData()
        {
            // 恢复数据,缓存->临时数据
            if (!bIsChanged)
            {
                return;
            }

            dTalentTempPoints.Clear();
            foreach (int nTalentIdKey in dTalentRecordPoints.Keys)
            {
                int nPoints = dTalentRecordPoints[nTalentIdKey];
                // 本地记录缓存
                dTalentTempPoints[nTalentIdKey] = nPoints;
            }

            nLeftLimitCount = nLeftLimitCountRecord;
            nAttackPoints = nAttackPointsRecord;
            nTrickPoints = nTrickPointsRecord;
            nHelperPoints = nHelperPointsRecord;
            sTempName = sRecordName;
            return;
        }

        public void ResetData()
        {
            // 重置数据
            nAttackPoints = 0;
            nHelperPoints = 0;
            nTrickPoints = 0;
            nLeftLimitCount = nTotalLimitCountRecord;
            dTalentTempPoints.Clear();
            bIsChanged = true;
        }

        public void SetSaveData()
        {
            // 覆盖缓存 临时数据->缓存
            if (!bIsChanged)
            {
                return;
            }

            dTalentRecordPoints.Clear();
            foreach (int nTalentIdKey in dTalentTempPoints.Keys)
            {
                int nPoints = dTalentTempPoints[nTalentIdKey];
                // 本地记录缓存
                dTalentRecordPoints[nTalentIdKey] = nPoints;
            }

            nLeftLimitCountRecord = nLeftLimitCount;
            nAttackPointsRecord = nAttackPoints;
            nTrickPointsRecord = nTrickPoints;
            nHelperPointsRecord = nHelperPoints;
            sRecordName = sTempName;

            return;
        }

        public int GetOneTypeOverLayPoints(int nTalentType)
        {
            if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
            {
                return this.nAttackPoints;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
            {
                return this.nTrickPoints;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
            {
                return this.nHelperPoints;
            }

            return 0;
        }

        public void SetOneTypeOverLayPoints(int nTalentType, int nAddPoint)
        {
            if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
            {
                this.nAttackPoints += nAddPoint;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
            {
                this.nTrickPoints += nAddPoint;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
            {
                this.nHelperPoints += nAddPoint;
            }

            nLeftLimitCount -= nAddPoint;
            return;
        }
    }
}
