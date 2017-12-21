/*
 * Copyright (c) 2011-2012 Stephen A. Pratt
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
/// Displays a menu for choosing simulations.
/// </summary>
public class MainSim 
    : MonoBehaviour 
{
    void Awake()
    {
        SimStandard.rootMenuAvailable = false;
    }

    void Update()
    {
        if (Input.GetKey(StdButtons.Quit))
            Application.Quit();
    }

    void OnGUI()
    {
        float winc = Screen.width * 0.3333f;
        Rect rect = new Rect(winc
            , Screen.height * 0.25f
            , winc
            , Screen.height * 0.5f);

        int iLevel = int.MinValue;

        GUILayout.BeginArea(rect);

        DisplayCenterText("CAINav Sample Pack");
        DisplayCenterText("critterai.org");

        GUILayout.Space(SimGUIUtil.LineHeight);
        DisplayCenterText("Feature Explorers");

        if (GUILayout.Button("NavmeshQuery - Fun Zone"))
            iLevel = 1;
        if (GUILayout.Button("NavmeshQuery - Training Center"))
            iLevel = 2;
        if (GUILayout.Button("PathCorridor"))
            iLevel = 3;

        GUILayout.Space(SimGUIUtil.LineHeight);
        DisplayCenterText("Demos");

        if (GUILayout.Button("Crowd Demo"))
            iLevel = 4;

        GUILayout.Space(SimGUIUtil.LineHeight);
        if (GUILayout.Button("Quit"))
            Application.Quit();

        GUILayout.EndArea();

        if (iLevel != int.MinValue)
        {
            SimStandard.rootMenuAvailable = true;
            Application.LoadLevel(iLevel);
        }
    }

    private void DisplayCenterText(string value)
    {
        GUILayout.BeginHorizontal();
        GUILayout.FlexibleSpace();
        GUILayout.Label(value);
        GUILayout.FlexibleSpace();
        GUILayout.EndHorizontal();
    }
       
}
