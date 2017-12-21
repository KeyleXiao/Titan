/*******************************************************************
** 文件名:	GFxGameHighLadder.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2016/01/05
** 版  本:	1.0
** 描  述:	GFx 段位天梯界面
				
** 应  用:  	对应Flash Layout文件：GameHighLadder.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2016/01/05
** 描  述: 拆分C#代码中与AS通讯的逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;


public partial class GFxGameHighLadder : SFMovieEx
{
    protected Value vRoomItemStruct;
    protected Value vRoomListFrame;
    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
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
        GetMemberWithCheck(_root, "RoomListFrame", out vRoomListFrame);
        GetMemberWithCheck(vRoomListFrame, "RoomItemStruct", out vRoomItemStruct);
        SetMovieVisibleOnCreate();
    }
    #endregion

   #region AS->C#

    // 返回按钮回调
    public void onReturnBtnClick()
    {
        Trace.TraceSFUIWaring("GFxMMODefaultHud", "GFXGameHigLader.onReturnBtnClick", "MMODefaultHud.SetState(GFxMMODefaultHud.EHallWinState.EHWS_None);");
    }
    // 刷新按钮回调
    public void onRefreshHighLadderClick()
    {
        // 请求服务器排名是否变化，然后下发 ps：目前未做处理
      
    }

#endregion

}
