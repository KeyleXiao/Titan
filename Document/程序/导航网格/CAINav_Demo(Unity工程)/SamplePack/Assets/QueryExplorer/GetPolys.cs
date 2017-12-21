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
    public sealed class GetPolys
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private Vector3 mPosition;
        private bool mHasPosition = false;

        private BufferHandler<uint> mPolyRefs;
        private int mResultCount = 0;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetPolys; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;

            SimGUIUtil.BuildLabelRegion(false);

            mPolyRefs = new BufferHandler<uint>("Search Buffer"
                , 1, QEUtil.SmallBufferInit, 1, QEUtil.SmallBufferMax);

            mResultCount = 0;

            SimGUIUtil.contextHelpText = string.Format("Scale Search: [{0}] [{1}] [{2}] [{3}]" 
                , StdButtons.AdjustXZMinus, StdButtons.AdjustXZPlus
                , StdButtons.AdjustYMinus, StdButtons.AdjustYPlus);

            SimGUIUtil.contextControlZone.height = SimGUIUtil.LineHeight * 2.5f;
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
            float offset = QEUtil.GetXZFactor();
            mHelper.extents[0] += offset;
            mHelper.extents[1] += QEUtil.GetYFactor();
            mHelper.extents[2] += offset;

            mPolyRefs.HandleResize();

            mResultCount = 0;
           
            mHasPosition = QEUtil.hasHit;
            if (!mHasPosition)
            {
                mMessage = "Outside source geometry.";
                return;
            }

            mHasPosition = true;
            mPosition = QEUtil.hitPosition;

            for (int i = 0; i < mPolyRefs.buffer.Length; i++)
            {
                mPolyRefs.buffer[i] = 0;
            }

            NavStatus status = mHelper.query.GetPolys(mPosition
                , mHelper.extents, mHelper.filter
                , mPolyRefs.buffer, out mResultCount);

            for (int i = 0; i < mResultCount; i++)
            {
                if (mPolyRefs.buffer[i] == 0)
                    Debug.Log("Invalid poly");
            }

            mMessage = "GetPolys: " + status.ToString();
        }

        public void OnGUI()
        {
            GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);

            mPolyRefs.OnGUI();

            GUILayout.EndArea();

            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 3;
            int iSlot = labels.SlotCount - 1 - LabelCount;

            string msg = mResultCount + " (";

            for (int i = 0; i < mResultCount; i++)
            {
                msg += mPolyRefs.buffer[i].ToString() + ((i == mResultCount - 1) ? "" : ",");
            }


            msg += ")";

            labels.Set(iSlot++, "Polys Found", msg);

            if (mHasPosition)
                labels.Set(iSlot++, "Geometry Point", Vector3Util.ToString(mPosition));
            else
                labels.Set(iSlot++, "Geometry Point", "None");

            labels.Set(iSlot++, "Search Extents", Vector3Util.ToString(mHelper.extents));

        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mPolyRefs.buffer, mResultCount);

            if (mHasPosition)
            {
                Color c = (mHelper.extents.x < 0 || mHelper.extents.y < 0)
                    ? Color.red 
                    : QEUtil.SelectColor;

                DebugDraw.Extents(mPosition, mHelper.extents, c);
            }
        }
    }
}
