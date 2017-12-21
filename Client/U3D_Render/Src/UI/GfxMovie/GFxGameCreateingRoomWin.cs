/*******************************************************************
** 文件名:	GFxGameCreateingRoomWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/03/19
** 版  本:	1.0
** 描  述:	GFx 玩家创建房间界面
				
** 应  用:  	对应Flash Layout文件：GameCreateingRoomWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;


public class GFxGameCreateingRoomWin : SFMovieEx
{
    private Value GameTypeFrame = null;
    private Value MapInfoFrame = null;
    private Value MapOptionFrame = null;

    private int CurrentGameMode = 0;

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        Value CreatingFrame;
        GetMemberWithCheck(MyRoot, "CreatingFrame", out CreatingFrame);
        GetMemberWithCheck(CreatingFrame, "GameTypeFrame", out GameTypeFrame);
        GetMemberWithCheck(CreatingFrame, "MapOptionFrame", out MapOptionFrame);
        GetMemberWithCheck(CreatingFrame, "MapInfoFrame", out MapInfoFrame);

        SetMovieVisibleOnCreate();
    }

    #region AS->C#
    public void onCreateRoomBtnClick()
    {
        ////判断AS丢来的房间属性数量是否正确
        //if (_mapProterty.GetArraySize() != 2)
        //{
        //    Trace.LogError("AS传来的房间属性不正常，创建房间失败");
        //    return;
        //}


        //int tmpMapID = (int)_mapProterty.GetElement(0);
        //Trace.LogError("tmpMapID=" + tmpMapID);
        //点击创房间之后进入等待其他玩家加入界面


        
        cmd_match_create_room data = new cmd_match_create_room();
        data.szName = "";

        ViewEventHelper.Instance.SendCommand<cmd_match_create_room>(GameLogicDef.GVIEWCMD_MATCH_CREATE_ROOM, ref data);

       // (SFGFxMovieManager.GetWindow<("GFxGameWaitingRoomWin") as GFxGameWaitingRoomWin).SetMovieVisible(true);
      //  this.SetMovieVisible(false);
    }

    public void onModeButtonClick(int _modeID)
    {
        CurrentGameMode = _modeID;
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM_MODE_MAPS, _modeID);
    }

    public void onMapButtonClick(int _nMapID,int _nSchemeConfigWarID)
    {
        cmd_match_create_room_click_map data = new cmd_match_create_room_click_map();
        data.nMapID = _nMapID;
        data.nSchemeConfigWarID = _nSchemeConfigWarID;

        ViewEventHelper.Instance.SendCommand<cmd_match_create_room_click_map>(GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM_CLICK_MAP, ref data);
    }

    public void onReturnBtnClick()
    {
        Trace.TraceSFUIWaring("GFxMMODefaultHud", "onReturnBtnClick", "MMODefaultHud.SetState(GFxMMODefaultHud.EHallWinState.EHWS_Hall);");

        //GFxMMODefaultHud MMODefaultHud = SFGFxMovieManager.GetWindow<GFxMMODefaultHud>();
        //if (SFGFxMovieManager.CheckTheMovieCanUse(MMODefaultHud))
        //{
        //    MMODefaultHud.SetState(GFxMMODefaultHud.EHallWinState.EHWS_Hall);
        //}
        //else
        {
            this.SetMovieVisible(false);
            Trace.TraceSFUIWaring("GFxGameHallWin", "onReturnBtnClick", "EnterMovie");
        }
    }

    public void onMatchBtnClick()
    {
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH);
    }

    public void onCancelMatchBtnClick()
    {
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_CANCEL_MATCH);
    }
    #endregion

    #region 接口

    public void SetGameModeBtnInfo(int _index, cmd_match_create_room_mode _data)
    {
        if (!Value.IsValueValid(GameTypeFrame))
            return;

        Boolean bSelect = Convert.ToBoolean(_data.bSelect);

        if (GameTypeFrame.Invoke("SetModeButtonInfo",_index, _data.nModeID, _data.szName, bSelect) == null)
        {
            Trace.LogError("call SetModeButtonInfo failed");
            return;
        }
    }

    public void SetMapDes(cmd_match_create_room_map_info _data)
    {
        if (Value.IsValueValid(MapInfoFrame))
        {
            if (MapInfoFrame.Invoke("SetMapInfo", _data.szDesc, _data.szPicPath) == null)
            {
                Trace.LogError("error ,call SetMapInfo failed!");
            }
        }
    }

    /// <summary>
    /// 设置地图布局里的地图按钮
    /// </summary>
    /// <param name="_mapBtnIndex">地图按钮在布局内的序号</param>
    public void SetGameModeMapLayout(int _mapBtnIndex, cmd_match_create_room_map _data)
    {
        //ModeID与ModeBtnIndex差1
        if (!Value.IsValueValid(MapOptionFrame))
            return;

        Boolean bSelect = Convert.ToBoolean(_data.bSelect);

        if (MapOptionFrame.Invoke("SetMapBtnInfo",_mapBtnIndex-1, _data.nMapID, _data.szName,bSelect,_data.nSchemeConfigWarID) == null)
        {
            Trace.LogError("call SetMapBtnInfo faild");
            return;
        }
    }

    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);
    }
    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
    }
#endregion
}
