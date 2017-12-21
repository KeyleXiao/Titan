using System;
using System.Collections.Generic;
using UnityEngine.Events;
using UnityEngine.Serialization;

namespace UnityEngine.EventSystems
{
    [AddComponentMenu("Event/Event Trigger")]
    public class EventTriggerEx :
        MonoBehaviour,
        //IPointerEnterHandler,
        //IPointerExitHandler,
        IPointerDownHandler,
        IPointerUpHandler,
        IPointerClickHandler
        //IInitializePotentialDragHandler,
        //IBeginDragHandler,
        //IDragHandler,
        //IEndDragHandler,
        //IDropHandler,
        //IScrollHandler,
        //IUpdateSelectedHandler,
        //ISelectHandler,
        //IDeselectHandler,
        //IMoveHandler,
        //ISubmitHandler,
        //ICancelHandler
    {
        [Serializable]
        public class TriggerEvent : UnityEvent<BaseEventData, TriggerParam>
        {}

        public class TriggerParam
        {
            public bool bBubble = true;

            
        }

        [Serializable]
        public class Entry
        {
            public EventTriggerType eventID = EventTriggerType.PointerClick;
            public TriggerEvent callback = new TriggerEvent();
        }

        [FormerlySerializedAs("delegates")]
        [SerializeField]
        protected  List<Entry> m_Delegates;

        [Obsolete("Please use triggers instead (UnityUpgradable) -> triggers", true)]
        public List<Entry> delegates;

        protected EventTriggerEx()
        {}

        public List<Entry> triggers
        {
            get
            {
                if (m_Delegates == null)
                    m_Delegates = new List<Entry>();
                return m_Delegates;
            }
            set { m_Delegates = value; }
        }

        static public EventTriggerEx Get(GameObject go)
        {
            EventTriggerEx listener = go.GetComponent<EventTriggerEx>();
            if (listener == null)
            {
                listener = go.AddComponent<EventTriggerEx>();
                listener.AddListener(EventTriggerType.PointerDown, listener.DefaultPointerDown);
            }
            return listener;
        }

        protected void DefaultPointerDown(BaseEventData eventData, TriggerParam param)
        {

        }

        public void BubbleEvent(EventTriggerType _nId, PointerEventData _ePointerEventData)
        {
            
            var currentParentGo = _ePointerEventData.pointerCurrentRaycast.gameObject.transform.parent.gameObject;

            //PointerEventData pointer = new PointerEventData(EventSystem.current);
            //pointer.position = Input.mousePosition;
            //List<RaycastResult> hits = new List<RaycastResult>();
            //EventSystem.current.RaycastAll(pointer, hits);

            //foreach(var hit in hits)
            //{
            //    Debug.Log("RaycastAll hit:" + hit.gameObject);
            //}
            //return;

            RaycastResult rayRes = _ePointerEventData.pointerCurrentRaycast;
            rayRes.gameObject = currentParentGo;
            _ePointerEventData.pointerCurrentRaycast = rayRes;

            GameObject ExecuteGo = null;
            switch (_nId)
            {
                case EventTriggerType.PointerDown:
                    ExecuteGo = ExecuteEvents.ExecuteHierarchy(currentParentGo, _ePointerEventData, ExecuteEvents.pointerDownHandler);
                    break;
                case EventTriggerType.PointerClick:
                    ExecuteGo = ExecuteEvents.ExecuteHierarchy(currentParentGo, _ePointerEventData, ExecuteEvents.pointerClickHandler);
                    break;
                case EventTriggerType.PointerUp:
                    ExecuteGo = ExecuteEvents.ExecuteHierarchy(currentParentGo, _ePointerEventData, ExecuteEvents.pointerUpHandler);
                    break;
            }

            if(ExecuteGo != null)
            {
                rayRes = _ePointerEventData.pointerCurrentRaycast;
                rayRes.gameObject = ExecuteGo;
                _ePointerEventData.pointerCurrentRaycast = rayRes;
                BubbleEvent(_nId, _ePointerEventData);
            }
        }

        protected bool CanHandleEvent(EventTriggerType _nID,GameObject _specGO)
        {

            switch (_nID)
            {
                case EventTriggerType.PointerUp:
                    return ExecuteEvents.CanHandleEvent<IPointerUpHandler>(_specGO);
                case EventTriggerType.PointerDown:
                    return ExecuteEvents.CanHandleEvent<IPointerDownHandler>(_specGO);
                case EventTriggerType.PointerClick:
                    return ExecuteEvents.CanHandleEvent<IPointerClickHandler>(_specGO);
                default:
                    Debug.LogWarning("目前能冒泡的事件只使用Up,Down,Click事件");
                    break;
            }

            return false;
        }

        private void Execute(EventTriggerType id, BaseEventData eventData)
        {
            for (int i = 0, imax = triggers.Count; i < imax; ++i)
            {
                var ent = triggers[i];
                if (ent.eventID == id && ent.callback != null)
                {
                    TriggerParam param = new TriggerParam();

                    //非PointerEventData的话就没意义了，因为拿不到当前获取的GameObject
                    if (eventData is PointerEventData)
                    {
                        PointerEventData pointer = eventData as PointerEventData;
                        //判断当前对象是否有响应的事件
                        if (CanHandleEvent(id, pointer.pointerCurrentRaycast.gameObject))
                            ent.callback.Invoke(eventData, param);
                        //判断是否需要进行事件冒泡
                        if (param.bBubble)
                            BubbleEvent(id, pointer);
                    }
                    else
                    {
                        ent.callback.Invoke(eventData, param);
                    }

                }
                
            }
        }

        public void AddListener(EventTriggerType _nID, UnityAction<BaseEventData, TriggerParam> _callback)
        {
            Entry entity = new Entry();
            entity.eventID = _nID;
            entity.callback.AddListener(_callback);
            triggers.Add(entity);
        }


        public virtual void OnPointerDown(PointerEventData eventData)
        {
            Execute(EventTriggerType.PointerDown, eventData);
        }

        public virtual void OnPointerUp(PointerEventData eventData)
        {
            Execute(EventTriggerType.PointerUp, eventData);
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            Execute(EventTriggerType.PointerClick, eventData);
        }


        #region 这些方法暂时不支持冒泡
        protected virtual void OnPointerEnter(PointerEventData eventData)
        {
            Execute(EventTriggerType.PointerEnter, eventData);
        }

        protected virtual void OnPointerExit(PointerEventData eventData)
        {
            Execute(EventTriggerType.PointerExit, eventData);
        }

        protected virtual void OnDrag(PointerEventData eventData)
        {
            Execute(EventTriggerType.Drag, eventData);
        }

        protected virtual void OnDrop(PointerEventData eventData)
        {
            Execute(EventTriggerType.Drop, eventData);
        }
        protected virtual void OnSelect(BaseEventData eventData)
        {
            Execute(EventTriggerType.Select, eventData);
        }

        protected virtual void OnDeselect(BaseEventData eventData)
        {
            Execute(EventTriggerType.Deselect, eventData);
        }

        protected virtual void OnScroll(PointerEventData eventData)
        {
            Execute(EventTriggerType.Scroll, eventData);
        }

        protected virtual void OnMove(AxisEventData eventData)
        {
            Execute(EventTriggerType.Move, eventData);
        }

        protected virtual void OnUpdateSelected(BaseEventData eventData)
        {
            Execute(EventTriggerType.UpdateSelected, eventData);
        }

        protected virtual void OnInitializePotentialDrag(PointerEventData eventData)
        {
            Execute(EventTriggerType.InitializePotentialDrag, eventData);
        }

        protected virtual void OnBeginDrag(PointerEventData eventData)
        {
            Execute(EventTriggerType.BeginDrag, eventData);
        }

        protected virtual void OnEndDrag(PointerEventData eventData)
        {
            Execute(EventTriggerType.EndDrag, eventData);
        }

        protected virtual void OnSubmit(BaseEventData eventData)
        {
            Execute(EventTriggerType.Submit, eventData);
        }

        protected virtual void OnCancel(BaseEventData eventData)
        {
            Execute(EventTriggerType.Cancel, eventData);
        }
        #endregion

    }
}
