/*******************************************************************
** 文件名:	GFxGameHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2014/12/10
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示
** 应  用:  显示玩家当前状态和基本按钮	
 *          对应Flash Layout文件：GameHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.1.16
** 描  述: 美术给了新UI，添加新UI英雄相关的功能
 *         移除之前的测试功能
********************************************************************
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.2.6
** 描  述: 增加Buff图标显示接口
********************************************************************
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.3.17
** 描  述: 
 *         1、更改父类为SFMovieEx
 *         2、在获取AS对象时添加检测
********************************************************************
***************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016.4.8
** 描  述: 
 *         1、更改游戏设置界面对应的swf文件
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using War;
using U3D_Render;
using U3D_Render.Utility;
using ASpeedGame.Data.GameMobaSchemes;

public partial class GFxGameHud : SFMovieEx//,ITimerHanlder,IEventHandler
{
    //enum EN_GameHud_Timer
    //{
    //    ENGT_Pointer = 0,     // 指示器定时器id
    //}

    private bool showCrossHair = true;

    /// <summary>
    /// 准星在Unity中坐标
    /// </summary>
    private Vector3 _vCrossHairPos = Vector3.zero;
    /// <summary>
    /// 准星在Unity中颜色
    /// </summary>
    private Color _vCrossHairColor = Color.gray;
    /// <summary>
    /// 自动寻路图片Y方向偏移基础值
    /// </summary>
    public int AutoMoveEffectYPosOffsetBase = 200;

    #region GFx属性
    /// <summary>
    /// 死亡倒计时MC
    /// </summary>
    //private Value ReLiveTimeMC;

    /// <summary>
    /// 准星MC
    /// </summary>
    private Value CrossHairMC;

    /// <summary>
    /// 右上角的战场信息显示
    /// </summary>
    //protected Value warTopRight;

    /// <summary>
    /// 广播击杀消息
    /// </summary>
    //protected Value vBroadCastMessageMgr;
    /*阵营ID*/
    //private int MainHeroCampID = 0;
    /// <summary>
    /// 自动寻路UI 显示元件
    /// </summary>
    protected SFDisplayInfo diAutoMove;
    /// <summary>
    /// 技能提示元件
    /// </summary>
    protected Value vSpellPrompt;
    #endregion


    /// <summary>
    /// 数值属性变化处理
    /// </summary>
    /// <param name="data"></param>
    override public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {
        if (e.objev == null)
        {
            return;
        }
    }


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        bCanSyncTickCount = true;
        base.Init(_parent, _sfMgr, _cp);

        SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);

        //TimerManager.SetTimer(this, (int)EN_GameHud_Timer.ENGT_Pointer, 0.5f);
        //EventManager.Subscibe(this, EVENT.ENTITY_SHOW_HERO, 0);
    }

    //override public void Update()
    //{
    //    if (!IsRegisterMovieSuccess || tmpAroundEntitys == null )
    //        return;
    //    //指示器更新
    //    GetPlayerAndUpdate();
    //}

    override public void OnDestroy()
    {
        //ClearBillboardCache();
        
        //EntityFactory.CreateMainHeroEntityEvent -= SetCamp;

        base.OnDestroy();

        //TimerManager.KillTimer(this, (int)EN_GameHud_Timer.ENGT_Pointer);
        //EventManager.UnSubscibe(this, EVENT.ENTITY_SHOW_HERO, 0);


    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);

        //GetMemberWithCheck(MyRoot, "ReLiveTimeMC", out ReLiveTimeMC);
        GetMemberWithCheck(MyRoot, "CrossHair_mc", out CrossHairMC);
        //GetMemberWithCheck(MyRoot, "warTopRight", out warTopRight);
        //GetMemberWithCheck(MyRoot, "BroadCastMessageMgr", out vBroadCastMessageMgr);
        //定义主英雄实体创建委托
        //EntityFactory.CreateMainHeroEntityEvent += SetCamp;

        //BillboardCache.InitBillboardCache(this.MyRoot, "Com.Scaleform.WarGameHud.WGH_HeroBearing", "Billboard");

        //if (Config.UseNewSlot)
        //{
        //    setWarGameHudInvisible();
        //}
    }

    //public void setWarGameHudInvisible()
    //{
    //    if (MyRoot != null && MyRoot.IsValid())
    //    {
    //        MyRoot.Invoke("setWarTopRightInvisible");
    //    }
    //}

    void CreateMobaGameHud()
    {
        //4次左右(第四次开始崩）
        //SFGFxMovieManager.Instance.CreateWindow<GFxMinimap>("GFxMinimap", "GFx/Minimap.swf", SFGFxMovieManager.DefaultBGColor, false);

        //3次左右崩(第三次结束就崩)
       // SFGFxMovieManager.Instance.CreateWindow<GFxTeamRightHud>("GFxTeamRightHud", "GFx/WarTeamRightHud.swf", SFGFxMovieManager.DefaultBGColor, false);
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarUponRightHud>("GFxWarUponRightHud", "GFx/TeamInfoTopRight.swf", SFGFxMovieManager.DefaultBGColor, false);
        //pop up movie
        //3次左右崩(第三次结束就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxDragonBall>("GFxDragonBall", "GFx/DragonBall.swf", SFGFxMovieManager.DefaultBGColor);
        //4次左右(第四次一进战场就崩)
        //5次左右(第五次一进战场就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarPauseMenu>("GFxWarPauseMenu", "GFx/WarGameSetting.swf", SFGFxMovieManager.DefaultBGColor);
        //3次左右(第三次一进战场就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarGoodsStore>("GFxWarGoodsStore", "GFx/WarGoodsStore.swf", SFGFxMovieManager.DefaultBGColor);
        //4次左右(第四次一进战场就崩）
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarHeroDetailedInfoPanel>("GFxWarHeroDetailedInfoPanel", "GFx/WarHeroDetailedInfoPanel.swf", SFGFxMovieManager.DefaultBGColor);
        //4次左右(第四次一进战场就崩）
        //SFGFxMovieManager.Instance.CreateWindow<GFxChatTable>("GFxChatTable", "GFx/ChatTable.swf", SFGFxMovieManager.DefaultBGColor);

    }

    void CalCrossHairPos()
    {
        if (!this.IsRegisterMovieSuccess)
            return;

        SFDisplayInfo di = CrossHairMC.GetDisplayInfo();
        double _x = di.X;
        double _y = di.Y;
        Vector3 CrossHairStagePos = new Vector3((float)_x, (float)_y);
        _vCrossHairPos = this.StagePointToScreenPoint(CrossHairStagePos);
        //#if UNITY_EDITOR
        //        _vCrossHairPos.y -= 14;
        //#endif
    }



    #region AS->C#
    public void SendChatKillInfo(String ChatKillStr)
    {
        Trace.TraceSFUIWaring("GFxChatTable", "GFxGameHud.SendChatKillInfo", "ShowChatInfo");

    }





    #endregion

    #region 接口
    /// <summary>
    /// 外部控制显示复活倒计时
    /// </summary>
    /// <param name="_time">倒计时：毫秒</param>
    //public void ShowReliveTime(float _time)
    //{
    //    if (!Value.IsValueValid(ReLiveTimeMC))
    //        return;

    //    //@param _time 输入的是毫秒
    //    //@param _bAutoRemove 是否自动移除倒计时
    //    if (ReLiveTimeMC.Invoke("ShowReliveTime", _time, false) == null)
    //    {
    //        Trace.LogError("call ShowReliveTime error!");
    //    }
    //}

    //public void RemoveReliveTime()
    //{

    //    if (!Value.IsValueValid(ReLiveTimeMC))
    //        return;

    //    if (ReLiveTimeMC.Invoke("RemoveTime") == null)
    //    {
    //        Trace.LogError("call RemoveTime error!");
    //    }
    //}

    /// <summary>
    /// 外部调用UI上广播连杀显示
    /// </summary>
    ///
    //public void onBroadKillcount(int isSampCampKill, int nCamp, int nTimeKillCount, int MurderEntityType, string sMurderName, int nMurderHeroID, int MasterEntityType, string sMasterName, int nMasterHeroID, bool bIsAce, string sMasterVocationName, string sMurdererVocationName)
    //{
    //    bool bIsSampCampKill = isSampCampKill > 0 ? true : false;
    //    if (vBroadCastMessageMgr != null && vBroadCastMessageMgr.IsValid())
    //    {
    //        if (null == vBroadCastMessageMgr.Invoke("onBroadKillcn", bIsSampCampKill, nTimeKillCount, MurderEntityType, sMurderName, nMurderHeroID, MasterEntityType, sMasterName, nMasterHeroID, bIsAce, nCamp,sMasterVocationName, sMurdererVocationName))
    //        {
    //            Trace.LogError("onBroadKillcount invoke == NULL");
    //        }


    //    }

    //}

    /// <summary>
    /// 外部调用UI上广播连击显示
    /// </summary>
    /// <param name="nCombaNum"></param>
    //public void onBroadCombaAttac(int nCombaNum)
    //{
    //    if (MyRoot != null && MyRoot.IsValid())
    //    {
    //        if (null == MyRoot.Invoke("onBroadCombaAttac", nCombaNum))
    //        {
    //            Trace.LogError("onBroadCombaAttac invoke == NULL ");
    //        }
    //    }
    //}

    //public void ShowCombaKillNum(int nKillNum)
    //{
    //    if (MyRoot != null && MyRoot.IsValid())
    //    {
    //        if (null == MyRoot.Invoke("ShowCombaKillNum", nKillNum))
    //        {
    //            Trace.LogError("ShowCombaKillNum invoke == NULL");
    //        }
    //    }
    //}

    /// <summary>
    /// 外部调用UI上广播击杀显示
    /// </summary>
    ///
    //public void onBroadImprotantEt(int isSampCampKill, bool bIsFirstBlood, bool bIsCampSkill, int nCamp, int nMasterEntityType, string sMasterName, int nMasterHeroID, int nMurdererEntityType, string sMurdererName, int nMurdererHeroID, int nKillCount, int nSoundID, string szDes, bool isAce, bool bIsOver, string sMasterVocationName, string sMurdererVocationName)
    //{
    //    bool bIsSampCampKill = isSampCampKill > 0 ? true : false;
    //    if (vBroadCastMessageMgr != null && vBroadCastMessageMgr.IsValid())
    //    {
    //        if (null == vBroadCastMessageMgr.Invoke("onBroadImprotantEt", bIsSampCampKill, bIsFirstBlood, bIsCampSkill, nCamp, nMasterEntityType, sMasterName, nMasterHeroID, nMurdererEntityType, sMurdererName, nMurdererHeroID, nKillCount, nSoundID, szDes, isAce, bIsOver, sMasterVocationName, sMurdererVocationName))
    //        {
    //            Trace.LogError("onBroadImprotantEt invoke == NULL");
    //            return;
    //        }
    //    }
    //}

    /// <summary>
    /// 外部调用UI上广播击杀显示
    /// </summary>
    ///
    //public void onBroadMonsterRelive(int isSampCamp, int nCamp, int nMasterEntityType, int nMurdererHeroID, string szName, int nSoundID, string szDes, int isEarlyRelive)
    //{
    //    bool bIsSampCamp = isSampCamp > 0 ? true : false;
    //    bool bIsEarlyRelive = isEarlyRelive > 0 ? true : false;
    //    if (vBroadCastMessageMgr != null && vBroadCastMessageMgr.IsValid())
    //    {
    //        if (null == vBroadCastMessageMgr.Invoke("onBroadMonsterRelive", bIsSampCamp, nCamp, nMasterEntityType, nMurdererHeroID, szName, nSoundID, szDes, bIsEarlyRelive))
    //        {
    //            Trace.LogError("onBroadMonsterRelive invoke == NULL");
    //        }
    //    }
    //}

    /// <summary>
    /// 外部调用UI上广播击杀显示
    /// </summary>
    ///
    //public void onBroadMonsterDieText(int isSampCamp, int nCamp, int nMasterEntityType, int nMurdererHeroID, string szName, int nSoundID, string szDes)
    //{
    //    bool bIsSampCamp = isSampCamp > 0 ? true : false;
    //    if (vBroadCastMessageMgr != null && vBroadCastMessageMgr.IsValid())
    //    {
    //        if (null == vBroadCastMessageMgr.Invoke("onBroadMonsterDieText", bIsSampCamp, nCamp, nMasterEntityType, nMurdererHeroID, szName, nSoundID, szDes))
    //        {
    //            Trace.LogError("onBroadMonsterDieText invoke == NULL");
    //        }
    //    }
    //}

    /// <summary>
    /// 外部调用战场结束显示界面
    /// </summary>
    ///
    public void OnShowEndOfWar(bool isWin)
    {

        Trace.TraceSFUIWaring("GFxWarBottomCenterHud", "OnShowEndOfWar", "setIsEnable");

    }

    /// <summary>
    /// 外部调用战场结束显示记录界面
    /// </summary>
    ///
    public void OnShowEndWarRecord()
    {
        SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Default);


        //GfxBloodManager.Clear();

    }



    /// <summary>
    /// 准星在Unity中坐标
    /// </summary>
    public Vector3 CrossHairPos
    {
        get
        {
            CalCrossHairPos();
            return this._vCrossHairPos;
        }

        set
        {
            if (!this.IsRegisterMovieSuccess || !Value.IsValueValid(CrossHairMC))
                return;

            SFDisplayInfo di = CrossHairMC.GetDisplayInfo();
            di.X += value.x;
            di.Y += value.y;
            CrossHairMC.SetDisplayInfo(di);

        }
    }

    /// <summary>
    /// 准星在Unity中颜色
    /// </summary>
    public Color CrossHairColor
    {
        get
        {
            return this._vCrossHairColor;
        }

        set
        {
            if (Value.IsValueValid(CrossHairMC))
            {
                CrossHairMC.SetColorTransform(new SFCxForm(value));
                this._vCrossHairColor = value;
            }
        }
    }

    /// <summary>
    /// 设置准星的显示隐藏
    /// </summary>
    public bool CrossHairVisible
    {
        set
        {
            if (Value.IsValueValid(CrossHairMC))
            {
                SFDisplayInfo di = CrossHairMC.GetDisplayInfo();
                di.Visible = value;
                CrossHairMC.SetDisplayInfo(di);
                showCrossHair = value;
            }
        }

        get
        {
            return showCrossHair;
        }
    }
    
    public bool AutoMoveEffectActive
    {
        get
        {
            if (diAutoMove == null || diAutoMove.Visible == false)
                return false;
            else
                return true;
        }

        set
        {
            if(diAutoMove == null)
            {
                Value mcAutoMove = new Value();
                bool res = MyRoot.AttachMovie(ref mcAutoMove, "Library.AutoMove", "mcAutoMove", -1);
                if (res)
                {
                    float offset = 0.0f;
                    if(EntityFactory.MainHero != null && EntityFactory.MainHero.GetComponent<CreatureProperty>() != null)
                    {
                        offset = EntityFactory.MainHero.GetComponent<CreatureProperty>().CreatureBoundingRad.y * AutoMoveEffectYPosOffsetBase;
                    }
                    diAutoMove = mcAutoMove.GetDisplayInfo();
                    diAutoMove.Visible = false;
                    diAutoMove.X = this.MovieDef.GetWidth() / 2 - (offset/2);
                    diAutoMove.Y = this.MovieDef.GetHeight() / 2 - offset;
                    mcAutoMove.SetDisplayInfo(diAutoMove);
                }
            }
            if(value != diAutoMove.Visible)
            {
                diAutoMove.Visible = value;
                MyRoot.GetMember("mcAutoMove").SetDisplayInfo(diAutoMove);
            }
        }
    }
    #endregion
    //#region 指示器
    ///// <summary>
    ///// 指示器对象显示半径
    ///// </summary>
    //public float BillboardRadius = 600;
    ///// <summary>
    ///// 指示器对象在这个角度内以直线方式绘制
    ///// </summary>
    //public int BillboardAngle = 30;
    ///// <summary>
    ///// 指示器坐标缓存
    ///// </summary>
    //protected UIBillboardCache BillboardCache = new UIBillboardCache();
    ///// <summary>
    ///// 临时实体数据
    ///// </summary>
    //private List<UIBillboardQueryResult> tmpAroundEntitys = new List<UIBillboardQueryResult>();
    ///// <summary>
    ///// 范围内活动的实体
    ///// </summary>
    //protected Dictionary<Int32, EntityView> RangeActivityEntity = new Dictionary<int, EntityView>();

    ///// <summary>
    ///// 需要隐藏的实体
    ///// </summary>
    //List<int> HideHeroList = new List<int>();

    ////保存附近玩家
    //public void saveAroundPlayer()
    //{
    //    EntityView ev = EntityFactory.getMainPlayer();
    //    if (ev == null)
    //    {
    //        return;
    //    }
    //    List<Int32> TmpEntityIDList = new List<int>(RangeActivityEntity.Keys);

    //    IList<EntityView> AroundEntitiesList = MapFinderManager.Nearest(ev.StateMachine.transform.position, ev.Sight, SPELL.CampFlag.CampFlag_Enemy);
    //    if (AroundEntitiesList == null)
    //    {
    //        return;
    //    }
    //    Int32 nTmpEntityID = 0;
    //    foreach (EntityView Entity_Ev in AroundEntitiesList)
    //    {
    //        nTmpEntityID = Entity_Ev.ID;
    //        if (Entity_Ev == null || Entity_Ev.StateMachine == null || Entity_Ev.StateMachine.GetState() == EntityState.Dead || Entity_Ev.Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
    //            continue;
    //        if (!RangeActivityEntity.ContainsKey(nTmpEntityID))
    //        {
    //            RangeActivityEntity.Add(nTmpEntityID, Entity_Ev);
    //        }
    //        else
    //        {
    //            TmpEntityIDList.Remove(nTmpEntityID);
    //        }
    //    }

    //    while (TmpEntityIDList.Count > 0)
    //    {
    //        nTmpEntityID = TmpEntityIDList[0];
    //        RangeActivityEntity.Remove(nTmpEntityID);
    //        TmpEntityIDList.RemoveAt(0);
    //    }
    //}

    ////更新指示器公告板信息
    //private void UpdateBillboard(System.Collections.Generic.List<UIBillboardQueryResult> entitys)
    //{
    //    BillboardCache.BeginProcessing();
    //    SFDisplayInfo info;
    //    UIBillboardMC tmpBillboard;
    //    for (int i = 0; i < entitys.Count; i++)
    //    {
    //        //从缓冲列表里获取可用的对象
    //        if (BillboardCache.GetUnusedBillboardMovieclip(entitys[i].entityID, out tmpBillboard))
    //        {
    //            info = tmpBillboard.Billboard.GetDisplayInfo();
    //            info.Visible = true;
    //            info.X = entitys[i].x;
    //            info.Y = entitys[i].y;
    //            tmpBillboard.Billboard.SetDisplayInfo(info);

    //            EntityView targetEntityView = EntityFactory.getEntityViewByID(entitys[i].entityID);
    //            int nEntityHeroID = targetEntityView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
    //            int nTargetCamp = (int)targetEntityView.CampFlag;
    //            //public function SetHeroBearing(_nTargetHeroPortraitID:int,_nTargetCamp:int,_strTargetDistance:String)

    //            if (tmpBillboard.Billboard.Invoke("SetHeroBearing", nEntityHeroID, nTargetCamp, entitys[i].angle,entitys[i].distance+"米") == null)
    //            {
    //                Trace.LogError("call SetHeroBearing failed.");
    //            }
    //        }
    //    }
    //    BillboardCache.EndProcessing();
    //}

    //private void GetPlayerAndUpdate()
    //{
    //    //移除上次调用存储的数据
    //    tmpAroundEntitys.Clear();
    //    if (RangeActivityEntity.Count > 0 && EntityFactory.MainHeroView.StateMachine.GetState() != EntityState.Dead)
    //    {
    //        foreach (KeyValuePair<int, EntityView> keypair in RangeActivityEntity)
    //        {
    //            GetQueryTarget(keypair.Key, keypair.Value.gameObject.transform);
    //        }
    //    }
    //    UpdateBillboard(tmpAroundEntitys);
    //}

    ////计算英雄与附近的玩家目标的角度并映射到UI上
    //private void GetQueryTarget(Int32 entityID, Transform TargetEntityTrans)
    //{
    //    Transform CameraTransform = SoldierCamera.MainInstance.getCameraTransform();
    //    //计算位置应该是计算英雄的位置，而不是相机的位置
    //    float checkDis = Vector3.Distance(TargetEntityTrans.position, EntityFactory.MainHero.transform.position);

    //    if (checkDis > EntityFactory.MainHeroView.Sight || ScreenRaycast.isInScreen(TargetEntityTrans.position))
    //        return;

    //    Vector3 vecHeroPos = EntityFactory.MainHeroView.gameObject.transform.position;
    //    Vector3 DirHeroToTarget = TargetEntityTrans.position - vecHeroPos;
    //    Vector3 DirHeroToCamera = CameraTransform.position - vecHeroPos;
    //    DirHeroToTarget.y = DirHeroToCamera.y = 0;


    //    //判断目标在相机的左右方，-1左方，+1右方
    //    Vector3 vecCross = Vector3.Cross(DirHeroToTarget.normalized, DirHeroToCamera.normalized);
    //    float fLorR = Mathf.Sign(-vecCross.y);

    //    //计算之间的夹角
    //    float angle = Vector3.Angle(DirHeroToTarget.normalized, DirHeroToCamera.normalized);        
    //    float relAngle = (angle * fLorR + 90) % 360;


    //    float MovieWidth = MovieDef.GetWidth();
    //    float MovieHeight = MovieDef.GetHeight();

    //    UIBillboardQueryResult qr = new UIBillboardQueryResult();
    //    qr.entityID = entityID;
    //    qr.x = (MovieWidth / 2) + BillboardRadius * (Mathf.Cos(Mathf.Deg2Rad * (relAngle)));
    //    qr.y = (MovieHeight / 2) + BillboardRadius * (Mathf.Sin(Mathf.Deg2Rad * (relAngle)));

    //    //在这个角度范围内图标水平移动
    //    if (relAngle > BillboardAngle && relAngle < BillboardAngle + 120)
    //    {
    //        qr.y = (MovieHeight / 2) + BillboardRadius * (Mathf.Sin(Mathf.Deg2Rad * BillboardAngle));
    //    }

    //    qr.distance = Convert.ToUInt32(checkDis);
    //    qr.angle = Convert.ToInt32(relAngle+90);
    //    tmpAroundEntitys.Add(qr);

    //}

    //public void ClearBillboardCache()
    //{
    //    if (BillboardCache != null)
    //    {
    //        BillboardCache.ClearCache();
    //        BillboardCache = null;
    //    }
        
    //    if (RangeActivityEntity == null)
    //    {
    //        RangeActivityEntity.Clear();
    //        RangeActivityEntity = null;
    //    }

    //    if (tmpAroundEntitys != null)
    //    {
    //        tmpAroundEntitys.Clear();
    //        tmpAroundEntitys = null;
    //    }
        
    //    if (HideHeroList != null)
    //    {
    //        HideHeroList.Clear();
    //        HideHeroList = null;
    //    }
    //}


    //public void OnTimer(int nTimerID)
    //{
    //    switch( nTimerID )
    //    {
    //        case (int)EN_GameHud_Timer.ENGT_Pointer:
    //            {
                     
    //                saveAroundPlayer();

    //                int nCount = HideHeroList.Count;
    //                for (int i = 0; i < nCount; ++i  )
    //                {
    //                    if (RangeActivityEntity.ContainsKey(HideHeroList[i]))
    //                    {
    //                        RangeActivityEntity.Remove(HideHeroList[i]);
    //                    }
    //                }
    //            }
    //            break;
    //        default:
    //            break;
    //    }
    //}

    

    //public void OnEvent(U3D_Render.Utility.EVENT eventID, int sourceID, System.Object objEvent)
    //{
    //    switch( eventID )
    //    {
    //        case EVENT.ENTITY_SHOW_HERO:
    //            {
    //                if ( objEvent == null )
    //                    return;
    //                SEntityShowHero event_data = (SEntityShowHero)objEvent;
    //                int nId = event_data.ev.ID;
    //                if (event_data.bIsHide && !HideHeroList.Contains(nId))
    //                    HideHeroList.Add(nId);
    //                else if (!event_data.bIsHide && HideHeroList.Contains(nId))
    //                    HideHeroList.Remove(nId);
    //            }
    //            break;
    //        default:
    //            break;
    //    }
    //}

    //#endregion
    #region 技能提示
    public void CreatureShowPormptUI(cmd_creature_ShowPromptUI data)
    {
        if (vSpellPrompt == null)
        {
            vSpellPrompt = new Value();
            MyRoot.AttachMovie(ref vSpellPrompt, "Com.Scaleform.WarGameHud.WGH_SpellPrompt", "vSpellPrompt", 0);
            SFDisplayInfo di = vSpellPrompt.GetDisplayInfo();
            di.X = 1920 / 2 + 100;
            di.Y = 1080 - 300;
            vSpellPrompt.SetDisplayInfo(di);
        }

        IntPtr DesPtr, IconPtr; //R
        ASpeedGame.Data.GameMobaSchemes.SSchemeShowSpellPrompt SchemesSpellPrompt = ASpeedGame.Data.GameMobaSchemes.GameMobaSchemesManager.Instance.GetSpellPromptInfo(data.nPromptID);


        if (SchemesSpellPrompt != null)
        {
            DesPtr = GameLogicAPI.getShortcutDes(SchemesSpellPrompt.nSlotID, (int)SLOT_TYPE.SLOT_TYPE_SKILL);
            IconPtr = GameLogicAPI.getSpellData(SchemesSpellPrompt.nSpellID);

            if (DesPtr != IntPtr.Zero && IconPtr != IntPtr.Zero )
            {
                string des = IntPtrHelper.Ptr2Str(DesPtr);
                SPELL.SPELL_DATA spellData = IntPtrHelper.toData<SPELL.SPELL_DATA>(IconPtr);

                if (vSpellPrompt == null || vSpellPrompt.Invoke("SetSpellPromptInfo", spellData.nIcon, des.ToUpper(), Convert.ToBoolean(data.nType)) == null)
                    Trace.LogWarning("SetSpellPromptInfo failed.");

            }
        }
    }
    #endregion
}
