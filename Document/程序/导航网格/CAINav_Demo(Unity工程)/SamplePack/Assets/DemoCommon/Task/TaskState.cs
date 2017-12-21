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

namespace org.critterai.agent.task
{
    /// <summary>
    /// Task states.
    /// </summary>
    public enum TaskState
    {
        /// <summary>
        /// The beahvior is not currently running and can
        /// be ignored.
        /// </summary>
        /// <remarks>
        /// A task will transition to this state only through
        /// a call to the exit method.
        /// <p>Permitted transition from: All</p>
        /// <p>Permitted transition to: Active, Complete, Failed</p>
        /// </remarks>
        Inactive,

        /// <summary>
        /// The task is running.  It's update should be run
        /// periodically to maintain the task.  The task 
        /// should not be abandoned without exiting properly using
        /// the exit method.
        /// </summary>
        /// <remarks>
        /// Some tasks will never enter this state because they
        /// complete immediately.
        /// <p>Permitted transition from:  Inactive, Complete, Failed</p>
        /// <p>Permitted transition to: All</p>
        /// </remarks>
        Active,

        /// <summary>
        /// The task failed during the last update.
        /// </summary>
        /// <remarks>
        /// <p>Permitted transition from: Inactive, Active, Complete</p>
        /// <p>Permitted transition to: Inactive, Active, Complete</p>
        /// </remarks>
        Failed,

        /// <summary>
        /// The task completed during the last call to update.
        /// </summary>
        /// <remarks>
        /// <p>Permitted transition from: Inactive, Active, Failed</p>
        /// <p>Permitted transition to: Inactive, Active, Failed</p>
        /// </remarks>
        Complete,

        /// <summary>
        /// An exit request has been made and is being processed.
        /// The exit method should continue to be called until
        /// a successful transition to Inactive.
        /// </summary>
        /// <remarks>
        /// <p>Permitted transition from: Active</p>
        /// <p>Permitted transition to: Inactive, Failed (if update method is called.)</p>
        /// </remarks>
        Exiting
    }
}
