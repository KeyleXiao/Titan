using System;
using System.Collections;
using System.Threading;
using UnityEngine;

namespace Effect
{
    [RequireComponent(typeof(Projector))]
    public class Effect_ProjectorModify : MonoBehaviour
    {

        public ModifyInfo ProjectorRoationInfo = new ModifyInfo();
        public ModifyInfo ProjectorSizeInfo = new ModifyInfo();
        public Vector3 OrginalProjectorInfo = Vector3.zero;
        private bool bPlay = false;

        Projector proj = null;
        Vector3 reslut = Vector3.zero;
        public void Bake()
        {
            proj = GetComponent<Projector>();
            OrginalProjectorInfo.x = transform.localEulerAngles.y;
            OrginalProjectorInfo.y = proj.orthographicSize;
        }

        public void Init()
        {
            bPlay = false;
            proj = GetComponent<Projector>();
        }

        public void Play()
        {
            bPlay = true;
            if (!proj)
            {
                proj = GetComponent<Projector>();
            }
            transform.localRotation = Quaternion.Euler(new Vector3(transform.localEulerAngles.x,OrginalProjectorInfo.y,transform.localEulerAngles.z));
            proj.orthographicSize = OrginalProjectorInfo.y;

            ProjectorRoationInfo.Reset();
            ProjectorSizeInfo.Reset();
            reslut.x = OrginalProjectorInfo.x;
            reslut.y = OrginalProjectorInfo.y;
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
            if(!proj)
            {
                proj = GetComponent<Projector>();
            }
            reslut.x = ProjectorRoationInfo.Evaluate(reslut.x);
            Vector3 rot = transform.localEulerAngles;
            rot.y = reslut.x;
            transform.localRotation = Quaternion.Euler(rot);

            reslut.y = ProjectorSizeInfo.Evaluate(reslut.y);
            proj.orthographicSize = reslut.y;
            
        }

    }
}
