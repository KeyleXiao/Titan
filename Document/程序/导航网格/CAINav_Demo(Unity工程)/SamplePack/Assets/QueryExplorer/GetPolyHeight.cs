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
using org.critterai.nav;

namespace org.critterai.samples.qe
{
    public sealed class GetPolyHeight
        : IQueryExplorerState
    {
        private NavGroup mHelper;

        private uint[] mPolyRef = new uint[1];  // Used for debug view.
        private float mHeight;

        private NavmeshPoint mPosition;
        private bool mHasPosition = false;
        private bool mHasHeight = false;

        private string mMessage;

        public QEStateType StateType
        {
            get { return QEStateType.GetPolyHeight; }
        }

        public void Enter(NavGroup helper)
        {
            mHelper = helper;
            SimGUIUtil.BuildLabelRegion(false);

            mHasPosition = false;
            mHasHeight = false;

            SimGUIUtil.contextHelpText = string.Format("Select Poly: [{0}]", StdButtons.SelectA);
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
            mHasPosition = false;
            mHasHeight = false;

            if (Input.GetKeyDown(StdButtons.SelectA))
            {
                Vector3 trash;
                NavmeshPoint target;
                QEUtil.SearchResult result =
                    QEUtil.HandleStandardPolySearch(mHelper, out trash, out target, out mMessage);

                mHasPosition = (result & QEUtil.SearchResult.HitGeometry) != 0;
                if ((result & QEUtil.SearchResult.HitNavmesh) == 0)
                    // Could not find polygon.
                    mPolyRef[0] = 0;
                else if (target.polyRef == mPolyRef[0])
                    // Toggle off the selection.
                    mPolyRef[0] = 0;
                else
                    // New good selection.
                    mPolyRef[0] = target.polyRef;
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

            NavStatus status = mHelper.query.GetPolyHeight(
                new NavmeshPoint(mPolyRef[0], mPosition.point)
                , out mHeight);

            mMessage = "GetPolyHeight: " + status.ToString();

            if (NavUtil.Failed(status))
                return;

            mHasHeight = true;
        }

        public void OnGUI()
        {
            LabelRegion labels = SimGUIUtil.labels;

            labels.Clear();
            labels.Last = mMessage;

            const int LabelCount = 2;
            int slotIndex = labels.SlotCount - 1 - LabelCount;

            if (mHasPosition)
                labels.Set(slotIndex++, "Geometry Position", Vector3Util.ToString(mPosition.point));
            else
                labels.Set(slotIndex++, "Geometry Position", "None");

            if (mHasHeight)
                labels.Set(slotIndex++, "Poly Height", string.Format("{0:F2}", mHeight));
            else
                labels.Set(slotIndex++, "Poly Height", "None");

            if (mHasHeight)
            {
                Rect rect = new Rect(Input.mousePosition.x - 5
                    , Screen.height - Input.mousePosition.y - 25
                    , 100, SimGUIUtil.LineHeight);
                 
                GUI.Label(rect, string.Format("{0:F2}", mHeight));
            }

        }

        public void OnRenderObject()
        {
            NavDebug.Draw(mHelper.mesh, mPolyRef, 1);
        }
    }
}
