/*******************************************************************
** 文件名:	PerformanceStat.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-10-26
** 版  本:	1.0
** 描  述:	用于性能统计


**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once

#ifndef __PERFORMANCESTAT_H__ 
#define __PERFORMANCESTAT_H__

//#define  PERFORMANCE_CLIENT_TEST


/** 用于客户端性能统计
*/
class CPerformanceStat
{
public:
	CPerformanceStat()
		: m_create_tick(0)
		, m_runtime_tick(0)
        , m_output_tick(0)
        , m_isAccumulate(true)
        , m_nOutputMinVal(0)
        , m_nOutputInterVal(60*1000)
	{
		memset(m_name, 0, sizeof(m_name));
	}

	bool create(const char* szName, bool isAccumulate=true, int nOutputInterVal=60*1000, int nOutputMinVal=0)
	{
		sstrcpyn(m_name, szName, sizeof(m_name));

		m_create_tick = getTickCount();
		m_runtime_tick = getTickCount();
        m_output_tick =  getTickCount();

        m_isAccumulate      = isAccumulate;     // 统计值输出后是否累积
        m_nOutputInterVal   = nOutputInterVal;
        m_nOutputMinVal     = nOutputMinVal;

        return true;
	}

	void release()
	{
		delete this;
	}

	inline void push(int k, int v)
	{
		std::map<int, TMAP_VALUECOUNT>::iterator iter = m_data.find(k);
		if (iter == m_data.end())
		{
			ValueNode newNode;
			TMAP_VALUECOUNT mapv;
			mapv[v] = newNode;
			m_data[k] = mapv;
		}

		ValueNode &node = m_data[k][v];
		++node.nValue;

        output(false);
	}

	void output(bool enforce_output)
	{
        if(!enforce_output && m_nOutputInterVal == 0)
        {
            return;
        }
        if (m_data.empty())
        {
            return;
        }
        if(getTickCount() < m_output_tick + m_nOutputInterVal)
        {
            return;
        }

        m_output_tick = getTickCount();

		WarningLn("--------------------------------------------------------------");
		char szBuf[128] = {0};
		for (std::map<int, TMAP_VALUECOUNT>::iterator iter = m_data.begin(); iter != m_data.end(); ++iter)
		{
			TMAP_VALUECOUNT & mapItem = iter->second;
			for (TMAP_VALUECOUNT::iterator it = mapItem.begin(); it != mapItem.end(); ++it)
			{
				ValueNode &node=it->second;
				if(node.nValue > m_nOutputMinVal)
				{
					ssprintf_s(szBuf, sizeof(szBuf), "%s:%d, Key=%d, count=%d, runtime=%d ms", m_name,(iter->first), (it->first), (node.nValue), (getTickCount()-node.dwCreateTime) );
                    //ssprintf_s(szBuf, sizeof(szBuf), "%s:%d, Key=%d, count=%d", m_name,(iter->first), (it->first), (node.nValue) );
					WarningLn(szBuf);
				}
			}
		}

        // 统计值输出后是否累积
        if(!m_isAccumulate)
        {
            m_data.clear();
        }
		WarningLn("--------------------------------------------------------------");
	}

private:
	char					m_name[GAME_NAME_MAXSIZE];
	DWORD					m_create_tick;
	DWORD					m_runtime_tick;
	DWORD					m_output_tick;

    // 统计值输出后是否累积
    bool                    m_isAccumulate;
    // 最少输出信息参考值
    int                     m_nOutputMinVal;
    // 输出到LOG中间隔ms, 0不输出
    int                     m_nOutputInterVal;

	// 节点
	struct ValueNode
	{
		int nValue;
		DWORD dwCreateTime;

		ValueNode()
		{
			nValue = 0;
			dwCreateTime = getTickCount();
		}
	};

	typedef	std::map<int, ValueNode> TMAP_VALUECOUNT;
	std::map<int, TMAP_VALUECOUNT>	m_data;
};


#endif	// __PERFORMANCESTAT_H__


