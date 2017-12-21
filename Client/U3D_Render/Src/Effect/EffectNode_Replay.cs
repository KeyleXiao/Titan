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
        public bool Loop = false;
        public float LoopLength = 1.0f;
        public float LoopDelatTime = 0.1f;
        public int LoopTimes = 1;
        private Coroutine waitForReplay = null;
        private int currLoopTimes = 0;
        void BeginReplay()
        {
            if (Loop && LoopLength > 0.03)
            {
                waitForReplay = GameUtil.StartCoroutineHelper(this,WaitForReplay());
            }
            currLoopTimes = 0;
        }

        void EndReplay()
        {
            currLoopTimes = 0;
            if(null != waitForReplay)
            {
                StopCoroutine(waitForReplay);
            }
        }

        IEnumerator WaitForReplay()
        {
            while(true)
            {
                yield return new WaitForSeconds(LoopLength);
                if (!Loop || LoopLength < 0.03)
                {
                    yield break;
                }
                if (LoopTimes > 0 && currLoopTimes >= LoopTimes)
                {
                    yield break;
                }

                if(LoopTimes > 0)
                {
                    currLoopTimes++;
                }
                
                StopEffectImpl();
                if(LoopDelatTime > 0.03f)
                {
                    yield return new WaitForSeconds(LoopDelatTime);
                    PlayImpl(this.currMask);
                }
                else
                {
                    PlayImpl(this.currMask);
                }
            }
        }
    }
}



