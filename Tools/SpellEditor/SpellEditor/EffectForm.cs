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
using EFFECT_SERVER;
using EFFECT_CLIENT;
using System.Runtime.InteropServices;

namespace SpellEditor
{
    public partial class EffectForm : Form
    {
        /// <summary>
        /// key-效果名，Value-效果数据
        /// </summary>
        public Dictionary<string, DataTable> mEffectSet = new Dictionary<string, DataTable>();

        /// <summary>
        /// key-效果名，Value-效果数据,反向查询
        /// </summary>
        public Dictionary<DataTable, string> mEffect_ReSet = new Dictionary<DataTable, string>();

        /// <summary>
        /// 当前选中的条件索引，也就是选中行的ID
        /// </summary>
        public int nSelectedEffect = -1; 

        /// <summary>
        /// 当前选中的效果名字
        /// </summary>
        public string nSelectedName = "";


        /// <summary>
        /// 效果类型与名字对应表
        /// </summary>
        private Dictionary<string, Type> m_EffectType = new Dictionary<string, Type>();

        /// <summary>
        /// 效果描述与名字对应表,用于中文显示
        /// </summary>
        private Dictionary<string, string> m_EffectDesc_Name = new Dictionary<string, string>();


        /// <summary>
        /// 名字与效果描述对应表,用于中文显示
        /// </summary>
        private Dictionary<string, string> m_EffectName_Desc = new Dictionary<string, string>();

        /// <summary>
        /// 效果结构体在GameWapper.dll中的关键字，用于在这个dll中查找条件结构体
        /// </summary>
        private const string WapperEffectKey = "Effect";

        /// <summary>
        /// 效果结构体在GameWapper.dll中的命名空间的关键字，用于在这个dll中查找条件结构体，最后那个点是需要的。。
        /// </summary>
        private const string ClientWapperEffectNameSpace = "EFFECT_CLIENT.";

        /// <summary>
        /// 条件结构体在GameWapper.dll中的命名空间的关键字，用于在这个dll中查找条件结构体，最后那个点是需要的。。
        /// </summary>
        private const string ServerWapperEffectNameSpace = "EFFECT_SERVER.";


        /// <summary>
        /// 是否在加载数据
        /// </summary>
        private bool isLoadingData = false;

        /// <summary>
        /// 通过编辑按钮取得需要编辑的效果ID
        /// </summary>
        private uint EditEffectID = UInt32.MaxValue;

        //private IDAllocator m_IDIDAllocator = new IDAllocator(IDAllocatorType.ID_MAX);
        private Dictionary<HeroEffectIDRange, IDAllocator> IDAllocatorList = new Dictionary<HeroEffectIDRange, IDAllocator>();
        public Dictionary<string, HeroEffectIDRange> effectIDRangeDict = new Dictionary<string, HeroEffectIDRange>();    //保存effectID范围的Dictionary


        /// <summary>
        /// 旧的行数，行数不一致ID才++
        /// </summary>
        private int LastRowCount = 0;

        const string InsetButtonHeaderText = "插入";
        public const string EffectIDRangeFileName = "HeroEffectIDRange";
        public EffectForm()
        {
            string idpatch = EffectIDRangeFileName;
            idpatch += ".csv";
            LoadEffectIDRangeCSV(idpatch);
            InitializeComponent();

            LoadEffectTypes();
            List<string> keys = new List<string>(effectIDRangeDict.Keys);
            if (keys.Count > 0)
            {
                this.eIDRangeCombobox.Text = keys[0];
                for (int i = 0; i < keys.Count; i++)
                {
                    this.eIDRangeCombobox.Items.Add(keys[i]);
                }
                curIDRangeItem = effectIDRangeDict[keys[0]];
                this.eIDRangeLabel.Text = curIDRangeItem.minID + "~" + curIDRangeItem.maxID;
            }
        }

        //加载英雄ID范围配置
        public bool LoadEffectIDRangeCSV(string filePath, int rowIndex = 2)
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
                    int max = 0;
                    int lineNum = 1;
                    while (reader.Peek() > 0)
                    {
                        string _line = reader.ReadLine();

                        string[] _split = _line.Split(',');
                        HeroEffectIDRange item = new HeroEffectIDRange();
                        item.heroID = int.Parse(_split[0]);
                        item.heroName = _split[1];
                        item.minID = int.Parse(_split[2]);
                        item.maxID = int.Parse(_split[3]);
                        string key = (_split[1]);

                        string name = _split[1];

                        //做些ID范围冲突检测
                        if (item.minID > item.maxID)
                        {
                            Trace.Error("ID范围配置，第" + lineNum + "行数据出错：最小ID必须小于最大ID");
                            return false;
                        }
                        if (max > item.minID)
                        {
                            Trace.Error("ID范围配置，第" + lineNum + "行数据出错：ID范围必须是从小到大排序");
                            return false;
                        }
                        max = item.maxID;

                        effectIDRangeDict.Add(key, item);
                        IDAllocator alloc = new IDAllocator(IDAllocatorType.ID_MAX);
                        alloc.IDStartIndex = (uint)item.minID;
                        IDAllocatorList.Add(item, alloc);
                        lineNum++;
                    }

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

