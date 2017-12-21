using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public static class MathExtensions
{
    public static bool IsNearlyEqual(this float _value1, float _value2, float ErrorTolerance = float.MinValue)
    {
        return Mathf.Abs(_value1 - _value2) <= ErrorTolerance;
    }
}

