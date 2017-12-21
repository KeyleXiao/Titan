/*******************************************************************
** 文件名:	GFxLoadingWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/03/30
** 版  本:	1.0
** 描  述:	GFx 载入界面
** 应  用:  	对应Flash Layout文件：GFxLoadingWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;


public partial class GFxLoadingWin : SFMovieEx
{
    #region GFx属性
    protected Value StatusIndicatorMC = null;
    protected Value vWarLoadingMgr = null;
    #endregion

    public enum ELoadingType
    {
        ELW_None,
        ELW_Default,
        ELW_MOBA,
    }
    private ELoadingType curLoadingTyep;
    private struct FPlayerFrameData
    {
        public int nIndex;
        public int nCampPlayerCount;
        public int nCamp;

        public FPlayerFrameData(int _nIndex,int _nCampPlayerCount,int _nCamp)
        {
            nIndex = _nIndex;
            nCampPlayerCount = _nCampPlayerCount;
            nCamp = _nCamp;
        }
    }
    //<pdbid,FrameData>
    private Dictionary<uint, FPlayerFrameData> dicPlayerFrameTable;



    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }
    public override void OnRegisterDocument(Value Movie)
    {
        base.OnRegisterDocument(Movie);
        GetMemberWithCheck(MyRoot, "StatusIndicatorMC", out StatusIndicatorMC);
        GetMemberWithCheck(MyRoot, "WarLoadingMgr", out vWarLoadingMgr);
        //InputManager.ImeCompositionMode = IMECompositionMode.On;
    }

    public override bool SetMovieVisible(bool _visible)
    {
        //设置UI的深度
        if (_visible)
        {
            //在最上层
            this.Depth =(int) SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Loading;
        }
        else
        {
            this.Depth = 0;
        }
        return base.SetMovieVisible(_visible); 
    }

    public void EnterMovie(ELoadingType type)
    {
        curLoadingTyep = type;
        switch (curLoadingTyep)
        {
            case ELoadingType.ELW_Default:
                EnterByDefault();
                break;
            case ELoadingType.ELW_MOBA:
                EnterByMOBA();
                break;
        }
        base.EnterMovie();
    }

    //不管从哪种情况进入加载界面，都从这里离开
    public override void LeaveMovie()
    {
        switch (curLoadingTyep)
        {
            case ELoadingType.ELW_Default:
                LeaveByDefault();
                break;
            case ELoadingType.ELW_MOBA:
                LeaveByMOBA();
                break;
        }
        curLoadingTyep = ELoadingType.ELW_None;
        base.LeaveMovie();
    }

    #region 普通加载界面

    public double SetIndicatorCurValue
    {
        set
        {
            if (!Value.IsValueValid(StatusIndicatorMC))
                return;
            if(StatusIndicatorMC.Invoke("SetStatusValue", value,100,0) == null)
            {
                Trace.LogError("call GFxLoadingWin.SetStatusValue failed!");
                return;
            }
        }
    }

    public void AttachStatusIndicatorBar()
    {
        if (!Value.IsValueValid(StatusIndicatorMC))
            return;

        if (MyRoot.Invoke("AttachStatusIndicatorBar") == null)
        {
            Trace.LogError("call AttachStatusIndicatorBar failed.");
            return;
        }
    }

    public void DisAttachStatusIndicatorBar()
    {
        if (!Value.IsValueValid(StatusIndicatorMC))
            return;

        if (MyRoot.Invoke("DisAttachStatusIndicatorBar") == null)
        {
            Trace.LogError("call DisAttachStatusIndicatorBar failed.");
            return;
        }
    }

    #endregion

    //普通的加载背景
    private void EnterByDefault()
    {
        this.SetMovieVisible(true);
        this.curLoadingTyep = ELoadingType.ELW_Default;
        AttachStatusIndicatorBar();
         //因为这里是显示普通的加载背景图，以后尽量用多几张图，然后随机显示
         SetLoadingWinBG("FSIR_3");
    }
    private void LeaveByDefault()
    {
        this.SetMovieVisible(false);
        DisAttachStatusIndicatorBar();
        DisAttachLoadingBg();
    }
    //加载moba游戏模式的背景图
    private void EnterByMOBA()
    {
        this.SetMovieVisible(true);
        SetLoadingWinBG("FSIR_2");
        AttachWarLoadingMgr();
        this.curLoadingTyep = ELoadingType.ELW_MOBA;
    }
    private void LeaveByMOBA()
    {
        this.SetMovieVisible(false);
        DisAttachWarPlayerFrame();
        DisAttachLoadingBg();
        DisAttachWarLoadingMgr();
    }

    public void RecvAllHeroId(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count cmdCount = helper.get<cmd_count>();
        int HeroID = 0;
        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            int nHeroId = helper.get<int>();
            // TODO 调用小强接口
            if(nHeroId != 0)
            {
                LightingEffectFactory.Instance.PushCacheVocation((VOCATION)nHeroId);
                HeroID = nHeroId;
            }
        }

        if(0 != HeroID)
        {
            LightingEffectFactory.Instance.ChangeHeroVocation((VOCATION)HeroID);
        }
    }
}
