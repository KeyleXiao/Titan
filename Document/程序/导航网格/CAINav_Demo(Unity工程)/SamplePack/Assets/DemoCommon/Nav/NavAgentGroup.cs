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
using Mathf = UnityEngine.Mathf;
using System.Collections.Generic;

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// Provides shared navigation configuration and resources.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This class provides a mix of default values, shared buffers, and pools of shared objects.
    /// </para>
    /// </remarks>
    public class NavAgentGroup
    {
        /// <summary>
        /// The default crowd configuration for all agents in the group.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Individual agents may deviate from this configuration over time.
        /// </para>
        /// </remarks>
        public CrowdAgentParams crowdAgentConfig;

        /// <summary>
        /// The default 'accurate' search radius.
        /// </summary>
        public float radiusAt = 0.1f;

        /// <summary>
        /// The default 'less accurate' search radius.
        /// </summary>
        public float radiusNear = 1.0f;

        /// <summary>
        /// The default search height tolerance.
        /// </summary>
        public float heightTolerance = 1.0f;

        /// <summary>
        /// The default angle tolerance.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Used to detect if two angles (usually rotations) are considered equal.
        /// </para></remarks>
        public float angleAt = 1.0f;

        /// <summary>
        /// A speed threshold used to determine when the agent is moving fast enough to apply 
        /// rotation changes.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This value is used when the rotation is derived from the velocity. (As happens when 
        /// the agent is part of a crowd.) Basically, if 
        /// <c>velocity.sqrMagniture &lt; turnThreashold</c>, then there is no need to update the 
        /// rotation. This helps prevent the agent roation from snapping around in wild directions 
        /// when at low velocities.
        /// </para>
        /// </remarks>
        public float turnThreshold = 0.05f;

        /// <summary>
        /// The maximum turn speed.
        /// </summary>
        public float maxTurnSpeed = 8;

        /// <summary>
        /// The maximum run speed.
        /// </summary>
        public float runSpeed = 3.5f;

        /// <summary>
        /// The maximum jog speed.
        /// </summary>
        public float jogSpeed = 2.8f;

        /// <summary>
        /// The maximum walk speed.
        /// </summary>
        public float walkSpeed = 1.5f;

        /// <summary>
        /// A shared temporary path buffer. [Length: <see cref="MaxPathSize"/>]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This buffer is for temporary use by all agents in the group. The content has no 
        /// meaning outside the method in which it is used.
        /// </para>
        /// </remarks>
        public readonly uint[] pathBuffer;

        private readonly int mMaxPathSize;
        private readonly int mMaxStraightPathSize;

        private readonly int mMaxCorridors;
        private readonly Stack<PathCorridor> mCorridors;

        private readonly int mMaxPaths;
        private readonly Stack<NavPath> mPaths;

        /// <summary>
        /// The default maximum path size.
        /// </summary>
        public int MaxPathSize
        {
            get { return mMaxPathSize; }
        }

        /// <summary>
        /// The default maximum straight path size.
        /// </summary>
        public int MaxStraightPathSize
        {
            get { return mMaxStraightPathSize; }
        }

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="crowdAgentConfig">
        /// The default agent configuration for  all agents in the group.
        /// </param>
        /// <param name="maxPathSize">The default maximum path size. [Limit: >= 1]</param>
        /// <param name="maxStraightPathSize">
        /// The default maximum straight path size. [Limit: >= 1]
        /// </param>
        /// <param name="maxPoolCorridors">
        /// The maximum number of corridors to allowed in the corridor pool. [Limit: >= 0]
        /// </param>
        /// <param name="maxPoolPaths">
        /// The maximum number of paths to allow in the path pool. [Limit: >= 0]
        /// </param>
        public NavAgentGroup(CrowdAgentParams crowdAgentConfig
            , int maxPathSize, int maxStraightPathSize, int maxPoolCorridors, int maxPoolPaths)
        {
            mMaxStraightPathSize = Mathf.Max(1, maxStraightPathSize);
            mMaxPathSize = Mathf.Max(1, maxPathSize);

            this.crowdAgentConfig = crowdAgentConfig;
            this.pathBuffer = new uint[maxPathSize];

            mMaxCorridors = Mathf.Max(0, maxPoolCorridors);
            mCorridors = new Stack<PathCorridor>(mMaxCorridors);

            mMaxPaths = Mathf.Max(0, maxPoolPaths);
            mPaths = new Stack<NavPath>(mMaxPaths);
        }

        /// <summary>
        /// Gets a standard corridor from the pool. (Or creates one if none is available.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// The corridor maximum path will equal <see cref="MaxPathSize"/>, and maximum corners 
        /// will equal <see cref="MaxStraightPathSize"/> .
        /// </para>
        /// </remarks>
        /// <param name="query">The query to assign to the corridor.</param>
        /// <param name="filter">The filter to assign to the corridor.</param>
        /// <returns>A standard corridor, or null on error.</returns>
        public PathCorridor GetCorridor(NavmeshPoint position, NavmeshQuery query, NavmeshQueryFilter filter)
        {
            if (mCorridors.Count > 0)
            {
                PathCorridor corr = mCorridors.Pop();

                if (PathCorridor.LoadLocals(corr, position, query, filter))
                    return corr;

                return null;
            }
            return new PathCorridor(mMaxPathSize, mMaxStraightPathSize, query, filter);
        }

        /// <summary>
        /// Returns a corridor to the pool. (If there is room.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// Invalid corridors are rejected. (E.g. Disposed or the wrong size.)
        /// </para>
        /// </remarks>
        /// <param name="corridor">The corridor to add to the pool.</param>
        public void ReturnCorridor(PathCorridor corridor)
        {
            if (mCorridors.Count >= mMaxCorridors || corridor == null || corridor.IsDisposed
                || corridor.MaxPathSize != mMaxPathSize
                || corridor.MaxCorners != mMaxStraightPathSize)
            {
                return;
            }

            PathCorridor.ReleaseLocals(corridor);
            mCorridors.Push(corridor);
        }

        /// <summary>
        /// Gets a standard path from the pool. (Or constructs one if none is available.)
        /// </summary>
        /// <returns>
        /// <para>
        /// The buffers will be sized based on <see cref="MaxPathSize"/> and 
        /// <see cref="MaxStraighPathSize"/>.
        /// </para>
        /// </returns>
        public NavPath GetPath()
        {
            if (mPaths.Count > 0)
                return mPaths.Pop();

            return new NavPath(mMaxPathSize, mMaxStraightPathSize);
        }

        /// <summary>
        /// Returns a path to the pool. (If there is room.)
        /// </summary>
        /// <remarks>
        /// <para>
        /// Invalid paths are rejected.(E.g. Wrong buffer sizes.)
        /// </para>
        /// </remarks>
        /// <param name="corridor">The path to add to the pool.</param>
        public void ReturnPath(NavPath path)
        {
            if (mPaths.Count >= mMaxPaths || path == null
                || path.path.Length != mMaxPathSize
                || path.straightPath.Length != mMaxStraightPathSize)
            {
                return;
            }

            path.pathCount = 0;
            path.straightCount = 0;

            mPaths.Push(path);
        }
    }
}
