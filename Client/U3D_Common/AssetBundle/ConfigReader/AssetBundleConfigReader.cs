
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
    string[][] Array = null;
    string tag = "";
    int _IgnoreRow = 1;//忽略csv表头几行



    public AssetBundleConfigReader(string text, string tag, int ignoreRow = 1)
    {
        LoadFromString(text, ignoreRow);
    }


    /// <summary>
    /// 读表时字符串头有可能有$符号，有则移除
    /// </summary>
    private void RemoveFirstIgnoreString(ref string _stringNeedRemove)
    {
        int index = _stringNeedRemove.IndexOf("$");
        if (index == 0)
        {
            _stringNeedRemove = _stringNeedRemove.Replace("$", "");
        }
    }

    private void LoadFromString(string text, int ignoreRow = 1)
    {
        _IgnoreRow = ignoreRow;


        //读取每一行的内容
        string[] lineArray = text.Split('\r');


        //创建二维数组
        Array = new string[lineArray.Length][];

        //把csv中的数据储存在二位数组中
        List<int> EmptyIndex = new List<int>();
        for (int i = 0; i < lineArray.Length; i++)
        {

            Array[i] = lineArray[i].Split(',');

            if (Array[i].Length > 0)
            {
                string s = Array[i][0];

                if (s.Length > 0 && s[0].Equals( '\n'))
                {

                    s = s.Substring(1);

                }
                Array[i][0] = s;
                if (Array[i].Length == 1 && (string.IsNullOrEmpty(s)))
                {
                    EmptyIndex.Add(i);
                    continue;
                }
            }
        }


        //移除空得行
        if (EmptyIndex.Count > 0)
        {
            List<string[]> tmp = Array.ToList();
            
            for (int i = EmptyIndex.Count-1; i >=0 ; i--)
            {
                tmp.RemoveAt(EmptyIndex[i]);
            }
            Array = tmp.ToArray();
        }

    }

    public void Dispose()
    {
        Array = null;
    }

    public int GetRecordCount()
    {
        if (Array == null)
            return 0;
        return Array.GetLength(0) - _IgnoreRow;// 1;
    }

    public int GetRecordCount(int dwRow)
    {
        if (Array == null)
            return 0;

        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return 0;
        }
        return Array[dwRow + _IgnoreRow].Length;
    }

    public int GetFieldCount()
    {
        if (Array == null)
            return 0;
        if (Array.Length < _IgnoreRow)
            return 0;
        return Array[0].Length;
    }

    public string GetFieldDesc(int dwCol)
    {
        if (Array == null)
            return "";
        if (dwCol >= Array[0].Length)
        {
            return "";
        }
        return Array[0][dwCol];
    }

    public object GetObject(int dwRow, int dwCol)
    {
        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return null;
        }
        if (dwCol >= Array[dwRow + _IgnoreRow].Length)
        {
            return null;
        }

        string ob = Array[dwRow + _IgnoreRow][dwCol];
        if (ob != null)
        {
            object value;
            Type nValType = ConvertString(ob, out value);
            if (nValType == typeof(float)
                || nValType == typeof(double))
            {
                float result = Convert.ToSingle(ob.ToString());
                return result;
            }
            if (nValType == typeof(byte)
                || nValType == typeof(short)
                || nValType == typeof(int))
            {
                int result = Convert.ToInt32(ob.ToString());
                return result;
            }
            else
            {
                return ob;
            }
        }

        return null;
    }

    public float GetFloat(int dwRow, int dwCol, float fDef)
    {
        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return fDef;
        }
        if (dwCol >= Array[dwRow + _IgnoreRow].Length)
        {
            return fDef;
        }

        float result = fDef;

        string ob = Array[dwRow + _IgnoreRow][dwCol];
        if (ob != null)
        {
            bool br = float.TryParse(ob, out result);
            if (br == false)
                result = fDef;
            return result;
        }

        return fDef;
    }


    public int GetInt(int dwRow, int dwCol, int nDef)
    {
        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return nDef;
        }
        if (dwCol >= Array[dwRow + _IgnoreRow].Length)
        {
            return nDef;
        }

        int result = nDef;

        string ob = Array[dwRow + _IgnoreRow][dwCol];
        if (ob != null)
        {
            bool br = int.TryParse(ob, out result);
            if (br == false)
                result = nDef;
            return result;
        }

        return nDef;
    }

    public long GetLong(int dwRow, int dwCol, long nDef)
    {
        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return nDef;
        }
        if (dwCol >= Array[dwRow + _IgnoreRow].Length)
        {
            return nDef;
        }

        long result = nDef;

        string ob = Array[dwRow + _IgnoreRow][dwCol];
        if (ob != null)
        {
            bool br = long.TryParse(ob, out result);
            if (br == false)
                result = nDef;
            return result;
        }

        return nDef;
    }

    public string GetString(int dwRow, int dwCol, string strVal)
    {
        if (dwRow + _IgnoreRow >= Array.GetLength(0))
        {
            return strVal;
        }
        if (dwCol >= Array[dwRow + _IgnoreRow].Length)
        {
            return strVal;
        }

        string ob = Array[dwRow + _IgnoreRow][dwCol];
        RemoveFirstIgnoreString(ref ob);
        if (ob != null)
        {
            return ob;
        }
        return strVal;
    }
    /// <summary>
    /// 将从配置里读的字符串根据某种符合进行分割并转换为int类型存入list
    /// </summary>
    /// <param name="_strSource">要分割的字符串</param>
    /// <param name="_charSeparator">分割符号</param>
    /// <param name="_splitOptions">分割后是否移除空白项，默认移除</param>
    /// <returns>分割后存入的list</returns>
    public static List<int> ConvertStringToIntArray(string _strSource, char[] _charSeparator, StringSplitOptions _splitOptions = StringSplitOptions.RemoveEmptyEntries)
    {
        string[] strArr = _strSource.Split(_charSeparator, _splitOptions);
        List<int> intArr = new List<int>();
        for (int index = 0; index < strArr.Length; index++)
        {
			int result;
			if(Int32.TryParse(strArr[index], out result))
			{
				intArr.Add(result);
			}
			else
			{
				Debug.LogError("配置转换成int失败,请策划检查:" + _strSource + "!");
			}
        }
        return intArr;
    }

    /// <summary>
    /// 将从配置里读的字符串根据某种符合进行分割并转换为string类型存入list
    /// </summary>
    /// <param name="_strSource">要分割的字符串</param>
    /// <param name="_charSeparator">分割符号</param>
    /// <param name="_splitOptions">分割后是否移除空白项，默认移除</param>
    /// <returns>分割后存入的list</returns>
    public static List<string> ToStringList(string _strSource, char[] _charSeparator,StringSplitOptions _splitOptions = StringSplitOptions.RemoveEmptyEntries)
    {
        string[] strArr = _strSource.Split(_charSeparator, _splitOptions);
        List<string> intArr = new List<string>();
        for (int index = 0; index < strArr.Length; index++)
        {
            intArr.Add(strArr[index]);
        }
        return intArr;
    }

    public static float[] ToFloatArray(string _strSource, char[] _charSeparator, StringSplitOptions _splitOptions = StringSplitOptions.RemoveEmptyEntries)
    {
        string[] strArr = _strSource.Split(_charSeparator, _splitOptions);
        float[] floatArr = new float[strArr.Length];

        float result;
        for (int index = 0; index < strArr.Length; index++)
        {
            result = float.TryParse(strArr[index], out result) ? result : 0f;
            floatArr[index] = result;
        }
        return floatArr;
    }

    public static Vector2 ToVector2(string _strSource, char[] _charSeparator, StringSplitOptions _splitOptions = StringSplitOptions.RemoveEmptyEntries)
    {
        float[] floatArr = ToFloatArray(_strSource, _charSeparator, _splitOptions);
        if (floatArr.Length == 0)
            return Vector2.zero;
        else if (floatArr.Length == 1)
            return new Vector2(floatArr[0], 0f);
        return new Vector2(floatArr[0], floatArr[1]);
    }

    public static Vector3 ToVector3(string _strSource, char[] _charSeparator, StringSplitOptions _splitOptions = StringSplitOptions.RemoveEmptyEntries)
    {
        float[] floatArr = ToFloatArray(_strSource, _charSeparator, _splitOptions);
        if (floatArr.Length == 0)
            return Vector3.zero;
        else if (floatArr.Length == 1)
            return new Vector3(floatArr[0], 0f);
        else if (floatArr.Length == 2)
            return new Vector3(floatArr[0], floatArr[1]);        
        return new Vector3(floatArr[0], floatArr[1],floatArr[2]);
    }



    public static Encoding GetEncoding(string filePath)
    {
        if (filePath == null)
        {
            throw new ArgumentNullException("filePath");
        }
        Encoding encoding1 = Encoding.Default;
        if (File.Exists(filePath))
        {
            try
            {
                using (FileStream stream1 = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                {
                    if (stream1.Length > 0)
                    {
                        using (StreamReader reader1 = new StreamReader(stream1, true))
                        {
                            char[] chArray1 = new char[1];
                            reader1.Read(chArray1, 0, 1);
                            encoding1 = reader1.CurrentEncoding;
                            reader1.BaseStream.Position = 0;
                            if (encoding1 == Encoding.UTF8)
                            {
                                byte[] buffer1 = encoding1.GetPreamble();
                                if (stream1.Length >= buffer1.Length)
                                {
                                    byte[] buffer2 = new byte[buffer1.Length];
                                    stream1.Read(buffer2, 0, buffer2.Length);
                                    for (int num1 = 0; num1 < buffer2.Length; num1++)
                                    {
                                        if (buffer2[num1] != buffer1[num1])
                                        {
                                            encoding1 = Encoding.Default;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    encoding1 = Encoding.Default;
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception exception1)
            {
                throw;
            }
            if (encoding1 == null)
            {
                encoding1 = Encoding.UTF8;
            }
        }
        return encoding1;
    }
}
//}

