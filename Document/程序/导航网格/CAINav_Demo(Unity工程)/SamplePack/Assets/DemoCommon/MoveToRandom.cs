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
using org.critterai.nav;

namespace org.critterai.agent
{
    /// <summary>
    /// Sets a navigation agent's goal to a random navigation mesh point.
    /// </summary>
    public sealed class MoveToRandom
        : BaseTask
    {
        protected override void LocalUpdate(AgentContext context)
        {
            NavmeshPoint pt;
            context.navCon.Query.GetRandomPoint(context.navCon.Filter, out pt);

            if (pt.polyRef == 0)
                state = TaskState.Failed;
            else
            {
                context.navCon.MoveTo(pt, RangeType.At, true);
                state = TaskState.Complete;
            }
        }
    }
}
