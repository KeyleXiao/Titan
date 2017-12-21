namespace SpellEditor
{
    partial class SpellEditorForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.spellList = new System.Windows.Forms.DataGridView();
            this.LogicEdtiorButton = new System.Windows.Forms.Button();
            this.ExportButton = new System.Windows.Forms.Button();
            this.LogArear = new System.Windows.Forms.RichTextBox();
            this.LoadButton = new System.Windows.Forms.Button();
            this.CopyLineButton = new System.Windows.Forms.Button();
            this.BinFileCheckBox = new System.Windows.Forms.CheckBox();
            this.CsvFilecheckBox = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.ImportPatchTextBox = new System.Windows.Forms.RichTextBox();
            this.ExportPatchTextBox = new System.Windows.Forms.RichTextBox();
            this.ImportPatchButton = new System.Windows.Forms.Button();
            this.ExportPatchButton = new System.Windows.Forms.Button();
            this.VocationFilterList = new System.Windows.Forms.ComboBox();
            this.职业过滤 = new System.Windows.Forms.Label();
            this.CalculatorButton = new System.Windows.Forms.Button();
            this.SerachBox = new System.Windows.Forms.RichTextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.SerachButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.spellList)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // spellList
            // 
            this.spellList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.spellList.Location = new System.Drawing.Point(13, 31);
            this.spellList.MultiSelect = false;
            this.spellList.Name = "spellList";
            this.spellList.RowTemplate.Height = 23;
            this.spellList.Size = new System.Drawing.Size(1008, 527);
            this.spellList.TabIndex = 1;
            this.spellList.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.spellList_CellDoubleClick);
            this.spellList.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.spellList_PreviewKeyDown);
            // 
            // LogicEdtiorButton
            // 
            this.LogicEdtiorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.LogicEdtiorButton.Location = new System.Drawing.Point(916, 565);
            this.LogicEdtiorButton.Name = "LogicEdtiorButton";
            this.LogicEdtiorButton.Size = new System.Drawing.Size(105, 23);
            this.LogicEdtiorButton.TabIndex = 0;
            this.LogicEdtiorButton.Text = "编辑技能逻辑";
            this.LogicEdtiorButton.UseVisualStyleBackColor = true;
            this.LogicEdtiorButton.Click += new System.EventHandler(this.LogicEditorButton_Click);
            // 
            // ExportButton
            // 
            this.ExportButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.ExportButton.Location = new System.Drawing.Point(103, 565);
            this.ExportButton.Name = "ExportButton";
            this.ExportButton.Size = new System.Drawing.Size(84, 23);
            this.ExportButton.TabIndex = 2;
            this.ExportButton.Text = "导    出";
            this.ExportButton.UseVisualStyleBackColor = true;
            this.ExportButton.Click += new System.EventHandler(this.ExportButton_Click);
            // 
            // LogArear
            // 
            this.LogArear.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.LogArear.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.LogArear.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LogArear.Location = new System.Drawing.Point(1041, 31);
            this.LogArear.Name = "LogArear";
            this.LogArear.ReadOnly = true;
            this.LogArear.Size = new System.Drawing.Size(223, 520);
            this.LogArear.TabIndex = 3;
            this.LogArear.Text = "";
            // 
            // LoadButton
            // 
            this.LoadButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.LoadButton.Location = new System.Drawing.Point(13, 565);
            this.LoadButton.Name = "LoadButton";
            this.LoadButton.Size = new System.Drawing.Size(84, 23);
            this.LoadButton.TabIndex = 4;
            this.LoadButton.Text = "加    载";
            this.LoadButton.UseVisualStyleBackColor = true;
            this.LoadButton.Click += new System.EventHandler(this.LoadButton_Click);
            // 
            // CopyLineButton
            // 
            this.CopyLineButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.CopyLineButton.Location = new System.Drawing.Point(193, 565);
            this.CopyLineButton.Name = "CopyLineButton";
            this.CopyLineButton.Size = new System.Drawing.Size(84, 23);
            this.CopyLineButton.TabIndex = 5;
            this.CopyLineButton.Text = "创建一行";
            this.CopyLineButton.UseVisualStyleBackColor = true;
            this.CopyLineButton.Click += new System.EventHandler(this.CopyLineButton_Click);
            // 
            // BinFileCheckBox
            // 
            this.BinFileCheckBox.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.BinFileCheckBox.AutoSize = true;
            this.BinFileCheckBox.Location = new System.Drawing.Point(508, 575);
            this.BinFileCheckBox.Name = "BinFileCheckBox";
            this.BinFileCheckBox.Size = new System.Drawing.Size(42, 16);
            this.BinFileCheckBox.TabIndex = 6;
            this.BinFileCheckBox.Text = "Bin";
            this.BinFileCheckBox.UseVisualStyleBackColor = true;
            this.BinFileCheckBox.CheckedChanged += new System.EventHandler(this.BinFileCheckBox_CheckedChanged);
            // 
            // CsvFilecheckBox
            // 
            this.CsvFilecheckBox.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.CsvFilecheckBox.AutoSize = true;
            this.CsvFilecheckBox.Location = new System.Drawing.Point(580, 575);
            this.CsvFilecheckBox.Name = "CsvFilecheckBox";
            this.CsvFilecheckBox.Size = new System.Drawing.Size(42, 16);
            this.CsvFilecheckBox.TabIndex = 7;
            this.CsvFilecheckBox.Text = "Csv";
            this.CsvFilecheckBox.UseVisualStyleBackColor = true;
            this.CsvFilecheckBox.CheckedChanged += new System.EventHandler(this.CsvFilecheckBox_CheckedChanged);
            // 
            // label1
            // 
            this.label1.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(437, 575);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 8;
            this.label1.Text = "文件类型：";
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 601);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "导入路径:";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 624);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 12);
            this.label3.TabIndex = 10;
            this.label3.Text = "导出路径:";
            // 
            // ImportPatchTextBox
            // 
            this.ImportPatchTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ImportPatchTextBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.ImportPatchTextBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportPatchTextBox.Location = new System.Drawing.Point(68, 598);
            this.ImportPatchTextBox.Multiline = false;
            this.ImportPatchTextBox.Name = "ImportPatchTextBox";
            this.ImportPatchTextBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.ImportPatchTextBox.Size = new System.Drawing.Size(1058, 19);
            this.ImportPatchTextBox.TabIndex = 11;
            this.ImportPatchTextBox.Text = "";
            this.ImportPatchTextBox.TextChanged += new System.EventHandler(this.ImportPatchTextBox_TextChanged);
            // 
            // ExportPatchTextBox
            // 
            this.ExportPatchTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ExportPatchTextBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.ExportPatchTextBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ExportPatchTextBox.Location = new System.Drawing.Point(68, 624);
            this.ExportPatchTextBox.Multiline = false;
            this.ExportPatchTextBox.Name = "ExportPatchTextBox";
            this.ExportPatchTextBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.ExportPatchTextBox.Size = new System.Drawing.Size(1058, 19);
            this.ExportPatchTextBox.TabIndex = 12;
            this.ExportPatchTextBox.Text = "";
            this.ExportPatchTextBox.TextChanged += new System.EventHandler(this.ExportPatchTextBox_TextChanged);
            // 
            // ImportPatchButton
            // 
            this.ImportPatchButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ImportPatchButton.Location = new System.Drawing.Point(1132, 594);
            this.ImportPatchButton.Name = "ImportPatchButton";
            this.ImportPatchButton.Size = new System.Drawing.Size(84, 23);
            this.ImportPatchButton.TabIndex = 13;
            this.ImportPatchButton.Text = "浏  览";
            this.ImportPatchButton.UseVisualStyleBackColor = true;
            this.ImportPatchButton.Click += new System.EventHandler(this.ImportPatchButton_Click);
            // 
            // ExportPatchButton
            // 
            this.ExportPatchButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ExportPatchButton.Location = new System.Drawing.Point(1132, 624);
            this.ExportPatchButton.Name = "ExportPatchButton";
            this.ExportPatchButton.Size = new System.Drawing.Size(84, 23);
            this.ExportPatchButton.TabIndex = 14;
            this.ExportPatchButton.Text = "浏  览";
            this.ExportPatchButton.UseVisualStyleBackColor = true;
            this.ExportPatchButton.Click += new System.EventHandler(this.ExportPatchButton_Click);
            // 
            // VocationFilterList
            // 
            this.VocationFilterList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.VocationFilterList.FormattingEnabled = true;
            this.VocationFilterList.Location = new System.Drawing.Point(779, 571);
            this.VocationFilterList.Name = "VocationFilterList";
            this.VocationFilterList.Size = new System.Drawing.Size(89, 20);
            this.VocationFilterList.TabIndex = 15;
            this.VocationFilterList.SelectedIndexChanged += new System.EventHandler(this.VocationFilterList_SelectedIndexChanged);
            // 
            // 职业过滤
            // 
            this.职业过滤.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.职业过滤.AutoSize = true;
            this.职业过滤.Location = new System.Drawing.Point(716, 575);
            this.职业过滤.Name = "职业过滤";
            this.职业过滤.Size = new System.Drawing.Size(59, 12);
            this.职业过滤.TabIndex = 16;
            this.职业过滤.Text = "职业过滤:";
            // 
            // CalculatorButton
            // 
            this.CalculatorButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.CalculatorButton.Location = new System.Drawing.Point(1027, 565);
            this.CalculatorButton.Name = "CalculatorButton";
            this.CalculatorButton.Size = new System.Drawing.Size(105, 23);
            this.CalculatorButton.TabIndex = 17;
            this.CalculatorButton.Text = "算号器";
            this.CalculatorButton.UseVisualStyleBackColor = true;
            this.CalculatorButton.Click += new System.EventHandler(this.CalculatorButton_Click);
            // 
            // SerachBox
            // 
            this.SerachBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.SerachBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.SerachBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.SerachBox.Location = new System.Drawing.Point(49, 10);
            this.SerachBox.Multiline = false;
            this.SerachBox.Name = "SerachBox";
            this.SerachBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.SerachBox.ShowSelectionMargin = true;
            this.SerachBox.Size = new System.Drawing.Size(116, 19);
            this.SerachBox.TabIndex = 18;
            this.SerachBox.Text = "";
            this.SerachBox.TextChanged += new System.EventHandler(this.SerachBox_TextChanged);
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(2, 14);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(41, 12);
            this.label4.TabIndex = 19;
            this.label4.Text = "搜索ID";
            // 
            // SerachButton
            // 
            this.SerachButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.SerachButton.Location = new System.Drawing.Point(170, 7);
            this.SerachButton.Name = "SerachButton";
            this.SerachButton.Size = new System.Drawing.Size(45, 23);
            this.SerachButton.TabIndex = 20;
            this.SerachButton.Text = "搜索";
            this.SerachButton.UseVisualStyleBackColor = true;
            this.SerachButton.Click += new System.EventHandler(this.SerachButton_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.SerachButton);
            this.panel1.Controls.Add(this.label4);
            this.panel1.Controls.Add(this.SerachBox);
            this.panel1.Location = new System.Drawing.Point(13, -4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(220, 34);
            this.panel1.TabIndex = 21;
            // 
            // SpellEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1276, 650);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.CalculatorButton);
            this.Controls.Add(this.职业过滤);
            this.Controls.Add(this.VocationFilterList);
            this.Controls.Add(this.ExportPatchButton);
            this.Controls.Add(this.ImportPatchButton);
            this.Controls.Add(this.ExportPatchTextBox);
            this.Controls.Add(this.ImportPatchTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CsvFilecheckBox);
            this.Controls.Add(this.BinFileCheckBox);
            this.Controls.Add(this.CopyLineButton);
            this.Controls.Add(this.LoadButton);
            this.Controls.Add(this.LogArear);
            this.Controls.Add(this.ExportButton);
            this.Controls.Add(this.spellList);
            this.Controls.Add(this.LogicEdtiorButton);
            this.Name = "SpellEditorForm";
            this.Text = "<深圳冰川网络>技能编辑器V0.1";
            this.Load += new System.EventHandler(this.SpellEditorForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.spellList)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView spellList;
        private System.Windows.Forms.Button LogicEdtiorButton;
        private System.Windows.Forms.Button ExportButton;
        private System.Windows.Forms.RichTextBox LogArear;
        private System.Windows.Forms.Button LoadButton;
        private System.Windows.Forms.Button CopyLineButton;
        private System.Windows.Forms.CheckBox BinFileCheckBox;
        private System.Windows.Forms.CheckBox CsvFilecheckBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RichTextBox ImportPatchTextBox;
        private System.Windows.Forms.RichTextBox ExportPatchTextBox;
        private System.Windows.Forms.Button ImportPatchButton;
        private System.Windows.Forms.Button ExportPatchButton;
        private System.Windows.Forms.ComboBox VocationFilterList;
        private System.Windows.Forms.Label 职业过滤;
        private System.Windows.Forms.Button CalculatorButton;
        private System.Windows.Forms.RichTextBox SerachBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button SerachButton;
        private System.Windows.Forms.Panel panel1;
    }
}

