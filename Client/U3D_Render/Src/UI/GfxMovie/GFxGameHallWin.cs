/*******************************************************************
** 文件名:	GFxGameHallWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/03/19
** 版  本:	1.0
** 描  述:	GFx 游戏大厅界面
				
** 应  用:  	对应Flash Layout文件：GameHallWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 吴欣伟
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;
using USpeedUI;


public partial class GFxGameHallWin : SFMovieEx
{
    protected Value vRoomItemStruct;
    protected Value vMapFrame;
    protected Value vRoomListFrame;
    /// <summary>
    /// 当前选择的房间ID
    /// </summary>
    private int _nCurrentSelectedRoomIndex = -1;


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        //  this.TheScaleModeType = ScaleModeType.SM_ShowAll;
    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }

#region 其余的外部接口
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

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(_root, "MapFrame", out vMapFrame);
        GetMemberWithCheck(_root, "RoomListFrame", out vRoomListFrame);
        GetMemberWithCheck(vRoomListFrame, "RoomItemStruct", out vRoomItemStruct);

        SetMovieVisibleOnCreate();
    }
    #endregion

    #region AS->C#

    /// <summary>
    /// 创建房间按钮
    /// </summary>
    public void onCreateRoomBtnClick()
    {
        //向逻辑层发显示层需要创建房间消息
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM);
    }
    /// <summary>
    /// 加入房间按钮
    /// </summary>
    /// <param name="_curSelectRoom">选择的房间对象</param>
    public void onJoinRoomBtnClick()
    {
        onGameViewEvent_MatchEnterRoom();
    }
    public void onRoomBtnDoubleClick(int _curSelectRoomIndex)
    {
        _nCurrentSelectedRoomIndex = _curSelectRoomIndex;
        onGameViewEvent_MatchEnterRoom();
    }

    public void onRoomBtnClick(int _curSelectRoomIndex)
    {
        _nCurrentSelectedRoomIndex = _curSelectRoomIndex;
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCM_MATCH_HALL_ROOM_DATA, _nCurrentSelectedRoomIndex);
    }
    public void onReturnBtnClick()
    {


        UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, true);
    }

    //通知逻辑层进入选定的房间
    private void onGameViewEvent_MatchEnterRoom()
    {
        if (_nCurrentSelectedRoomIndex != -1)
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM, _nCurrentSelectedRoomIndex);
        }
    }
#endregion

}
