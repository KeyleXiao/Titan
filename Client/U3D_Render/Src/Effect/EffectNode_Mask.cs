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
        private int currMask = 0;

        private void ApplyMask(int mask)
        {
            if(mask == 0)
            {
                return;
            }

            if((mask & (int)EffectNodeMask.Mask_DownLevelQuality) != 0)
            {
                int quality = (int)ImageSetting.GetData().currEffectQuality;
                int min = (int)EffectQuality.QUALITY_FASTEST;
                int max = (int)EffectQuality.QUALITY_BEST;
                quality--;
                quality = Mathf.Clamp(quality, min, max);

                realyQuality = (EffectQuality)quality;
            }

            if ((mask & (int)EffectNodeMask.Mask_UpLevelQuality) != 0)
            {
                int quality = (int)ImageSetting.GetData().currEffectQuality;
                int min = (int)EffectQuality.QUALITY_FASTEST;
                int max = (int)EffectQuality.QUALITY_BEST;
                quality++;
                quality = Mathf.Clamp(quality, min, max);

                realyQuality = (EffectQuality)quality;
            }
        }
    }
}



