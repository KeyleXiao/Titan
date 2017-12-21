/*******************************************************************
** 文件名:	GuideStructure_Leaf.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-8-23
** 版  本:	1.0
** 描  述:	新手引导树中用于处理单个叶子行为的节点
		
********************************************************************/
using Common.FSMSample;
using DataCenter;
using GameLogic;
using Spline;
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
    /// 设置当前引导行为节点完成状态
    /// </summary>
    class NOD_DelayFinish : TBTActionLeaf
    {
        private bool m_bFinishRemoveUIMessage;
        public NOD_DelayFinish(bool _bRemoveUIMessage = false)
        {
            m_bFinishRemoveUIMessage = _bRemoveUIMessage;
        }
        protected override void onExit(TBTWorkingData wData, int runningStatus)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            if (m_bFinishRemoveUIMessage)
            {
                USpeedUI.UGuideMsgRemoveData msg = new USpeedUI.UGuideMsgRemoveData();
                USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, msg);
            }

            SSchemeGuideNodeData cData = GuideManager.Instance.getNodeConfigData(gData.GuideCurrentNode);
            cData.bGuideFinished = true;
            base.onExit(wData, runningStatus);
        }

        public override string ToString()
        {
            return "延迟结束";
        }
    }

    /// <summary>
    /// 通知UI显示当前引导行为信息
    /// </summary>
    class NOD_ShowGuideUIInfoParam : TBTActionLeaf
    {
        private string m_strTextParam;
        private int m_nSpriteParam;
        private bool m_bModal;
        private UGuideMsgData.CustomUserData m_userData;

        private string m_strToStringDebugMessage;
        private const string m_strDefaultDebugMessage = "NOD_ShowGuideUIInfoParam Node:{0},TextParam:{1};SpriteID:{2}";

        /// <summary>
        /// 设置UI要显示时需要的数据参数ID
        /// </summary>
        /// <param name="_eTextParamID">字符串参数ID</param>
        /// <param name="_eSpriteParamID">整型参数ID</param>
        /// <param name="_strDebugMessage">Debug打印信息</param>
        public NOD_ShowGuideUIInfoParam(string _strTextParam, int _nSpriteParam = -1, string _strDebugMessage = null, bool _bModal = false, UGuideMsgData.CustomUserData _userData = null) : base()
        {
            m_strTextParam = _strTextParam;
            m_nSpriteParam = _nSpriteParam;
            m_strToStringDebugMessage = _strDebugMessage;
            m_bModal = _bModal;
            m_userData = _userData;
        }
        protected override void onEnter(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            if (m_strToStringDebugMessage == null)
                m_strToStringDebugMessage = string.Format(m_strDefaultDebugMessage, gData.GuideCurrentNode, m_strTextParam, m_nSpriteParam);

            base.onEnter(wData);
        }
        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            USpeedUI.UGuideMsgData msg = new USpeedUI.UGuideMsgData(m_strTextParam, gData.GuideCurrentNode, m_nSpriteParam, _bModal: m_bModal, _userData: m_userData);

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEMESSAGE, msg);

            return base.onExecute(wData);
        }

        public override string ToString()
        {
            return m_strToStringDebugMessage;
        }
    }
    /// <summary>
    /// 通知UI隐藏引导信息
    /// </summary>
    class NOD_HideGuideUIInfo : TBTActionLeaf
    {
        private bool m_bIsRemoveMessage;
        private bool m_bIsRemoveModal;
        private bool m_bIsHideView;

        private const string m_strDefaultDebugMessage = "NOD_HideGuideUIInfo Node:{0}";

        public NOD_HideGuideUIInfo(bool _bRemoveMessage = true, bool _bRemoveModal = true, bool _bHideView = true)
        {
            m_bIsRemoveMessage = _bRemoveMessage;
            m_bIsRemoveModal = _bRemoveModal;
            m_bIsHideView = _bHideView;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            USpeedUI.UGuideMsgRemoveData msg = new USpeedUI.UGuideMsgRemoveData(m_bIsRemoveMessage, m_bIsRemoveModal);
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, msg);

            return base.onExecute(wData);
        }

        public override string ToString()
        {
            return m_strDefaultDebugMessage;
        }
    }
    /// <summary>
    /// 通知UI显示当前引导行为的引导控件
    /// </summary>
    class NOD_ShowGuideUIWidget : TBTActionLeaf
    {
        private string m_strToStringDebugMessage;
        private const string m_strDefaultDebugMessage = "NOD_ShowGuideUIWidget Node:{0}";

        private int m_nStepID;
        private bool m_bDoOnce;
        private bool m_bClickWidgetFinished;
        public NOD_ShowGuideUIWidget(int _nStepID, bool _bClickWidgetFinished = false)
        {
            m_nStepID = _nStepID;
            m_bClickWidgetFinished = _bClickWidgetFinished;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            ShowWidget(gData);

            base.onEnter(wData);
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            int result = base.onExecute(wData);

            result = ShowWidget(gData);

            if (TBTRunningStatus.IsFinished(result))
            {
                result = m_bClickWidgetFinished ? TBTRunningStatus.USER_EXECUTING : result;
                UGuideWidget widget = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nStepID);
                if (widget != null && widget.IsWidgetClick())
                {
                    result = TBTRunningStatus.FINISHED;
                }
            }

            return result;
        }

        protected int ShowWidget(GuideWorkingData _data)
        {
            if (m_bDoOnce)
            {
                return TBTRunningStatus.USER_FINISHED;
            }

            UGuideWidget widget = GuideManager.Instance.GetGuideWidget(_data.GuideCurrentNode, m_nStepID);

            if (widget != null)
            {
                if (m_strToStringDebugMessage == null)
                    m_strToStringDebugMessage = string.Format(m_strDefaultDebugMessage, _data.GuideCurrentNode, widget.ToString());

                widget.Show();
                m_bDoOnce = true;
                return TBTRunningStatus.USER_FINISHED;
            }

            return TBTRunningStatus.USER_EXECUTING;
        }


        public override string ToString()
        {
            return m_strToStringDebugMessage;
        }
    }
    /// <summary>
    /// 通知UI隐藏当前引导行为的引导控件
    /// </summary>
    class NOD_HideGuideUIWidget : TBTActionLeaf
    {
        private string m_strToStringDebugMessage;
        private const string m_strDefaultDebugMessage = "NOD_HideGuideUIWidget Node:{0}";
        private int m_nStepID;

        public NOD_HideGuideUIWidget(int _nStepID)
        {
            m_nStepID = _nStepID;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            UGuideWidget widget = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nStepID);

            if (widget != null)
            {
                if (m_strToStringDebugMessage == null)
                    m_strToStringDebugMessage = string.Format(m_strDefaultDebugMessage, gData.GuideCurrentNode, widget.ToString());

                widget.Hide();
            }

            base.onEnter(wData);
        }
    }
    /// <summary>
    /// 搜索附近是否有敌方阵营的实体
    /// </summary>
    class NOD_CheckAroundEnemy : TBTActionLeaf
    {
        protected bool m_bDoOnce = false;
        private string m_strTextParam;
        private int m_nSpriteParam;
        private int m_nSearchNearRadius;

        public NOD_CheckAroundEnemy(string _strTextParam, int _nSpriteParam, int _nSearchNearRadius)
        {
            m_strTextParam = _strTextParam;
            m_nSpriteParam = _nSpriteParam;
            m_nSearchNearRadius = _nSearchNearRadius;
        }
        protected override int onExecute(TBTWorkingData wData)
        {
            int runningState = TBTRunningStatus.USER_EXECUTING;

            if (EntityFactory.MainHeroView == null || !EntityFactory.MainHeroView.IsValid)
                return runningState;

            GuideWorkingData gData = wData as GuideWorkingData;

            IList<EntityView> AroundEntitiesList = MapFinderManager.Nearest(EntityFactory.MainHeroView.StateMachine.transform.position, m_nSearchNearRadius, SPELL.CampFlag.CampFlag_Enemy);
            //检测附近范围内有没有敌人
            runningState = AroundEntitiesList.Count == 0 ? TBTRunningStatus.USER_EXECUTING : TBTRunningStatus.USER_FINISHED;

            if (runningState == TBTRunningStatus.USER_FINISHED && !m_bDoOnce)
            {//有敌人则记录并显示UI
                m_bDoOnce = true;
                USpeedUI.UGuideMsgData msg = new USpeedUI.UGuideMsgData(m_strTextParam, gData.GuideCurrentNode, m_nSpriteParam);
                USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEMESSAGE, msg);
            }

            return runningState;
        }

        public override string ToString()
        {
            return "检测附近敌人";
        }
    }
    /// <summary>
    /// 搜索附近是否有指定实体属性ID的实体
    /// </summary>
    class NOD_CheckAroundEntityByPropertyID : TBTActionLeaf
    {
        protected bool m_bDoOnce = false;
        private int m_nPropertyID;
        private int m_nSearchNearRadius;
        private SPELL.CampFlag m_eCampFlag;

        public NOD_CheckAroundEntityByPropertyID(int _nPropertyID, int _nSearchNearRadius, SPELL.CampFlag _eCampFlag = SPELL.CampFlag.CampFlag_Enemy)
        {
            m_nPropertyID = _nPropertyID;
            m_nSearchNearRadius = _nSearchNearRadius;
            m_eCampFlag = _eCampFlag;
        }
        protected override int onExecute(TBTWorkingData wData)
        {
            int runningState = TBTRunningStatus.USER_EXECUTING;

            if (EntityFactory.MainHeroView == null || !EntityFactory.MainHeroView.IsValid)
                return runningState;

            GuideWorkingData gData = wData as GuideWorkingData;

            List<EntityView> AroundEntitiesList = MapFinderManager.Nearest(EntityFactory.MainHeroView.StateMachine.transform.position, m_nSearchNearRadius, m_eCampFlag).ToList();

            if (AroundEntitiesList != null)
            {
                for (int i = AroundEntitiesList.Count - 1; i >= 0; i--)
                {
                    if (AroundEntitiesList[i].Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != m_nPropertyID)
                    {
                        AroundEntitiesList.RemoveAt(i);
                    }
                }

                //检测附近范围内有没有敌人
                runningState = AroundEntitiesList.Count == 0 ? TBTRunningStatus.USER_EXECUTING : TBTRunningStatus.USER_FINISHED;
            }


            return runningState;
        }

        public override string ToString()
        {
            return "检测附近敌人";
        }
    }
    /// <summary>
    /// 显示引导路径模型
    /// </summary>
    class NOD_ShowPathMesh : TBTActionLeaf
    {
        private SplineMesh m_SplineMesh = null;
        private Vector3 m_vec3SourcePosition = Vector3.zero;
        private Transform m_MainHeroTrans = null;
        private EntityView m_CurrentTargetEntity = null;
        private int[] m_nMonsterIDs;

        private GameObject m_Prefab;
        private SceneEffectParamBase param;
        private int m_nCheckPosEquleRange = -1;
        public NOD_ShowPathMesh(Transform _tranSource = null, GameObject _go = null, int _nCheckPosEquleRange = -1, params int[] _nMonsterIDs)
        {
            m_MainHeroTrans = _tranSource;
            m_nMonsterIDs = _nMonsterIDs;
            m_nCheckPosEquleRange = _nCheckPosEquleRange;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            m_MainHeroTrans = EntityFactory.MainHeroView.StateMachine.transform;

            InitSplineMesh();
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            //这里需要让他一直执行，否则不会更新样线
            int runningState = TBTRunningStatus.USER_EXECUTING;

            bool bIme = InitTarget();

            if (bIme ||
                (m_SplineMesh.gameObject.activeSelf && m_MainHeroTrans.position != m_vec3SourcePosition))
            {
                m_vec3SourcePosition = m_MainHeroTrans.position;
                SetSplineMeshPoint(0, m_vec3SourcePosition);

                m_SplineMesh.UpdateMesh();

                //Vector3 vec3Current = m_SplineMesh.Spline.GetLocationAtSplinePoint(0, ESplineCoordinateSpace.World);
                //Vector3 vec3Next = m_SplineMesh.Spline.GetLocationAtSplinePoint(1, ESplineCoordinateSpace.World);
                //if (vec3Current.IsNearlyEquals(vec3Next, 2))
                //{
                //    if (m_SplineMesh.Spline.GetNumberOfSplinePoints() == 2)
                //        return TBTRunningStatus.FINISHED;
                //    m_SplineMesh.Spline.RemoveSplinePoint(0);
                //}
            }

            if (m_nCheckPosEquleRange != -1 && GetfSplineMeshPoint(1).IsNearlyEquals(m_vec3SourcePosition, m_nCheckPosEquleRange))
                runningState = TBTRunningStatus.FINISHED;
            return runningState;
        }

        protected override void onExit(TBTWorkingData wData, int runningStatus)
        {
            base.onExit(wData, runningStatus);
            SceneEffectManager.Instance.DestroySceneEffect(ref param);
            GameObject splineMeshGO = m_SplineMesh.gameObject;
            ResNode.DestroyRes(ref splineMeshGO);
            m_MainHeroTrans = null;
            m_SplineMesh = null;
            m_Prefab = null;
        }

        public bool InitTarget()
        {
            if (m_SplineMesh != null)
            {
                IList<EntityView> MonsterEntitiesList = EntityFactory.GetMonsterList().ToList();

                if (MonsterEntitiesList.Count <= 0)
                {
                    m_SplineMesh.gameObject.SetActive(false);
                    return false;
                }
                //这里目前来说可以找到移动定位点，如果有其他敌方怪物在中间的话就有问题了
                EntityView tmpTarget = null;

                foreach (var ev in MonsterEntitiesList)
                {
                    if (ev.Property == null)
                        continue;

                    for (int i = 0; i < m_nMonsterIDs.Length; ++i)
                    {
                        if (ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == m_nMonsterIDs[i])
                        {
                            tmpTarget = ev;
                            break;
                        }
                    }

                    if (tmpTarget != null)
                        break;
                }
                if (tmpTarget == null)
                {
                    m_SplineMesh.gameObject.SetActive(false);
                    return false;
                }

                m_SplineMesh.gameObject.SetActive(true);

                //
                if (m_CurrentTargetEntity == null ||
                    m_CurrentTargetEntity.Property == null ||
                    (m_CurrentTargetEntity != null && m_CurrentTargetEntity.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) != tmpTarget.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID)) ||
                    (GetfSplineMeshPoint(1) != m_CurrentTargetEntity.gameObject.transform.position)
                    )
                {
                    m_CurrentTargetEntity = tmpTarget;
                    SetSplineMeshPoint(1, m_CurrentTargetEntity.gameObject.transform.position);
                    return true;
                }

            }
            return false;
        }

        public void InitSplineMesh()
        {
            if (m_SplineMesh != null)
            {
                return;
            }

            param = new SceneEffectParamBase((int)ESceneEffectID.SE_Guide_PathMesh, null, _bAutoPlay: false);
            SceneEffectManager.Instance.CreateSceneEffect(ref param);
            param.ReturnComponent.transform.position = new Vector3(999, 999, 999);
            m_Prefab = param.ReturnComponent.gameObject;


            GameObject pathGO = new GameObject("PathMesh");
            m_SplineMesh = pathGO.AddComponent<SplineMesh>();
            m_SplineMesh.IsAutoUpdateMesh = false;
            m_SplineMesh.MeshDistanceDelta = 4f;
            m_SplineMesh.MeshPrefab = m_Prefab;

            m_SplineMesh.transform.position = m_MainHeroTrans != null ? m_MainHeroTrans.position : Vector3.zero;

            m_SplineMesh.gameObject.SetActive(false);

        }

        public void SetSplineMeshPoint(int _nPointIndex, Vector3 _vec3Pos)
        {
            InitSplineMesh();

            //if(!m_SplineMesh.Spline.GetLocationAtSplinePoint(_nPointIndex,ESplineCoordinateSpace.World).IsNearlyEquals(_vec3Pos,0.5f))
            {
                m_SplineMesh.Spline.SetLocationAtSplinePoint(_nPointIndex, _vec3Pos, ESplineCoordinateSpace.World);
            }
        }

        public Vector3 GetfSplineMeshPoint(int _nPointIndex)
        {
            InitSplineMesh();

            return m_SplineMesh.Spline.GetLocationAtSplinePoint(_nPointIndex, ESplineCoordinateSpace.World);

        }

        public void Execute()
        {
            onExecute(null);
        }
    }


    /// <summary>
    /// 在UI上显示指定的引导光效
    /// </summary>
    class NOD_ShowGuide_Effect : TBTActionLeaf
    {
        protected int m_nStepID;
        protected int m_nEffectID;

        public NOD_ShowGuide_Effect(int _nStepID, int _nEffectID)
        {
            m_nStepID = _nStepID;
            m_nEffectID = _nEffectID;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);
            GuideWorkingData gData = wData as GuideWorkingData;

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_ADDGUIDEEFFECT,
new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nStepID, m_nEffectID));
        }
    }
    /// <summary>
    /// 隐藏UI上指定的引导光效
    /// </summary>
    class NOD_HideGuide_Effect : TBTActionLeaf
    {
        protected int m_nStepID;
        protected int m_nEffectID;

        public NOD_HideGuide_Effect(int _nStepID, int _nEffectID)
        {
            m_nStepID = _nStepID;
            m_nEffectID = _nEffectID;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);
            GuideWorkingData gData = wData as GuideWorkingData;

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_REMOVEGUIDEEFFECT,
new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nStepID, m_nEffectID));
        }
    }

    /// <summary>
    /// 使当前引导行为节点一直执行，多数用于buff控制的引导，等待下一个buff
    /// </summary>
    class NOD_AlwaysExecting : TBTActionLeaf
    {
        protected override int onExecute(TBTWorkingData wData)
        {
            return TBTRunningStatus.EXECUTING;
        }
    }
    /// <summary>
    /// 在UI上显示自动隐藏的引导信息
    /// </summary>
    class NOD_ShowAutoGuideUIInfoParam : TBTActionLeaf
    {
        private int m_nCurrentID = -1;
        private int m_nAutoGuideMessageID;
        public NOD_ShowAutoGuideUIInfoParam(int _nAutoGuideMessageID = -1)
        {
            m_nAutoGuideMessageID = _nAutoGuideMessageID;
        }
        public NOD_ShowAutoGuideUIInfoParam()
        {
            m_nAutoGuideMessageID = -1;
        }

        protected override bool onEvaluate(TBTWorkingData wData)
        {
            //GuideWorkingData gData = wData as GuideWorkingData;

            //bool IsAutoGuideUIPlaying = gData.GuideBlackBoard.GetValue<bool>(EGuideBBKey.IsAutoGuideMessagePlaying, false);
            //int AutoGuideMessageID = gData.GuideBlackBoard.GetValue<int>(EGuideBBKey.AutoGuideMessageID, -1);

            //if((!IsAutoGuideUIPlaying && AutoGuideMessageID != -1) || (IsAutoGuideUIPlaying && AutoGuideMessageID != m_nCurrentID))
            //{
            //    gData.GuideBlackBoard.SetValue(EGuideBBKey.IsAutoGuideMessagePlaying, true);
            //    m_nCurrentID = AutoGuideMessageID;
            //    return true;
            //}

            //return false;
            return true;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            bool IsAutoGuideUIPlaying = gData.GuideDynamicBB.GetValue<bool>(EGuideBBKey.IsAutoGuideMessagePlaying, false);
            int AutoGuideMessageID = m_nAutoGuideMessageID == -1 ? gData.GuideDynamicBB.GetValue<int>(EGuideBBKey.AutoGuideMessageID, -1) : m_nAutoGuideMessageID;

            //if ((!IsAutoGuideUIPlaying && AutoGuideMessageID != -1 && AutoGuideMessageID != m_nCurrentID))
            if ((AutoGuideMessageID != -1))//&& AutoGuideMessageID != m_nCurrentID))
            {
                gData.GuideDynamicBB.SetValue(EGuideBBKey.IsAutoGuideMessagePlaying, true);
                m_nCurrentID = AutoGuideMessageID;

                SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData((EGuideNodeID)m_nCurrentID);

                int nDelayRemoveTime = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
                if (guideNodeData.GuideIntParams.Count > (int)EGuideNodeConfigParamID.GuideParamID_1)
                {
                    int nSoundID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];

                    SoundManager.CreateUISound(nSoundID);
                }

                string strMessage = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];


                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_ADDAUTOMESSAGE, new UGuideMsgData(strMessage, (EGuideNodeID)m_nCurrentID, nDelayRemoveTime));

                gData.GuideDynamicBB.SetValue(EGuideBBKey.AutoGuideMessageID, -1);
            }


            base.onEnter(wData);
        }
    }
    /// <summary>
    /// 在UI上显示一直显示的引导信息
    /// </summary>
    class NOD_ShowAlwaysGuideUIInfoParam : TBTActionLeaf
    {
        private string m_strShowMessage;
        public NOD_ShowAlwaysGuideUIInfoParam(string _strMessage)
        {
            m_strShowMessage = _strMessage;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;


            UGuideMsgData data = new UGuideMsgData(m_strShowMessage, gData.GuideCurrentNode);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_ALWAYMESSAGE, data);

            return base.onExecute(wData);
        }
    }

    /// <summary>
    /// 检测主玩家相机旋转
    /// </summary>
    class NOD_CheckRotateCamera : TBTActionLeaf
    {
        private int m_nCheckRotateAngleDelta;
        private int m_nBeginAngle;
        private bool m_bHoriDir;

        public NOD_CheckRotateCamera(int _nCheckRotateAngleDelta, bool _bHoriDir)
        {
            m_nCheckRotateAngleDelta = _nCheckRotateAngleDelta;
            m_bHoriDir = _bHoriDir;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            m_nBeginAngle = 0;
            if (SoldierCamera.MainInstance() != null && SoldierCamera.MainInstance().target != null)
            {
                if (m_bHoriDir)
                    m_nBeginAngle = (int)SoldierCamera.MainInstance().target.rotation.eulerAngles.y;
                else
                    m_nBeginAngle = (int)SoldierCamera.MainInstance().target.rotation.eulerAngles.x;
            }



            base.onEnter(wData);
        }


        protected override int onExecute(TBTWorkingData wData)
        {
            int runningState = TBTRunningStatus.USER_EXECUTING;
            if (SoldierCamera.MainInstance<SoldierCamera>() != null && SoldierCamera.MainInstance<SoldierCamera>().target != null)
            {
                runningState = DeltaRotate() > m_nCheckRotateAngleDelta ? TBTRunningStatus.FINISHED : TBTRunningStatus.EXECUTING;

                //if(runningState == TBTRunningStatus.FINISHED)
                //    EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_GUIDE_ROTATECAMERA);

            }

            return runningState;//TBTRunningStatus.USER_EXECUTING;
        }

        private int DeltaRotate()
        {
            if (m_bHoriDir)
                return (int)Mathf.Abs(SoldierCamera.MainInstance<SoldierCamera>().target.rotation.eulerAngles.y - m_nBeginAngle);
            else
                return (int)Mathf.Abs(SoldierCamera.MainInstance<SoldierCamera>().target.rotation.eulerAngles.x - m_nBeginAngle);
        }
    }
    /// <summary>
    /// 播放引导行为指定的引导音效
    /// </summary>
    class NOD_PlaySound : TBTActionLeaf
    {
        protected int m_nSoundID;
        public NOD_PlaySound(int _nSoundID)
        {
            m_nSoundID = _nSoundID;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            SoundManager.CreateUISound(m_nSoundID);
        }


    }
    /// <summary>
    /// 发送目标界面消息
    /// </summary>
    class NOD_SendTargetWndMessage : TBTActionLeaf
    {
        protected WndID m_eGuideActonWnd;
        protected WndMsgID m_eGuideActionMsgID;
        protected UIMsgData m_widgetMsgData;

        public NOD_SendTargetWndMessage(WndID _eGuideActionWnd, WndMsgID _eMsgID, UIMsgData _widgetMsgData = null)
        {
            m_eGuideActonWnd = _eGuideActionWnd;
            m_eGuideActionMsgID = _eMsgID;
            m_widgetMsgData = _widgetMsgData;
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);
            UISystem.Instance.SendTargetWndMessage(m_eGuideActonWnd, m_eGuideActionMsgID, m_widgetMsgData);
        }

    }

    /// <summary>
    /// 控制游戏输入状态
    /// </summary>
    class NOD_SetInputManagerEnable : TBTActionLeaf
    {
        protected bool m_bEnable;

        public NOD_SetInputManagerEnable(bool _bEnable)
        {
            m_bEnable = _bEnable;
        }
        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);
            InputManager.Available = m_bEnable;
        }

    }
    /// <summary>
    /// 设置当前的引导行为节点ID
    /// </summary>
    class NOD_SetCurrentGuideNode : TBTActionLeaf
    {
        protected EGuideNodeID m_eGuideNodeID;

        public NOD_SetCurrentGuideNode(EGuideNodeID _eGuideNodeID)
        {
            m_eGuideNodeID = _eGuideNodeID;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            wData.As<GuideWorkingData>().GuideDynamicBB.SetValue(EGuideBBKey.LogicStepID, m_eGuideNodeID);

            return base.onExecute(wData);
        }
    }

    /// <summary>
    /// 发送实体消息到逻辑层
    /// </summary>
    class NOD_EntityEventHelper : TBTActionLeaf
    {
        private EntityLogicDef m_eCommandID;
        private int m_nParam;
        private string m_strParam;

        public NOD_EntityEventHelper(EntityLogicDef _eCommandID, int _nParam = 0, string _strParam = "")
        {
            m_eCommandID = _eCommandID;
            m_nParam = _nParam;
            m_strParam = _strParam;
        }
        protected override int onExecute(TBTWorkingData wData)
        {
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, m_eCommandID, m_nParam, m_strParam);
            return base.onExecute(wData);
        }
    }
    /// <summary>
    /// 自由对战引导
    /// </summary>
    class NOD_WaitForFreeMatchFinish : TBTActionLeaf
    {
        private int m_nEffectID;
        private int m_nGameModeID;
        private int m_nMatchLimit;

        public NOD_WaitForFreeMatchFinish(int _nEffectID, int _nGameModeID, int _nMatchLimit)
        {
            m_nEffectID = _nEffectID;
            m_nGameModeID = _nGameModeID;
            m_nMatchLimit = _nMatchLimit;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, 0);
            if (widget0 != null && widget0.gameObject.activeInHierarchy && widget0.gameObject.activeSelf && !widget0.IsWidgetShow())
                widget0.Show();
            else if (widget0 == null)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET,
                    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID, _bClickEvent: false));
            }

            UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, 1);
            if (widget1 != null && widget1.gameObject.activeInHierarchy && widget1.gameObject.activeSelf && !widget1.IsWidgetShow())
                widget1.Show();
            else if (widget1 == null)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET,
                    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 1, m_nEffectID, _oUserData: m_nGameModeID, _bClickEvent: false));
            }

            int result = TBTRunningStatus.EXECUTING;

            if (LogicDataCenter.lifeHeroDataManager.HistoryRecord.Count >= m_nMatchLimit)
            {
                result = base.onExecute(wData);
            }

            return result;
        }

        protected override void onExit(TBTWorkingData wData, int runningStatus)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, 0);
            if (widget0 != null)
                widget0.Hide();

            UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, 1);
            if (widget1 != null)
                widget1.Hide();

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_REMOVEGUIDEWIDGET,
                                                   new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID));
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_REMOVEGUIDEWIDGET,
                                                   new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 1, m_nEffectID, _oUserData: m_nGameModeID));

            base.onExit(wData, runningStatus);
        }
    }
    /// <summary>
    /// 结束等待完成排位模式引导
    /// </summary>
    class NOD_WaitForGameModeClickEnd : TBTActionLeaf
    {
        private int m_nEffectID;
        private int m_nGameModeID;
        private int m_nHallBtnStepID;
        private int m_nGameModeStepID;
        public NOD_WaitForGameModeClickEnd(int _nGameModeID, int _nEffectID, int _nHallBtnStepID = 0, int _nGameModeStepID = 1)
        {
            m_nEffectID = _nEffectID;
            m_nGameModeID = _nGameModeID;
            m_nHallBtnStepID = _nHallBtnStepID;
            m_nGameModeStepID = _nGameModeStepID;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nHallBtnStepID);
            if (widget0 != null)
                widget0.Hide();

            UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nGameModeStepID);
            if (widget1 != null)
                widget1.Hide();

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_REMOVEGUIDEWIDGET,
    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nHallBtnStepID, m_nEffectID));
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_REMOVEGUIDEWIDGET,
    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nGameModeStepID, m_nEffectID, _oUserData: m_nGameModeID));

            return base.onExecute(wData);
        }

    }
    /// <summary>
    /// 开始完成排位模式引导
    /// </summary>
    class NOD_WaitForGameModeClickBegin : TBTActionLeaf
    {
        private int m_nEffectID;
        private int m_nGameModeID;
        private int m_nHallBtnStepID;
        private int m_nGameModeStepID;

        public NOD_WaitForGameModeClickBegin(int _nGameModeID, int _nEffectID, int _nHallBtnStepID = 0, int _nGameModeStepID = 1)
        {
            m_nEffectID = _nEffectID;
            m_nGameModeID = _nGameModeID;
            m_nHallBtnStepID = _nHallBtnStepID;
            m_nGameModeStepID = _nGameModeStepID;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nHallBtnStepID);
            if (widget0 != null && widget0.gameObject.activeInHierarchy && widget0.gameObject.activeSelf && !widget0.IsWidgetShow())
                widget0.Show();
            else if (widget0 == null)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET,
                    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nHallBtnStepID, m_nEffectID, _bClickEvent: false));
            }

            UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, m_nGameModeStepID);
            if (widget1 != null && widget1.gameObject.activeInHierarchy && widget1.gameObject.activeSelf && !widget1.IsWidgetShow())
                widget1.Show();
            else if (widget1 == null)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET,
                    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, m_nGameModeStepID, m_nEffectID, _oUserData: m_nGameModeID, _bClickEvent: false));
            }

            int result = TBTRunningStatus.EXECUTING;

            //UGuideWidget widget = GuideManager.Instance.GetGuideWidget(gData.GuideCurrentNode, 1);

            //if (widget != null && widget.IsWidgetClick())
            //{
            //    result = base.onExecute(wData);
            //}

            if (LogicDataCenter.playerSystemDataManager != null && LogicDataCenter.lifeHeroDataManager.LifeHeroMatchType == ELifeHeroMatchType.HAVE_MatchType_Rank)
            {
                result = base.onExecute(wData);
            }

            return result;
        }
    }

    /// <summary>
    /// 本命英雄未满时引导处理逻辑
    /// </summary>
    class NOD_NotFullLifeHero : TBTActionLeaf
    {
        protected IFSM_Base[] m_LifeHeroState;
        private ELifeHeroState? m_eCurrentState;
        private LifeHeroStateParam m_nStateParam;

        public int EffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;
        public int EffectID2 = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;

        #region 本命英雄数据状态
        enum ELifeHeroState
        {
            /// <summary>
            /// 没有可激活的本命英雄
            /// </summary>
            NoActiveLifeHero,
            /// <summary>
            /// 有可激活的本命英雄
            /// </summary>
            ActiveLifeHero,

            LifeHeroStateMax
        }
        class NoActiveLifeHeroState : IFSM_Base
        {
            protected bool m_bDoOnce;

            public int Enter(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                //if(!m_bDoOnce)
                //{
                //    m_bDoOnce = true;

                //    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET,
                //        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 0, gData.ActionLeaf.EffectID,_bClickEvent:false));

                //    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET,
                //        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 1, gData.ActionLeaf.EffectID, _oUserData: 1,_bClickEvent:false));
                //}

                return 0;
            }

            public int Execute(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 0);
                if (widget0 != null && widget0.gameObject.activeInHierarchy && widget0.gameObject.activeSelf && !widget0.IsWidgetShow())
                    widget0.Show();
                else if (widget0 == null)
                {
                    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET,
                        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 0, gData.ActionLeaf.EffectID, _bClickEvent: false));
                }

                UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 1);
                if (widget1 != null && widget1.gameObject.activeInHierarchy && widget1.gameObject.activeSelf && !widget1.IsWidgetShow())
                    widget1.Show();
                else if (widget1 == null)
                {
                    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET,
                        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 1, gData.ActionLeaf.EffectID, _oUserData: 1, _bClickEvent: false));
                }
                return 0;
            }

            public int Exit(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 0);
                if (widget0 != null)
                    widget0.Hide();
                UGuideWidget widget1 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 1);
                if (widget1 != null)
                    widget1.Hide();

                return 0;
            }

            public uint GetStateID()
            {
                return (uint)ELifeHeroState.NoActiveLifeHero;
            }
        }
        class ActiveLifeHeroState : IFSM_Base
        {
            protected bool m_bDoOnce;

            public int Enter(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                //if (!m_bDoOnce)
                //{
                //    m_bDoOnce = true;

                //    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_ENTRANCE, WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONBEGIN,
                //        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 10, gData.ActionLeaf.EffectID2,_bClickEvent:false));
                //}


                return 0;
            }

            public int Execute(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 10);
                if (widget0 != null && widget0.gameObject.activeInHierarchy && widget0.gameObject.activeSelf && !widget0.IsWidgetShow())
                    widget0.Show();
                else if (widget0 == null)
                {
                    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_ENTRANCE, WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONBEGIN,
                        new UGuideWidgetMsgData((int)gData.WorkingData.GuideCurrentNode, 10, gData.ActionLeaf.EffectID2, _bClickEvent: false));

                }


                return 0;
            }

            public int Exit(IFSMParam _data)
            {
                LifeHeroStateParam gData = _data as LifeHeroStateParam;

                UGuideWidget widget0 = GuideManager.Instance.GetGuideWidget(gData.WorkingData.GuideCurrentNode, 10);
                if (widget0 != null)
                    widget0.Hide();

                return 0;
            }

            public uint GetStateID()
            {
                return (uint)ELifeHeroState.ActiveLifeHero;
            }
        }

        class LifeHeroStateParam : IFSMParam
        {
            public GuideWorkingData WorkingData;
            public NOD_NotFullLifeHero ActionLeaf;
        }
        #endregion

        public NOD_NotFullLifeHero()
        {
            SetPrecondition(new TBTPreconditionNOT(new CON_IsLisfHeroFullfill()));

            m_LifeHeroState = new IFSM_Base[(int)ELifeHeroState.LifeHeroStateMax];
            m_LifeHeroState[(uint)ELifeHeroState.NoActiveLifeHero] = new NoActiveLifeHeroState();
            m_LifeHeroState[(uint)ELifeHeroState.ActiveLifeHero] = new ActiveLifeHeroState();

            m_nStateParam = new LifeHeroStateParam();
            //策划需要改为不引导进入本命英雄仓
            ChangeState(ELifeHeroState.NoActiveLifeHero);

        }

        private IFSM_Base GetCurrentState()
        {
            if (m_eCurrentState == null)
                return null;
            return m_LifeHeroState[(uint)m_eCurrentState];
        }

        private void ChangeState(ELifeHeroState _eNewState)
        {
            if (m_eCurrentState == _eNewState)
                return;

            if (GetCurrentState() != null)
                GetCurrentState().Exit(m_nStateParam);

            m_eCurrentState = _eNewState;

            GetCurrentState().Enter(m_nStateParam);
        }

        protected override void onEnter(TBTWorkingData wData)
        {
            base.onEnter(wData);
            m_nStateParam.WorkingData = wData as GuideWorkingData;
            m_nStateParam.ActionLeaf = this;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            //策划需要改为不引导进入本命英雄仓
            //if (LogicDataCenter.lifeHeroDataManager.GetLifeHeroActivateType() == LifeHeroActivateType.NO_ACTIVATION)
            //    ChangeState(ELifeHeroState.NoActiveLifeHero);
            //else if (LogicDataCenter.lifeHeroDataManager.GetLifeHeroActivateType() == LifeHeroActivateType.CAN_ACTIVATION)
            //    ChangeState(ELifeHeroState.ActiveLifeHero);
            //else
            //    m_eCurrentState = null;


            if (m_eCurrentState != null)
            {
                m_LifeHeroState[(uint)m_eCurrentState].Execute(m_nStateParam);
            }

            return base.onExecute(wData);
        }

        protected override void onExit(TBTWorkingData wData, int runningStatus)
        {
            base.onExit(wData, runningStatus);
        }

    }
    /// <summary>
    /// 设值主引导步奏
    /// </summary>
    class NOD_SetLogicNextStep : TBTActionLeaf
    {
        public int nStepID;
        public NOD_SetLogicNextStep(int stepID)
        {
            nStepID = stepID;
        }
        protected override void onEnter(TBTWorkingData wData)
        {
            GuideManager.Instance.SetNoviceGuideType(nStepID);
        }
    }

    /// <summary>
    /// 通知UI显示当前奖励引导行为信息
    /// </summary>
    class NOD_ShowGuideRewardInfoParam : TBTActionLeaf
    {
        private string m_strTextParam;
        private bool m_bModal;

        private string m_strToStringDebugMessage;
        private const string m_strDefaultDebugMessage = "NOD_ShowGuideRewardInfoParam Node:{0},TextParam:{1};";

        /// <summary>
        /// 设置UI要显示时需要的数据参数ID
        /// </summary>
        /// <param name="_eTextParamID">字符串参数ID</param>
        /// <param name="_eSpriteParamID">整型参数ID</param>
        /// <param name="_strDebugMessage">Debug打印信息</param>
        public NOD_ShowGuideRewardInfoParam(string _strTextParam, string _strDebugMessage = null, bool _bModal = false) : base()
        {
            m_strTextParam = _strTextParam;
            m_strToStringDebugMessage = _strDebugMessage;
            m_bModal = _bModal;
        }
        protected override void onEnter(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            if (m_strToStringDebugMessage == null)
                m_strToStringDebugMessage = string.Format(m_strDefaultDebugMessage, gData.GuideCurrentNode, m_strTextParam);

            base.onEnter(wData);
        }
        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            USpeedUI.UGuideMsgData msg = new USpeedUI.UGuideMsgData(m_strTextParam, gData.GuideCurrentNode, _bModal: m_bModal);

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEREWARDMESSAGE, msg);

            return base.onExecute(wData);
        }

        public override string ToString()
        {
            return m_strToStringDebugMessage;
        }
    }
    /// <summary>
    /// 通知UI隐藏奖励引导信息
    /// </summary>
    class NOD_HideGuideRewardInfo : TBTActionLeaf
    {
        private bool m_bIsRemoveMessage;
        private bool m_bIsRemoveModal;
        private bool m_bIsHideView;

        private const string m_strDefaultDebugMessage = "NOD_HideGuideUIInfo Node:{0}";

        public NOD_HideGuideRewardInfo(bool _bRemoveMessage = true, bool _bRemoveModal = true, bool _bHideView = true)
        {
            m_bIsRemoveMessage = _bRemoveMessage;
            m_bIsRemoveModal = _bRemoveModal;
            m_bIsHideView = _bHideView;
        }

        protected override int onExecute(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;

            USpeedUI.UGuideMsgRemoveData msg = new USpeedUI.UGuideMsgRemoveData(m_bIsRemoveMessage, m_bIsRemoveModal);
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEREWARDMESSAGE, msg);

            return base.onExecute(wData);
        }

        public override string ToString()
        {
            return m_strDefaultDebugMessage;
        }
    }
}
