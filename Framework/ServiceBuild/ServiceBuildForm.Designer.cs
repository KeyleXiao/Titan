using System;
using System.Reflection;

namespace ServiceBuild
{
    partial class ServiceBuildForm
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
            this.Build_Csharp = new System.Windows.Forms.Button();
            this.Build_Cpp = new System.Windows.Forms.Button();
            this.OUTPUT_BOX = new System.Windows.Forms.TextBox();
            this.checkedListBox_Show = new System.Windows.Forms.CheckedListBox();
            this.BtnGenCode = new System.Windows.Forms.Button();
            this.ShowListBtn = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // Build_Csharp
            // 
            this.Build_Csharp.Location = new System.Drawing.Point(63, 184);
            this.Build_Csharp.Name = "Build_Csharp";
            this.Build_Csharp.Size = new System.Drawing.Size(216, 23);
            this.Build_Csharp.TabIndex = 0;
            this.Build_Csharp.Text = "Build C# file";
            this.Build_Csharp.UseVisualStyleBackColor = true;
            this.Build_Csharp.Click += new System.EventHandler(this.Build_Button_Click);
            // 
            // Build_Cpp
            // 
            this.Build_Cpp.Location = new System.Drawing.Point(414, 184);
            this.Build_Cpp.Name = "Build_Cpp";
            this.Build_Cpp.Size = new System.Drawing.Size(216, 23);
            this.Build_Cpp.TabIndex = 0;
            this.Build_Cpp.Text = "Build C++ File";
            this.Build_Cpp.UseVisualStyleBackColor = true;
            this.Build_Cpp.Click += new System.EventHandler(this.Build_Cpp_Click);
            // 
            // OUTPUT_BOX
            // 
            this.OUTPUT_BOX.Location = new System.Drawing.Point(24, 12);
            this.OUTPUT_BOX.Multiline = true;
            this.OUTPUT_BOX.Name = "OUTPUT_BOX";
            this.OUTPUT_BOX.Size = new System.Drawing.Size(691, 152);
            this.OUTPUT_BOX.TabIndex = 1;
            this.OUTPUT_BOX.TextChanged += new System.EventHandler(this.OUTPUT_BOX_TextChanged);
            // 
            // checkedListBox_Show
            // 
            this.checkedListBox_Show.FormattingEnabled = true;
            this.checkedListBox_Show.Location = new System.Drawing.Point(24, 227);
            this.checkedListBox_Show.Name = "checkedListBox_Show";
            this.checkedListBox_Show.Size = new System.Drawing.Size(691, 180);
            this.checkedListBox_Show.TabIndex = 3;
            // 
            // BtnGenCode
            // 
            this.BtnGenCode.Location = new System.Drawing.Point(514, 426);
            this.BtnGenCode.Name = "BtnGenCode";
            this.BtnGenCode.Size = new System.Drawing.Size(116, 23);
            this.BtnGenCode.TabIndex = 4;
            this.BtnGenCode.Text = "GenCode";
            this.BtnGenCode.UseVisualStyleBackColor = true;
            this.BtnGenCode.Click += new System.EventHandler(this.BtnGenCode_Click);
            // 
            // ShowListBtn
            // 
            this.ShowListBtn.Location = new System.Drawing.Point(405, 426);
            this.ShowListBtn.Name = "ShowListBtn";
            this.ShowListBtn.Size = new System.Drawing.Size(75, 23);
            this.ShowListBtn.TabIndex = 5;
            this.ShowListBtn.Text = "ShowList";
            this.ShowListBtn.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.ShowListBtn.UseVisualStyleBackColor = true;
            this.ShowListBtn.Click += new System.EventHandler(this.ShowListBtn_Click);
            // 
            // ServiceBuildForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(769, 461);
            this.Controls.Add(this.ShowListBtn);
            this.Controls.Add(this.BtnGenCode);
            this.Controls.Add(this.checkedListBox_Show);
            this.Controls.Add(this.OUTPUT_BOX);
            this.Controls.Add(this.Build_Cpp);
            this.Controls.Add(this.Build_Csharp);
            this.Name = "ServiceBuildForm";
            this.Text = "服务接口生成器";
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ServiceBuildForm_MouseMove);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        public void UpdateListBox(Type[] types)
        {
            this.checkedListBox_Show.Items.Clear();

            foreach(Type t in types)
            {
                if(t.Name.IndexOf("Service") > 0)
                {
                    this.checkedListBox_Show.Items.Add(t);
                }
            }
        }

        #endregion

        private System.Windows.Forms.Button Build_Csharp;
        private System.Windows.Forms.Button Build_Cpp;
        private System.Windows.Forms.TextBox OUTPUT_BOX;
        private System.Windows.Forms.CheckedListBox checkedListBox_Show;
        private System.Windows.Forms.Button BtnGenCode;
        private System.Windows.Forms.Button ShowListBtn;
    }
}

