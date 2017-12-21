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
using org.critterai.nav;

/// <summary>
/// A set of shared <see cref="CrowdAvoidanceParams"/> configuruations for use with a 
/// <see cref="CrowdManager"/> object.
/// </summary>
[System.Serializable]
public sealed class CrowdAvoidanceSet 
    : ScriptableObject 
{
    [SerializeField]
    private CrowdAvoidanceParams[] mConfigs;

    void OnEnable()
    {
        if (mConfigs == null || mConfigs.Length != CrowdManager.MaxAvoidanceParams)
            Reset();
    }

    /// <summary>
    /// A reference to the configuration at the specified index.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Attempting to set a configuration to null will have no effect.
    /// </para>
    /// <para>
    /// References are stored.  No cloning occurs.
    /// </para>
    /// </remarks>
    /// <param name="index">The index of the configuration.</param>
    public CrowdAvoidanceParams this[int index]
    {
        get { return mConfigs[index]; }
        set
        {
            if (value != null)
                mConfigs[index] = value;
        }
    }

    /// <summary>
    /// Restores the object to its default state, releasing all references to existing 
    /// configurations.
    /// </summary>
    public void Reset()
    {
        mConfigs = new CrowdAvoidanceParams[CrowdManager.MaxAvoidanceParams];

        mConfigs[0] = CrowdAvoidanceParams.CreateStandardLow();
        mConfigs[1] = CrowdAvoidanceParams.CreateStandardMedium();
        mConfigs[2] = CrowdAvoidanceParams.CreateStandardGood();
        mConfigs[3] = CrowdAvoidanceParams.CreateStandardHigh();

        for (int i = 4; i < mConfigs.Length; i++)
        {
            mConfigs[i] = new CrowdAvoidanceParams();
        }
    }
}
