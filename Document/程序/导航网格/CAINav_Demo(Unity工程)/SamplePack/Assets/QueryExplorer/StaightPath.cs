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
    public sealed class StraightPath 
        : FindPath
    {
        private string mLocalMessage = "";
        private int mPathId;

        private NavmeshPoint mTarget;
        private BufferHandler<Vector3> mStraightPath;
        private int mStraightCount;
        private int mTargetPathIndex;

        public override QEStateType StateType
        {
            get { return QEStateType.StraightPath; }
        }

        public override string HelpText
        {
            get
            {
                if (pathCount > 0)
                    return base.HelpText + ", Set Position: " + StdButtons.SelectB;
                else
                    return base.HelpText;
            }
        }

        protected override int LabelCount
        {
            get { return base.LabelCount + 1; }
        }

        public override void Enter(NavGroup helper)
        {
            base.Enter(helper);

            mTarget.polyRef = 0;
            mStraightCount = 0;
            mStraightPath = new BufferHandler<Vector3>("Straight Path Buffer"
                , 1, QEUtil.SmallBufferInit, 1, QEUtil.SmallBufferMax);

            SimGUIUtil.contextControlZone.height += SimGUIUtil.LineHeight * 2.5f;
        }

        public override void Exit()
        {
            mStraightPath = null;
            base.Exit();
        }

        public override void Update()
        {
            base.Update();

            if (pathCount == 0 || pathID != mPathId)
            {
                mStraightCount = 0;
                mTarget.polyRef = 0;
            }

            if (mStraightPath.HandleResize())
                mStraightCount = 0;

            mPathId = pathID;

            if (pathCount == 0)
                return;

            if (Input.GetKeyDown(StdButtons.SelectB))
            {
                QEUtil.SearchResult searchResult;
                Vector3 trasha;
                string trashb;
                NavmeshPoint point;

                // Path start.
                searchResult = 
                    QEUtil.HandleStandardPolySearch(helper, out trasha, out point, out trashb);

                if ((searchResult & QEUtil.SearchResult.HitNavmesh) == 0)
                    return;

                bool found = false;
                int iStartPoly = 0;
                for (; iStartPoly < pathCount; iStartPoly++)
                {
                    if (point.polyRef == path.buffer[iStartPoly])
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                    mTargetPathIndex = iStartPoly;
                else
                {
                    mTargetPathIndex = -1;
                    mLocalMessage = "Target outside path.";
                }


                mTarget = point;

                mStraightCount = 0;
            }

            if (mTarget.polyRef == 0)
                mLocalMessage = "No target";
            else if (mStraightCount == 0)
                GenerateStraightPath();

        }

        private void GenerateStraightPath()
        {
            int iTarget = Mathf.Max(0, mTargetPathIndex);
            NavStatus status = helper.query.GetStraightPath(mTarget.point, goal.point
                , path.buffer, iTarget, pathCount - iTarget
                , mStraightPath.buffer, null, null, out mStraightCount);

            if (mLocalMessage.Length > 0)
                mLocalMessage += ", ";

            mLocalMessage = "GetStraightPath: " + status.ToString();
        }

        protected override void HandleControls()
        {
            base.HandleControls();
            mStraightPath.OnGUI();
        }

        protected override void HandleLabels()
        {
            base.HandleLabels();
            AppendMessage(mLocalMessage);
            AddLabel("Straight Path Length", mStraightCount.ToString());
        }

        public override void OnRenderObject()
        {
            base.OnRenderObject();

            Color c = NavDebug.positionColor;
            c.a = 0.25f;

            if (mTarget.polyRef != 0)
                DebugDraw.DiamondMarker(mTarget.point, QEUtil.MarkerScaleNorm, c);

            if (mStraightCount > 0)
            {
                for (int i = 1; i < mStraightCount; i++)
                {
                    DebugDraw.Arrow(mStraightPath.buffer[i - 1], mStraightPath.buffer[i]
                        , 0, QEUtil.HeadScaleSm
                        , NavDebug.positionColor);
                }
            }
        }
    }
}
