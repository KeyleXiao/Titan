using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;

namespace USpeedUI.TooltipAndDialog
{
    public enum EDialogType
    {
        Default,   // 默认
        Public,    // 通用弹窗(聊天框里调用过来的)
        Botting,   // 挂机提示
        LifeHeroEntrance, // 本命英雄入口弹窗
        LifeHeroCulture,  // 本命英雄培养仓弹窗
        WarBroadcastPlayerContinuePlay,  // 战场广播界面，玩家断线重连弹窗
    }

    public class UIDialogHide : UIMsgData
    {
        public EDialogType dialogType;
        public UIDialogHide(EDialogType dialogType)
        {
            this.dialogType = dialogType;
        }
    }

    public class UIDialogData:UIMsgData
    {
        /// <summary>
        /// 标题
        /// </summary>
        public string strTitle;
        /// <summary>
        /// 主消息
        /// </summary>
        public string strMessage;
        /// <summary>
        /// 默认焦点按钮
        /// </summary>
        public string strFocusButton;
        /// <summary>
        /// 按钮组
        /// </summary>
        public DialogActions actButtons;
        /// <summary>
        /// 显示位置
        /// </summary>
        public Vector3? vec3Position;
        /// <summary>
        /// 主消息区域显示的图标
        /// </summary>
        public Sprite sprIcon;
        /// <summary>
        /// 模态窗口图标
        /// </summary>
        public Sprite sprModal;
        /// <summary>
        /// 模态窗口颜色
        /// </summary>
        public Color? colorModal;
        /// <summary>
        /// 是否为模态窗口
        /// </summary>
        public bool bModal;
        /// <summary>
        /// 模态区域点击时是否隐藏窗口
        /// </summary>
        public bool bHideByModalClick;
        /// <summary>
        /// 关闭按钮点击响应方法
        /// </summary>
        public UnityAction actCloseButton;
        /// <summary>
        /// 模态区域点击相应方法
        /// </summary>
        public UnityAction actModalClick;
        /// <summary>
        /// 禁止操作时间
        /// </summary>
        public int nForbidOperTime;
        /// <summary>
        /// 对话框类型
        /// </summary>
        public EDialogType DialogType;

        /// <summary>
        /// 同类型同ID对话框是否互斥
        /// </summary>
        public bool bMutex;

        public UIDialogData(string title = null,
                         string message = null,
                         DialogActions buttons = null,
                         string focusButton = null,
                         Vector3? position = null,
                         Sprite icon = null,

                         bool modal = false,
                         bool hideByModalClick = true,
                         Sprite modalSprite = null,
                         Color? modalColor = null,
                         UnityAction closeButtonAction = null,
                         int forbidOperTime = 0,
                         UnityAction modalClickAction = null,
                         EDialogType eDialogType = EDialogType.Default,
                         bool mutex = false)
        {
            strTitle = title;
            strMessage = message;
            strFocusButton = focusButton;
            actButtons = buttons;
            vec3Position = position;
            sprIcon = icon;
            sprModal = modalSprite;
            colorModal = modalColor;
            bModal = modal;
            bHideByModalClick = hideByModalClick;
            actCloseButton = closeButtonAction;
            nForbidOperTime = forbidOperTime;
            actModalClick = modalClickAction;
            DialogType = eDialogType;
            bMutex = mutex;
        }
    }

    public class UMeidatorDialog : UIBaseWnd<UViewDialog>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_Dialog;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        public override string GetPrefabPath()
        {
            return "Dialog/DialogView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);


            SetVisible(true);
            return base.Init();
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (!visible && m_wndView)
                m_wndView.Hide();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_ACCEPTWAIT_SHOW, this);

        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            if (m_wndView == null)
                return;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_DIALOG:
                    {
                        UIDialogData DialogData = data as UIDialogData;
                        m_wndView.Show(DialogData);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_DIALOG_HIDE:
                    {
                        m_wndView.Hide(data as UIDialogHide);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                case WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_ACCEPTWAIT_SHOW:
                    {
                        m_wndView.Hide();
                    }
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            m_wndView.Hide();
            return true;
        }
    }
    public class UViewDialog : UIBaseWndView, ITimerHanlder
    {
        [SerializeField]
        public Dialog DialogStyle;
        private List<Dialog> DialogStyleList = new List<Dialog>();
        private Dictionary<Dialog, int> TimeLimitDic = new Dictionary<Dialog, int>();
        private Dictionary<int, Dialog> DialogDataDic = new Dictionary<int, Dialog>();
        public override bool Init(IUIWnd wnd)
        {
            bool result = base.Init(wnd);

            Dialog.onDialogHideEvent += OnDialogHideEvent;

            return result;
        }

