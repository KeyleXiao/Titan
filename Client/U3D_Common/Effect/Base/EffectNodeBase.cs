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
    public abstract partial class EffectNodeBase : MonoBehaviour
    {
        public bool PlayOnAwake = false;
        public bool isStaticEffect = false;


        public abstract void Init();

        public abstract void PlayEffect(bool bEnemy = false, int HostEntityID = - 1, Transform src = null, Transform dst = null, int mask = 0);
        public abstract void PlayEffect(bool bEnemy, int HostEntityID, Vector3 srcPosition, Vector3 dstPosition, int mask = 0);

        public abstract void StopEffect();
    }
}




        