/*******************************************************************
** 文件名:	Effect_Play.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	播放特效

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;


namespace Effect
{
    using TrailsSystem;

    /// <summary>
    /// 播放特效
    /// </summary>
    public class Effect_Play : MonoBehaviour
    {

        public bool IncludeChildren = true;

        public ParticleSystem[] m_ParticleSystem;
        public ParticleSystemRenderer[] m_ParticleSystemRenderer;
        public Effect_TrailRenderer_Base[] m_TrailRenderer_Base;
        public Projector[] m_effect_Projector;
        public MeshRenderer[] m_MeshRender = new MeshRenderer[0];
        public SkinnedMeshRenderer[] m_SkinnedMeshRender = new SkinnedMeshRenderer[0];
        public Light[] m_Lights = new Light[0];
        public bool bBaked = false;

        public ParticleSystemScalingMode[] oldScaleModel = new ParticleSystemScalingMode[0];
        public Animator[] m_Animator;
        public RuntimeAnimatorController[] m_RunTimeController = new RuntimeAnimatorController[0];
        public Vector3[] m_AimatorOrginalPos = new Vector3[0];
        public Vector3[] m_AimatorOrginalScale = new Vector3[0];

        /// <summary>
        /// 是否在等待其他
        /// </summary>
        private bool isWaitingOthers = false;

        private int currMask;

        bool bInit = false;
        Coroutine waitPlayOtherCor = null;

        public void Start()
        {
            Init();
        }

        public void SetScale(float SpeedScale, bool AffectByParent)
        {
            if(null == m_ParticleSystem)
            {
                return;
            }

            int cout = m_ParticleSystem.Length;
            if(cout != oldScaleModel.Length)
            {
                Debug.LogWarning("Scale信息以改变，请重新bake");
                return;
            }
            for (int i = 0; i < cout; i++)
            {
                ParticleSystem ps = m_ParticleSystem[i];
                if (ps)
                {
                    ps.playbackSpeed *= SpeedScale;

                    if (AffectByParent)
                    {
                        ps.scalingMode = ParticleSystemScalingMode.Hierarchy;
                    }
                    else
                    {
                        ps.scalingMode = oldScaleModel[i];
                    }
                }


            }
        }

        public void SetParticleSpeed(float Speed)
        {
            if (null == m_ParticleSystem)
            {
                return;
            }

            int cout = m_ParticleSystem.Length;
            for (int i = 0; i < cout; i++)
            {
                ParticleSystem ps = m_ParticleSystem[i];
                if (ps)
                {
                    ps.playbackSpeed = Speed;
                }
            }
        }

        public void OnDestroy()
        {
            oldScaleModel = null;
            m_TrailRenderer_Base = null;
            m_Animator = null;
            m_RunTimeController = null;
            m_effect_Projector = null;
            m_ParticleSystem = null;
            m_ParticleSystemRenderer = null;
            m_AimatorOrginalPos = null;
            m_SkinnedMeshRender = null;
            m_MeshRender = null;
            m_Lights = null;
        }

        public void Bake( System.Collections.Generic.List<Light> excLights)
        {
            if (IncludeChildren)
            {
                m_Animator = GetComponentsInChildren<Animator>();
                m_ParticleSystem = GetComponentsInChildren<ParticleSystem>();
                m_TrailRenderer_Base = GetComponentsInChildren<Effect_TrailRenderer_Base>();
                m_effect_Projector = GetComponentsInChildren<Projector>();
                m_SkinnedMeshRender = GetComponentsInChildren<SkinnedMeshRenderer>();
                m_MeshRender = GetComponentsInChildren<MeshRenderer>();
                m_Lights = GetComponentsInChildren<Light>();
            }
            else
            {
                m_Animator = GetComponents<Animator>();
                m_ParticleSystem = GetComponents<ParticleSystem>();
                m_TrailRenderer_Base = GetComponents<Effect_TrailRenderer_Base>();
                m_effect_Projector = GetComponents<Projector>();
                m_SkinnedMeshRender = GetComponents<SkinnedMeshRenderer>();
                m_MeshRender = GetComponents<MeshRenderer>();
                m_Lights = GetComponents<Light>();
            }

            int cout = m_ParticleSystem.Length;
            m_ParticleSystemRenderer = new ParticleSystemRenderer[cout];
            oldScaleModel = new ParticleSystemScalingMode[m_ParticleSystem.Length];
            for (int i = 0; i < cout;i++ )
            {
                ParticleSystem ps = m_ParticleSystem[i];
                ParticleSystemRenderer re = ps.GetComponent<ParticleSystemRenderer>();
                re.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;

                oldScaleModel[i] = ps.scalingMode;
                re.receiveShadows = false;
                ps.playOnAwake = false;
                m_ParticleSystemRenderer[i] = re;
                ps.Stop();
            }
            m_AimatorOrginalPos = new Vector3[m_Animator.Length];
            m_AimatorOrginalScale = new Vector3[m_Animator.Length];
            m_RunTimeController = new RuntimeAnimatorController[m_Animator.Length];
            cout = m_Animator.Length;
            for (int i = 0; i < cout; i++)
            {
                m_Animator[i].cullingMode = AnimatorCullingMode.AlwaysAnimate;
                m_RunTimeController[i] = m_Animator[i].runtimeAnimatorController;
                m_AimatorOrginalPos[i] = m_Animator[i].transform.localPosition;
                m_AimatorOrginalScale[i] = m_Animator[i].transform.localScale;
                m_Animator[i].enabled = false;
            }

            foreach (Projector proj in m_effect_Projector)
            {
                proj.ignoreLayers = Effect.EffectNode.ProjectorIngonreMask;
            }

            System.Collections.Generic.List<Light> correctedLights = new System.Collections.Generic.List<Light>();
            foreach (Light li in m_Lights)
            {
                if (!excLights.Contains(li))
                {
                    correctedLights.Add(li);
                }
            }

            m_Lights = null;
            m_Lights = correctedLights.ToArray();
            correctedLights = null;
            bBaked = true;
        }

        public void Init()
        {
            if (bInit || !bBaked)
            {
                return;
            }

            foreach (Effect_TrailRenderer_Base trial in m_TrailRenderer_Base)
            {
                if(trial)
                {
                    trial.ClearSystem(false);
                }
                
            }

            foreach (Projector pro in m_effect_Projector)
            {
                if(pro)
                {
                    pro.enabled = false;
                }
               
            }

            foreach (SkinnedMeshRenderer pro in m_SkinnedMeshRender)
            {
                if (pro)
                {
                    pro.enabled = false;
                    pro.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
                    pro.receiveShadows = false;
                }
            }

            foreach (MeshRenderer pro in m_MeshRender)
            {
                if (pro)
                {
                    pro.enabled = false;
                    pro.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
                    pro.receiveShadows = false;
                }
            }

            foreach (Light pro in m_Lights)
            {
                if (pro)
                {
                    pro.enabled = false;
                    pro.shadows = LightShadows.None;
                }
            }

            for (int i = 0; i < m_Animator.Length; i++)
            {
                if(m_Animator[i])
                {
                    m_Animator[i].enabled = false;
                    m_Animator[i].applyRootMotion = false;
                }

            }


            bInit = true;

        }

        public void PlayOnce(int mask)
        {
           
            if (!bInit)
            {
                Init();
            }
            currMask = mask;
            PlayerImpl(currMask);

        }

        private void PlayerImpl(int mask)
        {
            if(!bBaked)
            {
                return;
            }

            PlayImpl_Animator();

            if (!isWaitingOthers)
            {
                //其他的要在本帧末尾才开始，因为有的K了动画的，先让动画播一遍
                waitPlayOtherCor = GameUtil.StartCoroutineHelper(this,waitPlayOther());
                isWaitingOthers = true;
            }

        }

        private void PlayerImpl_Other(int mask)
        {

            int cout = m_ParticleSystem.Length;
            for (int i = 0; i < cout; i++)
            {
                ParticleSystem ps = m_ParticleSystem[i];
                if (ps)
                {
                    ParticleSystemRenderer re = m_ParticleSystemRenderer[i];
                    re.enabled = true;
                    ps.Simulate(0);
                    ps.Play(false);
                }

            }

            foreach (Effect_TrailRenderer_Base trial in m_TrailRenderer_Base)
            {
                if (trial)
                {
                    trial.Emit = true;
                }

            }

            foreach (Projector pro in m_effect_Projector)
            {
                if (pro)
                {
                    pro.enabled = true;
                }

            }
            foreach (SkinnedMeshRenderer pro in m_SkinnedMeshRender)
            {
                if (pro)
                {
                    pro.enabled = true;
                }
            }

            foreach (MeshRenderer pro in m_MeshRender)
            {
                if (pro)
                {
                    pro.enabled = true;
                }
            }


            foreach (Light pro in m_Lights)
            {
                if (pro)
                {
                    pro.enabled = true && ((currMask & (int)EffectNodeMask.Mask_EnableLihgt) != 0);
                }
            }
        }

        private void PlayImpl_Animator()
        {
            if (m_Animator.Length != m_RunTimeController.Length)
            {
                Debug.LogWarning("Animator信息以改变，请重新bake");
                return;
            }
            for (int i = 0; i < m_Animator.Length; i++)
            {
                if (m_Animator[i])
                {
                    m_Animator[i].enabled = true;
                    //重置一下
                    m_Animator[i].runtimeAnimatorController = null;
                    m_Animator[i].runtimeAnimatorController = m_RunTimeController[i];
                }
            }
        }

        public void Stop()
        {
            if (!bBaked)
            {
                return;
            }
            if(isWaitingOthers)
            {
                isWaitingOthers = false;
                if (null != waitPlayOtherCor)
                {
                    StopCoroutine(waitPlayOtherCor);
                }

            }
            int cout = m_ParticleSystem.Length;
            if (cout != oldScaleModel.Length)
            {
                Debug.LogWarning("Scale信息以改变，请重新bake");
            }
            else
            {
                for (int i = 0; i < cout; i++)
                {
                    ParticleSystem ps = m_ParticleSystem[i];
                    if (ps)
                    {
                        ParticleSystemRenderer re = m_ParticleSystemRenderer[i];
                        re.enabled = false;
                        ps.Stop(false);
                        ps.scalingMode = oldScaleModel[i];
                    }

                }
            }


            foreach (Effect_TrailRenderer_Base trial in m_TrailRenderer_Base)
            {
                if(trial)
                {
                    trial.ClearSystem(false);
                }

            }

            foreach (Projector pro in m_effect_Projector)
            {
                if(pro)
                {
                    pro.enabled = false;
                }

            }

            Animator temp;
            for (int i = 0; i < m_Animator.Length; i++)
            {
                temp = m_Animator[i];
                if(temp)
                {
                    temp.enabled = false;
                    temp.runtimeAnimatorController = null;
                    temp.transform.localPosition = m_AimatorOrginalPos[i];
                    temp.transform.localScale = m_AimatorOrginalScale[i];
                }

            }

            foreach (SkinnedMeshRenderer pro in m_SkinnedMeshRender)
            {
                if (pro)
                {
                    pro.enabled = false;
                }
            }

            foreach (MeshRenderer pro in m_MeshRender)
            {
                if (pro)
                {
                    pro.enabled = false;
                }
            }

            foreach (Light pro in m_Lights)
            {
                if (pro)
                {
                    pro.enabled = false;
                }
            }
        }

        IEnumerator waitPlayOther()
        {
            yield return new WaitForEndOfFrame();

            isWaitingOthers = false;
            PlayerImpl_Other(currMask);
        }
    }
}


