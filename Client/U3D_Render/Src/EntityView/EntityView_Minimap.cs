/*******************************************************************
** 文件名:	EntityView_Minimap.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-12-29
** 版  本:	1.0
** 描  述:	单个实体表现（和小地图有关逻辑）


********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using GameLogic;

namespace U3D_Render
{
    /// <summary>
    /// 单个实体表现（和小地图有关逻辑）
    /// </summary>
    public partial class EntityView 
    {
        // 在黑暗的角落(小地图上不显示)
        public bool m_bInDarkCorner = false;

        /// <summary>
        /// 小地图上更新时间
        /// </summary>
        private uint m_minimapUpdateTick = 0;

        private UnityEngine.Vector3 m_lastLoc = UnityEngine.Vector3.zero;
        /// <summary>
        /// 上次在Grid上的位置
        /// </summary>
        public UnityEngine.Vector3 LastGridLoc
        {
            get { return m_lastLoc; }
            set { m_lastLoc = value; }
        }

        /// <summary>
        /// 能更新到小地图
        /// </summary>
        /// <returns></returns>
        public bool CanUpdateMinimap()
        {
            // 怪物在小地图上更新频率降低些
            if (m_type == ENTITY_TYPE.TYPE_MONSTER)
            {
                uint currentTick = GameLogicAPI.getTickCount();
                // 加快点试试
                if (currentTick < m_minimapUpdateTick + 300)
                {
                    return false;
                }
            }
            return true;
        } 

        /// <summary>
        /// 小地图更新后处理
        /// </summary>
        /// <returns></returns>
        public bool OnUpdateMinimap()
        {
            m_minimapUpdateTick = GameLogicAPI.getTickCount();

            return true;
        }
    }
}
