using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using DG.Tweening;
using UnityEngine.UI;
using USpeedUI.UEffect;

namespace USpeedUI.Sniperscope
{
    public class USniperscopeWnd : UIPopupWnd<USniperscopeView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_SNIPERSCOPE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/Sniperscope/SniperscopeView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, this);
            
            ImageSetting.OnSceneRenderingQualityChanged += onSceneRenderingQualityChanged;

            return base.Init();
        }
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, this);
            base.Destroy();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (cmdData != null)
                        {
                            SetVisible(Convert.ToBoolean(cmdData.nParam));
                            if(m_wndView != null)
                                m_wndView.SetFrontSightPos(DataCenter.WarFrontSightDataManager.Instance.FrontSightPos);
                        }
                    }
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (_bVisible)
                m_wndView.OpenSniperscope();
            else
                m_wndView.HideSniperscope();
        }

        private void onSceneRenderingQualityChanged(SceneRenderingQuality _newQuality, SceneRenderingQuality _oldQuality)
        {
            if(m_wndView != null)
            {
                m_wndView.onSceneRenderingQualityChanged(_newQuality, _oldQuality);
            }
        }
    }
    public class USniperscopeView : UIBaseWndView
    {
        public Vector3 DefaultScale = new Vector3(0.5f, 0.5f, 0.5f);
        public Vector3 TargetScale = Vector3.one;
        public float DefaultRotate = 90f;
        public float TargetRotate = 90f;
        public float RotateDuration = 0.2f;
        

        public RectTransform RotateLeftFrame = null;
        public RectTransform RotateRightFrame = null;

        [SerializeField]
        public Texture SniperscopeMaskSprite = null;
        [SerializeField]
        public Image SniperscopeImage = null;

        private UEffectParamBase m_materialEffectParam;

        public override bool Init(IUIWnd wnd)
        {
            m_materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, SniperscopeImage, true);
            if(UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref m_materialEffectParam))
            {
                (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetTexture("_AlphaTex", SniperscopeMaskSprite);
                (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.86f));
                (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_Vibrancy", 0f);

                onSceneRenderingQualityChanged(ImageSetting.GetData().currSceneRenderingQuality, SceneRenderingQuality.MAX);
            }


            HideSniperscope();
            return base.Init(wnd);
        }

        public bool bIsOpen = false;
        public bool bTest = false;
        public override void Update()
        {
            if(bTest)
            {
                if (bIsOpen)
                {
                    OpenSniperscope();
                    bIsOpen = false;
                }
                else
                {
                    HideSniperscope();
                    bIsOpen = true;
                }
                bTest = false;
            }

        }
        public void SetFrontSightPos(Vector2 pos)
        {
            GetTransform().anchoredPosition = pos;
        }
        public void OpenSniperscope()
        {
            GetTransform().DOScale(TargetScale, RotateDuration);
            RotateLeftFrame.DORotateQuaternion(Quaternion.Euler(0, 0, DefaultRotate + TargetRotate),RotateDuration+0.1f);
            RotateRightFrame.DORotateQuaternion(Quaternion.Euler(0, 0, -DefaultRotate + TargetRotate), RotateDuration+0.1f);
        }
        public void HideSniperscope()
        {
            GetTransform().localScale = DefaultScale;
            RotateLeftFrame.localRotation = (Quaternion.Euler(0, 0, DefaultRotate));
            RotateRightFrame.localRotation = (Quaternion.Euler(0, 0, -DefaultRotate));
        }

        internal void onSceneRenderingQualityChanged(SceneRenderingQuality _newQuality,SceneRenderingQuality _oldQuality)
        {
            if(_newQuality > SceneRenderingQuality.Good)
                (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.86f));
            else
                (m_materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0.5f, 0.5f, 0.5f, 0.86f));
        }
    }
}
