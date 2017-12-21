/*******************************************************************
** 文件名:	GFxHeroSelect2AS.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	吴欣伟
** 日  期:	2015/9/28
** 版  本:	1.0
** 描  述:	英雄选择界面逻辑,拆分C#代码中与AS通讯的逻辑
** 应  用:  对应Flash Layout文件：HeroSelectWin.swf/gfx
********************************************************************/

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;

public partial class GFxHeroSelect
{
    #region C#->AS
    //设置技能插槽技能信息
    public void SetSkillSlotInfo(int _slotIndex, int _IconID, string _skillName, string _skillDes, int _skillCD)
    {
        if (!Value.IsValueValid(CCFrame))
            return;

        //SetSkillSlotInfo(_skillSlot:int,_skillID:int,_skillName:String,_skillDes:String,_skillCD:int)
        if (this.CCFrame.Invoke("SetSkillSlotInfo", _slotIndex, _IconID, _skillName, _skillDes, _skillCD) == null)
        {
            Trace.LogError("error call SetSkillSlotInfo failed");
            return;
        }
        
    }

    public void SetHeroHeadSlotInfo(cmd_match_room_hero_slot _data)
    {
        //int LSelectBtnGroupLength = this.TLFrame.GetMember("LHeroPortraitBtnGroupLength");
        //Value tmpTopFrame = _data.nSlotIndex / LSelectBtnGroupLength < 1 ? TLFrame : TRFrame;
        if (!Value.IsValueValid(TFrame))
            return;

        int LSelectBtnGroupLength = TFrame.GetMember("LHeroPortraitBtnGroupLength");

        int SlotIndex = _data.nSlotIndex % LSelectBtnGroupLength;
        bool bIsLock = Convert.ToBoolean(_data.bIsLock);

        if (TFrame.Invoke("ChangeHeroBtn", SlotIndex, _data.nHeroID, bIsLock, false, _data.szHeroTooltip) == null)
        {
            Trace.LogError("call ChangeHeroBtn failded");
            return;
        }

        HeroPortraitTable.Add(_data.nSlotIndex, _data.nHeroID);
    }

    public void SetBtnNormalState(int _slotIndex, int _heroID)
    {
        if (!Value.IsValueValid(TFrame))
            return;

        int LSelectBtnGroupLength = TFrame.GetMember("LHeroPortraitBtnGroupLength");
        int SlotIndex = _slotIndex % LSelectBtnGroupLength;
        // 函数ChangeHeroBtn 暂时设置为0 之后修改

        if (PlayerLoginData.playercnt == 0)
        {
            Trace.LogWarning("SetBtnNormalState PlayerLoginData.playercnt = " + PlayerLoginData.playercnt);
            return;
        }
        int pID = (int)PlayerLoginData.players[0].pid;

        if (TFrame.Invoke("ChangeHeroBtn", _slotIndex, pID, _heroID, false, true) == null)
        {
            Trace.LogError("call ChangeHeroBtn failded");
            return;
        }
    }
    private void SetHeroHeadSlot()
    {
        //HeroPortraitTable.Clear();
        if (!Value.IsValueValid(TFrame))
            return;

        for (int i = 0; i < PlayerLoginData.playercnt; i++)
        {
            int pID = (int)PlayerLoginData.players[i].pid;
            if (TFrame.Invoke("ChangeHeroBtn", i, pID, PlayerLoginData.players[i].heroid, false, false, "") == null)
            {
                Trace.LogError("call ChangeHeroBtn failded");
                return;
            }
            
            if (!HeroPortraitTable.Contains(i))
                HeroPortraitTable.Add(i, PlayerLoginData.players[i].skinid);

        }
    }
    public void SetHeroProperty(cmd_choose_hero_sync _data)
    {
        //播放属性条动画
        if (!Value.IsValueValid(CCFrame))
            return;

        if (!this.CCFrame.GotoAndPlayFrame("ShowHeroProperty"))
        {
            Trace.LogError("error CCFrame.gotoAndPlay(ShowHeroProperty) failed");
            return;
        }

        //设置属性值
        //SetPropertyBar(_HP:int,_AD:int,_AP:int,_Hard:int)
        if (CCFrame.Invoke("SetPropertyBar", _data.nHp, _data.nPA, _data.nMA, _data.nOperateFactor) == null)
        {
            Trace.LogError("error CCFrame.SetPropertyBar failed");
            return;
        }
        //设置英雄名字
        //SetHeroName(_strHeroName:String)
        if (CCFrame.Invoke("SetHeroName", _data.szVocation) == null)
        {
            Trace.LogError("error CCFrame.SetHeroName failed");
            return;
        }
        //设置英雄类型图标
        //SetVocationTypeIcon(_strVocation:String,_strVocationName:String)
        if (CCFrame.Invoke("SetVocationTypeIcon", _data.szImage, _data.szLocate) == null)
        {
            Trace.LogError("error CCFrame.SetVocationTypeIcon failed");
            return;
        }
    }
    #endregion
}