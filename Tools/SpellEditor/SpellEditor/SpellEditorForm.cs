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
//职业扩展需要修改的地方：
//1.Config.cs--中增加一个字段用于区分
//2.在InitConfig中把字段添加进list
//3.SpellEditorForm.cs--增加对应的Table用来存储对应的数据
//4.SpellEditorForm.cs--SpellEditorForm_Load中添加进去
//5.SpellEditorForm.cs--DvideDataTableWithVocation中清理table
//6.SpellEditorForm.cs--ChangeVocationFilter中添加对应的case
//7.SpellEditorForm.cs--UpdateVocationTableRowCountSet添加对应的table
namespace SpellEditor
{
    public partial class SpellEditorForm : Form
    {
        #region 其他

        public static SpellEditorForm Instance = null;
        public static RichTextBox LogArearInstance = null;
        public SpellLogicForm mLogicEditor = null;
        public ConditionForm mConditionEditor = null;
        public EffectForm mEffectEditor = null;
        public BitMaskForm mBitMaskForm = null;

        public const string conditionFileName = "Condition";
        public const string effectFileName = "Effect";
        public const string SpellFileName = "Spell";
        public const string VocationFileName = "HeroType";
        public const string SpellLogincFileName = "SpellLogic";

        public const string SCHEME_NAME = "SPELL_DATA";

        /// <summary>
        /// 是否在加载数据
        /// </summary>
        private bool isLoadingData = false;

        /// <summary>
        /// ID分配器
        /// </summary>
        private IDAllocator m_IDIDAllocator = new IDAllocator(IDAllocatorType.ID_POOL);

        /// <summary>
        /// 导出文件头
        /// </summary>
        private string[] headerText = new string[2];

        /// <summary>
        /// 编辑小按钮名字
        /// </summary>
        private const string EditButtonHaderText = "编辑";

        /// <summary>
        /// 新增小按钮名字
        /// </summary>
        private const string AddButtonHaderText = "新增";


        // 剪切板里的一行
        private DataRow clipboardRow = null;


        #endregion

        #region 职业分类
        /// <summary>
        /// 全部数据
        /// </summary>
        public static DataTable gSpell_All_DataList = null;

        public static DataTable[] gSpell_Vocation_DataList = new DataTable[VOCATION.VOCATION_MAX - VOCATION.VOCATION_INVALID];
 
        ///////////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// 职业名称与职业table
        /// </summary>
        private Dictionary<VOCATION, DataTable> VocationTableSet = new Dictionary<VOCATION, DataTable>();

        /// <summary>
        /// 职业table的行数
        /// </summary>
        private Dictionary<DataTable, int> VocationTableRowCountSet = new Dictionary<DataTable, int>();

        /// <summary>
        /// 当前职业过滤
        /// </summary>
        private string CurrentVocationFilter = Config.Vocation_Filter_ALL;

        /// <summary>
        /// 当前职业
        /// </summary>
        private VOCATION CurrentVocation = VOCATION.VOCATION_INVALID;

        #endregion

        public SpellEditorForm()
        {
            InitializeComponent();
            WrapperReflection.Load();

            mLogicEditor = new SpellLogicForm();
            mConditionEditor = new ConditionForm();
            mEffectEditor = new EffectForm();
            mBitMaskForm = new BitMaskForm();
            Instance = this;
        }

        private void SpellEditorForm_Load(object sender, EventArgs e)
        {
            string patch = "../Server/Scp/";
            patch += VocationFileName;
            patch += ".csv";
            DataTableSerializer.LoadVocationCSV(patch);

            Config.InitConfig();
            ExportPatchTextBox.Text = Config.GetExportPatch();
            ImportPatchTextBox.Text = Config.GetImportPatch();
            LogArearInstance = LogArear;

            BitMaskFlagHelper.PostSpell();

            BulidSpellDatable(ref gSpell_All_DataList, Config.Vocation_Filter_ALL);

            // Build所有职业
            for (VOCATION v = VOCATION.VOCATION_INVALID; v < VOCATION.VOCATION_MAX; v++)
            {
                BulidSpellDatable(ref gSpell_Vocation_DataList[(int)v], Config.GetVocationName(v));
            }

            // 绑定所有职业
            for (VOCATION v = VOCATION.VOCATION_INVALID; v < VOCATION.VOCATION_MAX; v++)
            {
                VocationTableSet.Add(v, gSpell_Vocation_DataList[(int)v]);
            }

            VocationTableSet = VocationTableSet.OrderBy(o => (int)o.Key).ToDictionary(p => p.Key, o => o.Value);    //按VOCATION排一次序

            DataTableSerializer.BuildDataGridViewFromDataTable(ref spellList, gSpell_All_DataList, typeof(SPELL.SPELL_DATA));
            DataTableSerializer.FillDataGridViewFromDataTable(ref spellList, gSpell_All_DataList);
            DataTableSerializer.AddButtonToDataGridView(ref spellList, EditButtonHaderText, EditButtonHaderText, EditButtonCallBack);
            DataTableSerializer.AddButtonToDataGridView(ref spellList, AddButtonHaderText, AddButtonHaderText, AddButtonCallBack);

            spellList.SelectionChanged +=spellList_SelectionChanged;

            CsvFilecheckBox.Checked = true;
            VocationFilterList.DataSource = Config.VocationFilterList;
         
            Trace.Normal("请点击加载按钮加载配置文件...");
        }

