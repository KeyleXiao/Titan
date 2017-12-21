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
    /// Moves and rotates the agent transform based on the navigation agent state. 
    /// [Category: Navigation Mover]
    /// </summary>
    /// <remarks>
    /// <para>
    /// Position follows the planner exactly.  No attempt is made to offset the planner's 
    /// position based on world geometry. (E.g. If the planner says to walk through a wall, the 
    /// mover will do it.)
    /// </para>
    /// <para>
    /// Rotation speed is limited based on the value of <see cref="NavGroup.maxTurnSpeed"/>.  
    /// So the planner and transform rotations may not always match.
    /// </para>
    /// <para>
    /// The goal rotation will will take precidence over the planner rotation when near the goal.
    /// </para>
    /// <para>
    /// Does not interact with the navigation mesh at all. The up side: It works with all types of 
    /// planners.  The down side: It can feed positions with a polygon reference of zero back into
    /// the navigation state.
    /// </para>
    /// </remarks>
    public sealed class SimpleMover
        : IState
    {
        /// <summary>
        /// The agent being handled by the mover.
        /// </summary>
        public readonly NavAgent agent;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="agent">The agent to be handled by the mover.</param>
        public SimpleMover(NavAgent agent)
        {
            this.agent = agent;
        }

        /// <summary>
        /// Starts up the mover.
        /// </summary>
        /// <returns>True if successful.</returns>
        public bool Enter()
        {
            return (agent != null);
        }

        /// <summary>
        /// Exit the mover.
        /// </summary>
        public void Exit()
        {
        }

        /// <summary>
        /// Update the mover.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This method will never fail.
        /// </para>
        /// <para>
        /// With the exception of forced moves, updates will temporarily halt during planner 
        /// failure.
        /// </para>
        /// </remarks>
        /// <returns>True. (Never returns false.)</returns>
        public bool Update()
        {
            Transform trans = agent.transform;

            if ((agent.data.flags & NavFlag.ForceMove) != 0)
            {
                agent.data.position = agent.data.goal;

                if ((agent.data.flags & NavFlag.GoalRotationEnabled) != 0)
                    agent.data.rotation = agent.data.goalRotation;

                agent.transform.position = agent.data.position.point;
                agent.transform.rotation = agent.data.rotation;

                agent.data.flags = 
                    (agent.data.flags | NavFlag.HasNewPosition) & ~NavFlag.ForceMove;

                Debug.Log(agent.transform.name + ": Force Move");
                return true;
            }

            // Note: Keep this after the force move handling.
            if ((agent.data.flags & NavFlag.PlannerFailed) != 0)
                // Planner has failed. Early exit.
                return true;

            agent.data.position = agent.data.desiredPosition;
            trans.position = agent.data.position.point;

            Quaternion rotation = agent.data.rotation;

            if ((agent.data.flags & NavFlag.GoalRotationEnabled) != 0
                && agent.IsNear(agent.data.position.point, agent.data.goal.point))
            {
                // Close to goal.  So use the goal rotation.
                    rotation = agent.data.goalRotation;
            }
            else if (agent.data.desiredSpeedSq > agent.agentGroup.turnThreshold)
            {
                // Agent is not near the goal, so use the planner's rotation.
                // The conditional test prevents unnecessary seeking
                // at low speeds.
                rotation = Quaternion.LookRotation(
                    new Vector3(agent.data.desiredVelocity.x, 0, agent.data.desiredVelocity.z));
            }

            trans.rotation = Quaternion.Slerp(
                trans.rotation, rotation, Time.deltaTime * agent.agentGroup.maxTurnSpeed);

            agent.data.rotation = trans.rotation;

            return true;
        }
    }
}
