/*******************************************************************
** 文件名:	UpdateBehaviour.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	梁成
** 日  期:	2016-2-2
** 版  本:	1.0
** 描  述:	控制MonoBehaviour的更新行为

 * 1、通过继承本类
 * 2、在原来的类中不要直接使用Update函数，用下面的函数来替代
 * 3、可以重载下面的函数来处理相对应的逻辑		
********************************************************************/
using UnityEngine;
using System;


public class UpdateBehaviour : MonoBehaviour
{

    /// <summary>
    /// 用于替换Update函数执行逻辑
    /// </summary>
    public virtual void OnUpdate()
    {

    }

    /// <summary>
    /// 用于替换LateUpdate函数执行逻辑
    /// </summary>
    public virtual void OnLateUpdate()
    {

    }

    /// <summary>
    /// 用于替换FixedUpdate函数执行逻辑
    /// </summary>
    public virtual void OnFixedUpdate()
    {

    }

    /// <summary>
    /// 用于替换OnGUI函数执行逻辑
    /// </summary>
    public virtual void OnFrameGUI()
    {

    }


    /// <summary>
    /// 用于替换OnDestroy函数执行逻辑
    /// </summary>
    public virtual void OnFrameDestroy()
    {

    }
}