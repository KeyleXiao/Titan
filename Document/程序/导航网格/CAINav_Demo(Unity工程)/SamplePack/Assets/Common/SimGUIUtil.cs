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

namespace org.critterai.samples
{
    /// <summary>
    /// Provides common GUI management features.
    /// </summary>
	public static class SimGUIUtil
	{
        // Note: Quadrants 1 and 2 are used by help and the control zones.
        public const int GUIQuadrant = 3;

        public const float Margin = 10;
        public const float LineHeight = 22;

        public static bool hideGUI = false;

        public static Rect sysControlZone;
        public static Rect sysHelpZone;

        public static Rect contextControlZone;
        public static Rect contextHelpZone;
        public static bool contextActive = false;

        public static string sysHelpText;
        public static string contextHelpText;

        public static LabelRegion labels;

        public static bool hasHit;
        public static RaycastHit hit;
        public static float castDistance = 100;

        public static void Update()
        {
            // Handle Mouse Input

            if (InControlZone(Input.mousePosition))
            {
                hasHit = false;
                return;
            }

            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            hasHit = Physics.Raycast(ray, out hit, castDistance);
        }

        /// <summary>
        /// Should be called by the master simulation at startup, and when
        /// the screen size changes.
        /// </summary>
        public static void Initialize()
        {
            float caWidth = Screen.width / 4f - 2 * Margin;

            // The control zones always start with zero height.
            // User's are expected to update the height as needed.

            contextControlZone = new Rect(Margin, Margin, caWidth, 0);

            sysControlZone = new Rect(Screen.width - Screen.width / 4f + Margin
                , Margin
                , caWidth
                , 0);

            float helpHeight = LineHeight + Margin;
            float helpWidth =
                sysControlZone.xMin - contextControlZone.xMax - 2 * Margin;

            sysHelpZone = new Rect(contextControlZone.xMax + Margin
                , Margin
                , helpWidth
                , helpHeight);

            contextHelpZone = new Rect(contextControlZone.xMax + Margin
                , sysHelpZone.yMax
                , helpWidth
                , helpHeight);
        }

        public static bool InControlZone(Vector3 position)
        {
            position.y = Screen.height - position.y;

            return (contextActive && contextControlZone.Contains(position))
                || sysControlZone.Contains(position);
        }

        public static void OnGUI()
        {
            if (!hideGUI)
            {
                GUI.Label(sysHelpZone, sysHelpText);

                if (contextActive)
                    GUI.Label(contextHelpZone, contextHelpText);
            }

            if (labels != null)
                labels.OnGUI();
        }

        public static void BuildLabelRegion(bool force)
        {
            if (labels == null || force)
                labels = new LabelRegion(GUIQuadrant, Margin, LineHeight);

            labels.Clear();
        }
	}
}