        #region 界面消息

        private void spellList_SelectionChanged(object sender, EventArgs e)
        {
            DataGridView gridview = sender as DataGridView;
            if (gridview.SelectedCells.Count > 0)
            {
                if (gridview.SelectedCells[0] != null)
                {
                    int index = gridview.SelectedCells[0].RowIndex;
                    gridview.Rows[index].Selected = true;
                }
            }
        }

        /// <summary>
        /// DataGridView中的编辑按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private  void EditButtonCallBack(object sender, DataGridViewCellMouseEventArgs e)
        {
            DataGridView grid = (DataGridView)sender;

            DataTable ta = (DataTable)grid.DataSource;
            if (ta.Rows.Count > 0 && e.ColumnIndex >= 0 && e.RowIndex >= 0 && ta.Rows.Count > e.RowIndex)
            {
                if (grid.Columns[e.ColumnIndex].HeaderText.Equals(EditButtonHaderText))
                {
                    EditLogic(grid.Rows[e.RowIndex].Cells[0].Value);
                }

            }

        }

        /// <summary>
        /// DataGridView中的添加按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AddButtonCallBack(object sender, DataGridViewCellMouseEventArgs e)
        {
            DataGridView grid = (DataGridView)sender;
            DataTable ta = spellList.DataSource as DataTable;
            if (ta.Rows.Count > 0 && e.ColumnIndex >= 0 && e.RowIndex >= 0 && ta.Rows.Count > e.RowIndex)
            {
                if (grid.Columns[e.ColumnIndex].HeaderText.Equals(AddButtonHaderText))
                {
                    DataRow _row = null;
                    //是空的，直接新建一行
                    if (ta.Rows.Count <= 0)
                    {
                        _row = DataTableSerializer.FillDefaultNewRow(ref ta);
                    }
                    //非空复制上一行
                    else
                    {
                        _row = DataTableSerializer.CopyRowAt(ref ta, e.RowIndex);
                    }
                    m_IDIDAllocator.AcceptAlloc();
                    ta.Rows.InsertAt(_row,e.RowIndex+1);
                    int spellID = (int)_row[0];

                    if (ta != gSpell_All_DataList)
                    {
                        //对全局数据进行排序，更新
                        DeleteTableEvent(gSpell_All_DataList);

                        SortDataTableByASC(ref gSpell_All_DataList);

                        AddTableEvent(gSpell_All_DataList);
                    }
                    DvideDataTableWithVocation();
                    SortDataTableByASC(ref ta);

                    DataRow myrow = ta.Select("nID=" + spellID)[0];
                    int newRowIndex = ta.Rows.IndexOf(myrow);
                    //默认选中第一行
                    this.spellList[0, 0].Selected = false;
                    if (this.spellList.Rows.Count > 1)
                    {
                        this.spellList.Rows[newRowIndex].Selected = true;
                    }
                    else
                    {
                        this.spellList.Rows[0].Selected = true;
                    }
                    this.spellList.FirstDisplayedScrollingRowIndex = newRowIndex-1;
                }

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
            
            if (VocationTableRowCountSet.ContainsKey(dt))
            {
                if ( VocationTableRowCountSet[dt] != dt.Rows.Count)
                {
                    m_IDIDAllocator.AcceptAlloc();
                }

                DataRow row = e.Row;

                DeleteTableEvent(dt);
                DataTableSerializer.FillDefaultRow(ref row, dt);
                AddTableEvent(dt);

                ChangedRowWithVocation(row, dt);
                UpdateVocationTableRowCountSet();
            }
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
            if (VocationTableRowCountSet.ContainsKey(dt))
            {
                if (VocationTableRowCountSet[dt] != dt.Rows.Count)
                {
                    DeleteRowWithVocation(dt);
                    UpdateVocationTableRowCountSet();
                }
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
            m_IDIDAllocator.PushUnUsedId(Convert.ToUInt32(e.Row[0].ToString()));
            
        }

        /// <summary>
        /// 正在更改一行的值
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Row_NewEvent(object sender, DataTableNewRowEventArgs e)
        {
            if (isLoadingData)
                return;
            //为这一行的ID赋值
            e.Row[0] = m_IDIDAllocator.AllocId();
            e.Row.BeginEdit();
        }

        /// <summary>
        /// 添加一行按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CopyLineButton_Click(object sender, EventArgs e)
        {
            CopyLine();
            return;
        }


        /// <summary>
        /// 编辑技能逻辑按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LogicEditorButton_Click(object sender, EventArgs e)
        {
            Object selectIndex = null;
            if (this.spellList.SelectedRows.Count <= 0)
            {
                if (this.spellList.SelectedCells.Count <= 0)
                {
                    return;
                }
                selectIndex = this.spellList.SelectedCells[0].OwningRow.Cells[0].Value;
            }
            else
            {
                selectIndex = this.spellList.SelectedRows[0].Cells[0].Value;
            }

            EditLogic(selectIndex);
            return;
        }

        /// <summary>
        /// 导出按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExportButton_Click(object sender, EventArgs e)
        {
            DataTable ta = spellList.DataSource as DataTable;
            SortDataTable(ref ta);
            ExportConfig();
            return;
        }

        /// <summary>
        /// 加载按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LoadButton_Click(object sender, EventArgs e)
        {
            LoadConfig();
            return;
        }

        /// <summary>
        /// Bin确认框
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BinFileCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (BinFileCheckBox.Checked)
            {
                CsvFilecheckBox.Checked = false;
                DataTableSerializer.SetFileType(DataTableSerializer.FileType.FILETYPE_BIN);
            }
            else
            {
                CsvFilecheckBox.Checked = true;
            }

        }


        /// <summary>
        /// Csv确认框
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CsvFilecheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (CsvFilecheckBox.Checked)
            {
                BinFileCheckBox.Checked = false;
                DataTableSerializer.SetFileType(DataTableSerializer.FileType.FILETYPE_CSV);
            }
            else
            {
                BinFileCheckBox.Checked = true;
            }
        }


