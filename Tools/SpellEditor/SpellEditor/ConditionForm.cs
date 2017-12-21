using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.Collections;
using System.IO;
using CONDITION_SERVER;
using CONDITION_CLIENT;
using System.Runtime.InteropServices;

namespace SpellEditor
{
    public partial class ConditionForm : Form
    {
        /// <summary>
        /// key-条件名，Value-条件数据
        /// </summary>
        public Dictionary<string, DataTable> mConditionSet = new Dictionary<string, DataTable>();

        /// <summary>
        /// key-效果名，Value-效果数据,反向查询
        /// </summary>
        public Dictionary<DataTable, string> mConditionReSet = new Dictionary<DataTable, string>();

        /// <summary>
        /// 当前选中的条件索引，也就是选中行的ID
        /// </summary>
        public int nSelectedCondition = -1;

        /// <summary>
        /// 当前选中的效果名字
        /// </summary>
        public string nSelectedName = "";

        /// <summary>
        /// 条件类型与名字对应表
        /// </summary>
        private Dictionary<string, Type> m_ConditionType = new Dictionary<string, Type>();

        /// <summary>
        /// 条件描述与名字对应表,用于中文显示
        /// </summary>
        private Dictionary<string, string> m_ConditionDesc_Name = new Dictionary<string, string>();

        /// <summary>
        /// 名字与条件描述对应表,用于中文显示
        /// </summary>
        private Dictionary<string, string> m_ConditionName_Desc = new Dictionary<string, string>();

        /// <summary>
        /// 条件结构体在GamerWapper.dll中的关键字，用于在这个dll中查找条件结构体
        /// </summary>
        private const string WapperConditionKey1 = "ConditionServer";
        private const string WapperConditionKey2 = "ConditionClient";

        /// <summary>
        /// 条件结构体在GamerWapper.dll中的命名空间的关键字，用于在这个dll中查找条件结构体，最后那个点是需要的。。
        /// </summary>
        private const string ServerWapperConditionNameSpace = "CONDITION_SERVER.";

        /// <summary>
        /// 条件结构体在GamerWapper.dll中的命名空间的关键字，用于在这个dll中查找条件结构体，最后那个点是需要的。。
        /// </summary>
        private const string ClientWapperConditionNameSpace = "CONDITION_CLIENT.";


        /// <summary>
        /// 是否在加载数据
        /// </summary>
        private bool isLoadingData = false;

        /// <summary>
        /// 通过编辑按钮取得需要编辑的条件ID
        /// </summary>
        private uint EditConditonID = UInt32.MaxValue;

        /// <summary>
        /// 旧的行数，行数不一致ID才++
        /// </summary>
        private int LastRowCount = 0;

        private IDAllocator m_IDIDAllocator = new IDAllocator(IDAllocatorType.ID_POOL);

        const string InsetButtonHeaderText = "插入";

        public ConditionForm()
        {
            InitializeComponent();
            LoadConditionTypes();
        }
        private bool LoadConditionTypes()
        {
            this.conditionSelect.Columns.Add( "名称",200 );

            Type[] types = WrapperReflection.gAssembly.GetTypes();
            {
                string[] str = new string[2] { "-服务器", "-客户端" };
                foreach (var t in types)
                {
                    if (t.Name.IndexOf(WapperConditionKey1) == 0 || t.Name.IndexOf(WapperConditionKey2) >= 0)
                    {
                        ListViewItem item = new ListViewItem(new string[] { t.Name });
                        string s = t.Name.ToLower();
                        string ex = s.Contains("client") ? str[1]:str[0];
                        conditionSelect.Items.Add(WrapperReflection.getDesc(t) + ex );
                        conditionSelectList.Add(WrapperReflection.getDesc(t) + ex);
                        m_ConditionType.Add(t.Name, t);
                        m_ConditionDesc_Name.Add(WrapperReflection.getDesc(t) + ex, t.Name);
                        m_ConditionName_Desc.Add(t.Name, WrapperReflection.getDesc(t) + ex);
                    }

                    if (t.Name.IndexOf(WapperConditionKey1) == 0 || t.Name.IndexOf(WapperConditionKey2) >= 0)
                    {
                        DataTable dt = new DataTable();                      

                        FieldInfo[] fields = t.GetFields();

                        AddTableEvent(dt);
                       
                        foreach (FieldInfo f in fields)
                        {
                            DataColumn dc = dt.Columns.Add(f.Name, f.FieldType);
                            dc.Caption = WrapperReflection.getFieldDesc(f);
                            
                        }

                        //第一列是ID，不能在编辑器中编辑，所以是只读的
                        dt.Columns[0].ReadOnly = true;

                        mConditionSet.Add(t.Name, dt);
                        mConditionReSet.Add(dt, t.Name);
                    }
                }
            }

            this.FormClosing += ConditionForm_FormClosing;

            ////默认选中第一个
            //this.conditionSelect.Focus();
            //this.conditionSelect.Items[0].Selected = true;
            return true;
        }

