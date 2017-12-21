/*******************************************************************
** 文件名:	GuideDefine.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-12-12
** 版  本:	1.0
** 描  述:	引导所有枚举类型文件
		
********************************************************************/
namespace GUIDE
{
    /// <summary>
    /// 打印信息类型
    /// </summary>
    public enum EGuideDebugType : int
    {
        None,
        Debug,
        Profile,
    }

    /// <summary>
    /// 配置表数据
    /// </summary>
    public enum EGuideNodeConfig : int
    {
        GuideID,
        GuideActionStringParams,
        GuideActionIntParams,
        //GuidePreconditionIntParams,
        GuideActionLayerType,
    }
    
    /// <summary>
    /// 配置表字符串类型参数顺序
    /// </summary>
    public enum EGuideNodeConfigParamID : int
    {
        GuideParamID_0,
        GuideParamID_1,
        GuideParamID_2,
        GuideParamID_3,
        GuideParamID_4,
        GuideParamID_5,
    }

    public enum EGuideActionLayerType : int
    {
        None = -1,
        Sequence,
        Parallel,
        Selector,

        /// <summary>
        /// 自己扩展的顺序执行类型，主要根据逻辑层下发的序号执行下一步，否则在当前等待.
        /// </summary>
        LogicSequence,
    }
    /// <summary>
    /// 可用于外部控制的节点条件，目前还没用到，所以不完善
    /// </summary>
    public enum EGuideTriggerID : int
    {
        A,
        B,
        C,
        D,
    }

    /// <summary>
    /// 引导节点对应的ID
    /// </summary>
    public enum EGuideNodeID : int
    {
        GuideNodeID_None = -1,
        ///新手引导场景用序号ID
        GuideNodeID_0,        
        GuideNodeID_1,
        GuideNodeID_2,
        GuideNodeID_3,
        GuideNodeID_4,
        GuideNodeID_5,
        GuideNodeID_6,
        GuideNodeID_7,
        GuideNodeID_8,
        GuideNodeID_9,
        GuideNodeID_10,
        ///正式场景用的序号ID
        GuideNodeID_1001 = 1001,
        GuideNodeID_1002,
        GuideNodeID_1003,
        GuideNodeID_1004,
        GuideNodeID_1005,
        GuideNodeID_1006,
        GuideNodeID_1007,
        GuideNodeID_1008,
        GuideNodeID_1009,
        ///静态场景用的序号ID
        GuideNodeID_2001 = 2001,
        GuideNodeID_2002,
        GuideNodeID_2003,
        GuideNodeID_2004,
        GuideNodeID_2005,
        GuideNodeID_2006,
        GuideNodeID_2007,
        GuideNodeID_2008,

        //静态场景新手第二步序号ID
        GuideNodeID_3001 = 3001,
        GuideNodeID_3002,
        GuideNodeID_3003,
        GuideNodeID_3004,
        GuideNodeID_3005,
        GuideNodeID_3006,
        GuideNodeID_3007,

        //非主逻辑提示引导
        GuideNodeID_9001 = 9001,//血量低回城提示
        GuideNodeID_9002,//魔法值低回城提示
        //用于播放逻辑层下发的自动引导信息
        GuideNodeID_AutoMessageFromLogic = 10000,
        GuideNodeID_AutoMessageBegin = 10001,
        GuideNodeID_AutoMessageEnd   = 20000,
    }

    /// <summary>
    /// 引导节点用于判断当前场景状态
    /// </summary>
    public enum EGuideMatchTypeState : int
    {
        None,
        Enter,
        Leave,
    }

    /// <summary>
    /// 引导状态-黑板数据标示
    /// </summary>
    public enum EGuideStateBBKey : int
    {
        GameStageState,
        GuideMatchTypeID,
        PlayerNoviceGuideType,
        GuideStateID,
    }

    /// <summary>
    /// 动态引导-黑板数据标示
    /// </summary>
    public enum EGuideBBKey : int
    {
      
        DeadEntityID,
        LogicStepID,
        IsAutoGuideMessagePlaying,
        AutoGuideMessageID,
        CurrentGuideNode,
    }

    /// <summary>
    /// 引导状态中用户ID类型
    /// </summary>
    public enum EGuideStateUserIDType : int
    {
        EST_None = -1,
        /// <summary>
        /// 当前新手引导ID
        /// </summary>
        EST_NoviceID,
        /// <summary>
        /// 当前场景ID
        /// </summary>
        EST_SceneID,
    }
    /// <summary>
    /// 引导状态
    /// </summary>
    public enum EGuideStateData : int
    {
        EGSD_GuideStateID,
        EGSD_GameState,
        EGSD_UserIDType,
        EGSD_UserID,
        EGSD_GuideRootType,
        EGSD_GuideNodeIDList,
        EGSD_PlayerLevelLimit,
    }

    /// <summary>
    /// 引导步骤更改事件参数
    /// </summary>
    public class UpdateNoviceGuideTypeEventArgs : System.EventArgs
    {
        /// <summary>
        /// 引导ID
        /// </summary>
        public int CurrentNoviceGuideType;

        public UpdateNoviceGuideTypeEventArgs(int _nCurrentNoviceGuideType)
        {
            CurrentNoviceGuideType = _nCurrentNoviceGuideType;
        }
    }
}
