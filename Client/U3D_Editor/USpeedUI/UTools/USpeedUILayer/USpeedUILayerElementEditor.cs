using DG.DOTweenEditor.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEditor.UI;
using UnityEngine;

namespace USpeedUI
{
    [CustomEditor(typeof(USpeedUILayerElement))]
    [CanEditMultipleObjects]
    class USpeedUILayerElementEditor : SelectableEditor
    {
        USpeedUILayerElement targetUILayerElement;
        protected override void OnEnable()
        {
            targetUILayerElement = target as USpeedUILayerElement;
            if (targetUILayerElement)
            {
                targetUILayerElement.RegisterUIElement();
            }
        }

        public override void OnInspectorGUI()
        {
            targetUILayerElement = target as USpeedUILayerElement;
            EditorGUIUtils.SetGUIStyles();
            if (targetUILayerElement != null)
            {
                targetUILayerElement.SetSortingOrder();

                EditorGUILayout.Space();
                targetUILayerElement.UILayerElementType = (USpeedUILayerElement.EUILayerElementType)EditorGUILayout.EnumPopup("类型：",targetUILayerElement.UILayerElementType);
                switch (targetUILayerElement.UILayerElementType)
                {
                    case USpeedUILayerElement.EUILayerElementType.ELET_UI:
                        OnInspectorGUI_UI();
                        break;
                    case USpeedUILayerElement.EUILayerElementType.ELET_Effect:
                        OnInspectorGUI_Effect();
                        break;
                    default:
                        break;
                }

                EditorGUILayout.Space();

                DrawSortingLayer();

                EditorGUILayout.Space();
                if (GUILayout.Button("编辑模式下移动到其他Layer时点下我"))
                    targetUILayerElement.TransformParentChanged();
                EditorGUILayout.Space();
            }
        }

        private void OnInspectorGUI_UI()
        {
            targetUILayerElement.IsUIRequirRaycaster = EditorGUILayout.Toggle("是否需要UI射线检测", targetUILayerElement.IsUIRequirRaycaster);
        }
        private void OnInspectorGUI_Effect()
        {
        }

        private void DrawSortingLayer()
        {
            targetUILayerElement.IsUseSortingLayerOffset = EditorGUILayout.Toggle("是否设置层级偏移值", targetUILayerElement.IsUseSortingLayerOffset);
            if (targetUILayerElement.IsUseSortingLayerOffset)
            {
                EditorGUILayout.Space();
                GUILayout.BeginHorizontal();
                if (GUILayout.Button("-"))
                {
                    targetUILayerElement.SortingLayerOrderOffset--;
                }
                GUILayout.Label(targetUILayerElement.SortingLayerOrderOffset.ToString(), EditorGUIUtils.wrapCenterLabelStyle);
                if (GUILayout.Button("+"))
                {
                    targetUILayerElement.SortingLayerOrderOffset++;
                }
                if (GUILayout.Button("归零"))
                    targetUILayerElement.SortingLayerOrderOffset = 0;
                GUILayout.EndHorizontal();
                EditorGUILayout.LabelField("当前所在Layer的序号：", USpeedUILayerManager.CalculateLayerElementOrder(targetUILayerElement).ToString());
                    //(targetUILayerElement.SortingLayerOrder + targetUILayerElement.SortingLayerOrderOffset + targetUILayerElement.SortingLayrOrderParentOffset).ToString());
            }
            else
            {
                //if((targetUILayerElement.SortingLayerOrder + targetUILayerElement.SortingLayerOrderOffset + targetUILayerElement.SortingLayrOrderParentOffset > 0))
                if(USpeedUILayerManager.CalculateLayerElementOrder(targetUILayerElement) > 0)
                {
                    targetUILayerElement.SortingLayerOrder = 0;
                    targetUILayerElement.SortingLayerOrderOffset = 0;
                }
            }
        }
    }

    [CustomEditor(typeof(USpeedUILayer))]
    class USpeedUILayerEditor : Editor
    {
        USpeedUILayer targetUILayer;

        public void OnEnable()
        {
            targetUILayer = target as USpeedUILayer;
            if (targetUILayer)
                targetUILayer.RegisterUILayer();
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
        }
    }
}
