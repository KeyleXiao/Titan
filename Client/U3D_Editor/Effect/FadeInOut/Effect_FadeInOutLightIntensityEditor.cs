/*******************************************************************
** 文件名:	Effect_FadeInOutLightIntensity.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/8/4
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	灯光强度淡入淡出编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_FadeInOutLightIntensity))]
    public class Effect_FadeInOutLightIntensityEditor : Editor
    {
        Effect_FadeInOutLightIntensity pa;
        public void OnEnable()
        {
            pa = target as Effect_FadeInOutLightIntensity;
        }

        public override void OnInspectorGUI()
        {
            GUILayout.TextField("所有跟时间有关的变量小于0.03秒，等同于无效!");
            pa.HostLight = EditorGUILayout.ObjectField("灯光:", pa.HostLight,typeof(Light), true) as Light;
            pa.Loop = EditorGUILayout.Toggle("循环:", pa.Loop);

            GUI.enabled = !EditorApplication.isPlaying;
            pa.valueMode = (ValueMode)EditorGUILayout.EnumPopup("运算方式:", pa.valueMode);
            GUI.enabled = true;

            if (pa.valueMode == ValueMode.Constant)
            {
                ConstantUI();
            }
            else if (pa.valueMode == ValueMode.Curve)
            {
                CurveUI();
            }

            if (GUILayout.Button("Play"))
            {
                pa.Play();
            }
            if (GUILayout.Button("Stop"))
            {
                pa.Stop();
            }
        }


        void ConstantUI()
        {
            pa.FadeInDealy = EditorGUILayout.FloatField("淡入延时:", pa.FadeInDealy);
            pa.FadeInDuartion = EditorGUILayout.FloatField("淡入持续时间:", pa.FadeInDuartion);
            pa.FadeInFrom = EditorGUILayout.FloatField("淡入初始:", pa.FadeInFrom);
            pa.FadeInTo = EditorGUILayout.FloatField("淡入结束:", pa.FadeInTo);
            pa.FadeOutDelay = EditorGUILayout.FloatField("淡出延时:", pa.FadeOutDelay);
            pa.FadeOutDuartion = EditorGUILayout.FloatField("淡出持续时间:", pa.FadeOutDuartion);
            pa.FadeOutFrom = EditorGUILayout.FloatField("淡出初始:", pa.FadeOutFrom);
            pa.FadeOutTo = EditorGUILayout.FloatField("淡出结束:", pa.FadeOutTo);

        }

        void CurveUI()
        {
            pa.cure_DelayTime = EditorGUILayout.FloatField("开始延时:", pa.cure_DelayTime);
            pa.curve_DuartionTime = EditorGUILayout.FloatField("持续时间(必须大于0.03):", pa.curve_DuartionTime);
            if (pa.curve_DuartionTime < 0.03)
            {
                pa.curve_DuartionTime = 0.03f;
            }
            pa.curveScale = EditorGUILayout.FloatField("缩放:", pa.curveScale);
            pa.curve = EditorGUILayout.CurveField("曲线:", pa.curve);
        }
    }
}
