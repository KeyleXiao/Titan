using System.IO;
using System.Runtime.InteropServices;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows;
using System.Reflection;
using System.Collections;
using SpellEditor;

// DataTable序列化类
public static class DataTableSerializer
{

    public enum FileType
    {
        /// <summary>
        /// 无效的文件类型
        /// </summary>
        FILETYPE_INVALID,

        /// <summary>
        /// bin二进制文件
        /// </summary>
        FILETYPE_BIN,

        /// <summary>
        /// csv文件
        /// </summary>
        FILETYPE_CSV,

        FILETYPE_MAX
    }

    /// <summary>
    /// 文件导出返回值
    /// </summary>
    public enum FileExportReslut
    {
        /// <summary>
        /// 成功
        /// </summary>
        RESLUT_OK,

        /// <summary>
        /// 文件是空的
        /// </summary>
        RESLUT_EMPTYFILE,

        /// <summary>
        /// 发生错误
        /// </summary>
        RESLUT_ERROR,

        FILETYPE_MAX
    }

    /// <summary>
    /// 当前的文件类型，调用GetSaveFilePatch和GetOpenFilePatch会改变这个值，其他调用不会
    /// </summary>
    public static FileType CurrFileType
    {
        get
        {
            return m_CurrFileType;
        }
    }

    private const string DeleteButtonHeadrText = "删除";
    private static FileType m_CurrFileType = FileType.FILETYPE_INVALID;

    public static Dictionary<string, string> vocationNameDict = new Dictionary<string, string>();  //保存职业名称的Dictionary

    /// <summary>
    /// 将DataTable导出到CSV.
    /// </summary>
    /// <param name="table">DataTable</param>
    /// <param name="fullSavePath">保存路径</param>
    /// <param name="headerText">头两行文本,在追加模式下，这个参数被忽略</param>
    /// <param name="bAppend">是否追加</param>
    /// <returns>导出成功true;导出失败false</returns>
    public static bool SaveToCSV(this DataTable table, string fullSavePath,string[] headerText,bool bAppend = false)
    {
        if (!ArgumentChecked(table, fullSavePath))
            return false;

        StreamWriter _writerObj = null;
        try
        {

            //没有数据
            if (table.Rows.Count <= 0)
            {
                Trace.Warring(fullSavePath + "没有数据，不需要保存");
                return false;
            }

            string _bufferLine = "";
            using (_writerObj = new StreamWriter(fullSavePath, bAppend, Config.FileEncoding))
            {
                int i = 0;

                if (headerText != null && !bAppend)
                {
                    _writerObj.WriteLine(headerText[0]);
                    if(headerText.Length < 2)
                        _writerObj.WriteLine("");
                    _writerObj.WriteLine(headerText[1]);
                }

                //保存数据
                for ( i = 0; i < table.Rows.Count; i++)
                {
                    _bufferLine = "";
                    for (int j = 0; j < table.Columns.Count; j++)
                    {
                        string s = "";
                        s = table.Rows[i][j].ToString();
                        if (string.IsNullOrEmpty(s))
                        {
                            Trace.Warring("数据 [" + i.ToString() + "行，" + j.ToString() + "列]为空！");
                            ////删除文件
                            //_writerObj.Close();
                            //if (File.Exists(fullSavePath))
                            //    File.Delete(fullSavePath);
                            //return false;                            
                        }
                        if (-1 != s.IndexOf(','))
                        {
                            s = s.Replace(',', '，');
                        }
                        if (j > 0)
                            _bufferLine += ",";
                        //枚举类型，特殊处理
                        if (IsEnum(table.Columns[j].DataType))
                        {
                            int eumName = (int)table.Rows[i][j];
                            _bufferLine += Enum.Parse(table.Columns[j].DataType, eumName.ToString()).ToString();
                        }
                        else
                        {
                            _bufferLine += table.Rows[i][j].ToString();
                        }
                        
                    }
                    _writerObj.WriteLine(_bufferLine);
                }
                _writerObj.Flush();
                _writerObj.Close();
                return true;
            }
        }
        catch (Exception e)
        {
            Trace.Error(e.Message);
            //删除文件
            _writerObj.Close();
            if (File.Exists(fullSavePath))
                File.Delete(fullSavePath);
            return false;
        }
    }


    /// <summary>
    /// 参数检查
    /// </summary>
    /// <param name="table"></param>
    /// <param name="fullSavePath"></param>
    private static bool ArgumentChecked(DataTable table, string fullSavePath)
    {
        if (table == null)
        {
            Trace.Error("table为空！");
            return false;
        }
        if (string.IsNullOrEmpty(fullSavePath))
        {
            Trace.Error("路径为空！");
            return false;
        }
        return true;
    }

