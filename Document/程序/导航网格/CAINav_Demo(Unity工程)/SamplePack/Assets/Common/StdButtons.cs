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
    /// Input button mapping for sample projects.
    /// </summary>
    public static class StdButtons
    {
        /// <summary>
        /// The key used for primary select operations.
        /// </summary>
        public static KeyCode SelectA = KeyCode.Mouse0;

        /// <summary>
        /// The key used for secondary select operations.
        /// </summary>
        public static KeyCode SelectB = KeyCode.Space;

        /// <summary>
        /// The key used to trigger a set operation.
        /// </summary>
        public static KeyCode SetA = KeyCode.F;

        /// <summary>
        /// The key used to trigger an secondary set opetration.
        /// </summary>
        public static KeyCode SetB = KeyCode.R;

        /// <summary>
        /// The key used to trigger an secondary set opetration.
        /// </summary>
        public static KeyCode SetC = KeyCode.V;

        /// <summary>
        /// The key used to trigger positive y-axis translation and scaling 
        /// operations.
        /// </summary>
        public static KeyCode AdjustYPlus = KeyCode.RightBracket;

        /// <summary>
        /// The key used to trigger negative y-axis translation and scaling 
        /// operations.
        /// </summary>
        public static KeyCode AdjustYMinus = KeyCode.LeftBracket;

        /// <summary>
        /// The key used to trigger positive xz-plane scaling operations.
        /// </summary>
        public static KeyCode AdjustXZPlus = KeyCode.Equals;

        /// <summary>
        /// The key used to trigger negative xz-plane scaling operations.
        /// </summary>
        public static KeyCode AdjustXZMinus = KeyCode.Minus;

        /// <summary>
        /// The key used to toggle the GUI on/off.
        /// </summary>
        public static KeyCode ToggleGUI = KeyCode.Tab;

        /// <summary>
        /// The key used to quit the application.
        /// </summary>
        public static KeyCode Quit = KeyCode.Escape;
    }
}
