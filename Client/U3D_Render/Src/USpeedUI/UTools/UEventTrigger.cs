/*******************************************************************
** 文件名:	UEventTrigger.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-02-17
** 版  本:	1.0
** 描  述:	UI音效播放触发器，本脚本已集成基本UI事件响应（Click，Enter，Exit），其余UI事件需要时再添加
            UI音效使用的UI音效名字而不直接使用音效ID，具体UI音效名字与音效ID对应方法看UISoundConfig.csv
            一、编辑器使用方法：            
                1、给GameObject添加此脚本。
                2、选择UI响应事件类型
                3、右侧+按钮添加事件响应处理器类型
                4、选择当前已有音效名字
            二、脚本使用方法：
                1、给GameObject添加此脚本
                2、执行 UEventTrigger::CreateEventEntryHandler 事件处理器并给设置响应事件方法(AddEventCallBack/RemoveEventCallBack)或者设置音效属性SetSoundConfig等
                3、将创建的EventHandlerBase 对象通过 UEventTrigger::AddEventEntryHandler添加进去。
            三、扩展事件处理器：
                1、EEventHandlerType 添加处理器类型
                2、EventHandlerBase 添加处理器需要序列化的属性
                3、EventHandlerBase::HandlerExecute 添加新处理器事件触发时执行的方法
                4、新处理器的所有内容都用#region 处理器类型 #endregion 括起来
********************************************************************/
using Data.UISoundConfig;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace USpeedUI
{
    public static class UEventTriggerExtensions
    {
        public static byte[] ObjectToByteArray(this System.Object obj)
        {
            if (obj == null)
                return null;
            BinaryFormatter bf = new BinaryFormatter();
            MemoryStream ms = new MemoryStream();
            bf.Serialize(ms, obj);
            return ms.ToArray();
        }

        // Convert a byte array to an Object
        public static System.Object ByteArrayToObject(this byte[] arrBytes)
        {
            MemoryStream memStream = new MemoryStream();
            BinaryFormatter binForm = new BinaryFormatter();
            memStream.Write(arrBytes, 0, arrBytes.Length);
            memStream.Seek(0, SeekOrigin.Begin);
            System.Object obj = (System.Object)binForm.Deserialize(memStream);
            return obj;
        }
    }

    /// <summary>
    /// 事件响应处理器类型
    /// </summary>
    public enum EEventHandlerType
    {
        Sound,
        EventCallback,
    }  

    /// <summary>
    /// 事件执行时传的参数
    /// </summary>
    public struct STriggerExecuteParam
    {
        private BaseEventData m_BaseEventData;
        public BaseEventData EventData
        {
            get { return m_BaseEventData; }
            //set { m_BaseEventData = value; }
        }

        private EventTriggerType m_eEventTriggerType;
        public EventTriggerType TriggerType
        {
            get { return m_eEventTriggerType; }
            //set { m_eEventTriggerType = value; }
        }

        public STriggerExecuteParam(EventTriggerType _eEventTriggerType,BaseEventData _eventData)
        {
            m_BaseEventData = _eventData;
            m_eEventTriggerType = _eEventTriggerType;
        }
    }

    #region EventHandler
    [Serializable]
    //////////////////Unity 不支持类多态序列化
    public class EventHandlerBase
    {
        [SerializeField]
        protected EEventHandlerType m_handlerType;
        public EEventHandlerType HandlerType { get { return m_handlerType; } }

        /// <summary>
        /// 用 UEventTrigger的 CreateEventEntryHandler 方法实例化
        /// </summary>
        /// <param name="_eHandlerType"></param>
        public EventHandlerBase(EEventHandlerType _eHandlerType)
        {
            m_handlerType = _eHandlerType;
        }

        #region HandlerType == Sound
        [SerializeField]
        protected string m_strSoundName = string.Empty;

        public string SoundName
        {
            get { return m_strSoundName; }
            set { m_strSoundName = value; }
        }

        protected string m_strSoundFilter;
        public string SoundFilter { get { return m_strSoundFilter; } }

        protected string m_strSoundKey;
        public string SoundKey { get { return m_strSoundKey; } }

        public void SetSoundConfig(string _strFilter, string _strKey)
        {
            m_strSoundFilter = _strFilter;
            m_strSoundKey = _strKey;

            string tmpSoundName = string.Empty;
            UISoundConfig.Instance.GetSoundName(_strFilter, _strKey, ref tmpSoundName);
            m_strSoundName = tmpSoundName;
        }

        public int GetSoundID()
        {
            int resultID = -1;
            if (!string.IsNullOrEmpty(SoundName))
            {
                SSchemeUSoundData data = UISoundConfig.Instance.GetData(SoundName);
                if (data != null)
                {
                    resultID = data.USoundID;
                }
            }
            return resultID;
        }
        private void HandlerExecute_Sound(STriggerExecuteParam _executeParam)
        {
            SoundManager.CreateUISound(GetSoundID());
        }
        #endregion


        #region HanderType == EventCallback  把回调方法写这里为了统一且方便用户尽量少的关注其他类关系
        [Serializable]
        public class HanderEventCallback : UnityEvent<STriggerExecuteParam>
        { }
        /// <summary>
        /// 事件回调类型
        /// </summary>
        [SerializeField]
        private HanderEventCallback m_eventCallback = new HanderEventCallback();
        /// <summary>
        /// 订阅回调方法
        /// </summary>
        /// <param name="_callback"></param>
        public void AddEventCallBack(UnityAction<STriggerExecuteParam> _callback)
        {
            m_eventCallback.AddListener(_callback);
        }

        /// <summary>
        /// 移除订阅
        /// </summary>
        /// <param name="_callback"></param>
        public void RemoveEventCallBack(UnityAction<STriggerExecuteParam> _callback)
        {
            m_eventCallback.RemoveListener(_callback);
        }

        private void HandlerExecute_EventCallback(STriggerExecuteParam _executeParam)
        {
            m_eventCallback.Invoke(_executeParam);
        }
        #endregion

        public void HandlerExecute(STriggerExecuteParam _executeParam)
        {

            HandlerExecute_EventCallback(_executeParam);

            switch (HandlerType)
            {
                case EEventHandlerType.Sound:
                    {
                        HandlerExecute_Sound(_executeParam);
                    }                    
                    break;

                default:
                    break;
            }
        }
    }
    #endregion

    [Serializable]
    public class UEventTrigger : MonoBehaviour
        ,IPointerClickHandler
        ,IPointerEnterHandler
        ,IPointerExitHandler
        ,IInitializePotentialDragHandler
        ,IBeginDragHandler
        ,IDragHandler
        ,IEndDragHandler
        ,IDropHandler
        ,IScrollHandler
        ,IUpdateSelectedHandler
        ,ISelectHandler
        ,IDeselectHandler
        ,IMoveHandler
        ,ISubmitHandler
        ,ICancelHandler
    {
        #region 触发事件类型相关
        /// <summary>
        /// 事件触发类型对应的处理器列表
        /// </summary>
        [Serializable]
        public class EventEntry
        {
            public EventTriggerType eventID = EventTriggerType.PointerClick;

            [SerializeField]
            protected List<EventHandlerBase> m_EventHandlerList;

            public List<EventHandlerBase> EventHandlerList
            {
                get
                {
                    if (m_EventHandlerList == null)
                        m_EventHandlerList = new List<EventHandlerBase>();
                    return m_EventHandlerList;
                }
                set { m_EventHandlerList = value; }
            }

            public void AddEventHandler(EventHandlerBase _handler)
            {
                if(_handler != null && !ContainsHandler(_handler))
                {
                    EventHandlerList.Add(_handler);
                }                
            }

            public void RemoveEventHandler(EventHandlerBase _handler)
            {
                if(ContainsHandler(_handler))
                {
                    EventHandlerList.Remove(_handler);
                }
            }

            public bool ContainsHandler(EventHandlerBase _handler)
            {
                if (_handler == null || EventHandlerList.Count == 0)
                    return false;

                return EventHandlerList.Contains(_handler);
            }

            public void Execute(STriggerExecuteParam _executeParam)
            {
                if (m_EventHandlerList == null)
                    return;
                for (int i = 0; i < EventHandlerList.Count; i++) 
                {
                    EventHandlerList[i].HandlerExecute(_executeParam);
                }
            }
        }

        [SerializeField]
        protected List<EventEntry> m_EventTriggerList;
        /// <summary>
        /// 触发事件类型列表
        /// </summary>
        public List<EventEntry> Triggers
        {
            get
            {
                if (m_EventTriggerList == null)
                    m_EventTriggerList = new List<EventEntry>();
                return m_EventTriggerList;
            }
            set { m_EventTriggerList = value; }
        }

        /// <summary>
        /// 添加触发事件类型
        /// <para>外部调用记得先用 ContainsEventTriggerType 检查是否已存在</para>
        /// </summary>
        /// <param name="_eEventID">事件ID</param>
        public void AddEventEntry(EventTriggerType _eEventID)
        {
            EventEntry entry = new EventEntry();
            entry.eventID = _eEventID;
            Triggers.Add(entry);
        }

        /// <summary>
        /// 移除触发事件类型
        /// </summary>
        /// <param name="_eEventID">事件ID</param>
        public void RemoveEventEntry(EventTriggerType _eEventID)
        {
            int index = -1;
            for(int i=0;i<Triggers.Count;++i)
            {
                if(Triggers[i].eventID == _eEventID)
                {
                    index = i;
                    break;
                }
            }
            
            if(index != -1)
            {
                Triggers.RemoveAt(index);
            }
        }

        /// <summary>
        /// 是否已有触发事件
        /// </summary>
        /// <param name="_eEventID">事件ID</param>
        /// <param name="_nEventTypeIndex">事件序号</param>
        /// <returns></returns>
        private bool ContainsEventTriggerType(EventTriggerType _eEventID, out int _nEventTypeIndex)
        {
            for (int p = 0; p < Triggers.Count; ++p)
            {
                EventEntry delegateEntry = Triggers[p];

                if (delegateEntry.eventID == _eEventID)
                {
                    _nEventTypeIndex = p;
                    return true;
                }
            }
            _nEventTypeIndex = -1;
            return false;
        }
        #endregion

        #region 事件处理器

        /// <summary>
        /// 绑定事件类型和处理器
        /// </summary>
        /// <param name="_eEventID"></param>
        /// <param name="_handler"></param>
        private void _AddEventEntryHandler(EventTriggerType _eEventID, EventHandlerBase _handler)
        {
            int eventTypeIndex = -1;

            //添加事件类型
            if (!ContainsEventTriggerType(_eEventID, out eventTypeIndex))
            {
                AddEventEntry(_eEventID);
                eventTypeIndex = Triggers.Count - 1;
            }
            //给该类型添加处理器
            Triggers[eventTypeIndex].AddEventHandler(_handler);
        }

        /// <summary>
        /// 添加触发事件处理器
        /// </summary>
        /// <param name="_eEventID">事件ID</param>
        /// <param name="_eHandlerType">处理器类型</param>
        /// <param name="_handler">用户自己创建的处理器</param>
        /// <returns></returns>
        public void AddEventEntryHandler(EventTriggerType _eEventID, EventHandlerBase _handler)
        {
            if (_handler == null)
            {
                Trace.LogWarning("GeiWoDe Handler ShiKong,NiFangFuZaiDouWoXiao?");
                return;
            }

            _AddEventEntryHandler(_eEventID, _handler);

        }

        /// <summary>
        /// 移除触发事件处理器
        /// </summary>
        /// <param name="_eEventID">事件ID</param>
        /// <param name="_eHandlerType">处理器类型</param>
        /// <param name="_handler">需要移除的处理器</param>
        public void RemoveEventEntryHandler(EventTriggerType _eEventID, EventHandlerBase _handler)
        {
            if (_handler == null)
            {
                return;
            }
            int eventTypeIndex;
            if (!ContainsEventTriggerType(_eEventID, out eventTypeIndex))
                return;

            if (!Triggers[eventTypeIndex].ContainsHandler(_handler))
                return;

            Triggers[eventTypeIndex].RemoveEventHandler(_handler);
        }

        /// <summary>
        /// 获取处理器类型实例
        /// </summary>
        /// <param name="_eHandlerType"></param>
        /// <returns></returns>
        public EventHandlerBase CreateEventEntryHandler(EEventHandlerType _eHandlerType)
        {
            EventHandlerBase newHandler = null;

            newHandler = new EventHandlerBase(_eHandlerType);

            return newHandler;
        }

        /// <summary>
        /// 执行事件处理器方法
        /// </summary>
        /// <param name="id">触发事件</param>
        /// <param name="eventData">UI事件数据</param>
        private void Execute(STriggerExecuteParam eventData)
        {
            for (int i = 0, imax = Triggers.Count; i < imax; ++i)
            {
                var ent = Triggers[i];
                if (ent.eventID == eventData.TriggerType)
                    ent.Execute(eventData);
            }
        }
        #endregion

        #region 事件接口
        public void OnPointerClick(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.PointerClick,eventData);
            Execute(param);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.PointerExit, eventData);
            Execute(param);
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.PointerEnter, eventData);
            Execute(param);
        }

        public void OnInitializePotentialDrag(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.InitializePotentialDrag, eventData);
            Execute(param);
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.BeginDrag, eventData);
            Execute(param);
        }

        public void OnDrag(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Drag, eventData);
            Execute(param);
        }

        public void OnEndDrag(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.EndDrag, eventData);
            Execute(param);
        }

        public void OnDrop(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Drop, eventData);
            Execute(param);
        }

        public void OnScroll(PointerEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Scroll, eventData);
            Execute(param);
        }

        public void OnUpdateSelected(BaseEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.UpdateSelected, eventData);
            Execute(param);
        }

        public void OnSelect(BaseEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Select, eventData);
            Execute(param);
        }

        public void OnDeselect(BaseEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Deselect, eventData);
            Execute(param);
        }

        public void OnMove(AxisEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Move, eventData);
            Execute(param);
        }

        public void OnSubmit(BaseEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Submit, eventData);
            Execute(param);
        }

        public void OnCancel(BaseEventData eventData)
        {
            STriggerExecuteParam param = new STriggerExecuteParam(EventTriggerType.Cancel, eventData);
            Execute(param);
        }
        #endregion
    }
}

