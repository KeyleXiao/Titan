/*******************************************************************
** 文件名:	UWebBrowserDialogWidget.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-23
** 版  本:	1.0
** 描  述:	浏览器消息框显示控件
********************************************************************/
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UWidgets;


namespace UWidgets.WebBrowser
{
    public class UWebBrowserDialogWidget : UCustomUIBehaviour
    {
        [SerializeField]
        public Text DialogText;
        [SerializeField]
        public Button OkBtn;
        [SerializeField]
        public Button YesBtn;
        [SerializeField]
        public Button NoBtn;
        [SerializeField]
        public InputField DialogPrompt;

        private UBrowser m_browser;

        public enum UDialogEventType : byte
        {
            /// <summary>警告类型对话框</summary>
            DET_Alert = 0,
            /// <summary>确认类型对话框</summary>
            DET_Confirm = 1,
            /// <summary>提示类型对话框</summary>
            DET_Prompt = 2
        }

        public virtual void InitPrefabLinks()
        {
            if (DialogText == null)
                DialogText = GetTransform.Find("DialogText").gameObject.GetComponent<Text>();

            GetComponetBindAction(OkBtn, "OkBtn", _strAction: m_browser.OnDialogOKBtnClick);
            GetComponetBindAction(YesBtn, "YesBtn", _strAction: m_browser.OnDialogYesBtnClick);
            GetComponetBindAction(NoBtn, "NoBtn", _strAction: m_browser.OnDialogCancelBtnClick);

            if (DialogPrompt == null)
                DialogPrompt = GetTransform.Find("DialogPrompt").gameObject.GetComponent<InputField>();

            SetVisible(false);
        }

        public void InitBrowser(UBrowser _browser)
        {
            m_browser = _browser;

            InitPrefabLinks();
        }

        public void SetVisible(bool _bVisible)
        {
            GetTransform.gameObject.SetActive(_bVisible);
        }

        protected void SetComponentVisible(UIBehaviour _comp, bool _bVisible)
        {
            if (_comp != null)
                _comp.gameObject.SetActive(_bVisible);
        }

        protected void SetComponentText(UIBehaviour _comp, string _strText)
        {
            if (_comp == null)
                return;
            if (_comp is Text)
                (_comp as Text).text = _strText;
            else if (_comp is InputField)
                (_comp as InputField).text = _strText;
        }

        protected void GetComponetBindAction(UIBehaviour _comp, string _strComName, UnityAction<string> _strAction = null)
        {
            if (_comp == null)
            {
                Transform _compTrans = GetTransform.FindChild(_strComName);
                if(_compTrans != null)
                    _comp = _compTrans.GetComponent<UIBehaviour>();
            }

            if (_comp != null)
            {
                if (_comp is Button)
                {
                    (_comp as Button).onClick.AddListener(() => { _strAction.Invoke(DialogPrompt.text); });
                }
                else if (_comp is InputField)
                {
                    (_comp as InputField).onEndEdit.AddListener(_strAction);
                }
            }
        }

        public virtual void OnDialogShowEvent(int _nDialogType, string _strDialogMessage, string _strDialogPrompt)
        {
            switch ((UDialogEventType)_nDialogType)
            {
                case UDialogEventType.DET_Alert:
                    ShowDialog_Alert(_strDialogMessage, _strDialogPrompt);
                    break;
                case UDialogEventType.DET_Confirm:
                    ShowDialog_Confirm(_strDialogMessage, _strDialogPrompt);
                    break;
                case UDialogEventType.DET_Prompt:
                    ShowDialog_Prompt(_strDialogMessage, _strDialogPrompt);
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 警告类型对话框
        /// </summary>
        /// <param name="_strDialogMessage">对话框消息</param>
        /// <param name="_strDialogPrompt">对话框提示</param>
        public virtual void ShowDialog_Alert(string _strDialogMessage, string _strDialogPrompt)
        {
            SetVisible(true);
            SetComponentVisible(OkBtn, true);
            SetComponentVisible(YesBtn, false);
            SetComponentVisible(NoBtn, false);
            SetComponentVisible(DialogPrompt, false);
            SetComponentText(DialogPrompt, _strDialogPrompt);
            SetComponentText(DialogText, _strDialogMessage);
        }
        /// <summary>
        /// 确认类型对话框
        /// </summary>
        /// <param name="_strDialogMessage">对话框消息</param>
        /// <param name="_strDialogPrompt">对话框提示</param>
        public virtual void ShowDialog_Confirm(string _strDialogMessage, string _strDialogPrompt)
        {
            SetVisible(true);
            SetComponentVisible(OkBtn, false);
            SetComponentVisible(YesBtn, true);
            SetComponentVisible(NoBtn, true);
            SetComponentVisible(DialogPrompt, false);
            SetComponentText(DialogPrompt, _strDialogPrompt);
            SetComponentText(DialogText, _strDialogMessage);
        }
        /// <summary>
        /// 提示类型对话框
        /// </summary>
        /// <param name="_strDialogMessage">对话框消息</param>
        /// <param name="_strDialogPrompt">对话框提示</param>
        public virtual void ShowDialog_Prompt(string _strDialogMessage, string _strDialogPrompt)
        {
            SetVisible(true);
            SetComponentVisible(OkBtn, false);
            SetComponentVisible(YesBtn, true);
            SetComponentVisible(NoBtn, true);
            SetComponentVisible(DialogPrompt, true);
            SetComponentText(DialogPrompt, _strDialogPrompt);
            SetComponentText(DialogText, _strDialogMessage);
        }

    }
}
