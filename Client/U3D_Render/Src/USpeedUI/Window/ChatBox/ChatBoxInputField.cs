using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.Serialization;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.ChatBox;
using DataCenter;
using USpeedUI;
using USpeedUI.Chat;

namespace UnityEngine.UI
{
    internal static class SetPropertyUtility
    {
        public static bool SetColor(ref Color currentValue, Color newValue)
        {
            if (currentValue.r == newValue.r && currentValue.g == newValue.g && currentValue.b == newValue.b && currentValue.a == newValue.a)
                return false;

            currentValue = newValue;
            return true;
        }

        public static bool SetStruct<T>(ref T currentValue, T newValue) where T : struct
        {
            if (currentValue.Equals(newValue))
                return false;

            currentValue = newValue;
            return true;
        }

        public static bool SetClass<T>(ref T currentValue, T newValue) where T : class
        {
            if ((currentValue == null && newValue == null) || (currentValue != null && currentValue.Equals(newValue)))
                return false;

            currentValue = newValue;
            return true;
        }
    }

    /// <summary>
    /// Editable text input field.
    /// </summary>
    
    public class InputFieldEx
        : Selectable,
        IUpdateSelectedHandler,
        IBeginDragHandler,
        IDragHandler,
        IEndDragHandler,
        IPointerClickHandler,
        ISubmitHandler,
        ICanvasElement
    {
        public enum LineType
        {
            SingleLine,
            MultiLineSubmit,
            MultiLineNewline
        }

        [Serializable]
        public class SubmitEvent : UnityEvent<string> { }

        [Serializable]
        public class OnChangeEvent : UnityEvent<string> { }

		[Serializable]
		public class OnFocusEvent : UnityEvent<bool> { }

        #region Exposed properties
        /// <summary>
        /// Text Text used to display the input's value.
        /// </summary>
        [SerializeField]
        [FormerlySerializedAs("content")]
        protected RectTransform m_content;
        protected List<IChatItem> m_itemList = new List<IChatItem>();
        public bool Empty { get { return (m_itemList.Count == 0); } }

        [SerializeField]
        protected Graphic m_Placeholder;

        [SerializeField]
        protected LineType m_LineType = LineType.SingleLine;

        /// <summary>
        /// Maximum number of characters allowed before input no longer works.
        /// </summary>
        [FormerlySerializedAs("characterLimit")]
        [SerializeField]
        protected int m_CharacterLimit = 0;

        /// <summary>
        /// Event delegates triggered when the input field submits its data.
        /// </summary>
        [FormerlySerializedAs("onSubmit")]
        [SerializeField]
        protected SubmitEvent m_EndEdit = new SubmitEvent();

        // 按下回车事件
        [FormerlySerializedAs("onEnterSubmit")]
        [SerializeField]
        protected SubmitEvent m_EnterSubmit = new SubmitEvent();

        /// <summary>
        /// Event delegates triggered when the input field changes its data.
        /// </summary>
        [FormerlySerializedAs("onValueChange")]
        [SerializeField]
        protected OnChangeEvent m_OnValueChange = new OnChangeEvent();

        [FormerlySerializedAs("onFocusChange")]
        [SerializeField]
        protected OnFocusEvent m_OnFocusChange = new OnFocusEvent();

        [FormerlySerializedAs("selectionColor")]
        [SerializeField]
        protected Color m_SelectionColor = new Color(168f / 255f, 206f / 255f, 255f / 255f, 192f / 255f);

        [SerializeField]
        [Range(0f, 4f)]
        protected float m_CaretBlinkRate = 0.85f;

        #endregion

        [SerializeField]
        protected String m_lastInputString = "";
        protected int m_lastInputStringIndex = 0;
        protected bool m_lastInputInserted = false;
        protected String m_curIngoreInputString = "";
        protected int m_CaretPosition = 0;
        protected int m_CaretSelectPosition = 0;
        private RectTransform caretRectTrans = null;
        protected UIVertex[] m_CursorVerts = null;
        private CanvasRenderer m_CachedInputRenderer;
        private bool m_PreventFontCallback = false;
        [NonSerialized]
        protected Mesh m_Mesh;
        private bool m_AllowInput = false;
        private bool m_ShouldActivateNextUpdate = false;
        private bool m_UpdateDrag = false;
        private bool m_DragPositionOutOfBounds = false;
        private const float kHScrollSpeed = 0.05f;
        private const float kVScrollSpeed = 0.10f;
        protected bool m_CaretVisible;
        private Coroutine m_BlinkCoroutine = null;
        private float m_BlinkStartTime = 0.0f;
        protected int m_DrawStart = 0;
        protected int m_DrawEnd = 0;
        private Coroutine m_DragCoroutine = null;
        //private string m_OriginalText = "";
        private bool m_WasCanceled = false;
        private bool m_HasDoneFocusTransition = false;

        private List<float> m_LineHeight = new List<float>();

        protected InputFieldEx()
        { }

        protected Mesh mesh
        {
            get
            {
                if (m_Mesh == null)
                {
                    m_Mesh = new Mesh();
                    m_Mesh.name = "ChatBoxInputField";
                }
                    
                return m_Mesh;
            }
        }

        public bool isFocused
        {
            get { return m_AllowInput; }
        }

        public float caretBlinkRate
        {
            get { return m_CaretBlinkRate; }
            set
            {
                if (SetPropertyUtility.SetStruct(ref m_CaretBlinkRate, value))
                {
                    if (m_AllowInput)
                        SetCaretActive();
                }
            }
        }

        public Graphic placeholder { get { return m_Placeholder; } set { SetPropertyUtility.SetClass(ref m_Placeholder, value); } }

        public Color selectionColor { get { return m_SelectionColor; } set { SetPropertyUtility.SetColor(ref m_SelectionColor, value); } }

        public SubmitEvent onEndEdit { get { return m_EndEdit; } set { SetPropertyUtility.SetClass(ref m_EndEdit, value); } }
        public SubmitEvent onEnterSubmit { get { return m_EnterSubmit; } set { SetPropertyUtility.SetClass(ref m_EnterSubmit, value); } }

        public OnChangeEvent onValueChange { get { return m_OnValueChange; } set { SetPropertyUtility.SetClass(ref m_OnValueChange, value); } }

        public OnFocusEvent onFocusChange { get { return m_OnFocusChange; } set { SetPropertyUtility.SetClass(ref m_OnFocusChange, value); } }

        public int characterLimit { get { return m_CharacterLimit; } set { SetPropertyUtility.SetStruct(ref m_CharacterLimit, value); } }

        // Content Type related

        public LineType lineType { get { return m_LineType; } set { SetPropertyUtility.SetStruct(ref m_LineType, value); } }

        // Derived property
        public bool multiLine { get { return m_LineType == LineType.MultiLineNewline || lineType == LineType.MultiLineSubmit; } }
        public bool wasCanceled { get { return m_WasCanceled; } }

        protected void ClampPos(ref int pos)
        {
            if (pos < 0)
            {
                pos = 0;
            }
            else
            {
                int totalLen = GetTotalLen();
                if (pos > totalLen) pos = totalLen;
            }
        }

        protected int GetTotalLen()
        {
            int totalLen = 0;
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                IChatItem item = m_itemList[i];
                if (item != null)
                    totalLen += item.GetConvertCharCount();
            }
            return totalLen;
        }

        /// <summary>
        /// Current position of the cursor.
        /// Getters are public Setters are protected
        /// </summary>

        protected int caretPositionInternal { get { return m_CaretPosition/* + Input.compositionString.Length*/; } set { m_CaretPosition = value; ClampPos(ref m_CaretPosition); } }
        protected int caretSelectPositionInternal { get { return m_CaretSelectPosition/* + Input.compositionString.Length*/; } set { m_CaretSelectPosition = value; ClampPos(ref m_CaretSelectPosition); } }
        private bool hasSelection { get { return caretPositionInternal != caretSelectPositionInternal; } }

#if UNITY_EDITOR
        [Obsolete("caretSelectPosition has been deprecated. Use selectionFocusPosition instead (UnityUpgradable) -> selectionFocusPosition", true)]
        public int caretSelectPosition { get { return selectionFocusPosition; } protected set { selectionFocusPosition = value; } }
#endif

