/*******************************************************************
** 文件名:	ImageEffectManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/13
** 版  本:	1.0
** 描  述:	图像特效管理
** 应  用:  	管理所有程序写的各种特效

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
namespace ImageEffects
{


    /// <summary>
    /// 图像特效管理
    /// </summary>
    /// 
    public partial class ImageEffectManager :MonoBehaviour
    {
        public static Dictionary<Camera, ImageEffectManager> ImageEffectManagerTable = new Dictionary<Camera, ImageEffectManager>();

        public static bool IsStandAlone = true;

        /// <summary>
        /// 进阶后处理，比如fxaa，ssao之类的可以关闭的后处理，像glow之类的不可关闭
        /// </summary>
        public static bool AdvanceImangeEffectEnabled = true;

        public delegate Shader LoadShaderDelegate(string s);
        private static LoadShaderDelegate LoadShaderDelegateInstance = null;

        public class CameraRenderFlagsNode
        {
            public int guid = -1;
            public DepthTextureMode flags;
            public int RefCount = 0;
            public Camera hostCamera = null;
        }




        public static Shader API_GetShader(string shaderName)
        {
            if(null != LoadShaderDelegateInstance)
            {
                return LoadShaderDelegateInstance(shaderName);
            }

            return Shader.Find(shaderName);
        }

        public static void API_RegisterLoadShaderDelegate(LoadShaderDelegate fuc)
        {
            LoadShaderDelegateInstance = fuc;
        }

        public static void API_GameStart()
        {
            IsStandAlone = false;
            API_ResetSceneBrightness();
        }

        public static void API_GameEnd()
        {
            IsStandAlone = true;
            API_ResetSceneBrightness();
            ImageEffectManagerTable.Clear();
            LoadShaderDelegateInstance = null;
        }


        public static void API_InitSceneCamera(Camera Cam)
        {
            ImageEffectManager ime = Cam.GetComponent<ImageEffectManager>();
            if(ime)
            {
                ime.InitSceneCamera();
            }
        }
        
        public static void API_ModifySceneBrightness(float brightness)
        {
            float val = Mathf.Max(0, brightness);
            Shader.SetGlobalFloat("_EffectSceneBrightnessScale", val);
        }

        public static void API_ResetSceneBrightness()
        {
            API_ModifySceneBrightness(1.0f);
        }
        private static ImageEffectManager GetImageEffectManager(Camera cam)
        {
            ImageEffectManager result = null;
            if(cam)
            {
                ImageEffectManagerTable.TryGetValue(cam, out result);
            }
            
            return result;

        }

        public static void API_SetDirty(Camera Cam)
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.dirty = true;
            }
        }

        public static void API_SetDirty()
        {
            foreach(ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if(ime)
                {
                    ime.dirty = true;
                }
            }
        }

        public static void API_ResigiterFoucusChage(Camera Cam,System.Action<bool> call)
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.OnFoucusChange += call;
            }
        }

        public static void API_UnResigiterFoucusChage(Camera Cam, System.Action<bool> call)
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.OnFoucusChange -= call;
            }
        }

        public static void API_ResigiterFoucusChage( System.Action<bool> call)
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.OnFoucusChange += call;
                }
            }
        }

        public static void API_UnResigiterFoucusChage(System.Action<bool> call)
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.OnFoucusChange -= call;
                }
            }
        }

        public static T API_Add<T>(Camera Cam) where T : ImageEffectNode
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                return result.Add<T>();
            }
            return null;
        }

        public static List<T> API_Add_ALL<T>() where T : ImageEffectNode
        {
            List<T> result = new List<T>();
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    result.Add(ime.Add<T>());
                }
            }
            return result;
        }

        public static void API_DisableImageEffect<T>(Camera Cam) where T : ImageEffectNode
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.DisableImageEffect<T>();
            }
            
        }

        public static void API_DisableImageEffect<T>() where T : ImageEffectNode
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.DisableImageEffect<T>();
                }
            }
        }

        public static void API_EnableImageEffect<T>(Camera Cam) where T : ImageEffectNode
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.EnableImageEffect<T>();
            }
        }

        public static void API_EnableImageEffect<T>() where T : ImageEffectNode
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.EnableImageEffect<T>();
                }
            }
        }

        public static void API_DisableImageEffect(Camera Cam,Type t)
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.DisableImageEffect(t);
            }
        }

        public static void API_DisableImageEffect(Type t)
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.DisableImageEffect(t);
                }
            }

        }

        public static void API_EnableImageEffect(Camera Cam, Type t)
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.EnableImageEffect(t);
            }
        }

        public static void API_EnableImageEffect(Type t)
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.EnableImageEffect(t);
                }
            }

        }

        public static void API_Remove<T>(Camera Cam) where T : ImageEffectNode
        {
            ImageEffectManager result = GetImageEffectManager(Cam);
            if (result)
            {
                result.Remove<T>();
            }
        }

        public static void API_Remove<T>() where T : ImageEffectNode
        {
            foreach (ImageEffectManager ime in ImageEffectManagerTable.Values)
            {
                if (ime)
                {
                    ime.Remove<T>();
                }
            }
        }




        public static void API_ImageQuality_Fastest()
        {
            ImageEffectManager.ImageEffectShaderLOD = 100;
            Shader.globalMaximumLOD = 100;
            Shader.DisableKeyword("HIGH_QUALITY_GLOW");
            API_Refetsh();
            API_DisableImageEffect<ModifyGameBrightness>();
            API_DisableImageEffect<Tonemapping>();
            API_DisableImageEffect<GlowManager>();
            API_DisableImageEffect<DissolveGlowManager>();
            API_DisableImageEffect<VignetteAndChromaticAberration>();
            API_DisableImageEffect<ColorCorrectionCurves>();

            //API_DisableImageEffect<BloomAndFlares>();
            //API_DisableImageEffect<SSAOPro>();
            //API_DisableImageEffect<FXAA>();

            API_DisableImageEffect<VolumetricLightScattering>();
            API_DisableImageEffect<GlowOutLineManager>();
            API_DisableImageEffect<SunShafts>();
            API_DisableImageEffect<CandelaSSRR>();
            API_DisableImageEffect<DepthOfFieldScatter>();
            API_DisableImageEffect<ColorCorrectionCurves>();
        }

        public static void API_ImageQuality_Good()
        {

            ImageEffectManager.ImageEffectShaderLOD = 200;
            Shader.globalMaximumLOD = 200;
            Shader.DisableKeyword("HIGH_QUALITY_GLOW");
            API_Refetsh();
            API_EnableImageEffect<ModifyGameBrightness>();
            API_EnableImageEffect<GlowManager>();

            API_DisableImageEffect<VignetteAndChromaticAberration>();
            API_DisableImageEffect<GlowOutLineManager>();
            API_DisableImageEffect<Tonemapping>();
            API_DisableImageEffect<DissolveGlowManager>();
            API_DisableImageEffect<ColorCorrectionCurves>();
            //API_DisableImageEffect<BloomAndFlares>();
           // API_DisableImageEffect<SSAOPro>();
           // API_DisableImageEffect<FXAA>();
            API_DisableImageEffect<VolumetricLightScattering>();
            API_DisableImageEffect<SunShafts>();
            API_DisableImageEffect<CandelaSSRR>();
            API_DisableImageEffect<DepthOfFieldScatter>();
            API_DisableImageEffect<ColorCorrectionCurves>();
            
        }

        public static void API_ImageQuality_High()
        {

            ImageEffectManager.ImageEffectShaderLOD = 300;
            Shader.globalMaximumLOD = 300;
            Shader.EnableKeyword("HIGH_QUALITY_GLOW");
            API_Refetsh();
            API_EnableImageEffect<ModifyGameBrightness>();
            API_EnableImageEffect<GlowManager>();
            API_EnableImageEffect<GlowOutLineManager>();
            

            //API_DisableImageEffect<BloomAndFlares>();
           // API_DisableImageEffect<SSAOPro>();
            //API_DisableImageEffect<FXAA>();
            API_DisableImageEffect<VignetteAndChromaticAberration>();
            API_DisableImageEffect<DissolveGlowManager>();
            API_DisableImageEffect<CandelaSSRR>();
            API_DisableImageEffect<DepthOfFieldScatter>();
            API_DisableImageEffect<ColorCorrectionCurves>();
            API_DisableImageEffect<SunShafts>();
            API_DisableImageEffect<Tonemapping>();
            API_DisableImageEffect<VolumetricLightScattering>();
           
        }

        public static void API_ImageQuality_Best()
        {

            ImageEffectManager.ImageEffectShaderLOD = int.MaxValue;
            Shader.globalMaximumLOD = int.MaxValue;
            Shader.EnableKeyword("HIGH_QUALITY_GLOW");
            API_Refetsh();
            API_EnableImageEffect<ModifyGameBrightness>();
            API_EnableImageEffect<Tonemapping>();
            API_EnableImageEffect<GlowManager>();
            API_EnableImageEffect<DissolveGlowManager>();
            API_EnableImageEffect<VignetteAndChromaticAberration>();
            API_EnableImageEffect<ColorCorrectionCurves>();

           // API_EnableImageEffect<BloomAndFlares>();
            //API_EnableImageEffect<SSAOPro>();
           // API_EnableImageEffect<FXAA>();
            API_EnableImageEffect<SunShafts>();
            API_EnableImageEffect<VolumetricLightScattering>();
            API_EnableImageEffect<GlowOutLineManager>();
            API_EnableImageEffect<CandelaSSRR>();
            API_EnableImageEffect<DepthOfFieldScatter>();
        }

        public static void API_ClearSceneCamera(Camera Cam)
        {
            ImageEffectManager res = GetImageEffectManager(Cam);
            if (res)
            {
                res.ClearSceneCamera();
            }
        }

        public static CameraRenderFlagsNode API_AttachCameraRenderFlags(Camera cam, DepthTextureMode mod)
        {
            CameraRenderFlagsNode reslut = null;
            ImageEffectManager res = GetImageEffectManager(cam);
            if (res)
            {
                reslut = res.AttachCameraRenderFlags(mod);
            }

            return reslut;

        }

        public static void API_DeAttachCameraRenderFlags(Camera Cam, ref CameraRenderFlagsNode flags)
        {
            ImageEffectManager res = GetImageEffectManager(Cam);
            if (res)
            {
                res.DeAttachCameraRenderFlags(ref flags);
            }

            
        }

        public static void API_SetImagEffectParam<T>(string paramName,float Val) where T : ImageEffectNode
        {
            foreach (ImageEffectManager imr in ImageEffectManagerTable.Values)
            {
                if (imr)
                {
                    imr.SetImagEffectParam<T>(paramName, Val);
                }
            }
        }

        public static void API_SetImagEffectParam<T>(Camera cam,string paramName,float Val) where T : ImageEffectNode
        {
            ImageEffectManager result = GetImageEffectManager(cam);
            if (result)
            {
                result.SetImagEffectParam<T>(paramName, Val);
            }
        }

        private static void API_Refetsh()
        {
            foreach (ImageEffectManager imr in ImageEffectManagerTable.Values)
            {
                if (imr)
                {
                    imr.Refresh();
                }
            }
        }

        private static void API_Refetsh(Camera cam)
        {
            ImageEffectManager result = GetImageEffectManager(cam);
            if (result)
            {
                result.Refresh();
            }
        }

        public static void API_UpdatePlayer(Camera cam,GameObject hero)
        {
            ImageEffectManager result = GetImageEffectManager(cam);
            if (result)
            {
                result.hero = hero;
            }
        }

        public static void API_ApplicationFocusChanged(Camera cam,bool value)
        {
            ImageEffectManager result = GetImageEffectManager(cam);
            if (result)
            {
                result.ApplicationFocusChanged(value);
            }
        }
        public static void API_ApplicationFocusChanged(bool value)
        {
            foreach (ImageEffectManager imr in ImageEffectManagerTable.Values)
            {
                if (imr)
                {
                    imr.ApplicationFocusChanged(value);
                }
            }
        }


        public static List<ImageEffectNode> API_GetImangeEffectList(Camera cam)
        {
            ImageEffectManager result = GetImageEffectManager(cam);
            if (result)
            {
                return result.AllImageEffeNodeList;
            }
            return null;
        }
    }
}

