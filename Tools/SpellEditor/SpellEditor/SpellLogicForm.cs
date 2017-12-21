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
using SPELL;

namespace SpellEditor
{
    public partial class SpellLogicForm : Form
    {
        public static SpellLogicForm Instance = null; 
        public static Type gSpellEventEnum = null;
        

        public  int mCurrentSpellID = 0;
        public  Dictionary<int, List<SpellLogic>> mLogicTree = new Dictionary<int, List<SpellLogic>>();
        public string mCurrentEventName = "";

        /// <summary>
        /// 构造table时候的技能描述
        /// </summary>
        public const string SpellLogicSkillColDesc = "技能id";
        /// <summary> 
        /// 构造table时候的事件描述
        /// </summary>
        public const string SpellLogicEventColDesc = "事件id";
        /// <summary>
        /// 构造table时候的客户端条件列表
        /// </summary>
        public const string SpellLogicClientCondtionColDesc = "客户端条件列表";
        /// <summary>
        /// 构造table时候的客户端效果列表
        /// </summary>
        public const string SpellLogicClientEffectColDesc = "客户端效果列表";

        /// <summary>
        /// 构造table时候的服务器条件列表
        /// </summary>
        public const string SpellLogicServerCondtionColDesc = "服务器条件列表";
        /// <summary>
        /// 构造table时候的服务器效果列表描述
        /// </summary>
        public const string SpellLogicServerEffectColDesc = "服务器效果列表";

        /// <summary>
        /// 构造table时候，当遇到空的逻辑，用这个填充
        /// </summary>
        public const string SpellLogicInvalidFiled = "empty";

        private SpellLogic m_CurrentSpellLogic = null;
        /// <summary>
        /// 条件编辑 
        /// </summary>
        private ConditionForm mConditionEditor = null;
        /// <summary>
        /// 效果编辑
        /// </summary>
        private EffectForm mEffectEditor = null;

        /// <summary>
        /// 事件描述数组
        /// </summary>
        private Dictionary<string, string> SpellEventDescDic;
        public SpellLogicForm()
        {
            InitializeComponent();

            gSpellEventEnum = WrapperReflection.gAssembly.GetType("SPELL.SPELL_EVENT");

            this.eventsList.Columns.Add("事件", 200);
            this.srvCondList.Columns.Add("条件", 200);
            this.srvCondList.Columns.Add("内容", 500);
            this.clientCondList.Columns.Add("条件", 200);
            this.clientCondList.Columns.Add("内容", 500);
            this.srvEffectList.Columns.Add("效果", 200);
            this.srvEffectList.Columns.Add("内容", 500);
            this.clientEffectList.Columns.Add("效果", 200);
            this.clientEffectList.Columns.Add("内容", 500);

            SpellEventDescDic = DataTableSerializer.EnumToDic(gSpellEventEnum);

            this.EventsComboBox.DataSource = SpellEventDescDic.Values.ToArray();
            Instance = this;
        }

        private void SpellLogicForm_Load(object sender, EventArgs e)
        {
            mConditionEditor = SpellEditorForm.Instance.mConditionEditor;
            mEffectEditor = SpellEditorForm.Instance.mEffectEditor;
        }

        SpellLogic FindSpellLogic()
        {
            if (!mLogicTree.ContainsKey(mCurrentSpellID))
            {
                return null;
            }

            List<SpellLogic> logicList = mLogicTree[mCurrentSpellID];
            if (this.eventsList.SelectedIndices.Count <= 0)
            {
                return null;
            }

            int index = this.eventsList.SelectedIndices[0];
            if (index<0 || index >= logicList.Count)
            {
                return null;
            }
            m_CurrentSpellLogic = logicList[index];
            return logicList[index];
        }

