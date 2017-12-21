using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using UnityEngine.Events;
using UnityEngine.UI;
//using static UnityEngine.EventSystems.ExecuteEvents;

namespace UnityEngine.EventSystems
{
    internal class ObjectPool<T> where T : new()
    {
        private readonly Stack<T> m_Stack = new Stack<T>();
        private readonly UnityAction<T> m_ActionOnGet;
        private readonly UnityAction<T> m_ActionOnRelease;

        public int countAll { get; private set; }
        public int countActive { get { return countAll - countInactive; } }
        public int countInactive { get { return m_Stack.Count; } }

        public ObjectPool(UnityAction<T> actionOnGet, UnityAction<T> actionOnRelease)
        {
            m_ActionOnGet = actionOnGet;
            m_ActionOnRelease = actionOnRelease;
        }

        public T Get()
        {
            T element;
            if (m_Stack.Count == 0)
            {
                element = new T();
                countAll++;
            }
            else
            {
                element = m_Stack.Pop();
            }
            if (m_ActionOnGet != null)
                m_ActionOnGet(element);
            return element;
        }

        public void Release(T element)
        {
            if (m_Stack.Count > 0 && ReferenceEquals(m_Stack.Peek(), element))
                Debug.LogError("Internal error. Trying to destroy object that is already released to pool.");
            if (m_ActionOnRelease != null)
                m_ActionOnRelease(element);
            m_Stack.Push(element);
        }
    }

    public class UPointerEventDataEx : PointerEventData
    {
        public bool CanBubbleEvent;
        public UPointerEventDataEx(EventSystem eventSystem) : base(eventSystem)
        {
            CanBubbleEvent = true;
        }
        public void ResetEx()
        {
            CanBubbleEvent = true;
        }
    }

    public static class UExecuteEventsEx 
    {
        private static MethodInfo m_GetEventChain = null;
        /// <summary>
        /// GetEventChain(GameObject root, IList<Transform> eventChain)
        /// </summary>
        public static MethodInfo GetEventChain
        {
            get
            {
                if(m_GetEventChain == null)
                {
                    Type type = typeof(UnityEngine.EventSystems.ExecuteEvents);
                    m_GetEventChain = type.GetMethod("GetEventChain", BindingFlags.Static | BindingFlags.NonPublic);
                }
                return m_GetEventChain;
            }
        }

        private static FieldInfo m_InternalTransformList = null;
        public static List<Transform> InternalTransformList
        {
            get
            {
                if(m_InternalTransformList == null)
                {
                    Type type = typeof(UnityEngine.EventSystems.ExecuteEvents);
                    m_InternalTransformList = type.GetField("s_InternalTransformList", BindingFlags.Static | BindingFlags.NonPublic);
                }
                return m_InternalTransformList.GetValue(null) as List<Transform>;
            }
        }

        private static MethodInfo m_GetEventList = null;
        /// <summary>
        /// GetEventList<T>(GameObject go, IList<IEventSystemHandler> results) where T : IEventSystemHandler
        /// </summary>
        public static MethodInfo GetEventList
        {
            get
            {
                if (m_GetEventList == null)
                {
                    Type type = typeof(UnityEngine.EventSystems.ExecuteEvents);
                    m_GetEventList = type.GetMethod("GetEventList", BindingFlags.Static | BindingFlags.NonPublic);
                }
                return m_GetEventList;
            }
        }


        private static ObjectPool<List<IEventSystemHandler>> s_HandlerListPool = new ObjectPool<List<IEventSystemHandler>>(null, l => l.Clear());

