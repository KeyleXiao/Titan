using System;
using System.Collections;
using System.Threading;
using UnityEngine;

namespace Effect
{
    public class Effect_TransformModify : MonoBehaviour
    {
        public ModifyInfo3 PositionInfo = new ModifyInfo3();
        public ModifyInfo3 RoationInfo = new ModifyInfo3();
        public ModifyInfo3 ScaleInfo = new ModifyInfo3();


        public Vector3 orginalPos = Vector3.zero;
        public Quaternion orginalRot = Quaternion.identity;
        public Vector3 orginalScale = Vector3.one;

        private Vector3 reslutRot = Vector3.zero;
        private bool bPlay = false;

        public void Bake()
        {
            orginalPos = transform.localPosition;
            orginalRot = transform.localRotation;
            orginalScale = transform.localScale;
        }

        public void Init()
        {
            bPlay = false;
        }

        public void Play()
        {
            bPlay = true;
            transform.localPosition = orginalPos;
            transform.localRotation = orginalRot;
            transform.localScale = orginalScale;
            reslutRot = transform.localEulerAngles;
            PositionInfo.Reset();
            RoationInfo.Reset();
            ScaleInfo.Reset();
        }

        public void Stop()
        {
            bPlay = false;

        }

        public void LateUpdate()
        {
            if (!bPlay)
            {
                return;
            }
            transform.localPosition = PositionInfo.Evaluate(transform.localPosition);
            transform.localScale = ScaleInfo.Evaluate(transform.localScale);
            reslutRot = RoationInfo.Evaluate(reslutRot);
            transform.localRotation = Quaternion.Euler(reslutRot);
        }


    }
}
