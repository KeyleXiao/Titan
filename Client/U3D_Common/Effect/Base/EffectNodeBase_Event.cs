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

    public abstract partial class EffectNodeBase : MonoBehaviour
    {
        private static EffectQualityChangeEvent m_OnEffectQualityChange = new EffectQualityChangeEvent();

        /// <summary>
        /// 静态特效等级改变
        /// </summary>
        public abstract void OnStaticEffectQualityChange();

        public static void ChangeEffectQualityLevel()
        {
            if (null != m_OnEffectQualityChange)
            {
                m_OnEffectQualityChange.Invoke();
            }
        }

        public static void ResigterEffectQualityChange(UnityEngine.Events.UnityAction ev)
        {
            if (null != m_OnEffectQualityChange)
            {
                if (null != ev)
                {
                    m_OnEffectQualityChange.AddListener(ev);
                }
            }
        }

        public static void UnResigterEffectQualityChange(UnityEngine.Events.UnityAction ev)
        {
            if (null != m_OnEffectQualityChange)
            {
                if (null != ev)
                {
                    m_OnEffectQualityChange.RemoveListener(ev);
                }
            }
        }
    }
}




        