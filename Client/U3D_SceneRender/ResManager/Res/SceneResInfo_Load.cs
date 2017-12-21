/*******************************************************************
** 文件名:	SceneResInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfo : SceneInfo
{
    private static bool bPreaLoaded = false;

    /// <summary>
    /// 总共要加载多少个maintex
    /// </summary>
    private static int NeedToPreLoadedAllTexCount = 0;
    private static int currPreLoadedTexCount = 0;

    public static void StartPreLoad()
    {
        bPreaLoaded = false;
        currPreLoadedTexCount = 0;
        NeedToPreLoadedAllTexCount = 0;
    }

    /// <summary>
    /// 清除提前加载资源
    /// </summary>
    public static void ClearPreLoadedRes()
    {
        bPreaLoaded = true;
        currPreLoadedTexCount = 0;
        NeedToPreLoadedAllTexCount = 0;
    }

    /// <summary>
    /// 预加载是否完成
    /// </summary>
    /// <returns></returns>
    public static bool isPreLoadFinish()
    {
        //看看maintex加载完成了没
        if (currPreLoadedTexCount != NeedToPreLoadedAllTexCount &&
           NeedToPreLoadedAllTexCount != 0)
        {
            return false;
        }
        
        return true;
    }




}
