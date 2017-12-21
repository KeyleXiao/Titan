using System;
using System.Reflection;
using System.IO;
using System.Text;


using System.Globalization;
using Microsoft.CSharp;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Windows.Forms;

/// <summary>
/// Class1 的摘要说明
/// </summary>
public class ServiceBuilder
{
    static Dictionary<string, string> type_dic = new Dictionary<string, string>();

    public ServiceBuilder()
	{
	}

    static string read_template(string path)
    {
        string template_str = "";
        try
        {
            FileStream fs = new FileStream(path, FileMode.Open);

            StreamReader sr = new StreamReader(fs);
            template_str = sr.ReadToEnd();
            sr.Close();
            fs.Close();
        }
        catch (Exception ) { }

        return template_str;
    }

    static bool save_file(string path,string context)
    {
        try
        {
            FileStream fs = new FileStream(path, FileMode.Create);
            if (fs == null)
                return false;
            StreamWriter sr = new StreamWriter(fs);
            sr.Write(context);

            sr.Close();
            fs.Close();
        }catch (Exception) {
            return false;
        }
        return true;
    }

    static string get_c_name(string type_name)
    {
        if ( type_dic.Count==0 )
        {
            type_dic.Add("Boolean", "bool");
            type_dic.Add("Int32", "int");
            type_dic.Add("String", "string");
            type_dic.Add("Byte", "byte");
            type_dic.Add("Char", "char");
            type_dic.Add("Double", "double");
            type_dic.Add("Single", "float");
            type_dic.Add("UInt32", "DWORD");
            type_dic.Add("Int16", "short");
            type_dic.Add("Void", "void");

            // .. 可添加更多类型映射
        }

        if (type_dic.ContainsKey(type_name))
            return type_dic[type_name];
        
        return type_name;
    }

    public static void show_export_list()
    {
        FormCollection forms = Application.OpenForms;
        ServiceBuild.Program.maiFrame.UpdateListBox(Assembly.Load("CppHeadWrapper").GetTypes());
    }

    public static bool build(Type t,string path)
    {
        if (t == null)
            return false;

        string template_str = read_template(System.Windows.Forms.Application.StartupPath + "\\ServiceTemplateFrame.h");
        string service_name = t.Name;
        
        // 替换服务名
        template_str = template_str.Replace("%SERVICE_NAME%", service_name);

        // 替换内部段落
        for (int i = 1; i < 100; ++i)
        {
            string segment_template = read_template(System.Windows.Forms.Application.StartupPath + "\\Template_Segment_" + i + ".h");
            if (segment_template.Length < 1)
                break;

            string segments_str = "";

            Console.WriteLine("接口名称为： " + t.Name);
            MethodInfo[] mis = t.GetMethods();
            foreach (MethodInfo mi in mis)
            {
                // 编译器自动加的函数过滤掉
                if (mi.Name == "ToString" || mi.Name == "Equals" || mi.Name == "GetHashCode"
                    || mi.Name == "GetType")
                {
                    continue;
                }

                Console.WriteLine("接口函数为:" + mi.Name + " 返回:" + mi.ReturnType);

                string func_name = mi.Name;
                string return_type = get_c_name(mi.ReturnType.Name);
                
                string arg_delcare = "";
                string arg_list = "";

                ParameterInfo[] ps = mi.GetParameters();
                foreach (ParameterInfo p in ps)
                {
                    Console.WriteLine("       参数:" + p.Name + " 类型:" + p.ParameterType);

                    if (arg_delcare.Length != 0)
                    {
                        arg_delcare += "#";
                        arg_list += "#";
                    }

                    arg_delcare += get_c_name(p.ParameterType.Name) + " " + p.Name;
                    arg_list += p.Name;
                }

                // 如果有特例就读特例,没有就读标准模版
                string segment_str = read_template(System.Windows.Forms.Application.StartupPath + "\\" + return_type + "\\Template_Segment_" + i + ".h");
                if (segment_str.Length < 1)
                {
                    bool isconst = false;
                    object[] attris = mi.GetCustomAttributes(false);
                    foreach (object attr in attris)
                    {
                        if (attr.GetType().Name == "ReadOnlyAttribute")
                        {
                            isconst = true;
                            break;
                        }
                    }

                    if(isconst)
                    {
                        segment_str = read_template(System.Windows.Forms.Application.StartupPath + "\\Const\\Template_Segment_" + i + ".h");
                    }

                    if(segment_str.Length < 1)
                    {
                        segment_str = segment_template;
                    }
                }
                   

                segment_str = segment_str.Replace("%RETURN_TYPE%", return_type);
                segment_str = segment_str.Replace("%FUNC_NAME%", func_name);
                segment_str = segment_str.Replace("%ARG_DECLARE(,)%", arg_delcare.Replace("#", ","));
                segment_str = segment_str.Replace("%ARG_DECLARE(;)%", arg_delcare.Replace("#", ";"));
                segment_str = segment_str.Replace("%ARG_LIST(<<)%", arg_list.Replace("#", "<<"));
                segment_str = segment_str.Replace("%ARG_LIST(>>)%", arg_list.Replace("#", ">>"));
                segment_str = segment_str.Replace("%ARG_LIST(,)%", arg_list.Replace("#", ","));
                segment_str = segment_str.Replace("%NO_ARG%", arg_delcare.Length == 0 ? "//" : "");

                segments_str += "\r\n" + segment_str;
            }

            template_str = template_str.Replace("%SERVICE_SEGMENT_" + i + "%", segments_str);
        }

        string save_path = path/*System.Windows.Forms.Application.StartupPath*/ + "\\" + service_name + "_Stub.h";
        if ( !save_file(save_path, template_str) )
            return false;

        System.Diagnostics.Process.Start(save_path); 
        return true;
    }

