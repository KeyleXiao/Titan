using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;

namespace UIWidgets
{
    public class UTooltipTrigger : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, ISelectHandler, IDeselectHandler, IPointerDownHandler, IPointerUpHandler
    {
        [HideInInspector]
        public TooltipStyle tooltipStyle;
        [HideInInspector]
        public List<ParameterizedTextField> parameterizedTextFields;
        [HideInInspector]
        public List<DynamicImageField> dynamicImageFields;
        [HideInInspector]
        public List<DynamicSectionField> dynamicSectionFields;

        /// <summary>
        /// 该Tips是否由其他的GameObject来控制显示隐藏，而不是鼠标移到需要显示tips的对象上才显示
        /// </summary>
        [HideInInspector]
        public bool isRemotelyActivated;
        /// <summary>
        /// 当前Tips受这个Gameobject控制是否显示
        /// </summary>
        [HideInInspector]
        public GameObject remoteTrigger;

        public RectTransform RectTransform { get; private set; }

        /// <summary>
        /// TooltipStyle的实例
        /// </summary>
        public Tooltip Tooltip { get; set; }
        [Tooltip("控制tooltip背景的颜色和渐变量")]
        public Color backgroundTint = Color.white;
        public TipPosition tipPosition;
        public int minTextWidth = 100;
        public int maxTextWidth = 200;

        [HideInInspector, Tooltip("打开的时候tooltip不会因为移开Trigger而关闭，除非有其他地方要他关闭")]
        public bool staysOpen = false;
        [HideInInspector, Tooltip("打开的话将阻止所有trigger显示tooltips")]
        public bool isBlocking = false;
        //[HideInInspector, Tooltip("Tips显示是否随Canvas旋转而旋转")]
        //[Obsolete("该功能目前没完善，使用无效")]
        //private bool neverRotate = false;

        /// <summary>
        /// 需要显示Tooltip时开始计时，时间超过tooltipDelay的就显示Tips
        /// </summary>
        private float hoverTimer;
        /// <summary>
        /// 需要隐藏Tooltip是开始计时，时间超过popupTime的就隐藏Tips
        /// </summary>
        private float popupTimer;

        /// <summary>
        /// 延迟显示Tooltips时间
        /// </summary>
        private float tooltipDelay = 0.2f;
        /// <summary>
        /// 隐藏Tooltips时间
        /// </summary>
        private float popupTime = 2f;
        /// <summary>
        /// 是否已经初始化了
        /// </summary>
        private bool isInitialized;
        private bool isMouseOver;
        private bool isMouseDown;
        /// <summary>
        /// 当前Tip是否在显示
        /// </summary>
        private bool isOpened = false;

        public virtual void Start()
        {
            if (UTooltipManager.Instance.IsInitialized)
                Initialize();
            else
                UTooltipManager.Instance.TooltipMgrInitEvent.AddListener(Initialize);

        }
        /// <summary>
        /// Trigger隐藏或消失时把Tips关掉
        /// </summary>
        public virtual void OnDisable()
        {
            ForceHideTooltip();
        }

        private void Initialize()
        {
            if (isInitialized || UTooltipManager.Instance == null)
                return;

            RectTransform = gameObject.GetComponent<RectTransform>();

            if (tooltipStyle != null)
            {
                //检测当前ToolTipsStyle是否已经创建，是则直接用，否则创建一个
                if (!UTooltipManager.Instance.Tooltips.ContainsKey(tooltipStyle.TooltipID))
                {
                    TooltipStyle ttStyle = Instantiate(tooltipStyle);
                    ttStyle.name = tooltipStyle.name;
                    ttStyle.transform.SetParent(UTooltipManager.Instance.TooltipContainer.transform, false);
                    Tooltip newTooltip = new Tooltip() { GameObject = ttStyle.gameObject };
                    newTooltip.Initialize();
                    newTooltip.Deactivate();
                    UTooltipManager.Instance.Tooltips.Add(tooltipStyle.TooltipID, newTooltip);
                }
                Tooltip = UTooltipManager.Instance.Tooltips[tooltipStyle.TooltipID];
            }
            isInitialized = true;

            UTooltipManager.Instance.TooltipMgrInitEvent.RemoveListener(Initialize);
        }

        public void Update()
        {
            if (UTooltipManager.Instance == null)
            {
                return;
            }
            tooltipDelay = UTooltipManager.Instance.TooltipDelay;

            // Hover timer update
            if (hoverTimer > 0)
                hoverTimer += Time.unscaledDeltaTime;

            if (hoverTimer > tooltipDelay)
            {
                // Turn off the timer and show the tooltip.
                hoverTimer = 0;
                StartHover();
            }

            // Popup timer update
            if (popupTimer > 0)
                popupTimer += Time.unscaledDeltaTime;

            if (popupTimer > popupTime && Tooltip != null && !Tooltip.StaysOpen) // Turn off the timer and hide the tooltip.
                HidePopup();
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;
            // Ignore and exit if this tooltip is remotely activated, if there is already a blocking tooltip on the screen, or if the tooltip is already visible.
            if (isRemotelyActivated || UTooltipManager.Instance.BlockingTooltip != null)
                return;
            if (Tooltip.GameObject.activeInHierarchy)
                return;

            hoverTimer = 0.001f; // Start the timer.
        }

