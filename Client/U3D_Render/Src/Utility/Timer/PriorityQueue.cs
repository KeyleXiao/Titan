using System;
using System.Collections.Generic;
using System.Linq;

namespace U3D_Render.STL
{
    /// <summary>
    /// 最小堆优先队列，优先级低的在队列头
    /// 详细参考危机百科 http://en.wikipedia.org/wiki/Binary_heap
    /// </summary>
    /// <typeparam name="TPriority">用于排序比较的优先级参数</typeparam>
    /// <typeparam name="TValue">队列里的参数类型</typeparam>
    class PriorityQueue<TPriority, TValue> : IEnumerable<KeyValuePair<TPriority, TValue>>
    {
        //方便外部遍历队列
        public class ValueCollection : IEnumerable<TValue>
        {
            private readonly PriorityQueue<TPriority, TValue> parentCollection;

            public ValueCollection(PriorityQueue<TPriority, TValue> parentCollection)
            {
                this.parentCollection = parentCollection;
            }

            #region IEnumerable

            public IEnumerator<TValue> GetEnumerator()
            {
                foreach (KeyValuePair<TPriority, TValue> pair in this.parentCollection)
                {
                    yield return pair.Value;
                }
            }

            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
            {
                return this.GetEnumerator();
            }

            #endregion
        }

        private readonly IComparer<TPriority> priorityComparer;

        public PriorityQueue()
            : this(Comparer<TPriority>.Default)
        {
        }

        public PriorityQueue(IComparer<TPriority> comparer)
        {
            if (comparer == null)
                throw new ArgumentNullException();

            priorityComparer = comparer;
        }

        private readonly List<KeyValuePair<TPriority, TValue>> queue = new List<KeyValuePair<TPriority, TValue>>();
        private ValueCollection valueCollection = null;

        public ValueCollection Values
        {
            get
            {
                if (this.valueCollection == null)
                {
                    this.valueCollection = new ValueCollection(this);
                }

                return this.valueCollection;
            }
        }

        #region IEnumerable

        public IEnumerator<KeyValuePair<TPriority, TValue>> GetEnumerator()
        {
            return this.queue.GetEnumerator();
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        #endregion

        /// <summary>
        /// 清空队列
        /// </summary>
        public void Clear()
        {
            this.queue.Clear();
        }

        /// <summary>
        /// 添加到队列并根据优先级计算元素在队列的位置
        /// </summary>
        /// <param name="priority">新元素的优先级</param>
        /// <param name="value"></param>
        public void Push(TPriority priority, TValue value)
        {
            //加入到队列尾部
            this.queue.Add(new KeyValuePair<TPriority, TValue>(priority, value));
            //并向上比较重算位置
            BubbleUp();
        }

        /// <summary>
        /// 队列长度
        /// </summary>
        public int Count
        {
            get
            {
                return this.queue.Count;
            }
        }

        /// <summary>
        /// 获取队列优先级最低的元素
        /// </summary>
        public KeyValuePair<TPriority, TValue> Top
        {
            get
            {
                return this.queue[0];
            }
        }

        /// <summary>
        /// 获取队列优先级最低的元素并移出队列
        /// </summary>
        /// <returns>The minmal element</returns>
        public KeyValuePair<TPriority, TValue> Pop()
        {
            KeyValuePair<TPriority, TValue> ret = this.queue[0];
            //将队列优先级最高的放到队头
            this.queue[0] = this.queue[queue.Count - 1];
            this.queue.RemoveAt(this.queue.Count - 1);
            //向下比较重算位置
            BubbleDown();
            return ret;
        }

        /// <summary>
        /// 队里是否有指定的元素
        /// </summary>
        public bool Contains(TValue value)
        {
            return this.queue.Any(itm => EqualityComparer<TValue>.Default.Equals(itm.Value, value));
        }

        /// <summary>
        /// 移出第一个匹配的元素
        /// </summary>
        public bool Remove(TValue value)
        {
            var idx = this.queue.FindIndex(itm => EqualityComparer<TValue>.Default.Equals(itm.Value, value));
            if (idx == -1)
            {
                return false;
            }

            this.queue[idx] = this.queue[this.queue.Count - 1];
            this.queue.RemoveAt(this.queue.Count - 1);
            BubbleDown();

            return true;
        }

        /// <summary>
        /// 移出同一优先级的所有元素
        /// </summary>
        /// <returns></returns>
        public bool RemoveAtPriority(TPriority priority, Predicate<TValue> shouldRemove)
        {
            bool removed = false;

            for (int i = this.queue.Count - 1; i >= 0; --i)
            {
                if (this.queue[i].Key.Equals(priority) && (shouldRemove == null || shouldRemove(this.queue[i].Value)))
                {
                    this.queue[i] = this.queue[this.queue.Count - 1];
                    this.queue.RemoveAt(this.queue.Count - 1);
                    BubbleDown();

                    removed = true;
                }
            }

            return removed;
        }

        /// <summary>
        /// 最后一个元素向上比较，直到遇到一个优先级比他小的元素为止
        /// </summary>
        private void BubbleUp()
        {
            int node = this.queue.Count - 1; 
            while (node > 0)
            {
                int parent = (node - 1) >> 1;
                if (priorityComparer.Compare(this.queue[parent].Key, this.queue[node].Key) < 0)
                {
                    break; 
                }
                var tmp = this.queue[parent];
                this.queue[parent] = this.queue[node];
                this.queue[node] = tmp;
                node = parent;
            }
        }

        /// <summary>
        /// 第一个元素向下比较，直到遇到一个优先级比他高的元素为止
        /// </summary>
        private void BubbleDown()
        {
            int node = 0;
            while (true)
            {
                //与后两个元素比较
                int child0 = (node << 1) + 1;
                int child1 = (node << 1) + 2;
                int smallest;
                if (child0 < this.queue.Count && child1 < this.queue.Count)
                {
                    smallest = priorityComparer.Compare(this.queue[child0].Key, this.queue[child1].Key) < 0 ? child0 : child1;
                }
                else if (child0 < this.queue.Count)
                {
                    smallest = child0;
                }
                else if (child1 < this.queue.Count)
                {
                    smallest = child1;
                }
                else
                {//超出队长度
                    break;
                }

                if (priorityComparer.Compare(this.queue[node].Key, this.queue[smallest].Key) < 0)
                {
                    break; 
                }

                var tmp = this.queue[node];
                this.queue[node] = this.queue[smallest];
                this.queue[smallest] = tmp;
                node = smallest;
            }
        }
    }
}
