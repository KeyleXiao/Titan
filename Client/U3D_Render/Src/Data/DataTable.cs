//-----------------------------------------------------------------------------------------------
//class DataTable
//csv表格基类
//可以支持csv表格与Unity asset的读取转换
//
//想要利用一个csv表格存储数据，可以继承此类，比实现几个虚函数
//然后，调用ScpReader::ReadCSVFile(string filename, bool headerRow, DataTable datatable)读入
//-----------------------------------------------------------------------------------------------


using System;
using System.Text;
using UnityEngine;
using System.Collections.Generic;
using System.Reflection;
using System.IO;

public class DataTable
{        
    //SaveAction定义一个保存ScriptableObject的函数，负责将序列化好的data保存起来，例如EditorUtility.SetDirty(data);
    public delegate void SaveAction(ScriptableObject data);

    //GetObjectPath定义一个函数，负责获得GameObject的
    public delegate string GetObjectPath(GameObject obj);

    //这里定义两个分隔符，因为csv每个单元格是用逗号分隔的，所以单元格内的分隔符不能再用逗号
    //strSplitChar默认用来分隔Vector3
    //str2ndSplitChar在继承类中可以使用
    public const char separator = ';'; //字符串之间的分隔符
    public const char str2ndSplitChar = '/'; //字符串之间的2级分隔符

    //由csv文件读入各DataTable的继承类
    //需要在子类中实现    
    public virtual void AddRow() { }  //新增一行需要处理的事情
    public virtual void AddColumn(string name) { } //当csv表第一行为表头时，会触发此函数，逐列输入每一列的名字
    public virtual void AddItem(int column, string value) { } //每读入一个单元格触发一次，column表示读入第几列，value为单元格内容（字符串表示）
    

    //把path文件夹下的asset存入csv文件
    //需要在子类中实现
    public virtual bool asset2csv(string path, string csvFileName) { return true; }
    public virtual bool asset2csv(string path, string csvFileName, GetObjectPath getPath) { return true; }

    //把DataTable的继承类的内容存入assetPath文件夹
    //需要在子类中实现
    public virtual bool save2asset(string assetPath,SaveAction save) { return true; }

    
    //读入元素时类型转换
    //将某元素从str自动转换为data
    //可供子类中调用
    public virtual void ImportItem(FieldInfo field,string str,ref ScriptableObject data)
    {
        object cur_value = str;       

        try
        {
            cur_value = Convert.ChangeType(cur_value, field.FieldType);            
        }
        catch (System.Exception ex)
        {
            if (field.FieldType.BaseType == typeof(Enum))
            {
                int count = 0;
                foreach (var en in Enum.GetValues(field.FieldType))
                {
                    if (en.ToString() == str)
                    {
                        break;
                    }
                    count++;
                }
                cur_value = count;
            }
            else if (field.FieldType==typeof(UnityEngine.GameObject))
            {
                GameObject prefabs = Resources.Load(str) as GameObject;
                cur_value = prefabs;            
            }
            else if (field.FieldType==typeof(Vector3))
            {
                Vector3 vec=Vector3.zero;
                string vecstr = str;
                //去掉头尾括号
                vecstr=vecstr.Substring(0, vecstr.Length - 1);                
                vecstr=vecstr.Substring(1, vecstr.Length - 1);                
                string[] vecstrs = vecstr.Split(separator);
                if (vecstrs.Length == 3)
                {
                    for (int i = 0; i < 3; i++)
                    {                        
                        vec[i] = System.Single.Parse(vecstrs[i]);
                    }
                }
                cur_value = vec;
            }
            else
            {
                cur_value = null;
            }
        }
        finally
        {
            field.SetValue(data, cur_value);            
        }
    }

    //输出元素时类型转换
    //将某元素从data输入sw，如果附带了getPath函数，那么将通过函数获得data的路径，一并输入到sw中
    //可供子类中调用
    public void ExportItem(StreamWriter sw, MemberInfo info, object data, GetObjectPath getPath=null)
    {
        if (info.MemberType != MemberTypes.Field && info.MemberType != MemberTypes.Property)
        {
            return;
        }

        FieldInfo field = (FieldInfo)info;
        object cur_value = field.GetValue(data);

        if (cur_value != null && field.Attributes == FieldAttributes.Public)
        {
            sw.Write(info.Name + ',');
            //string strout = cur_value.ToString();
            if (field.FieldType == typeof(UnityEngine.GameObject))
            {
                GameObject preObj = (GameObject)cur_value;
                if (getPath != null)
                {
                    string prePath = getPath(preObj);
                    sw.Write(prePath + preObj.name);
                }
                else
                {
                    sw.Write(preObj.name);
                }
            }
            else if (field.FieldType == typeof(Vector3))
            {
                //自己定义了一种Vector3文本格式
                //因为csv是以逗号分格的，所以不能直接采用ToString()的"(x, y, z)"格式
                //另外，头尾加上括号，因为直接负数开头的话，excel里面会误解成公式
                Vector3 vec = (Vector3)cur_value;
                sw.Write("(" + vec.x.ToString() + separator + vec.y.ToString() + separator + vec.z.ToString() + ")");
            }
            else
            {
                sw.Write(cur_value.ToString());
            }
            sw.Write(',');
        }
    }
}