/*******************************************************************
** 文件名:	RunePageDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-11-29
** 版  本:	1.0
** 描  述:	皇冠系统数据中心
		
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
using ASpeedGame.Data.CrownGenstoneConfig;
using USpeedUI.TooltipAndDialog;
using USpeedUI.Chat;
using Data.SideButtonConfig;

namespace DataCenter
{
    public class CrownPageItemData
    {
        public int nPageTextIndex;
        public int nFaceValue;
        public int nPowerValue;
        public cmd_Crown_Page_Item_Info originalData = new cmd_Crown_Page_Item_Info();
        public cmd_Crown_Page_Item_Info reviseData = new cmd_Crown_Page_Item_Info();
       
        public void DoSaveData()
        {
            originalData.szName = reviseData.szName;
            originalData.nPageDBID = reviseData.nPageDBID;
            Array.Copy(reviseData.crownInlaySlotList, originalData.crownInlaySlotList, originalData.crownInlaySlotList.Length);
        }

        public void DoRecoverData()
        {
            reviseData.szName = originalData.szName;
            reviseData.nPageDBID = originalData.nPageDBID;
            Array.Copy(originalData.crownInlaySlotList, reviseData.crownInlaySlotList, reviseData.crownInlaySlotList.Length);
        }

        public void DoResetData()
        {
            Array.Clear(reviseData.crownInlaySlotList, 0, reviseData.crownInlaySlotList.Length);
        }

        public int nPageDBID
        {
            set { reviseData.nPageDBID = value; }
            get { return reviseData.nPageDBID; }
        }

        public CrownPageItemData()
        {
            originalData.crownInlaySlotList = new  int[UDefines.nInlaySlotMax];
            reviseData.crownInlaySlotList = new int[UDefines.nInlaySlotMax];
            nPageTextIndex = 0;
            nFaceValue = 0;
            nPowerValue = 0 ;
        }

        public bool IsChange()
        {
            bool bNameChange = reviseData.szName != originalData.szName;
            bool bInlayChange = false;
            for (int i = 0; i < reviseData.crownInlaySlotList.Length;++i )
            {
                if(reviseData.crownInlaySlotList[i]!=originalData.crownInlaySlotList[i])
                {
                    bInlayChange = true;
                    break;
                }
            }

            return bNameChange || bInlayChange;
        }
    }

    public class GemStoneAppraisalContrastsData
    {
        public cmd_Crown_Gemstone_Item_Info appraisalBeforeData = new cmd_Crown_Gemstone_Item_Info();
        public cmd_Crown_Gemstone_Item_Info appraisalAfterData = new cmd_Crown_Gemstone_Item_Info();

        public void ClearData()
        {
            appraisalBeforeData = new cmd_Crown_Gemstone_Item_Info();
            appraisalAfterData = new cmd_Crown_Gemstone_Item_Info();
        }
    }

    public class GemStoneAppraisalData
    {
        public const int m_nAppraisalSlotMax = 2;
        public int[] gemStoneAppraisalSlotList = new int[m_nAppraisalSlotMax];
        public GemStoneAppraisalContrastsData appraisalContrastsData = new GemStoneAppraisalContrastsData();

        public void ClearData()
        {
            Array.Clear(gemStoneAppraisalSlotList, 0, gemStoneAppraisalSlotList.Length);
            appraisalContrastsData.ClearData();
        }
    }

    public class GemStoneProductData
    {
        public cmd_Crown_Gemstone_Product_Info data = new cmd_Crown_Gemstone_Product_Info();
    }

    public class CrownPageDataManager
    {
        //////////////////////////////////////////////////////////////////////////
        // 当前玩家
        // 每页镶嵌情况数据
        private Dictionary<int, CrownPageItemData> m_crownPageDic = new Dictionary<int, CrownPageItemData>();
        // 玩家拥有宝石数据
        private Dictionary<int, cmd_Crown_Gemstone_Item_Info> m_gemStoneDic = new Dictionary<int, cmd_Crown_Gemstone_Item_Info>();
        // 获得当前页的镶嵌情况
        private CrownPageItemData curPageItemData;
        // 鉴定信息
        GemStoneAppraisalData m_appraisalData = new GemStoneAppraisalData();
        // 宝石产出信息
        GemStoneProductData m_productData = new GemStoneProductData();
        // 当前选中的背包宝石
        int m_nBackpackGemstoneDBID;
        // 鉴定界面显示情况
        private bool m_bIsAppraisalVisible = false;
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // 其他玩家 每页镶嵌情况数据
        private Dictionary<int, CrownPageItemData> m_crownOtherPageDic = new Dictionary<int, CrownPageItemData>();
        // 其他玩家 玩家拥有宝石数据
        private Dictionary<int, cmd_Crown_Gemstone_Item_Info> m_gemStoneOtherDic = new Dictionary<int, cmd_Crown_Gemstone_Item_Info>();
        // 其他玩家 获当前页的镶嵌情况
        private CrownPageItemData otherCurPageItemData;
        //////////////////////////////////////////////////////////////////////////

        public void RefreshGemstoneRedPoint()
        {
            if (!LogicDataCenter.playerSystemDataManager.bRequestSelf)
                return;

            if (curPageItemData == null)
                return;

            int nInlayTip = 0;
            int nInlayCount = 0;
            for (int i = 0; i < curPageItemData.reviseData.crownInlaySlotList.Length; ++i)
            {
                if (curPageItemData.reviseData.crownInlaySlotList[i] <= 0)
                {
                    nInlayTip = 1;
                }
                else
                {
                    ++nInlayCount;
                }
            }

            if (nInlayTip > 0 && nInlayCount >= m_gemStoneDic.Count)
            {
                nInlayTip = 0;
            }

            UIMsgCmdData uiInlayCount = new UIMsgCmdData((int)WndMsgID.WND_MSG_CROWNPAGE_REDPOINTTIP, nInlayTip, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_REDPOINTTIP, uiInlayCount);

            UpdateRuneButton(nInlayTip);
        }

        private void UpdateRuneButton(int inlayTip)
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_SetRune);

            if (inlayTip > 0 && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_SetRune;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_CROWN);
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

        public void UpdateCrownAllGemStoneData(IntPtr ptrParam, int nPtrLen)
        {
            m_gemStoneDic.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count countData = helper.get<cmd_count>();
            int nCount = countData.nCount;
            for (int i = 0; i < nCount; i++)
            {
                cmd_Crown_Gemstone_Item_Info data = helper.get<cmd_Crown_Gemstone_Item_Info>();
                data.nSlotID = i;
                m_gemStoneDic[data.nGemstoneDBID] = data;
            }

            SortAllGemStone();

            RefreshGemstoneRedPoint();
        }

        public void UpdateCrownAllPageData(IntPtr ptrParam, int nPtrLen)
        {
            CrownPageDic.Clear();
            //curPageItemData = null;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count countData = helper.get<cmd_count>();
            int nCount = countData.nCount;
            for (int i = 0; i < nCount; i++)
            {
                cmd_Crown_Page_Item_Info data = helper.get<cmd_Crown_Page_Item_Info>();
                CrownPageItemData pageData = new CrownPageItemData();
                pageData.nPageTextIndex = i+1;
                pageData.reviseData = data;
                pageData.DoSaveData();

                CrownPageDic[data.nPageDBID] = pageData;

                if (curPageItemData == null)
                {
                    curPageItemData = pageData;
                }
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, null);
            RefreshGemstoneRedPoint();
        }

        public void UpdateOtherCrownAllPageData(IntPtr ptrParam, int nPtrLen)
        {
            m_crownOtherPageDic.Clear();
            m_gemStoneOtherDic.Clear();
            otherCurPageItemData = null;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count countData = helper.get<cmd_count>();
            int nCount = countData.nCount;
            for (int i = 0; i < nCount; i++)
            {
                cmd_Crown_Page_Item_Info data = helper.get<cmd_Crown_Page_Item_Info>();
                CrownPageItemData pageData = new CrownPageItemData();
                pageData.nPageTextIndex = i+1;
                pageData.reviseData = data;
                pageData.DoSaveData();

                m_crownOtherPageDic[data.nPageDBID] = pageData;

                if (otherCurPageItemData == null)
                {
                    otherCurPageItemData = pageData;
                }
            }

            cmd_count countData2 = helper.get<cmd_count>();
            for (int i = 0; i < countData2.nCount; i++)
            {
                cmd_Crown_Gemstone_Item_Info data = helper.get<cmd_Crown_Gemstone_Item_Info>();
                data.nSlotID = i;
                m_gemStoneOtherDic[data.nGemstoneDBID] = data;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERUPDATEVIEW, null);
        }

        public void UpdateGemStoneItemData(cmd_Crown_Gemstone_Item_Info data)
        {
            if(data.nGemstoneID<=0)
            {
                if (m_gemStoneDic.ContainsKey(data.nGemstoneDBID))
                {
                    int nTmpSlot = m_gemStoneDic[data.nGemstoneDBID].nSlotID;
                    m_gemStoneDic.Remove(data.nGemstoneDBID);

                    // 更新后面的宝石槽号 
                    Dictionary<int, int> tmpUpdateSlotList = new Dictionary<int, int>();
                    foreach (cmd_Crown_Gemstone_Item_Info item in m_gemStoneDic.Values)
                    {
                        if (item.nSlotID > nTmpSlot)
                        {
                            tmpUpdateSlotList.Add(item.nGemstoneDBID, nTmpSlot);
                            nTmpSlot = item.nSlotID;
                        }
                    }

                    foreach (var item in tmpUpdateSlotList)
                    {
                        if (m_gemStoneDic.ContainsKey(item.Key))
                        {
                            cmd_Crown_Gemstone_Item_Info genstoneInfo = m_gemStoneDic[item.Key];
                            genstoneInfo.nSlotID = item.Value;
                            m_gemStoneDic[item.Key] = genstoneInfo;
                        }
                    }
                }
                
                int nSlotID = 0;
                foreach(int nGemstoneDBID in  m_appraisalData.gemStoneAppraisalSlotList)
                {
                    if (nGemstoneDBID == data.nGemstoneDBID)
                    {
                        m_appraisalData.gemStoneAppraisalSlotList[nSlotID] = 0;
                        break;
                    }

                    nSlotID++;
                }

                foreach (var itemPage in CrownPageDic.Values)
                {
                    for (int i = 0; i < itemPage.reviseData.crownInlaySlotList.Length; ++i)
                    {
                        if (itemPage.reviseData.crownInlaySlotList[i] == data.nGemstoneDBID)
                        {
                            itemPage.reviseData.crownInlaySlotList[i] = 0;
                            break;
                        }
                    }
                }
            }
            else
            {
                cmd_Crown_Gemstone_Item_Info oldData = new cmd_Crown_Gemstone_Item_Info();
                if (m_gemStoneDic.TryGetValue(data.nGemstoneDBID, out oldData))
                {
                    data.nSlotID = oldData.nSlotID;
                }
                else
                {
                    int nMaxSlotID = -1;
                    foreach (cmd_Crown_Gemstone_Item_Info item in m_gemStoneDic.Values)
                    {
                        if (item.nSlotID > nMaxSlotID)
                            nMaxSlotID = item.nSlotID;
                    }

                    data.nSlotID = nMaxSlotID + 1;
                }

                m_gemStoneDic[data.nGemstoneDBID] = data;
            }

            RefreshGemstoneRedPoint();
            SendCrownViewUpdateMessage();
        }

        public void UpdateGemStoneProductData(cmd_Crown_Gemstone_Product_Info data)
        {
            m_productData.data = data;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GEMSTONECHESTFLOP_UPDATEVIEW, null);
        }

        public void UpdateGemStoneAppraisalAfterData(cmd_Crown_Gemstone_Appraisal_Ret data)
        {
            if (m_gemStoneDic == null || !m_gemStoneDic.ContainsKey(data.nGemstoneDBID))
                return;

            if (data.nGemstoneDBID != m_appraisalData.appraisalContrastsData.appraisalBeforeData.nGemstoneDBID)
                return;

            cmd_Crown_Gemstone_Item_Info itemInfo = m_gemStoneDic[data.nGemstoneDBID];

            m_appraisalData.appraisalContrastsData.appraisalBeforeData = itemInfo;
            //m_appraisalData.appraisalContrastsData.appraisalAfterData = itemInfo;
        }

        public Dictionary<int, CrownPageItemData> CrownPageDic
        {
            set 
            {
                if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                    m_crownPageDic = value; 
                else
                    m_crownOtherPageDic = value;

            }

            get { return LogicDataCenter.playerSystemDataManager.bRequestSelf ? m_crownPageDic : m_crownOtherPageDic; }
        }

        public Dictionary<int, cmd_Crown_Gemstone_Item_Info> CrownGemtoneDic
        {
            set
            {
                if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                    m_gemStoneDic = value;
                else
                    m_gemStoneOtherDic = value;

            }

            get { return LogicDataCenter.playerSystemDataManager.bRequestSelf ? m_gemStoneDic : m_gemStoneOtherDic; }
        }

        public GemStoneProductData CrownGemstoneProductInfo
        {
            get { return m_productData; }
        }

        public Dictionary<int, CrownPageItemData> CrownPagePageInfo
        {
            get { return CrownPageDic; }
        }

        public Dictionary<int, cmd_Crown_Gemstone_Item_Info> CrownPageGemStoneInfo
        {
            get { return m_gemStoneDic; }
        }

        public GemStoneAppraisalContrastsData CrownPageAppraisalContrastsData 
        {
            get { return m_appraisalData.appraisalContrastsData; }
        }

        public int[] CrownPageAppraisalList
        {
            set { m_appraisalData.gemStoneAppraisalSlotList = value; }
            get { return m_appraisalData.gemStoneAppraisalSlotList; }
        }

        public int[] CrownPageInlayList
        {
            get
            {
                if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                {
                    return curPageItemData == null ?  null:curPageItemData.reviseData.crownInlaySlotList;
                }
                else
                {
                    return otherCurPageItemData == null ?  null:otherCurPageItemData.reviseData.crownInlaySlotList;
                }                
            }
        }

        public CrownPageItemData CrownPageCurPageItemData
        {
            set {  curPageItemData = value; }
            get { return curPageItemData; }
        }

        public CrownPageItemData CrownPageOtherCurPageItemData
        {
            set { otherCurPageItemData = value; }
            get { return otherCurPageItemData; }
        }

        public int BackPackSelGemstoneDBID
        {
            set { m_nBackpackGemstoneDBID = value; }
            get { return m_nBackpackGemstoneDBID; }
        }

        public string CrownPageName
        {
            get { return curPageItemData == null ? "" : curPageItemData.reviseData.szName; }
            set
            {
                if (curPageItemData == null)
                    return;

                if (curPageItemData.reviseData.szName == value)
                    return;

                curPageItemData.reviseData.szName = value;

                SendCrownViewUpdateMessage();
            }
        }

        public string OtherCrownPageName
        {
            get { return otherCurPageItemData == null ? "" : otherCurPageItemData.reviseData.szName; }
        }

        public int CrownPageTextIndex
        {
            get { return curPageItemData!=null?curPageItemData.nPageTextIndex:0; }
        }

        public int OtherCrownPageTextIndex
        {
            get { return otherCurPageItemData != null ? otherCurPageItemData.nPageTextIndex : 0; }
        }

        public int GetCurPageGemstoneCount()
        {
            if (curPageItemData == null)
                return 0;

            int nCount = 0;
            for (int i = 0; i < curPageItemData.reviseData.crownInlaySlotList.Length; ++i)
            {
                if (curPageItemData.reviseData.crownInlaySlotList[i] > 0)
                    ++nCount;
            }

            return nCount;            
        }

        public cmd_Crown_Gemstone_Item_Info GetGemStoneItemData(int nGemstoneDBID)
        {
            cmd_Crown_Gemstone_Item_Info data = new cmd_Crown_Gemstone_Item_Info();
            if (CrownGemtoneDic != null)
                CrownGemtoneDic.TryGetValue(nGemstoneDBID, out data);

            return data;
        }

        public int GetCrownPageCurDBPageIndex()
        {
            return curPageItemData == null ? -1 : curPageItemData.nPageDBID;
        }

        public int GetCrownPageTotalPageNum()
        {
            return CrownPageDic == null ? -1 : CrownPageDic.Count;
        }

        public bool AppraisalVisible
        {
            set { m_bIsAppraisalVisible = value; }
            get { return m_bIsAppraisalVisible; }
        }

        public Sprite GetCrownGemStoneSpriteByDBID(int nGemstoneDBID,int nSizeType = 1)
        {
            // toconfig
           cmd_Crown_Gemstone_Item_Info info ;
           if (!CrownGemtoneDic.TryGetValue(nGemstoneDBID, out info))
           return null;

           return GetCrownGemStoneSpriteByID(info.nGemstoneID);
        }

        public Sprite GetCrownGemStoneSpriteByIconID(int nGemstoneIconID, int nSizeType = 1)
        {
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Gemstone, WndID.WND_ID_CROWNPAGE, nSizeType, nGemstoneIconID);
        }

        public Sprite GetCrownGemStoneSpriteByID(int nGemstoneID, int nSizeType = 1)
        {
            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(nGemstoneID);
            if (config == null)
                return null;

            return GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID);
        }

        public void SaveCurCrownPage()
        {
            if (curPageItemData == null)
                return;

            if (!curPageItemData.IsChange())
                return;

            curPageItemData.DoSaveData();
            SendSaveCrownPageMessage();
            SendCrownViewUpdateMessage();
        }

        public void RescoverCurCrownPage()
        {
            if (curPageItemData == null)
                return;

            curPageItemData.DoRecoverData();
            SendCrownViewUpdateMessage();
        }

        public void ResetCurCrownPage()
        {
            if (curPageItemData == null)
                return;

            curPageItemData.DoResetData();
            SendCrownViewUpdateMessage();
        }

        public void DeleteCurCrownPage()
        {
            if (curPageItemData == null || CrownPageDic == null || CrownPageDic.Count<=2)
                return;

            CrownPageItemData newPageItemData = null;
            int   nSeachIndex = curPageItemData.nPageTextIndex == CrownPageDic.Count-1?curPageItemData.nPageTextIndex-1:curPageItemData.nPageTextIndex+1;
           foreach (CrownPageItemData item in CrownPageDic.Values)
           {
               if(item.nPageTextIndex == nSeachIndex)
               {
                   newPageItemData = item;
                   break;
               }
           }
   
            if (newPageItemData == null)
                return;

            SendDeleteCrownPageMessage();
            CrownPageDic.Remove(curPageItemData.nPageDBID);
            curPageItemData = newPageItemData;
            SendCrownViewUpdateMessage();
        }

        public bool AddCurCrownPage()
        {
            if (curPageItemData == null || CrownPageDic.Count>=10)
                return false;

            int nDBUniquePageIndexMax = 0;
            foreach (CrownPageItemData item in CrownPageDic.Values)
            {
                if (item.nPageDBID > nDBUniquePageIndexMax)
                    nDBUniquePageIndexMax = item.nPageDBID;
            }

            CrownPageItemData newPageData = new CrownPageItemData();
            newPageData.nPageDBID = nDBUniquePageIndexMax+1;
            newPageData.reviseData.szName = ULocalizationService.Instance.Get("UIView", "CrownGem", "GemPageName") + (CrownPageDic.Count + 1);
            newPageData.nPageTextIndex = CrownPageDic.Count + 1;
            newPageData.DoSaveData();
            CrownPageDic[newPageData.nPageDBID] = newPageData;
            curPageItemData = newPageData;
            SendSaveCrownPageMessage();
            SendCrownViewUpdateMessage();
            RefreshGemstoneRedPoint();

            return true;
        }

        public bool OtherChangeCrownPage(int nPageIndex)
        {
            if (nPageIndex <= 0)
                return false;

            if (CrownPageDic == null || CrownPageDic.Count <= 0)
                return false;

            if (otherCurPageItemData != null && otherCurPageItemData.nPageTextIndex == nPageIndex)
                return false;

            int nPageDBID = 0;
            foreach (CrownPageItemData pageItem in CrownPageDic.Values)
            {
                if (pageItem.nPageTextIndex == nPageIndex)
                {
                    nPageDBID = pageItem.nPageDBID;
                    break;
                }
            }

            if (nPageDBID <= 0)
                return false;

            CrownPageDic.TryGetValue(nPageDBID, out otherCurPageItemData);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, null);

            return true;
        }

        public bool ChangeCrownPage(int nPageIndex)
        {
            if (nPageIndex <= 0)
                return false;

            if (CrownPageDic == null||CrownPageDic.Count<=0)
                return false;

            if (curPageItemData != null && curPageItemData.nPageTextIndex == nPageIndex)
                return false;

            if (curPageItemData!=null)
            {
                if (curPageItemData.IsChange())
                    SaveCurCrownPage();
            }

            int nPageDBID = 0;
            foreach (CrownPageItemData pageItem in CrownPageDic.Values)
            {
                if (pageItem.nPageTextIndex == nPageIndex)
                {
                    nPageDBID = pageItem.nPageDBID;
                    break;
                }                
            }

            if (nPageDBID <= 0)
                return false;

            CrownPageDic.TryGetValue(nPageDBID, out curPageItemData);
            SendCrownViewUpdateMessage();
            RefreshGemstoneRedPoint();

            return true;
        }

        public void RoomSelectPage(int nPageTextIndex)
        {
            int nPageDBID = 0;
            int nPageDicIndex = 0;
            foreach (CrownPageItemData pageItem in CrownPageDic.Values)
            {
                if (nPageDicIndex++ != nPageTextIndex)
                    continue;

                nPageDBID = pageItem.nPageDBID;
                break;
            }

            if (nPageDBID <= 0)
                return;

            IntPtr ptr = new IntPtr(nPageDBID);
            EntityEventHelper.Instance.SendCommand<IntPtr>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_ROOMSELECTPAGE, ref ptr);
        }

        public void ShowOfCrownPage()
        {
            CrownPageItemData pageData = LogicDataCenter.crownPageDataManager.curPageItemData;
            if (pageData == null)
                return;

            CreatureProperty Property = EntityFactory.MainHeroView.Property;
            if (Property == null)
                return;

			ChatSubItemText_CrownGemstoneInfo.ShareToInputField(Property.CreatureName, pageData.nFaceValue, pageData.nPowerValue,
				Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID));
		}

        public void SendSaveCrownPageMessage()
        {
            if (curPageItemData == null)
                return;

            cmd_Crown_Page_Item_Info msgInfo = new cmd_Crown_Page_Item_Info();
            msgInfo.crownInlaySlotList = new int[curPageItemData.reviseData.crownInlaySlotList.Length];
            msgInfo.nPageDBID = curPageItemData.nPageDBID;
            msgInfo.szName = curPageItemData.reviseData.szName;
            Array.Copy(curPageItemData.reviseData.crownInlaySlotList, msgInfo.crownInlaySlotList, curPageItemData.reviseData.crownInlaySlotList.Length);
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Page_Item_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_SAVEPAGE, ref msgInfo);
        }

        public void SendSeilCrownPageMessage()
        {
            if (m_nBackpackGemstoneDBID <= 0)
                return;

            cmd_Crown_Gemstone_Seil msgInfo = new cmd_Crown_Gemstone_Seil();
            msgInfo.nGemstoneDBID = m_nBackpackGemstoneDBID;
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Gemstone_Seil>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DGEMSTONE_SEIL, ref msgInfo);
        }

        public void SendDeleteCrownPageMessage()
        {
            if (curPageItemData == null)
                return;

            cmd_Crown_Page_Delete_Info msgInfo = new cmd_Crown_Page_Delete_Info();
            msgInfo.nPageDBID = curPageItemData.nPageDBID;
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Page_Delete_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DELETEPAGE, ref msgInfo);
        }

        public void SendAppraisalGemstoneMessage()
        {
            if (CrownPageAppraisalList == null)
                return;

            cmd_Crown_Gemstone_Appraisal_Info msgInfo = new cmd_Crown_Gemstone_Appraisal_Info();
            msgInfo.gemstoneDBIDList = new int[CrownPageAppraisalList.Length];
            Array.Copy(CrownPageAppraisalList, msgInfo.gemstoneDBIDList,CrownPageAppraisalList.Length);
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Gemstone_Appraisal_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_AGEMSTONEPPRAISAL, ref msgInfo);
        }

        public void UpdateCrownGemstonePosition(int m_nSlotID, int nGemstoneDBID)
        {
            if (m_gemStoneDic == null || !m_gemStoneDic.ContainsKey(nGemstoneDBID))
                return;

            cmd_Crown_Gemstone_Item_Info Info = m_gemStoneDic[nGemstoneDBID];
            Info.nSlotID = m_nSlotID;
            m_gemStoneDic[nGemstoneDBID] = Info;
        }

        public int getCrownGemStoneDBIDBySlot(int nSlot)
        {
            foreach (cmd_Crown_Gemstone_Item_Info item in m_gemStoneDic.Values)
            {
                if (item.nSlotID == nSlot)
                    return item.nGemstoneDBID;
            }

            return 0;
        }

        public void UpdateCrownGemstonePosition(int nDestGemstoneDBID, int nDestSlotID, int nSrcGemstoneDBID, int nSrcSlotID)
        {
            if (m_gemStoneDic == null || !m_gemStoneDic.ContainsKey(nSrcGemstoneDBID))
                return;

            if (nDestGemstoneDBID == 0)
                nDestGemstoneDBID = getCrownGemStoneDBIDBySlot(nDestSlotID);

            if (!m_gemStoneDic.ContainsKey(nDestGemstoneDBID))
            {
                UpdateCrownGemstonePosition(nDestSlotID, nSrcGemstoneDBID);
                SendCrownViewUpdateMessage();
                return;
            }

            int nTmpPosition = nDestSlotID;
            cmd_Crown_Gemstone_Item_Info destInfo = m_gemStoneDic[nDestGemstoneDBID];
            cmd_Crown_Gemstone_Item_Info srcInfo = m_gemStoneDic[nSrcGemstoneDBID];
            destInfo.nSlotID = nSrcSlotID;
            srcInfo.nSlotID = nTmpPosition;
            m_gemStoneDic[nDestGemstoneDBID] = destInfo;
            m_gemStoneDic[nSrcGemstoneDBID] = srcInfo;
            SendCrownViewUpdateMessage();
        }

        public void UpdateCrownPageGemStoneInlay(int m_nSlotID, int nGemstoneDBID)
        {
            if (m_gemStoneDic == null)
                return;

            if (curPageItemData == null)
                return;

            if (m_nSlotID < 0 || m_nSlotID >= curPageItemData.reviseData.crownInlaySlotList.Length)
                return;

            curPageItemData.reviseData.crownInlaySlotList[m_nSlotID] = nGemstoneDBID;
            SendCrownViewUpdateMessage();
            RefreshGemstoneRedPoint();
        }

        public void UpdateCrownPageGemStoneInlayOrAppraisalSlot(int nGemstoneDBID)
        {
            if (nGemstoneDBID <= 0)
                return;

            if (m_gemStoneDic == null)
                return;

            if (curPageItemData == null)
                return;

            int nTmpSlotID = 0;
            bool bFull = true;
            if (m_bIsAppraisalVisible)
            {
                for (int i = 0; i < m_appraisalData.gemStoneAppraisalSlotList.Length; ++i)
                {
                    if (m_appraisalData.gemStoneAppraisalSlotList[i] == 0)
                    {
                        if(!IsAppraisalSlotCanUse(i, nGemstoneDBID, false))
                            return;

                        nTmpSlotID = i;
                        bFull = false;
                        break;
                    }
                }

                if (bFull)
                    return;

                m_appraisalData.gemStoneAppraisalSlotList[nTmpSlotID] = nGemstoneDBID;
                if (nTmpSlotID == 0)
                    m_appraisalData.appraisalContrastsData.appraisalBeforeData = m_gemStoneDic.ContainsKey(nGemstoneDBID) ? m_gemStoneDic[nGemstoneDBID] : new cmd_Crown_Gemstone_Item_Info();
            }
            else
            {
                for (int i = 0; i < curPageItemData.reviseData.crownInlaySlotList.Length; ++i)
                {
                    if (curPageItemData.reviseData.crownInlaySlotList[i] == 0)
                    {
                        nTmpSlotID = i;
                        bFull = false;
                        break;
                    }
                }

                if (bFull)
                    return;

                curPageItemData.reviseData.crownInlaySlotList[nTmpSlotID] = nGemstoneDBID;

                RefreshGemstoneRedPoint();
            }
         
            SendCrownViewUpdateMessage();
        }

        public void UpdateCrownPageGemStoneInlay(int nDestGemstoneDBID, int nDestSlotID, int nSrcGemstoneDBID, int nSrcSlotID)
        {
            if (m_gemStoneDic == null || !m_gemStoneDic.ContainsKey(nSrcGemstoneDBID))
                return;

            if (curPageItemData == null)
                return;

            if (nDestSlotID < 0 || nDestSlotID >= curPageItemData.reviseData.crownInlaySlotList.Length)
                return;
            if (nSrcSlotID < 0 || nSrcSlotID >= curPageItemData.reviseData.crownInlaySlotList.Length)
                return;

            int nTmpPosition = nDestSlotID;
            curPageItemData.reviseData.crownInlaySlotList[nSrcSlotID] = nDestGemstoneDBID;
            curPageItemData.reviseData.crownInlaySlotList[nTmpPosition] = nSrcGemstoneDBID;
            SendCrownViewUpdateMessage();
        }

        public void UpdateCrownPageGemStoneAppraisalSlot(int m_nSlotID, int nGemstoneDBID)
        {
            if (m_gemStoneDic == null )
                return;

            if (!IsAppraisalSlotCanUse(m_nSlotID, nGemstoneDBID, false))
                return;

            m_appraisalData.gemStoneAppraisalSlotList[m_nSlotID] = nGemstoneDBID;
            if (m_nSlotID == 0)
            {
                if (m_gemStoneDic.ContainsKey(nGemstoneDBID))
                    m_appraisalData.appraisalContrastsData.appraisalBeforeData = m_gemStoneDic[nGemstoneDBID];
                else
                    m_appraisalData.appraisalContrastsData.ClearData();
            }

            SendCrownViewUpdateMessage();
        }

        public void UpdateCrownPageGemStoneAppraisalSlot(int nDestGemstoneDBID, int nDestSlotID, int nSrcGemstoneDBID, int nSrcSlotID)
        {
            if (m_gemStoneDic == null || !m_gemStoneDic.ContainsKey(nSrcGemstoneDBID))
                return;

            if (nSrcSlotID == 0 && !m_gemStoneDic.ContainsKey(nDestGemstoneDBID))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_NOT_MAIN);
                return;
            }

            if (!IsAppraisalSlotCanUse(nDestSlotID, nSrcGemstoneDBID, true) || !IsAppraisalSlotCanUse(nSrcSlotID, nDestGemstoneDBID, true))
                return;

            m_appraisalData.gemStoneAppraisalSlotList[nSrcSlotID] = nDestGemstoneDBID;
            m_appraisalData.gemStoneAppraisalSlotList[nDestSlotID] = nSrcGemstoneDBID;

            if (nSrcSlotID == 0 || nDestSlotID == 0)
            {
                m_appraisalData.appraisalContrastsData.appraisalBeforeData = nDestSlotID == 0 ? m_gemStoneDic[nSrcGemstoneDBID] : m_gemStoneDic[nDestGemstoneDBID];
            }
         
            SendCrownViewUpdateMessage();
        }

        private void SendCrownViewUpdateMessage()
        {
            SaveCurCrownPage();
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, null);
        }

        public void ClearCrownAppraisalData()
        {
            m_appraisalData.ClearData();
        }

        public void Clear()
        {
            CrownPageDic.Clear();
            m_gemStoneDic.Clear();
            m_crownOtherPageDic.Clear();
            m_gemStoneOtherDic.Clear();
        }

        public void RequestOtherPlayerCrownPageData()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                return;

            if (LogicDataCenter.playerSystemDataManager.nRequestPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                return;

            m_crownOtherPageDic.Clear();
            m_gemStoneOtherDic.Clear();
            otherCurPageItemData = null;

            cmd_Crown_Request_Other_Page msgInfo = new cmd_Crown_Request_Other_Page();
            msgInfo.RequestFriendID = (uint)LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Request_Other_Page>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DOTHER_PAGE, ref msgInfo);
        }

        public bool IsCurCrownPageChanged()
        {
            if (curPageItemData == null)
                return true;

            return curPageItemData.IsChange();
        }

        public bool IsAppraisalSlotCanUse(int nSlotID,int nGemstoneDBID, bool bAppriasalDrag)
        {
            if (nSlotID < 0 || nSlotID >= m_appraisalData.gemStoneAppraisalSlotList.Length)
                return false;

            if (m_appraisalData.gemStoneAppraisalSlotList[nSlotID] == nGemstoneDBID)
                return false;

            if (m_gemStoneDic.ContainsKey(nGemstoneDBID))
            {
                if (nSlotID == 0)
                {
                    if (m_gemStoneDic[nGemstoneDBID].byGrade != (byte)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_MAIN_GRADE);
                        return false;
                    }
                }
                else
                {
                    int nMainGemstoneDBID = m_appraisalData.gemStoneAppraisalSlotList[0];
                    if (m_gemStoneDic.ContainsKey(nMainGemstoneDBID) && m_gemStoneDic[nMainGemstoneDBID].byGrade != (byte)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_MAIN_GRADE);
                        return false;
                    }

                    int curGemstoneDBID = m_appraisalData.gemStoneAppraisalSlotList[nSlotID];
                    if (bAppriasalDrag && !m_gemStoneDic.ContainsKey(curGemstoneDBID))
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_NOT_MAIN);
                        return false;
                    }

                    nMainGemstoneDBID = (bAppriasalDrag ? curGemstoneDBID : m_appraisalData.gemStoneAppraisalSlotList[0]);
                    if (nMainGemstoneDBID == 0)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_NOT_MAIN);
                        return false;
                    }

                    SSchemeCrownGemstoneConfig mainStoneCfg = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(m_gemStoneDic[nMainGemstoneDBID].nGemstoneID);
                    if (mainStoneCfg == null)
                        return false;

                    SSchemeCrownGemstoneConfig curStoneCfg = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(m_gemStoneDic[nGemstoneDBID].nGemstoneID);
                    if (curStoneCfg == null)
                        return false;

                    if (curStoneCfg.nGradeUpper > mainStoneCfg.nGradeUpper)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_ASS_GRADE);
                        return false;
                    }
                }
            }

            return true;
        }

        private void SortAllGemStone()
        {
            if (m_gemStoneDic.Values == null || m_gemStoneDic.Values.Count < 1)
            {
                return;
            }

            List<cmd_Crown_Gemstone_Item_Info> gemStoneList = new List<cmd_Crown_Gemstone_Item_Info>(m_gemStoneDic.Values);
            gemStoneList = gemStoneList.OrderByDescending(item => item.byGrade).OrderBy(item => item.nGemstoneID).ToList();
            m_gemStoneDic.Clear();
            int nSlotID = 0;
            foreach (cmd_Crown_Gemstone_Item_Info stone in gemStoneList)
            {
                cmd_Crown_Gemstone_Item_Info stoneCopy = stone;
                stoneCopy.nSlotID = nSlotID++;
                m_gemStoneDic[stoneCopy.nGemstoneDBID] = stoneCopy;
            }
        }
    }
}
