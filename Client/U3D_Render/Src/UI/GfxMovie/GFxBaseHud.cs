/*******************************************************************
** 文件名:	GFxBaseHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2014/12/26
** 版  本:	1.0
** 描  述:	所有UI都要共用的东西放这，如：GFx鼠标显示、隐藏
** 应  用:  	对应Flash Layout文件：BaseHud.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.3.17
** 描  述: 
 *         1、更改父类为SFMovieEx
 *         2、在获取AS对象时添加检测
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using U3D_Render.Common;
using GameLogic;


public class GFxBaseHud : SFMovieEx
{
    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;

		//将U3D系统鼠标隐藏
        //Cursor.visible = false;
/*
        if (InputManager.GetKeyDown(KeyCode.M))
        {
            SetCursorState(MouseCursorState.MCS_Sign);
        }
        if (InputManager.GetKeyDown(KeyCode.N))
        {
            SetCursorState(MouseCursorState.MCS_Default);
        }
		*/
    }

    public void OnRegisterBaseHud(Value _root)
    {
        MyRoot = _root;
        //正常来说这里应该不会失败吧....还是加个检测好了
        if (!MyRoot.IsValid())
        {
            Trace.LogError("BaseHud 注册root的AS对象失败！");
            return;
        }
        //GetMemberWithCheck(MyRoot, "_cursorManager", out CursorManager);

        SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Base);
        this.MyMovieIsRegisterSuccess = true;
    }
    
    #region SFMessageBox
    public enum EMessageBoxType : int
    {
        EMBT_None = 0,
        EMBT_OK = 1,
        EMBT_Cancel = 2,
        EMBT_Close = 4,
        EMBT_OKCancel = 8
    }
    
    /// <summary>
    /// AS->    public function CreateMessageBox(_title:String,_message:String,_type:uint,_btnOKCallBack:String=null,_btnCancelCallBack:String=null)
    /// </summary>
    /// <param name="_title">信息框标题</param>
    /// <param name="_message">信息内容，可html</param>
    /// <param name="_type">信息框显示类型</param>
    /// <param name="_btnOKCallBack">确认按钮点击回调，可为空</param>
    /// <param name="_btnCancelCallBack">取消按钮点击回调，可为空</param>
    public void CreateMessageBox(string _title, string _message, EMessageBoxType _type, string _btnOKCallBack = null, string _btnCancelCallBack = null)
    {
        int type = (Convert.ToInt32(_type));
        Trace.Log("CreateMessageBox");
        if (MyRoot.Invoke("CreateMessageBox", _title, _message, type, _btnOKCallBack, _btnCancelCallBack) == null)
        {
            Trace.LogError("error call CreateMessageBox failed");
            return;
        }
        SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Loading);
    }
    /// <summary>
    /// 游戏设置属性确认更改
    /// </summary>
    public void onGameSettingsConfirm()
    {
        GFxWarPauseMenu pauseMenu = SFGFxMovieManager.GetWindow<GFxWarPauseMenu>();
        if (SFGFxMovieManager.CheckTheMovieCanUse(pauseMenu))
        {
            pauseMenu.onGameSettingsConfirm();
        }
    }
    /// <summary>
    /// 游戏设置属性取消更改
    /// </summary>
    public void onGameSettingsCancel()
    {
        GFxWarPauseMenu pauseMenu = SFGFxMovieManager.GetWindow<GFxWarPauseMenu>();
        if (SFGFxMovieManager.CheckTheMovieCanUse(pauseMenu))
        {
            pauseMenu.onGameSettingsCancel();
        }
    }

    public void back2LoginState()
    {
        //GameLogicAPI.onGameViewEvent();
        // 退回到登录状态，todo.
    }

    public void forceLogin()
    {
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_FORCE_LOGIN);
    }
    #endregion
}

