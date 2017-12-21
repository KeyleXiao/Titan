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

namespace org.critterai.samples.qe
{
    // Do not change the values without also changing main QE class GUI.
    public enum QEStateType
    {
        GetNearestPointG = 0,
        GetNearestPointL = 1,
        GetNearestPointF = 2,

        FindDistanceToWall = 3,
        GetWallSegments = 4,
        GetSegments = 5,
        GetPolyHeight = 6,
        GetPolys = 7,

        GetPolysLocal = 8,
        FindPolysByRadius = 9,
        FindPolysByConvex = 10,

        FindPath = 11,
        StraightPath = 12,
        Raycast = 13,
        MoveOnSurface = 14,

        Inactive  // Always at the end.
    }
}
