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
using System.Collections.Generic;

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// Provides a simple navigation loop for agents.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The manager handles updates to the navigation group's crowd, agent planners, and agent 
    /// movers.  It requires a crowd and will fail if one is not available.
    /// </para>
    /// <para>The navigation loops occurs as follows:</para>
    /// <ol>
    /// <li>Update crowd.</li>
    /// <li>Update agent planners.</li>
    /// <li>Update agent movers.</li>
    /// </ol>
    /// <para>
    /// The manager is strict.  If a agent fails in an unrecoverable manner it is automatically 
    /// removed.
    /// </para>
    /// <para>
    /// The design assumes that all agents share the same <see cref="NavGroup"/> as the manager.
    /// The <see cref="CreateAgent"/> method will create a standard agent.
    /// </para>
    /// </remarks>
    public class NavManager
    {
        private static NavManager mActiveManager;

        public static NavManager ActiveManager
        {
            get { return NavManager.mActiveManager; }
            set { NavManager.mActiveManager = value; }
        }

        private IState[] mPlanners;
        private IState[] mMovers;

        private NavGroup mNavGroup;
        private Dictionary<byte, NavAgentGroup> mAgentGroups;

        private NavManager(int maxAgents
            , NavGroup navGroup, Dictionary<byte, NavAgentGroup> agentGroups)
        {
            maxAgents = Mathf.Max(1, maxAgents);

            mPlanners = new IState[maxAgents];
            mMovers = new IState[maxAgents];
            mNavGroup = navGroup;
            mAgentGroups = new Dictionary<byte, NavAgentGroup>(agentGroups);
        }

        public static NavManager Create(int maxAgents
            , NavGroup navGroup, Dictionary<byte, NavAgentGroup> agentGroups)
        {
            if (navGroup.crowd == null || navGroup.crowd.IsDisposed
                || navGroup.mesh == null || navGroup.mesh.IsDisposed
                || navGroup.query == null || navGroup.query.IsDisposed
                || agentGroups == null || agentGroups.Count == 0)
            {
                return null;
            }

            return new NavManager(maxAgents, navGroup, agentGroups);
        }

        /// <summary>
        /// The maximum number agents supported by the manager.
        /// </summary>
        public int MaxAgents { get { return mPlanners.Length;  } }

        /// <summary>
        /// The navigation group associated with the manager.
        /// </summary>
        public NavGroup NavGroup { get { return mNavGroup; } }

        /// <summary>
        /// The number of pre-defined agent configurations.
        /// </summary>
        public int AgentGroupCount { get { return mAgentGroups.Count; }}

        /// <summary>
        /// Creates a standard navigation agent using the specified configuration.
        /// </summary>
        /// <param name="agent">The agent's transform.</param>
        /// <returns>A new navigation agent, or null on error.</returns>
        public NavAgent CreateAgent(byte group, Transform agent)
        {
            if (!agent || !mAgentGroups.ContainsKey(group))
                return null;

            return new NavAgent(agent, mNavGroup, mAgentGroups[group]);
        }

        /// <summary>
        /// The number of agents being managed.
        /// </summary>
        /// <returns>The number of agents being managed.</returns>
        public int GetAgentCount()
        {
            int result = 0;
            for (int i = 0; i < mPlanners.Length; i++)
            {
                if (mPlanners[i] != null)
                    result++;
            }
            return result;
        }

        /// <summary>
        /// Adds an agent to the manager.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The manager expects to take ownership of the state objects.  It also assumes that the 
        /// state objects are inactive but ready to activate.
        /// </para>
        /// </remarks>
        /// <param name="planner">The agent's planner.</param>
        /// <param name="mover">The agent's mover.</param>
        /// <returns>The index of the agent, or -1 on failure.</returns>
        public int AddAgent(IState planner, IState mover)
        {
            if (planner == null || mover == null)
                return -1;

            for (int i = 0; i < mPlanners.Length; i++)
            {
                if (mPlanners[i] == null)
                {
                    if (!planner.Enter())
                        break;

                    if (!mover.Enter())
                    {
                        planner.Exit();
                        break;
                    }

                    mPlanners[i] = planner;
                    mMovers[i] = mover;

                    return i;
                }
            }

            return -1;
        }

        /// <summary>
        /// Removes the agent from the manager.
        /// </summary>
        /// <param name="index">The index of the agent to be removed.</param>
        public void RemoveAgent(int index)
        {
            if (mPlanners[index] != null)
                mPlanners[index].Exit();

            mPlanners[index] = null;

            if (mMovers[index] != null)
                mMovers[index].Exit();

            mMovers[index] = null;
        }

        /// <summary>
        /// Applies a new planner to an existing agent.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The manager expects to take ownership of the planner.  It also assumes that the planner 
        /// is inactive but ready to activate.
        /// </para>
        /// </remarks>
        /// <param name="index">The index of the agent.</param>
        /// <param name="planner">The new planner. (Must not be null.)</param>
        public bool SetAgentPlanner(int index, IState planner)
        {
            if (planner == null || mPlanners[index] == null)
                return false;

            mPlanners[index].Exit();

            if (planner.Enter())
            {
                mPlanners[index] = planner;
                return true;
            }
            else
            {
                RemoveAgent(index);
                return false;
            }
        }

        /// <summary>
        /// Applies a new mover to an existing agent.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The manager expects to take ownership of the mover.  It also assumes that the mover is 
        /// inactive but ready to activate.
        /// </para>
        /// </remarks>
        /// <param name="index">The index of the agent.</param>
        /// <param name="mover">The new mover. (Must not be null.)</param>
        public bool SetAgentMover(int index, IState mover)
        {
            if (mover == null || mMovers[index] == null)
                return false;

            mMovers[index].Exit();

            if (mover.Enter())
            {
                mMovers[index] = mover;
                return true;
            }
            else
            {
                RemoveAgent(index);
                return false;
            }
        }

        public void LateUpdate()
        {
            if (mNavGroup.crowd.IsDisposed)
                return;

            mNavGroup.crowd.Update(Time.deltaTime);

            for (int i = 0; i < mPlanners.Length; i++)
            {
                IState s = mPlanners[i];

                if (s == null || s.Update())
                    continue;

                // Recover.
                s.Exit();
                if (!s.Enter())
                    RemoveAgent(i);
            }

            for (int i = 0; i < mPlanners.Length; i++)
            {
                IState s = mMovers[i];

                if (s == null || s.Update())
                    continue;

                // Recover.
                s.Exit();
                if (!s.Enter())
                    RemoveAgent(i);
            }
        }
    }
}
