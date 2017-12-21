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
    public sealed class RaycastQE 
        : IQueryExplorerState 
    {
        private NavGroup mHelper;

        private BufferHandler<uint> mPolyRefs;
        private float mHitParam;
        private Vector3 mHitNorm;
        private int mResultCount;

        private NavmeshPoint mPosition;
        private NavmeshPoint mGoal;

        private string mMessages = "";

        public QEStateType StateType
        {
            get { return QEStateType.Raycast; }
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
            mHelper = helper;
            mPosition.polyRef = 0;
            mGoal.polyRef = 0;

            SimGUIUtil.BuildLabelRegion(false);

            mPolyRefs = new BufferHandler<uint>("Buffer size"
                , 1, QEUtil.SmallBufferInit, 1, QEUtil.SmallBufferMax);

            mResultCount = 0;

            SimGUIUtil.contextControlZone.height = SimGUIUtil.LineHeight * 2.5f;
            SimGUIUtil.contextActive = true;
        }

        public void Exit()
        {
            mPolyRefs = null;

            SimGUIUtil.labels.Clear();
            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextActive = false;
        }

        public void Update()
        {
            if (mPolyRefs.HandleResize())
            {
                mResultCount = 0;
                mHitNorm = Vector3.zero;
                mHitParam = 0;
            }

            if (Input.GetKeyDown(StdButtons.SelectA)|| Input.GetKeyDown(StdButtons.SelectB))
            {
                Vector3 trasha;
                string trashb;
                NavmeshPoint point;

                QEUtil.SearchResult searchResult = 
                    QEUtil.HandleStandardPolySearch(mHelper, out trasha, out point, out trashb);

                if ((searchResult & QEUtil.SearchResult.HitNavmesh) == 0)
                    return;

                if (mPosition.polyRef == 0 || Input.GetKeyDown(StdButtons.SelectB))
                    mPosition = point;
                else
                    mGoal = point;

                mResultCount = 0;
                mHitNorm = Vector3.zero;
                mHitParam = 0;
            }

            if (mResultCount == 0 && mPosition.polyRef != 0 && mGoal.polyRef != 0)
                CastRay();
        }

        private void CastRay()
        {
            NavStatus status = mHelper.query.Raycast(mPosition, mGoal.point
                , mHelper.filter
                , out mHitParam, out mHitNorm, mPolyRefs.buffer, out mResultCount);

            mMessages = "Raycast: " + status.ToString();
        }

        public void OnGUI()
        {
            SimGUIUtil.contextHelpText = HelpText;

            GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);
            mPolyRefs.OnGUI();
            GUILayout.EndArea();

            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            const int LabelCount = 3;
            int slot = labels.SlotCount - 1 - LabelCount;

            if (mHitParam > 1)
                labels.Set(slot++, "Hit parameter", "Ray hit goal.");
            else
                labels.Set(slot++, "Hit parameter", string.Format("{0:F2}", mHitParam));

            labels.Set(slot++, "Hit normal", Vector3Util.ToString(mHitNorm));
            labels.Set(slot++, "Path length", mResultCount.ToString());

            labels.Last = mMessages;
        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mPolyRefs.buffer, mResultCount);

            Color c = QEUtil.SelectColor;
            c.a = 0.25f;

            if (mPosition.polyRef != 0)
                DebugDraw.DiamondMarker(mPosition.point, QEUtil.MarkerScaleNorm, c);

            c = NavDebug.goalColor;
            c.a = 0.25f;

            if (mGoal.polyRef != 0)
                DebugDraw.DiamondMarker(mGoal.point, QEUtil.MarkerScaleNorm, c);

            if (mResultCount > 0)
            {
                if (mHitParam < 1)
                {
                    Vector3 hitPoint = 
                        mPosition.point + (mGoal.point - mPosition.point) * mHitParam;

                    DebugDraw.Arrow(mPosition.point, hitPoint
                        , 0, QEUtil.HeadScaleNorm
                        , NavDebug.goalColor);

                    DebugDraw.Arrow(hitPoint, hitPoint + mHitNorm * 0.5f
                        , 0, QEUtil.HeadScaleSm
                        , NavDebug.goalColor);
                }
                else
                {
                    DebugDraw.Arrow(mPosition.point, mGoal.point
                        , 0, QEUtil.HeadScaleNorm
                        , NavDebug.goalColor);
                }
            }
        }
    }
}
