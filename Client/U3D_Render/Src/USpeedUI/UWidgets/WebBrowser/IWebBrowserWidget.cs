/*******************************************************************
** 文件名:	IWebBrowserWidget.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-23
** 版  本:	1.0
** 描  述:	与浏览器框架交互的接口文件，都是外部调用浏览器内部的接口
********************************************************************/
using UnityEngine;

namespace UWidgets.WebBrowser
{
    [SerializeField]
    public interface IWebBrowserWidget
    {
        /// <summary>
        /// 初始化显示层浏览器
        /// </summary>
        /// <param name="_browser"></param>
        void OpenWebBrowser(UBrowser _browser, int _nWidth, int _nHeight, string _strInitUrl);
        /// <summary>
        /// 关闭显示层浏览器
        /// </summary>
        void CloseWebBrowser();
        /// <summary>
        /// 浏览器地址栏跳转
        /// </summary>
        /// <param name="_strUrl"></param>
        void OnBrowserNavigate(string _strUrl);
        /// <summary>
        /// 浏览器前进/后退
        /// </summary>
        /// <param name="_bForward"></param>
        void OnBrowserForwardBack(bool _bForward);
        /// <summary>
        /// 浏览器消息框回应
        /// </summary>
        /// <param name="_bBtnResult"></param>
        /// <param name="_strPromptResult"></param>
        void OnDialogResponse(bool _bBtnResult, string _strPromptResult);
        /// <summary>
        /// 回应网页查询到的JS方法
        /// </summary>
        /// <param name="_strRespond"></param>
        void OnRespondToJSQuery(string _strRespond);
    }
}
