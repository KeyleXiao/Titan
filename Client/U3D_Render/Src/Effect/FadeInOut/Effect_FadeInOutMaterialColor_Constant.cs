using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace Effect
{
    public partial class Effect_FadeInOutMaterialColor : MonoBehaviour
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

        public void ReStartFadeOutFinish()
        {
            if (FadeOutDuartion <= 0.03f)
            {
                currentFloat = Mathf.Lerp(FadeOutFrom, FadeOutTo, FadeTimer / FadeOutDuartion);
                Color CurrentColor;
                foreach (Material mat in matInstances)
                {
                    CurrentColor = OrginalColorMap[mat];
                    bool bNeedToUpdateColor = false;
                    if (FadeAlpha)
                    {
                        CurrentColor.a *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (FadeColor)
                    {
                        CurrentColor.r *= currentFloat;
                        CurrentColor.b *= currentFloat;
                        CurrentColor.g *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (bNeedToUpdateColor)
                    {
                        mat.SetColor(ColorName, CurrentColor);
                        EffectUtily.UpdateColor(mat, ColorName, CurrentColor);
                    }

                }

                InvokeFinishEvent();
                canDoFadeOut = false;
                return;
            }

            if (canDoFadeOut)
            {
                if (null != fadeOutFinish)
                {
                    StopCoroutine(fadeOutFinish);
                }
                fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh_Constant());
                return;
            }
        }

        private void InitDefaultVariables_Constant()
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
                Color c = OrginalColorMap[mat] * val;
                if (!FadeAlpha)
                {
                    c.a = OrginalColorMap[mat].a;
                }
                mat.SetColor(ColorName, c);
                EffectUtily.UpdateColor(mat, ColorName, c);
            }
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
                            fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh_Constant());
                        }
                    }
                    else//有延迟淡出
                    {
                        canDoFadeOut = false;
                        bFadeOutFinsh = false;
                        fadeOutStart = GameUtil.StartCoroutineHelper(this,WaitForFadeOutStart_Constant());
                    }
                }
                else //有淡入
                {
                    canDoFadeIn = true;
                    bFadeInFinsh = false;
                    fadeInFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeInFinsh_Constant());
                }
            }
            else
            {
                bFadeInFinsh = false;
                fadeInStart = GameUtil.StartCoroutineHelper(this,WaitForFadeInStart_Constant());
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

            Color CurrentColor;
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
                    CurrentColor = OrginalColorMap[mat];
                    bool bNeedToUpdateColor = false;

                    if (FadeAlpha)
                    {
                        CurrentColor.a *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (FadeColor)
                    {
                        CurrentColor.r *= currentFloat;
                        CurrentColor.b *= currentFloat;
                        CurrentColor.g *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (bNeedToUpdateColor)
                    {
                        mat.SetColor(ColorName, CurrentColor);
                        EffectUtily.UpdateColor(mat, ColorName, CurrentColor);
                    }
                    
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
                    CurrentColor = OrginalColorMap[mat];
                    bool bNeedToUpdateColor = false;
                    if (FadeAlpha)
                    {
                        CurrentColor.a *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (FadeColor)
                    {
                        CurrentColor.r *= currentFloat;
                        CurrentColor.b *= currentFloat;
                        CurrentColor.g *= currentFloat;
                        bNeedToUpdateColor = true;
                    }

                    if (bNeedToUpdateColor)
                    {
                        mat.SetColor(ColorName, CurrentColor);
                        EffectUtily.UpdateColor(mat, ColorName, CurrentColor);
                    }
                    
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

        IEnumerator WaitForFadeInStart_Constant()
       {
           yield return new WaitForSeconds(FadeInDealy);
           canDoFadeIn = true;
           bFadeInFinsh = false;
           FadeTimer = 0.0f;
           //判断是否拥有淡入
           if (FadeInDuartion > 0.03f)
           {
               fadeInFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeInFinsh_Constant());
           }
           else
           {
               canDoFadeIn = false;
               bFadeInFinsh = true;
           }
       }

        IEnumerator WaitForFadeInFinsh_Constant()
       {
           yield return new WaitForSeconds(FadeInDuartion);
           canDoFadeIn = false;
           bFadeInFinsh = true;
           FadeTimer = 0.0f;
           if (FadeOutDelay > 0.03f)//判断是否有延迟淡出
           {
               bFadeOutFinsh = false;
               fadeOutStart = GameUtil.StartCoroutineHelper(this,WaitForFadeOutStart_Constant());
           }
           else
           {
               if (FadeOutDuartion > 0.03f)//判断是否有淡出
               {
                   canDoFadeOut = true;
                   bFadeOutFinsh = false;
                   fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh_Constant());
               }
               else
               {
                   canDoFadeOut = false;
                   bFadeOutFinsh = true;
               }

           }
       }

        IEnumerator WaitForFadeOutStart_Constant()
       {
           yield return new WaitForSeconds(FadeOutDelay);
           canDoFadeOut = true;
           bFadeOutFinsh = false;
           FadeTimer = 0.0f;
           //判断是否拥有淡出

           if (FadeOutDuartion > 0.03f)
           {
               fadeOutFinish = GameUtil.StartCoroutineHelper(this,WaitForFadeOutFinsh_Constant());
           }
           else
           {
               canDoFadeOut = false;
               bFadeOutFinsh = true;
           }
       }


        IEnumerator WaitForFadeOutFinsh_Constant()
       {
           yield return new WaitForSeconds(FadeOutDuartion);
           FadeTimer = 0.0f;
           canDoFadeOut = false;
           bFadeOutFinsh = true;
       }
    }
}

