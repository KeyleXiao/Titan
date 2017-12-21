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
    public class FindPath 
        : IQueryExplorerState 
    {
        public int MaxPathBuffer = 100;

        protected NavGroup helper;
        protected int pathID = 0;   // Allows detection of replanning.

        protected NavmeshPoint position;
        protected NavmeshPoint goal;
        protected int pathCount;
        protected BufferHandler<uint> path;

        private string mMessages = "";
        private string mLocalMessage = "";
        private int mSlotIndex;

        private bool mShowClosedList = false;
        private bool mUseAutoAdvance = false;

        public virtual QEStateType StateType
        {
            get { return QEStateType.FindPath; }
        }

        public virtual string HelpText
        {
            get
            {
                if (position.polyRef == 0)
                    return string.Format("Set Start: [{0}]", StdButtons.SelectA);
                else if (mUseAutoAdvance)
                    return "Set Marker: " + StdButtons.SelectA;
                else
                {
                    return string.Format("Set Goal: [{0}], Set Start: [{1}]"
                        , StdButtons.SelectA, StdButtons.SetB);
                }
            }
        }

        protected virtual int LabelCount { get { return 1; } }

        public virtual void Enter(NavGroup helper)
        {
            this.helper = helper;

            pathCount = 0;
            position.polyRef = 0;
            goal.polyRef = 0;

            SimGUIUtil.BuildLabelRegion(false);

            path = 
                new BufferHandler<uint>("Path Buffer", 1, QEUtil.SmallBufferInit, 1, MaxPathBuffer);

            mLocalMessage = "No path";
            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextControlZone.height = SimGUIUtil.LineHeight * 5.5f;
            SimGUIUtil.contextActive = true;
        }

        public virtual void Exit()
        {
            path = null;
            SimGUIUtil.labels.Clear();
            SimGUIUtil.contextHelpText = "";
            SimGUIUtil.contextActive = false;
        }

        public virtual void Update()
        {
            if (path.HandleResize())
            {
                if (pathCount != 0)
                    pathID++;

                pathCount = 0;
                mLocalMessage = "No path";
            }

            if (Input.GetKeyDown(StdButtons.SelectA) || Input.GetKeyDown(StdButtons.SetB))
            {
                Vector3 trasha;
                string trashb;
                NavmeshPoint point;

                QEUtil.SearchResult searchResult = 
                    QEUtil.HandleStandardPolySearch(helper, out trasha, out point, out trashb);

                if ((searchResult & QEUtil.SearchResult.HitNavmesh) == 0)
                    return;

                // Switching to pure readability style.
                if (mUseAutoAdvance)
                {
                    if (!Input.GetKeyDown(StdButtons.SetB))
                    {
                        if (position.polyRef == 0)
                            position = point;
                        else if (goal.polyRef == 0)
                            goal = point;
                        else
                        {
                            position = goal;
                            goal = point;
                        }
                    }
                }
                else
                {
                    if (position.polyRef == 0 || Input.GetKeyDown(StdButtons.SetB))
                        position = point;
                    else
                        goal = point;
                }
                pathCount = 0;
            }

            if (pathCount == 0 && position.polyRef != 0 && goal.polyRef != 0)
            {
                GeneratePath();
                pathID++;
            }
        }

        private void GeneratePath()
        {
            NavStatus status = helper.query.FindPath(position, goal
                , helper.filter, path.buffer
                , out pathCount);

            pathID++; 
            mLocalMessage = "FindPath: " + status.ToString();
        }

        protected void AppendMessage(string value)
        {
            if (value.Length > 0)
                 mMessages += (mMessages.Length > 0 ? ", " : "") + value;
        }

        protected void AddLabel(string label, string value)
        {
            SimGUIUtil.labels.Set(mSlotIndex++, label, value);
        }

        public void OnGUI()
        {
            SimGUIUtil.contextHelpText = HelpText;

            mMessages = "";

            GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);
            HandleControls();
            GUILayout.EndArea();

            LabelRegion labels = SimGUIUtil.labels;
            labels.Clear();
            mSlotIndex = labels.SlotCount - 1 - LabelCount;

            HandleLabels();

            labels.Last = mMessages;
        }

        protected virtual void HandleLabels()
        {
            AddLabel("Path Length", pathCount.ToString());
            AppendMessage(mLocalMessage);
        }

        protected virtual void HandleControls() 
        {
            mShowClosedList = !GUILayout.Toggle(!mShowClosedList, "Show path");
            mShowClosedList = GUILayout.Toggle(mShowClosedList, "Show closed list");
            mUseAutoAdvance = GUILayout.Toggle(mUseAutoAdvance, "Auto-advance path");

            path.OnGUI();
        }

        public virtual void OnRenderObject()
        {
            if (mShowClosedList)
                NavDebug.Draw(helper.mesh, helper.query);
            else
                NavDebug.Draw(helper.mesh, path.buffer, pathCount);

            Color c = QEUtil.SelectColor;
            c.a = 0.25f;

            if (position.polyRef != 0)
                DebugDraw.DiamondMarker(position.point, QEUtil.MarkerScaleNorm, c);

            c = NavDebug.goalColor;
            c.a = 0.25f;

            if (goal.polyRef != 0)
                DebugDraw.DiamondMarker(goal.point, QEUtil.MarkerScaleNorm, c);
        }
    }
}
