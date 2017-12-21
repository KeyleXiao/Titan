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
    public class Effect_BillBoard : MonoBehaviour
    {
        public Vector3 OrginalAngle = Vector3.zero;
        Camera m_Cam = null;
        public void LateUpdate()
        {
            if(!Initialize.mainCam)
            {
                m_Cam = Camera.main;
            }
            else
            {
                m_Cam = Initialize.mainCam;
            }
            if(!m_Cam)
            {
                return;
            }

            transform.rotation = m_Cam.transform.rotation * Quaternion.Euler(OrginalAngle);
        }
    }
}


