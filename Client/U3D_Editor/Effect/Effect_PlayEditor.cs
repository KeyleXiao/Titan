/*******************************************************************
** 文件名:	Effect_PlayEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	EffectNode编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
using ImageEffects;
using U3D_Editor.Effect.DecalSystem;

namespace Effect
{
    public class EffectCheckMoudules
    {
        //粒子系统数目
        public List<ParticleSystem> ParticlesObjList;
        public const float ParticlesWeight = 0.5f;
        public const string ParticlesDesc = "粒子系统数目";

        //发射模型粒子系统数目
        public List<ParticleSystem> MeshParticlesObjList;
        public const float MeshParticlesWeight = 0.2f;
        public const string MeshParticlesDesc = "发射模型的粒子系统数目";

        //发射模型粒子系统的树木，面数超500
        public List<ParticleSystem> Over500MeshParticlesCountObjList;
        public const float Over500MeshParticlesCountObjWeight = 0.5f;
        public const string Over500MeshParticlesDesc = "发射的模型粒子面超过500";

        //发射模型粒子系统的树木，面数超1000
        public List<ParticleSystem> Over1000MeshParticlesCountObjList;
        public const float Over1000MeshParticlesCountObjWeight = 1.0f;
        public const string Over1000MeshParticlesDesc = "发射的模型粒子面超过1000";

        //发射模型粒子系统的树木，面数超1500
        public List<ParticleSystem> Over1500MeshParticlesCountObjList;
        public const float Over1500MeshParticlesCountObjWeight = 1.5f;
        public const string Over1500MeshParticlesDesc = "发射的模型粒子面超过1500";


        //发射模型粒子系统的树木，面数超3000
        public List<ParticleSystem> Over3000MeshParticlesCountObjList;
        public const float Over3000MeshParticlesCountObjWeight = 3.0f;
        public const string Over3000MeshParticlesDesc = "发射的模型粒子面超过3000";

        //超过100的粒子系统数目
        public List<ParticleSystem> Over100ParticlesObjList;
        public const float Over100ParticlesWeight = 0.5f;
        public const string Over100ParticlesDesc = "最大值超过100粒子系统数目";

        //超多200的粒子系统数目
        public List<ParticleSystem> Over200ParticlesObjList;
        public const float Over200ParticlesWeight = 1.0f;
        public const string Over200ParticlesDesc = "最大值超过200的粒子系统数目";

        //超多500的粒子系统数目
        public List<ParticleSystem> Over500ParticlesObjList;
        public const float Over500ParticlesWeight = 1.5f;
        public const string Over500ParticlesDesc = "最大值超过500的粒子系统数目";

        //超多1000的粒子系统数目
        public List<ParticleSystem> Over1000ParticlesObjList;
        public const float Over1000ParticlesWeight = 2.0f;
        public const string Over1000ParticlesDesc = "最大值超过1000的粒子系统数目";

        //静态模型数目
        public List<MeshRenderer> MeshRenderObjList;
        public const float MeshRenderWeight = 0.2f;
        public const string MeshRenderDesc = "静态网格数目";

        //动画模型数目
        public List<SkinnedMeshRenderer> SkinnedMeshRendererObjList;
        public const float SkinnedMeshRendererWeight = 0.5f;
        public const string SkinnedMeshRendererDesc = "动画网格数目";

        //Glow/溶解/描边材质球数目
        public List<GameObject> GlowOrDissloveOrOutLineMaterialObjList;
        public const float GlowOrDissloveOrOutLineMaterialWeight = 0.5f;
        public const string GlowOrDissloveOrOutLineMaterialDesc = "Glow/溶解发光/描边数目";

        //扭曲数目
        public List<GameObject> DistronList;
        public const float DistronWeight = 3.0f;
        public const string DistronDesc = "扭曲数目";

        //低级扭曲数目
        public List<GameObject> LowDistronList;
        public const float LowDistronWeight = 1.0f;
        public const string LowDistronDesc = "低级扭曲数目";

        //standrad类材质
        public List<GameObject> StandradList;
        public const float LowStandradWeight = 0.5f;
        public const string StandradDesc = "Standrad材质数目";
        
        //增加材质球脚本数目
        public List<Effect_AddMaterial> AddMaterialScriptList;
        public const float AddMaterialScriptWeight = 5.0f;
        public const string AddMaterialScriptDesc = "增加材质球数目";

        //替换材质球脚本数目
        public List<Effect_ReplaceMaterial> ReplaceMaterialScriptList;
        public const float ReplaceMaterialScriptWeight = 1.0f;
        public const string ReplaceMaterialScriptDesc = "替换材质球数目";

        //拖尾数目
        public List<Effect.TrailsSystem.Effect_TrailRenderer_Base> TrialRenderList;
        public const float TrialRenderWeight = 3.0f;
        public const string TrialRenderDesc = "拖尾数目";

        //武器拖尾数目
        public List<Effect.Effect_WeaponTrail> WeaponTrialList;
        public const float WeaponTrialWeight = 2.0f;
        public const string WeaponTrialDesc = "武器拖尾数目";

        //武器拖尾数目
        public List<Effect.Effect_WeaponTrail_Simple> Weapon_SimpleTrialList;
        public const float Weapon_SimpleTrialWeight = 1.0f;
        public const string Weapon_SimpleTrialDesc = "Simple武器拖尾数目";

        //武器拖尾数目
        public List<Effect.Effect_WeaponTrail_Simplest> Weapon_SimplestTrialList;
        public const float Weapon_SimplestTrialWeight = 0.5f;
        public const string Weapon_SimplestTrialDesc = "Simplest武器拖尾数目";

        //条带数目
        public List<Effect_LineRender> LineRenderList;
        public const float LineRenderWeight = 0.1f;
        public const string LineRenderDesc = "条带数目";

        //全屏数目
        public List<Effect_FullScreen> FullScreenList;
        public const float FullScreenWeight = 0.0f;
        public const string FullScreenDesc = "全屏数目";

        //特效位移数目
        public List<Effect_LocalMove> LocalMoveList;
        public const float LocalMoveWeight = 0.05f;
        public const string LocalMoveDesc = "特效位移数目";

        //高级冰数目
        public List<GameObject> IceBestList;
        public const float IceBestWeight = 4.0f;
        public const string IceBestDesc = "高级冰材质";

        //中等冰数目
        public List<GameObject> IceGoodList;
        public const float IceGoodWeight = 3.5f;
        public const string IceGoodDesc = "中等冰材质";


        //简单冰数目
        public List<GameObject> IceSimpleList;
        public const float IceSimpleWeight = 0.1f;
        public const string IceSimpleDesc = "简单的冰材质";

        //高级水数目
        public List<GameObject> WaterBestList;
        public const float WaterBestWeight = 4.0f;
        public const string WaterBestDesc = "高级水材质";

        //中等水数目
        public List<GameObject> WaterGoodList;
        public const float WaterGoodWeight = 3.5f;
        public const string WaterGoodDesc = "中等水材质";


        //简单水数目
        public List<GameObject> WaterSimpleList;
        public const float WaterSimpleWeight = 0.1f;
        public const string WaterSimpleDesc = "简单的水材质";

        //灯
        public List<Light> LightList;
        public const float LightWeight = 50.0f;
        public const string LightDesc = "灯光";

        //Rim法线材质
        public List<GameObject> Bump_RimList;
        public const float Bump_RimWeight = 0.5f;
        public const string Bump_RimDesc = "法线Rim";

        //流光材质
        public List<GameObject> FlowLightList;
        public const float FlowLightWeight = 0.5f;
        public const string FlowLightDesc = "流光材质";

        //动画
        public List<Animator> AnimatorList;
        public const float AnimatorWeight = 0.15f;
        public const string AnimatorDesc = "动画控制器";

        public void Init()
        {
            ParticlesObjList = null;
            ParticlesObjList = new List<ParticleSystem>();
            MeshParticlesObjList = null;
            MeshParticlesObjList = new List<ParticleSystem>();
            Over100ParticlesObjList = null;
            Over100ParticlesObjList = new List<ParticleSystem>();
            Over200ParticlesObjList = null;
            Over200ParticlesObjList = new List<ParticleSystem>();
            Over500ParticlesObjList = null;
            Over500ParticlesObjList = new List<ParticleSystem>();
            Over1000ParticlesObjList = null;
            Over1000ParticlesObjList = new List<ParticleSystem>();
            MeshRenderObjList = null;
            MeshRenderObjList = new List<MeshRenderer>();
            SkinnedMeshRendererObjList = null;
            SkinnedMeshRendererObjList = new List<SkinnedMeshRenderer>();
            GlowOrDissloveOrOutLineMaterialObjList = null;
            GlowOrDissloveOrOutLineMaterialObjList = new List<GameObject>();
            DistronList = null;
            DistronList = new List<GameObject>();
            AddMaterialScriptList = null;
            AddMaterialScriptList = new List<Effect_AddMaterial>();
            ReplaceMaterialScriptList = null;
            ReplaceMaterialScriptList = new List<Effect_ReplaceMaterial>();
            TrialRenderList = null;
            TrialRenderList = new List<Effect.TrailsSystem.Effect_TrailRenderer_Base>();
            WeaponTrialList = null;
            WeaponTrialList = new List<Effect.Effect_WeaponTrail>();
            Weapon_SimpleTrialList = null;
            Weapon_SimpleTrialList = new List<Effect.Effect_WeaponTrail_Simple>();
            Weapon_SimplestTrialList = null;
            Weapon_SimplestTrialList = new List<Effect.Effect_WeaponTrail_Simplest>();
            LineRenderList = null;
            LineRenderList = new List<Effect_LineRender>();
            FullScreenList = null;
            FullScreenList = new List<Effect_FullScreen>();
            LocalMoveList = null;
            LocalMoveList = new List<Effect_LocalMove>();

            IceBestList = null;
            IceBestList = new List<GameObject>();

            IceGoodList = null;
            IceGoodList = new List<GameObject>();

            IceSimpleList = null;
            IceSimpleList = new List<GameObject>();

            WaterBestList = null;
            WaterBestList = new List<GameObject>();

            WaterGoodList = null;
            WaterGoodList = new List<GameObject>();


            WaterSimpleList = null;
            WaterSimpleList = new List<GameObject>();

            LightList = null;
            LightList = new List<Light>();


            Bump_RimList = null;
            Bump_RimList = new List<GameObject>();

            
            FlowLightList = null;
            FlowLightList = new List<GameObject>();

            LowDistronList = null;
            LowDistronList = new List<GameObject>();

             Over500MeshParticlesCountObjList = null;
             Over500MeshParticlesCountObjList = new List<ParticleSystem>();
             Over1000MeshParticlesCountObjList = null;
             Over1000MeshParticlesCountObjList = new List<ParticleSystem>();
             Over1500MeshParticlesCountObjList = null;
             Over1500MeshParticlesCountObjList = new List<ParticleSystem>();
             Over3000MeshParticlesCountObjList = null;
             Over3000MeshParticlesCountObjList = new List<ParticleSystem>();

             AnimatorList = null;
             AnimatorList = new List<Animator>();

             StandradList = null;
             StandradList = new List<GameObject>();
        }

        public float GetUnit()
        {
            float curr = 0.0f;
            curr += ParticlesObjList.Count * ParticlesWeight;
            curr += MeshParticlesObjList.Count * MeshParticlesWeight;
            curr += Over100ParticlesObjList.Count * Over100ParticlesWeight;
            curr += Over200ParticlesObjList.Count * Over200ParticlesWeight;
            curr += Over500ParticlesObjList.Count * Over500ParticlesWeight;
            curr += Over1000ParticlesObjList.Count * Over1000ParticlesWeight;
            curr += MeshRenderObjList.Count * MeshRenderWeight;
            curr += SkinnedMeshRendererObjList.Count * SkinnedMeshRendererWeight;
            curr += GlowOrDissloveOrOutLineMaterialObjList.Count * GlowOrDissloveOrOutLineMaterialWeight;
            curr += DistronList.Count * DistronWeight;
            curr += AddMaterialScriptList.Count * AddMaterialScriptWeight;
            curr += ReplaceMaterialScriptList.Count * ReplaceMaterialScriptWeight;
            curr += TrialRenderList.Count * TrialRenderWeight;
            curr += WeaponTrialList.Count * WeaponTrialWeight;
            curr += Weapon_SimpleTrialList.Count * Weapon_SimpleTrialWeight;
            curr += Weapon_SimplestTrialList.Count * Weapon_SimplestTrialWeight;
            curr += LineRenderList.Count * LineRenderWeight;
            curr += FullScreenList.Count * FullScreenWeight;
            curr += LocalMoveList.Count * LocalMoveWeight;

            curr += IceBestList.Count * IceBestWeight;
            curr += IceGoodList.Count * IceGoodWeight;
            curr += IceSimpleList.Count * IceSimpleWeight;
            curr += WaterBestList.Count * WaterBestWeight;
            curr += WaterGoodList.Count * WaterGoodWeight;
            curr += WaterSimpleList.Count * WaterSimpleWeight;

            curr += LightList.Count * LightWeight;
            curr += Bump_RimList.Count * Bump_RimWeight;

            curr += FlowLightList.Count * FlowLightWeight;

            curr += LowDistronList.Count * LowDistronWeight;

            curr += StandradList.Count * LowStandradWeight;
            //curr += AnimatorList.Count * AnimatorWeight;
            foreach (ParticleSystem pas in Over500MeshParticlesCountObjList)
            {
                curr += Over500MeshParticlesCountObjWeight * pas.maxParticles;
            }

            foreach (ParticleSystem pas in Over1000MeshParticlesCountObjList)
            {
                curr += Over1000MeshParticlesCountObjWeight * pas.maxParticles;
            }
            foreach (ParticleSystem pas in Over1500MeshParticlesCountObjList)
            {
                curr += Over1500MeshParticlesCountObjWeight * pas.maxParticles;
            }
            foreach (ParticleSystem pas in Over3000MeshParticlesCountObjList)
            {
                curr += Over3000MeshParticlesCountObjWeight * pas.maxParticles;
            }
            return curr;
        }

        public void Check(GameObject RootObj)
        {
            Init();

            #region 粒子系统部分
            ParticleSystem[] ps = RootObj.GetComponentsInChildren<ParticleSystem>(true);
            for (int i = 0; i < ps.Length; i++)
            {
                ParticleSystemRenderer render = ps[i].GetComponent<ParticleSystemRenderer>();

                //粒子系统的权重
                ParticlesObjList.Add(ps[i]);

                /*粒子发射方式相关的权重*******************************************/
                //如果是发射模型的，权重再增加
                if (render.renderMode == ParticleSystemRenderMode.Mesh)
                {
                    //添加到发射模型列表
                    MeshParticlesObjList.Add(ps[i]);
                }
                /*粒子发射方式相关的权重*******************************************/


                /*粒子数目相关的权重***************************************************/
                if (ps[i].maxParticles > 100 && ps[i].maxParticles <= 200)
                {
                    Over100ParticlesObjList.Add(ps[i]);
                }
                else if (ps[i].maxParticles > 200 && ps[i].maxParticles < 500)
                {
                    Over200ParticlesObjList.Add(ps[i]);
                }
                else if (ps[i].maxParticles >= 500 && ps[i].maxParticles < 1000)
                {
                    Over500ParticlesObjList.Add(ps[i]);
                }
                else if (ps[i].maxParticles >= 1000)
                {
                    Over1000ParticlesObjList.Add(ps[i]);
                }

                if(render.renderMode == ParticleSystemRenderMode.Mesh)
                {
                    if(render.mesh)
                    {
                        int fac = render.mesh.GetTriangles(0).Length / 3;
                        if (fac > 500 && fac < 1000)
                        {
                            Over500MeshParticlesCountObjList.Add(ps[i]);
                        }
                        else if (fac >= 1000 && fac < 1500)
                        {
                            Over1000MeshParticlesCountObjList.Add(ps[i]);
                        }
                        else if (fac >= 1500 && fac < 3000)
                        {
                            Over1500MeshParticlesCountObjList.Add(ps[i]);
                        }
                        else if (fac >= 3000)
                        {
                            Over3000MeshParticlesCountObjList.Add(ps[i]);
                        }
                    }

                }
                /*粒子数目相关的权重***************************************************/
            }
            #endregion

            #region MeshRender部分

            MeshRenderer[] ms = RootObj.GetComponentsInChildren<MeshRenderer>(true);
            MeshRenderObjList.AddRange(ms);

            #endregion

            #region SkinnedMeshRender部分

            SkinnedMeshRenderer[] skinns = RootObj.GetComponentsInChildren<SkinnedMeshRenderer>(true);
            SkinnedMeshRendererObjList.AddRange(skinns);

            #endregion


            #region 材质检测检测相关

            Renderer[] renders = RootObj.GetComponentsInChildren<Renderer>(true);

            //材质球检测
            foreach (Renderer r in renders)
            {
                foreach (Material mat in r.sharedMaterials)
                {
                    //是不同的材质球
                    if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                        DissolveGlowManager.IsDissloveGlowMaterial(mat))
                    {
                        GlowOrDissloveOrOutLineMaterialObjList.Add(r.gameObject);
                    }
                    else if (EffectUtily.IsDistortionMaterial(mat))
                    {
                        DistronList.Add(r.gameObject);
                    }
                    else if (EffectUtily.IsLowDistortionMaterial(mat))
                    {
                        LowDistronList.Add(r.gameObject);
                    }
                    else if (EffectUtily.IsWater_Best(mat))
                    {
                        WaterBestList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsWater_Good(mat))
                    {
                        WaterGoodList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsWater_Simple(mat))
                    {
                        WaterSimpleList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsIce_Best(mat))
                    {
                        IceBestList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsIce_Good(mat))
                    {
                        IceGoodList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsBump_Rim(mat))
                    {
                        Bump_RimList.Add(r.gameObject);
                    }

                    else if (EffectUtily.IsStandrad(mat))
                    {
                        StandradList.Add(r.gameObject);
                    }
                }
            }
            Projector[] proj = RootObj.GetComponentsInChildren<Projector>();

            //材质球检测
            foreach (Projector r in proj)
            {
                Material mat = r.material;
                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(r.gameObject);
                }
                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(r.gameObject);
                }
                    else if (EffectUtily.IsLowDistortionMaterial(mat))
                {
                    LowDistronList.Add(r.gameObject);
                }
                else if (EffectUtily.IsWater_Best(mat))
                {
                    WaterBestList.Add(r.gameObject);
                }

                else if (EffectUtily.IsWater_Good(mat))
                {
                    WaterGoodList.Add(r.gameObject);
                }

                else if (EffectUtily.IsWater_Simple(mat))
                {
                    WaterSimpleList.Add(r.gameObject);
                }

                else if (EffectUtily.IsIce_Best(mat))
                {
                    IceBestList.Add(r.gameObject);
                }

                else if (EffectUtily.IsIce_Good(mat))
                {
                    IceGoodList.Add(r.gameObject);
                }

                else if (EffectUtily.IsBump_Rim(mat))
                {
                    Bump_RimList.Add(r.gameObject);
                }

                else if (EffectUtily.IsFlowLight(mat))
                {
                    FlowLightList.Add(r.gameObject);
                }
            }
            #endregion

            #region AddMaterial相关

            Effect_AddMaterial[] Adds = RootObj.GetComponentsInChildren<Effect_AddMaterial>(true);
            AddMaterialScriptList.AddRange(Adds);
            foreach (Effect_AddMaterial ead in Adds)
            {
                Material mat = ead.TargetMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Best(mat))
                {
                    WaterBestList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Good(mat))
                {
                    WaterGoodList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Simple(mat))
                {
                    WaterSimpleList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Best(mat))
                {
                    IceBestList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Good(mat))
                {
                    IceGoodList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Simple(mat))
                {
                    IceSimpleList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsBump_Rim(mat))
                {
                    Bump_RimList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsFlowLight(mat))
                {
                    FlowLightList.Add(ead.gameObject);
                }
            }

            #endregion

            #region ReplaceMaterial相关

            Effect_ReplaceMaterial[] rapps = RootObj.GetComponentsInChildren<Effect_ReplaceMaterial>(true);
            ReplaceMaterialScriptList.AddRange(rapps);
            foreach (Effect_ReplaceMaterial ead in rapps)
            {
                Material mat = ead.TargetMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Best(mat))
                {
                    WaterBestList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Good(mat))
                {
                    WaterGoodList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsWater_Simple(mat))
                {
                    WaterSimpleList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Best(mat))
                {
                    IceBestList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Good(mat))
                {
                    IceGoodList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsIce_Simple(mat))
                {
                    IceSimpleList.Add(ead.gameObject);
                }
                else if (EffectUtily.IsBump_Rim(mat))
                {
                    Bump_RimList.Add(ead.gameObject);
                }

                else if (EffectUtily.IsFlowLight(mat))
                {
                    FlowLightList.Add(ead.gameObject);
                }
            }

            #endregion


            #region 拖尾相关
            Effect.TrailsSystem.Effect_TrailRenderer_Base[] trbs = RootObj.GetComponentsInChildren<Effect.TrailsSystem.Effect_TrailRenderer_Base>(true);
            TrialRenderList.AddRange(trbs);

            foreach (Effect.TrailsSystem.Effect_TrailRenderer_Base tb in trbs)
            {
                Material mat = tb.TrailData.TrailMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsWater_Best(mat))
                {
                    WaterBestList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsWater_Good(mat))
                {
                    WaterGoodList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsWater_Simple(mat))
                {
                    WaterSimpleList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsIce_Best(mat))
                {
                    IceBestList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsIce_Good(mat))
                {
                    IceGoodList.Add(tb.gameObject);
                }

                else if (EffectUtily.IsIce_Simple(mat))
                {
                    IceSimpleList.Add(tb.gameObject);
                }
            }
            #endregion

            #region 武器刀光
            Effect.Effect_WeaponTrail[] ews = RootObj.GetComponentsInChildren<Effect.Effect_WeaponTrail>(true);
            Effect.Effect_WeaponTrail_Simple[] ewss = RootObj.GetComponentsInChildren<Effect.Effect_WeaponTrail_Simple>(true); ;
            Effect.Effect_WeaponTrail_Simplest[] ewsst = RootObj.GetComponentsInChildren<Effect.Effect_WeaponTrail_Simplest>(true); ;

            WeaponTrialList.AddRange(ews);
            Weapon_SimpleTrialList.AddRange(ewss);
            Weapon_SimplestTrialList.AddRange(ewsst);

            foreach (Effect.Effect_WeaponTrail e in ews)
            {
                Material mat = e.TrailMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(e.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(e.gameObject);
                }
            }

            foreach (Effect.Effect_WeaponTrail_Simple e in ewss)
            {
                Material mat = e.TrailMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(e.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(e.gameObject);
                }
            }

            foreach (Effect.Effect_WeaponTrail_Simplest e in ewsst)
            {
                Material mat = e.TrailMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(e.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(e.gameObject);
                }
            }
            #endregion

            #region 条带

            Effect_LineRender[] elr = RootObj.GetComponentsInChildren<Effect_LineRender>(true);

            LineRenderList.AddRange(elr);
            foreach (Effect_LineRender e in elr)
            {
                LineRenderer lr = e.GetComponent<LineRenderer>();
                Material mat = lr.sharedMaterial;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(e.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(e.gameObject);
                }
            }
            #endregion

            #region 全屏特效

            Effect_FullScreen[] efs = RootObj.GetComponentsInChildren<Effect_FullScreen>(true);
            FullScreenList.AddRange(efs);
            foreach (Effect_FullScreen e in efs)
            {
                Material mat = e.mat;

                //是不同的材质球
                if (GlowManager.IsGlowMaterial(mat) || GlowOutLineManager.IsGlowOutLineMaterial(mat) ||
                    DissolveGlowManager.IsDissloveGlowMaterial(mat))
                {
                    GlowOrDissloveOrOutLineMaterialObjList.Add(e.gameObject);
                }

                else if (EffectUtily.IsDistortionMaterial(mat))
                {
                    DistronList.Add(e.gameObject);
                }

                else if (EffectUtily.IsWater_Best(mat))
                {
                    WaterBestList.Add(e.gameObject);
                }

                else if (EffectUtily.IsWater_Good(mat))
                {
                    WaterGoodList.Add(e.gameObject);
                }

                else if (EffectUtily.IsWater_Simple(mat))
                {
                    WaterSimpleList.Add(e.gameObject);
                }

                else if (EffectUtily.IsIce_Best(mat))
                {
                    IceBestList.Add(e.gameObject);
                }

                else if (EffectUtily.IsIce_Good(mat))
                {
                    IceGoodList.Add(e.gameObject);
                }

                else if (EffectUtily.IsIce_Simple(mat))
                {
                    IceSimpleList.Add(e.gameObject);
                }
            }

            #endregion

            #region 特效位移

            Effect_LocalMove[] lm = RootObj.GetComponentsInChildren<Effect_LocalMove>(true);
            LocalMoveList.AddRange(lm);

            #endregion


            #region 灯光
            Light[] ligs = RootObj.GetComponentsInChildren<Light>(true);
            LightList.AddRange(ligs);

            #endregion

            #region Animator

            Animator[] anis = RootObj.GetComponentsInChildren<Animator>(true);
            AnimatorList.AddRange(anis);

            #endregion

        }
    }

    public struct EffectCheckResult
    {
        public EffectCheckMoudules fastModules;
        public EffectCheckMoudules goodModules;
        public EffectCheckMoudules highModules;
        public EffectCheckMoudules BestModules;
    }

    [CustomEditor(typeof(EffectNode))]
    public class Effect_PlayEditor : Editor
    {
        EffectNode pa;
        public void OnEnable()
        {
            pa = target as EffectNode;
            pa.PlayOnAwake = true;
            DecalEditorUtil.InitCallBack();
        }
        EffectCheckResult rt;
        Vector2 performanceView = Vector2.zero;
        bool bChecked = false;

        public override void OnInspectorGUI()
        {
            EditorGUILayout.BeginVertical("box"); 
            EditorGUILayout.Toggle("是否达标:", pa.PerformaceOk);

            if (!pa.ShowingRoomEffect)
            {
                pa.isXPEffect = EditorGUILayout.Toggle("是否XP:", pa.isXPEffect);
            }

            if (!pa.isXPEffect)
            {
                pa.ShowingRoomEffect = EditorGUILayout.Toggle("展示特效:", pa.ShowingRoomEffect);
            }

            pa.BestObj = EditorGUILayout.ObjectField("BestObj:", pa.BestObj, typeof(GameObject), true) as GameObject;
            pa.HighObj = EditorGUILayout.ObjectField("HighObj:", pa.HighObj, typeof(GameObject), true) as GameObject;
            pa.GoodObj = EditorGUILayout.ObjectField("GoodObj:", pa.GoodObj, typeof(GameObject), true) as GameObject;
            pa.FastestObj = EditorGUILayout.ObjectField("FastestObj:", pa.FastestObj, typeof(GameObject), true) as GameObject;
            pa.DebugMode = EditorGUILayout.Toggle("调试:", pa.DebugMode);
            pa.PlayOnAwake = EditorGUILayout.Toggle("PlayOnAwake:", pa.PlayOnAwake);
            pa.EffectLength = EditorGUILayout.FloatField("特效时长:", pa.EffectLength);

            pa.SrcObj = EditorGUILayout.ObjectField("源物体:", pa.SrcObj, typeof(Transform), true) as Transform;
            pa.DstObj = EditorGUILayout.ObjectField("目标物体:", pa.DstObj, typeof(Transform), true) as Transform;

            EditorGUILayout.EndVertical();

            GUILayout.Space(15.0f);
            EditorGUILayout.BeginVertical("box");
            pa.hasMarkDecal = EditorGUILayout.Toggle("是否附带刀痕", pa.hasMarkDecal);

            if (pa.hasMarkDecal)
            {
                if(pa.decalTran == null)
                {
                    string name = pa.transform.name + "_ShowCube";
                    GameObject decal = DecalEditorUtil.GetDecalShowCube(name);
                    pa.decalTran = decal.transform;
                    pa.decalTran.localPosition = Vector3.zero;
                    DecalShowCube showcube = pa.decalTran.gameObject.AddComponent<DecalShowCube>();
                    showcube.node = pa;
                }
                else
                {
                    if (GUILayout.Button("保存Transform数据"))
                    {
                        SaveDecalInfo(pa.decalTran);
                    }
                    if (pa.decalSprite!=null && GUILayout.Button("更新显示的材质"))
                    {
                        if (pa.decalMat)
                            pa.decalTran.GetComponent<MeshRenderer>().material = pa.decalMat;
                        else
                        {
                            pa.decalTran.GetComponent<MeshRenderer>().material = DecalEditorUtil.GetDefaultDecalMaterial();
                            pa.decalMat = DecalEditorUtil.GetDefaultDecalMaterial();
                        }
                        pa.decalTran.GetComponent<MeshRenderer>().material.mainTexture = pa.decalSprite.texture;
                        pa.decalTran.eulerAngles = Vector3.forward * pa.decalAngle;
                        pa.decalTran.position = pa.transform.position + pa.decalOffsetDir;
                        pa.decalTran.localScale = pa.decalScale;
                    }
                }

                EditorGUILayout.ObjectField("刀痕展现预置体：", pa.decalTran, typeof(Transform),true);
                pa.decalDelayTime = EditorGUILayout.FloatField("刀痕延迟时间：", pa.decalDelayTime);
                pa.decalAngle = EditorGUILayout.FloatField("刀痕倾斜角度：",pa.decalAngle);
                pa.decalScale = EditorGUILayout.Vector3Field("刀痕大小：", pa.decalScale);
                pa.decalOffsetDir = EditorGUILayout.Vector3Field("刀痕偏移：", pa.decalOffsetDir);

                pa.useAltas = EditorGUILayout.Toggle("是否使用图集：", pa.useAltas);
                if (pa.useAltas)   //是否使用图集
                    pa.decalSprite = DecalEditorUtil.DrawDecalSpriteList(pa.decalSprite);
                else
                    pa.decalSprite = EditorGUILayout.ObjectField("刀痕纹理", pa.decalSprite, typeof(Sprite), false) as Sprite;
                
                pa.decalMat = EditorGUILayout.ObjectField("刀痕材质", pa.decalMat, typeof(Material),false) as Material;

                GUILayout.Space(15.0f);

                EditorGUILayout.BeginVertical("box");
                pa.decalNeedFade = EditorGUILayout.Toggle("淡入淡出设置：", pa.decalNeedFade);
                if (pa.decalNeedFade)
                {
                    EditorGUILayout.FloatField("刀痕开始淡入时间：", 0);
                    pa.decalFadeInEndTime = EditorGUILayout.FloatField("刀痕结束淡入时间：", pa.decalFadeInEndTime);
                    pa.decalFadeOutStartTime = EditorGUILayout.FloatField("刀痕开始淡出时间：", pa.decalFadeOutStartTime);
                    pa.decalFadeOutEndTime = EditorGUILayout.FloatField("刀痕结束淡出时间：", pa.decalFadeOutEndTime);
                }
                GUILayout.Space(15.0f);
                EditorGUILayout.EndVertical();
            }
            GUILayout.Space(15.0f);
            EditorGUILayout.EndVertical();

            EditorGUILayout.BeginVertical("box");
            pa.Loop = EditorGUILayout.Toggle("循环:", pa.Loop);
            if (pa.Loop)
            {
                pa.LoopLength = EditorGUILayout.FloatField("循环长度:", pa.LoopLength);
                pa.LoopDelatTime = EditorGUILayout.FloatField("两次循环间隔(小于0.03无效):", pa.LoopDelatTime);
                pa.LoopTimes = EditorGUILayout.IntField("循环次数(小于0无限次):", pa.LoopTimes);
            }
            EditorGUILayout.EndVertical();

            GUILayout.Space(10.0f);
            if(Application.isPlaying)
            {
                if (GUILayout.Button("Play(敌方)"))
                {
                    pa.PlayEffect(true, -1, pa.SrcObj, pa.DstObj, (int)EffectNodeMask.Mask_EnableLihgt);
                }

                if (GUILayout.Button("Play(友方)"))
                {
                    pa.PlayEffect(false, -1, pa.SrcObj, pa.DstObj, (int)EffectNodeMask.Mask_EnableLihgt);
                }

                if (GUILayout.Button("Stop"))
                {
                    pa.StopEffect();
                    pa.ResetScale();
                    pa.SetParticleSpeed(1.0f);
                }

            }
            else
            {
                if (GUILayout.Button("Check"))
                {
                    rt = CheckPerformace(pa);
                    bChecked = true;
                }

                if (GUILayout.Button("Bake"))
                {
                    Bake(pa);
                    EditorUtility.SetDirty(pa);
                }
            }


            if (bChecked)
            {
                DrawPerformahceInfo();
            }

        }

        /// <summary>
        /// 通过DecalCubeShow保存刀痕信息
        /// </summary>
        /// <param name="tran"></param>
        public void SaveDecalInfo(Transform tran)
        {
            pa.decalAngle = tran.eulerAngles.z;
            pa.decalOffsetDir = tran.position - pa.transform.position;
            pa.decalScale = tran.localScale;
        }

        void DrawPerformahceInfo()
        {
            if(!bChecked)
            {
                return;
            }

            GUILayout.Space(10.0f);
            performanceView = GUILayout.BeginScrollView(performanceView);

            if(pa.FastestObj)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label("FastObject:");
                DrawModluesInfo(rt.fastModules,GetFastestMaxUnit(pa));
                GUILayout.EndVertical();
            }


            if(pa.GoodObj)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label("GoodObject:");
                DrawModluesInfo(rt.goodModules, GetGoodMaxUnit(pa));
                GUILayout.EndVertical();
            }


            if (pa.HighObj)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label("HighObj:");
                DrawModluesInfo(rt.highModules, GetHighMaxUnit(pa));
                GUILayout.EndVertical();
            }

            if (pa.BestObj)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label("BestObj:");
                DrawModluesInfo(rt.BestModules, GetBestMaxUnit(pa));
                GUILayout.EndVertical();
            }

            GUILayout.EndScrollView();
        }

        void DrawModluesInfo(EffectCheckMoudules modules,float refunit)
        {
            if(null == modules)
            {
                return;
            }
            float valu = modules.GetUnit();
            if(valu <= refunit)
            {
                EditorGUILayout.HelpBox("恭喜,你已超越其他特效了。你还能做得更好", MessageType.Info);
            }
            else
            {
                if (valu >= refunit)
                {
                    EditorGUILayout.HelpBox("呵呵，改！", MessageType.Error);
                }
                
            }

            GUILayout.Label("资源已经占用:" + valu + "，结余:" + (refunit - valu));

            if (modules.ParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.ParticlesDesc + ",权重:" + EffectCheckMoudules.ParticlesWeight + ",总和:" + modules.ParticlesObjList.Count * EffectCheckMoudules.ParticlesWeight);

                foreach (ParticleSystem s in modules.ParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }


            if (modules.MeshParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.MeshParticlesDesc + ",权重:" + EffectCheckMoudules.MeshParticlesWeight + ",总和:" + modules.MeshParticlesObjList.Count * EffectCheckMoudules.MeshParticlesWeight);

                foreach (ParticleSystem s in modules.MeshParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Over100ParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over100ParticlesDesc + ",权重:" + EffectCheckMoudules.Over100ParticlesWeight + ",总和:" + modules.Over100ParticlesObjList.Count * EffectCheckMoudules.Over100ParticlesWeight);

                foreach (ParticleSystem s in modules.Over100ParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Over200ParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over200ParticlesDesc + ",权重:" + EffectCheckMoudules.Over200ParticlesWeight + ",总和:" + 
                                modules.Over200ParticlesObjList.Count * EffectCheckMoudules.Over200ParticlesWeight);

                foreach (ParticleSystem s in modules.Over200ParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Over500ParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over500ParticlesDesc + ",权重:" + EffectCheckMoudules.Over500ParticlesWeight + ",总和:" +
                                modules.Over500ParticlesObjList.Count * EffectCheckMoudules.Over500ParticlesWeight);

                foreach (ParticleSystem s in modules.Over500ParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Over1000ParticlesObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over1000ParticlesDesc + ",权重:" + EffectCheckMoudules.Over1000ParticlesWeight + ",总和:" +
                                modules.Over1000ParticlesObjList.Count * EffectCheckMoudules.Over1000ParticlesWeight);

                foreach (ParticleSystem s in modules.Over1000ParticlesObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Over500MeshParticlesCountObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over500MeshParticlesDesc + ",权重:" + EffectCheckMoudules.Over500MeshParticlesCountObjWeight + ",总和:" +
                                modules.Over500MeshParticlesCountObjList.Count * EffectCheckMoudules.Over500MeshParticlesCountObjWeight);

                foreach (ParticleSystem s in modules.Over500MeshParticlesCountObjList)
                {
                    GUILayout.BeginHorizontal();
                    EditorGUILayout.LabelField("数量:" + s.maxParticles);
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();
            }

            if (modules.Over1000MeshParticlesCountObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over1000MeshParticlesDesc + ",权重:" + EffectCheckMoudules.Over1000MeshParticlesCountObjWeight + ",总和:" +
                                modules.Over1000MeshParticlesCountObjList.Count * EffectCheckMoudules.Over1000MeshParticlesCountObjWeight);

                foreach (ParticleSystem s in modules.Over1000MeshParticlesCountObjList)
                {
                    GUILayout.BeginHorizontal();
                    EditorGUILayout.LabelField("数量:" + s.maxParticles);
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();
            }

            if (modules.Over1500MeshParticlesCountObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over1500MeshParticlesDesc + ",权重:" + EffectCheckMoudules.Over1500MeshParticlesCountObjWeight + ",总和:" +
                                modules.Over1500MeshParticlesCountObjList.Count * EffectCheckMoudules.Over1500MeshParticlesCountObjWeight);

                foreach (ParticleSystem s in modules.Over1500MeshParticlesCountObjList)
                {
                    GUILayout.BeginHorizontal();
                    EditorGUILayout.LabelField("数量:" + s.maxParticles);
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();
            }


            if (modules.Over3000MeshParticlesCountObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Over3000MeshParticlesDesc + ",权重:" + EffectCheckMoudules.Over3000MeshParticlesCountObjWeight + ",总和:" +
                                modules.Over3000MeshParticlesCountObjList.Count * EffectCheckMoudules.Over3000MeshParticlesCountObjWeight);

                foreach (ParticleSystem s in modules.Over3000MeshParticlesCountObjList)
                {
                    GUILayout.BeginHorizontal();
                    EditorGUILayout.LabelField("数量:" + s.maxParticles);
                    EditorGUILayout.ObjectField(s, typeof(ParticleSystem), true);
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();
            }
            if (modules.MeshRenderObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.MeshRenderDesc + ",权重:" + EffectCheckMoudules.MeshRenderWeight + ",总和:" +
                                modules.MeshRenderObjList.Count * EffectCheckMoudules.MeshRenderWeight);

                foreach (MeshRenderer s in modules.MeshRenderObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(MeshRenderer), true);
                }
                GUILayout.EndVertical();
            }


            if (modules.SkinnedMeshRendererObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.SkinnedMeshRendererDesc + ",权重:" + EffectCheckMoudules.SkinnedMeshRendererWeight + ",总和:" +
                                modules.SkinnedMeshRendererObjList.Count * EffectCheckMoudules.SkinnedMeshRendererWeight);

                foreach (SkinnedMeshRenderer s in modules.SkinnedMeshRendererObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(SkinnedMeshRenderer), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.GlowOrDissloveOrOutLineMaterialObjList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.GlowOrDissloveOrOutLineMaterialDesc + ",权重:" + EffectCheckMoudules.GlowOrDissloveOrOutLineMaterialWeight + ",总和:" +
                                modules.GlowOrDissloveOrOutLineMaterialObjList.Count * EffectCheckMoudules.GlowOrDissloveOrOutLineMaterialWeight);

                foreach (GameObject s in modules.GlowOrDissloveOrOutLineMaterialObjList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.DistronList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.DistronDesc + ",权重:" + EffectCheckMoudules.DistronWeight + ",总和:" +
                                modules.DistronList.Count * EffectCheckMoudules.DistronWeight);

                foreach (GameObject s in modules.DistronList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.LowDistronList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.LowDistronDesc + ",权重:" + EffectCheckMoudules.LowDistronWeight + ",总和:" +
                                modules.LowDistronList.Count * EffectCheckMoudules.LowDistronWeight);

                foreach (GameObject s in modules.LowDistronList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.AddMaterialScriptList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.AddMaterialScriptDesc + ",权重:" + EffectCheckMoudules.AddMaterialScriptWeight + ",总和:" +
                                modules.AddMaterialScriptList.Count * EffectCheckMoudules.AddMaterialScriptWeight);

                foreach (Effect_AddMaterial s in modules.AddMaterialScriptList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect_AddMaterial), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.ReplaceMaterialScriptList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.ReplaceMaterialScriptDesc + ",权重:" + EffectCheckMoudules.ReplaceMaterialScriptWeight + ",总和:" +
                                modules.ReplaceMaterialScriptList.Count * EffectCheckMoudules.ReplaceMaterialScriptWeight);

                foreach (Effect_ReplaceMaterial s in modules.ReplaceMaterialScriptList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect_ReplaceMaterial), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.TrialRenderList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.TrialRenderDesc + ",权重:" + EffectCheckMoudules.TrialRenderWeight + ",总和:" +
                                modules.TrialRenderList.Count * EffectCheckMoudules.TrialRenderWeight);

                foreach (Effect.TrailsSystem.Effect_TrailRenderer_Base s in modules.TrialRenderList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.TrailsSystem.Effect_TrailRenderer_Base), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.WeaponTrialList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.WeaponTrialDesc + ",权重:" + EffectCheckMoudules.WeaponTrialWeight + ",总和:" +
                                modules.WeaponTrialList.Count * EffectCheckMoudules.WeaponTrialWeight);

                foreach (Effect.Effect_WeaponTrail s in modules.WeaponTrialList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_WeaponTrail), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Weapon_SimpleTrialList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Weapon_SimpleTrialDesc + ",权重:" + EffectCheckMoudules.Weapon_SimpleTrialWeight + ",总和:" +
                                modules.Weapon_SimpleTrialList.Count * EffectCheckMoudules.Weapon_SimpleTrialWeight);

                foreach (Effect.Effect_WeaponTrail_Simple s in modules.Weapon_SimpleTrialList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_WeaponTrail_Simple), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.Weapon_SimplestTrialList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Weapon_SimplestTrialDesc + ",权重:" + EffectCheckMoudules.Weapon_SimplestTrialWeight + ",总和:" +
                                modules.Weapon_SimplestTrialList.Count * EffectCheckMoudules.Weapon_SimplestTrialWeight);

                foreach (Effect.Effect_WeaponTrail_Simplest s in modules.Weapon_SimplestTrialList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_WeaponTrail_Simplest), true);
                }
                GUILayout.EndVertical();
            }

            if (modules.LineRenderList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.LineRenderDesc + ",权重:" + EffectCheckMoudules.LineRenderWeight + ",总和:" +
                                modules.LineRenderList.Count * EffectCheckMoudules.LineRenderWeight);

                foreach (Effect.Effect_LineRender s in modules.LineRenderList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_LineRender), true);
                }
                GUILayout.EndVertical();
            }


            if (modules.FullScreenList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.FullScreenDesc + ",权重:" + EffectCheckMoudules.FullScreenWeight + ",总和:" +
                                modules.FullScreenList.Count * EffectCheckMoudules.FullScreenWeight);

                foreach (Effect.Effect_FullScreen s in modules.FullScreenList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_FullScreen), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.LocalMoveList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.LocalMoveDesc + ",权重:" + EffectCheckMoudules.LocalMoveWeight + ",总和:" +
                                modules.LocalMoveList.Count * EffectCheckMoudules.LocalMoveWeight);

                foreach (Effect_LocalMove s in modules.LocalMoveList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Effect.Effect_LocalMove), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.IceBestList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.IceBestDesc + ",权重:" + EffectCheckMoudules.IceBestWeight + ",总和:" +
                                modules.IceBestList.Count * EffectCheckMoudules.IceBestWeight);

                foreach (GameObject s in modules.IceBestList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.IceGoodList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.IceGoodDesc + ",权重:" + EffectCheckMoudules.IceGoodWeight + ",总和:" +
                                modules.IceGoodList.Count * EffectCheckMoudules.IceGoodWeight);

                foreach (GameObject s in modules.IceGoodList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.IceSimpleList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.IceSimpleDesc + ",权重:" + EffectCheckMoudules.IceSimpleWeight + ",总和:" +
                                modules.IceSimpleList.Count * EffectCheckMoudules.IceSimpleWeight);

                foreach (GameObject s in modules.IceSimpleList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.WaterBestList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.WaterBestDesc + ",权重:" + EffectCheckMoudules.WaterBestWeight + ",总和:" +
                                modules.WaterBestList.Count * EffectCheckMoudules.WaterBestWeight);

                foreach (GameObject s in modules.WaterBestList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.WaterGoodList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.WaterGoodDesc + ",权重:" + EffectCheckMoudules.WaterGoodWeight + ",总和:" +
                                modules.WaterGoodList.Count * EffectCheckMoudules.WaterGoodWeight);

                foreach (GameObject s in modules.WaterGoodList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.WaterSimpleList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.WaterSimpleDesc + ",权重:" + EffectCheckMoudules.WaterSimpleWeight + ",总和:" +
                                modules.WaterSimpleList.Count * EffectCheckMoudules.WaterSimpleWeight);

                foreach (GameObject s in modules.WaterSimpleList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }
            if (modules.LightList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.LightDesc + ",权重:" + EffectCheckMoudules.LightWeight + ",总和:" +
                                modules.LightList.Count * EffectCheckMoudules.LightWeight);

                foreach (Light s in modules.LightList)
                {
                    EditorGUILayout.ObjectField(s, typeof(Light), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.Bump_RimList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.Bump_RimDesc + ",权重:" + EffectCheckMoudules.Bump_RimWeight + ",总和:" +
                                modules.Bump_RimList.Count * EffectCheckMoudules.Bump_RimWeight);

                foreach (GameObject s in modules.Bump_RimList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.FlowLightList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.FlowLightDesc + ",权重:" + EffectCheckMoudules.FlowLightWeight + ",总和:" +
                                modules.FlowLightList.Count * EffectCheckMoudules.FlowLightWeight);

                foreach (GameObject s in modules.FlowLightList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }

            if (modules.StandradList.Count > 0)
            {
                GUILayout.BeginVertical("box");
                GUILayout.Label(EffectCheckMoudules.StandradDesc + ",权重:" + EffectCheckMoudules.LowStandradWeight + ",总和:" +
                                modules.StandradList.Count * EffectCheckMoudules.LowStandradWeight);

                foreach (GameObject s in modules.StandradList)
                {
                    EditorGUILayout.ObjectField(s, typeof(GameObject), true);
                }

                GUILayout.EndVertical();
            }
            //if(modules.AnimatorList.Count > 0)
            //{
            //    GUILayout.BeginVertical("box");
            //    GUILayout.Label(EffectCheckMoudules.AnimatorDesc + ",权重:" + EffectCheckMoudules.AnimatorWeight + ",总和:" +
            //                    modules.AnimatorList.Count * EffectCheckMoudules.AnimatorWeight);

            //    foreach (Animator s in modules.AnimatorList)
            //    {
            //        EditorGUILayout.ObjectField(s, typeof(Animator), true);
            //    }

            //    GUILayout.EndVertical();
            //}
        }

        public static EffectCheckResult CheckPerformace(EffectNode pa)
        {
            bool fastPerformace = true;
            bool GoodPerformace = true;
            bool HighPerformace = true;
            bool BestPerformace = true;

            EffectCheckResult rest = new EffectCheckResult();
            pa.PerformaceUnit = 0;
            if (pa.FastestObj)
            {
                EffectCheckMoudules modules = new EffectCheckMoudules();
                modules.Check(pa.FastestObj);
                rest.fastModules = modules;
                fastPerformace = rest.fastModules.GetUnit() <= GetFastestMaxUnit(pa);
                pa.PerformaceUnit += rest.fastModules.GetUnit();
                
            }

            if (pa.GoodObj)
            {
                EffectCheckMoudules modules = new EffectCheckMoudules();
                modules.Check(pa.GoodObj);
                rest.goodModules = modules;
                    
                GoodPerformace = rest.goodModules.GetUnit() <= GetGoodMaxUnit(pa);
                pa.PerformaceUnit += rest.goodModules.GetUnit();
            }

            if (pa.HighObj)
            {
                EffectCheckMoudules modules = new EffectCheckMoudules();
                modules.Check(pa.HighObj);
                rest.highModules = modules;

                HighPerformace = rest.highModules.GetUnit() <= GetHighMaxUnit(pa);
                pa.PerformaceUnit += rest.highModules.GetUnit();
            }

            if (pa.BestObj)
            {
                EffectCheckMoudules modules = new EffectCheckMoudules();
                modules.Check(pa.BestObj);
                rest.BestModules = modules;

                BestPerformace = rest.BestModules.GetUnit() <= GetBestMaxUnit(pa);
                pa.PerformaceUnit += rest.BestModules.GetUnit();
            }

            pa.PerformaceOk = false;

            if (!fastPerformace)
            {
                Debug.LogError("Fast模块超标:" + pa.gameObject.name);
            }
            if (!GoodPerformace)
            {
                Debug.LogError("Good模块超标:" + pa.gameObject.name);
            }
            if (!HighPerformace)
            {
                Debug.LogError("High模块超标:" + pa.gameObject.name);
            }
            if (!BestPerformace)
            {
                Debug.LogError("Best模块超标:" + pa.gameObject.name);
            }
            Debug.Log("性能指标:" + pa.PerformaceUnit);
            pa.PerformaceOk = fastPerformace && GoodPerformace && HighPerformace && BestPerformace;

            return rest;

        }

        public const float MaxFastestEffectUnit = 4;
        public const float MaxGoodEffectUnit = 4;
        public const float MaxHighEffectUnit = 10;
        public const float MaxBestEffectUnit = 60;

        public static float GetFastestMaxUnit(EffectNode pa)
        {
            if (pa.isXPEffect)
            {
                return 15.0f;
            }
            if(pa.ShowingRoomEffect)
            {
                return 30.0f;
            }
            return MaxFastestEffectUnit;
        }

        public static float GetGoodMaxUnit(EffectNode pa)
        {
            if (pa.isXPEffect)
            {
                return 15.0f;
            }
            if (pa.ShowingRoomEffect)
            {
                return 30.0f;
            }
            return MaxGoodEffectUnit;
        }

        public static float GetHighMaxUnit(EffectNode pa)
        {
            if (pa.isXPEffect)
            {
                return 15.0f;
            }
            if (pa.ShowingRoomEffect)
            {
                return 30.0f;
            }
            return MaxHighEffectUnit;
        }

        public static float GetBestMaxUnit(EffectNode pa)
        {
            if (pa.isXPEffect)
            {
                return 999;
            }
            if (pa.ShowingRoomEffect)
            {
                return 999;
            }
            return MaxBestEffectUnit;
        }


        public static void Bake(EffectNode pa)
        {
            if (Application.isPlaying || !Application.isEditor)
            {
                Debug.LogWarning("Bake只能在编辑器下的非播放模式下调用!");
                return;
            }

            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerLightingEffectOnly), pa.gameObject, true);
            pa.effectColliders = pa.GetComponentsInChildren<Collider>();

            CleanUpAllMaterial(pa);
            if (pa.BestObj)
            {
                if (null == pa.BestModules)
                {
                    pa.BestModules = new EffectModules();
                }
                pa.BestModules.Clear();
                pa.BestModules.Bake(pa.BestObj);
            }

            if (pa.HighObj)
            {
                if (null == pa.HighModlues)
                {
                    pa.HighModlues = new EffectModules();
                }
                pa.HighModlues.Clear();
                pa.HighModlues.Bake(pa.HighObj);
            }

            if (pa.GoodObj)
            {
                if (null == pa.GoodModlues)
                {
                    pa.GoodModlues = new EffectModules();
                }
                pa.GoodModlues.Clear();
                pa.GoodModlues.Bake(pa.GoodObj);
            }

            if (pa.FastestObj)
            {
                if (null == pa.FastestModlues)
                {
                    pa.FastestModlues = new EffectModules();
                }
                pa.FastestModlues.Clear();
                pa.FastestModlues.Bake(pa.FastestObj);
            }
            else
            {
                Debug.LogWarning(pa.name + "--FastestObj 为空!");
                return;
            }

            CheckPerformace(pa);
            pa.bBaked = true;
            Debug.Log("bake成功");

            ParticleSystem[] allPs = pa.GetComponentsInChildren<ParticleSystem>();
            foreach (ParticleSystem ps in allPs)
            {
                ParticleSystem.ShapeModule sp = ps.shape;
                if (sp.shapeType == ParticleSystemShapeType.Mesh)
                {
                    if (sp.mesh)
                    {
                        string path = AssetDatabase.GetAssetPath(sp.mesh);
                        if (!path.Contains(AssetBundleImporter.effectParticleModelAssetPatch) && !path.Contains(AssetBundleImporter.MeshPath))
                        {
                            Debug.LogError(ps.gameObject.name + ",Shape模块引用了Mesh，但不在目录下:" + AssetBundleImporter.effectParticleModelAssetPatch + ",路径:" + path);
                        }
                    }
                }

                ParticleSystemRenderer rend = ps.GetComponent<ParticleSystemRenderer>();
                if(rend.renderMode == ParticleSystemRenderMode.Mesh)
                {
                    if (rend.mesh)
                    {
                        string path = AssetDatabase.GetAssetPath(rend.mesh);
                        if (!path.Contains(AssetBundleImporter.effectParticleModelAssetPatch) && !path.Contains(AssetBundleImporter.MeshPath))
                        {
                            Debug.LogError(ps.gameObject.name + ",Renderer引用了Mesh，但不在目录下:" + AssetBundleImporter.effectParticleModelAssetPatch + ",路径:" + path);
                        }
                    }
                }
            }
        }

        private static void CleanUpAllMaterial(EffectNode pa)
        {

            List<Material> allMat = new List<Material>();
            //清理材质球
            Renderer[] re = pa.GetComponentsInChildren<Renderer>(true);
            Projector[] pro = pa.GetComponentsInChildren<Projector>(true);
            Effect.Effect_AddMaterial[] addMat = pa.GetComponentsInChildren<Effect_AddMaterial>(true);
            Effect.Effect_ReplaceMaterial[] relpMat = pa.GetComponentsInChildren<Effect_ReplaceMaterial>(true);
            Effect.Effect_FullScreen[] flullMat = pa.GetComponentsInChildren<Effect_FullScreen>(true);
            LineRenderer[] LineMat = pa.GetComponentsInChildren<LineRenderer>(true);

            foreach (Renderer r in re)
            {
                allMat.AddRange(r.sharedMaterials);
            }

            foreach (Projector r in pro)
            {
                allMat.Add(r.material);
            }

            foreach (Effect_AddMaterial r in addMat)
            {
                allMat.Add(r.TargetMaterial);
            }
            foreach (Effect_ReplaceMaterial r in relpMat)
            {
                allMat.Add(r.TargetMaterial);
            }
            foreach (Effect_FullScreen r in flullMat)
            {
                allMat.Add(r.mat);
            }
            foreach (LineRenderer r in LineMat)
            {
                allMat.AddRange(r.sharedMaterials);
            }

            foreach(Material mat in allMat)
            {
                CleanUpMaterial(mat);
            }
        }
        private static void CleanUpMaterial(Material mat)
        {
            if (mat)
            {
                SerializedObject psSource = new SerializedObject(mat);
                SerializedProperty emissionProperty = psSource.FindProperty("m_SavedProperties");
                SerializedProperty texEnvs = emissionProperty.FindPropertyRelative("m_TexEnvs");
                SerializedProperty floats = emissionProperty.FindPropertyRelative("m_Floats");
                SerializedProperty colos = emissionProperty.FindPropertyRelative("m_Colors");

                CleanMaterialSerializedProperty(texEnvs, mat);
                CleanMaterialSerializedProperty(floats, mat);
                CleanMaterialSerializedProperty(colos, mat);
                psSource.ApplyModifiedProperties();

                EditorUtility.SetDirty(mat);
            }
        }

        private static void CleanMaterialSerializedProperty(SerializedProperty property, Material mat)
        {
            for (int j = property.arraySize - 1; j >= 0; j--)
            {
                string propertyName = property.GetArrayElementAtIndex(j).FindPropertyRelative("first").FindPropertyRelative("name").stringValue;
                if (!mat.HasProperty(propertyName))
                {
                    property.DeleteArrayElementAtIndex(j);
                }
            }
        }

    }

}
