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
    /// Plans movement to the goal using the crowd manager for local steering. 
    /// [Category: Navigation Planner]
    /// </summary>
    /// <remarks>
    /// <para>
    /// Notes:
    /// </para>
    /// <ul>
    /// <li>
    /// Optimized for use with an infrequenctly changing position and goal. This planner is 
    /// especially not suitable for frequent feedback of agent position.
    /// </li>
    /// <li>Constrains all points to the navigation mesh.</li>
    /// <li>
    /// Invalid input points will cause a failure on entry and will be ignored during update.
    /// </li>
    /// <li>Will add the agent to the crowd if necessary.</li>
    /// <li>Does not remove the agent from the crowd on failure or exit.</li>
    /// <li>
    /// This planner does not monitor or manage the <see cref="NavFlag.PlannerFailed"/> flag.  
    /// A master planner is expected to do so.
    /// </li>
    /// <li>Expects to have total ownership of all agent planning data.</li>
    /// </ul>
    /// </remarks>
    public sealed class MoveToCrowd
        : IState
    {

        /*
         * Design notes: 
         * 
         * This class trusts the crowd, not the mover.
         * The only time it queries the mover owned position is if it detects the new position 
         * flag.  Otherwise it gets its position from the crowd.
         * Just like the crowd, this class expects the mover to do as it is told.
         *  
         * The basic process:
         *    Constrain the position and goal to the navigation mesh.
         *    Get a path.
         *    Partially straighten the path to get a local target.
         *    Pass the local target to the crowd.
         *    When the agent gets near the local target, repeat local targetting until the goal is 
         *    reached.
         * 
         * The local target is the farthest corner the crowd is going to be able to handle.  
         * So 'local' is a relative value.
         * 
         * The crowd manager has problems reaching some targets, especially ones on the border of 
         * the navigation mesh.  In such cases the crowd agent may continually seek and never 
         * actually reach the target.
         * 
         * Intermediate targets are not a problem since a 'near' check is performed for detection.  
         * But the final goal can be a problem.  To deal with this, when the agent gets close to 
         * the goal the crowd agent is snapped to the goal with a max speed of zero and the agent 
         * is manually moved the final distance to the goal.
         * 
         * The design of the special handling is based on the fact that the goal is static.  Once 
         * the agent reaches the goal it won't obey the crowd manager any longer, but crowd 
         * feedback will occur since the crowd is aware of the agent.
         * 
         */

        private const byte StateNormal = 0;
        private const byte StateInRange = 1;
        private const byte StateIdle = 2;

        /// <summary>
        /// The agent being managed.
        /// </summary>
        public readonly NavAgent agent;

        private NavmeshPoint mTarget;

        /// <summary>
        /// True if the final goal is the crowd target, so no high level planning is needed.
        /// </summary>
        private bool mGoalInRange = false;

        /// <summary>
        /// Non-zero if the agent is at the goal, with its attendant special handling.
        /// </summary>
        /// <remarks>
        /// <para>
        /// 0 = No special goal handling. (Normal planning.)<br />
        /// 1 = A goal marker has been placed, need to manually move agent to exact goal.<br />
        /// 2 = At the goal.  Essentially in an idle state.<br />
        /// </para>
        /// </remarks>
        private byte mAtGoalState = StateNormal;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="agent">The agent to be managed.</param>
        public MoveToCrowd(NavAgent agent)
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
                // Agent in invalid state.
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

            if (agent.AddToCrowd(pos.point) == null)
            {
                Debug.LogError(string.Format("{0}: Could not add agent to the crowd."
                    , agent.transform.name));

                return false;
            }

            agent.data.desiredPosition = pos;
            agent.data.plannerGoal = goal;

            agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

            agent.SetCorridorAssets(false);
            agent.SetPathAssets(true);

            return HandlePositionFeedback();
        }

        /// <summary>
        /// Exit the planner.
        /// </summary>
        public void Exit()
        {
            agent.data.flags &= ~NavFlag.PathInUse;

            if (mAtGoalState != StateNormal)
                // Need to alert the next planner that it needs
                // to repost the crowd configuration.
                agent.data.flags |= NavFlag.CrowdConfigUpdated;
        }

        /// <summary>
        /// Update the planner.
        /// </summary>
        /// <returns>True if successful.</returns>
        public bool Update()
        {
            if (mAtGoalState == StateNormal)
                agent.SyncCrowdToDesired();

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
                       "{0}: Could not constrain new position to the navigation mesh. Ignoring: {1}"
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
                        "{0}: Could not constrain new goal to the navigation mesh. Ignoring: {1}"
                        , agent.transform.name, pt.ToString()));

                    newGoal = false;
                }
                else
                {
                    agent.data.plannerGoal = pt;
                }
            }

            if (mAtGoalState != StateNormal && newGoal || newPos)
                TransitionToNormalGoalState();

            agent.data.flags &= ~(NavFlag.HasNewPosition | NavFlag.HasNewGoal);

            if (newPos)
                // Note: This call inherently handles everything, not just the 
                // position change.
                return HandlePositionFeedback();

            if (!HandleNormalPlanning(newGoal))
                // A critical failure has occurred.
                return false;

            if ((agent.data.flags & NavFlag.CrowdConfigUpdated) != 0)
            {
                CrowdAgentParams config = agent.crowdConfig;

                if (mAtGoalState != StateNormal)
                    config.maxSpeed = 0;

                agent.crowdAgent.SetConfig(config);
                agent.data.flags &= ~NavFlag.CrowdConfigUpdated;
            }

            if (mAtGoalState == StateInRange)
            {
                // Need to manually move the agent to the goal.
                Vector3 pos = agent.data.desiredPosition.point;
                Vector3 goal = agent.data.plannerGoal.point;

                if (Vector2Util.SloppyEquals(
                    new Vector2(pos.x, pos.z), new Vector2(goal.x, goal.z), 0.005f))
                {
                    // Snap to the goal and, essentially, go into an idle
                    // state.
                    mAtGoalState = StateIdle;
                    agent.data.desiredPosition = agent.data.plannerGoal;
                    agent.data.desiredSpeedSq = 0;
                    agent.data.desiredVelocity = Vector3.zero;

                    // Debug.Log("Reached goal.  Going idle.");
                }
                else
                {
                    // Need to manually move the agent to the goal.
                    // Assumptions: The distance is short, and the crowd has 
                    // already slowed the agent's speed to a minimal value.

                    float desiredSpeed = Mathf.Sqrt(agent.data.desiredSpeedSq);

                    agent.data.desiredPosition.point = 
                        Vector3.MoveTowards(pos, goal, desiredSpeed * Time.deltaTime);

                    // While the point will be on the mesh surface, we
                    // can't assume we are already in the goal's polygon.
                    agent.data.desiredPosition.polyRef = 0;

                    agent.data.desiredVelocity =
                        (agent.data.desiredPosition.point - pos).normalized * desiredSpeed;
                }
            }

            return true;
        }

        /// <summary>
        /// Handles planning when position feedback is required. (Replaces normal planning.)
        /// </summary>
        /// <returns>False on a critical failure.</returns>
        private bool HandlePositionFeedback()
        {
            // Important: Should only call this method once the desired position and planner goal 
            // have been successfully constrained to the navigation mesh.

            // Make sure the path is still valid. Replan if needed.
            if (agent.PlanPath(agent.data.desiredPosition, agent.data.plannerGoal) <= 0)
            {
                // A critical failure.
                Debug.LogError(string.Format(
                    "{0}: Path planning failed on position feedback. Position: {1}, Goal: {2}"
                    , agent.transform.name
                    , agent.data.desiredPosition.ToString()
                    , agent.data.plannerGoal.ToString()));

                return false;
            }

            agent.RemoveFromCrowd();
            agent.AddToCrowd(agent.data.desiredPosition.point);

            // Next assignment will usually force crowd target replanning.
            mTarget = agent.data.desiredPosition;

            // Since the path is known to be good, setting the local
            // target should never fail.
            return SetLocalTarget();
        }

        /// <summary>
        /// Manages the crowd agent target, replanning the long distance path if needed.
        /// </summary>
        /// <returns>False on critical failure.</returns>
        private bool HandleNormalPlanning(bool hasNewGoal)
        {
            // Re-targetting will only occur if there is a new goal or getting close to an 
            // intermediate target.

            if (hasNewGoal 
                || !mGoalInRange && agent.IsNear(agent.data.desiredPosition.point, mTarget.point))
            {
                // First try to set the target without replanning.
                if (!SetLocalTarget())
                {
                    // This can happen when approaching the target from 
                    // outside the planned path, which can happen when
                    // the crowd manager takes a different path.
                    // (Or a new goal was set.)

                    if (agent.PlanPath(agent.data.desiredPosition, agent.data.plannerGoal) <= 0)
                    {
                        // Critical failure.
                        Debug.LogError(string.Format(
                            "{0}: Path replan failed. Position: {1}, Goal: {2}"
                            , agent.transform.name
                            , agent.data.desiredPosition.ToString()
                            , agent.data.plannerGoal.ToString()));
                        return false;
                    }

                    // Next call should not not fail since the path is known to be good.
                    return SetLocalTarget();
                }
            }
            else if (mGoalInRange)
            {
                /*
                 * Need to monitor to see if the goal has been reached.  This can be tricky.  
                 * The optimal 'at' and 'near' range ranges are usually too restrictive and too 
                 * large respectively. So a fixed value, based on experience, is used instead.
                 * (OK to make this value configurable.)
                 * 
                 * Assumption: Other parts of the code detect if a re-evaluation of position 
                 * is needed.
                 */
                if (mAtGoalState == StateNormal)
                {
                    if (Vector3Util.IsInRange(agent.data.desiredPosition.point
                        , agent.data.plannerGoal.point
                        , 0.12f, agent.data.heightTolerence))
                    {
                        // Remove from the crowd and add back, exactly at the goal with a zero max 
                        // speed.  Basically, puts a static obstacle into the crowd.

                        mAtGoalState = StateInRange;
                        agent.RemoveFromCrowd();

                        CrowdAgentParams config = agent.crowdConfig;
                        config.maxSpeed = 0;

                        agent.crowdAgent = 
                            agent.navGroup.crowd.AddAgent(agent.data.plannerGoal.point, config);

                        agent.data.flags &= ~NavFlag.CrowdConfigUpdated;

                        // Debug.Log("Transition to 'at goal' task.");
                    }
                }
            }
            return true;
        }

        /// <summary>
        /// Uses the long distance path to determine the best target and, if necessary, feeds it 
        /// to the crowd manager.
        /// </summary>
        /// <returns>False if a target could not be determined. (Agent wandered outside 
        /// current path.)</returns>
        private bool SetLocalTarget()
        {
            mGoalInRange = false;

            NavmeshPoint target;

            int targetIndex = agent.path.GetLocalTarget(
                agent.data.desiredPosition, agent.data.plannerGoal
                , 32     // TODO: Make configurable.
                , agent.navGroup.query
                , out target);

            if (targetIndex == -1)
            {
                // Position or goal not in current path. (Not necessarily an error.) May have been 
                // caused by the crowd manager, which has its own path planning.
                return false;
            }
            else if (target.point != mTarget.point)
            {
                // New target.  Send to crowd manager.
                if (mAtGoalState != StateNormal)
                {
                    // Assume that the agent is no longer at the goal.
                    agent.data.flags |= NavFlag.CrowdConfigUpdated;

                    if (mAtGoalState != StateNormal)
                        TransitionToNormalGoalState();
                }

                agent.crowdAgent.RequestMoveTarget(target);
            }

            mTarget = target;
            mGoalInRange = (mTarget.point == agent.data.plannerGoal.point);

            // Debug.LogWarning(mTarget.ToString() + ", " + mGoalInRange);

            return true;
        }

        private void TransitionToNormalGoalState()
        {
            mAtGoalState = StateNormal;

            if (agent.data.desiredPosition.polyRef == 0)
                // Must have been in an idle state.
                agent.data.desiredPosition = agent.GetPointSearch(agent.data.desiredPosition);

            agent.data.flags |= NavFlag.CrowdConfigUpdated;
        }
    }
}
