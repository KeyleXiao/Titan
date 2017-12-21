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
using org.critterai.nav.u3d;
using org.critterai.nav;

namespace org.critterai.samples.qe
{
    /// <summary>
    /// Provides various Query Explorer utilities.
    /// </summary>
    public static class QEUtil
    {
        public const float HeadScaleSm = 0.15f;
        public const float HeadScaleNorm = 0.25f;

        public const float MarkerScaleSm = 0.15f;
        public const float MarkerScaleNorm = 0.25f;

        /// <summary>
        /// The continuous scaling speed used for resizing GUI elements.
        /// </summary>
        public const float GUISpeed = 1.0f;

        public const int SmallBufferInit = 20;
        public const int SmallBufferMax = 50;

        public enum SearchResult
        {
            /// <summary>
            /// No hit anywhere.
            /// </summary>
            Failed,
            /// <summary>
            /// Hit the geometry, but could not be constrained to the
            /// navigation mesh.
            /// </summary>
            HitGeometry,

            /// <summary>
            /// Hit the geometry and was able to contrain it to the
            /// navigation mesh.
            /// </summary>
            HitNavmesh
        }

        public static Rect buttonZone;

        public static float hitRayLength = 100;
        public static bool hasHit = false;
        public static Vector3 hitPosition;

        public static Color SelectColor { get { return Color.cyan; } }

        public static bool InControlZone(Vector3 position)
        {
            if (SimGUIUtil.hideGUI)
                return false;
            if (SimGUIUtil.InControlZone(position))
                return true;
            position.y = Screen.height - position.y;
            return (buttonZone.Contains(position));
        }

        public static bool Raycast()
        {
            if (InControlZone(Input.mousePosition))
            {
                hasHit = false;
            }
            else
            {
                RaycastHit hit;
                Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

                if (!Physics.Raycast(ray, out hit, hitRayLength))
                    hasHit = false;
                else
                {
                    hitPosition = hit.point;
                    hasHit = true;
                }
            }
            return hasHit;
        }

        public static float GetXZFactor()
        {
            if (Input.GetKey(StdButtons.AdjustXZMinus))
                return -GUISpeed * Time.deltaTime;
            else if (Input.GetKey(StdButtons.AdjustXZPlus))
                return GUISpeed * Time.deltaTime;
            else
                return 0;
        }

        public static float GetYFactor()
        {
            if (Input.GetKey(StdButtons.AdjustYMinus))
                return -GUISpeed * Time.deltaTime;
            else if (Input.GetKey(StdButtons.AdjustYPlus))
                return GUISpeed * Time.deltaTime;
            else
                return 0;
        }

        /// <summary>
        /// Provides a standard way detecting where the current 
        /// <see cref="hitPosition"/> is on the navigation mesh.
        /// </summary>
        public static SearchResult HandleStandardPolySearch(NavGroup helper
            , out Vector3 geomPoint
            , out NavmeshPoint navPoint
            , out string message)
        {
            if (!hasHit)
            {
                message = "Outside source geometry.";
                navPoint = new NavmeshPoint();
                geomPoint = Vector3.zero;
                return SearchResult.Failed;
            }

            geomPoint = hitPosition;
            
            NavStatus status = 
                helper.query.GetNearestPoint(geomPoint, helper.extents, helper.filter, out navPoint);

            message = "GetNearestPoint: " + status.ToString();

            if (NavUtil.Failed(status))
                return SearchResult.HitGeometry;

            if (navPoint.polyRef == 0)
            {
                message = "Too far from navmesh: GetNearestPoint: " + status.ToString();
                return SearchResult.HitGeometry;
            }

            return SearchResult.HitNavmesh;
        }
    }
}
