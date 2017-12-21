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

/// <summary>
/// A simple camera controller that allows free movement along all axes.
/// </summary>
[AddComponentMenu("Camera-Control/Free Move")]
public class FreeMove 
    : MonoBehaviour
{
    public static string MoveFB = "MoveFB";
    public static string StrafeRL = "StrafeRL";
    public static string MoveUD = "MoveUD";

    /// <summary>
    /// If TRUE, movement will relative to the orientation of local axes rather 
    /// than the global axes.
    /// </summary>
    public bool adaptToDirection = true;

    public float speed = 3;
    private Transform trans;
    private Vector3 moveDirection = Vector3.zero;

    void FixedUpdate()
    {
        moveDirection = new Vector3(Input.GetAxis(StrafeRL)
            , 0
            , Input.GetAxis(MoveFB));

        moveDirection.y += Input.GetAxis(MoveUD);
        moveDirection *= speed * Time.deltaTime;

        if (adaptToDirection)
            trans.Translate(moveDirection, Space.Self);
        else
            trans.Translate(moveDirection, Space.World);
    }

    void Start()
    {
        trans = transform;
    }
}