        public void OnMouseOver()
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            // Ignore if we've already captured the MouseOver event.
            if (isMouseOver) return;
            // Ignore and exit if this tooltip is remotely activated, if there is already a blocking tooltip on the screen, or if the tooltip is already visible.
            if (isRemotelyActivated || UTooltipManager.Instance.BlockingTooltip != null) return;
            if (Tooltip.GameObject.activeInHierarchy) return;

            hoverTimer = 0.001f; // Start the timer.
            isMouseOver = true;
        }

        public void OnMouseDown()
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            // Ignore if we've already captured the MouseDown event.
            if (isMouseDown) return;
            // Ignore and exit if this tooltip is remotely activated, if there is already a blocking tooltip on the screen, or if the tooltip is already visible.
            if (isRemotelyActivated || UTooltipManager.Instance.BlockingTooltip != null) return;
            if (Tooltip.GameObject.activeInHierarchy) return;

            hoverTimer = 0.001f; // Start the timer.
            isMouseDown = true;
        }

        public void OnMouseExit()
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            isMouseOver = false;
            StopHover();
        }


        public void OnPointerDown(PointerEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            // Ignore and exit if this tooltip is remotely activated, if there is already a blocking tooltip on the screen, or if the tooltip is already visible.
            if (isRemotelyActivated || UTooltipManager.Instance.BlockingTooltip != null)
                return;
            if (Tooltip.GameObject.activeInHierarchy)
                return;

            hoverTimer = 0.001f; // Start the timer.
        }

        public void OnSelect(BaseEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            // Ignore and exit if this tooltip is remotely activated, if there is already a blocking tooltip on the screen, or if the tooltip is already visible.
            if (isRemotelyActivated || UTooltipManager.Instance.BlockingTooltip != null)
                return;
            if (Tooltip.GameObject.activeInHierarchy)
                return;

            hoverTimer = 0.001f; // Start the timer.
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;
            StopHover();
        }
        public bool IsUpStay = false;
        public void OnPointerUp(PointerEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;
            if (!IsUpStay)
                StopHover();
            else
            {
                StopHover();
                StartHover();
            }
        }

        public void OnDeselect(BaseEventData eventData)
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;
            StopHover();
        }

        public void StartHover()
        {
            if (UTooltipManager.Instance == null || !this.enabled || !isInitialized)
                return;

            // Fix if minWidth is greater than maxWidth.
            if (minTextWidth > maxTextWidth)
                maxTextWidth = minTextWidth;

            Tooltip.WarmUp();
            Tooltip.StaysOpen = staysOpen;
            //Tooltip.NeverRotate = neverRotate;
            Tooltip.IsBlocking = isBlocking;
            isOpened = true;//-
            UTooltipManager.Instance.SetTextAndSize(this);

            // Show and position the tooltip.
            StartCoroutine(UTooltipManager.Instance.Show(this));
        }

        /// <summary>Forces the tooltip to be hidden and deactivated, and also resets all timers so the tooltip won't automatically re-enable (unless it's triggered again).</summary>
        public void ForceHideTooltip()
        {
            // Reset all timers to prevent re-showing the tooltip automatically.
            hoverTimer = popupTimer = 0;

            // Reset the text fields and images for the tooltip, and hide it, regardless of how it was triggered (hover over or remote popup, or through code).
            //if (Tooltip != null && Tooltip.GameObject != null)
            if (Tooltip != null && Tooltip.GameObject != null && isOpened)
            {
                isOpened = false;
                Tooltip.Deactivate();
            }
        }

        public void StopHover()
        {
            if (Tooltip == null || Tooltip.GameObject == null)
                return;
            if (isRemotelyActivated || Tooltip.StaysOpen)
                return;

            isOpened = false;//-
            hoverTimer = 0; // Stop the timer and prevent the tooltip from showing.

            // Reset the text fields and images for the tooltip, and hide it.
            if (Tooltip != null && Tooltip.GameObject != null)
                Tooltip.Deactivate();
        }

        public void HidePopup()
        {
            popupTimer = 0; // Stop the timer and prevent the tooltip from showing.
            remoteTrigger = null; // reset the object that's remotely triggering this popup.

            // Reset the text fields and images for the tooltip, and hide it.
            if (Tooltip != null && Tooltip.GameObject != null)
                Tooltip.Deactivate();
        }

        /// <summary>Manually pop a tooltip up without requiring hovering. This is useful for in-game tutorials or NPC barks.</summary>
        /// <param name="duration">Number of seconds to display the tooltip.</param>
        /// <param name="triggeredBy">The game object that triggered this popup. Allows us to prevent multiple triggering.</param>
        /// <param name="isBlocking">Does this tooltip block all other tooltips while it is being displayed?</param>
        public void Popup(float duration, GameObject triggeredBy)
        {
            if (popupTimer > 0 || UTooltipManager.Instance == null || UTooltipManager.Instance.BlockingTooltip != null)
                return;

            Initialize();
            remoteTrigger = triggeredBy;

            // Fix if minWidth is greater than maxWidth.
            if (minTextWidth > maxTextWidth)
                maxTextWidth = minTextWidth;

            Tooltip.WarmUp();
            Tooltip.StaysOpen = staysOpen;
            //Tooltip.NeverRotate = neverRotate;
            Tooltip.IsBlocking = isBlocking;
            UTooltipManager.Instance.SetTextAndSize(this);

            // Show and position the tooltip.
            StartCoroutine(UTooltipManager.Instance.Show(this));
            popupTimer = 0.001f; // start the popup timer.
            popupTime = duration; // set the duration of the popup.
        }

        public void SetText(string parameterName, string text)
        {
            if (UTooltipManager.Instance == null)
                return;
            // If the list of parameterized text fields doesn't exist, create it.
            if (parameterizedTextFields == null)
                parameterizedTextFields = new List<ParameterizedTextField>();

            // Check to see if we find a matching field. If so, set its text to what was passed in.
            bool fieldExists = false;
            for (int i = 0; i < parameterizedTextFields.Count; i++)
            {
                if (parameterizedTextFields[i].name == parameterName)
                {
                    parameterizedTextFields[i].value = text;
                    fieldExists = true;
                }
            }

            // Finally, if the text field doesn't exist in the parameterized field list, create it and set its text to what was passed in.
            if (!fieldExists)
            {
                string delimiter = UTooltipManager.Instance.textFieldDelimiter;
                parameterizedTextFields.Add(new ParameterizedTextField()
                { name = parameterName, placeholder = String.Format("{0}{1}{0}", delimiter, parameterName), value = text });
            }
        }

        public void SetImage(string parameterName, Sprite sprite)
        {
            // If the list of dynamic image fields doesn't exist, create it.
            if (dynamicImageFields == null)
                dynamicImageFields = new List<DynamicImageField>();

            // Check to see if we find a matching field. If so, set its sprite to what was passed in.
            bool fieldExists = false;
            for (int i = 0; i < dynamicImageFields.Count; i++)
            {
                if (dynamicImageFields[i].name == parameterName)
                {
                    dynamicImageFields[i].replacementSprite = sprite;
                    fieldExists = true;
                }
            }

            // Finally, if the image field doesn't exist in the list, create it and set its image to what was passed in.
            if (!fieldExists)
            {
                dynamicImageFields.Add(new DynamicImageField()
                { name = parameterName, placeholderSprite = null, replacementSprite = sprite });
            }
        }

        public void TurnSectionOn(string parameterName)
        {
            ToggleSection(parameterName, true);
        }

        public void TurnSectionOff(string parameterName)
        {
            ToggleSection(parameterName, false);
        }

        public void ToggleSection(string parameterName, bool isOn)
        {
            // If the list of dynamic section fields doesn't exist, create it.
            if (dynamicSectionFields == null)
                dynamicSectionFields = new List<DynamicSectionField>();

            // Check to see if we find a matching field. If so, set its sprite to what was passed in.
            bool fieldExists = false;
            for (int i = 0; i < dynamicSectionFields.Count; i++)
            {
                if (dynamicSectionFields[i].name == parameterName)
                {
                    dynamicSectionFields[i].isOn = isOn;
                    fieldExists = true;
                }
            }

            // Finally, if the image field doesn't exist in the list, create it and set its image to what was passed in.
            if (!fieldExists)
                dynamicSectionFields.Add(new DynamicSectionField() { name = parameterName, isOn = isOn });
        }

        public void Clear()
        {
            if (dynamicSectionFields != null)
            {
                foreach (var tmpField in dynamicSectionFields)
                {
                    tmpField.isOn = false;
                }
            }

            if (dynamicImageFields != null)
            {
                foreach (var tmpField in dynamicImageFields)
                {
                    tmpField.placeholderSprite = null;
                    tmpField.replacementSprite = null;
                }
            }
            if (parameterizedTextFields != null)
            {
                foreach (var tmpField in parameterizedTextFields)
                {
                    tmpField.value = string.Empty;
                }
            }
        }
    }
}
