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
/// Used in conjunction with <see cref="AgentGroupSettings"/> to maps byte values to well known 
/// names for the purpose of defining agent groups. (Editor Only.)
/// </summary>
/// <remarks>
/// <para>
/// Meant for use as a global editor asset. (One per project, stored at a standard path.)
/// </para>
/// </remarks>
[System.Serializable]
public sealed class AgentGroupNames
    : ScriptableObject
{
    public const int MaxGroups = 256;

    /// <summary>
    /// The agent group name for groups with no defined well known name.
    /// </summary>
    public const string Undefined = "<Undefined>";

    [SerializeField]
    internal string[] groupNames;

    /// <summary>
    /// Gets a clone of the group names.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is the full array, including the <see cref="Undefined"/> values.
    /// </para>
    /// </remarks>
    /// <returns>A clone of the group names. [Length: <see cref="Navmesh.MasArea"/> + 1]</returns>
    public string[] GetGroupNames()
    {
        return (string[])groupNames.Clone();
    }
}
