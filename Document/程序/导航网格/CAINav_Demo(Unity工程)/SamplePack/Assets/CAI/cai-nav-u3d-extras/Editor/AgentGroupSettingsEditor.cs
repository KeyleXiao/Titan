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
using org.critterai.u3d.editor;
using org.critterai.nav;

/// <summary>
/// <see cref="AgentGroupSettings"/> editor.
/// </summary>
/// <exclude />
[CustomEditor(typeof(AgentGroupSettings))]
public class AgentGroupSettingsEditor
    : Editor
{
    private const string AddGroupName = "AddGroup";

    private static bool[] mShowGroup;

    private string[] mAvoidanceNames;
    private AgentGroupNames mNamesAsset;
    private bool mFocusNew = false;
    private byte mNewArea = 0;

    void OnEnable()
    {
        AgentGroupSettings targ = (AgentGroupSettings)target;

        mNamesAsset = AgentGroupNamesEditor.GetGlobalAsset();

        string[] names = mNamesAsset.groupNames;

        // Make sure the names asset is initialized.
        if (names == null || names.Length != AgentGroupNames.MaxGroups)
        {
            names = new string[AgentGroupNames.MaxGroups];

            for (int i = 1; i < names.Length; i++)
            {
                names[i] = AgentGroupNames.Undefined;
            }

            names[0] = "Default";

            mNamesAsset.groupNames = names;

            EditorUtility.SetDirty(mNamesAsset);
        }

        // Make sure settings are defined for names.
        for (int i = 0; i < names.Length; i++)
        {
            if (names[i] == AgentGroupNames.Undefined || targ.GetGroupIndex((byte)i) >= 0)
                continue;

            targ.AddGroup((byte)i);

            EditorUtility.SetDirty(target);
        }

        // Initialize the show flags.

        if (mShowGroup == null || mShowGroup.Length != names.Length)
            mShowGroup = new bool[names.Length];

        mAvoidanceNames = CAINavEditorSettingsEditor.GetAvoidanceNames();

        mNewArea = NextUndefined();
    }

    private byte NextUndefined()
    {
        string[] names = mNamesAsset.groupNames;

        for (int i = 0; i < names.Length; i++)
        {
            if (names[i] == AgentGroupNames.Undefined)
                return (byte)i;
        }

        return 0;
    }

    /// <summary>
    /// Controls behavior of the inspector.
    /// </summary>
    public override void OnInspectorGUI()
    {
        AgentGroupSettings targ = (AgentGroupSettings)target;

        EditorGUILayout.Separator();

        int delTarget = -1;

        if (targ.GroupCount == 0)
            // A reset, or something else odd happened.
            OnEnable();

        string[] names = mNamesAsset.groupNames;

        bool blockDelete = (targ.GroupCount < 2);

        // Note: Because we are looping on the names, we are guarenteed to cover
        // all the definitions in the target.  names is always the most complete list.
        for (int iNames = 0; iNames < names.Length; iNames++)
        {
            int iTarg = targ.GetGroupIndex((byte)iNames);

            if (names[iNames] == AgentGroupNames.Undefined)
            {
                if (iTarg < 0)
                    // It is truly undefined.
                    continue;

                // Fix the error.  All defined groups must have a name.
                names[iNames] = "Agent Group " + iNames;
            }

            mShowGroup[iNames] = EditorGUILayout.Foldout(
                mShowGroup[iNames], string.Format("{0} ({1})", names[iNames], iNames));

            if (!mShowGroup[iNames])
                continue;

            EditorGUILayout.BeginHorizontal();

            string sn = EditorGUILayout.TextField("Name", names[iNames]);

            if (sn != names[iNames]
                && sn != AgentGroupNames.Undefined)
            {
                bool foundDup = false;
                foreach (string n in names)
                {
                    if (sn == n)
                    {
                        foundDup = true;
                        break;
                    }
                }

                if (!foundDup)
                {
                    names[iNames] = sn;
                    EditorUtility.SetDirty(mNamesAsset);
                }
            }

            GUI.enabled = (!blockDelete);

            if (GUILayout.Button("X", GUILayout.Width(20)))
            {
                delTarget = iTarg;

                names[iNames] = AgentGroupNames.Undefined;
                EditorUtility.SetDirty(mNamesAsset);

                mShowGroup[iNames] = false;

                GUI.changed = true;
            }

            GUI.enabled = true;

            EditorGUILayout.EndHorizontal();

            float fv;

            fv = EditorGUILayout.FloatField("Agent Radius", targ.GetAgentRadius(iTarg));
            targ.SetAgentRadius(iTarg, fv);

            fv = EditorGUILayout.FloatField("Agent Height", targ.GetAgentHeight(iTarg));
            targ.SetAgentHeight(iTarg, fv);

            fv = EditorGUILayout.FloatField("Walk Speed", targ.GetMaxWalkSpeed(iTarg));
            targ.SetMaxWalkSpeed(iTarg, fv);

            fv = EditorGUILayout.FloatField("Jog Speed", targ.GetMaxJogSpeed(iTarg));
            targ.SetMaxJogSpeed(iTarg, fv);

            fv = EditorGUILayout.FloatField("Run Speed", targ.GetMaxRunSpeed(iTarg));
            targ.SetMaxRunSpeed(iTarg, fv);

            fv = EditorGUILayout.FloatField("Max Acceleration", targ.GetMaxAcceleration(iTarg));
            targ.SetMaxAcceleration(iTarg, fv);

            fv = EditorGUILayout.FloatField("Max Turn Speed", targ.GetMaxTurnSpeed(iTarg));
            targ.SetMaxTurnSpeed(iTarg, fv);

            fv = EditorGUILayout.FloatField("Query Range", targ.GetQueryRange(iTarg));
            targ.SetQueryRange(iTarg, fv);

            fv = EditorGUILayout.FloatField("Optimization Range", targ.GetOptimizationRange(iTarg));
            targ.SetOptimizationRange(iTarg, fv);

            fv = EditorGUILayout.FloatField("Separation Weight", targ.GetSeparationWeight(iTarg));
            targ.SetSeparationWeight(iTarg, fv);

            int iv;

            iv = EditorGUILayout.Popup(
                "Avoidance Quality", targ.GetAvoidanceType(iTarg), mAvoidanceNames);
            targ.SetAvoidanceType(iTarg, iv);

            CrowdUpdateFlags flags = (CrowdUpdateFlags)
                EditorGUILayout.EnumMaskField("Update Flags", targ.GetCrowdUpdateFlags(iTarg));
            targ.SetCrowdUpdateFlags(iTarg, flags);

            iv = EditorGUILayout.IntField("Corridor Pool Size", targ.GetCorridorPoolSize(iTarg));
            targ.SetCorridorPoolSize(iTarg, iv);

            iv = EditorGUILayout.IntField("Path Pool Size", targ.GetPathPoolSize(iTarg));
            targ.SetPathPoolSize(iTarg, iv);
        }

        if (delTarget >= 0)
        {
            // Note: The name is already taken care of.
            targ.RemoveGroup(delTarget);

            mNewArea = NextUndefined();
            mFocusNew = true;  // So don't have odd GUI behavior.
        }

        EditorGUILayout.Separator();

        EditorGUILayout.BeginHorizontal();

        GUI.SetNextControlName(AddGroupName);
        mNewArea = (byte)Mathf.Min(
            AgentGroupNames.MaxGroups - 1, EditorGUILayout.IntField(mNewArea, GUILayout.Width(80)));
        if (mFocusNew)
        {
            GUI.FocusControl(AddGroupName);
            mFocusNew = false;
        }

        GUI.enabled = (names[mNewArea] == AgentGroupNames.Undefined);

        if (GUILayout.Button("Add New Group"))
        {
            names[mNewArea] = "Agent Group " + mNewArea;
            EditorUtility.SetDirty(mNamesAsset);

            targ.AddGroup(mNewArea);
            mShowGroup[mNewArea] = true;

            mNewArea = NextUndefined();
            mFocusNew = true;
            GUI.changed = true;
        }

        GUI.enabled = true;

        EditorGUILayout.EndHorizontal();

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Maximum allowed group id: " + (AgentGroupNames.MaxGroups - 1)
            , EditorUtil.HelpStyle, GUILayout.ExpandWidth(true));

        EditorGUILayout.Separator();

        if (GUI.changed)
            EditorUtility.SetDirty(target);
    }

    [MenuItem(EditorUtil.MainMenu + "Agent Group Settings", false, EditorUtil.GlobalGroup)]
    internal static void SelectAsset()
    {
        AgentGroupSettings item = EditorUtil.GetGlobalAsset<AgentGroupSettings>();
        Selection.activeObject = item;
    }
}
