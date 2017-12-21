using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System;

using GameLogic;
using U3D_Render;

// 定义实体ID
using ENTITY_ID = System.Int32;
using SPELL;
using DataCenter;

public struct EntityViewCommand
{
    public int cmdID;
    public int nParam;
    public string strParam;
    public IntPtr ptrParam;
    public int datalen;
    public byte[] databytes;
    public Type datatype;
}

public struct UIViewCommand
{
    public GameObject entity;
    public Int32 nEntityID; 
    public int cmdID;
    public int nParam;
    public string strParam;
    public IntPtr ptrParam;
    public int datalen;
    public byte[] databytes;
    public Type datatype;
}


/// <summary>
/// 实体管理工厂类
/// </summary>
public class EntityFactory : MonoBehaviour
{
    public int EntityNum = 0;
    // 人物实体数量
    public int ActorNum = 0;
    // 怪物实体数量
    public int MonsterNum = 0;
    // 主角英雄ID
    public static int MainHeroID = 0;

    // 主角英雄对象
    public static GameObject MainHero = null;

    // 主角英雄对象View
    public static EntityView MainHeroView = null;

    //距离外的人简化移动，直接setPosition
    public static float MovingSimplfyDis = 10.0f;

    // 实体容器
    public EntityContainer entityContainer
    { 
        get
        {
            return m_entityContainer;
        }
    }
    // 实体容器
    private EntityContainer m_entityContainer = new EntityContainer();

    private static List<int> m_NeedToLoadEntity = new List<int>(100);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static EntityFactory Instance = null;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //主英雄被创建事件
    public delegate void CreateMainHeroEntityEventHanlder(CreateMainHeroEntityEventArgs e);
    public static event CreateMainHeroEntityEventHanlder CreateMainHeroEntityEvent;

    public delegate void EntityNumPropChangeEventHanlder( EntityNumPropEventArgs e);
    protected static event EntityNumPropChangeEventHanlder EntityNumPropEvent;

    private Coroutine cor_UpdateEnumeratorOptimitzed = null;
    private Coroutine cor_CommunicationEnumerator = null;
    
    // 注册消息处理函数
    public static void SetEntityNumPropHandler(EntityNumPropChangeEventHanlder EntityNumProphandler)
    {
        //EntityNumPropEvent.GetInvocationList()  之后优化
        // 不要添加相同的函数
        EntityNumPropEvent -= EntityNumProphandler;//先移除，保证不重复订阅
        EntityNumPropEvent += EntityNumProphandler;
    }
    public static void RemoveEntityNumPropHandler(EntityNumPropChangeEventHanlder EntityNumProphandler)
    {
        EntityNumPropEvent -= EntityNumProphandler;
    }
    protected static void OnIprEvent(EntityNumPropEventArgs e)
    {
        if (EntityNumPropEvent != null)
        {
            EntityNumPropEvent(e);
        }
    }


    //响应消息
    public static void RaiseIprEvent(EntityNumPropEventArgs e)
    {
        OnIprEvent(e);
    }

    /// <summary>
    /// 
    /// </summary>
    public void Awake()
    {
        Instance = this;
        cor_UpdateEnumeratorOptimitzed = StartCoroutine(UpdateEnumeratorOptimitzed());

        cor_CommunicationEnumerator = StartCoroutine(CommunicationEnumerator());

        //注册Mask函数
        SetEntityNumPropHandler(ResponseEntityMaskPropEvent);

        dynamicObstacles = new Dictionary<uint, GameObject>();
        m_friendPlayerList = new List<uint>();
        m_friendPlayerListIndex = 0;
    }

    public void OnDestroy()
    {

        if (cor_UpdateEnumeratorOptimitzed != null)
        {
            StopCoroutine(cor_UpdateEnumeratorOptimitzed);
        }
            
        
        if (GloabaleManager)
        {
            GloabaleManager.Restore();
        }
        m_NeedToLoadEntity.Clear();
        if (cor_CommunicationEnumerator != null)
        {
            StopCoroutine(cor_CommunicationEnumerator);
        }
        MainHero = null;
        MainHeroView = null;
        MainHeroID = 0;
    }


