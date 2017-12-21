/*******************************************************************
** 文件名:	EffectNode_CheckPerformace.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效节点
** 应  用:  	特效节点，性能检测相关

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ImageEffects;
namespace Effect
{
    /// <summary>
    /// 特效节点
    /// </summary>
    public partial class EffectNode : EffectNodeBase
    {
        /// <summary>
        /// XP特效
        /// </summary>
        public bool isXPEffect = false;

        //是否是展示特效
        public bool ShowingRoomEffect = false;

        /// <summary>
        /// 性能参考指标
        /// </summary>
        public float PerformaceUnit = 9999f;

        //性能是否ok
        public bool PerformaceOk = false;

        public static int ProjectorIngonreMask
        {
            get
            {
                int mask = 0;
                mask |= 1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerPlayerFriend);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerMonsterFriend);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerHero);
                mask |= 1 << LayerMask.NameToLayer(Config.LayerBuilding);
                return mask;
            }
        }
    }
}



