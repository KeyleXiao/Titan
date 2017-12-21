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
using org.critterai.nav.u3d.editor;
using org.critterai.u3d.editor;
using org.critterai.nav.u3d;

/// <summary>
/// <see cref="NavManagerProvider"/> editor.
/// </summary>
/// <exclude />
[CustomEditor(typeof(NavManagerProvider))]
public class NavManagerProviderEditor 
    : Editor 
{
    /// <summary>
    /// Controls behavior of the inspector.
    /// </summary>
    public override void OnInspectorGUI()
    {
        NavManagerProvider targ = (NavManagerProvider)target;

        INavmeshData navmeshData = targ.NavmeshData;

        string msg = "";

        if (navmeshData == null || !navmeshData.HasNavmesh)
            msg = "No navigation mesh.\n";

        if (!targ.AvoidanceSet)
            msg += "No avoidance set.\n";

        if (!targ.AgentGroups)
            msg += "No agent groups.\n";

        if (msg.Length > 0)
            GUILayout.Box(msg.Trim(), EditorUtil.ErrorStyle, GUILayout.ExpandWidth(true));

        EditorGUILayout.Separator();

        bool bv = EditorGUILayout.Toggle("Show Mesh", targ.DebugEnabled);
        if (bv != targ.DebugEnabled)
        {
            if (bv)
                // Turn off asset draw.
                org.critterai.nav.u3d.editor.NavmeshSceneDraw.Instance.Hide();

            targ.DebugEnabled = bv;
        }

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Navigation Settings");

        EditorGUILayout.Separator();

        targ.NavmeshData = NavEditorUtil.OnGUINavmeshDataField("Navmesh Data", targ.NavmeshData);

        targ.AgentGroups = (AgentGroupSettings)EditorGUILayout.ObjectField(
            "Agent Groups", targ.AgentGroups, typeof(AgentGroupSettings), false);

        EditorGUILayout.Separator();

        targ.MaxQueryNodes = EditorGUILayout.IntField("Max Query Nodes", targ.MaxQueryNodes);
        targ.MaxPathSize = EditorGUILayout.IntField("Max Path Size", targ.MaxPathSize);
        targ.MaxStraightPathSize = 
            EditorGUILayout.IntField("Max Straight Path", targ.MaxStraightPathSize);

        targ.MaxAgents = EditorGUILayout.IntField("Max Nav Agents", targ.MaxAgents);

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Crowd Settings");

        EditorGUILayout.Separator();

        targ.MaxCrowdAgents = EditorGUILayout.IntField("Max Crowd Agents", targ.MaxCrowdAgents);
        targ.MaxAgentRadius = EditorGUILayout.FloatField("Max Agent Radius", targ.MaxAgentRadius);

        EditorGUILayout.Separator();

        targ.AvoidanceSet = (CrowdAvoidanceSet)EditorGUILayout.ObjectField(
            "Avoidance Set", targ.AvoidanceSet, typeof(CrowdAvoidanceSet), false);

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Accuracy Settings");

        EditorGUILayout.Separator();

        targ.Extents = EditorGUILayout.Vector3Field("Extents", targ.Extents);
        targ.WideExtents = EditorGUILayout.Vector3Field("Wide Extents", targ.WideExtents);

        EditorGUILayout.Separator();

        targ.RadiusAt = EditorGUILayout.FloatField("Radius At", targ.RadiusAt);
        targ.RadiusNear = EditorGUILayout.FloatField("Radius Near", targ.RadiusNear);
        targ.HeightTolerance = EditorGUILayout.FloatField("Height Tolerance", targ.HeightTolerance);

        EditorGUILayout.Separator();

        targ.TurnThreshold = EditorGUILayout.FloatField("Turn Threshold", targ.TurnThreshold);
        targ.AngleAt = EditorGUILayout.FloatField("Angle At", targ.AngleAt);

        EditorGUILayout.Separator();

        EditorGUILayout.Separator();

        if (GUI.changed)
            EditorUtility.SetDirty(target);
    }

    [MenuItem(
        EditorUtil.NavGameObjectMenu + "Nav Manager Provider", false, EditorUtil.GameObjectGroup)]
    static void CreateGameObject()
    {
        GameObject go = new GameObject("Nav Provider");
        go.transform.position = new Vector3(1000, 1000, 1000);

        go.AddComponent<NavManagerProvider>();

        Selection.activeGameObject = go;
    }
}