    #region 属性回调
    //响应Mask消息
    public void ResponseEntityMaskPropEvent(EntityNumPropEventArgs e)
    {
        if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_MASK)
        {
            U3D_Render.EntityView ev = e.objev;
            if (ev != null)
            {
                //GfxBloodManager.ChangeMask(ev);
                USpeedUI.Blood.UBloodManager.Instance.ChangeMask(ev);
                //TopNameManager.ChangeMask(ev);
            }
        }
        //处理死亡
        if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_HP)
        {
            if (null == e.objev)
                return;
            int CurHp = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
            if (CurHp <= 0)
            {
                //e.objev.StateMachine.ChangeState((int)EntityState.Dead, IntPtr.Zero);
            }
        }
    }
    #endregion

    #region 查找实体相关函数

    // 获取实体列表
    public static ICollection<EntityView> GetEntityList()
    {
        return EntityFactory.Instance.m_entityContainer.Entities;
    }

    public static ICollection<EntityView> GetMonsterList()
    {
        return EntityFactory.Instance.m_entityContainer.Monsters;
    }

    // 获取主角英雄列表
    public static ICollection<EntityView> GetHeroList()
    {
        List<EntityView> evs = new List<EntityView>();
        foreach (EntityView ev in EntityFactory.Instance.m_entityContainer.Players)
        {
            if (ev != null && ev.IsHero)
            {
                evs.Add(ev);
            }
        }
        return evs;
    }

    // 获取玩家列表
    public static ICollection<EntityView> GetPlayerList()
    {
        return EntityFactory.Instance.m_entityContainer.Players;
    }

    /// <summary>
    /// 获取实体ID对应的实体对象
    /// </summary>
    /// <param name="id">实体ID</param>
    /// <returns></returns>
    public static GameObject getEntityGameObjectByID(ENTITY_ID id)
    {
        EntityView objev = EntityFactory.Instance.m_entityContainer.Get(id);
        return (objev != null ? objev.gameObject : null);
    }

    /// <summary>
    /// 获取实体ID对应的实体视图
    /// </summary>
    /// <param name="id">实体ID</param>
    static public EntityView getEntityViewByID(ENTITY_ID id)
    {
        return EntityFactory.Instance.m_entityContainer.Get(id);
    }

    /// <summary>
    /// 获取人物实体ID对应的实体视图,用本函数搜索速度会更快
    /// </summary>
    /// <param name="id">人物实体ID</param>
    static public EntityView getPlayerViewByID(ENTITY_ID id)
    {
        return EntityFactory.Instance.m_entityContainer.Get(id, ENTITY_TYPE.TYPE_PLAYER_ROLE);
    }

    /// <summary>
    /// 获取怪物实体ID对应的实体视图,用本函数搜索速度会更快
    /// </summary>
    /// <param name="id">怪物实体ID</param>
    static public EntityView getMonsterViewByID(ENTITY_ID id)
    {
        return EntityFactory.Instance.m_entityContainer.Get(id, ENTITY_TYPE.TYPE_MONSTER);
    }

    /// <summary>
    /// 是否存在此EntityView对象
    /// </summary>
    static public bool IsExists(EntityView ev)
    {
        return EntityFactory.Instance.m_entityContainer.IsExists(ev);
    }
    #endregion

    #region 实体创建清除

    private static GameObject entry_loadEntry(EntityView objev)
    {
        ENTITY_ID id = objev.ID;

        //已经被卸载了.
        if (EntityFactory.Instance.m_entityContainer.Get(id) == null)
        {
            return null;
        }
        EntityViewItem evItem = objev.createinfo;

        ENTITY_TYPE entityType = (ENTITY_TYPE)evItem.EntityType;
        UnityEngine.Object objPrefab = PrefabManager.GetPrefab(entityType);
        if (null == objPrefab)
        {
            Trace.LogError("找不到对应的类型的prefab，请检查PrefabManager中的Init函数，是否忘记加载？ " + entityType.ToString());
            return null;
        }
        GameObject entity = null;
        entity = objPrefab as GameObject;

        if (entity == null)
        {
            Trace.LogError("实例预设体对象失败！ " + entityType.ToString());
            return null;
        }

        entity.name += "-" + id.ToString();
        if (entity.transform.childCount > 0)
        {
            Trace.LogWarning("EntityView GameObject 有子节点！" + entity.transform.GetChild(0).name);
        }

        // 设置游戏对象
        objev.SetGameObject(entity);

        // 设置创建数据
        if (!objev.InitBuildeData(evItem))
        {
            Trace.LogError("初始化实体对象数据失败！ id=" + id.ToString());
            return null;
        }


        //如果是队友，则加入队友列表，
        if (objev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE && objev.CampFlag == CampFlag.CampFlag_Friend)
        {
            if (!m_friendPlayerList.Contains((uint)objev.ID))
            {
                m_friendPlayerList.Add((uint)objev.ID);
            }

        }

        //string entityname = "UnKnow";
        //Skin sk = SkinManager.GetSkin(objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN));
        //if (sk != null)
        //{
        //    entityname = sk.ResSkinObj.AssetName;
        //}
        //entity.name = entityname;
        //entity.name += "(entity" + evItem.EntityID.ToString() + ")";

        if (id == MainHeroID)
        {
            entity.transform.parent = null;
            MainHeroView = objev;
            
            if (CreateMainHeroEntityEvent != null)
            {
                CreateMainHeroEntityEventArgs e = new CreateMainHeroEntityEventArgs();
                e.MainHeroID = evItem.nHeroID;
                e.MainHeroUID = MainHeroID;
				e.nMatchTypeID = GameLogicAPI.getCurRoomMatchType();
                CreateMainHeroEntityEvent(e);
                LogicDataCenter.playerSystemDataManager.Reset();
            }

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MASTER_VIEW_LOADED);
            Trace.Log("Load Hero Entry:"+entity.name);
            
        }
        else
        {
            entity.transform.parent = Instance.transform;
        }

        objPrefab = null;
        if(ENTITY_TYPE.TYPE_PLAYER_ROLE == entityType)
        {
            // 发送人物加载完指令到逻辑层
            EntityEventHelper.Instance.SendCommand(id, EntityLogicDef.ENTITY_CMD_LOAD_COMPLETED);
        }

        BaseStateMachine bs = entity.GetComponent<BaseStateMachine>();

        //已经有位置信息，创建模型时立即同步瞬移过去，之后的同步消息是走过去
        if (objev.data.nActorID == evItem.EntityID)
        {
            Vector3 pos;
            pos.x = objev.data.fPosition_x;
            pos.y = objev.data.fPosition_y;
            pos.z = objev.data.fPosition_z;

            Vector3 rot;
            rot.x = objev.data.fRotation_x;
            rot.y = objev.data.fRotation_y;
            rot.z = objev.data.fRotation_z;

            //怪物要走传送，不能直接设置位置
            if (entityType == ENTITY_TYPE.TYPE_MONSTER)
            {
                if (bs)
                {
                    cmd_creature_transport data = new cmd_creature_transport();
                    data.fPosition_x = pos.x;
                    data.fPosition_y = pos.y;
                    data.fPosition_z = pos.z;

                    data.fRotation_x = rot.x;
                    data.fRotation_y = rot.y;
                    data.fRotation_z = rot.z;

                    data.bUseAngle = 1;
                    bs.Transport(data);
                }
            }
            else
            {
                entity.transform.SetPosition(pos);
                entity.transform.eulerAngles = rot;
            }
        }

        CheckEntityMaskToRangeSearch(objev);
        //执行延迟处理的消息
        EntityViewCommandHandler.onCommandsDelay(objev);
        return entity;
    }


    private static void entry_unloadEntry(EntityView ev)
    {
        GameObject obj = ev.gameObject;

        if (obj != null)
        {
            // 操作的主英雄，用于分块调度
            if (MainHero == obj)
            {
                Initialize.Instance.ResetGameManager();
                MainHero = null;
                MainHeroView = null;
                MainHeroID = 0;
            }
            
            ReleaseEntity(ev);

            
        }
    }


    public static void entry_syncEntry(ENTITY_ID id, cmd_creature_rigidbody_sync data)
    {
        EntityView objev = EntityFactory.Instance.m_entityContainer.Get(id);
        if (objev != null)
        {
            objev.data = data;
        }
    }

    //记录队友ID，死亡的时候跳转到队友视角
    private static List<uint> m_friendPlayerList;
    private static int m_friendPlayerListIndex;
    public static U3D_Render.EntityView getFriendPlayer(bool getNext)
    {
        if (getNext)
        {
            m_friendPlayerListIndex++;
        }
        for (int i = 0; i < m_friendPlayerList.Count;i++)
        {
            int index = (m_friendPlayerListIndex + i) % m_friendPlayerList.Count;
            uint id = m_friendPlayerList[index];
            U3D_Render.EntityView attachev = EntityFactory.getEntityViewByID((int)id);
            if (attachev != null && attachev.gameObject && attachev.gameObject.transform &&
                attachev.StateMachine != null && attachev.StateMachine.GetState() != EntityState.Dead && attachev.StateMachine.GetState() != EntityState.Floating)
            {
                m_friendPlayerListIndex = index;
                return attachev;
            }
        }

        //切到英雄自己
        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.gameObject != null)
        {
            m_friendPlayerListIndex = 0;
            return EntityFactory.MainHeroView;
        }
        return null;
    }

    private static int m_PlayerListIndex;


    // 获取主角阵营英雄列表（包含主角）
    public static U3D_Render.EntityView geNextPlayer(bool isNext = true)
    {
        if (m_friendPlayerList!=null && m_friendPlayerList.Count > 0)
        {
            if (isNext)
                m_PlayerListIndex++;
            else
                m_PlayerListIndex--;
            if (m_PlayerListIndex < 0)
                m_PlayerListIndex = m_friendPlayerList.Count;
            m_PlayerListIndex = m_PlayerListIndex % (m_friendPlayerList.Count+1);
            if (m_PlayerListIndex == 0)
                return MainHeroView;
            else
            {
                uint id = m_friendPlayerList[m_PlayerListIndex-1];
                U3D_Render.EntityView attachev = EntityFactory.getEntityViewByID((int)id);
                if (attachev != null && attachev.CampFlag == CampFlag.CampFlag_Friend && attachev.gameObject && attachev.gameObject.transform &&
                    attachev.StateMachine != null && attachev.StateMachine.GetState() != EntityState.Dead && attachev.StateMachine.GetState() != EntityState.Floating)
                {
                    return attachev;
                }
            }
        }
        return null;
    }
    public static U3D_Render.EntityView skipDeadFriendPlayer(uint skipID)
    {
        if (m_friendPlayerListIndex > -1 && m_friendPlayerListIndex < m_friendPlayerList.Count)
        {
            if (skipID == m_friendPlayerList[m_friendPlayerListIndex])
            {
                return getFriendPlayer(true);
            }
        }
        return null;
    }


    /// <summary>
    /// 创建实体
    /// </summary>
    /// <param name="id">实体ID</param>
    /// <param name="SkinModelInfo">模型信息</param>
    /// <returns></returns>
    public static ENTITY_ID entry_createEntry(EntityViewItem item)
    {
        if (item.EntityType <= (int)ENTITY_TYPE.TYPE_ENTITY || item.EntityType >= (int)ENTITY_TYPE.TYPE_MAX)
        {
            Trace.Error("创建实体时实体类型不合法！ type=" + item.EntityType);
            return 0;
        }


        //已经有了，不创建
        EntityView objev = EntityFactory.Instance.m_entityContainer.Get(item.EntityID);

        if (null != objev)
        {
            return item.EntityID;
        }

        objev = new EntityView((ENTITY_ID)item.EntityID, (ENTITY_TYPE)item.EntityType);
        if (LogicDataCenter.pkDataManager.BIsPK && item.EntityID == EntityFactory.MainHeroView.StateMachine.PK_targetID)
        {
            item.byCampFlag = (byte)CampFlag.CampFlag_Enemy;
        }
        objev.createinfo = item;

        bool isHero = item.byIsHero == 1;
        //主角不管
        if (!isHero)
        {
            m_NeedToLoadEntity.Add(item.EntityID);
        }
        
        EntityFactory.Instance.m_entityContainer.Add(objev);

        // 设置主角
        if ((ENTITY_TYPE)item.EntityType == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            EntityFactory.Instance.ActorNum++;

            if (isHero)
            {
                Trace.LogWarning("设置了主角ID, id=" + item.EntityID.ToString());
                MainHeroID = item.EntityID;
            }
        }
        else if ((ENTITY_TYPE)item.EntityType == ENTITY_TYPE.TYPE_MONSTER)
        {
            EntityFactory.Instance.MonsterNum++;
        }
        LogicDataCenter.warMinimapDataManager.AddObject(objev);
        LogicDataCenter.warOBDataManager.AddObject(objev);
        //Trace.Log(string.Format("==================entry_createEntry===================" + item.EntityID.ToString()));
        EntityFactory.Instance.EntityNum++;
        return item.EntityID;
    }



    /// <summary>
    /// 释放实体
    /// </summary>
    /// <param name="objev"></param>
    /// <returns></returns>
    public static bool ReleaseEntity(EntityView objev)
    {
        if (objev == null)
        {
            Trace.LogError("释放实体时找不到目标实体！");
            return false;
        }

        if (m_friendPlayerList.Count > 0)
        {
            if (m_friendPlayerListIndex >= 0 && m_friendPlayerListIndex < m_friendPlayerList.Count && objev.ID == m_friendPlayerList[m_friendPlayerListIndex])
            {
                if (SoldierCamera.MainInstance() != null && EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.gameObject != null)
                {
                    SoldierCamera.MainInstance<SoldierCamera>().SwitchLookAtSolider(EntityFactory.MainHeroView); //切回到主角
                }
            }

            if (m_friendPlayerList.Contains((uint)objev.ID))
            {
                m_friendPlayerList.Remove((uint)objev.ID);
            }
        }

        ENTITY_ID id = objev.ID;
        ENTITY_TYPE type = objev.Type;
        m_NeedToLoadEntity.Remove(id);
        EntityFactory.Instance.m_entityContainer.Remove(ref objev);
        
        bool isHero = objev.IsHero; 
        // 主角特殊处理一下
        if (isHero)
        {
            Initialize.Instance.ResetGameManager();
            MainHero = null;
            MainHeroView = null;
            MainHeroID = 0;
        }
        LogicDataCenter.warMinimapDataManager.RemoveObject(objev);
        LogicDataCenter.warOBDataManager.RemoveObject(objev);
        objev.Destroy();

        EntityFactory.Instance.EntityNum--;

        switch(type)
        {
            case ENTITY_TYPE.TYPE_PLAYER_ROLE:  { EntityFactory.Instance.ActorNum--; break; }
            case ENTITY_TYPE.TYPE_MONSTER:      { EntityFactory.Instance.MonsterNum--; break; }
            default: break;
        }

        objev = null;

        if (type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (isHero)
            {
                Trace.Log("释放主角实体对象, id=" + id.ToString() + ", type=" + type.ToString());
            }
            else
            {
                Trace.Log("释放实体对象, id=" + id.ToString() + ", type=" + type.ToString());
            }
        }

        return true;
    }


    /// <summary>
    /// 删除实体
    /// </summary>
    /// <param name="id">实体ID</param>
    /// <param name="bDestroyGo">是否删除GO</param>
    /// <returns></returns>
    public static bool entry_destroyEntry(ENTITY_ID id)
    {
        EntityView objev = EntityFactory.Instance.m_entityContainer.Get(id);
        if (objev == null)
        {
            return false;
        }

        // 设置实体为无效性
        objev.SetInvalid();

        ReleaseEntity(objev);

        return true;
    }


    public static bool entry_destroyAllEntity()
    {
        List<int> tmpList = new List<int>();
        foreach (EntityView item in EntityFactory.Instance.m_entityContainer.Entities)
        {
            tmpList.Add(item.ID);
        }

        foreach (int key in tmpList)
        {
            entry_destroyEntry(key);
        }
        return true;
    }
    #endregion




    /// <summary>
    /// 退出Unity时回收对象
    /// </summary>
    public void Clear()
    {
        if (Initialize.Instance.ForbidQuit)
            return;

        System.Collections.Generic.List<EntityView> list = new System.Collections.Generic.List<EntityView>();
        list.AddRange(EntityFactory.Instance.m_entityContainer.Entities);

        foreach (EntityView objev in list)
        {
            ReleaseEntity(objev);
        }

        EntityFactory.Instance.m_entityContainer.Clear();
        list.Clear();
    }

    /// <summary>
    /// 发送命令给实体
    /// </summary>
    /// <param name="entityID">实体ID</param>
    /// <param name="cmdID">命令ID,具体定义见(EntityViewDef.h)</param>
    /// <param name="nParam">整形参数</param>
    /// <param name="strParam">字符串参数</param>
    /// <param name="ptrParam">指针参数</param>
    /// <returns></returns>
    public static bool entry_sendCommand(ENTITY_ID entityID, int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen, int nTickCount)
    {
        //Trace.LogWarning("entry_sendCommand cmdID=" + cmdID); 
        return EntityViewCommandHandler.onCommand(entityID, cmdID, nParam, strParam, ptrParam, nPtrLen,nTickCount);
    }

    const float timeinterval = 0.03f;
    WaitForSeconds updateTimer = new WaitForSeconds(timeinterval);

    public void Update()
    {
        //F11，打开调试信息
        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.EntityViewDebugOpen))
        {
            bOpenDebugInfo = !bOpenDebugInfo;
        }
    }


    IEnumerator CommunicationEnumerator()
    {
        while (true)
        {
            if (MainHero == null)
            {
                yield return new WaitForSeconds(timeinterval);
                continue;
            }
            
            //每1/4秒同步数据到C++逻辑层
            EntityBatchCommunicator.Update();
            yield return new WaitForSeconds(0.25f);
        }
    }

    IEnumerator UpdateEnumeratorOptimitzed()
    {
        Camera mainCamera = Initialize.mainCam;
        while (true)//整个过程10秒时间作为一个处理区间
        {
            if (!mainCamera)
            {
                mainCamera = Initialize.mainCam;
            }
            //先加载主角
            if (MainHero == null)
            {
                if (MainHeroID != 0)
                {
                    EntityView evHero = EntityFactory.Instance.m_entityContainer.Get(MainHeroID);
                    if (evHero != null)
                    {
                        entry_loadEntry(evHero);
                        MainHero = evHero.gameObject;
                        //设置Hero给ImageEffect,做后处理区域控制
                        ImageEffects.ImageEffectManager.API_UpdatePlayer(SceneResInfoManager.CurrentActiveInstance.SceneCamera, EntityFactory.MainHero);
                        yield return updateTimer;
                        continue;
                    }
                }
            }
            if (m_NeedToLoadEntity.Count > 0)
            {
                entry_loadEntry(m_entityContainer.Get(m_NeedToLoadEntity[0]));
                m_NeedToLoadEntity.RemoveAt(0);
            }
            yield return updateTimer;
        }
    }

    public static GameObject EffectGroup;
    public static GameObject SkinCacheGroup;
    public static LightingEffectManager GloabaleManager = null;
    public static void Init()
    {
        EntityViewCommandHandler.Start();

        GameObject go = new GameObject("EntityFactory");
        DontDestroyOnLoad(go);
        go.AddComponent<EntityFactory>();


        EffectGroup = new GameObject("EffectGroup");
        GloabaleManager = EffectGroup.AddComponent<LightingEffectManager>(); //这里放全局光效
        GloabaleManager.Init();
        GloabaleManager.isGlobal = true;

        DontDestroyOnLoad(EffectGroup);


        SkinCacheGroup = new GameObject("SkinCacheGroup");
        DontDestroyOnLoad(SkinCacheGroup);


        Trace.Log("==================initialize EntityView Factory===================");
    }

    public bool checkPotision = false;

    [Range(0.0f, 1.0f)]
    public float apprfactor1 = 0.05f;

    [Range(0.0f, 1.0f)]
    public float apprfactor2 = 0.4f;

    [Range(-4.0f, 4.0f)]
    public float timeScaleLevel = 0.0f;

    [Range(0, 30)]
    public int vavyingFrame = 8;

    [Range(0, 2)]
    public int smoothlevel = 1;

    /// <summary>
    /// 是否打开了调试信息
    /// </summary>
    private static bool bOpenDebugInfo = false;
    public void OnGUIInterial()
    {
        if (checkPotision)
        {
            int n = transform.childCount;
            for (int i = 0; i < n; i++)
            {
                Transform t = transform.GetChild(i);
                BaseStateMachine bsm = t.GetComponent<BaseStateMachine>();
                if (bsm != null && bsm.checkPosition == true)
                {
                    bsm.OnGUIEx();
                }
            }
        }


        float timescale = Mathf.Pow(2.0f, timeScaleLevel);
        if (Mathf.Abs(timescale - Time.timeScale) > 0.01f)
        {
            Time.timeScale = timescale;
        }
    }

    public static EntityView getMainPlayer()
    {
        return MainHeroView;
    }

    private static void  CheckEntityMaskToRangeSearch(EntityView objev)
    {
        if ((objev.InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_SEARCH_SPECIAL) != (int)ENTITY_MASKFLAG.MASKFLAG_SEARCH_SPECIAL)
        {
            return;
        }

        EntityTimeSearchManager.Instance.AddEntityTimeSearch(objev);
    }

    public static void RefershPlayerCollider()
    {
        ICollection<U3D_Render.EntityView> list = EntityFactory.GetPlayerList();
        foreach (U3D_Render.EntityView ev in list)
        {
            if (U3D_Render.EntityView.isNullOrEmpty(ev))
            {
                continue;
            }
            CreatureStateMachine cs = ev.StateMachine as CreatureStateMachine;
            if (cs)
            {
                cs.RefershCharacterController();
            }
        }
    }

    #region 画动态障碍物

    private Dictionary<uint,GameObject> dynamicObstacles;
    public bool drawDynamicObstacle = true;
    public void DrawDynamicObstacle(cmd_DynamicObstacle_Info param)
    {
        
        if (!drawDynamicObstacle)
            return;
        if (dynamicObstacles.ContainsKey(param.uidTarget))
        {
            GameObject oldObject = dynamicObstacles[param.uidTarget];
            ResNode.DestroyRes(ref oldObject);
            dynamicObstacles.Remove(param.uidTarget);            
        }

        if (param.nIsAdd == 1)
        {
            GameObject newObstacle = new GameObject();
            newObstacle.name = "Obstacle" + param.uidTarget;
            switch (param.type)
            {
                case 1: //方体
                    float xWidth = (param.nDatas[3] - param.nDatas[0]) / 2;
                    float height = (param.nDatas[4] - param.nDatas[1]) / 2;
                    float zWidth = (param.nDatas[5] - param.nDatas[2]) / 2;
                    GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
                    cube.transform.parent = newObstacle.transform;
                    cube.transform.localScale = new Vector3(xWidth, height, zWidth);
                    cube.transform.position = new Vector3(
                        (param.nDatas[3] + param.nDatas[0]) / 2,
                        (param.nDatas[4] + param.nDatas[1]) / 2,
                        (param.nDatas[5] + param.nDatas[2]) / 2
                        );
                    cube.transform.eulerAngles = Vector3.up * param.angle;
                    cube.GetComponent<MeshRenderer>().enabled = false;

                    break;
                case 2: //圆柱体
                    GameObject cylinder = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
                    cylinder.transform.parent = newObstacle.transform;
                    cylinder.transform.localScale = new Vector3(param.nDatas[3], param.nDatas[4], param.nDatas[3]);
                    cylinder.transform.position = new Vector3(param.nDatas[0], param.nDatas[1], param.nDatas[2]);
                    cylinder.transform.rotation = Quaternion.identity;
                    cylinder.GetComponent<MeshRenderer>().enabled = false;

                    break;
            }
            //for (int i = 0; i < param.nPosVertexNum; i++)
            //{
            //    int nextIndex = i + 1;
            //    if (nextIndex >= param.nPosVertexNum) nextIndex = 0;
            //    Vector3 posa = new Vector3(param.nPosVertex[i * 3], param.nPosVertex[i * 3 + 1], param.nPosVertex[i * 3 + 2]);
            //    //Debug.Log("Vector:" + posa.ToString());
            //    GameObject newVector = GameObject.CreatePrimitive(PrimitiveType.Sphere);                
            //    Transform newVectorTransform = newVector.transform;
            //    GameObject.ResNode.DestroyRes(ref newVectorTransform.GetComponent<Collider>());
            //    newVectorTransform.parent = newObstacle.transform;
            //    newVectorTransform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
            //    newVectorTransform.SetPosition( posa);                

            //    Vector3 posb = new Vector3(param.nPosVertex[nextIndex * 3], param.nPosVertex[nextIndex * 3 + 1], param.nPosVertex[nextIndex * 3 + 2]);
            //    GameObject newEdge = GameObject.CreatePrimitive(PrimitiveType.Cube);                
            //    Transform newEdgeTransform = newEdge.transform;
            //    GameObject.ResNode.DestroyRes(ref newEdgeTransform.GetComponent<Collider>());
            //    newEdgeTransform.parent = newObstacle.transform;
            //    newEdgeTransform.localScale = new Vector3(0.1f, 5.0f, (posb - posa).magnitude);
            //    newEdgeTransform.SetPosition((posa + posb) / 2.0f);
            //    newEdgeTransform.LookAt(posb);                
            //}
            dynamicObstacles[param.uidTarget] = newObstacle;
        }
        else
        {
            if (dynamicObstacles.ContainsKey(param.uidTarget))
            {
                GameObject targetObj = dynamicObstacles[param.uidTarget];
                ResNode.DestroyRes(ref targetObj);
                dynamicObstacles.Remove(param.uidTarget);
            }
        }

    }
    #endregion
}