        /// <summary>
        /// Get: Returns the focus position as thats the position that moves around even during selection.
        /// Set: Set both the anchor and focus position such that a selection doesn't happen
        /// </summary>

        public int caretPosition
        {
            get { return m_CaretSelectPosition/* + Input.compositionString.Length*/; }
            set { selectionAnchorPosition = value; selectionFocusPosition = value; }
        }

        /// <summary>
        /// Get: Returns the fixed position of selection
        /// Set: If Input.compositionString is 0 set the fixed position
        /// </summary>

        public int selectionAnchorPosition
        {
            get { return m_CaretPosition/* + Input.compositionString.Length*/; }
            set
            {
                if (Input.compositionString.Length != 0)
                    return;

                m_CaretPosition = value;
                ClampPos(ref m_CaretPosition);
            }
        }

        /// <summary>
        /// Get: Returns the variable position of selection
        /// Set: If Input.compositionString is 0 set the variable position
        /// </summary>

        public int selectionFocusPosition
        {
            get { return m_CaretSelectPosition/* + Input.compositionString.Length*/; }
            set
            {
                if (Input.compositionString.Length != 0)
                    return;

                m_CaretSelectPosition = value;
                ClampPos(ref m_CaretSelectPosition);
            }
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            //if (m_Text == null)
            //    m_Text = string.Empty;
            m_DrawStart = 0;
            m_DrawEnd = GetTotalLen();
            //if (m_TextComponent != null)
            {
                //m_TextComponent.RegisterDirtyVerticesCallback(MarkGeometryAsDirty);
                //m_TextComponent.RegisterDirtyVerticesCallback(UpdateLabel);
                UpdateLabel();
            }
        }

        protected override void OnDisable()
        {
            // the coroutine will be terminated, so this will ensure it restarts when we are next activated
            m_BlinkCoroutine = null;

            DeactivateInputField();
            //if (m_TextComponent != null)
            {
                //m_TextComponent.UnregisterDirtyVerticesCallback(MarkGeometryAsDirty);
                //m_TextComponent.UnregisterDirtyVerticesCallback(UpdateLabel);
            }
            CanvasUpdateRegistry.UnRegisterCanvasElementForRebuild(this);

            if (m_CachedInputRenderer)
                m_CachedInputRenderer.SetMesh(null);

            if (m_Mesh)
                DestroyImmediate(m_Mesh);
            m_Mesh = null;

            base.OnDisable();
        }

        IEnumerator CaretBlink()
        {
            // Always ensure caret is initially visible since it can otherwise be confusing for a moment.
            m_CaretVisible = true;
            yield return null;

            while (isFocused && m_CaretBlinkRate > 0)
            {
                // the blink rate is expressed as a frequency
                float blinkPeriod = 1f / m_CaretBlinkRate;

                // the caret should be ON if we are in the first half of the blink period
                bool blinkState = (Time.unscaledTime - m_BlinkStartTime) % blinkPeriod < blinkPeriod / 2;
                if (m_CaretVisible != blinkState)
                {
                    m_CaretVisible = blinkState;
                    UpdateGeometry();
                }

                // Then wait again.
                yield return null;
            }
            m_BlinkCoroutine = null;
        }

        void SetCaretVisible()
        {
            if (!m_AllowInput)
                return;

            m_CaretVisible = true;
            m_BlinkStartTime = Time.unscaledTime;
            SetCaretActive();
        }

        // SetCaretActive will not set the caret immediately visible - it will wait for the next time to blink.
        // However, it will handle things correctly if the blink speed changed from zero to non-zero or non-zero to zero.
        void SetCaretActive()
        {
            if (!m_AllowInput)
                return;

            if (m_CaretBlinkRate > 0.0f)
            {
                if (m_BlinkCoroutine == null)
                    m_BlinkCoroutine = StartCoroutine(CaretBlink());
            }
            else
            {
                m_CaretVisible = true;
            }
        }

        protected virtual void OnFocus()
        {
			// SelectAll();
		}

		protected void SelectAll()
        {
            caretPositionInternal = GetTotalLen();
            caretSelectPositionInternal = 0;
        }

        public void MoveTextEnd(bool shift)
        {
            int position = GetTotalLen();

            if (shift)
            {
                caretSelectPositionInternal = position;
            }
            else
            {
                caretPositionInternal = position;
                caretSelectPositionInternal = caretPositionInternal;
            }
            UpdateLabel();
        }

        public void MoveTextStart(bool shift)
        {
            int position = 0;

            if (shift)
            {
                caretSelectPositionInternal = position;
            }
            else
            {
                caretPositionInternal = position;
                caretSelectPositionInternal = caretPositionInternal;
            }

            UpdateLabel();
        }

        static string clipboard
        {
            get
            {
                return GUIUtility.systemCopyBuffer;
            }
            set
            {
                GUIUtility.systemCopyBuffer = value;
            }
        }

        /// <summary>
        /// Update the text based on input.
        /// </summary>
        // TODO: Make LateUpdate a coroutine instead. Allows us to control the update to only be when the field is active.
        public virtual void LateUpdate()
        {
            // Only activate if we are not already activated.
            if (m_ShouldActivateNextUpdate)
            {
                if (!isFocused)
                {
                    ActivateInputFieldInternal();
                    m_ShouldActivateNextUpdate = false;
                    return;
                }

                // Reset as we are already activated.
                m_ShouldActivateNextUpdate = false;
            }

            // 特殊处理
            if (m_lastInputString != Input.compositionString && Input.compositionString != " ")
            {
                UpdateLabel();
            }
        }

        private Vector2 ScreenToLocal(PointerEventData eventData)
        {
            Vector2 screen = eventData.position;
            Canvas theCanvas = UISystem.Instance.GetCanvas();
            if (theCanvas == null)
                return screen;

            Vector3 pos = Vector3.zero;
            if (theCanvas.renderMode == RenderMode.ScreenSpaceOverlay)
            {
                pos = m_content.InverseTransformPoint(screen);
            }
            else
            {
                Ray mouseRay = eventData.pressEventCamera.ScreenPointToRay(screen);
                float dist;
                Plane plane = new Plane(m_content.forward, m_content.position);
                plane.Raycast(mouseRay, out dist);
                pos = m_content.InverseTransformPoint(mouseRay.GetPoint(dist));
            }
            Rect contentRect = m_content.rect;
            return new Vector2((pos.x + contentRect.width * 0.5f) * theCanvas.scaleFactor, (pos.y + contentRect.height * 0.5f) * theCanvas.scaleFactor);
        }

        private int GetUnclampedCharacterLineFromPosition(Vector2 pos)
        {
            if (!multiLine)
                return 0;

            return 0;
            /*
            RectTransform rectTransform = this.transform as RectTransform;
            float height = rectTransform.rect.yMax;

            // Position is above first line.
            if (pos.y > height)
                return -1;

            for (int i = 0; i < m_LineHeight.Count; ++i)
            {
                float lineHeight = m_LineHeight[i];
                if (pos.y <= height && pos.y > (height - lineHeight))
                    return i;
                height -= lineHeight;
            }

            // Position is after last line.
            return m_LineHeight.Count;
            */
        }

        protected float GetContentItemStartX(int index)
        {
            if (index <= 0)
                return 0f;

            float startX = 0f;
            int curIndex = 0;
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                IChatItem item = m_itemList[i];
                if (item == null)
                    return 0f;

                int nCharCount = item.GetConvertCharCount();
                if (curIndex + nCharCount <= index)
                {
                    curIndex += nCharCount;
                    startX += (item.GetItemWidth());
                }
                else
                {
                    int offsetIndex = index - curIndex;
                    startX += item.GetCharStart(offsetIndex);
                    return startX;
                }
            }

