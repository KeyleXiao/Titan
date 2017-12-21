/*******************************************************************
** 文件名:	SNSInputDialogWndView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/5/23
** 版  本:	1.0
** 描  述:	SNS系统输入窗
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using ASpeedGame.Data.ShieldWorld;


namespace USpeedUI.SNS
{
	public delegate void SNSInputDialogConfirm(string text);
    public delegate void SNSInputDialogClose(SNSOpenInputDialogMsgData.EMDialogType type);

	public class SNSInputDialogWnd : UIPopupWnd<SNSInputDialogWndView>
    {
		public override WndID GetID()
		{
            return WndID.WND_ID_SNS_INPUTDIALOG;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "SNS/SNSInputDialogWndView";
        }

        public override bool Init()
        {
			m_bHaveEscCallBack = true;
			m_isKeyClose = true;

			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, this);

            return base.Init();
        }
		
		public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
						// 进入登录界面,清理上一局数据
						if(m_wndView != null)
						{
							m_wndView.Clear();
						}

						UnloadView();
					}
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
					}
                    break;
                case WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG:
                    {
						SetVisible(true);
						m_bHaveEscCallBack = true;

                        if (m_wndView != null)
                        {
                            m_wndView.openInputDialog(data as SNSOpenInputDialogMsgData);
						}
                    }
                    break;
				default:
                    break;
            }
        }


		public override bool OnEscCallBack()
		{
			m_wndView.onEscCallBack();

			return base.OnEscCallBack();
		}

		// 所有的子窗口都关闭后触发
		public void onAllSubWndClosed()
		{
			m_bHaveEscCallBack = false;
			SetVisible(false);
		}
	}

	/// <summary>
	/// 输入窗管理类
	/// </summary>
	public class SNSInputDialogWndView : UIBaseWndView
	{
		public GameObject DefaultInputDialog;

		// 修改名字（群名字或好友名字）
		public int ChangeType { set; get; }
		public int CurrentID { set; get; }
		public string CurrentName { set; get; }

		private Dictionary<int, SNSInputDialogSubWndView> m_InputDialogList;

		SNSInputDialogWnd Wnd
		{
			get
			{
				return m_wnd as SNSInputDialogWnd;
			}
		}

		public override bool Init(IUIWnd wnd)
		{
			base.Init(wnd);

			m_InputDialogList = new Dictionary<int, SNSInputDialogSubWndView>();

			return true;
		}

		public override void Start()
		{
			base.Start();
		}

		public override void Update()
		{
			base.Update();
		}

		public void Clear()
		{
			m_InputDialogList.Clear();
		}

		public void openInputDialog(SNSOpenInputDialogMsgData msgData)
		{
			// 判断是否可以创建新的输入窗口
			if (msgData.type != SNSOpenInputDialogMsgData.EMDialogType.EM_COMMON)
			{
				if (m_InputDialogList.ContainsKey((int)msgData.type))
				{
					return;
				}
			}

			SNSInputDialogSubWndView dialog = ResNode.InstantiateRes<SNSInputDialogSubWndView>(DefaultInputDialog);
			if (dialog == null)
				return;

			dialog.Init(msgData.type, msgData.title, msgData.defaultText, msgData.placeHolderText);
			dialog.OnConfirmClick = msgData.onClickDelegate;
			dialog.OnCloseClick = onInputDialogClose;

			m_InputDialogList.Add((int)msgData.type, dialog);

			dialog.transform.SetParent(DefaultInputDialog.transform.parent, false);
			dialog.gameObject.SetActive(true);

			// 全选输入框所有内容
			dialog.SelectAllInputFiledContent();
		}

		// 输入对话框关闭，则移除掉
		public void onInputDialogClose(SNSOpenInputDialogMsgData.EMDialogType type)
		{
			m_InputDialogList.Remove((int)type);

			onSubWndClosed();
		}

		public void onEscCallBack()
		{
			// 移除最后一个元素
			var tmpList = m_InputDialogList.Values.ToList();
			if(tmpList.Count != 0)
			{
				tmpList.Last().onClickClose();
			}
		}

		// 有子窗口关闭时间发生
		public void onSubWndClosed()
		{
			// 判断是否所有的子窗口关闭了
			if (m_InputDialogList.Count == 0)
				Wnd.onAllSubWndClosed();
		}
    }
	
	/// <summary>
	/// 输入窗逻辑
	/// </summary>
    public class SNSInputDialogSubWndView : MonoBehaviour
    {
        public Text Title;
        public InputField ContentInputField;
        public Text PlaceHolderText;

        private SNSInputDialogConfirm OnClickDelegate;
        public SNSInputDialogConfirm OnConfirmClick { set { OnClickDelegate = value; } }

        private SNSInputDialogClose OnCloseDelegate;
        public SNSInputDialogClose OnCloseClick { set { OnCloseDelegate = value; } }

        private SNSOpenInputDialogMsgData.EMDialogType m_Type;

        public void Init(SNSOpenInputDialogMsgData.EMDialogType type, string title, string defaultText, string placeHolderText)
        {
            m_Type = type;
            Title.text = title;
            ContentInputField.text = defaultText;
            PlaceHolderText.text = placeHolderText;
        }

        public void onClickConfirm()
        {
            string input = ContentInputField.text;

            // 检查屏蔽词
            if (ShieldWord.Instance.IsHaveShieldWord(input))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_UPDATE_NAME_HAVE_SHEILD);
                return;
            }

            OnClickDelegate(ContentInputField.text);

            onClickClose();
        }

        public void onClickClose()
        {
            var temp1 = gameObject;
            ResNode.DestroyRes(ref temp1);
            OnCloseDelegate(m_Type);
        }

		// 全选输入框所有内容
		public void SelectAllInputFiledContent()
		{
			ContentInputField.ActivateInputField();
		}

	}
}
