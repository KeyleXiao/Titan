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
    public sealed class GetNearestPointG
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private Vector3 mSearchPoint;
        private NavmeshPoint mNavmeshPoint;
        private bool mHasSearchPoint = false;
        private bool mFoundPoly = false;

        private uint[] mSelectedPoly = new uint[1]; // For debug draw.

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetNearestPointG; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;
            SimGUIUtil.BuildLabelRegion(false);

            mHasSearchPoint = false;
            mFoundPoly = false;

            SimGUIUtil.contextHelpText = string.Format("Scale Search: [{0}] [{1}] [{2}] [{3}]"
                , StdButtons.AdjustXZMinus, StdButtons.AdjustXZPlus
                , StdButtons.AdjustYMinus, StdButtons.AdjustYPlus);

            SimGUIUtil.contextActive = true;
        }

        public void Exit()
        {
            SimGUIUtil.labels.Clear();
            SimGUIUtil.contextActive = false;
        }

        public void Update()
        {
            float offset = QEUtil.GetXZFactor();
            mHelper.extents[0] += offset;
            mHelper.extents[1] += QEUtil.GetYFactor();
            mHelper.extents[2] += offset;

            mHasSearchPoint = false;
            mFoundPoly = false;

            QEUtil.SearchResult result = QEUtil.HandleStandardPolySearch(
                mHelper, out mSearchPoint, out mNavmeshPoint, out mMessage);

            mFoundPoly = (result & QEUtil.SearchResult.HitNavmesh) != 0;
            mHasSearchPoint = mFoundPoly || ((result & QEUtil.SearchResult.HitGeometry) != 0);
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 3;
            int iSlot = labels.SlotCount - 1 - LabelCount;

            const string PointLabel = "Point on mesh";

            if (mHasSearchPoint)
            {
                labels.Set(iSlot++, "PolyRef", mNavmeshPoint.polyRef.ToString());

                if (mFoundPoly)
                    labels.Set(iSlot++, PointLabel, Vector3Util.ToString(mNavmeshPoint.point));
                else
                    labels.Set(iSlot++, PointLabel, "Unknown");
            }
            else
                iSlot += 2;

            labels.Set(iSlot++, "Extents", Vector3Util.ToString(mHelper.extents));
        }

        public void OnRenderObject()
        {
            Color c = NavDebug.goalColor;
            c.a = 0.25f;

            if (mFoundPoly)
            {
                mSelectedPoly[0] = mNavmeshPoint.polyRef;
                NavDebug.Draw(mHelper.mesh, mSelectedPoly, 1);

                DebugDraw.DiamondMarker(mNavmeshPoint.point, QEUtil.MarkerScaleNorm, c);
            }
            else
                NavDebug.Draw(mHelper.mesh, false);

            if (mHasSearchPoint)
            {
                c = (mHelper.extents[0] < 0 || mHelper.extents[1] < 0) 
                    ? Color.red 
                    : QEUtil.SelectColor;

                DebugDraw.Extents(mSearchPoint, mHelper.extents, c);
            }
        }
    }
}
