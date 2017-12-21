using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SpellEditor
{
    public class MaskNodeCollection
    {
        private Dictionary<int, Type>  _nodeList;
        public Dictionary<int, Type> Value
        {
            get { return _nodeList; }
        }

        public MaskNodeCollection()
        {
            _nodeList = new Dictionary<int, Type>();
        }

        public void Add(int n, Type type)
        {
            if (!_nodeList.ContainsKey(n))
            {
                _nodeList.Add(n, type);
            }
        }

        public Type Get(int n)
        {
            return _nodeList.ContainsKey(n) ? _nodeList[n] : null;
        }
    }

    public class Scheme
    {
        private Dictionary<string, MaskNodeCollection> _list;

        public Scheme()
        {
            _list = new Dictionary<string, MaskNodeCollection>();
        }

        public void Add(string strName, int nKey, Type type)
        {
            if (!_list.ContainsKey(strName))
            {
                MaskNodeCollection mc = new MaskNodeCollection();
                mc.Add(nKey, type);

                _list.Add(strName, mc);
            }
            else
            {
                _list[strName].Add(nKey, type);
            }
        }

        public MaskNodeCollection Get(string strName)
        {
            if (!_list.ContainsKey(strName))
            {
                return null;
            }
            return _list[strName];
        }
    }

    /// <summary>
    /// 
    /// </summary>
    static class BitMaskFlagHelper
    {
        static Scheme m_scheme = new Scheme();

        static public void Add(string strScheme, int nColumIndex, Type tEnumType)
        {
            m_scheme.Add(strScheme, nColumIndex, tEnumType);
        }

        static public Type Get(string strScheme, int nColumnIndex)
        {
            MaskNodeCollection mc = m_scheme.Get(strScheme);
            return mc != null ? mc.Get(nColumnIndex) : null;
        }

        /// <summary>
        /// 技能配置有点特殊，额外处理下
        /// </summary>
        static public void PostSpell()
        {
            Type type = typeof(SPELL.SPELL_DATA);
            System.Reflection.FieldInfo[] fields = type.GetFields();

            int nColunIndex = 0;
            for (int i = 0; i < fields.Length; ++i)
            {
                System.Reflection.FieldInfo f = fields[i];
                Type t = WrapperReflection.GetBitMaskObj(f);
                if (t == null)
                {
                    continue;
                }

                nColunIndex = i;

                // 由于中间加了一个备注列，要跳过
                if (i >= 2)
                {
                    nColunIndex += 1;
                }
                Add(type.Name, nColunIndex, t);
            }
        }

        static public void Post(Type type)
        {
            System.Reflection.FieldInfo[] fields = type.GetFields();

            for (int i = 0; i < fields.Length; ++i)
            {
                System.Reflection.FieldInfo f = fields[i];
                Type t = WrapperReflection.GetBitMaskObj(f);
                if (t == null)
                {
                    continue;
                }

                Add(type.Name, i, t);
            }
        }

    }
}