    public static bool LoadVocationCSV(string filePath, int rowIndex = 2)
    {
        if (string.IsNullOrEmpty(filePath))
        {
            Trace.Error("路径为空！");
            return false;
        }
        if (!File.Exists(filePath))
        {
            Trace.Error("文件不存在：" + filePath);
            return false;
        }
        try
        {
            using (StreamReader reader = new StreamReader(filePath, Config.FileEncoding, false))
            {
                reader.ReadLine();
                reader.ReadLine();
                while (reader.Peek() > 0)
                {
                    string _line = reader.ReadLine();

                    string[] _split = _line.Split(',');
                    VOCATION v = (VOCATION)int.Parse(_split[0]);
                    string key = v.ToString();
                    string name = _split[5];
                    vocationNameDict.Add(key, name);
                }
                vocationNameDict.Add(VOCATION.VOCATION_INVALID.ToString(),"通用技能");
                vocationNameDict.Add(VOCATION.VOCATION_RANDOM.ToString(), "随机");
                vocationNameDict[VOCATION.VOCATION_MAX.ToString()] = "最大职业";

                reader.Close();
                return true;
            }
        }
        catch (ArgumentException e)
        {
            string erroMsg;
            //erroMsg = "[" + i.ToString() + "列," + j.ToString() + "行]参数错误,";
            erroMsg = e.Message;
            Trace.Error(erroMsg);
            return false;
        }
    }


    /// <summary>
    /// 将CSV文件数据导入到DataTable中
    /// </summary>
    /// <param name="table">需要填充的DataTable</param>
    /// <param name="filePath">路径</param>
    /// <param name="rowIndex">行索引</param>
    /// <param name="DataKeyFromHeadrText">从头中取得填充数据的Key</param>
    /// <param name="DataKeyText">Key字符串数组</param>
    /// <returns></returns>
    public static bool LoadCSVToTable(ref DataTable table, string filePath,int rowIndex = 2,bool bDataKeyFromText = false,string[] DataKeyText = null)
    {
        if (!ArgumentChecked(table, filePath))
            return false;
        if (rowIndex < 0)
        {
            Trace.Error("DataTableSerializer.LoadCSV，RowIndex参数小于0！");
            return false;
        }
        if (!File.Exists(filePath))
        {
            Trace.Error("文件不存在：" + filePath);
            return false;
        }
        table.Clear();
        int i = 0, j = 0;
        try
        {
            //外部提供的Key
            List<string> DestDataKey = null;
            //CSV中的Key，保存在文件的第二行
            List<string> SrcDataKey = null;

            using (StreamReader reader = new StreamReader(filePath, Config.FileEncoding, false))
            {
                reader.Peek();
                while (reader.Peek() > 0)
                {
                    j = j + 1;
                    string _line = reader.ReadLine();

                    string[] _split = _line.Split(',');

                    //Key的索引比Row的索引大的话，是不合法的值
                    if (null == DataKeyText)
                    {
                        bDataKeyFromText = false;
                    }
                    
                    if (bDataKeyFromText && j == 2)
                    {
                        DestDataKey = new List<string>();
                        SrcDataKey = new List<string>();
                        DestDataKey.AddRange(DataKeyText);
                        SrcDataKey.AddRange(_split);
                    }

                    if (j >= rowIndex + 1)
                    {
                        if (table.Columns.Count != _split.Length)
                        {
                            Trace.Warring("文件:" + filePath + "，第" + (j).ToString() + "行格式与所需要的格式不一致，本行只有" +
                                (_split.Length).ToString() + "列，所需要的格式为:" + table.Columns.Count + "列，已填充默认值，请注意检查！");
                            //table.Rows.Clear();
                            //return false;
                        }
                        DataRow _row = FillDefaultNewRow(ref table,false);
                        int srcIndex = 0;

                        for (i = 0; i < table.Columns.Count; i++)
                        {
                            if (srcIndex > _split.Length - 1)
                            {
                                break;
                            }
                            if (string.IsNullOrEmpty(_split[srcIndex]))
                            {
                                Trace.Error("文件：" + filePath + ",[" + j.ToString() + "行，" + i.ToString() + "列]为空！");
                                table.Rows.Clear();
                                return false;
                            }
                            
                            if (bDataKeyFromText)
                            {
                                string CurrentKey = table.Columns[i].ColumnName;
                                //双方都有这个key，才填充这个数据
                                if (DestDataKey.Contains(CurrentKey) && SrcDataKey.Contains(CurrentKey))
                                {
                                    //枚举类型，特殊处理
                                    if (IsEnum(table.Columns[i].DataType))
                                    {

                                        _row[i] = Enum.Parse(table.Columns[i].DataType, _split[i]);
                                    }
                                    else
                                    {
                                        _row[i] = _split[SrcDataKey.IndexOf(CurrentKey)];
                                    }
                                    
                                    srcIndex++;
                                }
                                else
                                {
                                    Trace.Normal(CurrentKey + ",在文件中没有数据，属于被删除或是新增数据。");
                                }
                            }
                            else
                            {
                                //枚举类型，特殊处理
                                if (IsEnum(table.Columns[i].DataType))
                                {

                                    _row[i] = Enum.Parse(table.Columns[i].DataType, _split[i]);
                                }
                                else
                                {
                                    _row[i] = _split[i];
                                }

                                
                            }
                        }
                        table.Rows.Add(_row);
                    }
                }
                
                reader.Close();
                return true;
            }
        }
        catch (ArgumentException e)
        {
            string erroMsg;
            erroMsg = "[" + i.ToString() + "列," + j.ToString() + "行]参数错误,";
            erroMsg += e.Message;
            Trace.Error(erroMsg);
            return false;
        }

    }