        /// <summary>
        /// 删除table的事件
        /// </summary>
        /// <param name="ta"></param>
        private void DeleteTableEvent(DataTable ta)
        {
            ta.TableNewRow -= Row_NewEvent;
            ta.RowChanged -= Row_ChangedEvent;
            ta.RowDeleted -= Row_RowDeleted;
            ta.RowDeleting -= Row_RowDeleteing;
        }

        /// <summary>
        /// 添加table事件
        /// </summary>
        /// <param name="ta"></param>
        private void AddTableEvent(DataTable ta)
        {
            ta.TableNewRow += Row_NewEvent;
            ta.RowChanged += Row_ChangedEvent;
            ta.RowDeleted += Row_RowDeleted;
            ta.RowDeleting += Row_RowDeleteing;
        }

        #region 消息事件
        /// <summary>
        /// 关闭窗口时
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ConditionForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.conditionList.EndEdit();
            this.conditionList.DataSource = null;
        }


        /// <summary>
        /// 成功删除行的值
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Row_RowDeleted(object sender, DataRowChangeEventArgs e)
        {
            if (isLoadingData)
                return;
            DataTable dt = (DataTable)sender;
            if (LastRowCount != dt.Rows.Count)
            {
                LastRowCount = dt.Rows.Count;
            }

        }

        /// <summary>
        /// 正在删除行的值
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Row_RowDeleteing(object sender, DataRowChangeEventArgs e)
        {
            if (isLoadingData)
                return;
            SpellEditorForm.Instance.mLogicEditor.DeleteConditionOnLogic(Convert.ToInt32(e.Row[0].ToString()));
            m_IDIDAllocator.PushUnUsedId(Convert.ToUInt32(e.Row[0].ToString()));
        }

        /// <summary>
        /// 成功改变一行的值
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Row_ChangedEvent(object sender, DataRowChangeEventArgs e)
        {
            if (isLoadingData)
                return;
            DataTable dt = (DataTable)sender;
            if (LastRowCount != dt.Rows.Count)
            {
                LastRowCount = dt.Rows.Count;
                m_IDIDAllocator.AcceptAlloc();

                if (dt.Columns.Count > 2)
                {
                    DataRow row = e.Row;
                    DataTableSerializer.FillDefaultRow(ref row, dt);
                }

            }
        }

