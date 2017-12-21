/*
 * Copyright (c) 2011 Stephen A. Pratt
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
using org.critterai.samples;

/// <summary>
/// Provides simulation behavior common to all simulations.
/// </summary>
public class SimStandard 
    : MonoBehaviour 
{
    public const string MainHelpText =
        "Camera: Move: [w s a d e c], Rotate: Hold [mouse1] (The other one.)";

    public static bool rootMenuAvailable = false;

    public bool enableHideRenderers = false;

    private bool mRenderersHidden = false;

    void Awake()
    {
        SimGUIUtil.Initialize();
        SimGUIUtil.sysHelpText = MainHelpText;
    }

    void Update()
    {
        if (Input.GetKeyDown(StdButtons.Quit))
        {
            Application.Quit();
            return;
        }

        if (Input.GetKeyDown(StdButtons.ToggleGUI))
            SimGUIUtil.hideGUI = !SimGUIUtil.hideGUI;

        if (!enableHideRenderers && mRenderersHidden)
            ToggleRenderers();

        SimGUIUtil.sysControlZone.height = (rootMenuAvailable ? 28 : 0)
            + (enableHideRenderers ? 28 : 0);
    }

    void OnGUI()
    {
        if (SimGUIUtil.hideGUI)
            return;

        if (!(rootMenuAvailable || enableHideRenderers))
            return;

        GUILayout.BeginArea(SimGUIUtil.sysControlZone, GUI.skin.box);

        if (rootMenuAvailable && GUILayout.Button("Menu"))
            Application.LoadLevel(0);

        if (enableHideRenderers)
        {
            bool bn = GUILayout.Toggle(mRenderersHidden, "Hide Geometry");
            if (bn != mRenderersHidden)
                ToggleRenderers();
        }

        GUILayout.EndArea();
    }

    private void ToggleRenderers()
    {
        MeshRenderer[] renderers
            = (MeshRenderer[])FindObjectsOfType(typeof(MeshRenderer));

        foreach (MeshRenderer renderer in renderers)
            renderer.enabled = mRenderersHidden;

        mRenderersHidden = !mRenderersHidden;


    }
}