        /// <summary>
        /// 插入条件
        /// </summary>
        /// <param name="cid">条件ID</param>
        public void PushConditon(int cid)
        {
            string cidName = mConditionEditor.FindCondition(cid)[0];

            if (cidName.Contains("服务器"))
            {
                if (m_CurrentSpellLogic.mSrvConditions.Contains(cid))
                {
                    Trace.Warring("事件：" + m_CurrentSpellLogic.nEventID.ToString() + " 已经包含了conditon:" + cidName + "的相同ID：" + cid.ToString() + "！");
                    return;
                }
                Trace.Normal("添加条件：" + cidName + ",id:" + cid);
                m_CurrentSpellLogic.mSrvConditions.Add(cid);
            }
            else
            {
                if (m_CurrentSpellLogic.mClientConditions.Contains(cid))
                {
                    Trace.Warring("事件：" + m_CurrentSpellLogic.nEventID.ToString() + " 已经包含了conditon:" + cidName + "的相同ID：" + cid.ToString() + "！");
                    return;
                }
                Trace.Normal("添加条件：" + cidName + ",id:" + cid);
                m_CurrentSpellLogic.mClientConditions.Add(cid);
            }
        }

        /// <summary>
        /// 插入效果
        /// </summary>
        /// <param name="eid"></param>
        public void PushEffect(int eid)
        {
            string eidName = this.mEffectEditor.FindEffect(eid)[0];

            if (eidName.Contains("服务器"))
            {
                if (m_CurrentSpellLogic.mSrvEffects.Contains(eid))
                {
                    Trace.Warring("事件：" + m_CurrentSpellLogic.nEventID.ToString() + " 已经包含了Effect:" + eidName + "的相同ID：" + eid.ToString() + "！");
                    return;
                }
                Trace.Normal("添加服务器效果：" + eidName + ",id:" + eid);
                m_CurrentSpellLogic.mSrvEffects.Add(eid);
            }
            else if(eidName.Contains("客户端"))
            {
                if (m_CurrentSpellLogic.mClientEffects.Contains(eid))
                {
                    Trace.Warring("事件：" + m_CurrentSpellLogic.nEventID.ToString() + " 已经包含了Effect:" + eidName + "的相同ID：" + eid.ToString() + "！");
                    return;
                }
                Trace.Normal("添加客户端效果：" + eidName + ",id:" + eid);
                m_CurrentSpellLogic.mClientEffects.Add(eid);
            }
            else
            {
                Trace.Error("添加未知效果：" + eidName + ",id:" + eid + "失败");
            }
        }

        /// <summary>
        /// 在logic中删除效果
        /// </summary>
        /// <param name="eid"></param>
        public void DeleteEffectOnLogic(int eid)
        {
            foreach ( List<SpellLogic> loglist in mLogicTree.Values)
            {
                foreach (SpellLogic logic in loglist)
                {
                    if (logic.mSrvEffects.Contains(eid))
                        logic.mSrvEffects.Remove(eid);

                    if (logic.mClientEffects.Contains(eid))
                        logic.mClientEffects.Remove(eid);
                }
            }
        }

        /// <summary>
        /// 在logic中删除条件
        /// </summary>
        /// <param name="cid"></param>
        public void DeleteConditionOnLogic(int cid)
        {
            foreach (List<SpellLogic> loglist in mLogicTree.Values)
            {
                foreach (SpellLogic logic in loglist)
                {
                    if (logic.mSrvConditions.Contains(cid))
                        logic.mSrvConditions.Remove(cid);

                    if (logic.mClientConditions.Contains(cid))
                        logic.mClientConditions.Remove(cid);
                }
            }
        }

        public bool CopySpellLogic(int nSrcSpellID, int nDestSpellID)
        {
            if(!mLogicTree.ContainsKey(nSrcSpellID))
            {
                return true;
            }

            if(!mLogicTree.ContainsKey(nDestSpellID))
            {
                mLogicTree[nDestSpellID] = new List<SpellLogic>();
            }

            foreach (SpellLogic logic in mLogicTree[nSrcSpellID])
            {
                mLogicTree[nDestSpellID].Add(logic);
            }

            return true;
        }

        #region Show系列

