/*******************************************************************
** 文件名:	ULocalizedTextEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	林建聪
** 日  期:	2016/3/30
** 版  本:	1.0
** 描  述:	方便在编辑状态下显示多语言文件
** 应  用:  
 *          
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/

using System;
using System.Linq;
using UnityEditor;
using UnityEngine;
using USpeedUI;
using UnityEngine.UI;

[CustomEditor(typeof(ULocalizedText), true)]
public class ULocalizedTextEditor : Editor
{
    protected ULocalizedText Target;
    protected bool m_bUseStringForFilter;
    protected string m_strStringForFilter;

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        Target = target as ULocalizedText;

        if (Application.isPlaying)
            return;

        var service = ULocalizationService.Instance;

        if (service == null || service.LocalizeDataTable == null || service.LocalizeDataTable.Count == 0)
        {
            // ViewConfigManager.Init();
            ULocalizationService.Instance.Load(ViewConfigManager.Editor_DataScpPath + ULocalizationService.LocalizationConfigFileName);

            var p = EditorGUILayout.TextField("Key", Target.Key);

            if (p != Target.Key)
            {
                Target.Key = p;
                EditorUtility.SetDirty(target);
            }

            EditorGUILayout.LabelField("Error ", "LocalizationService Not Found");
        }
        else
        {
            //切换语言
            var languages = service.Languages.Select(o => o.Name).ToArray();
            var langIndex = Array.IndexOf(languages, service.Language.Name);
            var selectLanguages = EditorGUILayout.Popup("Language", langIndex, languages);
            if(langIndex != selectLanguages)
            {
                service.Language = service.Languages.FirstOrDefault(o => o.Name == languages[selectLanguages]);
                
                EditorUtility.SetDirty(target);
            }
            //切换语言后，File，Filter，Key有可能都不变，所以这里重新取一次
            if (!string.IsNullOrEmpty(Target.Key))
            {
                Target.Value = service.Get(Target.File, Target.Filter, Target.Key);
            }

            //切换语言文本
            var files = service.LocalizeDataTable.Select(o => o.Key).ToArray();
            var fileIndex = Array.IndexOf(files, Target.File);
            
            var selectFile = EditorGUILayout.Popup("File", fileIndex, files);

            if (fileIndex == -1)
                Target.File = string.Empty;
            if (selectFile != fileIndex)
            {
                Target.File = files[selectFile];// fileIndex != -1 ? files[selectFile] :  string.Empty; 
                EditorUtility.SetDirty(target);
            }


            //切换组
            if (!string.IsNullOrEmpty(Target.File))
            {
                var filters = service.LocalizeDataTable[Target.File].Select(o => o.Key).ToArray();
                var filterIndex = Array.IndexOf(filters, Target.Filter);


                m_bUseStringForFilter = EditorGUILayout.Toggle("是否用字符串形式", m_bUseStringForFilter);
                int selectFilter = -1;
                if(m_bUseStringForFilter)
                {
                    m_strStringForFilter = Target.Filter;
                    m_strStringForFilter = m_strStringForFilter = EditorGUILayout.TextField("Filter", m_strStringForFilter);
                    selectFilter = Array.IndexOf(filters, m_strStringForFilter);

                    //没找到,那就按他输入的来处理
                    if (selectFilter == -1)
                        Target.Filter = m_strStringForFilter;
                }
                else
                {
                    selectFilter = EditorGUILayout.Popup("Filter", filterIndex, filters);
                }

                if (filterIndex == -1)
                    Target.Filter = string.Empty;
                if (filterIndex != selectFilter)
                {
                    Target.Filter = filters[selectFilter];
                    EditorUtility.SetDirty(target);
                }

                //切换Key
                if(!string.IsNullOrEmpty(Target.Filter))
                {
                    var keys = service.LocalizeDataTable[Target.File][Target.Filter].Select(o => o.Key).ToArray();
                    var keyIndex = Array.IndexOf(keys, Target.Key);

                    var selectKey = EditorGUILayout.Popup("Key", keyIndex, keys);
                    if (keyIndex == -1)
                    {
                        Target.Key = string.Empty;
                    }
                    if (keyIndex != selectKey)
                    {

                       Target.Key = keys[selectKey];
                       Target.Value = service.Get(Target.File, Target.Filter, Target.Key);

                       EditorUtility.SetDirty(target);
                    }
                }
            }

        }

        Target.Fallback = EditorGUILayout.TextField("FallBack ", Target.Fallback);

        if (!string.IsNullOrEmpty(Target.Value))
        {
            EditorGUILayout.LabelField("Value ", Target.Value);

            ///重新设置值的时候不知道怎么让控件重绘，先用这么挫的方法...
            if(Target.LocalizedText != null)
            {
                Target.LocalizedText.text = Target.Value;
                Target.LocalizedText.enabled = false;
                Target.LocalizedText.enabled = true;
            }
        }

        if (GUILayout.Button("重置文本所以属性"))
        {
            Target.File = Target.Filter = Target.Key = Target.Value = string.Empty;
        }
        if(GUILayout.Button("重加载当前语言"))
        {
            service.ReLoad();
        }
    }

}

