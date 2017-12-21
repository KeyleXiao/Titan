using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SpellEditor
{
    public enum IDAllocatorType
    {
        /// <summary>
        /// 重复使用
        /// </summary>
        ID_POOL,

        /// <summary>
        /// 每次分配都是上一次的最大值
        /// </summary>
        ID_MAX
    }

    public class HeroEffectIDRange
    {
        public int heroID;
        public string heroName;
        public int minID;
        public int maxID;

    }

    /// <summary>
    /// ID分配器
    /// </summary>
    public class IDAllocator
    {

        public uint IDStartIndex = 1;
        /// <summary>
        /// 当前使用ID的索引，要保证条件ID的唯一性。
        /// </summary>
        private uint m_CurrentUsedIdindex = 1;

        /// <summary>
        /// 临时的ID
        /// </summary>
        private uint m_TempAllocID = uint.MaxValue;

        /// <summary>
        /// 已经使用过的ID；
        /// </summary>
        public HashSet<uint> m_UsedIdList = new HashSet<uint>();

        
        /// <summary>
        /// 没有使用过的ID；
        /// </summary>
        private HashSet<uint> m_UnUsedIdList = new HashSet<uint>();

        /// <summary>
        /// ID分配是否按照最大值ID
        /// </summary>
        private IDAllocatorType m_allocType = IDAllocatorType.ID_POOL;


        public IDAllocator(IDAllocatorType allocType)
        {
            m_allocType = allocType;
        }

        /// <summary>
        /// 计算没有使用过的ID
        /// </summary>
        public void CalculateUnUsedID()
        {
            m_UnUsedIdList.Clear();
            m_TempAllocID = uint.MaxValue;
            if (m_allocType == IDAllocatorType.ID_POOL)
            {
                for (uint i = IDStartIndex; i <= m_CurrentUsedIdindex; i++)
                {
                    m_UnUsedIdList.Add(i);
                }

                foreach (uint ind in m_UsedIdList)
                {
                    m_UnUsedIdList.Remove(ind);
                }

                if (m_UsedIdList.Count > 0)
                    m_CurrentUsedIdindex++;
            }
            else
            {
                if (m_UsedIdList.Count > 0)
                {
                    m_CurrentUsedIdindex++;
                    m_UsedIdList.Clear();
                }
            }
        }

        /// <summary>
        /// 分配一个ID
        /// </summary>
        /// <returns></returns>
        public uint AllocId()
        {
            if (m_allocType == IDAllocatorType.ID_POOL)
            {
                return AllocIdWithPool();
            }

            return AllocIdWithMaxValue();
        }

        public uint AllocId(uint min,uint max)
        {
            //如果没有调用AcceptAlloc函数，直接返回m_TempAllocID
            if (m_TempAllocID != uint.MaxValue)
            {
                if (m_UnUsedIdList.Count > 0)
                {
                    PushUnUsedId(m_TempAllocID);
                    m_TempAllocID = m_UnUsedIdList.First(number => number >= min && number<=max);
                    m_UnUsedIdList.Remove(m_TempAllocID);
                }
                return m_TempAllocID;
            }
            uint id = min;
            if (m_UnUsedIdList.Count > 0)
            {
                id = m_UnUsedIdList.First(number => number >= min && number <= max);
                m_UnUsedIdList.Remove(id);
            }
            else
            {
                while (m_UsedIdList.Contains(id) && id <= max)
                {
                    id++;
                }
            }
            m_TempAllocID = id;
            m_UsedIdList.Add(id);
            return id;
        }

        /// <summary>
        /// 重复使用没有分配的ID
        /// </summary>
        /// <returns></returns>
        private uint AllocIdWithPool()
        {
            //如果没有调用AcceptAlloc函数，直接返回m_TempAllocID
            if (m_TempAllocID != uint.MaxValue)
            {
                if (m_UnUsedIdList.Count > 0)
                {
                    PushUnUsedId(m_TempAllocID);
                    m_TempAllocID = m_UnUsedIdList.First();
                    m_UnUsedIdList.Remove(m_TempAllocID);
                }
                return m_TempAllocID;
            }
            uint id = IDStartIndex;
            if (m_UnUsedIdList.Count > 0)
            {
                id = m_UnUsedIdList.First();
                m_UnUsedIdList.Remove(id);
            }
            else
            {
                while (m_UsedIdList.Contains(id))
                {
                    id = m_CurrentUsedIdindex;
                    m_CurrentUsedIdindex++;
                }
            }
            m_TempAllocID = id;
            m_UsedIdList.Add(id);
            return id;
        }



        /// <summary>
        /// 分配一个ID，直接在当前ID上+1
        /// </summary>
        /// <returns></returns>
        private uint AllocIdWithMaxValue()
        {
            //如果没有调用AcceptAlloc函数，当前使用的ID减一
            if (m_TempAllocID != uint.MaxValue)
            {
                m_CurrentUsedIdindex--;
            }
            uint id = IDStartIndex;
            //如果有未使用过的ID
            if (m_UnUsedIdList.Count > 0)
            {
                id = m_UnUsedIdList.Last();
                //如果删除的那个ID是最大ID，则返回最大ID，最大ID于当前使用的ID关系是最大ID=当前使用的ID-1
                if (id == m_CurrentUsedIdindex - 1)
                {
                    m_TempAllocID = id;
                    m_UsedIdList.Add(id);
                    m_UnUsedIdList.Remove(id);
                    return m_TempAllocID;
                }
                else //否则，直接在最大ID上+1
                {
                    id = m_CurrentUsedIdindex;
                    m_CurrentUsedIdindex++;
                }
                
            }
            else//否则，直接在最大ID上+1
            {
                id = m_CurrentUsedIdindex;
                m_CurrentUsedIdindex++;
            }
            m_TempAllocID = id;
            m_UsedIdList.Add(id);
            return id;
        }

        /// <summary>
        /// 确认分配，只有确认分配了，下次Alloc才会有效，要不然始终返回上一次分配的值
        /// </summary>
        public void AcceptAlloc()
        {
            m_TempAllocID = uint.MaxValue;
        }

        /// <summary>
        /// 清除分配器
        /// </summary>
        public void ClearIDAllocator()
        {
            m_UsedIdList.Clear();
            m_UnUsedIdList.Clear();
            m_CurrentUsedIdindex = IDStartIndex;
            m_TempAllocID = uint.MaxValue;
            
        }


        /// <summary>
        /// 仅仅是在加载配置的时候调用，其他时间无需调用，用于配置加载完了，计算未使用的ID
        /// </summary>
        /// <param name="id"></param>
        public void PushUsedId(uint id)
        {
            m_UsedIdList.Add(id);
            if (id > m_CurrentUsedIdindex)
            {
                m_CurrentUsedIdindex = id;
            }
               
        }

        public void PushUnUsedId(uint id)
        {
            m_UnUsedIdList.Add(id);
        }
       
    }
}
