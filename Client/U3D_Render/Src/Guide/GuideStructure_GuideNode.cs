/*******************************************************************
** 文件名:	GuideStructure.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-8-23
** 版  本:	1.0
** 描  述:	新手引导树中用于管理引导节点所有行为的子树
		
********************************************************************/
using Common.FSMSample;
using DataCenter;
using GameLogic;
using Spline;
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
    ////////////////////////新手场景引导//////////////////////////////////////////////////////////////////////////

    abstract class GSD_GuideRoot_01BaseNodeData : GuideNodeDataParallel
    {
        protected int m_nLogicStepID = -1;
    }

    /// <summary>
    /// 基础操作引导
    /// </summary>
    class GSD_BasicOperation : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_0;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_0_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_5];

            //取字符串数据
            string strGuideTextParam0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            this.AddChild(new TBTActionSequence()
                .AddChild(new NOD_PlaySound(m_nSoundID_0))
                .AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1)))
                )
                .AddChild(new NOD_SetInputManagerEnable(true))
                .AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0, nGuideSignSpriteID));
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_5];
            SoundManager.DeleteSound(m_nSoundID_0);
            SoundManager.DeleteSound(m_nSoundID_1);
        }
    }

    /// <summary>
    /// 旋转镜头操作
    /// </summary>
    class GSD_RotateCamera : GuideNodeDataSequence//GSD_GuideRoot_01BaseNodeData
    {
        private int m_nEffectID;
        private int m_nEffectID2;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int nRotatorDelta = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_MouseRotate;
            m_nEffectID2 = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_MouseRotate2;


            //取字符串数据
            string strGuideTextParam0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            string strGuideTextParam1 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            string strGuideTextParam2 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)));

            AddChild(new NOD_ShowGuide_Effect(0, m_nEffectID));
            AddChild(new TBTActionParallel().SetRunningStatusRelationship(TBTActionParallel.ECHILDREN_RELATIONSHIP.AND)
                                            .AddChild(new NOD_CheckRotateCamera(nRotatorDelta, true))
                                            .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0, nGuideSignSpriteID)));
            AddChild(new NOD_HideGuide_Effect(0, m_nEffectID));
            AddChild(new NOD_ShowGuide_Effect(0, m_nEffectID2));
            AddChild(new TBTActionParallel().SetRunningStatusRelationship(TBTActionParallel.ECHILDREN_RELATIONSHIP.AND)
                                            .AddChild(new NOD_CheckRotateCamera(nRotatorDelta, false))
                                            .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam1, nGuideSignSpriteID)));
            AddChild(new NOD_HideGuide_Effect(0, m_nEffectID2));
            AddChild(new NOD_ShowGuide_Effect(0, m_nEffectID));
            AddChild(new TBTActionParallel().SetRunningStatusRelationship(TBTActionParallel.ECHILDREN_RELATIONSHIP.AND)
                                            .AddChild(new NOD_CheckRotateCamera(nRotatorDelta, true))
                                            .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam2, nGuideSignSpriteID)));

            AddChild(new NOD_EntityEventHelper(EntityLogicDef.ENTITY_CMD_GUIDE_ROTATECAMERA, 0));
            AddChild(new NOD_AlwaysExecting());
            //.AddChild(new NOD_CheckRotateCamera(nRotatorDelta,true))
            //.AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0, nGuideSignSpriteID));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            GuideWorkingData gData = wData as GuideWorkingData;

            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            SoundManager.DeleteSound(m_nSoundID_0);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_REMOVEGUIDEEFFECT,
