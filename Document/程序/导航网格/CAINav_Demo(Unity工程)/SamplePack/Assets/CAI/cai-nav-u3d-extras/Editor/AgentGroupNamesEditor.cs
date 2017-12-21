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
using UnityEditor;
using System.Collections.Generic;
using org.critterai.nav;
using org.critterai.u3d.editor;

/// <summary>
/// <see cref="AgentGroupNames"/> editor.
/// </summary>
/// <exclude />
[CustomEditor(typeof(AgentGroupNames))]
public sealed class AgentGroupNamesEditor
    : Editor
{
    /// <summary>
    /// Provides a standard GUI control for selecting an agent group name.
    /// </summary>
    public class NameGUIControl
    {
        private readonly List<string> mNamesAll = new List<string>(Navmesh.MaxArea + 1);
        private readonly string[] mNamesShort;
        private readonly string mLabel;

        internal NameGUIControl(string label, string[] names)
        {
            mLabel = label;

            for (int i = 0; i < names.Length; i++)
            {
                if (names[i] != AgentGroupNames.Undefined)
                    mNamesAll.Add(names[i]);
            }

            mNamesAll.Add(AgentGroupNames.Undefined);

            mNamesShort = mNamesAll.ToArray();

            mNamesAll.Clear();

            mNamesAll.AddRange(names);
        }

        /// <summary>
        /// Displays a selection control with the default label.
        /// </summary>
        /// <param name="currentGroup">The current group.</param>
        /// <returns>The selected group.</returns>
        public byte OnGUI(byte currentGroup)
        {
            return OnGUI(mLabel, currentGroup);
        }

        /// <summary>
        /// Displays a selection control with a custom label.
        /// </summary>
        /// <param name="currentGroup">The current group.</param>
        /// <param name="labelOverride">The custom label.</param>
        /// <returns>The selected group.</returns>
        public byte OnGUI(string labelOverride, byte currentGroup)
        {
            string name = mNamesAll[currentGroup];

            int undef = mNamesShort.Length - 1;

            int i = undef;
            for (; i >= 0; i--)
            {
                if (name == mNamesShort[i])
                    break;
            }

            if (i == undef)
                mNamesShort[undef] = currentGroup + " " + mNamesShort[undef];

            int ni = EditorGUILayout.Popup(labelOverride, i, mNamesShort);

            mNamesShort[undef] = AgentGroupNames.Undefined;

            if (i == ni || ni == undef)
                return currentGroup;

            return (byte)mNamesAll.IndexOf(mNamesShort[ni]);
        }
    }

    /// <summary>
    /// Creates a new GUI control.
    /// </summary>
    /// <param name="label"></param>
    /// <returns></returns>
    public static NameGUIControl CreateGUIControl(string label)
    {
        AgentGroupNames settings = EditorUtil.GetGlobalAsset<AgentGroupNames>();
        return new NameGUIControl(label, (string[])settings.groupNames.Clone());
    }

    /// <summary>
    /// Gets the global asset, creating one if it doesn't exist.
    /// </summary>
    /// <returns>The global asset.</returns>
    public static AgentGroupNames GetGlobalAsset()
    {
        return EditorUtil.GetGlobalAsset<AgentGroupNames>(); ;
    }

    /// <summary>
    /// Controls behavior of the inspector.
    /// </summary>
    public override void OnInspectorGUI()
    {
        EditorGUILayout.Separator();

        EditorGUILayout.LabelField(
            "This asset contains well known names for agent groups. It can't be edited directly."
            , EditorUtil.HelpStyle, GUILayout.ExpandWidth(true));

        EditorGUILayout.Separator();

        if (GUILayout.Button("Edit Agent Groups"))
            AgentGroupSettingsEditor.SelectAsset();

        EditorGUILayout.Separator();
    }
}
