/*******************************************************************
** 文件名:	UWebBrowser_PC_Widget.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-23
** 版  本:	1.0
** 描  述:	实现PC上与浏览器框架交互的接口文件，都是外部调用浏览器内部的接口
            目前使用浏览器核心是CEF3
********************************************************************/
using USpeedUI.UEffect;
using WebBrowser;

namespace UWidgets.WebBrowser
{
    public class UWebBrowser_PC_Widget : WebBrowser2D, IWebBrowserWidget
    {
        protected UBrowser m_Browser;

        private bool _bInit = false;

        public override void OnDestroy()
        {
            if (_bInit)
            {
                base.OnDestroy();
                _bInit = false;

                if(m_materialEffectParam != null)
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectMaterial, ref m_materialEffectParam);
            }
        }

        public virtual void OnEnable()
        {
            if (_bInit)
            {
                if (m_materialEffectParam == null)
                {
                    m_materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Contrast, Browser2D, true);
                    if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref m_materialEffectParam))
                    {
                        (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetTexture("_MainTex", Browser2D.texture);
                        (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_ContrastValue", 3f);
                    }
                }
            }
        }

        private UEffectParamBase m_materialEffectParam;

        #region 接口实现
        public void OpenWebBrowser(UBrowser _browser, int _nWidth, int _nHeight, string _strInitUrl)
        {
            _bInit = true;
            m_Browser = _browser;
            Width = _nWidth;
            Height = _nHeight;
            InitialURL = _strInitUrl;

            OnEngineConnected += UWebBrowser_PC_Widget_OnEngineConnected;
            OnJSQuery += UWebBrowser_PC_Widget_OnJSQuery;
            OnPageLoaded += UWebBrowser_PC_Widget_OnPageLoaded;
            OnDialogShow += UWebBrowser_PC_Widget_OnDialogShow;

            gameObject.SetActive(true);
        }

        public void CloseWebBrowser()
        {
            OnDestroy();
        }

        public void OnBrowserNavigate(string _strUrl)
        {
            OnNavigate(_strUrl);
        }

        public void OnBrowserForwardBack(bool _bForward)
        {
            GoBackForward(_bForward);
        }

        public void OnDialogResponse(bool _bBtnResult, string _strPromptResult)
        {
            DialogPromptResult = _strPromptResult;
            DialogResult(_bBtnResult);
        }

        public void OnRespondToJSQuery(string _strRespond)
        {
            RespondToJSQuery(_strRespond);
        }
        #endregion

        #region 处理内部事件
        private void UWebBrowser_PC_Widget_OnDialogShow(int _nDialogType, string _strDialogMessage, string _strDialogPrompt)
        {
            if(m_Browser != null)
                m_Browser.OnDialogShowEvent.Invoke(_nDialogType, _strDialogMessage, _strDialogPrompt);
        }

        private void UWebBrowser_PC_Widget_OnPageLoaded(string _strLoadedURL)
        {
            if(m_Browser != null && m_Browser.OnPageLoadedEvent != null)
                m_Browser.OnPageLoadedEvent.Invoke(_strLoadedURL);
        }

        private void UWebBrowser_PC_Widget_OnEngineConnected(string _strQuery)
        {
            if(m_Browser != null && m_Browser.OnConnectedEvent != null)
            m_Browser.OnConnectedEvent.Invoke(_strQuery);
        }

        private void UWebBrowser_PC_Widget_OnJSQuery(string _strQuery)
        {
            if(m_Browser != null && m_Browser.OnJsQueryEvent != null)
                m_Browser.OnJsQueryEvent.Invoke(_strQuery);
        }
        #endregion
    }
}
