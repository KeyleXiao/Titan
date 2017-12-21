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
using UnityEngine;
using org.critterai.nav.u3d;
using org.critterai;
using org.critterai.nav;
using System.Collections.Generic;

/// <summary>
/// Provides a <see cref="NavManager"/> object at run-time.
/// </summary>
/// <remarks>
/// <para>
/// In general, the scene's simulation manager will get the navigation manager and delete the
/// provider in order to free up memory.
/// </para>
/// </remarks>
[ExecuteInEditMode]
public class NavManagerProvider
    : MonoBehaviour 
{
    [SerializeField]
    private ScriptableObject mNavmeshData = null;

    [SerializeField]
    private AgentGroupSettings mGroupsSettings = null;

    [SerializeField]
    private CrowdAvoidanceSet mAvoidanceSet = null;

    [SerializeField]
    private int mMaxQueryNodes = 2048;

    [SerializeField]
    private int mMaxPath = 256;

    [SerializeField]
    private int mMaxStraightPath = 4;

    [SerializeField]
    private int mMaxAgents = 40;

    [SerializeField]
    private int mMaxCrowdAgents = 40;

    [SerializeField]
    private float mMaxAgentRadius = 0.5f;

    [SerializeField]
    private Vector3 mExtents = new Vector3(1, 1, 1);

    [SerializeField]
    private Vector3 mWideExtents = new Vector3(10, 10, 10);

    [SerializeField]
    private float mRadiusAt = 0.1f;

    [SerializeField]
    private float mRadiusNear = 1.0f;

    [SerializeField]
    private float mHeightTolerance = 1.0f;

    [SerializeField]
    private float mTurnThreshold = 0.05f;

    [SerializeField]
    private float mAngleAt = 1.0f;

    private bool mDebugEnabled;
    private Navmesh mDebugMesh;
    private int mDebugVersion;

    /// <summary>
    /// True if debug visualizations are enabled.
    /// </summary>
    public bool DebugEnabled
    {
        get { return mDebugEnabled; }
        set
        {
            mDebugEnabled = value;
            if (!mDebugEnabled)
                mDebugMesh = null;
        }
    }

    /// <summary>
    /// The maximum search nodes to use for the query object.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Does not apply to the <see cref="CrowdManager"/>.
    /// </para>
    /// </remarks>
    public int MaxQueryNodes
    {
        get { return mMaxQueryNodes; }
        set { mMaxQueryNodes = Mathf.Max(1, value); }
    }

    /// <summary>
    /// The maximum number agents the manager will support.
    /// </summary>
    public int MaxAgents
    {
        get { return mMaxAgents; }
        set { mMaxAgents = Mathf.Max(1, value); }
    }

    public int MaxCrowdAgents
    {
        get { return mMaxCrowdAgents; }
        set { mMaxCrowdAgents = Mathf.Max(1, value); }
    }

    /// <summary>
    /// The <see cref="CrowdManager"/>'s maximum agent radius.
    /// </summary>
    public float MaxAgentRadius
    {
        get { return mMaxAgentRadius; }
        set { mMaxAgentRadius = Mathf.Max(0, value); }
    }

    /// <summary>
    /// The value of the standard search extents.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This field is ignored if <see cref="enableCrowdManager"/> is true. In that case the 
    /// <see cref="CrowdManager"/> extents will be used.
    /// </para>
    /// </remarks>
    public Vector3 Extents
    {
        get { return mExtents; }
        set
        {
            mExtents = new Vector3(MathUtil.ClampToPositiveNonZero(value.x)
                , MathUtil.ClampToPositiveNonZero(value.y)
                , MathUtil.ClampToPositiveNonZero(value.z));
        }
    }

    public Vector3 WideExtents
    {
        get { return mWideExtents; }
        set
        {
            mWideExtents = new Vector3(MathUtil.ClampToPositiveNonZero(value.x)
                , MathUtil.ClampToPositiveNonZero(value.y)
                , MathUtil.ClampToPositiveNonZero(value.z));
        }
    }

    public float AngleAt
    {
        get { return mAngleAt; }
        set { mAngleAt = Mathf.Max(0, value); }
    }

    public float RadiusAt
    {
        get { return mRadiusAt; }
        set { mRadiusAt = Mathf.Max(MathUtil.Tolerance, value); }
    }

    public float RadiusNear
    {
        get { return mRadiusNear; }
        set { mRadiusNear = Mathf.Max(MathUtil.Tolerance, value); }
    }

    public float HeightTolerance
    {
        get { return mHeightTolerance; }
        set { mHeightTolerance = Mathf.Max(MathUtil.Tolerance, value); }
    }

    public float TurnThreshold
    {
        get { return mTurnThreshold; }
        set { mTurnThreshold = Mathf.Max(0, value); }
    }

    public int MaxPathSize
    {
        get { return mMaxPath; }
        set { mMaxPath = Mathf.Max(1, value); }
    }

    public int MaxStraightPathSize
    {
        get { return mMaxStraightPath; }
        set { mMaxStraightPath = Mathf.Max(2, value); }
    }

    public INavmeshData NavmeshData
    {
        get { return (mNavmeshData ? (INavmeshData)mNavmeshData : null); }
        set
        {
            if (value is ScriptableObject)
                mNavmeshData = (ScriptableObject)value;
        }
    }

    public AgentGroupSettings AgentGroups
    {
        get { return mGroupsSettings; }
        set { mGroupsSettings = value; }
    }

    public CrowdAvoidanceSet AvoidanceSet
    {
        get { return mAvoidanceSet; }
        set { mAvoidanceSet = value; }
    }

    public NavManager CreateManager()
    {
        if (!(mNavmeshData && NavmeshData.HasNavmesh))
        {
            Debug.LogError(name + ": Aborted initialization. Navigation mesh not available.");
            return null;
        }

        if (!mAvoidanceSet || !mGroupsSettings)
        {
            Debug.LogError(
                name + ": Aborted initialization. Avoidance and/or agent groups not available.");
            return null;
        }

        Navmesh navmesh = NavmeshData.GetNavmesh();
        NavmeshQuery query;
        NavStatus status = NavmeshQuery.Create(navmesh, mMaxQueryNodes, out query);
        if ((status & NavStatus.Sucess) == 0)
        {
            Debug.LogError(
                name + ": Aborted initialization. Failed query creation: " + status.ToString());
            return null;
        }

        CrowdManager crowd = CrowdManager.Create(mMaxCrowdAgents, mMaxAgentRadius, navmesh);
        if (crowd == null)
        {
            Debug.LogError(name + ": Aborted initialization. Failed crowd creation.");
            return null;
        }

        for (int i = 0; i < CrowdManager.MaxAvoidanceParams; i++)
        {
            crowd.SetAvoidanceConfig(i, mAvoidanceSet[i]);
        }

        NavGroup mGroup = new NavGroup(navmesh, query, crowd, crowd.QueryFilter, mExtents, false);
        
        int count = mGroupsSettings.GroupCount;
        Dictionary<byte,NavAgentGroup> mAgentGroups = new Dictionary<byte,NavAgentGroup>(count);

        for (int i = 0; i < count; i++)
        {
            byte groupId;
            NavAgentGroup group = 
                mGroupsSettings.CreateAgentGroup(i, mMaxPath, mMaxStraightPath, out groupId);

            group.angleAt = mAngleAt;
            group.heightTolerance = mHeightTolerance;
            group.radiusAt = mRadiusAt;
            group.radiusNear = mRadiusNear;
            group.turnThreshold = mTurnThreshold;

            mAgentGroups.Add(groupId, group);
        }

        return NavManager.Create(mMaxAgents, mGroup, mAgentGroups);
    }

    void OnRenderObject()
    {
        if (!mDebugEnabled)
            return;

        INavmeshData data = NavmeshData;

        if (!mNavmeshData || !data.HasNavmesh)
        {
            mDebugMesh = null;
            return;
        }

        if (mDebugMesh == null || data.Version != mDebugVersion)
        {
            mDebugMesh = data.GetNavmesh();
            mDebugVersion = data.Version;

            if (mDebugMesh == null)
                return;
        }

        NavDebug.Draw(mDebugMesh, false);
    }
}
