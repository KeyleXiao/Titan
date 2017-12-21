/*******************************************************************
** 文件名:	GFxWarGoodsStore.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：WarGoodsStore.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using ASpeedGame.Data.GameMobaSchemes;
using System.Collections.Generic;
using GameLogic;
using System.Runtime.InteropServices;

partial class GFxWarGoodsStore
{
    #region CS->AS
    /// <summary>
    /// 打包当前英雄推荐物品信息到SF显示
    /// </summary>
    /// <param name="_typeList">推荐物品类型列表</param>
    /// <param name="_goodType">推荐物品类型</param>
    private void BoxRecommendedGoodsByGoodsType( System.Collections.Generic.List<SSchemeMobaGoods> _typeList, int _goodType)
    {
        //if (!Value.IsValueValid(vRecommendedGoodsPage))
        //    return;

        //for (int i = 0; i < _typeList.Count; i++)
        //{

        //    Value ASGoodsValue;

        //    GameMobaSchemesManager.Instance.CreateASMobaGoods(_typeList[i], out ASGoodsValue, this);

        //    if (!Value.IsValueValid(ASGoodsValue) || vRecommendedGoodsPage.Invoke("AddGoodsToRecommendedTypeList", ASGoodsValue, _goodType) == null)
        //    {
        //        Trace.LogError("call AddGoodsToRecommendedTypeList error");
        //        return;
        //    }
        //}

    }

    /// <summary>
    /// 打包所有物品信息到SF显示
    /// </summary>
    /// <param name="_allGoodsTable"></param>
    private void BoxAllGoods(Hashtable _allGoodsTable)
    {
        //if (!Value.IsValueValid(vAllGoodsPage))
        //    return;

        //Value AllGoodsArr = CreateArray();
        //AllGoodsArr.SetArraySize((uint)_allGoodsTable.Count - 1);//以物品ID为key，没有ID为0 的物品，但hashtable 自动填补了ID为0的key，所以多一个
        //uint index = 0;
        //foreach (SSchemeMobaGoods SchemeGoods in _allGoodsTable.Values)
        //{
        //    if (SchemeGoods.GoodsID == 0)
        //        continue;

        //    Value ASGoods;
        //    GameMobaSchemesManager.Instance.CreateASMobaGoods(SchemeGoods, out ASGoods, this);

        //    AllGoodsArr.SetElement(index, ASGoods);
        //    index++;
        //}
        //if (vAllGoodsPage.Invoke("SetListDataProvider", AllGoodsArr) == null)
        //{
        //    Trace.LogError("call SetListDataProvider failed");
        //    return;
        //}
    }

    private void BoxAllDefaultGoods(Hashtable _allGoodsTable,List<int> _defaultGoodsType)
    {
        //Dictionary<int,Value> allDefaultGoods = new Dictionary<int,Value>();
        //int Dickey = -1;
        //char[] charSeparator = new char[] { MobaSchemesConfig.separator };

        //foreach (SSchemeMobaGoods SchemeGoods in _allGoodsTable.Values)
        //{
        //    if (SchemeGoods.GoodsID == 0)
        //        continue;

        //    foreach(string label in SchemeGoods.GoodsLabel.Split(charSeparator))
        //    {
        //        if(label == string.Empty)
        //        {
        //            Dickey = 0;
        //        }
        //        else
        //            Dickey = Convert.ToInt32(label) / 1000;

        //        foreach (int defaultType in _defaultGoodsType)
        //        {
        //            if (Dickey == defaultType)
        //            {
        //                Value ASGoods;
        //                GameMobaSchemesManager.Instance.CreateASMobaGoods(SchemeGoods, out ASGoods, this);
        //                if(!allDefaultGoods.ContainsKey(SchemeGoods.GoodsID))
        //                    allDefaultGoods.Add(SchemeGoods.GoodsID,ASGoods);
        //            }
        //        }

        //    }
        //}

        //BoxAllDefaltGoods(allDefaultGoods);
    }

    /// <summary>
    /// 打包默认物品信息到SF显示
    /// </summary>
    /// <param name="_allDefaultGoods"></param>
    private void BoxAllDefaltGoods(Dictionary<int, Value> _allDefaultGoods)
    {
        //if (!Value.IsValueValid(vDefaultGoodsPage))
        //    return;
        if (!Value.IsValueValid(vAllGoodsPage))
            return;

        Value DefaultGoodsArr = CreateArray();
        DefaultGoodsArr.SetArraySize((uint)_allDefaultGoods.Count - 1);//以物品ID为key，没有ID为0 的物品，但hashtable 自动填补了ID为0的key，所以多一个
        uint index = 0;
        foreach (Value ASGoods in _allDefaultGoods.Values)
        {
            DefaultGoodsArr.SetElement(index, ASGoods);
            index++;
        }

        if (vAllGoodsPage.Invoke("SetDefaultListDataProvider", DefaultGoodsArr) == null)
        {
            Trace.LogError("call vAllGoodsPage.SetDefaultListDataProvider failed");
            return;
        }
        //if (vDefaultGoodsPage.Invoke("SetListDataProvider", DefaultGoodsArr) == null)
        //{
        //    Trace.LogError("call SetListDataProvider failed");
        //    return;
        //}

    }

    /// <summary>
    /// 打包物品标签描述到SF显示
    /// </summary>
    /// <param name="_labelDescriptionTable">物品标签列表</param>
    private void BoxGoodsLabelDescriptrion(Hashtable _labelDescriptionTable)
    {
        if (!Value.IsValueValid(vAllGoodsTypeFrame))
            return;

        Dictionary<int, List<int>> LabelAndSubLabelDic = new Dictionary<int, List<int>>();

        //根据配置信息建立物品标签二维列表
        foreach (int labelKey in _labelDescriptionTable.Keys)
        {
            if (labelKey == 0)
                continue;

            int Dickey = labelKey / 1000;
            
            //大于0为子节点，小于0为父节点
            if (Dickey > 0)
            {
                if (!LabelAndSubLabelDic.ContainsKey(Dickey))
                {
                    LabelAndSubLabelDic[Dickey] = new List<int>();
                }
                LabelAndSubLabelDic[Dickey].Add(labelKey);
            }
            else
            {
                if (!LabelAndSubLabelDic.ContainsKey(labelKey))
                    LabelAndSubLabelDic[labelKey] = new List<int>();
            }
        }

        int ParentIndex = 0;
        int SubIndex = 0;
        //遍历物品标签二维列表并在SF创建对应的实例显示
        foreach (int key in LabelAndSubLabelDic.Keys)
        {
            string Label = Convert.ToString(_labelDescriptionTable[key]);
            //Send To AS Create Label
            //Trace.LogWarning("Label:" + Label);
            if (vAllGoodsTypeFrame.Invoke("CreateLabel", ParentIndex, key, Label) == null)
            {
                Trace.LogError("call CreateLabel failed");
                return;
            }
            foreach (int subKey in LabelAndSubLabelDic[key])
            {
                string SubLabel = Convert.ToString(_labelDescriptionTable[subKey]);
                //Trace.LogWarning("SubLabel:" + SubLabel);
                if (vAllGoodsTypeFrame.Invoke("CreateSubLabel", ParentIndex, SubIndex, subKey, SubLabel) == null)
                {
                    Trace.LogError("call CreateSubLabel failed");
                    return;
                }
                SubIndex++;
            }

            ParentIndex++;
            SubIndex = 0;
        }
    }

    /// <summary>
    /// 显示当前选择的物品可合成的所有物品
    /// </summary>
    /// <param name="_nCurrentSelectedGoods">当前选择的物品ID</param>
    private void ShowSelectedGoodsCanCompose(int _nCurrentSelectedGoods)
    {
        //if (!Value.IsValueValid(vCanComposeFrame) || !Value.IsValueValid(vStoreWindow))
        //    return;

        ////查找出当前物品可合成物品ID
        //SSchemeMobaGoods CurrentSelectedGoods = new SSchemeMobaGoods();
        //GameMobaSchemesManager.Instance.GetMobaGoods(_nCurrentSelectedGoods, ref CurrentSelectedGoods);
        ////当前选择的物品没有可合成的物品，则返回
        //if(CurrentSelectedGoods.GoodsCanComposeList.Length <= 0)//if (SchemeMobaGoodsList == null)
        //{
        //    if (vCanComposeFrame.Invoke("ResetCanComposeGoodsList") == null)
        //        Trace.LogError("call CanComposeGoodsList.ResetListDataProvider failed");

        //    return;
        //}
        
        //if(vStoreWindow.Invoke("SetCanComposeGoodsList", CurrentSelectedGoods.GoodsCanComposeList,MobaSchemesConfig.separator.ToString()) == null)
        //{
        //    Trace.LogError("call vStoreWindow.SetCanComposeGoodsList failed.");
        //    return;
        //}

        //if (Value.IsValueValid(vCanComposeFrame))
        //{
        //    if (vCanComposeFrame.Invoke("SetGoodsRenderCanBuy", nPlayerCurrentMoney) == null)
        //    {
        //        Trace.LogError("call CanComposeFrame.SetGoodsRenderCanBuy failed");
        //    }
        //}
    }

    /// <summary>
    /// 显示当前选择的物品合成树
    /// </summary>
    /// <param name="_nCurrentSelectedGoods">当前选择的物品ID</param>
    private void ShowSelectedGoodsComposeTree(int _nCurrentSelectedGoods)
    {
        if (!Value.IsValueValid(vGoodsComposeTreeMgr))
            return;

        List<GoodsCTNode> GoodsNodeList = new List<GoodsCTNode>();
        //迭代找到当前物品合成所需的所有子物品
        IteratorFindNeedGoods(_nCurrentSelectedGoods, GoodsNodeList);

        //移除旧的物品合成树
        if (vGoodsComposeTreeMgr.Invoke("DestroyGoodsComposeTree") == null)
        {
            Trace.LogError("call GoodsComposeTreeMgr.DestroyGoodsComposeTree failed");
            return;
        }

        //实例化树的所有子节点
        for (int i = 0; i < GoodsNodeList.Count; i++)
        {
            if (vGoodsComposeTreeMgr.Invoke("CreateGoodsComposeTreeNode", GoodsNodeList[i].ASGoodsValue, GoodsNodeList[i].nChildCount) == null)
            {
                Trace.LogError("call CreateGoodsComposeTreeNode failed");
                return;
            }
        }

        //绘制树、连接线等
        if (vGoodsComposeTreeMgr.Invoke("BuildGoodsComposeTree") == null)
        {
            Trace.LogError("call BuildGoodsComposeTree failed");
            return;
        }

        if (Value.IsValueValid(vGoodsComposeTreeMgr))
        {
            if (vGoodsComposeTreeMgr.Invoke("SetGoodsRenderCanBuy", nPlayerCurrentMoney) == null)
            {
                Trace.LogError("call GoodsComposeTreeMgr.SetGoodsRenderCanBuy failed");
            }
        }
    }
    
    /// <summary>
    /// 接受逻辑层下发的当前购买物品信息
    /// </summary>
    /// <param name="_data">当前购买物品信息</param>
    public void WarGoodsStoreBuyGoods(IntPtr ptrParam, int nPtrLen)
    {
        if (!Value.IsValueValid(vStoreWindow))
            return;
        ResetAssociatedWithPurchasedGoodsSlot();


        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

        cmd_war_persongoodsinfo_count obj = helper.get<cmd_war_persongoodsinfo_count>();

        for (int i = 0; i < obj.nCount; ++i )
        {
            cmd_war_persongoodsinfo data = helper.get<cmd_war_persongoodsinfo>();

            SetPurchasedGoodsSlot(data);
            //Value ASGoodsValue;
            //GameMobaSchemesManager.Instance.GetASMobaGoods(data.nGoodID, out ASGoodsValue, this);

            //if (!Value.IsValueValid(ASGoodsValue) || vStoreWindow.Invoke("SetPurchasedGoodsSlot", data.nSlotIndex, ASGoodsValue, data.nCanUseTimes,1) == null)
            //{
            //    Trace.LogError("error call SetPurchasedGoodsSlot failed");
            //    return;
            //}



            curPurchasedGoodsList.Add(data);
        }

        //等逻辑层发完整个已购买物品列表，再做判断
        //判断哪些物品的价格需要变更的
        {
            //这里没用FindWhichGoodsCanEffect()方法，因为该方法好像会重复迭代很多次
            FindWhichGoodsCanEffect();// IteratorFindCanComposeGoods(curPurchasedGoodsList[0].nGoodID, ref allCanComposeGoodsMap);

            //curPurchasedGoodsList = null;
        }
    }

    private void SetPurchasedGoodsSlot(cmd_war_persongoodsinfo data)
    {
        //Value ASGoodsValue;
        ////GameMobaSchemesManager.Instance.GetASMobaGoods(data.nGoodID, out ASGoodsValue, this);

        //if (!Value.IsValueValid(ASGoodsValue) || vStoreWindow.Invoke("SetPurchasedGoodsSlot", data.nSlotIndex, ASGoodsValue, data.nCanUseTimes, 1) == null)
        //{
        //    Trace.LogError("error call SetPurchasedGoodsSlot failed");
        //    return;
        //}

    }

    /// <summary>
    /// 遍历所有已购买物品插槽，根据已购买物品ID更改与之相关的物品价格
    /// </summary>
    private void FindWhichGoodsCanEffect()
    {
         for (int i=0;i< curPurchasedGoodsList.Count; i++)
        {
            if(curPurchasedGoodsList[i].nGoodID != 0)
            {
                IteratorFindCanComposeGoods(curPurchasedGoodsList[i].nGoodID, ref allCanComposeGoodsMap);
            }
        }
    }

    /// <summary>
    /// 迭代当前已购买物品可合成物品，并存入重置列表
    /// </summary>
    /// <param name="_nCurrentGoodsID">当前已购买物品</param>
    /// <param name="_AllCanComposeGoodsMap">物品重置列表</param>
    private void IteratorFindCanComposeGoods(int _nCurrentGoodsID, ref Dictionary<int, List<int>> _AllCanComposeGoodsMap)
    {
        List<SSchemeMobaGoods> CurPurchasedGoodsCanComposeList;

        int deltaCost = 0;
        int newCost = 0;
        CurPurchasedGoodsCanComposeList = GameMobaSchemesManager.Instance.GetCanComposeSchemeGoodsList(_nCurrentGoodsID);
        List<cmd_war_persongoodsinfo> curPurchasedGoodsCopyList;

        m_nRunFindNum++;
        if (m_nRunFindNum == 1) m_nFirstGoodsID = _nCurrentGoodsID;
        if (m_nRunFindNum > 10)
        {
            Trace.LogError("配置物品的可合成物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nCurrentGoodsID.ToString());

            m_nRunFindNum--;
            m_nFirstGoodsID = 0;
            return;
        }

        //根据当前物品是否有可合成物品列表，来决定是否要更改价格
        if (CurPurchasedGoodsCanComposeList == null)
        {
            m_nRunFindNum--;
            return;
        }
            
        for (int j = 0; j < CurPurchasedGoodsCanComposeList.Count; j++)
        {

            curPurchasedGoodsCopyList = new List<cmd_war_persongoodsinfo>(curPurchasedGoodsList.ToArray());

            deltaCost = 0;
            //迭代合成所需物品列表
            IteratorFindNeedComposeGoods(CurPurchasedGoodsCanComposeList[j].GoodsID, curPurchasedGoodsCopyList, ref deltaCost);

            newCost = CurPurchasedGoodsCanComposeList[j].GoodsCost - deltaCost;

            SetAssociatedWithPurchasedGoodsSlot(CurPurchasedGoodsCanComposeList[j].GoodsID, newCost);

            if (!_AllCanComposeGoodsMap.ContainsKey(CurPurchasedGoodsCanComposeList[j].GoodsID))
            {
                _AllCanComposeGoodsMap.Add(CurPurchasedGoodsCanComposeList[j].GoodsID, new List<int>(0));
            }
            //迭代当前物品的可合成物品列表
            IteratorFindCanComposeGoods(CurPurchasedGoodsCanComposeList[j].GoodsID, ref _AllCanComposeGoodsMap);
        }

        curPurchasedGoodsCopyList = null;
        m_nRunFindNum--;
    }


    /// <summary>
    /// 迭代当前物品的合成所需物品列表，判断该列表里面是否有已购买物品，有则，扣除已购买物品价格
    /// </summary>
    /// <param name="_nCurGoodsID">当前物品</param>
    /// <param name="_curPurchasedGoodsCoypList">当前已购买物品列表</param>
    /// <param name="_nDeltaCost">当前需要扣除的价格</param>
    private void IteratorFindNeedComposeGoods(int _nCurGoodsID, List<cmd_war_persongoodsinfo> _curPurchasedGoodsCoypList, ref int _nDeltaCost)
    {
        //List<int> CurGoodsNeedList;
        //CurGoodsNeedList = GameMobaSchemesManager.Instance.GetSchemeGoodsNeedComposeList(_nCurGoodsID);
        //SSchemeMobaGoods CurSchemeMobaGoods = new SSchemeMobaGoods();

        //m_nRunFindNum++;
        //if (m_nRunFindNum == 1) m_nFirstGoodsID = _nCurGoodsID;
        //if (m_nRunFindNum > 10)
        //{
        //    m_nRunFindNum--;
        //    m_nFirstGoodsID = 0;
        //    Trace.LogError("配置物品的可合成所需物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nCurGoodsID.ToString());
        //    return;
        //}

        //if (CurGoodsNeedList == null)
        //{
        //    m_nRunFindNum--;
        //    return;
        //}

        //for (int i = 0; i < CurGoodsNeedList.Count; i++)
        //{
        //    //当前物品需要的合成物品遍历
        //    int needGoodID = CurGoodsNeedList[i];
        //    bool bFind = false;
        //    for (int k = 0; k < _curPurchasedGoodsCoypList.Count; k++)
        //    {
        //        int curHaveGoodID = _curPurchasedGoodsCoypList[k].nGoodID;
        //        if (curHaveGoodID == 0)
        //            continue;
        //        if(needGoodID == curHaveGoodID)
        //        {
        //            //找到了
        //            bFind = true;
        //            GameMobaSchemesManager.Instance.GetMobaGoods(needGoodID, ref CurSchemeMobaGoods);
        //            // 减去价格
        //            _nDeltaCost += CurSchemeMobaGoods.GoodsCost;
        //            _curPurchasedGoodsCoypList.RemoveAt(k);
        //            break;
        //        }
        //    }

        //    if (!bFind)
        //    {
        //        IteratorFindNeedComposeGoods(needGoodID, _curPurchasedGoodsCoypList, ref _nDeltaCost);
        //    }
        //}


        //for (int k = 0; k < _curPurchasedGoodsCoypList.Count; k++)
        //{
        //    if (_curPurchasedGoodsCoypList[k].nGoodID == 0)
        //        continue;
        //    for (int i = 0; i < CurGoodsNeedList.Count; i++)
        //    {
        //        //如果当前已购买物品与当前物品合成列表物品相同则，不需要计算当前合成物品的子物品
        //        if (_curPurchasedGoodsCoypList[k].nGoodID != CurGoodsNeedList[i])
        //            IteratorFindNeedComposeGoods(CurGoodsNeedList[i], _curPurchasedGoodsCoypList, ref _nDeltaCost);

        //        if (_curPurchasedGoodsCoypList[k].nGoodID == CurGoodsNeedList[i])
        //        {
        //            GameMobaSchemesManager.Instance.GetMobaGoods(_curPurchasedGoodsCoypList[k].nGoodID, ref CurSchemeMobaGoods);
        //            _nDeltaCost += CurSchemeMobaGoods.GoodsCost;
        //            CurGoodsNeedList.RemoveAt(i);
        //            _curPurchasedGoodsCoypList.RemoveAt(k);
        //            k = i = -1;//因为移除了，所以要重新遍历
        //            break;
        //        }
        //    }
        //}

        //CurSchemeMobaGoods = null;
        //m_nRunFindNum--;
    }

    /// <summary>
    /// 重置与已购买物品相关的物品价格
    /// </summary>
    private void ResetAssociatedWithPurchasedGoodsSlot()
    {
        //int nDeltaCost = 0;
        //SSchemeMobaGoods lastSchemeGoods = new SSchemeMobaGoods();
        //SSchemeMobaGoods schemeGoods = new SSchemeMobaGoods();

        //foreach (KeyValuePair<int, List<int>> keypair in allCanComposeGoodsMap)
        //{
        //    nDeltaCost = 0;

        //    GameMobaSchemesManager.Instance.GetMobaGoods(keypair.Key, ref schemeGoods);
        //    if (schemeGoods != null)
        //    {
        //        nDeltaCost = schemeGoods.GoodsCost;
        //    }

        //    SetAssociatedWithPurchasedGoodsSlot(keypair.Key, nDeltaCost);
        //}
        //if(Value.IsValueValid(vGoodsComposeTreeMgr))
        //{
        //    if(vGoodsComposeTreeMgr.Invoke("ResetTreeNodePurchasedGoodsState") == null)
        //    {
        //        Trace.LogError("call ResetTreeNodePurchasedGoodsState failed.");
        //        return;
        //    }
        //}
        //allCanComposeGoodsMap.Clear();

        //curPurchasedGoodsList.Clear();
    }

    /// <summary>
    /// 设置物品界面内所有物品渲染对象的显示效果，判断价格是否在玩家可购买的范围内
    /// </summary>
    /// <param name="_nCurMoney">当前价格</param>
    private void SetGoodsRenderCanBuy(int _nCurMoney)
    {
        if(Value.IsValueValid(vAllGoodsPage))
        {
            if(vAllGoodsPage.Invoke("SetGoodsRenderCanBuy",_nCurMoney) == null)
            {
                Trace.LogError("call AllGoodsPage.SetGoodsRenderCanBuy failed");
            }
        }

        if(Value.IsValueValid(vRecommendedGoodsPage))
        {
            if(vRecommendedGoodsPage.Invoke("SetGoodsRenderCanBuy",_nCurMoney) == null)
            {
                Trace.LogError("call RecommendedGoodsPage.SetGoodsRenderCanBuy failed");
            }
        }

        if(Value.IsValueValid(vCanComposeFrame))
        {
            if(vCanComposeFrame.Invoke("SetGoodsRenderCanBuy",_nCurMoney) == null)
            {
                Trace.LogError("call CanComposeFrame.SetGoodsRenderCanBuy failed");
            }
        }

        if (Value.IsValueValid(vGoodsComposeTreeMgr))
        {
            if (vGoodsComposeTreeMgr.Invoke("SetGoodsRenderCanBuy", _nCurMoney) == null)
            {
                Trace.LogError("call GoodsComposeTreeMgr.SetGoodsRenderCanBuy failed");
            }
        }

        //if (Value.IsValueValid(vDefaultGoodsPage))
        //{
        //    if (vDefaultGoodsPage.Invoke("SetGoodsRenderCanBuy", _nCurMoney) == null)
        //    {
        //        Trace.LogError("call vDefaultGoodsPage.SetGoodsRenderCanBuy failed");
        //    }
        //}
    }

    /// <summary>
    /// 修改与当前已购买物品相关的物品价格，如当前已购买物品可合成物品的价格
    /// </summary>
    /// <param name="_nCurGoodsID">当前与已购买物品相关的物品</param>
    /// <param name="_nNewCost">该物品新的价格</param>
    private void SetAssociatedWithPurchasedGoodsSlot(int _nCurGoodsID, int _nNewCost)
    {
        if (Value.IsValueValid(vAllGoodsPage))
        {
            if (vAllGoodsPage.Invoke("SetAssociatedWithPurchasedGoodsSlot", _nCurGoodsID, _nNewCost) == null)
            {
                Trace.LogError("call AllGoodsPage.SetAssociatedWithPurchasedGoodsSlot failed");
            }
        }
        if (Value.IsValueValid(vRecommendedGoodsPage))
        {
            if (vRecommendedGoodsPage.Invoke("SetAssociatedWithPurchasedGoodsSlot", _nCurGoodsID, _nNewCost) == null)
            {
                Trace.LogError("call RecommendedGoodsPage.SetAssociatedWithPurchasedGoodsSlot failed");
            }
        }

        if (Value.IsValueValid(vGoodsComposeTreeMgr))
        {
            if (vGoodsComposeTreeMgr.Invoke("SetPurchasedGoodsEx", _nCurGoodsID, _nNewCost) == null)
            {
                Trace.LogError("call vGoodsComposeTreeMgr.SetPurchasedGoodsEx failed.");
            }
        }

        if (Value.IsValueValid(vGoodsDetalInfo))
        {
            if (vGoodsDetalInfo.Invoke("SetAssociatedWithPurchasedGoodsSlot", _nCurGoodsID, _nNewCost) == null)
            {
                Trace.LogError("call vGoodsDetalInfo.SetAssociatedWithPurchasedGoodsSlot failed.");
            }
        }

        if(Value.IsValueValid(vCanComposeFrame))
        {
            if(vCanComposeFrame.Invoke("SetAssociatedWithPurchasedGoodsSlot",_nCurGoodsID,_nNewCost) == null)
            {
                Trace.LogError("call vCanComposeFrame.SetAssociatedWithPurchasedGoodsSlot faied");
            }
        }

        //if (Value.IsValueValid(vDefaultGoodsPage))
        //{
        //    if (vDefaultGoodsPage.Invoke("SetAssociatedWithPurchasedGoodsSlot", _nCurGoodsID, _nNewCost) == null)
        //    {
        //        Trace.LogError("call vDefaultGoodsPage.SetAssociatedWithPurchasedGoodsSlot faied");
        //    }
        //}


        SetGoodsRenderCanBuy(nPlayerCurrentMoney);
    }

    public void SetCanBuyOrSell(bool _bCanBuyOrSell)
    {
        bCanBuyOrSell = _bCanBuyOrSell;
        if (!Value.IsValueValid(vStoreWindow))
            return;

        if(vStoreWindow.Invoke("SetCanBuyOrSell",_bCanBuyOrSell) == null)
        {
            Trace.LogError("call SetCanBuyOrSell failed.");
            return;
        }
    }

    public void SetCanCancelOpration(bool _bCanCancelOpration)
    {
        bCanCancelOpration = _bCanCancelOpration;
        if (!Value.IsValueValid(vStoreWindow))
            return;

        if (vStoreWindow.Invoke("SetCanCancelOpration", _bCanCancelOpration) == null)
        {
            Trace.LogError("call SetCanCancelOpration failed.");
            return;
        }
    }
    #endregion
}

