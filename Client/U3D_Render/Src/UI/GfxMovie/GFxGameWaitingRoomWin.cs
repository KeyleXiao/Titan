/*******************************************************************
** 文件名:	GFxGameWaitingRoomWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/03/19
** 版  本:	1.0
** 描  述:	GFx 玩家等待开始游戏界面
				
** 应  用:  	对应Flash Layout文件：GameWaitingRoomWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 吴欣伟
** 日  期: 2015.9.28
** 描  述: 
 *         1、拆分C#中与AS通讯部分逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;
using Stage_ManagerDef;
using System.Collections.Generic;
using USpeedUI;
using ASpeedGame.Data.HeroAIConfig;
using DataCenter;
public partial class GFxGameWaitingRoomWin : SFMovieEx
{
    /// <summary>
    /// 是否可以选择英雄
    /// </summary>
    public bool CanSelectHero = false;
    public bool CanChooseDisableHero = false;       // false 为不可以选择禁用英雄  true为可以选择禁用英雄
    private string RoleSlotStateReady = "准备中";
    private string RoleSlotStateConfirm = "已准备";
    private EMatchRoomState _eMatchRoomState;
    public int HeroIconID = 0;
    public bool bIsSelectOrSkinState = true;    // true为选人状态，false为人物皮肤状态
    // 储存当前所有的的召唤师技能id
    public List<cmd_SmAllSkillInfo> SmAllSkillInfoList = new List<cmd_SmAllSkillInfo>();
    public List<MachineInfo> dDropMachineInfo = new List<MachineInfo>();
    public List<SSchemeHeroAIConfig> dDropHeroAIInfo = new List<SSchemeHeroAIConfig>();
    public Dictionary<int, int> dicDropHeroIdWithIndex = new Dictionary<int, int>();
    public GFxTalentMenu talentMenu;
    public int nConfigWarID = 0;
    public bool bIsLocalMaster = false;
    public enum SelectPhase
    {
        // 选择禁用英雄
        ENUM_DISABLE = 1,
        // 选择英雄
        ENUM_SELECT,
        // 等待选择禁用英雄
        ENUM_WAIT_DISABLEHERO
    };

    public enum SummonerSkillSolt
    {
        // 天赋技能槽位1
        ENUM_SMSKILLSOLT1 = 1,
        // 天赋技能槽位2
        ENUM_SMSKILLSOLT2,

        ENUM_SMSKILLMAX
    };
    public class MachineInfo
    {
        // 机器名字描述
        public string stDesc;

        // 机器人图片id
        public int nIconId;
        // 机器人是否被禁
        public bool bLock;
    }

    public class HeroAIInfo
    {
        // 英雄ID
        public int nHeroID;

        // AIID
        public int nAIID;

        // AI名字描述
        public string szAIDesc;

    }

    #region GFx属性
    protected Value vSelectHeroFrame;
    protected Value vSelectHeroTimeFrame;
    protected Value vSelectDisableHeroTImeFrame;
    protected Value vMapInfoFrame;


    protected Value vRedTeamFrame;
    protected Value vBlueTeamFrame;
    protected Value vPlayerFunctionFrame;
    protected Value vHeroFunctionFrame;
    protected Value vBottomFrame;
    protected Value vSkillFrame;
    protected Value vPopUpWindow;
    protected Value vSkinFrame;
    protected Value vSkinSkillFrame;
    #endregion

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        bCanSyncTickCount = true;

    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "BottomFrame", out vBottomFrame);
        GetMemberWithCheck(MyRoot, "SelectHeroFrame", out vSelectHeroFrame);
        GetMemberWithCheck(MyRoot, "SelectHeroTimeFrame", out vSelectHeroTimeFrame);
        GetMemberWithCheck(MyRoot, "SelectDisableHeroTimeFrame", out vSelectDisableHeroTImeFrame);
        GetMemberWithCheck(MyRoot, "MapInfoFrame", out vMapInfoFrame);
        GetMemberWithCheck(MyRoot, "RedTeamFrame", out vRedTeamFrame);
        GetMemberWithCheck(MyRoot, "BlueTeamFrame", out vBlueTeamFrame);
        GetMemberWithCheck(MyRoot, "HeroFunctionFrame", out vHeroFunctionFrame);
        GetMemberWithCheck(MyRoot, "SkillFrame", out vSkillFrame);
        GetMemberWithCheck(MyRoot, "SkinMainFrame", out vSkinFrame);
        GetMemberWithCheck(vHeroFunctionFrame, "PopUpWindow", out vPopUpWindow);
        GetMemberWithCheck(vHeroFunctionFrame, "FunctionButtonFrameButton", out vPlayerFunctionFrame);
        GetMemberWithCheck(vSkinFrame, "SkinSkillFrame", out vSkinSkillFrame);
        SetMovieVisibleOnCreate();


    }

    #region AS->C#
    //房主开始游戏按钮点击回调
    public void onBeginGameBtnClick()
    {
        //房主点开始游戏
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_START_GAME);

        setStartChooseHeroIconVisible(true);

        //         bool bIsLock = false;
        // 
        //         if (vSelectHeroFrame.Invoke("SetHeroLockOrUnlock", 2, bIsLock) == null)
        //         {
        //             Trace.LogError(" call SetHeroLockOrUnlock failed ");
        //             return;
        //         }
        //获取房间属性
        // GFxGameCreateingRoomWin.RoomProterty tmpRoomProterty = (SFGFxMovieManager.GetWindow<("GFxGameCreateingRoomWin") as GFxGameCreateingRoomWin).GetRoomProterty;

        //房间内所有玩家准备后，进入战场场景
        //添加一个场景资源载入消息
        //SceneManager.Instance.ProgressEvent += LoadSceneOver;
        //SceneManager.Instance.LoadScene(tmpRoomProterty.MapName);

    }
    //选英雄确定按钮点击回调
    public void onReadyGameBtnClick()
    {
        sendSummonerSkillInfo();

        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_CONFIRM_HERO);
        SetReadayButton(false);
        ChangeStateOfSkinViewAndHeroFrame(true);
        SetSkinIcon();
        RequestSkillInfo(HeroIconID);
        SetCurPageIndexofStartGame();
    }

    public void SetCurPageIndexofStartGame()
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE, null);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, null);
    }
    // 将确认后的召唤师技能信息发送到逻辑增
    public void sendSummonerSkillInfo()
    {
        cmd_SmSillInfo_Return cmdInfo = new cmd_SmSillInfo_Return();
        cmdInfo.nAllSoltSkillId = new int[2];
        for (int i = 1; i < (int)(SummonerSkillSolt.ENUM_SMSKILLMAX); ++i)
        {
            int nIndex = i - 1;
            int nSkillId = GetBaseButtonTalentIndex(i);
            cmdInfo.nAllSoltSkillId[nIndex] = nSkillId;
        }
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_SmSillInfo_Return>(ref cmdInfo);
        int len = Marshal.SizeOf(cmdInfo);

        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SUMMONER_SKILL_INFO, len, "", ptr, len);

        OnSummonerEveryButtonClick(GetBaseButtonTalentIndex(1),0);
    }

    //点击新位置，丢给逻辑层查看是否可以转到新位置
    public void onRoleSlotMouseClick(int _slotCamp, int _slotID)
    {
        if (_eMatchRoomState >= EMatchRoomState.MatchRoomState_SelectHero)
            return;

        cmd_match_room_hero_pos_change data = new cmd_match_room_hero_pos_change();
        data.nCampTo = _slotCamp;
        data.nPlayerSlotIDTo = _slotID;

        ViewEventHelper.Instance.SendCommand<cmd_match_room_hero_pos_change>(GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE, ref data);
    }
    //离开房间
    public void onReturnBtnClick()
    {
        //通知逻辑层要离开房间
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
    }
    //点击英雄头像
    public void onHeroHeadClick(int _nHeroID)
    {
        //CanSelectHero:true
        //可以更换玩家英雄头像
        if (CanSelectHero)
        {
            if (CanChooseDisableHero == false)
            {
                setStartChooseHeroIconVisible(false);
            }
            else
            {

                if (_nHeroID == -1)
                {
                    return;
                }
                CanChooseDisableHero = false;
                SetSelectHeroListLock(true);
            }
            HeroIconID = _nHeroID;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_CHANGE, _nHeroID);
        }
    }


    //点击移除房间成员按钮
    public void onRemovePlayerBtnClick(int _slotCamp, int _slotID)
    {
        cmd_match_room_hero_exit data = new cmd_match_room_hero_exit();
        data.nCamp = _slotCamp;
        data.nPlayerSlotID = _slotID;

        ViewEventHelper.Instance.SendCommand<cmd_match_room_hero_exit>(GameLogicDef.GVIEWCM_MATCH_MASTER_KICK_ROLE, ref data);
    }
    public void onHeroInArea(int heroID)
    {
        SetSkillDesVisible(true);
        RequestSkillInfo(heroID);
    }
    public void onHeroOutArea()
    {
        SetSkillDesVisible(false);
    }
    //倒计时结束
    public void onSelectHeroTimerComplete()
    {
        //Trace.Log("onSelectHeroTimerComplete");
    }
    // 当点击按钮时
    public void OnSummonerButtonClick()
    {
        // 当点击英雄天赋按钮时像逻辑层请求所有天赋id
        setAllSummonerSkill();
    }
    // 当点击按钮时
    public void OnSummonerCloseButtonClick()
    {
        if (!bIsSelectOrSkinState)
        {
            SetSkinVisible(true);
        }

    }
    // 当召唤师技能改变之后回调
    public void OnSummonerEveryButtonClick(int nSkillFirst, int nSkillSecond)
    {
        cmd_SmSillInfo_Return cmdInfo = new cmd_SmSillInfo_Return();
        cmdInfo.nAllSoltSkillId = new int[2];

        cmdInfo.nAllSoltSkillId[0] = nSkillFirst;
        cmdInfo.nAllSoltSkillId[1] = nSkillSecond;
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_SmSillInfo_Return>(ref cmdInfo);
        int len = Marshal.SizeOf(cmdInfo);

        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SUMMONER_BROAD_INFO, len, "", ptr, len);
    }
    public void OnSummonerButtonClickEX()
    {
        if (!bIsSelectOrSkinState)
        {
            SetSkinVisible(false);
        }
        //   this.vPopUpWindowSetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer);
    }

    // 机器人部分////////////////////////////////////////////////////
    public void onAddMachineBtnClick(int nSlotCamp, int nSlotID)
    {
        // 给逻辑层发送添加假人信息，数据结构后续定义
        cmd_computerplayer_add data = new cmd_computerplayer_add();
        data.nPos = nSlotID;
        data.nCamp = nSlotCamp;
        ViewEventHelper.Instance.SendCommand<cmd_computerplayer_add>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_ADD, ref data);
    }

    public void onRemoveMachineBtnClick(int nSlotCamp, int nSlotID)
    {
        // 给逻辑层发送移除加假人信息，数据结构后续定义
        cmd_computerplayer_remove data = new cmd_computerplayer_remove();
        data.nPos = nSlotID;
        data.nCamp = nSlotCamp;
        ViewEventHelper.Instance.SendCommand<cmd_computerplayer_remove>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE, ref data);
    }

    // 给逻辑层发送选择假人信息，数据结构后续定义
    public void onChangeMachineBtnClick(int nSlotCamp, int nSlotID, int nDropIndex)// 下拉的位置
    {
        cmd_computerplayer_herochange data = new cmd_computerplayer_herochange();
        data.nPos = nSlotID;
        data.nCamp = nSlotCamp;
        data.nHeroID = dDropMachineInfo[nDropIndex].nIconId;
        ViewEventHelper.Instance.SendCommand<cmd_computerplayer_herochange>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE, ref data);
        
        //找到有配置就设置初始化下拉列表AI数据
        // 给as发送假人的AI配置相关，假人名字这些数据还没接受
        SetHeroAIInfo(nSlotCamp, nSlotID, data.nHeroID);
    }

    // 给逻辑层发送选择假人信息，数据结构后续定义
    public void onChangeHeroAIBtnClick(int nSlotCamp, int nSlotID,int nDropHeroIndex, int nDropAIID)// 下拉的位置
    {
        cmd_computerplayer_heroaichange data = new cmd_computerplayer_heroaichange();
        data.nPos = nSlotID;
        data.nCamp = nSlotCamp;
        data.nHeroID = dDropMachineInfo[nDropHeroIndex].nIconId;
        data.nAIID = nDropAIID;
        ViewEventHelper.Instance.SendCommand<cmd_computerplayer_heroaichange>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE, ref data);

    }

    public void onTalentOpenButtonClick()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_ROOMOPENTALENT,null);
    }

    public void onTalentCloseButtonClick()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);

        SetTalentDropInfo();
    }

    public void onTalentPageIndexChange(int nPageIndex)
    {
        LogicDataCenter.talentPageDataManager.nCurPageSortIndex = nPageIndex + 1;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, null);
    }

    public void onRunePageIndexChange(int nPageIndex)
    {
        LogicDataCenter.runePageDataManager.nCurPageSortIndex = nPageIndex + 1;
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE, null);
    }
    // 机器人部分////////////////////////////////////////////////////
    #endregion

    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(true);
        SetTalentDropInfo();
        SetRuneDropInfo();
        InitDropMachineInfo();
            
    }

    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);

        // 离开房间时初始化界面信息
        InitSelectHeroStatus();

    }

    public void SetMatchRoomState(int _newState)
    {
        _eMatchRoomState = (EMatchRoomState)_newState;
    }

    public void MatchStartRoomRule()
    {
        //不能点击准备按钮
        SetReadayButton(false);
        //不能点返回按钮
        //SetReturnBtnEnable(false);
        //不能选择英雄
        SetSelectHeroListLock(true);
    }


    //某个玩家进入禁止英雄状态
    public void MatchRoomRuleBanHero(IntPtr ptrParam, int nLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nLen);
        cmd_match_room_ban_hero_count obj = helper.get<cmd_match_room_ban_hero_count>();

        // 重置10个位置高亮的部分
        setAllPosNotBright();

        int nNeedTime = 0;
        int nCamp = 0;
        SelectPhase nSelectState = SelectPhase.ENUM_WAIT_DISABLEHERO;
        for (int i = 0; i < obj.nCount; ++i)
        {
            cmd_match_room_rule_ban_hero cmdData = helper.get<cmd_match_room_rule_ban_hero>();
            if (nNeedTime == 0) nNeedTime = cmdData.nTimeInterval;
            if (nCamp == 0) nCamp = cmdData.nCamp;
            if (cmdData.bIsSelf == 1)
            {
                CanSelectHero = true;
                CanChooseDisableHero = true;
                // 如果是轮到本地玩家去选择禁用英雄， 则解除这个锁
                SetSelectHeroListLock(false);

                nSelectState = SelectPhase.ENUM_DISABLE;
            }
            //高亮哪个位置可以禁用英雄
            SetChooseDisablePos(cmdData.nCamp, cmdData.nPos);
        }
        // 启用选择禁用英雄的倒计时
        ShowSelectDisableHeroTime(nNeedTime, nCamp, (int)nSelectState);
    }


    // 禁用英雄是选择英雄后会返回禁用英雄的信息
    public void MatchRoomRuleBanHeroList(IntPtr ptrParam, int nParam)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);

        cmd_count cmdCount = helper.get<cmd_count>();

        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            int nHeroId = helper.getInt();

            // 将禁用的英雄置灰，并设置为不可选
            setOneHeroIsDisable(nHeroId, true);
        }

    }

    // 禁用英雄是选择英雄后会返回禁用英雄的信息
    public void MatchRoomGlobleBanHeroList(IntPtr ptrParam, int nParam)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);

        cmd_count cmdCount = helper.get<cmd_count>();

        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            int nHeroId = helper.getInt();

            // 将禁用的英不显示
            setOneHeroIsDisable(nHeroId, true);
        }

    }

    // 禁用机器人
    public void MatchRoomRuleBanComputerList(IntPtr ptrParam, int nParam)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);

        cmd_count cmdCount = helper.get<cmd_count>();

        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            int nHeroId = helper.getInt();

            //将两个阵营全都禁用
            setOneCampComputerIsDisable(nHeroId, true, 0);
            setOneCampComputerIsDisable(nHeroId, true, 1);
        }

    }

    //开始选择英雄
    public void MatchRoomRuleSelectHero(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count cmdCount = helper.get<cmd_count>();

        // 重置10个位置高亮的部分
        setAllPosNotBright();

        int nTime = 0;
        int nCamp = 0;
        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            cmd_match_room_rule_ban_hero cmdInfo = helper.get<cmd_match_room_rule_ban_hero>();
            if (nTime == 0) nTime = cmdInfo.nTimeInterval;
            if (nCamp == 0) nCamp = cmdInfo.nCamp;
            if (cmdInfo.bIsSelf == 1)
            {
                CanSelectHero = true;
                //准备按钮开启
                SetHeroConfirmBtnVisible();
                SetSelectHeroListLock(false);
            }

            //高亮哪个位置可以禁用英雄
            SetChooseDisablePos(cmdInfo.nCamp, cmdInfo.nPos);


        }
        //启用选择英雄倒计时
        ShowSelectDisableHeroTime(nTime, nCamp, (int)SelectPhase.ENUM_SELECT);
    }

    // 禁止选英雄的个数
    public void MatchRoomRuleSetHeroLock(cmd_match_room_hero_can_select data)
    {

        bool bCanSelect = data.bCanSelect == 0 ? true : false;
        setOneHeroIsDisable(data.nHeroID, bCanSelect);
    }

    // 初始化选英雄状态
    public void InitSelectHeroStatus()
    {
        setAllPosNotBright();
        // 设置所有可用英雄
        SetAllHeroCanSelect();
        // 是否处于选英雄状态
        CanSelectHero = false;
        // 是否在选禁用英雄状态
        CanChooseDisableHero = false;
        // 是否锁定英雄列表不让玩家选择
        SetSelectHeroListLock(false);
        ResetWaitingRoomWin();
        SmAllSkillInfoList.Clear();
        dDropMachineInfo.Clear();

        // 关闭su
        HideAllIcon();
        ChangeStateOfSkinViewAndHeroFrame(false);
        SetWaitingRoomStateOutOfTalentPage();
        CloseTalentPageView();
        bIsLocalMaster = false;
    }

    public void CloseTalentPageView()
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
    }

    // 保存召唤师技能信息
    public void saveAllSummonerSkill(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count cmdCount = helper.get<cmd_count>();

        for (int i = 0; i < cmdCount.nCount; ++i)
        {
            cmd_SmAllSkillInfo cmdInfo = helper.get<cmd_SmAllSkillInfo>();
            SmAllSkillInfoList.Insert(i, cmdInfo);
        }
    }
    public void RequestSkillInfo(int nHeroID)
    {
        cmd_match_room_Mouse_HeroArea data = new cmd_match_room_Mouse_HeroArea();
        data.nHeroVocationID = nHeroID;
        ViewEventHelper.Instance.SendCommand<cmd_match_room_Mouse_HeroArea>(GameLogicDef.GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA, ref data);
    }
    
    // 退出天赋状态
    public void SetWaitingRoomStateOutOfTalentPage()
    {        
        if (SFGFxMovieManager.CheckTheMovieCanUse(talentMenu))
        {
            talentMenu.SetWaitingRoomStateOutOfTalentPage();
        }
    }

    public void InitDropMachineInfo()
    {
        int nDropIdex = 0;

        string strHeroName = "";
        PersonSkinModelItem modelData = new PersonSkinModelItem();
        for (int i = (int)VOCATION.VOCATION_RANDOM; i < (int)VOCATION.VOCATION_MAX; i++)
        {

            // 查找英雄AI配置
            SSchemeHeroAIConfig SchmeInfo = null;
            HeroAIConfig.Instance.GetHeroAIInfo(nConfigWarID,i, ref SchmeInfo);
            if (SchmeInfo == null)
            {
                continue;
            }
            if (SchmeInfo.bIsOpen == false)
            {
                continue;
            }

            MachineInfo machineInfo = new MachineInfo();
            machineInfo.nIconId = i;// 是图片id
            machineInfo.bLock = false;

            strHeroName = "";
            if (i >= 0 && GameLogicAPI.GetPersonModel((UInt16)i, ref modelData))
            {
                strHeroName = modelData.szHeroName;
            }
            machineInfo.stDesc = strHeroName;
            dicDropHeroIdWithIndex[i] = nDropIdex;// 储存英雄图片id对应的槽位
            dDropMachineInfo.Add(machineInfo);
            nDropIdex++;
        }
        // 给每个槽位发送基本的数据

        // 给as发送假人的名字和图片id，假人名字这些数据还没接受
        for (int i = 0; i < 10; i++)
        {
            int nSlotId = i > 4 ? i - 5 : i;
            int nSlotCamp = i > 4 ? 1 : 0;
            SetMachineInfo(nSlotCamp, nSlotId);
        }

    }
}
