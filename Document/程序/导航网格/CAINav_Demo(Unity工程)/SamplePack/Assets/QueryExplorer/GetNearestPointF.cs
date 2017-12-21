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
    public sealed class GetNearestPointF
        : IQueryExplorerState 
    {
        private NavGroup mHelper;

        private float mYOffset = 1;

        private uint[] mPolyRef = new uint[1];  // Used for debug view.

        private NavmeshPoint mPosition;
        private NavmeshPoint mNearest;
        private bool mHasPosition = false;
        private bool mHasNearestPoint = false;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetNearestPointF; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;
            SimGUIUtil.BuildLabelRegion(false);

            mHasPosition = false;
            mHasNearestPoint = false;

            SimGUIUtil.contextHelpText = string.Format(
                "Select Poly: [{0}], Translate Y: [{1}] [{2}]"
                , StdButtons.SelectA, StdButtons.AdjustYMinus, StdButtons.AdjustYPlus);

            SimGUIUtil.contextControlZone.height = 0;
            SimGUIUtil.contextActive = true;
        }

        public void Exit()
        {
            SimGUIUtil.labels.Clear();
            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextActive = false;
        }

        public void Update()
        {
            // Must set message before returning.

            mHasPosition = false;
            mHasNearestPoint = false;

            if (Input.GetKeyDown(StdButtons.SelectA))
            {
                Vector3 trash;
                QEUtil.SearchResult result =
                    QEUtil.HandleStandardPolySearch(mHelper, out trash, out mNearest, out mMessage);

                mHasPosition = (result & QEUtil.SearchResult.HitGeometry) != 0;
                if ((result & QEUtil.SearchResult.HitNavmesh) == 0)
                    // Could not find polygon.
                    mPolyRef[0] = 0;
                else if (mNearest.polyRef == mPolyRef[0])
                    // Toggle off the selection.
                    mPolyRef[0] = 0;
                else
                    // New good selection.
                    mPolyRef[0] = mNearest.polyRef;
            }

            if (mPolyRef[0] == 0)
            {
                mMessage = "No polygon selected.";
                return;
            }

            RaycastHit hit;

            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            if (!Physics.Raycast(ray, out hit, 100))
            {
                mMessage = "Outside source geometry.";
                return;
            }

            mPosition.point = hit.point;
            mHasPosition = true;

            mYOffset += QEUtil.GetYFactor();

            mPosition.point.y += mYOffset;

            NavStatus status = mHelper.query.GetNearestPointF(
                mNearest.polyRef, mPosition.point, out mNearest.point);

            mMessage = "GetNearestPointF: " + status.ToString();

            if (NavUtil.Failed(status))
                return;

            mHasNearestPoint = true;
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 2;
            int slotIndex = labels.SlotCount - 1 - LabelCount;

            if (mHasPosition)
                labels.Set(slotIndex++, "Position", Vector3Util.ToString(mPosition.point));
            else
                labels.Set(slotIndex++, "Position", "None");

            if (mHasNearestPoint)
                labels.Set(slotIndex++, "Nearest Point", Vector3Util.ToString(mNearest.point));
            else
                labels.Set(slotIndex++, "Nearest Point", "None");
        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mPolyRef, 1);

            Color c = QEUtil.SelectColor;
            c.a = 0.25f;

            if (mHasPosition)
                DebugDraw.DiamondMarker(mPosition.point, QEUtil.MarkerScaleNorm, c);

            if (mHasNearestPoint)
            {
                c = NavDebug.goalColor;
                c.a = 0.25f;

                DebugDraw.DiamondMarker(mNearest.point, QEUtil.MarkerScaleNorm, c);

                DebugDraw.Arrow(mPosition.point, mNearest.point
                    , 0, QEUtil.HeadScaleNorm
                    , NavDebug.goalColor);
            }
        }
    }
}
