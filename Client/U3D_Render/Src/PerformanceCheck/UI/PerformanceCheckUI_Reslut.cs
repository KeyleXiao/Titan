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
using UnityEngine.UI;
public class PerformanceCheckUI_Reslut : MonoBehaviour
{
    public Text tips = null;
    public GameObject ConfigOneRoot = null;
    public Button ConfigOneBtn = null;
    public Text ConfigOneBtnDesc = null;
    public Text ConfigOneDesc = null;

    public GameObject ConfigTwoRoot = null;
    public Button ConfigTwoBtn = null;
    public Text ConfigTwoBtnDesc = null;
    public Text ConfigTwoDesc = null;
    public void Init()
    {
        ConfigOneRoot.SetActive(false);
        ConfigTwoRoot.SetActive(false);
    }

    public void Clear()
    {

    }
}



