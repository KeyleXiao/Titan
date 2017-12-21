using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UEffect
{
    [Serializable]
    public class UEffectNodeData_Prefab : UEffectNodeData
    {
        [NonSerialized]
        public UEffectPrefabType EffectType;
        [NonSerialized]
        //效果在该类型里面的id
        public int EffectPrefabTypeID;

        public UEffectNodeData_Prefab(UEffectNode _node):base(_node)
        {
            
        }

        public override void DoAwake()
        {
            
        }

        public override void DoOnDestroy()
        {
            UEffectManager.Instance.UnRegisterEffectPrefabNode(GetNode(), EffectType);
        }

        public override void DoOnDisable()
        {

        }

        public override void DoOnEnable()
        {

        }

        public override void DoRegisterEffectNode()
        {

        }

        public override void DoStart()
        {

        }

        public override void DoUnRegisterEffectNode()
        {

        }

        public override void DoUpdate()
        {
            Prefab_Update_AutoDestroy();
        }

        #region Particles
        /// <summary>
        /// 用于判断是否可以自动移除效果
        /// </summary>
        protected bool m_Prefab_AutoDestroy;
        /// <summary>
        /// 效果下的粒子系统列表
        /// </summary>
        protected List<ParticleSystem> m_Prefab_ListParticles;

        public void InitParticles()
        {
            if(m_EffectNode != null)
                m_Prefab_ListParticles = this.m_EffectNode.GetComponentsInChildren<ParticleSystem>().ToList();
        }
        public void SetAutoDestroy()
        {
            m_Prefab_AutoDestroy = true;
        }
        /// <summary>
        ///检测是否有循环播放的粒子系统，有的话不能自动移除
        /// </summary>
        /// <returns></returns>
        public bool IsLoopEffect()
        {
            return m_Prefab_ListParticles.Where(x => { return x.loop; }).Count() > 0;
        }
        public void ReplayEffect()
        {
            if (m_Prefab_ListParticles == null)
                return;
            foreach(ParticleSystem ps in m_Prefab_ListParticles)
            {
                ps.Play();
            }
        }

        public void StopEffect()
        {
            if (m_Prefab_ListParticles == null)
                return;
            foreach (ParticleSystem ps in m_Prefab_ListParticles)
            {
                ps.Stop();
            }
        }
        private void Prefab_Update_AutoDestroy()
        {
            if (m_Prefab_AutoDestroy)
            {
                bool allStopped = true;
                foreach (ParticleSystem ps in m_Prefab_ListParticles)
                {
                    if (ps.particleCount > 0)
                    {
                        allStopped = false;
                    }
                }
                if (allStopped)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref GetNode().NodeParam);
                }
                     
            }
        }


        #endregion
    }
    partial class UEffectNode
    {

    }
}
