/*******************************************************************
** 文件名:	Effect_FollowObject.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	跟随物体

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
    /// 跟随物体,他必须在Effect_Play和delayPlay之后调用
    /// </summary>
    public class Effect_FollowObject : MonoBehaviour
    {
        public Transform TargetObj = null;
        public bool FollowPosition = true;
        public bool FollowAngle = false;
        public bool FollowScale = false;

        public Vector3 orginalPosition = Vector3.zero;
        public Vector3 orginalScale = Vector3.zero;
        public Quaternion orginalRot = Quaternion.identity;
        public void OnDestroy()
        {
            TargetObj = null;
        }

        public void Bake()
        {
            orginalPosition = transform.localPosition;
            orginalScale = transform.localScale;
            orginalRot = transform.localRotation;
        }

        public void Init()
        {

        }

        public void LateUpdate()
        {
            if(TargetObj)
            {
                if (FollowPosition)
                {
                    transform.SetPosition( TargetObj.position);
                }

                if (FollowAngle)
                {
                    transform.rotation = TargetObj.rotation;
                }

                if (FollowScale)
                {
                    transform.localScale = TargetObj.localScale;
                }
            }
        }

        public void Play()
        {
            if(TargetObj)
            {
                enabled = true;
            }

        }


        public void Stop()
        {
            enabled = false;
            if (TargetObj)
            {
                if (FollowPosition)
                {
                    transform.SetLocalPosition(orginalPosition);
                }

                if (FollowAngle)
                {
                    transform.localRotation = orginalRot;
                }

                if (FollowScale)
                {
                    transform.localScale = orginalScale;
                }

            }
        }

    }
}