    public static Type load_cpp_file(string path,string type_name)
    {
        string code_str = read_template(path);

        CompilerParameters cparams = new CompilerParameters();
        cparams.GenerateExecutable = true;
        String output = "F:\test.exe";
        cparams.OutputAssembly = output;
        cparams.CompilerOptions = "/optimize";
        //CppCodeProvider cpp = new CppCodeProvider();
        CodeDomProvider pro = CodeDomProvider.CreateProvider("cpp");
        cparams.ReferencedAssemblies.Add("System.dll");

        CompilerResults res = pro.CompileAssemblyFromSource(cparams, code_str);

        string result = ""; //final result that is displayed

        //parms.GenerateInMemory = true;

        if (res.Errors.Count > 0)
        {
            foreach (CompilerError err in res.Errors)
                result += "Line No.: " + err.Line + " Error No.: " + err.ErrorNumber + " - " + err.ErrorText + "\n";

            return null;
        }
        else
        {
            result = "Executable sucessfully created with file name : ";
            // 通过反射，获取类型
            Assembly objAssembly = res.CompiledAssembly;
            return objAssembly.GetType(type_name);
        }
    }

    public static Type load_csharp_file(string path,string type_name)
    {
        string code_str = read_template(path);

        // 1.CSharpCodePrivoder
        CSharpCodeProvider objCSharpCodePrivoder = new CSharpCodeProvider();

        // 2.ICodeComplier
        ICodeCompiler objICodeCompiler = objCSharpCodePrivoder.CreateCompiler();

        // 3.CompilerParameters
        CompilerParameters objCompilerParameters = new CompilerParameters();
        objCompilerParameters.ReferencedAssemblies.Add("System.dll");
        objCompilerParameters.GenerateExecutable = false;
        objCompilerParameters.GenerateInMemory = true;
        objCompilerParameters.CompilerOptions = "/unsafe";

        // 4.CompilerResults
        CompilerResults cr = objICodeCompiler.CompileAssemblyFromSource(objCompilerParameters, code_str);

        if (cr.Errors.HasErrors)
        {
            Console.WriteLine("编译错误：");
            foreach (CompilerError err in cr.Errors)
            {
                ServiceBuild.ServiceBuildForm.output(err.ErrorText +"line:"+ err.Line);
            }

            return null;
        }
        else
        {
            // 通过反射，获取类型
            Assembly objAssembly = cr.CompiledAssembly;
            return objAssembly.GetType(type_name);
        }
    }
}
