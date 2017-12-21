/*******************************************************************
** 文件名:	GFxHeroSelect.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/20
** 版  本:	1.0
** 描  述:	英雄选择界面逻辑
** 应  用:  对应Flash Layout文件：HeroSelectWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.3.17
** 描  述: 
 *         1、更改父类为SFMovieEx
 *         2、在获取AS对象时添加检测
** 修改人: 谭强均
** 日  期: 2015.3.20
** 描  述: 
 *         1、增加人物模型显示
** 修改人: 谭强均
** 日  期: 2015.6.18
** 描  述: 
 *         1、人物显示模型分离
** 修改人: 马传飞
** 日  期: 2015.9.2
** 描  述: 
 *         1、默认显示角色（创建 和刚登录时）\
** 修改人: 吴欣伟
** 日  期: 2015.9.28
** 描  述: 
 *         1、拆分C#代码中与AS通讯的逻辑
********************************************************************/

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;

public class HeroSelectEventArgs : EventArgs
{
	public int SelectHeroID;
}

public partial class GFxHeroSelect : SFMovieEx
{
    /// <summary>
    /// 英雄选择回调
    /// </summary>
    public static event EventHandler<HeroSelectEventArgs> OnHeroSelectChanged;

    /// <summary>
    /// 进入本界面
    /// </summary>
    public static event EventHandler OnHeroSelectEnter;

    /// <summary>
    /// 离开本界面
    /// </summary>
    public static event EventHandler OnHeroSelectLeave;

    /// <summary>
    /// 界面选择的英雄ID
    /// </summary>
    private int HeroID = 1;
    private ulong uPid = 0;

    /// <summary>
    /// 英雄头像ID与插槽ID表
    /// </summary>
    Hashtable HeroPortraitTable = new Hashtable();

    gamelogic_updateplayers PlayerLoginData = new gamelogic_updateplayers();
    #region GFx属性
    /// <summary>
    /// HeroSelectWin 布局文件上的几个mc
    /// </summary>
    //private Value TLFrame,TRFrame,BLFrame,BCFrame,BRFrame;
    private Value TFrame, CCFrame;
    /// <summary>
    /// 英雄选择界面上的技能插槽
    /// </summary>
    //private Dictionary<int, Value> SkillSlotGroup = new Dictionary<int, Value>();
    /// <summary>
    /// 最大的技能插槽数量
    /// </summary>
    private int MaxSkillSlot = 5;
#endregion

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

    }

    public override void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "TFrame", out TFrame);
        GetMemberWithCheck(MyRoot, "CCFrame", out CCFrame);
        HeroID = (int)MyRoot.GetMember("HeroID");

        SetMovieVisibleOnCreate();
        this.MyMovieIsRegisterSuccess = true;

        if (OnHeroSelectChanged != null)
        {
            HeroSelectEventArgs e = new HeroSelectEventArgs();
            e.SelectHeroID = HeroID;
            OnHeroSelectChanged(this, e);
        }
    }
    public void SendDefaultHero()
    {
        if (OnHeroSelectChanged != null)
        {
            HeroSelectEventArgs e = new HeroSelectEventArgs();
            e.SelectHeroID = HeroID;
            OnHeroSelectChanged(this, e);
        }
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_SELECT_HERO, HeroID);
    }
    // 获得当前玩家所拥有的第一个角色的iconId
    public int getHeroFirstIconId()
    {
        if (PlayerLoginData.playercnt == 0)
        {
            Trace.LogWarning("c# getHeroFirstIconId PlayerLoginData.playercnt = " + PlayerLoginData.playercnt);
            return 0;
        }
        return PlayerLoginData.players[0].heroid;
    }

    // 获得当前玩家所拥有的第一个角色的pid
    public uint getHeroFirstPid()
    {
        if (PlayerLoginData.playercnt == 0)
        {
            return 0;
        }
        return PlayerLoginData.players[0].pid;
    }

    // 设置当前玩家的英雄pid与heroId
    public void setHeroDefaultInfo(uint uPid, int nIconId )
    {
        if (uPid == 0 || nIconId == 0)
        {
            Trace.LogWarning("c# setHeroDefaultInfo uPid = " + uPid + "HeroId" + nIconId);
            return;
        }
        HeroID = nIconId;
        this.uPid = uPid;
        MyRoot.SetMember("SelectedPID", uPid);
        MyRoot.SetMember("HeroID", nIconId);
    }

    // 获得默认英雄的upid 和nIconId;
    public void getDefaultHeroInfo(ref ulong uPId, ref int nIconId )
    {
        uPId = this.uPid;
        nIconId = HeroID;
    }

    public void SendPlayerSelectHero()
    {
        //暂时随机一个数字区分角色
        int Index = UnityEngine.Random.Range(0, 100);
        string PlayerName = "角色名" + Index;
        //将玩家选择的英雄信息丢给逻辑层
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_ENTER_GAME, HeroID, PlayerName);
    }

    #region 其他外部接口
    //离开英雄选择界面
    public override void LeaveMovie()
    {
        base.LeaveMovie();
        this.SetMovieVisible(false);
        if (OnHeroSelectLeave != null)
        {
            OnHeroSelectLeave(this, new EventArgs());
        }
        //离开界面时要把表清除
        HeroPortraitTable.Clear();
    
    }
    public override void EnterMovie()
    {
        base.EnterMovie();
        SetHeroHeadSlot();
        //HeroBtnPress(1,1);
        this.SetMovieVisible(true);
        if (OnHeroSelectEnter != null)
        {
            OnHeroSelectEnter(this, new EventArgs());
        }

        if (HeroID == 0)
        {
            HeroID = PlayerLoginData.players[0].heroid;
        }
        //SendDefaultHero();
    }
    //接受逻辑层发来的显示英雄消息
    public void SetUserSelectHero(cmd_choose_hero_sync _data)
    {
        //显示选择的按钮状态
        foreach (DictionaryEntry keypar in HeroPortraitTable)
        {
            if (Convert.ToInt32(keypar.Value) == Convert.ToInt32(_data.HeroID))
            {
                SetBtnNormalState(Convert.ToInt32(keypar.Key), _data.HeroID);
            }
        }

        //显示选择的英雄模型
        if (OnHeroSelectChanged != null)
        {
            HeroSelectEventArgs e = new HeroSelectEventArgs();
            e.SelectHeroID = _data.HeroID;
            OnHeroSelectChanged(this, e);
        }

        //显示选择的英雄技能
        SetSkillSlotInfo(_data.byIndex, _data.SpellData.nIcon, _data.SpellData.szName, _data.SpellData.szDesc, _data.nCD);
        //显示英雄的属性
        SetHeroProperty(_data);
    }

    /// <summary>
    /// 先存储逻辑层发来的角色信息
    /// </summary>
    /// <param name="_data"></param>
    public void PlayerLoginInfo(gamelogic_updateplayers _data)
    {
        PlayerLoginData = _data;
        SetHeroHeadSlot();

    }
