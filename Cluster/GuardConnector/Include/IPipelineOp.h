#pragma once
#include "TFunc.h"


class PipelineExecuter;
namespace Redis
{
	class PipelineOp
	{
		friend class PipelineExecuter;
	public:
		virtual ~PipelineOp()
		{
			for (size_t i = 0; i < m_vecConverter.size(); i++)
			{
				IConverter*& pConverter = m_vecConverter.at(i);
				safeDelete(pConverter);
			}
			m_vecConverter.clear();
		}

	protected:
		void pushConverter(IConverter* pConverter) { m_vecConverter.push_back(pConverter); }

		void converter()
		{
			for (size_t i = 0; i < m_vecConverter.size(); i++)
			{
				IConverter*& pConverter = m_vecConverter.at(i);
				pConverter->Convert();
			}
		}

	private:
		typedef	std::vector<IConverter*>	VecConverter;

		VecConverter	m_vecConverter;
	};
};