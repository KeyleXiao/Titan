/*******************************************************************
** 文件名:	EffectNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效节点
** 应  用:  	特效节点，挂在根节点上面

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
    /// 特效节点
    /// </summary>
    public partial class EffectNode : EffectNodeBase
    {
        private Dictionary<Renderer, Material> m_DispatchedRender = new Dictionary<Renderer, Material>();

        /// <summary>
        /// 分发渲染器，分发到对应的glow、DissloveGlow、GlowOutLine中
        /// </summary>
        void DispatchRender(EffectModules modules)
        {
            foreach (Renderer r in modules.m_Normal_Renders)
            {
                if (!r)
                {
                    Debug.LogWarning(name + ",bake信息已经改变，请美术重新bake!");
                    continue;
                }

                //层级不对，一边去
                if (r.gameObject.layer != LayerMask.NameToLayer(Config.LayerLightingEffectOnly))
                {
                    continue;
                }

                Material mat = r.sharedMaterial;
                if (!EffectUtily.DispatchRenderToGlow(r, mat,0))
                {
                    if (!EffectUtily.DispatchRenderToDissolveGlow(r, mat,0))
                    {
                        if (EffectUtily.DispatchRenderToGlowOutLine(r, mat, 0))
                        {
                            m_DispatchedRender.Add(r, mat);
                        }
                    }
                    else
                    {
                        m_DispatchedRender.Add(r, mat);
                    }
                }
                else
                {
                    m_DispatchedRender.Add(r, mat);
                }
            }
        }


        /// <summary>
        /// 回收渲染器,从glow、DissloveGlow、GlowOutLine中回收渲染器
        /// </summary>
        void GabageRender()
        {
            foreach (KeyValuePair<Renderer, Material> r in m_DispatchedRender)
            {
                if (!EffectUtily.GabageRenderFromGlow(r.Key,r.Value))
                {
                    if(!EffectUtily.GabageRenderFromDissolveGlow(r.Key,r.Value))
                    {
                        EffectUtily.GabageRenderFromGlowOutLine(r.Key,r.Value);
                    }
                }
            }
            m_DispatchedRender.Clear();
        }

        void OnDeviceStateChanged(bool isDeviceLost)
        {
            if (isDeviceLost)
            {
                if (bPlay)
                {
                    StopEffectImpl();
                }
            }
            else
            {
                if (bPlay)
                {
                    PlayImpl(currMask);
                }
            }

        }
    }
}



