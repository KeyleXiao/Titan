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
using System;

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// Navigation related flags.
    /// </summary>
    [Flags]
    public enum NavFlag : uint
    {
        /// <summary>
        /// The mover should bypass normal movement restrictions and move to 
        /// the goal position and rotation immediately.
        /// [Set: AI, Unset: Mover]
        /// </summary>
        /// <remarks>
        /// <para>Usually set by the AI and cleared by the mover. The mover
        /// is responsible for notifying the planner of any changes
        /// in position. Planners are not responsible for monitoring this flag.
        /// </para>
        /// <para>Designers should expect that setting this flag will
        /// force an expensive replan, since most planners are designed
        /// for infrequent forced changes to the position.</para>
        /// </remarks> 
        ForceMove = 0x01,

        /// <summary>
        /// The goal is dynamic. (Changes frequency in small increments. E.g. Following behavior.)
        /// [Owner: AI]
        /// </summary>
        /// <remarks>
        /// <para>
        /// This flag is only applicable if a planner that supports dynamic goals is
        /// available.  If none is available then follow behavior may be expensive.
        /// </para>
        /// </remarks>
        GoalIsDynamic = 0x02,

        /// <summary>
        /// The goal has been changed in a manner that requires handling
        /// by the planner. (I.e. A new goal has been requested.)
        /// [Set: AI, Unset: Planner]
        /// </summary>
        /// <remarks>
        /// <para>Usually set by the AI and cleared by the planner.</para>
        /// </remarks>
        HasNewGoal  = 0x04,

        /// <summary>
        /// The position has been changed in a manner that requires feedback
        /// into the planner.
        /// [Set: Mover, Unset: Planner]
        /// </summary>
        /// <remarks>
        /// <para>Usually set by the mover and cleared by the planner.</para>
        /// <para>The planner usually assumes its orders are being carried out.
        /// So the position is only evaluated when the planner is initialized.
        /// This flag tells the planner that it needs to recheck the position.
        /// </para>
        /// <para>Designers should expect that setting this flag will
        /// force an expensive replan, since most planners are designed
        /// for infrequent forced changes to the position.</para>
        /// </remarks>
        HasNewPosition = 0x08,

        /// <summary>
        /// A hint to use the crowd for movement. [Owner: AI]
        /// </summary>
        /// <remarks>This is not meant to be a directive.  It is used to
        /// allow one part of the AI to make a suggestion to another.</remarks>
        UseCrowd = 0x10,

        /// <summary>
        /// Set to indicate the goal rotation has a useful value.
        /// </summary>
        GoalRotationEnabled = 0x20,

        /// <summary>
        /// The planner failed.  (AI is expected to take corrective action.)
        /// [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// The planner is responsible for removing this flag when appropriate. 
        /// So it is normal for the flag to be removed when a planner starts up,
        /// just in case the flag was set by a previous planner.
        /// </remarks>
        PlannerFailed = 0x100,

        /// <summary>
        /// The mover failed.  (AI expected to take corrected action.)
        /// [Owner: Mover]
        /// </summary>
        /// <remarks>
        /// The mover is responsible for removing this flag when appropriate. 
        /// So it is normal for the flag to be removed when a mover starts up,
        /// just in case the flag was set by a previous mover.
        /// </remarks>
        MoverFailed = 0x200,

        /// <summary>
        /// The agent's crowd agent configuration has been updated and needs
        /// to be propogated to the crowd.
        /// [Set: AI, Unset: Planner]
        /// </summary>
        /// <remarks>
        /// This flag is usually set by the AI and handled by the planner.
        /// </remarks>
        CrowdConfigUpdated = 0x400,

        /// <summary>
        /// The planner is actively managing the corridor and its status is 
        /// up-to-date.
        /// [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>It is common for planners to leave the corridor in place but
        /// tempoarily untended.  So this flag should be checked before
        /// non-planners use corridor data.</para>
        /// </remarks>
        CorridorInUse = 0x1000,

        /// <summary>
        /// The planner is activaly managing the path and it status is
        /// up-to-date.
        /// [Owner: Planner]
        /// </summary>
        /// <remarks>
        /// <para>It is common for planners to leave the path in place but
        /// tempoarily untended.  So this flag should be checked before
        /// non-planners use path data.</para>
        /// </remarks>
        PathInUse = 0x2000,
    }
}
