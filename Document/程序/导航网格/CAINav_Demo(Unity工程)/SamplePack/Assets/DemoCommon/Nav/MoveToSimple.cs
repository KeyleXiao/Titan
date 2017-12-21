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
    /// Plans movement to a the goal using simple steering. 
    /// [Category: Navigation Planner]
    /// </summary>
    /// <remarks>
    /// <para>High level planning is based around a path corridor.  The agent is moved along the 
    /// corridor at maximum speed with no local obstacle avoidance. This is useful when there is 
    /// no concern of collision with other agents.</para>
    /// <para>Notes:</para>
    /// <ul>
    /// <li>Constrains all points to the navigation mesh.</li>
    /// <li>
    /// Invalid input points will cause a failure on entry and will be ignored during update.
    /// </li>
    /// <li>Does not use the crowd manager.</li>
    /// <li>Leaves the corridor in place on exit.</li>
    /// <li>
    /// This planner does not monitor or manage the <see cref="NavFlag.PlannerFailed"/> flag.  
    /// A master planner is expected to do so.
    /// </li>
    /// <li>Expects to have total ownership of all agent planning data.</li>
    /// </ul>
    /// </remarks>
    public sealed class MoveToSimple
        : IState
    {
        // TODO:  Make configurable.  Convert to seconds?
        private static int OptimizationFrequency = 10;

        /// <summary>
        /// The agent to be managed.
        /// </summary>
        public readonly NavAgent agent;

        private float mSpeed;
        private int mOptimizationTimer;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="agent">The agent to be managed.</param>
        public MoveToSimple(NavAgent agent)
        {
            this.agent = agent;
        }

        /// <summary>
        /// Starts up the planner.
        /// </summary>
        /// <returns>True on success.</returns>
        public bool Enter()
        {
            if (agent.navGroup.query == null)
                return false;

            NavmeshPoint pos = agent.GetPointSearch(agent.data.position);
            if (pos.polyRef == 0)
            {
                Debug.LogError(string.Format(
                    "{0}: Could not constrain position to navigation mesh. {1}"
                    , agent.transform.name, pos.ToString()));

                return false;
            }

            NavmeshPoint goal = agent.GetPointSearch(agent.data.goal);
            if (goal.polyRef == 0)
            {
                Debug.LogError(string.Format("{0}: Could not constrain goal to navigation mesh. {1}"
                    , agent.transform.name, goal.ToString()));

                return false;
            }

            agent.RemoveFromCrowd();
            agent.SetCorridorAssets(true);
            agent.SetPathAssets(false);

            if (agent.PlanCorridor(pos, goal) <= 0)
            {
                Debug.LogError(string.Format("{0}: Could not plan corridor on enter."
                    , agent.transform.name));

                agent.data.flags &= ~NavFlag.CorridorInUse;
                return false;
            }

            agent.data.desiredPosition = agent.corridor.Position;
            agent.data.plannerGoal = agent.corridor.Target;
            agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

            mSpeed = Mathf.Sqrt(agent.data.desiredSpeedSq);
            mOptimizationTimer = OptimizationFrequency;

            return true;
        }

        /// <summary>
        /// Exits the planner.
        /// </summary>
        public void Exit()
        {
            agent.data.flags &= ~NavFlag.CorridorInUse;
        }

        /// <summary>
        /// Updates the planner.
        /// </summary>
        /// <returns>True on success.</returns>
        public bool Update()
        {

            // Detect position and goal changes, and constrain to navmesh.

            bool newPos = (agent.data.flags & NavFlag.HasNewPosition) != 0;
            bool newGoal = (agent.data.flags & NavFlag.HasNewGoal) != 0;

            NavmeshPoint pt;

            if (newPos)
            {
                pt = agent.GetPointSearch(agent.data.position);

                if (pt.polyRef == 0)
                {
                    // Ignore new position.
                    Debug.LogWarning(string.Format(
                        "{0}: Could not constrain position to navigation mesh. Ignoring: {1}"
                        , agent.transform.name, pt.ToString()));

                    newPos = false;
                }
                else
                    agent.data.desiredPosition = pt;
            }

            if (newGoal)
            {
                pt = agent.GetPointSearch(agent.data.goal);

                if (pt.polyRef == 0)
                {
                    // Ignore new goal.
                    Debug.LogWarning(string.Format(
                        "{0}: Could not constrain goal to navigation mesh. Ignoring: {1}"
                        , agent.transform.name, pt.ToString()));

                    newGoal = false;
                }
                else
                    agent.data.plannerGoal = pt;
            }

            agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

            // Handle planning.

            if (newGoal || newPos)
            {
                // Replanning needed.
                if (!HandlePlanning(newPos, newGoal))
                    // Critical failure.
                    return false;
            }

            // Is any movement needed?

            if (Vector3Util.SloppyEquals(agent.data.desiredPosition.point
                , agent.data.plannerGoal.point
                , NavAgent.ExactTolerance))
            {
                // At goal.  Don't need to do anything.
                mSpeed = 0;
                agent.data.desiredPosition = agent.data.plannerGoal;
                agent.data.desiredVelocity = Vector3.zero;
                agent.data.desiredSpeedSq = 0;
                return true;
            }

            // Handle speed adjustments.

            float maxDelta = agent.crowdConfig.maxAcceleration * Time.deltaTime;
            float desiredSpeed = agent.crowdConfig.maxSpeed;

            // This shortcut test assumes the agent can halt in less than the
            // specified number of radii.
            if (Vector3Util.GetDistance2D(agent.data.desiredPosition.point
                    , agent.data.plannerGoal.point) < agent.crowdConfig.radius * 3)
            {
                // Slow down to as low of 20% of maximum speed.
                desiredSpeed = Mathf.Max(mSpeed - maxDelta, desiredSpeed * 0.2f);
            }
            else
                // Speed up to as high as maximum speed.
                desiredSpeed = Mathf.Min(mSpeed + maxDelta, desiredSpeed);

            // Perform the move.

            if (--mOptimizationTimer < 1)
            {
                agent.corridor.OptimizePathTopology(true);
                mOptimizationTimer = OptimizationFrequency;
            }

            Vector3 movePos = Vector3.MoveTowards(agent.data.desiredPosition.point
                , agent.corridor.Corners.verts[0]
                , desiredSpeed * Time.deltaTime);

            agent.corridor.MovePosition(movePos);
            movePos = agent.corridor.Position.point;

            agent.data.desiredVelocity =
                (movePos - agent.data.desiredPosition.point).normalized * desiredSpeed;

            agent.data.desiredSpeedSq = desiredSpeed * desiredSpeed;
            agent.data.desiredPosition = agent.corridor.Position;

            mSpeed = desiredSpeed;

            return true;
        }

        private bool HandlePlanning(bool retargetPos, bool retargetGoal)
        {
            /*
             * Design notes:
             * 
             * Tries to do a corridor move on the position and/or target.
             * A replan is only triggered if the move(s) fail to reach the desired polygon(s).
             * 
             * It is important to note that the replan may fail to reach the goal.  But that is a 
             * problem for the AI, not the planner.
             */

            bool needsFullReplan = false;

            // Convenience vars.  Only used for input.  Not updated.
            NavmeshPoint goal = agent.data.plannerGoal;
            NavmeshPoint pos = agent.data.desiredPosition;

            PathCorridor corridor = agent.corridor;

            if (retargetGoal && retargetPos)
            {
                corridor.Move(pos.point, goal.point);
                retargetGoal = (goal.polyRef != corridor.Target.polyRef
                    || pos.polyRef != corridor.Position.polyRef);
            }
            else if (retargetPos)
            {
                corridor.MovePosition(pos.point);
                needsFullReplan = (pos.polyRef != corridor.Position.polyRef);         
            }
            else if (retargetGoal)
            {
                corridor.MoveTarget(goal.point);
                needsFullReplan = (goal.polyRef != corridor.Target.polyRef);
            }

            if (needsFullReplan)
            {
                // Debug.Log("Full Replan");
                if (agent.PlanCorridor(pos, goal) <= 0)
                {
                    Debug.LogError(agent.transform.name + ": Could not replan corridor.");
                    return false;
                }
            }
            else if (retargetPos || retargetGoal)
            {
                // Debug.Log("Optimize Only");
                // Any chagnes are likely to be larger than normal.  So force an optimization.
                agent.corridor.OptimizePathTopology(true);
                mOptimizationTimer = OptimizationFrequency;
            }

            agent.data.desiredPosition = agent.corridor.Position;
            agent.data.plannerGoal = agent.corridor.Target;

            return true;
        }
    }
}
