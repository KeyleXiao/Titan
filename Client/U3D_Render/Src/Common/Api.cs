using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;


public class Api
{
	/**
	System.DateTime.Now.Ticks
	每个计时周期表示一百纳秒，即一千万分之一秒。 1 毫秒内有 10,000 个计时周期。
	此属性的值表示自 0001 年 1 月 1 日午夜 12:00:00（表示 DateTime.MinValue）以来经过的以 100 纳秒为间隔的间隔数。 它不包括归因于闰秒的嘀嗒数。
	*/
	/** 获得系统Tick数( 一个 32 位带符号整数，它包含自上次启动计算机以来所经过的时间（以毫秒为单位）)
	@param   
	@param   
	@return 
	*/
	public static int GetTickCount()
	{
		return System.Environment.TickCount;
	}

	public static List<int> StringToInt(string strStringSource, char charSepartor)
	{

		if (strStringSource == "")
		{
			Debug.LogError("StringToInt:strStringSource==null ");
		}

		string[] myArray = new string[32]; //一维矩阵
		List<int> arr = new List<int>();

		try
		{
			myArray = strStringSource.Split(charSepartor);//截取              
		}
		catch (Exception ex)
		{
			Debug.LogError("clsArrayList(" + strStringSource + " , " + charSepartor + ")" + "" + ex.Message.ToString());
		}

		for (int i = 0; i < myArray.Length; i++)
		{
			try
			{
				arr.Add((int)System.Int32.Parse(myArray[i]));
			}
			catch (Exception e)
			{
				Debug.Log(e.Message);
			}

		}
		return arr;
	}

	public static List<List<int>> StringToIntList(string strStringSource, char charSepartor1, char charSepartor2)
	{

		List<List<int>> ParamList = new List<List<int>>();

		if (strStringSource == "")
		{
			Debug.LogError("StringToInt:strStringSource==null ");
			return ParamList;
		}
		string[] myArray1 = new string[32]; //二维矩阵
		string[] myArray2 = new string[32]; //二维矩阵

		try
		{
			myArray1 = strStringSource.Split(charSepartor1);//截取              
		}
		catch (Exception ex)
		{
			Debug.LogError("clsArrayList(" + strStringSource + " , " + charSepartor1 + ")" + "" + ex.Message.ToString());
		}

		for (int i = 0; i < myArray1.Length; i++)
		{
			try
			{
				myArray2 = myArray1[i].Split(charSepartor2);//截取              
			}
			catch (Exception ex)
			{
				Debug.LogError("clsArrayList(" + strStringSource + " , " + charSepartor2 + ")" + "" + ex.Message.ToString());
			}

			List<int> Param = new List<int>();

			for (int j = 0; j < myArray2.Length; j++)
			{
				Param.Add((int)System.Int32.Parse(myArray2[j]));
			}
			ParamList.Add(Param);
		}
		return ParamList;
	}

	public static List<List<float>> StringToFloatList(string strStringSource, char charSepartor1, char charSepartor2)
	{

		if (strStringSource == "")
		{
			Debug.LogError("StringToInt:strStringSource==null ");
		}

		string[] myArray1 = new string[32]; //二维矩阵
		string[] myArray2 = new string[32]; //二维矩阵

		List<List<float>> ParamList = new List<List<float>>();

		try
		{
			myArray1 = strStringSource.Split(charSepartor1);//截取              
		}
		catch (Exception ex)
		{
			Debug.LogError("clsArrayList(" + strStringSource + " , " + charSepartor1 + ")" + "" + ex.Message.ToString());
		}

		for (int i = 0; i < myArray1.Length; i++)
		{
			try
			{
				myArray2 = myArray1[i].Split(charSepartor2);//截取              
			}
			catch (Exception ex)
			{
				Debug.LogError("clsArrayList(" + strStringSource + " , " + charSepartor2 + ")" + "" + ex.Message.ToString());
			}

			List<float> Param = new List<float>();

			for (int j = 0; j < myArray2.Length; j++)
			{
				float fVaule = StrToFloat(myArray2[j]);
				Param.Add(fVaule);
			}
			ParamList.Add(Param);
		}
		return ParamList;
	}

	static float StrToFloat(object FloatString)
	{
		float result;
		if (FloatString != null)
		{
			if (float.TryParse(FloatString.ToString(), out result))
				return result;
			else
			{
				return (float)0.00;
			}
		}
		else
		{
			return (float)0.00;
		}
	}


	public static TimeSpan GetDataTime(UInt32 elapsedTicks)
	{
		DateTime currentDate = DateTime.Now;
		long lCurTicks = currentDate.Second - (long)elapsedTicks;
		TimeSpan timedata = TimeSpan.FromSeconds(lCurTicks);
		return timedata;
	}

	/// <summary>
	/// 尽量别使用，反射的消耗很大。复制组件，对原生的Unity组件无效，因为原生的Unity组件是native，无法使用反射
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dest"></param>
	public static void CopyComponentFields(UnityEngine.Object src, UnityEngine.Object dest)
	{
		if (src == null || dest == null)
		{
			return;
		}
		System.Type t1 = src.GetType();
		System.Type t2 = dest.GetType();

		if (t1.Name.Equals(t2.Name))
		{
			const string unityNativeComponentName = "UnityEngine";
			if (t1.FullName.Contains(unityNativeComponentName))
			{
				Debug.LogError("native Component 不能使用此函数，请遍历逐个变量复制,类型:" + t1.FullName);
				return;
			}

			foreach (System.Reflection.FieldInfo fi in t1.GetFields())
			{
				fi.SetValue(dest, fi.GetValue(src));
			}
		}



	}

	/// <summary>
	/// DateTime时间格式转换为Unix时间戳格式
	/// </summary>
	/// <param name="time"> DateTime时间格式</param>
	/// <returns>Unix时间戳格式</returns>
	public static int convertDateTime2TimeStamp(System.DateTime time)
	{
		DateTime endTime = time.ToLocalTime();
		DateTime startTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).ToLocalTime();
		return (int)(endTime - startTime).TotalSeconds;
	}

	/// <summary>
	/// 时间戳转为C#格式时间
	/// </summary>
	/// <param name="timeStamp">Unix时间戳格式</param>
	/// <returns>C#格式时间</returns>
	public static DateTime convertTimeStamp2DateTime(long timeStamp)
	{
		DateTime dtStart = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).ToLocalTime();

		long ticks = 10000000 * timeStamp;
		TimeSpan toNow = new TimeSpan(ticks);

		return dtStart.Add(toNow);
	}

	/// <summary>
	/// UTF8编码转换成GB2312
	/// </summary>
	/// <param name="strUTF8"></param>
	/// <returns></returns>
	public static string UTF8ToGB2312(string strUTF8)
	{
		byte[] byteUTF8 = Encoding.UTF8.GetBytes(strUTF8);
		byte[] byteGB = Encoding.Convert(Encoding.UTF8, Encoding.GetEncoding("GB2312"), byteUTF8);
		string strGB2312 = Encoding.GetEncoding("GB2312").GetString(byteGB);

		return strGB2312;
	}
}
