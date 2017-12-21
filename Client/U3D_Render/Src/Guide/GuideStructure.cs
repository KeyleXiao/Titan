/*******************************************************************
** 文件名:	GuideStructure.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-12-12
** 版  本:	1.0
** 描  述:	引导所有数据结构文件
		
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2017.8.23
** 描  述: 拆分文件
********************************************************************/

using Stage_ManagerDef;
using System.Collections.Generic;
using TsiU;
using UnityEngine;

namespace GUIDE
{
    #region 引导状态
    /// <summary>
    /// 引导状态-根节点
    /// </summary>
    class GuideState_Root : TBTAction
    {
        protected EStage_State m_eOldStage;
        protected int m_nNoviceID;
        public bool TryGetGameStateSelector(EStage_State _eState, out GuideState_GameState_Selector _selector)
        {
            for (int i = 0; i < GetChildCount(); ++i)
            {
                _selector = GetChild<GuideState_GameState_Selector>(i);
                if (_selector != null && _selector.SelectorGameState == _eState)
                {
                    return true;
                }
            }
            _selector = null;
            return false;
        }

        protected class TBTActionPrioritizedSelectorContext : TBTActionContext
        {
            internal int currentSelectedIndex;
            internal int lastSelectedIndex;

            public TBTActionPrioritizedSelectorContext()
            {
                currentSelectedIndex = -1;
                lastSelectedIndex = -1;
            }
        }
        public GuideState_Root()
            : base(-1)
        {
        }
        protected override bool onEvaluate(/*in*/TBTWorkingData wData)
        {
            GuideWorkingData gWData = wData as GuideWorkingData;
            EStage_State tmpCurState = EStage_State.ESS_NONE;
            int tmpNoviceID = -1;
            if (gWData != null)
            {
                tmpCurState = gWData.GuideStateBB.GetValue<EStage_State>(EGuideStateBBKey.GameStageState, EStage_State.ESS_NONE);
                tmpNoviceID = gWData.GuideStateBB.GetValue<int>(EGuideStateBBKey.PlayerNoviceGuideType, -1);
            }

            if (m_eOldStage != EStage_State.ESS_NONE && m_eOldStage != tmpCurState)
            {
                GuideManager.Instance.DestroyOldDynamicGuideRoot(wData);
            }
            else if(m_eOldStage != EStage_State.ESS_NONE && m_eOldStage == tmpCurState)
            {
                if(tmpNoviceID != -1 && tmpNoviceID != m_nNoviceID)
                {
                    GuideManager.Instance.DestroyOldDynamicGuideRoot(wData);
                }
            }
            m_eOldStage = tmpCurState;
            m_nNoviceID = tmpNoviceID;


            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            thisContext.currentSelectedIndex = -1;
            int childCount = GetChildCount();
            for (int i = 0; i < childCount; ++i)
            {
                TBTAction node = GetChild<TBTAction>(i);
                if (node.Evaluate(wData))
                {
                    thisContext.currentSelectedIndex = i;
                    return true;
                }
            }
            return false;
        }
        protected override int onUpdate(TBTWorkingData wData)
        {
            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            int runningState = TBTRunningStatus.FINISHED;

            if (thisContext.currentSelectedIndex != thisContext.lastSelectedIndex)
            {
                if (IsIndexValid(thisContext.lastSelectedIndex))
                {
                    TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
                    node.Transition(wData);
                }
                thisContext.lastSelectedIndex = thisContext.currentSelectedIndex;

            }

            if (IsIndexValid(thisContext.lastSelectedIndex))
            {
                TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
                runningState = node.Update(wData);
                if (TBTRunningStatus.IsFinished(runningState))
                {
                    thisContext.lastSelectedIndex = -1;
                }
            }

            return runningState;
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
            if (node != null)
            {
                node.Transition(wData);
            }
            thisContext.lastSelectedIndex = -1;
        }
    }
    /// <summary>
    /// 引导状态-游戏状态节点
    /// </summary>
    class GuideState_GameState_Selector : TBTActionPrioritizedSelector
    {
        protected EStage_State m_eSelectorGameState;
        public EStage_State SelectorGameState { get { return m_eSelectorGameState; } }


        public GuideState_GameState_Selector(EStage_State _eGameState)
        {
            m_eSelectorGameState = _eGameState;
            this.SetPrecondition(new CON_IsGameStateExit(_eGameState));
        }

