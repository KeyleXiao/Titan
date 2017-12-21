using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;

namespace UnityEditor.UI
{
    [CustomEditor(typeof(Dropdown), true)]
    [CanEditMultipleObjects]
    public class UDropdownEditor : DropdownEditor
    {
        SerializedProperty m_Template;

        protected override void OnEnable()
        {
            base.OnEnable();
            m_Template = serializedObject.FindProperty("m_Template");
            if((m_Template.objectReferenceValue as RectTransform) != null)
            {
                USpeedUILayerElement element = (m_Template.objectReferenceValue as RectTransform).GetComponent<USpeedUILayerElement>();
                if (element == null)
                    element = (m_Template.objectReferenceValue as RectTransform).gameObject.AddComponent<USpeedUILayerElement>();
                element.UILayerElementType = USpeedUILayerElement.EUILayerElementType.ELET_UI;
                element.IsUseSortingLayerOffset = false;
                element.IsUIRequirRaycaster = true;
            }
        }
    }
}
