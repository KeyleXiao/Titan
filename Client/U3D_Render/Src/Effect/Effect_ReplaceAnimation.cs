/*******************************************************************
** 文件名:	EffectEnumDef.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2016/05/09
** 版  本:	1.0
** 描  述:	特效枚举定义
** 应  用:  	特效枚举定义

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
namespace Effect
{
    [System.Serializable]
    public class AnimationReplacePair
    {
        public AnimationClip orgClip;
        public AnimationClip replaceClip;
    }

    public class Effect_ReplaceAnimation : MonoBehaviour
    {
        public AnimationReplacePair[] clipPairsA;
        public AnimationReplacePair[] clipPairsB;
    }

}