        /// <summary>
        /// 导入路径的浏览按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ImportPatchButton_Click(object sender, EventArgs e)
        {
            string s= Config.ChangeImportPatch();
            if (!string.IsNullOrEmpty(s))
                ImportPatchTextBox.Text = s;

        }


        /// <summary>
        /// 导出路径的浏览按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExportPatchButton_Click(object sender, EventArgs e)
        {
            string s = Config.ChangeExportPatch();
            if(!string.IsNullOrEmpty(s))
                ExportPatchTextBox.Text = s;

        }

        /// <summary>
        /// 导入路径框
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ImportPatchTextBox_TextChanged(object sender, EventArgs e)
        {

            if (null == LogArearInstance)
                return;
            if (!string.IsNullOrEmpty(this.ImportPatchTextBox.Text))
            {
                Config.ChangeImportPatch(this.ImportPatchTextBox.Text);
            }
                
        }

        /// <summary>
        /// 导出路径框
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExportPatchTextBox_TextChanged(object sender, EventArgs e)
        {

            if (null == LogArearInstance)
                return;
            if (!string.IsNullOrEmpty(this.ExportPatchTextBox.Text))
            {
                Config.ChangeExportPatch(this.ExportPatchTextBox.Text);
            }
        }

        /// <summary>
        /// 职业过滤
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void VocationFilterList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ChangeVocationFilter(VocationFilterList.SelectedItem.ToString());

        }

        /// <summary>
        /// 算号器
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CalculatorButton_Click(object sender, EventArgs e)
        {
            CalculatorForm cf = new CalculatorForm();
            cf.Show();
        }

        
        #endregion

        #region Editor

        /// <summary>
        /// 复制
        /// </summary>
        private void CopyLine()
        {
            DataTable destTable = spellList.DataSource as DataTable;
            if (null == destTable)
                return;

            
            DataRow _row = null;
            //是空的，直接新建一行
            if (destTable.Rows.Count <= 0)
            {
                _row = DataTableSerializer.FillDefaultNewRow(ref destTable);
            }
            //非空复制上一行
            else
            {
                _row = DataTableSerializer.CopySelfRow(ref destTable);
            }
            if (CurrentVocation != VOCATION.VOCATION_MAX)
            {
                _row[1] = (int)CurrentVocation;
            }
            destTable.Rows.Add(_row);

            DvideDataTableWithVocation();
            

            //默认选中第一行
            this.spellList[0, 0].Selected = false;
            if (this.spellList.Rows.Count > 1)
            {
                this.spellList.Rows[this.spellList.Rows.Count - 2].Selected = true;
            }
            else
            {
                this.spellList.Rows[0].Selected = true;
            }
            //ExportButton.Enabled = true;
            //LogicEdtiorButton.Enabled = true;
        }

        /// <summary>
        /// 编辑逻辑
        /// </summary>
        /// <param name="Index"></param>
        private void EditLogic(object Index)
        {

            int id = Convert.ToInt32(Index);
            mLogicEditor.ShowSpell(id);

            mLogicEditor.ShowDialog(this);
        }

        /// <summary>
        /// 构建spell用的datatable
        /// </summary>
        /// <param name="ta"></param>
        /// <param name="tableName"></param>
        private void BulidSpellDatable(ref DataTable table, string tableName)
        {
            if (null == table)
            {
                table = new DataTable();
            }

            Type spell_data = typeof(SPELL.SPELL_DATA);

            FieldInfo[] fields = spell_data.GetFields();

            //加入ID,Vocation：
            FieldInfo f0 = fields[0];
            DataColumn da0 = table.Columns.Add(f0.Name, f0.FieldType);
            da0.Caption = WrapperReflection.getFieldDesc(f0);
            headerText[0] = da0.Caption;
            headerText[1] = f0.Name;

            FieldInfo f1 = fields[1];
            DataColumn da1 = table.Columns.Add(f1.Name, f1.FieldType);
            da1.Caption = WrapperReflection.getFieldDesc(f1);
            headerText[0] += "," + da1.Caption;
            headerText[1] += "," + f1.Name;

            //加入备注：
            string caption = "备注";
            string name = "strRemark";
            DataColumn da2 = table.Columns.Add(name, typeof(string));
            da2.Caption = caption;
            headerText[0] += "," + da2.Caption;
            headerText[1] += "," + name;

            //加入其它字段
            for (int i = 2; i < fields.Length; i++)
            {
                FieldInfo f = fields[i];
                DataColumn da = table.Columns.Add(f.Name, f.FieldType);
                da.Caption = WrapperReflection.getFieldDesc(f);

                headerText[0] += ',';
                headerText[1] += ',';

                headerText[0] += da.Caption;
                headerText[1] += f.Name;
            }

            //第一列是ID，不能在编辑器中编辑，所以是只读的
            //ta.Columns[0].ReadOnly = true;

            VocationTableRowCountSet.Add(table, -1);

            AddTableEvent(table);
            table.TableName = tableName;

        }

