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
using org.critterai.nav.u3d;
using org.critterai.agent.task;
using UnityEngine;

namespace org.critterai.agent
{
    /// <summary>
    /// Requests the agent move to straight to a goal.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The polygon reference of the goal is passed to the agent, even though it has no effect.  
    /// So if one is provided, it should be valid.
    /// </para>
    /// </remarks>
    public sealed class SetUseCrowd
        : BaseTask
    {
        public readonly bool useCrowd;

        public SetUseCrowd(bool useCrowd)
        {
            this.useCrowd = useCrowd;
        }

        protected override void LocalUpdate(AgentContext context)
        {
            context.navCon.SetUseCrowd(useCrowd);
            state = TaskState.Complete;
        }
    }
}

