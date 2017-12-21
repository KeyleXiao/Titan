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
    /// Provides a simple re-sizable buffer that provides its own
    /// GUI component.
    /// </summary>
    /// <typeparam name="T">The type of buffer to create.</typeparam>
    public class BufferHandler<T>
    {
        /// <summary>
        /// The label to use for the GUI.
        /// </summary>
        public string label;

        /// <summary>
        /// The length of the elements in the buffer.
        /// </summary>
        public int stride;

        /// <summary>
        /// The maximum size allowed for the buffer. (Applied via the GUI.)
        /// </summary>
        public int sizeMax;

        /// <summary>
        /// The minimum size allowed for the buffer.  (Applied via the GUI.)
        /// </summary>
        public int sizeMin;

        /// <summary>
        /// The size to apply to the buffer when it is next resized.
        /// </summary>
        public int sizeTarget;

        /// <summary>
        /// The buffer.
        /// </summary>
        public T[] buffer;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="label">The label to use for the GUI.</param>
        /// <param name="stride">The length of the elements in the buffer.
        /// </param>
        /// <param name="initial">The initial size of the buffer.
        /// (Element count.  length = initial * stride)</param>
        /// <param name="min">The maximum size allowed for the buffer. </param>
        /// <param name="max">The minimum size allowed for the buffer.</param>
        public BufferHandler(string label
            , int stride
            , int initial, int min, int max)
        {
            this.label = label;
            this.stride = Mathf.Max(1, stride);
            sizeMin = Mathf.Max(1, min);
            sizeMax = Mathf.Max(min, max);
            sizeTarget = Mathf.Clamp(initial, sizeMin, sizeMax);
            buffer = new T[sizeTarget * stride];
        }

        /// <summary>
        /// Clears and resizes the buffer if needed based on the value of
        /// <see cref="sizeTarget"/>
        /// </summary>
        /// <returns>TRUE if the buffer was resized and cleared.
        /// FALSE if no resize is needed.</returns>
        public bool HandleResize()
        {
            if (buffer.Length != sizeTarget * stride)
            {
                buffer = new T[sizeTarget * stride];
                return true;
            }
            return false;
        }

        /// <summary>
        /// The number of elements the buffer can hold.
        /// </summary>
        public int MaxElementCount { get { return buffer.Length / stride; } }

        /// <summary>
        /// Displays the GUI in the current container.
        /// </summary>
        public void OnGUI()
        {
            GUILayout.Label(label);

            GUILayout.BeginHorizontal();

            sizeTarget = (int)GUILayout.HorizontalSlider(sizeTarget
                , sizeMin
                , sizeMax);
            GUILayout.Label(sizeTarget.ToString());

            GUILayout.EndHorizontal();
        }

    }
}
