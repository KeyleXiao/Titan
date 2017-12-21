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
    public sealed class FindDistanceToWall
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private NavmeshPoint mPosition;
        private Vector3 mWallNormal;
        private Vector3 mWallHitPoint;
        private float mSearchRadius = 1.0f;
        private bool mHasWallHit = false;
        private bool mHasPosition = false;
        private float mWallDistance;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.FindDistanceToWall; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;
            SimGUIUtil.BuildLabelRegion(false);

            mHasPosition = false;
            mHasWallHit = false;

            SimGUIUtil.contextHelpText = string.Format("Scale Search Radius: [{0}] [{1}]"
                , StdButtons.AdjustXZMinus, StdButtons.AdjustXZPlus);

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
            mHasPosition = false;
            mHasWallHit = false;
            mWallDistance = 0;

            mSearchRadius += QEUtil.GetXZFactor();

            Vector3 trash;
            QEUtil.SearchResult result =
                QEUtil.HandleStandardPolySearch(mHelper, out trash, out mPosition, out mMessage);

            mHasPosition = (result & QEUtil.SearchResult.HitNavmesh) != 0;

            if (!mHasPosition) 
                return;

            NavStatus status = mHelper.query.FindDistanceToWall(mPosition, mSearchRadius
                , mHelper.filter
                , out mWallDistance, out mWallHitPoint, out mWallNormal);

            mMessage = "FindDistanceToWall: " + status.ToString() + ".";

            if ((status & NavStatus.Sucess) == 0)
                return;

            if (mWallDistance == mSearchRadius)
                mMessage += " Nearest wall is outside search radius.";
            else
                mHasWallHit = true;
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 4;
            int slot = labels.SlotCount - 1 - LabelCount;

            if (mHasPosition)
            {
                Rect rect = new Rect(Input.mousePosition.x - 5
                    , Screen.height - Input.mousePosition.y - 25
                    , 100, SimGUIUtil.LineHeight + 5);

                GUI.Label(rect, string.Format("{0:F3}", mWallDistance));

                labels.Set(slot++, "Wall Distance", string.Format("{0:F3}", mWallDistance));
                labels.Set(slot++, "Wall Point", Vector3Util.ToString(mWallHitPoint));
                labels.Set(slot++, "Wall Normal", Vector3Util.ToString(mWallNormal));
                labels.Set(slot++, "Search Radius", string.Format("{0:F2}", mSearchRadius));
            }
        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, false);

            if (mHasPosition)
                DebugDraw.Circle(mPosition.point, mSearchRadius, QEUtil.SelectColor);

            if (mHasWallHit)
            {
                DebugDraw.DiamondMarker(mWallHitPoint, QEUtil.MarkerScaleSm, Color.green);

                Vector3 normal = (mWallHitPoint + mWallNormal);
                DebugDraw.Arrow(mWallHitPoint, normal, 0, QEUtil.HeadScaleSm, NavDebug.goalColor);
            }
        }
    }
}
