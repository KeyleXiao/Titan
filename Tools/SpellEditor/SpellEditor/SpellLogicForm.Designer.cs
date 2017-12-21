namespace SpellEditor
{
    partial class SpellLogicForm
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
            this.srvCondList = new System.Windows.Forms.ListView();
            this.srvEffectList = new System.Windows.Forms.ListView();
            this.eventsList = new System.Windows.Forms.ListView();
            this.label3 = new System.Windows.Forms.Label();
            this.AddEvent = new System.Windows.Forms.Button();
            this.DeleteEvent = new System.Windows.Forms.Button();
            this.srvDelCond = new System.Windows.Forms.Button();
            this.srvAddCond = new System.Windows.Forms.Button();
            this.srvDelEffect = new System.Windows.Forms.Button();
            this.srvAddEffect = new System.Windows.Forms.Button();
            this.EventsComboBox = new System.Windows.Forms.ComboBox();
            this.srvEditCond = new System.Windows.Forms.Button();
            this.srvEditEffect = new System.Windows.Forms.Button();
            this.srvGrp = new System.Windows.Forms.GroupBox();
            this.clientGrp = new System.Windows.Forms.GroupBox();
            this.clientEditEffect = new System.Windows.Forms.Button();
            this.clientDelEffect = new System.Windows.Forms.Button();
            this.clientEditCond = new System.Windows.Forms.Button();
            this.clientDelCond = new System.Windows.Forms.Button();
            this.clientEffectList = new System.Windows.Forms.ListView();
            this.clientCondList = new System.Windows.Forms.ListView();
            this.srvGrp.SuspendLayout();
            this.clientGrp.SuspendLayout();
            this.SuspendLayout();
            // 
            // srvCondList
            // 
            this.srvCondList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.srvCondList.FullRowSelect = true;
            this.srvCondList.Location = new System.Drawing.Point(6, 20);
            this.srvCondList.MultiSelect = false;
            this.srvCondList.Name = "srvCondList";
            this.srvCondList.Size = new System.Drawing.Size(334, 273);
            this.srvCondList.TabIndex = 0;
            this.srvCondList.UseCompatibleStateImageBehavior = false;
            this.srvCondList.View = System.Windows.Forms.View.Details;
            // 
            // srvEffectList
            // 
            this.srvEffectList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.srvEffectList.FullRowSelect = true;
            this.srvEffectList.Location = new System.Drawing.Point(5, 343);
            this.srvEffectList.MultiSelect = false;
            this.srvEffectList.Name = "srvEffectList";
            this.srvEffectList.Size = new System.Drawing.Size(335, 237);
            this.srvEffectList.TabIndex = 1;
            this.srvEffectList.UseCompatibleStateImageBehavior = false;
            this.srvEffectList.View = System.Windows.Forms.View.Details;
            // 
            // eventsList
            // 
            this.eventsList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.eventsList.FullRowSelect = true;
            this.eventsList.Location = new System.Drawing.Point(855, 32);
            this.eventsList.MultiSelect = false;
            this.eventsList.Name = "eventsList";
            this.eventsList.Size = new System.Drawing.Size(152, 481);
            this.eventsList.TabIndex = 3;
            this.eventsList.UseCompatibleStateImageBehavior = false;
            this.eventsList.View = System.Windows.Forms.View.Details;
            this.eventsList.SelectedIndexChanged += new System.EventHandler(this.eventsList_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(853, 12);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "事件列表";
            // 
            // AddEvent
            // 
            this.AddEvent.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.AddEvent.Location = new System.Drawing.Point(943, 546);
            this.AddEvent.Name = "AddEvent";
            this.AddEvent.Size = new System.Drawing.Size(64, 23);
            this.AddEvent.TabIndex = 4;
            this.AddEvent.Text = "添加事件";
            this.AddEvent.UseVisualStyleBackColor = true;
            this.AddEvent.Click += new System.EventHandler(this.AddEvent_Click);
            // 
            // DeleteEvent
            // 
            this.DeleteEvent.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.DeleteEvent.Location = new System.Drawing.Point(855, 546);
            this.DeleteEvent.Name = "DeleteEvent";
            this.DeleteEvent.Size = new System.Drawing.Size(62, 23);
            this.DeleteEvent.TabIndex = 4;
            this.DeleteEvent.Text = "删除事件";
            this.DeleteEvent.UseVisualStyleBackColor = true;
            this.DeleteEvent.Click += new System.EventHandler(this.DeleteEvent_Click);
            // 
            // srvDelCond
            // 
            this.srvDelCond.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvDelCond.Location = new System.Drawing.Point(6, 299);
            this.srvDelCond.Name = "srvDelCond";
            this.srvDelCond.Size = new System.Drawing.Size(78, 23);
            this.srvDelCond.TabIndex = 4;
            this.srvDelCond.Text = "删除条件";
            this.srvDelCond.UseVisualStyleBackColor = true;
            this.srvDelCond.Click += new System.EventHandler(this.DeleteSrvCondition_Click);
            // 
            // srvAddCond
            // 
            this.srvAddCond.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvAddCond.Location = new System.Drawing.Point(742, 311);
            this.srvAddCond.Name = "srvAddCond";
            this.srvAddCond.Size = new System.Drawing.Size(75, 23);
            this.srvAddCond.TabIndex = 4;
            this.srvAddCond.Text = "添加条件";
            this.srvAddCond.UseVisualStyleBackColor = true;
            this.srvAddCond.Click += new System.EventHandler(this.AddCondition_Click);
            // 
            // srvDelEffect
            // 
            this.srvDelEffect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvDelEffect.Location = new System.Drawing.Point(6, 586);
            this.srvDelEffect.Name = "srvDelEffect";
            this.srvDelEffect.Size = new System.Drawing.Size(75, 23);
            this.srvDelEffect.TabIndex = 4;
            this.srvDelEffect.Text = "删除效果";
            this.srvDelEffect.UseVisualStyleBackColor = true;
            this.srvDelEffect.Click += new System.EventHandler(this.DeleteSrvEffect_Click);
            // 
            // srvAddEffect
            // 
            this.srvAddEffect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvAddEffect.Location = new System.Drawing.Point(742, 598);
            this.srvAddEffect.Name = "srvAddEffect";
            this.srvAddEffect.Size = new System.Drawing.Size(75, 23);
            this.srvAddEffect.TabIndex = 4;
            this.srvAddEffect.Text = "添加效果";
            this.srvAddEffect.UseVisualStyleBackColor = true;
            this.srvAddEffect.Click += new System.EventHandler(this.AddEffect_Click);
            // 
            // EventsComboBox
            // 
            this.EventsComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.EventsComboBox.FormattingEnabled = true;
            this.EventsComboBox.Location = new System.Drawing.Point(855, 520);
            this.EventsComboBox.Name = "EventsComboBox";
            this.EventsComboBox.Size = new System.Drawing.Size(152, 20);
            this.EventsComboBox.TabIndex = 5;
            // 
            // srvEditCond
            // 
            this.srvEditCond.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvEditCond.Location = new System.Drawing.Point(265, 299);
            this.srvEditCond.Name = "srvEditCond";
            this.srvEditCond.Size = new System.Drawing.Size(75, 23);
            this.srvEditCond.TabIndex = 6;
            this.srvEditCond.Text = "编辑条件";
            this.srvEditCond.UseVisualStyleBackColor = true;
            this.srvEditCond.Click += new System.EventHandler(this.EditSrvConditionButton_Click);
            // 
            // srvEditEffect
            // 
            this.srvEditEffect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.srvEditEffect.Location = new System.Drawing.Point(265, 586);
            this.srvEditEffect.Name = "srvEditEffect";
            this.srvEditEffect.Size = new System.Drawing.Size(75, 23);
            this.srvEditEffect.TabIndex = 7;
            this.srvEditEffect.Text = "编辑效果";
            this.srvEditEffect.UseVisualStyleBackColor = true;
            this.srvEditEffect.Click += new System.EventHandler(this.EditSrvEffectButton_Click);
            // 
            // srvGrp
            // 
            this.srvGrp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.srvGrp.Controls.Add(this.srvCondList);
            this.srvGrp.Controls.Add(this.srvEditEffect);
            this.srvGrp.Controls.Add(this.srvEffectList);
            this.srvGrp.Controls.Add(this.srvEditCond);
            this.srvGrp.Controls.Add(this.srvDelCond);
            this.srvGrp.Controls.Add(this.srvDelEffect);
            this.srvGrp.Location = new System.Drawing.Point(12, 12);
            this.srvGrp.Name = "srvGrp";
            this.srvGrp.Size = new System.Drawing.Size(346, 627);
            this.srvGrp.TabIndex = 8;
            this.srvGrp.TabStop = false;
            this.srvGrp.Text = "服务器";
            // 
            // clientGrp
            // 
            this.clientGrp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.clientGrp.Controls.Add(this.clientEditEffect);
            this.clientGrp.Controls.Add(this.clientDelEffect);
            this.clientGrp.Controls.Add(this.clientEditCond);
            this.clientGrp.Controls.Add(this.clientDelCond);
            this.clientGrp.Controls.Add(this.clientEffectList);
            this.clientGrp.Controls.Add(this.clientCondList);
            this.clientGrp.Location = new System.Drawing.Point(379, 12);
            this.clientGrp.Name = "clientGrp";
            this.clientGrp.Size = new System.Drawing.Size(346, 627);
            this.clientGrp.TabIndex = 9;
            this.clientGrp.TabStop = false;
            this.clientGrp.Text = "客户端";
            // 
            // clientEditEffect
            // 
            this.clientEditEffect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.clientEditEffect.Location = new System.Drawing.Point(265, 586);
            this.clientEditEffect.Name = "clientEditEffect";
            this.clientEditEffect.Size = new System.Drawing.Size(75, 23);
            this.clientEditEffect.TabIndex = 9;
            this.clientEditEffect.Text = "编辑效果";
            this.clientEditEffect.UseVisualStyleBackColor = true;
            this.clientEditEffect.Click += new System.EventHandler(this.clientEditEffect_Click);
            // 
            // clientDelEffect
            // 
            this.clientDelEffect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.clientDelEffect.Location = new System.Drawing.Point(9, 586);
            this.clientDelEffect.Name = "clientDelEffect";
            this.clientDelEffect.Size = new System.Drawing.Size(75, 23);
            this.clientDelEffect.TabIndex = 8;
            this.clientDelEffect.Text = "删除效果";
            this.clientDelEffect.UseVisualStyleBackColor = true;
            this.clientDelEffect.Click += new System.EventHandler(this.clientDelEffect_Click);
            // 
            // clientEditCond
            // 
            this.clientEditCond.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.clientEditCond.Location = new System.Drawing.Point(265, 299);
            this.clientEditCond.Name = "clientEditCond";
            this.clientEditCond.Size = new System.Drawing.Size(75, 23);
            this.clientEditCond.TabIndex = 7;
            this.clientEditCond.Text = "编辑条件";
            this.clientEditCond.UseVisualStyleBackColor = true;
            this.clientEditCond.Click += new System.EventHandler(this.clientEditCond_Click);
            // 
            // clientDelCond
            // 
            this.clientDelCond.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.clientDelCond.Location = new System.Drawing.Point(6, 299);
            this.clientDelCond.Name = "clientDelCond";
            this.clientDelCond.Size = new System.Drawing.Size(78, 23);
            this.clientDelCond.TabIndex = 5;
            this.clientDelCond.Text = "删除条件";
            this.clientDelCond.UseVisualStyleBackColor = true;
            this.clientDelCond.Click += new System.EventHandler(this.clientDelCond_Click);
            // 
            // clientEffectList
            // 
            this.clientEffectList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.clientEffectList.FullRowSelect = true;
            this.clientEffectList.Location = new System.Drawing.Point(6, 343);
            this.clientEffectList.MultiSelect = false;
            this.clientEffectList.Name = "clientEffectList";
            this.clientEffectList.Size = new System.Drawing.Size(335, 237);
            this.clientEffectList.TabIndex = 2;
            this.clientEffectList.UseCompatibleStateImageBehavior = false;
            this.clientEffectList.View = System.Windows.Forms.View.Details;
            // 
            // clientCondList
            // 
            this.clientCondList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.clientCondList.FullRowSelect = true;
            this.clientCondList.Location = new System.Drawing.Point(6, 20);
            this.clientCondList.MultiSelect = false;
            this.clientCondList.Name = "clientCondList";
            this.clientCondList.Size = new System.Drawing.Size(334, 273);
            this.clientCondList.TabIndex = 1;
            this.clientCondList.UseCompatibleStateImageBehavior = false;
            this.clientCondList.View = System.Windows.Forms.View.Details;
            // 
            // SpellLogicForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1019, 651);
            this.Controls.Add(this.clientGrp);
            this.Controls.Add(this.srvGrp);
            this.Controls.Add(this.srvAddEffect);
            this.Controls.Add(this.srvAddCond);
            this.Controls.Add(this.EventsComboBox);
            this.Controls.Add(this.DeleteEvent);
            this.Controls.Add(this.AddEvent);
            this.Controls.Add(this.eventsList);
            this.Controls.Add(this.label3);
            this.KeyPreview = true;
            this.Name = "SpellLogicForm";
            this.Text = "技能逻辑编辑器";
            this.Load += new System.EventHandler(this.SpellLogicForm_Load);
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.SpellLogicForm_KeyPress);
            this.srvGrp.ResumeLayout(false);
            this.clientGrp.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView srvCondList;
        private System.Windows.Forms.ListView srvEffectList;
        private System.Windows.Forms.ListView eventsList;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button AddEvent;
        private System.Windows.Forms.Button DeleteEvent;
        private System.Windows.Forms.Button srvDelCond;
        private System.Windows.Forms.Button srvAddCond;
        private System.Windows.Forms.Button srvDelEffect;
        private System.Windows.Forms.Button srvAddEffect;
        private System.Windows.Forms.ComboBox EventsComboBox;
        private System.Windows.Forms.Button srvEditCond;
        private System.Windows.Forms.Button srvEditEffect;
        private System.Windows.Forms.GroupBox srvGrp;
        private System.Windows.Forms.GroupBox clientGrp;
        private System.Windows.Forms.ListView clientCondList;
        private System.Windows.Forms.ListView clientEffectList;
        private System.Windows.Forms.Button clientEditEffect;
        private System.Windows.Forms.Button clientDelEffect;
        private System.Windows.Forms.Button clientEditCond;
        private System.Windows.Forms.Button clientDelCond;
    }
}