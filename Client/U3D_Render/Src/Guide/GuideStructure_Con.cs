/*******************************************************************
** 文件名:	GuideStructure_Con.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-8-23
** 版  本:	1.0
** 描  述:	新手引导树中用于控制节点流程的条件节点
		
********************************************************************/
using DataCenter;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using System.Linq;
using TsiU;
using U3D_Render;
using UnityEngine;
using USpeedUI;
using USpeedUI.Guide;

namespace GUIDE
{
    /// <summary>
    /// 自定义的条件基类
    /// </summary>
    abstract class GuideNodePrecondition : TBTPreconditionLeaf
    {
        protected int m_nRunningState = TBTRunningStatus.EXECUTING;
        protected abstract bool CustomPrecondition(TBTWorkingData wData);

        public override bool IsTrue(TBTWorkingData wData)
        {
            if (CustomPrecondition(wData) && TBTRunningStatus.IsExecuting(m_nRunningState))
            {
                m_nRunningState = TBTRunningStatus.FINISHED;
            }
            return TBTRunningStatus.IsFinished(m_nRunningState);
        }
    }


    /// <summary>
    /// 判断
    /// </summary>
    class CON_IsPersonModelPlayModeExit : GuideNodePrecondition
    {
        private int m_nCheckPlayMode;
        public CON_IsPersonModelPlayModeExit(int _nCheckPlayMode)
        {
            m_nCheckPlayMode = _nCheckPlayMode;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
            {
                int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref modelData);
                result = modelData.nPlayMode == m_nCheckPlayMode;
            }
            TLogger.PROFILE("判断主角英雄是否需要" + (PlayMode)m_nCheckPlayMode + " 模式操作::" + result);

