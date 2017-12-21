/*******************************************************************
** 文件名:	RunePageDataManager.cs 
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
using ASpeedGame.Data.RunePage;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI.PlayerFrame;
using USpeedUI;
using UnityEngine;
using U3D_Render.Common;
using Data.SideButtonConfig;
namespace DataCenter
{
    public class RunePageDataManager
    {
        public List<RunePageDataItem> runePageDataManager;
        public List<RunePageDataItem> runePageDataManagerSelfRecord;// 自己数据的备份
        public int nCurPageSortIndex;
        public RunePageDataItem uCurRunePageData;
        public Hashtable allRuneButtonInfo = null;
        public List<int> lTopButtonDataIndex;
        public int nMaxDataIndex = 0;
        public int nMaxSortIndex = 0;
        public Dictionary<int, List<RuneItemInfo>> dicRuneInlayTypeStoreInfo;
        public Dictionary<int, List<RuneItemInfo>> dicRuneLevelStoreInfo;
        public Dictionary<int, List<RuneItemInfo>> dicRuneInlayTypeLeftStoreInfo;
        public int nCurPropertyType = 0;
        public List<int> lRuneLevelState;
        public const int nRuneMaxCount = 7;
        public RunePageDataManager()
        {
            UModelRuneInit();
        }

        public void Init()
        {
            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void Clear()
        {
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:   //等级变化
                    {
                        UpdateRuneButton();
                    }
                    break;
                default:
                    break;
            }
        }

        public void UModelRuneInit()
        {
            runePageDataManager = new List<RunePageDataItem>(20);
            runePageDataManagerSelfRecord = new List<RunePageDataItem>(20);
            if (allRuneButtonInfo == null)
            {
                allRuneButtonInfo = RunePage.Instance.GetAllRunePageTable();
                if (allRuneButtonInfo == null)
                {
                    //ViewConfigManager.Init();// 单独测试 模拟加载
                    allRuneButtonInfo = RunePage.Instance.GetAllRunePageTable();
                }
            }

            lTopButtonDataIndex = new List<int>(20);
            dicRuneInlayTypeStoreInfo = new Dictionary<int, List<RuneItemInfo>>();
            dicRuneLevelStoreInfo = new Dictionary<int, List<RuneItemInfo>>();
            dicRuneInlayTypeLeftStoreInfo = new Dictionary<int, List<RuneItemInfo>>();
            lRuneLevelState = new List<int>(3);
            lRuneLevelState.Add((int)RuneLevel.ENUM_RUNELEVEL_1);
            lRuneLevelState.Add((int)RuneLevel.ENUM_RUNELEVEL_2);
            lRuneLevelState.Add((int)RuneLevel.ENUM_RUNELEVEL_3);
            nCurPropertyType = (int)RunePropertyType.ENUM_RUNEPROPERTYTYPE_ALL;
            List<RuneItemInfo> listRuneItemInfoHallmark = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoGlyph = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoSigil = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoCream = new List<RuneItemInfo>();
            dicRuneInlayTypeStoreInfo[(int)RuneType.ENUM_RUNETYPE_HALLMARK] = listRuneItemInfoHallmark;
            dicRuneInlayTypeStoreInfo[(int)RuneType.ENUM_RUNETYPE_GLYPH] = listRuneItemInfoGlyph;
            dicRuneInlayTypeStoreInfo[(int)RuneType.ENUM_RUNETYPE_SIGIL] = listRuneItemInfoSigil;
            dicRuneInlayTypeStoreInfo[(int)RuneType.ENUM_RUNETYPE_CREAM] = listRuneItemInfoCream;

            List<RuneItemInfo> listRuneItemInfoHallmarkEx = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoGlyphEx = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoSigilEx = new List<RuneItemInfo>();
            List<RuneItemInfo> listRuneItemInfoCreamEx = new List<RuneItemInfo>();

            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_HALLMARK] = listRuneItemInfoHallmarkEx;
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_GLYPH] = listRuneItemInfoGlyphEx;
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_SIGIL] = listRuneItemInfoSigilEx;
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_CREAM] = listRuneItemInfoCreamEx;
        }

        /////////////////////////////////////////////////////数据载入区///////////////////////////////////////////////////////

        public bool ReceiveSelfEntityViewRuneInfo(ref IntPtr ptrParam, int nPtrLen)
        {
            
            runePageDataManager.Clear();
            // 先压出个人全部符文store 
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count storeCountData = helper.get<cmd_count>();
            int nStoreCount = storeCountData.nCount;
            for (int k = 0; k < nStoreCount; k++)
            {
                cmd_Rune_InStore_Item storeItemdata = helper.get<cmd_Rune_InStore_Item>();
                int nRuneId = storeItemdata.nRuneId;
                SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[nRuneId] as SSchemeRuneConfig;
                if (runeConfig == null)
                {
                    continue;
                }

                RuneItemInfo runeItemInfo = new RuneItemInfo();
                runeItemInfo.nRuneId = nRuneId;
                runeItemInfo.nRuneNum = storeItemdata.nRuneNum;

                dicRuneInlayTypeStoreInfo[(int)runeConfig.nRuneInlayType].Add(runeItemInfo);
            }

            // 再压出每个符文页的详细信息
            cmd_count runePagedata = helper.get<cmd_count>();
            int nRunePageCount = runePagedata.nCount;
            this.nMaxSortIndex = nRunePageCount;
            for (int i = 0; i < runePagedata.nCount; ++i)
            {
                NewPagePointsItem(i + 1);
                if (uCurRunePageData == null)
                {
                    continue;
                }

                cmd_Rune_Single_PageInfo data = helper.get<cmd_Rune_Single_PageInfo>();
                for (int j = 0; j < data.nRuneGroove.Length; j++)
                {
                    int nRuneId = data.nRuneGroove[j];
                    SetRunePagePointsInfo(j + 1, nRuneId, true);
                }

                if (data.sRunePageName == "")
                {
                    uCurRunePageData.sTempName = "RunePage" + (i + 1);
                }
                else
                {
                    uCurRunePageData.sTempName = data.sRunePageName;
                }

                this.SetRunePageChange(true,true);
                uCurRunePageData.SetSaveData();                                // 必须有数据改变才保存  
                this.SetRunePageChange(false, true);
                int nDataBaseIndex = data.nPageIndex;
                lTopButtonDataIndex.Add(nDataBaseIndex);                        // 储存数据库页数 因为数据库创建删除都是根据数据库页数来的，避免重复

                if (nMaxDataIndex < nDataBaseIndex)
                {
                    nMaxDataIndex = nDataBaseIndex;
                }
            }

            uCurRunePageData = runePageDataManager[0];
            CopySelfRunePageDateManagerInfo();
            UpdateRuneInlayTypeLeftStore();
            SetRuneChangeMessage();
            return true;
        }

        public bool ReceiveOtherPlayerEntityViewRuneInfo(int nOtherPlayerId)
        {
            OtherActorRuneInfo otherInfo = LogicDataCenter.playerSystemDataManager.GetActorAllRune(nOtherPlayerId);
            if (otherInfo == null)
            {
                return false;
            }

            // 再压出每个符文页的详细信息
            int nRunePageCount = otherInfo.totalRune.nPageNum;
            if (nRunePageCount <= 0)
            {
                return false;
            }

            this.nMaxSortIndex = nRunePageCount;
            for (int i = 0; i < nRunePageCount; ++i)
            {
                NewPagePointsItem(i + 1);
                if (uCurRunePageData == null)
                {
                    continue;
                }

                if (i < 0 || i >= otherInfo.totalRunePage.Count)
                {
                    continue;
                }

                cmd_Rune_Single_PageInfo data = otherInfo.totalRunePage[i];
                for (int j = 0; j < data.nRuneGroove.Length; j++)
                {
                    int nRuneId = data.nRuneGroove[j];
                    SetRunePagePointsInfo(j + 1, nRuneId, true);
                }

                if (data.sRunePageName == "")
                {
                    uCurRunePageData.sTempName = "RunePage" + (i + 1);
                }
                else
                {
                    uCurRunePageData.sTempName = data.sRunePageName;
                }

                this.SetRunePageChange(true);
                uCurRunePageData.SetSaveData();
                this.SetRunePageChange(false);
            }

            return true;
        }

        public bool RescoverSelfRunePageDateManagerInfo()
        {
            if (runePageDataManagerSelfRecord == null || runePageDataManagerSelfRecord.Count <= 0 )
            {
                return false;
            }

            if (runePageDataManager.Count <= 0)
            {
                for (int i = 0; i < runePageDataManagerSelfRecord.Count; ++i)
                {
                    if (runePageDataManagerSelfRecord[i] == null)
                    {
                        continue;
                    }

                    runePageDataManager.Add(runePageDataManagerSelfRecord[i]);
                }
            }

            if (runePageDataManager.Count > 0)
            {
                this.uCurRunePageData = runePageDataManager[0];
				return true;       
            }
            else
            {
                return false;
            }
        }

        public void CopySelfRunePageDateManagerInfo()
        {
            if (runePageDataManager == null || runePageDataManager.Count <= 0)
            {
                return;
            }

            for (int i = 0; i < runePageDataManager.Count; ++i)
            {
                if (runePageDataManager[i] == null)
                {
                    continue;
                }

                runePageDataManagerSelfRecord.Add(runePageDataManager[i]);
            }        
        }

        public void UpdateRuneStoreData(cmd_Rune_InStore_Item storeItemdata)
        {
            int nRuneId = storeItemdata.nRuneId;
            SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[nRuneId] as SSchemeRuneConfig;
            if (runeConfig == null)
            {
                return;
            }

            if (!dicRuneInlayTypeStoreInfo.ContainsKey((int)runeConfig.nRuneInlayType))
            {
                return;
            }

            int nRuneInlayType = (int)runeConfig.nRuneInlayType;
            bool bHasRune = false;
            foreach (RuneItemInfo runeItemInfo in dicRuneInlayTypeStoreInfo[nRuneInlayType])
            {
                if (runeItemInfo.nRuneId == nRuneId)
                {
                    runeItemInfo.nRuneNum += storeItemdata.nRuneNum;
                    bHasRune = true;
                    break;
                }
            }

            if (!bHasRune)
            {
                RuneItemInfo runeItemInfo = new RuneItemInfo();
                runeItemInfo.nRuneId = nRuneId;
                runeItemInfo.nRuneNum = storeItemdata.nRuneNum;

                dicRuneInlayTypeStoreInfo[nRuneInlayType].Add(runeItemInfo);
            }

            UpdateRuneInlayTypeLeftStore();
            SetRuneChangeMessage();
        }

        // 更新左侧剩余符文缓存 
        public void UpdateRuneInlayTypeLeftStore()
        {
            return;
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_HALLMARK].Clear();
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_GLYPH].Clear();
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_SIGIL].Clear();
            dicRuneInlayTypeLeftStoreInfo[(int)RuneType.ENUM_RUNETYPE_CREAM].Clear();
            foreach (int nRuneInlayType in dicRuneInlayTypeStoreInfo.Keys)
            {
                foreach (RuneItemInfo runeItemInfo in dicRuneInlayTypeStoreInfo[nRuneInlayType])
                {
                    int nSlotRuneCount = 0;
                    foreach (int nRuneId in uCurRunePageData.dRuneTempSlotInfo.Values)
                    {
                        if (nRuneId == runeItemInfo.nRuneId)
                            nSlotRuneCount++;
                    }

                    int nRuneCount = runeItemInfo.nRuneNum - nSlotRuneCount;
                    if (nRuneCount <= 0)
                    {
                        continue;
                    }

                    RuneItemInfo newRuneItemInfo = new RuneItemInfo();
                    newRuneItemInfo.nRuneId = runeItemInfo.nRuneId;
                    newRuneItemInfo.nRuneNum = nRuneCount;
                    dicRuneInlayTypeLeftStoreInfo[nRuneInlayType].Add(newRuneItemInfo);
                }
            }

            SetRuneChangeMessage();
        }
        /////////////////////////////////////////////////////页的新增切换和显示////////////////////////////////////////////////////////

        // 玩家点击某页的时候或者是初始化某页的显示的时候调用,删除的时候也调用.负责点数的显示和页面的切换。关于切换页面显示方面都调用这个
        public void SetCurPageIndex(int nPageIndex)
        {
            if (runePageDataManager == null)
            {
                return;
            }

            if (runePageDataManager.Count < nPageIndex || nPageIndex <= 0)
            {
                return;
            }

            this.nCurPageSortIndex = nPageIndex;
            uCurRunePageData = this.runePageDataManager[this.nCurPageSortIndex - 1];

        }
        // 负责new,创建新的单页数据
        public void NewPagePointsItem(int nPageIndex)
        {
            RunePageDataItem talentMenuPagePoints = new RunePageDataItem();
            talentMenuPagePoints.nCreateIndex = nPageIndex;
            runePageDataManager.Add(talentMenuPagePoints);
            this.uCurRunePageData = talentMenuPagePoints;
        }

        /////////////////////////////////////////////////////数据缓存模块////////////////////////////////////////////////////////////

        public int GetRunePagePointsInfo(int nRuneId)
        {
            if (uCurRunePageData == null)
            {
                return 0;
            }

            if (uCurRunePageData.dRuneTempSlotInfo == null)
            {
                return 0;
            }

            if (!uCurRunePageData.dRuneTempSlotInfo.ContainsKey(nRuneId))
            {
                return 0;
            }

            return uCurRunePageData.dRuneTempSlotInfo[nRuneId];
        }

        public void SetRunePagePointsInfo(int nSlotId, int nRuneId, bool bAddOrReduce)
        {
            if (uCurRunePageData == null)
            {
                return;
            }

            if (uCurRunePageData.dRuneTempSlotInfo == null)
            {
                return;
            }

            if (!bAddOrReduce)
            {
                nRuneId = uCurRunePageData.dRuneTempSlotInfo[nSlotId];
            }
            else
            {
                uCurRunePageData.dRuneTempSlotInfo[nSlotId] = nRuneId;
            }

            SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[nRuneId] as SSchemeRuneConfig;
            if (runeConfig == null)
            {
                return;
            }
            if (!bAddOrReduce)
            {
                uCurRunePageData.dRuneTempSlotInfo[nSlotId] = 0;
            }

            UBB.toHtml(ref runeConfig.szRuneName, UBB_FORMAT_TYPE.UGUI);
            SetDoubleNameProperty(runeConfig.szRuneName, runeConfig.szParametersValueArray, bAddOrReduce, runeConfig.nPropertyNameType);
            SetRunePageChange(true);
        }

        // 得到缓存旧数据某个天赋点的信息
        public int GetRuneSavePagePointsInfo(int nRuneId)
        {
            if (uCurRunePageData == null)
            {
                return 0;
            }

            if (uCurRunePageData.dRuneRecordSlotPoints == null)
            {
                return 0;
            }

            if (!uCurRunePageData.dRuneRecordSlotPoints.ContainsKey(nRuneId))
            {
                return 0;
            }

            return uCurRunePageData.dRuneRecordSlotPoints[nRuneId];
        }

        public void SetRunePageChange(bool bChange,bool bFirstChange = false)
        {
            if (uCurRunePageData == null)
            {
                return;
            }

            //RunePageWnd runePageWnd = ((RunePageWnd)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYER_RUNETPAGE));
            UPlayerFrame playerFrameWnd = (UPlayerFrame)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYERFRAME);
            RunePageWnd runePageWnd = playerFrameWnd.GetSubWnd(PlayerSubWndType.WND_RUNE) as RunePageWnd;
            runePageWnd.SetTalentIsChange(bChange);

            uCurRunePageData.bIsChanged = bChange;

            if (bChange & !bFirstChange)
            {
                SetRuneChangeMessage();
            }

            return;
        }

        public bool GetRunePageChange()
        {
            if (uCurRunePageData == null)
            {
                return false;
            }

            return uCurRunePageData.bIsChanged;
        }
        // 清除当前页的所有数据
        public void ResetPointsData()
        {
            if (uCurRunePageData == null)
            {
                return;
            }

            this.uCurRunePageData.ResetData();
        }

        // 删除当前页的所有数据,整个pagepoint对象删掉
        public void DeleteRunePagePointsInfo(int nPageIndex)
        {
            if (nPageIndex <= 0 || runePageDataManager.Count < nPageIndex || lTopButtonDataIndex.Count < nPageIndex || runePageDataManagerSelfRecord.Count < nPageIndex)
            {
                UnityEngine.Debug.LogError("DeleteRunePagePointsInfo: Out Of ListArray");
                return;
            }

            nPageIndex -= 1;
            this.runePageDataManager.RemoveAt(nPageIndex);
            this.runePageDataManagerSelfRecord.RemoveAt(nPageIndex);
            this.lTopButtonDataIndex.RemoveAt(nPageIndex);
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

            if (nPageIndex > 0 && nPageIndex <= 20)
            {
                runePageDataManagerSelfRecord.Add(runePageDataManager[nPageIndex - 1]);
            }

            if (uCurRunePageData == null)
            {
                return;
            }
            // 赋值一系列初始新页数据

            uCurRunePageData.sRecordName = "RunePage" + nPageIndex;
            uCurRunePageData.sTempName = "RunePage" + nPageIndex;
            SetRunePageChange(true);
            SetCurPageIndex(nPageIndex);
        }

        public void SetRescoverCurPage()
        {
            // 恢复后回归不改变状态
            RescoverPointsData();
            SetRunePageChange(false);
        }

        public void SetResetCurPage()
        {
            // 不仅重置显示0，还要清空当前页的储存的加点数据
            SetRunePageChange(true);
            ResetPointsData();
        }

        public void RescoverPointsData()
        {
            if (uCurRunePageData == null)
            {
                return;
            }
            uCurRunePageData.RescoverPointsData();
        }

        public void SetName(string sName)
        {
            if (uCurRunePageData == null)
            {
                return;
            }
            uCurRunePageData.sTempName = sName;
        }
        public void SavePointsData()
        {
            if (uCurRunePageData == null)
            {
                return;
            }
            uCurRunePageData.SetSaveData();
        }

        // 删除页逻辑,不仅要显示删除，同时点数也要删除;
        // 目前删除的逻辑：将当前页和序号页分离。this.nCurPageSortIndex是序号，nViewIndex是创建的真是当前页，储存到数据库的索引，会增大到int最大值
        public void SetDeletePage(ref cmd_Delete_Rune_Page cmdPointDelete)
        {
            int nDeletePageDataIndex = GetDataIndex(nCurPageSortIndex);
            var nMaxIndex = nMaxSortIndex;
            int nNextIndex = 0;
            DeleteRunePagePointsInfo(this.nCurPageSortIndex);

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
            cmdPointDelete.nPageIdx = nDeletePageDataIndex;
            return;
        }

        // 保存操作，发送到客户端
        public bool SetSaveSingleRunePage(ref cmd_Rune_Single_PageInfo cmdSinglePoint)
        {
            // 当前页的名字和加点数都没修改直接返回
            if (!uCurRunePageData.bIsChanged)
            {
                return false;
            }

            if (uCurRunePageData == null)
            {
                return false;
            }

            if (uCurRunePageData.dRuneTempSlotInfo == null)
            {
                return false;
            }

            SavePointsData();

            int nDicTempCount = uCurRunePageData.dRuneTempSlotInfo.Count;

            cmdSinglePoint.nRuneGroove = new int[nRuneMaxCount];
            int nRuneNum = 0;
            foreach (int nRuneIdKey in uCurRunePageData.dRuneTempSlotInfo.Keys)
            {
                if (uCurRunePageData.dRuneTempSlotInfo[nRuneIdKey] == 0)
                {
                    continue;
                }
                nRuneNum++;
                int nPoints = uCurRunePageData.dRuneTempSlotInfo[nRuneIdKey];
                cmdSinglePoint.nRuneGroove[nRuneIdKey - 1] = nPoints;
            }

            SetRunePageChange(false);
            int nDataIndex = GetDataIndex(this.nCurPageSortIndex);
            cmdSinglePoint.nPageIndex = nDataIndex;
            cmdSinglePoint.nRuneNum = nRuneNum;
            cmdSinglePoint.sRunePageName = this.uCurRunePageData.sTempName;

            return true;
        }

        public int GetDataIndex(int nSortIndexPageIndex)
        {
            if (nSortIndexPageIndex <= 0 || lTopButtonDataIndex.Count < nSortIndexPageIndex)
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

        public void SetRunePropertyType(int nPropertyType)
        {
            this.nCurPropertyType = nPropertyType;
        }

        public int GetRunePropertyType()
        {
            return nCurPropertyType;
        }

        public void SetRuneLevelState(int nLevelSlotId, bool bIsOn)
        {
            if (bIsOn)
            {
                if (!lRuneLevelState.Contains(nLevelSlotId))
                {
                    lRuneLevelState.Add(nLevelSlotId);
                }
            }
            else
            {
                if (lRuneLevelState.Contains(nLevelSlotId))
                {
                    lRuneLevelState.Remove(nLevelSlotId);
                }
            }
        }

        public List<int> GetRuneLevelState()
        {
            return this.lRuneLevelState;
        }

        public Dictionary<int, List<RuneItemInfo>> GetInlayTypeAllStore()
        {
            return this.dicRuneInlayTypeStoreInfo;
        }

        public Dictionary<int, int> GetRuneSlotInfo()
        {
            if (this.uCurRunePageData == null)
            {
                return null;
            }

            return this.uCurRunePageData.dRuneTempSlotInfo;
        }

        public bool IsAddOperater(int nType)
        {
            if ((nType & (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_ADD) == (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_ADD)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool IsPersentOperater(int nType)
        {
            if ((nType & (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_PERSENT) == (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_PERSENT)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool IsEnlargeHundredOperater(int nType)
        {
            if ((nType & (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_ENLARGEHUNDRED) == (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_ENLARGEHUNDRED)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool IsRuneGrow(int nType)
        {
            if ((nType & (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_GROW) == (int)RunePropertyNameType.RUNEPROPERTYNAMETYPE_GROW)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetDoubleNameProperty(string sRuneName, string sRuneValue, bool bAddOrReduce, int nType)
        {
            string[] sRuneValueArray = ((string)(sRuneValue)).Split(';');
            string[] sRuneNameArray = null;
            if (sRuneValueArray.Length > 1)
            {
                sRuneName = sRuneName.Replace("\n", "");
                sRuneNameArray = ((string)(sRuneName)).Split(';');
            }

            if (sRuneNameArray != null)
            {
                if (sRuneNameArray.Length != sRuneValueArray.Length)
                {
                    Debug.LogWarning("SetRunePagePointsInfo : runeinfo.csv doubleproperty config is wrong");
                    return;
                }

            }

            for (int i = 0; i < sRuneValueArray.Length; i++)
            {
                string sPropertyName = "";
                if (sRuneNameArray == null)
                {
                    sPropertyName = sRuneName;
                }
                else
                {
                    sPropertyName = sRuneNameArray[i];
                }

                double nRuneProValue = (double.Parse(sRuneValueArray[i]));

                if (!bAddOrReduce)
                {
                    nRuneProValue = -nRuneProValue;
                }

                double nOldValue = 0;
                if (uCurRunePageData.dRuneTypeNameTempCount.ContainsKey(sPropertyName))
                {
                    nOldValue = uCurRunePageData.dRuneTypeNameTempCount[sPropertyName].typeValue;
                }
                else
                {
                    RuneTypeNameInfo nameInfo = new RuneTypeNameInfo();
                    nameInfo.nType = nType;
                    uCurRunePageData.dRuneTypeNameTempCount[sPropertyName] = nameInfo;
                }

                uCurRunePageData.dRuneTypeNameTempCount[sPropertyName].typeValue = nOldValue + nRuneProValue;
            }
        }

        public string GetRuneOnePageName(int nPageIndex)
        {
            if (this.runePageDataManager == null)
            {
                return null;
            }

            if (runePageDataManager[nPageIndex - 1] == null)
            {
                return null;
            }

            return runePageDataManager[nPageIndex - 1].sTempName;
        }

        public int GetRunePageCount()
        {
            if(runePageDataManager == null)
            {
                return 0;
            }

            return runePageDataManager.Count;
        }

        public void SetRuneChangeMessage()
        {
            UpdateRuneButton();
        }

        public bool IsNeedShowRuneAddTip()
        {
            if (uCurRunePageData == null || uCurRunePageData.dRuneTempSlotInfo == null)
            {
                return false;
            }

            foreach (int nRuneIdKey in uCurRunePageData.dRuneTempSlotInfo.Keys)
            {
                int nPoints = uCurRunePageData.dRuneTempSlotInfo[nRuneIdKey];
                if (nPoints > 0)
                {
                    continue;
                }

                RuneType eRuneType = GetRuneTypeByRuneSlotID(nRuneIdKey);
                if (dicRuneInlayTypeLeftStoreInfo.ContainsKey((int)eRuneType))
                {
                    if (dicRuneInlayTypeLeftStoreInfo[(int)eRuneType].Count > 0)
                        return true;
                }
            }

            return false;
        }

        public RuneType GetRuneTypeByRuneSlotID(int nSlotID)
        {
            RuneType eRuneType;
            switch(nSlotID)
            {
                case 1:
                case 2:
                    {
                        eRuneType = RuneType.ENUM_RUNETYPE_HALLMARK;
                    }
                    break;
                case 3:
                case 4  :
                    {
                        eRuneType = RuneType.ENUM_RUNETYPE_SIGIL;
                    }
                    break;
                case 5:
                case 6:
                    {
                        eRuneType = RuneType.ENUM_RUNETYPE_GLYPH;
                    }
                    break;
                case 7:
                    {
                        eRuneType = RuneType.ENUM_RUNETYPE_CREAM;
                    }
                    break;
                    default:
                        {
                            eRuneType = 0;
                        }
                    break;
            }

            return eRuneType;
        }

        // 符文提示
        private void UpdateRuneButton()
        {
            if (EntityFactory.MainHeroView == null)
                return;
            
            int playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            bool bRuneShow = IsNeedShowRuneAddTip();
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_SetRune);

            if (bRuneShow && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_SetRune;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_RUNE);
                    ButtonType.bIsRequestSelf = true;
                    ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(addMsg);
            }
            else
            {
                SideButtonListRemoveData removeMsg = new SideButtonListRemoveData();
                removeMsg.nItemID = (int)ESideButtonItemType.ESBT_SetRune;
                LogicDataCenter.sideButtonDataManager.RemoveItem(removeMsg);
            }
        }
    }
}
