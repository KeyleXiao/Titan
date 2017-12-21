/*
 * Copyright (c) 2011-2012 Stephen A. Pratt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
using UnityEngine;
using org.critterai.nav.u3d;
using org.critterai.u3d;
using org.critterai.geom;
using org.critterai.nav;

namespace org.critterai.samples.qe
{
    public sealed class GetSegments
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private uint[] mSelectedPoly = new uint[1];
        private Vector3[] mSegments = new Vector3[2 * Navmesh.MaxAllowedVertsPerPoly];
        private uint[] mSegmentRefs = new uint[Navmesh.MaxAllowedVertsPerPoly];
        private int mSegmentCount = 0;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetSegments; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;
            SimGUIUtil.BuildLabelRegion(false);

            mSelectedPoly[0] = 0;
            mSegmentCount = 0;

            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextControlZone.height = 0;
            SimGUIUtil.contextActive = true;
        }

        public void Exit()
        {
            SimGUIUtil.labels.Clear();
            SimGUIUtil.contextActive = false;
        }

        public void Update()
        {
            mSelectedPoly[0] = 0;
            mSegmentCount = 0;

            Vector3 trash;
            NavmeshPoint pt;

            QEUtil.SearchResult result =
                QEUtil.HandleStandardPolySearch(mHelper, out trash, out pt, out mMessage);

            if ((result & QEUtil.SearchResult.HitNavmesh) == 0)
                return;

             mSelectedPoly[0] = pt.polyRef;

             NavStatus status = mHelper.query.GetPolySegments(mSelectedPoly[0]
                 , mHelper.filter
                 , mSegments, mSegmentRefs, out mSegmentCount);

             mMessage = "GetPolySegments: " + status.ToString();
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 3;
            int slotIndex = labels.SlotCount - 1 - LabelCount;

            int portal = 0;
            int wall = 0;

            for (int i = 0; i < mSegmentCount; i++)
            {
                if (mSegmentRefs[i] == 0)
                    wall++;
                else
                    portal++;
            }

            labels.Set(slotIndex++, "Wall segments", wall.ToString());
            labels.Set(slotIndex++, "Protal segments", portal.ToString());
            labels.Set(slotIndex++, "Total segments", mSegmentCount.ToString());

            if (mSegmentCount > 0)
            {
                Rect rect = new Rect(Input.mousePosition.x - 5
                    , Screen.height - Input.mousePosition.y - 25
                    , 100, SimGUIUtil.LineHeight + 5);

                GUI.Label(rect, mSegmentCount.ToString());
            }
        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mSelectedPoly, 1);

            if (mSegmentCount == 0)
                return;

            DebugDraw.SimpleMaterial.SetPass(0);

            GL.Begin(GL.LINES);
            GL.Color(QEUtil.SelectColor);

            for (int i = 0; i < mSegmentCount; i++)
            {
                int p = i * 2;

                GL.Vertex(mSegments[p + 0]);
                GL.Vertex(mSegments[p + 1]);

                if (mSegmentRefs[i] == 0)
                    continue;

                Vector2 a = new Vector2(mSegments[p + 0].x, mSegments[p + 0].z);
                Vector2 b = new Vector2(mSegments[p + 1].x, mSegments[p + 1].z);

                Vector2 norm2 = Line2.GetNormalAB(a, b);
                Vector3 norm3 = new Vector3(norm2.x, 0, norm2.y) * -0.25f;

                Vector3 mid = new Vector3((a.x + b.x) / 2
                    , (mSegments[p + 0].y + mSegments[p + 1].y) / 2
                    , (a.y + b.y) / 2);

                DebugDraw.AppendArrow(mid, mid + norm3, 0, QEUtil.HeadScaleSm);
            }

            GL.End();
        }

    }
}