        /// <summary>
        /// 把DataTable里的数据按职业切分
        /// </summary>
        private void DvideDataTableWithVocation()
        {
            // 清空所有职业数据
            for (VOCATION v = VOCATION.VOCATION_INVALID; v < VOCATION.VOCATION_MAX; v++)
            {
                gSpell_Vocation_DataList[(int)v].Clear();
            }

            foreach (DataRow dr in gSpell_All_DataList.Rows)
            {
                int voc = Int32.Parse(dr[1].ToString());
                VOCATION v = (VOCATION)voc;
                object[] data = dr.ItemArray;
                DeleteTableEvent(VocationTableSet[v]);
                DataRow r = VocationTableSet[v].NewRow();
                r.ItemArray = data;
                VocationTableSet[v].Rows.Add(r);
                //恢复事件
                AddTableEvent(VocationTableSet[v]);
            }
        }

        /// <summary>
        /// 根据职业类型改变对应的DataTable的Row
        /// </summary>
        /// <param name="dr">改变的Row</param>
        /// <param name="hostTable">Row所属的Table</param>
        private void ChangedRowWithVocation(DataRow dr,DataTable hostTable)
        {
            int voc = Int32.Parse(dr[1].ToString());
            VOCATION v = (VOCATION)voc;

            object[] data = dr.ItemArray;
            //源table和目标table不是同一个

            if (!hostTable.TableName.Equals(VocationTableSet[v].TableName))
            {
                //这里不需要事件的参与
                DeleteTableEvent(VocationTableSet[v]);
                DataRow r = VocationTableSet[v].NewRow();
                r.ItemArray = data;
                VocationTableSet[v].Rows.Add(r);
                //恢复事件
                AddTableEvent(VocationTableSet[v]);

                //如果源table不是all-table，将行从源table中删除。
                if (!gSpell_All_DataList.TableName.Equals(hostTable.TableName))
                {
                    DeleteTableEvent(hostTable);
                    hostTable.Rows.Remove(dr);

                    //根据所有职业的table构建出总的table
                    BuildAllVocationTableFromSubVocationTable();

                    spellList.Refresh();
                    AddTableEvent(hostTable);

                }
            }

            //不是在所有职业的table下修改数据
            if (!gSpell_All_DataList.TableName.Equals(hostTable.TableName))
            {
                int VocationCount = 0;
                foreach (DataTable t in VocationTableSet.Values)
                {
                    VocationCount += t.Rows.Count;
                }
                //所有职业的table行数与 all-table行数相等，则修改数据
                if (VocationCount != gSpell_All_DataList.Rows.Count)
                {
                    DeleteTableEvent(gSpell_All_DataList);
                    DataRow r = gSpell_All_DataList.NewRow();
                    r.ItemArray = data;
                    gSpell_All_DataList.Rows.Add(r);
                    AddTableEvent(gSpell_All_DataList);
                }
                else
                {

                }
            }
            else
            {
                //重新切分数据
                DvideDataTableWithVocation();
            }
        }

        /// <summary>
        /// 根据职业类型删除对应的DataTable的Row
        /// </summary>
        /// <param name="hostTable">Row所属的Table</param>
        private void DeleteRowWithVocation(DataTable hostTable)
        {
            DeleteTableEvent(hostTable);

            //如果删除的是all-table里的数据
            if (hostTable.TableName.Equals(gSpell_All_DataList.TableName))
            {
                //重新切分数据
                DvideDataTableWithVocation();
            }
            else
            {
                //根据所有职业的table构建出总的table
                BuildAllVocationTableFromSubVocationTable();
            }

            AddTableEvent(hostTable);

        }

        /// <summary>
        /// 改变职业过滤
        /// </summary>
        /// <param name="filter"></param>
        private void ChangeVocationFilter(string filter)
        {
            if (filter != Config.Vocation_Filter_ALL)
            {
                bool isFind = false;

                foreach (KeyValuePair<string, string> kv in DataTableSerializer.vocationNameDict)
                {
                    if (kv.Value == filter)
                    {
                        filter = kv.Key;
                        isFind = true;
                        break;

                    }
                }
                if (!isFind)
                    return;
            }

            switch (filter)
            {
                case Config.Vocation_Filter_ALL:
                    {
                        BuildAllVocationTableFromSubVocationTable();
                        DataTableSerializer.FillDataGridViewFromDataTable(ref spellList, gSpell_All_DataList);
                        CurrentVocation = VOCATION.VOCATION_MAX;
                    }
                    break;
               
                default:
                    {
                        VOCATION v = Config.GetVocationByName(filter);
                        DataTableSerializer.FillDataGridViewFromDataTable(ref spellList, VocationTableSet[v]);
                        CurrentVocation = v;
                    }
                    break;
            }
            CurrentVocationFilter = filter;
        }

