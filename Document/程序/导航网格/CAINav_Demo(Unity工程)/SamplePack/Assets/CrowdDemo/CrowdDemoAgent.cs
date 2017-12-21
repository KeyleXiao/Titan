/*
 * Copyright (c) 2012 Stephen A. Pratt
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
using org.critterai.agent.task;
using org.critterai.agent;
using org.critterai.samples;
using UnityEngine;
using org.critterai.nav;
using org.critterai.nav.u3d;

[System.Serializable]
public class CrowdDemoAgent
    : Agent
{
    private class ManualController
        : BaseTask
    {
        public Agent followTarget;

        protected override void LocalUpdate(AgentContext context) 
        {
            if (state == TaskState.Inactive)
                state = TaskState.Active;

            if (!context.userControlled)
            {
                state = TaskState.Complete;
                return;
            }

            if (followTarget)
                context.navCon.AdjustFollow(followTarget.NavPosition);
        }

        public void SimUpdate(AgentContext context)
        {
            if (state != TaskState.Active)
                return;

            if (!(SimGUIUtil.hasHit && Input.GetKeyDown(StdButtons.SelectA)))
                return;


            // TODO: Add feature back once the follow crowd planner can handle long
            // distance planning.  Not a good demo of the feature until then.

            // Agent hitAgent = SimGUIUtil.hit.collider.gameObject.GetComponent<Agent>();

            //if (hitAgent)
            //{
            //    followTarget = hitAgent;

            //    // Assume that target agent is on the navigation mesh.
            //    context.navCon.Follow(followTarget.NavPosition, RangeType.Near);

            //    return;
            //}

            followTarget = null;

            NavmeshPoint pt = context.navCon.GetNavPoint(SimGUIUtil.hit.point);

            if (pt.polyRef == 0)
                // Not a valid target.
                return;

            context.navCon.MoveTo(pt, RangeType.At, true);
        }
    }

    private ManualController mUserControl;

    protected override void PostUpdate()
    {
        mUserControl.SimUpdate(context);
    }

    protected override ITask GetBrain()
    {
        SequentialNode wander = SequentialNode.Create(2)
            .Load(new MoveToRandom())
            .Load(new WaitForNavStrict());

        ParallelNode wanderControl = ParallelNode.Create(2, CompletionPolicy.Any)
            .Load(new UntilUserControlled())
            .Load(wander);

        mUserControl = new ManualController();

        PrioritySelector main = PrioritySelector.Create(2)
            .Load(IsUserControlled.Instance, mUserControl)
            .Load(TrueCondition.Instance, wanderControl);

        SequentialNode root = SequentialNode.Create(2)
            .Load(new SetUseCrowd(true))
            .Load(main);

        ITask result = new KeepActive(root);

        return result;
    }
}
