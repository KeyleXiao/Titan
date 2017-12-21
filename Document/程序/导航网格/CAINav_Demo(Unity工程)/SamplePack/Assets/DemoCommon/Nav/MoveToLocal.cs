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
    /// Steers straight to the goal without regard for the navigation mesh or obstructions. 
    /// [Category: Navigation Planner]
    /// </summary>
    /// <remarks>
    /// <para>This planner is useful for movement to a point that is not on the navigation mesh.  
    /// E.g. For the final movement toward something that is slightly off the edge of the mesh.
    /// </para> 
    /// <para>Notes:</para>
    /// <ul>
    /// <li>Releases all unneeded navigation assets on enter. (I.e. Path helper, corridor, 
    /// crowd agent.)
    /// </li>
    /// <li>
    /// This planner does not monitor or manage the <see cref="NavFlag.PlannerFailed"/> flag.  
    /// A master planner is expected to do so.
    /// </li>
    /// <li>Expects to have total ownership of all agent planning data.</li>
    /// </ul>
    /// </remarks>
    public sealed class MoveToLocal
        : IState
    {
        /// <summary>
        /// The agent to be managed.
        /// </summary>
        public readonly NavAgent agent;

        private float mSpeed;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="agent">The agent to be managed.</param>
        public MoveToLocal(NavAgent agent)
        {
            this.agent = agent;
        }

        /// <summary>
        /// Starts up the planner.
        /// </summary>
        /// <returns>True on success.</returns>
        public bool Enter()
        {
            // Note: Will never return false.
            mSpeed = Mathf.Sqrt(agent.data.desiredSpeedSq);
            agent.RemoveFromCrowd();
            agent.SetCorridorAssets(false);
            agent.SetPathAssets(true);
            return true;
        }

        /// <summary>
        /// Exits the planner.
        /// </summary>
        public void Exit()
        {
        }

        /// <summary>
        /// Updates the planner.
        /// </summary>
        /// <returns>True on success.</returns>
        public bool Update()
        {
            // Note: Will never return false.

            Vector3 pos = agent.data.position.point;
            Vector3 goal = agent.data.goal.point;

            // Uses a more accurate than normal location check
            // in order to prevent snapping at the end of movement.
            if (Vector3Util.SloppyEquals(pos, goal, NavAgent.ExactTolerance))
            {
                mSpeed = 0;
                agent.data.desiredPosition = agent.data.goal;
                agent.data.desiredVelocity = Vector3.zero;
                agent.data.desiredSpeedSq = 0;
                return true;
            }

            float maxDelta = agent.crowdConfig.maxAcceleration * Time.deltaTime;
            float desiredSpeed = agent.crowdConfig.maxSpeed;

            // This test assumes the agent can halt in less than the given number of radii.
            if (Vector3Util.GetDistance2D(pos, goal) < agent.crowdConfig.radius * 3)
                desiredSpeed = Mathf.Max(mSpeed - maxDelta, desiredSpeed * 0.2f);
            else
                desiredSpeed = Mathf.Min(mSpeed + maxDelta, desiredSpeed);

            Vector3 result = Vector3.MoveTowards(pos, goal, desiredSpeed * Time.deltaTime);

            agent.data.desiredPosition.point = result;
            agent.data.desiredPosition.polyRef = 0;
            agent.data.desiredVelocity = (result - pos).normalized * desiredSpeed;
            agent.data.desiredSpeedSq = desiredSpeed * desiredSpeed;

            mSpeed = desiredSpeed;

            return true;
        }
    }
}