        /// <summary>
        /// 更新TableRowCountSet
        /// </summary>
        private void UpdateVocationTableRowCountSet()
        {
            VocationTableRowCountSet[gSpell_All_DataList] = gSpell_All_DataList.Rows.Count;

            for (VOCATION v = VOCATION.VOCATION_INVALID; v < VOCATION.VOCATION_MAX; v++)
            {
                DataTable dt = gSpell_Vocation_DataList[(int)v];
                VocationTableRowCountSet[dt] = dt.Rows.Count;
            }
        }

        /// <summary>
        /// 从各个职业的table构建出all-table
        /// </summary>
        private void BuildAllVocationTableFromSubVocationTable()
        {
            if (isLoadingData)
                return;
            DeleteTableEvent(gSpell_All_DataList);
            gSpell_All_DataList.Clear();
            var VocationTableSet_SortedByKey = (from n in VocationTableSet

                                               orderby ((int)n.Key) ascending

                                   select n);
            foreach (KeyValuePair<VOCATION,DataTable> kv in VocationTableSet_SortedByKey)
            {
                DataTable dt = kv.Value;
                foreach (DataRow dr in dt.Rows)
                {
                    gSpell_All_DataList.Rows.Add(dr.ItemArray);
                }
            }
            AddTableEvent(gSpell_All_DataList);
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

        #endregion

        #region 导入导出

        /// <summary>
        /// 加载配置
        /// </summary>
        private void LoadConfig()
        {
            if (null == gSpell_All_DataList)
                return;
            string patch = ImportPatchTextBox.Text;

            if (string.IsNullOrEmpty(patch))
                return;

            //bin和csv逻辑文件判断
            if (DataTableSerializer.FileType.FILETYPE_INVALID == DataTableSerializer.CurrFileType ||
                DataTableSerializer.FileType.FILETYPE_MAX == DataTableSerializer.CurrFileType)
            {
                Trace.Error("您所打开的文件格式不正确，请确认：" + DataTableSerializer.CurrFileType.ToString());
                return;
            }

            isLoadingData = true;
            if (DataTableSerializer.FileType.FILETYPE_CSV == DataTableSerializer.CurrFileType)
            {
                if (!LoadConfigFromCSV(patch))
                {
                    isLoadingData = false;
                    return;
                }

            }
            else if (DataTableSerializer.FileType.FILETYPE_BIN == DataTableSerializer.CurrFileType)
            {
                if (!LoadConfigFromBin(patch))
                {
                    isLoadingData = false;
                    return;
                }

            }
            Trace.Normal("加载配置文件成功!");

            DvideDataTableWithVocation();
            ChangeVocationFilter(CurrentVocationFilter);

            //默认选中最后第一行
            this.spellList[0, 0].Selected = false;
            if (this.spellList.Rows.Count > 1)
            {
                this.spellList.Rows[this.spellList.Rows.Count - 2].Selected = true;
            }
            else
            {
                this.spellList.Rows[0].Selected = true;
            }
            isLoadingData = false;
        }

        /// <summary>
        /// 导出配置
        /// </summary>
        private void ExportConfig()
        {
            if (null == gSpell_All_DataList)
                return;

            string patch = ExportPatchTextBox.Text;

            if (string.IsNullOrEmpty(patch))
                return;

            //bin和csv逻辑文件判断
            if (DataTableSerializer.FileType.FILETYPE_INVALID == DataTableSerializer.CurrFileType ||
                DataTableSerializer.FileType.FILETYPE_MAX == DataTableSerializer.CurrFileType)
            {
                Trace.Error("保存配置失败,文件格式错误：" + DataTableSerializer.CurrFileType.ToString());
                return;
            }

            //如果不是当前的filter不是所有职业，合并一下
            if (CurrentVocation != VOCATION.VOCATION_MAX)
            {
                BuildAllVocationTableFromSubVocationTable();
            }

            SortDataTable(ref gSpell_All_DataList);
            //if (DataTableSerializer.FileType.FILETYPE_CSV == DataTableSerializer.CurrFileType)
            {
                if (!ExportConfigToCSV(patch))
                {
                    return;
                }

            }
            //else if (DataTableSerializer.FileType.FILETYPE_BIN == DataTableSerializer.CurrFileType)
            {
                if (!ExportConfigToBin(patch))
                {
                    return;
                }

            }
        }


        /// <summary>
        /// 从bin加载配置
        /// </summary>
        /// <param name="patch"></param>
        /// <returns></returns>
        private bool LoadConfigFromBin(string patch)
        {
            string FullPatch = patch;
            FullPatch += SpellFileName;
            FullPatch += ".bin";
            //技能配置
            if (!LoadSpellTableFromBin(FullPatch))
            {
                Trace.Error("加载bin格式skill配置文件失败：" + FullPatch);
                return false;
            }

            FullPatch = patch;
            FullPatch += effectFileName;
            FullPatch += ".bin";

            //效果配置
            if (!mEffectEditor.LoadEffectTableFromBin(FullPatch))
            {
                Trace.Error("加载bin格式effect配置文件失败：" + FullPatch);
                return false;
            }

            FullPatch = patch;
            FullPatch += conditionFileName;
            FullPatch += ".bin";

            //条件配置
            if (!mConditionEditor.LoadCoditionTableFromBin(FullPatch))
            {
                Trace.Error("加载bin格式condition配置文件失败：" + FullPatch);
                return false;
            }

            FullPatch = patch;
            FullPatch += SpellLogincFileName;
            FullPatch += ".csv";
            //逻辑
            if (!mLogicEditor.LoadSpellLogicFromCsv(FullPatch))
            {
                Trace.Error("加载csv格式spelllogic配置文件失败：" + FullPatch);
                return false;
            }
            return true;
        }

        /// <summary>
        /// 从csv加载配置
        /// </summary>
        /// <param name="patch"></param>
        /// <returns></returns>
        private bool LoadConfigFromCSV(string patch)
        {
            bool bResult = true;
            string FullPatch = patch;
            FullPatch += SpellFileName;
            FullPatch += ".csv";
            //技能配置
            if (!LoadSpellTableFromCsv(FullPatch))
            {
                Trace.Error("加载csv格式skill配置文件失败：" + FullPatch);
                bResult =  false;
            }

            FullPatch = patch;
            FullPatch += conditionFileName;
            FullPatch += ".csv";
            //条件配置
            if (!mConditionEditor.LoadCoditionTableFromCsv(FullPatch))
            {
                Trace.Error("加载csv格式condition配置文件失败：" + FullPatch);
                bResult = false;
            }

            //效果配置
            FullPatch = patch;
            FullPatch += effectFileName;
            FullPatch += ".csv";
            if (!mEffectEditor.LoadEffectTableFromCsv (FullPatch))
            {
                Trace.Error("加载csv格式effect配置文件失败：" + FullPatch);
                bResult = false;
            }


            FullPatch = patch;
            FullPatch += SpellLogincFileName;
            FullPatch += ".csv";
            //逻辑
            if (!mLogicEditor.LoadSpellLogicFromCsv(FullPatch))
            {
                Trace.Error("加载csv格式spelllogic配置文件失败：" + FullPatch);
                bResult = false;
            }
            return bResult;
        }


        /// <summary>
        /// 将配置导出到bin
        /// </summary>
        /// <param name="patch"></param>
        /// <returns></returns>
        private bool ExportConfigToBin(string patch)
        {
            string FullPatch = patch;
            FullPatch += SpellFileName;
            FullPatch += ".bin";

            ExportSpellToBin(FullPatch);



            //效果配置

            FullPatch = patch;
            FullPatch += effectFileName;
            FullPatch += ".bin";

            mEffectEditor.ExportEffectTableToBin(FullPatch);


            FullPatch = patch;
            FullPatch += conditionFileName;
            FullPatch += ".bin";

            //条件配置
            mConditionEditor.ExportCoditionTableToBin(FullPatch);

            ////导出bin也导出logic的csv
            //FullPatch = patch;
            //FullPatch += SpellLogincFileName;
            //FullPatch += ".csv";

            //mLogicEditor.ExportSpellLogicToCsv(FullPatch);
            return true;
        }


        /// <summary>
        /// 将配置导出到CSV
        /// </summary>
        /// <param name="patch"></param>
        /// <returns></returns>
        private bool ExportConfigToCSV(string patch)
        {
            string FullPatch = patch;
            FullPatch += SpellFileName;
            FullPatch += ".csv";

            ExportSpellToCSV(FullPatch);

            FullPatch = patch;
            FullPatch += conditionFileName;
            FullPatch += ".csv";

            //条件配置
            mConditionEditor.ExportCoditionTableToCsv(FullPatch);


           //效果配置
            FullPatch = patch;
            FullPatch += effectFileName;
            FullPatch += ".csv";
            mEffectEditor.ExportEffectTableToCsv(FullPatch);



            FullPatch = patch;
            FullPatch += SpellLogincFileName;
            FullPatch += ".csv";

            mLogicEditor.ExportSpellLogicToCsv(FullPatch);

            return true;
        }


        private bool LoadSpellTableFromBin(string FullPatch)
        {
            m_IDIDAllocator.ClearIDAllocator();
            //技能配置
            if (!DataTableSerializer.LoadBinToTable(ref gSpell_All_DataList, FullPatch, WrapperReflection.gAssembly.GetType("SPELL.SPELL_DATA")))
            {
                return false;
            }
            if (gSpell_All_DataList.Rows.Count > 0)
            {
                for (int i = 0; i < gSpell_All_DataList.Rows.Count; i++)
                {
                    //取出最大的ID
                    uint Id = Convert.ToUInt32(gSpell_All_DataList.Rows[i][0]);
                    m_IDIDAllocator.PushUsedId(Id);
                }

            }
            m_IDIDAllocator.CalculateUnUsedID();
            return true;
        }

        private bool LoadSpellTableFromCsv(string FullPatch)
        {
            m_IDIDAllocator.ClearIDAllocator();
            //技能配置
            if (!DataTableSerializer.LoadCSVToTable(ref gSpell_All_DataList, FullPatch, 2, true, headerText[1].Split(',')))
            {
                return false;
            }
            SortDataTable(ref gSpell_All_DataList);
            if (gSpell_All_DataList.Rows.Count > 0)
            {
                for (int i = 0; i < gSpell_All_DataList.Rows.Count; i++)
                {
                    //取出最大的ID
                    uint Id = Convert.ToUInt32(gSpell_All_DataList.Rows[i][0]);
                    m_IDIDAllocator.PushUsedId(Id);
                }

            }
            m_IDIDAllocator.CalculateUnUsedID();
            return true;
        }

        private void ExportSpellToCSV(string FullPatch)
        {
            if (gSpell_All_DataList.Rows.Count > 0)
            {
                CheckDatas(gSpell_All_DataList.Rows);
                //技能配置
                if (!DataTableSerializer.SaveToCSV(gSpell_All_DataList, FullPatch, headerText))
                {
                    Trace.Error("保存配置失败:" + FullPatch);
                }
                else
                {
                    Trace.Normal("保存配置成功:" + FullPatch);
                }

            }
            else
            {
                Trace.Warring("文件为空：" + FullPatch);
            }
        }

        private static bool CheckDatas(DataRowCollection rows)
        {
            for (int i = 0; i < rows.Count; i++)
            {
                // 准备时间
                int preTimeCol = (int)SPELL.SPELL_DATA_ID.SPDATA_PREPARE_TIME + 1;
                int preTime = int.Parse(rows[i][preTimeCol].ToString());

                // 持续时间
                int durationTimeCol = (int)SPELL.SPELL_DATA_ID.SPDATA_DURATION + 1;
                int durationTime = int.Parse(rows[i][durationTimeCol].ToString());

                // 僵持时间
                int rigdityTimeCol = (int)SPELL.SPELL_DATA_ID.SPDATA_RIGIDITY + 1;
                int rigdityTime = int.Parse(rows[i][rigdityTimeCol].ToString());

                //准备时间不可以同时大于僵直时间和持续时间
                if ( preTime > durationTime  && preTime > rigdityTime)
                {
                    Trace.Error("保存配置失败:   技能ID："+ rows[i][0] +  "  ---行数: " + i);
                    Trace.Error("准备时间不可以同时大于僵直时间和持续时间!");
                    return false;
                }
            }
            return true;
        }

        private void ExportSpellToBin(string FullPatch)
        {
            if (gSpell_All_DataList.Rows.Count > 0)
            {
                //技能配置
                if (!DataTableSerializer.SaveToBin(gSpell_All_DataList, FullPatch, WrapperReflection.gAssembly.GetType("SPELL.SPELL_DATA")))
                {
                    Trace.Error("保存配置失败:" + FullPatch);
                }
                Trace.Normal("保存配置成功:" + FullPatch);
            }
            else
            {
                Trace.Warring("文件为空：" + FullPatch);
            }
        }
        #endregion

        private void spellList_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if(!e.Control)
            {
                return;
            }

            switch(e.KeyCode)
            {
                case Keys.C:
                    {
                        clipboardRow = null;

                        DataTable srcTable = spellList.DataSource as DataTable;
                        if (srcTable == null)
                        {
                            Trace.Error("spellList_PreviewKeyDown---srcTable == null");
                            return;
                        }

                        DataGridViewRow curRow = this.spellList.CurrentRow;
                        if (curRow.Index >= srcTable.Rows.Count)
                        {
                            Trace.Warring("spellList_PreviewKeyDown---curRow.Index >= srcTable.Rows.Count");
                            return;
                        }

                        // ctrl + c 不生成技能ID
                        srcTable.TableNewRow -= Row_NewEvent;

                        clipboardRow = srcTable.NewRow();
                        DataRow copyRow = srcTable.Rows[curRow.Index];

                        for (int i = 0; i < srcTable.Columns.Count; ++i)
                        {
                            clipboardRow[i] = copyRow[i];
                        }

                        // 恢复
                        srcTable.TableNewRow += Row_NewEvent;
                    }
                    break;

                case Keys.V:
                    {
                        if(clipboardRow == null)
                        {
                            return;
                        }

                        m_IDIDAllocator.AcceptAlloc();

                        DataGridViewRow curRow = this.spellList.CurrentRow;

                        DataTable destTable = spellList.DataSource as DataTable;

                        DataRow newRow = destTable.NewRow();
                        for (int i = 1; i < destTable.Columns.Count; ++i)
                        {
                            newRow[i] = clipboardRow[i];
                        }
                        destTable.Rows.InsertAt(newRow, curRow.Index);

                        DvideDataTableWithVocation();

                        // 拷贝技能logic
                        mLogicEditor.CopySpellLogic((int)clipboardRow[0], (int)newRow[0]);
                    }
                    break;
            }
        }