        public static GameObject ExecuteHierarchy<T>(GameObject root, BaseEventData eventData, UnityEngine.EventSystems.ExecuteEvents.EventFunction<T> callbackFunction) where T : IEventSystemHandler
        {

            GetEventChain.Invoke(null, new object[] { root, InternalTransformList });
            // GetEventChain(root, s_InternalTransformList);
            GameObject firstExecuteObject = null;
            UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;

            for (var i = 0; i < InternalTransformList.Count; i++)
            {
                var transform = InternalTransformList[i];

                //当前找到第一个处理事件的对象且需要进行冒泡处理
                if (UnityEngine.EventSystems.ExecuteEvents.Execute(transform.gameObject, eventData, callbackFunction))
                {
                    if(firstExecuteObject == null)
                        firstExecuteObject = transform.gameObject;

                    if (eventDataEx == null || !eventDataEx.CanBubbleEvent)
                        return firstExecuteObject;
                }
            }
            return firstExecuteObject;
        }

        public static bool ExecuteEx<T>(GameObject target, BaseEventData eventData, ExecuteEvents.EventFunction<T> functor) where T : IEventSystemHandler
        {
            GetEventChain.Invoke(null, new object[] { target, InternalTransformList });
            var internalHandlers = s_HandlerListPool.Get();
            MethodInfo GetEventListGeneric = GetEventList.MakeGenericMethod(new Type[] { typeof(T) });

            for (int i=0;i< InternalTransformList.Count;i++)
            {
                var transform = InternalTransformList[i];
                //GetEventList<T>(target, internalHandlers);
                GetEventListGeneric.Invoke(null, new object[] { transform.gameObject, internalHandlers });
            }


            //	if (s_InternalHandlers.Count > 0)
            //		Debug.Log("Executinng " + typeof (T) + " on " + target);
            UPointerEventDataEx eventDataEx = eventData as UPointerEventDataEx;
            for (var i = 0; i < internalHandlers.Count; i++)
            {
                T arg;
                try
                {
                    arg = (T)internalHandlers[i];
                }
                catch (Exception e)
                {
                    var temp = internalHandlers[i];
                    Debug.LogException(new Exception(string.Format("Type {0} expected {1} received.", typeof(T).Name, temp.GetType().Name), e));
                    continue;
                }

                try
                {
                    functor(arg, eventDataEx);
                }
                catch (Exception e)
                {
                    Debug.LogException(e);
                }


                if (!eventDataEx.CanBubbleEvent)
                    break;
            }

            var handlerCount = internalHandlers.Count;
            s_HandlerListPool.Release(internalHandlers);
            return handlerCount > 0;

        }
    }

    public class USpeedInputModule : StandaloneInputModule
    {
        private static FieldInfo m_GetMouseState = null;

        protected UPointerEventDataEx m_CurrentPointerEx = null;
        public UPointerEventDataEx CurrentPointerEx
        {
            get
            {
                return m_CurrentPointerEx;
            }

            set
            {
                m_CurrentPointerEx = value;
            }
        }

        protected MouseState GetMouseState
        {
            get
            {
                if(m_GetMouseState == null)
                {
                    Type type = typeof(UnityEngine.EventSystems.PointerInputModule);
                    m_GetMouseState = type.GetField("m_MouseState", BindingFlags.Instance | BindingFlags.NonPublic);
                }
                return m_GetMouseState.GetValue(this) as MouseState;
            }
        }
        protected override MouseState GetMousePointerEventData(int id)
        {
            UPointerEventDataEx leftData;
            
            var created = GetPointerData(kMouseLeftId, out leftData, true);

            leftData.Reset();

            if (created)
                leftData.position = Input.mousePosition;

            Vector2 pos = Input.mousePosition;
            leftData.delta = pos - leftData.position;
            leftData.position = pos;
            leftData.scrollDelta = Input.mouseScrollDelta;
            leftData.button = PointerEventData.InputButton.Left;
            eventSystem.RaycastAll(leftData, m_RaycastResultCache);
            var raycast = FindFirstRaycast(m_RaycastResultCache);
            leftData.pointerCurrentRaycast = raycast;
            m_RaycastResultCache.Clear();

            // copy the apropriate data into right and middle slots
            UPointerEventDataEx rightData;
            GetPointerData(kMouseRightId, out rightData, true);
            CopyFromTo(leftData, rightData);
            rightData.button = PointerEventData.InputButton.Right;

            UPointerEventDataEx middleData;
            GetPointerData(kMouseMiddleId, out middleData, true);
            CopyFromTo(leftData, middleData);
            middleData.button = PointerEventData.InputButton.Middle;

            GetMouseState.SetButtonState(PointerEventData.InputButton.Left, StateForMouseButton(0), leftData);
            GetMouseState.SetButtonState(PointerEventData.InputButton.Right, StateForMouseButton(1), rightData);
            GetMouseState.SetButtonState(PointerEventData.InputButton.Middle, StateForMouseButton(2), middleData);

            return GetMouseState;

        }
        public override void Process()
        {
            bool usedEvent = SendUpdateEventToSelectedObject();

            if (eventSystem.sendNavigationEvents)
            {
                if (!usedEvent)
                    usedEvent |= SendMoveEventToSelectedObject();

                if (!usedEvent)
                    SendSubmitEventToSelectedObject();
            }

            ProcessMouseEvent(0);
        }

