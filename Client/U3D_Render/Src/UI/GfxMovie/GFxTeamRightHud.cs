/*******************************************************************
** 文件名:	GFxTeamRightHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/9/12
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：TeamInfoHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;

partial class GFxTeamRightHud : SFMovieEx
{
    //实体ID与对应分配的玩家信息插槽 列表
    private Hashtable entityHPSContainer = new Hashtable();
    // 敌人id与对应分配的信息插槽列列表
    private Hashtable rivalryGfxTable = new Hashtable();
    //实体PUUID与对应的UID
    private Hashtable PBUIDTable = new Hashtable();
    //队友右侧信息栏信息
    private string rightHeroPortraitClass = "Com.Scaleform.Hud.Right_HeroPortrait";
    private string rightheroPortraitInsName = "Right_HeroPortrait";
    //敌方死亡右侧信息栏信息
    private string heroPortraitClass = "Com.Scaleform.Hud.TIH_HeroPortrait";
    private string heroPortraitInsName = "TIH_HeroPortrait";
    private int nRivalryNum = 0;
    //放置的空点基点
    private Value empytPointMC;

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        bCanSyncTickCount = true;
        // SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    override public void Update()
    {

    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "EmpytPointMC", out empytPointMC);
        SetMovieVisibleOnCreate();

    }

    // 显示敌对英雄死亡cd时间
    public void ShowRivalryDieInfo(cmd_war_important_entiyreliveinfo data)
    {
        int nMyCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
        // 队友方和自己不存
        if (TestIsSameCampWithMyCamp(ref data.nCamp, ref nMyCamp) || EntityFactory.MainHeroID == data.uReliveUID)
        {
            return;
        }
        int nEntityId = (int)data.uReliveUID;
        int nReliveTime = (int)data.dwRevliveTime;
        SetRivalryHeroInfo(nEntityId, data.nHeroIconID, nReliveTime);
    }
    // 存入uid，供点击右侧头像通过pbuid获得uid
    public void SaveUIDAndPBUID(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count nCountdata = helper.get<cmd_count>();
        int nMyCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
        for (int i = 0; i < nCountdata.nCount; ++i)
        {
            cmd_Send_UID_And_PBUID data = helper.get<cmd_Send_UID_And_PBUID>();
            // 敌方和自己不存
            if (!TestIsSameCampWithMyCamp(ref data.nCamp, ref nMyCamp) || EntityFactory.MainHeroID == data.uUID)
            {
                continue;
            }
            PBUIDTable[data.uPBUID] = data.uUID;
        }
    }

    public bool TestIsSameCampWithMyCamp(ref int nCamp, ref int nMyCamp)
    {
        if (nMyCamp == nCamp)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    private void SendTeamRightClickPlayerUID(int _nUID)
    {
        SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
        if (sc != null)
        {
            sc.TeamRightClickPlayerUID = _nUID;
        }
    }

    #region AS->C#
    public void OnRivalryRelive(int nId)
    {
        this.nRivalryNum = 0;
        if (rivalryGfxTable.Contains(nId) == false)
        {
            return;
        }
        // 真正的移除子节点
        empytPointMC.RemoveMovieAS3(rivalryGfxTable[nId] as Value);
        rivalryGfxTable.Remove(nId);
        ShowRivalryDieList();
    }

    public void onMouseClickPortrait(uint uClickPortraitPBUID)
    {
        if (PBUIDTable.ContainsKey(uClickPortraitPBUID))
        {
            SendTeamRightClickPlayerUID(Convert.ToInt32(PBUIDTable[uClickPortraitPBUID]));
        }
    }
    #endregion
}

