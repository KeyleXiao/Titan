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
using System;

namespace org.critterai.agent.task
{
    /// <summary>
    /// Implements a task.
    /// </summary>
    /// <remarks>
    /// <p>Implementations of this interface require strict adherence to the
    /// following contract:</p>
    /// 
    /// <p>Once the Update method is called the task is considered in use
    /// until the Exit method finishes.  This is because the task may
    /// have reserved resorces that need to be released.</p>
    /// 
    /// <p>Implementations that aggregate tasks are required to specify how
    /// the Exit method of child tasks is handled, both on child completion
    /// and during the node exit process.
    /// </p>
    /// 
    /// <p>Instances are expected to have only one owner operating on it at a 
    /// time. Otherwise task is undefined.</p>
    /// 
    /// <p>See the method remarks for further contract requirements.</p>
    /// 
    /// </remarks>
    public interface ITask
    {
        /// <summary>
        /// The current state of the task.
        /// </summary>
        TaskState State { get; }

        /// <summary>
        /// Run the task.
        /// </summary>
        /// <remarks>
        /// <p>Calling this operation while in any state other than Active
        /// or Inactive will result in immediate failure.</p>
        /// <p>Calling this method while the task is in the Exiting state
        /// will result in the exit process continuing with transition to the
        /// failure state once the exit completes.</p>
        /// </remarks>
        /// <returns>The state of the task at the end of the update.
        /// </returns>
        TaskState Update(AgentContext context);

        /// <summary>
        /// Performs any necessary cleanup of the task then transitions it 
        /// to the <see cref="TaskState.Inactive"/> state.  
        /// </summary>
        /// <remarks>
        /// <p>Once the Update method is called, the task is considered to
        /// still be in use until this method is complete.</p>
        /// <p>Calling this method will have not side effects if called
        /// while in the Inactive state.</p>
        /// </remarks>
        /// <returns>True if the exit operation is complete.  
        /// False if the exit operation needs to be called again in order 
        /// to finish the process.</returns>
        Boolean Exit(AgentContext context);
    }
}
