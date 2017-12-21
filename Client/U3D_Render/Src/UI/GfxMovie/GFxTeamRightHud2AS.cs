/*******************************************************************
** 文件名:	GFxTeamInfoHud2AS.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：TeamInfoHud.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;

partial class GFxTeamRightHud
{
    #region C#->AS
    //更新血条，蓝条，等级时候调用as的相应接口
    override public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {
        int id = e.data.nPBUID;
        if (!entityHPSContainer.ContainsKey(id))
        {
            return;
        }
        EntityView ev = e.objev;    // EntityFactory.getEntityViewByID(id);
        Value RightHeroPortraitSlot = entityHPSContainer[id] as Value;
        if (!RightHeroPortraitSlot.IsValid())
        {
            return;
        }
        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_HP:
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                {
                    int nMaxHP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                    int nHP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);

                    if (RightHeroPortraitSlot.Invoke("SetHealthBarValue", nMaxHP, nHP) == null)
                    {
                        Trace.LogError("error call SetHealthBarValue failed");
                        return;
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_MP:
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                {
                    int nMaxMP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);
                    int nMP = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);

                    if (RightHeroPortraitSlot.Invoke("SetMagicBarValue", nMaxMP, nMP) == null)
                    {
                        Trace.LogError("error call SetMagicBarValue failed");
                        return;
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                {
                    int nLevel = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                    if (RightHeroPortraitSlot.Invoke("SetLevelValue", nLevel) == null)
                    {
                        Trace.LogError("error call SetLevelValue failed");
                        return;
                    }
                }
                break;
        }
    }


    //右侧信息栏，同阵营玩家信息槽
    public void SetPlayerHeroSlotInfo(IntPtr ptrParam, int nPtrLen)
    {

        if (!Value.IsValueValid(empytPointMC))
        {
            return;
        }
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count nCountdata = helper.get<cmd_count>();
        for (int i = 1; i <= nCountdata.nCount; ++i)
        {
            cmd_entity_camp_palyer_uidinfo data = helper.get<cmd_entity_camp_palyer_uidinfo>();
            Value NewHPS = new Value();
            //连接as接口
            if (!empytPointMC.AttachMovie(ref NewHPS, this.rightHeroPortraitClass, this.rightheroPortraitInsName + i, 1))
            {
                Trace.LogError("attach RightHeroPortraitSlot Value failed");
                return;
            }
            if (!Value.IsValueValid(NewHPS))
                return;

            //建立相应位置
            SFDisplayInfo NewHPSDI = NewHPS.GetDisplayInfo();
            NewHPSDI.Y = (70) * i;
            NewHPSDI.X = 205;
            NewHPS.SetDisplayInfo(NewHPSDI);
            //将头像信息存入哈希表
            entityHPSContainer[(int)data.uPBUID] = NewHPS;
            ////初始化头像，设置头像
            //Value CurHPS = entityHPSContainer[(int)data.uPBUID] as Value;

            if (NewHPS.Invoke("SetHeroIcon", data.nHeroID) == null)
            {
                Trace.LogError("error call SetHeroIcon failed!");
                return;
            }
            NewHPS.SetMember("uPBUID", data.uPBUID);
        }
    }

    // 設置敵對英雄頭像及cd時間
    public void SetRivalryHeroInfo(int uId, int nHeroIconID, int nReliveTime)
    {

        if (!Value.IsValueValid(empytPointMC))
            return;
        this.nRivalryNum++;
        Value NewHPS = new Value();
        if (!empytPointMC.AttachMovie(ref NewHPS, this.heroPortraitClass, this.heroPortraitInsName + this.nRivalryNum, 1))
        {
            Trace.LogError("error attach RightHeroPortraitSlot Value failed");
            return;
        }
        if (!Value.IsValueValid(NewHPS))
            return;
        SFDisplayInfo NewHPSDI = NewHPS.GetDisplayInfo();
        NewHPSDI.Y = (70) * this.nRivalryNum;
        NewHPSDI.X = 130;
        NewHPS.SetDisplayInfo(NewHPSDI);
        rivalryGfxTable[uId] = NewHPS;

        if (NewHPS.Invoke("SetHeroIcon", nHeroIconID, uId) == null || NewHPS.Invoke("SetHeroReliveTime", nReliveTime, true) == null)
        {
            Trace.LogError("error SetRivalryHeroInfo call SetHeroIcon or SetHeroReliveTime failed!");
            return;
        }
    }

    // 遍历敌人死亡列表，改变位置即可
    public void ShowRivalryDieList()
    {
        foreach (int key in rivalryGfxTable.Keys)
        {
            this.nRivalryNum++;
            Value NewHPS = rivalryGfxTable[key] as Value;
            if (!Value.IsValueValid(NewHPS))
            {
                return;
            }
            SFDisplayInfo NewHPSDI = NewHPS.GetDisplayInfo();
            NewHPSDI.Y = (70) * this.nRivalryNum;
            NewHPS.SetDisplayInfo(NewHPSDI);
        }

    }

    //设置战场玩家信息复活时间
    public void SetTeamRightInfoPlayerReliveTime(cmd_war_important_entiyreliveinfo data)
    {
        //获得死亡者UID
        int _nEntityID = (int)data.uPBUID;
        if (!entityHPSContainer.ContainsKey(_nEntityID))
        {
            return;
        }
        int _nReliveTime = (int)data.dwRevliveTime;
        Value RightHeroPortrait = entityHPSContainer[_nEntityID] as Value;

        if (!Value.IsValueValid(RightHeroPortrait))
            return;

        if (RightHeroPortrait.Invoke("SetHeroReliveTime", _nReliveTime) == null)
        {
            Trace.LogError("error call SetHeroReliveTime failed!");
            return;
        }

    }
    //设置玩家大招图标的点亮和熄灭
    public void SetTeamRightInfoShowFinalSkillIcon(cmd_player_skill_state data)
    {
        int _nEntityID = (int)data.uPBUID;
        if (!entityHPSContainer.ContainsKey(_nEntityID))
        {
            return;
        }
        Value RightHeroPortrait = entityHPSContainer[_nEntityID] as Value;

        if (!Value.IsValueValid(RightHeroPortrait))
            return;

        if (RightHeroPortrait.Invoke("SetULTInfo", data.nCoolTime) == null)
        {
            Trace.LogError("error call SetTeamRightInfoShowFinalSkillIcon failed!");
            return;
        }
    }

    //玩家离开战场时设置头像效果
    public void SetTeamRightInfoHeroLeaveWar(int nEntityID)
    {
        if (!entityHPSContainer.ContainsKey(nEntityID))
        {
            return;
        }

        Value RightHeroPortrait = entityHPSContainer[nEntityID] as Value;

        if (!Value.IsValueValid(RightHeroPortrait))
            return;

        if (RightHeroPortrait.Invoke("SetHeroLeaveWar") == null)
        {
            Trace.LogError("error call SetHeroLeaveWar failed!");
            return;
        }

    }

    //玩家返回战场时设置头像效果
    public void SetTeamRightInfoHeroReturnWar(int nEntityID)
    {
        if (!entityHPSContainer.ContainsKey(nEntityID))
        {
            return;
        }

        Value RightHeroPortrait = entityHPSContainer[nEntityID] as Value;

        if (!Value.IsValueValid(RightHeroPortrait))
            return;

        if (RightHeroPortrait.Invoke("SetHeroReturnWar") == null)
        {
            Trace.LogError("error call SetHeroReturnWar failed!");
            return;
        }

    }
    #endregion
}