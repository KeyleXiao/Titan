/*******************************************************************
** 文件名:	UPopupAction.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/22
** 版  本:	1.0
** 描  述:	弹出菜单元素容器
** 应  用:  方便初始化的时候构建菜单元素

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace USpeedUI.UWidgets
{
    [Serializable]
    //自定义字典容器
    public class UPopupAction : IDictionary<string, UListComponentItemDataPopup>
    {
        List<string> keys = new List<string>();
        List<UListComponentItemDataPopup> values = new List<UListComponentItemDataPopup>();
        List<KeyValuePair<string, UListComponentItemDataPopup>> elements = new List<KeyValuePair<string, UListComponentItemDataPopup>>();

        /// <summary>
        /// 获取/设置字典元素
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public UListComponentItemDataPopup this[string key]
        {
            get
            {
                var index = keys.IndexOf(key);
                return elements[index].Value;
            }

            set
            {
                var index = keys.IndexOf(key);
                elements[index] = new KeyValuePair<string, UListComponentItemDataPopup>(key, value);
            }
        }
        /// <summary>
        /// 字典长度
        /// </summary>
        public int Count
        {
            get
            {
                return elements.Count;
            }
        }

        /// <summary>
        /// 是否只读
        /// </summary>
        public bool IsReadOnly
        {
            get
            {
                return false;
            }
        }

        /// <summary>
        /// 把所有键打到容器里
        /// </summary>
        public ICollection<string> Keys
        {
            get
            {
                return elements.Convert<KeyValuePair<string, UListComponentItemDataPopup>, string>(GetKey);
            }
        }
        private string GetKey(KeyValuePair<string, UListComponentItemDataPopup> item)
        {
            return item.Key;
        }

        /// <summary>
        /// 把所有值打到容器里
        /// </summary>
        public ICollection<UListComponentItemDataPopup> Values
        {
            get
            {
                return elements.Convert<KeyValuePair<string, UListComponentItemDataPopup>, UListComponentItemDataPopup>(GetValue);
            }
        }

        private UListComponentItemDataPopup GetValue(KeyValuePair<string, UListComponentItemDataPopup> item)
        {
            return item.Value;
        }

        /// <summary>
        /// 添加元素
        /// </summary>
        /// <param name="item"></param>
        public void Add(KeyValuePair<string, UListComponentItemDataPopup> item)
        {
            Add(item.Key, item.Value);
        }

        /// <summary>
        /// 添加元素
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        public void Add(string key, UListComponentItemDataPopup value)
        {
            if(key == null)
            {
                throw new ArgumentNullException("Key", "Key 是空的。");
            }
            if(ContainsKey(key))
            {
                throw new ArgumentException("不能拥有相同的Key");
            }
            keys.Add(key);
            values.Add(value);
            elements.Add(new KeyValuePair<string, UListComponentItemDataPopup>(key, value));

            //这里固定了显示的内容与key一致
            value.ItemTypeName = key;
        }

        /// <summary>
        /// 清空容器
        /// </summary>
        public void Clear()
        {
            keys.Clear();
            values.Clear();
            elements.Clear();
        }

        /// <summary>
        /// 判断值是否在容器里
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public bool Contains(KeyValuePair<string, UListComponentItemDataPopup> item)
        {
            return elements.Contains(item);
        }

        /// <summary>
        /// 判断键是否在容器里
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool ContainsKey(string key)
        {
            return keys.Contains(key);
        }

        /// <summary>
        /// 拷贝到数组
        /// </summary>
        /// <param name="array"></param>
        /// <param name="arrayIndex"></param>
        public void CopyTo(KeyValuePair<string, UListComponentItemDataPopup>[] array, int arrayIndex)
        {
            elements.CopyTo(array, arrayIndex);
        }

        /// <summary>
        /// 返回容器的枚举类型
        /// </summary>
        /// <returns></returns>
        public IEnumerator<KeyValuePair<string, UListComponentItemDataPopup>> GetEnumerator()
        {
            return elements.GetEnumerator();
        }

        /// <summary>
        /// 根据值移除元素
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public bool Remove(KeyValuePair<string, UListComponentItemDataPopup> item)
        {
            if(!elements.Contains(item))
            {
                return false;
            }
            var index = elements.IndexOf(item);
            keys.RemoveAt(index);
            values.RemoveAt(index);
            elements.RemoveAt(index);
            return true;
        }

        /// <summary>
        /// 根据键移除元素
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool Remove(string key)
        {
            if(key == null)
            {
                throw new ArgumentNullException("Key", "键是空的。");
            }
            if(!ContainsKey(key))
            {
                return false;
            }
            var index = keys.IndexOf(key);
            keys.RemoveAt(index);
            values.RemoveAt(index);
            elements.RemoveAt(index);

            return true;
        }

        public bool TryGetValue(string key, out UListComponentItemDataPopup value)
        {
            if (key == null)
            {
                throw new ArgumentNullException("key", "Key is null.");
            }

            if (!ContainsKey(key))
            {
                value = default(UListComponentItemDataPopup);
                return false;
            }

            value = values[keys.IndexOf(key)];
            return true;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return elements.GetEnumerator();
        }
    }
}
