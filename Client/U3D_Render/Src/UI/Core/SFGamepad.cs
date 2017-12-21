/**********************************************************************

Filename    :	SFGamepad.cs
Content     :	Logic for processing GamePad events. Feel free to modify to suit your requirements.
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/
using System;
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;

namespace Scaleform
{

    /// <summary>
    /// The SFGamepad class provides an interface for game controllers as an input device.
    /// </summary>
    public class SFGamepad
    {
        /// <summary>
        /// GamePad definition with a Keymap, Joystick array and index of the controller.
        /// </summary>
        protected struct GamePad
        {
            public UnityButtonToSFKey[] Keymap;
            public Joystick[] Joysticks;
            public int ControllerIndex;
        };

        /// <summary>
        /// The Joystick definition maps to an analog joystick with two axes of movement.
        /// </summary>
        protected struct Joystick
        {
            public String HorizontalAxisName;
            public String VerticalAxisName;
            public double HorizontalAxisThresholdD;
            public double VerticalAxisThresholdD;
            public bool RightKeyPressed, LeftKeyPressed, UpKeyPressed, DownKeyPressed;
            public float RightTimerF, LeftTimerF, UpTimerF, DownTimerF;
        };

        /// <summary>
        /// UnityButtonToSFKey pair which maps a Unity button and SFKeyCode.
        /// </summary>
        protected struct UnityButtonToSFKey
    	{
    		public UnityButtonToSFKey(String unityButton, SFKey.Code sfKeyCode)
    		{
    			UnityButton = unityButton;
    			SFKeyCode	= sfKeyCode;
    		}

    		public String UnityButton;
    		public SFKey.Code SFKeyCode;
    	};

        protected GamePad[] GamePads;

    	protected SFManager SFMgr;
    	protected double HorizontalAxisThresholdD = 0.5;
    	protected double VerticalAxisThresholdD = 0.5;
        protected float RepeatTimerThresholdF = .5f;

        public SFGamepad() { }

        public SFGamepad(SFManager sfMgr)
        {
            SFMgr = sfMgr;
        }

        /// <summary>
        /// Initialize the GamePad and create default mappings and default joystick.
        /// </summary>
        public void Init()
        {
            GamePads = new GamePad[1];

            GamePads[0] = new GamePad();

            GamePads[0].Keymap = new UnityButtonToSFKey[4];
            GamePads[0].Keymap[0] = new UnityButtonToSFKey("Fire1", SFKey.Code.A);
            GamePads[0].Keymap[1] = new UnityButtonToSFKey("Fire2", SFKey.Code.B);
            GamePads[0].Keymap[2] = new UnityButtonToSFKey("Fire3", SFKey.Code.X);
            GamePads[0].Keymap[3] = new UnityButtonToSFKey("Jump", SFKey.Code.Y);
            GamePads[0].ControllerIndex = 0;

            GamePads[0].Joysticks = new Joystick[1];
            GamePads[0].Joysticks[0] = new Joystick();
            GamePads[0].Joysticks[0].HorizontalAxisName = "Horizontal";
            GamePads[0].Joysticks[0].VerticalAxisName = "Vertical";
            GamePads[0].Joysticks[0].HorizontalAxisThresholdD = this.HorizontalAxisThresholdD;
            GamePads[0].Joysticks[0].VerticalAxisThresholdD = this.VerticalAxisThresholdD;
        }

        /// <summary>
        /// Process GamePad keys, axes, and buttons for dispatch to SFManager.
        /// </summary>
    	public void Update()
    	{
            for (int a = 0; a < GamePads.Length; a++)
            {
                // Let's process GamePad keys first. 
                int UnityButtonToSFKey_size = GamePads[a].Keymap.Length;
                for (int i = 0; i < UnityButtonToSFKey_size; i++)
                {
                    if (InputManager.GetButtonDown(GamePads[a].Keymap[i].UnityButton))
                    {
                        SFMgr.HandleKeyDownEvent(GamePads[a].Keymap[i].SFKeyCode, 0, GamePads[a].ControllerIndex);
                    }
                    if (InputManager.GetButtonUp(GamePads[a].Keymap[i].UnityButton))
                    {
                        SFMgr.HandleKeyUpEvent(GamePads[a].Keymap[i].SFKeyCode, 0, GamePads[a].ControllerIndex);
                    }
                }

                // Let's process axis now.
                for (int b = 0; b < GamePads[a].Joysticks.Length;b++ )
                {
                    double horizontal = InputManager.GetAxis(GamePads[a].Joysticks[b].HorizontalAxisName);
                    double vertical = InputManager.GetAxis(GamePads[a].Joysticks[b].VerticalAxisName);

                    bool currR = horizontal > GamePads[a].Joysticks[b].HorizontalAxisThresholdD;
                    bool currL = horizontal < -GamePads[a].Joysticks[b].HorizontalAxisThresholdD;
                    bool currD = vertical < -GamePads[a].Joysticks[b].VerticalAxisThresholdD;
                    bool currU = vertical > GamePads[a].Joysticks[b].VerticalAxisThresholdD;

                    if (currR)
                    {
                        GamePads[a].Joysticks[b].RightTimerF += UnityEngine.Time.deltaTime;

                        if (!GamePads[a].Joysticks[b].RightKeyPressed || GamePads[a].Joysticks[b].RightTimerF > RepeatTimerThresholdF)
                        {
                            GamePads[a].Joysticks[b].RightTimerF = 0f;
                            SFMgr.HandleKeyDownEvent(SFKey.Code.Right, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].RightKeyPressed = true;
                        }
                    }
                    else
                    {
                        if (GamePads[a].Joysticks[b].RightKeyPressed)
                        {
                            SFMgr.HandleKeyUpEvent(SFKey.Code.Right, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].RightKeyPressed = false;
                        }
                    }

                    if (currL)
                    {
                        GamePads[a].Joysticks[b].LeftTimerF += UnityEngine.Time.deltaTime;

                        if (!GamePads[a].Joysticks[b].LeftKeyPressed || GamePads[a].Joysticks[b].LeftTimerF > RepeatTimerThresholdF)
                        {
                            GamePads[a].Joysticks[b].LeftTimerF = 0f;
                            SFMgr.HandleKeyDownEvent(SFKey.Code.Left, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].LeftKeyPressed = true;
                        }
                    }
                    else
                    {
                        if (GamePads[a].Joysticks[b].LeftKeyPressed)
                        {
                            SFMgr.HandleKeyUpEvent(SFKey.Code.Left, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].LeftKeyPressed = false;
                        }
                    }

                    if (currD)
                    {
                        GamePads[a].Joysticks[b].DownTimerF += UnityEngine.Time.deltaTime;

                        if (!GamePads[a].Joysticks[b].DownKeyPressed || GamePads[a].Joysticks[b].DownTimerF > RepeatTimerThresholdF)
                        {
                            GamePads[a].Joysticks[b].DownTimerF = 0f;
                            SFMgr.HandleKeyDownEvent(SFKey.Code.Down, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].DownKeyPressed = true;
                        }
                    }
                    else
                    {
                        if (GamePads[a].Joysticks[b].DownKeyPressed)
                        {
                            SFMgr.HandleKeyUpEvent(SFKey.Code.Down, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].DownKeyPressed = false;
                        }
                    }

                    if (currU)
                    {
                        GamePads[a].Joysticks[b].UpTimerF += UnityEngine.Time.deltaTime;

                        if (!GamePads[a].Joysticks[b].UpKeyPressed || GamePads[a].Joysticks[b].UpTimerF > RepeatTimerThresholdF)
                        {
                            GamePads[a].Joysticks[b].UpTimerF = 0f;
                            SFMgr.HandleKeyDownEvent(SFKey.Code.Up, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].UpKeyPressed = true;
                        }
                    }
                    else
                    {
                        if (GamePads[a].Joysticks[b].UpKeyPressed)
                        {
                            SFMgr.HandleKeyUpEvent(SFKey.Code.Up, 0, GamePads[a].ControllerIndex);
                            GamePads[a].Joysticks[b].UpKeyPressed = false;
                        }
                    }
                }
            }
    	}

        /// <summary>
        /// IsConsole
        /// </summary>
    	bool IsConsole()
    	{
    		return false;
    	}
    } // class SFGamepad
} // namespace Scaleform