    /// <summary>
    /// 将DataTable导出到BIN.
    /// </summary>
    /// <param name="table">数据表</param>
    /// <param name="fullSavePath">保存路径</param>
    /// <param name="ItemTotal">数量</param>
    /// <param name="structType">结构类型</param>
    /// <param name="structType">打开类型</param>
    /// <returns></returns>
    public static bool SaveToBin( DataTable table, string fullSavePath,Type structType,FileMode openMode = FileMode.OpenOrCreate)
    {
        if (!ArgumentChecked(table, fullSavePath))
            return false;
        FileStream _writerObj = null;
        try
        {
            //没有数据
            if (table.Rows.Count <= 0)
            {
                Trace.Warring(fullSavePath + "没有数据，不需要保存");
                return false;
            }

            using (_writerObj = new FileStream(fullSavePath, openMode, FileAccess.Write))
            {
                BIN_FILE_HEADER header = new BIN_FILE_HEADER();
                header.szMagicFlag = "BIN";
                header.szTypeName = structType.Name;
                header.nTypeSize = Marshal.SizeOf(structType);
                header.nItemTotal = table.Rows.Count;
                WriteStruct(ref header, _writerObj);
                FieldInfo[] fields = structType.GetFields();

                // 逐个输出
                for (int i = 0; i < table.Rows.Count; i++)
                {
                    Object obj = Activator.CreateInstance(structType); 
                    
                    foreach (var field in fields)
                    {
                        //枚举类型要特殊处理
                        if (IsEnum(field.FieldType))
                        {
                            Object o = Enum.ToObject(field.FieldType, table.Rows[i][field.Name]);
                            field.SetValue(obj, o);
                        }
                        else
                        {
                            object val = Convert.ChangeType(table.Rows[i][field.Name], field.FieldType);
                            if (field.FieldType.FullName == "System.String")
                            {
                                Object[] attrs = field.GetCustomAttributes(false);
                                if (attrs[0].GetType() == typeof(MarshalAsAttribute))
                                {
                                    MarshalAsAttribute attr = (MarshalAsAttribute)(attrs[0]);
                                    int sizeConst = attr.SizeConst;
                                    if (sizeConst < val.ToString().Length)
                                    {
                                        Trace.Error("字符串长度过长！超出了当前字符串最大长度，请联系程序扩充！： \n 类型： " + header.szTypeName + "\n 字符串： " + val.ToString());
                                        return false;
                                    }
                                }
                            }
                            field.SetValue(obj, val);
                        }
                    }

                    WriteStruct(ref obj, _writerObj);
                }
                return true;
            }
        }
        catch (Exception e)
        {
            Trace.Error(e.Message);
            //删除文件
            _writerObj.Close();
            if (File.Exists(fullSavePath))
                File.Delete(fullSavePath);
            return false;
        }
    }



