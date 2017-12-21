/*******************************************************************
** 文件名:	GFxTalentMenu.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2016/2/24
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：GFxTalentMenu.swf/gfx*/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;
using ASpeedGame.Data.TalentPage;
using System.Runtime.InteropServices;
public class TalentPageInfo
{
    public string szTalentPageName;
    public int nAttackPointAll;
    public int nTrickPointAll;
    public int nHelpPointAll;
}

public partial class GFxTalentMenu : SFMovieEx
{
    public Value TalentMenuFrame;
    public Value TalentCenterPointFrame;
    public List<string> TalentPointsList = new List<string>();
    public Dictionary<int, TalentPageInfo> dTalentPageInfo=new Dictionary<int, TalentPageInfo>();
    public bool bIsHasLoadPageInfo;
   
    public enum TalentType
    {
        // 攻击系
        ENUM_TALENTTYPE_ATTACK = 1,
        // 诡诈系
        ENUM_TALENTTYPE_TRICK,
        // 辅助系
        ENUM_TALENTTYPE_HELP
    };

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        bCanSyncTickCount = true;
   
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;

    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "talentMenuManager", out TalentMenuFrame);
        GetMemberWithCheck(TalentMenuFrame, "TalentPointsCenter", out TalentCenterPointFrame);
        SetTalentButtonBaseInfo();
        bIsHasLoadPageInfo = false;

    }

    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);
        this.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer+1);
        OnRequestTalentPagePoints();
    }

    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
    }
    #region AS->C#

    public void OnRequestTalentPagePoints()
    {
        // 暂定加载一次
        if(this.bIsHasLoadPageInfo)
        {
            return;
        }
        // 请求所有加点情况
        cmd_Entity_Request_Talent_Page cmdPointAll = new cmd_Entity_Request_Talent_Page();
        cmdPointAll.bIsRequestSelf = 1;
        cmdPointAll.RequestFriendID = 0;
        IntPtrHelper helper = new IntPtrHelper();
        
        IntPtr ptr = helper.toPtr<cmd_Entity_Request_Talent_Page>(ref cmdPointAll);
        int len = Marshal.SizeOf(cmdPointAll);
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQUEST_TALENT_PAGE_ALL, len, "", ptr, len);
        this.bIsHasLoadPageInfo = true;
    }

    public void OnTalentPagePointsInfo(int nPageIndex, string szPointsData, string szTalentId, string szTalentPageName,int nAttackPoints,int nTrickPoints,int nHelpPoints)
    {

        // 请求保存当前页数据
        TalentPageInfo talentPageInfo = new TalentPageInfo();
        talentPageInfo.szTalentPageName = szTalentPageName;
        talentPageInfo.nAttackPointAll = nAttackPoints;
        talentPageInfo.nTrickPointAll = nTrickPoints;
        talentPageInfo.nHelpPointAll = nHelpPoints;
        this.dTalentPageInfo[nPageIndex] = talentPageInfo;

        cmd_TalentPage_Points_Single cmdSinglePoint = new cmd_TalentPage_Points_Single();
        cmdSinglePoint.nCurPageIndex = nPageIndex;
        string[] sTalentIdArray = ((string)(szTalentId)).Split(';');
        int nArrayLen=sTalentIdArray.Length-1;
        cmdSinglePoint.nTalentId = new int[nArrayLen];
        cmdSinglePoint.nTalentPoints = new int[nArrayLen];
        for (int i = 0; i < nArrayLen; i++)
        {

            cmdSinglePoint.nTalentId[i] = int.Parse(sTalentIdArray[i]);
            cmdSinglePoint.nTalentPoints[i] = int.Parse(szPointsData[i].ToString()); ;
        }
        cmdSinglePoint.nTalentNum = (byte)nArrayLen;
        cmdSinglePoint.szTalentPageName = szTalentPageName;
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_TalentPage_Points_Single>(ref cmdSinglePoint);
        int len = Marshal.SizeOf(cmdSinglePoint);
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SAVE_TALENT_PAGE_SINGLE, len, "", ptr, len);
    }

    public void OnTalentPagePointsDelete(int nPageIndex)
    {
        this.dTalentPageInfo.Remove(nPageIndex);
        cmd_Entity_Talent_Page_Detele cmdPointDelete = new cmd_Entity_Talent_Page_Detele();
        cmdPointDelete.nCurTalentPage = nPageIndex;

        IntPtrHelper helper = new IntPtrHelper();

        IntPtr ptr = helper.toPtr<cmd_Entity_Talent_Page_Detele>(ref cmdPointDelete);
        int len = Marshal.SizeOf(cmdPointDelete);
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_BEGIN_TALENT_PAGE_DELETE, len, "", ptr, len);

    }
    // 获得数据库索引，保存和删除都是数据库索引而不是排序号
    public void GetCurPageDataIndex()
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return;
        }
         int nCurPageNum = TalentMenuFrame.Invoke("GetCurPageDataIndex");
         cmd_Entity_Talent_Start_Effect cmdPointEffect = new cmd_Entity_Talent_Start_Effect();
         cmdPointEffect.nCurTalentPage = nCurPageNum;
   
         IntPtrHelper helper = new IntPtrHelper();

         IntPtr ptr = helper.toPtr<cmd_Entity_Talent_Start_Effect>(ref cmdPointEffect);
         int len = Marshal.SizeOf(cmdPointEffect);
         GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_BEGIN_TALENT_PAGE_EFFECT, len, "", ptr, len);

    }

     public void OnTalentCloseClick()
     {
         GFxGameWaitingRoomWin waitingRoom = SFGFxMovieManager.GetWindow<GFxGameWaitingRoomWin>();
         if (SFGFxMovieManager.CheckTheMovieCanUse(waitingRoom))
         {
             waitingRoom.SetTalentDropInfo();
         }
     }
   
    #endregion
}