        private void SortDataTableByASC(ref DataTable ta)
        {
            ta.AsDataView().Sort = "nVocation,strRemark ASC";
            ta = ta.DefaultView.ToTable();
            spellList.DataSource = ta;
            DeleteTableEvent(ta);
            AddTableEvent(ta);
        }

        private void SortDataTable(ref DataTable ta)
        {
            //ta.AsDataView().Sort = "nVocation,strRemark ASC";
            DataTable tempT = new DataTable();
            tempT = ta.Copy();
            //按照备注的字符串切分排序：NUM0_NUM1_CONTENT

            //ArrayList list = new ArrayList(tempT.Rows);
            List<DataRow> drList = new List<DataRow>();
            foreach(DataRow dr in tempT.Rows)
            {
                drList.Add(dr);
            }
            //实例化MyCompare
            RemarkComparer com = new RemarkComparer();
            drList.Sort(com);
            //ta.LoadDataRow(list.ToArray(),LoadOption.OverwriteChanges);
            DeleteTableEvent(ta);
            DateTime startTime = DateTime.Now;
            ta.Clear();
            for (int i = 0; i < drList.Count; i++) {
                //ta.Rows[i].ClearErrors();
                //ta.Rows[i].ItemArray =drList[i].ItemArray;
                //drList[i].ItemArray.CopyTo(ta.Rows[i].ItemArray,0);

                DataRow dr = ta.NewRow();
                dr.ItemArray = drList[i].ItemArray;
                ta.Rows.InsertAt(dr, i);
            }

            TimeSpan et = (DateTime.Now - startTime);
            Trace.Normal("use Time: " + et.ToString());
            //ta = ta.DefaultView.ToTable();    //不能加，加了会new一个新的table，就导致VocationSet引用错误，此时导致任意修改都不保存

            //重置spellList(当前表)的DataSource，不然排序后会新建一个ta，以前的datasource将会丢失，此时的任意修改都不会保存
            spellList.DataSource = ta;

            AddTableEvent(ta);
        }