    /// <summary>
    /// 将DataTable加载到Bin.
    /// </summary>
    /// <param name="table">DataTable</param>
    /// <param name="fullSavePath">保存路径</param>
    /// <param name="structType">结构体类型</param>
    /// <returns>导出成功true;导出失败false</returns>
    public static bool LoadBinToTable(ref DataTable table, string fullSavePath, Type structType)
    {
        if (!ArgumentChecked(table, fullSavePath))
            return false;
        table.Clear();
        FileStream _readObj = null;
        try
        {
            using (_readObj = new FileStream(fullSavePath, FileMode.Open, FileAccess.Read))
            {
                BIN_FILE_HEADER header = new BIN_FILE_HEADER();
                ReadStruct(ref header, _readObj);

                if (!header.szMagicFlag.Equals("BIN"))
                {
                    Trace.Error("bin文件的szMagicFlag不一致！：[" + header.szMagicFlag + "," + "BIN" + "]");
                    return false;
                }

                if (!header.szTypeName.Equals(structType.Name))
                {
                    Trace.Error("bin文件的szTypeName不一致！：[" + header.szTypeName + "," + structType.Name + "]");
                    return false;
                }

                if (header.nTypeSize != Marshal.SizeOf(structType))
                {
                    Trace.Error("bin文件的nTypeSize不一致！：[" + header.nTypeSize + "," + Marshal.SizeOf(structType) + "]" );
                    return false;
                }


                FieldInfo[] fields = structType.GetFields();
                // 逐个输入
                for (int i = 0; i < header.nItemTotal; i++)
                {
                    Object structTypeInstance = Activator.CreateInstance(structType);
                    ReadStructFromType(ref structTypeInstance, _readObj, structType);
                    DataRow _row = table.NewRow();
                    int rowNum = 0;
                    for(int j = 0;j < fields.Length;j++)
                    {
                        _row[rowNum] = fields[j].GetValue(structTypeInstance);
                        rowNum++;
                        if (rowNum == 2)    //跳过备注这一行
                            rowNum++;
                    }
                    table.Rows.Add(_row);
                }
                
                return true;
            }
        }
        catch (Exception e)
        {
            Trace.Error(e.Message);
            return false;
        }
    }


    /// <summary>
    /// 从流中读取struct的通用方法
    /// </summary>
    /// <param name="_struct">struct类型</param>
    /// <param name="stream">流</param>
    /// <returns>字节读取数</returns>
    public static int ReadStruct<T>(ref T _struct, Stream stream)
    {
          int size = Marshal.SizeOf(_struct);
          byte[] buffer = new byte[size];
          size = stream.Read(buffer, 0, size); //已读取字节，用size少用一个变量
          GCHandle pinned = GCHandle.Alloc(buffer, GCHandleType.Pinned);
          _struct = (T)Marshal.PtrToStructure(pinned.AddrOfPinnedObject(), typeof(T));
          pinned.Free();
          return size;
    }



    /// <summary>
    /// 通过Type，从流中读取Struct的通用方法
    /// </summary>
    /// <param name="_struct"></param>
    /// <param name="stream"></param>
    /// <param name="StructType"></param>
    /// <returns></returns>
    public static int ReadStructFromType(ref object _struct, Stream stream,Type StructType)
    {
        int size = Marshal.SizeOf(_struct);
        byte[] buffer = new byte[size];
        size = stream.Read(buffer, 0, size); //已读取字节，用size少用一个变量
        GCHandle pinned = GCHandle.Alloc(buffer, GCHandleType.Pinned);
        _struct = Marshal.PtrToStructure(pinned.AddrOfPinnedObject(), StructType);
        pinned.Free();
        return size;
    }

    /// <summary>
    /// 将struct写入到流中得通用方法
    /// </summary>
    /// <param name="_struct">struct类型</param>
    /// <param name="stream">流</param>
    /// <returns>字节读取数</returns>
    public static int WriteStruct<T>(ref T _struct, Stream stream)
    {
        int size = Marshal.SizeOf(_struct);
        byte[] buffer = new byte[size];
        
        IntPtr bufferIntPtr = Marshal.AllocHGlobal(size);

        try
        {
            Marshal.StructureToPtr(_struct, bufferIntPtr, true);
            Marshal.Copy(bufferIntPtr, buffer, 0, size);
        }
        finally  
        {
            Marshal.FreeHGlobal(bufferIntPtr);  
        }

        stream.Write(buffer, 0, size);
        return size;
    }


    /// <summary>
    /// 通过文件夹选择面板取得保存路径
    /// </summary>
    /// <returns></returns>
    public static string OpenFolderBrowserDialog()
    {
        string localFilePath = "";

        FolderBrowserDialog BrowDialog = new FolderBrowserDialog();
        BrowDialog.ShowNewFolderButton = true;
        BrowDialog.Description = "请选择位置";
        if (BrowDialog.ShowDialog() == DialogResult.OK)
        {
            localFilePath = BrowDialog.SelectedPath;
            if (localFilePath[localFilePath.Length - 1] != '\\')
                localFilePath += '\\';
        }

        return localFilePath;
    }



