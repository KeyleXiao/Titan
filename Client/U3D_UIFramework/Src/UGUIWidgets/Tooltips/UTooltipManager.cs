using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace UIWidgets
{
    public class UTooltipManagerInitEvent : UnityEvent
    {

    }

    public class UTooltipManagerInitializeParam
    {
        public const float DefaultTooltipDelay = 0.33f;
        public const float DefaultTooltipDuration = 0.2f;

        public RectTransform TooltipContainer;
        public Camera UICamera;
        public Canvas TooltipCanvas;
        public float TooltipDelay = DefaultTooltipDelay;
        public float TooltipDuration = DefaultTooltipDuration;
        public UTooltipManagerInitializeParam(RectTransform _RootTransform = null,Camera _Camera = null,Canvas _Canvas = null,
                                                float _fTooltipDelay = 0.0f,float _fTooltipDuration = 0.0f)
        
        {
            TooltipContainer = _RootTransform;
            UICamera = _Camera;
            TooltipCanvas = _Canvas;
            TooltipDelay = _fTooltipDelay;
            TooltipDuration = _fTooltipDuration;
        }
    }

    public class UTooltipManager 
    {
        #region 初始化从外部获取的值和外部需要获取的这些值
        private UTooltipManagerInitializeParam m_InitializeParam;
        public RectTransform TooltipContainer
        {
            get
            {
                return m_InitializeParam.TooltipContainer;
            }
        }

        public Camera UICamera
        {
            get
            {
                if (m_InitializeParam != null)
                    return m_InitializeParam.UICamera;
                else
                    Debug.LogWarning("UTooltipManager的m_InitializeParam没初始化，也就是UViewTooltip没执行UTooltipManager.Initialize方法。");
                return null;
            }
        }

        public Canvas TooltipCanvas
        {
            get
            {
                if (m_InitializeParam != null)
                    return m_InitializeParam.TooltipCanvas;
                else
                    Debug.LogWarning("UTooltipManager的m_InitializeParam没初始化，也就是UViewTooltip没执行UTooltipManager.Initialize方法。");
                return null;
            }
        }
        public float TooltipDelay
        {
            get
            {
                if (m_InitializeParam == null)
                    Debug.LogWarning("UTooltipManager的m_InitializeParam没初始化，也就是UViewTooltip没执行UTooltipManager.Initialize方法。");

                return m_InitializeParam != null? m_InitializeParam.TooltipDelay : UTooltipManagerInitializeParam.DefaultTooltipDelay;
            }
        }
        public float TooltipFadeDuration
        {
            get
            {
                if (m_InitializeParam == null)
                    Debug.LogWarning("UTooltipManager的m_InitializeParam没初始化，也就是UViewTooltip没执行UTooltipManager.Initialize方法。");

                return m_InitializeParam != null? m_InitializeParam.TooltipDuration : UTooltipManagerInitializeParam.DefaultTooltipDuration;
            }
        }
        #endregion

        public string textFieldDelimiter = "%";

        public bool IsInitialized
        {
            get { return isInitialized; }
        }
        public UTooltipManagerInitEvent TooltipMgrInitEvent = new UTooltipManagerInitEvent();
        /// <summary>
        /// 在显示的时候将未转UBB的字符串丢到UBB那边转换
        /// </summary>
        public Func<string,string> TooltipMgrSetTextEvent;

        private static UTooltipManager instance;
        private bool isInitialized;

        /// <summary>
        /// 超出边框时将额外计算位置，设为true为不使用，可以减少一些性能损耗
        /// </summary>
        public bool overflowProtection = true;


        public static UTooltipManager Instance
        {
            get
            {
                if (instance == null)
                    instance = new UTooltipManager();
            	if (instance == null)
                {
                    Debug.LogError("创建UTooltipManager 失败，查查原因.");
                    return null;
                }
                    

                return instance;
            }
        }

        /// <summary>Holds a reference to the in-scene, runtime gameobject associated with each tooltip style prefab.</summary>
        public Dictionary<int, Tooltip> Tooltips { get; set; }

        /// <summary>When populated, this is the only tooltip allowed on the screen.</summary>
        public Tooltip BlockingTooltip { get; set; }

        public void Initialize(UTooltipManagerInitializeParam _param)
        {
            if (isInitialized)
                return;

            m_InitializeParam = _param;

            Tooltips = new Dictionary<int, Tooltip>();

            TooltipMgrInitEvent.Invoke();
    
            isInitialized = true;
        }
				
        public void Destroy()
        {
            TooltipMgrInitEvent.RemoveAllListeners();
        }

        /// <summary>Sets the parameterized text fields on the tooltip.</summary>
        /// <remarks>This is separate from the Show() method because we need to wait a frame or two so the text's RectTransform has time to update its preferredWidth.</remarks>
        public void SetTextAndSize(UTooltipTrigger trigger)
        {
			Tooltip tooltip = trigger.Tooltip;
            if (tooltip == null || trigger.parameterizedTextFields == null)
                return;

            if (tooltip.TextFields == null || tooltip.TextFields.Count == 0)
                return;

            LayoutElement mainTextContainer = tooltip.TooltipStyle.mainTextContainer;
            if (mainTextContainer == null)
                Debug.LogWarning(String.Format("No main text container defined on tooltip style \"{0}\". Note: This LayoutElement is needed in order to resize text appropriately.", trigger.Tooltip.GameObject.name));
            else
                mainTextContainer.preferredWidth = trigger.minTextWidth;

            for (int i = 0; i < tooltip.TextFields.Count; i++)
            {
                Text txt = tooltip.TextFields[i].Text;
                if (txt.text.Length < 3)
                    continue; // text is too short to contain a parameter, so skip it.

                for (int j = 0; j < trigger.parameterizedTextFields.Count; j++)
                {
                    if (!String.IsNullOrEmpty(trigger.parameterizedTextFields[j].value))
                    {
                        string tmpText = txt.text.Replace(trigger.parameterizedTextFields[j].placeholder, trigger.parameterizedTextFields[j].value);
                        if(TooltipMgrSetTextEvent != null)
                        {
                            tmpText = TooltipMgrSetTextEvent.Invoke(trigger.parameterizedTextFields[j].value);
                            trigger.parameterizedTextFields[j].value = tmpText;
                        }                           
                        
                        txt.text = txt.text.Replace(trigger.parameterizedTextFields[j].placeholder, trigger.parameterizedTextFields[j].value);
                    }
                        
                }

                if (mainTextContainer != null)
                {
                    // if the text would be wider than allowed, constrain the main text container to that limit.
                    if (txt.preferredWidth > trigger.maxTextWidth)
                        mainTextContainer.preferredWidth = trigger.maxTextWidth;
                    // otherwise, if it's within the allotted space but bigger than the text container's default width, expand the main text container to accommodate.
                    else if (txt.preferredWidth > trigger.minTextWidth && txt.preferredWidth > mainTextContainer.preferredWidth)
                        mainTextContainer.preferredWidth = txt.preferredWidth;
                }
            }
        }

        /// <summary>Displays the tooltip.</summary> 
        /// <remarks>
        /// This method first waits a couple frames before sizing and positioning the tooltip. 
        /// This is necessary in order to get an accurate preferredWidth property of the dynamic text field.
        /// </remarks>
        public IEnumerator Show(UTooltipTrigger trigger)
        {
            if (trigger.tooltipStyle == null)
            {
                Debug.LogWarning("UTooltipTrigger \"" + trigger.name + "\" has no associated TooltipStyle. Cannot show tooltip.");
                yield break;
            }

            Tooltip tooltip = trigger.Tooltip;
            Image tooltipBkgImg = tooltip.BackgroundImage;


            // Replace dynamic image placeholders with the correct images.
            if (trigger.dynamicImageFields != null)
            {
                for (int i = 0; i < trigger.dynamicImageFields.Count; i++)
                {
                    for (int j = 0; j < tooltip.ImageFields.Count; j++)
                    {
                        if (tooltip.ImageFields[j].Name == trigger.dynamicImageFields[i].name)
                        {
                            if (trigger.dynamicImageFields[i].replacementSprite == null)
                                tooltip.ImageFields[j].Image.sprite = tooltip.ImageFields[j].Original;
                            else
                                tooltip.ImageFields[j].Image.sprite = trigger.dynamicImageFields[i].replacementSprite;
                        }
                    }
                }
            }

            // Toggle dynamic sections on or off.
            if (trigger.dynamicSectionFields != null)
            {
                for (int i = 0; i < trigger.dynamicSectionFields.Count; i++)
                {
                    for (int j = 0; j < tooltip.SectionFields.Count; j++)
                    {
                        if (tooltip.SectionFields[j].Name == trigger.dynamicSectionFields[i].name)
                            tooltip.SectionFields[j].GameObject.SetActive(trigger.dynamicSectionFields[i].isOn);
                    }
                }
            }

            // Wait for 2 frames so we get an accurate PreferredWidth on the Text component.
            yield return Utilites.WaitForFrames(2);


            // Set the position of the tooltip.
            tooltip.SetPosition(trigger, TooltipCanvas, UICamera);

            // Set the tint color of the tooltip panel and tips.
            tooltipBkgImg.color = trigger.backgroundTint;

            // If this is a blocking tooltip, assign it as such.
            if (tooltip.IsBlocking)
                BlockingTooltip = tooltip;

            // Display the tooltip.
            tooltip.Display(TooltipFadeDuration);
        }

        /// <summary>Closes all visible tooltips.</summary>
        /// <remarks>Useful for when your player may have a tutorial tooltip up and hits ESC to go to the main menu, for example.</remarks>
        public void CloseAll()
        {
            // NOTE: If you needed to, you could store a reference to the open tooltip in the UTooltipManager so you could re-open it later, 
            // when the player picks up where they left off.
            UTooltipTrigger[] tooltipTriggers = UnityEngine.Object.FindObjectsOfType<UTooltipTrigger>();
            for (int i = 0; i < tooltipTriggers.Length; i++)
                tooltipTriggers[i].ForceHideTooltip();
        }
    }
}
