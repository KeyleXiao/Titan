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
    /// Provides a way of displaying a very basic GUI label list.
    /// </summary>
    /// <remarks>
    /// <p>Labels are displayed from lowest to highest index in the
    /// specified quadrant of the screen. The width of the labels will be
    /// the quadrant size minus the specfied margins.</p></remarks>
    public sealed class LabelRegion
    {
        private Rect mRegion;
        private string[] mSlots;
        private float mSlotHeight;
        private int mQuadrant;
        private float mMargin;
        private float mLabelHeight;

        /// <summary>
        /// Number of available label positions.
        /// </summary>
        public int SlotCount { get { return mSlots.Length; } }

        /// <summary>
        /// The area of the screen used for drawing.
        /// </summary>
        public Rect Region
        {
            get { return mRegion; }
        }

        /// <summary>
        /// The quadrant of the screen to display the labels in.
        /// </summary>
        /// <remarks>
        /// <p>1 : Upper left<br/>
        /// 2: Upper right<br/>
        /// 3: Lower left<br/>
        /// 4: Lower right</p>
        /// </remarks>
        public int Quadrant
        {
            get { return mQuadrant; }
        }

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="quadrant">1 : Upper left, 2: Upper right
        /// , 3: Lower left, 4: Lower right</param>
        /// <param name="margin">The margin to place around the drawing area.
        /// </param>
        /// <param name="labelHeight">The height of the drawing area for
        /// each label.</param>
        public LabelRegion(int quadrant, float margin, float labelHeight)
        {
            mQuadrant = Mathf.Clamp(quadrant, 1, 4);
            mMargin = Mathf.Max(0, margin);
            mLabelHeight = Mathf.Max(10, labelHeight);
            Reset();
        }

        /// <summary>
        /// Call if the screen size changes.
        /// </summary>
        public void Reset()
        {
            float height = Screen.height / 2 - 2 * mMargin;
            float width = Screen.width / 2 - 2 * mMargin;

            switch (mQuadrant)
            {
                case 1:
                    mRegion = new Rect(mMargin, mMargin, width, height);
                    break;
                case 2:
                    mRegion = new Rect(Screen.width / 2 + mMargin, mMargin
                        , width, height);
                    break;
                case 3:
                    mRegion = new Rect(mMargin, Screen.height / 2 + mMargin
                        , width, height);
                    break;
                case 4:
                    mRegion = new Rect(Screen.width / 2 + mMargin
                        , Screen.height / 2 + mMargin
                        , width, height);
                    break;
            }

            mSlotHeight = mLabelHeight;
            mSlots = new string[Mathf.FloorToInt(mRegion.height / mSlotHeight)];
        }

        /// <summary>
        /// Clears all labels.
        /// </summary>
        public void Clear()
        {
            for (int i = 0; i < mSlots.Length; i++)
            {
                mSlots[i] = "";
            }
        }

        /// <summary>
        /// Sets the last label.  (Lowest on the screen.)
        /// </summary>
        public string Last
        {
            get { return mSlots[mSlots.Length - 1]; }
            set { mSlots[mSlots.Length - 1] = value; }
        }

        /// <summary>
        /// Sets the specified label.
        /// </summary>
        /// <param name="index">The index position.</param>
        /// <param name="label">The label prefix.</param>
        /// <param name="content">The label content.</param>
        public void Set(int index, string label, string content)
        {
            mSlots[index] = label + ": " + content;
        }

        /// <summary>
        /// The label at the specified position.
        /// </summary>
        /// <param name="index">The label position.</param>
        /// <returns>The label at the specified position.</returns>
        public string  this[int index]
        {
            get { return mSlots[index]; }
            set { mSlots[index] = value; }
        }

        /// <summary>
        /// Display the labels.
        /// </summary>
        public void OnGUI()
        {
            Rect rect = mRegion;
            rect.y -= 2 * mSlotHeight;
            rect.height = mSlotHeight;
            for (int i = 0; i < mSlots.Length; i++)
            {
                rect.y += mSlotHeight;
                if (mSlots[i] == "")
                    continue;
                GUI.Label(rect, mSlots[i]);
            }
        }
    }
}
