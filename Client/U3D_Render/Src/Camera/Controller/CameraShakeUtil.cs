/*******************************************************************
** 文件名:	CameraShakeUtil.cs
** 描  述:	摄像机震屏光效播放工具类（同时只能播放一个摄像机光效，单例！！）
** 应  用:  	
            处理摄像机震屏光效播放：
            StartShake
            HandleCameraShake
********************************************************************/
using UnityEngine;

namespace U3D_Render.CameraControl
{

    class CameraShakeUtil
    {

        private CameraShakeContext shakeContex;
        private CameraEffectUtil camEffectUtil;
        //镜头抖动
        public bool shake;
        private float shakeSpeed = 2.0f;
        private float cShakePos;
        private int shakeTimes = 8;
        private float cShake;
        private float cShakeSpeed;
        private int cShakeTimes;

        private float maxShakeFov = 45;
        private float minShakeFov = 45.0f;
        private float shakeFovSpeed = 0.0f;
        private float oldCShake;

        public void Reset()
        {
            cShakeTimes = 0;
            cShake = 0.0f;
            cShakeSpeed = shakeSpeed;
            shake = false;
        }

        #region 镜头抖动效果

        public void Init(CameraShakeContext contex,CameraEffectUtil effectUtil)
        {
            shakeContex = contex;
            camEffectUtil = effectUtil;
        }

        public void StartShake(float distance0)
        {
            float proximity = distance0 / shakeContex.maxShakeDistance;

            if (proximity > 1.0f) return;

            proximity = Mathf.Clamp(proximity, 0.0f, 1.0f);

            proximity = 1.0f - proximity;

            cShakeSpeed = Mathf.Lerp(shakeContex.minShakeSpeed, shakeContex.maxShakeSpeed, proximity);
            shakeTimes = (int)Mathf.Lerp(shakeContex.minShakeTimes, shakeContex.maxShakeTimes, proximity);
            cShakeTimes = 0;
            cShakePos = Mathf.Lerp(shakeContex.minShake, shakeContex.maxShake, proximity);
            this.oldCShake = 0f;
            shake = true;
        }

        public Vector3 HandleCameraShake(Vector3 tran)
        {
            if (this.shake)
            {
                if (shakeContex.isRandom)
                {
                    this.cShake += this.cShakeSpeed * camEffectUtil.deltaTime;
                    if (Mathf.Abs(this.cShake) > this.cShakePos)
                    {
                        this.cShakeSpeed *= -1f;
                        this.cShakeTimes++;
                        if (this.cShakeTimes >= this.shakeTimes)
                        {
                            this.shake = false;
                        }
                        if (this.cShake > 0.0)
                        {
                            this.cShake = this.cShakePos;
                        }
                        else
                        {
                            this.cShake = -this.cShakePos;
                        }
                    }
                    tran.y += this.cShake;
                    camEffectUtil.m_effectCurFov += this.shakeFovSpeed * camEffectUtil.deltaTime;
                    if (((this.shakeFovSpeed > 0f) && (camEffectUtil.m_effectCurFov > this.maxShakeFov)) || ((this.shakeFovSpeed < 0f) && (camEffectUtil.m_effectCurFov < this.minShakeFov)))
                    {
                        this.shakeFovSpeed *= -1f;
                    }
                    return tran;
                }
                else
                {
                    uint num = GameLogicAPI.getTickCount();
                    if (num >= camEffectUtil.m_effectStartTick)
                    {
                        if (num >= camEffectUtil.m_effectStopTick)
                        {
                            camEffectUtil.stopEffect();
                            return tran;
                        }
                        uint effectStartTick = camEffectUtil.m_effectStartTick;
                        if (camEffectUtil.m_nCameraAxisIndex >= 1)
                        {
                            effectStartTick = (uint)camEffectUtil.m_cameraAxis[camEffectUtil.m_nCameraAxisIndex - 1].tick;
                        }
                        if (num <= camEffectUtil.m_nextEffectNodeTick)
                        {
                            if (camEffectUtil.m_nextEffectNodeTick <= num)
                            {
                                camEffectUtil.m_nextEffectNodeTick = num + 1;
                            }
                            float totalTime = ((float)(camEffectUtil.m_nextEffectNodeTick - effectStartTick)) / 1000f;
                            float time = ((float)(num - effectStartTick)) / 1000f;
                            //Debug.Log(time);
                            this.cShake = this.GetShakeValue(time, totalTime);
                            Vector3 soldierForwardDir = SoldierCamera.MainInstance().GetSoliderGo().transform.forward;
                            tran += (Vector3)((this.cShake - this.oldCShake) * Vector3.Normalize(shakeContex.shakeDir + (shakeContex.isDirDependRole ? soldierForwardDir : Vector3.zero)));
                            this.oldCShake = this.cShake;
                            return tran;
                        }
                        this.shake = false;
                    }
                }
            }
            return tran;
        }

        //获得震屏某一帧的摄像机位移参数
        private float GetShakeValue(float time, float totalTime)
        {
            float frequence = ((this.shakeTimes * 2) * 3.141593f) / (totalTime * this.cShakeSpeed);
            float t_min = (((2f * this.shakeTimes) - 0.5f) * 3.141593f) / frequence;
            float sin_min = Mathf.Sin(t_min * frequence);
            float log_val = Mathf.Log(shakeContex.minShake / (shakeContex.maxShake * Mathf.Abs(sin_min)));
            float exponent = -t_min / log_val;
            float t = time;
            float left = Mathf.Exp(-t / exponent);
            float right = Mathf.Sin(frequence * t);
            if (Mathf.Abs(right) <= 0.001f)
            {
                right = 0f;
            }
            return ((shakeContex.maxShake * left) * right);
        }
        #endregion
    }
}
