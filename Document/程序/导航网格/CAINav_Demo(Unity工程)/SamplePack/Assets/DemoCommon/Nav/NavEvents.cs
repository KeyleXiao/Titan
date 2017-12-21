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
using org.critterai.nav.u3d;
using org.critterai.nav;

namespace org.critterai.nav.u3d
{
    /// <summary>
    /// Navigation event type.
    /// </summary>
    /// <remarks>
    /// <para>
    /// <b>Note:</b> Goal related events are disabled while a agent's goal is marked as
    /// dynamic. (E.g. While following.)
    /// </para>
    /// </remarks>
    public enum NavEventType : byte
    {
        /// <summary>
        /// The agent has reached its goal.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This event type is disabled while the agent's goal is dynamic.  
        /// </para>
        /// <para>
        /// This event is sent only when the agent trasitions to 'at goal'.  It is
        /// not send out continuously while at the goal.
        /// </para>
        /// </remarks>
        ReachedGoal,

        /// <summary>
        /// A new goal has been set.
        /// </summary>
        /// <remarks>
        /// This event type is diabled while the agent's goal is dyanmic.
        /// </remarks>
        NewGoal,

        /// <summary>
        /// Navigation has failed.
        /// </summary>
        NavFailed,

        /// <summary>
        /// Navigation has been suspended.
        /// </summary>
        NavSuspended,

        /// <summary>
        /// Navigation has activated. (Exited a failed or suspected state.)
        /// </summary>
        NavActivated,
    }

    public class NavigationEvent
        : System.EventArgs
    {
        public readonly NavEventType navEvent;

        public NavigationEvent(NavEventType navEvent)
        {
            this.navEvent = navEvent;
        }
    }
}
