using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace Spline
{
    [CustomEditor(typeof(SplineComponent))]
    class SplineComponentEditor : Editor
    {
        SplineComponent m_selectSpline;
        SplineComponent SelectSpline
        {
            get
            {
                if (m_selectSpline == null)
                {
                    m_selectSpline = target as SplineComponent;
                    m_selectSpline.Init();
                }

                return m_selectSpline;
            }
            set
            {
                if (m_selectSpline == null)
                {
                    m_selectSpline = value;
                    m_selectSpline.Init();
                }
            }
        }

        private int m_selectedIndex = -1;
        private int m_selectedTangentIndex = 0;

        private Transform handleTransform;
        private Quaternion handleRotation;
        private const float handleSize = 0.04f;
        private const float pickSize = 0.06f;

        public override void OnInspectorGUI()
        {
            //base.OnInspectorGUI();

            if (!SelectSpline.IsInit)
            {
                SelectSpline.Init();
            }

            EditorGUI.BeginChangeCheck();
            bool IsLoop = GUILayout.Toggle(SelectSpline.IsClosedLoop(), "是否循环");
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "样线循环");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetClosedLoop(IsLoop, true);
            }

            EditorGUI.BeginChangeCheck();
            SelectSpline.DrawDebug = GUILayout.Toggle(SelectSpline.DrawDebug, "是否绘制样线");
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "绘制样线");
                EditorUtility.SetDirty(SelectSpline);
                Repaint();
            }

            if (m_selectedIndex != -1)
            {
                DrawSelectedPointInspector();
            }

            if (GUILayout.Button("尾部加点"))
            {
                Undo.RecordObject(SelectSpline, "尾部加点");

                int lastIndex = SelectSpline.GetSplineCurvesPosition().Points.Count - 1;

                FSplinePoint newPoint = new FSplinePoint(
                 SelectSpline.GetSplineCurvesPosition().Points[lastIndex].InVal + 1,
                 SelectSpline.GetSplineCurvesPosition().Points[lastIndex].OutVal + new Vector3(10, 0),
                 SelectSpline.GetSplineCurvesPosition().Points[lastIndex].LeaveTangent
                 );
                SelectSpline.AddPoint(newPoint);
                EditorUtility.SetDirty(SelectSpline);
            }
        }


        void OnSceneGUI()
        {
            if (!SelectSpline.DrawDebug || !SelectSpline.IsInit)
                return;

            SelectSpline = target as SplineComponent;
            handleTransform = SelectSpline.transform;
            handleRotation = Tools.pivotRotation == PivotRotation.Local ?
        handleTransform.rotation : Quaternion.identity;


            // Vector3 p0 = ShowPoint(0);

            for (int i = 0; i < SelectSpline.GetNumberOfSplinePoints(); i++)
            {
                ShowTangentPoint(i);
                ShowPoint(i);
            }

        }

        private Vector3 ShowPoint(int index)
        {
            Vector3 point = SelectSpline.GetLocationAtSplineInputKey(index, ESplineCoordinateSpace.World);
            float size = 3;// HandleUtility.GetHandleSize(point);
            if (index == 0)
            {
                size *= 2f;
            }
            Handles.color = Color.green;//modeColors[(int)SelectSpline.GetSplinePointType(index)];

            if (Handles.Button(point, handleRotation, size * handleSize, size * pickSize, Handles.DotCap))
            {
                m_selectedIndex = index;
                m_selectedTangentIndex = 0;
                Repaint();
            }
            if (m_selectedIndex == index)
            {
                EditorGUI.BeginChangeCheck();
                point = Handles.DoPositionHandle(point, handleRotation);

                if (EditorGUI.EndChangeCheck())
                {
                    Undo.RecordObject(SelectSpline, "移动关键点");
                    EditorUtility.SetDirty(SelectSpline);
                    SelectSpline.SetLocationAtSplinePoint(index, point, ESplineCoordinateSpace.World);
                }
            }
            return point;
        }

        private void ShowTangentPoint(int index)
        {
            if (m_selectedIndex != index)
                return;
            Vector3 point = SelectSpline.GetLocationAtSplineInputKey(index, ESplineCoordinateSpace.World);
            float size = HandleUtility.GetHandleSize(point);

            Vector3 Location = SelectSpline.GetLocationAtSplinePoint(index, ESplineCoordinateSpace.World);
            Vector3 LeaveTangent = SelectSpline.GetLeaveTangentAtSplinePoint(index, ESplineCoordinateSpace.World);
            Vector3 ArriveTangent = SelectSpline.GetArriveTangentAtSplinePoint(index, ESplineCoordinateSpace.World);

            DrawTangent(index, Location, ArriveTangent, -1, size);
            DrawTangent(index, Location, LeaveTangent, 1, size);

            //Arrive:-1,Leave:1
            if (m_selectedTangentIndex == 1)
            {
                EditorGUI.BeginChangeCheck();
                LeaveTangent = Handles.DoPositionHandle(Location + LeaveTangent, handleRotation);

                if (EditorGUI.EndChangeCheck())
                {
                    Undo.RecordObject(SelectSpline, "移动离开切线点");
                    EditorUtility.SetDirty(SelectSpline);
                    SelectSpline.SetTangentsAtSplinePoint(index, ArriveTangent, LeaveTangent - Location, ESplineCoordinateSpace.World);
                }
            }
            //Arrive:-1,Leave:1
            if (m_selectedTangentIndex == -1)
            {
                EditorGUI.BeginChangeCheck();
                ArriveTangent = Handles.DoPositionHandle(Location - ArriveTangent, handleRotation);
                if (EditorGUI.EndChangeCheck())
                {
                    Undo.RecordObject(SelectSpline, "移动进入切线点");
                    EditorUtility.SetDirty(SelectSpline);
                    SelectSpline.SetTangentsAtSplinePoint(index, -1 * (ArriveTangent - Location), LeaveTangent, ESplineCoordinateSpace.World);
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="index"></param>
        /// <param name="Location"></param>
        /// <param name="Tangent"></param>
        /// <param name="Direction">Arrive:-1,Leave:1</param>
        /// <param name="HandlerSize"></param>
        void DrawTangent(int index, Vector3 Location, Vector3 Tangent, int Direction, float HandlerSize)
        {
            if (!SelectSpline.IsClosedLoop() && ((index == 0 && Direction == -1) || (index == SelectSpline.GetNumberOfSplinePoints() - 1 && Direction == 1)))
                return;

            Handles.color = Color.gray;
            Handles.DrawLine(Location, Location + Tangent * Direction);

            Handles.color = Color.yellow;
            if (Handles.Button(Location + Tangent * Direction, handleRotation, HandlerSize * handleSize, HandlerSize * pickSize, Handles.DotCap))
            {
                m_selectedTangentIndex = Direction;
                Repaint();
            }
        }

        private void DrawSelectedPointInspector()
        {
            GUILayout.Label("当前选择的关键点");
            int index = m_selectedIndex;
            EditorGUI.BeginChangeCheck();

            Vector3 point = EditorGUILayout.Vector3Field("Position", SelectSpline.GetLocationAtSplinePoint(index, ESplineCoordinateSpace.World));
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "Move Position");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetLocationAtSplinePoint(index, point, ESplineCoordinateSpace.World);
            }

            EditorGUI.BeginChangeCheck();
            Vector3 ArriveTangent = EditorGUILayout.Vector3Field("ArriveTangent", SelectSpline.GetArriveTangentAtSplinePoint(index, ESplineCoordinateSpace.World));
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "Move ArriveTangent");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetArriveTangentAtSplilnePoint(index, ArriveTangent, ESplineCoordinateSpace.World);
            }

            EditorGUI.BeginChangeCheck();
            Vector3 LeaveTangent = EditorGUILayout.Vector3Field("LeaveTangent", SelectSpline.GetLeaveTangentAtSplinePoint(index, ESplineCoordinateSpace.World));
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "Move LeaveTangent");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetLeaveTangentAtSplilnePoint(index, LeaveTangent, ESplineCoordinateSpace.World);
            }

            EditorGUI.BeginChangeCheck();
            ESplinePointType InterpMode = (ESplinePointType)EditorGUILayout.EnumPopup("InterpMode", SelectSpline.GetSplinePointType(index));
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "Change InterpMode");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetSplinePointType(index, InterpMode);
            }

            //EditorGUI.BeginChangeCheck();
            //Quaternion Rotation = EditorGUILayout.("Rotation", SelectSpline.GetQuaternionAtSplineInputKey((float)index,ESplineCoordinateSpace.World);
            //if (EditorGUI.EndChangeCheck())
            //{
            //    Undo.RecordObject(SelectSpline, "Change Rotation");
            //    EditorUtility.SetDirty(SelectSpline);
            //    SelectSpline.SetScaleAtSplinePoint(index, Rotation);
            //}

            EditorGUI.BeginChangeCheck();
            Vector3 Scale = EditorGUILayout.Vector3Field("Scale", SelectSpline.GetScaleAtSplinePoint(index));
            if (EditorGUI.EndChangeCheck())
            {
                Undo.RecordObject(SelectSpline, "Change Scale");
                EditorUtility.SetDirty(SelectSpline);
                SelectSpline.SetScaleAtSplinePoint(index, Scale);
            }
        }
    }

}
