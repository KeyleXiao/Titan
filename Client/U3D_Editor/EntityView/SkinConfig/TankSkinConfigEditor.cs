/*******************************************************************
** 文件名:	TankSkinConfigEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	皮肤配置脚本
** 应  用:  	皮肤配置脚本编辑器，tank

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using UnityEditor;
namespace Effect
{
    [CustomEditor(typeof(TankSkinConfig))]
    public class TankSkinConfigEditor : EntitySkinConfigEditor
    {

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
            EditorGUILayout.HelpBox("必须根据实际情况拖拽RightWeaponRender、" +
                                    "LeftWeraponRender、BodyRnder!" +
                                    "必须根据实际情况填写CacheCout，比如弓兵，同时存在" +
                                    "的数目大概会达到30个，这个值要填30！", MessageType.Info);
        }
    }

}
