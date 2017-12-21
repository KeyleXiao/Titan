using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DG.Tweening;
using USpeedUI.UEffect;

namespace USpeedUI.FloatMoney
{
    public class FloatMoneyWnd : UIPopupWnd<FloatMoneyWndView>
    {
        private int nGold;

        public override WndID GetID()
        {
            return WndID.WND_ID_FLOATMONEY;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "FloatMoney/FloatMoneyView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            //订阅数值属性回调
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            //退订数值属性回调
            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        private void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_GOLD:
                    {
                        if (nGold == 0)
                        {
                            nGold = e.data.nValue;
                            return;
                        }
                        
                        int deltaNum = e.data.nValue - e.data.nBeforeValue;
                        if (e.data.nEntityID == EntityFactory.MainHeroID && deltaNum > 0 && e.data.nBeforeValue != 0)
                        {
                            if (!UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_POPUP01).gameObject.activeSelf)
                                return;

                            SetVisible(true);
                            if (m_wndView != null)
                            {
                                m_wndView.SetFloatMoney(deltaNum); 
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }

    }

    public class FloatMoneyWndView : UIBaseWndView
    {
        public RectTransform FloatMoneyRect;
        public Text AddMoney;

        public float fJumpDis = 60.0f;
        public float fJumpPower = 30.0f;
        public float fJumpTime = 0.7f;
        public float fFadeTime = 1.0f;
        //public float fEffectTime = 0.7f;

        private Sequence m_Sequence;
        //private UEffectParamBase m_effectParam;

        public void SetFloatMoney(int nAddMoney)
        {
            AddMoney.text = "+" + nAddMoney;
            FloatMoneyRect.anchoredPosition = new Vector2(fJumpDis / 2, 0);
            FloatMoneyRect.gameObject.GetComponent<CanvasGroup>().alpha = 1.0f;

            m_Sequence = DOTween.Sequence();
            m_Sequence.Append(FloatMoneyRect.DOJumpAnchorPos(new Vector2(FloatMoneyRect.anchoredPosition.x - fJumpDis, FloatMoneyRect.anchoredPosition.y), fJumpPower, 1, fJumpTime)
                .Insert(0, FloatMoneyRect.gameObject.GetComponent<CanvasGroup>().DOFade(0.0f, fFadeTime).SetDelay(fJumpTime))
                .OnComplete(SequenceComplete));
        }

        private void SequenceComplete()
        {
            if (m_Sequence != null)
            {
                m_Sequence.Kill();
                m_wnd.UnloadView();

                //ClearEffect();
                //UEffectPrefabType prefabType = UEffectPrefabType.UEPT_TeammateView_AddGold;
                //Vector3 pos = Vector3.zero;
                //m_effectParam = new UEffectPrefabParam(prefabType, FloatMoneyRect, pos, 0, false);
                //UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                //FloatMoneyRect.DOAnchorPos(new Vector2(-700.0f, 400.0f), fEffectTime).OnComplete(() => EffectComplete());
            }   
        }

        //private void EffectComplete()
        //{
        //    ClearEffect();
        //    SetVisible(false);
        //}

        //private void ClearEffect()
        //{
        //    if (m_effectParam != null)
        //    {
        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
        //    }
        //}
    }
}
