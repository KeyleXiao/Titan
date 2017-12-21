/*******************************************************************
** 文件名:	GFxWarGoodsStore.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/8/8
** 版  本:	1.0
** 描  述:	战场购买物品界面
** 应  用:  
 *          对应Flash Layout文件：WarGoodsStore.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/03/10
** 描  述: 增加快捷键绑定
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using ASpeedGame.Data.GameMobaSchemes;
using System.Collections.Generic;
using GameLogic;
using System.Runtime.InteropServices;

partial class GFxWarGoodsStore : SFMovieEx
{
    protected Value vStoreWindow;
    /// <summary>
    /// 当前英雄推荐物品显示页面
    /// </summary>
    protected Value vRecommendedGoodsPage;
    /// <summary>
    /// 所有物品显示页面
    /// </summary>
    protected Value vAllGoodsPage;
    /// <summary>
    /// 英雄默认物品页
    /// </summary>
    protected Value vDefaultGoodsPage;
    /// <summary>
    /// 当前玩家的金钱文本
    /// </summary>
    protected Value vMoneyTF;
    /// <summary>
    /// 当前选择物品能合成的物品框
    /// </summary>
    protected Value vCanComposeFrame;
    /// <summary>
    /// 当前选择物品合成树
    /// </summary>
    protected Value vGoodsComposeTreeMgr;
    /// <summary>
    /// 所有物品页的标签框
    /// </summary>
    protected Value vAllGoodsTypeFrame;
    /// <summary>
    /// 当前选择物品详细信息框
    /// </summary>
    protected Value vGoodsDetalInfo;
    //当前选择的物品ID
    protected int nCurrentSelectedGoodsID;
    //当前选择的已购买物品插槽ID
    protected int nCurSelectedPurchasedSlotID;
    //临时存储玩家金钱数量
    protected int nPlayerCurrentMoney;
    //判断当前能否执行物品购买，出售，撤销操作
    protected bool bCanBuyOrSell;
    //判断当前能否执行撤销操作
    protected bool bCanCancelOpration;
	//检测函数迭代次数，避免死循环
    private int m_nRunFindNum = 0;
	//检测函数迭代次数，避免死循环
    private int m_nFirstGoodsID = 0;

    /// <summary>
    /// 判断是否商店界面是否已经打开
    /// </summary>
    public bool IsGoodsStoreOpened
    {
        get
        {
            return bIsVisible;
        }
    }
    /// <summary>
    /// 设置当前选择的物品ID，相应会通知AS显示该物品的可合成物品与需要合成的物品
    /// </summary>
    public int CurrentSelectedGoodsID
    {
        set
        {
            if (nCurrentSelectedGoodsID == value)
                return;

            nCurrentSelectedGoodsID = value;

        }

        get
        { return nCurrentSelectedGoodsID; }
    }
    /// <summary>
    /// 当前下发的已购买物品列表
    /// </summary>
    protected List<cmd_war_persongoodsinfo> curPurchasedGoodsList = new List<cmd_war_persongoodsinfo>();
    /// <summary>
    /// 当前已购买物品插槽数量
    /// </summary>
    protected int nPurchasedGoodsSlotCount = 7;
    /// <summary>
    /// 存储与当前已购买物品相关的所有物品
    /// </summary>
    protected Dictionary<int, List<int>> allCanComposeGoodsMap = new Dictionary<int, List<int>>();

    GameMobaSchemesManager MobaSchemesManager = null;
    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

        MobaSchemesManager = GameMobaSchemesManager.Instance;
        bIsFloat = true;
    }

    public override void OnDestroy()
    {
        PopupMovieManager.Instance.UnBindKeyCodeForMovie(this);
        base.OnDestroy();

        curPurchasedGoodsList.Clear();
        curPurchasedGoodsList = null;

        allCanComposeGoodsMap.Clear();
        allCanComposeGoodsMap = null;
    }

    public override void Update()
    {

    }

    public override void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {
        if(e.objev == null || !this.IsRegisterMovieSuccess)
        {
            return;
        }

        switch(e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT:
                {
                    if(e.objev.IsHero)
                    {
                        nPlayerCurrentMoney = e.data.nValue;

                        if (this.bIsVisible)
                        {
                            this.vMoneyTF.SetText(e.data.nValue.ToString());
                            SetGoodsRenderCanBuy(e.data.nValue);
                        }
                    }
                }
                break;
        }
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "StoreWindow", out vStoreWindow);

        GetMemberWithCheck(vStoreWindow, "MoneyTF", out vMoneyTF);
        GetMemberWithCheck(vStoreWindow, "AllGoodsPage", out vAllGoodsPage);

        GetMemberWithCheck(vAllGoodsPage, "AllGoodsTypeFrame", out vAllGoodsTypeFrame);

        GetMemberWithCheck(vStoreWindow, "RecommendedGoodsPage", out vRecommendedGoodsPage);
        GetMemberWithCheck(vStoreWindow, "CanComposeFrame", out vCanComposeFrame);
        GetMemberWithCheck(vStoreWindow, "GoodsComposeTreeMgr", out vGoodsComposeTreeMgr);
        GetMemberWithCheck(vStoreWindow, "GoodsDetalInfo", out vGoodsDetalInfo);

        PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.OpenGoodStore), this);

        GameMobaSchemesManager.MobaGoodsSchemesInitEndEvent += GetMobaGoodsFromSchemes;

        SetMovieVisibleOnCreate();
    }
    #region AS->CS
    public void onMobaGoodsItemClick(int _nGoodsID, int _nPurchasedSlotID,bool bInfluenceCanCompose,bool bInfluenceComposeTree)
    {
        CurrentSelectedGoodsID = _nGoodsID;
        nCurSelectedPurchasedSlotID = _nPurchasedSlotID;

        if(bInfluenceCanCompose)
            ShowSelectedGoodsCanCompose(nCurrentSelectedGoodsID);
        if(bInfluenceComposeTree)
            ShowSelectedGoodsComposeTree(nCurrentSelectedGoodsID);
    }
    public void onRevokeBtn()
    {
        if(bCanBuyOrSell)
        {
            cmd_war_revocation_operation data = new cmd_war_revocation_operation();
            data.uUID = (uint)EntityFactory.MainHeroID;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_war_revocation_operation>(ref data);
            int len = Marshal.SizeOf(data);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_REVOCATION_OPERATION, len, "", ptr, len);
        }

    }
    public void onSellBtnClick()
    {
        if(bCanBuyOrSell && CurrentSelectedGoodsID != 0 && nCurSelectedPurchasedSlotID != -1)
        {
            cmd_war_personsellgoods data = new cmd_war_personsellgoods();
            data.nGoodID = CurrentSelectedGoodsID;
            data.nSlotIndex = nCurSelectedPurchasedSlotID;

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_war_personsellgoods>(ref data);
            int len = Marshal.SizeOf(data);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_SELL_GOOD, len, "", ptr, len);
        }
    }

    public void onBuyBtnClick()
    {
        onSendWarBuyGoodsToLogic(CurrentSelectedGoodsID);
    }
    public void onGoodsItemBuyByRightClick(int _goodsID)
    {
        onSendWarBuyGoodsToLogic(_goodsID);
    }

    public void onCloseButtonClick()
    {
        PopupMovieManager.Instance.PopMovie(this);
    }
    #endregion

    private void onSendWarBuyGoodsToLogic(int _bugGoodsID)
    {
        if (!bCanBuyOrSell)
        {
            GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_GOODS_CANNOTBUY);
            return;
        }
            
        cmd_war_personbuygoods buyGoods = new cmd_war_personbuygoods();
        buyGoods.nGoodID = _bugGoodsID;


        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_war_personbuygoods>(ref buyGoods);
        int len = Marshal.SizeOf(buyGoods);
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_BUY_GOOD, len, "", ptr, len);

    }
    private void GetMobaGoodsFromSchemes(GameMobaSchemesManager.MobaGoodsSchemesEventArgs arg)
    {
        //BoxGoodsLabelDescriptrion(arg.LabelDescirptionTable);

        //BoxRecommendedGoodsByGoodsType( arg.RecommendGoods.RecommendGoods_EerlyList, (int)(EMobaRecommendGoods.EMRG_EerlyStage - 1));
        //BoxRecommendedGoodsByGoodsType( arg.RecommendGoods.RecommendGoods_MiddleList, (int)(EMobaRecommendGoods.EMRG_MiddleStage - 1));
        //BoxRecommendedGoodsByGoodsType( arg.RecommendGoods.RecommendGoods_DownWindList, (int)(EMobaRecommendGoods.EMRG_DownWindStage - 1));
        //BoxRecommendedGoodsByGoodsType( arg.RecommendGoods.RecommendGoods_UpWindList, (int)(EMobaRecommendGoods.EMRG_UpWindStage - 1));
        //BoxRecommendedGoodsByGoodsType( arg.RecommendGoods.RecommendGoods_ConsumeGoodsList, (int)(EMobaRecommendGoods.EMRG_ConsumeGoods - 1));
        //if (arg.AllMobaGoodsTable.Count != 0 )
        //{
        //    BoxAllGoods(arg.AllMobaGoodsTable);
        //}

        //BoxAllDefaultGoods(arg.AllMobaGoodsTable, arg.RecommendGoods.RecommendGoods_DefaultGoodsTypeList);

        //GameMobaSchemesManager.MobaGoodsSchemesInitEndEvent -= GetMobaGoodsFromSchemes;
    }

    private void IteratorFindNeedGoods(int _nCurrentGoodsID,List<GoodsCTNode> _list)
    {
        //GoodsCTNode node = new GoodsCTNode();

        ////GameMobaSchemesManager.Instance.GetASMobaGoods(_nCurrentGoodsID, out node.ASGoodsValue, this);
        
        //List<int> CurrentGoodsNeedComposeList = GameMobaSchemesManager.Instance.GetSchemeGoodsNeedComposeList(_nCurrentGoodsID);

        //m_nRunFindNum++;
        //if (m_nRunFindNum == 1) m_nFirstGoodsID = _nCurrentGoodsID;
        //if (m_nRunFindNum > 10)
        //{
        //    Trace.LogError("配置物品的合成所需物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nCurrentGoodsID.ToString());
        //    m_nRunFindNum--;
        //    m_nFirstGoodsID = 0;
        //    return;
        //}

        //if (CurrentGoodsNeedComposeList == null)
        //{
        //    node.nChildCount = 0;
        //    _list.Add(node);
        //}
        //else
        //{
        //    node.nChildCount = CurrentGoodsNeedComposeList.Count;
        //    _list.Add(node);

        //    for (int i = 0; i < CurrentGoodsNeedComposeList.Count; i++)
        //    {
        //        IteratorFindNeedGoods(CurrentGoodsNeedComposeList[i], _list);
        //    }
        //}

        //m_nRunFindNum--;
    }
    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(bIsVisible);

        //do something when movie is open
        this.vMoneyTF.SetText(nPlayerCurrentMoney.ToString());
        SetGoodsRenderCanBuy(nPlayerCurrentMoney);
        FindWhichGoodsCanEffect();
    }
    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(bIsVisible);
    }

    /// <summary>
    /// 清除已购买物品槽
    /// </summary>
    public void ClearPurchasedGoodsSlot()
    {
        cmd_war_persongoodsinfo data = new cmd_war_persongoodsinfo();
        data.nGoodID = 0;
        data.nCanUseTimes = 0;
        for (int i = 0;i<7;i++)
        {
            data.nSlotIndex = i;
            SetPurchasedGoodsSlot(data);
        }

        vRecommendedGoodsPage.Invoke("Clear");
        GameMobaSchemesManager.MobaGoodsSchemesInitEndEvent += GetMobaGoodsFromSchemes;

    }
}

class GoodsCTNode
{
    public int nChildCount;
    public Value ASGoodsValue;
}