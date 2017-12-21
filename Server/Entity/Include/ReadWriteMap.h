/*******************************************************************
** 文件名:	ReadWriteMap.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-8-29
** 版  本:	1.0
** 描  述:	读写MAP

		
********************************************************************/


#pragma once


#ifndef __READWRITEMAP_H__

#include "net\reader_writer_lock.h"
#include <map>
#include <concurrent_vector.h>

// 读写MAP
template<class _Kty,
class _Ty,
class _Pr = less<_Kty>,
class _Alloc = allocator<pair<const _Kty, _Ty> > >
class ReadWriteMap
{
    TinyReaderWriterLock                    m_lock;             // 读写锁

    std::map<_Kty, _Ty, _Pr, _Alloc>        m_map;

public:
    
    void put(_Kty key, _Ty value)
    {
        m_lock.EnterWrite();
        
        m_map[key] = value;
        
        m_lock.LeaveWrite();
    }

    void remove(_Kty key)
    {
        m_lock.EnterWrite();

        m_map.erase(key);

        m_lock.LeaveWrite();
    }

    void clear()
    {
        m_lock.EnterWrite();

        m_map.clear();

        m_lock.LeaveWrite();
    }

    // 读取
    _Ty get(_Kty key)
    {
        _Ty retValue;

        m_lock.EnterRead();
        
        std::map<_Kty, _Ty, _Pr, _Alloc>::iterator iter  = m_map.find(key);
        if (iter != m_map.end())
        {
            retValue = iter->second;
        }

        m_lock.LeaveRead();

        return retValue;
    }

    size_t size()
    {
        size_t nCount = 0;

        m_lock.EnterRead();
        
        nCount = m_map.size();

        m_lock.LeaveRead();

        return nCount;
    }
};

#endif // !__READWRITEMAP_H__