        protected bool GetPointerData(int id, out UPointerEventDataEx data, bool create)
        {
            PointerEventData pData;
            if (!m_PointerData.TryGetValue(id, out pData) && create)
            {
                data = new UPointerEventDataEx(eventSystem)
                {
                    pointerId = id,
                };
                m_PointerData.Add(id, data);
                return true;
            }

            data = pData as UPointerEventDataEx;
            data.ResetEx();
            return false;
        }

        protected new PointerEventData GetLastPointerEventData(int id)
        {
            UPointerEventDataEx data;
            GetPointerData(id, out data, false);
            return data;
        }

        protected new void ProcessMouseEvent(int id)
        {
            var mouseData = GetMousePointerEventData(id);

            var leftButtonData = mouseData.GetButtonState(PointerEventData.InputButton.Left).eventData;
            CurrentPointerEx = leftButtonData.buttonData as UPointerEventDataEx;
            // Process the first mouse button fully
            ProcessMousePress(leftButtonData);
            ProcessMove(leftButtonData.buttonData);
            ProcessDrag(leftButtonData.buttonData);

            // Now process right / middle clicks
            var rightButtonData = mouseData.GetButtonState(PointerEventData.InputButton.Right).eventData;
            CurrentPointerEx = rightButtonData.buttonData as UPointerEventDataEx;
            ProcessMousePress(rightButtonData);
            ProcessDrag(rightButtonData.buttonData);

            var middleButtonData = mouseData.GetButtonState(PointerEventData.InputButton.Middle).eventData;
            CurrentPointerEx = rightButtonData.buttonData as UPointerEventDataEx;
            ProcessMousePress(middleButtonData);
            ProcessDrag(middleButtonData.buttonData);

            if (!Mathf.Approximately(leftButtonData.buttonData.scrollDelta.sqrMagnitude, 0.0f))
            {
                var scrollHandler = ExecuteEvents.GetEventHandler<IScrollHandler>(leftButtonData.buttonData.pointerCurrentRaycast.gameObject);
                UExecuteEventsEx.ExecuteHierarchy(scrollHandler, leftButtonData.buttonData, ExecuteEvents.scrollHandler);
            }
            //CurrentPointerEx = null;
        }