        public override void Destroy()
        {
            base.Destroy();

            Dialog.onDialogHideEvent -= OnDialogHideEvent;

            DialogStyleList.Clear();
            DialogStyleList = null;
            TimeLimitDic.Clear();
            TimeLimitDic = null;
            DialogDataDic.Clear();

            TimerManager.KillTimer(this, 1);
        }

        public void Show(UIDialogData _data)
        {
            this.gameObject.SetActive(true);
            Dialog tmpDialog = DialogStyle.Template();
            
            if (!DialogStyleList.Contains(tmpDialog))
            {
                DialogStyleList.Add(tmpDialog);                
            }

            if(_data.bMutex)
            {
                int key = (int)_data.DialogType;
                if (DialogDataDic.ContainsKey(key))
                {
                    DialogDataDic[key].Hide();
                }

                if(!DialogDataDic.ContainsKey(key))
                {
                    DialogDataDic.Add(key, tmpDialog);
                }
            }


            if (!TimeLimitDic.ContainsKey(tmpDialog) && _data.nForbidOperTime > 0)
                TimeLimitDic[tmpDialog] = _data.nForbidOperTime;

            if (_data.bModal && _data.colorModal == null)
                _data.colorModal = new Color(0.15f, 0.15f, 0.15f,0.5f);
            U3D_Render.Common.UBB.toHtml(ref _data.strMessage);
            tmpDialog.Show(_data.strTitle, _data.strMessage, _data.actButtons, 
                                        _data.strFocusButton, _data.vec3Position, _data.sprIcon, 
                                        _data.bModal, _data.bHideByModalClick, _data.sprModal, _data.colorModal, 
                                        this.transform,_data.actCloseButton,_data.actModalClick);

            tmpDialog.ToggleButtonInteractive(_data.nForbidOperTime <= 0);

            if (TimeLimitDic.Count > 0)
            {
                TimerManager.KillTimer(this, 1);
                TimerManager.SetTimer(this, 1, 1);
            }
        }

        public void Hide(UIDialogHide hideDialog)
        {
            if (hideDialog == null)
                return;

            Dialog dialog;
            if (DialogDataDic.TryGetValue((int)hideDialog.dialogType, out dialog))
            {
                if (dialog.CloseButton != null)
                    dialog.CloseButton.onClick.Invoke();
                else
                    dialog.Hide();
            }
        }

        public void Hide()
        {
            this.gameObject.SetActive(false);
            for (int i = DialogStyleList.Count - 1; i >= 0; i--)
            {
                if (DialogStyleList[i].CloseButton != null)
                    DialogStyleList[i].CloseButton.onClick.Invoke();
                else
                    DialogStyleList[i].Hide();
            }
            
            DialogStyleList.Clear();
        }

        public void OnTimer(int nTimerID)
        {
            List<Dialog> CopyList = new List<Dialog>();
            foreach (var item in TimeLimitDic)
                CopyList.Add(item.Key);

            foreach (var item in CopyList)
            {
                TimeLimitDic[item] -= 1;

                if (TimeLimitDic[item] <= 0)
                {
                    item.ToggleButtonInteractive(true);
                    TimeLimitDic.Remove(item);
                }
                    
            }

            if (TimeLimitDic.Count <= 0)
                TimerManager.KillTimer(this, 1);   
        }

        protected void OnDialogHideEvent(Dialog _hideDialog)
        {
            if (DialogStyleList.Contains(_hideDialog))
                DialogStyleList.Remove(_hideDialog);

            foreach(var item in DialogDataDic)
            {
                if(item.Value == _hideDialog)
                {
                    DialogDataDic.Remove(item.Key);
                    break;
                }
            }

            if (DialogStyleList.Count == 0)
                this.gameObject.SetActive(false);
        }
    }
}
