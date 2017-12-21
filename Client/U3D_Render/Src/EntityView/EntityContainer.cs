/*******************************************************************
** 文件名:	EntityContainer.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-12-17
** 版  本:	1.0
** 描  述:	实体池

		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// 定义实体ID
using ENTITY_ID = System.Int32;

namespace U3D_Render
{
    /// <summary>
    /// 实体池
    /// </summary>
    public class EntityContainer 
    {
        /// <summary>
        /// 全部实体
        /// </summary>
        public ICollection<EntityView> Entities
        {
            get{ return m_entities.Values; }
        }
        private Dictionary<int, EntityView> m_entities = new Dictionary<int, EntityView>();

        /// <summary>
        /// 全部人物实体
        /// </summary>
        public ICollection<EntityView> Players
        {
            get { return m_playerEntities.Values; }
        }
        private Dictionary<int, EntityView> m_playerEntities = new Dictionary<int, EntityView>();

        /// <summary>
        /// 全部怪物实体
        /// </summary>
        public ICollection<EntityView> Monsters
        {
            get { return m_monsterEntities.Values; }
        }
        private Dictionary<int, EntityView> m_monsterEntities = new Dictionary<int, EntityView>();


        /// <summary>
        /// 清空所有记录
        /// </summary>
        public void Clear()
        {
            m_entities.Clear();
            m_playerEntities.Clear();
            m_monsterEntities.Clear();
        }


        /// <summary>
        /// 增加实体
        /// </summary>
        /// <param name="evObj">要增加的目标实体</param>
        /// <returns></returns>
        public bool Add(EntityView evObj)
        {
            if (evObj == null)
                return false;

            m_entities[evObj.ID] = evObj;

            switch(evObj.Type)
            {
                case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                    {
                        m_playerEntities[evObj.ID] = evObj;
                    }
                    break;

                case ENTITY_TYPE.TYPE_MONSTER:
                    {
                        m_monsterEntities[evObj.ID] = evObj;
                    }
                    break;

                case ENTITY_TYPE.TYPE_TANK:
                    {

                    }
                    break;
                default:
                    break;
            }

            return true;
        }

        /// <summary>
        /// 移除实体
        /// </summary>
        /// <param name="evObj">要移除的目标实体</param>
        /// <returns></returns>
        public bool Remove(ref EntityView evObj)
        {
            if (evObj == null)
                return false;

            m_entities.Remove(evObj.ID);

            switch (evObj.Type)
            {
                case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                    {
                        m_playerEntities.Remove(evObj.ID);
                    }
                    break;

                case ENTITY_TYPE.TYPE_MONSTER:
                    {
                        m_monsterEntities.Remove(evObj.ID);
                    }
                    break;

                case ENTITY_TYPE.TYPE_TANK:
                    {

                    }
                    break;
                default:
                    break;
            }

            return true;
        }

        /// <summary>
        /// 获取指定的实体
        /// </summary>
        /// <param name="entity_id">实体ID</param>
        /// <returns></returns>
        public EntityView Get(ENTITY_ID entity_id)
        {
            return Get(entity_id, ENTITY_TYPE.TYPE_MAX);
        }

        /// <summary>
        /// 获取指定的实体
        /// </summary>
        /// <param name="entity_id">实体ID</param>
        /// <param name="entity_type">实体类型ENTITY_TYPE</param>
        /// <returns></returns>
        /// <remarks>指定类型时搜索速度会更快</remarks>
        public EntityView Get(ENTITY_ID entity_id, ENTITY_TYPE entity_type)
        {
            IDictionary<int, EntityView> collection = null;

            if (entity_type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                collection = m_playerEntities;
            }
            else if (entity_type == ENTITY_TYPE.TYPE_MONSTER)
            {
                collection = m_monsterEntities;
            }
            else
            {
                collection = m_entities;
            }

            if (collection.Count == 0)
            {
                return null;
            }

            if (!collection.ContainsKey(entity_id))
            {
                return null;
            }

            return collection[entity_id];
        }

        /// <summary>
        /// 是否存在此EntityView对象
        /// </summary>
        /// <returns></returns>
        public bool IsExists(EntityView ev)
        {
            if (ev == null) return false;

            return m_entities.ContainsKey(ev.ID);
        }

    }   // end EntityContainer

}
