/*******************************************************************
** 文件名:	GFxMMODefaultHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/7/13
** 版  本:	1.0
** 描  述:	非战场界面的
** 应  用:  
 *          对应Flash Layout文件：MainChatHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using GameLogic;
using USpeedUI;

partial class GFxMMODefaultHud : SFMovieEx
{
    #region 大厅界面
    private GFxGameHallWin HallWin;
    private GFxGameCreateingRoomWin CreatingRoomWin;
    private GFxGameWaitingRoomWin WaitingRoomWin;
    private GFxGameHighLadder HighLadder;
    private GFxSummonerFrame SummonerFrame;
    //protected Value vStrengthLineButton;
    /// <summary>
    /// 大厅所有界面是否已经创建完成
    /// </summary>
    private bool bCreateAllHallWin = false;
    /// <summary>
    /// 大厅状态
    /// </summary>
    public enum EHallWinState
    {
        EHWS_None,
        EHWS_Hall,
        EHWS_Creating,
        EHWS_Waiting,
        EHWS_HighLadder,
        EHWS_Summoner
    }
    /// <summary>
    /// 当前大厅状态
    /// </summary>
    private EHallWinState CurHallWinState = EHallWinState.EHWS_None;
    /// <summary>
    /// 当前大厅界面
    /// </summary>
    private SFMovieEx CurHallWin = null;
    #endregion

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

        CreateHallWin();
    }

    override public void Update()
    {
        onTick();

        if (InputManager.GetKey(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.SystemPannel)))
        {

            SetState(EHallWinState.EHWS_None);
        }
    }

    override public void OnDestroy()
    {

        HallWin = null;
        CreatingRoomWin = null;
        WaitingRoomWin = null;
        HighLadder = null;
        this.SummonerFrame = null;
        CurHallWinState = EHallWinState.EHWS_None;
        bCreateAllHallWin = false;


        base.OnDestroy();

    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        //GetMemberWithCheck(_root, "OpenStrengthLineButton", out vStrengthLineButton);
    }

    #region AS->CS

    #endregion

    #region 大厅
    #region AS->CS
    public void onOpenHallButtonClick()
    {
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ENTER_HALL);
        //测试用
        //GameLogicAPI.onGameViewEvent((int)GameLogicDef.GVIEWCMD_KIN_QUIT);
        //GameLogicAPI.onGameViewEvent((int)GameLogicDef.GVIEWCMD_KIN_OPEN);
        //GameLogicAPI.onGameViewEvent((int)GameLogicDef.GVIEWCMD_KIN_DISMISS);
        //GameLogicAPI.onGameViewEvent((int)GameLogicDef.GVIEWCMD_KIN_CREATE);
        //GameLogicAPI.onGameViewEvent((int)GameLogicDef.GVIEWCMD_KIN_APPLY);      
    }

    // 段位入口点击回调
    public void onOpenHighLadderButtonClick()
    {

        if (!SetState(EHallWinState.EHWS_HighLadder))
        {
            onEnter(EHallWinState.EHWS_HighLadder);
        }
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HIGH_LADDER);
    }

    // 召唤师信息入口
    public void onOpenSummonerInfoButtonClick()
    {
        // TODO显示召唤师信息
        this.SetState(EHallWinState.EHWS_Summoner);
    }

    #endregion

    private bool CreateHallWin()
    {
        bool res = false;
        if (bCreateAllHallWin)
            return res;

        if (HallWin == null)
        {
            //创建大厅界面,默认隐藏
            // HallWin = SFGFxMovieManager.Instance.CreateWindow<GFxGameHallWin>("GFxGameHallWin", "GFx/GameHallWin.swf", SFGFxMovieManager.DefaultBGColor);
            //   HallWin.SetViewport(ox, oy, w, h);
        }
        if (CreatingRoomWin == null)
        {
            //创建游戏房间界面,默认隐藏
            // CreatingRoomWin = SFGFxMovieManager.Instance.CreateWindow<GFxGameCreateingRoomWin>("GFxGameCreateingRoomWin", "GFx/GameCreatingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
        }
        if (WaitingRoomWin == null)
        {
            //创建玩家等待界面,默认隐藏
            //if (!Config.UseNewSlot)
            //{
            //    WaitingRoomWin = SFGFxMovieManager.Instance.CreateWindow<GFxGameWaitingRoomWin>("GFxGameWaitingRoomWin", "GFx/GameWaitingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
           // }
            // WaitingRoomWin = SFGFxMovieManager.Instance.CreateWindow<GFxGameWaitingRoomWin>("GFxGameWaitingRoomWin", "GFx/GameWaitingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
        }




        if (HallWin != null && CreatingRoomWin != null && WaitingRoomWin != null && HighLadder && SummonerFrame)
        {
            bCreateAllHallWin = true;
            res = true;
        }
        return res;
    }
    public bool SetState(EHallWinState nState)
    {
        if (nState == CurHallWinState)
        {
            return false;
        }
        Trace.Log("MMOHudState old:" + CurHallWinState);
        onExit(CurHallWinState);
        CurHallWinState = nState;
        onEnter(CurHallWinState);
        Trace.Log("MMOHudState new:" + CurHallWinState);
        return true;
    }
    private void onEnter(EHallWinState nState)
    {
        switch (nState)
        {
            case EHallWinState.EHWS_None:
                {
                    if (CurHallWin != null)
                    {
                        CurHallWin.SetMovieVisible(false);
                        if (CurHallWin == WaitingRoomWin)
                        {
                            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
                        }
                        else
                            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_EXIT_HALL);

                        UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, true);
                        CurHallWin = null;
                    }

                }
                break;
            case EHallWinState.EHWS_Hall:
                {
                    CurHallWin = HallWin;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(HallWin))
                    {
                        HallWin.EnterMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Waiting:
                {
                    CurHallWin = WaitingRoomWin;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(WaitingRoomWin))
                    {
                        WaitingRoomWin.EnterMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Creating:
                {
                    CurHallWin = CreatingRoomWin;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(CreatingRoomWin))
                    {
                        CreatingRoomWin.EnterMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_HighLadder:
                {
                    CurHallWin = HighLadder;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(HighLadder))
                    {
                        HighLadder.EnterMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Summoner:
                {
                    CurHallWin = SummonerFrame;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(SummonerFrame))
                    {
                        SummonerFrame.EnterMovie();
                    }
                }
                break;
            default:
                {
                    break;
                }
        }
        if (CurHallWin != null)
        {
            CurHallWin.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer);
        }


    }
    private void onTick()
    {

        switch (CurHallWinState)
        {
            case EHallWinState.EHWS_None:
                break;
            case EHallWinState.EHWS_Hall:
                {

                }
                break;
            case EHallWinState.EHWS_Waiting:
                break;
            case EHallWinState.EHWS_Creating:
                break;
        }

    }
    private void onExit(EHallWinState nState)
    {
        switch (nState)
        {
            case EHallWinState.EHWS_None:

                break;
            case EHallWinState.EHWS_Hall:
                {
                    if (SFGFxMovieManager.CheckTheMovieCanUse(HallWin))
                    {
                        HallWin.LeaveMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Waiting:
                {
                    if (SFGFxMovieManager.CheckTheMovieCanUse(WaitingRoomWin))
                    {
                        WaitingRoomWin.LeaveMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Creating:
                {
                    if (SFGFxMovieManager.CheckTheMovieCanUse(CreatingRoomWin))
                    {
                        CreatingRoomWin.LeaveMovie();
                    }
                }
                break;
            case EHallWinState.EHWS_Summoner:
                {
                    CurHallWin = SummonerFrame;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(SummonerFrame))
                    {
                        SummonerFrame.LeaveMovie();
                    }
                }
                break;
        }

    }
    #endregion

}