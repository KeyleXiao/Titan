/*******************************************************************
** 文件名:	EffectNode_Modules.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效节点
** 应  用:  	特效节点，模块相关

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

//粒子时长计算有问题，全部循环和动画问题
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ImageEffects;
namespace Effect
{
    /// <summary>
    /// 特效模块类，主要装一些特效功能脚本
    /// </summary>
    [System.Serializable]
    public class EffectModules
    {
        [HideInInspector]
        public Effect_Play[] m_effect_Play = null;
        [HideInInspector]
        public Effect_DealyPlay[] m_effect_DealyPlay = null;
        [HideInInspector]
        public Effect_UVScroll[] m_effect_UVScroll = null;
        [HideInInspector]
        public Effect_FadeInOutMaterialColor[] m_effect_fadematerialcolor = null;
        [HideInInspector]
        public Effect_FadeInOutMaterialFloat[] m_effect_fadematerialfloat = null;
        [HideInInspector]
        public Effect_AddMaterial[] m_effect_AddMaterial = null;
        [HideInInspector]
        public Effect_ReplaceMaterial[] m_effect_ReplaceMaterial = null;
        [HideInInspector]
        public Effect_UVTextureAnimator[] m_effect_UVTextureAnimator = null;
        [HideInInspector]
        public Effect_WeaponTrail[] m_effect_WeaponTrail = null;
        [HideInInspector]
        public Effect_WeaponTrail_Simplest[] m_effect_WeaponTrail_Simplest = null;
        [HideInInspector]
        public Effect_WeaponTrail_Simple[] m_effect_WeaponTrail_Simple = null;
        [HideInInspector]
        public Effect_VerticesModify[] m_effect_VerticesModify = null;
        [HideInInspector]
        public Effect_LineRender[] m_effect_LineRender = null;
        [HideInInspector]
        public Effect_FullScreen[] m_effect_FullScreen = null;

        [HideInInspector]
        public Effect_FollowObject[] m_effect_FollowObject = null;

        [HideInInspector]
        public Effect_Random3D[] m_effect_Random3D = new Effect_Random3D[0];

        [HideInInspector]
        public Effect_DisableObj[] m_effect_DisableObj = new Effect_DisableObj[0];

        [HideInInspector]
        public Effect_LocalMove[] m_effect_LocalMove = new Effect_LocalMove[0];

        [HideInInspector]
        public Effect_FadeInOutLightIntensity[] m_effect_fadelightIntensity = null;

        [HideInInspector]
        public Effect_ModifyParticlesShape[] m_effect_modifyparticlesshape = null;
        [HideInInspector]
        public Effect_MaterialGradientColor[] m_effect_materialgradientcolor = null;
        [HideInInspector]
        public Effect_TransformModify[] m_effect_transformmodify = null;
        [HideInInspector]
        public Effect_ProjectorModify[] m_effect_projectorModify = null;
        [HideInInspector]
        public Effect_CampColor[] m_effect_campColor = null;
        
        [HideInInspector]
        public Effect_Scale[] m_effect_scale = null;

        
        [HideInInspector]
        public bool isBaked = false;

        //public UVProjector[] m_effect_UVProjector = null;
        //public UVParticle[] m_effect_UVParticle = null;
        //public UVMesh[] m_effect_UVMesh = null;

        private EffectNode hostNode = null;
        /// <summary>
        /// 所有的渲染器
        /// </summary>
        [HideInInspector]
        public Renderer[] m_All_Renders = null;

        /// <summary>
        /// 没有被任何脚本操控的渲染器，被脚本操控的渲染器由各个脚本自己负责
        /// </summary>
        [HideInInspector]
        public List<Renderer> m_Normal_Renders =null;

        public void Bake(GameObject go)
        {
            
            m_effect_Play = go.GetComponentsInChildren<Effect_Play>();
            m_effect_DealyPlay = go.GetComponentsInChildren<Effect_DealyPlay>();
            //m_effect_UVProjector = go.GetComponentsInChildren<UVProjector>();
            //m_effect_UVParticle = go.GetComponentsInChildren<UVParticle>();
            //m_effect_UVMesh = go.GetComponentsInChildren<UVMesh>();
            m_effect_UVScroll = go.GetComponentsInChildren<Effect_UVScroll>();
            m_effect_fadematerialfloat = go.GetComponentsInChildren<Effect_FadeInOutMaterialFloat>();
            m_effect_fadematerialcolor = go.GetComponentsInChildren<Effect_FadeInOutMaterialColor>();
            m_effect_fadelightIntensity = go.GetComponentsInChildren<Effect_FadeInOutLightIntensity>();
            m_effect_AddMaterial = go.GetComponentsInChildren<Effect_AddMaterial>();
            m_effect_ReplaceMaterial = go.GetComponentsInChildren<Effect_ReplaceMaterial>();
            m_effect_UVTextureAnimator = go.GetComponentsInChildren<Effect_UVTextureAnimator>();
            m_effect_WeaponTrail = go.GetComponentsInChildren<Effect_WeaponTrail>();
            m_effect_WeaponTrail_Simplest = go.GetComponentsInChildren<Effect_WeaponTrail_Simplest>();
            m_effect_WeaponTrail_Simple = go.GetComponentsInChildren<Effect_WeaponTrail_Simple>();
            m_effect_VerticesModify = go.GetComponentsInChildren<Effect_VerticesModify>();
            m_effect_LineRender = go.GetComponentsInChildren<Effect_LineRender>();
            m_effect_FullScreen = go.GetComponentsInChildren<Effect_FullScreen>();
            m_effect_FollowObject = go.GetComponentsInChildren<Effect_FollowObject>();
            m_effect_Random3D = go.GetComponentsInChildren<Effect_Random3D>();
            m_effect_DisableObj = go.GetComponentsInChildren<Effect_DisableObj>();
            m_effect_LocalMove = go.GetComponentsInChildren<Effect_LocalMove>();
            m_effect_modifyparticlesshape = go.GetComponentsInChildren<Effect_ModifyParticlesShape>();
            m_effect_materialgradientcolor = go.GetComponentsInChildren<Effect_MaterialGradientColor>();
            m_effect_transformmodify = go.GetComponentsInChildren<Effect_TransformModify>();
            m_effect_campColor = go.GetComponentsInChildren<Effect_CampColor>();
            m_effect_projectorModify = go.GetComponentsInChildren<Effect_ProjectorModify>();
            m_effect_scale = go.GetComponentsInChildren<Effect_Scale>();
            foreach (Effect_Scale e in m_effect_scale)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Bake();
            }

            foreach (Effect_ProjectorModify e in m_effect_projectorModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Bake();
            }
            foreach (Effect_TransformModify e in m_effect_transformmodify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Bake();
            }

            Light[] m_All_Lights = go.GetComponentsInChildren<Light>();
            List<Light> m_ReferenceFromFadeLight = new List<Light>();

            foreach(Light lig in m_All_Lights)
            {
                if(isOnFadeLightController(lig))
                {
                    m_ReferenceFromFadeLight.Add(lig);
                }
            }

            foreach(Effect_Play p in m_effect_Play)
            {
                p.Bake(m_ReferenceFromFadeLight);
            }
            
            foreach (Effect_DealyPlay p in m_effect_DealyPlay)
            {
                p.Bake(m_ReferenceFromFadeLight);
            }
            foreach (Effect_FollowObject p in m_effect_FollowObject)
            {
                p.Bake();
            }
            m_ReferenceFromFadeLight.Clear();
            m_ReferenceFromFadeLight = null;
            m_All_Renders = go.GetComponentsInChildren<Renderer>();
            if (null == m_Normal_Renders)
            {
                m_Normal_Renders = new List<Renderer>();
            }
            m_Normal_Renders.Clear();
            foreach (Renderer r in m_All_Renders)
            {
                //看看有没有被AddMaterial脚本引用
                if (isOnAddMaterial(r))
                {
                    continue;
                }
                else
                {
                    //看看有没有被ReplaceMaterial脚本引用
                    if (isOnReplaceMaterial(r))
                    {
                        continue;
                    }
                    else
                    {
                        //有没有被材质控制器引用
                        if (isOnMaterialControl(r))
                        {
                            continue;
                        }
                        m_Normal_Renders.Add(r);
                    }
                }
            }
            isBaked = true;
        }

        public void Clear()
        {
            hostNode = null;
            m_effect_Play = null;
            m_effect_DealyPlay = null;
            m_effect_UVScroll = null;
            m_effect_fadematerialcolor = null;
            m_effect_fadematerialfloat = null;
            m_effect_fadelightIntensity = null;
            m_effect_AddMaterial = null;
            m_effect_ReplaceMaterial = null;
            m_effect_UVTextureAnimator = null;
            m_effect_WeaponTrail = null;
            m_effect_WeaponTrail_Simple = null;
            m_effect_WeaponTrail_Simplest = null;
            m_effect_VerticesModify = null;
            m_effect_LineRender = null;
            m_effect_FullScreen = null;
            m_effect_FollowObject = null;
            m_effect_Random3D = null;
            m_effect_DisableObj = null;
            m_All_Renders = null;
            m_Normal_Renders = null;
            m_effect_LocalMove = null;
            m_effect_materialgradientcolor = null;
            m_effect_transformmodify = null;
            m_effect_campColor = null;
            m_effect_scale = null;
        }

        public void Init(EffectNode host)
        {
            if (!isBaked)
            {
                return;
            }
            hostNode = host;
            foreach (Effect_Scale e in m_effect_scale)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();

            }

            foreach (Effect_DealyPlay e in m_effect_DealyPlay)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();

            }

            foreach (Effect_Play e in m_effect_Play)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();

            }

            foreach (Effect_CampColor e in m_effect_campColor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();

            }
            
            foreach (Effect_FollowObject e in m_effect_FollowObject)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_UVScroll e in m_effect_UVScroll)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();

            }
            foreach (Effect_AddMaterial e in m_effect_AddMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }
            foreach (Effect_ReplaceMaterial e in m_effect_ReplaceMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_WeaponTrail e in m_effect_WeaponTrail)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }


            foreach (Effect_WeaponTrail_Simple e in m_effect_WeaponTrail_Simple)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_WeaponTrail_Simplest e in m_effect_WeaponTrail_Simplest)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_VerticesModify e in m_effect_VerticesModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.HostEntityID = hostNode.EntityID;
                e.Init();
            }

            foreach (Effect_LineRender e in m_effect_LineRender)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.GolbalSrcObj = hostNode.SrcObj;
                e.GolbalDstObj = hostNode.DstObj;

                e.Init();
            }


            foreach (Effect_FullScreen e in m_effect_FullScreen)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }


            foreach (Effect_Random3D e in m_effect_Random3D)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_DisableObj e in m_effect_DisableObj)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_LocalMove e in m_effect_LocalMove)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_TransformModify e in m_effect_transformmodify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }

            foreach (Effect_ProjectorModify e in m_effect_projectorModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Init();
            }
        }

        public void SetParticleSpeed(float Speed)
        {
            foreach (Effect_Play e in m_effect_Play)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.SetParticleSpeed(Speed);
            }

            foreach (Effect_DealyPlay e in m_effect_DealyPlay)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.SetParticleSpeed(Speed);
            }
        }

        public void ResetScale()
        {
            foreach (Effect_Scale e in m_effect_scale)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.ResetScale();

            }
        }

        public void SetScale(float sizeScale)
        {
            foreach (Effect_Scale e in m_effect_scale)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.ApplyScale(sizeScale);
            }

        }

        public void Play(bool bEnemy,int mask)
        {
            if(!isBaked)
            {
                return;
            }

            foreach (Effect_ModifyParticlesShape e in m_effect_modifyparticlesshape)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play(hostNode.EntityID);
            }
            

            foreach (Effect_Play e in m_effect_Play)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.enabled = true;
                e.PlayOnce(mask);
            }
            foreach (Effect_DealyPlay e in m_effect_DealyPlay)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.enabled = true;
                e.Reset();
                e.Run(mask);
            }

            foreach (Effect_CampColor e in m_effect_campColor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play(bEnemy);

            }

            //foreach (UVProjector e in m_effect_UVProjector)
            //{
            //    e.Play();
            //}
            //foreach (UVParticle e in m_effect_UVParticle)
            //{
            //    e.Play();
            //}
            //foreach (UVMesh e in m_effect_UVMesh)
            //{
            //    e.Play();
            //}

            foreach (Effect_UVScroll e in m_effect_UVScroll)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play();
                }
            }

            foreach (Effect_FadeInOutMaterialFloat e in m_effect_fadematerialfloat)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play();
                }
            }
            
            foreach (Effect_FadeInOutMaterialColor e in m_effect_fadematerialcolor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play();
                }

            }

            foreach (Effect_FadeInOutLightIntensity e in m_effect_fadelightIntensity)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();

            }

            foreach (Effect_UVTextureAnimator e in m_effect_UVTextureAnimator)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play();
                }
            }

            foreach (Effect_WeaponTrail e in m_effect_WeaponTrail)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_WeaponTrail_Simple e in m_effect_WeaponTrail_Simple)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_WeaponTrail_Simplest e in m_effect_WeaponTrail_Simplest)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_AddMaterial e in m_effect_AddMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                //if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play(hostNode.EntityID);
                }
            }

            foreach (Effect_ReplaceMaterial e in m_effect_ReplaceMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                //if (e.RunMode == EffectRunMode.Self)
                {
                    e.Play(hostNode.EntityID);
                }
            }

            foreach (Effect_VerticesModify e in m_effect_VerticesModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.HostEntityID = hostNode.EntityID;
                e.Play();
            }

            foreach (Effect_LineRender e in m_effect_LineRender)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.GolbalSrcObj = hostNode.SrcObj;
                if(e.targetType == LineRenderTargetType.OutSiedePointed)
                {
                    e.GolbalDstObj = hostNode.DstObj;
                }
                else if(e.targetType == LineRenderTargetType.Self)
                {
                    e.GolbalDstObj = hostNode.transform;
                }

                e.Play();
            }

            foreach (Effect_FullScreen e in m_effect_FullScreen)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play(hostNode.EntityID);
            }

            foreach (Effect_FollowObject e in m_effect_FollowObject)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }


            foreach (Effect_Random3D e in m_effect_Random3D)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_DisableObj e in m_effect_DisableObj)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_LocalMove e in m_effect_LocalMove)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_MaterialGradientColor e in m_effect_materialgradientcolor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_TransformModify e in m_effect_transformmodify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }

            foreach (Effect_ProjectorModify e in m_effect_projectorModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Play();
            }
        }

        public void Stop()
        {
            if(!isBaked)
            {
                return;
            }
            foreach (Effect_Play e in m_effect_Play)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.enabled = false;
                e.Stop();
            }

            foreach (Effect_DealyPlay e in m_effect_DealyPlay)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.enabled = false;
                e.Reset();
            }

            foreach (Effect_CampColor e in m_effect_campColor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();

            }

            //foreach (UVProjector e in m_effect_UVProjector)
            //{
            //    e.Stop();
            //}
            //foreach (UVParticle e in m_effect_UVParticle)
            //{
            //    e.Stop();
            //}
            //foreach (UVMesh e in m_effect_UVMesh)
            //{
            //    e.Stop();
            //}


            foreach (Effect_UVScroll e in m_effect_UVScroll)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }
            }

            foreach (Effect_FadeInOutMaterialFloat e in m_effect_fadematerialfloat)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }

            }

            foreach (Effect_FadeInOutMaterialColor e in m_effect_fadematerialcolor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }
            }

            foreach (Effect_FadeInOutLightIntensity e in m_effect_fadelightIntensity)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();

            }

            foreach (Effect_UVTextureAnimator e in m_effect_UVTextureAnimator)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }
            }


            foreach (Effect_WeaponTrail e in m_effect_WeaponTrail)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_WeaponTrail_Simple e in m_effect_WeaponTrail_Simple)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_WeaponTrail_Simplest e in m_effect_WeaponTrail_Simplest)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_AddMaterial e in m_effect_AddMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                //if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }
            }


            foreach (Effect_ReplaceMaterial e in m_effect_ReplaceMaterial)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                //if (e.RunMode == EffectRunMode.Self)
                {
                    e.Stop();
                }
            }

            foreach (Effect_VerticesModify e in m_effect_VerticesModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }


            foreach (Effect_LineRender e in m_effect_LineRender)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_FullScreen e in m_effect_FullScreen)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_FollowObject e in m_effect_FollowObject)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_Random3D e in m_effect_Random3D)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_DisableObj e in m_effect_DisableObj)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_LocalMove e in m_effect_LocalMove)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_MaterialGradientColor e in m_effect_materialgradientcolor)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_TransformModify e in m_effect_transformmodify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }

            foreach (Effect_ProjectorModify e in m_effect_projectorModify)
            {
                if (!e)
                {
                    Debug.LogWarning(hostNode.name + ",bake信息已改变，请重新点击bake");
                    continue;
                }
                e.Stop();
            }
        }

        public void ForceDeleteTestModel()
        {
            foreach (Effect_AddMaterial e in m_effect_AddMaterial)
            {
                if (!e)
                {
                    continue;
                }

                e.ForceDeleteTestModel();

            }

            foreach (Effect_ReplaceMaterial e in m_effect_ReplaceMaterial)
            {
                if (!e)
                {
                    continue;
                }

                e.ForceDeleteTestModel();
            }
        }

        #region 渲染器检测
        /// <summary>
        /// 这个渲染器是否被AddMaterial引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnAddMaterial(Renderer r)
        {
            if (m_effect_AddMaterial.Length > 0)
            {
                foreach (Effect_AddMaterial addMaterial in m_effect_AddMaterial)
                {
                    Renderer[] res = addMaterial.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 是否被灯光控制器引用
        /// </summary>
        /// <param name="lig"></param>
        /// <returns></returns>
        bool isOnFadeLightController(Light lig)
        {
            if(m_effect_fadelightIntensity.Length > 0)
            {
                foreach(Effect_FadeInOutLightIntensity li in m_effect_fadelightIntensity)
                {
                    if(li.HostLight == lig)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 这个渲染器是否被ReplaceMaterial引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnReplaceMaterial(Renderer r)
        {
            if (m_effect_ReplaceMaterial.Length > 0)
            {
                foreach (Effect_ReplaceMaterial ReplaceMaterial in m_effect_ReplaceMaterial)
                {
                    Renderer[] res = ReplaceMaterial.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }


        bool isOnMaterialControl(Renderer r)
        {
            if (!isOnUVScroll(r))
            {
                if (!isOnFadeInOutMaterialFloat(r))
                {
                    if (!isOnFadeInOutMaterialColor(r))
                    {
                        if (!isOnUVTextureAnimator(r))
                        {
                            if (!isOnMaterialGradientColor(r))
                            {
                                return false;
                            }
                        }
                    }
                }
            }

            return true;
        }


        /// <summary>
        /// 这个渲染器是否被UVScroll引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnUVScroll(Renderer r)
        {
            if (m_effect_UVScroll.Length > 0)
            {
                foreach (Effect_UVScroll ef in m_effect_UVScroll)
                {
                    Renderer[] res = ef.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }


        /// <summary>
        /// 这个渲染器是否被isOnFadeInOutMaterialColor引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnUVTextureAnimator(Renderer r)
        {
            if (m_effect_UVTextureAnimator.Length > 0)
            {
                foreach (Effect_UVTextureAnimator ef in m_effect_UVTextureAnimator)
                {
                    Renderer[] res = ef.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }


        /// <summary>
        /// 这个渲染器是否被FadeInOutMaterialFloat引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnFadeInOutMaterialFloat(Renderer r)
        {
            if (m_effect_fadematerialfloat.Length > 0)
            {
                foreach (Effect_FadeInOutMaterialFloat ef in m_effect_fadematerialfloat)
                {
                    Renderer[] res = ef.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 这个渲染器是否被FadeInOutMaterialFloat引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnMaterialGradientColor(Renderer r)
        {
            if (m_effect_fadematerialfloat.Length > 0)
            {
                foreach (Effect_MaterialGradientColor ef in m_effect_materialgradientcolor)
                {
                    Renderer[] res = ef.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// 这个渲染器是否被isOnFadeInOutMaterialColor引用
        /// </summary>
        /// <param name="r"></param>
        /// <returns></returns>
        bool isOnFadeInOutMaterialColor(Renderer r)
        {
            if (m_effect_fadematerialcolor.Length > 0)
            {
                foreach (Effect_FadeInOutMaterialColor ef in m_effect_fadematerialcolor)
                {
                    Renderer[] res = ef.GetRendersOnBakeTime();
                    if (res.Length > 0)
                    {
                        foreach (Renderer re in res)
                        {
                            if (re == r)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        #endregion
    }

    /// <summary>
    /// 特效节点
    /// </summary>
    public partial class EffectNode : EffectNodeBase
    {

        
        //public static EffectQuality QualityLevel = EffectQuality.QUALITY_GOOD;
        //private static EffectQuality OldQualityLevel = EffectQuality.QUALITY_GOOD;

        public GameObject BestObj = null;
        public GameObject HighObj = null;
        public GameObject GoodObj = null;
        public GameObject FastestObj = null;


        [HideInInspector]
        public EffectModules BestModules = null;
        [HideInInspector]
        public EffectModules HighModlues = null;
        [HideInInspector]
        public EffectModules GoodModlues = null;
        [HideInInspector]
        public EffectModules FastestModlues = null;

        /// <summary>
        /// 静态特效等级改变
        /// </summary>
        public override void OnStaticEffectQualityChange()
        {
            if (!isStaticEffect || !gameObject || !gameObject.activeInHierarchy)
            {
                return;
            }
            StopEffect();
            PlayEffect(isEnemy, -1, Vector3.zero, Vector3.zero);
        }

        #region 光效击中事件

        public System.Action<Collider> hitEvent = null;
        public void OnTriggerEnter(Collider collider)
        {
            if (hitEvent != null)
            {
                hitEvent(collider);
            }
        }
        #endregion

        #region 目标物体缓存
        private static Queue<GameObject> m_UnUsedEmptyGameObject = new Queue<GameObject>();
        private static Queue<GameObject> m_AllEmptyGameObject = new Queue<GameObject>();
        private static GameObject EffectTargetCacheRoot = null;


        public static void InitTargetCache()
        {
            if(!EffectTargetCacheRoot)
            {
                EffectTargetCacheRoot = new GameObject();
                EffectTargetCacheRoot.name = "EffectTargetCacheRoot";
                GameObject.DontDestroyOnLoad(EffectTargetCacheRoot);
            }

            for(int i = 0;i < 50;i++)
            {
                GameObject go = new GameObject();
                go.hideFlags = HideFlags.HideAndDontSave;
                go.SetActive(false);
                go.transform.SetParent(EffectTargetCacheRoot.transform);
                m_UnUsedEmptyGameObject.Enqueue(go);
                m_AllEmptyGameObject.Enqueue(go);
            }
        }

        public static void ClearTargetCache()
        {
            m_UnUsedEmptyGameObject.Clear();
            foreach(GameObject go in m_AllEmptyGameObject)
            {
                GameObject.DestroyImmediate(go);
            }
            GameObject.DestroyImmediate(EffectTargetCacheRoot);
        }

        public static GameObject getTargetCache()
        {
            GameObject go = null;
            if(m_UnUsedEmptyGameObject.Count > 0)
            {
                go = m_UnUsedEmptyGameObject.Dequeue();
                go.SetActive(true);
            }
            else
            {
                go = new GameObject();
                go.hideFlags = HideFlags.HideAndDontSave;
                m_AllEmptyGameObject.Enqueue(go);
                
            }
            return go;
        }

        public static void CacheTarget(GameObject go)
        {
            if (!go || !EffectTargetCacheRoot)
            {
                return;
            }
            go.SetActive(false);

            go.transform.SetParent(EffectTargetCacheRoot.transform);
            m_UnUsedEmptyGameObject.Enqueue(go);
        }
        #endregion
    }
}



