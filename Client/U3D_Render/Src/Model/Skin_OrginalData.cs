/*******************************************************************
** 文件名:	Skin_OrginalData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤结点
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/19
** 描  述:  增加缓存皮肤函数
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 皮肤结点，通过这个来控制皮肤的所有功能
/// </summary>
public partial class Skin
{
    struct SkinOrginalData
    {
        /// <summary>
        /// Main点的缩放
        /// </summary>
        public Vector3 mainBonesScale;
    }

    /// <summary>
    /// 去的原始数据
    /// </summary>
    private bool bGetOrginalData = false;
    private SkinOrginalData OrginalData;
    public void ApplyOrginalData(SkinInstance instance)
    {
        if (SkinInstance.isNullOrEmpry(instance))
        {
            return;
        }

        Transform mainTrs = instance.GetBonesTransform("Main");
        if (mainTrs)
        {
            mainTrs.localScale = OrginalData.mainBonesScale;
        }
    }

    public void SaveOrginalData(SkinInstance instance)
    {
        if (bGetOrginalData)
        {
            return;
        }
        if (SkinInstance.isNullOrEmpry(instance))
        {
            return;
        }

        bGetOrginalData = true;
        OrginalData = new SkinOrginalData();
        OrginalData.mainBonesScale = Vector3.one;
        Transform mainTrs = instance.GetBonesTransform("Main");
        if (mainTrs)
        {
            OrginalData.mainBonesScale = mainTrs.localScale;
        }
    }

    private void ClearSaveOrginalData()
    {
        bGetOrginalData = false;
    }
}