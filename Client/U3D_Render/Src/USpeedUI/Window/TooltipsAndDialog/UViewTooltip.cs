using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.TooltipAndDialog
{
    public class UMediatorTooltip : UIBaseWnd<UViewTooltip>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_TOOLTIPS;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        public override string GetPrefabPath()
        {
            return "Tooltips/TooltipView";
        }

        public override bool Init()
        {
            SetVisible(true);
            return base.Init();
        }
    }

    public class UViewTooltip : UIBaseWndView
    {
        public float TooltipDelay = 0.33f;
        public float FadeDuration = 0.2f;

        public override bool Init(IUIWnd wnd)
        {
            return base.Init(wnd);

        }

        public override void Start()
        {
            base.Start();

            UTooltipManager.Instance.Initialize(new UTooltipManagerInitializeParam(
                                                this.GetTransform(),UISystem.Instance.GetCamera(),
                                                UISystem.Instance.GetCanvas(), TooltipDelay, FadeDuration));

            UTooltipManager.Instance.TooltipMgrSetTextEvent += HelperTooltipSetText;

        }

        /// <summary>
        /// 为了不在 U3D_Framework里面调用U3D_Render方法，定了个委托来转换带UBB的提示
        /// </summary>
        /// <param name="_strTooltipText"></param>
        /// <returns></returns>
        public string HelperTooltipSetText(string _strTooltipText)
        {
            U3D_Render.Common.UBB.toHtml(ref _strTooltipText);
            return _strTooltipText;
        }

        public override void Destroy()
        {
            base.Destroy();
            UTooltipManager.Instance.Destroy();
            UTooltipManager.Instance.TooltipMgrSetTextEvent -= HelperTooltipSetText;

        }
    }
}
