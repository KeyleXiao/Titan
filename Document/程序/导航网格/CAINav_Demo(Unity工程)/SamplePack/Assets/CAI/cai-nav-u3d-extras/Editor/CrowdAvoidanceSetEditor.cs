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
using org.critterai.nav;
using org.critterai.u3d.editor;

/// <summary>
/// <see cref="CrowdAvoidanceSet"/> editor.
/// </summary>
/// <exclude />
[CustomEditor(typeof(CrowdAvoidanceSet))]
public class CrowdAvoidanceSetEditor
    : Editor
{

    private string[] mNames;
    private static bool[] mDisplay;

    void OnEnable()
    {
        mNames = CAINavEditorSettingsEditor.GetAvoidanceNames();

        if (mDisplay == null || mDisplay.Length != mNames.Length)
            mDisplay = new bool[mNames.Length];
    }

    /// <summary>
    /// Controls behavior of the inspector.
    /// </summary>
    public override void OnInspectorGUI()
    {
        CrowdAvoidanceSet targ = (CrowdAvoidanceSet)target;

        EditorGUIUtility.LookLikeControls(150);

        for (int i = 0; i < CrowdManager.MaxAvoidanceParams; i++)
        {
            //bool changed = GUI.changed;
            mDisplay[i] = EditorGUILayout.Foldout(mDisplay[i], mNames[i]);
            //GUI.changed = changed;

            if (mDisplay[i])
            {
                CrowdAvoidanceParams config = targ[i];

                float fv;

                fv = EditorGUILayout.FloatField("Velocity Bias", config.velocityBias);
                config.velocityBias = Mathf.Max(0, fv);

                fv = EditorGUILayout.FloatField(
                    "Desired Velocity Weight", config.weightDesiredVelocity);
                config.weightDesiredVelocity = Mathf.Max(0, fv);

                fv = EditorGUILayout.FloatField(
                    "Current Velocity Weight", config.weightCurrentVelocity);
                config.weightCurrentVelocity = Mathf.Max(0, fv);

                fv = EditorGUILayout.FloatField("Side Weight", config.weightSide);
                config.weightSide = Mathf.Max(0, fv);

                fv = EditorGUILayout.FloatField("TOI Weight", config.weightToi);
                config.weightToi = Mathf.Max(0, fv);

                fv = EditorGUILayout.FloatField("Horizontal Time", config.horizontalTime);
                config.horizontalTime = Mathf.Max(0, fv);

                int bv;

                bv = EditorGUILayout.IntField("Grid Size", config.gridSize);
                config.gridSize = (byte)Mathf.Clamp(bv, 0, byte.MaxValue);

                bv = EditorGUILayout.IntField("Adaptive Divisions", config.adaptiveDivisions);
                config.adaptiveDivisions = (byte)Mathf.Clamp(bv, 0, byte.MaxValue);

                bv = (byte)EditorGUILayout.IntField("Adaptive Rings", config.adaptiveRings);
                config.adaptiveRings = (byte)Mathf.Clamp(bv, 0, byte.MaxValue);

                bv = (byte)EditorGUILayout.IntField("Adaptive Depth", config.adaptiveDepth);
                config.adaptiveDepth = (byte)Mathf.Clamp(bv, 0, byte.MaxValue);

            }
        }
        EditorGUILayout.Separator();

        if (GUI.changed)
            EditorUtility.SetDirty(target);
    }

    [MenuItem(EditorUtil.MainMenu + "Crowd Avoidance Settings", false, EditorUtil.GlobalGroup)]
    static void EditSettings()
    {
        CrowdAvoidanceSet item = EditorUtil.GetGlobalAsset<CrowdAvoidanceSet>();
        Selection.activeObject = item;
    }
}
