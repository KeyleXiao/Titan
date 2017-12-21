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
    /// A master navigation planner that manages the standard planner types.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This planner will monitor the agent's target mode and switch planners as needed.  It 
    /// includes handling for all <see cref="NavMode"/> types.
    /// </para>
    /// </remarks>
    public sealed class NavPlanner
        : IState
    {
        /// <summary>
        /// The agent managed by the planner.
        /// </summary>
        public readonly NavAgent agent;

        private MoveToCrowd mMoveToCrowd;
        private FollowGoalCrowd mFollowGoal;
        private MoveToLocal mLocalMove;
        private MoveToSimple mSimpleMove;
        private SuspendInCrowd mSuspendInCrowd;

        // This field should never be set to null.
        private IState mPlanner = NullState.Instance;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="agent">The agent to be managed.</param>
        public NavPlanner(NavAgent agent)
        {
            this.agent = agent;

            // Note: The active planner is set during enter method.

            mMoveToCrowd = new MoveToCrowd(agent);
            mFollowGoal = new FollowGoalCrowd(agent);
            mLocalMove = new MoveToLocal(agent);
            mSimpleMove = new MoveToSimple(agent);
            mSuspendInCrowd = new SuspendInCrowd(agent);
        }

        /// <summary>
        /// Initialize the planner.
        /// </summary>
        /// <remarks>The initial state will be 'suspended'.</remarks>
        /// <returns>True if the enter operation is successful.</returns>
        public bool Enter()
        {
            if (agent == null)
            {
                Debug.LogError(
                    agent.transform.name + ": Planner failed on enter: Navigation agent is null.");

                agent.data.flags |= NavFlag.PlannerFailed;
                return false;
            }

            Suspend();
            agent.data.desiredPosition = agent.data.position;
            agent.data.flags &= ~NavFlag.PlannerFailed;

            return true;
        }

        /// <summary>
        /// Exit the planner.
        /// </summary>
        public void Exit()
        {
            Suspend();
        }

        /// <summary>
        /// Update the planner.
        /// </summary>
        /// <remarks>
        /// <para>
        /// <b>Special case: </b>Will never return false.  Instead, the planner will set the 
        /// <see cref="NavFlags.PlannerFailed"/> flag and enter a suspended state.  If this happens
        /// the planner will need to be exited and re-entered to restore it to operation.
        /// </para>
        /// </remarks>
        public bool Update()
        {
            HandleState();

            if ((agent.data.flags & NavFlag.PlannerFailed) != 0)
            {
                Debug.LogError(agent.transform.name + ": Planner failed.");
                Suspend();
            }
            else if (!mPlanner.Update())
            {
                agent.data.flags |= NavFlag.PlannerFailed;
                Debug.LogError(agent.transform.name+ ": Planner update failed: " + mPlanner);
            }

            return true;
        }

        private void HandleState()
        {
            NavMode target = agent.data.targetMode;

            switch (target)
            {
                case NavMode.MoveToCrowd:

                    if (mPlanner != mMoveToCrowd)
                        TransitionState(mMoveToCrowd);

                    break;

                case NavMode.FollowGoalCrowd:

                    if (mPlanner != mFollowGoal)
                        TransitionState(mFollowGoal);

                    break;

                case NavMode.MoveToSimple:

                    if (mPlanner != mSimpleMove)
                        TransitionState(mSimpleMove);

                    break;

                case NavMode.MoveToLocal:

                    if (mPlanner != mLocalMove)
                        TransitionState(mLocalMove);

                    break;

                case NavMode.SuspendInCrowd:

                    if (mPlanner != mSuspendInCrowd)
                        TransitionState(mSuspendInCrowd);

                    break;

                case NavMode.Suspended:

                    if (mPlanner != NullState.Instance)
                        Suspend();

                    break;
            }
        }

        private void Suspend()
        {
            mPlanner.Exit();
            mPlanner = NullState.Instance;

            agent.RemoveFromCrowd();
            agent.SetCorridorAssets(false);
            agent.SetPathAssets(false);

            // Note: The desired position is purposely left alone since it may be the cause of the 
            // problem. Don't want to wipe out that bit of information.
            // agent.data.desiredPosition = agent.data.position;
            agent.data.desiredVelocity = Vector3.zero;
            agent.data.desiredSpeedSq = 0;

            agent.data.flags &= ~NavFlag.PlannerFailed;
        }

        private void TransitionState(IState state)
        {
            mPlanner.Exit();
            mPlanner = state;

            if (mPlanner.Enter())
                agent.data.flags &= ~NavFlag.PlannerFailed;
            else
            {
                agent.data.flags |= NavFlag.PlannerFailed;
                Debug.LogError(agent.transform.name + ": Planner transition failed: " + state);
            }
        }
    }
}
