using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
    [Serializable]
    public class UEffectNodeData_Material : UEffectNodeData
    {
        [SerializeField]
        protected UEffectMaterialParam m_MaterialParam = new UEffectMaterialParam();
        public UEffectMaterialParam MaterialParam
        {
            get { return m_MaterialParam; }
            set { m_MaterialParam = value; }
        }

        public UEffectMaterialType MateralType
        {
            get { return m_MaterialParam.MaterialType; }
            set
            {
                DoUnRegisterEffectNode();
                m_MaterialParam.MaterialType = value;
                DoRegisterEffectNode();
            }
        }

        public UEffectNodeData_Material(UEffectNode _node) : base(_node)
        { }
        public override void DoAwake()
        {
            if(MaterialParam.GraphicsTarget == null)
            {
                var graphic = GetNode().GetComponent<Graphic>();
                if(graphic == null)
                {
                    graphic = GetNode().gameObject.AddComponent<Image>();
                }
                MaterialParam.GraphicsTarget = graphic;
            }
        }

        public override void DoOnDestroy()
        {
            DoUnRegisterEffectNode();
        }

        public override void DoOnDisable()
        {
            DoUnRegisterEffectNode();
        }

        public override void DoOnEnable()
        {
            DoRegisterEffectNode();
        }

        public override void DoRegisterEffectNode()
        {
            if (m_bIsRegister)
                return;

            UEffectParamBase param = m_MaterialParam as UEffectParamBase;
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref param);
            m_bIsRegister = true;
        }

        public override void DoStart()
        {
            DoRegisterEffectNode();
        }

        public override void DoUnRegisterEffectNode()
        {
            if (!m_bIsRegister)
                return;

            UEffectParamBase param = m_MaterialParam as UEffectParamBase;
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectMaterial, ref param);

            m_bIsRegister = false;
        }

        public override void DoUpdate()
        {
        }
    }

    partial class UEffectNode
    {

    }

}