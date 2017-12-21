/**********************************************************************

Filename    :	SFEvents.cs
Content     :	Definition of SFEvent Wrapper
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Scaleform
{
/// <summary>
    /// A base class for input event notification.
/// </summary>
    
[StructLayout(LayoutKind.Sequential)]
public class SFEvent
{
    public SFEvent()
    {
        Type = SFEventType.Unknown;
    }
    public SFEvent(SFEventType eventType)
    {
        Type = eventType;
    }
    /// <summary>
    /// List of event types.
    /// </summary>
    public enum SFEventType
    {
        Unknown = 0,       
        // Informative events sent to the player.
        MouseMove, //Informs the movie about mouse cursor movement.
        MouseDown, //Informs the movie about mouse button being pressed.
        MouseUp, //Informs the movie about mouse button being released.
        MouseWheel,//Informs the movie about mouse wheel being scrolled.
        KeyDown, //Informs the movie about keyboard key being pressed.
        KeyUp,//Informs the movie about keyboard key being released.
        SceneResize,//Informs the movie about its stage area being resized.
        SetFocus,//Informs the movie about keyboard focus gain.
        KillFocus,//Informs the movie about keyboard focus loss.
        Char,//Informs the movie about the character key being typed.
        IME//Informs the movie about the IME events.
    };
    
    // What kind of event this is.
    public SFEventType          Type;
    // State of special keys
    public SFKeyModifiers       EvModifiers;
    
    public UInt32   Dummy;
    
}

/// <summary>
/// A mouse event for a movie clip.
/// </summary>
public class MouseEvent:SFEvent
{
    public float X; //Mouse cursor x coordinate in relation to movie viewport at the time of mouse event.
    public float Y; //Mouse cursor y coordinate in relation to movie viewport at the time of mouse event.
    public float ScrollDelta; //The scroll delta of the mouse wheel, used if event Type is MouseWheel.
    public int Button;//The mouse button that was pressed.
    public int MouseIndex;//The index of the mouse button that was pressed. This is 0 for the first (left) mouse button, 1 for the second (right) mouse button and 2 for the middle button.
    
    public MouseEvent(): base()
    {
        Button = 0; X = 0; Y = 0; ScrollDelta = 0.0f; MouseIndex = 0;
    }
    
    public MouseEvent(SFEvent.SFEventType eventType, float xpos, float ypos, float scrollVal = 0.0f, int mouseIdx = 0, int button = 0)
        :base(eventType)
    {
        Button = button; X = xpos; Y = ypos; ScrollDelta = scrollVal;
        MouseIndex = mouseIdx;
    }
}

 /// <summary>
///  A keyboard event for a movie clip.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public class KeyEvent:SFEvent
{
    public SFKey.Code KeyCode;//Key, and whether it was pressed up or down.
    public Byte AsciiCode; //ASCII code. 0, if unavailable. 
    public UInt32 WcharCode; //Unicode for typed character, 0 if unavailable. 
    public Byte KeyboardIndex; //The index of the physical keyboard controller.
    
    public KeyEvent(Event ev, UInt32 down): base()
    {
        AsciiCode        = 0;
        WcharCode        = 0;
        KeyboardIndex    = 0;
        if (down == 1)
        {
            Type            = SFEvent.SFEventType.KeyDown;
        }
        else
        {
            Type            = SFEvent.SFEventType.KeyUp;
        }
        if (ev.keyCode == UnityEngine.KeyCode.A)
        {
            KeyCode         = SFKey.Code.A;    
        }
    }
}

/// <summary>
/// Used to provide Scaleform with codes of character being typed.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public class CharEvent:SFEvent
{
    public UInt32 WcharCode;// Wide character code (Unicode).
    public Byte             KeyboardIndex; // The index of the physical keyboard controller.

/// <summary>
/// CharEvent constructor.
/// </summary>
/// <param name="ev"></param>
    public CharEvent(Event ev): base()
    {
        KeyboardIndex    = 0;
        WcharCode        = ev.character; 
    }
}

} // namespace Scaleform;