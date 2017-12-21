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
using org.critterai.u3d;
using org.critterai.nav.u3d;
using org.critterai.nav;

/// <summary>
/// Provides a simple debug view of a <see cref="NavAgent"/> object.
/// </summary>
/// <remarks><para>The navigation agent must be set manually when the
/// component is created.</para>
/// </remarks>
public sealed class NavDebugView
    : MonoBehaviour
{
    private NavAgent mAgent = null;
    private CrowdAgentDebug mCrowdDebug = null;
    private CornerData mCorners = null;
    private PathCorridorData mCorridor = null;
    private bool mDebugEnabled = false;

    public bool DebugEnabled
    {
        get { return mDebugEnabled; }
        set
        {
            mDebugEnabled = value;
            if (!mDebugEnabled)
            {
                mCrowdDebug = null;
                mCorners = null;
                mCorridor = null;
            }
        }
    }

    public void Initialize(NavAgent agent)
    {
        this.mAgent = agent;
        mCrowdDebug = null;
        mCorners = null;
        mCorridor = null;
    }

    void OnRenderObject()
    {
        if (!mDebugEnabled || mAgent == null)
            return;

        if (mAgent.crowdAgent != null)
        {
            if (mCrowdDebug == null)
            {
                mCorners = null;
                mCorridor = null;
                mCrowdDebug = new CrowdAgentDebug(mAgent.navGroup.crowd.Navmesh);
            }
            mCrowdDebug.DrawAll(mAgent.crowdAgent);
        }
        else if ((mAgent.data.flags & NavFlag.CorridorInUse) != 0)
        {
            if (mCorridor == null)
            {
                mCorners = new CornerData();
                mCorridor = new PathCorridorData(mAgent.corridor.MaxPathSize);
                mCrowdDebug = null;
            }
            int n = mAgent.corridor.FindCorners(mCorners);
            mCorners.cornerCount = n; // v0.3.0 bug workaround.
            mAgent.corridor.GetData(mCorridor);
            NavDebug.Draw(mAgent.navGroup.mesh, mCorridor);
            NavDebug.Draw(mCorners);
        }

        if (mAgent.crowdAgent == null)
        {
            Vector3 pos = mAgent.data.position.point;
            float height = mAgent.crowdConfig.height;  // Might be zero.

            Vector3 velocity = mAgent.data.desiredVelocity;
            DebugDraw.Arrow(pos + Vector3.up * height
                , pos + velocity + Vector3.up * height
                , 0, 0.05f, CrowdAgentDebug.desiredVelocityColor);
        }

        DebugDraw.DiamondMarker(mAgent.data.position.point
            , 0.1f
            , NavDebug.positionColor);

        DebugDraw.DiamondMarker(mAgent.data.goal.point
            , 0.1f
            , NavDebug.goalColor);
    }
}