        private bool LoadEffectTypes()
        {
            this.effectSelect.Columns.Add( "名称",200 );

            Type[] types = WrapperReflection.gAssembly.GetTypes();
            {
                string[] str = new string[2] { "-服务器", "-客户端" };
                foreach (var t in types)
                {
                    if (t.Name.IndexOf(WapperEffectKey) >= 0)
                    {
                        ListViewItem item = new ListViewItem(new string[] { t.Name });
                        string s = t.Name.ToLower();
                        string ex = s.Contains("client") ? str[1] : str[0];
                        this.effectSelect.Items.Add(WrapperReflection.getDesc(t) + ex);
                        effectSelectList.Add(WrapperReflection.getDesc(t) + ex);
                        m_EffectType.Add(t.Name, t);
                        m_EffectDesc_Name.Add(WrapperReflection.getDesc(t) + ex, t.Name);
                        m_EffectName_Desc.Add(t.Name, WrapperReflection.getDesc(t) + ex);
                    }

                    if (t.Name.IndexOf(WapperEffectKey) >= 0)
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

                        mEffectSet.Add(t.Name, dt);
                        mEffect_ReSet.Add(dt, t.Name);
                    }
                }
            }
            this.FormClosing    += EffectForm_FormClosing;
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
        private void EffectForm_FormClosing(object sender, FormClosingEventArgs  e)
        {
            this.effectList.EndEdit();
            this.effectList.DataSource = null;
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
            SpellEditorForm.Instance.mLogicEditor.DeleteEffectOnLogic(Convert.ToInt32(e.Row[0].ToString()));
            uint id = Convert.ToUInt32(e.Row[0].ToString());
            IDAllocator alloc = GetIDAllocator(id);
            if (alloc != null) {
                alloc.PushUnUsedId(id);
            }
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
                AcceptAllocAll();
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
            if (curIDRangeItem != null)
            {
                IDAllocator alloc = GetIDAllocator((uint)curIDRangeItem.minID);
                uint newID = alloc.AllocId();
                if (newID <= (uint)curIDRangeItem.maxID) {
                    e.Row[0] = newID;
                    DataTable dt = (DataTable)sender;
                    if (dt.Columns.Count <= 1)//只有一行的时候，直接应用改变
                    {
                        dt.AcceptChanges();
                    }
                }
                else
                {
                    Trace.Error("该ID组已满");
                }
            }
        }

        /// <summary>
        /// 选择效果名
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EffectSelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.effectSelect.SelectedItems.Count <= 0)
                return;
            