    /// <summary>
    /// 设置文件类型
    /// </summary>
    /// <param name="typeName"></param>
    public static void SetFileType(FileType typeName)
    {
        m_CurrFileType = typeName;
    }


    /// <summary>
    /// 判断Type是不是枚举
    /// </summary>
    /// <param name="t">类型</param>
    /// <returns></returns>
    public static bool IsEnum(Type t)
    {
        string baseType = t.BaseType.ToString();
        baseType = baseType.ToLower();

        return baseType.Contains("enum");

    }

    public static bool IsVocationEnum(Type t)
    {
        string strtype = t.ToString();
        strtype = strtype.ToLower();

        return strtype.Contains("vocation");

    }

    /// <summary>
    /// 判断Type是不是DataGridViewButton
    /// </summary>
    /// <param name="t">类型</param>
    /// <returns></returns>
    public static bool IsDataGridViewButton(Type t)
    {
        string baseType = t.ToString();
        baseType = baseType.ToLower();

        return baseType.Contains("datagridviewbuttoncolumn");

    }


    /// <summary>
    /// 判断Type是不是字符串
    /// </summary>
    /// <param name="t">类型</param>
    /// <returns></returns>
    public static bool IsString(Type t)
    {
        string baseType = t.ToString().ToLower();
        return baseType.Contains("string");

    }

    /// <summary>
    /// 判断Type是不是DBNull，DBNull表示这个单元格没有数据
    /// </summary>
    /// <param name="t">类型</param>
    /// <returns></returns>
    public static bool IsDBNull(Type t)
    {
        string baseType = t.ToString().ToLower();
        return baseType.Contains("dbnull");

    }


    /// <summary>
    /// 枚举名和值转换到DataTable
    /// </summary>
    /// <param name="enumType">枚举实例</param>
    /// <param name="key">名</param>
    /// <param name="val">值</param>
    /// <returns>DataTable</returns>
    public static DataTable EnumToDataTable(Type enumType, string key, string val,bool isVacation = false)
    {

        // 获取所有枚举的值
        Array values = Enum.GetValues(enumType);
        string[] names = Enum.GetNames(enumType);

        // 载入事件类型中文描述
        FieldInfo[] enumfields = enumType.GetFields();

        for (int i = 0; i < names.Length; i++)
        {
            FieldInfo f = enumType.GetField(names[i]);
            //if (f.Name.Contains("MAX"))
            //{
            //    names[i] = null;
            //}
            //else
            if (!isVacation)
            {
                names[i] = WrapperReflection.getFieldDesc(f);
            }
            else
            {
                names[i] = vocationNameDict[names[i]];
            }

        } 
        DataTable dict = new DataTable();
        dict.Columns.Add(new DataColumn(key, typeof(string)));
        dict.Columns.Add(new DataColumn(val, typeof(int)));

        for (int i = 0; i < values.Length; i++)
        {
            string s = names[i];
            if (!string.IsNullOrEmpty(s))
            {
                DataRow dr = dict.NewRow();
                dr[key] = s;
                dr[val] = ((int)values.GetValue(i));
                dict.Rows.Add(dr);
            }

        }
        return dict;
    }


