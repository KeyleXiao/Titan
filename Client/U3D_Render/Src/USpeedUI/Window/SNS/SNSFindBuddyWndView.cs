/*******************************************************************
** 文件名:	SNSFindBuddyWndView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/17
** 版  本:	1.0
** 描  述:	SNS查找好友
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.Common;
using DataCenter;
using DG.Tweening;
using USpeedUI.UWidgets;
using UIWidgets;
using GameLogic;


namespace USpeedUI.SNS
{
	public class SNSFindBuddyWnd : UIPopupWnd<SNSFindBuddyWndView>
	{
		public override WndID GetID()
		{
			return WndID.WND_ID_SNS_FINDBUDDY;
		}

		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP02;
		}

		public override string GetPrefabPath()
		{
			return "SNS/SNSFindBuddyWndView";
		}

		public override bool Init()
		{
			m_isKeyClose = true;

			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_FINDBUDDY_WND, this);

			
			return base.Init();
		}

		public override void Destroy()
		{
			base.Destroy();
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_FINDBUDDY_WND, this);
		}

		public override void OnMessage(WndMsgID msgID, UIMsgData data)
		{
			switch (msgID)
			{
				case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
					{
						// 进入登录界面,清理上一局数据
						if (m_wndView != null)
						{
							m_wndView.Clear();
						}

						UnloadView();
					}
					break;
				case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
					{
						//SetVisible(true);
					}
					break;
				case WndMsgID.WND_MSG_SNS_SHOW_FINDBUDDY_WND:
					{
						SetVisible(true);
					}
					break;
				default:
					break;
			}
		}
	}


	public class SNSFindBuddyWndView : UIBaseWndView
    {
        public InputField FindInputField;

		public override bool Init(IUIWnd wnd)
		{
			base.Init(wnd);

			return true;
        }

        public bool Clear()
        {
            return true;
        }

        public void closeFindBuddyWnd()
        {
             gameObject.SetActive(false);
        }

        public void onClickFindBuddy()
        {
            if (FindInputField.text.Length <= 0)
            {
                return;
            }

            gameview_sns_req_add_buddy_by_name data = new gameview_sns_req_add_buddy_by_name();
            data.szActorName = FindInputField.text;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_SNS_REQ_ADD_BUDDY_BY_NAME, ref data);

            FindInputField.text = "";
        }
    }
}
