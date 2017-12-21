
using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
 	[AddComponentMenu("UI/Extensions/Primitives/ULineRenderer")]
   public class ULineRenderer : UPrimitiveBase
    {
        private enum ESegmentType
        {
            EST_Start,
            EST_Middle,
            EST_End,
        }

        /// <summary>
        /// �߶����ӷ�ʽ
        /// </summary>
        public enum EJoinType
        {
            EJT_Bevel,//б��
            EJT_Miter//б��
        }
        public enum EBezierType
        {
            EBT_None,
            EBT_Quick,
            EBT_Basic,
            EBT_Improved,
        }

        private const float MIN_MITER_JOIN = 15 * Mathf.Deg2Rad;

        private const float MIN_BEVEL_NICE_JOIN = 30 * Mathf.Deg2Rad;

        private static readonly Vector2 UV_TOP_LEFT = Vector2.zero;
        private static readonly Vector2 UV_BOTTOM_LEFT = new Vector2(0, 1);
        private static readonly Vector2 UV_TOP_CENTER = new Vector2(0.5f, 0);
        private static readonly Vector2 UV_BOTTOM_CENTER = new Vector2(0.5f, 1);
        private static readonly Vector2 UV_TOP_RIGHT = new Vector2(1, 0);
        private static readonly Vector2 UV_BOTTOM_RIGHT = new Vector2(1, 1);

        private static readonly Vector2[] startUvs = new[] { UV_TOP_LEFT, UV_BOTTOM_LEFT, UV_BOTTOM_CENTER, UV_TOP_CENTER };
        private static readonly Vector2[] middleUvs = new[] { UV_TOP_CENTER, UV_BOTTOM_CENTER, UV_BOTTOM_CENTER, UV_TOP_CENTER };
        private static readonly Vector2[] endUvs = new[] { UV_TOP_CENTER, UV_BOTTOM_CENTER, UV_BOTTOM_RIGHT, UV_TOP_RIGHT };

        [SerializeField]
        protected Rect m_UVRect = new Rect(0f, 0f, 1f, 1f);
        /// <summary>
        /// ���㼯��
        /// </summary>
        [Tooltip("���㼯�ϣ���ǰRectTransform�����½�Ϊԭ��")]
        [SerializeField]
        protected Vector2[] m_points;
        /// <summary>
        /// ������ϸ
        /// </summary>
        [Tooltip("�ߴ�ϸ")]
        public float LineThickness = 2;
        /// <summary>
        /// ʹ��ҳ�߾�
        /// </summary>
        [Tooltip("�Ƿ�ʹ��ʹ��ҳ�߾�")]
        public bool UseMargins;
        /// <summary>
        /// ҳ�߾��С
        /// </summary>
        [Tooltip("����RectTransform��Ե����")]
        public Vector2 Margin;
        /// <summary>
        /// �Ƿ��� RectTransform��Width,Height ������
        /// </summary>
        [Tooltip("�Ƿ��� RectTransform��Width,Height ������")]
        public bool RelativeSize;
        /// <summary>
        /// �Ƿ����߶εķ�ʽ��ʾ
        /// </summary>
        [Tooltip("�Ƿ����߶εķ�ʽ��ʾ")]
        public bool LineList = false;
        /// <summary>
        /// �Ƿ��߶ε����˵�
        /// </summary>
        [Tooltip("�Ƿ��߶ε����˵�")]
        public bool LineCaps = false;
        /// <summary>
        /// �߶����ӷ�ʽ
        /// </summary>
        [Tooltip("�߶����ӷ�ʽ")]
        public EJoinType LineJoins = EJoinType.EJT_Bevel;

        public EBezierType BezierMode = EBezierType.EBT_None;
        public int BezierSegmentsPerCurve = 10;

        /// <summary>
        /// UV rectangle used by the texture.
        /// </summary>
        public Rect uvRect
        {
            get
            {
                return m_UVRect;
            }
            set
            {
                if (m_UVRect == value)
                    return;
                m_UVRect = value;
                SetVerticesDirty();
            }
        }

        /// <summary>
        /// ���߶���
        /// </summary>
        public Vector2[] Points
        {
            get
            {
                return m_points;
            }
            set
            {
                if (m_points == value)
                    return;
                m_points = value;
                SetAllDirty();
            }
        }

        protected override void OnPopulateMesh(VertexHelper vh)
        {
            if (m_points == null)
                return;
            Vector2[] pointsToDraw = m_points;
            //ʹ��Bezierģʽ���ߵĻ�
            if (BezierMode != EBezierType.EBT_None && m_points.Length > 3)
            {
                BezierPath bezierPath = new BezierPath();

                bezierPath.SetControlPoints(pointsToDraw);
                bezierPath.SegmentsPerCurve = BezierSegmentsPerCurve;
                List<Vector2> drawingPoints;
                switch (BezierMode)
                {
                    case EBezierType.EBT_Basic:
                        drawingPoints = bezierPath.GetDrawingPoints0();
                        break;
                    case EBezierType.EBT_Improved:
                        drawingPoints = bezierPath.GetDrawingPoints1();
                        break;
                    default:
                        drawingPoints = bezierPath.GetDrawingPoints2();
                        break;
                }
                pointsToDraw = drawingPoints.ToArray();
            }

            var sizeX = rectTransform.rect.width;
            var sizeY = rectTransform.rect.height;
            var offsetX = -rectTransform.pivot.x * rectTransform.rect.width;
            var offsetY = -rectTransform.pivot.y * rectTransform.rect.height;

            if (!RelativeSize)
            {
                sizeX = 1;
                sizeY = 1;
            }

            if (UseMargins)
            {
                sizeX -= Margin.x;
                sizeY -= Margin.y;
                offsetX += Margin.x / 2f;
                offsetY += Margin.y / 2f;
            }

            vh.Clear();

            // Generate the quads that make up the wide line
            var segments = new List<UIVertex[]>();
            if (LineList)
            {
                for (var i = 1; i < pointsToDraw.Length; i += 2)
                {
                    var start = pointsToDraw[i - 1];
                    var end = pointsToDraw[i];
                    start = new Vector2(start.x * sizeX + offsetX, start.y * sizeY + offsetY);
                    end = new Vector2(end.x * sizeX + offsetX, end.y * sizeY + offsetY);

                    if (LineCaps)
                    {
                        segments.Add(CreateLineCap(start, end, ESegmentType.EST_Start));
                    }

                    segments.Add(CreateLineSegment(start, end, ESegmentType.EST_Middle));

                    if (LineCaps)
                    {
                        segments.Add(CreateLineCap(start, end, ESegmentType.EST_End));
                    }
                }
            }
            else
            {
                for (var i = 1; i < pointsToDraw.Length; i++)
                {
                    var start = pointsToDraw[i - 1];
                    var end = pointsToDraw[i];
                    start = new Vector2(start.x * sizeX + offsetX, start.y * sizeY + offsetY);
                    end = new Vector2(end.x * sizeX + offsetX, end.y * sizeY + offsetY);

                    if (LineCaps && i == 1)
                    {
                        segments.Add(CreateLineCap(start, end, ESegmentType.EST_Start));
                    }

                    segments.Add(CreateLineSegment(start, end, ESegmentType.EST_Middle));

                    if (LineCaps && i == pointsToDraw.Length - 1)
                    {
                        segments.Add(CreateLineCap(start, end, ESegmentType.EST_End));
                    }
                }
            }

            //���߶ζ��������Ⱦ��������߶ε����ӿ�
            for (var i = 0; i < segments.Count; i++)
            {
                if (!LineList && i < segments.Count - 1)
                {
                    var vec1 = segments[i][1].position - segments[i][2].position;
                    var vec2 = segments[i + 1][2].position - segments[i + 1][1].position;
                    var angle = Vector2.Angle(vec1, vec2) * Mathf.Deg2Rad;

                    //������ζ����˳ʱ����ת
                    var sign = Mathf.Sign(Vector3.Cross(vec1.normalized, vec2.normalized).z);

                    // �����߶μ��б�ӵ�
                    var miterDistance = LineThickness / (2 * Mathf.Tan(angle / 2));
                    var miterPointA = segments[i][2].position - vec1.normalized * miterDistance * sign;
                    var miterPointB = segments[i][3].position + vec1.normalized * miterDistance * sign;

                    var joinType = LineJoins;
                    if (joinType == EJoinType.EJT_Miter)
                    {
                        // Make sure we can make a miter join without too many artifacts.
                        if (miterDistance < vec1.magnitude / 2 && miterDistance < vec2.magnitude / 2 && angle > MIN_MITER_JOIN)
                        {
                            segments[i][2].position = miterPointA;
                            segments[i][3].position = miterPointB;
                            segments[i + 1][0].position = miterPointB;
                            segments[i + 1][1].position = miterPointA;
                        }
                        else
                        {
                            joinType = EJoinType.EJT_Bevel;
                        }
                    }

                    if (joinType == EJoinType.EJT_Bevel)
                    {
                        if (miterDistance < vec1.magnitude / 2 && miterDistance < vec2.magnitude / 2 && angle > MIN_BEVEL_NICE_JOIN)
                        {
                            if (sign < 0)
                            {
                                segments[i][2].position = miterPointA;
                                segments[i + 1][1].position = miterPointA;
                            }
                            else
                            {
                                segments[i][3].position =  miterPointB;
                                segments[i + 1][0].position =  miterPointB;
                            }
                        }

                        var join = new UIVertex[] { segments[i][2], segments[i][3], segments[i + 1][0], segments[i + 1][1] };
                        vh.AddUIVertexQuad(join);
                    }
                }
                vh.AddUIVertexQuad(segments[i]);
            }
        }

        private UIVertex[] CreateLineCap(Vector2 start, Vector2 end, ESegmentType type)
        {
            if (type == ESegmentType.EST_Start)
            {
                var capStart = start - ((end - start).normalized * LineThickness / 2);
                return CreateLineSegment(capStart, start, ESegmentType.EST_Start);
            }
            else if (type == ESegmentType.EST_End)
            {
                var capEnd = end + ((end - start).normalized * LineThickness / 2);
                return CreateLineSegment(end, capEnd, ESegmentType.EST_End);
            }

            Debug.LogError("Bad ESegmentType passed in to CreateLineCap. Must be ESegmentType.EST_Start or ESegmentType.EST_End");
            return null;
        }

        private UIVertex[] CreateLineSegment(Vector2 start, Vector2 end, ESegmentType type)
        {
            var uvs = middleUvs;
            if (type == ESegmentType.EST_Start)
                uvs = startUvs;
            else if (type == ESegmentType.EST_End)
                uvs = endUvs;

            Vector2 offset = new Vector2(start.y - end.y, end.x - start.x).normalized * LineThickness / 2;
            var v1 = start - offset;
            var v2 = start + offset;
            var v3 = end + offset;
            var v4 = end - offset;
            return SetVbo(new[] { v1, v2, v3, v4 }, uvs);
        }
    }
}