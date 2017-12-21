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
    public sealed class MoveAlongSurface
        : IQueryExplorerState 
    {
        private NavGroup mHelper;

        private string mMessages = "";

        private NavmeshPoint mPosition;
        private NavmeshPoint mGoal;
        private NavmeshPoint mTarget;
        private BufferHandler<uint> mPolyRefs;
        private Vector3[] mStraightPath;
        private int mVisitedCount = 0;
        private int mStraightCount = 0;

        public QEStateType StateType
        {
            get { return QEStateType.MoveOnSurface; }
        }

        private string HelpText
        {
            get
            {
                if (mPosition.polyRef == 0)
                    return string.Format("Set start: [{0}]", StdButtons.SelectA);
                else
                {
                    return string.Format("Marker: [{0}], Move start: [{1}]"
                        , StdButtons.SelectA, StdButtons.SelectB);
                }
            }
        }

        public void Enter(NavGroup helper)
        {
            this.mHelper = helper;
            mPosition.polyRef = 0;
            mGoal.polyRef = 0;

            SimGUIUtil.BuildLabelRegion(false);

            mPolyRefs = new BufferHandler<uint>("Visited Buffer Size"
                , 1, QEUtil.SmallBufferInit, 1, QEUtil.SmallBufferMax);

            mStraightPath = new Vector3[mPolyRefs.MaxElementCount];

            SimGUIUtil.contextControlZone.height = SimGUIUtil.LineHeight * 2.5f;
            SimGUIUtil.contextActive = true;
        }

        public void Exit()
        {
            SimGUIUtil.labels.Clear();
            mPolyRefs = null;
            mStraightPath = null;
            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextActive = false;
        }

        public void Update()
        {
            if (mPolyRefs.HandleResize())
            {
                mStraightPath = new Vector3[mPolyRefs.MaxElementCount];
                mVisitedCount = 0;
                mStraightCount = 0;
            }

            if (Input.GetKeyDown(StdButtons.SelectA)
                || Input.GetKey(StdButtons.SelectB))
            {
                Vector3 trasha;
                string trashb;
                NavmeshPoint point;

                QEUtil.SearchResult searchResult = 
                    QEUtil.HandleStandardPolySearch(mHelper, out trasha, out point, out trashb);

                if ((searchResult & QEUtil.SearchResult.HitNavmesh) == 0)
                    return;

                if (mPosition.polyRef == 0 || Input.GetKey(StdButtons.SelectB))
                {
                    mPosition = point;
                    mGoal.polyRef = 0;
                }
                else
                    mGoal = point;

                mVisitedCount = 0;
                mStraightCount = 0;
            }

            if (mVisitedCount == 0
                && mPosition.polyRef != 0
                && mGoal.polyRef != 0)
            {
                NavStatus status = mHelper.query.MoveAlongSurface(mPosition, mGoal.point
                    , mHelper.filter
                    , out mTarget.point, mPolyRefs.buffer, out mVisitedCount);

                mMessages = "MoveAlongSurface: " + status.ToString();

                if (mVisitedCount > 0)
                {
                    mHelper.query.GetStraightPath(mPosition.point, mTarget.point
                        , mPolyRefs.buffer, 0, mVisitedCount
                        , mStraightPath
                        , null, null, out mStraightCount);
                }
            }
        }


        public void OnGUI()
        {
            SimGUIUtil.contextHelpText = HelpText;

            GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);
            mPolyRefs.OnGUI();
            GUILayout.EndArea();

            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            const int LabelCount = 1;
            int slot = labels.SlotCount - 1 - LabelCount;

            labels.Set(slot++, "Visited Polys", mVisitedCount.ToString());

            labels.Last = mMessages;
        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mPolyRefs.buffer, mVisitedCount);

            Color c = QEUtil.SelectColor;
            c.a = 0.25f;

            if (mPosition.polyRef != 0)
                DebugDraw.DiamondMarker(mPosition.point, QEUtil.MarkerScaleNorm, c);

            c = NavDebug.positionColor;
            c.a = 0.25f;

            if (mGoal.polyRef != 0)
                DebugDraw.DiamondMarker(mGoal.point, QEUtil.MarkerScaleNorm, c);

            c = NavDebug.goalColor;
            c.a = 0.25f;

            if (mVisitedCount > 0)
                DebugDraw.DiamondMarker(mTarget.point, QEUtil.MarkerScaleNorm, c);

            if (mStraightCount > 0)
            {
                for (int i = 1; i < mStraightCount; i++)
                {
                    DebugDraw.Arrow(mStraightPath[i - 1], mStraightPath[i]
                        , 0, QEUtil.HeadScaleNorm
                        , NavDebug.goalColor);
                }
            }
        }
    }
}
