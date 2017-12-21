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

public class SpellLogic
{
    public SPELL_EVENT nEventID;
    public List<int> mClientConditions = new List<int>();
    public List<int> mClientEffects = new List<int>();
    public List<int> mSrvConditions = new List<int>();
    public List<int> mSrvEffects = new List<int>();
}