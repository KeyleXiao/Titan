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
    /// Runs the tasks sequentially, updating a maximum of one child
    /// per update.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Example:
    /// Call update on sequence.
    /// Sequence updates first child. Child returns complete.
    /// Sequence returns active.
    /// Call update on sequence.
    /// Sequence updates child 2.  Child returns complete.
    /// Sequence returns active.
    /// Call update on sequence.
    /// Sequence updates child 3.  Child returns active.
    /// Sequence returns active.
    /// Etc.
    /// </para>
    /// </remarks>
    public sealed class SequentialNode
        : BaseTask
    {
        private readonly ITask[] mTasks;
        private int mCount = 0;

        private int mSelectedTask = -1;

        public SequentialNode(int size)
        {
            size = System.Math.Max(1, size);
            mTasks = new ITask[size];
        }

        protected override void LocalUpdate(AgentContext context)
        {
            if (state == TaskState.Inactive)
            {
                if (mCount == 0)
                {
                    state = TaskState.Complete;
                    return;
                }

                mSelectedTask = 0;
                state = TaskState.Active;
            }

            state = mTasks[mSelectedTask].Update(context);

            if (state == TaskState.Complete)
            {
                if (++mSelectedTask < mTasks.Length)
                    // Current task has completed OK and there
                    // are more to run.
                    state = TaskState.Active;
                else
                    // Prepare for the exit process.
                    mSelectedTask--;
            }
        }

        protected override bool LocalExit(AgentContext context)
        {
            // Exit behaivors in reverse order.
            while (mSelectedTask != -1)
            {
                if (mTasks[mSelectedTask].Exit(context))
                    mSelectedTask--;
                else
                    return false;
            }
            return true;
        }

        public static SequentialNode Create(int size)
        {
            return new SequentialNode(size);
        }

        public SequentialNode Load(ITask task)
        {
            if (task == null || mCount == mTasks.Length)
                return null;

            mTasks[mCount++] = task;

            return this;
        }
    }
}
