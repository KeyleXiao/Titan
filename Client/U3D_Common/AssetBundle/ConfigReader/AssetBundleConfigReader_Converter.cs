
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;
using UnityEngine;

//namespace Zen
//{
public partial class AssetBundleConfigReader
{
    public static Type ConvertString(string value, out object convertedValue)
    {
        // First check the whole number types, because floating point types will always parse whole numbers
        // Start with the smallest types
        byte byteResult;
        if (byte.TryParse(value, out byteResult))
        {
            convertedValue = byteResult;
            return typeof(byte);
        }

        short shortResult;
        if (short.TryParse(value, out shortResult))
        {
            convertedValue = shortResult;
            return typeof(short);
        }

        int intResult;
        if (int.TryParse(value, out intResult))
        {
            convertedValue = intResult;
            return typeof(int);
        }

        long longResult;
        if (long.TryParse(value, out longResult))
        {
            convertedValue = longResult;
            return typeof(long);
        }

        ulong ulongResult;
        if (ulong.TryParse(value, out ulongResult))
        {
            convertedValue = ulongResult;
            return typeof(ulong);
        }

        // No need to check the rest of the unsigned types, which will fit into the signed whole number types

        // Next check the floating point types
        float floatResult;
        if (float.TryParse(value, out floatResult))
        {
            convertedValue = floatResult;
            return typeof(float);
        }


        // It's not clear that there's anything that double.TryParse() and decimal.TryParse() will parse 
        // but which float.TryParse() won't
        double doubleResult;
        if (double.TryParse(value, out doubleResult))
        {
            convertedValue = doubleResult;
            return typeof(double);
        }

        decimal decimalResult;
        if (decimal.TryParse(value, out decimalResult))
        {
            convertedValue = decimalResult;
            return typeof(decimal);
        }

        // It's not a number, so it's either a bool, char or string
        bool boolResult;
        if (bool.TryParse(value, out boolResult))
        {
            convertedValue = boolResult;
            return typeof(bool);
        }

        char charResult;
        if (char.TryParse(value, out charResult))
        {
            convertedValue = charResult;
            return typeof(char);
        }

        convertedValue = value;
        return typeof(string);
    }

    /// <summary>
    /// Compare two types and find a type that can fit both of them
    /// </summary>
    /// <param name="typeA">First type to compare</param>
    /// <param name="typeB">Second type to compare</param>
    /// <returns>The type that can fit both types, or string if they're incompatible</returns>
    public static Type FindCommonType(Type typeA, Type typeB)
    {
        // Build the singleton type map (which will rebuild it in a typesafe manner
        // if it's not already built).
        BuildTypeMap();

        if (!typeMap.ContainsKey(typeA))
            return typeof(string);

        if (!typeMap[typeA].ContainsKey(typeB))
            return typeof(string);

        return typeMap[typeA][typeB];
    }


    // Dictionary to map two types to a common type that can hold both of them
    private static Dictionary<Type, Dictionary<Type, Type>> typeMap = null;

    // Locker object to build the singleton typeMap in a typesafe manner
    private static object locker = new object();

    /// <summary>
    /// Build the singleton type map in a typesafe manner.
    /// This map is a dictionary that maps a pair of types to a common type.
    /// So typeMap[typeof(float)][typeof(uint)] will return float, while
    /// typemap[typeof(char)][typeof(bool)] will return string.
    /// </summary>
    private static void BuildTypeMap()
    {
        lock (locker)
        {
            if (typeMap == null)
            {
                typeMap = new Dictionary<Type, Dictionary<Type, Type>>()
                    {
                        // Comparing byte
                        {typeof(byte), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(byte) },
                            { typeof(short), typeof(short) },
                            { typeof(int), typeof(int) },
                            { typeof(long), typeof(long) },
                            { typeof(ulong), typeof(ulong) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing short
                        {typeof(short), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(short) },
                            { typeof(short), typeof(short) },
                            { typeof(int), typeof(int) },
                            { typeof(long), typeof(long) },
                            { typeof(ulong), typeof(ulong) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing int
                        {typeof(int), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(int) },
                            { typeof(short), typeof(int) },
                            { typeof(int), typeof(int) },
                            { typeof(long), typeof(long) },
                            { typeof(ulong), typeof(ulong) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing long
                        {typeof(long), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(long) },
                            { typeof(short), typeof(long) },
                            { typeof(int), typeof(long) },
                            { typeof(long), typeof(long) },
                            { typeof(ulong), typeof(ulong) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing ulong
                        {typeof(ulong), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(ulong) },
                            { typeof(short), typeof(ulong) },
                            { typeof(int), typeof(ulong) },
                            { typeof(long), typeof(ulong) },
                            { typeof(ulong), typeof(ulong) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing float
                        {typeof(float), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(float) },
                            { typeof(short), typeof(float) },
                            { typeof(int), typeof(float) },
                            { typeof(long), typeof(float) },
                            { typeof(ulong), typeof(float) },
                            { typeof(float), typeof(float) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing double
                        {typeof(double), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(double) },
                            { typeof(short), typeof(double) },
                            { typeof(int), typeof(double) },
                            { typeof(long), typeof(double) },
                            { typeof(ulong), typeof(double) },
                            { typeof(float), typeof(double) },
                            { typeof(double), typeof(double) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing decimal
                        {typeof(decimal), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(decimal) },
                            { typeof(short), typeof(decimal) },
                            { typeof(int), typeof(decimal) },
                            { typeof(long), typeof(decimal) },
                            { typeof(ulong), typeof(decimal) },
                            { typeof(float), typeof(decimal) },
                            { typeof(double), typeof(decimal) },
                            { typeof(decimal), typeof(decimal) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing bool
                        {typeof(bool), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(string) },
                            { typeof(short), typeof(string) },
                            { typeof(int), typeof(string) },
                            { typeof(long), typeof(string) },
                            { typeof(ulong), typeof(string) },
                            { typeof(float), typeof(string) },
                            { typeof(double), typeof(string) },
                            { typeof(decimal), typeof(string) },
                            { typeof(bool), typeof(bool) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing char
                        {typeof(char), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(string) },
                            { typeof(short), typeof(string) },
                            { typeof(int), typeof(string) },
                            { typeof(long), typeof(string) },
                            { typeof(ulong), typeof(string) },
                            { typeof(float), typeof(string) },
                            { typeof(double), typeof(string) },
                            { typeof(decimal), typeof(string) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(char) },
                            { typeof(string), typeof(string) },
                        }},

                        // Comparing string
                        {typeof(string), new Dictionary<Type, Type>() {
                            { typeof(byte), typeof(string) },
                            { typeof(short), typeof(string) },
                            { typeof(int), typeof(string) },
                            { typeof(long), typeof(string) },
                            { typeof(ulong), typeof(string) },
                            { typeof(float), typeof(string) },
                            { typeof(double), typeof(string) },
                            { typeof(decimal), typeof(string) },
                            { typeof(bool), typeof(string) },
                            { typeof(char), typeof(string) },
                            { typeof(string), typeof(string) },
                        }},

                    };
            }
        }
    }
}
//}

