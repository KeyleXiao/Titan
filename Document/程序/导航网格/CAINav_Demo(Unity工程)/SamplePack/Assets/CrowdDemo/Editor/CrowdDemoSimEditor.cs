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

[CustomEditor(typeof(CrowdDemoSim))]
public class CrowdDemoSimEditor
    : Editor
{
    private AgentGroupNamesEditor.NameGUIControl mNamesControl;

    public void OnEnable()
    {
        mNamesControl = AgentGroupNamesEditor.CreateGUIControl("Group");
    }


    public override void OnInspectorGUI()
    {
        CrowdDemoSim targ = (CrowdDemoSim)target;

        EditorGUIUtility.LookLikeControls(80);

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Agent A");

        targ.agentA = (GameObject)EditorGUILayout.ObjectField(
            "Model", targ.agentA, typeof(GameObject), true);

        targ.agentGroupA = mNamesControl.OnGUI(targ.agentGroupA);

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Agent B");


        targ.agentB = (GameObject)EditorGUILayout.ObjectField(
            "Model", targ.agentB, typeof(GameObject), true);

        targ.agentGroupB = mNamesControl.OnGUI(targ.agentGroupB);

        EditorGUILayout.Separator();

        EditorGUILayout.LabelField("Agent C");

        targ.agentC = (GameObject)EditorGUILayout.ObjectField(
            "Model", targ.agentC, typeof(GameObject), true);

        targ.agentGroupC = mNamesControl.OnGUI(targ.agentGroupC);

        EditorGUILayout.Separator();

        if (GUI.changed)
            EditorUtility.SetDirty(target);
    }
}