        class RemarkComparer : IComparer<DataRow>//自定义排序的Compare类：按照职业，备注排序
        {
            private CaseInsensitiveComparer ObjectCompare;
            public RemarkComparer()
            {
                ObjectCompare = new CaseInsensitiveComparer();
            }
            public int Compare(DataRow x, DataRow y)//比较大小
            {
                DataRow rowA = (DataRow)x, rowB = (DataRow)y;
                int vocationA = int.Parse(rowA["nVocation"].ToString());
                int vocationB = int.Parse(rowB["nVocation"].ToString());
                string remarkA = rowA["strRemark"].ToString();
                string remarkB = rowB["strRemark"].ToString();
                try
                {

                    if (vocationA != vocationB)
                    {
                        return vocationA.CompareTo(vocationB);
                    }
                    else
                    {
                        string[] splitsA = remarkA.Split('_');
                        string[] splitsB = remarkB.Split('_');
                        if (splitsA[0] == "empty")
                        {
                            return 1;
                        }
                        else if(splitsB[0] == "empty")
                        {
                            return -1;
                        }
                        int n = 0;
                        bool flag = false;
                        int result = 0;
                        while (!flag && n < splitsA.Length-1 && n < splitsB.Length-1)
                        {
                            result = int.Parse(splitsA[n]) - int.Parse(splitsB[n]);
                            if (result != 0)
                            {
                                result = (result > 0 ? 1 : -1);  //数字越小越前面
                                flag = true;    //知道顺序了
                            }
                            else
                                n++;
                        }
                        //前面一整串字符都相同，那么就判断_的个数，少的排在前面
                        if (!flag)
                        {
                            return (splitsA.Length <= splitsB.Length ? 1 : -1);
                        }
                        else
                        {
                            return result;
                        }
                    }
                }
                catch (Exception e)
                {
                    Trace.Error(e.Message + ": " + vocationA + " " + vocationB + " " + remarkA + " " + remarkB);
                    return 0;
                }
            }                                   
        }