    /// <summary>
    /// 通过DataTable来构建DataGridView
    /// </summary>
    /// <param name="grid"></param>
    /// <param name="ta"></param>
    /// <param name="structType">这个dataTable所对应的结构体类型</param>
    /// <param name="bOpenDataCheck">是否开启数据验证</param>
    public static void BuildDataGridViewFromDataTable(ref DataGridView grid, DataTable ta, Type structType = null, bool bOpenDataCheck = true)
    {
        if (null == grid)
            return;
        if (null == ta)
            return;
        if (ta.Columns.Count <= 0)
            return;

        //是否拥有tooltips
        bool bHaveToolTips = true;
        FieldInfo[] fields = null;

        if (null != structType)
        {
            fields = structType.GetFields();
            if (fields.Length != ta.Columns.Count)
            {
                bHaveToolTips = false;
            }
        }
        else
        {
            bHaveToolTips = false;
        }

        if (grid.Columns.Count > 0)
            grid.Columns.Clear();
        if (grid.Rows.Count > 0)
            grid.Rows.Clear();

        //不让他自己生成列，改用手动生成
        grid.AutoGenerateColumns = false;
        grid.EditMode = DataGridViewEditMode.EditOnEnter;
        grid.MultiSelect = false;
        //grid.AllowUserToAddRows = false;
        for (int i = 0; i < ta.Columns.Count; i++)
        {
            DataColumn colunm = ta.Columns[i];
            Type FiledType = colunm.DataType;
            //枚举类型特殊处理,生成下拉框
            if (DataTableSerializer.IsEnum(FiledType))
            {
                bool isVocation = IsVocationEnum(FiledType);
                DataGridViewComboBoxColumn col = new DataGridViewComboBoxColumn();
                col.DataSource = DataTableSerializer.EnumToDataTable(FiledType, "key", "value", isVocation);
                col.DisplayMember = "key";
                col.ValueMember = "value";
                col.HeaderText = colunm.Caption;
                //属性绑定使用它的列名
                col.DataPropertyName = colunm.ColumnName;
                col.DefaultCellStyle.NullValue = "请选择";

                if (bHaveToolTips)
                {
                    col.ToolTipText = WrapperReflection.getFieldDesc(fields[i]);
                }
                grid.Columns.Add(col);
            }
            else
            {
                DataGridViewTextBoxColumn cel = new DataGridViewTextBoxColumn();
                cel.HeaderText = colunm.Caption;
                //属性绑定使用它的列名
                cel.DataPropertyName = colunm.ColumnName;
                cel.ValueType = FiledType;
                if (bHaveToolTips)
                {
                    cel.ToolTipText = WrapperReflection.getFieldDesc(fields[i]);
                }
                grid.Columns.Add(cel);
            }
        }

        AddButtonToDataGridView(ref grid, DeleteButtonHeadrText, DeleteButtonHeadrText, DeleteButtonCallBack);
        if (bOpenDataCheck)
        {
            grid.CellValidating -= grid_CellValidating;
            grid.CellValidating += grid_CellValidating;
        }
        
    }

    /// <summary>
    /// 添加一个按钮到DataGridView
    /// </summary>
    /// <param name="grid"></param>
    /// <param name="headerText">按钮标题</param>
    /// <param name="text">默认文本</param>
    /// <param name="callBack">回调</param>
    /// <param name="displayIndex">显示顺序</param>
    /// <param name="UseColumnTextForButtonValue">text参数是否作为按钮文本的值</param>
    public static void AddButtonToDataGridView(ref DataGridView grid, string headerText, string text, DataGridViewCellMouseEventHandler callBack,int displayIndex = 0, bool UseColumnTextForButtonValue = true)
    {
        //删除按钮列
        DataGridViewButtonColumn ButtonCoumn = new DataGridViewButtonColumn();
        ButtonCoumn.HeaderText = headerText;
        ButtonCoumn.Text = text;
        ButtonCoumn.AutoSizeMode = DataGridViewAutoSizeColumnMode.ColumnHeader;
        ButtonCoumn.DisplayIndex = displayIndex;
        ButtonCoumn.UseColumnTextForButtonValue = UseColumnTextForButtonValue;

        grid.CellMouseUp -= callBack;
        grid.CellMouseUp += callBack;
        grid.Columns.Add(ButtonCoumn);
    }


    /// <summary>
    /// 通过DataTable来填充DataGridView
    /// </summary>
    /// <param name="grid"></param>
    /// <param name="ta"></param>
    public static void FillDataGridViewFromDataTable(ref DataGridView grid, DataTable ta)
    {
        if (null == grid)
            return;
        if (null == ta)
            return;
        if (ta.Columns.Count <= 0)
            return;
        //同一个无需填充了。
        if (grid.DataSource == ta)
            return;

        
        grid.DataSource = ta;

        ////填充枚举值
        //for (int i = 0; i < ta.Columns.Count; i++)
        //{
        //    DataColumn col = ta.Columns[i];
        //    if (IsEnum(col.DataType))
        //    {
        //        for (int j = 0; j < ta.Rows.Count; j++)
        //        {
        //            grid[i, j].Value = ta.Rows[j][i];
        //        }
        //    }
        //}
    }


