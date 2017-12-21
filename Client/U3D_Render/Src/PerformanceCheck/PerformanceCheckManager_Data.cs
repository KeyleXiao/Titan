/*******************************************************************
** 文件名:	PerformanceCheckManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:   性能检测脚本
** 应  用:  	跟随物体

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class PerformanceCheckManager : MonoBehaviour
{
    public class PerformanceCheckReslut
    {

        /// <summary>
        /// 没有做性能检测
        /// </summary>
        public bool NotDoPerformanceCheck = false;

        /// <summary>
        /// 不支持RenderTexture
        /// </summary>
        public bool notSupportRT = false;

        /// <summary>
        /// 不支持高等级
        /// </summary>
        public bool notSupportHighLevel = false;

        /// <summary>
        /// 几何渲染等级，分别为1，2，3，4，对应低，中，高，极高
        /// </summary>
        public int GeometricRenderLevel = 1;

        /// <summary>
        /// 后处理渲染级别，分别为1，2，3，4，对应低，中，高，极高
        /// </summary>
        public int ImageEffectRenderLevel = 1;

        /// <summary>
        /// 很差的电脑
        /// </summary>
        public bool VeryLowMachine = false;
    }

    public PerformanceCheckReslut ReslutData = new PerformanceCheckReslut();
}