#endregion

    #region AS->C#
    /// ///////////////临时,等大厅完善再移除
    public override void OnDestroy()
    {

        base.OnDestroy();
        //移除场景资源载入消息
        SceneManager.Instance.ProgressEvent -= LoadSceneOver;
    }
    public void onCreateHeroBtnClick()
    {
        this.LeaveMovie();
        SFGFxMovieManager.GetWindow<GFxHeroCreateWin>().EnterMovie();

    }

    public void DirectlyEnterScene(string sceneName, int mapID)
    {

        //销毁角色选择界面的模型
        //(SFGFxMovieManager.GetWindow<("GFxHeroSelect") as GFxHeroSelect).ClearModels();
        //销毁选择角色界面
        SFGFxMovieManager.Instance.DestroyWindow("GFxHeroSelect");
        //销毁游戏大厅界面
        //销毁创建房间界面
        SFGFxMovieManager.Instance.DestroyWindow("GFxGameCreateingRoomWin");
        //销毁玩家等待房间界面
        SFGFxMovieManager.Instance.DestroyWindow("GFxGameWaitingRoomWin");


        //登录之后再把游戏界面创建出来,全部默认为显示
        //SFGFxMovieManager.Instance.CreateWindow<GFxGameHud>("GFxGameHud", "GFx/GameHud.swf", SFGFxMovieManager.DefaultBGColor);
        /////////////////////////////////////////////////////////////////创建附加的界面
        //SFGFxMovieManager.Instance.CreateWindow<GFxMinimap>("GFxMinimap", "GFx/Minimap.swf", SFGFxMovieManager.DefaultBGColor);
       // SFGFxMovieManager.Instance.CreateWindow<GFxMainChatHud>("GFxMainChatHud", "GFx/MainChatHud.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxDragonBall>("GFxDragonBall", "GFx/DragonBall.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarRecordTable>("GFxWarRecordTable", "GFx/WarRecordTable.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarTalent>("GFxWarTalent", "GFx/WarTalent.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxMMODefaultHud>("GFxMMODefaultHud", "GFx/MMODefaultHud.swf", SFGFxMovieManager.DefaultBGColor);

        SceneManager.Instance.ProgressEvent += LoadSceneOver;
        SceneManager.Instance.LoadScene(sceneName, mapID, Vector3.zero);


    }
    public void LoadSceneOver(object sender, SceneProgressEventArgs e)
    {
        if (e.progress >= 1)
        {
            SendPlayerSelectHero();
            this.SetMovieVisible(false);
        }
    }
    /////////////////////////////////

    /// <summary>
    /// 玩家准备消息
    /// </summary>
    /// <param name="_HeroID"></param>
    public void ReadyBtnPress(double _pID)
    {
        HeroID = (int)_pID;
        //将玩家唯一的角色ID丢给逻辑层
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_ENTER_GAME,(int) _pID);
    }

    /// <summary>
    /// 当前选择的英雄头像
    /// </summary>
    /// <param name="_HeroID"></param>
    public void HeroBtnPress(int _pID, int _IconId )
    {
        //this.HeroID = (int)_HeroID;
        this.HeroID = (int)_IconId;

        if (OnHeroSelectChanged != null)
        {
            HeroSelectEventArgs e = new HeroSelectEventArgs();
            //e.SelectHeroID = HeroID;
            e.SelectHeroID = _IconId;
            OnHeroSelectChanged(this, e);
        }
        //更换选着的模型
        //ChangeSelectedModel(HeroID);
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_SELECT_HERO, this.HeroID);

    }
#endregion

}

