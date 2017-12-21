/*******************************************************************
** 文件名:	Dialog.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-26
** 版  本:	1.8.0
** 描  述:	消息框对象
		
********************************************************************/

using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;
using System.Collections.Generic;
using System;

namespace UIWidgets {
	/// <summary>
	/// Dialog.
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/Dialog")]
	public class Dialog : MonoBehaviour, ITemplatable
	{
		[SerializeField]
        protected Button defaultButton;

		/// <summary>
		/// Gets or sets the default button.
		/// </summary>
		/// <value>The default button.</value>
		public Button DefaultButton {
			get {
				return defaultButton;
			}
			set {
				defaultButton = value;
			}
		}
		
		[SerializeField]
        protected Text titleText;
		
		/// <summary>
		/// Gets or sets the text component.
		/// </summary>
		/// <value>The text.</value>
		public Text TitleText {
			get {
				return titleText;
			}
			set {
				titleText = value;
			}
		}

		[SerializeField]
        protected Text contentText;
		
		/// <summary>
		/// Gets or sets the text component.
		/// </summary>
		/// <value>The text.</value>
		public Text ContentText {
			get {
				return contentText;
			}
			set {
				contentText = value;
			}
		}

		[SerializeField]
        protected Image dialogIcon;

		/// <summary>
		/// Gets or sets the icon component.
		/// </summary>
		/// <value>The icon.</value>
		public Image Icon {
			get {
				return dialogIcon;
			}
			set {
				dialogIcon = value;
			}
		}

        [SerializeField]
        protected Button closeButton;
        public Button CloseButton
        {
            get { return closeButton; }
            set { closeButton = value; }
        }

		DialogInfoBase dialogInfo;

		/// <summary>
		/// Gets the dialog info.
		/// </summary>
		/// <value>The dialog info.</value>
		public DialogInfoBase DialogInfo {
			get {
				if (dialogInfo==null)
				{
					dialogInfo = GetComponent<DialogInfoBase>();
				}
				return dialogInfo;
			}
		}

		bool isTemplate = true;
		
		/// <summary>
		/// Gets a value indicating whether this instance is template.
		/// </summary>
		/// <value><c>true</c> if this instance is template; otherwise, <c>false</c>.</value>
		public bool IsTemplate {
			get {
				return isTemplate;
			}
			set {
				isTemplate = value;
			}
		}
		
		/// <summary>
		/// Gets the name of the template.
		/// </summary>
		/// <value>The name of the template.</value>
		public string TemplateName {
			get;
			set;
		}

		static Templates<Dialog> templates;

		/// <summary>
		/// Dialog templates.
		/// </summary>
		public static Templates<Dialog> Templates {
			get {
				if (templates==null)
				{
					templates = new Templates<Dialog>();
				}
				return templates;
			}
			set {
				templates = value;
			}
		}

        /// <summary>
        /// 记录Dialog挂接的源父窗口，可以说用完的时候回到这个节点下
        /// </summary>
        public Transform ParentTrans = null;

        public static Action<Dialog> onDialogHideEvent = null;

        /// <summary>
        /// Awake is called when the script instance is being loaded.
        /// </summary>
        public virtual void Awake()
		{
			if (IsTemplate)
			{
				gameObject.SetActive(false);
			}
		}

		/// <summary>
		/// This function is called when the MonoBehaviour will be destroyed.
		/// </summary>
        public virtual void OnDestroy()
		{
			DeactivateButtons();

			if (!IsTemplate)
			{
				templates = null;
				return ;
			}
			//if FindTemplates never called than TemplateName==null
			if (TemplateName!=null)
			{
				Templates.Delete(TemplateName);
			}
		}

		/// <summary>
		/// Return dialog instance by the specified template name.
		/// </summary>
		/// <param name="template">Template name.</param>
		static public Dialog Template(string template)
		{
			return Templates.Instance(template);
		}

