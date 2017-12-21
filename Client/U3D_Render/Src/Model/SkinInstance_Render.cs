/*******************************************************************
** 文件名:	SkinInstance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤实例，外部通过这个来使用这个皮肤
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 这个是皮肤实例，外部通过这个来使用这个皮肤
/// </summary>
public partial class SkinInstance                        
{
    public void SetVisible(bool bVisible)
    {
        if (isValid)
        {
            if (AllRenders.Count > 0)
            {
                int cout = AllRenders.Count;
                for (int i = 0; i < cout; i++)
                {
                    AllRenders[i].enabled = bVisible;
                }
            }
        }
    }

    public void SetBindEffectEnabled(bool bEnable)
    {
        if(!isValid)
        {
            return;
        }
        if (switchEffectWithHeroDistance)
        {
            switchEffectWithHeroDistance.DisableAll();
            if (bEnable)
            {
                switchEffectWithHeroDistance.UpdateEffect();
            }

        }

        if (playEntityStaticEffect)
        {
            if (bEnable)
            {
                playEntityStaticEffect.Play(null);
            }
            else
            {
                playEntityStaticEffect.Stop();
            }
        }
    }
}
