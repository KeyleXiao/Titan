/*******************************************************************
** 文件名:	GuideManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-12-12
** 版  本:	2.0
** 描  述:	引导管理文件
            新手引导用行为树的思路执行各个引导节点。
            当前版本分两棵树处理:
            一、引导状态树(具体可看GuideStateConfig.csv)
                                        GameStateRoot(select)
                                  /            |                    \
              GameState_ESS_GAME(4)   GameState_ESS_MATCHROOM(6)   GameState_ESS_BATTLE(7)
                       |                       |                                |
                    UserID(0)               UserID(0)                         UserID(1)
                    /       \                                                 /       \
 PlayerNoviceGuideType(0)   PlayerNoviceGuideType(1)           GuideMatchTypeID(10)  GuideMatchTypeID(7) 
 二、动态引导树
    动态引导树由多个引导数据作为叶子节点组成，根据引导数据配置的父节点进行挂接。

引导状态->引导行为->引导逻辑

1、GuideNodeData 为单个引导的节点类，子节点Action按Sequence方式执行，Init方法里设置该引导的Precondition和各个Action节点。
2、由于项目较多数据都是单例，条件和执行节点基本都直接调用需要的单例来获取数据，所以有点弱化了Blackboard功能。
3、对于一些数据非单例情况，目前建议外部通过GuideManager.SetBBValue 设置一个变量存储，内部通过获取相应的变量用于条件、执行节点。
4、WIll-TODO：目前应该有个问题是执行最后一个引导后不会再更新，因为所有引导的条件都不满足。
********************************************************************/
using DataCenter;
using GameLogic;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using TsiU;
using UnityEngine;
using USpeedUI.Guide;

namespace GUIDE
{
    class GuideLogger : TILoggerListener
    {
        public void log(string msg)
        {
            UnityEngine.Debug.Log(msg);
        }
    };
    class GuideManager : Singleton<GuideManager>, IModelConfig,ITimerHanlder
    {
        protected static bool m_bInit;

        /// <summary>
        /// 需要关闭新手引导的不能直接改这里的值，
        /// 目前更改 SetWaitForMainPlayerCreate 方法内的值。
        /// </summary>
        public static bool enable = false;

        private static EGuideDebugType m_eGuideDebugType;
        public static EGuideDebugType GuideDebugType
        {
            set
            {
                //if (m_eGuideDebugType == value)
                //    return;

                m_eGuideDebugType = value;
                switch (value)
                {
                    default:
                    case EGuideDebugType.None:
                        {
                            foreach (var t in Enum.GetValues(typeof(TLogger.LOGGER_CHANNEL)))
                            {
                                TLogger.instance.EnableChannel((TLogger.LOGGER_CHANNEL)t, false);
                            }
                        }
                        break;
                    case EGuideDebugType.Debug:
                        {
                            TLogger.instance.EnableChannel(TLogger.LOGGER_CHANNEL.DEBUG, true);
                            TLogger.instance.EnableChannel(TLogger.LOGGER_CHANNEL.PROFILE, true);
                        }
                        break;
                    case EGuideDebugType.Profile:
                        {
                            TLogger.instance.EnableChannel(TLogger.LOGGER_CHANNEL.DEBUG, false);
                            TLogger.instance.EnableChannel(TLogger.LOGGER_CHANNEL.PROFILE, true);
                        }
                        break;
                }
            }
            get { return m_eGuideDebugType; }
        }

        public static void Init()
        {
            if (m_bInit)
                return;
            m_bInit = true;

            TLogger.instance.Init();
            TLogger.instance.AddLogListener(new GuideLogger());
            GuideDebugType = EGuideDebugType.None;

            instance.Begin();

        }

        public static void Update()
        {
            if (!m_bInit || !enable)
                return;

            instance.Tick();
        }

        public static void Destroy()
        {
            if (!m_bInit)
                return;
            m_bInit = false;

            TLogger.instance.Uninit();
            instance.End();
        }

