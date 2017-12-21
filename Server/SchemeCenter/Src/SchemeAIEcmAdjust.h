/*******************************************************************
** 文件名:	SchemeAIEcmAdjust.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/3/2017
** 版  本:	1.0
** 描  述:	
** 应  用:  	
********************************************************************/

#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"

struct EcmAdjustNode
{
	int nDiffMin;
	int nDiffMax;
	float fFactorEarly;   // 前N把的补偿系数
    float fFactorNormal;  // 正常的补偿系数

	EcmAdjustNode()
	{
		memset(this, 0, sizeof(*this));
	}
};

// AI ID配置表
class CSchemeAIEcmAdjust : public ISchemeAIEcmAdjust, public ISchemeUpdateSink, public CSchemeCache<CSchemeAIEcmAdjust>
{
public:
    CSchemeAIEcmAdjust();

	bool LoadScheme();

	void Close(void);

	virtual float getAdjustFactor(int nMatchCount, int nEcmDiff);

	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:
	// 元素应该很少
	std::vector<EcmAdjustNode> m_vecNode;

    // 前N把fFactorEarly
    int m_nSplitCount;
};