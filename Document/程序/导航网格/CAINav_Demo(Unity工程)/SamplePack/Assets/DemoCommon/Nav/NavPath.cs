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
    /// Provides basic navigation buffers and features useful for path management.
    /// </summary>
    public class NavPath
    {
        /// <summary>
        /// The path buffer.
        /// </summary>
        public uint[] path;

        /// <summary>
        /// The straight path buffer.
        /// </summary>
        public uint[] straightPath;

        /// <summary>
        /// The straight path point buffer.
        /// </summary>
        public Vector3[] straightPoints;

        /// <summary>
        /// The straight path flag buffer.
        /// </summary>
        public WaypointFlag[] straightFlags;

        /// <summary>
        /// The length of the path.
        /// </summary>
        public int pathCount = 0;

        /// <summary>
        /// The length of the straight path.
        /// </summary>
        public int straightCount = 0;

        /// <summary>
        /// Conststructor.
        /// </summary>
        /// <param name="maxPathSize">The maximum length of the path. [Limit: >= 1]</param>
        /// <param name="maxStraightPathSize">
        /// The maximum length of the straight path. [Limit: >= 0]
        /// </param>
        public NavPath(int maxPathSize, int maxStraightPathSize)
        {
            path = new uint[Mathf.Max(1, maxPathSize)];
            maxStraightPathSize = Mathf.Max(1, maxStraightPathSize);
            straightPoints = new Vector3[maxStraightPathSize];
            straightFlags = new WaypointFlag[maxStraightPathSize];
            straightPath = new uint[maxStraightPathSize];
        }

        /// <summary>
        /// The reference of the last polygon in the path.
        /// </summary>
        public uint LastPathPolyRef
        {
            get { return (pathCount == 0 ? 0 : path[pathCount - 1]); }
        }

        /// <summary>
        /// Returns the path index of the provided polygon reference.
        /// (Optimized for references nearer the end of the path.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// The search is performed within the bounds of the <paramref name="startIndex"/> and the 
        /// end of the path.
        /// </para>
        /// </remarks>
        /// <param name="startIndex">The index of the beginning of the path. (Zero if the full path 
        /// should be searched.)  [Limit: 0 &lt;= value &lt; pathCount]
        /// </param>
        /// <param name="endRef">The reference to search for.</param>
        /// <returns>The path index, or -1 if the polygon reference could not be found.</returns>
        public int FindPolyRefReverse(int startIndex, uint endRef)
        {
            for (int i = pathCount - 1; i >= startIndex; i--)
            {
                if (path[i] == endRef)
                    return i;
            }
            return -1;
        }

        /// <summary>
        /// Returns the index of the provided polygon reference within the path.
        /// </summary>
        /// <param name="startIndex">The start index of the search.</param>
        /// <param name="polyRef">The polygon reference to search for.</param>
        /// <returns>
        /// The index of the provided polygon referenced within the path. Or -1 if not found.
        /// </returns>
        public int FindPolyRef(int startIndex, uint polyRef)
        {
            for (int i = startIndex; i < pathCount; i++)
            {
                if (path[i] == polyRef)
                    return i;
            }
            return -1;
        }

        /// <summary>
        /// Gets the point at the specified straight path buffer index.
        /// </summary>
        /// <param name="index">The straight path index.</param>
        /// <returns>The point at the specified straight path buffer index.</returns>
        public NavmeshPoint GetStraightPoint(int index)
        {
            return new NavmeshPoint(straightPath[index], straightPoints[index]);
        }

        /// <summary>
        /// Attempts to fix the existing path such that <c>path[0]</c> contains 
        /// <paramref name="startRef"/> and <c>path[pathCount - 1]</c> contains 
        /// <paramref name="endRef"/>
        /// </summary>
        /// <remarks>
        /// <para>
        /// Using this method can avoid unnecessary full re-planning. It will succeed as long as 
        /// both the start and end polygons exist anywhere, in any order, in the current path.
        /// </para>
        /// <para>
        /// If the fix is successful, the straight path will be set to zero.
        /// </para>
        /// </remarks>
        /// <param name="startRef">The polygon reference of the desired path start.</param>
        /// <param name="endRef">The polygon reference of the desired path end.</param>
        /// <returns>TRUE if the path was successfully fixed.</returns>
        public bool FixPath(uint startRef, uint endRef)
        {
            // Check for early exit special cases.

            if (pathCount == 0)
                return false;

            if (startRef == endRef)
            {
                path[0] = startRef;
                pathCount = 1;
                straightCount = 0;
                return true;
            }
            if (startRef == path[0] && endRef == path[pathCount - 1])
                return true;

            // Get the path indices of the polygons, if available.

            int iStart = FindPolyRef(0, startRef);
            if (iStart == -1)
                return false;

            int iEnd = FindPolyRefReverse(0, endRef);
            if (iEnd == -1)
                return false;

            // We know the path can be fixed.

            pathCount = Mathf.Abs(iEnd - iStart) + 1;

            if (iStart > iEnd)
            {
                System.Array.Reverse(path, iEnd, pathCount);
                iStart = iEnd;
            }

            // Trim the path.
            for (int i = 0; i < pathCount; i++)
            {
                path[i] = path[iStart + i];
            }

            straightCount = 0;
            return true;
        }

        /// <summary>
        /// Builds a standard straight path from the start to the goal point using the exiting path.
        /// </summary>
        /// <remarks>
        /// <para>Limits:</para>
        /// <ul>
        /// <li>The path must exist and both the start and goal points must be within the path.</li>
        /// <li>
        /// The goal's polygon reference must be in or after the start points polygon reference.
        /// </li>
        /// </ul>
        /// </remarks>
        /// <param name="start">The start point.</param>
        /// <param name="goal">The goal point.</param>
        /// <returns>The status of the operation.</returns>
        public NavStatus BuildStraightPath(NavmeshPoint start, NavmeshPoint goal, NavmeshQuery query)
        {
            int iStart = FindPolyRef(0, start.polyRef);
            int iGoal = -1;

            if (iStart != -1)
                iGoal = FindPolyRefReverse(iStart, goal.polyRef);

            if (iGoal == -1)
                return (NavStatus.Failure | NavStatus.InvalidParam);

            NavStatus status = query.GetStraightPath(start.point, goal.point
                , path, iStart, iGoal - iStart + 1
                , straightPoints, straightFlags, straightPath, out straightCount);

            if (straightCount == 0)
                return NavStatus.Failure;

            return status;
        }

        /// <summary>
        /// Builds a straight path and gets the point farthest along the path that does not exceed 
        /// the specified maximum number of polygons.
        /// </summary>
        /// <remarks>
        /// <para>Limits:</para>
        /// <ul>
        /// <li>The path must exist and both the start and goal points must be within the path.</li>
        /// <li>
        /// The goal's polygon reference must be in or after the start points polygon reference.
        /// </li>
        /// </ul>
        /// <para>
        /// <b>Special Case:</b>The result will exceed <paramref name="maxLength"/> if the 
        /// first straight path point is greater than <paramref name="maxLength"/> from the start 
        /// point.
        /// </para>
        /// </remarks>
        /// <param name="start">The start point within the current path.</param>
        /// <param name="goal">The end point located in or after the start point polygon.</param>
        /// <param name="maxLength">
        /// The maximum allowed number of polygons between the start and target. [Limit: >= 1]
        /// </param>
        /// <param name="target">The resulting target point.</param>
        /// <returns>The straight path index of the target point, or -1 on error.</returns>
        public int GetLocalTarget(NavmeshPoint start, NavmeshPoint goal, int maxLength
            , NavmeshQuery query
            , out NavmeshPoint target)
        {
            if (NavUtil.Failed(BuildStraightPath(start, goal, query)))
            {
                target = new NavmeshPoint();
                return -1;
            }

            int targetIndex = straightCount;  // Will be decremented.
            int iStart = FindPolyRef(0, start.polyRef);

            // Start at the end of the straight path and search back toward
            // the start until the number of polygons is less than
            // maxLength.
            uint targetRef = 0;
            do
            {
                targetIndex--;

                targetRef = (straightPath[targetIndex] == 0 ?
                        goal.polyRef : straightPath[targetIndex]);
            }
            while (FindPolyRefReverse(iStart, targetRef) - iStart + 1 
                    > maxLength 
                && targetIndex > 0);

            target = new NavmeshPoint(targetRef, straightPoints[targetIndex]);

            return targetIndex;
        }
    }
}
