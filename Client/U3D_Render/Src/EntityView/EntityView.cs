/*******************************************************************
** 文件名:	EntityView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-04-16
** 版  本:	1.0
** 描  述:	

		单个实体表现
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/30
** 描  述:  释放对象组件信息时增加皮肤缓存处理
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using GameLogic;
using SPELL;
using System.Threading;
using USpeedUI.Blood;

// 定义实体ID
using ENTITY_ID = System.Int32;
using U3D_Render.Utility;
namespace U3D_Render
{
    /// <summary>
    /// 单个实体表现
    /// </summary>
    public partial class EntityView 
    {

        #region Data Area
        /// <summary>
        /// 实体ID
        /// </summary>
        public ENTITY_ID ID
        {
            get { return m_id; }
            set { m_id = value; }
        }
        
        private ENTITY_ID m_id = 0;

        /// <summary>
        /// 实体类型
        /// </summary>
        public ENTITY_TYPE Type
        {
            get { return m_type; }
            set { m_type = value; }
        }
        private ENTITY_TYPE m_type = ENTITY_TYPE.TYPE_ENTITY;

        /// <summary>
        /// 是否为主角
        /// </summary>
        public bool IsHero
        {
            get { return m_isHero; }
            set { m_isHero = value; }
        }
        private bool m_isHero = false;      // 是否为主角

        /// <summary>
        /// 是否FPS英雄
        /// </summary>
        public bool IsFPS
        {
            get
            {
                if (Property)
                {
                    return Property.FPSMode;
                }
                return false;
            }
        }

        /// <summary>
        /// 阵营标识
        /// </summary>
        public CampFlag CampFlag
        {
            get { return m_campFlag; }
            set { m_campFlag = value; }
        }
        private CampFlag m_campFlag;

        /// <summary>
        /// 实体标记 ENTITY_MASKFLAG，实体初始化时的mask，实时更新的mask，请从属性中取
        /// </summary>
        public int InitMaskFlag
        {
            get { return m_InitmaskFlag; }
            set { m_InitmaskFlag = value; }
        }

        private int m_InitmaskFlag = 0;

        /// <summary>
        /// 用于标注ev的一些附加信息
        /// </summary>
        public int Flag
        {
            get { return m_nFlag; }
        }
        private int m_nFlag = 0;

        public int Sight
        {
            get { return m_nSight; }
        }
        private int m_nSight = 0;
        ///// <summary>
        ///// 实体项目创建临时数据,初始化创建完将释放对象
        ///// </summary>
        //public EntityCreateItemData CreateItemData
        //{
        //    get { return m_createItemData; }
        //    set { m_createItemData = value; }
        //}
        //public EntityCreateItemData m_createItemData = new EntityCreateItemData();
        public EntityViewItem createinfo; 

        /// <summary>
        /// boss血条显示优先级
        /// </int>
        public int BossBloodShowPriority
        {
            get { return m_BossBloodShowPriority; }
        }
        private int m_BossBloodShowPriority = 0;

        /// <summary>
        /// boss血条显示距离
        /// </summary>
        public int BossBloodShowDistance
        {
            get { return m_BossBloodShowDistance; }
        }
        private int m_BossBloodShowDistance = 0;

        /// <summary>
        /// 是否有效，绑定GameObject后才能生效
        /// </summary>
        public bool IsValid
        {
            get { return m_isValid; }
        }
        private bool m_isValid = false;

        /// <summary>
        /// 状态机对象
        /// </summary>
        public BaseStateMachine StateMachine
        {
            get
            {
                return m_stateMachine;
            }
        }
        private BaseStateMachine m_stateMachine = null;

        /// <summary>
        /// 属性对象
        /// </summary>
        public CreatureProperty Property
        {
            get
            {
                return m_property;
            }
        }
        // 属性对象
        private CreatureProperty m_property = null;

        /// <summary>
        /// 属性对象
        /// </summary>
        public LightingEffectManager LightingEffectManager
        {
            get
            {
                if (m_LightEffectManager == null)
                {
                    m_LightEffectManager = m_gameObject.GetComponent<LightingEffectManager>();
                }
                return m_LightEffectManager;
            }
        }
        // 属性对象
        private LightingEffectManager m_LightEffectManager = null;

        /// <summary>
        /// 属性对象
        /// </summary>
        public SkillCtrl SkillCtrl
        {
            get
            {
                if (m_SkillCtrl == null)
                {
                    m_SkillCtrl = m_gameObject.GetComponent<SkillCtrl>();
                }
                return m_SkillCtrl;
            }
        }
        // 属性对象
        private SkillCtrl m_SkillCtrl = null;

        /// <summary>
        /// 实体实体对应的游戏对象
        /// </summary>
        public GameObject gameObject
        {
            get { return m_gameObject; }
        }
        private GameObject m_gameObject = null;

        /// <summary>
        /// 实体实体对应的Transform
        /// </summary>
        public Transform transform
        {
            get
            {
                if (!m_transform)
                {
                    if (gameObject)
                    {
                        m_transform = gameObject.transform;
                    }
                }

                return m_transform;
            }

        }
        private Transform m_transform = null;

        /// <summary>
        /// 皮肤控制器
        /// </summary>
        public SkinCtrl.EntitySkinControl SkinControl
        {
            get
            {
                return skinControl;
            }
        }
        /// <summary>
        /// 皮肤控制
        /// </summary>
        private SkinCtrl.EntitySkinControl skinControl;


        public cmd_creature_rigidbody_sync data;
        public Queue<EntityViewCommand> commands = new Queue<EntityViewCommand>();
        public float alpha = 1.0f;

        /// <summary>
        /// 动态添加的组件，用于后面做删除
        /// </summary>
        public LinkedList<MonoBehaviour> m_DynamicCommopnent = new LinkedList<MonoBehaviour>();
        #endregion

        #region 构造函数和析构函数
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="id">实体ID</param>
        /// <param name="type">实体类型</param>
        public EntityView(ENTITY_ID id, ENTITY_TYPE type)
        {
            //Trace.LogWarning("EntityView::EntityView() id=" + m_id.ToString() + ", ThreadID=" + Thread.CurrentThread.ManagedThreadId);
            m_id = id;
            m_type = type;
        }


        public virtual void Destroy()
        {
            Restore();
            DestoryCompentObject();
        }

        /// <summary>
        /// 释放对象所绑定的组件信息
        /// </summary>
        private void DestoryCompentObject()
        {
            // 删除游戏对象
            if (m_gameObject)
            {

                PrefabManager.CachePrefab(m_type, m_gameObject);
            }
            m_transform = null;
            m_isValid = false;
            m_gameObject = null;
        }

        /// <summary>
        /// 删除动态添加的脚本
        /// </summary>
        private void DestroyDynamicCommopnent()
        {
            if(m_DynamicCommopnent.Count > 0)
            {
                foreach(MonoBehaviour mb in m_DynamicCommopnent)
                {
                    var temp = mb;
                    ResNode.DestroyRes(ref temp);
                }
                m_DynamicCommopnent.Clear();
            }
        }
        /// <summary>
        /// 重置
        /// </summary>
        public void Restore()
        {
            CacheSkin();
            //要在他们之前清理，因为动态脚本是依赖静态脚本的
            DestroyDynamicCommopnent();
            if (skinControl)
            {
                skinControl.Clear();
                skinControl = null;
            }
            if (m_property)
            {
                m_property.Restore();
                m_property = null;
            }

            if (m_stateMachine)
            {
                m_stateMachine.Clear();
                m_stateMachine.ResetAllMembers();
                m_stateMachine = null;
            }
            if (m_LightEffectManager)
            {
                m_LightEffectManager.Clear();
                m_LightEffectManager.Restore();
                m_LightEffectManager = null;
            }
            m_SkillCtrl = null;
            UBloodManager.Instance.DestroyBlood(ID);
            commands.Clear();
            //commands = null;
        }

        /// <summary>
        /// 缓存皮肤
        /// </summary>
        public void CacheSkin()
        {
            if (m_gameObject)
            {
                //if (m_type == ENTITY_TYPE.TYPE_MONSTER)
                {

                    m_stateMachine.SetVisible(true);
                    skinControl.CollectAllSkin();
                }
            }
        }
        #endregion


        #region Method Area
        /// <summary>
        /// 设置无效
        /// </summary>
        public void SetInvalid()
        {
            m_isValid = false;
        }

        /// <summary>
        /// 设置游戏对象
        /// </summary>
        /// <param name="gameObject">游戏对象</param>
        public void SetGameObject(GameObject gameObject)
        {
            if (m_gameObject)
            {
                Debug.LogError("EntityView Error,gameobject is not null!" + m_gameObject.name + ",isValid:" + m_isValid + ",chidCout:" + m_gameObject.transform.childCount);
                ResNode.DestroyRes(ref m_gameObject);
            }

            m_gameObject = gameObject;
        }

        /// <summary>
        /// 初始化创建数据
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public bool InitBuildeData(EntityViewItem item)
        {
            if (m_gameObject == null)
            {
                Trace.LogError("没有加载游戏对象导致初始化创建数据失败！id=" + item.EntityID);
                return false;
            }
            m_stateMachine = m_gameObject.GetComponent<BaseStateMachine>();
            if (m_stateMachine == null)
            {
                Trace.LogError("没有加载实体状态机导致初始化创建数据失败！id=" + item.EntityID);
                return false;
            }
            m_stateMachine.Init();
            m_property = m_gameObject.GetComponent<CreatureProperty>();
            if (m_property == null)
            {
                Trace.LogError("没有加载生物属性导致初始化创建数据失败！id="+item.EntityID);
                return false;
            }

            // 是否为主角,1=主角
            IsHero = (item.byIsHero == 1);

            if (IsHero)
            {
                Property.PropertyMaxLimit = ENTITY_PROPERTY.PROPERTY_MAX;
            }

            if (!Property.Create())
            {
                return false;
            }
      
            // 阵营标识
            CampFlag = (CampFlag)item.byCampFlag;

            // 实体标识
            InitMaskFlag = item.nMaskFlag;

            // 附加数据,怪物时为怪物类型
            if (ENTITY_TYPE.TYPE_MONSTER == (ENTITY_TYPE)item.EntityType)
            {
                //m_nFlag = (item.ndata1 == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER ? 1 : 0);
                m_nFlag = item.ndata1;
            }

            m_nSight = item.nSight;

            m_BossBloodShowDistance = item.nBossBloodShowDistance;
            m_BossBloodShowPriority = item.nBossBloodPriority;

            // 绑定其他游戏组件对象
            if (!BindGameCompent())
            {
                return false;
            }

            // 设置属性所属主体对象
            Property.Master = this;            
            // 名字	
            Property.CreatureName = item.szName;
            // 模型路径	
            Property.ModelPath = item.szModelPath;

            // 英雄ID
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION, item.nHeroID);

            // 皮肤ID
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN, item.nSkinID);

            // 性别
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_SEX, item.bySexType);
            // 等级
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL, item.nLevel);
            // 阵营
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP, item.byCamp);

            // 标志位
            Property.SetNumProp(ENTITY_PROPERTY.PROPERTY_MASK, item.nMaskFlag);

            // 设置对象有效
            m_isValid = true;

            //Trace.Emphasize("初始化实体完成！ id=" + ID);
            return true;
        }


        /// <summary>
        /// 创建皮肤控制器
        /// </summary>
        private void BuildSkinControl()
        {
            // 不同实体类型加载
            switch (Type)
            {
                case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                    {
                        skinControl = m_gameObject.GetComponent<SkinCtrl.PersonSkinControl>();
                        skinControl.Init();
                    }
                    break;

                case ENTITY_TYPE.TYPE_MONSTER:
                    {
                        skinControl = m_gameObject.GetComponent<SkinCtrl.MonsterSkinControl>();
                        skinControl.Init();
                    }
                    break;

                case ENTITY_TYPE.TYPE_TANK:
                    {
                        skinControl = m_gameObject.GetComponent<SkinCtrl.TankSkinControl>();
                        skinControl.Init();
                    }
                    break;

                default:
                    break;
            }          
        }

        /// <summary>
        /// 绑定其他游戏组件对象
        /// </summary>
        /// <returns></returns>
        private bool BindGameCompent()
        {
            // 添加必要组件
            LightingEffectManager effectViewManager = m_gameObject.GetComponent<LightingEffectManager>();

            effectViewManager.Init();
            AttackedCollisionGroup attackedCollision = m_gameObject.GetComponent<AttackedCollisionGroup>();
            if (attackedCollision != null)
            {
                attackedCollision.nEntityID = ID;
            }

            // 初始化状态机数据
            StateMachine.entityID = ID;

            // 创建皮肤控制器
            BuildSkinControl();

            bool bRet = false;
            // 不同实体类型加载
            switch (Type)
            {
                case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                    {
                        if (IsHero)
                        {
                            bRet = BindHeroComponent();
                        }
                        else
                        {
                            bRet = BindPlayerComponent();
                        }
                    }
                    break;

                case ENTITY_TYPE.TYPE_MONSTER:
                    {
                        bRet = BindMonsterComponent();
                    }
                    break;

                case ENTITY_TYPE.TYPE_TANK:
                    {
                        bRet = BindTankComponent();
                    }
                    break;

                default:
                    {
                        Trace.Error("不能识别的类型,id=" + ID + ", type=" + (byte)Type);
                    }
                    break;
            }
            return bRet;
        }

        /// <summary>
        /// 绑定主角组件
        /// </summary>
        /// <returns></returns>
        private bool BindHeroComponent()
        {
            Trace.Log("BindHeroComponent() -> Hero, id="+ID.ToString());

            CreatureProperty property = this.Property;
            BaseStateMachine fsm = this.StateMachine;

            if (property == null || fsm == null)
            {
                Trace.Log("BindHeroComponent failed. CreatureProperty==null or BaseStateMachine == null !!!");
                return false;
            }

            m_gameObject.layer = LayerMask.NameToLayer(Config.LayerHero);
            fsm.baseLayer = LayerMask.NameToLayer(Config.LayerHero);
            fsm.pickableCount = 1;
            fsm.isNotAttack = false;
            MouseCtrl mousectrl = m_gameObject.GetComponent<MouseCtrl>();
            if (mousectrl == null)
            {
                mousectrl = m_gameObject.AddComponent<MouseCtrl>();
            }
            m_DynamicCommopnent.AddLast(mousectrl);
            SkillCtrl skillctrl = m_gameObject.GetComponent<SkillCtrl>();
            if (skillctrl == null)
            {
                skillctrl = m_gameObject.AddComponent<SkillCtrl>();
                skillctrl.resetOrderset(0);//property.SkillOrderSetName);
            }
            m_DynamicCommopnent.AddLast(skillctrl);
            ScreenRaycast raycast = m_gameObject.GetComponent<ScreenRaycast>();
            if (raycast == null)
            { 
                raycast = m_gameObject.AddComponent<ScreenRaycast>(); 
            }
            m_DynamicCommopnent.AddLast(raycast);

            fsm.isHero = true;
            fsm.canControl = true;

            SoldierController soliderController = m_gameObject.GetComponent<SoldierController>();
            if (soliderController == null) 
            { 
                soliderController = m_gameObject.AddComponent<SoldierController>();
            }
            m_DynamicCommopnent.AddLast(soliderController);

            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                OBController obController = m_gameObject.GetComponent<OBController>();
                if (obController == null)
                    obController = m_gameObject.AddComponent<OBController>();

                m_DynamicCommopnent.AddLast(obController);
            }

            fsm.maxRunForwardSpeed = property.runSpeed_Forward;
            fsm.maxRunBackwardSpeed = property.runSpeed_Back;
            fsm.maxRunSidewaySpeed = property.runSpeed_LeftRight;

            //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
            //if (screenCastLayer != null)
            //{
            //    screenCastLayer.setHero(true);
            //    GameObject.ResNode.DestroyRes(ref screenCastLayer);
            //}

            return true;
        }

        /// <summary>
        /// 绑定一般角色组件
        /// </summary>
        /// <returns></returns>
        private bool BindPlayerComponent()
        {
            //Trace.Log("BindPlayerComponent() -> Player, id=" + ID.ToString());

            CreatureProperty property = m_gameObject.GetComponent<CreatureProperty>();
            BaseStateMachine fsm = m_gameObject.GetComponent<BaseStateMachine>();

            if (property == null || fsm == null)
            {
                Trace.Log("entity_bindPlayerComponent failed. CreatureProperty==null or BaseStateMachine == null !!!");
                return false;
            }
            fsm.isNotAttack = false;
            if (0 != (InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_NOT_SELECT))
            {
                m_gameObject.layer = LayerMask.NameToLayer(Config.LayerNeutrality);
                fsm.baseLayer = LayerMask.NameToLayer(Config.LayerNeutrality);
                fsm.pickableCount = 1;
                //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                //if (screenCastLayer != null)
                //{
                //    screenCastLayer.setLayer(Config.LayerNeutrality);
                //}
            }
            else
            {
                switch (CampFlag)
                {
                case CampFlag.CampFlag_Friend:
                    {
                        m_gameObject.layer = LayerMask.NameToLayer(Config.LayerPlayerFriend);
                        fsm.baseLayer = LayerMask.NameToLayer(Config.LayerPlayerFriend);
                        fsm.pickableCount = 1;
                        //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                        //if (screenCastLayer != null)
                        //{
                        //    //screenCastLayer.setLayer(Config.LayerScreenRaycastFriend);
                        //    screenCastLayer.setLayer(Config.LayerPlayerFriend);
                        //}
                    }
                    break;

                case CampFlag.CampFlag_Enemy:
                    {
                        m_gameObject.layer = LayerMask.NameToLayer(Config.LayerPlayerEnemy);
                        fsm.baseLayer = LayerMask.NameToLayer(Config.LayerPlayerEnemy);
                        fsm.pickableCount = 1;
                        //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                        //if (screenCastLayer != null)
                        //{
                        //    //screenCastLayer.setLayer(Config.LayerScreenRaycastEnemy);
                        //    screenCastLayer.setLayer(Config.LayerPlayerEnemy);
                        //}
                    }
                    break;

                default:
                    break;
                }
            }

            fsm.maxRunForwardSpeed = property.runSpeed_Forward;
            fsm.maxRunBackwardSpeed = property.runSpeed_Back;
            fsm.maxRunSidewaySpeed = property.runSpeed_LeftRight;

            if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null && this.StateMachine != null)
            {
                if (EntityFactory.MainHeroView.StateMachine.PK_targetID == this.ID)
                {
                    this.StateMachine.setPK(true);
                }
                else
                {
                    this.StateMachine.setPK(false);
                }
            }

            return true;
        }

        /// <summary>
        /// 绑定MONSTER组件
        /// </summary>
        /// <returns></returns>
        private bool BindMonsterComponent()
        {
            CreatureProperty property = m_gameObject.GetComponent<CreatureProperty>();
            BaseStateMachine fsm = m_gameObject.GetComponent<BaseStateMachine>();         

            bool isWard = false;
            if (0!=(InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_AS_WARD))
            {
                isWard = true;
            }
            

            if (property == null || fsm == null)
            {
                Trace.Log("entity_bindMonsterComponent failed. CreatureProperty==null or BaseStateMachine == null !!!");
                return false;
            }


            fsm.isNotAttack = false;
            if (0 != (InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_NOT_SELECT) && !isWard)
            {
                m_gameObject.layer = LayerMask.NameToLayer(Config.LayerNeutrality);
                fsm.baseLayer = LayerMask.NameToLayer(Config.LayerNeutrality);
                fsm.pickableCount = 1;
                //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                //if (screenCastLayer != null)
                //{
                //    screenCastLayer.setLayer(Config.LayerNeutrality);
                //}
            }
            else if (0 != (InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_NOT_ATTACK))
            {
                m_gameObject.layer = LayerMask.NameToLayer(Config.LayerMonsterEnemy);
                fsm.baseLayer = LayerMask.NameToLayer(Config.LayerMonsterEnemy);
                fsm.pickableCount = 1;
                fsm.isNotAttack = true;
            }
            else
            {
                switch (CampFlag)
                {
                    case CampFlag.CampFlag_Friend:
                        {
                            string layerName = Config.LayerMonsterFriend;
                            if (isWard)
                            {
                                layerName = Config.LayerWard_Friend;
                            }

                            m_gameObject.layer = LayerMask.NameToLayer(layerName);
                            fsm.baseLayer = LayerMask.NameToLayer(layerName);
                            fsm.pickableCount = 1;
                            //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                            //if (screenCastLayer != null)
                            //{
                            //    //screenCastLayer.setLayer(Config.LayerScreenRaycastFriend);
                            //    screenCastLayer.setLayer(layerName);
                            //}
                        }
                        break;

                    case CampFlag.CampFlag_Enemy:
                        {
                            string layerName = Config.LayerMonsterEnemy;
                            if (isWard)
                            {
                                layerName = Config.LayerWard_Enemy;
                            }

                            m_gameObject.layer = LayerMask.NameToLayer(layerName);
                            fsm.baseLayer = LayerMask.NameToLayer(layerName);
                            fsm.pickableCount = 1;
                            //ScreenCastLayer screenCastLayer = m_gameObject.GetComponent<ScreenCastLayer>();
                            //if (screenCastLayer != null)
                            //{
                            //    //screenCastLayer.setLayer(Config.LayerScreenRaycastEnemy);
                            //    screenCastLayer.setLayer(layerName);
                            //}
                        }
                        break;

                    default:
                        break;
                }
            }
            return true;
        }

        /// <summary>
        /// 绑定TANK组件
        /// </summary>
        /// <returns></returns>
        private bool BindTankComponent()
        {
            return true;
        }


        public T GetComponent<T>() where T : UnityEngine.Component
        {
            return m_gameObject.GetComponent<T>();
        }

        public T AddComponent<T>() where T : UnityEngine.Component  
        {
            T comp = m_gameObject.GetComponent<T>();
            if (comp == null)
            {
                comp = m_gameObject.AddComponent<T>();
            }

            return comp;
        }

        #endregion

        #region OnCommand函数
        /// <summary>
        /// 数值属性变化处理
        /// </summary>
        /// <param name="data"></param>
        public bool OnCommand_UpdateNumProp(entity_update_Numproperty data)
        {
            if ( this.Property == null )
            {
                return false;
            }

            // 更新属性
            return this.Property.UpdateNumProp(data);
        }
        #endregion

        public static bool isNullOrEmpty(EntityView view)
        {
            if(null == view || !view.IsValid)
            {
                return true;
            }

            return false;
        }
    }
}