        /// <summary>
        /// 新建一行时
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Row_NewEvent(object sender, DataTableNewRowEventArgs e)
        {
            //为这一行的ID赋值
            e.Row[0] = m_IDIDAllocator.AllocId();
            DataTable dt = (DataTable)sender;
            if (dt.Columns.Count <= 1)//只有一行的时候，直接应用改变
            {
                dt.AcceptChanges();
            }

        }
        /// <summary>
        /// 选择条件名
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void conditionSelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.conditionSelect.SelectedItems.Count <= 0)
                return;

            string strName = this.conditionSelect.SelectedItems[0].Text;
            strName = m_ConditionDesc_Name[strName];
            Type t = WrapperReflection.gAssembly.GetType(ServerWapperConditionNameSpace + strName);
            if (t == null)
            {
                t = WrapperReflection.gAssembly.GetType(ClientWapperConditionNameSpace + strName);
                if(t == null)
                    return;
            }
                

            this.ConditionDesc.Text = WrapperReflection.getDesc(t);

            nSelectedName = strName;
           
            DataTableSerializer.BuildDataGridViewFromDataTable(ref conditionList, mConditionSet[strName],t);
            DataTableSerializer.FillDataGridViewFromDataTable(ref conditionList, mConditionSet[strName]);
            DataTableSerializer.AddButtonToDataGridView(ref conditionList, InsetButtonHeaderText, InsetButtonHeaderText, ConditionInsetButtonCallBack);

            //显示到需要编辑的那一行
            if (EditConditonID != UInt32.MaxValue)
            {
                DataTable tb = mConditionSet[strName];
                for (int i = 0; i < tb.Rows.Count; i++)
                {
                    DataRow dr = tb.Rows[i];
                    uint ut;
                    if (!UInt32.TryParse(dr[0].ToString(), out ut))
                    {
                        break;
                    }
                    if (ut == EditConditonID)
                    {
                        conditionList[0, i].Selected = true;
                        break;
                    }
                }
            }
        }

        /// <summary>
        /// 选择条件列表。
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void conditionList_SelectedIndexChanged(object sender, EventArgs e)
        {
            DataTable dt = (DataTable)this.conditionList.DataSource;

            if (this.conditionList.SelectedRows.Count <= 0)
                return;

            string s = this.conditionList.SelectedRows[0].Cells[0].Value.ToString();
            nSelectedCondition = Convert.ToInt32(s);
        }

        
        /// <summary>
        /// 插入按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
         private void InsetButton_Click(object sender, EventArgs e)
         {
             if (this.conditionSelect.SelectedItems.Count <= 0)
             {
                 Trace.Error("请先选择条件！");
                 return;
             }
             string strName = this.conditionSelect.SelectedItems[0].Text;
             strName = m_ConditionDesc_Name[strName];
             DataTable ta = mConditionSet[strName];


             Object conditionIndex = null;
             if(this.conditionList.SelectedRows.Count <= 0)
             {
                 if (this.conditionList.SelectedCells.Count <= 0)
                 {
                     return;
                 }
                 conditionIndex = this.conditionList.SelectedCells[0].OwningRow.Cells[0].Value;
             }
             else
             {
                 conditionIndex = this.conditionList.SelectedRows[0].Cells[0].Value;
             }

             int cid = Convert.ToInt32(conditionIndex);
             SpellLogicForm.Instance.PushConditon(cid);
         }


         /// <summary>
         /// 每一行的插入按钮回调
         /// </summary>
         /// <param name="sender"></param>
         /// <param name="e"></param>
         private static void ConditionInsetButtonCallBack(object sender, DataGridViewCellMouseEventArgs e)
         {
             DataGridView grid = (DataGridView)sender;
             
             DataTable ta = (DataTable)grid.DataSource;
             if (ta.Rows.Count > 0 && e.ColumnIndex >= 0 && e.RowIndex >= 0 && ta.Rows.Count > e.RowIndex)
             {
                 if (grid.Columns[e.ColumnIndex].HeaderText.Equals(InsetButtonHeaderText))
                 {
                     int cid = Convert.ToInt32(grid.Rows[e.RowIndex].Cells[0].Value);
                     SpellLogicForm.Instance.PushConditon(cid);
                 }
             }

         }
        #endregion


        #region 逻辑操作
         /// <summary>
         /// 查找条件
         /// </summary>
         /// <param name="condition_id">条件ID</param>
         /// <returns></returns>
         public string[] FindCondition(int condition_id)
         {
                foreach (KeyValuePair<string, DataTable> kvp in mConditionSet)
                {
                    DataTable dt = kvp.Value;
                    for (int i = 0; i < dt.Rows.Count; i++)
                    {
                        //第0列是ID
                        int m_cid = Convert.ToInt32(dt.Rows[i][0].ToString());
                        if (m_cid == condition_id)
                        {
                            string s = "";
                            for (int j = 0; j < dt.Columns.Count; j++)
                            {
                                s += dt.Rows[i][j].ToString() + ",";
                            }

                            return new string[] { m_ConditionName_Desc[kvp.Key], s };
                        }
                    }
                }
             return new string[] { "未知", "未知" };
         }

         /// <summary>
         /// 清除条件
         /// </summary>
         /// <param name="cid">条件ID</param>
         public void CleanConditonTable(int cid)
         {
            foreach (KeyValuePair<string, DataTable> kvp in mConditionSet)
            {
                DataTable dt = kvp.Value;
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    int m_cid = Convert.ToInt32(dt.Rows[i][0].ToString());
                    if (m_cid == cid)
                    {
                        mConditionSet[kvp.Key].Clear();
                        return;
                    }
                }
            }
         }

         /// <summary>
         /// 删除条件
         /// </summary>
         /// <param name="conditonName">条件名</param>
         public void DeleteConditon(string conditonName)
         {
                if (mConditionSet.ContainsKey(conditonName))
                {
                    mConditionSet[conditonName].Clear();
                }
         }


         /// <summary>
         /// 显示条件
         /// </summary>
         /// <param name="conditionStr">条件字符串，目前是XX,XX格式，第一个是条件名，后面的是ID</param>
         public void EditCondition(string conditionStr)
         {
             EditConditonID = UInt32.MaxValue;
             string []s = conditionStr.Split(',');
             string conditionName = s[0];
             if (s.Length >= 2)
             {
                 if(!UInt32.TryParse(s[1], out EditConditonID))
                 {
                     EditConditonID = UInt32.MaxValue;
                 }
             }

             foreach (ListViewItem str in this.conditionSelect.Items)
             {
                 if (str.Text.Equals(conditionName))
                 {
                     if (conditionSelect.SelectedItems.Count > 0)
                     {
                         conditionSelect.SelectedItems[0].Selected = false;
                     }
                    
                     str.Selected = true;
                     break;
                 }
             }

             ShowDialog();

         }

        #endregion


        #region 导入\出相关


         #region CSV
         /// <summary>
         /// 导出条件列表到csv
         /// </summary>
         /// <returns></returns>
         public DataTableSerializer.FileExportReslut ExportCoditionTableToCsv(string fullPatch)
         {
             List<DataTable> tableList = new List<DataTable>();
             BuildSaveFormatTable(tableList);

             DataTable table = null;
             int index = 0;
             string[] headerText = new string[2];
             headerText[0] = "条件名,条件ID,参数";
             headerText[1] = "string,int,int";
             bool bFirstSave = true;
             foreach (KeyValuePair<string, DataTable> dt in mConditionSet)
             {
                 table = tableList[index];
                 bool bNeedToSave = false;
                 //遍历所有的行
                 for (int i = 0; i < dt.Value.Rows.Count; i++)
                 {
                     DataRow _row = table.NewRow();
                     //第0个永远是条件名
                     _row[0] = dt.Key;
                     for (int j = 0; j < dt.Value.Columns.Count; j++)
                     {
                         _row[j + 1] = dt.Value.Rows[i][j];
                     }
                     //将这行添加到这个table
                     table.Rows.Add(_row);
                     bNeedToSave = true;
                 }
                 if (bNeedToSave)
                 {
                     if (bFirstSave)
                     {
                         //如果存在这个文件，删了。因为导出这个是追加模式的
                         if (File.Exists(fullPatch))
                             File.Delete(fullPatch);
                     }
                     //写入到文件
                     if (!DataTableSerializer.SaveToCSV(table, fullPatch,headerText, !bFirstSave))
                     {
                         Trace.Error("文件导出失败：" + fullPatch);
                         return DataTableSerializer.FileExportReslut.RESLUT_ERROR;
                     }
                     bFirstSave = false;
                     bNeedToSave = false;
                 }

                 index++;
             }
             if (bFirstSave)
             {
                 Trace.Warring("文件为空：" + fullPatch);
                 return DataTableSerializer.FileExportReslut.RESLUT_EMPTYFILE;
             }

             Trace.Normal("文件导出成功：" + fullPatch);  
             return DataTableSerializer.FileExportReslut.RESLUT_OK;
         }

         /// <summary>
         /// 从Csv中加载条件列表
         /// </summary>
         /// <returns></returns>
         public bool LoadCoditionTableFromCsv(string fullPatch)
         {
             //如果不存在这个文件
             if (!File.Exists(fullPatch))
             {
                 Trace.Error("LoadCsvCoditionTable，文件不存在：" + fullPatch);
                 return false;
             }
             List<DataTable> tableList = new List<DataTable>();

             Dictionary<string,DataTable> ta =  BuildSaveFormatTable(null);

             m_IDIDAllocator.ClearIDAllocator();
             isLoadingData = true;
             if (!LoadSaveFormatTableToCsv(ref ta, fullPatch))
             {
                 isLoadingData = false;
                 return false;
             }
             tableList.AddRange(ta.Values);
             DataTable table = null;
             int index = 0;
             foreach (KeyValuePair<string, DataTable> dt in mConditionSet)
             {
                 table = tableList[index];
                 DeleteTableEvent(dt.Value);
                 dt.Value.Clear();
                 //遍历所有的行
                 for (int i = 0; i < table.Rows.Count; i++)
                 {
                     string nam = table.Rows[i][0].ToString();
                     //名字不一样，跳过
                     if (!nam.Equals(dt.Key))
                     {
                         break;
                     }
                     
                     DataRow _row = dt.Value.NewRow();
                     for (int j = 0; j < dt.Value.Columns.Count; j++)
                     {
                         _row[j] = table.Rows[i][j + 1];
                     }
                     //将这行添加到这个table
                     dt.Value.Rows.Add(_row);
                 }
                 AddTableEvent(dt.Value);
                index++;
             }
             m_IDIDAllocator.CalculateUnUsedID();
             isLoadingData = false;
             return true;
         }

         /// <summary>
         /// 加载csv到存储模式的table
         /// </summary>
         /// <param name="tablSet">需要填充的table合集</param>
         /// <param name="filePath">文件路径</param>
         /// <param name="rowIndex">索引</param>
         /// <returns></returns>
         private bool LoadSaveFormatTableToCsv(ref Dictionary<string, DataTable> tableSet,string filePath, int rowIndex = 2)
         {
             if (tableSet == null || string.IsNullOrEmpty(filePath))
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
             int i = 0, j = 0;
             try
             {
                 using (StreamReader reader = new StreamReader(filePath, Config.FileEncoding, false))
                 {
                     reader.Peek();
                     string CurrentLineName = "";
                     //是否第一次读
                     bool bFirstRead = true;
                     DataTable table = null;
                     while (reader.Peek() > 0)
                     {
                         j = j + 1;
                         string _line = reader.ReadLine();
                         if (j >= rowIndex + 1)
                         {
                             string[] _split = _line.Split(',');
                             if (bFirstRead)
                             {
                                 CurrentLineName = _split[0];
                                 bFirstRead = false;
                                 if (!tableSet.ContainsKey(CurrentLineName))
                                 {
                                     Trace.Warring("在参数tableSet中找不到类型：" + CurrentLineName + "数据已忽略");
                                     continue;
                                 }
                                 table = tableSet[CurrentLineName];
                                 table.Clear();
                             }
                             
                             //上一次的的名字和这次的名字不一样
                             if (!CurrentLineName.Equals(_split[0]))
                             {
                                 if (!tableSet.ContainsKey(CurrentLineName))
                                 {
                                     Trace.Warring("在参数tableSet中找不到类型：" + CurrentLineName + "数据已忽略");
                                     continue;
                                 }
                                 CurrentLineName = _split[0];
                                 table = tableSet[CurrentLineName];
                                 table.Clear();
                             }
 
                             if (table.Columns.Count != _split.Length)
                             {
                                 Trace.Warring("文件:" + filePath + "，第" + (j).ToString() + "行格式与所需要的格式不一致，本行只有" +
                                     (_split.Length).ToString() + "列，所需要的格式为:" + table.Columns.Count + "列，已填充默认值，请注意检查！");
                                 //table.Rows.Clear();
                                 //return false;
                             }
                             DataRow _row = DataTableSerializer.FillDefaultNewRow(ref table);
                             for (i = 0; i < table.Columns.Count; i++)
                             {
                                 if (i > _split.Length - 1)
                                 {
                                     break;
                                 }

                                 if (string.IsNullOrEmpty(_split[i]))
                                 {
                                     Trace.Error("文件：" + filePath + ",[" + j.ToString() + "行，" + i.ToString() + "列]为空！");
                                     table.Rows.Clear();
                                     return false;
                                 }
                                 //第二个是技能ID
                                 if (i == 1)
                                 {
                                     //取出最大的ID
                                     uint Id = Convert.ToUInt32(_split[i]);
                                     m_IDIDAllocator.PushUsedId(Id);
                                 }
                                 //枚举类型，特殊处理
                                 if (DataTableSerializer.IsEnum(table.Columns[i].DataType))
                                 {

                                     _row[i] = Enum.Parse(table.Columns[i].DataType, _split[i]);
                                 }
                                 else
                                 {
                                     _row[i] = _split[i];
                                 }
                             }
                             table.Rows.Add(_row);
                         }
                     }
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
             return true;
         }

         #endregion


         #region BIN

         /// <summary>
         /// 导出条件列表到bin
         /// </summary>
         /// <returns></returns>
         public DataTableSerializer.FileExportReslut ExportCoditionTableToBin(string fullPatch)
         {

             bool bFirstSave = true;
             foreach (KeyValuePair<string, DataTable> dt in mConditionSet)
             {
                 Type t = m_ConditionType[dt.Key];

                 bool bNeedToSave = false;
                 if (dt.Value.Rows.Count > 0)
                 {
                     bNeedToSave = true;
                     if (bFirstSave)
                     {
                         //如果存在这个文件，删了。因为导出这个是追加模式的
                         if (File.Exists(fullPatch))
                             File.Delete(fullPatch);
                         bFirstSave = false;
                     }
                 }
                     
                 if (bNeedToSave)
                 {
                     //写入到文件
                     if (!DataTableSerializer.SaveToBin(dt.Value, fullPatch, t, FileMode.Append))
                     {
                         Trace.Error("文件导出失败：" + fullPatch);
                         return DataTableSerializer.FileExportReslut.RESLUT_ERROR;
                     }
                 }
             }
             if (bFirstSave)
             {
                 Trace.Warring("文件为空：" + fullPatch);
                 return DataTableSerializer.FileExportReslut.RESLUT_EMPTYFILE;
             }
             Trace.Normal("文件导出成功：" + fullPatch);
             return DataTableSerializer.FileExportReslut.RESLUT_OK;
         }

         /// <summary>
         /// 从bin中加载条件列表
         /// </summary>
         /// <returns></returns>
         public bool LoadCoditionTableFromBin(string fullPatch)
         {
             //如果不存在这个文件
             if (!File.Exists(fullPatch))
             {
                 Trace.Error("LoadBinCoditionTable，文件不存在：" + fullPatch);
                 return false;
             }
             m_IDIDAllocator.ClearIDAllocator();
             isLoadingData = true;
             if (!LoadConditionTableFromBinImp(fullPatch))
             {
                 isLoadingData = false;
                 return false;
             }
             m_IDIDAllocator.CalculateUnUsedID();
             isLoadingData = false;
             return true;
         }

         /// <summary>
         /// 加载bin的条件table
         /// </summary>
         /// <param name="tableArray">table数组</param>
         /// <param name="filePath">文件路径</param>
         /// <returns></returns>
         private bool LoadConditionTableFromBinImp(string filePath)
         {
             if (string.IsNullOrEmpty(filePath))
                 return false;

             if (!File.Exists(filePath))
             {
                 Trace.Error("文件不存在：" + filePath);
                 return false;
             }
             FileStream _readObj = null;

            try
            {
                using (_readObj = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                {
                    foreach(KeyValuePair<string,DataTable> kv in mConditionSet )
                    {
                        
                        BIN_FILE_HEADER header = new BIN_FILE_HEADER();
                        //已经读取完了，跳出
                        if (DataTableSerializer.ReadStruct(ref header, _readObj) <= 0)
                        {
                            break;
                        }
                        if (!mConditionSet.ContainsKey(header.szTypeName))
                        {
                            Trace.Error("加载bin格式的条件时遇到无法识别的类型：" + header.szTypeName);
                            continue;
                        }
                        DataTable table = mConditionSet[header.szTypeName];
                        Type structType = m_ConditionType[header.szTypeName];
                        table.Clear();
                        FieldInfo[] fields = structType.GetFields();
                        // 逐个输入
                        for (int i = 0; i < header.nItemTotal; i++)
                        {
                            Object structTypeInstance = Activator.CreateInstance(structType);
                            DataTableSerializer.ReadStructFromType(ref structTypeInstance, _readObj, structType);
                            DataRow _row = table.NewRow();

                            for (int j = 0; j < fields.Length; j++)
                            {
                                _row[j] = fields[j].GetValue(structTypeInstance);
                                //第一个是技能ID
                                if (j == 0)
                                {
                                    //取出最大的ID
                                    uint Id = Convert.ToUInt32(_row[j]);
                                    m_IDIDAllocator.PushUsedId(Id);
                                }
                            }
                            table.Rows.Add(_row);
                        }
                    }

                }
            }
            catch (Exception e)
            {
                 Trace.Error(e.Message);
                 return false;
            }
           return true;
         }

         #endregion

        /// <summary>
         /// 构建出存储用的table
        /// </summary>
        /// <param name="table"></param>
        /// <returns>返回类型名与新table的对应关系</returns>
        private Dictionary<string,DataTable> BuildSaveFormatTable(List<DataTable> table)
        {
            Dictionary<string, DataTable> tableDic = new Dictionary<string,DataTable>();
            //构建出事件名，数据的table
            foreach (KeyValuePair<string, DataTable> dt in mConditionSet)
            {
                DataTable ta = new DataTable();
                //先给table添加一行事件名
                ta.Columns.Add("事件名", typeof(string));
                for (int i = 0; i < dt.Value.Columns.Count; i++)
                {
                    //再添加原来table的列
                    ta.Columns.Add(dt.Value.Columns[i].Caption, dt.Value.Columns[i].DataType);
                }
                if(table != null)
                    table.Add(ta);
                tableDic.Add(dt.Key, ta);
            }
            return tableDic;
        }

        #endregion


        #region 搜索

        private string SerachText = string.Empty;
        List<string> conditionSelectList = new List<string>();
        private int SerachID = -1;

        private void ConditionSerachBox_TextChanged(object sender, EventArgs e)
        {
            RichTextBox box = sender as RichTextBox;
            conditionSelect.Items.Clear();
            if (string.IsNullOrEmpty(box.Text))
            {
                foreach (string s in conditionSelectList)
                {
                    conditionSelect.Items.Add(s);
                }
                return;
            }
            SerachText = box.Text;

            foreach (string s in conditionSelectList)
            {
                string lowerText = s.ToLower();
                //忽略大小写
                if (lowerText.Contains(SerachText))
                {
                    conditionSelect.Items.Add(s);
                }
            }

        }

        private void ConditionIDSerach_TextChanged(object sender, EventArgs e)
        {
            RichTextBox box = sender as RichTextBox;

            if (string.IsNullOrEmpty(box.Text))
            {
                return;
            }
            if (!int.TryParse(box.Text, out SerachID))
            {
                Trace.Error("请输入整数!");
            }
        }

        private void ConditionSerachIDButton_Click(object sender, EventArgs e)
        {
            //Table索引
            DataTable FoundTable = null;
            //Table的行索引
            int Rowindex = 0;
            bool bFound = false;
            foreach (DataTable ta in mConditionSet.Values)
            {
                FoundTable = ta;
                foreach (DataRow dr in FoundTable.Rows)
                {
                    if (dr[0].Equals(SerachID))
                    {
                        bFound = true;
                        break;
                    }
                    Rowindex++;
                }
                if (bFound)
                {
                    break;
                }
                Rowindex = 0;
            }

            if (bFound)
            {
                string tableName = mConditionReSet[FoundTable];

                Type t = WrapperReflection.gAssembly.GetType(ServerWapperConditionNameSpace + tableName);
                if (t == null)
                {
                    t = WrapperReflection.gAssembly.GetType(ClientWapperConditionNameSpace + tableName);
                    if (t == null)
                        return;
                }

                this.ConditionDesc.Text = WrapperReflection.getDesc(t);
                ConditionSerachBox.Text = WrapperReflection.getDesc(t);
                DataTableSerializer.BuildDataGridViewFromDataTable(ref conditionList, mConditionSet[tableName], t);
                DataTableSerializer.FillDataGridViewFromDataTable(ref conditionList, mConditionSet[tableName]);
                DataTableSerializer.AddButtonToDataGridView(ref conditionList, InsetButtonHeaderText, InsetButtonHeaderText, ConditionInsetButtonCallBack);
                conditionList.Rows[Rowindex].Selected = true;
            }
            else
            {
                Trace.Error("找不到条件ID");
            }
        }

        #endregion

        private void conditionList_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void ConditionForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)(27))
            {
                this.Close();
                return;
            }
        }


    }
}
