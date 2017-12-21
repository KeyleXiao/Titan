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
    /// Adds the agent to the crowd, but maintains it in its current location.
    /// [Category: Navigation Planner]
    /// </summary>
    /// <remarks>
    /// <para>
    /// Notes:
    /// </para>
    /// <ul>
    /// <li>
    /// This planner effectively suspends planning but keeps the agent in the crowd so the 
    /// agent still effects other agent movement. It will not alter the agent's current position.
    /// </li>
    /// <li>
    /// The crowd agent is constrained to the navigation mesh.  So this planner works best when the 
    /// actual agent position is on or near the mesh.  Otherwise switching to another planner may 
    /// result in unexpected snapping behavior.
    /// </li>
    /// <li>Will add the agent to the crowd if necessary.</li>
    /// <li>
    /// This planner does not monitor or manage the <see cref="NavFlag.PlannerFailed"/> flag.  
    /// A master planner is expected to do so.
    /// </li>
    /// <li>Expects to have total ownership of all agent planning data.</li>
    /// </ul>
    /// </remarks>
    public sealed class SuspendInCrowd
        : IState
    {
        /// <summary>
        /// The agent being managed.
        /// </summary>
        public readonly NavAgent agent;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="agent">The agent to be managed.</param>
        public SuspendInCrowd(NavAgent agent)
        {
            this.agent = agent;
        }

        /// <summary>
        /// Starts up the planner.
        /// </summary>
        /// <returns>True if successful.</returns>
        public bool Enter()
        {
            if (agent == null || agent.navGroup.query == null || agent.navGroup.crowd == null)
                return false;

            NavmeshPoint pos = agent.GetPointSearch(agent.data.position);
            if (pos.polyRef == 0)
            {
                Debug.LogError(string.Format(
                    "{0}: Could not constrain position to navigation mesh. {1}"
                    , agent.transform.name, pos.ToString()));

                return false;
            }

            agent.RemoveFromCrowd();

            CrowdAgentParams config = agent.crowdConfig;
            config.maxSpeed = 0;

            agent.crowdAgent = 
                agent.navGroup.crowd.AddAgent(agent.data.plannerGoal.point, config);

            if (agent.crowdAgent == null)
            {
                Debug.LogError(string.Format("{0}: Could not add agent to the crowd."
                    , agent.transform.name));

                return false;
            }

            agent.data.flags &= ~NavFlag.CrowdConfigUpdated;

            agent.data.desiredPosition = agent.data.position;
            agent.data.desiredSpeedSq = 0;
            agent.data.desiredVelocity = Vector3.zero;

            agent.SetCorridorAssets(false);
            agent.SetPathAssets(false);

            return true;
        }

        /// <summary>
        /// Exit the planner.
        /// </summary>
        public void Exit()
        {
            agent.data.flags |= NavFlag.CrowdConfigUpdated;
        }

        /// <summary>
        /// Update the planner.
        /// </summary>
        /// <returns>True if successful.</returns>
        public bool Update()
        {
            if ((agent.data.flags & NavFlag.CrowdConfigUpdated) != 0)
            {
                CrowdAgentParams config = agent.crowdConfig;
                config.maxSpeed = 0;
                agent.crowdAgent.SetConfig(config);
                agent.data.flags &= ~NavFlag.CrowdConfigUpdated;
            }

            return true;
        }
    }
}
