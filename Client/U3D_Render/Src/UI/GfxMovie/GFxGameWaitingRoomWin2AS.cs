/*******************************************************************
** 文件名:	GFxGameWaitingRoomWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	吴欣伟
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	GFx 玩家等待开始游戏界面,拆分C#中与AS通讯部分逻辑
				
** 应  用:  	对应Flash Layout文件：GameWaitingRoomWin.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;
using U3D_Render.Common;
using Stage_ManagerDef;
using System.Collections.Generic;
using ASpeedGame.Data.HeroAIConfig;
using DataCenter;
using USpeedUI;
public partial class GFxGameWaitingRoomWin
{
    #region C#->AS
    public void ResetPlayerFunction()
    {
        if (Value.IsValueValid(vPlayerFunctionFrame))
        {
            if (vPlayerFunctionFrame.Invoke("ResetPlayerFunction") == null)
            {
                Trace.LogError("call ResetPlayerFunction failed.");
                return;
            }
        }
    }
    /// <summary>
    /// 设置房主的权利
    /// </summary>
    /// <param name="_bIsLocal">当前设置的是否为本地客户端</param>
    /// <param name="_bIsMaster">是否为房主</param>
    public void SetIsMaster(bool _bIsLocal, bool _bIsMaster)
    {
        if (Value.IsValueValid(vPlayerFunctionFrame))
        {
            if (vPlayerFunctionFrame.Invoke("SetIsMaster", _bIsMaster) == null)
            {
                Trace.LogError("call SetIsMaster failed.");
                return;
            }
        }
        if (vRedTeamFrame.Invoke("SetMachineAddButtonShow", _bIsMaster) == null)
        {
            Trace.LogError("call vRedTeamFrame.SetMachineAddButtonShow failed");
            return;
        }
        if (vBlueTeamFrame.Invoke("SetMachineAddButtonShow", _bIsMaster) == null)
        {
            Trace.LogError("call vBlueTeamFrame.SetMachineAddButtonShow failed");
            return;
        }
     
        HideAllIcon();
    }
    public void SetRoleSlot(cmd_match_room_role_data _data)
    {
        Value tmpTeamFrame = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);

        if (!Value.IsValueValid(tmpTeamFrame))
            return;
        /*
			_slotIndex:在TeamFrame里的序号
			_slotID:为所有RoleSlot里的序号
			_slotType:0：非本地玩家；1：本地玩家
		*/
        bool bIsComputerPlayer = _data.bIsComputerPlayer == 0 ? false : true;
        int nHeroId = _data.bIsComputerPlayer == 0 ? 0 : _data.nHeroID;
        bool bIsMaster = _data.bIsMaster == 0 ? false:true;
        if (tmpTeamFrame.Invoke("SetRoleSlotInfo", 0, bIsMaster, _data.nPlayerSlotID, nHeroId, _data.szPlayerName, RoleSlotStateReady, bIsComputerPlayer, bIsLocalMaster) == null)
        {
            Trace.LogError("call SetRoleSlotInfo faild");
            return;
        }


    }
    public void SetOwnerRoleSlot(cmd_match_room_role_data _data)
    {
        // 设置所有可选英雄为可用
        SetAllHeroCanSelect();

        Value tmpTeamFrame = null;
        ResetPlayerFunction();
        ChangeStateOfSkinViewAndHeroFrame(false);
        SetIsMaster(true, Convert.ToBoolean(_data.bIsMaster));

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);
        if (!Value.IsValueValid(tmpTeamFrame))
            return;
        /*
			_slotIndex:在TeamFrame里的序号
			_slotID:为所有RoleSlot里的序号			
         * _slotType:0：非本地玩家；1：本地玩家
		*/
        bool bIsMaster = _data.bIsMaster == 0 ? false : true;
        bIsLocalMaster = IsLocalMaster(1, bIsMaster);
        if (tmpTeamFrame.Invoke("SetRoleSlotInfo", 1, bIsMaster, _data.nPlayerSlotID, 0, _data.szPlayerName, "准备中", false, bIsLocalMaster) == null)
        {
            Trace.LogError("call SetRoleSlotInfo faild");
        }
    }
    // 重置10个位置不为高亮
    public void setAllPosNotBright()
    {
        if (!Value.IsValueValid(vRedTeamFrame) || !Value.IsValueValid(vBlueTeamFrame))
            return;
        /*
			_slotIndex:在TeamFrame里的序号
			_slotID:为所有RoleSlot里的序号
			_slotType:0：非本地玩家；1：本地玩家
		*/
        if (vRedTeamFrame.Invoke("setAllPosNotBright") == null || vBlueTeamFrame.Invoke("setAllPosNotBright") == null)
        {
            Trace.LogError("call setAllPosNotBright faild");
            return;
        }
    }

    // 设置某个位置为高亮
    public void SetChooseDisablePos(int nCamp, int nPos)
    {
        Value tmpTeamFrame = null;

        GetTeamFrame(nCamp, out tmpTeamFrame);

        if (!Value.IsValueValid(tmpTeamFrame))
            return;
        /*
			_slotIndex:在TeamFrame里的序号
			_slotID:为所有RoleSlot里的序号
			_slotType:0：非本地玩家；1：本地玩家
		*/
        if (tmpTeamFrame.Invoke("SetChooseDisablePos", nPos) == null)
        {
            Trace.LogError("call SetChooseDisablePos faild");
            return;
        }
    }

    // 设置某个英雄是否被禁用 bIsAble == true 标识禁用  bIsAble == false 表示不被禁用
    public void setOneHeroIsDisable(int nHero, bool bIsAble)
    {

        if (Value.IsValueValid(vSelectHeroFrame))
        {
            if (vSelectHeroFrame.Invoke("setOneHeroIsDisable", nHero, bIsAble) == null)
            {
                Trace.LogError("call setOneHeroIsDisable failed.");
                return;
            }
        }

    }
    public void setOneCampComputerIsDisable(int nHeroIconId, bool bIsLock, int nCamp)
    {
        Value tmpFromTeamFrame = null;
        //缓存原位置的信息 
        //拿到队伍
        GetTeamFrame(nCamp, out tmpFromTeamFrame);

        if (!dicDropHeroIdWithIndex.ContainsKey(nHeroIconId))
        {
            return;
        }
        if (Value.IsValueValid(tmpFromTeamFrame))
        {
            if (tmpFromTeamFrame.Invoke("SetCampComputerCanUse", dicDropHeroIdWithIndex[nHeroIconId], bIsLock) == null)
            {
                Trace.LogError("call SetCampComputerCanUse failed.");
                return;
            }
        }

    }


    public void GetTeamFrame(int camp, out Value teamValue)
    {
        if (!Value.IsValueValid(vRedTeamFrame) || !Value.IsValueValid(vBlueTeamFrame))
        {
            teamValue = null;
            return;
        }
        teamValue = camp == 0 ? vRedTeamFrame : vBlueTeamFrame;

    }

    public void ShowSelectHeroTime(int _curTime)
    {
        if (!Value.IsValueValid(vSelectHeroTimeFrame))
            return;
        if (vSelectHeroTimeFrame.Invoke("ShowSelectHeroTime", _curTime) == null)
        {
            Trace.LogError("call ShowSelectHeroTime faild");
            return;
        }
        CanSelectHero = true;

    }

    public void ShowSelectDisableHeroTime(int nCurTime, int nCamp, int enPhase)
    {

        if (!Value.IsValueValid(vSelectDisableHeroTImeFrame))
            return;
        if (vSelectDisableHeroTImeFrame.Invoke("ShowSelectDisableHeroTime", nCurTime * 1000, nCamp, enPhase) == null)
        {
            Trace.LogError("call ShowSelectDisableHeroTime failed");
            return;
        }

    }

    // 设置禁用英雄的所有信息
    public void HideAllIcon()
    {
        if (!Value.IsValueValid(vSelectDisableHeroTImeFrame))
            return;
        if (vSelectDisableHeroTImeFrame.Invoke("HideAllIcon") == null)
        {
            Trace.LogError("call HideAllIcon failed");
            return;
        }


    }


    public void HideSelectHeroTime()
    {
        if (!Value.IsValueValid(vSelectHeroTimeFrame))
            return;
        if (vSelectHeroTimeFrame.Invoke("HideSelectHeroTime") == null)
        {
            Trace.LogError("call HideSelectHeroTime faild");
            return;
        }

        CanSelectHero = false;

        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_END_SELECT_HERO);
    }

    public void CreateGameModeLayout(cmd_match_hall_add_room _data)
    {
        nConfigWarID = _data.nConfigWarID;
    }

    public void ChangeRoleSlot(cmd_match_room_hero_pos_change _data, int _isSelf)
    {
        Value tmpFromTeamFrame = null;
        Value tmpFromtRoleSlot = null;
        Value tmpFromRoleSlotInfo = null;

        Value tmpToTeamFrame = null;
        //缓存原位置的信息 
        //拿到队伍
        GetTeamFrame(_data.nCampFrom, out tmpFromTeamFrame);
        if (!Value.IsValueValid(tmpFromTeamFrame))
            return;
        //拿到RoleSlot
        tmpFromtRoleSlot = tmpFromTeamFrame.Invoke("GetRoleSlot", _data.nPlayerSlotIDFrom);
        if (!Value.IsValueValid(tmpFromtRoleSlot))
            return;

        //拿RoleSlot要显示的信息

        tmpFromRoleSlotInfo = tmpFromtRoleSlot.Invoke("GetRoleSlotViewInfo");
        if (!Value.IsValueValid(tmpFromRoleSlotInfo))
            return;

        //原位置缓存的信息丢给新位置
        GetTeamFrame(_data.nCampTo, out tmpToTeamFrame);
        if (!Value.IsValueValid(tmpToTeamFrame))
            return;
        bool bIsMaster = (bool)tmpFromRoleSlotInfo.GetElement(0);
        int nHeroID = (int)tmpFromRoleSlotInfo.GetElement(1);
        string strPlayerName = (string)tmpFromRoleSlotInfo.GetElement(2);
        string strPlayerState = (string)tmpFromRoleSlotInfo.GetElement(3);
        //自己切换位置，那就是本地玩家了
        //这里有bug，本地非本地都走这里改位置的
        if (tmpToTeamFrame.Invoke("SetRoleSlotInfo", _isSelf, bIsMaster, _data.nPlayerSlotIDTo, nHeroID
                , strPlayerName, strPlayerState, false, bIsLocalMaster) == null)
        {
            Trace.LogError("call SetRoleSlotInfo faild");
            return;
        }
        //并把原位置设空
        if (tmpFromtRoleSlot.Invoke("ResetViewInfoEmpty") == null)
        {
            Trace.LogError("call ResetViewInfoEmpty Failed");
            return;
        }
    }

    public void SetRoleSlotEmpty(cmd_match_room_hero_exit _data)
    {
        Value tmpTeamFrame = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);

        if (Value.IsValueValid(tmpTeamFrame))
        {
            if (tmpTeamFrame.Invoke("ResetRoleSlot", _data.nPlayerSlotID) == null)
            {
                Trace.LogError("call ResetRoleSlot faild");
            }
        }
    }

    public void RoomMasterChange(cmd_match_room_master_change _data)
    {
        /* 
         * 房主权力目前指，是否带“开始游戏”按钮
         */

        //房主主动离开，转移房主权力
        //清空旧房主位置信息
        //房主权力丢给新房主
        //新房主是否是本地玩家
        //房主主动转移权力，没有离开房间
        //房主权力丢给新房主
        //新房主是否是本地玩家
        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;
        Value tmpRoleSlotInfoArr = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);
        if (!Value.IsValueValid(tmpTeamFrame))
            return;
        //获取新位置信息
        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", _data.nPlayerSlotID);
        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;

        tmpRoleSlotInfoArr = tmpOldRoleSlot.Invoke("GetRoleSlotViewInfo");
        if (!Value.IsValueValid(tmpRoleSlotInfoArr))
            return;

        int nHeroID = (int)tmpRoleSlotInfoArr.GetElement(1);
        string strPlayerName = (string)tmpRoleSlotInfoArr.GetElement(2);
        string strPlayerState = (string)tmpRoleSlotInfoArr.GetElement(3);

        if (_data.nSlef == 1)
            SetIsMaster(Convert.ToBoolean(_data.nSlef), true);

        this.bIsLocalMaster = IsLocalMaster(_data.nSlef,true);
        //再重新组合赋值
        if (tmpTeamFrame.Invoke("SetRoleSlotInfo", _data.nSlef, true, _data.nPlayerSlotID, nHeroID, strPlayerName, strPlayerState, false,bIsLocalMaster) == null)
        {
            Trace.LogError("call SetRoleSlotInfo faild");
        }
    }

    public void ChangeRoleSlotHero(cmd_match_room_hero_change _data)
    {

        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);
        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        //获取新位置信息
        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", _data.nPlayerSlotID);

        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;

        if (tmpOldRoleSlot.Invoke("SetRoleSlotHero", _data.nHeroID) == null)
        {
            Trace.LogError("call SetRoleSlotHero failed");
            return;
        }
    }

    public void SetHeroHeadSlotInfo(IntPtr nHeroHeadSlotInfo, int nPtrLen)
    {
        if (!Value.IsValueValid(vSelectHeroFrame))
            return;
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(nHeroHeadSlotInfo, nPtrLen);
        cmd_match_room_hero_count countData = helper.get<cmd_match_room_hero_count>();
        for (int i = 0; i < countData.nHeroCount; ++i)
        {
            cmd_match_room_hero_slot data = helper.get<cmd_match_room_hero_slot>();

            bool bIsLock = Convert.ToBoolean(data.bIsLock);
            bool bIsHave = Convert.ToBoolean(data.bCurrentIsHave);

            if (vSelectHeroFrame.Invoke("SetHeroPortraitSlotInfo", data.nSlotIndex, data.nHeroID, !bIsLock, bIsHave, data.szHeroTooltip) == null)
            {
                Trace.LogError("call SetHeroPortraitSlotInfo failded");
                return;
            }
        }
    }

    public void SetHeroConfirmBtnVisible()
    {
        if (Value.IsValueValid(vPlayerFunctionFrame))
        {
            if (vPlayerFunctionFrame.Invoke("SetSelectHeroComfirm") == null)
            {
                Trace.LogError("call SetSelectHeroComfirm failed.");
                return;
            }
        }
    }

    public void SetRoleSlotHeroConfirm(cmd_match_room_confirm_hero _data)
    {
        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);
        //获取新位置信息
        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", _data.nPlayerSlotID);

        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;

        if (tmpOldRoleSlot.Invoke("SetRoleSlotPlayerState", RoleSlotStateConfirm) == null)
        {
            Trace.LogError("call SetRoleSlotPlayerState failed");
            return;
        }
    }

    public void ShowEnterWarTime(int _curTime)
    {
        if (!Value.IsValueValid(vSelectHeroTimeFrame))
            return;

        if (vSelectHeroTimeFrame.Invoke("ShowEnterWarTime", _curTime) == null)
        {
            Trace.LogError("call ShowEnterWarTime faild");
            return;
        }

        HideAllIcon();
    }

    public void SetWaitingMapInfoFrame(string _mapName, string _roomDes)
    {
        if (!Value.IsValueValid(vMapInfoFrame))
            return;

        if (vMapInfoFrame.Invoke("SetWaitingMapInfoFrame", _mapName, _roomDes) == null)
        {
            Trace.LogError("error call SetWaitingMapInfoFrame failed!");
            return;
        }
    }

    public void ClearSelectHeroFrameAllProtraitSlot()
    {
        if (Value.IsValueValid(vSelectHeroFrame))
        {
            if (vSelectHeroFrame.Invoke("ClearSelectHeroFrameAllProtraitSlot") == null)
            {
                Trace.LogError("call ClearSelectHeroFrameAllProtraitSlot failed");
                return;
            }
        }
    }

    public void ResetWaitingRoomWin()
    {
        if (Value.IsValueValid(vRedTeamFrame))
        {
            if (vRedTeamFrame.Invoke("ResetAllRoleSlot") == null)
            {
                Trace.LogError("call ResetAllRoleSlot failed");
            }
        }
        if (Value.IsValueValid(vBlueTeamFrame))
        {
            if (vBlueTeamFrame.Invoke("ResetAllRoleSlot") == null)
            {
                Trace.LogError("call ResetAllRoleSlot failed");
            }
        }
        if (Value.IsValueValid(vHeroFunctionFrame))
        {
            if (vHeroFunctionFrame.Invoke("ResetSumonerFunctionFrame") == null)
            {
                Trace.LogError("call ResetSumonerFunctionFrame failed");
            }
        }
        HideSelectHeroTime();
    }

    // 设置准备按钮
    public void SetReadayButton(bool isEnabled)
    {
        if (Value.IsValueValid(vPlayerFunctionFrame))
        {
            if (vPlayerFunctionFrame.Invoke("SetReadayButton", isEnabled) == null)
            {
                Trace.LogError("call SetReadyButton failed");
            }
        }
    }

    // 选英雄时如果没有选英雄置灰
    public void setStartChooseHeroIconVisible(bool bIsHide)
    {
        if (Value.IsValueValid(vPlayerFunctionFrame))
        {
            if (vPlayerFunctionFrame.Invoke("setStartChooseHeroIconVisible", bIsHide) == null)
            {
                Trace.LogError("call setStartChooseHeroIconVisible failed");
            }
        }
        if (Value.IsValueValid(vRedTeamFrame))
        {
            // 房主开始后隐藏机器人
            if (vRedTeamFrame.Invoke("SetHideMachine") == null)
            {
                Trace.LogError("call vRedTeamFrame.SetHideMachine failed");
                return;
            }
        }
        if (Value.IsValueValid(vBlueTeamFrame))
        {
            // 房主开始后隐藏机器人
            if (vBlueTeamFrame.Invoke("SetHideMachine") == null)
            {
                Trace.LogError("call vBlueTeamFrame.SetHideMachine failed");
                return;
            }
        }
    }

    /// <summary>
    /// 设置返回按钮是否可用
    /// </summary>
    public void SetReturnBtnEnable(bool _bEnable)
    {
        if (Value.IsValueValid(vBottomFrame))
        {
            if (vBottomFrame.Invoke("SetReturnBtnEnable", _bEnable) == null)
            {
                Trace.LogError("call SetReturnBtnEnable failed.");
                return;
            }
        }
    }

    public void SetSelectHeroListLock(bool _bLock)
    {
        if (Value.IsValueValid(vSelectHeroFrame))
        {
            if (vSelectHeroFrame.Invoke("SetSelectHeroListLock", _bLock) == null)
            {
                Trace.LogError("call SetSelectHeroListLock failed.");
                return;
            }
        }
    }

    public void SetAllHeroCanSelect()
    {
        if (Value.IsValueValid(vSelectHeroFrame))
        {
            if (vSelectHeroFrame.Invoke("SetAllHeroCanSelect") == null)
            {
                Trace.LogError("call SetAllHeroCanSelect failed");
                return;
            }
        }
    }
    //  设置默认天赋技能信息.调用as，设置上次记录的两个基本召唤师技能(参数：1、2基本槽位，图片id，召唤师技能槽位对应着召唤师天赋id)
    public void setDefSummonerSkill(IntPtr ptrParam, int nPtrLen)
    {

        if (!Value.IsValueValid(vHeroFunctionFrame))
        {
            return;
        }
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count cmdCount = helper.get<cmd_count>();
        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            cmd_SummonerSkill_Info cmdInfo = helper.get<cmd_SummonerSkill_Info>();
            if (vHeroFunctionFrame.Invoke("SetBaseSummonerButtonIcon", i + 1, cmdInfo.nIconId, cmdInfo.nSkillId, cmdInfo.summonerSkillDesc) == null)
            {
                Trace.LogError("call SetBaseSummonerButtonIcon failed");
                continue;
            }
        }
    }
    // 可以在准备按钮点击后调用该函数，调用as，记录本次选择的召唤师技能槽位，后续需要再写发送逻辑层记录1、2槽位的召唤师技能槽位
    public int GetBaseButtonTalentIndex(int slotIndex)
    {
        if (Value.IsValueValid(vHeroFunctionFrame))
        {
            if (vHeroFunctionFrame.Invoke("GetBaseButtonTalentIndex", slotIndex) == null)
            {
                Trace.LogError("call GetBaseButtonTalentIndex failed");
                return 0;
            }
            int TalentIndex = 0;
            return TalentIndex = vHeroFunctionFrame.Invoke("GetBaseButtonTalentIndex", slotIndex);
        }
        return 0;
    }
    // 获得所有召唤师技能信息，调用as，设置12个召唤师技能的点亮使能和图片id
    public void setAllSummonerSkill()
    {
        int nCount = SmAllSkillInfoList.Count;
        for (int i = 0; i < nCount; ++i)
        {
            cmd_SmAllSkillInfo cmdInfo = SmAllSkillInfoList[i];
            bool bIsLock = cmdInfo.nIsUnlock == 1 ? true : false;
            if (Value.IsValueValid(vPopUpWindow))
            {
                if (vPopUpWindow.Invoke("SetChooseSummonerBtnClick", bIsLock, cmdInfo.nIconId, cmdInfo.summonerSkillDesc, cmdInfo.nId) == null)
                {
                    Trace.LogError("call SetChooseSummonerBtnClick failed");
                    return;
                }
            }
        }
    }

    public void SetSkillText(IntPtr nSkillTextInfo, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(nSkillTextInfo, nPtrLen);
        cmd_match_room_hero_skill_desc_count SkillDescCountdata = helper.get<cmd_match_room_hero_skill_desc_count>();
        for (int i = 0; i < SkillDescCountdata.nSkillDescCount; ++i)
        {
            cmd_match_room_hero_skill_desc data = helper.get<cmd_match_room_hero_skill_desc>();
            if (Value.IsValueValid(vSkillFrame))
            {
                // 名字或者技能描述为空返回，并放出错误信息
                if (data.nSpellIconID == 0 || data.heroSkillDes == null)
                {
                    Trace.LogWarning("SetSkillText Warning:Skill Desc==null");
                    continue;
                }
                //解析UBB
                UBB.toHtml(ref data.heroSkillDes);
                if (bIsSelectOrSkinState)
                {
                    if (vSkillFrame.Invoke("SetText", data.nSlotID, data.heroSkillDes, data.nSpellIconID) == null)
                    {
                        Trace.LogError("call SetSkillText failed");
                        return;
                    }
                }
                else
                {
                    if (vSkinSkillFrame.Invoke("SetText", data.nSlotID, data.heroSkillDes, data.nSpellIconID) == null)
                    {
                        Trace.LogError("call SetSkillText failed");
                        return;
                    }
                }
            }
        }
    }

    public void ChangeStateOfSkinViewAndHeroFrame(bool bIsSHowSkin)
    {
        if (vSkinFrame.Invoke("SetSkinVisible", bIsSHowSkin) == null)
        {
            Trace.LogError("call ChangeStateOfSkinViewAndHeroFrame failed");
            return;
        }
        if (vSelectHeroFrame.Invoke("SetSelectHeroVisible", !bIsSHowSkin) == null)
        {
            Trace.LogError("call ChangeStateOfSkinViewAndHeroFrame failed");
            return;
        }
        bIsSelectOrSkinState = !bIsSHowSkin;
    }
    public void SetSkinIcon()
    {
        if (vSkinFrame.Invoke("SetSkinIcon", HeroIconID) == null)
        {
            Trace.LogError("call SetSkinIcon failed");
            return;
        }
    }
    public void SetSkinVisible(bool bIsShow)
    {
        if (vSkinSkillFrame.Invoke("SetSkinVisible", bIsShow) == null)
        {
            Trace.LogError("call SetSkinVisible failed");
            return;
        }
    }

    public void SetSkillDesVisible(bool bVisible)
    {
        if (Value.IsValueValid(vSkillFrame))
        {
            if (vSkillFrame.Invoke("SetOutFrameVisible", bVisible) == null)
            {
                Trace.LogError("call onHeroOutArea failed");
                return;
            }
        }
    }

    public void showSummonerSkillInRoom(int nCamp, int nPos, int nSkill1, int nSkill2)
    {

        if (nPos >= 5 && nPos < 10)
        {
            nPos -= 5;
        }

        Value tmpTeamFrame = null;

        GetTeamFrame(nCamp, out tmpTeamFrame);

        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        if (tmpTeamFrame.Invoke("SetRoleSummonerInfo", nPos, nSkill1, nSkill2) == null)
        {
            Trace.LogError("call SetRoleSummonerInfo faild");
            return;
        }
    }


    // 当刚登陆进来时设置左右两边的召唤师技能信息
    public void SetSummonerPicLogin(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_war_personInfo_count obj = helper.get<cmd_war_personInfo_count>();
        for (int i = 0; i < obj.nCount; ++i)
        {
            cmd_Summoner_Skill_Change data = helper.get<cmd_Summoner_Skill_Change>();

            if (data.nPos >= 10)
            {
                continue;
            }
            showSummonerSkillInRoom(data.nCamp, data.nPos, data.nSummonerSkill[0], data.nSummonerSkill[1]);
        }
    }
    // 给单个机器发送列表数据（机器人描述和机器人图片id），目前只自己定义了描述
    public void SetMachineInfo(int nCamp, int nSlotID)
    {

        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;

        GetTeamFrame(nCamp, out tmpTeamFrame);
        //获取新位置信息
        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", nSlotID);

        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;
        // 将配置文件中获取的用户名集合以数组的形式传递给AS
        Value vAllMachineArray = this.CreateArray();

        //用户名数组下表,用于传递给AS
        uint nMachineNum = (uint)dDropMachineInfo.Count;
        vAllMachineArray.SetArraySize(nMachineNum);

        for (uint i = 0; i < nMachineNum; i++)
        {
            // 保证传递给AS后，下拉菜单中选项以最近登陆使用的顺序排列用户名
            Value ASMachineValue;
            CreateASMachineInfo(dDropMachineInfo[(int)i], out ASMachineValue);
            vAllMachineArray.SetElement(i, ASMachineValue);
        }
        if (tmpOldRoleSlot.Invoke("SetDropDownMachineName", vAllMachineArray) == null)
        {
            Trace.LogError("call SetDropDownMachineName failed");
            return;
        }

    }

    // 进入天赋状态,发送数据到房间
    public void SetTalentDropInfo()
    {
        Value vTalentDropArray = this.CreateArray();
        vTalentDropArray.SetArraySize((uint)LogicDataCenter.talentPageDataManager.GetMaxSortIndex());
        uint nIndex = 0;
        foreach ( TalentPageDataItem pageItem in LogicDataCenter.talentPageDataManager.talentPageDataManager)
        {
            Value ASTalentDropValue;
            CreateASTalentDropInfo(pageItem, out ASTalentDropValue);
            vTalentDropArray.SetElement(nIndex, ASTalentDropValue);
            nIndex++;
        }
        if (!Value.IsValueValid(vHeroFunctionFrame))
            return;

        if (vHeroFunctionFrame.Invoke("SetTalentDropDownUserName", vTalentDropArray, LogicDataCenter.talentPageDataManager.nCurPageSortIndex) == null)
        {
            //Trace.LogError("call SetTalentDropDownUserName failed");
            return;
        }
    }

    // 进入符文状态,发送符文数据到房间
    public void SetRuneDropInfo()
    {
        Value vRuneDropArray = this.CreateArray();
        vRuneDropArray.SetArraySize((uint)LogicDataCenter.runePageDataManager.GetMaxSortIndex());
        uint nIndex = 0;
        foreach (RunePageDataItem pageItem in LogicDataCenter.runePageDataManager.runePageDataManager)
        {
            Value ASRuneDropValue;
            CreateASRuneDropInfo(pageItem, out ASRuneDropValue);
            vRuneDropArray.SetElement(nIndex, ASRuneDropValue);
            nIndex++;
        }
        if (!Value.IsValueValid(vHeroFunctionFrame))
            return;

        if (vHeroFunctionFrame.Invoke("SetRuneDropDownUserName", vRuneDropArray, LogicDataCenter.runePageDataManager.nCurPageSortIndex) == null)
        {
            //Trace.LogError("call SetRuneDropDownUserName failed");
            return;
        }
    }

    // 禁止选英雄的个数
    public void MatchRoomRuleSetComputerLock(cmd_match_room_computer_hero_can_select data)
    {
        bool bCanLock = data.bCanSelect == 0 ? true : false;

        setOneCampComputerIsDisable(data.nHeroID, bCanLock, data.nCamp);

    }


    // 电脑改变的数据
    public void ChangeRoleSlotComputer(cmd_match_room_computer_hero_change _data)
    {

        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;

        GetTeamFrame(_data.nCamp, out tmpTeamFrame);
        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        //获取新位置信息
        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", _data.nPlayerSlotID);

        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;

        if (!dicDropHeroIdWithIndex.ContainsKey(_data.nHeroID))
        {
            if (tmpOldRoleSlot.Invoke("SetRoleSlotMachine", -1, true) == null)
            {
                Trace.LogError("call SetRoleSlotHero failed");
            }
            return;
        }

        int nMachineIndex = dicDropHeroIdWithIndex[_data.nHeroID];
        if (tmpOldRoleSlot.Invoke("SetRoleSlotMachine", nMachineIndex, false) == null)
        {
            Trace.LogError("call SetRoleSlotHero failed");
            return;
        }
        // 给as发送假人的AI配置相关，假人名字这些数据还没接受
        SetHeroAIDropInfo(_data.nHeroID, tmpOldRoleSlot);
        
    }

    // 给单个机器发送列表数据（机器人描述和机器人图片id），目前只自己定义了描述
    public void SetHeroAIInfo(int nCamp, int nSlotID, int nHeroID)
    {
        if (nHeroID <= (int)VOCATION.VOCATION_INVALID)
        {
            return;
        }
        Value tmpTeamFrame = null;
        Value tmpOldRoleSlot = null;

        GetTeamFrame(nCamp, out tmpTeamFrame);
        //获取新位置信息
        if (!Value.IsValueValid(tmpTeamFrame))
            return;

        tmpOldRoleSlot = tmpTeamFrame.Invoke("GetRoleSlot", nSlotID);

        if (!Value.IsValueValid(tmpOldRoleSlot))
            return;


        SetHeroAIDropInfo(nHeroID, tmpOldRoleSlot);

    }

    public void SetHeroAIDropInfo(int nHeroID, Value RoleSlot)
    {

        // 将配置文件中获取的用户名集合以数组的形式传递给AS
        Value vAllHeroAIArray = this.CreateArray();

        // 查找英雄AI配置
        SSchemeHeroAIConfig SchmeInfo = null;
        HeroAIConfig.Instance.GetHeroAIInfo(nConfigWarID, nHeroID, ref SchmeInfo);

        //用户名数组下表,用于传递给AS 
        if (SchmeInfo == null)
        {
            return;
        }

        string szAIDescList = SchmeInfo.szAIDESArray;
        string[] AIDescArrayStr = szAIDescList.Split(';');
        int nAIDescArrayLength = AIDescArrayStr.Length;

        string szAIIDList = SchmeInfo.szAIIDArray;
        string[] AIIDArrayStr = szAIIDList.Split(';');
        int nAIIDArrayLength = AIIDArrayStr.Length;

        if (nAIDescArrayLength != nAIIDArrayLength)
        {
            Trace.LogError("配置出错 请检查 HeroAIConfig.csv heroID = " + nHeroID);
        }

        uint nHeroAINum = (uint)nAIDescArrayLength;
        vAllHeroAIArray.SetArraySize(nHeroAINum);

        for (uint i = 0; i < nHeroAINum; i++)
        {
            // 保证传递给AS后，下拉菜单中选项以最近登陆使用的顺序排列用户名
            Value ASHeroAIValue;
            HeroAIInfo tHeroAIInfo = new HeroAIInfo();
            tHeroAIInfo.nHeroID = SchmeInfo.nHeroId;
            tHeroAIInfo.szAIDesc = AIDescArrayStr[i];
            tHeroAIInfo.nAIID = Convert.ToInt32(AIIDArrayStr[i]);

            CreateASHeroAIInfo(tHeroAIInfo, out ASHeroAIValue);
            vAllHeroAIArray.SetElement(i, ASHeroAIValue);
        }

        if (RoleSlot.Invoke("SetDropDownHeroAIName", vAllHeroAIArray, SchmeInfo.bIsOpen) == null)
        {
            Trace.LogError("call SetDropDownHeroName failed");
            return;
        }

    }

    public void CreateASHeroAIInfo(HeroAIInfo HeroAIInfo, out Value ASHeroAIValue)
    {
        ASObjectEx ASMachineEx;
        ASMachineEx.names = new string[] { "szAIDesc", "nHeroID", "nAIID" };
        ASMachineEx.values = new object[] { HeroAIInfo.szAIDesc, HeroAIInfo.nHeroID, HeroAIInfo.nAIID };
        ASMachineEx.types = new Type[] { typeof(string), typeof(int), typeof(int) };
        ASHeroAIValue = Value.ConvertToASObjectEx(ASMachineEx, this, "Com.Scaleform.Common.HeroAIInfo");
    }


    public void CreateASMachineInfo(MachineInfo machineInfo, out Value ASMachineValue)
    {
        ASObjectEx ASMachineEx;
        ASMachineEx.names = new string[] { "MachineName", "MachineIconID", "bIsLock" };
        ASMachineEx.values = new object[] { machineInfo.stDesc, machineInfo.nIconId, machineInfo.bLock };
        ASMachineEx.types = new Type[] { typeof(string), typeof(int), typeof(bool) };
        ASMachineValue = Value.ConvertToASObjectEx(ASMachineEx, this, "Com.Scaleform.Common.MachineInfo");
    }

    public void CreateASTalentDropInfo(TalentPageDataItem pageItem, out Value ASTalentDropValue)
    {
        ASObjectEx ASTalentDropEx;
        ASTalentDropEx.names = new string[] { "TalentName", "AttackPoints", "TrickPoints", "HelpPoints" };
        ASTalentDropEx.values = new object[] { pageItem.sTempName, pageItem.nAttackPoints, pageItem.nTrickPoints, pageItem.nHelperPoints };
        ASTalentDropEx.types = new Type[] { typeof(string), typeof(int), typeof(int), typeof(int) };
        ASTalentDropValue = Value.ConvertToASObjectEx(ASTalentDropEx, this, "Com.Scaleform.Common.TalentDropInfo");
    }

    public void CreateASRuneDropInfo(RunePageDataItem pageItem, out Value ASRuneDropValue)
    {
        ASObjectEx ASRuneDropEx;
        ASRuneDropEx.names = new string[] { "RuneName" };
        ASRuneDropEx.values = new object[] { pageItem.sTempName };
        ASRuneDropEx.types = new Type[] { typeof(string) };
        ASRuneDropValue = Value.ConvertToASObjectEx(ASRuneDropEx, this, "Com.Scaleform.Common.RuneDropInfo");
    }

    public bool IsLocalMaster(int bLocal,bool bMaster)
    {
        if(bLocal!=0&&bMaster)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
	
    #endregion
}