        #region 初始化表
        /// <summary>
        /// 动态引导-行为节点引导配置数据
        /// </summary>
        private Dictionary<EGuideNodeID, SSchemeGuideNodeData> dicGuideConfigTable = new Dictionary<EGuideNodeID, SSchemeGuideNodeData>();
        /// <summary>
        /// 引导状态-配置数据
        /// </summary>
        private List<SSchemeGuideStateData> dicGuideStateConfigTable = null;
        /// <summary>
        /// 动态引导行为节点-引导类型
        /// </summary>
        private Dictionary<EGuideNodeID, Type> dicGuideNodeDataTypeTable = null;

        /// <summary>
        /// 引导状态根节点
        /// </summary>
        protected GuideState_Root m_GuideStateRoot = null;
        /// <summary>
        /// 动态引导根节点
        /// </summary>
        protected IGuideDynamicRoot m_GuideDynamicRoot = null;
        /// <summary>
        /// 引导数据
        /// </summary>
        private GuideWorkingData m_guideData;

        public delegate void UpdateNoviceGuideTypeEventHanlder(UpdateNoviceGuideTypeEventArgs e);
        /// <summary>
        /// 新手引导步骤更改事件代理
        /// </summary>
        public static event UpdateNoviceGuideTypeEventHanlder onUpdateNoviceGuideTypeEvent;
        #endregion

        /// <summary>
        /// 逻辑层下发新的引导步骤
        /// </summary>
        /// <param name="ptr"></param>
        /// <param name="nLen"></param>
        public void OnUpdateNoviceGuideType(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_entity_update_voviceguidedata cmd = helper.get<cmd_entity_update_voviceguidedata>();
            m_guideData.GuideStateBB.SetValue(EGuideStateBBKey.PlayerNoviceGuideType, cmd.guidedata);

            //分发引导步骤更改事件
            if(onUpdateNoviceGuideTypeEvent != null)
            {
                UpdateNoviceGuideTypeEventArgs eventArgs = new UpdateNoviceGuideTypeEventArgs(cmd.guidedata);
                onUpdateNoviceGuideTypeEvent(eventArgs);
            }         
        }
        /// <summary>
        /// 设置当前引导步骤
        /// </summary>
        /// <param name="_nStepID"></param>
        public void SetNoviceGuideType(int _nStepID)
        {
            //m_guideData.GuideStateBB.SetValue(EGuideStateBBKey.PlayerNoviceGuideType, _nStepID);

            cmd_entity_set_noviceguide msgInfo = new cmd_entity_set_noviceguide();
            msgInfo.nNoviceGuideType = _nStepID;

            EntityEventHelper.Instance.SendCommand<cmd_entity_set_noviceguide>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_ENTITY_SET_NOVICEGUIDE, ref msgInfo);
        }


        protected void Begin()
        {
            BindNodeIDAndData();
            InitGuideState();
            SetWaitForMainPlayerCreate();
        }

        protected void Tick()
        {
            if(m_GuideStateRoot != null)
            {
                m_guideData.GuideStateBB.SetValue(EGuideStateBBKey.GameStageState, StageManager.Instance.getCurrentState());
                m_guideData.GuideStateBB.SetValue(EGuideStateBBKey.GuideMatchTypeID, (int)GameLogicAPI.getCurRoomMatchType());


                if (m_GuideStateRoot.Evaluate(m_guideData))
                {
                    m_GuideStateRoot.Update(m_guideData);
                }
            }

            //SelectorGuideRoot();

            if(m_GuideDynamicRoot != null)
            {
                m_GuideDynamicRoot.Tick(m_guideData);
            }

            //Debug.Log("IsAllGuideFinished::" + IsAllGuideFinished());
        }

        protected void End()
        {
            dicGuideNodeDataTypeTable.Clear();
            dicGuideNodeDataTypeTable = null;
        }


