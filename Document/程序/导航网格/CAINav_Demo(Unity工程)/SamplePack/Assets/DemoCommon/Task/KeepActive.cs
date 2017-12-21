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
    /// Runs the root task until is fails.
    /// </summary>
    /// <remarks>
    /// <para>If the root task completes, it will be exited and restarted.</para>
    /// <para>
    /// Useful in long running parallel nodes.
    /// </para>
    /// </remarks>
    public sealed class KeepActive
        : BaseTask
    {
        private ITask rootTask;


        public KeepActive(ITask task)
        {
            rootTask = task;
        }

        protected override void LocalUpdate(AgentContext context)
        {
            bool failed;
            TaskUtil.Process(context, rootTask, out failed);
            state = (failed ? TaskState.Failed : TaskState.Active);
        }

        protected override bool LocalExit(AgentContext context)
        {
            return rootTask.Exit(context);
        }
    }
}
