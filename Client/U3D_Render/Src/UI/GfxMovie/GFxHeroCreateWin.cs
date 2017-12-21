/*******************************************************************
** 文件名:	GFxHeroCreateWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2014/11/11
** 版  本:	1.0
** 描  述:	GFx 静态英雄创建界面
				其余的登录界面逻辑类都用GFx开头区分
** 应  用:  	对应Flash Layout文件：HeroCreateWin.swf/gfx

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
using System.Xml;
using ASpeedGame.Data.GameSettingsXml;
using U3D_Render.Common;
using ASpeedGame.Data.ShieldWorld;

public class GFxHeroCreateWin : SFMovieEx
{
    /// <summary>
    /// 界面选择的英雄类型
    /// </summary>
    private int HeroType = 1;



    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        //  SetViewport(0, 0, 500, 600);
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        SetMovieVisibleOnCreate();

    }

    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);
        this.SetFocus(true);
    }
    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
        this.SetFocus(false);
    }

    public void onConfirmBtnClick(int _nHeroID,string _strHeroName)
    {
        if (ShieldWorld.Instance.IsConformString(_strHeroName) == false)
        {
            GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_HEROCREATE_BADNAME);
            return;
        }
        

        gameview_create_player data = new gameview_create_player();
        data.heroid = _nHeroID;
        data.name = _strHeroName;
        // 皮肤id暂时按照英雄id来定 之后修改
        data.skinid = /*_nHeroID*/0;

        ViewEventHelper.Instance.SendCommand<gameview_create_player>(GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER, ref data);
    }

    public void onReturnBtnClick()
    {
        this.LeaveMovie();

        Trace.TraceSFUIWaring("GFxHeroSelect", "onReturnBtnClick", "EnterMovie、getDefaultHeroInfo、HeroBtnPress");
    }
}
