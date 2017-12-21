/*******************************************************************
** 文件名:	EntitySkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;


namespace U3D_Render.SkinCtrl
{
    public class SkinControlAsyncLoadingObj
    {
        public bool loadFinish = false;
        public int nSkinID = EntitySkinControl.InvalidSkinID;

        public void Clear()
        {
            loadFinish = false;
            nSkinID = EntitySkinControl.InvalidSkinID;
        }
    }

    /// <summary>
    /// 实体皮肤控制
    /// </summary>
    public partial class EntitySkinControl: MonoBehaviour
    {
        private  Dictionary<int, SkinControlAsyncLoadingObj> allocedAsyncObjTable = new Dictionary<int, SkinControlAsyncLoadingObj>();

        public SkinControlAsyncLoadingObj AllocSkinControlAsyncLoadingObj(int nSkinID)
        {
            SkinControlAsyncLoadingObj result = null;
            if (!allocedAsyncObjTable.TryGetValue(nSkinID, out result))
            {
                result = new SkinControlAsyncLoadingObj();
                result.Clear();
                allocedAsyncObjTable.Add(nSkinID, result);
            }

            return result;
        }

        public void ClearAllocSkinControlAsyncLoadingObj()
        {
            allocedAsyncObjTable.Clear();
        }
    }
}
