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
    /// Provides task utilities.
    /// </summary>
    public static class TaskUtil
    {
        /// <summary>
        /// Returns true if the state one of the 'finished' states. (E.g. Failed or Complete.)
        /// </summary>
        /// <param name="state">The state.</param>
        /// <returns>True if the state is one of the 'finished' states.</returns>
        public static bool IsFinished(TaskState state)
        {
            return (state == TaskState.Complete || state == TaskState.Failed);
        }

        /// <summary>
        /// Returns true if the state indicates it is legal to call the task's update method.
        /// </summary>
        /// <param name="state">The state.</param>
        /// <returns>True is the task's update method may be called.</returns>
        public static bool IsRunnable(TaskState state)
        {
            return (state == TaskState.Inactive || state == TaskState.Active);
        }


        /// <summary>
        /// Processes the task based on its current state.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Only performs a single action per call.  So it will always take multiple calls for a 
        /// task to be fully processed.
        /// </para>
        /// <para>
        /// The <paramref name="failure"/> flag indicates whether the task is in a failed state.
        /// (Note: It may still need processing.)
        /// </para>
        /// </remarks>
        /// <param name="task">The behaivor to process.</param>
        /// <param name="failure">True if the task is detected in a failed state.</param>
        /// <returns>True if the task needs more processing.</returns>
        public static bool Process(AgentContext context, ITask task, out bool failure)
        {
            failure = false;
            switch (task.State)
            {
                case TaskState.Active:
                    task.Update(context);
                    return true;
                case TaskState.Complete:
                    return !task.Exit(context);
                case TaskState.Exiting:
                    return !task.Exit(context);
                case TaskState.Failed:
                    failure = true;
                    return !task.Exit(context);
                case TaskState.Inactive:
                    task.Update(context);
                    return true;
            }
            return true;
        }

        /// <summary>
        /// Processes the task based on its current state.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Only performs a single action per call.  So it will always take multiple calls for a 
        /// task to be fully processed.
        /// </para>
        /// </remarks>
        /// <param name="task">The behaivor to process.</param>
        /// <returns>True if the task needs more processing.</returns>
        public static bool Process(AgentContext context, ITask task)
        {
            switch (task.State)
            {
                case TaskState.Active:
                    task.Update(context);
                    return true;
                case TaskState.Complete:
                    return !task.Exit(context);
                case TaskState.Exiting:
                    return !task.Exit(context);
                case TaskState.Failed:
                    return !task.Exit(context);
                case TaskState.Inactive:
                    task.Update(context);
                    return true;
            }
            return true;
        }

        /// <summary>
        /// Processes the task based on its current state, optimized to complete the task more 
        /// quickly.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The exit process will be triggered as soon as completion is dectected.  So this 
        /// method may perform two operations (update and exit) in a single call.
        /// </para>
        /// </remarks>
        /// <param name="task">The task to process.</param>
        /// <returns>True if the task needs more processing.</returns>
        public static bool ProcessImmediate(AgentContext context, ITask task)
        {
            switch (task.State)
            {
                case TaskState.Active:
                    if (IsFinished(task.Update(context)))
                        return !task.Exit(context);
                    return true;
                case TaskState.Complete:
                    return !task.Exit(context);
                case TaskState.Exiting:
                    return !task.Exit(context);
                case TaskState.Failed:
                    return !task.Exit(context);
                case TaskState.Inactive:
                    if (IsFinished(task.Update(context)))
                        return !task.Exit(context);
                    return true;
            }
            return true;
        }
    }
}
