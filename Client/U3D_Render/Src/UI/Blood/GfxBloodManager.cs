using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;
using War;
/// <summary>
/// Blood管理器，负责Blood的创建删除以及更新
/// </summary>
public partial class GfxBloodManager
{


    #region API

    /// <summary>
    /// 创建一个Blood，返回一个GfxBlood实例，失败返回null
    /// </summary>
    /// <param name="entityID">实体ID</param>
    /// <param name="Host">实体</param>
    /// <param name="campType">阵营类型</param>
    /// <param name="CurValue">当前值</param>
    /// <param name="MaxValue">最大值</param>
    public static GfxBlood CreateBlood(U3D_Render.EntityView objev, int entityID, GameObject Host, int campType, int CurValue = 100, int MaxValue = 100)
    {
        if (SFGFxMovieManager.Instance == null)
        {
            Trace.LogError("SFGFxMovieManager没有初始化，请不要在Awake中创建！");
            return null;
        }
        if (!bInit)
        {
            Trace.LogError("GfxBloodManager必须初始化！");
            return null;
        }
        //有可能GfxBloodMovie还没有创建完成.
        if (GfxBloodMovie.Instance == null)
        {
            //Trace.LogError("GfxBloodMovie尚未创建完成，请等待创建完成后再调用。");
            return null;
        }

        CreateBossBlood();

        if (!objev.IsValid)
        {
            Trace.LogWarning("实体视图无效！id=" + entityID);
            return null;
        }

        if (m_BloodMap.ContainsKey(entityID))
        {
            Trace.LogWarning(entityID + "已经有血条，返回已有的实例");
            GfxBlood gba = m_BloodMap[entityID];
            return gba;
        }

        CurValue = Mathf.Clamp(CurValue, 0, MaxValue);
        

        ENTITY_TYPE entityType = objev.Type;
        
        if (!m_entiyTypeMap.ContainsKey(entityType))
        {
            Trace.LogWarning(entityID + "找不到实体类型：" + entityType + "使用怪物类型代替，请在GfxBloodManager：：Init()中添加");
            entityType = ENTITY_TYPE.TYPE_MONSTER;
        }

        //按照策划要求，目前不创建己方小兵血条,其他己方物体不变
        if (objev.Flag == (int)EWarMonsterExec.EWME_Soldier && campType == (int)GFxCampManager.GFxCampTpye.CT_Friend)
        {
            return null;
        }

        MovieNode trs = new MovieNode();
        GfxBlood gb = GetBlood(entityType,entityID == EntityFactory.MainHeroID,ref trs);
        if(null == gb)
        {
            return null;
        }
        gb.Init(entityID, Host, campType, entityType);
        gb.CreateBillBorad();
        //获取最大值和当前值
        MaxValue = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
        CurValue = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);

        gb.SetMaxValue(MaxValue);
        gb.SetCurValue(CurValue);

        //设置一下血量数字信息
        string bloodInfo = CurValue + "/" + MaxValue;
        gb.SetBloodInfo(bloodInfo);

        //设置血量数字信息是否显示，通过读取本地配置文件
        gb.SetBloodInfoVisible(bBloodInfoVisible);

        //判断一下如果是主角，则通过读取本地配置文件，设置主角血量是否显示
        if (entityID == EntityFactory.MainHeroID)
        {
            //读取本地配置文件，获取玩家血条是否需要显示
            bPlayerBloodVisible = ASpeedGame.Data.GameSettingsXml.GameSettingsXmlManager.Instance.GameSettingsModel.showPlayerBlood.AValue;
            gb.SetPlayerVisible(bPlayerBloodVisible);
        }

        gb.ChangeMovieNode(trs);
        m_BloodMap.Add(entityID, gb);
        ChangeMask(objev, true);
        //if (!go)
        //{
        //    go = GameObject.CreatePrimitive(PrimitiveType.Plane);

