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
    /// Core agent navigation data.
    /// </summary>
    public struct NavAgentData
    {
        /*
         * Design note:  
         * 
         * Keep this as a structure for now.  Want it to be interop friendly.
         * Also, don't really want references accidentally getting passed all over the place.
         * 
         */

        /// <summary>
        /// The status of the most recent navigation mesh or query operation.
        /// </summary>
        public NavStatus navStatus;

        /// <summary>
        /// The current navigation position. 
        /// [Owner: Mover]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This field is usually managed by the mover.
        /// </para>
        /// <para>
        /// The normal process for forcing movement to a position is for the AI to set the goal 
        /// and 'force move' flag to alert the mover to move immediately to the goal.  The mover's 
        /// action then triggers replannig by the planner.
        /// </para>
        /// <para>
        /// Expected to be valid for the current planner. So if the planner uses the 
        /// navigation mesh, then this value should be within a searchable distance of the 
        /// navigation mesh. Otherwise it may be ignored by the planner.
        /// </para>
        /// <para>
        /// May or may not be pre-constrained to the navigation mesh.  So it may have 
        /// a polygon reference of zero. Planners must be able to deal with this.
        /// </para>
        /// <para>
        /// Expected to be a navigation position, which may not be a world position.  It is common, 
        /// for example, for the agent world position to be height-offset from the navigation 
        /// position.  The AI should take this into account.
        /// </para>
        /// </remarks>
        public NavmeshPoint position;

        /// <summary>
        /// The current agent rotation. 
        /// [Owner: Mover]
        /// </summary>
        public Quaternion rotation;

        /// <summary>
        /// The desired position. 
        /// [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// The planner, or the AI if the planner is suspended, sets this value.  The mover is 
        /// responsible for implementing movement to this position.
        /// </para>
        /// </remarks>
        public NavmeshPoint desiredPosition;

        /// <summary>
        /// The desired velocity. 
        /// [Owner: Planner]
        /// </summary>
        public Vector3 desiredVelocity;

        /// <summary>
        /// The desired speed squared. (speed * speed)
        /// [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This is a conveniance value.  It elimitates the need for users to calculate the value 
        /// individually.
        /// </para>
        /// </remarks>
        public float desiredSpeedSq;

        /// <summary>
        /// Navigation related flags. 
        /// [Owner: Various]
        /// </summary>
        public NavFlag flags;

        /// <summary>
        /// The desired navigation mode. 
        /// [Owner: AI]
        /// </summary>
        /// <remarks>
        /// <para>
        /// The AI sets this value.  The planner is then responsible for transitioning to the mode.  
        /// If the transition fails, the planner is expected to set the 
        /// <see cref="NavFlags.PlannerFailed"/> flag.
        /// </para>
        /// </remarks>
        public NavMode targetMode;

        /// <summary>
        /// The desired final position. 
        /// [Owner: AI]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This value is expected to be valid for the current planner. So if the planner uses the 
        /// navigation mesh, then this value should be within a searchable distance of the 
        /// navigation mesh. Otherwise it may be ignored by the planner.
        /// </para>
        /// <para>
        /// This value may or may not be pre-constrained to the navigation mesh.  So it may have a 
        /// polygon reference of zero. Planners must be able to deal with this.</para>
        /// </remarks>
        public NavmeshPoint goal;

        /// <summary>
        /// The planner's goal. [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This is the goal the planner is using, irrespective of the exact position of AI goal.
        /// </para>
        /// <para>
        /// The most common reason for a divergence with the AI goal is that the AI goal is not on 
        /// the surface of the navigation mesh, and the planner needs to constrain the goal to the 
        /// mesh.
        /// </para>
        /// <para>
        /// The AI can use this value to determine when it needs to switch to a different planner 
        /// to reach its goal.
        /// </para>
        /// </remarks>
        public NavmeshPoint plannerGoal;

        /// <summary>
        /// The desired rotation to be at when the goal position is reached.
        /// [Owner: AI]
        /// </summary>
        public Quaternion goalRotation;

        /// <summary>
        /// The type of range check to perform when determining if a position has been reached. 
        /// [Owner: AI]
        /// </summary>
        public RangeType goalType;

        /// <summary>
        /// The radius to use when determining if a position has been reached. (For accurate checks.) 
        /// [Owner: AI]
        /// </summary>
        public float radiusAt;

        /// <summary>
        /// The radius to use when determining if a point is in the vicinity of another point. 
        /// [Owner: AI]
        /// </summary>
        public float radiusNear;

        /// <summary>
        /// The height tolerance to use for range checks. [Owner: AI]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This value is useful for range changes that need an accurate xz-plane tolerance with a 
        /// loose height tolerance. This is a common need since the height of the navigation mesh
        /// is often offset from the surface of the geometry.
        /// </para>
        /// </remarks>
        public float heightTolerence;
    }
}
