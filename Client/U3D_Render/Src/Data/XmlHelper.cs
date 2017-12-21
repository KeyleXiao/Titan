using System;
using System.Collections;
using System.Xml;

namespace HeTuGame.Data
{
	public class XmlHelper
	{
		public static int GetIntValue(XmlNode xmlNode, string attributeName, int defaultValue)
		{
			XmlAttribute attribute											= xmlNode.Attributes[attributeName];
			int retValue;
			if(attribute != null && Int32.TryParse(attribute.Value, out retValue))
				return retValue;

			return defaultValue;
		}

        public static float GetFloatValue(XmlNode xmlNode, string attributeName, float defaultValue)
        {
            XmlAttribute attribute = xmlNode.Attributes[attributeName]; ;
            float retValue;
            if (attribute != null && float.TryParse(attribute.Value, out retValue))
                return retValue;
            return defaultValue;
        }

		public static string GetStringValue(XmlNode xmlNode, string attributeName, string defaultValue)
		{
			XmlAttribute attribute											= xmlNode.Attributes[attributeName];
			if(attribute != null)
				return attribute.Value;

			return defaultValue;
		}

		public static bool GetBoolValue(XmlNode xmlNode, string attributeName, bool defaultValue)
		{
			XmlAttribute attribute											= xmlNode.Attributes[attributeName];
			if(attribute != null)
				return attribute.Value != "0";

			return defaultValue;
		}

        public static void SetBoolValue(XmlNode xmlNode,string attributeName,bool value)
        {
            XmlAttribute attribute = xmlNode.Attributes[attributeName];
            string dValue = value ? "1" : "0";
            if (attribute != null)
            {
                attribute.Value = dValue;
            }
            else
            {
                Trace.LogError("设置xml属性失败");
            }
        }

        public static void SetIntValue(XmlNode xmlNode, string attributeName, int value)
        {
            XmlElement e = (XmlElement)xmlNode;
            string dValue = value.ToString();
            e.SetAttribute(attributeName, dValue);
        }

        public static void SetStringValue(XmlNode xmlNode, string attributeName, string value)
        {
            XmlElement e = (XmlElement)xmlNode;
            e.SetAttribute(attributeName, value);
        }

		public static XmlNode[] SelectNodes(XmlNode node, string name)
		{
			ArrayList list													= new ArrayList();
			foreach(XmlNode selnode in node.ChildNodes)
			{
				if(selnode.Name == name)
					list.Add(selnode);
			}
			return (XmlNode[])list.ToArray(typeof(XmlNode));
		}
	}
}
