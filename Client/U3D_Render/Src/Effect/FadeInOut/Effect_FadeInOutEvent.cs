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
using UnityEngine.Events;
namespace Effect
{
    public class FloatFadeFinishEvent : UnityEvent<Effect_FadeInOutMaterialFloat>
    {
    }

    public class LightFadeFinishEvent : UnityEvent<Effect_FadeInOutLightIntensity>
    {
    }
    public class ColorFadeFinishEvent : UnityEvent<Effect_FadeInOutMaterialColor>
    {
    }
    public class SceneBrightnesFadeFinishEvent : UnityEvent<Effect_ModifSceneBrightness>
    {
    }
}




        