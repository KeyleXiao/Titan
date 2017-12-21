using System;
using System.Reflection;
using System.Collections;


// 将包装类反射出来
public class WrapperReflection
{
    public static Type gDescAttributeType = null;
    public static Assembly gAssembly = null;
    public static Type gBitMaskFlagType = null;

    public WrapperReflection()
	{
	}

    static public bool Load()
    {
        gAssembly = Assembly.Load("GameWarpper");
        gDescAttributeType = gAssembly.GetType("DescAttribute");
        gBitMaskFlagType = gAssembly.GetType("BitMaskFlagAttribute");
        return true;
    }

    static public string getDesc(Type t)
    {
        string desc = "";
        object[] descAttrs = t.GetCustomAttributes(gDescAttributeType, false);
        if (descAttrs != null)
        {
            foreach (var attr in descAttrs)
            {
                DescAttribute descAttr = attr as DescAttribute;
                desc += descAttr.description;
            }
        }

        if (string.IsNullOrEmpty(desc))
        {
            desc = t.Name;
        }
        return desc;
    }

    static public string getFieldDesc(FieldInfo t)
    {
        string desc = "";
        object[] descAttrs = t.GetCustomAttributes(gDescAttributeType, false);
        if (descAttrs != null)
        {
            foreach (var attr in descAttrs)
            {
                DescAttribute descAttr = attr as DescAttribute;
                desc += descAttr.description;
            }
        }

        if (string.IsNullOrEmpty(desc))
        {
            desc = t.Name;
        }
        return desc;
    }

    static public Type GetBitMaskObj(FieldInfo f)
    {
        object[] attrbutes = f.GetCustomAttributes(gBitMaskFlagType, false);
        if (attrbutes != null)
        {
            foreach (var attr in attrbutes)
            {
                BitMaskFlagAttribute thisAttr = attr as BitMaskFlagAttribute;
                if (thisAttr == null)
                {
                    break;
                }
                string strEnumName = thisAttr.getValue();
                if (string.IsNullOrEmpty(strEnumName))
                {
                    break;
                }
                Type t = gAssembly.GetType(strEnumName);
                if (t.IsEnum)
                {
                    return t;
                }
            }
        }

        return null;
    }

}
