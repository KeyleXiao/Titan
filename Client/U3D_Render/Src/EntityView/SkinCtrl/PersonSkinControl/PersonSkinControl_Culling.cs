/*******************************************************************
** 文件名:	PersonSkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	人物实体皮肤控制
********************************************************************/
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render.SkinCtrl
{
    /// <summary>
    /// 人物皮肤控制
    /// </summary>
    public partial class PersonSkinControl : EntitySkinControl
    {

        private Vector3 lastPos = Vector3.zero;

        public override void ResetCull()
        {
            OnCull();
            base.ResetCull();         
        }

        private void UpdatePositionToSkinManager()
        {
            if (currentSkinId == InvalidSkinID)
            {
                return;
            }
            //if(null == hostQuadTreeNode)
            //{
            //    return;
            //}

            if(!isActiveAndEnabled)
            {
                return;
            }

            if(Time.frameCount % 4 != 0)
            {
                return;
            }

            if( GameUtil.GetSqrDistanceWithOutY(lastPos,transform.position) > 0.025f)
            {
                lastPos = transform.position;
                SkinManager.UpdateSkinControl(this);
            }

        }

        public override void OnCull()
        {
            if (!SkinManager.enabledSkinCull)
            {
                return;
            }
            if (currentSkinId == InvalidSkinID)
            {
                return;
            }

            if(isCull)
            {
                return;
            }
            isCull = true;
            RemoveCallBack(currentSkinId);
            InvokeSkinCullEvent();
            int currentValidSkinID = InvalidSkinID;

            if (m_currentLoadedSkinInstance != null && m_currentLoadedSkinInstance.isValid)
            {
                currentValidSkinID = m_currentLoadedSkinInstance.nSkinID;
                SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
                SkinManager.DeleteSkinAsync(currentValidSkinID, OnSkinLoadFinish, NotReleaseFromCache);
               
                m_currentLoadedSkinInstance = null;
            }

            if (currentValidSkinID != currentSkinId)
            {
                SkinManager.DeleteSkinAsync(currentSkinId, OnSkinLoadFinish, NotReleaseFromCache);
            }

            ClearPersonSkinDataFromMachine(m_machine);
        }

        public override void OnNotCull()
        {
            if (!SkinManager.enabledSkinCull)
            {
                return;
            }

            if (currentSkinId == InvalidSkinID)
            {
                return;
            }

            if (!isCull)
            {
                return;
            }
            isCull = false;
            if(null != m_currentLoadedSkinInstance)
            {
                Debug.LogError("皮肤切换重大bug");
                return;
            }

            if(currentSkinId == InvalidSkinID)
            {
                return;
            }
			if (!SkinManager.GetSkinAsync(currentSkinId, OnSkinLoadFinish))
            {
                RemoveCallBack(currentSkinId);
                return;
            }
            PushToCallBack(currentSkinId, -1);
            InvokeSkinNotCullEvent();
        }

    }
}
