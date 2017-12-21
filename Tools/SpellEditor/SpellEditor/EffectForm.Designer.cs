using System.Collections.Generic;

namespace SpellEditor
{
    partial class EffectForm
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
            this.effectList = new System.Windows.Forms.DataGridView();
            this.effectSelect = new System.Windows.Forms.ListView();
            this.label1 = new System.Windows.Forms.Label();
            this.ConditionDesc = new System.Windows.Forms.Label();
            this.InsetButton = new System.Windows.Forms.Button();
            this.EffectSerachBox = new System.Windows.Forms.RichTextBox();
            this.effectIDSerach = new System.Windows.Forms.RichTextBox();
            this.EffectSerachIDButton = new System.Windows.Forms.Button();
            this.eIDRangeCombobox = new System.Windows.Forms.ComboBox();
            this.eIDRangeLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.effectList)).BeginInit();
            this.SuspendLayout();
            // 
            // effectList
            // 
            this.effectList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.effectList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.effectList.Location = new System.Drawing.Point(13, 65);
            this.effectList.MultiSelect = false;
            this.effectList.Name = "effectList";
            this.effectList.RowTemplate.Height = 23;
            this.effectList.Size = new System.Drawing.Size(655, 438);
            this.effectList.TabIndex = 0;
            this.effectList.SelectionChanged += new System.EventHandler(this.EffectList_SelectedIndexChanged);
            // 
            // effectSelect
            // 
            this.effectSelect.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.effectSelect.Location = new System.Drawing.Point(693, 65);
            this.effectSelect.MultiSelect = false;
            this.effectSelect.Name = "effectSelect";
            this.effectSelect.Size = new System.Drawing.Size(206, 438);
            this.effectSelect.TabIndex = 1;
            this.effectSelect.UseCompatibleStateImageBehavior = false;
            this.effectSelect.View = System.Windows.Forms.View.Details;
            this.effectSelect.SelectedIndexChanged += new System.EventHandler(this.EffectSelect_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(693, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "请选择效果:";
            // 
            // ConditionDesc
            // 
            this.ConditionDesc.AutoSize = true;
            this.ConditionDesc.Location = new System.Drawing.Point(12, 13);
            this.ConditionDesc.Name = "ConditionDesc";
            this.ConditionDesc.Size = new System.Drawing.Size(59, 12);
            this.ConditionDesc.TabIndex = 3;
            this.ConditionDesc.Text = "效果描述:";
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
            // EffectSerachBox
            // 
            this.EffectSerachBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.EffectSerachBox.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.EffectSerachBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.EffectSerachBox.Location = new System.Drawing.Point(693, 40);
            this.EffectSerachBox.Multiline = false;
            this.EffectSerachBox.Name = "EffectSerachBox";
            this.EffectSerachBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.EffectSerachBox.ShowSelectionMargin = true;
            this.EffectSerachBox.Size = new System.Drawing.Size(206, 19);
            this.EffectSerachBox.TabIndex = 19;
            this.EffectSerachBox.Text = "";
            this.EffectSerachBox.TextChanged += new System.EventHandler(this.EffectSerachBox_TextChanged);
            // 
            // effectIDSerach
            // 
            this.effectIDSerach.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.effectIDSerach.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.effectIDSerach.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.effectIDSerach.Location = new System.Drawing.Point(14, 40);
            this.effectIDSerach.Multiline = false;
            this.effectIDSerach.Name = "effectIDSerach";
            this.effectIDSerach.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.effectIDSerach.ShowSelectionMargin = true;
            this.effectIDSerach.Size = new System.Drawing.Size(86, 19);
            this.effectIDSerach.TabIndex = 20;
            this.effectIDSerach.Text = "";
            this.effectIDSerach.TextChanged += new System.EventHandler(this.effectIDSerach_TextChanged);
            // 
            // EffectSerachIDButton
            // 
            this.EffectSerachIDButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.EffectSerachIDButton.Location = new System.Drawing.Point(117, 36);
            this.EffectSerachIDButton.Name = "EffectSerachIDButton";
            this.EffectSerachIDButton.Size = new System.Drawing.Size(45, 23);
            this.EffectSerachIDButton.TabIndex = 21;
            this.EffectSerachIDButton.Text = "搜索";
            this.EffectSerachIDButton.UseVisualStyleBackColor = true;
            this.EffectSerachIDButton.Click += new System.EventHandler(this.EffectSerachIDButton_Click);
            // 
            // eIDRangeCombobox
            // 
            this.eIDRangeCombobox.FormattingEnabled = true;
            this.eIDRangeCombobox.Location = new System.Drawing.Point(190, 38);
            this.eIDRangeCombobox.Name = "eIDRangeCombobox";
            this.eIDRangeCombobox.Size = new System.Drawing.Size(121, 20);
            this.eIDRangeCombobox.TabIndex = 22;
            this.eIDRangeCombobox.SelectedIndexChanged += new System.EventHandler(this.eIDRangeCombobox_SelectedIndexChanged);
            // 
            // eIDRangeLabel
            // 
            this.eIDRangeLabel.AutoSize = true;
            this.eIDRangeLabel.Location = new System.Drawing.Point(340, 41);
            this.eIDRangeLabel.Name = "eIDRangeLabel";
            this.eIDRangeLabel.Size = new System.Drawing.Size(41, 12);
            this.eIDRangeLabel.TabIndex = 25;
            this.eIDRangeLabel.Text = "label4";
            // 
            // EffectForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(937, 554);
            this.Controls.Add(this.eIDRangeLabel);
            this.Controls.Add(this.eIDRangeCombobox);
            this.Controls.Add(this.EffectSerachIDButton);
            this.Controls.Add(this.effectIDSerach);
            this.Controls.Add(this.EffectSerachBox);
            this.Controls.Add(this.InsetButton);
            this.Controls.Add(this.ConditionDesc);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.effectSelect);
            this.Controls.Add(this.effectList);
            this.KeyPreview = true;
            this.Name = "EffectForm";
            this.Text = "效果";
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.EffectForm_KeyPress);
            ((System.ComponentModel.ISupportInitialize)(this.effectList)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView effectList;
        private System.Windows.Forms.ListView effectSelect;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label ConditionDesc;
        private System.Windows.Forms.Button InsetButton;
        private System.Windows.Forms.RichTextBox EffectSerachBox;
        private System.Windows.Forms.RichTextBox effectIDSerach;
        private System.Windows.Forms.Button EffectSerachIDButton;
        private System.Windows.Forms.ComboBox eIDRangeCombobox;
        private System.Windows.Forms.Label eIDRangeLabel;
    }
}