    /// <summary>
    /// 填充一行默认值，返回被填充的行,会触发Table的RowChanged以、RowChanging、TableNewRow事件
    /// </summary>
    /// <param name="ta"></param>
    /// <param name="bIngoreFirstCol">是否忽略第一列，第一列通常是ID，自动填充的，如果想手动填，设false，不过能会和其他ID发生冲突</param>
    /// <param name="StrDefalutValue">字符串默认值</param>
    /// <param name="iDefaultValue">整形默认值</param>
    /// <returns>返回被填充的行</returns>
    public static  DataRow FillDefaultNewRow(ref DataTable ta, bool bIngoreFirstCol = true,string StrDefalutValue = "empty",int iDefaultValue = 0)
    {
        if (null == ta)
        {
            return null;
        }

        DataRow dt = ta.NewRow();
        int startIndex = 0;
        //是否跳过第一列
        if (bIngoreFirstCol)
        {
            startIndex = 1;
        }
        //if (ta.Rows.Count <= 0)
        {
            for (int i = startIndex; i < ta.Columns.Count; i++)
            {
                if (DataTableSerializer.IsEnum(ta.Columns[i].DataType))
                {
                    dt[i] = Enum.ToObject(ta.Columns[i].DataType, Enum.GetValues(ta.Columns[i].DataType).GetValue(0));
                }
                else if (DataTableSerializer.IsString(ta.Columns[i].DataType))
                {
                    dt[i] = StrDefalutValue;
                }
                else
                {
                    dt[i] = iDefaultValue;
                }
            }
        }

        return dt;
    }

    /// <summary>
    /// 填充一行默认值,会触发Table的RowChanged、RowChanging事件
    /// </summary>
    /// <param name="ta"></param>
    /// <param name="host"> 所属的table</param>
    /// <param name="bOnlyFillDBNull"> 是否只填充没有数据的成员</param>
    /// <param name="bIngoreFirstCol">是否忽略第一列，第一列通常是ID，自动填充的，如果想手动填，设false，不过能会和其他ID发生冲突</param>
    /// <param name="StrDefalutValue">字符串默认值</param>
    /// <param name="iDefaultValue">整形默认值</param>
    /// <returns>返回被填充的行</returns>
    public static void FillDefaultRow(ref DataRow row, DataTable host,bool bOnlyFillDBNull = true,bool bIngoreFirstCol = true, string StrDefalutValue = "empty", int iDefaultValue = 0)
    {
        if (null == host)
        {
            return ;
        }
        if (null == row)
        {
            return;
        }

        int startIndex = 0;
        //是否跳过第一列
        if (bIngoreFirstCol)
        {
            startIndex = 1;
        }

        for (int i = startIndex; i < host.Columns.Count; i++)
        {
            //是按钮，跳过
            if (DataTableSerializer.IsDataGridViewButton(host.Columns[i].GetType()))
            {
                continue;
            }
            if (DataTableSerializer.IsEnum(host.Columns[i].DataType))
            {
                if (IsDBNull(row[i].GetType()) || !bOnlyFillDBNull)
                    row[i] = Enum.ToObject(host.Columns[i].DataType, Enum.GetValues(host.Columns[i].DataType).GetValue(0));
            }
            else if (DataTableSerializer.IsString(host.Columns[i].DataType))
            {
                if (IsDBNull(row[i].GetType()) || !bOnlyFillDBNull)
                    row[i] = StrDefalutValue;
            }
            else
            {
                if (IsDBNull(row[i].GetType()) || !bOnlyFillDBNull)
                    row[i] = iDefaultValue;
            }
        }

    }


    /// <summary>
    /// 将DataTable复制多一行，返回被复制的行，会触发Table的RowChanged以、RowChanging、TableNewRow事件
    /// </summary>
    /// <param name="ta"></param>
    /// <param name="bIngoreFirstCol">是否忽略第一列，第一列通常是ID，自动填充的，如果想手动填，设false，不过能会和其他ID发生冲突</param>
    /// <returns></returns>
    public static DataRow CopySelfRow(ref DataTable ta, bool bIngoreFirstCol = true)
    {
        if (null == ta)
        {
            return null;
        }
        if (ta.Rows.Count <= 0)
        {
            return FillDefaultNewRow(ref ta);
        }
        DataRow dt = ta.NewRow();
        DataRow src = ta.Rows[ta.Rows.Count - 1];
        int startIndex = 0;
        if (bIngoreFirstCol)
        {
            startIndex = 1;
        }
        for (int i = startIndex; i < ta.Columns.Count; i++)
        {
            dt[i] = src[i];
        }
        return dt;
    }

