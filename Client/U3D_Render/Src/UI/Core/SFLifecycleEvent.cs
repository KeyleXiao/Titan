/**********************************************************************

Filename    :	SFLifecycleEvent.cs
Content     :	Defintion of LifecycleEvent
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
    
public class SFLifecycleEvent
{
    
    public SFLifecycleEvent(Movie movie)
    {
        Movie = movie;
    }
    public Movie Movie;
    public virtual void Execute() {}
    
}

} // namespace Scaleform

