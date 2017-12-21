using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using U3D_Render.Common;

namespace USpeedUI.SystemFloatText
{
    public class SystemFloatTextWnd : UIBaseWnd<SystemFloatTextWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_SYSTEMFLOATTEXT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        public override string GetPrefabPath()
        {
            return "SystemFloatText/SystemFloatTextView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        this.LoadView();
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE:
                    {
                        if (m_wndView != null)
                        {
                            USystemFloatTextData textData = data as USystemFloatTextData;
                            m_wndView.CreateFloatText(textData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class SystemFloatTextWndView : UIBaseWndView
    {
        private float showTime = 0;
        private float timer = 0;

        public float[] posYArr;

        public URichText floatText;

        public override bool Init(IUIWnd wnd)
        {
            floatText.gameObject.SetActive(false);
            return base.Init(wnd);
        }

        public void CreateFloatText(USystemFloatTextData data)
        {
            if (data.nPos > posYArr.Length || data.nPos < 1)
            {
                data.nPos = 1;
            }
            floatText.rectTransform.anchoredPosition = new Vector2(0, posYArr[data.nPos - 1]);
            string stInfo = data.message;
            UBB.toHtml(ref stInfo, UBB_FORMAT_TYPE.UGUI);
            floatText.text = stInfo;
            if (data.nShowTime < 0)
            {
                data.nShowTime = 0;
            }
            showTime = data.nShowTime;
            floatText.gameObject.SetActive(true);
        }

        public override void Update()
        {
            if (showTime > 0)
            {
                timer += Time.deltaTime;
                if (timer > (showTime / 1000))
                {
                    floatText.text = "";
                    floatText.gameObject.SetActive(false);
                    showTime = 0;
                    timer = 0;
                }
            }
        }
    }
}
