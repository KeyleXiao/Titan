using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.IO;
using System;
using AssetEx;

namespace AssetEx
{
    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    //extends Node<FieldNode> 
    public class FieldNode
    {
        public TypeNode typenode;
        public object value;
        public bool error = false;

        Dictionary<String, FieldNode> _childs ;//= new Dictionary<String, FieldNode>();

        public void add(FieldNode n)
        {
            if (_childs == null) _childs = new Dictionary<string, FieldNode>();
            _childs[n.typenode.name] = n;
        }

        public Dictionary<String, FieldNode> getChilds() { return _childs; }
        public void cleanChilds() { _childs = null; }

        public FieldNode getChild(String name)
        {
            if (_childs == null) return null;
            if (_childs.ContainsKey(name)) return _childs[name];
            //for (FieldNode child : this) {
            //    if (child.getType().fieldName().equals(name)) {
            //        return child;
            //    }
            //}
            return null;
        }

        public object getChildValue(String name)
        {
            FieldNode child = getChild(name);
            if (child == null)
            {
                return null;
            }

            object v = child.value;
            return v;
        }

        public T getChildValue<T>(String name)
        {
            FieldNode child = getChild(name);
            if (child == null)
            {
                return default(T);
            }

            object v = child.value;
            return (T)v;
        }

        public void setChildValue(String name, object value)
        {
            FieldNode child = getChild(name);
            if (child == null)
            {
                //throw new RuntimeTypeException("Field " + name + " doesn't exist");
            }
            child.value = value;
        }

        public T getArrayData<T>()
        {
            FieldNode arrayField = getChild("Array");
            if (arrayField == null)
            {
                if (_childs != null)
                {
                    foreach (KeyValuePair<string, FieldNode> pair in _childs)
                    {
                        FieldNode childnode = pair.Value;
                        if (childnode.typenode.isArray)
                        {
                            arrayField = childnode;
                            break;
                        }
                    }
                }
                if (arrayField == null)
                {
                    return default(T);
                }
            }

            return (T)arrayField.getChildValue<T>("data");
        }

        public object getArrayData()
        {
            FieldNode arrayField = getChild("Array");
            if (arrayField == null)
            {
                if (_childs != null)
                {
                    foreach (KeyValuePair<string, FieldNode> pair in _childs)
                    {
                        FieldNode childnode = pair.Value;
                        if (childnode.typenode.isArray)
                        {
                            arrayField = childnode;
                            break;
                        }
                    }
                }
                if (arrayField == null)
                {
                    return null;
                }
            }

            return arrayField.getChildValue("data");
        }

        public void setArrayData(object value)
        {
            FieldNode arrayField = getChild("Array");
            if (arrayField == null)
            {
                throw new Exception("Field is not an array");
            }

            arrayField.setChildValue("data", value);
        }

        public T getChildArrayData<T>(String name)
        {
            return getChild(name).getArrayData<T>();
        }

        public object getChildArrayData(String name)
        {
            return getChild(name).getArrayData();
        }

        public void setChildArrayData(String name, object value)
        {
            getChild(name).setArrayData(value);
        }

        public byte getSInt8(String name)
        {
            return getChildValue<byte>(name);
        }

        public void setSInt8(String name, byte v)
        {
            setChildValue(name, v);
        }

        public short getUInt8(String name)
        {
            return (short)(getChildValue<byte>(name) & 0xff);
        }

        public void setUInt8(String name, byte v)
        {
            setChildValue(name, v);
        }

        public short getSInt16(String name)
        {
            return getChildValue<short>(name);
        }

        public void setSInt16(String name, short v)
        {
            setChildValue(name, v);
        }

        public int getUInt16(String name)
        {
            return getChildValue<short>(name) & 0xffff;
        }

        public void setUInt16(String name, int v)
        {
            setChildValue(name, v & 0xffff);
        }

        public int getSInt32(String name)
        {
            return getChildValue<int>(name);
        }

        public void setSInt32(String name, int v)
        {
            setChildValue(name, v);
        }

        public long getUInt32(String name)
        {
            return getChildValue<long>(name) & 0xffffffffL;
        }

        public void getUInt32(String name, long v)
        {
            setChildValue(name, v & 0xffffffffL);
        }

        public long getSInt64(String name)
        {
            return getChildValue<long>(name);
        }

        public void setSInt64(String name, long v)
        {
            setChildValue(name, v);
        }

        //public BigInteger getUInt64(String name) {
        //    return getChildValue(name, BigInteger.class);
        //}

        //public void setUInt64(String name, BigInteger v)
        //{
        //    setChildValue(name, v);
        //}

        public float getFloat(String name)
        {
            return getChildValue<float>(name);
        }

        public void setFloat(String name, float v)
        {
            setChildValue(name, v);
        }

        public double getDouble(String name)
        {
            return getChildValue<double>(name);
        }

        public void setDouble(String name, double v)
        {
            setChildValue(name, v);
        }

        public bool getBoolean(String name)
        {
            return getChildValue<bool>(name);
        }

        public void setBoolean(String name, bool v)
        {
            setChildValue(name, v);
        }

        public String getString(String name)
        {
            return getChildValue<String>(name);
        }

        public void setString(String name, String v)
        {
            setChildValue(name, v);
        }

        public object getObject(String name)
        {
            return getChildValue<object>(name);
        }

        public void setObject(String name, FieldNode v)
        {
            setChildValue(name, v);
        }

        public override string ToString()
        {
            string s = "";

            if (value != null)
            {
                s += value.ToString();
            }
            else
            {
                s += "null";
                s += "<";
                if (typenode != null)
                    s += typenode.type.ToString();
                s += ">";
            }

            s += ",child=" + (_childs == null ? "0" : _childs.Count.ToString());
            return s;
        }


        //@Override
        //public int hashCode() {
        //    int hash = super.hashCode();
        //    hash = 97 * hash + Objects.hashCode(this.type);
        //    hash = 97 * hash + Objects.hashCode(this.value);
        //    return hash;
        //}

        //@Override
        //public boolean equals(Object obj) {
        //    if (obj == null) {
        //        return false;
        //    }
        //    if (getClass() != obj.getClass()) {
        //        return false;
        //    }
        //    final FieldNode other = (FieldNode) obj;
        //    if (!Objects.equals(this.type, other.type)) {
        //        return false;
        //    }
        //    if (!Objects.equals(this.value, other.value)) {
        //        return false;
        //    }
        //    return true;
        //}
    }
}
