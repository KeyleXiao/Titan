/*******************************************************************
** 文件名:	GFxMainChatHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 主聊天框信息
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

class GFxMainChatHud : SFMovieEx
{

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
    }

    override public void Update()
    {

    }

    public void OnRegisterMainChatHud(Value _root)
    {
        MyRoot = _root;

        this.MyMovieIsRegisterSuccess = true;


        Vector3 ScreenPos = Vector3.zero;
        Vector3 StagePos = Vector3.zero;
        double _h = (double)MyRoot.GetMember("height");
        ScreenPos.x = 0;
        ScreenPos.y = (float)_h;
        StagePos = this.ScreenPointToStagePoint(ScreenPos);
        double _x = StagePos.x;
        double _y = StagePos.y;

        MyRoot.SetMember("x", _x);
        MyRoot.SetMember("y", _y);
    }
}