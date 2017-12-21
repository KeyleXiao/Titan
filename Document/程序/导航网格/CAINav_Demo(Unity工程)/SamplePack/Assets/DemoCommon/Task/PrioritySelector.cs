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
    /// Searches an ordered list of tasks and associated conditions, 
    /// running the first task whose condition evaluates to TRUE.
    /// </summary>
    /// <remarks>
    /// <para>The order of the tasks in the list dictates priority.  During 
    /// evaluation, the first task whose associated condition evaluates 
    /// to TRUE will be selected to run.</para>
    /// <p>Th selector will complete if no conditions are selected.</p>
    /// </remarks>
    public sealed class PrioritySelector
        : BaseTask
    {
        private const int NONE = -1;

        private readonly ICondition[] mConditions;
        private readonly ITask[] mTasks;
        private int mCount = 0;

        private int mSelectedTask = NONE;

        private PrioritySelector(int size)
        {
            size = Math.Max(1, size);
            mConditions = new ICondition[size];
            mTasks = new ITask[size];
        }

        protected override void LocalUpdate(AgentContext context)
        {
            if (state == TaskState.Inactive)
            {
                mSelectedTask = NONE;
                for (int i = 0; i < mConditions.Length; i++)
                {
                    if (mConditions[i].EvaluateCondition(context))
                    {
                        mSelectedTask = i;
                        break;
                    }
                }

                if (mSelectedTask == NONE)
                {
                    // Could not find a task to run.
                    state = TaskState.Complete;
                    return;
                }

                state = TaskState.Active;
            }

            state = mTasks[mSelectedTask].Update(context);
        }

        protected override bool LocalExit(AgentContext context)
        {
            if (mSelectedTask != NONE && !mTasks[mSelectedTask].Exit(context))
                // Current task is still
                // exiting.
                return false;

            mSelectedTask = NONE;
            return true;
        }

        public static PrioritySelector Create(int size)
        {
            return new PrioritySelector(size);
        }

        public PrioritySelector Load(ICondition condition, ITask task)
        {
            if (condition == null || task == null || mCount == mConditions.Length)
                return null;

            mConditions[mCount] = condition;
            mTasks[mCount] = task;
            mCount++;

            return this;
        }
    }
}