        public bool TryGetUserIDSelector(EGuideStateUserIDType _eUserIDType, int _nUserID, out GuideState_UserID_Selector _selector)
        {
            for (int i = 0; i < GetChildCount(); ++i)
            {
                _selector = GetChild<GuideState_UserID_Selector>(i);
                if (_selector != null && _selector.UserIDType == _eUserIDType && _selector.UserID == _nUserID)
                {
                    return true;
                }
            }
            _selector = null;
            return false;
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
        }
    }
    /// <summary>
    /// 引导状态-用户ID节点
    /// </summary>
    class GuideState_UserID_Selector : TBTActionParallel
    {
        protected EGuideStateUserIDType m_eUserIDType;
        public EGuideStateUserIDType UserIDType { get { return m_eUserIDType; } }
        protected int m_nUserID;
        public int UserID { get { return m_nUserID; } }

        public GuideState_UserID_Selector(EGuideStateUserIDType _eUserIDType, int _nUserID)
        {
            m_eUserIDType = _eUserIDType;
            m_nUserID = _nUserID;
            this.SetEvaluationRelationship(ECHILDREN_RELATIONSHIP.OR);
            //判断条件
            this.SetPrecondition(new CON_IsUserIDExit(m_eUserIDType, m_nUserID));
        }


        public bool TryGetLayerTypeAction(EGuideActionLayerType _eLayerType, out TBTAction _selector)
        {
            for (int i = 0; i < GetChildCount(); ++i)
            {
                _selector = GetChild<TBTAction>(i);
                if (_selector != null)
                {
                    if (_eLayerType == EGuideActionLayerType.Sequence && _selector is TBTActionSequence)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Parallel && _selector is TBTActionParallel)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Selector && _selector is TBTActionPrioritizedSelector)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.LogicSequence && _selector is GuideLogicSequence)
                        return true;
                }
            }
            _selector = null;
            return false;
        }


        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
        }
    }
    /// <summary>
    /// 引导状态-叶子节点
    /// </summary>
    class GuideState_CreateDynamicGuideLeaf : TBTActionLeaf
    {
        protected int m_nGuideStateID;

        public GuideState_CreateDynamicGuideLeaf(int _nGuideStateID)
        {
            m_nGuideStateID = _nGuideStateID;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);

            GuideWorkingData data = wData as GuideWorkingData;
            if (data != null)
            {
                data.GuideStateBB.SetValue(EGuideStateBBKey.GuideStateID, m_nGuideStateID);
                GuideManager.Instance.CreateNewDynamicGuideRoot(data);
            }
        }
    }
    #endregion

    #region 动态引导
    /// <summary>
    /// 动态引导-根节点接口
    /// </summary>
    interface IGuideDynamicRoot
    {
        void Init();
        void Tick(TBTWorkingData _data);
        void Destroy(TBTWorkingData _data);
        bool TryGetLayerTypeAction(EGuideActionLayerType _eLayerType, out TBTAction _selector);
        TBTTreeNode AddChild(TBTTreeNode _action);
    }
    /// <summary>
    /// 动态引导-并行判断条件为或类型的节点
    /// </summary>
    class GuideDynamicRoot_ParallelOr : TBTActionParallel, IGuideDynamicRoot
    {
        public GuideDynamicRoot_ParallelOr()
        {
            SetEvaluationRelationship(ECHILDREN_RELATIONSHIP.OR);
        }

        public virtual void Destroy(TBTWorkingData _data)
        {
            Transition(_data);
            GuideManager.Instance.RestoreGuideNodeConfigData();
        }

        public virtual void Init()
        {

        }

        public void Tick(TBTWorkingData _data)
        {
            if (Evaluate(_data))
            {
                Update(_data);
            }
        }

        public virtual bool TryGetLayerTypeAction(EGuideActionLayerType _eLayerType, out TBTAction _selector)
        {
            for (int i = 0; i < GetChildCount(); ++i)
            {
                _selector = GetChild<TBTAction>(i);
                if (_selector != null)
                {
                    if (_eLayerType == EGuideActionLayerType.Sequence && _selector is TBTActionSequence)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Parallel && _selector is TBTActionParallel)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Selector && _selector is TBTActionPrioritizedSelector)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.LogicSequence && _selector is GuideLogicSequence)
                        return true;
                }
            }
            _selector = null;
            return false;
        }
    }
    /// <summary>
    /// 动态引导-序列节点
    /// </summary>
    class GuideDynamicRoot_Sequence : TBTActionSequence, IGuideDynamicRoot
    {
        public GuideDynamicRoot_Sequence()
        {
        }
        public virtual void Destroy(TBTWorkingData _data)
        {
            Transition(_data);
            GuideManager.Instance.RestoreGuideNodeConfigData();
        }

        public virtual void Init()
        {

        }

        public void Tick(TBTWorkingData _data)
        {
            if (Evaluate(_data))
            {
                Update(_data);
            }
        }

        public virtual bool TryGetLayerTypeAction(EGuideActionLayerType _eLayerType, out TBTAction _selector)
        {
            for (int i = 0; i < GetChildCount(); ++i)
            {
                _selector = GetChild<TBTAction>(i);
                if (_selector != null)
                {
                    if (_eLayerType == EGuideActionLayerType.Sequence && _selector is TBTActionSequence)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Parallel && _selector is TBTActionParallel)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.Selector && _selector is TBTActionPrioritizedSelector)
                        return true;
                    else if (_eLayerType == EGuideActionLayerType.LogicSequence && _selector is GuideLogicSequence)
                        return true;
                }
            }
            _selector = null;
            return false;
        }

    }




    /// <summary>
    /// 动态引导-行为节点-选择类型节点
    /// </summary>
    class GuideNodeDataSelector : TBTAction
    {
        protected class TBTActionPrioritizedSelectorContext : TBTActionContext
        {
            internal int currentSelectedIndex;
            internal int lastSelectedIndex;

            public TBTActionPrioritizedSelectorContext()
            {
                currentSelectedIndex = -1;
                lastSelectedIndex = -1;
            }
        }
        public GuideNodeDataSelector()
            : base(-1)
        {
        }
        protected override bool onEvaluate(/*in*/TBTWorkingData wData)
        {
            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            thisContext.currentSelectedIndex = -1;
            int childCount = GetChildCount();
            for (int i = 0; i < childCount; ++i)
            {
                TBTAction node = GetChild<TBTAction>(i);
                if (node.Evaluate(wData))
                {
                    thisContext.currentSelectedIndex = i;
                    return true;
                }
            }
            return false;
        }
        protected override int onUpdate(TBTWorkingData wData)
        {
            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            int runningState = TBTRunningStatus.FINISHED;
            if (thisContext.currentSelectedIndex != thisContext.lastSelectedIndex)
            {
                if (IsIndexValid(thisContext.lastSelectedIndex))
                {
                    TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
                    node.Transition(wData);
                }
                thisContext.lastSelectedIndex = thisContext.currentSelectedIndex;
            }
            if (IsIndexValid(thisContext.lastSelectedIndex))
            {
                TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
                runningState = node.Update(wData);
                if (TBTRunningStatus.IsFinished(runningState))
                {
                    node.Transition(wData);
                    thisContext.lastSelectedIndex = -1;
                }
            }
            return runningState;
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            TBTActionPrioritizedSelectorContext thisContext = getContext<TBTActionPrioritizedSelectorContext>(wData);
            TBTAction node = GetChild<TBTAction>(thisContext.lastSelectedIndex);
            if (node != null)
            {
                node.Transition(wData);
            }
            thisContext.lastSelectedIndex = -1;
        }
    }
    /// <summary>
    /// 动态引导-行为节点-序列类型节点
    /// </summary>
    abstract class GuideNodeDataSequence : TBTActionSequence
    {
        public GuideNodeDataSequence() : base()
        {
            Init();
        }

        protected virtual void Init()
        {
        }

        protected override int onUpdate(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;
            gData.GuideCurrentNode = GuideNodeID();
            return base.onUpdate(wData);
        }

        public abstract EGuideNodeID GuideNodeID();
    }
    /// <summary>
    /// 动态引导-行为节点-并行类型节点
    /// </summary>
    abstract class GuideNodeDataParallel : TBTActionParallel
    {
        protected bool m_bRunning;

        public GuideNodeDataParallel()
        {
            Init();
            this.SetRunningStatusRelationship(ECHILDREN_RELATIONSHIP.AND);

        }
        protected virtual void Init() { }

        protected override int onUpdate(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;
            gData.GuideCurrentNode = GuideNodeID();
            return base.onUpdate(wData);
        }

        protected override bool onEvaluate(TBTWorkingData wData)
        {
            bool result = base.onEvaluate(wData);
            //if (m_bRunning == false && result == true)
            //    m_bRunning = true;
            return result;
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            m_bRunning = false;
        }

        public abstract EGuideNodeID GuideNodeID();

    }
    /// <summary>
    /// 动态引导-行为节点-序列类型节点
    /// <para>专门用于处理通过buff判断处理逻辑的序列节点</para>
    /// </summary>
    class GuideLogicSequence : TBTAction
    {
        protected int m_nLogicStepID = -1;

        //-------------------------------------------------------
        protected class GuideRoot_01Context : TBTActionContext
        {
            internal int currentSelectedIndex;
            public GuideRoot_01Context()
            {
                currentSelectedIndex = -1;
            }
        }
        //-------------------------------------------------------
        private bool _continueIfErrorOccors;
        //-------------------------------------------------------
        public GuideLogicSequence()
            : base(-1)
        {
            _continueIfErrorOccors = false;
        }
        public GuideLogicSequence SetContinueIfErrorOccors(bool v)
        {
            _continueIfErrorOccors = v;
            return this;
        }
        //------------------------------------------------------
        protected override bool onEvaluate(/*in*/TBTWorkingData wData)
        {
            UpdateLogicData(wData);

            GuideRoot_01Context thisContext = getContext<GuideRoot_01Context>(wData);
            int checkedNodeIndex = -1;
            if (IsIndexValid(thisContext.currentSelectedIndex))
            {
                checkedNodeIndex = thisContext.currentSelectedIndex;
            }
            else {
                checkedNodeIndex = 0;
            }
            if (IsIndexValid(checkedNodeIndex))
            {
                TBTAction node = GetChild<TBTAction>(checkedNodeIndex);
                if (node.Evaluate(wData))
                {
                    thisContext.currentSelectedIndex = checkedNodeIndex;
                    return true;
                }
            }
            return false;
        }
        protected override int onUpdate(TBTWorkingData wData)
        {
            GuideRoot_01Context thisContext = getContext<GuideRoot_01Context>(wData);
            int runningStatus = TBTRunningStatus.FINISHED;
            TBTAction node = GetChild<TBTAction>(thisContext.currentSelectedIndex);
            runningStatus = node.Update(wData);
            if (_continueIfErrorOccors == false && TBTRunningStatus.IsError(runningStatus))
            {
                thisContext.currentSelectedIndex = -1;
                return runningStatus;
            }

            return runningStatus;
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            GuideRoot_01Context thisContext = getContext<GuideRoot_01Context>(wData);
            TBTAction node = GetChild<TBTAction>(thisContext.currentSelectedIndex);
            if (node != null)
            {
                node.Transition(wData);
            }
        }

        protected void UpdateLogicData(TBTWorkingData _data)
        {
            GuideWorkingData gData = _data.As<GuideWorkingData>();
            if (gData != null && m_nLogicStepID != gData.GuideDynamicBB.GetValue<int>(EGuideBBKey.LogicStepID, -1))
            {
                if (m_nLogicStepID != -1)
                {
                    Transition(_data);
                }
                m_nLogicStepID = gData.GuideDynamicBB.GetValue<int>(EGuideBBKey.LogicStepID, -1);

                GuideRoot_01Context thisContext = getContext<GuideRoot_01Context>(_data);

                thisContext.currentSelectedIndex++;
                if (!IsIndexValid(thisContext.currentSelectedIndex))
                {
                    thisContext.currentSelectedIndex = -1;
                }
            }
        }

    }
    #endregion

    /// <summary>
    /// 对应配置GuideConfig.csv的数据
    /// </summary>
    public class SSchemeGuideNodeData
    {
        /// <summary>
        /// 新手引导序号，每个引导的唯一值
        /// </summary>
        public int nGuideID;

        public List<int> GuideIntParams = new List<int>();
        public List<string> GuideStringParams = new List<string>();

        /// <summary>
        /// 新手引导层级类型
        /// 0:sequence 1:Selector 2:Parallel
        /// </summary>
        public int nGuideActionLayerType;


        /// <summary>
        /// 该引导是否已完成
        /// 目前还没确定是否需要存储到服务器，也没存到本地玩家配置表
        /// 所以每次重启客户端都会重置所有新手引导
        /// </summary>
        public bool bGuideFinished;
    }

    /// <summary>
    /// 主引导状态，主要根据游戏状态和用户设置的ID来区分引导节点挂载位置、引导流程
    /// </summary>
    public class SSchemeGuideStateData
    {
        /// <summary>
        /// 引导状态ID
        /// </summary>
        public int GuideStateID = 0;
        /// <summary>
        /// 游戏状态类型
        /// </summary>
        public EStage_State GameStateID = EStage_State.ESS_NONE;
        /// <summary>
        /// 引导状态中用户ID类型
        /// </summary>
        public EGuideStateUserIDType UserIDType = EGuideStateUserIDType.EST_None;
        /// <summary>
        /// 用户ID
        /// </summary>
        public int UserID = 0;
        /// <summary>
        /// 动态引导根节点类型
        /// </summary>
        public EGuideActionLayerType GuideRootType = EGuideActionLayerType.None;
        /// <summary>
        /// 该引导状态下用到的引导节点ID列表
        /// </summary>
        public List<int> GuideNodeIDList = null;
        /// <summary>
        /// 玩家等级限制
        /// </summary>
        public int PlayerLevelLimit = -1;
    }

    /// <summary>
    /// 用于于树各个节点的，由于树节点取不到Blackboard所以，通过WorkingData来取
    /// </summary>
    public class GuideWorkingData : TBTWorkingData
    {
        /// <summary>
        /// 引导状态黑板
        /// </summary>
        public GuideBlackboard_EGuideStateBBKey GuideStateBB { get; set; }
        /// <summary>
        /// 动态引导黑板
        /// </summary>
        public GuideBlackboard_EGuideBBKey GuideDynamicBB { get; set; }
        public EGuideNodeID GuideCurrentNode { get; set; }
    }

    #region Blackboard结构类，用字典方式存储/获取不同类型的临时数据
    /// <summary>
    /// 黑板数据
    /// </summary>
    public class GuideBlackboard_EGuideStateBBKey
    {
        private Dictionary<EGuideStateBBKey, BlackboardItem> _items;

        public GuideBlackboard_EGuideStateBBKey()
        {
            Reset();
        }

        public void SetValue(EGuideStateBBKey key, object v)
        {
            BlackboardItem item;
            if (_items.ContainsKey(key) == false)
            {
                item = new BlackboardItem();
                _items.Add(key, item);
            }
            else
            {
                item = _items[key];
            }
            item.SetValue(v);
        }
        public T GetValue<T>(EGuideStateBBKey key, T defaultValue)
        {
            if (_items.ContainsKey(key) == false)
            {
                return defaultValue;
            }
            return _items[key].GetValue<T>();
        }

        /// <summary>
        /// 清空所有数据
        /// </summary>
        public void Reset()
        {
            _items = new Dictionary<EGuideStateBBKey, BlackboardItem>();
        }
        
    }
    public class BlackboardItem
    {
        private object _value;
        public void SetValue(object v)
        {
            _value = v;
        }
        public T GetValue<T>()
        {
            return (T)_value;
        }
    }

    public class GuideBlackboard_EGuideBBKey
    {

        private Dictionary<EGuideBBKey, BlackboardItem> _items;

        public GuideBlackboard_EGuideBBKey()
        {
            Reset();
        }

        public void SetValue(EGuideBBKey key, object v)
        {
            BlackboardItem item;
            if (_items.ContainsKey(key) == false)
            {
                item = new BlackboardItem();
                _items.Add(key, item);
            }
            else
            {
                item = _items[key];
            }
            item.SetValue(v);
        }
        public T GetValue<T>(EGuideBBKey key, T defaultValue)
        {
            if (_items.ContainsKey(key) == false)
            {
                return defaultValue;
            }
            return _items[key].GetValue<T>();
        }

        /// <summary>
        /// 清空所有数据
        /// </summary>
        public void Reset()
        {
            _items = new Dictionary<EGuideBBKey, BlackboardItem>();
        }

    }
    #endregion

    public class GuideTest : MonoBehaviour
    {
        private NOD_ShowPathMesh pathMesh = null;
        public Transform Source;
        public Transform Target;
        public GameObject Prefab;

        public void Start()
        {
            pathMesh = new NOD_ShowPathMesh(Source, Prefab);

            pathMesh.InitSplineMesh();

            pathMesh.SetSplineMeshPoint(1, Target.position);
        }

        public void Update()
        {
            pathMesh.Execute();
        }

    }

}
