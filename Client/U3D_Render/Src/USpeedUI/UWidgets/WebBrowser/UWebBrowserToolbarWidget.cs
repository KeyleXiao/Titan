/*******************************************************************
** 文件名:	UWebBrowserToolbarWidget.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-23
** 版  本:	1.0
** 描  述:	浏览器工具栏显示控件
********************************************************************/
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using DG.Tweening;

namespace UWidgets.WebBrowser
{
    public class UWebBrowserToolbarWidget : UCustomUIBehaviour, IPointerExitHandler
    {
        [SerializeField]
        public InputField UrlField;
        [SerializeField]
        public Image Background;
        [SerializeField]
        public Button BackBtn;
        [SerializeField]
        public Button ForwardBtn;
        [SerializeField]
        public Button CloseBtn;

        public Button PullBtn;

        public RectTransform ControllerTrans;

        /// <summary>
        /// 工具栏功能枚举
        /// </summary>
        public enum EToolbar : int
        {
            ETB_Url = 0x01,
            ETB_Back = 0x02,
            ETB_Forward = 0x04,
            ETB_Close = 0x08,
        }
        [HideInInspector]
        public EToolbar Toolbar;
        public bool IsAutoHide = true;


        private UBrowser m_browser;
        private Dictionary<EToolbar, UIBehaviour> m_ToolbarItems = new Dictionary<EToolbar, UIBehaviour>();

        public void InitPrefabLinks()
        {
            GetComponetBindAction(UrlField, "Controller/UrlField", EToolbar.ETB_Url, _strAction: m_browser.OnToolbarUrlFieldEndEdit);

            GetComponetBindAction(BackBtn, "Controller/BackBtn", EToolbar.ETB_Back, _action: m_browser.OnToolbarBackBtnClick);

            GetComponetBindAction(ForwardBtn, "Controller/ForwardBtn", EToolbar.ETB_Forward, _action: m_browser.OnToolbarForwardBtnClick);

            GetComponetBindAction(CloseBtn, "Controller/CloseBtn", EToolbar.ETB_Close, _action: m_browser.OnToolbarCloseBtnClick);

            GetComponetBindAction(PullBtn, "Controller/PullBtn", null, _action: OnPullBtnClick);

            SetToolbarItemUse(Toolbar);

            if (Background == null)
            {
                //2d
                Background = GetTransform.GetComponent<Image>();
            }

            if (ControllerTrans == null)
            {
                ControllerTrans = GetTransform.FindChild("Controller").transform as RectTransform;
            }

            if ((int)Toolbar == 0)
            {
                ControllerTrans.gameObject.SetActive(false);
            }

            if (IsAutoHide)
            {
                Background.raycastTarget = false;
                ControllerTrans.anchoredPosition = new Vector2(0, 25);
                ControllerTrans.sizeDelta = new Vector2(0, 50);
            }
        }

        protected void GetComponetBindAction(UIBehaviour _comp, string _strComName, EToolbar? _eToolbarType, UnityAction _action = null, UnityAction<string> _strAction = null)
        {
            if (_comp == null)
            {
                Transform _compTrans = GetTransform.FindChild(_strComName);
                if (_compTrans != null)
                    _comp = _compTrans.GetComponent<UIBehaviour>();
            }

            if (_comp != null)
            {
                if (_comp is Button)
                {
                    (_comp as Button).onClick.AddListener(_action);
                }
                else if (_comp is InputField)
                {
                    (_comp as InputField).onEndEdit.AddListener(_strAction);
                }


                if (_eToolbarType != null)
                {
                    m_ToolbarItems.Add((EToolbar)_eToolbarType, _comp);
                }
            }
        }

        /// <summary>
        /// 网页加载完成时返回当前显示的网页地址
        /// </summary>
        public void OnPageLoaded(string _strLoadedUrl)
        {
            UrlField.text = _strLoadedUrl;
        }
        public void InitBrowser(UBrowser _browser)
        {
            m_browser = _browser;

            InitPrefabLinks();
        }

        protected void OnPullBtnClick()
        {
            if (IsAutoHide)
            {
                Background.raycastTarget = true;

                ControllerTrans.DOAnchorPos(new Vector2(0, 0), 1f).SetAutoKill(true);
                ControllerTrans.DOSizeDelta(new Vector2(0, 0), 1f).SetAutoKill(true).OnKill(() => { UrlField.interactable = true; UrlField.ActivateInputField(); });
            }


        }

        public void OnPointerExit(PointerEventData eventData)
        {
            if (IsAutoHide)
            {
                ControllerTrans.DOAnchorPos(new Vector2(0, 25), 1f).SetAutoKill(true);
                ControllerTrans.DOSizeDelta(new Vector2(0, 50), 1f).SetAutoKill(true);
                UrlField.interactable = false;
                UrlField.DeactivateInputField();
                Background.raycastTarget = false;
            }
        }

        protected void UseToolbarItem(UIBehaviour _comp, EToolbar _eToolbarType)
        {
            if (((int)Toolbar & (int)_eToolbarType) == (int)_eToolbarType)
                _comp.gameObject.SetActive(true);
            else
                _comp.gameObject.SetActive(false);
        }
        /// <summary>
        /// 设置控制面板需要显示的元素
        /// </summary>
        /// <param name="_eNewToolbar"></param>
        public void SetToolbarItemUse(EToolbar _eNewToolbar)
        {
            Toolbar = _eNewToolbar;
            foreach (KeyValuePair<EToolbar, UIBehaviour> keyValuePair in m_ToolbarItems)
            {
                UseToolbarItem(keyValuePair.Value, keyValuePair.Key);
            }
        }
    }
}