            string strName = this.effectSelect.SelectedItems[0].Text;
            OnSelectEffect( strName);
            return;

        }

        /// <summary>
        /// 选择效果列表。
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EffectList_SelectedIndexChanged(object sender, EventArgs e)
        {
            DataTable dt = (DataTable)this.effectList.DataSource;

            if (this.effectList.SelectedRows.Count <= 0)
                return;

            string s = this.effectList.SelectedRows[0].Cells[0].Value.ToString();
            nSelectedEffect = Convert.ToInt32(s);
        }

        
        /// <summary>
        /// 插入按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
         private void InsetButton_Click(object sender, EventArgs e)
         {
             if (this.effectSelect.SelectedItems.Count <= 0)
             {
                 Trace.Error("请先选择条件！");
                 return;
             }
             string strName = this.effectSelect.SelectedItems[0].Text;
             strName = m_EffectDesc_Name[strName];
             DataTable ta = mEffectSet[strName];

             Object effectIndex = null;
             if (this.effectList.SelectedRows.Count <= 0)
             {
                 if (this.effectList.SelectedCells.Count <= 0)
                 {
                     return;
                 }
                 effectIndex = this.effectList.SelectedCells[0].OwningRow.Cells[0].Value;
             }
             else
             {
                 effectIndex = this.effectList.SelectedRows[0].Cells[0].Value;
             }

             int eid = Convert.ToInt32(effectIndex);
             SpellLogicForm.Instance.PushEffect(eid);
         }


         /// <summary>
         /// 每一行的插入按钮回调插入按钮回调
         /// </summary>
         /// <param name="sender"></param>
         /// <param name="e"></param>
         private static void EffectInsetButtonCallBack(object sender, DataGridViewCellMouseEventArgs e)
         {
             DataGridView grid = (DataGridView)sender;

             DataTable ta = (DataTable)grid.DataSource;
             if (ta.Rows.Count > 0 && e.ColumnIndex >= 0 && e.RowIndex >= 0 && ta.Rows.Count > e.RowIndex)
             {
                 if (grid.Columns[e.ColumnIndex].HeaderText.Equals(InsetButtonHeaderText))
                 {
                     int eid = Convert.ToInt32(grid.Rows[e.RowIndex].Cells[0].Value);
                     SpellLogicForm.Instance.PushEffect(eid);
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
         public string[] FindEffect(int condition_id)
         {
            foreach (KeyValuePair<string, DataTable> kvp in mEffectSet)
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
                        return new string[] { m_EffectName_Desc[kvp.Key], s };
                    }
                }
             }
             return new string[] { "未知", "未知" };
         }

         /// <summary>
         /// 清除条件
         /// </summary>
         /// <param name="cid">条件ID</param>
         public void CleanEffectTable(int cid)
         {
            foreach (KeyValuePair<string, DataTable> kvp in mEffectSet)
            {
                DataTable dt = kvp.Value;
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    int m_cid = Convert.ToInt32(dt.Rows[i][0].ToString());
                    if (m_cid == cid)
                    {
                        mEffectSet[kvp.Key].Clear();
                        return;
                    }
                }
            }
         }

         /// <summary>
         /// 删除条件
         /// </summary>
         /// <param name="conditonName">条件名</param>
         public void DeleteEffect(string conditonName)
         {
            if (mEffectSet.ContainsKey(conditonName))
            {
                mEffectSet[conditonName].Clear();
            }
         }


        /// <summary>
        /// 显示效果
        /// </summary>
         /// <param name="effectStr">条件字符串，目前是XX,XX格式，第一个是条件名，后面的是ID</param>
         public void EditEffect(string effectStr)
         {
             EditEffectID = UInt32.MaxValue;
             string[] s = effectStr.Split(',');
             string effectName = s[0];
             if (s.Length >= 2)
             {
                 if (!UInt32.TryParse(s[1], out EditEffectID))
                 {
                     EditEffectID = UInt32.MaxValue;
                 }
             }

             foreach (ListViewItem str in this.effectSelect.Items)
             {
                 if (str.Text.Equals(effectName))
                 {
                     if (effectSelect.SelectedItems.Count > 0)
                     {
                         effectSelect.SelectedItems[0].Selected = false;
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
         /// <returns>1-成功；0-表为空；-1失败</returns>
         public DataTableSerializer.FileExportReslut ExportEffectTableToCsv(string fullPatch)
         {
             List<DataTable> tableList = new List<DataTable>();
             BuildSaveFormatTable( tableList);

             DataTable table = null;
             int index = 0;
             bool bFirstSave = true;
             string[] headerText = new string[2];
             headerText[0] = "效果名,效果ID,参数";
             headerText[1] = "string,int,int";
             foreach (KeyValuePair<string, DataTable> dt in mEffectSet)
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
                     if (!DataTableSerializer.SaveToCSV(table, fullPatch, headerText,!bFirstSave))
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
         public bool LoadEffectTableFromCsv(string fullPatch)
         {
             //如果不存在这个文件
             if (!File.Exists(fullPatch))
             {
                 Trace.Error("LoadCsvEffectTable，文件不存在：" + fullPatch);
                 return false;
             }
             List<DataTable> tableList = new List<DataTable>();

             Dictionary<string, DataTable> ta = BuildSaveFormatTable(null);

             DataTable[] TableArray = tableList.ToArray();
             isLoadingData = true;
             ClearIDAllocatorAll();  
             if (!LoadSaveFormatTableFromCsv(ref ta, fullPatch))
             {
                 isLoadingData = false;
                 return false;
             }
             
             tableList.AddRange(ta.Values);
             DataTable table = null;
             int index = 0;
             foreach (KeyValuePair<string, DataTable> dt in mEffectSet)
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
             CalculateUnUsedIDAll();
             isLoadingData = false;
             return true;
         }

         /// <summary>
         /// 加载csv到存储模式的table
         /// </summary>
         /// <param name="tableArray">table数组</param>
         /// <param name="tableName">table名字</param>
         /// <param name="filePath">文件路径</param>
         /// <param name="rowIndex">索引</param>
         /// <returns></returns>
         private bool LoadSaveFormatTableFromCsv(ref Dictionary<string, DataTable> tableSet, string filePath, int rowIndex = 2)
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
                                    IDAllocator alloc = GetIDAllocator(Id);
                                    if (alloc != null)
                                    {
                                        alloc.PushUsedId(Id);
                                    }
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
         public DataTableSerializer.FileExportReslut ExportEffectTableToBin(string fullPatch)
         {

             bool bFirstSave = true;
             foreach (KeyValuePair<string, DataTable> dt in mEffectSet)
             {
                 Type t = m_EffectType[dt.Key];
                 bool bNeedToSave = false;
                 if (dt.Value.Rows.Count > 0)
                     bNeedToSave = true;
                 if (bNeedToSave)
                 {
                     if (bFirstSave)
                     {
                         //如果存在这个文件，删了。因为导出这个是追加模式的
                         if (File.Exists(fullPatch))
                             File.Delete(fullPatch);
                         bFirstSave = false;
                     }
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
         public bool LoadEffectTableFromBin(string fullPatch)
         {
             //如果不存在这个文件
             if (!File.Exists(fullPatch))
             {
                 Trace.Error("LoadBinEffectTable，文件不存在：" + fullPatch);
                 return false;
             }
             isLoadingData = true;
             ClearIDAllocatorAll();
             if (!LoadEffectTableFromBinImp(fullPatch))
             {
                 isLoadingData = false;
                 return false;
             }
             CalculateUnUsedIDAll();
             isLoadingData = false;
             return true;
         }

         /// <summary>
         /// 加载bin的条件table
         /// </summary>
         /// <param name="tableArray">table数组</param>
         /// <param name="filePath">文件路径</param>
         /// <returns></returns>
         private bool LoadEffectTableFromBinImp(string filePath)
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
                    foreach (KeyValuePair<string, DataTable> kv in mEffectSet)
                    {
                        BIN_FILE_HEADER header = new BIN_FILE_HEADER();
                        //已经读取完了，跳出
                        if (DataTableSerializer.ReadStruct(ref header, _readObj) <= 0)
                        {
                            break;
                        }

                        if (!mEffectSet.ContainsKey(header.szTypeName))
                        {
                            Trace.Error("加载bin格式的效果时遇到无法识别的类型：" + header.szTypeName);
                            continue;
                        }

                        DataTable table = mEffectSet[header.szTypeName];
                        Type structType = m_EffectType[header.szTypeName];

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
                                    IDAllocator alloc = GetIDAllocator(Id);
                                    if (alloc != null)
                                    {
                                        alloc.PushUsedId(Id);
                                    }
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
         private Dictionary<string, DataTable> BuildSaveFormatTable(List<DataTable> table)
        {
            if (null == table)
                table = new List<DataTable>();
            Dictionary<string, DataTable> tableDic = new Dictionary<string, DataTable>();
            //构建出事件名，数据的table
            foreach (KeyValuePair<string, DataTable> dt in mEffectSet)
            {
                DataTable ta = new DataTable();
                //先给table添加一行事件名
                ta.Columns.Add("事件名", typeof(string));
                for (int i = 0; i < dt.Value.Columns.Count; i++)
                {
                    //再添加原来table的列
                    ta.Columns.Add(dt.Value.Columns[i].Caption, dt.Value.Columns[i].DataType);
                }
                if (table != null)
                    table.Add(ta);
                tableDic.Add(dt.Key, ta);
            }
            return tableDic;
        }

        #endregion

        #region 事件具体执行函数

         private void OnSelectEffect(string effectName)
         {
             effectName = m_EffectDesc_Name[effectName];
             Type t = WrapperReflection.gAssembly.GetType(ServerWapperEffectNameSpace + effectName);
             if (t == null)
             {
                 t = WrapperReflection.gAssembly.GetType(ClientWapperEffectNameSpace + effectName);
                 if (t == null)
                     return;
             }


             this.ConditionDesc.Text = WrapperReflection.getDesc(t);

             nSelectedName = effectName;

             DataTableSerializer.BuildDataGridViewFromDataTable(ref effectList, mEffectSet[effectName], t);
             DataTableSerializer.FillDataGridViewFromDataTable(ref effectList, mEffectSet[effectName]);
             DataTableSerializer.AddButtonToDataGridView(ref effectList, InsetButtonHeaderText, InsetButtonHeaderText, EffectInsetButtonCallBack);

             //显示到需要编辑的那一行
             if (EditEffectID != UInt32.MaxValue)
             {
                 DataTable tb = mEffectSet[effectName];
                 for (int i = 0; i < tb.Rows.Count; i++)
                 {
                     DataRow dr = tb.Rows[i];
                     uint ut;
                     if (!UInt32.TryParse(dr[0].ToString(), out ut))
                     {
                         break;
                     }
                     if (ut == EditEffectID)
                     {
                         effectList[0, i].Selected = true;
                         break;
                     }
                 }
             }
         }

         #endregion

         #region 搜索

         private string SerachText = string.Empty;
         private int SerachID = -1;
         List<string> effectSelectList = new List<string>();

         private void EffectSerachBox_TextChanged(object sender, EventArgs e)
         {
             RichTextBox box = sender as RichTextBox;
             effectSelect.Items.Clear();
             if(string.IsNullOrEmpty(box.Text))
             {
                 foreach (string s in effectSelectList)
                 {
                      effectSelect.Items.Add(s);
                 }
                 return;
             }
             SerachText = box.Text.ToLower();
             foreach (string s in effectSelectList)
             {
                 string lowerText = s.ToLower();
                 //忽略大小写
                 if (lowerText.Contains(SerachText))
                 {
                     effectSelect.Items.Add(s);
                 }
             }
         }

         private void effectIDSerach_TextChanged(object sender, EventArgs e)
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

         private void EffectSerachIDButton_Click(object sender, EventArgs e)
         {
             //Table索引
             DataTable FoundTable = null;
             //Table的行索引
             int Rowindex = 0;
             bool bFound = false;
             foreach (DataTable ta in mEffectSet.Values)
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
                 string tableName = mEffect_ReSet[FoundTable];

                 Type t = WrapperReflection.gAssembly.GetType(ServerWapperEffectNameSpace + tableName);
                 if (t == null)
                 {
                     t = WrapperReflection.gAssembly.GetType(ClientWapperEffectNameSpace + tableName);
                     if (t == null)
                         return;
                 }

                 this.ConditionDesc.Text = WrapperReflection.getDesc(t);
                 EffectSerachBox.Text = WrapperReflection.getDesc(t);
                 DataTableSerializer.BuildDataGridViewFromDataTable(ref effectList, mEffectSet[tableName], t);
                 DataTableSerializer.FillDataGridViewFromDataTable(ref effectList, mEffectSet[tableName]);
                 DataTableSerializer.AddButtonToDataGridView(ref effectList, InsetButtonHeaderText, InsetButtonHeaderText, EffectInsetButtonCallBack);
                 effectList.Rows[Rowindex].Selected = true;

             }
             else
             {
                 Trace.Error("找不到效果ID");
             }
         }

        #endregion

        HeroEffectIDRange curIDRangeItem = null;
        private void eIDRangeCombobox_SelectedIndexChanged(object sender, EventArgs e)
        {
            curIDRangeItem = effectIDRangeDict[this.eIDRangeCombobox.SelectedItem.ToString()];
            this.eIDRangeLabel.Text = curIDRangeItem.minID + "~" + curIDRangeItem.maxID;
        }

        private IDAllocator GetIDAllocator(uint id)
        {
            foreach(KeyValuePair<HeroEffectIDRange,IDAllocator> kv in IDAllocatorList)
            {
                if (id <= kv.Key.maxID && id>=kv.Key.minID)
                {
                    return IDAllocatorList[kv.Key];
                }
            }
            Trace.Error("ID范围错误！！！！： " + id);
            return null;
        }
        private void AcceptAllocAll()
        {
            foreach (KeyValuePair<HeroEffectIDRange, IDAllocator> kv in IDAllocatorList)
            {
                kv.Value.AcceptAlloc();
            }
        }

        private void ClearIDAllocatorAll()
        {
            foreach (KeyValuePair<HeroEffectIDRange, IDAllocator> kv in IDAllocatorList)
            {
                kv.Value.ClearIDAllocator();
            }
        }

        private void CalculateUnUsedIDAll()
        {
            foreach (KeyValuePair<HeroEffectIDRange, IDAllocator> kv in IDAllocatorList)
            {
                kv.Value.CalculateUnUsedID();
            }
        }

        private void EffectForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)(27))
            {
                this.Close();
                return;
            }
        }
    }
}
