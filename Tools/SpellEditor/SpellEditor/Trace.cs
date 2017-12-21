using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using SpellEditor;
using System.Drawing;

/// <summary>
/// 输出类
/// </summary>
public static class Trace
{
    static bool bFirstLog = true;

    public static void Error(string msg)
    {
        if (string.IsNullOrEmpty(msg))
        {
            msg = "emptyString";
        }
        if (!bFirstLog)
            SpellEditorForm.LogArearInstance.AppendText("\n");
        bFirstLog = false;
        ChangeTextColor(Color.Red);
        SpellEditorForm.LogArearInstance.AppendText(msg);
        //滚动到最后
        SpellEditorForm.LogArearInstance.ScrollToCaret();
        
    }

    public static void Normal(string msg)
    {
        if (string.IsNullOrEmpty(msg))
        {
            msg = "emptyString";
        }
        if (!bFirstLog)
            SpellEditorForm.LogArearInstance.AppendText("\n");
        bFirstLog = false;
        ChangeTextColor(Color.Black);
        SpellEditorForm.LogArearInstance.AppendText(msg);
        //滚动到最后
        SpellEditorForm.LogArearInstance.ScrollToCaret();
    }

    public static void Warring(string msg)
    {
        if (string.IsNullOrEmpty(msg))
        {
            msg = "emptyString";
        }
        if (!bFirstLog)
            SpellEditorForm.LogArearInstance.AppendText("\n");
        bFirstLog = false;
        ChangeTextColor(Color.Yellow);
        SpellEditorForm.LogArearInstance.AppendText(msg);
        //滚动到最后
        SpellEditorForm.LogArearInstance.ScrollToCaret();
    }


    public static void Clear()
    {
        bFirstLog = true;
        SpellEditorForm.LogArearInstance.Clear();
    }

    private static void ChangeTextColor(Color col)
    {
        SpellEditorForm.LogArearInstance.SelectionColor = col;
    }
}