            return startX;
        }

        protected float GetContentItemWidth(int index)
        {
            if (index < 0)
                return 0f;

            int curIndex = 0;
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                IChatItem item = m_itemList[i];
                if (item == null)
                    return 0f;

                int nCharCount = item.GetConvertCharCount();
                if (curIndex + nCharCount <= index)
                {
                    curIndex += nCharCount;
                }
                else
                {
                    int offsetIndex = index - curIndex;
                    return item.GetCharWidth(offsetIndex);
                }
            }

            return 0f;
        }

        public String GetCharString(int beginIndex, int endIndex)
        {
            String result = "";
            int totalLen = GetTotalLen();
            if (beginIndex < 0 || beginIndex >= totalLen)
                return result;
            if (endIndex < beginIndex || endIndex >= totalLen)
                endIndex = totalLen - 1;

            int curIndex = 0;
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                IChatItem item = m_itemList[i];
                if (item == null)
                    return result;

                int itemCharCount = item.GetConvertCharCount();
                if (curIndex + itemCharCount <= beginIndex)
                {
                    curIndex += itemCharCount;
                }
                else
                {
                    int offsetIndex = beginIndex - curIndex;
                    result += item.GetCharString(offsetIndex, endIndex - curIndex);
                    curIndex += itemCharCount;
                    beginIndex = curIndex;
                    if (beginIndex > endIndex)
                        break;
                }
            }

            return result;
        }

        public virtual void ClearContent()
        {
            DeleteContentChar(0, GetTotalLen());
        }

        /// <summary>
        /// Given an input position in local space on the Text return the index for the selection cursor at this position.
        /// </summary>

        protected int GetCharacterIndexFromPosition(Vector2 pos)
        {
            if (m_itemList.Count == 0)
                return 0;

            int line = GetUnclampedCharacterLineFromPosition(pos);
            if (line < 0)
                return 0;

            int startCharIndex = GetLineStartPosition(line);
            int endCharIndex = GetLineEndPosition(line);

            for (int i = startCharIndex; i < endCharIndex; i++)
            {
                float startX = GetContentItemStartX(i);
                float width = GetContentItemWidth(i);
                float distToCharStart = pos.x - startX;
                float distToCharEnd = startX + width - pos.x;
                if (distToCharStart < distToCharEnd)
                    return i;
            }

            return endCharIndex;
        }

        private bool MayDrag(PointerEventData eventData)
        {
            return IsActive() &&
                   IsInteractable() &&
                   eventData.button == PointerEventData.InputButton.Left;
        }

        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            if (!MayDrag(eventData))
                return;

            m_UpdateDrag = true;
        }

        public virtual void OnDrag(PointerEventData eventData)
        {
            if (!MayDrag(eventData))
                return;

            Vector2 localMousePos;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(m_content, eventData.position, eventData.pressEventCamera, out localMousePos);
            localMousePos.x += m_content.rect.width * 0.5f;
            caretSelectPositionInternal = GetCharacterIndexFromPosition(localMousePos) + m_DrawStart;
            MarkGeometryAsDirty();

            m_DragPositionOutOfBounds = !RectTransformUtility.RectangleContainsScreenPoint(m_content, eventData.position, eventData.pressEventCamera);
            if (m_DragPositionOutOfBounds && m_DragCoroutine == null)
                m_DragCoroutine = StartCoroutine(MouseDragOutsideRect(eventData));

            eventData.Use();
        }

        IEnumerator MouseDragOutsideRect(PointerEventData eventData)
        {
            RectTransform rectTransform = this.transform as RectTransform;
            while (m_UpdateDrag && m_DragPositionOutOfBounds)
            {
                Vector2 localMousePos;
                RectTransformUtility.ScreenPointToLocalPointInRectangle(m_content, eventData.position, eventData.pressEventCamera, out localMousePos);
                localMousePos.x += m_content.rect.width * 0.5f;

                Rect rect = rectTransform.rect;

                if (multiLine)
                {
                    if (localMousePos.y > rect.yMax)
                        MoveUp(true, true);
                    else if (localMousePos.y < rect.yMin)
                        MoveDown(true, true);
                }
                else
                {
                    if (localMousePos.x < rect.xMin)
                        MoveLeft(true, false);
                    else if (localMousePos.x > rect.xMax)
                        MoveRight(true, false);
                }
                UpdateLabel();
                float delay = multiLine ? kVScrollSpeed : kHScrollSpeed;
                yield return new WaitForSeconds(delay);
            }
            m_DragCoroutine = null;
        }

        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (!MayDrag(eventData))
                return;

            m_UpdateDrag = false;
        }

        public override void OnPointerDown(PointerEventData eventData)
        {
            if (!MayDrag(eventData))
                return;

            EventSystem.current.SetSelectedGameObject(gameObject, eventData);

            bool hadFocusBefore = m_AllowInput;
            base.OnPointerDown(eventData);

            // Only set caret position if we didn't just get focus now.
            // Otherwise it will overwrite the select all on focus.
            if (hadFocusBefore)
            {
                Vector2 pos;
                RectTransformUtility.ScreenPointToLocalPointInRectangle(m_content, eventData.position, eventData.pressEventCamera, out pos);
                pos.x += m_content.rect.width * 0.5f;
                caretSelectPositionInternal = caretPositionInternal = GetCharacterIndexFromPosition(pos) + m_DrawStart;
            }
            UpdateLabel();
            eventData.Use();
        }

        protected enum EditState
        {
            Continue,
            Finish
        }

        protected EditState KeyPressed(Event evt)
        {

            var currentEventModifiers = evt.modifiers;
            RuntimePlatform rp = Application.platform;
            bool isMac = (rp == RuntimePlatform.OSXEditor || rp == RuntimePlatform.OSXPlayer);
            bool ctrl = isMac ? (currentEventModifiers & EventModifiers.Command) != 0 : (currentEventModifiers & EventModifiers.Control) != 0;
            bool shift = (currentEventModifiers & EventModifiers.Shift) != 0;
            bool alt = (currentEventModifiers & EventModifiers.Alt) != 0;
            bool ctrlOnly = ctrl && !alt && !shift;

            switch (evt.keyCode)
            {
                case KeyCode.Backspace:
                    {
                        Backspace();
                        return EditState.Continue;
                    }

                case KeyCode.Delete:
                    {
                        ForwardSpace();
                        return EditState.Continue;
                    }

                case KeyCode.Home:
                    {
                        MoveTextStart(shift);
                        return EditState.Continue;
                    }

                case KeyCode.End:
                    {
                        MoveTextEnd(shift);
                        return EditState.Continue;
                    }

                // Select All
                case KeyCode.A:
                    {
                        if (ctrlOnly)
                        {
                            SelectAll();
                            return EditState.Continue;
                        }
                        break;
                    }

                // Copy
                case KeyCode.C:
                    {
                        if (ctrlOnly)
                        {
                            clipboard = GetSelectedString();
                            return EditState.Continue;
                        }
                        break;
                    }

                // Paste
                case KeyCode.V:
                    {
                        if (ctrlOnly)
                        {
							// 支持物件复制
							List<SChatMessageObjectInfo> itemList = ChatMessageManager.parseChatContent(clipboard, "255+255+255");
							foreach(var item in itemList)
							{
								pasteChatItem(item);
							}
							//Append(clipboard);
                            return EditState.Continue;
                        }
                        break;
                    }

                // Cut
                case KeyCode.X:
                    {
                        if (ctrlOnly)
                        {
                            clipboard = GetSelectedString();
                            Delete();
                            SendOnValueChangedAndUpdateLabel();
                            return EditState.Continue;
                        }
                        break;
                    }

                case KeyCode.LeftArrow:
                    {
                        if (OnPressHorizonArrow(true))
                            return EditState.Continue;

                        MoveLeft(shift, ctrl);
                        return EditState.Continue;
                    }

                case KeyCode.RightArrow:
                    {
                        if (OnPressHorizonArrow(false))
                            return EditState.Continue;

                        MoveRight(shift, ctrl);
                        return EditState.Continue;
                    }

                case KeyCode.UpArrow:
                    {
                        if (OnPressVerticalArrow(true))
                            return EditState.Continue;

                        MoveUp(shift);
                        return EditState.Continue;
                    }

                case KeyCode.DownArrow:
                    {
                        if (OnPressVerticalArrow(false))
                            return EditState.Continue;

                        MoveDown(shift);
                        return EditState.Continue;
                    }

                // Submit
                case KeyCode.Return:
                case KeyCode.KeypadEnter:
                    {
                        if (lineType != LineType.MultiLineNewline)
                        {
                            if (onEnterSubmit != null)
                                onEnterSubmit.Invoke("");
                            return EditState.Finish;
                        }
                        break;
                    }

                case KeyCode.Escape:
                    {
                        m_WasCanceled = true;
                        return EditState.Finish;
                    }
            }

            char c = evt.character;
            // Dont allow return chars or tabulator key to be entered into single line fields.
            if (!multiLine && (c == '\t' || c == '\r' || c == 10))
                return EditState.Continue;

            // Convert carriage return and end-of-text characters to newline.
            if (c == '\r' || (int)c == 3)
                c = '\n';

            if (IsValidChar(c))
            {
                Append(c);
            }

            if (c == 0)
            {
                if (Input.compositionString.Length > 0)
                {
                    if (m_curIngoreInputString != Input.compositionString)
                        m_curIngoreInputString = "";

                    UpdateLabel();
                }
            }
            return EditState.Continue;
        }

        private bool IsValidChar(char c)
        {
            // Delete key on mac
            if ((int)c == 127)
                return false;
            // Accept newline and tab
            if (c == '\t' || c == '\n')
                return true;

            return true;
        }

        /// <summary>
        /// Handle the specified event.
        /// </summary>
        private Event m_ProcessingEvent = new Event();

        public void ProcessEvent(Event e)
        {
            KeyPressed(e);
        }

        public virtual void OnUpdateSelected(BaseEventData eventData)
        {
            if (!isFocused)
                return;

            bool consumedEvent = false;
            while (Event.PopEvent(m_ProcessingEvent))
            {
                if (m_ProcessingEvent.rawType == EventType.KeyDown)
                {
                    consumedEvent = true;
                    var shouldContinue = KeyPressed(m_ProcessingEvent);
                    if (shouldContinue == EditState.Finish)
                    {
                        DeactivateInputField();
                        break;
                    }
                }

                switch (m_ProcessingEvent.type)
                {
                    case EventType.ValidateCommand:
                    case EventType.ExecuteCommand:
                        switch (m_ProcessingEvent.commandName)
                        {
                            case "SelectAll":
                                SelectAll();
                                consumedEvent = true;
                                break;
                        }
                        break;
                }
            }

            if (consumedEvent)
                UpdateLabel();

            eventData.Use();
        }

        private string GetSelectedString()
        {
            if (!hasSelection)
                return "";

            int startPos = caretPositionInternal;
            int endPos = caretSelectPositionInternal;

            // Ensure pos is always less then selPos to make the code simpler
            if (startPos > endPos)
            {
                int temp = startPos;
                startPos = endPos;
                endPos = temp;
            }

            return GetCharString(startPos, endPos - 1);
        }

        private void MoveRight(bool shift, bool ctrl)
        {
            if (hasSelection && !shift)
            {
                // By convention, if we have a selection and move right without holding shift,
                // we just place the cursor at the end.
                caretPositionInternal = caretSelectPositionInternal = Mathf.Max(caretPositionInternal, caretSelectPositionInternal);
                return;
            }

            int position = caretSelectPositionInternal + 1;

            if (shift)
                caretSelectPositionInternal = position;
            else
                caretSelectPositionInternal = caretPositionInternal = position;
        }

        private void MoveLeft(bool shift, bool ctrl)
        {
            if (hasSelection && !shift)
            {
                // By convention, if we have a selection and move left without holding shift,
                // we just place the cursor at the start.
                caretPositionInternal = caretSelectPositionInternal = Mathf.Min(caretPositionInternal, caretSelectPositionInternal);
                return;
            }

            int position = caretSelectPositionInternal - 1;

            if (shift)
                caretSelectPositionInternal = position;
            else
                caretSelectPositionInternal = caretPositionInternal = position;
        }

        private int DetermineCharacterLine(int charPos)
        {
            if (!multiLine)
                return 0;

            //for (int i = 0; i < generator.lineCount - 1; ++i)
            //{
            //    if (generator.lines[i + 1].startCharIdx > charPos)
            //        return i;
            //}

            //return generator.lineCount - 1;
            return 0;
        }

        // 暂时不支持上下
        /// <summary>
        ///  Use cachedInputTextGenerator as the y component for the UICharInfo is not required
        /// </summary>

        private int LineUpCharacterPosition(int originalPos, bool goToFirstChar)
        {
            return 0;

//             if (originalPos >= cachedInputTextGenerator.characterCountVisible)
//                 return 0;
// 
//             UICharInfo originChar = cachedInputTextGenerator.characters[originalPos];
//             int originLine = DetermineCharacterLine(originalPos, cachedInputTextGenerator);
// 
//             // We are on the last line return last character
//             if (originLine - 1 < 0)
//                 return goToFirstChar ? 0 : originalPos;
// 
// 
//             int endCharIdx = cachedInputTextGenerator.lines[originLine].startCharIdx - 1;
// 
//             for (int i = cachedInputTextGenerator.lines[originLine - 1].startCharIdx; i < endCharIdx; ++i)
//             {
//                 if (cachedInputTextGenerator.characters[i].cursorPos.x >= originChar.cursorPos.x)
//                     return i;
//             }
//             return endCharIdx;
        }

        /// <summary>
        ///  Use cachedInputTextGenerator as the y component for the UICharInfo is not required
        /// </summary>

        private int LineDownCharacterPosition(int originalPos, bool goToLastChar)
        {
            return GetTotalLen();

//             if (originalPos >= cachedInputTextGenerator.characterCountVisible)
//                 return text.Length;
// 
//             UICharInfo originChar = cachedInputTextGenerator.characters[originalPos];
//             int originLine = DetermineCharacterLine(originalPos, cachedInputTextGenerator);
// 
//             // We are on the last line return last character
//             if (originLine + 1 >= cachedInputTextGenerator.lineCount)
//                 return goToLastChar ? text.Length : originalPos;
// 
//             // Need to determine end line for next line.
//             int endCharIdx = GetLineEndPosition(cachedInputTextGenerator, originLine + 1);
// 
//             for (int i = cachedInputTextGenerator.lines[originLine + 1].startCharIdx; i < endCharIdx; ++i)
//             {
//                 if (cachedInputTextGenerator.characters[i].cursorPos.x >= originChar.cursorPos.x)
//                     return i;
//             }
//             return endCharIdx;
        }

        private void MoveDown(bool shift)
        {
            MoveDown(shift, true);
        }

        private void MoveDown(bool shift, bool goToLastChar)
        {
            if (hasSelection && !shift)
            {
                // If we have a selection and press down without shift,
                // set caret position to end of selection before we move it down.
                caretPositionInternal = caretSelectPositionInternal = Mathf.Max(caretPositionInternal, caretSelectPositionInternal);
            }

            int position = multiLine ? LineDownCharacterPosition(caretSelectPositionInternal, goToLastChar) : GetTotalLen();

            if (shift)
                caretSelectPositionInternal = position;
            else
                caretPositionInternal = caretSelectPositionInternal = position;
        }

        private void MoveUp(bool shift)
        {
            MoveUp(shift, true);
        }

        private void MoveUp(bool shift, bool goToFirstChar)
        {
            if (hasSelection && !shift)
            {
                // If we have a selection and press up without shift,
                // set caret position to start of selection before we move it up.
                caretPositionInternal = caretSelectPositionInternal = Mathf.Min(caretPositionInternal, caretSelectPositionInternal);
            }

            int position = multiLine ? LineUpCharacterPosition(caretSelectPositionInternal, goToFirstChar) : 0;

            if (shift)
                caretSelectPositionInternal = position;
            else
                caretSelectPositionInternal = caretPositionInternal = position;
        }

        private void DeleteContentChar(int beginIndex, int count)
        {
            int totalLen = GetTotalLen();
            if (beginIndex < 0 || beginIndex >= totalLen || count <= 0)
                return;

            int curIndex = 0;
            for (int i = 0; i < m_itemList.Count; )
            {
                IChatItem item = m_itemList[i];
                if (item == null)
                    return;

                int itemCharCount = item.GetConvertCharCount();
                if (curIndex + itemCharCount <= beginIndex)
                {
                    curIndex += itemCharCount;
                    ++i;
                }
                else
                {
                    int offsetIndex = beginIndex - curIndex;
                    int leftCount = itemCharCount - offsetIndex;
                    if (offsetIndex == 0 && leftCount <= count)
                    {
                        // 删除整个物件
                        item.Destroy();
                        m_itemList.RemoveAt(i);
                        // i保持不变
                    }
                    else
                    {
                        item.DeleteChar(offsetIndex, count);
                        ++i;
                    }
                    
                    curIndex += itemCharCount;
                    beginIndex = curIndex;
                    count -= leftCount;
                    if (count <= 0)
                        break;
                }
            }
        }

        private void Delete()
        {
            if (caretPositionInternal == caretSelectPositionInternal)
                return;

            if (caretPositionInternal < caretSelectPositionInternal)
            {
                DeleteContentChar(caretPositionInternal, caretSelectPositionInternal - caretPositionInternal);
                caretSelectPositionInternal = caretPositionInternal;
            }
            else
            {
                DeleteContentChar(caretSelectPositionInternal, caretPositionInternal - caretSelectPositionInternal);
                caretPositionInternal = caretSelectPositionInternal;
            }
            AlignContent();
        }

        private void ForwardSpace()
        {
            if (hasSelection)
            {
                Delete();
                SendOnValueChangedAndUpdateLabel();
            }
            else
            {
                if (caretPositionInternal < GetTotalLen())
                {
                    DeleteContentChar(caretPositionInternal, 1);
                    SendOnValueChangedAndUpdateLabel();
                    AlignContent();
                }
            }
        }

        private void Backspace()
        {
            if (hasSelection)
            {
                Delete();
                SendOnValueChangedAndUpdateLabel();
            }
            else
            {
                if (caretPositionInternal > 0)
                {
                    DeleteContentChar(caretPositionInternal - 1, 1);
                    caretSelectPositionInternal = caretPositionInternal = caretPositionInternal - 1;
                    SendOnValueChangedAndUpdateLabel();
                }
            }
        }

        // Insert the character and update the label.
        private void Insert(char c)
        {
            if (m_lastInputString.Length > 0)
            {
                // 部分无良输入法会在不修改Input.compositionString的情况下Insert, 这里特殊处理
                DeleteContentChar(m_lastInputStringIndex, m_lastInputString.Length);
                caretPositionInternal = caretSelectPositionInternal = m_lastInputStringIndex;
                m_lastInputString = "";
                m_lastInputInserted = true;
                m_curIngoreInputString = Input.compositionString;
            }

            string replaceString = c.ToString();
            Delete();

            // Can't go past the character limit
            if (characterLimit > 0 && (GetTotalLen() >= characterLimit))
                return;

            bool bDone = false;
            int curCharIndex = 0;
            int curChildIndex = 0;
            for ( ; curChildIndex < m_itemList.Count; ++curChildIndex)
            {
                IChatItem item = m_itemList[curChildIndex];
                if (item == null)
                    return;

                int itemCharCount = item.GetConvertCharCount();
                if (curCharIndex + itemCharCount < m_CaretPosition)
                {
                    curCharIndex += itemCharCount;
                }
                else
                {
                    int offsetIndex = m_CaretPosition - curCharIndex;
                    if (item.InsertChar(offsetIndex, replaceString))
                    {
                        bDone = true;
                        break;
                    }

                    if (offsetIndex > 0)
                    {
                        ++curChildIndex;
                        if (curChildIndex < m_itemList.Count)
                        {
                            item = m_itemList[curChildIndex];
                            if (item != null && item.InsertChar(0, replaceString))
                            {
                                bDone = true;
                            }
                        }
                    }

                    break;
                }
            }

            if (bDone == false)
            {
                ResNode itemPrefab = LogicDataCenter.chatMessageManager.GetChatItemPrefabByObjType(ChatItemType.ObjType_Text);
                if (itemPrefab == null)
                    return;

                GameObject itemObj = itemPrefab.InstanceMainRes();
                if (itemObj == null)
                    return;

                IChatItem subItem = itemObj.GetComponent<IChatItem>();
                if (subItem == null)
                    return;
                subItem.SetResNode(itemPrefab);
                itemObj.transform.SetParent(m_content, false);
                itemObj.transform.SetSiblingIndex(curChildIndex);
                m_itemList.Insert(curChildIndex, subItem);

                SChatMessageObjectInfo info;
                info.type = ChatItemType.ObjType_Text;
                info.subType = ChatItemSubType.ObjSubType_Text_Text;
                info.text = replaceString;
                info.param = null;
                if (subItem.SetItemData(info) == false)
                    return;
            }

            caretSelectPositionInternal = caretPositionInternal += replaceString.Length;
            AlignContent();

            SendOnValueChanged();
        }

        public void InsertItem(GameObject itemObj, IChatItem newItem, int nIndex)
        {
            Delete();
            if (nIndex < 0)
            {
                nIndex = m_CaretPosition;
            }
            
            // Can't go past the character limit
            if (characterLimit > 0 && (GetTotalLen() >= characterLimit || m_itemList.Count > 10))
                return;

            int curCharIndex = 0;
            int curChildIndex = 0;
            for ( ; curChildIndex < m_itemList.Count; ++curChildIndex)
            {
                IChatItem item = m_itemList[curChildIndex];
                if (item == null)
                    return;

                int itemCharCount = item.GetConvertCharCount();
                if (curCharIndex + itemCharCount < nIndex)
                {
                    curCharIndex += itemCharCount;
                }
                else
                {
                    int offsetIndex = nIndex - curCharIndex;
                    if (offsetIndex <= 0)
                        break;

                    if (offsetIndex >= itemCharCount)
                    {
                        ++curChildIndex;
                        break;
                    }

                    ++curChildIndex;
                    IChatItem item1, item2;
                    item.SplitByIndex(offsetIndex, out item1, out item2);
                    item2.GetTransform().SetParent(m_content, false);
                    item2.GetTransform().SetSiblingIndex(curChildIndex);
                    m_itemList.Insert(curChildIndex, item2);
                    break;
                }
            }

            newItem.GetTransform().SetParent(m_content, false);
            newItem.GetTransform().SetSiblingIndex(curChildIndex);
            m_itemList.Insert(curChildIndex, newItem);

            caretSelectPositionInternal = caretPositionInternal += newItem.GetConvertCharCount();
            AlignContent();

            SendOnValueChanged();
        }

        // 校正item位置
        private void AlignContent()
        {
            int count = m_itemList.Count;
            float curX = 0f;
            float fDrawStartOffset = GetContentItemStartX(m_DrawStart);
            for (int i = 0; i < count; ++i)
            {
                IChatItem item = m_itemList[i];
                if (item == null)
                    continue;

                float curY = -19.0f;    // 纵坐标先写死
                if (item.GetItemType() == ChatItemType.ObjType_Image)
                    curY = -25f;
                item.GetTransform().anchoredPosition = new Vector3(curX - fDrawStartOffset, curY, 0f);
                curX += (item.GetItemWidth());
            }
        }

        private void SendOnValueChangedAndUpdateLabel()
        {
            SendOnValueChanged();
            UpdateLabel();
        }

        private void SendOnValueChanged()
        {
            if (onValueChange != null)
                onValueChange.Invoke("");
        }

		protected void SendOnFocusChanged(bool focus)
		{
			if (onFocusChange != null)
				onFocusChange.Invoke(focus);
		}
        /// <summary>
        /// Submit the input field's text.
        /// </summary>

        protected void SendOnSubmit()
        {
            if (onEndEdit != null)
                onEndEdit.Invoke("");
        }

        /// <summary>
        /// Append the specified text to the end of the current.
        /// </summary>

        protected virtual void Append(string input)
        {
            for (int i = 0, imax = input.Length; i < imax; ++i)
            {
                char c = input[i];

                if (c >= ' ')
                {
                    Append(c);
                }
            }
        }

        protected virtual void Append(char input)
        {
            // If the input is invalid, skip it
            if (input == 0)
                return;

            // Append the character and update the label
            Insert(input);
        }

        /// <summary>
        /// Update the visual text Text.
        /// </summary>

        protected void UpdateLabel()
        {
            // 未激活时不更新内容
            if (!m_AllowInput)
                return;
            //if (m_TextComponent != null && m_TextComponent.font != null && !m_PreventFontCallback)
            {
                // TextGenerator.Populate invokes a callback that's called for anything
                // that needs to be updated when the data for that font has changed.
                // This makes all Text components that use that font update their vertices.
                // In turn, this makes the InputField that's associated with that Text component
                // update its label by calling this UpdateLabel method.
                // This is a recursive call we want to prevent, since it makes the InputField
                // update based on font data that didn't yet finish executing, or alternatively
                // hang on infinite recursion, depending on whether the cached value is cached
                // before or after the calculation.
                //
                // This callback also occurs when assigning text to our Text component, i.e.,
                // m_TextComponent.text = processed;

                m_PreventFontCallback = true;

                //string fullText;
                //if (Input.compositionString.Length > 0)
                //    fullText = text.Substring(0, m_CaretPosition) + Input.compositionString + text.Substring(m_CaretPosition);
                //else
                //    fullText = text;
                //string processed = fullText;
                // 特殊处理下试试。某些输入法下compositionString不正规
                bool bUnformalIME = false;
                if (Input.compositionString == " ")
                    bUnformalIME = true;
                if (m_curIngoreInputString.Length > 0 && Input.compositionString == m_curIngoreInputString)
                    bUnformalIME = true;
                if (m_lastInputString != Input.compositionString && m_lastInputInserted == false && !bUnformalIME)
                {
                    if (m_lastInputString.Length > 0)
                    {
                        // 删除旧的
                        DeleteContentChar(m_lastInputStringIndex, m_lastInputString.Length);
                    }
                    else
                    {
                        m_lastInputStringIndex = m_CaretPosition;
                    }

                    // 添加输入中物件
                    if (Input.compositionString.Length > 0)
                    {
                        ResNode itemPrefab = LogicDataCenter.chatMessageManager.GetChatItemPrefabByObjType(ChatItemType.ObjType_Text);
                        if (itemPrefab != null)
                        {
                            GameObject itemObj = itemPrefab.InstanceMainRes();
                            if (itemObj != null)
                            {
                                IChatItem subItem = itemObj.GetComponent<IChatItem>();
                                if (subItem != null)
                                {
                                    SChatMessageObjectInfo info;
                                    info.type = ChatItemType.ObjType_Text;
                                    info.subType = ChatItemSubType.ObjSubType_Text_Text;
                                    info.text = Input.compositionString;
                                    info.param = new Dictionary<String, String>();
                                    info.param.Add("color", LogicDataCenter.chatMessageManager.CreateColorParam(140, 235, 30));
                                    subItem.SetItemData(info);

                                    InsertItem(itemObj, subItem, m_lastInputStringIndex);
                                }
                            }
                        }
                    }

                    if (Input.compositionString.Length  > 0)
                    {
                        m_lastInputString = Input.compositionString;
                        caretPositionInternal = caretSelectPositionInternal = m_lastInputStringIndex + m_lastInputString.Length;
                    }
                    else
                    {
                        caretPositionInternal = caretSelectPositionInternal = m_lastInputStringIndex;
                        m_lastInputString = Input.compositionString;
                    }
                }

                m_lastInputInserted = false;

                //bool isEmpty = string.IsNullOrEmpty(fullText);
                bool isEmpty = (m_itemList.Count == 0);

                if (m_Placeholder != null)
                    m_Placeholder.enabled = isEmpty;

                // If not currently editing the text, set the visible range to the whole text.
                // The UpdateLabel method will then truncate it to the part that fits inside the Text area.
                // We can't do this when text is being edited since it would discard the current scroll,
                // which is defined by means of the m_DrawStart and m_DrawEnd indices.
                if (!m_AllowInput)
                {
                    m_DrawStart = 0;
                    m_DrawEnd = GetTotalLen();
                }

                if (!isEmpty)
                {
                    // Determine what will actually fit into the given line
                    //Vector2 extents = m_TextComponent.rectTransform.rect.size;

                    //var settings = m_TextComponent.GetGenerationSettings(extents);
                    //settings.generateOutOfBounds = true;

                    //cachedInputTextGenerator.Populate(processed, settings);

                    SetDrawRangeToContainCaretPosition(caretSelectPositionInternal);

                    //processed = processed.Substring(m_DrawStart, Mathf.Min(m_DrawEnd, processed.Length) - m_DrawStart);

                    SetCaretVisible();
                }
                //m_TextComponent.text = processed;
                MarkGeometryAsDirty();
                m_PreventFontCallback = false;

                AlignContent();
            }
        }

        private bool IsSelectionVisible()
        {
            if (m_DrawStart > caretPositionInternal || m_DrawStart > caretSelectPositionInternal)
                return false;

            if (m_DrawEnd < caretPositionInternal || m_DrawEnd < caretSelectPositionInternal)
                return false;

            return true;
        }

        private int GetLineStartPosition(int line)
        {
            return 0;
            // 先只做单行
            //line = Mathf.Clamp(line, 0, gen.lines.Count - 1);
            //return gen.lines[line].startCharIdx;
        }

        private int GetLineEndPosition(int line)
        {
            return GetTotalLen();
            // line = Mathf.Max(line, 0);
            // if (line + 1 < gen.lines.Count)
            //    return gen.lines[line + 1].startCharIdx;
            //return gen.characterCountVisible;
        }

        private void SetDrawRangeToContainCaretPosition(int caretPos)
        {
            // the extents gets modified by the pixel density, so we need to use the generated extents since that will be in the same 'space' as
            // the values returned by the TextGenerator.lines[x].height for instance.
            Vector2 extents = m_content.rect.size;
            /*
            if (multiLine)
            {
                var lines = cachedInputTextGenerator.lines;
                int caretLine = DetermineCharacterLine(caretPos, cachedInputTextGenerator);
                int height = (int)extents.y;

                // Have to compare with less or equal rather than just less.
                // The reason is that if the caret is between last char of one line and first char of next,
                // we want to interpret it as being on the next line.
                // This is also consistent with what DetermineCharacterLine returns.
                if (m_DrawEnd <= caretPos)
                {
                    // Caret comes after drawEnd, so we need to move drawEnd to a later line end that comes after caret.
                    m_DrawEnd = GetLineEndPosition(cachedInputTextGenerator, caretLine);
                    for (int i = caretLine; i >= 0 && i < lines.Count; --i)
                    {
                        height -= lines[i].height;
                        if (height < 0)
                            break;

                        m_DrawStart = GetLineStartPosition(cachedInputTextGenerator, i);
                    }
                }
                else
                {
                    if (m_DrawStart > caretPos)
                    {
                        // Caret comes before drawStart, so we need to move drawStart to an earlier line start that comes before caret.
                        m_DrawStart = GetLineStartPosition(cachedInputTextGenerator, caretLine);
                    }

                    int startLine = DetermineCharacterLine(m_DrawStart, cachedInputTextGenerator);
                    int endLine = startLine;
                    m_DrawEnd = GetLineEndPosition(cachedInputTextGenerator, endLine);
                    height -= lines[endLine].height;
                    while (true)
                    {
                        if (endLine < lines.Count - 1)
                        {
                            endLine++;
                            if (height < lines[endLine].height)
                                break;
                            m_DrawEnd = GetLineEndPosition(cachedInputTextGenerator, endLine);
                            height -= lines[endLine].height;
                        }
                        else if (startLine > 0)
                        {
                            startLine--;
                            if (height < lines[startLine].height)
                                break;
                            m_DrawStart = GetLineStartPosition(cachedInputTextGenerator, startLine);

                            height -= lines[startLine].height;
                        }
                        else
                            break;
                    }
                }
            }
            else
            */
            {
                int nTotalLen = GetTotalLen();
                if (m_DrawEnd > nTotalLen)
                    m_DrawEnd = nTotalLen;

                float width = 0.0f;
                if (caretPos > m_DrawEnd || (caretPos == m_DrawEnd && m_DrawStart > 0))
                {
                    // fit characters from the caretPos leftward
                    m_DrawEnd = caretPos;
                    for (m_DrawStart = m_DrawEnd - 1; m_DrawStart >= 0; --m_DrawStart)
                    {
                        float charWidth = GetContentItemWidth(m_DrawStart);
                        if (width + charWidth > extents.x)
                            break;

                        width += charWidth;
                    }
                    ++m_DrawStart;  // move right one to the last character we could fit on the left
                }
                else
                {
                    if (caretPos < m_DrawStart)
                        m_DrawStart = caretPos;

                    m_DrawEnd = m_DrawStart;
                }

                // fit characters rightward
                for (; m_DrawEnd < nTotalLen; ++m_DrawEnd)
                {
                    float charWidth = GetContentItemWidth(m_DrawEnd);
                    width += charWidth;
                    if (width > extents.x)
                        break;
                }
            }
        }

        private void MarkGeometryAsDirty()
        {
#if UNITY_EDITOR
            if (!Application.isPlaying)// || UnityEditor.PrefabUtility.GetPrefabObject(gameObject) != null)
                return;
#endif

            CanvasUpdateRegistry.RegisterCanvasElementForGraphicRebuild(this);
        }

        public virtual void Rebuild(CanvasUpdate update)
        {
            switch (update)
            {
                case CanvasUpdate.LatePreRender:
                    UpdateGeometry();
                    break;
            }
        }

        public virtual void LayoutComplete()
        { }

        public virtual void GraphicUpdateComplete()
        { }

        private void UpdateGeometry()
        {
#if UNITY_EDITOR
            if (!Application.isPlaying)
                return;
#endif
            if (m_CachedInputRenderer == null)
            {
                GameObject go = new GameObject(transform.name + " Input Caret");
                go.hideFlags = HideFlags.DontSave;
                //go.transform.SetParent(m_TextComponent.transform.parent);
                go.transform.SetParent(transform, false);
                go.transform.SetAsLastSibling();
                go.layer = gameObject.layer;

                caretRectTrans = go.AddComponent<RectTransform>();
                caretRectTrans.anchoredPosition = Vector3.zero;
                m_CachedInputRenderer = go.AddComponent<CanvasRenderer>();
                m_CachedInputRenderer.SetMaterial(Graphic.defaultGraphicMaterial, Texture2D.whiteTexture);

                // Needed as if any layout is present we want the caret to always be the same as the text area.
                go.AddComponent<LayoutElement>().ignoreLayout = true;

                //AssignPositioningIfNeeded();
            }

            if (m_CachedInputRenderer == null)
                return;

            OnFillVBO(mesh);
            m_CachedInputRenderer.SetMesh(mesh);
        }

        private void AssignPositioningIfNeeded()
        {
            RectTransform rectTransform = transform as RectTransform;
            if (caretRectTrans != null &&
                (caretRectTrans.localPosition != rectTransform.localPosition ||
                 caretRectTrans.localRotation != rectTransform.localRotation ||
                 caretRectTrans.localScale != rectTransform.localScale ||
                 caretRectTrans.anchorMin != rectTransform.anchorMin ||
                 caretRectTrans.anchorMax != rectTransform.anchorMax ||
                 caretRectTrans.anchoredPosition != rectTransform.anchoredPosition ||
                 caretRectTrans.sizeDelta != rectTransform.sizeDelta ||
                 caretRectTrans.pivot != rectTransform.pivot))
            {
                caretRectTrans.localPosition = rectTransform.localPosition;
                caretRectTrans.localRotation = rectTransform.localRotation;
                caretRectTrans.localScale = rectTransform.localScale;
                caretRectTrans.anchorMin = rectTransform.anchorMin;
                caretRectTrans.anchorMax = rectTransform.anchorMax;
                caretRectTrans.anchoredPosition = rectTransform.anchoredPosition;
                caretRectTrans.sizeDelta = rectTransform.sizeDelta;
                caretRectTrans.pivot = rectTransform.pivot;
            }
        }

        private void OnFillVBO(Mesh vbo)
        {
            using (var helper = new VertexHelper())
            {
                if (!isFocused)
                {
                    helper.FillMesh(vbo);
                    return;
                }

                /*
                Rect inputRect = m_content.rect;
                Vector2 extents = inputRect.size;

                // get the text alignment anchor point for the text in local space
                Vector2 textAnchorPivot = new Vector2(0f, 0.5f);
                Vector2 refPoint = Vector2.zero;
                refPoint.x = Mathf.Lerp(inputRect.xMin, inputRect.xMax, textAnchorPivot.x);
                refPoint.y = Mathf.Lerp(inputRect.yMin, inputRect.yMax, textAnchorPivot.y);

                // Ajust the anchor point in screen space
                Vector2 roundedRefPoint = m_baseImage.PixelAdjustPoint(refPoint);

                // Determine fraction of pixel to offset text mesh.
                // This is the rounding in screen space, plus the fraction of a pixel the text anchor pivot is from the corner of the text mesh.
                Vector2 roundingOffset = roundedRefPoint - refPoint + Vector2.Scale(extents, textAnchorPivot);
                roundingOffset.x = roundingOffset.x - Mathf.Floor(0.5f + roundingOffset.x);
                roundingOffset.y = roundingOffset.y - Mathf.Floor(0.5f + roundingOffset.y);
                */

                Rect inputRect = m_content.rect;
                Vector2 roundingOffset = new Vector2(-inputRect.width * 0.5f, 0f);
                if (!hasSelection)
                    GenerateCursor(helper, roundingOffset);
                else
                    GenerateHightlight(helper, roundingOffset);

                helper.FillMesh(vbo);
            }
        }

        private void GenerateCursor(VertexHelper vbo, Vector2 roundingOffset)
        {
            if (!m_CaretVisible)
                return;

            if (m_CursorVerts == null)
            {
                CreateCursorVerts();
            }

            float width = 2f;
            float height = 20f;
            int adjustedPos = Mathf.Max(0, caretPositionInternal - m_DrawStart);

            Vector2 startPosition = Vector2.zero;

            // Calculate startPosition
            //if (GetTotalLen() + 1 > adjustedPos || adjustedPos == 0)
            {
                // UICharInfo cursorChar = gen.characters[adjustedPos];
                startPosition.x = GetContentItemStartX(caretPositionInternal);
                startPosition.x -= GetContentItemStartX(m_DrawStart);
            }

            // TODO: Only clamp when Text uses horizontal word wrap.
            if (startPosition.x > m_content.rect.width)
                startPosition.x = m_content.rect.width;
            if (startPosition.x < 0f)
                startPosition.x = 0f;

            int characterLine = DetermineCharacterLine(adjustedPos);
            float lineHeights = SumLineHeights(characterLine);
            startPosition.y = m_content.rect.yMin + lineHeights;

            m_CursorVerts[0].position = new Vector3(startPosition.x, startPosition.y + height, 0.0f);
            m_CursorVerts[1].position = new Vector3(startPosition.x + width, startPosition.y + height, 0.0f);
            m_CursorVerts[2].position = new Vector3(startPosition.x + width, startPosition.y, 0.0f);
            m_CursorVerts[3].position =  new Vector3(startPosition.x, startPosition.y, 0.0f);

            if (roundingOffset != Vector2.zero)
            {
                for (int i = 0; i < m_CursorVerts.Length; i++)
                {
                    UIVertex uiv = m_CursorVerts[i];
                    uiv.position.x += roundingOffset.x;
                    uiv.position.y += roundingOffset.y;
                    m_CursorVerts[i] = uiv;
                }
            }

            vbo.AddUIVertexQuad(m_CursorVerts);

            startPosition.y = Screen.height - startPosition.y;
            Input.compositionCursorPos = startPosition;
        }

        private void CreateCursorVerts()
        {
            m_CursorVerts = new UIVertex[4];

            for (int i = 0; i < m_CursorVerts.Length; i++)
            {
                m_CursorVerts[i] = UIVertex.simpleVert;
                m_CursorVerts[i].color = Color.white;
                m_CursorVerts[i].uv0 = Vector2.zero;
            }
        }

        private float SumLineHeights(int endLine)
        {
            float height = 0.0f;
            for (int i = 0; i < m_LineHeight.Count && i < endLine; ++i)
            {
                height += m_LineHeight[i];
            }

            return height;
        }

        private void GenerateHightlight(VertexHelper vbo, Vector2 roundingOffset)
        {
            int startChar = Mathf.Max(0, caretPositionInternal);
            int endChar = Mathf.Max(0, caretSelectPositionInternal);
            float fOffsetX = GetContentItemStartX(m_DrawStart);

            // Ensure pos is always less then selPos to make the code simpler
            if (startChar > endChar)
            {
                int temp = startChar;
                startChar = endChar;
                endChar = temp;
            }

            endChar -= 1;
            int currentLineIndex = DetermineCharacterLine(startChar);
            int nextLineStartIdx = GetLineEndPosition(currentLineIndex);

            UIVertex vert = UIVertex.simpleVert;
            vert.uv0 = Vector2.zero;
            vert.color = selectionColor;

            for (int i = startChar; i <= endChar; ++i)
            {
                    float fStartX = GetContentItemStartX(i) - fOffsetX;
                    float fEndX = fStartX + GetContentItemWidth(i);
                    float lineHeights = 20f;//SumLineHeights(currentLineIndex);
                    float fStartY = m_content.rect.yMin;//m_TextComponent.rectTransform.rect.yMax - (lineHeights / m_TextComponent.pixelsPerUnit);
                    float fEndY = fStartY + lineHeights;//startPosition.y - height / m_TextComponent.pixelsPerUnit)

                    // Checking xMin as well due to text generator not setting possition if char is not rendered.
                    if (fEndX < 0f || fStartX > m_content.rect.width)
                        continue;
                    if (fStartX < 0f)
                        fStartX = 0f;
                    if (fEndX > m_content.rect.width)
                        fEndX = m_content.rect.width;

                    var startIndex = vbo.currentVertCount;
                    vert.position = new Vector3(fStartX, fEndY, 0.0f) + (Vector3)roundingOffset;
                    vbo.AddVert(vert);

                    vert.position = new Vector3(fEndX, fEndY, 0.0f) + (Vector3)roundingOffset;
                    vbo.AddVert(vert);

                    vert.position = new Vector3(fEndX, fStartY, 0.0f) + (Vector3)roundingOffset;
                    vbo.AddVert(vert);

                    vert.position = new Vector3(fStartX, fStartY, 0.0f) + (Vector3)roundingOffset;
                    vbo.AddVert(vert);

                    vbo.AddTriangle(startIndex, startIndex + 1, startIndex + 2);
                    vbo.AddTriangle(startIndex + 2, startIndex + 3, startIndex + 0);
            }
        }

        public void ActivateInputField()
        {
            if (!IsActive() || !IsInteractable())
                return;

            m_ShouldActivateNextUpdate = true;
        }

        private void ActivateInputFieldInternal()
        {
            if (EventSystem.current.currentSelectedGameObject != gameObject)
                EventSystem.current.SetSelectedGameObject(gameObject);

            Input.imeCompositionMode = IMECompositionMode.On;
            OnFocus();

            m_AllowInput = true;

            //m_OriginalText = text;
            m_WasCanceled = false;
            m_curIngoreInputString = Input.compositionString;       // 激活时输入法缓存中的内容先忽略
            SetCaretVisible();
            UpdateLabel();
        }

        public override void OnSelect(BaseEventData eventData)
        {
            base.OnSelect(eventData);
            ActivateInputField();
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button != PointerEventData.InputButton.Left)
                return;

            ActivateInputField();
        }

        public void DeactivateInputField()
        {
            // Not activated do nothing.
            if (!m_AllowInput)
                return;

            m_HasDoneFocusTransition = false;
            m_AllowInput = false;

            if (IsInteractable())
            {
                //if (m_WasCanceled)
                //    text = m_OriginalText;

                //m_CaretPosition = m_CaretSelectPosition = 0;

                SendOnSubmit();

                Input.imeCompositionMode = IMECompositionMode.Auto;
            }

            MarkGeometryAsDirty();
        }

        public override void OnDeselect(BaseEventData eventData)
        {
            DeactivateInputField();
            base.OnDeselect(eventData);
        }

        public virtual void OnSubmit(BaseEventData eventData)
        {
            if (!IsActive() || !IsInteractable())
                return;

            if (!isFocused)
                m_ShouldActivateNextUpdate = true;
        }

        protected override void DoStateTransition(SelectionState state, bool instant)
        {
            if (m_HasDoneFocusTransition)
                state = SelectionState.Highlighted;
            else if (state == SelectionState.Pressed)
                m_HasDoneFocusTransition = true;

            base.DoStateTransition(state, instant);
        }

        protected virtual bool OnPressHorizonArrow(bool bLeft)
        {
            return false;
        }

        protected virtual bool OnPressVerticalArrow(bool bUp)
        {
            return false;
        }

		/// <summary>
		///  复制聊天物件
		/// </summary>
		/// <param name="objInfo"></param>
		private void pasteChatItem(SChatMessageObjectInfo objInfo)
		{
			ResNode itemPrefab = LogicDataCenter.chatMessageManager.GetChatItemPrefabByObjType(objInfo.type);
			if (itemPrefab == null)
				return;

			GameObject itemObj = itemPrefab.InstanceMainRes();
			if (itemObj == null)
				return;

			IChatItem subItem = itemObj.GetComponent<IChatItem>();
			subItem.SetResNode(itemPrefab);

			if (subItem == null)
			{
				itemPrefab.DestroyInstanceRes(ref itemObj);
				return;
			}

			if (subItem.SetItemData(objInfo) == false)
			{
				itemPrefab.DestroyInstanceRes(ref itemObj);
				return;
			}

			//ActivateInputField();
			selectionFocusPosition = caretPosition;
			InsertItem(itemObj, subItem, -1);
		}
    }
}

