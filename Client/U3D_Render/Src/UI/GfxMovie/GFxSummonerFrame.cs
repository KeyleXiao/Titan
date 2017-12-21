/*******************************************************************
** 文件名:	GFxSummonerFrame.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2016 - Speed
** 创建人:	马传飞
** 日  期:	2016/2/24
** 版  本:	1.0
** 描  述:	召唤师所有信息框架
** 应  用:  
 *          对应Flash Layout文件：SummonerFrame.swf/gfx
********************************************************************
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2016/3/17
** 描  述: 更改完善框架
********************************************************************/
using Scaleform;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

partial class GFxSummonerFrame : SFMovieEx
{
    #region AS实例
    public GFxTalentMenu TalentPage;
    protected Value vSummonerFrame;
    private bool bCreateAllSummonerFrame = false;
    private ESummonerState CurSummonerFrameState = ESummonerState.ESS_None;
    /// <summary>
    /// 当前召唤师界面
    /// </summary>
    private SFMovieEx CurSummonerFrame = null;
 
    /// <summary>
    /// 召唤师状态
    /// </summary>
    public enum ESummonerState
    {
        ESS_None,
        ESS_Info,
        ESS_Hero,
        ESS_Record,
        ESS_TalentPage
    }
    #endregion

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        bCanSyncTickCount = true;
        CreateSummonerFrame();
    }

    override public void Update()
    {

        if (InputManager.GetKey(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.SystemPannel)))
        {

            SetState(ESummonerState.ESS_None);
        }
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);

        GetMemberWithCheck(MyRoot, "SummonerFrame", out vSummonerFrame);

       // SetMovieVisibleOnCreate();
        CreateSummonerFrame();
    }
	
    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);
        this.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer);

    }
	
    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
    }
	
    public bool CreateSummonerFrame()
    {
        bool res = false;
        if (bCreateAllSummonerFrame)
            return res;








        if (TalentPage != null)
        {
            bCreateAllSummonerFrame = true;
            res = true;
        }
        return res;
    }
	
    override public void OnDestroy()
    {
        SFGFxMovieManager.Instance.DestroyWindow("GFxTalentMenu");

        TalentPage = null;

        CurSummonerFrameState = ESummonerState.ESS_None;
        bCreateAllSummonerFrame = false;


        base.OnDestroy();

    }
	
    public bool SetState(ESummonerState nState)
    {
        if (nState == CurSummonerFrameState)
        {
            return false;
        }
        onExit(CurSummonerFrameState);
        CurSummonerFrameState = nState;
        onEnter(CurSummonerFrameState);

        return true;
    }
	
    private void onEnter(ESummonerState nState)
    {
        switch (nState)
        {
            case ESummonerState.ESS_TalentPage:
                {
                    CurSummonerFrame = TalentPage;
                    if (SFGFxMovieManager.CheckTheMovieCanUse(TalentPage))
                    {
                        TalentPage.EnterMovie();
                       // ShowSummonerFrame(true);
                    }

                }
                break;
            default:
                {
                    break;
                }
        }
        if (CurSummonerFrame != null)
        {
            CurSummonerFrame.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer+1);
        }


    }
	
    private void onExit(ESummonerState nState)
    {
        switch (nState)
        {
            case ESummonerState.ESS_None:

                break;
            case ESummonerState.ESS_TalentPage:
                {
                    if (SFGFxMovieManager.CheckTheMovieCanUse(TalentPage))
                    {
                        TalentPage.LeaveMovie();
                    }
                }
                break;
       
        }

    }
    #region AS->CS
	
    public void OnSummonerFrameClickButtonIndex(int nSummonerIndex)
    {
       // CreateSummonerFrame((ESummonerState)nSummonerIndex);
     
        SetState((ESummonerState)nSummonerIndex);
 
    }
	
     public void onClickCloseSummoner()
     {
         SetState(ESummonerState.ESS_None);
        Trace.TraceSFUIWaring("GFxMMODefaultHud", "GFxSummonerFrame.onClickCloseSummoner", " mMoMenu.SetState(GFxMMODefaultHud.EHallWinState.EHWS_None);");

     }
    #endregion
}