        public bool ShowConditions()
        {
            if (this.srvCondList.Items.Count > 0)
            {
                this.srvCondList.Items.Clear();
            }

            if (this.clientCondList.Items.Count > 0)
            {
                this.clientCondList.Items.Clear();
            }

            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return false;

            foreach (var cid in logic.mSrvConditions)
            {
                ListViewItem item = new ListViewItem(mConditionEditor.FindCondition(cid));
                this.srvCondList.Items.Add(item);
            }

            foreach (var cid in logic.mClientConditions)
            {
                ListViewItem item = new ListViewItem(mConditionEditor.FindCondition(cid));
                this.clientCondList.Items.Add(item);
            }

            return true;
        }

        public bool ShowEffects()
        {
            if (this.srvEffectList.Items.Count > 0)
            {
                this.srvEffectList.Items.Clear();
            }

            if (this.clientEffectList.Items.Count > 0)
            {
                this.clientEffectList.Items.Clear();
            }

            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return false;

            foreach (var cid in logic.mSrvEffects)
            {
                ListViewItem item = new ListViewItem(mEffectEditor.FindEffect(cid));
                this.srvEffectList.Items.Add(item);
            }

            foreach (var cid in logic.mClientEffects)
            {
                ListViewItem item = new ListViewItem(mEffectEditor.FindEffect(cid));
                this.clientEffectList.Items.Add(item);
            }

            return true;
        }

        public bool ShowSpell(int nSpellID)
        {
            mCurrentSpellID = nSpellID;
            ShowSpellEvens(nSpellID);

            return true;
        }

        public bool ShowSpellEvens(int nSpellID)
        {
            this.eventsList.Items.Clear();

            if (!mLogicTree.ContainsKey(nSpellID))
            {
                mLogicTree[nSpellID] = new List<SpellLogic>();
            }

            List<SpellLogic> logicList = mLogicTree[nSpellID];
            foreach (SpellLogic logic in logicList)
            {
                this.eventsList.Items.Add(SpellEventDescDic[logic.nEventID.ToString()]);
            }
            //if (this.eventsList.Items.Count > 0)
            //{
            //    this.eventsList.Focus();
            //    this.eventsList.Items[this.eventsList.Items.Count - 1].Selected = true;
            //    mCurrentEventName = this.eventsList.Items[this.eventsList.Items.Count - 1].Text;
            //}
            //else
            //{
            //    mCurrentEventName = "";
            //}

            ShowConditions();
            ShowEffects();
            return true;
        }
        #endregion

        #region 事件消息
        private void AddEvent_Click(object sender, EventArgs e)
        {
            if (!mLogicTree.ContainsKey(mCurrentSpellID))
            {
                return;
            }

            List<SpellLogic> logicList = mLogicTree[mCurrentSpellID];

            SPELL_EVENT sp_event = (SPELL_EVENT)Enum.GetValues(gSpellEventEnum).GetValue(this.EventsComboBox.SelectedIndex);
            
            //foreach (SpellLogic logi in logicList)
            //{
            //    if (logi.nEventID == sp_event)
            //    {
            //        Trace.Warring("已经存在事件:" + sp_event.ToString() + "，请勿重复添加！");
            //        return;
            //    }
            //}
            SpellLogic logic = new SpellLogic();
            logic.nEventID = sp_event;

            logicList.Add(logic);
            ShowSpellEvens(mCurrentSpellID);
        }

        private void DeleteEvent_Click(object sender, EventArgs e)
        {
            if (!mLogicTree.ContainsKey(mCurrentSpellID))
            {
                return;
            }

            List<SpellLogic> logicList = mLogicTree[mCurrentSpellID];

            if (this.eventsList.SelectedIndices.Count <= 0)
            {
                return;
            }

            int index = this.eventsList.SelectedIndices[0];
            if (index < 0 || index >= logicList.Count)
            {
                return;
            }
            SpellLogic spec = logicList[index];


            logicList.RemoveAt(index);
            ShowSpellEvens(mCurrentSpellID);
            //事件列表为空
            if (this.eventsList.Items.Count <= 0)
            {
                this.srvCondList.Items.Clear();

            }
        }

        private void AddCondition_Click(object sender, EventArgs e)
        {
            if (this.eventsList.Items.Count <= 0)
            {
                Trace.Error("请先添加事件！");
                return;
            }
            if (this.eventsList.SelectedIndices.Count <= 0)
            {
                Trace.Error("请选择一个事件");
                return;
            }

            SpellLogic logic = FindSpellLogic();

            if (logic == null)
            {
                return;
            }
            mConditionEditor.ShowDialog();

            ShowConditions();
        }