		/// <summary>
		/// Return Dialog instance using current instance as template.
		/// </summary>
		public Dialog Template()
		{
			if ((TemplateName!=null) && Templates.Exists(TemplateName))
			{
				//do nothing
			}
			else if (!Templates.Exists(gameObject.name))
			{
				Templates.Add(gameObject.name, this);
			}
			else if (Templates.Get(gameObject.name)!=this)
			{
				Templates.Add(gameObject.name, this);
			}
			return Templates.Instance(gameObject.name);
		}

		/// <summary>
		/// The modal key.
		/// </summary>
		protected int? ModalKey;

        protected UnityAction m_actionModalClick = null;

        /// <summary>
        /// Show dialog.
        /// </summary>
        /// <param name="title">Title.</param>
        /// <param name="message">Message.</param>
        /// <param name="buttons">Buttons.</param>
        /// <param name="focusButton">Set focus on button with specified name.</param>
        /// <param name="position">Position.</param>
        /// <param name="icon">Icon.</param>
        /// <param name="modal">If set to <c>true</c> modal.</param>
        /// <param name="modalSprite">Modal sprite.</param>
        /// <param name="modalColor">Modal color.</param>
        /// <param name="canvas">Canvas.</param>
        public virtual void Show(string title = null,
                         string message = null,
                         DialogActions buttons = null,
                         string focusButton = null,
                         Vector3? position = null,
                         Sprite icon = null,

                         bool modal = false,
                         bool hideByModalClick = true,
		                 Sprite modalSprite = null,
		                 Color? modalColor = null,

		                 Transform canvas = null,

                         UnityAction closeBtnAction = null,
                         UnityAction modalclickAction = null)
		{
			if (position==null)
			{
				position = new Vector3(0, 0, 0);
			}

			SetInfo(title, message, icon);

            ParentTrans = canvas;

            var topCanvas = Utilites.FindTopmostCanvas(ParentTrans);
            if (topCanvas != null)
            {
                transform.SetParent(topCanvas, false);
            }
            
            if (modal)
			{
                if(hideByModalClick)
                    ModalKey = ModalHelper.Open(this, modalSprite, modalColor, Hide, transform as RectTransform);
                else
                    ModalKey = ModalHelper.Open(this, modalSprite, modalColor, null, transform as RectTransform);
                m_actionModalClick = modalclickAction;
            }
			else
			{
				ModalKey = null;
			}

			transform.SetAsLastSibling();

			transform.localPosition = (Vector3)position;
			gameObject.SetActive(true);

			CreateButtons(buttons, focusButton);

            if(CloseButton != null)
            {
                CloseButton.onClick.AddListener(Hide);
                if (closeBtnAction != null)
                {
                    CloseButton.onClick.AddListener(closeBtnAction);
                }
            }
		}

		/// <summary>
		/// Sets the info.
		/// </summary>
		/// <param name="title">Title.</param>
		/// <param name="message">Message.</param>
		/// <param name="icon">Icon.</param>
		public virtual void SetInfo(string title=null, string message=null, Sprite icon=null)
		{
			if (DialogInfo!=null)
			{
				DialogInfo.SetInfo(title, message, icon);
			}
			else
			{
				if ((title!=null) && (TitleText!=null))
				{
					TitleText.text = title;
                }
				if ((message!=null) && (ContentText!=null))
				{
					ContentText.text = message;
				}
				if ((icon!=null) && (Icon!=null))
				{
					Icon.sprite = icon;
				}
			}
		}

		/// <summary>
		/// Close dialog.
		/// </summary>
		public virtual void Hide()
		{
			if (ModalKey!=null)
			{
				ModalHelper.Close((int)ModalKey);
                if (m_actionModalClick != null)
                {
                    m_actionModalClick.Invoke();
                    m_actionModalClick = null;
                }
                    
            }

            if (onDialogHideEvent != null)
                onDialogHideEvent(this);

            Return();
		}

		/// <summary>
		/// The buttons cache.
		/// </summary>
		protected Stack<Button> buttonsCache = new Stack<Button>();

