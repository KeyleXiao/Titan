namespace SpellEditor
{
    partial class ConditionForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.conditionList = new System.Windows.Forms.DataGridView();
            this.conditionSelect = new System.Windows.Forms.ListView();
            this.label1 = new System.Windows.Forms.Label();
            this.ConditionDesc = new System.Windows.Forms.Label();
            this.InsetButton = new System.Windows.Forms.Button();
            this.ConditionSerachBox = new System.Windows.Forms.RichTextBox();
            this.ConditionSerachIDButton = new System.Windows.Forms.Button();
            this.ConditionIDSerach = new System.Windows.Forms.RichTextBox();
            ((System.ComponentModel.ISupportInitialize)(this.conditionList)).BeginInit();
            this.SuspendLayout();
            // 
            // conditionList
            // 
            this.conditionList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.conditionList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.conditionList.Location = new System.Drawing.Point(13, 65);
            this.conditionList.MultiSelect = false;
            this.conditionList.Name = "conditionList";
            this.conditionList.RowTemplate.Height = 23;
            this.conditionList.Size = new System.Drawing.Size(655, 438);
            this.conditionList.TabIndex = 0;
            this.conditionList.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.conditionList_CellContentClick);
            this.conditionList.SelectionChanged += new System.EventHandler(this.conditionList_SelectedIndexChanged);
            // 
            // conditionSelect
            // 
            this.conditionSelect.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.conditionSelect.Location = new System.Drawing.Point(693, 65);
            this.conditionSelect.MultiSelect = false;
            this.conditionSelect.Name = "conditionSelect";
            this.conditionSelect.Size = new System.Drawing.Size(206, 438);
            this.conditionSelect.TabIndex = 1;
            this.conditionSelect.UseCompatibleStateImageBehavior = false;
            this.conditionSelect.View = System.Windows.Forms.View.Details;
            this.conditionSelect.SelectedIndexChanged += new System.EventHandler(this.conditionSelect_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(693, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "请选择条件:";
            // 
            // ConditionDesc
            // 
            this.ConditionDesc.AutoSize = true;
            this.ConditionDesc.Location = new System.Drawing.Point(12, 13);
            this.ConditionDesc.Name = "ConditionDesc";
            this.ConditionDesc.Size = new System.Drawing.Size(59, 12);
            this.ConditionDesc.TabIndex = 3;
            this.ConditionDesc.Text = "条件描述:";
            // 
            // InsetButton
            // 
            this.InsetButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.InsetButton.Location = new System.Drawing.Point(747, 519);
            this.InsetButton.Name = "InsetButton";
            this.InsetButton.Size = new System.Drawing.Size(93, 23);
            this.InsetButton.TabIndex = 4;
            this.InsetButton.Text = "插  入";
            this.InsetButton.UseVisualStyleBackColor = true;
            this.InsetButton.Click += new System.EventHandler(this.InsetButton_Click);
            // 
            // ConditionSerachBox
            // 
            this.ConditionSerachBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ConditionSerachBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.ConditionSerachBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ConditionSerachBox.Location = new System.Drawing.Point(693, 40);
            this.ConditionSerachBox.Multiline = false;
            this.ConditionSerachBox.Name = "ConditionSerachBox";
            this.ConditionSerachBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.ConditionSerachBox.ShowSelectionMargin = true;
            this.ConditionSerachBox.Size = new System.Drawing.Size(206, 19);
            this.ConditionSerachBox.TabIndex = 20;
            this.ConditionSerachBox.Text = "";
            this.ConditionSerachBox.TextChanged += new System.EventHandler(this.ConditionSerachBox_TextChanged);
            // 
            // ConditionSerachIDButton
            // 
            this.ConditionSerachIDButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.ConditionSerachIDButton.Location = new System.Drawing.Point(117, 36);
            this.ConditionSerachIDButton.Name = "ConditionSerachIDButton";
            this.ConditionSerachIDButton.Size = new System.Drawing.Size(45, 23);
            this.ConditionSerachIDButton.TabIndex = 23;
            this.ConditionSerachIDButton.Text = "搜索";
            this.ConditionSerachIDButton.UseVisualStyleBackColor = true;
            this.ConditionSerachIDButton.Click += new System.EventHandler(this.ConditionSerachIDButton_Click);
            // 
            // ConditionIDSerach
            // 
            this.ConditionIDSerach.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.ConditionIDSerach.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.ConditionIDSerach.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ConditionIDSerach.Location = new System.Drawing.Point(14, 40);
            this.ConditionIDSerach.Multiline = false;
            this.ConditionIDSerach.Name = "ConditionIDSerach";
            this.ConditionIDSerach.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.ConditionIDSerach.ShowSelectionMargin = true;
            this.ConditionIDSerach.Size = new System.Drawing.Size(86, 19);
            this.ConditionIDSerach.TabIndex = 22;
            this.ConditionIDSerach.Text = "";
            this.ConditionIDSerach.TextChanged += new System.EventHandler(this.ConditionIDSerach_TextChanged);
            // 
            // ConditionForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(937, 554);
            this.Controls.Add(this.ConditionSerachIDButton);
            this.Controls.Add(this.ConditionIDSerach);
            this.Controls.Add(this.ConditionSerachBox);
            this.Controls.Add(this.InsetButton);
            this.Controls.Add(this.ConditionDesc);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.conditionSelect);
            this.Controls.Add(this.conditionList);
            this.KeyPreview = true;
            this.Name = "ConditionForm";
            this.Text = "条件";
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.ConditionForm_KeyPress);
            ((System.ComponentModel.ISupportInitialize)(this.conditionList)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView conditionList;
        private System.Windows.Forms.ListView conditionSelect;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label ConditionDesc;
        private System.Windows.Forms.Button InsetButton;
        private System.Windows.Forms.RichTextBox ConditionSerachBox;
        private System.Windows.Forms.Button ConditionSerachIDButton;
        private System.Windows.Forms.RichTextBox ConditionIDSerach;
    }
}