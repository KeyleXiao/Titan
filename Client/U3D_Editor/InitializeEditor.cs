/*******************************************************************
** 文件名:	
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;

[CustomEditor(typeof(Initialize))]
public class InitializeEditor : Editor
{
    private Initialize m_Instance;
    public void OnEnable()
    {
        m_Instance = this.target as Initialize;
    }
    public override void OnInspectorGUI()
    {

        base.OnInspectorGUI();
    }
}