            return result;
        }
    }
    /// <summary>
    /// 当前节点是否已结束
    /// </summary>
    class CON_IsNodeFinished : GuideNodePrecondition
    {
        private EGuideNodeID m_nNodeID;
        public CON_IsNodeFinished(EGuideNodeID _nNodeID = EGuideNodeID.GuideNodeID_None)
        {
            m_nNodeID = _nNodeID;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            SSchemeGuideNodeData cData = GuideManager.Instance.getNodeConfigData(m_nNodeID);
            result = cData != null ? !cData.bGuideFinished : false;
            if (cData != null)
                TLogger.PROFILE("判断引导::" + m_nNodeID + " 是否已结束::" + cData.bGuideFinished);

            return result;
        }
        public override bool IsTrue(TBTWorkingData wData)
        {
            return CustomPrecondition(wData);
        }
    }
    /// <summary>
    /// 本地玩家附近是否有怪物死亡
    /// </summary>
    class CON_IsNearbyMonsterDead : GuideNodePrecondition
    {
        private int m_nNearbySize;
        private bool m_bDoOnce = false;
        public CON_IsNearbyMonsterDead(int _nNearbySize)
        {
            m_nNearbySize = _nNearbySize;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = m_bDoOnce;

            if (!m_bDoOnce)
            {
                GuideWorkingData gData = wData as GuideWorkingData;
                Vector3 MonsterDeadPos = gData.GuideDynamicBB.GetValue<Vector3>(EGuideBBKey.DeadEntityID, Vector3.zero);
                if (MonsterDeadPos.magnitude != 0 && EntityFactory.MainHero != null)
                {
                    result = (EntityFactory.MainHero.transform.position - MonsterDeadPos).sqrMagnitude <= m_nNearbySize;
                    //清空下
                    if (result)
                    {
                        gData.GuideDynamicBB.SetValue(EGuideBBKey.DeadEntityID, Vector3.zero);
                        m_bDoOnce = true;
                    }
                }
                TLogger.PROFILE("判断玩家附近是否有敌方实体死亡（实体死亡会销毁比较难检测到）:" + result);
            }

            return result;
        }
    }

    /// <summary>
    /// 属性值超过标准值时显示提示，一定时间后仍超过的继续提示
    /// </summary>
    class CON_IsPropertyCompareStandardWithDelay : GuideNodePrecondition, ITimerHanlder
    {
        private int m_nPropertyID;
        private int m_nMaxPropertyID;
        private int m_nValueStandard;
        private int m_nDelayTime;
        private int m_nCompareType;

        protected bool m_bFirst = true;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_nValueStandard">属性检测值</param>
        /// <param name="_nPropertyID">属性ID</param>
        /// <param name="_nMaxPropertyID">最大属性ID（填上的话属性检测值为百分比）</param>
        /// <param name="_nDelayTime">金钱超过检测值后一定时间后再次显示提示</param>
        public CON_IsPropertyCompareStandardWithDelay(int _nPropertyID, int _nValueStandard, int _nDelayTime, int _nMaxPropertyID = -1, int _nCompareType = 1)
        {
            m_nPropertyID = _nPropertyID;
            m_nMaxPropertyID = _nMaxPropertyID;
            m_nValueStandard = _nValueStandard;
            m_nDelayTime = _nDelayTime;
            m_nCompareType = _nCompareType;
        }

        public void OnTimer(int nTimerID)
        {
            m_bFirst = true;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView != null)
            {
                int nCompareValue = m_nValueStandard;
                if (m_nMaxPropertyID != -1)
                {
                    float per = (m_nValueStandard / 100f);
                    float res = EntityFactory.MainHeroView.Property.GetNumProp((ENTITY_PROPERTY)m_nMaxPropertyID);
                    nCompareValue = (int)(res * per);
                }
                if (m_nCompareType > 1)
                    result = EntityFactory.MainHeroView.Property.GetNumProp((ENTITY_PROPERTY)m_nPropertyID) > nCompareValue;
                else if (m_nCompareType < 1)
                    result = EntityFactory.MainHeroView.Property.GetNumProp((ENTITY_PROPERTY)m_nPropertyID) < nCompareValue;
                else
                    result = EntityFactory.MainHeroView.Property.GetNumProp((ENTITY_PROPERTY)m_nPropertyID) == nCompareValue;
            }
            //
            if (result)
            {
                //第一次超过标准值直接显示
                if (m_bFirst)
                {
                    result = true;
                    m_bFirst = false;
                }
                else
                {
                    TimerManager.SetTimer(this, 0, m_nDelayTime, 1);
                    result = false;
                }
            }
            else
            {
                m_bFirst = true;
                TimerManager.KillTimer(this, 0);
            }
            TLogger.PROFILE("判断玩家属性:" + (ENTITY_PROPERTY)m_nPropertyID + " 的数值是否达到" + m_nValueStandard + ":" + result);

            return result;
            //return base.IsTrue(wData);
        }

        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null && EntityFactory.MainHeroView.Property)
            {
                result = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TALENT_POINT) >= m_nValueStandard;
            }
            //
            if (result && TBTRunningStatus.IsFinished(m_nRunningState))
            {
                TimerManager.SetTimer(this, 0, 60, 1);
            }
            TLogger.PROFILE("判断玩家战场内金钱数量是否达到" + m_nValueStandard + ":" + result);

            return result;
        }
    }
    /// <summary>
    /// 本地玩家英雄是否挂有某buff
    /// </summary>
    class CON_IsBuffExit : GuideNodePrecondition
    {
        private int m_nBuffID;
        private int? m_nCheckCount;
        private bool m_bOldExitBuff;
        public CON_IsBuffExit(int _nBuffID)
        {
            m_nBuffID = _nBuffID;
        }
        public CON_IsBuffExit SetBuffCheckCount(int _nCount)
        {
            m_nCheckCount = _nCount;
            return this;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = (GameLogicAPI.IsExistBuff((uint)m_nBuffID));

            if (m_nCheckCount != null)
            {
                if (m_bOldExitBuff != result && result)
                {
                    m_nCheckCount--;
                }
                m_bOldExitBuff = result;

                result = result && m_nCheckCount <= 0;

            }
            TLogger.PROFILE("判断玩家英雄身上是否有BUFF(" + m_nBuffID + "):" + result);

            return result;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = (GameLogicAPI.IsExistBuff((uint)m_nBuffID));

            if (m_nCheckCount != null)
            {
                if (m_bOldExitBuff != result && result)
                {
                    m_nCheckCount--;
                }
                m_bOldExitBuff = result;

                result = result && m_nCheckCount <= 0;

            }
            TLogger.PROFILE("判断玩家英雄身上是否有BUFF(" + m_nBuffID + "):" + result);

            return result;
        }
    }
    /// <summary>
    /// 本地玩家等级
    /// </summary>
    class CON_IsPlayerLevelExit : TBTPreconditionLeaf//GuideNodePrecondition
    {
        private int m_nPlayerLevel;
        public CON_IsPlayerLevelExit(int _nPlayerLevel)
        {
            m_nPlayerLevel = _nPlayerLevel;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;
            if (m_nPlayerLevel == -1)
            {
                result = true;
            }
            else
            {
                ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
                if (info.playerInfo.pDbid == 0)
                    return false;
                result = info.playerInfo.nLv <= m_nPlayerLevel;
                TLogger.PROFILE("判断当前玩家等级是否符合引导所需等级(" + m_nPlayerLevel + "):" + result);
            }
            return result;
        }

        protected bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;
            if (m_nPlayerLevel == -1)
            {
                result = true;
            }
            else
            {
                ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo();
                result = info.playerInfo.nLv == m_nPlayerLevel;
                TLogger.PROFILE("判断当前玩家等级是否符合引导所需等级(" + m_nPlayerLevel + "):" + result);
            }
            return result;
        }
    }

    /// <summary>
    /// 判断当前逻辑层处理的ID
    /// </summary>
    class CON_IsLogicStepID : TBTPreconditionLeaf//GuideNodePrecondition
    {
        private int m_nLogicStepID = -1;

        public CON_IsLogicStepID(int _nLogicStepID)
        {
            m_nLogicStepID = _nLogicStepID;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;
            if (m_nLogicStepID == -1)
            {
                result = false;
            }
            else
            {
                int currentLogicStepID = wData.As<GuideWorkingData>().GuideDynamicBB.GetValue<int>(EGuideBBKey.LogicStepID, -1);

                result = (currentLogicStepID != -1 && currentLogicStepID == m_nLogicStepID);

                TLogger.PROFILE("判断当前逻辑层的步骤ID(" + currentLogicStepID + ")是否与当前数据一致:" + result);
            }
            return result;
        }
    }

    /// <summary>
    /// 判断附近是否有propertyID 的实体
    /// </summary>
    class CON_IsNearbyPropertyIDEntityExit : GuideNodePrecondition
    {
        private int m_nPropertyID;
        private int m_nSearchNearRadius;
        private SPELL.CampFlag m_eCampFlag;


        public CON_IsNearbyPropertyIDEntityExit(int _nPropertyID, int _nSearchNearRadius, SPELL.CampFlag _eCampFlag = SPELL.CampFlag.CampFlag_Enemy)
        {
            m_nPropertyID = _nPropertyID;
            m_nSearchNearRadius = _nSearchNearRadius;
            m_eCampFlag = _eCampFlag;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView == null || !EntityFactory.MainHeroView.IsValid)
                return result;

            GuideWorkingData gData = wData as GuideWorkingData;

            List<EntityView> AroundEntitiesList = MapFinderManager.Nearest(EntityFactory.MainHeroView.StateMachine.transform.position, m_nSearchNearRadius, m_eCampFlag).ToList();

            if (AroundEntitiesList != null)
            {
                for (int i = AroundEntitiesList.Count - 1; i >= 0; i--)
                {
                    if (AroundEntitiesList[i].Property == null || AroundEntitiesList[i].Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != m_nPropertyID)
                    {
                        AroundEntitiesList.RemoveAt(i);
                    }
                }

                //检测附近范围内有没有实体
                result = AroundEntitiesList.Count != 0;
            }

            return result;
        }
    }

    /// <summary>
    /// 判断附近是否有propertyID 的实体死亡
    /// </summary>
    class CON_IsEntityDeadByPropertyID : GuideNodePrecondition
    {
        private int m_nPropertyID;
        private SPELL.CampFlag m_eCampFlag;

        public CON_IsEntityDeadByPropertyID(int _nPropertyID, SPELL.CampFlag _eCampFlag = SPELL.CampFlag.CampFlag_Enemy)
        {
            m_nPropertyID = _nPropertyID;
            m_eCampFlag = _eCampFlag;
        }

        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView == null || !EntityFactory.MainHeroView.IsValid)
                return result;

            GuideWorkingData gData = wData as GuideWorkingData;

            int nDeadEntityID = gData.GuideDynamicBB.GetValue<int>(EGuideBBKey.DeadEntityID, -1);
            EntityView ev = EntityFactory.getEntityViewByID(nDeadEntityID);

            if (nDeadEntityID != -1 && ev != null)
            {
                int propertyID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

                result = propertyID == m_nPropertyID;
            }

            return result;

        }
    }
    /// <summary>
    /// 判断附是否有按下某快捷键
    /// </summary>
    class CON_IsKeyDown : GuideNodePrecondition
    {
        private KeyCode[] m_eCheckKeyCode;
        private FuntionShortCutSlotIndex m_eShortCutIndex;
        private bool m_bInputMgrEnableAffected;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_eShortCutIndex">快捷键标示</param>
        /// <param name="_bInputMgrEnableAffected">是否受 InputManager.enable 影响</param>
        /// <param name="_eCheckKeyCode">KeyCode标示</param>
        public CON_IsKeyDown(FuntionShortCutSlotIndex _eShortCutIndex = FuntionShortCutSlotIndex.None, bool _bInputMgrEnableAffected = false, params KeyCode[] _eCheckKeyCode)
        {
            m_eCheckKeyCode = _eCheckKeyCode;
            m_eShortCutIndex = _eShortCutIndex;
            m_bInputMgrEnableAffected = _bInputMgrEnableAffected;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            if (m_eShortCutIndex == FuntionShortCutSlotIndex.None)
            {
                for (int i = 0; i < m_eCheckKeyCode.Length; ++i)
                {
                    if (m_bInputMgrEnableAffected)
                        result = InputManager.GetKeyDown(m_eCheckKeyCode[i]);
                    else
                        result = InputManager.Raw_GetKeyDown(m_eCheckKeyCode[i]);

                    if (result)
                        break;
                }
            }
            else
                result = FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(m_eShortCutIndex);

            return result;
        }
    }
    /// <summary>
    /// 本地玩家英雄属性是否>=指定值
    /// </summary>
    class CON_IsPropertyUpToStandard : TBTPreconditionLeaf
    {
        protected int m_nPropertyID;
        protected int m_nStandardValue;

        public CON_IsPropertyUpToStandard(int _nPropertyID, int _nStandardValue)
        {
            m_nPropertyID = _nPropertyID;
            m_nStandardValue = _nStandardValue;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;

            if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null && EntityFactory.MainHeroView.Property)
            {
                result = EntityFactory.MainHeroView.Property.GetNumProp((ENTITY_PROPERTY)m_nPropertyID) >= m_nStandardValue;
            }

            return result;
        }
    }
    /// <summary>
    /// 判断某UI界面显示状态
    /// </summary>
    class CON_IsWndViewVisible : GuideNodePrecondition
    {
        protected bool m_bCheckVisibleState;
        protected WndID m_eWndID;

        public CON_IsWndViewVisible(bool _bCheckVisibleState, WndID _eWndID)
        {
            m_bCheckVisibleState = _bCheckVisibleState;
            m_eWndID = _eWndID;
        }
        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            IUIWnd wnd = UISystem.Instance.GetWnd(m_eWndID);
            return wnd == null ? false : wnd.IsVisible() == m_bCheckVisibleState;
        }
    }
    /// <summary>
    /// 判断UI引导元素是否有点击
    /// </summary>
    class CON_IsGuideUIWidgetClicked : GuideNodePrecondition
    {
        protected EGuideNodeID m_eGuideNodeID;
        protected int m_nStepID;

        public CON_IsGuideUIWidgetClicked(int _nStepID, EGuideNodeID _eGuideNodeID = EGuideNodeID.GuideNodeID_None)
        {
            m_eGuideNodeID = _eGuideNodeID;
            m_nStepID = _nStepID;
        }

        protected override bool CustomPrecondition(TBTWorkingData wData)
        {
            bool result = false;

            GuideWorkingData gData = wData as GuideWorkingData;

            EGuideNodeID tmpGuideNodeID = m_eGuideNodeID == EGuideNodeID.GuideNodeID_None ? gData.GuideCurrentNode : m_eGuideNodeID;

            UGuideWidget widget = GuideManager.Instance.GetGuideWidget(tmpGuideNodeID, m_nStepID);

            if (widget != null)
            {
                result = widget.IsWidgetClick();
            }

            return result;
        }

    }

    /// <summary>
    /// 判断游戏状态是否与输入值一致
    /// </summary>
    class CON_IsGameStateExit : TBTPreconditionLeaf
    {
        private EStage_State m_eGameState;
        public CON_IsGameStateExit(EStage_State _eGameState)
        {
            m_eGameState = _eGameState;
        }
        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = true;
            GuideWorkingData gData = wData as GuideWorkingData;

            if (m_eGameState != EStage_State.ESS_NONE && gData != null)
            {
                result = (gData.GuideStateBB.GetValue<EStage_State>(EGuideStateBBKey.GameStageState, EStage_State.ESS_NONE) == m_eGameState);
            }

            TLogger.PROFILE("判断当前比赛类型ID是否是(" + m_eGameState + "):" + result);

            return result;
        }
    }

    /// <summary>
    /// 判断用户ID类型和ID是否与输入值一致
    /// </summary>
    class CON_IsUserIDExit : TBTPreconditionLeaf
    {
        private EGuideStateUserIDType m_eUserIDType;
        protected int m_nUserID;

        public CON_IsUserIDExit(EGuideStateUserIDType _eUserIDType, int _nUserID)
        {
            m_eUserIDType = _eUserIDType;
            m_nUserID = _nUserID;
        }
        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = true;
            GuideWorkingData gData = wData as GuideWorkingData;

            if (gData != null)
            {
                if (m_eUserIDType == EGuideStateUserIDType.EST_NoviceID)
                {
                    result = (gData.GuideStateBB.GetValue<int>(EGuideStateBBKey.PlayerNoviceGuideType, -1) == m_nUserID);
                }
                else
                {
                    result = (gData.GuideStateBB.GetValue<int>(EGuideStateBBKey.GuideMatchTypeID, -1)) == m_nUserID;
                }
            }

            TLogger.PROFILE("判断当前用户ID类型:" + m_eUserIDType + ",ID:" + m_nUserID + "" + result);

            return result;
        }
    }
    /// <summary>
    /// 判断当前的引导行为节点类型
    /// </summary>
    class CON_IsCurrentGuideNode : TBTPreconditionLeaf
    {
        private EGuideNodeID m_eGuideNodeID;

        public CON_IsCurrentGuideNode(EGuideNodeID _eNodeID)
        {
            m_eGuideNodeID = _eNodeID;
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;

            EGuideNodeID currentLogicStepID = wData.As<GuideWorkingData>().GuideDynamicBB.GetValue<EGuideNodeID>(EGuideBBKey.LogicStepID, EGuideNodeID.GuideNodeID_None);
            result = currentLogicStepID == m_eGuideNodeID;

            TLogger.PROFILE("判断当引导ID(" + currentLogicStepID + ")是否与当前数据一致:" + result);

            return result;
        }
    }
    /// <summary>
    /// 判断主玩家实体是否已创建
    /// </summary>
    class CON_IsMainPlayerCreated : TBTPreconditionLeaf
    {

        public CON_IsMainPlayerCreated()
        {
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;

            result = EntityFactory.MainHeroView != null;

            TLogger.PROFILE("判断主玩家是否已创建:" + result);

            return result;
        }
    }
    /// <summary>
    /// 判断本命英雄是否已收集完毕
    /// </summary>
    class CON_IsLisfHeroFullfill : TBTPreconditionLeaf
    {
        public CON_IsLisfHeroFullfill()
        {
        }

        public override bool IsTrue(TBTWorkingData wData)
        {
            bool result = false;

            result = LogicDataCenter.lifeHeroDataManager.HasCollectedAllLifeHero();

            TLogger.PROFILE("判断玩家是否已找完所有本命英雄:" + result);

            return result;
        }
    }
}