        //    go.GetComponent<Renderer>().sharedMaterial.SetTexture("_MainTex", GfxBloodMovie.BloodRenderTexture);
        //} 
        if (entityType == ENTITY_TYPE.TYPE_PLAYER_ROLE && entityID != EntityFactory.MainHeroID)
        {
            setHeroProperty(objev);
        }
        return gb;
    }

    public static void setHeroProperty(U3D_Render.EntityView objev)
    {
        int heroID = objev.ID;
        int nMaxHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
        int nCurHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
        int nTempHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);
        int nLevel = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
        int nCamp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);

        EntityView objMaster = EntityFactory.getPlayerViewByID(EntityFactory.MainHeroID);
        int nMainHeroCamp = (objMaster != null ? objMaster.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP) : 0);
        if (nCamp == nMainHeroCamp)
        {
            nCamp = 2;
        }
        else
        {
            nCamp = 3;
        }

        SetCamType(heroID, nCamp);
        SetMaxValue(heroID, nMaxHp);
        SetCurValue(heroID, nCurHp);
        SetLevel(heroID, nLevel);
        SetTempValue(heroID, nTempHp);
        //设置血量数字信息
        string bloodInfo = nCurHp + "/" + nMaxHp;
        SetBloodInfo(heroID, bloodInfo);
    }


    //static GameObject go;

    public static void UpdateBlood(int entityID, GameObject Host, int campType)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gb = m_BloodMap[entityID];
            gb.Entry = Host;
            gb.SetCamType(campType);
        }
    }

    /// <summary>
    /// 切换血量数字信息是否显示
    /// </summary>
    /// <param name="bvisible">是否显示</param>
    public static void ChangeBloodInfoVisible(bool bvisible)
    {
        if (bBloodInfoVisible == bvisible)
        {
            return;
        }
        foreach (GfxBlood var in m_BloodMap.Values)
        {
            var.SetBloodInfoVisible(bvisible);
        }
        bBloodInfoVisible = bvisible;
    }

    /// <summary>
    /// 根据选项界面的修改项目名称，切换对应血量信息显示状态
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public static void ChangeBloodSetting(object sender, GameSettingBoolEventArgs e)
    {
        switch (e.GameSettingEventName)
        {
            case"bShowPlayerBlood":
                ChangePlayerBlood(e.GameSettingBoolValue);
                break;
            case "bShowBloodInfo":
                ChangeBloodInfoVisible(e.GameSettingBoolValue);
                break;
            default:
                break;
        }
    }

    /// <summary>
    /// 设置怪物血条的颜色
    /// </summary>
    /// <param name="nId"></param>
    /// <param name="bIsChange"></param>
    public static void SetBloodColor(int nId, bool bIsChange)
    {
        if (m_BloodMap.ContainsKey(nId) == false)
        {
            return;
        }

        m_BloodMap[nId].SetBloodColor(bIsChange);
    }

    /// <summary>
    /// 切换主角血量是否显示
    /// </summary>
    /// <param name="val">是否显示</param>
    public static void ChangePlayerBlood(bool val)
    {
        if (m_BloodMap.ContainsKey(EntityFactory.MainHeroID) && bPlayerBloodVisible != val)
        {
            bPlayerBloodVisible = val;
            if (!bPlayerDead)
            {
                m_BloodMap[EntityFactory.MainHeroID].SetPlayerVisible(val);
            }
        }
    }

    /// <summary>
    /// 初始化，必须先调用哦
    /// </summary>
    public static void Init()
    {
        if (!bInit)
        {
            //下面两个必须一起
            m_entiyTypeMap.Add(ENTITY_TYPE.TYPE_MONSTER, MonsterBloodAS3RefrenceName);
            m_entiyTypeMap.Add(ENTITY_TYPE.TYPE_PLAYER_ROLE, PlayerBloodAS3RefrenceName);
            m_entiyTypeMap.Add(ENTITY_TYPE.TYPE_TANK, BuildingsBloodAS3RefrenceName);

            m_entiySizeMap.Add(ENTITY_TYPE.TYPE_MONSTER, new Vector2(50.0f, 18.0f));
            m_entiySizeMap.Add(ENTITY_TYPE.TYPE_PLAYER_ROLE, new Vector2(115.0f, 25.0f));
            m_entiySizeMap.Add(ENTITY_TYPE.TYPE_TANK, new Vector2(88.0f,12.0f));

            InitCache();

            //读取本地配置文件，设置是否显示血量数字信息
            bBloodInfoVisible = ASpeedGame.Data.GameSettingsXml.GameSettingsXmlManager.Instance.GameSettingsModel.showBloodInfo.AValue;

            bInit = true;
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
            //SceneManager.SceneLoadFinish += OnSceneLoadFinish;
            TargetFrame.OnSelectedTarget += OnSelectTarget;
            TargetFrame.OnDisSelectedTarget += OnUnSelectTarget;
            GFxWarPauseMenu.BoolEvent += ChangeBloodSetting;

        }
    }


    public static void Clear()
    {
        if(!bInit)
        {
            return;
        }
        DestroyALL();
        m_entiySizeMap.Clear();
        m_entiyTypeMap.Clear();
        EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        TargetFrame.OnSelectedTarget -= OnSelectTarget;
        TargetFrame.OnDisSelectedTarget -= OnUnSelectTarget;
        GFxWarPauseMenu.BoolEvent -= ChangeBloodSetting;
        bInit = false;
    }

    public static void Update()
    {
        if (!bInit)
            return;


        NormalUpdate();


        UpdateBossBlood();

    }

    /// <summary>
    /// 改变mask
    /// </summary>
    /// <param name="id"></param>
    /// <param name="bUseInitMask"></param>
    public static void ChangeMask(EntityView ev,bool bUseInitMask = false)
    {
        if (ev != null)
        {
            if (ev.Type == ENTITY_TYPE.TYPE_MONSTER)
            {
                int mas = 0;
                if (bUseInitMask)
                {
                    mas = ev.InitMaskFlag;
                }
                else
                {
                    mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

                    if (!bFirstVisibleMask)
                    {
                        bFirstVisibleMask = true;
                        return;
                    }
                }
                if (IsHaveBossBlood(mas))
                {
                    if (!BossBloodEntityTable.ContainsKey(ev.ID))
                    {
                        BossBloodEntityTable.Add(ev.ID, ev);
                        BossBloodProityList.Add(ev);
                        SortBossBloodProityList();
                    }
                }
                if (mas != 0)
                {
                    SetVisible(ev.ID, (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_BLOOD) != (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_BLOOD);
                }
                else
                {
                    SetVisible(ev.ID,true);
                }
            }
        }
    }

    private static void SortBossBloodProityList()
    {
        if (BossBloodProityList.Count > 1)
        {
            BossBloodProityList.Sort((EntityView lef, EntityView rig) =>
            {

                ////只看优先级
                //return lef.BossBloodShowPriority.CompareTo(rig.BossBloodShowPriority);


                //先看优先级，然后再看距离
                if (lef.BossBloodShowPriority == rig.BossBloodShowPriority)
                {
                    return lef.BossBloodShowDistance.CompareTo(rig.BossBloodShowDistance);

                }
                return lef.BossBloodShowPriority.CompareTo(rig.BossBloodShowPriority);


                ////距离相当，则按照优先级排序
                //if (lef.BossBloodShowDistance == rig.BossBloodShowDistance)
                //{
                //    if (lef.BossBloodShowPriority == rig.BossBloodShowPriority)
                //    {
                //        return 0;
                //    }
                //    if (lef.BossBloodShowPriority > rig.BossBloodShowPriority)
                //    {
                //        return 1;
                //    }
                //    if (lef.BossBloodShowPriority < rig.BossBloodShowPriority)
                //    {
                //        return -1;
                //    }
                //}
                //return lef.BossBloodShowDistance.CompareTo(rig.BossBloodShowDistance);
            });
        }

    }

    /// <summary>
    /// 清除所有的组件
    /// </summary>
    public static void DestroyALL()
    {
        if (Movie.GetSingleton<GfxBloodMovie>() == null)
        {
            return;
        }
        ClearCache();
        if (null != m_BloodMap)
        {
            foreach (KeyValuePair<int, GfxBlood> var in m_BloodMap)
            {
                GfxBlood gba = var.Value;
                if (GfxBloodMovie.Instance != null)
                {
                    GfxBloodMovie.Instance.RemoveMovieAS3(gba.Instance);
                }
                gba.Clear();
            }
            CurrentColIndex = 0;
            CurrentRowIndex = 0;
            UnUsedMovieNodeList.Clear();
            m_BloodMap.Clear();
        }
        if (null != BossBlood)
        {
            if (GfxDynamicMovie.Instance != null)
            {
                GfxDynamicMovie.Instance.RemoveMovieAS3(BossBlood.Instance);
            }
            BossBlood.Clear();
            BossBlood = null;
            CurrentBossBlodHostID = -1;
        }
        BossBloodEntityTable.Clear();
        BossBloodProityList.Clear();
        //GFxWarPauseMenu.BoolEvent -= ChangeBloodSetting;
    }


    /// <summary>
    /// 摧毁指定的组件
    /// </summary>
    /// <param name="instance">需要清除的Blood</param>
    public static void Destroy(GfxBlood instance)
    {
        if (Movie.GetSingleton<GfxBloodMovie>() == null)
        {
            return;
        }
        if (null == instance)
        {
            return;
        }
        int id = instance.ID;
        if (instance.movieNode.isValid)
        {
            UnUsedMovieNodeList.Enqueue(instance.movieNode);
        }

        if (GfxBloodMovie.Instance != null)
        {
            GfxBloodMovie.Instance.RemoveMovieAS3(instance.Instance);
        }
        instance.Clear();

        if (m_BloodMap.ContainsKey(id))
        {
            m_BloodMap[id] = null;
            m_BloodMap.Remove(id);
        }
        instance = null;


    }

    /// <summary>
    /// 设置血条的最大值
    /// </summary>
    /// <param name="entityID">ID</param>
    /// <param name="v">值</param>
    public static void SetMaxValue(int entityID, int v)
    {

        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetMaxValue(v);
        }
    }


    /// <summary>
    /// 设置血条的当前值
    /// </summary>
    /// <param name="entityID">ID</param>
    /// <param name="v">值</param>
    public static void SetCurValue(int entityID, int v)
    {
        if (v < 0)
        {
            Trace.LogWarning("ID:" + entityID + "，的当前血量小于0！请注意！！，值为：" + v);
            return;
        }
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetCurValue(v);
        }
    }

    public static void SetCamType(int entityID, int cam)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetCamType(cam);
        }
    }

    /// <summary>
    /// 设置临时血量
    /// </summary>
    /// <param name="entityID"></param>
    /// <param name="cam"></param>
    public static void SetTempValue(int entityID, int val)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetTemp(val);
        }
    }

    /// <summary>
    /// 设置血条的可见性
    /// </summary>
    /// <param name="entityID">ID</param>
    /// <param name="v">值</param>
    public static void SetVisible(int entityID, bool v)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetVisible(v);
        }
    }

    /// <summary>
    /// 设置血条上的等级
    /// </summary>
    /// <param name="entityID">ID</param>
    /// <param name="v">值</param>
    public static void SetLevel(int entityID, int v)
    {
        
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetLevel(v);
        }     
    }

    /// <summary>
    /// 设置血条上的数字信息
    /// </summary>
    /// <param name="entityID">ID</param>
    /// <param name="bloodInfo">值</param>
    public static void SetBloodInfo(int entityID, string bloodInfo)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            gba.SetBloodInfo(bloodInfo);
        }
    }

    public static GfxBlood FindBlood(int entityID)
    {
        if (m_BloodMap.ContainsKey(entityID))
        {
            GfxBlood gba = m_BloodMap[entityID];
            return gba;
        }
        return null;
    }

    /// <summary>
    /// 一般更新
    /// </summary>
    private static void NormalUpdate()
    {
        InvalidList.Clear();
        if(null != m_BloodMap)
        {
            foreach (GfxBlood var in m_BloodMap.Values)
            {
                GfxBlood gba = var;
                U3D_Render.EntityView view = EntityFactory.getEntityViewByID(gba.ID);
                if (view == null)
                {
                    InvalidList.Enqueue(gba);
                    continue;
                }
                if (!gba.Entry)
                {
                    InvalidList.Enqueue(gba);
                    continue;
                }

                if (!gba.Entry.activeSelf || !view.StateMachine.GetVisible())//实体没有被激活或者不可见
                {
                    if (gba.GetEnable())
                    {
                        gba.SetEnable(false);
                    }
                }
                else//正常的实体
                {
                    if (!gba.GetEnable())
                    {
                        gba.SetEnable(true);
                    }
                    gba.OnUpdate();
                }

            }
        }
        

        while (InvalidList.Count > 0)
        {
            //移除无效的Blood节点
            GfxBlood b = InvalidList.Dequeue();
            if(OptimitzedControlPlane.Instance.EnityCreateOptimize)
            {
                CacheBlood(b);
            }
            else
            {
                GfxBloodManager.Destroy(b);
            }
           
        }
        InvalidList.Clear();
    }

    /// <summary>
    /// 更新BOSS血条
    /// </summary>
    private static void UpdateBossBlood()
    {
        if (!EntityFactory.MainHero)
        {
            return;
        }


        JitterBossBlood();
        
        //检测是否有boss血条进入到显示范围
        for (int i = 0; i < BossBloodProityList.Count; i++)
        {
            EntityView gv = BossBloodProityList[i];
            if (null == gv)
            {
                continue;
            }
            if (!gv.gameObject)
            {
                continue;
            }
            if (gv.BossBloodShowDistance > 0)
            {
                float dis = Vector3.SqrMagnitude(EntityFactory.MainHero.transform.position - gv.gameObject.transform.position);
                if (dis < gv.BossBloodShowDistance * gv.BossBloodShowDistance)
                {
                    if (gv.CampFlag != SPELL.CampFlag.CampFlag_Friend &&
                        gv.CampFlag != SPELL.CampFlag.CampFlag_Self
                       )
                    {
                        EntityEnterBossBloodDistance(gv.ID);
                    }
                    
                    break;
                }
                else
                {
                    EntityLeaveBossBloodDistance(gv.ID);
                }
            }
        }

        if (null != BossBlood)
        {
            BossBlood.OnUpdate();
        }
    }

    #endregion

    #region 回调

    /// <summary>
    /// 实体死亡
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnEntityDead(int entityID, ENTITY_TYPE t)
    {
        if (Movie.GetSingleton<GfxBloodMovie>()==null)
        {
            return;
        }
        if (entityID == CurrentSelectedID ||
            entityID == EntityFactory.MainHeroID)
        {
            CurrentSelectedID = -1;
            ChangeBossBloodTarget(-1);
        }

        //怪物
        if (t == ENTITY_TYPE.TYPE_MONSTER)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                if (OptimitzedControlPlane.Instance.EnityCreateOptimize)
                {
                    CacheBlood(m_BloodMap[entityID]);
                }
                else
                {
                    Destroy(m_BloodMap[entityID]);
                }
               
            }

            if (BossBloodEntityTable.ContainsKey(entityID))
            {
                BossBloodProityList.Remove(BossBloodEntityTable[entityID]);
                BossBloodEntityTable.Remove(entityID);
            }

            EntityLeaveBossBloodDistance(entityID);
        }

        //玩家
        if (t == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                //如果是主角，特殊处理一下
                if (entityID == EntityFactory.MainHeroID)
                {
                    bPlayerDead = true;
                    m_BloodMap[entityID].SetPlayerVisible(false);

                }
                m_BloodMap[entityID].SetVisible(false);
            }
        }
    }


    /// <summary>
    /// 实体复活
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnEntityRelive(int entityID, ENTITY_TYPE t)
    {
        //玩家
        if (t == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                //如果是主角，特殊处理一下
                if (entityID == EntityFactory.MainHeroID )
                {
                    bPlayerDead = false;
                    if (bPlayerBloodVisible)
                    {
                        m_BloodMap[entityID].SetPlayerVisible(true);
                    }
                    return;
                }
                m_BloodMap[entityID].SetVisible(true);
            }
        }
    }

    /// <summary>
    /// 实体受到攻击
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnEntityHit(int SrcID, int DstID)
    {
        if (DstID < 0 || CurrentBossBlodHostID < 0 ||
            SrcID != EntityFactory.MainHeroID)
        {
            return;
        }

        if (DstID == CurrentBossBlodHostID)
        {
            bBossBloodJitter = true;
            JitterTimer = 0.0f;
        }

    }

    /// <summary>
    /// 选择目标
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnSelectTarget(TargetFrame.SelectTargetParam param)
    {
        CurrentSelectedID = param.nTargetId;
        EntityView ev = EntityFactory.getEntityViewByID(CurrentSelectedID);
        if (null == ev)
        {
            CurrentSelectedID = -1;
        }
        else
        {
            if (!param.bIngoreBossMark && !IsHaveBossBlood(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK)))
            {
                CurrentSelectedID = -1;
            }
        }

        //如果有选中的目标或者没有进入BOSS血条距离的实体，则切换
        if (CurrentSelectedID > 0 || CurrentEntityEnterBossBloodDistanceID < 0)
        {
            ChangeBossBloodTarget(CurrentSelectedID, param.bIngoreBossMark);
        }
           
    }

    /// <summary>
    /// 不选择目标
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnUnSelectTarget(int entityID)
    {
        CurrentSelectedID = -1;

        //如果有选中的目标或者没有进入BOSS血条距离的实体，则切换
        if (CurrentSelectedID > 0 || CurrentEntityEnterBossBloodDistanceID < 0)
        {
            ChangeBossBloodTarget(CurrentSelectedID);
        }
    }

    //public static void OnSceneLoadFinish(object sender, SceneLoadFinishEventArgs eventArgs)
    //{
    //    DestroyALL();
    //}


    //数值属性回调函数
    public static void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
    {
        
        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_HP:			// 血量
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
            case (int)ENTITY_PROPERTY.PROPERTY_CAMP:	    // 阵营
            case (int)ENTITY_PROPERTY.PROPERTY_SHIELD:		// 护盾（临时血量）
                {
                    int id = e.data.nEntityID;
                    EntityView ev = e.objev;    // EntityFactory.getEntityViewByID(id);
                    if (ev != null)
                    {
                        int nCamp = e.data.nOtherData;
                        int MaxHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                        int CurHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                        int TempHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);

                        string BloodInfo = CurHp + "/" + MaxHp;

                        SetCamType(id, nCamp);
                        //血量最大值为0，将会隐藏血条
                        SetMaxValue(id, MaxHp);
                        SetCurValue(id, CurHp);
                        SetTempValue(id, TempHp);
                        SetBloodInfo(id, BloodInfo);

                        UpdateBossBloodData(id, ev.Property.CreatureName, MaxHp, CurHp, TempHp);


                    }
                    //nParam = data.CurHp;
                    //Trace.LogError(string.Format("==================onCommand_SetBlood=================== : {0}", nParam));
                }
                break;

            case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:       // 等级变化
                {
                    SetLevel(e.data.nEntityID, e.data.nValue);
                }
                break;

            default:
                break;
        }

    }
    #endregion

    #region Movie管理
    /// <summary>
    /// 将swf的每个血条单位的宽
    /// </summary>
    public const int MovieCell_Width = 115;

    /// <summary>
    /// 将swf的每个血条单位的高
    /// </summary>
    public const int MovieCell_Height = 25;

    /// <summary>
    /// 最大行
    /// </summary>
    public const int MaxRowCount = (int)GfxBloodMovie.BloodMovieWidth / MovieCell_Width;

    /// <summary>
    /// 最大列
    /// </summary>
    public const int MaxColCount = (int)GfxBloodMovie.BloodMovieHeight  / MovieCell_Height;

    private static int CurrentRowIndex = 0;
    private static int CurrentColIndex = 0;

    /// <summary>
    /// 为血条分配一个位置，分配失败，则表示Movie已经满了。
    /// </summary>
    /// <param name="t"></param>
    /// <returns></returns>
    private static bool AllocBloodPos(ENTITY_TYPE t,ref MovieNode btr)
    {
        if (m_entiySizeMap.ContainsKey(t))
        {
            Vector2 BloodSize = m_entiySizeMap[t];
            btr.isValid = false;
            if (UnUsedMovieNodeList.Count > 0)
            {
                MovieNode bt = UnUsedMovieNodeList.Dequeue();
                btr.RowIndex = bt.RowIndex;
                btr.ColIndex = bt.ColIndex;

                btr.start.x = MovieCell_Width * bt.RowIndex;
                btr.start.y = MovieCell_Height * bt.ColIndex;

                btr.end.x = btr.start.x + BloodSize.x;
                btr.end.y = btr.start.y + BloodSize.y;
                btr.vSize = BloodSize;

                btr.isValid = true;
            }
            else
            {
                btr = new MovieNode();
                btr.start.x = MovieCell_Width * CurrentRowIndex;
                btr.start.y = MovieCell_Height * CurrentColIndex;

                btr.end.x = btr.start.x + BloodSize.x;
                btr.end.y = btr.start.y + BloodSize.y;
                btr.vSize = BloodSize;


                btr.RowIndex = CurrentRowIndex;
                btr.ColIndex = CurrentColIndex;

                CurrentRowIndex++;
                if (CurrentRowIndex >= MaxRowCount)
                {
                    CurrentColIndex++;
                    if (CurrentColIndex >= MaxColCount)
                    {
                        return false;
                    }
                    CurrentRowIndex = 0;
                }

                btr.isValid = true;
            }
            return true;
        }
       
        return false;
    }

    #endregion

    #region 静态属性
    private const string PlayerSelfBloodAS3RefrenceName = "Com.Scaleform.Blood.PlayerSelfBlood";
    private const string PlayerBloodAS3RefrenceName = "Com.Scaleform.Blood.PlayerBlood";
    private const string MonsterBloodAS3RefrenceName = "Com.Scaleform.Blood.MonsterBlood";
    private const string BuildingsBloodAS3RefrenceName = "Com.Scaleform.Blood.BuildingBlood";
    private const string BossBloodAS3RefrenceName = "Com.Scaleform.Blood.BossBlood";

    private static bool bInit = false;

    /// <summary>
    /// 第一次走跟visible相关的mask，就是创建时候的mask，屏蔽掉
    /// </summary>
    private static bool bFirstVisibleMask = false;

    /// <summary>
    /// 是否显示血量数字信息，第一次通过读取本地配置文件获取
    /// </summary>
    private static bool bBloodInfoVisible = false;

    /// <summary>
    /// 是否显示主角血量
    /// </summary>
    private static bool bPlayerBloodVisible = false;

    /// <summary>
    /// 主角是否处于死亡状态
    /// </summary>
    private static bool bPlayerDead = false;

    /// <summary>
    /// 实体类型和血条类型表
    /// </summary>
    private static Dictionary<ENTITY_TYPE, string> m_entiyTypeMap = new Dictionary<ENTITY_TYPE, string>();

    /// <summary>
    /// 实体类型和血条大小关系
    /// </summary>
    private static Dictionary<ENTITY_TYPE, Vector2> m_entiySizeMap = new Dictionary<ENTITY_TYPE, Vector2>();

    private static Dictionary<int, GfxBlood> m_BloodMap = new Dictionary<int, GfxBlood>();
    private static Queue<GfxBlood> InvalidList = new Queue<GfxBlood>();
    private static Queue<MovieNode> UnUsedMovieNodeList = new Queue<MovieNode>();

    #endregion

    #region 大血条

    #region 属性

    /// <summary>
    /// 拥有大血条的实体列表
    /// </summary>
    private static Dictionary<int, EntityView> BossBloodEntityTable = new Dictionary<int, EntityView>();

    /// <summary>
    /// 显示boss血条的优先级列表
    /// </summary>
    private static List<EntityView> BossBloodProityList = new List<EntityView>();

    /// <summary>
    /// 大血条
    /// </summary>
    private static GfxBlood BossBlood = null;

    /// <summary>
    /// Boss血条的位置
    /// </summary>
    private static Vector2 OrginalBossBloodPos = new Vector2(960, 120);

    /// <summary>
    /// 血条是否抖动
    /// </summary>
    private static bool bBossBloodJitter = false;

    /// <summary>
    /// 血条抖动X，最大-最小
    /// </summary>
    private static Vector2 BossBloodJitterX = new Vector2(10, -10);

    /// <summary>
    /// 血条抖动X，最大-最小
    /// </summary>
    private static Vector2 BossBloodJitterY = new Vector2(10, -10);

    /// <summary>
    /// 抖动时间
    /// </summary>
    private static float JitterDuration = 0.5f;

    /// <summary>
    /// 抖动计时器
    /// </summary>
    private static float JitterTimer = 0.0f;

    /// <summary>
    /// 当前Boss血条所对应的ID;
    /// </summary>
    private static int CurrentBossBlodHostID = -1;

    /// <summary>
    /// 当前进入Boss血条的实体ID
    /// </summary>
    private static int CurrentEntityEnterBossBloodDistanceID = -1;

    /// <summary>
    /// 当前选中的BOSS血条ID
    /// </summary>
    private static int CurrentSelectedID = -1;
    #endregion


    /// <summary>
    /// 抖动大血条
    /// </summary>
    private static void JitterBossBlood()
    {
        if (!bBossBloodJitter ||
            null == BossBlood)
        {
            return;
        }

        JitterTimer += Time.deltaTime;
        if (JitterTimer > JitterDuration)
        {
            bBossBloodJitter = false;
            JitterTimer = 0.0f;
            BossBlood.SetPositionDirectly_StagePoint(OrginalBossBloodPos);
        }

        Vector2 JitterPos;
        float MaxX = BossBloodJitterX.x * GfxUtil.ScreenScale.x;
        float MaxY = BossBloodJitterY.x * GfxUtil.ScreenScale.y;
        float MinX = BossBloodJitterX.y * GfxUtil.ScreenScale.x;
        float MinY = BossBloodJitterY.y * GfxUtil.ScreenScale.y;
        JitterPos.x = Random.Range(MinX, MaxX) + OrginalBossBloodPos.x;
        JitterPos.y = Random.Range(MinY, MaxY) + OrginalBossBloodPos.y;
        BossBlood.SetPositionDirectly_StagePoint(JitterPos);
    }

    /// <summary>
    /// 创建BOSS血条
    /// </summary>
    private static void CreateBossBlood()
    {
        if (null == BossBlood)
        {

            Value BossIns = new Value();
            if (!GfxDynamicMovie.Instance.AttachMovie(ref BossIns, BossBloodAS3RefrenceName, BossBloodAS3RefrenceName + "asd", -1))
            {
                Trace.LogError("Flash -- BossBlood创建失败！");
                BossIns = null;
            }

            BossBlood = new GfxBlood(BossIns);
            BossBlood.Init(-1, null, -1,ENTITY_TYPE.TYPE_MONSTER);
            BossBlood.SetMaxValue(100);
            BossBlood.SetCurValue(100);
            BossBlood.SetVisible(false);
            BossBlood.SetPositionDirectly_StagePoint(OrginalBossBloodPos);

        }
    }

    /// <summary>
    /// 更新BOSS血条的数据
    /// </summary>
    /// <param name="id"></param>
    /// <param name="name"></param>
    /// <param name="MaxValue"></param>
    /// <param name="CurValue"></param>
    /// <param name="TempValue"></param>
    private static void UpdateBossBloodData(int id,string name,int MaxValue, int CurValue, int TempValue)
    {
        if (null == BossBlood)
        {
            return;
        }
        if (id != CurrentBossBlodHostID || id < 0)
        {
            if (CurrentBossBlodHostID < 0)
            {
                BossBlood.SetVisible(false);
            }
            
            return;
        }


        BossBlood.SetMaxValue(MaxValue);
        BossBlood.SetCurValue(CurValue);
        BossBlood.SetTemp(TempValue);
        BossBlood.SetName(name);
    }

    private static bool IsHaveBossBlood(int mas)
    {
        return (mas & (int)ENTITY_MASKFLAG.MASKFLAG_SHOW_BIGBLOOD) == (int)ENTITY_MASKFLAG.MASKFLAG_SHOW_BIGBLOOD;
    }

    /// <summary>
    /// 改变BOSS血条显示的目标
    /// </summary>
    /// <param name="entityID">实体ID，小于0会隐藏Boss血条</param>
    private static void ChangeBossBloodTarget(int entityID, bool ingoreBossMark=false)
    {
        if (null == BossBlood)
        {
            return;
        }
        if (entityID < 0)
        {
            CurrentBossBlodHostID = -1;
            BossBlood.SetVisible(false);
            return;
        }
        EntityView objv = EntityFactory.getEntityViewByID(entityID);
        if (null == objv)
        {
            BossBlood.SetVisible(false);
            CurrentBossBlodHostID = -1;
        }
        else
        {
            int mas = objv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
            if (0 == mas && !ingoreBossMark)
            {
                return;
            }
            //如果不需要显示，则返回
            if (!IsHaveBossBlood(mas))
            {
                return;
            }
            bool bHaveShowBossBlood = ingoreBossMark || IsHaveBossBlood(mas);
            if (!bHaveShowBossBlood)
            {
                BossBlood.SetVisible(false);
                CurrentBossBlodHostID = -1;
                return;
            }

            BossBlood.SetVisible(objv.StateMachine.GetVisible());
            CurrentBossBlodHostID = entityID;

            int MaxHp = objv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
            int CurHp = objv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
            int TempHp = objv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);
            UpdateBossBloodData(entityID, objv.Property.CreatureName, MaxHp, CurHp, TempHp);

        }
    }

    private static void EntityEnterBossBloodDistance(int entityID)
    {
        if (CurrentEntityEnterBossBloodDistanceID == entityID &&
            CurrentSelectedID > 0)
        {
            return;
        }
        CurrentEntityEnterBossBloodDistanceID = entityID;
        ChangeBossBloodTarget(CurrentEntityEnterBossBloodDistanceID);
    }

    private static void EntityLeaveBossBloodDistance(int entityID)
    {
        //有选中目标的话，优先选中的目标
        if (CurrentSelectedID > 0)
        {
            return;
        }
        
        if (CurrentEntityEnterBossBloodDistanceID < 0)
        {
            return;
        }
        if (CurrentEntityEnterBossBloodDistanceID != entityID)
        {
            return;
        }
        CurrentEntityEnterBossBloodDistanceID = -1;
        ChangeBossBloodTarget(CurrentEntityEnterBossBloodDistanceID);
    }

    #endregion
}
