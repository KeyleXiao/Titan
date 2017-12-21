#pragma once



class CSetting
{
public:
	CSetting()
		: m_nTimeout(2)
	{}

	long getTimeout() const { return m_nTimeout; }
	void setTimeout(long val) { m_nTimeout = val; }

private:
	long	m_nTimeout;
};