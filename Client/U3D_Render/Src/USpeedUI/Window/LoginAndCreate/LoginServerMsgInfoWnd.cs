/*******************************************************************
** 文件名:	LoginServerMsgInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	登陆消息界面	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using GameLogic;
using U3D_Render.Common;
namespace USpeedUI.LoginServerMsgInfo
{

    public class ULoginServerMsgInfoWnd : UIPopupWnd<ULoginServerMsgInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_LOGINSERVERMSGINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Login/LoginServerMsgView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_OPEN_LOGIN_SERVERINFO:
                    {
                        if (m_wndView == null || !m_wndView.IsVisible())
                        {
                            SetVisible(true);
                        }

                        UIMsgCmdData uData = msgData as UIMsgCmdData;
                        cmd_open_servermsg_wnd cmdData = IntPtrHelper.toData<cmd_open_servermsg_wnd>(uData.ptrParam);
                        if (m_wndView != null)
                            m_wndView.updateLoginServerMsgInfo(cmdData);
                    }
                    break;

                case WndMsgID.WND_MSG_COMMON_LOADSCENE:
                    {
                        SetVisible(false);
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    public class ULoginServerMsgInfoWndView : UIBaseWndView
    {
        public Text Title;
        public Text MsgText;
        

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "LoginServerMsgInfo", "Title");
            return base.Init(wnd);
        }

        public void updateLoginServerMsgInfo(cmd_open_servermsg_wnd cmdData)
        {
           //更新界面信息
            UBB.toHtml(ref cmdData.szMsg);
            MsgText.text = cmdData.szMsg;
        }

        public void onCloseBtnClick()
        {
            //关闭界面信息
            SetVisible(false);
        }
       
    }
}
