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
using ASpeedGame.Data.PlayerSettingLocalConfig;
using UnityEngine.UI;

public partial class PerformanceCheckImageEffect : MonoBehaviour
{
    public void OnRenderImage(RenderTexture sour,RenderTexture dest)
    {
        Graphics.Blit(sour, dest);
    }
}



