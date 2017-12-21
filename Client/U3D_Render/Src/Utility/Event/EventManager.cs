/*******************************************************************
** 文件名:	EventManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-5
** 版  本:	1.0
** 描  述:	事件实现

		
********************************************************************/
using System;
using System.Collections.Generic;


namespace U3D_Render.Utility
{
    internal struct SEventKey
    {
        private Int64 m_value;

        public SEventKey(EVENT eventID, int sourceID)
        {
            m_value = ((int)eventID) << 32 + sourceID;
        }

    }

    internal class EventCollect
    {
        IList<IEventHandler> m_collection = null;
        public EventCollect()
        {
            m_collection = new List<IEventHandler>();
        }

        public bool Add(IEventHandler handler)
        {
            if (!m_collection.Contains(handler))
                m_collection.Add(handler);
            return true;
        }

        public bool Remove(IEventHandler handler)
        {
            return m_collection.Remove(handler);
        }

        public void OnEvent(EVENT eventID, int sourceID, System.Object objEvent)
        {
            foreach (IEventHandler hander in m_collection) {
                if (hander != null)
                    hander.OnEvent(eventID, sourceID, objEvent);
            }
        }
    }

    public class EventManager
    {

        private static Dictionary<SEventKey, EventCollect> m_collections = null;

        static EventManager()
        {
            m_collections = new Dictionary<SEventKey, EventCollect>();
        }

        /// <summary>
        /// 订阅执行事件
        /// </summary>
        /// <param name="handler">事件回调对象句柄</param>
        /// <param name="eventID">事件ID</param>
        /// <param name="sourceID">发送源标识</param>
        /// <returns></returns>
        static public bool Subscibe(IEventHandler handler, U3D_Render.Utility.EVENT eventID, int sourceID)
        {

             SEventKey key = new SEventKey(eventID, sourceID);
             if (!m_collections.ContainsKey(key))
                 m_collections[key] = new EventCollect();

             return m_collections[key].Add(handler);
			 
        }

        /// <summary>
        /// 取消订阅执行事件
        /// </summary>
        /// <param name="handler">事件回调对象句柄</param>
        /// <param name="eventID">事件ID</param>
        /// <param name="sourceID">发送源标识</param>
        /// <returns></returns>

        static public bool UnSubscibe(IEventHandler handler, U3D_Render.Utility.EVENT eventID, int sourceID)
        {
            SEventKey key = new SEventKey(eventID, sourceID);
            if (!m_collections.ContainsKey(key))
                return true;
            
            return m_collections[key].Remove(handler);
        }

        /// <summary>
        /// 发送执行事件
        /// </summary>
        /// <param name="eventID">事件ID</param>
        /// <param name="sourceID">发送源标识</param>
        /// <param name="objEvent">事件上下文</param>
        static public void FireExecute(U3D_Render.Utility.EVENT eventID, int sourceID, System.Object objEvent)
        {

            SEventKey key1 = new SEventKey(eventID, sourceID);
            if (sourceID > 0 && m_collections.ContainsKey(key1))
            {
                m_collections[key1].OnEvent(eventID, sourceID, objEvent);
            }

            SEventKey key2 = new SEventKey(eventID, 0);
            if (m_collections.ContainsKey(key2))
            {
                m_collections[key2].OnEvent(eventID, sourceID, objEvent);
            }
        }    

    }
}