        private void DeleteSrvCondition_Click(object sender, EventArgs e)
        {
            if (this.srvCondList.SelectedItems.Count <= 0)
                return;
            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return;

            logic.mSrvConditions.RemoveAt(this.srvCondList.SelectedItems[0].Index);
            ShowConditions();
        }

        private void DeleteSrvEffect_Click(object sender, EventArgs e)
        {
            if (this.srvEffectList.SelectedItems.Count <= 0)
                return;

            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return;

            logic.mSrvEffects.RemoveAt(this.srvEffectList.SelectedItems[0].Index);
            ShowEffects();
        }

        private void AddEffect_Click(object sender, EventArgs e)
        {
            if (this.eventsList.Items.Count <= 0)
            {
                Trace.Error("请先添加事件！");
                return;
            }
            if (this.eventsList.SelectedIndices.Count <= 0)
            {
                Trace.Error("请选择一个事件");
                return;
            }
            SpellLogic logic = FindSpellLogic();

            if (logic == null)
            {
                return;
            }

            mEffectEditor.ShowDialog();
            ShowEffects();
        }

        private void eventsList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.eventsList.SelectedIndices.Count <= 0)
            {
                return;
            }
            ShowConditions();
            ShowEffects();
            mCurrentEventName = this.eventsList.SelectedItems[0].Text;
        }


        /// <summary>
        /// 编辑效果按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EditSrvEffectButton_Click(object sender, EventArgs e)
        {
            if (this.srvEffectList.Items.Count <= 0)
            {
                Trace.Warring("没有条件");
                return;
            }
            if (this.srvEffectList.SelectedItems.Count <= 0)
            {
                Trace.Error("请先选中一个效果");
                return;
            }
            string name = this.srvEffectList.SelectedItems[0].SubItems[0].Text;
            string text = this.srvEffectList.SelectedItems[0].SubItems[1].Text.Split(',')[0];
            string s = name + "," + text;
            mEffectEditor.EditEffect(s);
            ShowEffects();
        }
        /// <summary>
        /// 编辑条件按钮
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void EditSrvConditionButton_Click(object sender, EventArgs e)
        {
            if (this.srvCondList.Items.Count <= 0)
            {
                Trace.Warring("没有条件");
                return;
            }

            if (this.srvCondList.SelectedItems.Count <= 0)
            {
                Trace.Error("请先选中一个条件");
                return;
            }
            string name = this.srvCondList.SelectedItems[0].SubItems[0].Text;
            string text = this.srvCondList.SelectedItems[0].SubItems[1].Text.Split(',')[0];
            string s = name + "," + text;
            mConditionEditor.EditCondition(s);
            ShowConditions();
        }
        #endregion

        #region 导入\出相关

        /// <summary>
        /// 导出spelllogic脚本到csv
        /// </summary>
        /// <param name="fullPatch"></param>
        /// <returns></returns>
        public DataTableSerializer.FileExportReslut ExportSpellLogicToCsv(string fullPatch)
        {
            if (null == mConditionEditor)
            {
                mConditionEditor = SpellEditorForm.Instance.mConditionEditor;
                mEffectEditor = SpellEditorForm.Instance.mEffectEditor;
            }
            DataTable t = new DataTable();
            if (!EncodeSpellLogicDataTable(ref t, mLogicTree))
            {
                return DataTableSerializer.FileExportReslut.RESLUT_ERROR;
            }
            string[] headerText = new string[2];
            headerText[0] = "技能ID,事件ID,客户端条件列表,客户端效果列表,服务器条件列表,服务器效果列表";
            headerText[1] = "int,int,string,string,string,string";
            if (t.Rows.Count <= 0)
            {
                Trace.Warring("文件为空：" + fullPatch);
                return DataTableSerializer.FileExportReslut.RESLUT_EMPTYFILE;
            }
            if (!DataTableSerializer.SaveToCSV(t, fullPatch,headerText))
            {
                Trace.Error("文件导出失败：" + fullPatch);
                return DataTableSerializer.FileExportReslut.RESLUT_ERROR;
            }
            Trace.Normal("文件导出成功：" + fullPatch);
            return DataTableSerializer.FileExportReslut.RESLUT_OK;
        }


        /// <summary>
        /// 从csv中加载spelllogic
        /// </summary>
        /// <param name="fullPatch"></param>
        /// <returns></returns>
        public bool LoadSpellLogicFromCsv(string fullPatch)
        {
            DataTable t = new DataTable();
            if (!BuildSpellLogicDataTable(ref t))
            {
                return false;
            }

            if (!DataTableSerializer.LoadCSVToTable(ref t, fullPatch))
            {
                return false;
            }
            mLogicTree.Clear();
            if (!DecodeSpellLogicDataTable(ref mLogicTree, t))
            {
                return false;
            }
            return true;
        }


        public bool ExportSpellLogicToBin(string fullPatch)
        {
            return true;
        }


        /// <summary>
        /// 从bin中加载spelllogic
        /// </summary>
        /// <param name="fullPatch"></param>
        /// <returns></returns>
        public bool LoadSpellLogicFromBin(string fullPatch)
        {
            return true;
        }


        /// <summary>
        /// 将SpellLogic压缩成一个DataTable
        /// </summary>
        /// <param name="dest">目标</param>
        /// <param name="source">源</param>
        /// <returns></returns>
        public bool EncodeSpellLogicDataTable(ref DataTable dest, Dictionary<int, List<SpellLogic>> source)
        {
            //添加数据--列，一共六列
            //第一列  技能ID
            //第二列  时间ID
            //第三列  客户端条件列表
            //第四列  客户端效果列表
            //第五列  服务器条件列条
            //第六列  服务器效果列表

            //格式：
            //技能id   事件id(枚举值) 客户端条件列表   客户端效果列表    服务器条件列条    服务器效果列表
            //  0       1          1;2;3           1;2;3         1;2;3          1;2;3
            //  0       2          4                 5             7             8
            //  1       1          2                 3             4             5

            if (null == dest || null == source)
                return false;

            if (!BuildSpellLogicDataTable(ref dest))
            {
                return false;
            }

            //遍历结构构造行
            foreach (KeyValuePair<int, List<SpellLogic>> va in mLogicTree)
            {
                List<SpellLogic> spl = va.Value;
                for (int i = 0; i < spl.Count; i++)
                {

                    DataRow row = dest.NewRow();
                    string strClientCondtion = SpellLogicInvalidFiled;
                    string strClientEffect = SpellLogicInvalidFiled;
                    string strServerCondtion = SpellLogicInvalidFiled;
                    string strServerEffect = SpellLogicInvalidFiled;

                    int eventID = (int)spl[i].nEventID;

                    // 客户端条件列表
                    if(spl[i].mClientConditions.Count > 0)
                    {
                        strClientCondtion = spl[i].mClientConditions[0].ToString();

                        for (int j = 1; j < spl[i].mClientConditions.Count; ++j)
                        {
                            strClientCondtion += ";";
                            strClientCondtion += spl[i].mClientConditions[j].ToString();
                        }
                    }

                    // 客户端效果列表
                    if(spl[i].mClientEffects.Count > 0)
                    {
                        strClientEffect = spl[i].mClientEffects[0].ToString(); 

                        for(int j = 1; j < spl[i].mClientEffects.Count; ++j)
                        {
                            strClientEffect += ";";
                            strClientEffect += spl[i].mClientEffects[j].ToString(); 
                        }
                    }

                    // 服务器条件列表
                    if(spl[i].mSrvConditions.Count > 0)
                    {
                        strServerCondtion = spl[i].mSrvConditions[0].ToString();
 
                        for(int j = 1; j < spl[i].mSrvConditions.Count; ++j)
                        {
                            strServerCondtion += ";";
                            strServerCondtion += spl[i].mSrvConditions[j].ToString(); 
                        }
                    }

                    // 服务器效果列表
                    if(spl[i].mSrvEffects.Count > 0)
                    {
                        strServerEffect = spl[i].mSrvEffects[0].ToString();
 
                        for(int j = 1; j < spl[i].mSrvEffects.Count; ++j)
                        {
                            strServerEffect += ";";
                            strServerEffect += spl[i].mSrvEffects[j].ToString(); 
                        }
                    }

                    row[0] = va.Key;
                    row[1] = eventID;
                    row[2] = strClientCondtion;
                    row[3] = strClientEffect;
                    row[4] = strServerCondtion;
                    row[5] = strServerEffect;

                    dest.Rows.Add(row);
                }
            }
            return true;
        }


        /// <summary>
        /// 从一个DataTable解压出SpellLogic
        /// </summary>
        /// <param name="dest">目标</param>
        /// <param name="source">源</param>
        /// <returns></returns>
        public bool DecodeSpellLogicDataTable(ref Dictionary<int, List<SpellLogic>> dest, DataTable source)
        {
            //添加数据--列，一共六列
            //第一列  技能ID
            //第二列  时间ID
            //第三列  客户端条件列表
            //第四列  客户端效果列表
            //第五列  服务器条件列条
            //第六列  服务器效果列表

            //格式：
            //技能id   事件id(枚举值) 客户端条件列表   客户端效果列表    服务器条件列条    服务器效果列表
            //  0       1          1;2;3           1;2;3         1;2;3          1;2;3
            //  0       2          4                 5             7             8
            //  1       1          2                 3             4             5

            if (null == source || null == dest )
            {
                return false;
            }
            if (source.Rows.Count <= 0)
            {
                return true;
            }
            if (source.Columns.Count != 6)
            {
                return false;
            }

            if (!source.Columns[0].ColumnName.Equals(SpellLogicSkillColDesc))
            {
                return false;
            }

            if (!source.Columns[1].ColumnName.Equals(SpellLogicEventColDesc))
            {
                return false;
            }

            if (!source.Columns[2].ColumnName.Equals(SpellLogicClientCondtionColDesc))
            {
                return false;
            }

            if (!source.Columns[3].ColumnName.Equals(SpellLogicServerCondtionColDesc))
            {
                return false;
            }
            if (!source.Columns[4].ColumnName.Equals(SpellLogicClientEffectColDesc))
            {
                return false;
            }

            if (!source.Columns[5].ColumnName.Equals(SpellLogicServerEffectColDesc))
            {
                return false;
            }

            List<SpellLogic> logic = new List<SpellLogic>();//逻辑列表
            List<SPELL_EVENT> eventList = new List<SPELL_EVENT>();//事件列表
            List<string> clientCondList = new List<string>();//客户端条件列条
            List<string> clientEffectList = new List<string>();//客户端效果列表
            List<string> srvCondList = new List<string>(); // 服务器条件列表
            List<string> srvEffectList = new List<string>(); // 服务器效果列表

            int CurrentSkillID = Convert.ToInt32(source.Rows[0][0]);

            //取出数据
            for (int i = 0; i < source.Rows.Count; i++)
            {
                DataRow row = source.Rows[i];
                int skillID = Convert.ToInt32(row[0]);

                //如果不是同一个技能id，说明已经读完这个id
                if (skillID != CurrentSkillID)
                {
                    if (ConverToSpellLogicList(ref logic, eventList, clientCondList, clientEffectList, srvCondList, srvEffectList, CurrentSkillID))
                    {
                        //添加到目标dic
                        dest.Add(CurrentSkillID, logic);

                        //清除上一个ID的内容
                        logic = null;
                        clientEffectList = null;
                        clientCondList = null;
                        eventList = null;
                        logic = new List<SpellLogic>();//逻辑列表
                        eventList = new List<SPELL_EVENT>();//事件列表
                        clientCondList = new List<string>();//客户端条件列条
                        clientEffectList = new List<string>();//客户端效果列表
                        srvCondList = new List<string>(); // 服务器条件列表
                        srvEffectList = new List<string>(); // 服务器效果列表

                        //读取新的ID
                        CurrentSkillID = skillID;

                        //同一个id，读取数据
                        int eventID = Convert.ToInt32(row[1]);
                        eventList.Add((SPELL_EVENT)eventID);//事件ID
                        clientCondList.Add(Convert.ToString(row[2]));//客户端条件
                        clientEffectList.Add(Convert.ToString(row[3]));//客户端效果
                        srvCondList.Add(Convert.ToString(row[4]));//服务器条件
                        srvEffectList.Add(Convert.ToString(row[5]));//服务器效果
                    }

                }
                else
                {
                    //同一个id，读取数据
                    int eventID = Convert.ToInt32(row[1]);
                    eventList.Add((SPELL_EVENT)eventID);//事件ID
                    clientCondList.Add(Convert.ToString(row[2]));//客户端条件
                    clientEffectList.Add(Convert.ToString(row[3]));//客户端效果
                    srvCondList.Add(Convert.ToString(row[4]));//服务器条件
                    srvEffectList.Add(Convert.ToString(row[5]));//服务器效果
                }

            }

            //将最后一个添加到目标dic,
            if (ConverToSpellLogicList(ref logic, eventList, clientCondList, clientEffectList, srvCondList, srvEffectList, CurrentSkillID))
            {
                dest.Add(CurrentSkillID, logic);
            }
          

            return true;
        }


        /// <summary>
        /// 转换成逻辑链表
        /// </summary>
        /// <param name="logic">目标</param>
        /// <param name="eventList">事件链表</param>
        /// <param name="conditionList">条件链表</param>
        /// <param name="effectList">效果链表</param>
        /// <returns></returns>
        private bool ConverToSpellLogicList(ref List<SpellLogic> logic, List<SPELL_EVENT> eventList, List<string> clientCondList, List<string> clientEffectList, List<string> srvCondList, List<string> srvEffectList, int CurrentSkillID)
        {
            if (logic == null)
            {
                return false;
            }

            try
            {
                logic.Clear();
      
                for (int i = 0; i < eventList.Count; i++)
                {
                    SpellLogic log = new SpellLogic();
                    log.nEventID = eventList[i];

                    // 填充客户端条件列表
                    string conditionStr = clientCondList[i];
                    if (!conditionStr.Equals(SpellLogicInvalidFiled) && !string.IsNullOrEmpty(conditionStr))
                    {
                        string[] conditions = conditionStr.Split(';');
                        for (int k = 0; k < conditions.Length; k++)
                        {
                            int condID = Convert.ToInt32(conditions[k]);
                            if (!log.mClientConditions.Contains(condID))
                            {
                                log.mClientConditions.Add(condID);
                            }
                            else
                            {
                                Trace.Warring("spellogic中有重复的效果id：" + condID + "请检查配置文件,技能ID：" + CurrentSkillID.ToString());
                            }
                        }
                    }

                    // 填充服务器条件列表
                    conditionStr = srvCondList[i];
                    if (!conditionStr.Equals(SpellLogicInvalidFiled) && !string.IsNullOrEmpty(conditionStr))
                    {
                        string[] conditions = conditionStr.Split(';');
                        for (int k = 0; k < conditions.Length; k++)
                        {
                            int condID = Convert.ToInt32(conditions[k]);
                            if (!log.mSrvConditions.Contains(condID))
                            {
                                log.mSrvConditions.Add(condID);
                            }
                            else
                            {
                                Trace.Warring("spellogic中有重复的效果id：" + condID + "请检查配置文件,技能ID：" + CurrentSkillID.ToString());
                            }
                        }
                    }

                    // 填充客户端效果列表
                    string effectStr = clientEffectList[i];
                    if (!effectStr.Equals(SpellLogicInvalidFiled) && !string.IsNullOrEmpty(effectStr))
                    {
                        string[] effects = effectStr.Split(';');
                        for (int k = 0; k < effects.Length; k++)
                        {
                            int condID = Convert.ToInt32(effects[k]);

                            if (!log.mClientEffects.Contains(condID))
                            {
                                log.mClientEffects.Add(condID);
                            }
                            else
                            {
                                Trace.Warring("spellogic中有重复的效果id：" + condID + "请检查配置文件,技能ID：" + CurrentSkillID.ToString());
                            }


                        }
                    }

                    // 填充服务器效果列表
                    effectStr = srvEffectList[i];
                    if (!effectStr.Equals(SpellLogicInvalidFiled) && !string.IsNullOrEmpty(effectStr))
                    {
                        string[] effects = effectStr.Split(';');
                        for (int k = 0; k < effects.Length; k++)
                        {
                            int condID = Convert.ToInt32(effects[k]);

                            if (!log.mSrvEffects.Contains(condID))
                            {
                                log.mSrvEffects.Add(condID);
                            }
                            else
                            {
                                Trace.Warring("spellogic中有重复的效果id：" + condID + "请检查配置文件,技能ID：" + CurrentSkillID.ToString());
                            }


                        }
                    }

                    logic.Add(log);

                }
                return true;
            }
            catch (Exception e)
            {
                Trace.Error(e.Message);
                return false;
            }

        }


        /// <summary>
        /// 生成一个spelllogic格式的datatable
        /// </summary>
        /// <param name="ta"></param>
        /// <returns></returns>
        private bool BuildSpellLogicDataTable(ref DataTable ta)
        {
            if (null == ta)
                return false;
            ta.Columns.Clear();
            ta.Rows.Clear();
            ta.Clear();

            //添加数据--列，一共六列
            //第一列  技能ID
            //第二列  时间ID
            //第三列  客户端条件列表
            //第四列  服务器条件列条
            //第五列  客户端效果列表
            //第六列  服务器效果列表
            ta.Columns.Add(SpellLogicSkillColDesc, typeof(int));
            ta.Columns.Add(SpellLogicEventColDesc, typeof(int));
            ta.Columns.Add(SpellLogicClientCondtionColDesc, typeof(string));
            ta.Columns.Add(SpellLogicServerCondtionColDesc, typeof(string));
            ta.Columns.Add(SpellLogicClientEffectColDesc, typeof(string));
            ta.Columns.Add(SpellLogicServerEffectColDesc, typeof(string));
            return true;
        }
        #endregion

        private void SpellLogicForm_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)(27))
            {
                this.Close();
                return;
            }
        }

        private void clientDelCond_Click(object sender, EventArgs e)
        {
            if (this.clientCondList.SelectedItems.Count <= 0)
                return;
            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return;

            logic.mClientConditions.RemoveAt(this.clientCondList.SelectedItems[0].Index);
            ShowConditions();
        }

        private void clientEditCond_Click(object sender, EventArgs e)
        {
            if (this.clientCondList.Items.Count <= 0)
            {
                Trace.Warring("没有条件");
                return;
            }

            if (this.clientCondList.SelectedItems.Count <= 0)
            {
                Trace.Error("请先选中一个条件");
                return;
            }
            string name = this.clientCondList.SelectedItems[0].SubItems[0].Text;
            string text = this.clientCondList.SelectedItems[0].SubItems[1].Text.Split(',')[0];
            string s = name + "," + text;
            mConditionEditor.EditCondition(s);
            ShowConditions();
        }

        private void clientDelEffect_Click(object sender, EventArgs e)
        {
            if (this.clientEffectList.SelectedItems.Count <= 0)
                return;

            SpellLogic logic = FindSpellLogic();
            if (logic == null)
                return;

            logic.mClientEffects.RemoveAt(this.clientEffectList.SelectedItems[0].Index);
            ShowEffects();
        }

        private void clientEditEffect_Click(object sender, EventArgs e)
        {
            if (this.clientEffectList.Items.Count <= 0)
            {
                Trace.Warring("没有条件");
                return;
            }
            if (this.clientEffectList.SelectedItems.Count <= 0)
            {
                Trace.Error("请先选中一个效果");
                return;
            }
            string name = this.clientEffectList.SelectedItems[0].SubItems[0].Text;
            string text = this.clientEffectList.SelectedItems[0].SubItems[1].Text.Split(',')[0];
            string s = name + "," + text;
            mEffectEditor.EditEffect(s);
            ShowEffects();
        }

    }
}
