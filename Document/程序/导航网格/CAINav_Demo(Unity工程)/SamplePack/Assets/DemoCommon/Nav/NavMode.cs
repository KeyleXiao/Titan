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

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// High level navigation planning mode.
    /// </summary>
    /// <remarks>
    /// The mode may indicate the target or current state, depending on
    /// context.
    /// </remarks>
    public enum NavMode : byte
    {
        /// <summary>
        /// The planner is suspended.
        /// </summary>
        Suspended,

        /// <summary>
        /// Place the agent into the crowd, but suspend it at its current position. (Zero speed.)
        /// </summary>
        SuspendInCrowd,

        /// <summary>
        /// The planner uses the crowd manager to reach an infrequently changing goal. 
        /// (Suitable for long distance planning.)
        /// </summary>
        MoveToCrowd,

        /// <summary>
        /// The planner uses using basic pathfinding to reach an infrequently changing goal.
        /// </summary>
        MoveToSimple,

        /// <summary>
        /// The planner ignores the navigation mesh to move to the goal.
        /// </summary>
        MoveToLocal,

        /// <summary>
        /// The planner uses the crowd manager to reach a frequently changing goal. (Not suitable 
        /// for long distance planning.)
        /// </summary>
        FollowGoalCrowd
    }
}