namespace USpeedUI.ChatBox
{
    public class ChatBoxInputField : InputFieldEx
    {
        public ChatBoxWndView wndView;
        public RectTransform bakContentLog;

        private int m_nCurSelLogIndex = -1;

        protected override bool OnPressHorizonArrow(bool bLeft)
        {
            if (Empty)
            {
                wndView.StepChangeChannel(bLeft);
                return true;
            }

            return false;
        }

        protected override bool OnPressVerticalArrow(bool bUp)
        {
            if (bUp)
            {
                if (m_nCurSelLogIndex < 0)
                    m_nCurSelLogIndex = bakContentLog.childCount - 1;
                else if (m_nCurSelLogIndex > 0)
                    --m_nCurSelLogIndex;
            }
            else
            {
                if (m_nCurSelLogIndex < 0)
                    return true;
                else if (m_nCurSelLogIndex < bakContentLog.childCount - 1)
                    ++m_nCurSelLogIndex;
            }
            FillContentWithLog();
            return true;
        }

        protected override void OnFocus()
        {
			SendOnFocusChanged(true);
            m_nCurSelLogIndex = -1;
		}

		private void FillContentWithLog()
        {
            if (m_nCurSelLogIndex < 0 || m_nCurSelLogIndex >= bakContentLog.childCount)
                return;

            base.ClearContent();

            Transform parentTrans = bakContentLog.GetChild(m_nCurSelLogIndex);
            for (int i = 0; i < parentTrans.childCount; ++i)
            {
                IChatItem subItem = parentTrans.GetChild(i).GetComponent<IChatItem>();
                if (subItem == null)
                    continue;

                IChatItem newItem = subItem.Clone();
                newItem.GetTransform().SetParent(m_content, false);
                m_itemList.Add(newItem);
            }

            MoveTextEnd(false);
        }

        public override void ClearContent()
        {
            caretPosition = 0;

            if (Empty)
                return;

            if (bakContentLog.childCount >= 10)
            {
                var temp1 = bakContentLog.GetChild(0).gameObject;
                ResNode.DestroyRes(ref temp1);
            }

            GameObject newObj = new GameObject("LogObj");
            newObj.SetActive(false);
            newObj.layer = LayerMask.NameToLayer(Config.LayerUI);
            Transform trans = newObj.transform;
            trans.SetParent(bakContentLog, false);

            for (int i = 0; i < m_itemList.Count; ++i)
            {
                m_itemList[i].GetTransform().SetParent(trans, false);
            }

            m_itemList.Clear();

            m_nCurSelLogIndex = -1;

            // 不实际清除，改为转移到log对象里
            // base.ClearContent();
        }
    }
}