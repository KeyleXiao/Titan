/*
 * Copyright (c) 2011-2012 Stephen A. Pratt
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
using org.critterai;
using org.critterai.nav.u3d;
using org.critterai.agent.task;
using org.critterai.agent;
using org.critterai.nav;

[System.Serializable]
public abstract class Agent
    : MonoBehaviour
{
    public byte agentGroup = 0;

    protected AgentContext context;
    private ITask mBrain;

    private NavAgent mNav;  // Local reference kept for debug purposes.

    private NavManager mManager;
    private int mManagerIndex = -1;

    private bool mDebugEnabled = false;

    protected abstract ITask GetBrain();

    public bool DebugEnabled
    {
        get { return mDebugEnabled; }
        set
        {
            // Only allow debug if navigation is available.
            mDebugEnabled = (mNav == null ? false : value);

            if (mDebugEnabled)
            {
                if (context == null)
                    InitializeOnce();

                NavDebugView ndv =  GetComponent<NavDebugView>();

                if (ndv == null)
                    ndv = gameObject.AddComponent<NavDebugView>();

                ndv.Initialize(mNav);
                ndv.DebugEnabled = true;
            }
            else
                Destroy(GetComponent<NavDebugView>());
        }
    }

    public bool UserControlled
    {
        get { return context.userControlled; }
        set { context.userControlled = value; }
    }

    public NavmeshPoint NavPosition
    {
        get { return context.navCon.Position; }
    }

    public NavmeshPoint NavGoal
    {
        get { return context.navCon.Goal; }
    }

    void Start()
    {
        if (context == null || context.navCon == null)
            InitializeOnce();
    }

    private void InitializeOnce()
    {
        // The agent will be disabled on any error.

        if (!enabled || NavManager.ActiveManager == null)
        {
            Debug.LogError(name + ": Failed navigation initialization. Missing resources or bad"
                + " configuration. Agent forced to disabled state.");

            enabled = false;
            return;
        }

        mManager = NavManager.ActiveManager;

        mNav = mManager.CreateAgent(agentGroup, transform);

        if (mNav == null)
        {
            Debug.LogError(name + ": Failed navigation initialization. Could not get navigation"
                + " agent from manager. Agent forced to disabled state.");

            enabled = false;
            return;
        }

        mNav.data.rotation = transform.rotation;
        mNav.data.position = mNav.GetPointSearch(transform.position);

        if (mNav.data.position.polyRef == 0)
        {
            mNav = null;
            Debug.LogError(name + ": Failed to constrain position to the navigation mesh."
                + " Agent forced to disabled state.");

            enabled = false;
            return;
        }

        IState planner = new NavPlanner(mNav);
        IState mover = new SimpleMover(mNav);

        mManagerIndex = mManager.AddAgent(planner, mover);
        if (mManagerIndex == -1)
        {
            Debug.LogError(name + 
                ": Could not add agent to navigation manager. Agent forced to disabled state.");

            enabled = false;
            return;
        }

        context = new AgentContext(this.gameObject, new NavController(mNav));

        mBrain = GetBrain();

        if (mBrain == null)
        {
            mManager.RemoveAgent(mManagerIndex);

            Debug.LogError(
                name + ": Could not get the agent brain. Agent forced to disabled state.");

            enabled = false;
            return;
        }
    }

    void Update()
    {
        context.navCon.Update();
        PostUpdate();
    }

    /// <summary>
    /// Called at the end of the normal update.
    /// </summary>
    protected virtual void PostUpdate() { }

    void FixedUpdate()
    {
        bool failed;
        TaskUtil.Process(context, mBrain, out failed);

        if (failed)
            Debug.LogError(name + ": Agent brain failed. Will attempt a re-start.");
        else
            PostFixedUpdate();
    }

    /// <summary>
    /// Called at the end of the fixed update. (Unless there is a brain error.)
    /// </summary>
    protected virtual void PostFixedUpdate() { }

    void OnDestroy()
    {
        if (mManager != null && mManagerIndex >= 0)
            mManager.RemoveAgent(mManagerIndex);
        mManager = null;
    }
}
