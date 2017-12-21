using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace Effect
{
    public partial class SceneFadeInOutMaterialFloat : MonoBehaviour
    {
        //淡入延时,小于0.03直接开始
        public float FadeInDealy = 0;

        //淡入持续时间,小于等于0.03表示没有淡入
        public float FadeInDuartion = 0;

        public float FadeInFrom = 0.0f;
        public float FadeInTo = 0.0f;


        //淡出延时,小于0.03直接开始
        public float FadeOutDelay = 0;

        //淡出持续时间,小于等于0表示没有淡出
        public float FadeOutDuartion = 0.0f;

        public float FadeOutFrom = 0.0f;
        public float FadeOutTo = 0.0f;

        [System.NonSerialized]
        public float FadeTimer = 0.0f;
        [System.NonSerialized]
        public float currentFloat = 0.0f;

        private bool canDoFadeIn = false;
        private bool bFadeInFinsh = true;
        private bool canDoFadeOut = false;
        private bool bFadeOutFinsh = true;


        Coroutine fadeInStart;
        Coroutine fadeInFinish;
        Coroutine fadeOutStart;
        Coroutine fadeOutFinish;
        public void ReStartFadeOutFinish_Constant()
        {

            if (FadeOutDuartion <= 0.03f)
            {
                currentFloat = FadeOutFrom;

                foreach (Material mat in matInstances)
                {
                    mat.SetFloat(PropertyName, currentFloat);
                    EffectUtily.UpdateFloat(mat, PropertyName, currentFloat);
                }
                canDoFadeOut = false;
                return;
            }
            if (canDoFadeOut)
            {
                if (null != fadeOutFinish)
                {
                    StopCoroutine(fadeOutFinish);
                }
                fadeOutFinish = GameUtil.StartCoroutineHelper(this, WaitForFadeOutFinsh());
                return;
            }

        }

        void Stop_Constant()
        {
            if (null != fadeInStart)
            {
                StopCoroutine(fadeInStart);
            }
            if (null != fadeInFinish)
            {
                StopCoroutine(fadeInFinish);
            }
            if (null != fadeOutStart)
            {
                StopCoroutine(fadeOutStart);
            }
            if (null != fadeOutFinish)
            {
                StopCoroutine(fadeOutFinish);
            }
            FadeTimer = 0.0f;

            canDoFadeIn = false;
            bFadeInFinsh = true;
            canDoFadeOut = false;
            bFadeOutFinsh = true;
        }

        

        void Reset_Constant()
        {
            //判断是否需要延迟淡入或者需要淡入
            if (FadeInDealy <= 0.03f) //小于0.03f不需要延时，直接开
            {
                if (FadeInDuartion <= 0.03f)//小于0.03f，没有淡入
                {
                    if (FadeOutDelay <= 0.03f) //小于0.03f,直接开淡出
                    {
                        if (FadeOutDuartion <= 0.03f)//小于0.03f,没有淡出
                        {

                        }
                        else //有淡出
                        {
                            canDoFadeOut = true;
                            bFadeOutFinsh = false;
                            fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh());
                        }
                    }
                    else//有延迟淡出
                    {
                        canDoFadeOut = false;
                        bFadeOutFinsh = false;
                        fadeOutStart = GameUtil.StartCoroutineHelper(this,WaitForFadeOutStart());
                    }
                }
                else //有淡入
                {
                    canDoFadeIn = true;
                    bFadeInFinsh = false;
                    fadeInFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeInFinsh());
                }
            }
            else
            {
                bFadeInFinsh = false;
                fadeInStart = GameUtil.StartCoroutineHelper(this,WaitForFadeInStart());
            }
        }

        void InitDefaultVariables_Constant()
        {
            foreach (Material mat in matInstances)
            {
                if (!mat)
                {
                    continue;
                }
                float val = 0.0f;
                if (FadeInDuartion > 0.03f)
                {
                    val = FadeInFrom;
                }
                else if (FadeOutDuartion > 0.03f)
                {
                    val = FadeOutFrom;
                }

                mat.SetFloat(PropertyName, val);
            }
        }

        void Update_Constant()
        {
            if (FadeOutDuartion <= 0.03f)
            {
                if (canDoFadeOut || !bFadeOutFinsh)
                {
                    canDoFadeOut = false;
                    bFadeOutFinsh = true;
                    if (null != fadeOutStart)
                    {
                        StopCoroutine(fadeOutStart);
                    }
                }

            }

            if (FadeInDuartion <= 0.03f)
            {
                if (canDoFadeIn || !bFadeInFinsh)
                {
                    canDoFadeIn = false;
                    bFadeInFinsh = true;
                    if (null != fadeInFinish)
                    {
                        StopCoroutine(fadeInFinish);
                    }

                    if (null != fadeInStart)
                    {
                        StopCoroutine(fadeInStart);
                    } 
                }

            }

            if (canDoFadeIn)
            {
                FadeTimer += Time.deltaTime;
                currentFloat = Mathf.Lerp(FadeInFrom, FadeInTo, FadeTimer / FadeInDuartion);
                foreach (Material mat in matInstances)
                {
                    if (!mat)
                    {
                        continue;
                    }
                    mat.SetFloat(PropertyName, currentFloat);
                    EffectUtily.UpdateFloat(mat, PropertyName, currentFloat);
                }

            }

            if (canDoFadeOut)
            {
                FadeTimer += Time.deltaTime;
                currentFloat = Mathf.Lerp(FadeOutFrom, FadeOutTo, FadeTimer / FadeOutDuartion);
                
                foreach (Material mat in matInstances)
                {
                    if (!mat)
                    {
                        continue;
                    }
                    mat.SetFloat(PropertyName, currentFloat);
                    EffectUtily.UpdateFloat(mat, PropertyName, currentFloat);
                }
            }

            if (FadeInDuartion > 0.03f && bFadeInFinsh && FadeOutDuartion < 0.03f)//有淡入，没淡出
            {
                Stop_Constant();
                if (Loop)
                {
                    Reset_Constant();
                }
                else
                {
                    bPlay = false;
                    InvokeFinishEvent();
                }

            }
            else if (FadeOutDuartion > 0.03f && bFadeOutFinsh && FadeInDuartion < 0.03f) //有淡出，没淡入
            {
                Stop_Constant();
                if (Loop)
                {
                    Reset_Constant();
                }
                else
                {
                    bPlay = false;
                    InvokeFinishEvent();
                }
            }
            else if (bFadeOutFinsh && bFadeInFinsh) //两者都有
            {
                Stop_Constant();
                if (Loop)
                {
                    Reset_Constant();
                }
                else
                {
                    bPlay = false;
                    InvokeFinishEvent();
                }
            }
            else if (FadeInDuartion < 0.03f && FadeOutDuartion < 0.03f)//没有淡入，也没有淡出，直接回调
            {
                Stop_Constant();
                bPlay = false;
                InvokeFinishEvent();
            }

        }


        IEnumerator WaitForFadeInStart()
        {
            yield return new WaitForSeconds(FadeInDealy);
            canDoFadeIn = true;
            bFadeInFinsh = false;
            FadeTimer = 0.0f;
            //判断是否拥有淡入
            if (FadeInDuartion > 0.03f)
            {
                fadeInFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeInFinsh());
            }
            else
            {
                canDoFadeIn = false;
                bFadeInFinsh = true;
            }
        }

        IEnumerator WaitForFadeInFinsh()
        {
            yield return new WaitForSeconds(FadeInDuartion);
            canDoFadeIn = false;
            bFadeInFinsh = true;
            FadeTimer = 0.0f;
            if (FadeOutDelay > 0.03f)//判断是否有延迟淡出
            {
                bFadeOutFinsh = false;
                fadeOutStart = GameUtil.StartCoroutineHelper(this,WaitForFadeOutStart());
            }
            else
            {
                if (FadeOutDuartion > 0.03f)//判断是否有淡出
                {
                    canDoFadeOut = true;
                    bFadeOutFinsh = false;
                    fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh());
                }
                else
                {
                    canDoFadeOut = false;
                    bFadeOutFinsh = true;
                }

            }
        }

        IEnumerator WaitForFadeOutStart()
        {
            yield return new WaitForSeconds(FadeOutDelay);
            canDoFadeOut = true;
            bFadeOutFinsh = false;
            FadeTimer = 0.0f;
            //判断是否拥有淡出

            if (FadeOutDuartion > 0.03f)
            {
                fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh());
            }
            else
            {
                canDoFadeOut = false;
                bFadeOutFinsh = true;
            }
        }


        IEnumerator WaitForFadeOutFinsh()
        {
            yield return new WaitForSeconds(FadeOutDuartion);
            FadeTimer = 0.0f;
            canDoFadeOut = false;
            bFadeOutFinsh = true;
        }
    }
}

