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

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// Plans movement to a dynamic goal using only the crowd manager.
    /// [Category: Navigation Planner]
    /// </summary>
    /// <remarks>
    /// <para>
    /// Planning is restricted to the distance limitations of the crowd manager.
    /// Other planners should be used to bring the agent within the crowd manager planning range 
    /// before starting to use this planner.
    /// </para>
    /// <para>
    /// This planner will not perform well if the speed of the goal exceeds the maximum speed of 
    /// the agent, or if the goal moves into or through areas of the navigation mesh not 
    /// accessible to the agent.
    /// </para>
    /// <para>
    /// <para>
    /// This planner monitors the goal constantly.  Expensive replanning will occur if the 
    /// <see cref="NavFlags.HasNewGoal"/> flag is detected.  So the flag should only be set for 
    /// 'large' changes to the goal location.
    /// </para>
    /// <para>
    /// Notes:
    /// </para>
    /// <ul>
    /// <li>
    /// Optimized for use with a goal that changes frequently. (E.g. Multiple times a second.)
    /// </li>
    /// <li>Constrains all points to the navigation mesh.</li>
    /// <li>
    /// Invalid input points will cause a failure on entry and will be ignored during update.
    /// </li>
    /// <li>This planner is not suitable for frequent feedback of agent position.</li>
    /// <li>Will add the agent to the crowd if necessary.</li>
    /// <li>Does not remove the agent from the crowd on failure or exit.</li>
    /// <li>
    /// This planner does not monitor or manage the  <see cref="NavFlag.PlannerFailed"/> flag. 
    /// A master planner is expected to do so.
    /// </li>
    /// <li>Expects to have total ownership of all agent planning data.</li>
    /// </ul>
    /// </remarks>
    public sealed class FollowGoalCrowd
        : IState
    {
        /*
         * Design notes:
         * 
         * This is a very simple planner that uses the crowd manager for all planning.
         * 
         * If the new position flag is set, it will completely re-initialize the crowd agent.  
         * It is not suitable for frequent position feedback.
         * 
         * The goal is constantly fed to the crowd agent with a the new goal flag triggering a 
         * new goal request.
         */

        /// <summary>
        /// The agent managed by the planner.
        /// </summary>
        public readonly NavAgent agent;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="agent">The agent to manage.</param>
        public FollowGoalCrowd(NavAgent agent)
        {
            this.agent = agent;
        }

        /// <summary>
        /// Initialize the planner.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This method will fail on any problem, including failure to constrain the input points 
        /// to the navigation mesh and problems adding the agent to the crowd manager.
        /// </para>
        /// </remarks>
        /// <returns>True if the startup was a success.</returns>
        public bool Enter()
        {
            if (agent.navGroup.query == null)
                return false;

            NavmeshPoint pos = agent.GetPointSearch(agent.data.position);
            NavmeshPoint goal = agent.GetPointSearch(agent.data.goal);

            if (pos.polyRef == 0 || goal.polyRef == 0)
            {
                Debug.LogError(string.Format(
                    "{0}: Could not constrain point(s) to navigation mesh. Position: {1}, Goal: {2}"
                    , agent.transform.name, pos.ToString(), goal.ToString()));

                return false;
            }

            if (agent.AddToCrowd(pos.point) == null)
            {
                Debug.LogError(
                    string.Format("{0}:  Could not add agent to crowd.", agent.transform.name));

                return false;
            }

            agent.SetCorridorAssets(false);
            agent.SetPathAssets(false);

            if (!agent.crowdAgent.RequestMoveTarget(goal))
            {
                Debug.LogError(
                    string.Format("{0}:  Request agent move failed.", agent.transform.name));

                return false;
            }

            agent.data.plannerGoal = goal;
            agent.data.desiredPosition = pos;  // Just in case it's new.

            agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

            return true;
        }

        /// <summary>
        /// Exit the planner.
        /// </summary>
        public void Exit()
        {
        }

        /// <summary>
        /// Update the planner.
        /// </summary>
        /// <returns>True if the update completed successfully.</returns>
        public bool Update()
        {
            agent.SyncCrowdToDesired();

            // Constrain the goal to the navigation mesh.
            NavmeshPoint ngoal = agent.GetPointSearch(agent.data.goal);
            if (ngoal.polyRef == 0)
            {
                Debug.LogWarning(string.Format(
                    "{0}: Failed constrain goal to navmesh: Ignoring: {1}"
                    , agent.transform.name, ngoal.ToString()));

                // Leave the flag alone.  It will be handled later.
                ngoal = agent.data.plannerGoal;  // Revert to last good.
            }

            agent.data.plannerGoal = ngoal;

            // Handle a full planner reset if the agent position has changed.
            if ((agent.data.flags & NavFlag.HasNewPosition) != 0)
            {
                NavmeshPoint pt = agent.GetPointSearch(agent.data.position);

                if (pt.polyRef == 0)
                {
                    // Ignore the new position.
                    Debug.LogWarning(string.Format(
                        "{0}:  Failed constrain pos to navmesh: Ignoring: {1}"
                        , agent.transform.name, agent.data.position.ToString()));

                    agent.data.flags &= ~NavFlag.HasNewPosition;
                }
                else
                {
                    // Assume large change in position. So force a full reset  of planning.

                    // Note: Ignoring failure of add since it can only occur from invalid crowd 
                    // manager use cases.
                    agent.AddToCrowd(pt.point);
                    agent.crowdAgent.RequestMoveTarget(ngoal);

                    agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

                    agent.data.desiredPosition = pt;

                    //Debug.Log(string.Format(
                    //    "{0}: Reset planning.  New position."
                    //    , agent.transform.name));

                    return true;
                }
            }

            // Feed the goal to the crowd manager.

            if ((agent.data.flags & NavFlag.HasNewGoal) != 0)
            {
                agent.crowdAgent.RequestMoveTarget(ngoal);
                agent.data.flags &= ~NavFlag.HasNewGoal;
                //Debug.Log(string.Format("{0}: Reset crowd goal."
                //    , agent.transform.name));
            }
            else
                agent.crowdAgent.AdjustMoveTarget(ngoal);

            // Handle posting configation changes to the crowd.
            if ((agent.data.flags & NavFlag.CrowdConfigUpdated) != 0)
            {
                agent.crowdAgent.SetConfig(agent.crowdConfig);
                agent.data.flags &= ~NavFlag.CrowdConfigUpdated;
            }

            return true;
        }
    }
}
