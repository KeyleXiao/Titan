using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI.UWidgets;
using UnityEngine;

namespace USpeedUI.UEffect
{
    /// <summary>
    /// UI效果类型基类，然而多态对序列化不友好支持
    /// </summary>
    [Serializable]
    public class UEffectNodeData
    {
        [SerializeField]
        protected UEffectNode m_EffectNode;
        protected UEffectNode GetNode() { return m_EffectNode; }

        /// <summary>
        /// 是否已经进行注册
        /// </summary>
        protected bool m_bIsRegister;

        public UEffectNodeData(UEffectNode _node)
        {
            m_EffectNode = _node;
        }

        virtual public void DoAwake(){}
        virtual public void DoStart() {}
        virtual public void DoOnDestroy() {}
        virtual public void DoOnEnable() {}
        virtual public void DoOnDisable() {}
        virtual public void DoUpdate() {}
        virtual public void DoRegisterEffectNode() {}
        virtual public void DoUnRegisterEffectNode() {}
    }

    public partial class UEffectNode : UCustomUIBehaviour
    {
        [System.NonSerialized]
        public UEffectParamBase NodeParam = null;

        /// <summary>
        /// 效果类型
        /// </summary>
        [SerializeField]
        protected UEffectType m_EffectType;
        public UEffectType EffectType
        {
            get { return m_EffectType; }
            set
            {
                if (m_EffectType != value)
                {
                    DoOnDisable();
                    DoOnDestroy();
                    m_EffectType = value;
                    CreateEffectNodeDate();
                    DoAwake();
                    DoOnEnable();
                }
            }
        }

        private void CreateEffectNodeDate()
        {
            m_EffectNodeMesh = null;
            m_EffectNodePrefab = null;

            switch (m_EffectType)
            {
                case UEffectType.UET_EffectPrefab:
                    m_EffectNodePrefab = new UEffectNodeData_Prefab(this);
                    return;
                case UEffectType.UET_EffectMesh:
                    m_EffectNodeMesh = new UEffectNodeData_Mesh(this);
                    return;
                case UEffectType.UET_EffectMaterial:
                    m_EffectNodeMaterial = new UEffectNodeData_Material(this);
                    return;
            }
        }
            
        public UEffectNodeData EffectNodeData
        {
            get
            {
                switch (EffectType)
                {
                    case UEffectType.UET_EffectPrefab:
                        return m_EffectNodePrefab;
                    case UEffectType.UET_EffectMesh:
                        return m_EffectNodeMesh;
                    case UEffectType.UET_EffectMaterial:
                        return m_EffectNodeMaterial;
                    default:
                        return null;
                }
                
            }
        }
        
        [SerializeField]
        protected UEffectNodeData_Mesh m_EffectNodeMesh = null;
        [SerializeField]
        protected UEffectNodeData_Prefab m_EffectNodePrefab = null;
        [SerializeField]
        protected UEffectNodeData_Material m_EffectNodeMaterial = null;


        protected override void Awake()
        {
            base.Awake();
            DoAwake();
        }
        private void DoAwake()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoAwake();
            }
        }

        protected override void Start()
        {
            base.Start();
            DoStart();
        }
        private void DoStart()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoStart();
            }
        }

        protected override void OnDestroy()
        {
            DoOnDestroy();
            base.OnDestroy();
        }
        private void DoOnDestroy()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoOnDestroy();
            }
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            DoOnEnable();
        }
        private void DoOnEnable()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoOnEnable();
            }
        }

        protected override void OnDisable()
        {
            base.OnDisable();
            DoOnDisable();
        }
        private void DoOnDisable()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoOnDisable();
            }
        }

        public void Update()
        {
            DoUpdate();
        }
        private void DoUpdate()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoUpdate();
            }
        }

        public void RegisterEffectNode()
        {
            DoRegisterEffectNode();
        }
        private void DoRegisterEffectNode()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoRegisterEffectNode();
            }
        }

        public void UnRegisterEffectNode()
        {
            DoUnRegisterEffectNode();
        }
        private void DoUnRegisterEffectNode()
        {
            if(EffectNodeData != null)
            {
                EffectNodeData.DoUnRegisterEffectNode();
            }
        }

        public T To<T>()where T:UEffectNodeData
        {
            return (T)EffectNodeData;
        }
    }
}
