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
using org.critterai.nav;

namespace org.critterai.samples.qe
{
    public sealed class GetWallSegments
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private uint[] mSelectedPoly = new uint[1];
        private Vector3[] mSegments = new Vector3[2 * Navmesh.MaxAllowedVertsPerPoly];
        private int mSegmentCount = 0;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetWallSegments; }
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

            NavmeshPoint pt;
            Vector3 trash;

            QEUtil.SearchResult result =
                QEUtil.HandleStandardPolySearch(mHelper, out trash, out pt, out mMessage);

            if ((result & QEUtil.SearchResult.HitNavmesh) == 0)
                return;

             mSelectedPoly[0] = pt.polyRef;

             NavStatus status = mHelper.query.GetPolySegments(mSelectedPoly[0]
                 , mHelper.filter
                 , mSegments, out mSegmentCount);

             mMessage = "GetPolySegments: " + status.ToString();
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 1;
            int slotIndex = labels.SlotCount - 1 - LabelCount;

            labels.Set(slotIndex++, "Wall segments", mSegmentCount.ToString());

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
            }

            GL.End();

        }

    }
}
