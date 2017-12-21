/*******************************************************************
** 文件名:	SkinManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤管理器
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
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager 
{

    /// <summary>
    /// 皮肤模型信息
    /// </summary>
    private class SkinModelInfo
    {
        /// <summary>
        /// 皮肤ID
        /// </summary>
        public int nSkinID;

        /// <summary>
        /// 皮肤类型，指能够用在哪种实体上
        /// </summary>
        public ENTITY_TYPE SkinType;

        public float fSkinScale;

        /// <summary>
        /// 皮肤路径
        /// </summary>
        public string strPatch;

    }
}


