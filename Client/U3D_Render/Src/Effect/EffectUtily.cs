/*******************************************************************
** 文件名:	EffectUtily.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2016/1/9
** 版  本:	1.0
** 描  述:	特效工具集
** 应  用:  	特效工具集

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using ImageEffects;
using System.Collections.Generic;
namespace Effect
{
    public enum EffectDispatchResult
    {
        Dispatch_None,
        Diapatch_Glow,
        Dispatch_DissloveGlow,
        Dispatch_GlowOutLine,
    }

    public class EffectUtily 
    {
        /// <summary>
        /// 分发渲染器，分发到对应的glow、DissloveGlow、GlowOutLine中
        /// </summary>
        public static EffectDispatchResult DispatchRender(Renderer r, Material mat, int subMeshIndex = 0)
        {
            if (!r)
            {
                return EffectDispatchResult.Dispatch_None;
            }

            if (!EffectUtily.DispatchRenderToGlow(r, mat, subMeshIndex))
            {
                if (!EffectUtily.DispatchRenderToDissolveGlow(r, mat, subMeshIndex))
                {
                    if(EffectUtily.DispatchRenderToGlowOutLine(r, mat, subMeshIndex))
                    {
                        return EffectDispatchResult.Dispatch_GlowOutLine;
                    }
                    return EffectDispatchResult.Dispatch_None;
                }
                return EffectDispatchResult.Dispatch_DissloveGlow;
            }
            return EffectDispatchResult.Diapatch_Glow;
        }


        /// <summary>
        /// 分发网格，分发到对应的glow、DissloveGlow、GlowOutLine中
        /// </summary>
        public static EffectDispatchResult DispatchMesh(Mesh m, Matrix4x4 matrix, Material mat, int layer = -1)
        {
            if (!mat)
            {
                return EffectDispatchResult.Dispatch_None;
            }

            if(layer != -1)
            {
                if(layer != LayerMask.NameToLayer( Config.LayerLightingEffectOnly))
                {
                    return EffectDispatchResult.Dispatch_None;
                }
            }

            if (!EffectUtily.DispatchMeshToGlow(m, matrix, mat))
            {
                if (!EffectUtily.DispatchMeshToDissolveGlow(m, matrix, mat))
                {
                    if(EffectUtily.DispatchMeshToGlowOutLine(m, matrix, mat))
                    {
                        return EffectDispatchResult.Dispatch_GlowOutLine;
                    }
                    return EffectDispatchResult.Dispatch_None;
                }
                return EffectDispatchResult.Dispatch_DissloveGlow;
            }
            return EffectDispatchResult.Diapatch_Glow;
        }


        /// <summary>
        /// 回收渲染器,从glow、DissloveGlow、GlowOutLine中回收渲染器
        /// </summary>
        public static void GabageRender(Renderer r, Material mat)
        {
            if(!r)
            {
                return;
            }
            if (!EffectUtily.GabageRenderFromGlow(r, mat))
            {
                if (!EffectUtily.GabageRenderFromDissolveGlow(r, mat))
                {
                    EffectUtily.GabageRenderFromGlowOutLine(r, mat);
                }
            }
        }
        

        /// <summary>
        /// 更新UV流动
        /// </summary>
        /// <param name="mat"></param>
        /// <param name="textureProperty"></param>
        /// <param name="scroll"></param>
        public static void UpdateUVScroll(Material mat,string textureProperty,Vector2 scroll)
        {
            if(!mat || textureProperty.Equals(string.Empty) )
            {
                return;
            }
            if (!UpdateUVScrollToGlow(mat, textureProperty, scroll))
            {
                if (!UpdateUVScrollToDissolveGlow(mat, textureProperty, scroll))
                {
                    UpdateUVScrollGlowOutLine(mat, textureProperty, scroll);
                }
            }
        }


        /// <summary>
        /// 更新UV流动
        /// </summary>
        /// <param name="mat"></param>
        /// <param name="textureProperty"></param>
        /// <param name="uvTitle"></param>
        public static void UpdateUVAnimation( Material mat, string textureProperty, Vector2 uvTitle)
        {
            if (!mat || textureProperty.Equals(string.Empty) )
            {
                return;
            }

            if (!UpdateUVAnimationToGlow(mat, textureProperty, uvTitle))
            {
                if (!UpdateUVAnimationToDissolveGlow(mat, textureProperty, uvTitle))
                {
                    UpdateUVAnimationGlowOutLine(mat, textureProperty, uvTitle);
                }
            }
        }


        /// <summary>
        /// 更新浮点值
        /// </summary>
        /// <param name="mat"></param>
        /// <param name="floatProperty"></param>
        /// <param name="value"></param>
        public static void UpdateFloat(Material mat, string floatProperty, float value)
        {
            if (!mat || floatProperty.Equals(string.Empty))
            {
                return;
            }

            if (!UpdateFloatToGlow(mat, floatProperty, value))
            {
                if (!UpdateFloatToDissolveGlow(mat, floatProperty, value))
                {
                    UpdateFloatGlowOutLine(mat, floatProperty, value);
                }
            }
        }


        /// <summary>
        /// 更新颜色
        /// </summary>
        /// <param name="mat"></param>
        /// <param name="colorProperty"></param>
        /// <param name="value"></param>
        public static void UpdateColor(Material mat, string colorProperty, Color value)
        {
            if (!mat || colorProperty.Equals(string.Empty))
            {
                return;
            }

            if (!UpdateColorToGlow(mat, colorProperty, value))
            {
                if (!UpdateColorToDissolveGlow(mat, colorProperty, value))
                {
                    UpdateColorGlowOutLine(mat, colorProperty, value);
                }
            }
        }


        /// <summary>
        /// 更新KeyWorld
        /// </summary>
        /// <param name="mat"></param>
        /// <param name="KeyWorld"></param>
        /// <param name="Enabled"></param>
        public static void UpdateKeyWorld(Material mat, string KeyWorld, bool Enabled)
        {
            if (!mat || KeyWorld.Equals(string.Empty))
            {
                return;
            }

            if(!UpdateKeyWorldToGlow(mat, KeyWorld, Enabled))
            {
                if (!UpdateKeyWorldToDissolveGlow(mat, KeyWorld, Enabled))
                {
                    UpdateKeyWorldToGlowOutLine(mat, KeyWorld, Enabled);
                }
            }
        }

        #region 分发网格
        public static bool DispatchMeshToDissolveGlow(Mesh m, Matrix4x4 matrix, Material mat)
        {

            if (DissolveGlowManager.IsDissloveGlowMaterial(mat))
            {
                foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
                {
                    Instance.PushMesh(m, matrix, mat);
                }
                return true;
            }
            return false;
        }

        public static bool DispatchMeshToGlow(Mesh m, Matrix4x4 matrix, Material mat)
        {
            if (GlowManager.IsGlowMaterial(mat))
            {
                foreach (GlowManager Instance in GlowManager.InstanceList)
                {
                    Instance.PushMesh(m, matrix, mat);
                }
                return true;

            }
            return false;
        }

        public static bool DispatchMeshToGlowOutLine(Mesh m, Matrix4x4 matrix, Material mat)
        {
            if (GlowOutLineManager.IsGlowOutLineMaterial(mat))
            {
                foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
                {
                    Instance.PushMesh(m, matrix, mat);
                }
                return true;
            }
            return false;
        }

        #endregion

        #region 正常渲染器
        public static bool DispatchRenderToDissolveGlow(Renderer r, Material mat, int subMesh)
        {
            if (DissolveGlowManager.IsDissloveGlowMaterial(mat))
            {
                foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
                {
                    Instance.PushRender(r, mat,subMesh);
                }
                return true;

            }
            return false;
        }

        public static bool DispatchRenderToGlow(Renderer r, Material mat, int subMesh)
        {
            if (GlowManager.IsGlowMaterial(mat))
            {
                foreach (GlowManager Instance in GlowManager.InstanceList)
                {
                    Instance.PushRender(r, mat, subMesh);
                }
                return true;

            }
            return false;
        }

        public static bool DispatchRenderToGlowOutLine(Renderer r, Material mat, int subMesh)
        {
            if (GlowOutLineManager.IsGlowOutLineMaterial(mat))
            {
                foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
                {
                    Instance.PushRender(r, mat, subMesh);
                }
                return true;

            }
            return false;
        }


        public static bool GabageRenderFromDissolveGlow(Renderer r, Material mat)
        {
            if (DissolveGlowManager.IsDissloveGlowMaterial(mat))
            {
                foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
                {
                    Instance.RemoveRender(r, mat);
                }
                return true;
            }
            return false;
        }

        public static bool GabageRenderFromGlow(Renderer r, Material mat)
        {
            if (GlowManager.IsGlowMaterial(mat))
            {
                foreach (GlowManager Instance in GlowManager.InstanceList)
                {
                    Instance.RemoveRender(r, mat);
                }
                return true;
            }
            return false;
        }

        public static bool GabageRenderFromGlowOutLine(Renderer r, Material mat)
        {
            if (GlowOutLineManager.IsGlowOutLineMaterial(mat))
            {
                foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
                {
                    Instance.RemoveRender(r, mat);
                }
                return true;
            }

            return false;
        }

        #endregion

        #region 更新材质球属性

        #region UVScroll
        public static bool UpdateUVScrollToDissolveGlow(Material mat, string textureProperty, Vector2 scroll)
        {
            foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVScroll(mat, textureProperty, scroll);
                }

            }
            return true;
        }

        public static bool UpdateUVScrollToGlow(Material mat, string textureProperty, Vector2 scroll)
        {
            foreach (GlowManager Instance in GlowManager.InstanceList)
            {
                if(Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVScroll(mat, textureProperty, scroll);
                }
                
            }
            return true;
        }

        public static bool UpdateUVScrollGlowOutLine(Material mat, string textureProperty, Vector2 scroll)
        {
            foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVScroll(mat, textureProperty, scroll);
                }

            }
            return true;
        }
        #endregion

        #region Color
        public static bool UpdateColorToDissolveGlow(Material mat, string colorProperty, Color value)
        {

            foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateColor(mat, colorProperty, value);
                }

            }
            return true;

        }

        public static bool UpdateColorToGlow(Material mat, string colorProperty, Color value)
        {
            foreach (GlowManager Instance in GlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateColor(mat, colorProperty, value);
                }

            }
            return true;
        }

        public static bool UpdateColorGlowOutLine(Material mat, string colorProperty, Color value)
        {
            foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateColor(mat, colorProperty, value);
                }

            }
            return true;

        }
        #endregion

        #region Float
        public static bool UpdateFloatToDissolveGlow(Material mat, string floatProperty, float value)
        {
            foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateFloat(mat, floatProperty, value);
                }

            }
            return true;
        }

        public static bool UpdateFloatToGlow(Material mat, string floatProperty, float value)
        {
            foreach (GlowManager Instance in GlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateFloat(mat, floatProperty, value);
                }

            }
            return true;
        }

        public static bool UpdateFloatGlowOutLine(Material mat, string floatProperty, float value)
        {
            foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateFloat(mat, floatProperty, value);
                }

            }
            return true;

        }
        #endregion

        #region UVAnimation
        public static bool UpdateUVAnimationToDissolveGlow(Material mat, string textureProperty, Vector2 uvTitle)
        {
            foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVAnimation(mat, textureProperty, uvTitle);
                }

            }
            return true;

        }

        public static bool UpdateUVAnimationToGlow(Material mat, string textureProperty, Vector2 uvTitle)
        {
            foreach (GlowManager Instance in GlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVAnimation(mat, textureProperty, uvTitle);
                }

            }
            return true;

        }

        public static bool UpdateUVAnimationGlowOutLine(Material mat, string textureProperty, Vector2 uvTitle)
        {
            foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateUVAnimation(mat, textureProperty, uvTitle);
                }

            }
            return true;

        }
        #endregion

        #region UpdateKeyWorld
        public static bool UpdateKeyWorldToDissolveGlow(Material mat, string KeyWorld, bool Enabled)
        {
            foreach (DissolveGlowManager Instance in DissolveGlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateKeyWorld(mat, KeyWorld, Enabled);
                }

            }
            return true;

        }

        public static bool UpdateKeyWorldToGlow(Material mat, string KeyWorld, bool Enabled)
        {
            foreach (GlowManager Instance in GlowManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateKeyWorld(mat, KeyWorld, Enabled);
                }

            }
            return true;

        }

        public static bool UpdateKeyWorldToGlowOutLine(Material mat, string KeyWorld, bool Enabled)
        {
            foreach (GlowOutLineManager Instance in GlowOutLineManager.InstanceList)
            {
                if (Instance.ContainsMaterial(mat))
                {
                    Instance.UpdateKeyWorld(mat, KeyWorld, Enabled);
                }

            }
            return true;

        }
        #endregion

        #endregion

        public static bool IsDistortionMaterial(Material mat)
        {
            if(!mat)
            {
                return false;
            }
            string key = "Effect/Distortion";
            return mat.shader.name.Contains(key);


        }

        public static bool IsLowDistortionMaterial(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            string key = "LDistort";
            return mat.shader.name.Contains(key);


        }

        private static LinkedList<GameObject> m_TestModelList = new LinkedList<GameObject>();
        public static void SaveTestModel(GameObject go)
        {
            m_TestModelList.AddLast(go);
        }

        public static void ClearTestModel()
        {
            foreach(GameObject go in m_TestModelList)
            {
                if(go)
                {
                    GameObject temp = go;
                    ResNode.DestroyRes(ref temp);
                }
            }
        }

        /// <summary>
        /// 4.0
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsWater_Best(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return false;
        }

        /// <summary>
        /// 3.5
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsWater_Good(Material mat)
        {
            if(!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Water/SlimeCutOut");
        }

        /// <summary>
        /// 0.1
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsWater_Simple(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return false;
        }

        /// <summary>
        /// 4.0
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsIce_Best(Material mat)
        {
            if(!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Ice/PreVertex");
        }

        /// <summary>
        /// 3.5
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsIce_Good(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Ice/Realistic_Alpha") ||
                mat.shader.name.Contains("Ice/Realistic_Opacity");
        }

        /// <summary>
        /// 0.1
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsIce_Simple(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Ice/Simplest");
        }

        /// <summary>
        /// 0.5
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsBump_Rim(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Effect/Other/Bump_Rim");
        }

        /// <summary>
        /// 0.5
        /// </summary>
        /// <param name="mat"></param>
        /// <returns></returns>
        public static bool IsFlowLight(Material mat)
        {
            if (!mat)
            {
                return false;
            }
            return mat.shader.name.Contains("Effect/FlowLight/");
        }
    }
}




        