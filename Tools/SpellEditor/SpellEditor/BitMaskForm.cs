using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections.Specialized;

namespace SpellEditor
{
    public partial class BitMaskForm : Form
    {
        
        /// <summary>
        /// 所有的checkbox
        /// </summary>
        List<CheckBox> _checkboxes = new List<CheckBox>();

        /// <summary>
        /// 当前来源控件
        /// </summary>
        Control _dataSourceCtrl = null;
        DataTable _dataSource = null;
        int _nRow;
        int _nColumn;

        bool _canRefresh = false;

        public BitMaskForm()
        {
            InitializeComponent();
            InitializeControls();
        }

        public void InitializeControls()
        {
            // 查找所有的checkbox控件
            List<CheckBox> controls = new List<CheckBox>();
            foreach (Control ctrl in Controls)
            {
                if (ctrl is CheckBox)
                {
                    controls.Add((CheckBox)ctrl);
                }
            }
            _checkboxes = controls.OrderBy(u => Convert.ToInt32(u.Text.ToLower().Replace("checkbox", ""))).ToList(); ;

            for (int i = 0; i < _checkboxes.Count; ++i)
            {
                _checkboxes[i].Text = "";// (1 << i).ToString();
                _checkboxes[i].Tag = (1 << i);
            }

        }

        public void InitalizePanle(Type type, Control control, DataTable dataSource, int nRow, int nColumn)
        {
            _canRefresh = false;

            if (type == null)
            {
                return;
            }

            _dataSourceCtrl = control;
            _dataSource = dataSource;
            _nRow = nRow;
            _nColumn = nColumn;

            // 当前来源值
            int nMaskValue = 0;
            if (control is DataGridView)
            {
                nMaskValue = (int)((control as DataGridView).Rows[nRow].Cells[nColumn].Value); //
            }
            else
            {
                nMaskValue = (int)dataSource.Rows[nRow][nColumn];
            }

            this.Text = WrapperReflection.getDesc(type); 


            // 处理文本显示
            Dictionary<int, string> dic = GetNVCFromEnumValue(type);
            //var list = dic.OrderBy(s => s.Key);

            for (int i = 0; i < _checkboxes.Count; ++i)
            {
                int nVal = (int)_checkboxes[i].Tag;
                if ( dic.ContainsKey((int)_checkboxes[i].Tag) )
                {
                    _checkboxes[i].Text = dic[nVal];
                    _checkboxes[i].Visible = true;
                }
                else
                {
                    _checkboxes[i].Text = "";
                    _checkboxes[i].Visible = false;
                }
            }

            // 处理数值
            for (int i = 0; i < _checkboxes.Count; ++i)
            {
                int nValue = (int)(_checkboxes[i].Tag);
                _checkboxes[i].Checked = ((nValue & nMaskValue) == nValue);
            }

            _canRefresh = true;
        }

        /// <summary>
        /// 从枚举类型和它的特性读出并返回一个键值对
        /// </summary>
        /// <param name="enumType">Type,该参数的格式为typeof(需要读的枚举类型)</param>
        /// <returns>键值对</returns>
        public static Dictionary<int, string> GetNVCFromEnumValue(Type enumType)
        {
            Dictionary<int, string> dic = new Dictionary<int, string>();

            System.Reflection.FieldInfo[] fields = enumType.GetFields();
            string strText = string.Empty;
            int nValue = 0;
            foreach (System.Reflection.FieldInfo field in fields)
            {
                if (field.FieldType.IsEnum)
                {
                    nValue = (int)enumType.InvokeMember(field.Name, System.Reflection.BindingFlags.GetField, null, null, null);
                    strText = WrapperReflection.getFieldDesc(field);
                    dic.Add(nValue, strText);
                }
            }
            return dic;
        }

        private void BitMaskForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void checkBox_CheckedChanged(object sender, EventArgs e)
        {
            if (!_canRefresh)
                return;
            
            int nNewValue = 0;
            for (int i = 0; i < _checkboxes.Count; ++i)
            {
                int nVal = (int)_checkboxes[i].Tag;
                if (_checkboxes[i].Visible && _checkboxes[i].Checked)
                {
                    nNewValue += (int)_checkboxes[i].Tag;
                }
            }

            // 刷新来源控制上的数值
            if (_dataSourceCtrl is DataGridView)
            {
                (_dataSourceCtrl as DataGridView).Rows[_nRow].Cells[_nColumn].Value = nNewValue;
            }
            else 
            {
                if (_dataSource != null)
                {
                    _dataSource.Rows[_nRow][_nColumn] = nNewValue;
                }
            }

        }

        private void BitMaskForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)(27))
            {
                this.Close();
                return;
            }
        }

    }
}
