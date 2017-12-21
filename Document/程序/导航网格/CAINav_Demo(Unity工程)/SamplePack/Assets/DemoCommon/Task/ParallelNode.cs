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
    /// Runs all tasks in parallel until one or all complete, depending on the configuration.
    /// </summary>
    /// <remarks>
    /// The Exit method will only be called on child tasks when the
    /// the node is exited.  So multiple children may be ouside the
    /// Inactive state.
    /// </remarks>
    public class ParallelNode
        : BaseTask
    {
        private readonly ITask[] mTasks;
        private readonly CompletionPolicy mCompletionPolicy = CompletionPolicy.All;

        private int mCount = 0;

        private ParallelNode(int size, CompletionPolicy completionPolicy)
        {
            mTasks = new ITask[System.Math.Max(1, size)];
            this.mCompletionPolicy = completionPolicy;
        }

        /// <summary>
        /// Runs all tasks in the order they were loaded during construction.
        /// </summary>
        /// <remarks>
        /// <p>If the completion policy is Any, then execution of tasks will halt
        /// as soon as one is detected to have finished.  Otherwise all tasks will be
        /// run during each update.</p>
        /// <p>If the completion policy is All, then all tasks will be run on the first
        /// update call.  During susequenct update calls, only active tasks will be run, 
        /// until all are complete.</p>
        /// <p>If failure detection is enabled, then execution of tasks will
        /// halt as soon as any task fails.  All remaining active tasks will
        /// be exited.</p>
        /// </remarks>
        /// <returns>The post-update state of the aggregate task.</returns>
        protected sealed override void LocalUpdate(AgentContext context)
        {
            bool isFirstUpdate = false;
            if (state == TaskState.Inactive)
            {
                isFirstUpdate = true;
                state = TaskState.Active;
            }

            int completionCount = 0;

            bool isComplete = false;
            for (int i = 0; i < mCount; i++)
            {
                ITask task = mTasks[i];

                TaskState childState = task.State;
                if (isFirstUpdate || childState == TaskState.Active)
                {
                    childState = task.Update(context);
                    if (childState == TaskState.Complete)
                        completionCount++;
                    else if (childState == TaskState.Failed)
                    {
                        state = TaskState.Failed;
                        return;
                    }
                    if (mCompletionPolicy == CompletionPolicy.Any
                                && completionCount > 0)
                    {
                        isComplete = true;
                        break;
                    }
                }
            }

            if (isComplete || mTasks.Length == completionCount)
                    state = TaskState.Complete;
        }

        protected override bool LocalExit(AgentContext context)
        {
            bool exitComplete = true;
            // Reverse order is not required.  But it is safest
            // an provides the most flexibility in how the children
            // interact.
            for (int i = mCount - 1; i > -1; i--)
            {
                if (mTasks[i].State != TaskState.Inactive && !mTasks[i].Exit(context))
                    exitComplete = false;
            }
            return exitComplete;
        }

        public static ParallelNode Create(int size, CompletionPolicy policy)
        {
            return new ParallelNode(size, policy);
        }

        public ParallelNode Load(ITask task)
        {
            if (task == null || mCount == mTasks.Length)
                return null;

            mTasks[mCount++] = task;

            return this;
        }
    }
}
