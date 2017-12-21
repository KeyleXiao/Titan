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
using UnityEngine;

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// A simple navigation state controller.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This class is part of the AI domain.  The expected use case is that the AI uses this 
    /// controller to make requests, then the navigation system responds with planning and the 
    /// locomoation system with movement. In general, the AI should not make direct changes to
    /// the underlying <see cref="NavAgent"/>.  The standard exception to this rule is error
    /// handling.
    /// </para>
    /// </remarks>
    public sealed class NavController
    {
        /// <summary>
        /// The number of consecutive navigation failures required before the controller will indicate
        /// failure and send an event.
        /// </summary>
        public const int NavFailureThreshold = 60;

        private readonly NavAgent agent;

        private bool mAtGoal = false;
        private int mNavFailedCount = 0;
        private bool mNavFailed = false;
        private bool mNavSuspended = true;

        /// <summary>
        /// Navigation event.
        /// </summary>
        public event System.EventHandler<NavigationEvent> OnNavEvent;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="agent">The navigation agent to be managed by the controller.</param>
        public NavController(NavAgent agent) 
        {
            this.agent = agent;
        }

        public NavmeshQuery Query
        {
            get { return agent.navGroup.query; }
        }

        public NavmeshQueryFilter Filter
        {
            get { return agent.navGroup.filter; }
        }

        /// <summary>
        /// The agent is at the goal.
        /// </summary>
        public bool IsAtGoal { get { return mAtGoal; } }

        /// <summary>
        /// Navigation has failed and requires manual intervension of some type.
        /// </summary>
        public bool NavHasFailed { get { return mNavFailed; } }

        /// <summary>
        /// True if navigation is active. (Not failed, not suspended.)
        /// </summary>
        public bool NavIsActive 
        { 
            get { return !(mNavFailed || mNavSuspended); } 
        }

        /// <summary>
        /// The agent transform.  (Represents the world position and rotation.)
        /// </summary>
        public Transform Transform { get { return agent.transform; } }

        /// <summary>
        /// The navigation position.
        /// </summary>
        public NavmeshPoint Position { get { return agent.data.position; } }

        /// <summary>
        /// The navigation rotation.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This value is almost always the same as the world rotation.
        /// </para>
        /// </remarks>
        public Quaternion Rotation { get { return agent.data.rotation; } }

        /// <summary>
        /// The navigation goal.
        /// </summary>
        public NavmeshPoint Goal { get { return agent.data.goal; } }

        /// <summary>
        /// The rotation the agent should be at when it reaches the goal.
        /// </summary>
        public Quaternion GoalRotation
        {
            get { return agent.data.goalRotation; }
            set { agent.data.goalRotation = value; }
        }

        /// <summary>
        /// If true, the goal rotation should be used.  If false, the goal rotation can be
        /// ignored.
        /// </summary>
        public bool UseGoalRotation
        {
            get { return (agent.data.flags | NavFlag.GoalRotationEnabled) != 0;}
            set
            {
                if (value)
                    agent.data.flags |= NavFlag.GoalRotationEnabled;
                else
                    agent.data.flags &= ~NavFlag.GoalRotationEnabled;
            }
        }

        /// <summary>
        /// Clears the <see cref="IsAtGoal"/> state so that the controller will re-evaluate the 
        /// agent position during the next update.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This will result in the <see cref="NavEventType.ReachedGoal"/> event being re-sent if
        /// the agent is at the goal, even when the goal has not changed.
        /// </para>
        /// </remarks>
        public void RequestGoalCheck()
        {
            mAtGoal = false;
        }

        /// <summary>
        /// The agent is in follow mode. (Its goal is dynamic.)
        /// </summary>
        public bool IsFollowing
        {
            get { return (agent.data.flags & NavFlag.GoalIsDynamic) != 0; }
        }

        /// <summary>
        /// Set a new follow goal. (A goal that is expected to be dynamic.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// This method will result in replanning.  Use <see cref="AdjustFollowGoal"/> to adjust
        /// the location of an existing follow goal.
        /// </para>
        /// <para>
        /// The goal rotation is ignored while in follow mode.
        /// </para>
        /// <para>
        /// The new 'goal event' will be triggered.  The 'at goal' event will be disabled while
        /// in follow mode.
        /// </para>
        /// </remarks>
        /// <param name="goal">The new goal.</param>
        /// <param name="goalType">The range type.</param>
        /// <returns>True if sucessful.</returns>
        public bool Follow(NavmeshPoint goal, RangeType rangeType)
        {
            goal = agent.GetPointSearch(goal);

            if (goal.polyRef == 0)
                return false;

            agent.data.targetMode = agent.ChooseMoveToMode();
            agent.data.goal = goal;
            agent.data.goalType = rangeType;

            agent.data.flags = (agent.data.flags | NavFlag.HasNewGoal | NavFlag.GoalIsDynamic) 
                & ~NavFlag.GoalRotationEnabled;

            // Forces evaluation during next upate.
            mAtGoal = false;

            PostEvent(NavEventType.NewGoal);

            return true;
        }

        /// <summary>
        /// Adjust the position of an existing follow goal.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Expected to be called frequently. Problems can occur if the goal delta is too large.
        /// </para>
        /// <para>
        /// Must be in follow mode. (See: <see cref="Follow"/>)
        /// </para>
        /// <para>
        /// Will not trigger a 'new goal' event.
        /// </para>
        /// </remarks>
        /// <param name="goal">The adjusted goal.</param>
        /// <returns>True if successful.</returns>
        public bool AdjustFollow(NavmeshPoint goal)
        {
            goal = agent.GetPointSearch(goal);

            if (goal.polyRef == 0 || (agent.data.flags & NavFlag.GoalIsDynamic) == 0)
                // Invalid request.
                return false;

            agent.data.goal = goal;

            // Always force evaluation during next upate.
            mAtGoal = false;

            return true;
        }

        /// <summary>
        /// Move to the goal using the standard navigation mode.
        /// </summary>
        /// <param name="goal">The goal.</param>
        /// <param name="rangeType">The range check type.</param>
        /// <param name="ignoreGoalRotation">
        /// True if the final agent rotation does not matter.
        /// </param>
        /// <returns>True if the move request succeeded.</returns>
        public bool MoveTo(NavmeshPoint goal, RangeType rangeType, bool ignoreGoalRotation)
        {
            goal = agent.GetPointSearch(goal);

            if (goal.polyRef == 0)
                return false;

            agent.data.flags &= ~NavFlag.GoalIsDynamic;
            agent.data.targetMode = agent.ChooseMoveToMode();
            agent.data.goal = goal;
            agent.data.goalType = rangeType;

            if (ignoreGoalRotation)
                agent.data.flags = 
                    (agent.data.flags | NavFlag.HasNewGoal) & ~NavFlag.GoalRotationEnabled;
            else
                agent.data.flags |= (NavFlag.HasNewGoal | NavFlag.GoalRotationEnabled);

            // Forces evaluation during next upate.
            mAtGoal = false;

            PostEvent(NavEventType.NewGoal);

            return true;
        }

        /// <summary>
        /// Move to the goal using the standard navigation mode.
        /// </summary>
        /// <param name="goal">The goal.</param>
        /// <param name="goalRotation">The rotation at the end of the movement.</param>
        /// <param name="rangeType">The range check type.</param>
        /// <returns>True if the move request succeeded.</returns>
        public bool MoveTo(NavmeshPoint goal, Quaternion goalRotation, RangeType rangeType)
        {
            goal = agent.GetPointSearch(goal);

            if (goal.polyRef == 0)
                return false;

            agent.data.flags &= ~NavFlag.GoalIsDynamic;
            agent.data.targetMode = agent.ChooseMoveToMode();
            agent.data.goal = goal;
            agent.data.goalRotation = goalRotation;
            agent.data.goalType = rangeType;
            agent.data.flags |= (NavFlag.HasNewGoal | NavFlag.GoalRotationEnabled);

            // Forces evaluation during next upate.
            mAtGoal = false;

            PostEvent(NavEventType.NewGoal);

            return true;
        }

        /// <summary>
        /// Move straight to the goal, ignoring the navigation mesh.
        /// </summary>
        /// <param name="goal">The goal.</param>
        /// <param name="rangeType">The range check type.</param>
        /// <param name="ignoreGoalRotation">
        /// True if the final agent rotation does not matter.
        /// </param>
        /// <returns>True if the move request succeeded.</returns>
        public void MoveStraightTo(NavmeshPoint goal, RangeType rangeType, bool ignoreGoalRotation)
        {
            agent.data.flags &= ~NavFlag.GoalIsDynamic;
            agent.data.targetMode = NavMode.MoveToLocal;
            agent.data.goal = goal;
            agent.data.goalType = rangeType;

            if (ignoreGoalRotation)
                agent.data.flags = 
                    (agent.data.flags | NavFlag.HasNewGoal) & ~NavFlag.GoalRotationEnabled;
            else
                agent.data.flags |= (NavFlag.HasNewGoal | NavFlag.GoalRotationEnabled);

            // Forces evaluation during next upate.
            mAtGoal = false;

            PostEvent(NavEventType.NewGoal);
        }

        /// <summary>
        /// Move straight to the goal, ignoring the navigation mesh.
        /// </summary>
        /// <param name="goal">The goal.</param>
        /// <param name="goalRotation">The rotation at the end of the movement.</param>
        /// <param name="rangeType">The range check type.</param>
        /// <returns>True if the move request succeeded.</returns>
        public void MoveStraightTo(NavmeshPoint goal, Quaternion goalRotation, RangeType rangeType)
        {
            agent.data.flags &= ~NavFlag.GoalIsDynamic;
            agent.data.targetMode = NavMode.MoveToLocal;
            agent.data.goal = goal;
            agent.data.goalRotation = goalRotation;
            agent.data.goalType = rangeType;
            agent.data.flags |= (NavFlag.HasNewGoal | NavFlag.GoalRotationEnabled);

            // Forces evaluation during next upate.
            mAtGoal = false;

            PostEvent(NavEventType.NewGoal);
        }

        /// <summary>
        /// Performs an widening search for the point if it does not have a polygon reference.
        /// </summary>
        /// <remarks>
        /// <para>
        /// A search will only be attempted if the point's polygon reference is zero.
        /// </para>
        /// <para>
        /// The result's polygon reference will remain zero if the search failed.
        /// </para>
        /// </remarks>
        /// <param name="point">A point that needs to be constrained to the navigation mesh.</param>
        /// <returns>The point, constrained to the navigation mesh.</returns>
        public NavmeshPoint GetNavPoint(NavmeshPoint point)
        {
            return agent.GetPointSearch(point);
        }

        /// <summary>
        /// Performs an widening search for the point.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The result's polygon reference will be zero if the search failed.
        /// </para>
        /// </remarks>
        /// <param name="point">A point that needs to be on the navigation mesh.</param>
        /// <returns>The point, constrained to the navigation mesh.</returns>
        public NavmeshPoint GetNavPoint(Vector3 point)
        {
            return agent.GetPointSearch(point);
        }

        /// <summary>
        /// Suspend navigation. (See: <see cref="Navmode.Suspended"/>)
        /// </summary>
        public void Suspend()
        {
            agent.data.targetMode = NavMode.Suspended;
        }

        /// <summary>
        /// Sets whether or not to use the crowd for navigation. (When available.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// Enabling use of the crowd will not have an impact if the crowd is not available, though
        /// if the crowd become available at a later point it will be enabled.
        /// </para>
        /// </remarks>
        /// <param name="allow">True if the crowd is the preferred navigation mode.</param>
        public void SetUseCrowd(bool allow)
        {
            if (allow)
            {
                agent.data.flags |= NavFlag.UseCrowd;

                if (agent.data.targetMode == NavMode.MoveToSimple)
                    agent.data.targetMode = agent.ChooseMoveToMode();
            }
            else
            {
                agent.data.flags &= ~NavFlag.UseCrowd;

                if (agent.data.targetMode == NavMode.MoveToCrowd)
                    agent.data.targetMode = agent.ChooseMoveToMode();
            }
        }

        /// <summary>
        /// Updates navigation.  (Expected to be called every frame.)
        /// </summary>
        public void Update()
        {
            // Detect Goal in range.

            bool isInRange = agent.IsInRange(agent.data.goal.point, agent.data.position.point);

            if (isInRange && !mAtGoal)
            {
                mAtGoal = true;

                if ((agent.data.flags & NavFlag.GoalIsDynamic) == 0)
                    PostEvent(NavEventType.ReachedGoal);
            }
            else if (!isInRange && mAtGoal)
                mAtGoal = false;

            // Detect navigation failure.

            if ((agent.data.flags & (NavFlag.PlannerFailed | NavFlag.MoverFailed)) == 0)
            {
                if (mNavFailed)
                {
                    mNavFailed = false;
                    mNavFailedCount = 0;

                    if (!mNavSuspended)
                        PostEvent(NavEventType.NavActivated);
                }
            }
            else if (!mNavFailed)
            {
                mNavFailedCount++;

                if (mNavFailedCount > NavFailureThreshold)
                {
                    mNavFailed = true;

                    PostEvent(NavEventType.NavFailed);

                    if (!mNavSuspended)
                        PostEvent(NavEventType.NavSuspended);
                }
            }
            if ((agent.data.flags | NavFlag.PlannerFailed) != 0)
            {
                mNavFailedCount++;

                if (mNavFailedCount > 10)
                {
                    mNavFailed = true;

                }
            }
            else
            {
                mNavFailed = false;
                mNavFailedCount = 0;
            }

            // Detect suspension

            if (mNavSuspended && agent.data.targetMode != NavMode.Suspended)
            {
                mNavSuspended = false;

                if (!mNavFailed)
                    PostEvent(NavEventType.NavActivated);     
            }
            else if (!mNavSuspended && agent.data.targetMode == NavMode.Suspended)
            {
                mNavSuspended = true;

                if (!mNavFailed)
                    PostEvent(NavEventType.NavSuspended);
            }
        }

        private void PostEvent(NavEventType type)
        {
            if (OnNavEvent != null)
                OnNavEvent(this, new NavigationEvent(type));
        }
    }
}