    /// <summary>
    /// 复制DataTable的指定行，返回被复制的行，会触发Table的RowChanged以、RowChanging、TableNewRow事件
    /// </summary>
    /// <param name="ta"></param>
    /// <param name="bIngoreFirstCol">是否忽略第一列，第一列通常是ID，自动填充的，如果想手动填，设false，不过能会和其他ID发生冲突</param>
    /// <returns></returns>
    public static DataRow CopyRowAt(ref DataTable ta, int rowID,bool bIngoreFirstCol = true)
    {
        if (null == ta)
        {
            return null;
        }
        if (ta.Rows.Count <= 0)
        {
            return FillDefaultNewRow(ref ta);
        }
        DataRow dt = ta.NewRow();
        DataRow src = ta.Rows[rowID];
        int id = (int)src[0];
        int startIndex = 0;
        if (bIngoreFirstCol)
        {
            startIndex = 1;
        }
        for (int i = startIndex; i < ta.Columns.Count; i++)
        {
            dt[i] = src[i];
        }
        return dt;
    }

    /// <summary>
    /// 单元格数据验证
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private static void grid_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
    {
        
        DataGridView dv = (DataGridView)sender;

        DataGridViewCell dc = dv[e.ColumnIndex, e.RowIndex];
        DataTable dt = (DataTable)dv.DataSource;
        string s = e.FormattedValue.ToString();

        if (e.ColumnIndex > dt.Columns.Count  || e.RowIndex > dt.Rows.Count )
        {
            return;
        }
        if (string.IsNullOrEmpty(s))
        {
            dv.Rows[e.RowIndex].ErrorText = null;
            return;
        }
        //是按钮，跳过
        if (DataTableSerializer.IsDataGridViewButton(dv.Columns[e.ColumnIndex].GetType()))
        {
            return;
        }

        try
        {
            //特殊处理枚举
            if (DataTableSerializer.IsEnum(dt.Columns[e.ColumnIndex].DataType))
            {
                Type enumType = dt.Columns[e.ColumnIndex].DataType;
                string[] enumNames = Enum.GetNames(enumType);
                // 载入事件类型中文描述
                FieldInfo[] enumfields = enumType.GetFields();
                for (int i = 1; i < enumfields.Length; i++)
                {
                    FieldInfo f = enumfields[i];
                    enumNames[enumfields.Length - i - 1] = WrapperReflection.getFieldDesc(f);
                } 
                if (!enumNames.Contains(s) && enumType!=typeof(VOCATION))   //职业类型不做改检测，因为DESC的中文跟配置的已经不同了。。
                {
                    Trace.Error("枚举值无效!");
                    e.Cancel = true;
                }
            }
            else
            {
                Convert.ChangeType(s, dt.Columns[e.ColumnIndex].DataType);
            }
            dv.Rows[e.RowIndex].ErrorText = null;
        }
        catch (Exception)
        {
            Trace.Error("输入不合法！当前单元格类型需要：" + dc.ValueType.ToString() + "，输入值：" + e.FormattedValue.ToString() + 
                ",请重新输入!");

            dv.Rows[e.RowIndex].ErrorText = "第" + e.ColumnIndex.ToString() +"个输入不合法，类型为:" + dc.ValueType.ToString();
            e.Cancel = true;

        }
    }


    /// <summary>
    /// 删除按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private static void DeleteButtonCallBack(object sender, DataGridViewCellMouseEventArgs e)
    {
        DataGridView grid = (DataGridView)sender;

        DataTable ta = (DataTable)grid.DataSource;
        if (ta.Rows.Count > 0 && e.ColumnIndex >= 0 && e.RowIndex >= 0 && ta.Rows.Count > e.RowIndex)
        {
            if (grid.Columns[e.ColumnIndex].HeaderText.Equals(DeleteButtonHeadrText))
            {
                ta.Rows.RemoveAt(e.RowIndex);
                //移除最后一行的时候需要刷新整个列表，要不然会显示不对
                if (e.RowIndex == ta.Rows.Count)
                {
                    grid.Visible = false;
                    grid.Visible = true;
                }
            }

        }

    }


    /// <summary>
    /// 枚举转为一个字典，val是枚举成员的描述，key是枚举值的字符串形式
    /// </summary>
    /// <param name="enumType">枚举实例</param>
    /// <returns>字典</returns>
    public static Dictionary<string,string> EnumToDic(Type enumType)
    {
        Dictionary<string, string> dic = new Dictionary<string, string>();
        if (IsEnum(enumType))
        {
            // 载入事件类型中文描述
            FieldInfo[] enumfields = enumType.GetFields();
            string[] names = Enum.GetNames(enumType);

            for (int i = 0; i < names.Length; i++)
            {
                FieldInfo f = enumType.GetField(names[i]);
                dic.Add(f.Name, WrapperReflection.getFieldDesc(f));
            } 

           
        }
            return dic;
    }
}