        /// <summary>
        /// 绑定动态引导行为节点数据
        /// </summary>
        protected void BindNodeIDAndData()
        {
            dicGuideNodeDataTypeTable = new Dictionary<EGuideNodeID, Type>()
            {
                {EGuideNodeID.GuideNodeID_0,typeof( GSD_BasicOperation) },
                {EGuideNodeID.GuideNodeID_1,typeof( GSD_RotateCamera) },
                {EGuideNodeID.GuideNodeID_2,typeof( GSD_BasicOperation2) },
                {EGuideNodeID.GuideNodeID_3,typeof( GSD_AttackOperation) },
                {EGuideNodeID.GuideNodeID_4,typeof( GSD_TalentLevel3) },
                {EGuideNodeID.GuideNodeID_5,typeof( GSD_AttackOperation2) },
                {EGuideNodeID.GuideNodeID_6,typeof( GSD_TalentLevel6) },
                {EGuideNodeID.GuideNodeID_7,typeof( GSD_AttackEnemy) },
                {EGuideNodeID.GuideNodeID_8,typeof( GSD_AttackTower) },
                {EGuideNodeID.GuideNodeID_9,typeof( GSD_AttackBase) },

                {EGuideNodeID.GuideNodeID_1001,typeof(GSD_5v5_ChooseAWay) },
                {EGuideNodeID.GuideNodeID_1002,typeof(GSD_5v5_AttackTower1) },
                {EGuideNodeID.GuideNodeID_1003,typeof(GSD_5v5_AttackTower2) },
                {EGuideNodeID.GuideNodeID_1004,typeof(GSD_5v5_AttackTower3) },
                {EGuideNodeID.GuideNodeID_1005,typeof(GSD_5v5_AttackBase) },
                {EGuideNodeID.GuideNodeID_1006,typeof(GSD_5v5_LearnUseWing) },
                {EGuideNodeID.GuideNodeID_1007,typeof(GSD_5v5_MuchMoney) },
                {EGuideNodeID.GuideNodeID_1008,typeof(GSD_5v5_RotateCamera) },
                {EGuideNodeID.GuideNodeID_1009,typeof(GSD_ReturnMainScene) },

                {EGuideNodeID.GuideNodeID_2001,typeof(GSD_Static_BeginGuide) },
                {EGuideNodeID.GuideNodeID_2002,typeof(GSD_Static_LifeHeroEntrance) },
                {EGuideNodeID.GuideNodeID_2003,typeof(GSD_Static_LifeHeroCulture) },
                {EGuideNodeID.GuideNodeID_2004,typeof(GSD_Static_LisfHeroAllFind) },
                {EGuideNodeID.GuideNodeID_2005,typeof(GSD_Static_RankEntrance) },
                {EGuideNodeID.GuideNodeID_2006,typeof(GSD_Static_WaitForRank) },
                {EGuideNodeID.GuideNodeID_2007,typeof(GSD_Static_HeroReward) },
                {EGuideNodeID.GuideNodeID_2008,typeof(GSD_Static_FreeMatch) },

                {EGuideNodeID.GuideNodeID_3001,typeof(GSD_Static_SecondGuide_1) },
                {EGuideNodeID.GuideNodeID_3002,typeof(GSD_Static_SecondGuide_2) },
                {EGuideNodeID.GuideNodeID_3003,typeof(GSD_Static_SecondGuide_3) },
                {EGuideNodeID.GuideNodeID_3004,typeof(GSD_Static_SecondGuide_4) },
                {EGuideNodeID.GuideNodeID_3005,typeof(GSD_Static_SecondGuide_5) },
                {EGuideNodeID.GuideNodeID_3006,typeof(GSD_Static_SecondGuide_6) },
                {EGuideNodeID.GuideNodeID_3007,typeof(GSD_Static_SecondGuide_7) },

                {EGuideNodeID.GuideNodeID_9001,typeof(GSD_Tips_LowHP) },
                {EGuideNodeID.GuideNodeID_9002,typeof(GSD_Tips_LowMP) },
                {EGuideNodeID.GuideNodeID_AutoMessageFromLogic,typeof(NOD_ShowAutoGuideUIInfoParam) },
            };
        }
        /// <summary>
        /// 根据动态引导行为节点ID实例化对象
        /// </summary>
        /// <param name="_nNodeID"></param>
        /// <returns></returns>
        protected internal TBTAction getNodeDataInstance(EGuideNodeID _nNodeID)
        {
            if (dicGuideNodeDataTypeTable != null && dicGuideNodeDataTypeTable.ContainsKey(_nNodeID))
            {
                return dicGuideNodeDataTypeTable[_nNodeID].Assembly.CreateInstance(dicGuideNodeDataTypeTable[_nNodeID].ToString()) as TBTAction;
            }
            return null;
        }
        /// <summary>
        /// 根据动态引导行为节点ID取节点配置属性
        /// </summary>
        /// <param name="_nNodeID"></param>
        /// <returns></returns>
        protected internal SSchemeGuideNodeData getNodeConfigData(EGuideNodeID _nNodeID)
        {
            if (dicGuideConfigTable != null && dicGuideConfigTable.ContainsKey(_nNodeID))
            {
                return dicGuideConfigTable[_nNodeID];
            }
            return null;
        }


