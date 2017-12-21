using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    public class Effect_LocalMove : MonoBehaviour
    {
        public float startDelay = 0.0f;
        public float moveTime = 1.0f;

        public AnimationCurve xPath = new AnimationCurve();
        public AnimationCurve yPath = new AnimationCurve();
        public AnimationCurve zPath = new AnimationCurve();

        public bool bEnableX = false;
        public bool bEnableY = false;
        public bool bEnableZ = false;

        bool bInit = false;
        private bool bPlay = false;
        private bool bStartPlayYield = false;
        private float playTime = 0.0f;
        private Vector3 orginalPosition = Vector3.one;

        public void Start()
        {
            Init();
        }

        public void Init()
        {
            if (!bInit)
            {
                orginalPosition = transform.localPosition;
                bInit = true;
            }
        }

        public void Play()
        {
            if (!bInit || bPlay)
                return;

            if (!bStartPlayYield)
            {
                if (startDelay > 0.03f)
                {
                    GameUtil.StartCoroutineHelper(this,DelayPlay());
                    bStartPlayYield = true;
                }
                else
                {
                    PlayImpl();
                }
            }
        }

        public void Stop()
        {
            bPlay = false;
            bStartPlayYield = false;
            transform.localPosition = orginalPosition;
            StopAllCoroutines();
        }

        IEnumerator DelayPlay()
        {
            yield return new WaitForSeconds(startDelay);
            bStartPlayYield = false;
            PlayImpl();
        }

        private void PlayImpl()
        {
            bPlay = true;
            playTime = 0.0f;
            transform.localPosition = orginalPosition;
        }

        public void Update()
        {
            if (!bInit || !bPlay || moveTime <= 0 || playTime > moveTime)
                return;

            playTime += Time.deltaTime;

            Vector3 pos = orginalPosition;
            if (bEnableX)
                pos.x += xPath.Evaluate(playTime);
            if (bEnableY)
                pos.y += yPath.Evaluate(playTime);
            if (bEnableZ)
                pos.z += zPath.Evaluate(playTime);

            transform.localPosition = pos;
        }
    }
}