		/// <summary>
		/// The buttons in use.
		/// </summary>
		protected Dictionary<string,Button> buttonsInUse = new Dictionary<string,Button>();

		/// <summary>
		/// The buttons actions.
		/// </summary>
		protected Dictionary<string,UnityAction> buttonsActions = new Dictionary<string,UnityAction>();

		/// <summary>
		/// Creates the buttons.
		/// </summary>
		/// <param name="buttons">Buttons.</param>
		/// <param name="focusButton">Focus button.</param>
		protected virtual void CreateButtons(DialogActions buttons, string focusButton)
		{
			defaultButton.gameObject.SetActive(false);

			if (buttons==null)
			{
				return ;
			}

			buttons.ForEach(x => {
				var button = GetButton();

				UnityAction callback = () => {
					if (x.Value == null  || x.Value())
					{
						Hide();
					}
				};

				buttonsInUse.Add(x.Key, button);
				buttonsActions.Add(x.Key, callback);

				button.gameObject.SetActive(true);
				button.transform.SetAsLastSibling();

				var dialog_button = button.GetComponentInChildren<DialogButtonComponent>();
				if (dialog_button!=null)
				{
					dialog_button.SetButtonName(x.Key);
				}
				else
				{
					var text = button.GetComponentInChildren<Text>();
					if (text!=null)
					{
						text.text = x.Key;
					}
				}

				button.onClick.AddListener(buttonsActions[x.Key]);
                

                if (x.Key==focusButton)
				{
					button.Select();
				}
			});
		}

		/// <summary>
		/// Gets the button.
		/// </summary>
		/// <returns>The button.</returns>
		protected virtual Button GetButton()
		{
			if (buttonsCache.Count > 0)
			{
				return buttonsCache.Pop();
			}

			var button = Instantiate(DefaultButton) as Button;

			button.transform.SetParent(DefaultButton.transform.parent, false);

			//Utilites.FixInstantiated(DefaultButton, button);

			return button;
		}

		/// <summary>
		/// Return this instance to cache.
		/// </summary>
		protected virtual void Return()
		{
			Templates.ToCache(this);

			DeactivateButtons();
			ResetParametres();
            ResetCloseBtnAction();
        }

		/// <summary>
		/// Deactivates the buttons.
		/// </summary>
		protected virtual void DeactivateButtons()
		{
			buttonsInUse.ForEach(DeactivateButton);

			buttonsInUse.Clear();
			buttonsActions.Clear();
		}

		/// <summary>
		/// Deactivates the button.
		/// </summary>
		/// <param name="button">Button.</param>
		protected virtual void DeactivateButton(KeyValuePair<string,Button> button)
		{
			button.Value.gameObject.SetActive(false);
			button.Value.onClick.RemoveListener(buttonsActions[button.Key]);
			buttonsCache.Push(button.Value);
		}

		/// <summary>
		/// Resets the parametres.
		/// </summary>
		protected virtual void ResetParametres()
		{
			var template = Templates.Get(TemplateName);

			var title = template.TitleText!=null ? template.TitleText.text : "";
			var content = template.ContentText!=null ? template.ContentText.text : "";
			var icon = template.Icon!=null ? template.Icon.sprite : null;

			SetInfo(title, content, icon);

            transform.SetParent(ParentTrans, false);
        }

        protected virtual void ResetCloseBtnAction()
        {
            if(CloseButton != null)
            {
                CloseButton.onClick.RemoveAllListeners();
            }
        }
		/// <summary>
		/// Default function to close dialog.
		/// </summary>
		static public bool Close()
		{
			return true;
		}

        /// <summary>
        /// Set all buttons interactive state
        /// </summary>
        public void ToggleButtonInteractive(bool _bCanInteractive)
        {
            foreach (var btn in buttonsInUse)
            {
                btn.Value.interactable = _bCanInteractive;
            }
        }
	}
}
