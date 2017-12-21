/*******************************************************************
** 文件名:	Effect_Random3D.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	随机3D信息,不支持粒子系统的

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;


namespace Effect
{
    /// <summary>
    /// 随机3D信息
    /// </summary>
    public class Effect_Random3D : MonoBehaviour
    {
        bool bInit = false;

        private Vector3 OrginalScale = Vector3.one;
        private Vector3 OrginalPos = Vector3.one;
        private Quaternion OrginalAngle = Quaternion.identity;

        public bool RandomScale = false;
        public Vector3 MinScale = Vector3.one;
        public Vector3 MaxScale = Vector3.one * 2;

        public bool RandomAngle = false;
        public Vector3 MinAngle = Vector3.zero;
        public Vector3 MaxAngle = Vector3.one * 90;

        public bool RandomPos = false;
        public Vector3 MinPos = Vector3.zero;
        public Vector3 MaxPos = Vector3.one;
        public void Start()
        {
            Init();
        }

        public void Init()
        {
            if(!bInit)
            {
                OrginalScale = transform.localScale;
                OrginalAngle = transform.localRotation;
                OrginalPos = transform.localPosition;
                bInit = true;
            }

        }

        public void Play()
        {
            PlayerImpl();
        }

        private void PlayerImpl()
        {
            if(RandomScale)
            {
                Vector3 scale = Vector3.zero;
                scale.x = Random.Range(MinScale.x, MaxScale.x);
                scale.y = Random.Range(MinScale.y, MaxScale.y);
                scale.z = Random.Range(MinScale.z, MaxScale.z);

                transform.localScale = scale;
            }

            if(RandomAngle)
            {
                Vector3 ang = Vector3.zero;
                ang.x = Random.Range(MinAngle.x, MaxAngle.x);
                ang.y = Random.Range(MinAngle.y, MaxAngle.y);
                ang.z = Random.Range(MinAngle.z, MaxAngle.z);

                transform.localRotation = Quaternion.Euler(ang);
            }

            if (RandomPos)
            {
                Vector3 pos = Vector3.zero;
                pos.x = Random.Range(MinPos.x, MaxPos.x);
                pos.y = Random.Range(MinPos.y, MaxPos.y);
                pos.z = Random.Range(MinPos.z, MaxPos.z);

                transform.localPosition = pos;
            }
        }

        public void Stop()
        {
            transform.localRotation = OrginalAngle;
            transform.localScale = OrginalScale;
            transform.localPosition = OrginalPos;
        }

    }
}