        #region 搜索

        private int SearchId = -1;
        private void SerachBox_TextChanged(object sender, EventArgs e)
        {
            RichTextBox box = sender as RichTextBox;
            if (string.IsNullOrEmpty(box.Text))
            {
                return;
            }

            if (!int.TryParse(box.Text, out SearchId))
            {
                Trace.Warring("搜索--请输入整数");
                SearchId = -1;
                return;
            }
        }

        private void SerachButton_Click(object sender, EventArgs e)
        {
            if (SearchId < 0)
            {
                return;
            }

            foreach (DataGridViewRow row in spellList.Rows)
            {
                if (row.Cells[0].Value == null)
                {
                    break;
                }

                if (row.Cells[0].Value.Equals(SearchId))
                {
                    row.Selected = true;
                    this.spellList.FirstDisplayedScrollingRowIndex = row.Index;
                    return;
                }
            }
            Trace.Warring("搜索--找不到技能ID:" + SearchId);
        }

        #endregion


        private void spellList_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex == -1)
                return;

            DataGridView dgv = (DataGridView)sender;

            DataTable dataSource = dgv.DataSource as DataTable;
            if (dataSource.Rows.Count == 0)
                return;

            object oval = dataSource.Rows[e.RowIndex][e.ColumnIndex];
            object cellVal = dgv.Rows[e.RowIndex].Cells[e.ColumnIndex].Value;

            if (oval != null && cellVal is int)
            {
                Type type = BitMaskFlagHelper.Get(SCHEME_NAME, e.ColumnIndex);
                if (type == null)
                {
                    return;
                }

                if (type.IsEnum)
                {
                    mBitMaskForm.InitalizePanle(type, dgv, dataSource, e.RowIndex, e.ColumnIndex);
                    mBitMaskForm.ShowDialog();
                    //mBitMaskForm.Show();
                } 
            }
        }

    }
}


