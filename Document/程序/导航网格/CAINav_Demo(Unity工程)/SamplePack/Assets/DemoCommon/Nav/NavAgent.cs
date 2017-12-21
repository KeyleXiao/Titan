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
    /// Provides navigation related data and utility features common to 
    /// navigation agents.
    /// </summary>
    public class NavAgent
    {
        /// <summary>
        /// The tolerance to use when testing for an exact match between two points.
        /// </summary>
        public const float ExactTolerance = 0.005f;

        /// <summary>
        /// Shared navigation objects being used by the agent.
        /// </summary>
        public NavGroup navGroup;

        /// <summary>
        /// The extents to use for wide area searches.
        /// </summary>
        /// <remarks>
        /// <para>
        /// <b>Warning</b>: Don't set the values too high.  The extents must never be large enough 
        /// to encompass more than 128 navigation mesh polygons.
        /// </para>
        /// </remarks>
        public Vector3 wideExtents = new Vector3(10, 10, 10);

        /// <summary>
        /// The core agent navigation data. [Owner: Various]
        /// </summary>
        public NavAgentData data;

        /// <summary>
        /// Shared navigation configuration and resources.
        /// </summary>
        public NavAgentGroup agentGroup;

        /// <summary>
        /// The agent transform. (World position.)
        /// </summary>
        public Transform transform;

        /// <summary>
        /// The crowd agent. [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// Will be null if the crowd is not in currently in use.
        /// </para>
        /// </remarks>
        public CrowdAgent crowdAgent;

        /// <summary>
        /// The current path corridor. [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This field is normally of use only to the planner and for debug purposes.
        /// </para>
        /// <para>
        /// The state of the corridor is valid only if the <see cref="NavFlags.CorridorInUse"/> 
        /// flag is set.
        /// </para>
        /// </remarks>
        public PathCorridor corridor;

        /// <summary>
        /// The agent's path helper. [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This field is normally of use only to the planner and for debug purposes.
        /// </para>
        /// <para>
        /// The state of the path is only valid if the <see cref="NavFlags.PathInUse"/> flag is set.
        /// </para>
        /// </remarks>
        public NavPath path;

        /// <summary>
        /// The in-use crowd agent configuration.
        /// </summary>
        /// <remarks>
        /// <para>If the agent is added to the crowd, the <see cref="crowdAgent"/> contains the 
        /// authoritative settings. This field contains the configuration that will be used 
        /// when not using the crowd, or when adding/re-adding the agent to the crowd.</para>
        /// <para>
        /// Setting the <see cref="NavFlags.CrowdConfigUpdated"/> will alert the planner that a 
        /// new configuration needs to be pushed to the crowd.
        /// </para>
        /// <para>
        /// The default configation is defined in <see cref="navGroup"/>
        /// </para>
        /// </remarks>
        public CrowdAgentParams crowdConfig;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="group">The navigation mesh group the agent
        /// belongs to.</param>
        /// <param name="transform">The agent's transform.</param>
        public NavAgent(Transform transform, NavGroup navGroup, NavAgentGroup agentGroup)
        {
            this.agentGroup = agentGroup;
            this.navGroup = new NavGroup(navGroup, false);
            this.transform = transform;
            this.wideExtents = navGroup.extents * 10;
            RevertToAgentGroup();
        }

        /// <summary>
        /// Reverts the agent's group related configuration settings to the values in 
        /// <see cref="agentGroup"/>.
        /// </summary>
        /// <remarks>Handles updates to the crowd agent. So the 
        /// <see cref="NavFlags.CrowdConfigUpdated"/> flag will not be set.
        /// </remarks>
        public void RevertToAgentGroup()
        {
            crowdConfig = agentGroup.crowdAgentConfig;

            data.radiusAt = agentGroup.radiusAt;
            data.radiusNear = agentGroup.radiusNear;
            data.heightTolerence = agentGroup.heightTolerance;

            if (crowdAgent != null)
                crowdAgent.SetConfig(crowdConfig);
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
        public NavmeshPoint GetPointSearch(NavmeshPoint point)
        {
            NavmeshPoint result = point;
            if (result.polyRef == 0)
            {
                data.navStatus = navGroup.query.GetNearestPoint(
                    point.point, navGroup.extents, navGroup.filter, out result);
                if (result.polyRef == 0)
                {
                    data.navStatus = navGroup.query.GetNearestPoint(
                        point.point , wideExtents, navGroup.filter , out result);
                }
            }
            return result;
        }

        /// <summary>
        /// Performs an widening search for the point.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The result's polygon reference will be zero if the search failed.
        /// </para>
        /// </remarks>
        /// <param name="point">A point that needs to be on the navigation mesh.
        /// </param>
        /// <returns>The point, constrained to the navigation mesh.</returns>
        public NavmeshPoint GetPointSearch(Vector3 point)
        {
            NavmeshPoint result;
            data.navStatus = navGroup.query.GetNearestPoint(point
                , navGroup.extents, navGroup.filter
                , out result);
            if (result.polyRef == 0)
            {
                data.navStatus = navGroup.query.GetNearestPoint(point
                    , wideExtents, navGroup.filter
                    , out result);
            }

            return result;
        }

        /// <summary>
        /// Finds a path from the start point to the end point using the <see cref="path"/> object.
        /// </summary>
        /// <remarks>
        /// <para
        /// ><b>Warning</b>: The path object must be available before calling this method.
        /// </para>
        /// <para>
        /// This method will perform full planning only if <paramref name="start"/> or 
        /// <paramref name="end"/> is outside the existing path.
        /// </para>
        /// <para>
        /// The input points are all expected to be valid. (E.g. polyRef != 0)
        /// </para>
        /// </remarks>
        /// <param name="start">
        /// A valid navigation mesh point representing the start of the path.
        /// </param>
        /// <param name="end">
        /// A valid navigation mesh point representing the end of the path.
        /// </param>
        /// <returns>The length of the path, or -1 on failure.</returns>   
        public int PlanPath(NavmeshPoint start, NavmeshPoint end)
        {
            path.straightCount = 0;

            if (start.polyRef == 0 || end.polyRef == 0 || path == null)
                return -1;

            if (start.polyRef == end.polyRef)
            {
                // Debug.Log("Replan Path: Points in same poly.");
                path.pathCount = 1;
                path.path[0] = start.polyRef;
                data.navStatus = NavStatus.Sucess;
                return 1;
            }

            if (path.FixPath(start.polyRef, end.polyRef))
            {
                // Debug.Log("Replan Path: Fixed existing path");
                data.navStatus = NavStatus.Sucess;
                return path.pathCount;
            }

            // Debug.Log("Replace Path: Full query.");
            data.navStatus = navGroup.query.FindPath(start
                , end
                , navGroup.filter
                , path.path
                , out path.pathCount);

            return path.pathCount;
        }

        /// <summary>
        /// Finds a path from the start point to the end point and loads it into the corridor.
        /// </summary>
        /// <remarks>
        /// <para>
        /// <b>Warning</b>: The corridor must be available before calling this method.
        /// </para>
        /// <para>
        /// The input points are all expected to be valid. (E.g. polyRef != 0)
        /// </para>
        /// </remarks>
        /// <param name="start">
        /// A valid navigation mesh point representing the start of the path.
        /// </param>
        /// <param name="end">
        /// A valid navigation mesh point representing the end of the path.
        /// </param>
        /// <returns>The length of the path, or -1 on failure.</returns>
        public int PlanCorridor(NavmeshPoint start, NavmeshPoint end)
        {
            // Note: Don't try to re-use the current corridor.  Unlike
            // paths, the corridor is more likely to become malformed
            // over time.

            int pathCount;
            if (start.polyRef == 0 || end.polyRef == 0 || corridor == null)
                return -1;

            if (start.polyRef == end.polyRef)
            {
                corridor.Reset(start);
                corridor.MoveTarget(end.point);
                data.navStatus = NavStatus.Sucess;
                return 1;
            }
            else
            {
                data.navStatus = navGroup.query.FindPath(start
                    , end
                    , navGroup.filter
                    , agentGroup.pathBuffer
                    , out pathCount);
            }

            corridor.Reset(start);

            if (pathCount > 0)
            {
                corridor.SetCorridor(end.point
                    , agentGroup.pathBuffer
                    , pathCount);
            }

            return pathCount;
        }

        /// <summary>
        /// True if the two points are near each other. (Less accurate check.)
        /// </summary>
        /// <param name="u">Point u.</param>
        /// <param name="v">Point v.</param>
        /// <returns>True if the two points are near each other. </returns>
        public bool IsNear(Vector3 u, Vector3 v)
        {
            return Vector3Util.IsInRange(u, v, data.radiusNear, data.heightTolerence);
        }

        /// <summary>
        /// True if the two points are in range of each other. (Based on the current goal type.)
        /// </summary>
        /// <param name="u">Point u.</param>
        /// <param name="v">Point v.</param>
        /// <returns>True if the two points are in range of each other. </returns>
        public bool IsInRange(Vector3 u, Vector3 v)
        {
            switch (data.goalType)
            {
                case RangeType.At:
                    return Vector3Util.IsInRange(u, v, data.radiusAt, data.heightTolerence);
                case RangeType.Near:
                    return Vector3Util.IsInRange(u, v, data.radiusNear, data.heightTolerence);
                default:
                    // Assume most restrictive.
                    return Vector3Util.SloppyEquals(u, v, ExactTolerance);
            }
        }

        /// <summary>
        /// Transfers the crowd agent's values to the 'desired' values. (E.g. crowd postion to 
        /// desired position) [Owner: Planner]
        /// </summary>
        /// <remarks>s
        /// <para>
        /// The <see cref="crowdAgent"/> must be available before calling this method.
        /// </para>
        /// </remarks>
        public void SyncCrowdToDesired()
        {
            data.desiredPosition.polyRef  = crowdAgent.PositionPoly;
            data.desiredPosition.point = crowdAgent.Position;

            data.desiredVelocity = crowdAgent.Velocity;
            data.desiredSpeedSq = data.desiredVelocity.sqrMagnitude;
        }

        /// <summary>
        /// Removes the agent from the crowd. (Safe to call if the agent is not in the crowd.)
        /// </summary>
        public void RemoveFromCrowd()
        {
            if (crowdAgent != null)
            {
                navGroup.crowd.RemoveAgent(crowdAgent);
                crowdAgent = null;
            }
        }

        /// <summary>
        /// Adds the agent to the crowd if it is not already in the crowd. [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// Notes:
        /// </para>
        /// <ul>
        /// <li>Does not submit a move request to the crowd.</li>
        /// <li>Handles the <see cref="NavFlags.CrowdConfigUpdated"/> flag.</li>
        /// <li>Checks <paramref name="position"/> against the active agent.  If it is different, 
        /// the agent will be repositioned. (Removed/re-added.)
        /// </li>
        /// </ul>
        /// </remarks>
        /// <param name="position">The position to use when adding the agent. (Usually a known, 
        /// good navigation mesh position.)
        /// </param>
        /// <returns>
        /// The result of the add.  (Or the current agent reference if no add was needed.)
        /// </returns>
        public CrowdAgent AddToCrowd(Vector3 position)
        {
            if (navGroup.crowd == null)
                return null;
            if (crowdAgent == null)
            {
                crowdAgent = navGroup.crowd.AddAgent(position, crowdConfig);
            }
            else
            {
                if (crowdAgent.Position != position)
                {
                    RemoveFromCrowd();
                    crowdAgent = navGroup.crowd.AddAgent(position, crowdConfig);
                } 
                else if ((data.flags | NavFlag.CrowdConfigUpdated) != 0)
                    crowdAgent.SetConfig(crowdConfig);
            }

            data.flags &= ~NavFlag.CrowdConfigUpdated;
            return crowdAgent;
        }

        /// <summary>
        /// Sets up the corridor assets.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Notes: Resets the corridor to the current navigation position.  So the position
        /// should be valid before getting the corridor.
        /// </para>
        /// <ul>
        /// <li>This operation sets/unsets the corridor in-use flag as appropriate.</li>
        /// <li>Re-uses existing assests as appropriate.</li>
        /// <li>Utilizes the agent group pool.</li>
        /// </ul>
        /// </remarks>
        /// <param name="enabled">
        /// True if the corridor assets should be made ready to use.  False to release all corridor 
        /// assets.
        /// </param>
        public void SetCorridorAssets(bool enabled)
        {
            if (enabled)
            {
                if (corridor == null)
                {
                    corridor = 
                        agentGroup.GetCorridor(data.position, navGroup.query, navGroup.filter);
                }
                data.flags |= NavFlag.CorridorInUse;
            }
            else
            {
                agentGroup.ReturnCorridor(corridor);
                corridor = null;
                data.flags &= ~NavFlag.CorridorInUse;
            }
        }

        /// <summary>
        /// Sets up the path assets.
        /// </summary>
        /// <remarks>
        /// <para>Notes:</para>
        /// <ul>
        /// <li>This operation sets/unsets the path in-use flag as appropriate.</li>
        /// <li>Re-uses existing assest as appropriate.</li>
        /// <li>Utilizes the agent group pool.</li>
        /// </ul>
        /// </remarks>
        /// <param name="enabled">
        /// True if the path assets should be made ready to use.  False to release all path assets.
        /// </param>
        public void SetPathAssets(bool enabled)
        {
            if (enabled)
            {
                if (path == null)
                    path = agentGroup.GetPath();

                data.flags |= NavFlag.PathInUse;
            }
            else
            {
                agentGroup.ReturnPath(path);
                path = null;
                data.flags &= ~NavFlag.PathInUse;
            }
        }

        /// <summary>
        /// Recommends a navigation mode based on the current state of the agent.
        /// </summary>
        /// <returns>A recommended navigation mode.</returns>
        public NavMode ChooseMoveToMode()
        {
            CrowdManager crowd = navGroup.crowd;

            if ((data.flags & NavFlag.UseCrowd) != 0 && 
                ((crowdAgent != null) || (crowd != null && crowd.AgentCount < crowd.MaxAgents)))
            {
                // Want to use the crowd, and can use crowd.
                if ((data.flags & NavFlag.GoalIsDynamic) == 0)
                    return NavMode.MoveToCrowd;
                else
                    return NavMode.FollowGoalCrowd;
            }

            //Debug.LogWarning(string.Format("{0}: Flags: {1}, CrowdAgent: {2}, Crowd: {3}, CanFitInCrowd: {4}"
            //    , transform.name
            //    , data.flags
            //    , (crowdAgent != null)
            //    , (crowd != null)
            //    , (crowd.AgentCount < crowd.MaxAgents)));

            return NavMode.MoveToSimple;
        }
    }
}