        /// <summary>
        /// 外部设置当前引导系统需要的数据
        /// </summary>
        public void SetBBValue(EGuideBBKey _eBBKey, object _oValue)
        {
            if (m_guideData!=null && m_guideData.GuideDynamicBB != null)
            {
                m_guideData.GuideDynamicBB.SetValue(_eBBKey, _oValue);
            }
        }

        public void Load(string strPath)
        {
            _LoadGuideNodeConfig(ViewConfigManager.GetConfigFullPath("GuideConfig.csv"));
            _LoadGuideStateConfig(ViewConfigManager.GetConfigFullPath("GuideStateConfig.csv"));
        }
        public void UnLoad()
        {
            if (dicGuideConfigTable != null)
            {
                dicGuideConfigTable.Clear();
                dicGuideConfigTable = null;
            }
        }
        protected void _LoadGuideNodeConfig(string _strPath)
        {
            dicGuideConfigTable = new Dictionary<EGuideNodeID, SSchemeGuideNodeData>();

            ScpReader reader = new ScpReader(_strPath, true);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeGuideNodeData sConfig = new SSchemeGuideNodeData();
                sConfig.nGuideID = reader.GetInt(i, (int)EGuideNodeConfig.GuideID, -1);

                sConfig.GuideStringParams = ScpReader.ToStringList(reader.GetString(i, (int)EGuideNodeConfig.GuideActionStringParams, ""), new char[] { ';' });
                sConfig.GuideIntParams = ScpReader.ConvertStringToIntArray(reader.GetString(i, (int)EGuideNodeConfig.GuideActionIntParams, ""), new char[] { ';' });
                //sConfig.GuidePreconditionIntParams = ScpReader.ConvertStringToIntArray(reader.GetString(i, (int)EGuideNodeConfig.GuidePreconditionIntParams, ""), new char[] { ';' });

                sConfig.nGuideActionLayerType = reader.GetInt(i, (int)EGuideNodeConfig.GuideActionLayerType, 0);

                dicGuideConfigTable.Add((EGuideNodeID)sConfig.nGuideID, sConfig);
            }
            reader.Dispose();
            reader = null;

        }
        protected void _LoadGuideStateConfig(string _strPath)
        {
            dicGuideStateConfigTable = new List<SSchemeGuideStateData>();

            ScpReader reader = new ScpReader(_strPath, true);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeGuideStateData sConfig = new SSchemeGuideStateData();
                
                sConfig.GuideStateID = reader.GetInt(i, (int)EGuideStateData.EGSD_GuideStateID, -1);
                sConfig.GameStateID = (EStage_State)reader.GetInt(i, (int)EGuideStateData.EGSD_GameState, 0);
                sConfig.UserIDType = (EGuideStateUserIDType)reader.GetInt(i, (int)EGuideStateData.EGSD_UserIDType, 0) ;
                sConfig.UserID = reader.GetInt(i, (int)EGuideStateData.EGSD_UserID, -1);
                sConfig.GuideRootType = (EGuideActionLayerType)reader.GetInt(i, (int)EGuideStateData.EGSD_GuideRootType, -1);
                sConfig.GuideNodeIDList = ScpReader.ConvertStringToIntArray(reader.GetString(i, (int)EGuideStateData.EGSD_GuideNodeIDList, ""), new char[] { ';' });
                sConfig.PlayerLevelLimit = reader.GetInt(i, (int)EGuideStateData.EGSD_PlayerLevelLimit, -1);

                dicGuideStateConfigTable.Add(sConfig);
            }
            reader.Dispose();
            reader = null;
        }



        #region 动态引导节点相关
        /// <summary>
        /// 只用于创建动态引导的根节点
        /// </summary>
        /// <param name="_eLayerType"></param>
        /// <returns></returns>
        protected IGuideDynamicRoot CreateDynamicRoot(EGuideActionLayerType _eLayerType)
        {
            switch(_eLayerType)
            {
                case EGuideActionLayerType.Sequence:
                    return new GuideDynamicRoot_Sequence();
                case EGuideActionLayerType.Parallel:
                    return new GuideDynamicRoot_ParallelOr();
                default:
                    {
                        Trace.LogWarning("当前没有:" + _eLayerType + "类型的动态父节点可创建");
                        return null;
                    }
            }
        }

        protected TBTAction CreateLayerTypeAction(EGuideActionLayerType _eLayerType)
        {
            if (_eLayerType == EGuideActionLayerType.LogicSequence)
                return new GuideLogicSequence();
            else if (_eLayerType == EGuideActionLayerType.Selector)
                return new GuideNodeDataSelector();//new TBTActionPrioritizedSelector();
            else if (_eLayerType == EGuideActionLayerType.Parallel)
                return new GuideDynamicRoot_ParallelOr();
            else //(_eLayerType == EGuideActionLayerType.Sequence)
                return new TBTActionSequence();
        }
        #endregion

        #region 引导状态节点相关
        protected void InitGuideState()
        {
            ////////////////////////////////////////////////////////初始化引导状态行为树
            m_GuideStateRoot = new GuideState_Root();
            for (int i = 0; i < dicGuideStateConfigTable.Count; ++i)
            {
                SSchemeGuideStateData data = dicGuideStateConfigTable[i];
                if (data != null && data.GuideNodeIDList != null && data.GuideNodeIDList.Count > 0)
                {
                    GuideState_GameState_Selector gameStateSelector = null;
                    //判断已经有该状态类型
                    if (!m_GuideStateRoot.TryGetGameStateSelector(data.GameStateID, out gameStateSelector))
                    {//没有则增加
                        gameStateSelector = new GuideState_GameState_Selector(data.GameStateID);
                        m_GuideStateRoot.AddChild(gameStateSelector);
                    }

                    //判断是否已有该用户ID
                    GuideState_UserID_Selector userIDSelector = null;
                    if (!gameStateSelector.TryGetUserIDSelector(data.UserIDType, data.UserID, out userIDSelector))
                    {
                        userIDSelector = new GuideState_UserID_Selector(data.UserIDType, data.UserID);
                        gameStateSelector.AddChild(userIDSelector);
                    }

                    //挂接创建动态引导根节点逻辑
                    if(userIDSelector != null)
                    {
                        GuideState_CreateDynamicGuideLeaf dynamic = new GuideState_CreateDynamicGuideLeaf(data.GuideStateID);
                        dynamic.SetPrecondition(new TBTPreconditionAND(new CON_IsMainPlayerCreated(),new CON_IsPlayerLevelExit(data.PlayerLevelLimit)));
                        userIDSelector.AddChild(dynamic);
                    }
                }
            }

            m_guideData = new GuideWorkingData();
            m_guideData.GuideDynamicBB = new GuideBlackboard_EGuideBBKey();
            m_guideData.GuideStateBB = new GuideBlackboard_EGuideStateBBKey();
            m_guideData.GuideStateBB.SetValue(EGuideStateBBKey.PlayerNoviceGuideType, ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP);
        }

        public void DestroyOldDynamicGuideRoot(TBTWorkingData wData)
        {
            if(m_GuideDynamicRoot != null)
            {
                m_GuideDynamicRoot.Destroy(wData);
                m_GuideDynamicRoot = null;
                (wData as GuideWorkingData).GuideDynamicBB.Reset();
            }
        }

        public void CreateNewDynamicGuideRoot(TBTWorkingData wData)
        {
            if(m_GuideDynamicRoot == null)
            {
                GuideWorkingData data = wData as GuideWorkingData;
                if (data != null && data.GuideDynamicBB != null)
                {
                    int GuideStateID = data.GuideStateBB.GetValue<int>(EGuideStateBBKey.GuideStateID, -1);
                    SSchemeGuideStateData stateData = GetGuideStateData(GuideStateID);
                    if (stateData != null)
                    {
                        m_GuideDynamicRoot = CreateDynamicRoot(stateData.GuideRootType);


                        for (int j = 0; j < stateData.GuideNodeIDList.Count; ++j)
                        {
                            EGuideNodeID guideNodeID = (EGuideNodeID)stateData.GuideNodeIDList[j];
                            SSchemeGuideNodeData guideNodeData = null;
                            if (dicGuideConfigTable.TryGetValue(guideNodeID, out guideNodeData))
                            {
                                //先根据引导节点需要挂接的父节点类型处理
                                EGuideActionLayerType layerType = (EGuideActionLayerType)guideNodeData.nGuideActionLayerType;
                                TBTAction layerTypeAction = null;
                                if (!m_GuideDynamicRoot.TryGetLayerTypeAction(layerType, out layerTypeAction))
                                {
                                    layerTypeAction = CreateLayerTypeAction(layerType);
                                    m_GuideDynamicRoot.AddChild(layerTypeAction);
                                }

                                layerTypeAction.AddChild(getNodeDataInstance(guideNodeID));
                            }
                            else
                                Trace.LogWarning("没有引导ID为:" + guideNodeID + " 的引导数据,配错了？");

                        }
                    }
                }
            }
        }

        public void RestoreGuideNodeConfigData()
        {
            foreach(var config in dicGuideConfigTable.Values)
            {
                config.bGuideFinished = false;
            }
        }

        protected SSchemeGuideStateData GetGuideStateData(int _nGuideStateID)
        {
            for (int i = 0; i < dicGuideStateConfigTable.Count; ++i)
            {
                if (dicGuideStateConfigTable[i].GuideStateID == _nGuideStateID)
                    return dicGuideStateConfigTable[i];
            }
            return null;
        }
        #endregion

        #region UGuideWidget
        protected List<UGuideWidget> m_listGuideWidget = new List<UGuideWidget>();

        public void RegisterUGuideWidget(UGuideWidget _widget)
        {
            m_listGuideWidget.Add(_widget);
        }

        public void UnRegisterUGuideWidget(UGuideWidget _widget)
        {
            m_listGuideWidget.Remove(_widget);
        }

        public UGuideWidget GetGuideWidget(EGuideNodeID _eGuideID, int _nStepID)
        {
            if (m_listGuideWidget.Count <= 0 && _eGuideID == EGuideNodeID.GuideNodeID_None)
                return null;

            for (int i = 0; i < m_listGuideWidget.Count; ++i)
            {
                if (m_listGuideWidget[i].GuideID == _eGuideID && m_listGuideWidget[i].GuideStepID == _nStepID)
                    return m_listGuideWidget[i];
            }

            return null;
        }

        public void ForceDestroyUGuideWidget()
        {
            if (m_listGuideWidget.Count <= 0)
                return;

            UGuideWidget widget = null; 
            GameObject guideWidgetGO = null;
            while (m_listGuideWidget.Count > 0)
            {
                widget = m_listGuideWidget[0];
                guideWidgetGO = widget.gameObject;
                ResNode.DestroyRes(ref guideWidgetGO);
                m_listGuideWidget.RemoveAt(0);
            }

        }
        #endregion

        /// <summary>
        /// 判断新手引导步骤是否大于等于指定的步骤
        /// </summary>
        public bool IsCurrentGuideGreaterEqual(ENNOVICEGUIDE_TYPE _eGuide)
        {
            bool result = true;
            //之后加个判断从数据库取当前玩家是否已完成新手引导做判断
            if (m_guideData != null)
            {
                //判断当前指定引导是否完成
                result = m_guideData.GuideStateBB.GetValue<int>(EGuideStateBBKey.PlayerNoviceGuideType, -1) >= (int)_eGuide;
            }

            return result;
        }


        /// <summary>
        /// 判断新手引导步骤是否少于等于指定的步骤
        /// </summary>
        public bool IsCurrentGuideLessEqual(ENNOVICEGUIDE_TYPE _eGuide)
        {
            bool result = true;
            //之后加个判断从数据库取当前玩家是否已完成新手引导做判断
            if (m_guideData != null)
            {
                //判断当前指定引导是否完成
                result = m_guideData.GuideStateBB.GetValue<int>(EGuideStateBBKey.PlayerNoviceGuideType, -1) <= (int)_eGuide;
            }

            return result;
        }
        /// <summary>
        /// 判断新手引导步骤是否少于等于指定的步骤
        /// </summary>
        /// <param name="_eGuide"></param>
        public bool IsCurrentGuideEqual(ENNOVICEGUIDE_TYPE _eGuide)
        {
            bool result = true;
            //之后加个判断从数据库取当前玩家是否已完成新手引导做判断
            if (m_guideData != null)
            {
                //判断当前指定引导是否完成
                result = m_guideData.GuideStateBB.GetValue<int>(EGuideStateBBKey.PlayerNoviceGuideType, -1) == (int)_eGuide;
            }

            return result;
        }

        internal void SaveGuideNode()
        {
            //WILL-TODO:暂不处理，看是否需要存到服务器，不需要的话考虑将已完成的引导ID存到玩家本地配置PlayerSettingLocalConfig
        }

        /// <summary>
        /// 设置玩家实体创建事件回调
        /// </summary>
        public void SetWaitForMainPlayerCreate()
        {
            //现在还不需要跳过新手引导，需要跳过时，把这里开启并注释第二行
            //EntityFactory.CreateMainHeroEntityEvent += WaitForMainPlayerCreate;
            GuideManager.enable = true;
#if ENABLE_YUNYING_MODE
            GuideManager.enable = false;
#endif
        }
        /// <summary>
        /// 延迟一定时间启用新手引导
        /// </summary>
        /// <param name="e"></param>
        private void WaitForMainPlayerCreate(CreateMainHeroEntityEventArgs e)
        {
            TimerManager.SetTimer(this, 0, 1,1);
        }

        public void OnTimer(int nTimerID)
        {
            EntityFactory.CreateMainHeroEntityEvent -= WaitForMainPlayerCreate;

            ENNOVICEGUIDE_TYPE PlayerNoviceGuideType = m_guideData.GuideStateBB.GetValue<ENNOVICEGUIDE_TYPE>(EGuideStateBBKey.PlayerNoviceGuideType, ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER);
            if (PlayerNoviceGuideType == ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUID_NEWPLAYER)
            {
                NewPlayerGuide();
            }
            else if((int)PlayerNoviceGuideType <= (int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_MAX-1)
            {
                GuideManager.enable = true;
            }
        }
        private void NewPlayerGuide()
        {
            USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
            title: "新手引导？",
            message: "开启新手引导？",
            buttons: new UIWidgets.DialogActions() {
                                      {
                                          USpeedUI.ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),
                                          () => {GuideManager.enable = true; return true;}
                                      },
                                      {
                                          USpeedUI.ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),
                                          () => 
                                          {
                                              GuideManager.enable = false;
                                              //这里应该还要改下逻辑层的引导步骤
                                              SetNoviceGuideType((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_MAX-1);
                                              return true;
                                          }
                                      }
            },
            modal: true,
            hideByModalClick: false,
            modalColor: new Color(1, 0, 0, 1)
            );

            USpeedUI.UISystem.Instance.SendTargetWndMessage(USpeedUI.WndID.WND_ID_Dialog, USpeedUI.WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
    }
}
