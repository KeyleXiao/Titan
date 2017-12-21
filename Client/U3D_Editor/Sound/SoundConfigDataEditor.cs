/*******************************************************************
** 文件名:	SoundConfigDataEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音配置编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;

namespace SoundSystem
{
    //[CustomEditor(typeof(SoundConfigData))]
    //public class SoundConfigDataEditor : Editor
    //{
        //SoundConfigData pa;
    //public void OnEnable()
        //{
        //    pa = target as SoundConfigData;
            
        //}
        //public override void OnInspectorGUI()
        //{
        //    pa.nID = EditorGUILayout.IntField("音效ID:", pa.nID);
        //    pa.nPlayTimes = EditorGUILayout.IntField("播放次数:", pa.nPlayTimes);
        //    pa.b3DSound = EditorGUILayout.Toggle("是否3D声音:", pa.b3DSound);
        //    pa.sourceClip = EditorGUILayout.ObjectField("声音剪辑:", pa.sourceClip, typeof(AudioClip), false) as AudioClip;
        //}
    //}
}