new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID));
        }

    }

    /// <summary>
    /// shift加速与双击方向键
    /// </summary>
    class GSD_BasicOperation2 : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            //取字符串数据
            string strGuideTextParam0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            this.AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)))
                .AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0, nGuideSignSpriteID));
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            SoundManager.DeleteSound(m_nSoundID_0);

        }
    }

    /// <summary>
    /// 基础攻击引导
    /// </summary>
    class GSD_AttackOperation : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            this.AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)))
                .AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            SoundManager.DeleteSound(m_nSoundID_0);
        }
    }

    /// <summary>
    /// 3级天赋学习引导
    /// </summary>
    class GSD_TalentLevel3 : GSD_GuideRoot_01BaseNodeData
    {
        private int m_nTalentID;
        private int m_nEffectID;
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_4;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            m_nTalentID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int nGuideWidgetStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_TalentLearnItem;

            //消息 WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET 数据结构
            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(),0,m_nEffectID, _oUserData: m_nTalentID);


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            //this.AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
            AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)));
            AddChild(new NOD_SetInputManagerEnable(false));
            ////目前只能通过天赋ID来找到该天赋对应的显示组件所在的位置
            //通知HeroTalentWnd内的talentID显示组件添加GuideWidget
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WARHEROTALENTVIEW, WndMsgID.WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET, msgData));
            //通知WarBottomCenterWnd模拟点击WarTalentButton 达到显示天赋页效果
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WAR_BOTTOMCENTERVIEW, WndMsgID.WND_MSG_WARBUTTOMCENTER_WARTALENTBUTTONCLICK));
            //显示引导光效
            AddChild(new NOD_ShowGuideUIWidget(nGuideWidgetStepID));
            //显示引导描述
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));
            AddChild(new NOD_AlwaysExecting());
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WARHEROTALENTVIEW, WndMsgID.WND_ID_HEROTALENTVIEW_REMOVEGUIDEWIDGET,
    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID, m_nTalentID));
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];

            SoundManager.DeleteSound(m_nSoundID_0);
            InputManager.Available = true;
            base.onTransition(wData);
        }
    }

    /// <summary>
    /// 基础攻击引导
    /// </summary>
    class GSD_AttackOperation2 : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_5;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            this.AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)))
                .AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            SoundManager.DeleteSound(m_nSoundID_0);
        }
    }

    /// <summary>
    /// 6级天赋引导学习
    /// </summary>
    class GSD_TalentLevel6 : GSD_GuideRoot_01BaseNodeData
    {
        private int m_nTalentID;
        private int m_nEffectID;
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_6;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            m_nTalentID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int nGuideWidgetStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];

            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_TalentLearnItem;

            //消息 WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET 数据结构
            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _oUserData: m_nTalentID);


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            // this.AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
            AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)));
            AddChild(new NOD_SetInputManagerEnable(false));
            ////目前只能通过天赋ID来找到该天赋对应的显示组件所在的位置
            //通知HeroTalentWnd内的talentID显示组件添加GuideWidget
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WARHEROTALENTVIEW, WndMsgID.WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET, msgData));
            //通知WarBottomCenterWnd模拟点击WarTalentButton 达到显示天赋页效果
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WAR_BOTTOMCENTERVIEW, WndMsgID.WND_MSG_WARBUTTOMCENTER_WARTALENTBUTTONCLICK));
            //显示引导光效
            AddChild(new NOD_ShowGuideUIWidget(nGuideWidgetStepID));
            //显示引导描述
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));
            AddChild(new NOD_AlwaysExecting());

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            GuideWorkingData gData = wData as GuideWorkingData;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WARHEROTALENTVIEW, WndMsgID.WND_ID_HEROTALENTVIEW_REMOVEGUIDEWIDGET,
    new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID, m_nTalentID));

            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];

            SoundManager.DeleteSound(m_nSoundID_0);

            InputManager.Available = true;

            base.onTransition(wData);
        }
    }

    /// <summary>
    /// 攻击敌方英雄
    /// </summary>
    class GSD_AttackEnemy : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_7;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0));
            AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            SoundManager.DeleteSound(m_nSoundID_0);

        }
    }

    /// <summary>
    /// 摧毁敌方防御塔
    /// </summary>
    class GSD_AttackTower : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_8;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            this.SetEvaluationRelationship(ECHILDREN_RELATIONSHIP.OR);
            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_0_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_5];



            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0));
            AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0))
                                                 .AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1))));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_5];

            SoundManager.DeleteSound(m_nSoundID_0);
            SoundManager.DeleteSound(m_nSoundID_1);

        }
    }

    /// <summary>
    /// 摧毁敌方基地
    /// </summary>
    class GSD_AttackBase : GSD_GuideRoot_01BaseNodeData
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_9;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nGuideSignSpriteID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nLogicStepID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nMonsterID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];


            this.SetPrecondition(new CON_IsLogicStepID(m_nLogicStepID));
            this.AddChild(new TBTActionSequence().AddChild(new NOD_PlaySound(m_nSoundID_0)));
            this.AddChild(new NOD_ShowPathMesh(_nMonsterIDs: m_nMonsterID_0))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, nGuideSignSpriteID));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];

            SoundManager.DeleteSound(m_nSoundID_0);
        }
    }

    ///////////////////////////////新手人机战场引导/////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 显示选择线路提示
    /// </summary>
    class GSD_5v5_ChooseAWay : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1001;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nTowerID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int nRadius = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsLogicStepID(11)));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam));
            AddChild(new NOD_PlaySound(nSoundID));
            AddChild(new NOD_ShowPathMesh(_nMonsterIDs: nTowerID, _nCheckPosEquleRange: nRadius));
            AddChild(new NOD_DelayFinish());
        }
    }

    /// <summary>
    /// 显示推外塔
    /// </summary>
    class GSD_5v5_AttackTower1 : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1002;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            int nEntityPropertyID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            int nRadius = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];


            SPELL.CampFlag nEntityCampFlag = SPELL.CampFlag.CampFlag_Friend;

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsNearbyPropertyIDEntityExit(nEntityPropertyID, nRadius, nEntityCampFlag)));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam));
            AddChild(new TBTActionTimer().SetElapseTime(4)
                                         .AddChild(new TBTActionSequence()
                                                        .AddChild(new NOD_PlaySound(nSoundID_1))));
            //AddChild(new NOD_PlaySound(303).SetPrecondition(new CON_IsEntityDeadByPropertyID(2122, SPELL.CampFlag.CampFlag_Enemy)));
            AddChild(new NOD_DelayFinish());
        }
    }
    /// <summary>
    /// 显示推高塔
    /// </summary>
    class GSD_5v5_AttackTower2 : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1003;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            int nEntityPropertyID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];

            SPELL.CampFlag nEntityCampFlag = SPELL.CampFlag.CampFlag_Enemy;

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsEntityDeadByPropertyID(nEntityPropertyID, nEntityCampFlag)));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam));
            AddChild(new NOD_DelayFinish());
        }
    }

    /// <summary>
    /// 显示推水晶
    /// </summary>
    class GSD_5v5_AttackTower3 : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1004;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            int nEntityPropertyID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];

            SPELL.CampFlag nEntityCampFlag = SPELL.CampFlag.CampFlag_Enemy;

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsEntityDeadByPropertyID(nEntityPropertyID, nEntityCampFlag)));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam));
            AddChild(new NOD_DelayFinish());
        }
    }

    /// <summary>
    /// 显示推基地
    /// </summary>
    class GSD_5v5_AttackBase : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1005;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            int nEntityPropertyID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];

            SPELL.CampFlag nEntityCampFlag = SPELL.CampFlag.CampFlag_Enemy;

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsEntityDeadByPropertyID(nEntityPropertyID, nEntityCampFlag)));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam));
            AddChild(new NOD_DelayFinish());
        }
    }
    /// <summary>
    /// 飞行学习引导
    /// </summary>
    class GSD_5v5_LearnUseWing : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1006;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int _nWingBuffID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int _nAutoGuideMessageID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int _nAutoGuideMessageID2 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];
            KeyCode[] keys = { KeyCode.LeftShift, KeyCode.RightShift };

            SetPrecondition(new CON_IsBuffExit(_nWingBuffID));
            AddChild((new NOD_ShowAutoGuideUIInfoParam(_nAutoGuideMessageID))
    .SetPrecondition(new CON_IsPropertyUpToStandard((int)ENTITY_PROPERTY.PROPERTY_STAMINA, 50)));

            AddChild((new NOD_ShowAutoGuideUIInfoParam(_nAutoGuideMessageID2))
    .SetPrecondition(new TBTPreconditionNOT(new CON_IsPropertyUpToStandard((int)ENTITY_PROPERTY.PROPERTY_STAMINA, 50))));//(new CON_IsKeyDown(_eCheckKeyCode: keys)));
            //AddChild((new NOD_DelayFinish()).SetPrecondition(new CON_IsKeyDown(FuntionShortCutSlotIndex.UseWing)));
        }
    }
    /// <summary>
    /// 金钱溢出，购买引导
    /// </summary>
    class GSD_5v5_MuchMoney : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1007;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int _nMoneyCount = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int _nAutoGuideMessageID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int _nDelayTime = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            SetPrecondition(new CON_IsPropertyCompareStandardWithDelay((int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT, _nMoneyCount, _nDelayTime, _nCompareType: 2));
            AddChild((new NOD_ShowAutoGuideUIInfoParam(_nAutoGuideMessageID)));

        }
    }
    /// <summary>
    /// 低血量引导
    /// </summary>
    class GSD_Tips_LowHP : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_9001;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int _nStanderValue = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int _nAutoGuideMessageID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int _nDelayTime = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];


            SetPrecondition(new CON_IsPropertyCompareStandardWithDelay((int)ENTITY_PROPERTY.PROPERTY_HP, _nStanderValue, _nDelayTime, (int)ENTITY_PROPERTY.PROPERTY_MAX_HP, 0));
            AddChild((new NOD_ShowAutoGuideUIInfoParam(_nAutoGuideMessageID)));

        }
    }
    /// <summary>
    /// 低魔法值引导
    /// </summary>
    class GSD_Tips_LowMP : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_9002;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int _nStanderValue = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int _nAutoGuideMessageID = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int _nDelayTime = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            SetPrecondition(new CON_IsPropertyCompareStandardWithDelay((int)ENTITY_PROPERTY.PROPERTY_MP, _nStanderValue, _nDelayTime, (int)ENTITY_PROPERTY.PROPERTY_MAX_MP, 0));
            AddChild((new NOD_ShowAutoGuideUIInfoParam(_nAutoGuideMessageID)));

        }
    }

    /// <summary>
    /// 旋转镜头操作
    /// </summary>
    class GSD_5v5_RotateCamera : GuideNodeDataSequence//GSD_GuideRoot_01BaseNodeData
    {
        private int m_nEffectID;
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1008;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //取整型数据
            int nRotatorDelta = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_MouseRotate;


            //取字符串数据
            string strGuideTextParam0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            this.SetPrecondition(new CON_IsLogicStepID(10));
            this.AddChild(new NOD_PlaySound(m_nSoundID_0))
                .AddChild(new NOD_SetInputManagerEnable(true))
                .AddChild(new NOD_ShowGuide_Effect(0, m_nEffectID))
                .AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0))
                .AddChild(new NOD_CheckRotateCamera(nRotatorDelta, true))
                .AddChild(new NOD_EntityEventHelper(EntityLogicDef.ENTITY_CMD_GUIDE_ROTATECAMERA, 1))
                .AddChild(new NOD_HideGuide_Effect(0, m_nEffectID))
                .AddChild(new NOD_DelayFinish());

            //.AddChild(new NOD_CheckRotateCamera(nRotatorDelta,true))
            //.AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam0, nGuideSignSpriteID));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            GuideWorkingData gData = wData as GuideWorkingData;
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            SoundManager.DeleteSound(m_nSoundID_0);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEGUIDE, WndMsgID.WND_MSG_GAMEGUIDE_REMOVEGUIDEEFFECT,
new UGuideWidgetMsgData((int)gData.GuideCurrentNode, 0, m_nEffectID));
        }

    }

    class GSD_ReturnMainScene : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_1009;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //取字符串数据
            string strGuideTextParam = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //取整型数据
            int nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_ExitButton_huicheng;
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, nEffectID);
            UGuideWidgetMsgData msgData2 = new UGuideWidgetMsgData((int)GuideNodeID(), 0, 0, _bForceGuide: true);

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsWndViewVisible(true, WndID.WND_ID_WAR_RECORDTABLE_END)));
            //通知战场结束统计界面添加引导控件
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WAR_RECORDTABLE_END, WndMsgID.WND_MSG_RECORDTABLE_END_EXITBTN_ADDGUIDEWIDGET, msgData));
            AddChild(new NOD_SetInputManagerEnable(false));
            AddChild(new NOD_ShowGuideUIInfoParam(strGuideTextParam, 0));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new NOD_ShowGuideUIWidget(0, true));
            AddChild(new NOD_HideGuideUIInfo());
			//通知战场结束统计界面移除引导控件
            //WILL-TODO: 这里是不是有点问题？
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData2));
            AddChild(new NOD_SetInputManagerEnable(true));
            AddChild(new NOD_DelayFinish());
        }
    }


    ////////////////////////////////引导进入新手场景//////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 主城主界面开始第一次引导
    /// </summary>
    class GSD_Static_BeginGuide : GuideNodeDataSequence
    {
        protected int m_nEffectID;
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2001;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int nGuideWidgetStepID = 0;//= guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_WarHallButton;
            string strText = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtnGuide");

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int m_nSoundID_0_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _oUserData: strText, _bForceGuide: true);


            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_ANTIADDICT, WndMsgID.WND_MSG_ANTIADDICT_GUIDE_ACTIONBEGIN));
            AddChild(new NOD_SetInputManagerEnable(false));
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0));
            AddChild(new NOD_PlaySound(m_nSoundID_0));
            //游戏模式入口按钮引导 修改了触发引导ID的话记得修改 UPlayerButtonFrameView 里面按钮事件订阅的引导ID
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData));
            AddChild(new NOD_ShowGuideUIWidget(nGuideWidgetStepID));
            AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1)));
            AddChild(new NOD_HideGuideUIInfo().SetPrecondition(new CON_IsGuideUIWidgetClicked(0)));
            AddChild(new NOD_AlwaysExecting());

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            GuideWorkingData gData = wData as GuideWorkingData;

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            SoundManager.DeleteSound(m_nSoundID_0);
            SoundManager.DeleteSound(m_nSoundID_1);


            USpeedUI.UGuideMsgRemoveData msg = new USpeedUI.UGuideMsgRemoveData();
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, msg);

        }
    }
    /// <summary>
    /// 新手奖励
    /// </summary>
    class GSD_Static_HeroReward : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2007;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //对话内容1
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            KeyCode[] keys = { KeyCode.Mouse0, KeyCode.Space };

            this.SetPrecondition(new CON_IsNodeFinished(GuideNodeID()));
            AddChild(new NOD_SetInputManagerEnable(false));

            //显示第一句对话内容
            AddChild(new NOD_ShowGuideRewardInfoParam(m_strDes_0));

            //两秒后才能继续操作,鼠标左键或空格点击后隐藏对话内容
            AddChild(new TBTActionTimer().SetElapseTime(1.5f)
                     .AddChild(new NOD_HideGuideRewardInfo()
                     .SetPrecondition(new CON_IsKeyDown(_bInputMgrEnableAffected: false, _eCheckKeyCode: keys)))
                    );

            //还原输入功能
            AddChild(new NOD_SetInputManagerEnable(true));
            AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_WAITFORFREEMATCH));
            AddChild(new NOD_ReportGuideStep((int)EMMatchGuideStep.GuideStep_HeroReward));
            AddChild(new NOD_DelayFinish(true));
        }
    }
    /// <summary>
    /// 等待自由对战
    /// </summary>
    class GSD_Static_FreeMatch : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2008;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int EffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;

            int nMatchLimit = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nGameMode = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];


            SetPrecondition(new CON_IsNodeFinished(GuideNodeID()));
            AddChild(new NOD_WaitForFreeMatchFinish(EffectID, nGameMode, nMatchLimit));
            //进入临时步骤，有实际步骤时再修改
            AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_LIFEHEROENTRY));
        }
    }
    /// <summary>
    /// 本命英雄第一界面引导
    /// </summary>
    class GSD_Static_LifeHeroEntrance : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2002;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //对话内容1
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            ////对话内容2
            //string m_strDes_1 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            ////对话内容3
            //string m_strDes_2 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_2];

            int guideWidgetEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            KeyCode[] keys = { KeyCode.Mouse0, KeyCode.Space };
            UGuideMsgData.CustomUserData userData = new UGuideMsgData.CustomUserData(-600, null, 1);



            this.SetPrecondition(new CON_IsNodeFinished(GuideNodeID()));
            AddChild(new NOD_SetInputManagerEnable(false));

            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_ENTRANCE, WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 0, guideWidgetEffectID, _bForceGuide: true)));

            //显示第一句对话内容
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0, _bModal: true, _userData: userData));
            AddChild(new NOD_PlaySound(nSoundID_0));

            ////两秒后才能继续操作,鼠标左键或空格点击后显示第二句对话内容
            //AddChild(new TBTActionTimer().SetElapseTime(1.5f)
            //         .AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_1, _bModal: true, _userData: userData)
            //         .SetPrecondition(new CON_IsKeyDown(_bInputMgrEnableAffected: false, _eCheckKeyCode: keys))));

            ////两秒后才能继续操作,鼠标左键或空格点击后显示第三句对话内容
            //AddChild(new TBTActionTimer().SetElapseTime(1.5f)
            //         .AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_2, _bModal: true, _userData: userData)
            //         .SetPrecondition(new CON_IsKeyDown(_bInputMgrEnableAffected: false, _eCheckKeyCode: keys)))
            //        );

            //两秒后才能继续操作,鼠标左键或空格点击后隐藏对话内容
            AddChild(new TBTActionTimer().SetElapseTime(1.5f)
                     .AddChild(new NOD_HideGuideUIInfo()
                     .SetPrecondition(new CON_IsKeyDown(_bInputMgrEnableAffected: false, _eCheckKeyCode: keys)))
                    );

            //显示UI控件
            AddChild(new NOD_ShowGuideUIWidget(0, true));
            //点击UI控件时关闭UI控件
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_ENTRANCE, WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONEND, new UGuideWidgetMsgData((int)GuideNodeID(), 0, guideWidgetEffectID)).SetPrecondition(new CON_IsGuideUIWidgetClicked(0)));
            //还原输入功能
            AddChild(new NOD_SetInputManagerEnable(true));
            //测试用的
            AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_LIFEHEROCULTURE));
            AddChild(new NOD_ReportGuideStep((int)EMMatchGuideStep.GuideStep_LifeHero_Start));
            AddChild(new NOD_DelayFinish(true));
        }
    }
    /// <summary>
    /// 本命英雄仓引导
    /// </summary>
    class GSD_Static_LifeHeroCulture : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2003;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_Button;
            int m_nHideDelayTime = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            //int m_nEffectID2 = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;
            //int m_nEffectID3 = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_ExitButton_huicheng;
            //对话内容1
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            //string m_strDes_1 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            //string m_strDes_2 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_2];


            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsWndViewVisible(true, WndID.WND_ID_LIFE_HERO_CULTURE)));
            AddChild(new NOD_SetInputManagerEnable(false));
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_CULTURE, WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _bForceGuide: false)));

            //引导提示
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0));
            //引导点击第一个培养器
            AddChild(new NOD_ShowGuideUIWidget(0, false));
            AddChild(new NOD_PlaySound(nSoundID_0));

            ////初始化引导点第一个本命英雄相关数据
            //AddChild(new NOD_CallGuideActionBegin(WndID.WND_ID_LIFE_HERO_SELECT, WndMsgID.WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 1, m_nEffectID2, _bForceGuide: true)));
            ////引导点第一个本命英雄
            //AddChild(new TBTActionTimer().SetElapseTime(0.5f).AddChild(new NOD_ShowGuideUIWidget(1, true)));
            ////初始化引导点注入数据
            //AddChild(new NOD_CallGuideActionBegin(WndID.WND_ID_LIFE_HERO_SELECT, WndMsgID.WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 2, m_nEffectID3, _bForceGuide: true)));
            ////引导点注入
            //AddChild(new NOD_ShowGuideUIWidget(2, true));

            ////初始化引导进入本命英雄晋升界面
            //AddChild(new NOD_CallGuideActionBegin(WndID.WND_ID_LIFE_HERO_CULTURE, WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 3, m_nEffectID3, _bForceGuide: true)));

            ////显示引导提示
            //AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_1));
            ////引导点进入晋升界面
            //AddChild(new NOD_ShowGuideUIWidget(3, true));


            ////初始化晋升界面提示引导
            //AddChild(new NOD_CallGuideActionBegin(WndID.WND_ID_HERODETAIL, WndMsgID.WND_MSG_HERODETAIL_GUIDE_ACTIONBEGIN, new UGuideWidgetMsgData((int)GuideNodeID(), 4, m_nEffectID3, _bForceGuide: true)).SetPrecondition(new CON_IsWndViewVisible(true,WndID.WND_ID_HERODETAIL)));

            ////显示引导提示
            //AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_2));
            ////显示引导光效
            //AddChild(new NOD_ShowGuideUIWidget(4));


            //延迟后进入下一步引导
            //AddChild(new NOD_DelayFinish(true));
            AddChild(new TBTActionTimer().SetElapseTime(m_nHideDelayTime).AddChild(new NOD_HideGuideUIInfo()));
            AddChild(new NOD_HideGuideUIWidget(0));
            AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_LISFHEROALLFIND));
            AddChild(new NOD_SetInputManagerEnable(true));
            AddChild(new NOD_DelayFinish(true));
        }
    }
    /// <summary>
    /// 这里会用到引导ID做判断，更改引导ID时注意下。
    /// </summary>
    class GSD_Static_LisfHeroAllFind : GuideNodeDataSequence
    {
        int m_nEffectID3 = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_ExitButton_huicheng;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2004;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //玩家实体创建且显示着玩家功能按钮界面时执行
            //SetPrecondition(new TBTPreconditionAND(new CON_IsWndViewVisible(true,WndID.WND_ID_PLAYERBUTTONFRAME), new CON_IsMainPlayerCreated()));
            SetPrecondition(new CON_IsMainPlayerCreated());
            AddChild
                (
                    new TBTActionParallel().SetEvaluationRelationship(TBTActionParallel.ECHILDREN_RELATIONSHIP.OR)
                    //如果所有本命英雄找完，则进入下一引导
                    .AddChild
                    (
                        new TBTActionSequence().SetPrecondition(new CON_IsLisfHeroFullfill())
                            //这里会用到引导ID做判断，更改引导ID时注意下。
                            .AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_CULTURE, WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONBEGIN,
                                      new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID3)))
                            .AddChild(new TBTActionTimer().SetElapseTime(6f).AddChild(new NOD_ShowGuideUIWidget(0)))
                            .AddChild(new TBTActionTimer().SetElapseTime(2f).AddChild(
                                                     new NOD_SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_CULTURE, WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONEND,
                                                                                   new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID3))))
                            .AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_RANKENTRANCE))
                    )
                    //如果本命英雄未找满，则引导比赛继续等待找满
                    .AddChild(new NOD_NotFullLifeHero())
                );

            //并行处理本命英雄相关。


            //测试用的
            //AddChild(new NOD_TmpNextStep(5));
            // AddChild(new TBTActionLeaf().SetPrecondition(new CON_IsGuideUIWidgetClicked(0, GuideNodeID())));
        }
    }
    /// <summary>
    /// 排位入口引导
    /// </summary>
    class GSD_Static_RankEntrance : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2005;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            //对话内容1
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            SetPrecondition(new TBTPreconditionAND(new CON_IsNodeFinished(GuideNodeID()), new CON_IsWndViewVisible(true, WndID.WND_ID_WAR_MAIN_FRAME)));

            AddChild(new NOD_ReportGuideStep((int)EMMatchGuideStep.GuideStep_LifeHero_Finish));
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0, _bModal: true));
            AddChild(new NOD_PlaySound(nSoundID_0));
            AddChild(new TBTActionTimer().SetElapseTime(4)
                     .AddChild(new TBTActionParallel()
                               .AddChild(new NOD_HideGuideUIInfo())
                               .AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_WAITFORRANK))));
            AddChild(new NOD_DelayFinish(true));
        }
    }
    /// <summary>
    /// 等待完成排位比赛引导
    /// </summary>
    class GSD_Static_WaitForRank : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_2006;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int EffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_LifeHero_jiantou01;
            int nGameMode = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
                            

            AddChild(new NOD_WaitForGameModeClickBegin(nGameMode, EffectID));
            AddChild(new NOD_WaitForGameModeClickEnd(nGameMode, EffectID));
            //进入临时步骤，有实际步骤时再修改
            AddChild(new NOD_SetLogicNextStep((int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TEMPSTEP));
        }

    }
    
    ////////////////////////////////引导进入人机战场////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 主城主界面语音啰嗦引导
    /// </summary>
    class GSD_Static_SecondGuide_1 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3001;
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//312
            SoundManager.DeleteSound(m_nSoundID_0);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_ANTIADDICT, WndMsgID.WND_MSG_ANTIADDICT_GUIDE_ACTIONBEGIN,null);

            //隐藏大厅引导控件相关逻辑
            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, 0, _bForceGuide: true);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData);
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//312
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            string strText = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtn");
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_WarHallButton;

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _oUserData: strText, _bForceGuide: true);



            SetPrecondition(new CON_IsNodeFinished(GuideNodeID()));

            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_ANTIADDICT, WndMsgID.WND_MSG_ANTIADDICT_GUIDE_ACTIONBEGIN));
            AddChild(new NOD_SetInputManagerEnable(false));
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0, _bModal: false));

            //游戏模式入口按钮引导 修改了触发引导ID的话记得修改 UPlayerButtonFrameView 里面按钮事件订阅的引导ID
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData));

            AddChild(new NOD_PlaySound(m_nSoundID_0));
            ////////////////////////////////////////////////////////////
            AddChild(new TBTActionTimer().SetElapseTime(0.5f).AddChild(new TBTActionSequence().AddChild(new NOD_ShowGuideUIWidget(0, true))
                .AddChild(new TBTActionParallel().AddChild(new NOD_DelayFinish()).AddChild(new NOD_SetCurrentGuideNode(EGuideNodeID.GuideNodeID_3003)))
                ));
            //AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1)));


            //AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1)));
            ////////////////////////////////////////////////////////////
            /*
            #region old
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int nGuideWidgetStepID = 0;
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_WarHallButton;
            string strText = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtn");

            int m_nSoundID_0 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//312
            int m_nSoundID_0_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];
            int m_nSoundID_1 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_2];//318
            int m_nSoundID_1_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_3];
            int m_nSoundID_2 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_4];//309


            AddChild(new NOD_PlaySound(m_nSoundID_0));
            AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_0_delay).AddChild(new NOD_PlaySound(m_nSoundID_1)));
            AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_1_delay).AddChild(new NOD_PlaySound(m_nSoundID_2)));


            //初始化开战指引
            AddChild(new TBTActionTimer().SetElapseTime(2).AddChild(new NOD_ShowHallWidget(strText, m_nEffectID,0)));
            //显示开战指引
            AddChild(new NOD_ShowGuideUIWidget(nGuideWidgetStepID));


            int m_nSound_4 = guideNodeData.GuideIntParams[5];//317
            //初始化游戏模式指引
            AddChild(new NOD_ShowHallGameModeWidget(2, m_nEffectID,1));
            //确定点击了开战指引和已经显示游戏模式界面后，显示游戏模式指引
            AddChild((new NOD_ShowGuideUIWidget(1)).SetPrecondition(new TBTPreconditionAND(new CON_IsGuideUIWidgetClicked(0), new CON_IsWndViewVisible(true, WndID.WND_ID_WAR_MAIN_FRAME))));
            AddChild(new NOD_PlaySound(m_nSound_4));


            int m_nSound_5 = guideNodeData.GuideIntParams[6];//325
            //匹配时间界面显示后，匹配时间界面初始化引导相关属性
            AddChild((new NOD_ShowGuideActionBegin(WndID.WND_ID_MATCHTIMEINFO, WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONBEGIN))
                    .SetPrecondition(new CON_IsWndViewVisible(true, WndID.WND_ID_MATCHTIMEINFO)));
            AddChild(new NOD_PlaySound(m_nSound_5));

            int m_nSound_6 = guideNodeData.GuideIntParams[7];//322

            //确定点击游戏模式指引后，且房间界面已显示
            //房间界面初始化引导相关属性，初始化英雄选择引导和显示英雄选择引导
            AddChild(new TBTActionTimer().SetElapseTime(2).SetPrecondition(new TBTPreconditionAND(new CON_IsGuideUIWidgetClicked(1), new CON_IsWndViewVisible(true, WndID.WND_ID_GAMEWAITINGROOMVIEW)))
                .AddChild(new TBTActionSequence().AddChild(new NOD_ShowGuideActionBegin(WndID.WND_ID_GAMEWAITINGROOMVIEW,WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONBEGIN))
                                                 .AddChild(new NOD_PlaySound(m_nSound_6))
                                                 .AddChild(new NOD_ShowWarWaitingRoom_SelectHero_Widget(2, m_nEffectID))
                                                 .AddChild(new NOD_ShowGuideUIWidget(2)))
                                                 );

            int m_nSound_7 = guideNodeData.GuideIntParams[8];//316
            //初始化开始按钮引导
            AddChild(new NOD_ShowWarWaitingRoom_StartBtn_Widget(3, m_nEffectID));
            //显示开始按钮引导
            AddChild(new NOD_ShowGuideUIWidget(3).SetPrecondition(new CON_IsGuideUIWidgetClicked(2)));
            AddChild(new NOD_PlaySound(m_nSound_7));

            //进入游戏前播放音效
            int m_nSound_8 = guideNodeData.GuideIntParams[9];//326
            AddChild(new TBTActionTimer().SetElapseTime(2).AddChild(new NOD_PlaySound(m_nSound_8)));
            #endregion
            */
        }
    }
    /// <summary>
    /// 引导打开游戏模式界面
    /// </summary>
    class GSD_Static_SecondGuide_2 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3002;
        }

        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            string strText = ULocalizationService.Instance.Get("UIView", "PLayerButtonFrameView", "HallBtn");
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_WarHallButton;

            int m_nSoundID_1_delay = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];
            int m_nSoundID_2 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];//309

            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _oUserData: strText, _bForceGuide: true);




            SetPrecondition(new CON_IsCurrentGuideNode(GuideNodeID()));

            //游戏模式入口按钮引导  修改了触发引导ID的话记得修改 UPlayerButtonFrameView 里面按钮事件订阅的引导ID
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData));


            AddChild(new TBTActionTimer().SetElapseTime(m_nSoundID_1_delay).AddChild(new NOD_PlaySound(m_nSoundID_2)));
            //初始化开战指引
            //AddChild(new TBTActionTimer().SetElapseTime(1).AddChild(new NOD_ShowHallWidget(strText, m_nEffectID, 0)));
            //显示开战指引
            AddChild(new NOD_ShowGuideUIWidget(0));
            AddChild(new TBTActionTimer().SetElapseTime(0.5f).AddChild(new TBTActionSequence().AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0))
                .AddChild(new NOD_SetCurrentGuideNode(EGuideNodeID.GuideNodeID_3003))
                ));


        }

        protected override void onTransition(TBTWorkingData wData)
        {
            //隐藏大厅引导控件相关逻辑
            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, 0, _bForceGuide: true);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PLAYERBUTTONFRAME, WndMsgID.WND_MSG_PLAYERBUTTONFRAME_WARHALLBUTTON_ADDGUIDEWIDGET, msgData);
        }
    }
    /// <summary>
    /// 引导选取游戏模式
    /// </summary>
    class GSD_Static_SecondGuide_3 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3003;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_GameModeButton_Small;

            int m_nSound_4 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//317
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _oUserData: (int)War.EWarModeDef.MODE_MachineVS, _bForceGuide: true);

            //初始化游戏模式指引
            SetPrecondition(new CON_IsCurrentGuideNode(GuideNodeID()));

            //通知大厅游戏模式添加引导控件
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET, msgData));
            //通知大厅游戏模式处理引导相关操作
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONBEGIN));
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0).SetPrecondition(new CON_IsWndViewVisible(true, WndID.WND_ID_WAR_MAIN_FRAME)));
            //确定点击了开战指引和已经显示游戏模式界面后，显示游戏模式指引
            AddChild((new NOD_ShowGuideUIWidget(0)).SetPrecondition(new TBTPreconditionAND(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3001), new CON_IsWndViewVisible(true, WndID.WND_ID_WAR_MAIN_FRAME))));
            AddChild(new NOD_PlaySound(m_nSound_4));
            AddChild(new NOD_SetCurrentGuideNode(EGuideNodeID.GuideNodeID_3004).SetPrecondition(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3003)));
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSound_4 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//317

            SoundManager.DeleteSound(m_nSound_4);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_MAIN_FRAME, WndMsgID.WND_MSG_WAR_MAIN_GUIDE_ACTIONEND, null);

        }
    }
    /// <summary>
    /// 引导邀请界面点击开始游戏
    /// </summary>
    class GSD_Static_SecondGuide_4 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3004;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_ExitButton_huicheng;

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID, _bForceGuide: true);

            SetPrecondition(new CON_IsCurrentGuideNode(GuideNodeID()));
            AddChild(new NOD_HideGuideUIInfo());
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_RANK_INVITE, WndMsgID.WND_MSG_WAR_MAIN_GAMEMODEBTN_ADDGUIDEWIDGET, msgData));
            AddChild((new NOD_ShowGuideUIWidget(0)).SetPrecondition(new TBTPreconditionAND(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3003), new CON_IsWndViewVisible(true, WndID.WND_ID_RANK_INVITE))));
            AddChild(new NOD_SetCurrentGuideNode(EGuideNodeID.GuideNodeID_3005).SetPrecondition(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3004)));
        }
    }
    /// <summary>
    /// 等待匹配时间时控制引导相关部分逻辑与界面显示
    /// </summary>
    class GSD_Static_SecondGuide_5 : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3005;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            int m_nSound_5 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//325

            SetPrecondition(new CON_IsCurrentGuideNode(GuideNodeID()));
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_RANK_INVITE, WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONBEGIN));
            //匹配时间界面显示后，匹配时间界面初始化引导相关属性
            AddChild((new NOD_SendTargetWndMessage(WndID.WND_ID_MATCHTIMEINFO, WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONBEGIN))
                    .SetPrecondition(new CON_IsWndViewVisible(true, WndID.WND_ID_MATCHTIMEINFO)));
            AddChild(new NOD_PlaySound(m_nSound_5));
            AddChild(new NOD_AlwaysExecting());
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_MATCHTIMEINFO, WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONEND, null);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_RANK_INVITE, WndMsgID.WND_MSG_MSTCHIIMEINFO_GUIDE_ACTIONEND, null);

            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSound_5 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//325
            SoundManager.DeleteSound(m_nSound_5);
        }
    }
    /// <summary>
    /// 英雄选取引导
    /// </summary>
    class GSD_Static_SecondGuide_6 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3006;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_SelectHero;

            int m_nSound_6 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//322
            int m_nInfoYPos = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_1];

            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID);

            //确定点击游戏模式指引后，且房间界面已显示
            //房间界面初始化引导相关属性，初始化英雄选择引导和显示英雄选择引导
            AddChild(new NOD_HideGuideUIInfo());

            TBTActionTimer timerAction = new TBTActionTimer();
            timerAction.SetElapseTime(0.3f).SetPrecondition(new TBTPreconditionAND(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3003),
                                                              new CON_IsWndViewVisible(true, WndID.WND_ID_GAMEWAITINGROOMVIEW)));
            timerAction.AddChild(new TBTActionSequence()
                                //通知房间界面处理引导相关操作
                                .AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_GAMEWAITINGROOMVIEW, WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONBEGIN))
                                //通知房间界面添加选英雄引导控件
                                .AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_GAMEWAITINGROOMVIEW, WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SELECTHERO_ADDGUIDEWIDGET,msgData))
                                .AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0, _userData: new UGuideMsgData.CustomUserData(_fCustomYPos: m_nInfoYPos)))
                                .AddChild(new NOD_PlaySound(m_nSound_6))
                                .AddChild(new NOD_ShowGuideUIWidget(0)));

            AddChild(timerAction);

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GAMEWAITINGROOMVIEW, WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONEND, null);


            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSound_6 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//322
            SoundManager.DeleteSound(m_nSound_6);
        }
    }
    /// <summary>
    /// 战场开始按钮点击引导
    /// </summary>
    class GSD_Static_SecondGuide_7 : GuideNodeDataSequence
    {
        protected int m_nEffectID;

        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3007;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            m_nEffectID = (int)USpeedUI.UEffect.UEffectPrefabType.UEPT_GuideWidget_GameStart;

            int m_nSound_7 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//316
            string m_strDes_0 = guideNodeData.GuideStringParams[(int)EGuideNodeConfigParamID.GuideParamID_0];

            UGuideWidgetMsgData msgData = new UGuideWidgetMsgData((int)GuideNodeID(), 0, m_nEffectID);

            //通知房间界面 添加开始按钮引导控件
            AddChild(new NOD_SendTargetWndMessage(WndID.WND_ID_GAMEWAITINGROOMVIEW, WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_STARTBTN_ADDGUIDEWIDGET, msgData));
            //显示开始按钮引导
            AddChild(new NOD_ShowGuideUIWidget(0).SetPrecondition(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3005)));
            AddChild(new NOD_PlaySound(m_nSound_7));
            AddChild(new NOD_ShowGuideUIInfoParam(m_strDes_0));

        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSound_7 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//316
            SoundManager.DeleteSound(m_nSound_7);
        }
    }
    /// <summary>
    /// 开战前啰嗦的语音提示
    /// </summary>
    class GSD_Static_SecondGuide_8 : GuideNodeDataSequence
    {
        public override EGuideNodeID GuideNodeID()
        {
            return EGuideNodeID.GuideNodeID_3008;
        }
        protected override void Init()
        {
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());

            //进入游戏前播放音效
            int m_nSound_8 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//326
            AddChild(new NOD_PlaySound(m_nSound_8).SetPrecondition(new CON_IsGuideUIWidgetClicked(0, EGuideNodeID.GuideNodeID_3006)));
            AddChild(new NOD_HideGuideUIInfo());
            AddChild(new NOD_AlwaysExecting());
        }

        protected override void onTransition(TBTWorkingData wData)
        {
            base.onTransition(wData);
            SSchemeGuideNodeData guideNodeData = GuideManager.Instance.getNodeConfigData(GuideNodeID());
            int m_nSound_8 = guideNodeData.GuideIntParams[(int)EGuideNodeConfigParamID.GuideParamID_0];//326
            SoundManager.DeleteSound(m_nSound_8);
            USpeedUI.UGuideMsgRemoveData msg = new USpeedUI.UGuideMsgRemoveData();
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, msg);
            InputManager.Available = true;

        }
    }

}
