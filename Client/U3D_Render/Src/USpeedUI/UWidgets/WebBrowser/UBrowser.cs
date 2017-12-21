/*******************************************************************
** 文件名:	UBrowser.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-23
** 版  本:	1.0
** 描  述:	项目中浏览器部分，负责控制消息框组件、工具栏组件和网页显示组件
********************************************************************/
using System;
using UnityEngine;
using UnityEngine.Events;
using USpeedUI.UWidgets;

namespace UWidgets.WebBrowser
{
    public class UBrowser : UCustomUIBehaviour
    {
        public UWebBrowserDialogWidget WebBrowserDialog;
        public UWebBrowserToolbarWidget WebBrowserToolbar;

        [SerializeField]
        public IWebBrowserWidget WebBrowser;

        public int Width = 1024;

        public int Height = 768;

        public string InitialURL = "http://www.q1.com";

        protected UWebBrowserToolbarWidget.EToolbar m_tmpToolbar;

        protected override void Awake()
        {
            InitPrefabLinks();
        }

        protected override void Start()
        {
            base.Start();

            GetRectTransform.sizeDelta = new Vector2(Width, Height);

            if(WebBrowser != null)
                WebBrowser.OpenWebBrowser(this, Width, Height, InitialURL);
            OnPageLoadedEvent.AddListener(OnPageLoaded);
            OnDialogShowEvent.AddListener(OnDialogShow);
        }

        public virtual void InitPrefabLinks()
        {
            if (WebBrowser == null)
            {
                Transform widgetTrans = GetTransform.FindChild("IWebBrowserWidget");
                if (widgetTrans != null)
                {
                    WebBrowser = widgetTrans.GetComponent<IWebBrowserWidget>();
                    if (WebBrowser == null)
                    {
                        Trace.LogWarning("没有找到 IwebBrowserWidget 组件。");
                    }
                }
            }

            if (WebBrowserDialog == null)
            {
                Transform widgetTrans = GetTransform.FindChild("WebBrowserDialog");
                if (widgetTrans != null)
                {
                    WebBrowserDialog = widgetTrans.GetComponent<UWebBrowserDialogWidget>();
                    if (WebBrowserDialog == null)
                    {
                        Trace.LogWarning("没有找到 UWebBrowserDialogWidget 组件。");
                    }
                }
            }
            WebBrowserDialog.InitBrowser(this);


            if (WebBrowserToolbar == null)
            {
                Transform widgetTrans = GetTransform.FindChild("WebBrowserToolbar");
                if (widgetTrans != null)
                {
                    WebBrowserToolbar = widgetTrans.GetComponent<UWebBrowserToolbarWidget>();
                    if (WebBrowserToolbar == null)
                    {
                        Trace.LogWarning("没有找到 UWebBrowserToolbarWidget 组件。");
                    }
                }
            }
            WebBrowserToolbar.InitBrowser(this);

        }

        /// <summary>
        /// 网页加载完成时返回当前显示的网页地址
        /// </summary>
        protected void OnPageLoaded(string _strLoadedUrl)
        {
            if(WebBrowserToolbar != null)
                WebBrowserToolbar.OnPageLoaded(_strLoadedUrl);
        }
        protected void OnDialogShow(int _nDialogType, string _strDialogMessage, string _strDialogPrompt)
        {
            if(WebBrowserToolbar != null)
                WebBrowserDialog.OnDialogShowEvent(_nDialogType, _strDialogMessage, _strDialogPrompt);
        }

        #region 侦听浏览器 内部->外部 事件
        [Serializable]
        public class JavaScriptQueryEvent : UnityEvent<string> { }
        /// <summary>
        /// 接收网页查询到的JS方法调用事件
        /// 参数1：网页传来的数据
        /// </summary>
        [SerializeField]
        public JavaScriptQueryEvent OnJsQueryEvent = new JavaScriptQueryEvent();

        [Serializable]
        public class BrowserEngineConnectedEvent : UnityEvent<string> { }
        /// <summary>
        /// 连接浏览器引擎事件
        /// 参数1：连接消息
        /// </summary>
        [SerializeField]
        public BrowserEngineConnectedEvent OnConnectedEvent = new BrowserEngineConnectedEvent();
        [Serializable]
        public class BrowserEngineDisConnectedEvent : UnityEvent<string> { }
        /// <summary>
        /// 取消连接浏览器引擎事件（意味着关闭浏览器界面）
        /// 参数1：断开连接消息（预留）
        /// </summary>
        [SerializeField]
        public BrowserEngineConnectedEvent OnDisConnectedEvent = new BrowserEngineConnectedEvent();


        [Serializable]
        public class PageLoadedEvent : UnityEvent<string> { }
        /// <summary>
        /// 网页加载完成事件
        /// 参数1：加载完成地址
        /// </summary>
        [SerializeField]
        public PageLoadedEvent OnPageLoadedEvent = new PageLoadedEvent();

        [Serializable]
        public class DialogShowEvent : UnityEvent<int, string, string> { }
        /// <summary>
        /// 网页加载完成事件
        /// 参数1：消息框类型
        /// 参数2：消息框信息
        /// 参数3：可提示用户进行输入的对话框消息
        /// </summary>
        [SerializeField]
        public DialogShowEvent OnDialogShowEvent = new DialogShowEvent();


        #endregion

        #region 调用浏览器 外部->内部 方法
        public void OnToolbarCloseBtnClick()
        {
            if(WebBrowser != null)
                WebBrowser.CloseWebBrowser();
            if(OnDisConnectedEvent != null)
                OnDisConnectedEvent.Invoke(string.Empty);
        }

        public void OnToolbarForwardBtnClick()
        {
            if(WebBrowser != null)
                WebBrowser.OnBrowserForwardBack(true);
        }

        public void OnToolbarBackBtnClick()
        {
            if(WebBrowser != null)
            WebBrowser.OnBrowserForwardBack(false);
        }

        public void OnToolbarUrlFieldEndEdit(string _strUrl)
        {
            if(WebBrowser != null)
                WebBrowser.OnBrowserNavigate(_strUrl);
        }

        public void OnDialogOKBtnClick(string _strPromptResult)
        {
            if(WebBrowserDialog != null)
                WebBrowserDialog.SetVisible(false);
            if(WebBrowser != null)
                WebBrowser.OnDialogResponse(true, _strPromptResult);
        }

        public void OnDialogYesBtnClick(string _strPromptResult)
        {
            if(WebBrowserDialog != null)
                WebBrowserDialog.SetVisible(false);
            if(WebBrowser != null)
                WebBrowser.OnDialogResponse(true, _strPromptResult);
        }

        public void OnDialogCancelBtnClick(string _strPromptResult)
        {
            if(WebBrowserDialog != null)
                WebBrowserDialog.SetVisible(false);
            if(WebBrowser != null)
                WebBrowser.OnDialogResponse(false, _strPromptResult);
        }

        public void OnRespondToJSQuery(string _strRespond)
        {
            if(WebBrowser != null)
                WebBrowser.OnRespondToJSQuery(_strRespond);
        }

        public void SetToolbarType(UWebBrowserToolbarWidget.EToolbar _eValue)
        {
            if(WebBrowserToolbar != null)
                WebBrowserToolbar.SetToolbarItemUse(_eValue);
        }
        #endregion
    }
}
