using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ServiceBuild
{
    public partial class ServiceBuildForm : Form
    {
        static System.Windows.Forms.OpenFileDialog openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
        static string strOutput = "";

        public ServiceBuildForm()
        {
            InitializeComponent();
        }

        static public void output(string txt)
        {
            strOutput += txt;
            strOutput += "\r\n";
        }

        private void Build_Button_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "C#文件(*.cs)|*.cs|所有文件(*.*)|*.*";

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string FileName = openFileDialog1.FileName;

                int pos = FileName.LastIndexOf('\\');
                string path_name = FileName.Substring(0, pos);
                string type_name = FileName.Substring(pos + 1);
                type_name = type_name.Substring(0, type_name.Length - 3);

                Type t = ServiceBuilder.load_csharp_file(FileName, type_name);
                ServiceBuilder.build(t, path_name);
            }
        }

        private void Build_Cpp_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "C++文件(*.h)|*.h|所有文件(*.*)|*.*";

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string FileName = openFileDialog1.FileName;

                int pos = FileName.LastIndexOf('\\');
                string path_name = FileName.Substring(0, pos);
                string type_name = FileName.Substring(pos + 1);
                type_name = type_name.Substring(0, type_name.Length - 3);

                Type t = ServiceBuilder.load_cpp_file(FileName, type_name);
                ServiceBuilder.build(t, path_name);
            }
        }

        private void OUTPUT_BOX_TextChanged(object sender, EventArgs e)
        {

        }

        private void ServiceBuildForm_MouseMove(object sender, MouseEventArgs e)
        {
            this.OUTPUT_BOX.Text = strOutput;
        }

        private void ShowListBtn_Click(object sender, EventArgs e)
        {
            ServiceBuilder.show_export_list();
        }

        private void BtnGenCode_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < this.checkedListBox_Show.Items.Count; i++)
            {
                if (this.checkedListBox_Show.GetItemChecked(i))
                {
                    ServiceBuilder.build((Type)this.checkedListBox_Show.Items[i], ".");
                }
            }
        }
    }
}
