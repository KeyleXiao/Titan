/*******************************************************************
** 文件名:	TalentPageDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-13
** 版  本:	1.0
** 描  述:	天赋页界面数据模块(逻辑都在这)
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ASpeedGame.Data.TalentPage;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI;
using USpeedUI.TalentPagePart;
namespace DataCenter
{

    public class TalentPageDataManager
    {
        public List<TalentPageDataItem> talentPageDataManager;
        public int nCurPageSortIndex;
        public TalentPageDataItem uCurTalentPageData;
        public Hashtable allTalentButtonInfo = null;
        public List<int> lTopButtonDataIndex;
        public int nMaxDataIndex = 0;
        public int nMaxSortIndex = 0;
        public int nTalentTotalLimitCount = 0;
        public Dictionary<int, int> dicSameTypeLevelIdPoints;
        public int nPlayerLevel =0;
        public TalentPageDataManager()
        {
            UModelTalentInit();
        }

        public void UModelTalentInit()
        {
            talentPageDataManager = new List<TalentPageDataItem>(20);
            if (allTalentButtonInfo == null)
            {
                allTalentButtonInfo = TalentPage.Instance.GetAllTalentPageTable();
                if (allTalentButtonInfo == null)
                {
                    //ViewConfigManager.Init();// 单独测试 模拟加载
                    allTalentButtonInfo = TalentPage.Instance.GetAllTalentPageTable();
                }

            }
            lTopButtonDataIndex = new List<int>(20);
            dicSameTypeLevelIdPoints = new Dictionary<int, int>(2);
        }

        /////////////////////////////////////////////////////数据载入区///////////////////////////////////////////////////////

        public void ReceiveEntityViewTalentInfo(IntPtr ptrParam, int nPtrLen, ref int nPageNum)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count nCountdata = helper.get<cmd_count>();
            nPageNum = nCountdata.nCount;
            this.nMaxSortIndex = nCountdata.nCount;

            for (int i = 0; i < nCountdata.nCount; ++i)
            {
                NewPagePointsItem(i + 1);
                cmd_TalentPage_Points_Single data = helper.get<cmd_TalentPage_Points_Single>();

                if (uCurTalentPageData == null)
                {
                    continue;
                }

                for (int j = 0; j < data.nTalentNum; j++)
                {

                    int nPoint = data.nTalentPoints[j];
                    int nTalentId = data.nTalentId[j];
                    uCurTalentPageData.dTalentTempPoints[nTalentId] = nPoint;

                    int nTalentType = TalentPage.Instance.GetSingleTalentType(nTalentId);
                    if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
                    {
                        uCurTalentPageData.nAttackPoints += nPoint;
                    }
                    else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
                    {
                        uCurTalentPageData.nTrickPoints += nPoint;
                    }
                    else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
                    {
                        uCurTalentPageData.nHelperPoints += nPoint;
                    }
                }

                if (data.szTalentPageName == "" || data.szTalentPageName==null)
                {
                    uCurTalentPageData.sTempName = "TalentPage" + (i + 1);
                }
                else
                {
                    uCurTalentPageData.sTempName = data.szTalentPageName;
                }

                this.SetTalentPageChange(true);
                uCurTalentPageData.SetSaveData();                                // 必须有数据改变才保存  
                this.SetTalentPageChange(false);
                int nDataBaseIndex = data.nCurPageIndex;
                lTopButtonDataIndex.Add(nDataBaseIndex);                        // 储存数据库页数 因为数据库创建删除都是根据数据库页数来的，避免重复

                if (nMaxDataIndex < nDataBaseIndex)
                {
                    nMaxDataIndex = nDataBaseIndex;
                }
            }

            UpdatePersentLevel();
        }

        public void UpdatePersentLevel()
        {
            if(talentPageDataManager.Count==0)
            {
                return;
            }

            int nTotalLimitCount = GetTalentUseCount();
            nTalentTotalLimitCount = nTotalLimitCount;
            foreach (TalentPageDataItem dataTalentItem in this.talentPageDataManager)
            {
                dataTalentItem.nTotalLimitCountRecord = nTotalLimitCount;
                dataTalentItem.nLeftLimitCount = nTotalLimitCount - dataTalentItem.nAttackPoints - dataTalentItem.nTrickPoints - dataTalentItem.nHelperPoints;
                if (dataTalentItem.nLeftLimitCount<0)
                {
                    dataTalentItem.ResetData();
                }

                dataTalentItem.nLeftLimitCountRecord = dataTalentItem.nLeftLimitCount;
            }
        }

        public int GetTalentUseCount()
        {
            if(nPlayerLevel==0)
            {
                nPlayerLevel = GameLogicAPI.getPlayerLv();
            }

            int nTalentLevelId = this.nPlayerLevel;
            int nTalentUseCountByLevel = TalentPageLevel.Instance.GetTalentLevelUseCount(nTalentLevelId);
            int nTalentMaxCount = GameLogicAPI.getHeroTalentMaxUseCount();
         
            return nTalentUseCountByLevel > nTalentMaxCount ? nTalentMaxCount : nTalentUseCountByLevel;
        }

        public bool SetPlayerLevelChange(int nLevel)
        {
            if (nPlayerLevel == nLevel)
            {
                return false; 
            }

            this.nPlayerLevel = nLevel;
            if (nPlayerLevel == 0)
            {
                return false;
            }

            UpdatePersentLevel();
            return true;
        }
        /////////////////////////////////////////////////////加点减点模块////////////////////////////////////////////////////////

        public int CalculateAddPoint(int nTalentId, ref int nTalentOutType)
        {
            if(!allTalentButtonInfo.ContainsKey(nTalentId))
            {
                return 0;
            }

            SSchemeTalentConfig talentButtonInfo = allTalentButtonInfo[nTalentId] as SSchemeTalentConfig;
        
            int nHasAddPoints = GetTalentPagePointsInfo(nTalentId);

            if (nHasAddPoints >= talentButtonInfo.nMaxAddNum)
            {
                return 0;
            }

            int nTalentType = talentButtonInfo.nTalentType;
            nTalentOutType = nTalentType;
            int nLimitCount = GetOneTypeOverLayPoints(nTalentType);
            // 前置天赋点数未达到要求
            if (nLimitCount < talentButtonInfo.nTalentAllowNum)
            {
                return 0;
            }

            int nTalentLevel = talentButtonInfo.nTalentLevel;
            // 由于是数组，效验长度
            if (nTalentType > TalentPage.DefaultTypeCount || nTalentType < 1 || nTalentLevel > TalentPage.DefaultLevelCount || nTalentLevel < 1)
            {
                UnityEngine.Debug.LogError("CalculateAddPoint: nTalentType or nTalentLevel is out of array");
                return 0;
            }

            // 以下逻辑处理加一点还是加5点
            // 要么是等级1的未加满加1点；要么是等级2的有加过点了加一点，2未加点直接加5点
            int nNeedAddPoint = 0;
            int nLeftTotalNum = GetTalentPageLeftTotalNum();//剩余可用点数
            if (nHasAddPoints > 0 || nTalentLevel % 2 == 0)
            {
                nNeedAddPoint = 1;
            }
            else
            {
                bool bHasPoint = false;

                for (int j = 0; j < TalentPage.DefaultSameLevelCount; j++)           // 判断加几点
                {
                    int nOtherTalentId = TalentPage.Instance.aTalentArray[nTalentType - 1, nTalentLevel - 1, j];
                    if (nOtherTalentId == 0 || nOtherTalentId == nTalentId)
                    {
                        continue;
                    }
                    int nOtherPoints = GetTalentPagePointsInfo(nOtherTalentId);
                    if (nOtherPoints != 0)
                    {
                        bHasPoint = true;
                        break;
                    }

                }

                if ((nTalentLevel % 2 != 0) && !bHasPoint)
                {
                    nNeedAddPoint = talentButtonInfo.nMaxAddNum;
                    if (nLeftTotalNum < nNeedAddPoint)
                    {
                        nNeedAddPoint = 1;
                    }
                }
                else
                {
                    nNeedAddPoint = 1;
                }
            }

            dicSameTypeLevelIdPoints.Clear();
            int nLocalPoints = nHasAddPoints + nNeedAddPoint;                   //当前点击的天赋点数
            int nOtherPoint = 0;                                                //当前点击的同等级天赋点数
            bool bOtherHasPoint = false;
            for (int i = 0; i < TalentPage.DefaultSameLevelCount; i++)          // 处理其他点的减少
            {
                int nOtherTalentId = TalentPage.Instance.aTalentArray[nTalentType - 1, nTalentLevel - 1, i];
                if (nOtherTalentId == 0 || nOtherTalentId == nTalentId)
                {
                    continue;
                }
                int nOtherOldPoints = GetTalentPagePointsInfo(nOtherTalentId);
                if (nLeftTotalNum == 0)
                {

                    if (nOtherOldPoints == 0)
                    {
                        continue;
                    }
                    else
                    {
                        nOtherPoint = nOtherOldPoints - nNeedAddPoint;
                        bOtherHasPoint = true;
                    }
                }
                else
                {
                    if (nLocalPoints + nOtherOldPoints >= talentButtonInfo.nMaxAddNum)
                    {
                        nOtherPoint = talentButtonInfo.nMaxAddNum - nLocalPoints;
                    }
                    else
                    {
                        // 未达到最大点数，不需要变动
                        continue;
                    }
                }

                if (nOtherOldPoints != nOtherPoint)
                {
                    // 有改变就要存
                    SetTalentPagePointsInfo(nOtherTalentId, nOtherPoint);
                    SetOneTypeOverLayPoints(nTalentType, nOtherPoint - nOtherOldPoints);
                    dicSameTypeLevelIdPoints[nOtherTalentId] = nOtherPoint;
                }
                nOtherPoint = 0;
            }

            if (nLeftTotalNum == 0 && !bOtherHasPoint)
            {
                nNeedAddPoint = 0;
            }
            else
            {
                SetTalentPagePointsInfo(nTalentId, nLocalPoints);
                SetOneTypeOverLayPoints(nTalentType, nNeedAddPoint);
                SetTalentPageChange(true);
            }

            return nNeedAddPoint;

        }

        public int CalculateReducePoint(int nTalentId, ref int nTalentType)
        {
            int nOldPoints = GetTalentPagePointsInfo(nTalentId);
            if (nOldPoints <= 0)
            {
                return -1;
            }
            bool bCanReduce = true;
            SSchemeTalentConfig talentButtonInfo = allTalentButtonInfo[nTalentId] as SSchemeTalentConfig;
            if (talentButtonInfo.nTalentLevel != (int)(TalentLevel.ENUM_TalentLevel_Max - 1))      // 处理同类型的上一等级是否存在点数
            {
                int nNextLevel = talentButtonInfo.nTalentLevel + 1;
                for (int j = 0; j < TalentPage.DefaultSameLevelCount; j++)
                {
                    int nNextLevelTalentId = TalentPage.Instance.aTalentArray[talentButtonInfo.nTalentType - 1, nNextLevel - 1, j];
                    if (nNextLevelTalentId == 0)
                    {
                        continue;
                    }
                    int nNextLevelPoints = GetTalentPagePointsInfo(nNextLevelTalentId);
                    if (nNextLevelPoints > 0)
                    {
                        bCanReduce = false;
                        break;
                    }

                }
            }
            if (!bCanReduce)
            {
                return -1;
            }
            nTalentType = talentButtonInfo.nTalentType;
            nOldPoints--;
            SetTalentPagePointsInfo(nTalentId, nOldPoints);
            SetTalentPageChange(true);
            SetOneTypeOverLayPoints(talentButtonInfo.nTalentType, -1);
            return nOldPoints;
        }
        /////////////////////////////////////////////////////页的新增切换和显示////////////////////////////////////////////////////////

        // 玩家点击某页的时候或者是初始化某页的显示的时候调用,删除的时候也调用.负责点数的显示和页面的切换。关于切换页面显示方面都调用这个
        public void SetCurPageIndex(int nPageIndex)
        {
            if (talentPageDataManager==null)
            {
                return;
            }

            if (nPageIndex <= 0 || nPageIndex > talentPageDataManager.Count)
            {
                return;
            }

            this.nCurPageSortIndex = nPageIndex;
            uCurTalentPageData = this.talentPageDataManager[this.nCurPageSortIndex - 1];

        }
        // 负责new,创建新的单页数据
        public void NewPagePointsItem(int nPageIndex)
        {
            TalentPageDataItem talentMenuPagePoints = new TalentPageDataItem();
            talentMenuPagePoints.nCreateIndex = nPageIndex;
            talentPageDataManager.Add(talentMenuPagePoints);
            this.uCurTalentPageData = talentMenuPagePoints;
        }

        /////////////////////////////////////////////////////数据缓存模块////////////////////////////////////////////////////////////

        public int GetTalentPagePointsInfo(int nTalentId)
        {
            if (uCurTalentPageData == null)
            {
                return 0;
            }

            if (uCurTalentPageData.dTalentTempPoints == null)
            {
                return 0;
            }

            if (!uCurTalentPageData.dTalentTempPoints.ContainsKey(nTalentId))
            {
                return 0;
            }

            return uCurTalentPageData.dTalentTempPoints[nTalentId];
        }

        public void SetTalentPagePointsInfo(int nTalentId, int nTalentPoints)
        {
            if (uCurTalentPageData == null)
            {
                return;
            }

            if (uCurTalentPageData.dTalentTempPoints == null)
            {
                return;
            }

            uCurTalentPageData.dTalentTempPoints[nTalentId] = nTalentPoints;
        }

        // 每种类型的加点数总数操作;
        public int GetOneTypeOverLayPoints(int nTalentType)
        {
            if (uCurTalentPageData == null)
            {
                return 0;
            }

            return uCurTalentPageData.GetOneTypeOverLayPoints(nTalentType);
        }

        public void SetOneTypeOverLayPoints(int nTalentType, int nAddPoint)
        {
            if (uCurTalentPageData == null)
            {
                return;
            }

            uCurTalentPageData.SetOneTypeOverLayPoints(nTalentType, nAddPoint);
        }

        public int GetTalentPageLeftTotalNum()
        {
            if (uCurTalentPageData == null)
            {
                return 0;
            }

            return this.uCurTalentPageData.nLeftLimitCount;
        }

        public void SetTalentPageChange(bool bChange)
        {
            if (uCurTalentPageData == null)
            {
                return;
            }

            TalentPageWnd talentPageWnd = ((TalentPageWnd)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYER_TALENTPAGE));
            talentPageWnd.SetTalentIsChange(bChange);

            uCurTalentPageData.bIsChanged = bChange;
            return;
        }

        public bool GetTalentPageChange()
        {
            if (uCurTalentPageData == null)
            {
                return false;
            }

            return uCurTalentPageData.bIsChanged;
        }
        // 清除当前页的所有数据
        public void ResetPointsData()
        {
            if (uCurTalentPageData == null)
            {
                return;
            }

            this.uCurTalentPageData.ResetData();
        }

        // 删除当前页的所有数据,整个pagepoint对象删掉
        public void DeleteTalentPagePointsInfo(int nPageIndex)
        {
            if (talentPageDataManager.Count < nPageIndex || lTopButtonDataIndex.Count < nPageIndex)
            {
                UnityEngine.Debug.LogError("DeleteTalentPagePointsInfo: Out Of ListArray");
                return;
            }

            this.talentPageDataManager.RemoveAt(nPageIndex - 1);
            this.lTopButtonDataIndex.RemoveAt(nPageIndex - 1);
            return;
        }
        /////////////////////////////////////////////////////保存删除新增恢复重置功能区////////////////////////////////////////////////////////


        // 设置新页的逻辑,需要保存;
        public void SetNewPage(int nPageIndex)
        {
            // 先保存当前页再保存新页
            this.nMaxDataIndex++;
            this.nMaxSortIndex++;
            lTopButtonDataIndex.Add(nMaxDataIndex);// 储存数据库页数

            // 新页new操作
            NewPagePointsItem(nPageIndex);
            if (uCurTalentPageData == null)
            {
                return;
            }
            // 赋值一系列初始新页数据
            uCurTalentPageData.nTotalLimitCountRecord = nTalentTotalLimitCount;
            uCurTalentPageData.nLeftLimitCount = nTalentTotalLimitCount;
            uCurTalentPageData.nLeftLimitCountRecord = nTalentTotalLimitCount;
            uCurTalentPageData.sRecordName = "TalentPage" + nPageIndex;
            uCurTalentPageData.sTempName = "TalentPage" + nPageIndex;
            SetTalentPageChange(true);
            SetCurPageIndex(nPageIndex);
        }

        public void SetRescoverCurPage()
        {
            // 恢复后回归不改变状态
            RescoverPointsData();
            SetTalentPageChange(false);
        }

        public void SetResetCurPage()
        {
            // 不仅重置显示0，还要清空当前页的储存的加点数据
            SetTalentPageChange(true);
            ResetPointsData();
        }

        public void RescoverPointsData()
        {
            if (uCurTalentPageData == null)
            {
                return;
            }
            uCurTalentPageData.RescoverPointsData();
        }

        public void SetName(string sName)
        {
            if (uCurTalentPageData == null)
            {
                return;
            }
            uCurTalentPageData.sTempName = sName;
        }
        public void SavePointsData()
        {
            if (uCurTalentPageData == null)
            {
                return;
            }
            uCurTalentPageData.SetSaveData();
        }

        // 删除页逻辑,不仅要显示删除，同时点数也要删除;
        // 目前删除的逻辑：将当前页和序号页分离。this.nCurPageSortIndex是序号，nViewIndex是创建的真是当前页，储存到数据库的索引，会增大到int最大值
        public void SetDeletePage(ref cmd_Entity_Talent_Page_Detele cmdPointDelete)
        {
            int nDeletePageDataIndex = GetDataIndex(nCurPageSortIndex);
            var nMaxIndex = nMaxSortIndex;
            int nNextIndex = 0;
            DeleteTalentPagePointsInfo(this.nCurPageSortIndex);

            if (this.nCurPageSortIndex == nMaxIndex)
            {
                nNextIndex = nCurPageSortIndex - 1;
            }
            else if (this.nCurPageSortIndex < nMaxIndex)
            {
                nNextIndex = nCurPageSortIndex;

            }
            if (nNextIndex != 0)
            {
                SetCurPageIndex(nNextIndex);
            }

            nMaxSortIndex--;
            cmdPointDelete.nCurTalentPage = nDeletePageDataIndex;
            return;
        }

        // 保存操作，发送到客户端
        public bool SetSaveSingleTalentPage(ref cmd_TalentPage_Points_Single cmdSinglePoint)
        {
            // 当前页的名字和加点数都没修改直接返回
            if (!uCurTalentPageData.bIsChanged)
            {
                return false;
            }

            if (uCurTalentPageData == null)
            {
                return false;
            }

            if (uCurTalentPageData.dTalentTempPoints == null)
            {
                return false;
            }

            SavePointsData();
            int nTalentCount = 0;
            int nDicTempCount = uCurTalentPageData.dTalentTempPoints.Count;

            cmdSinglePoint.nTalentId = new int[nDicTempCount];
            cmdSinglePoint.nTalentPoints = new int[nDicTempCount];

            foreach (int nTalentIdKey in uCurTalentPageData.dTalentTempPoints.Keys)
            {
                if (uCurTalentPageData.dTalentTempPoints[nTalentIdKey] == 0)
                {
                    continue;
                }

                int nPoints = uCurTalentPageData.dTalentTempPoints[nTalentIdKey];
                cmdSinglePoint.nTalentId[nTalentCount] = nTalentIdKey;
                cmdSinglePoint.nTalentPoints[nTalentCount] = nPoints;
                nTalentCount++;
            }

            SetTalentPageChange(false);
            int nDataIndex = GetDataIndex(this.nCurPageSortIndex);
            cmdSinglePoint.nCurPageIndex = nDataIndex;
            cmdSinglePoint.nTalentNum = nTalentCount;
            cmdSinglePoint.szTalentPageName = this.uCurTalentPageData.sTempName;

            return true;
        }

        public int GetDataIndex(int nSortIndexPageIndex)
        {
            if (nSortIndexPageIndex<=0||lTopButtonDataIndex.Count < nSortIndexPageIndex)
            {
                return 0;
            }

            return this.lTopButtonDataIndex[nSortIndexPageIndex - 1];
        }

        public int GetDataIndex()
        {
            if (this.nCurPageSortIndex <= 0 || lTopButtonDataIndex.Count < this.nCurPageSortIndex)
            {
                return 0;
            }

            return this.lTopButtonDataIndex[this.nCurPageSortIndex - 1];
        }

        public int GetMaxSortIndex()
        {
            return nMaxSortIndex;
        }

        public int GetMaxDataIndex()
        {
            return nMaxDataIndex;
        }

        public TalentChangePageModelInfo GetAllTalentViewData(int nPageIndex)
        {
            if (this.talentPageDataManager == null)
            {
                return null;
            }

            if (nPageIndex == -1)
            {
                nPageIndex = nCurPageSortIndex;
            }

            if (nPageIndex <= 0 || talentPageDataManager.Count < nPageIndex)
            {
                return null;
            }

            if (talentPageDataManager[nPageIndex - 1] == null)
            {
                return null;
            }

            TalentChangePageModelInfo talentPageInfo = new TalentChangePageModelInfo();
            TalentPageDataItem uCurTalentPageData = talentPageDataManager[nPageIndex - 1];
            talentPageInfo.szTalentPageName = uCurTalentPageData.sTempName;
            talentPageInfo.nAttackPointAll = uCurTalentPageData.nAttackPoints;
            talentPageInfo.nTrickPointAll = uCurTalentPageData.nTrickPoints;
            talentPageInfo.nHelpPointAll = uCurTalentPageData.nHelperPoints;
            talentPageInfo.nLeftPoints = uCurTalentPageData.nLeftLimitCount;

            return talentPageInfo;
        }

        public bool IsCurTalentPageEmptyPoints()
        {
            if (uCurTalentPageData == null)
            {
                return false;
            }

            if (uCurTalentPageData.nLeftLimitCount > 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        public Dictionary<int,int> GetTalentPageTempPoints()
        {
            if (uCurTalentPageData == null)
            {
                return null;
            }

            return uCurTalentPageData.dTalentTempPoints;
        }
    }
}
