using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UEffect;

namespace USpeedUI.CommonBlurBG
{
    public class CommonBlurBGWnd : UIBaseWnd<CommonBlurBGWndView>
    {
        public override WndID GetID()
        {
            return WndID.WMD_ID_COMMONBLURBG;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "CommomState/BlurBG/CommonBlurBGView";
        }

        public override bool Init()
        {
            base.Init();
            LoadView();
            return true;
        }
    }

    public class CommonBlurBGWndView : UIBaseWndView
    {
        public Image bgImg;

        private bool bInit = false;

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            if (!bInit && visible)
            {
                UEffectParamBase param = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, bgImg, true);

                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref param))
                {
                    (param as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.9f));
                }
                bInit = true;
            }

            //如果静态场景技能栏UI存在，则切换显示
            IUIWnd staticBottomCenterWnd = UISystem.Instance.GetWnd(WndID.WND_ID_STATICBOTTOMCENTER);
            if (staticBottomCenterWnd != null && staticBottomCenterWnd.GetWndView() != null)
                UISystem.Instance.ShowWnd(WndID.WND_ID_STATICBOTTOMCENTER, !visible);
        }
    }

}
