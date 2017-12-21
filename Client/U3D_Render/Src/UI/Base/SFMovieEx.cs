/*******************************************************************
** 文件名:	SFMovieEx.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/28
** 版  本:	1.0
** 描  述:	GFx界面自定义基础类
** 应  用:  
 *          
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;

public class SFMovieEx : Movie
{
    /////////////////////////////////////////////////////////////////基本属性
    protected Value MyRoot = null;
    /// <summary>
    /// 判断swf是否已经注册成功
    /// </summary>
    protected bool MyMovieIsRegisterSuccess = false;
    //创建界面之后是否马上显示
    protected bool MyMovieIsVisibleOnCreate = false;
    /// <summary>
    /// Root的DisplayInfo
    /// </summary>
    protected SFDisplayInfo MyRootDI = null;
    /// <summary>
    /// 存储当前movie显示状态
    /// </summary>
    protected bool bIsVisible = false;
    /// <summary>
    /// 是否为悬浮窗口
    /// 悬浮窗口可以被拖动、鼠标点击时可以置顶、弹出式窗口
    /// 注：窗口要能接受鼠标事件
    /// </summary>
    protected bool bIsFloat = false;
    /// <summary>
    /// 是否需要同步Tick到AS那边，如果movie(swf)有使用到同步定时器，启用该同步
    /// true: 同步tick  false：不同步tick
    /// </summary>
    protected bool bCanSyncTickCount = false;
    /////////////////////////////////////////////////////////////////


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp) //:base(_sfMgr, _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        SFMgr = _sfMgr;

        EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
    }

    override public void Update()
    {

    }

    override public void OnDestroy()
    {
        Trace.TraceSFUIWaring("GFxTooltipHud", "GFxTooltipHud", "HideTooltipEx");

        if (GFxTooltipHud.Instance != null)
        {
            GFxTooltipHud.Instance.HideTooltipEx(GFxTooltipHud.Instance.CurrentTooltipType);
        }
        EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        OnUnResisterDocument();
        base.OnDestroy();
    }

    override public void SyncTickCountToAS(double _nDelta)
    {
        if (!bCanSyncTickCount)
            return;

        if (Value.IsValueValid(MyRoot))
        {
            if (MyRoot.Invoke("DocumentSyncTick", _nDelta) == null)
            {
                bCanSyncTickCount = false;
                Trace.LogError("movie: " + MovieName + " ,call DocumentSyncTick falid.");
                return;
            }
        }
    }

    public virtual void OnRegisterDocument(Value _root)
    {
        MyRoot = _root;
        MyMovieIsRegisterSuccess = true;
        InitDocumentWidgetEnd();
        SetMovieVisibleOnCreate();
    }

    public virtual void OnUnResisterDocument()
    {
        if(MyRoot == null)
        {
            Trace.LogError("movie: " + MovieName + " MyRoot 对象时空的，检测fla文档类是否是新版本的。");
            return;
        }
        if(MyRoot.Invoke("OnUnResisterDocument") == null)
        {
            Trace.LogError("movie:" + this.MovieName + " call OnUnResisterDocument failed.");
            return;
        }
    }

    /// <summary>
    /// 子类重构这个方法就好了
    /// </summary>
    protected virtual void InitDocumentWidgetEnd()
    {
        InitDocumentWidgetEnd2AS();
    }
    protected void InitDocumentWidgetEnd2AS(params object[] obj)
    {
        if (Value.IsValueValid(MyRoot))
        {
            //基类必须要的
            double tickCount = Convert.ToDouble(GameLogicAPI.getSyncTickCount());
            
            object[] defaultObj = new object[] { tickCount, bCanSyncTickCount };

            //组合两个数组
            object[] invokeObjs = new object[defaultObj.Length + obj.Length];

            defaultObj.CopyTo(invokeObjs, 0);
            obj.CopyTo(invokeObjs, defaultObj.Length);

            if (MyRoot.Invoke("InitDocumentWidgetEnd", invokeObjs) == null)
                Trace.LogError("call InitDocumentWidgetEnd failed.");
        }

    }

    public bool IsRegisterMovieSuccess
    {
        get
        {
            return this.MyMovieIsRegisterSuccess;
        }
    }

    public bool IsVisibleOnCreate
    {
        get
        {
            return this.MyMovieIsVisibleOnCreate;
        }
        set
        {
            this.MyMovieIsVisibleOnCreate = value;
        }
    }

    public bool IsVisible
    {
        get
        {
            return bIsVisible;
        }
    }

    public bool IsFloat
    {
        get
        { return bIsFloat; }
    }

    /// <summary>
    /// 设置界面的显示、隐藏
    /// </summary>
    /// <param name="_visible"></param>
    /// <returns></returns>
    public virtual bool SetMovieVisible(Boolean _visible)
    {
        bool res = false;

        if (MyRootDI != null)
        {
            bIsVisible = _visible;
            MyRootDI.Visible = _visible;
            MyRoot.SetDisplayInfo(MyRootDI);
            res = true;
        }
        else
        {
            Trace.LogWarning("界面显示设置失败!");
            res = false;
        }
        return res;
    }
    /// <summary>
    /// 显示界面
    /// </summary>
    public virtual void EnterMovie()
    {
        bIsVisible = true;
    }
    /// <summary>
    /// 隐藏界面
    /// </summary>
    public virtual void LeaveMovie()
    {
        bIsVisible = false;
    }
    /************************************************************************/
    /* Tooltip                                                              */
    /************************************************************************/
    public void ShowTooltip(string _tooltipType, Value _tooltipElememts)
    {
        //Trace.LogError("TooltipType=" + _tooltipType);
        //Trace.TraceSFUIWaring("GFxTooltipHud", "GFxTooltipHud", "ParseTooltipInformaton");
        if (GFxTooltipHud.Instance != null)
        {
            GFxTooltipHud.Instance.ParseTooltipInformaton(_tooltipType, _tooltipElememts);
        }
    }
    /************************************************************************/
    /* UiSound                                                              */
    /************************************************************************/
    public void OnCallCreatSound( int nSoundId )
    {
        if ( nSoundId == 0 )
        {
            return;
        }
        SoundManager.CreateUISound(nSoundId);
    }

    public void OnDeleteSoundAll()
    {
        SoundManager.DeleteAll();
    }


    public void HideTooltip(string _tooltipType)
    {
        if (GFxTooltipHud.Instance != null)
        {
            GFxTooltipHud.Instance.HideTooltipEx(_tooltipType);
        }
    }

     /************************************************************************/
     /*                                                                      */
     /************************************************************************/
    /// <summary>
    /// 获取_valueForm里面的Value，获取失败时带报错
    /// </summary>
    /// <param name="_valueForm">从该Value内获取值</param>
    /// <param name="_elemName">需要获取值的实例名</param>
    /// <param name="_MemberValue">实例名对应的Value值</param>
    /// <returns>true获取成功；false获取失败</returns>
    protected bool GetMemberWithCheck(Value _valueForm, String _elemName,out Value _MemberValue)
    {
        Value result = new Value();

        if (!_valueForm.GetMember(_elemName, ref result))
        {
            Trace.LogError("获取"+_elemName+"值失败！");
            _MemberValue = null;
            return false;
        }

        _MemberValue = result;
        return true;
    }

    protected void SetMovieVisibleOnCreate()
    {
        SetMovieRootDisplayInfoOnCreate();

        if (IsVisibleOnCreate)
        {
            SetMovieVisible(true);
        }
    }

    private void SetMovieRootDisplayInfoOnCreate()
    {
        if (MyRootDI == null && Value.IsValueValid(MyRoot))
        {
            this.MyRootDI = MyRoot.GetDisplayInfo();
        }
    }

    /// <summary>
    /// 数值属性变化处理
    /// 子类覆盖使用
    /// </summary>
    /// <param name="data"></param>
    public virtual void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {

    }
}