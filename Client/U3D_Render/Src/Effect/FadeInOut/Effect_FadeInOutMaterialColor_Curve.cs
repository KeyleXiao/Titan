using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace Effect
{
    public partial class Effect_FadeInOutMaterialColor : MonoBehaviour
    {
        public AnimationCurve curve = new AnimationCurve(new Keyframe(0, 1.0f), new Keyframe(1.0f, 1.0f));
        public float curve_DuartionTime = 0.03f;
        public float cure_DelayTime = 0.0f;
        public float curveScale = 1.0f;
        private bool bCanElvate = false;
        private float currDuatrtion = 0.0f;
        Coroutine waitingForStart_cure_cor = null;
        private void InitDefaultVariables_Curve()
        {
            foreach (Material mat in matInstances)
            {
                if (!mat)
                {
                    continue;
                }
                float val = curve.Evaluate(0) * curveScale;

                Color c = OrginalColorMap[mat] * val;
                if (!FadeAlpha)
                {
                    c.a = OrginalColorMap[mat].a;
                }
                mat.SetColor(ColorName, c);
                EffectUtily.UpdateColor(mat, ColorName, c);
            }
        }


        void Reset_Curve()
        {
            if(cure_DelayTime > 0.03f)
            {
                bCanElvate = false;
                waitingForStart_cure_cor = GameUtil.StartCoroutineHelper(this,WaitForStart_Cureve());
            }
            else
            {
                if (curve_DuartionTime < 0)
                {
                    curve_DuartionTime = 0.03f;
                }
                bCanElvate = true;
            }
            currDuatrtion = 0.0f;
        }


        void Update_Curve()
        {
            if(!bCanElvate)
            {
                return;
            }
            if(currDuatrtion < curve_DuartionTime)
            {
                currDuatrtion += Time.deltaTime;
                float time = currDuatrtion / curve_DuartionTime;
                float v = curve.Evaluate(time);
                v *= curveScale;
                Color CurrentColor;
                foreach (Material mat in matInstances)
                {
                    if(!mat)
                    {
                        continue;
                    }
                    CurrentColor = OrginalColorMap[mat];
                    bool bNeedToUpdateColor = false;
                    if (FadeAlpha)
                    {
                        CurrentColor.a *= v;
                        bNeedToUpdateColor = true;
                    }

                    if (FadeColor)
                    {
                        CurrentColor.r *= v;
                        CurrentColor.b *= v;
                        CurrentColor.g *= v;
                        bNeedToUpdateColor = true;
                    }

                    if (bNeedToUpdateColor)
                    {
                        mat.SetColor(ColorName, CurrentColor);
                        EffectUtily.UpdateColor(mat, ColorName, CurrentColor);
                    }

                }
            }
            else
            {
                Stop_Curve();
                if(Loop)
                {
                    Reset_Curve();
                }
                else
                {
                    bPlay = false;
                    InvokeFinishEvent();
                }

            }
        }

        void Stop_Curve()
        {
            if (null != waitingForStart_cure_cor)
            {
                StopCoroutine(waitingForStart_cure_cor);
            }
            currDuatrtion = 0.0f;
            bCanElvate = false;
        }

        IEnumerator WaitForStart_Cureve()
        {
            yield return new WaitForSeconds(cure_DelayTime);
            bCanElvate = true;
        }
    }
}