        protected new void ProcessMousePress(MouseButtonEventData data)
        {
            var pointerEvent = data.buttonData;
            var currentOverGo = pointerEvent.pointerCurrentRaycast.gameObject;

            // PointerDown notification
            if (data.PressedThisFrame())
            {
                pointerEvent.eligibleForClick = true;
                pointerEvent.delta = Vector2.zero;
                pointerEvent.dragging = false;
                pointerEvent.useDragThreshold = true;
                pointerEvent.pressPosition = pointerEvent.position;
                pointerEvent.pointerPressRaycast = pointerEvent.pointerCurrentRaycast;

                DeselectIfSelectionChanged(currentOverGo, pointerEvent);

                // search for the control that will receive the press
                // if we can't find a press handler set the press
                // handler to be what would receive a click.
                //原来不会冒泡是酱紫的： var newPressed = ExecuteEvents.ExecuteHierarchy(currentOverGo, pointerEvent, ExecuteEvents.pointerDownHandler);
                var newPressed = UExecuteEventsEx.ExecuteHierarchy(currentOverGo, pointerEvent, ExecuteEvents.pointerDownHandler);

                // didnt find a press handler... search for a click handler
                if (newPressed == null || newPressed != currentOverGo)
                    newPressed = ExecuteEvents.GetEventHandler<IPointerClickHandler>(currentOverGo);

                // Debug.Log("Pressed: " + newPressed);

                float time = Time.unscaledTime;

                if (newPressed == pointerEvent.lastPress)
                {
                    var diffTime = time - pointerEvent.clickTime;
                    if (diffTime < 0.3f)
                        ++pointerEvent.clickCount;
                    else
                        pointerEvent.clickCount = 1;

                    pointerEvent.clickTime = time;
                }
                else
                {
                    pointerEvent.clickCount = 1;
                }

                pointerEvent.pointerPress = newPressed;
                pointerEvent.rawPointerPress = currentOverGo;

                pointerEvent.clickTime = time;

                // Save the drag handler as well
                pointerEvent.pointerDrag = ExecuteEvents.GetEventHandler<IDragHandler>(currentOverGo);

                if (pointerEvent.pointerDrag != null)
                    ExecuteEvents.Execute(pointerEvent.pointerDrag, pointerEvent, ExecuteEvents.initializePotentialDrag);
            }

            // PointerUp notification
            if (data.ReleasedThisFrame())
            {
                // Debug.Log("Executing pressup on: " + pointer.pointerPress);
                //ExecuteEvents.Execute(pointerEvent.pointerPress, pointerEvent, ExecuteEvents.pointerUpHandler);
                UExecuteEventsEx.ExecuteEx(pointerEvent.pointerPress, pointerEvent, ExecuteEvents.pointerUpHandler);

                // Debug.Log("KeyCode: " + pointer.eventData.keyCode);

                // see if we mouse up on the same element that we clicked on...
                var pointerUpHandler = ExecuteEvents.GetEventHandler<IPointerClickHandler>(currentOverGo);

                // PointerClick and Drop events
                if (pointerEvent.pointerPress == pointerUpHandler && pointerEvent.eligibleForClick)
                {
                    UExecuteEventsEx.ExecuteEx(pointerEvent.pointerPress, pointerEvent, ExecuteEvents.pointerClickHandler);
                }
                else if (pointerEvent.pointerDrag != null && pointerEvent.dragging)
                {
                    UExecuteEventsEx.ExecuteHierarchy(currentOverGo, pointerEvent, ExecuteEvents.dropHandler);
                }

                pointerEvent.eligibleForClick = false;
                pointerEvent.pointerPress = null;
                pointerEvent.rawPointerPress = null;

                if (pointerEvent.pointerDrag != null && pointerEvent.dragging)
                    ExecuteEvents.Execute(pointerEvent.pointerDrag, pointerEvent, ExecuteEvents.endDragHandler);

                pointerEvent.dragging = false;
                pointerEvent.pointerDrag = null;

                // redo pointer enter / exit to refresh state
                // so that if we moused over somethign that ignored it before
                // due to having pressed on something else
                // it now gets it.
                if (currentOverGo != pointerEvent.pointerEnter)
                {
                    HandlePointerExitAndEnter(pointerEvent, null);
                    HandlePointerExitAndEnter(pointerEvent, currentOverGo);
                }
            }
        }

    }


    public static class EventSystemExtension
    {
        public static UPointerEventDataEx CurrentPointerEventData(this EventSystem _EventSystem)
        {
            UPointerEventDataEx resultEventData = null;
            var inputModule = _EventSystem.currentInputModule as USpeedInputModule;
            if (inputModule != null)
            {
                resultEventData = inputModule.CurrentPointerEx;
            }
            else
            {
                Debug.LogWarning("当前UI输入模块非USpeedInputModule");
            }

           return resultEventData;
        }
    }
}
