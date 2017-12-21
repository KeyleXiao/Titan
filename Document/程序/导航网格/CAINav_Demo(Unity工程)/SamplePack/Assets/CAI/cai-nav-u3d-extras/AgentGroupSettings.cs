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
using org.critterai.nav;
using System.Collections.Generic;

/// <summary>
/// Provides standard navigation configuration settings for agents.
/// </summary>
/// <remarks>
/// <para>
/// Meant for use as a global editor asset. (One per project, stored at a standard path.)
/// </para>
/// </remarks>
[System.Serializable]
public class AgentGroupSettings
    : ScriptableObject 
{
    /*
     * Design notes:
     * 
     * The API is optimized for the Unity Editor.
     * 
     */

    /// <summary>
    /// The maximum number of groups.
    /// </summary>
    public const int MaxGroups = 256;

    private const CrowdUpdateFlags DefaultUpdateFlags = CrowdUpdateFlags.AnticipateTurns
        | CrowdUpdateFlags.CrowdSeparation | CrowdUpdateFlags.ObstacleAvoidance
        | CrowdUpdateFlags.OptimizeVis | CrowdUpdateFlags.OptimizeTopo;

    private const byte DefaultAvoidance = 2;

    private const float DefaultTurn = 8;
    private const float DefaultRun = 3.5f;
    private const float DefaultJog = 2.8f;
    private const float DefaultWalk = 1.5f;
    private const float DefaultRadius = 0.4f;
    private const float DefaultHeight = 1.8f;
    private const float DefaultAccel = 8;
    private const float DefaultQuery = 0.4f * 8;
    private const float DefaultOpti = 0.4f * 30;
    private const float DefaultSep = 2.0f;

    private const int DefaultPool = 10;

    private const int GroupStide = 10;
    private const int WalkOffset = 0;
    private const int JogOffset = 1;
    private const int RunOffset = 2;
    private const int TurnOffset = 3;
    private const int RadiusOffset = 4;
    private const int HeightOffset = 5;
    private const int AccelOffset = 6;
    private const int QueryOffset = 7;
    private const int OptiOffset = 8;
    private const int SepOffset = 9;

    [SerializeField]
    private List<byte> mGroups;

    [SerializeField]
    private List<CrowdUpdateFlags> mUpdateFlags;

    [SerializeField]
    private List<float> mGroupData;

    [SerializeField]
    private List<int> mPoolData;

    void OnEnable()
    {
        if (mGroups == null || mGroups.Count == 0)
        {
            mGroups = new List<byte>();
            mUpdateFlags = new List<CrowdUpdateFlags>();
            mGroupData = new List<float>();
            mPoolData = new List<int>();
        }
    }

    /// <summary>
    /// The number of groups defined.
    /// </summary>
    public int GroupCount { get { return mGroups.Count / 2; } }

    /// <summary>
    /// Gets a group's id.  (The well known value representing the group.)
    /// </summary>
    /// <param name="index">
    /// The storage index. [Limits: 0 &tl;= value &lt; <see cref="GroupCount"/>]
    /// </param>
    /// <returns>The group. [Limits: 0 &tl;= value &lt; <see cref="MaxGroups"/>]</returns>
    public byte GetGroup(int index)
    {
        return mGroups[index * 2 + 0];
    }

    /// <summary>
    /// The storage index associated with the group id.
    /// </summary>
    /// <param name="group">
    /// The group id. [Limits: 0 &tl;= value &lt; <see cref="MaxGroups"/>]
    /// </param>
    /// <returns>The storge index of the group.</returns>
    public int GetGroupIndex(byte group)
    {
        for (int p = 0; p < mGroups.Count; p += 2)
        {
            if (mGroups[p] == group)
                return p / 2;
        }
        return -1;
    }

    /// <summary>
    /// Sets the id of an existing group.
    /// </summary>
    /// <param name="index"></param>
    /// <param name="value">The new group id.</param>
    public void SetGroup(int index, byte value)
    {
        if (mGroups[index * 2 + 0] == value || GetGroupIndex(value) >= 0)
            return;

        mGroups[index * 2 + 0] = value;
    }

    public byte GetAvoidanceType(int index)
    {
        return mGroups[index * 2 + 1];
    }

    public void SetAvoidanceType(int index, int value)
    {
        mGroups[index * 2 + 1] = (byte)Mathf.Clamp(value, 0, CrowdManager.MaxAvoidanceParams - 1);
    }

    public float GetMaxTurnSpeed(int index)
    {
        return mGroupData[index * GroupStide + TurnOffset];
    }

    public void SetMaxTurnSpeed(int index, float value)
    {
        mGroupData[index * GroupStide + TurnOffset] = Mathf.Max(0, value);
    }

    public float GetMaxWalkSpeed(int index)
    {
        return mGroupData[index * GroupStide + WalkOffset];
    }

    public void SetMaxWalkSpeed(int index, float value)
    {
        mGroupData[index * GroupStide + WalkOffset] = Mathf.Max(0, value);
    }

    public float GetMaxJogSpeed(int index)
    {
        return mGroupData[index * GroupStide + JogOffset];
    }

    public void SetMaxJogSpeed(int index, float value)
    {
        mGroupData[index * GroupStide + JogOffset] = Mathf.Max(0, value);
    }

    public float GetMaxRunSpeed(int index)
    {
        return mGroupData[index * GroupStide + RunOffset];
    }

    public void SetMaxRunSpeed(int index, float value)
    {
        mGroupData[index * GroupStide + RunOffset] = Mathf.Max(0, value);
    }

    public float GetAgentRadius(int index)
    {
        return mGroupData[index * GroupStide + RadiusOffset];
    }

    public void SetAgentRadius(int index, float value)
    {
        mGroupData[index * GroupStide + RadiusOffset] = Mathf.Max(0, value);
    }

    public float GetAgentHeight(int index)
    {
        return mGroupData[index * GroupStide + HeightOffset];
    }

    public void SetAgentHeight(int index, float value)
    {
        mGroupData[index * GroupStide + HeightOffset] = Mathf.Max(0, value);
    }

    public float GetMaxAcceleration(int index)
    {
        return mGroupData[index * GroupStide + AccelOffset];
    }

    public void SetMaxAcceleration(int index, float value)
    {
        mGroupData[index * GroupStide + AccelOffset] = Mathf.Max(0, value);
    }

    public float GetQueryRange(int index)
    {
        return mGroupData[index * GroupStide + QueryOffset];
    }

    public void SetQueryRange(int index, float value)
    {
        mGroupData[index * GroupStide + QueryOffset] = Mathf.Max(0, value);
    }

    public float GetOptimizationRange(int index)
    {
        return mGroupData[index * GroupStide + OptiOffset];
    }

    public void SetOptimizationRange(int index, float value)
    {
        mGroupData[index * GroupStide + OptiOffset] = Mathf.Max(0, value);
    }

    public float GetSeparationWeight(int index)
    {
        return mGroupData[index * GroupStide + SepOffset];
    }

    public void SetSeparationWeight(int index, float value)
    {
        mGroupData[index * GroupStide + SepOffset] = Mathf.Max(0, value);
    }

    public int GetCorridorPoolSize(int index)
    {
        return mPoolData[index * 2 + 0];
    }

    public void SetCorridorPoolSize(int index, int value)
    {
        mPoolData[index * 2 + 0] = Mathf.Max(0, value);
    }

    public int GetPathPoolSize(int index)
    {
        return mPoolData[index * 2 + 1];
    }

    public void SetPathPoolSize(int index, int value)
    {
        mPoolData[index * 2 + 1] = Mathf.Max(0, value);
    }

    public CrowdUpdateFlags GetCrowdUpdateFlags(int index)
    {
        return mUpdateFlags[index];
    }

    public void SetCrowdUpdateFlags(int index, CrowdUpdateFlags flags)
    {
        mUpdateFlags[index] = flags;
    }

    public bool AddGroup(byte group)
    {
        if (GetGroupIndex(group) >= 0)
            return false;

        mGroups.Add(group);
        mGroups.Add(DefaultAvoidance);
        mUpdateFlags.Add(DefaultUpdateFlags);

        mGroupData.Add(DefaultWalk);
        mGroupData.Add(DefaultJog);
        mGroupData.Add(DefaultRun);
        mGroupData.Add(DefaultTurn);
        mGroupData.Add(DefaultRadius);
        mGroupData.Add(DefaultHeight);
        mGroupData.Add(DefaultAccel);
        mGroupData.Add(DefaultQuery);
        mGroupData.Add(DefaultOpti);
        mGroupData.Add(DefaultSep);

        mPoolData.Add(DefaultPool);
        mPoolData.Add(DefaultPool);

        return true;
    }

    public void RemoveGroup(int index)
    {
        mGroups.RemoveRange(index * 2, 2);
        mUpdateFlags.RemoveAt(index);
        mGroupData.RemoveRange(index * GroupStide, GroupStide);
        mPoolData.RemoveRange(index * 2, 2);
    }

    public NavAgentGroup CreateAgentGroup(int index, int maxPath, int maxStraightPath
        , out byte group)
    {
        group = mGroups[index * 2 + 0];

        CrowdAgentParams cap = new CrowdAgentParams();

        cap.avoidanceType = mGroups[index * 2 + 1];
        cap.collisionQueryRange = mGroupData[index * GroupStide + QueryOffset];
        cap.height = mGroupData[index * GroupStide + HeightOffset];
        cap.maxAcceleration = mGroupData[index * GroupStide + AccelOffset];
        cap.maxSpeed = mGroupData[index * GroupStide + JogOffset];
        cap.pathOptimizationRange = mGroupData[index * GroupStide + OptiOffset];
        cap.radius = mGroupData[index * GroupStide + RadiusOffset];
        cap.separationWeight = mGroupData[index * GroupStide + SepOffset];
        cap.updateFlags = mUpdateFlags[index];

        NavAgentGroup result = new NavAgentGroup(
            cap, maxPath, maxStraightPath, mPoolData[index * 2 + 0], mPoolData[index * 2 + 1]);

        result.jogSpeed = mGroupData[index * GroupStide + JogOffset];
        result.maxTurnSpeed = mGroupData[index * GroupStide + TurnOffset];
        result.runSpeed = mGroupData[index * GroupStide + RunOffset];
        result.walkSpeed = mGroupData[index * GroupStide + WalkOffset];

        return result;
    }
}
