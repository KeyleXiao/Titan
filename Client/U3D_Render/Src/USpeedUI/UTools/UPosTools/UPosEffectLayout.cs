using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
    public class UPosEffectLayout : LayoutGroup
    {
        public EPosEffectType m_PosEffectType = EPosEffectType.PET_None;
        public EPosEffectType PosEffectType
        {
            get
            {
                return m_PosEffectType;
            }

            set
            {
                if(m_PosEffectType != value)
                {
                    if (PosEffect != null)
                        PosEffect.DestroyPosEffect();

                    m_PosEffectType = value;
                    ChangePosEffectType();
                }
            }
        }

        public IPosEffect PosEffect = null;

        protected void ChangePosEffectType()
        {
            switch (m_PosEffectType)
            {
                case EPosEffectType.PET_Curved:
                    PosEffect = new UPosEffect_Curved(this.rectTransform);
                    break;
                case EPosEffectType.PET_Radial:
                    PosEffect = new UPosEffect_Radial(this.rectTransform);
                    break;
                case EPosEffectType.PET_None:
                case EPosEffectType.PET_Max:
                default:
                    throw new NotImplementedException("不能实例化类型："+m_PosEffectType);
            }
        }

        protected override void OnEnable()
        {
            base.OnEnable();
        }
        public override void CalculateLayoutInputVertical()
        {
            PosEffect.CalculatePos();
        }

        public override void CalculateLayoutInputHorizontal()
        {
            PosEffect.CalculatePos();
        }

        public override void SetLayoutHorizontal()
        {

        }

        public override void SetLayoutVertical()
        {

        }
        protected override void OnRectTransformDimensionsChange()
        {
            PosEffect.RectTransformDimensionsChange();
            PosEffect.CalculatePos();
        }

#if UNITY_EDITOR
        protected override void OnValidate()
        {
            base.OnValidate();
            PosEffect.CalculatePos();
        }
#endif

    }
}
